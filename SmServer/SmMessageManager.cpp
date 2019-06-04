#include "pch.h"
#include "SmMessageManager.h"
#include "SmUserManager.h"
#include "Json/json.hpp"
#include "SmRealtimeSymbolServiceManager.h"
#include "SmSessionManager.h"
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
	if (!socket) return;
	if (socket && message.length() == 0) {
		
		return;
	}

	try {
		auto json_object = json::parse(message);

		int req_id = json_object["req_id"];
		switch (req_id)
		{
		case 1:
			OnLogin(json_object, socket);
			break;
		case  2:
			OnRegisterSymbol(json_object, socket);
			break;
		case 3:
			OnRegisterSymbolCycle(json_object, socket);
			break;
		default:
			break;
		}
	}
	catch (std::exception e) {
		std::string error = e.what();
	}
}


void SmMessageManager::OnLogin(nlohmann::json& obj, SmWebsocketSession* socket)
{
	try {
		auto user_info = obj["user_info"];
		std::string id = user_info["id"];
		std::string pwd = user_info["pwd"];
		SmUserManager* userMgr = SmUserManager::GetInstance();
		userMgr->AddUser(id, pwd, socket);
		SendResult(id, 0, "login success!");
	}
	catch (std::exception e) {
		std::string error = e.what();
	}
}

void SmMessageManager::OnLogout(nlohmann::json& obj, SmWebsocketSession* socket)
{
	try {
		auto user_info = obj["user_info"];
		std::string id = user_info["id"];
		SmUserManager* userMgr = SmUserManager::GetInstance();
		userMgr->Logout(id);
		SendResult(id, 0, "logout success!");
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


void SmMessageManager::OnRegisterSymbol(nlohmann::json& obj, SmWebsocketSession* socket)
{
	try {
		std::string id = obj["user_id"];
		std::string symCode = obj["symbol_code"];
		SmRealtimeSymbolServiceManager* rtlSymMgr = SmRealtimeSymbolServiceManager::GetInstance();
		rtlSymMgr->RegisterSymbol(id, symCode);

		SendResult(id, 0, "register symbol success!");
	}
	catch (std::exception e) {
		std::string error = e.what();
	}
}

void SmMessageManager::OnUnregisterSymbol(nlohmann::json& obj, SmWebsocketSession* socket)
{
	try {
		std::string id = obj["user_id"];
		std::string symCode = obj["symbol_code"];
		SmRealtimeSymbolServiceManager* rtlSymMgr = SmRealtimeSymbolServiceManager::GetInstance();
		rtlSymMgr->UnregisterSymbol(id, symCode);

		SendResult(id, 0, "register symbol success!");
	}
	catch (std::exception e) {
		std::string error = e.what();
	}
}

void SmMessageManager::OnRegisterSymbolCycle(nlohmann::json& obj, SmWebsocketSession* socket)
{
	try {
		std::string id = obj["id"];
		std::string symCode = obj["symbol_code"];
		std::string chart_type = obj["chart_type"];
		std::string cycle = obj["cycle"];
		SendResult(id, 0, "register symbol cycle success!");
	}
	catch (std::exception e) {
		std::string error = e.what();
	}
}

void SmMessageManager::OnUnregisterSymbolCycle(nlohmann::json& obj, SmWebsocketSession* socket)
{
	try {
		std::string id = obj["id"];
		std::string symCode = obj["symbol_code"];
		std::string chart_type = obj["chart_type"];
		std::string cycle = obj["cycle"];
		SendResult(id, 0, "register symbol cycle success!");
	}
	catch (std::exception e) {
		std::string error = e.what();
	}
}
