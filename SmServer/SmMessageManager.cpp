#include "pch.h"
#include "SmMessageManager.h"
#include "SmUserManager.h"
#include "Json/json.hpp"
#include "SmRealtimeSymbolServiceManager.h"
using namespace nlohmann;
SmMessageManager::SmMessageManager()
{

}

SmMessageManager::~SmMessageManager()
{

}

void SmMessageManager::OnMessage(std::string message, SmWebsocketSession* socket)
{
	if (!socket)
		return;

	CString msg;
	msg.Format(_T("message = %s, %x\n"), message.c_str(), socket);
	TRACE(msg);
	ParseMessage(message, socket);
}

void SmMessageManager::ParseMessage(std::string message, SmWebsocketSession* socket)
{
	if (!socket || message.length() == 0)
		return;

	auto json_object = json::parse(message);
	try {
		int req_id = json_object["req_id"];
		switch (req_id)
		{
		case 1:
			OnLogin(message, socket);
			break;
		case  2:
			OnRegisterSymbol(message);
			break;
		case 3:
			OnRegisterSymbolCycle(message);
			break;
		default:
			break;
		}
	}
	catch (std::exception e) {
		std::string error = e.what();
	}
}

void SmMessageManager::OnLogin(std::string message, SmWebsocketSession* socket)
{
	try {
		auto rcvd_json = json::parse(message);
		auto user_info = rcvd_json["user_info"];
		std::string id = user_info["id"];
		std::string pwd = user_info["pwd"];
		SmUserManager* userMgr = SmUserManager::GetInstance();
		userMgr->AddUser(id, pwd, socket);
		SendResult(id, 0, "success!");
	}
	catch (std::exception e) {
		std::string error = e.what();
	}
}

void SmMessageManager::SendResult(std::string user_id, int result_code, std::string result_msg)
{
	json res = {
		{"result", result_code},
		{"message", result_msg}
	};
	SmUserManager* userMgr = SmUserManager::GetInstance();
	userMgr->SendResultMessage(user_id, res.dump(4));
}

void SmMessageManager::OnRegisterSymbol(std::string message)
{
	try {
		auto rcvd_json = json::parse(message);
		std::string id = rcvd_json["user_id"];
		std::string symCode = rcvd_json["symbol_code"];
		SmRealtimeSymbolServiceManager* rtlSymMgr = SmRealtimeSymbolServiceManager::GetInstance();
		rtlSymMgr->RegisterSymbol(id, symCode);

		SendResult(id, 0, "success!");
	}
	catch (std::exception e) {
		std::string error = e.what();
	}
}

void SmMessageManager::OnRegisterSymbolCycle(std::string message)
{
	try {
		auto rcvd_json = json::parse(message);
		std::string id = rcvd_json["id"];
		SendResult(id, 0, "success!");
	}
	catch (std::exception e) {
		std::string error = e.what();
	}
}
