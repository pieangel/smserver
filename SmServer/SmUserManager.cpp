#include "SmUserManager.h"
#include "SmUser.h"
#include "SmWebsocketSession.h"
#include "SmRealtimeSymbolServiceManager.h"
#include "SmTimeSeriesDBManager.h"
#include "Json/json.hpp"
#include "SmServiceDefine.h"
using namespace nlohmann;
SmUserManager::SmUserManager()
{

}

SmUserManager::~SmUserManager()
{
	std::lock_guard<std::mutex> lock(_mutex);
	for (auto it = _UserMap.begin(); it != _UserMap.end(); ++it) {
		SmUser* user = it->second;
		ClearAllService(user);
		delete user;
	}
}

SmUser* SmUserManager::FindUserBySocket(SmWebsocketSession* socket)
{
	if (!socket)
		return nullptr;
	auto it = _SocketToUserMap.find(socket);
	if (it != _SocketToUserMap.end()) {
		return it->second;
	}

	return nullptr;
}

SmUser* SmUserManager::AddUser(std::string id, SmWebsocketSession* socket)
{
	std::lock_guard<std::mutex> lock(_mutex);
	if (!socket)
		return nullptr;

	SmUser* user = FindUser(id);
	if (!user) {
		user = new SmUser();
	}
	user->Id(id);
	user->Socket(socket);
	user->Connected(true);
	_UserMap[id] = user;
	_SocketToUserMap[socket] = user;
	return user;
}

SmUser* SmUserManager::AddUser(std::string id, std::string pwd, SmWebsocketSession* socket)
{
	std::lock_guard<std::mutex> lock(_mutex);

	if (!socket)
		return nullptr;
	SmUser* user = FindUser(id);
	if (!user) {
		user = new SmUser();
	}
	user->Id(id);
	user->Password(pwd);
	user->Socket(socket);
	user->Connected(true);
	_UserMap[id] = user;
	_SocketToUserMap[socket] = user;
	AddUserToDatabase(id, pwd);
	return user;
}

std::string SmUserManager::CheckUserInfo(std::string id, std::string pwd, SmWebsocketSession* socket)
{
	std::string result_msg = "";
	if (!IsExistUser(id)) {
		AddUser(id, pwd, socket);
		result_msg = "User registered successfully!";
		return result_msg;
	}
	SmTimeSeriesDBManager* dbMgr = SmTimeSeriesDBManager::GetInstance();
	std::pair<std::string, std::string> id_pwd = dbMgr->GetUserInfo(id);
	if (id_pwd.first.compare(id) != 0) {
		result_msg = "ID error!";
		return result_msg;
	}
	if (id_pwd.second.compare(pwd) != 0) {
		result_msg = "Password error!";
		return result_msg;
	}

	AddUser(id, pwd, socket);
	result_msg = "Login success!";
	return result_msg;
}

void SmUserManager::AddUserToDatabase(std::string id, std::string pwd)
{
	SmTimeSeriesDBManager* dbMgr = SmTimeSeriesDBManager::GetInstance();
	dbMgr->AddUserToDatabase(id, pwd);
}

void SmUserManager::RemoveUser(std::string id)
{
	auto it = _UserMap.find(id);
	if (it != _UserMap.end()) {
		_UserMap.erase(it);
	}
}

void SmUserManager::ClearAllService(SmUser* user)
{
	if (!user)
		return;

	SmRealtimeSymbolServiceManager* rtlSymSvcMgr = SmRealtimeSymbolServiceManager::GetInstance();
	rtlSymSvcMgr->UnregisterAllSymbol(user->Id());
}


void SmUserManager::SendLoginResult(std::string user_id, std::string msg)
{
	json send_object;
	send_object["res_id"] = SmProtocol::res_login;
	send_object["result_msg"] = msg;
	send_object["result_code"] = 0;
	std::string content = send_object.dump(4);
	SendResultMessage(user_id, content);
}

