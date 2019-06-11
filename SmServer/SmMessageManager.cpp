#include "pch.h"
#include "SmMessageManager.h"
#include "SmUserManager.h"
#include "Json/json.hpp"
#include "SmRealtimeSymbolServiceManager.h"
#include "SmSessionManager.h"
#include "SmServiceDefine.h"
#include "SmOrderDefine.h"
#include "SmTotalOrderManager.h"
#include "SmChartDefine.h"
#include "SmTimeSeriesCollector.h"
#include "SmTimeSeriesServiceManager.h"
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
		SmProtocol sm_protocol = [](int id) {
			return (SmProtocol)id;
		}(req_id);

		switch (sm_protocol)
		{
		case SmProtocol::req_login:
			OnLogin(json_object, socket);
			break;
		case  SmProtocol::req_register_symbol:
			OnRegisterSymbol(json_object);
			break;
		case SmProtocol::req_register_symbol_cycle:
			OnRegisterSymbolCycle(json_object);
			break;
		case SmProtocol::req_order_new:
			OnOrderNew(json_object);
			break;
		case SmProtocol::req_order_modify:
			OnOrderModify(json_object);
			break;
		case SmProtocol::req_order_cancel:
			OnOrderCancel(json_object);
			break;
		case SmProtocol::req_chart_data:
			OnReqChartData(json_object);
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
		//std::string result = userMgr->CheckUserInfo(id, pwd, socket);
		//SendResult(id, 0, result);
		userMgr->AddUser(id, pwd, socket);
		SendResult(id, 0, "Login Success!");
	}
	catch (std::exception e) {
		std::string error = e.what();
	}
}

void SmMessageManager::OnLogout(nlohmann::json& obj)
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


void SmMessageManager::OnRegisterSymbol(nlohmann::json& obj)
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

void SmMessageManager::OnUnregisterSymbol(nlohmann::json& obj)
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

void SmMessageManager::OnRegisterSymbolCycle(nlohmann::json& obj)
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

void SmMessageManager::OnUnregisterSymbolCycle(nlohmann::json& obj)
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

void SmMessageManager::OnOrderNew(nlohmann::json& obj)
{
	SmOrderRequest req;
	SmTotalOrderManager* totOrderMgr = SmTotalOrderManager::GetInstance();
	totOrderMgr->OnOrder(std::move(req));
}

void SmMessageManager::OnOrderModify(nlohmann::json& obj)
{
	SmOrderRequest req;
	SmTotalOrderManager* totOrderMgr = SmTotalOrderManager::GetInstance();
	totOrderMgr->OnOrder(std::move(req));
}

void SmMessageManager::OnOrderCancel(nlohmann::json& obj)
{
	SmOrderRequest req;
	SmTotalOrderManager* totOrderMgr = SmTotalOrderManager::GetInstance();
	totOrderMgr->OnOrder(std::move(req));
}

void SmMessageManager::OnReqChartData(nlohmann::json& obj)
{
	try {
		std::string id = obj["user_id"];
		std::string symCode = obj["symbol_code"];
		std::string chart_type = obj["chart_type"];
		std::string cycle = obj["cycle"];
		std::string count = obj["count"];
		SmChartDataRequest req;
		req.user_id = id;
		req.symbolCode = symCode;
		req.chartType = (SmChartType)std::stoi(chart_type);
		req.cycle = std::stoi(cycle);
		req.count = std::stoi(count);
		req.next = 0;
		SmTimeSeriesServiceManager* timeSvcMgr = SmTimeSeriesServiceManager::GetInstance();
		timeSvcMgr->OnChartDataRequest(std::move(req));
		SendResult(id, 0, "request chart data success!");
	}
	catch (std::exception e) {
		std::string error = e.what();
	}
}