void SmUserManager::SendLogoutResult(std::string user_id)
{
	json send_object;
	send_object["res_id"] = SmProtocol::res_logout;
	send_object["result_msg"] = "Logout Success!";
	send_object["result_code"] = 0;
	std::string content = send_object.dump(4);
	SendResultMessage(user_id, content);
}

void SmUserManager::SendBroadcastMessage(std::string message)
{
	// Put the message in a shared pointer so we can re-use it for each client
	auto const ss = boost::make_shared<std::string const>(std::move(message));

	// Make a local list of all the weak pointers representing
	// the sessions, so we can do the actual sending without
	// holding the mutex:
	std::vector<std::weak_ptr<SmWebsocketSession>> v;
	{
		std::lock_guard<std::mutex> lock(_mutex);
		v.reserve(_UserMap.size());
		for (auto it = _UserMap.begin(); it != _UserMap.end(); ++it) {
			SmUser* user = it->second;
			if (user->Connected() && user->Socket())
				v.emplace_back(user->Socket()->weak_from_this());
		}
	}

	// For each session in our local list, try to acquire a strong
   // pointer. If successful, then send the message on that session.
	for (auto const& wp : v)
		if (auto sp = wp.lock())
			sp->send(ss);

}

SmUser* SmUserManager::FindUser(std::string id)
{
	auto it = _UserMap.find(id);
	if (it != _UserMap.end()) {
		return it->second;
	}

	return nullptr;
}

void SmUserManager::ResetUserBySocket(SmWebsocketSession* socket)
{
	std::lock_guard<std::mutex> lock(_mutex);

	auto it = _SocketToUserMap.find(socket);
	if (it != _SocketToUserMap.end()) {
		SmUser* user = it->second;
		ClearAllService(user);
		user->Reset();
	}
}

void SmUserManager::SendResultMessage(std::string user_id, std::string message)
{
	// Put the message in a shared pointer so we can re-use it for each client
	auto const ss = boost::make_shared<std::string const>(std::move(message));

	// Make a local list of all the weak pointers representing
	// the sessions, so we can do the actual sending without
	// holding the mutex:
	std::vector<std::weak_ptr<SmWebsocketSession>> v;
	{
		std::lock_guard<std::mutex> lock(_mutex);
		v.reserve(1);
		auto it = _UserMap.find(user_id);
		if (it != _UserMap.end()) {
			SmUser* user = it->second;
			if (user->Connected() && user->Socket())
				v.emplace_back(user->Socket()->weak_from_this());
		}
	}

	// For each session in our local list, try to acquire a strong
	// pointer. If successful, then send the message on that session.
	for (auto const& wp : v)
		if (auto sp = wp.lock())
			sp->send(ss);
}

void SmUserManager::Logout(std::string id)
{
	SmUser* user = FindUser(id);
	if (!user)
		return;
	if (user->Socket())
		ResetUserBySocket(user->Socket());
}

bool SmUserManager::IsExistUser(std::string id)
{
	SmTimeSeriesDBManager* dbMgr = SmTimeSeriesDBManager::GetInstance();
	std::string query_string = "select \"id\", \"pwd\" from \"user_info\" where  \"id\" = \'" + id + "\'";

	std::string resp = dbMgr->ExecQuery(query_string);

	std::string result = resp;

	auto json_object = json::parse(resp);
	auto values = json_object["results"][0]["series"];
	if (values.is_null()) {
		return false;
	}

	values = json_object["results"][0]["series"][0]["values"];
	std::string str_id = values[0][1];
	
	if (str_id.compare(id) == 0)
		return true;
	else
		return false;
}

void SmUserManager::OnLogin(std::string id, std::string pwd, SmWebsocketSession* socket)
{
	std::string result = CheckUserInfo(id, pwd, socket);
	SendLoginResult(id, result);
}

void SmUserManager::OnLogout(std::string id)
{
	RemoveUser(id);
	SendLogoutResult(id);
}
