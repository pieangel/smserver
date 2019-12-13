//#include "pch.h"
#include "SmProtocolManager.h"
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
#include "SmSymbolManager.h"
#include "SmSymbol.h"
#include "SmMarketManager.h"
#include "SmChartDataManager.h"
#include "SmChartData.h"
#include "SmMarketManager.h"
#include "SmWebsocketSession.h"
#include "SmMongoDBManager.h"
#include "SmGlobal.h"
#include "SmAccountManager.h"
#include "SmTotalPositionManager.h"
#include "SmAccount.h"
#include "SmSymbol.h"
#include "Log/loguru.hpp"
#include "SmSymbolManager.h"
using namespace nlohmann;
SmProtocolManager::SmProtocolManager()
{

}

SmProtocolManager::~SmProtocolManager()
{

}

void SmProtocolManager::OnMessage(std::string message, SmWebsocketSession* socket)
{
	if (!socket)
		return;

	//CString msg;
	//msg.Format(_T("message = %s, %x\n"), message.c_str(), socket);
	//TRACE(msg);

	char buffer[4096];
	sprintf(buffer, "OnMessage%s\n", message.c_str());
	//OutputDebugString(buffer);
	ParseMessage(message, socket);
}

void SmProtocolManager::ParseMessage(std::string message, SmWebsocketSession* socket)
{
	if (!socket) return;
	if (socket && message.length() == 0) {
		
		return;
	}

	try {
		auto json_object = json::parse(message);
		json_object["session_id"] = socket->SessionID();

		int req_id = json_object["req_id"];
		SmProtocol sm_protocol = [](int id) {
			return (SmProtocol)id;
		}(req_id);

		switch (sm_protocol)
		{
		case SmProtocol::req_login:
			OnReqLogin(json_object, socket);
			break;
		case  SmProtocol::req_register_symbol:
			OnRegisterSymbol(json_object);
			break;
		case SmProtocol::req_register_chart_cycle_data:
			OnRegisterChartCycleData(json_object);
			break;
		case SmProtocol::req_order_new:
		case SmProtocol::req_order_modify:
		case SmProtocol::req_order_cancel:
			OnOrder(json_object);
			break;
		case SmProtocol::req_chart_data:
			OnReqChartData(json_object, socket);
			break;
		case SmProtocol::req_sise_data:
			OnReqSiseData(json_object);
			break;
		case SmProtocol::req_hoga_data:
			OnReqHogaData(json_object);
			break;
		case SmProtocol::req_symbol_master:
			OnReqSymbolMaster(json_object);
			break;
		case SmProtocol::req_symbol_master_all:
			OnReqSymbolMasterAll(json_object);
			break;
		case SmProtocol::req_recent_sise_data_all:
			OnReqRecentSiseDataAll(json_object);
			break;
		case SmProtocol::req_sise_data_all:
			OnReqSiseDataAll(json_object);
			break;
		case SmProtocol::req_register_recent_realtime_sise_all:
			OnReqRegisterRecentRealtimeSiseAll(json_object);
			break;
		case SmProtocol::req_market_list:
			OnReqMarketList(json_object);
			break;
		case SmProtocol::req_symbol_list_by_category:
			OnReqSymbolListByCategory(json_object);
			break;
		case SmProtocol::req_register_sise_socket: {
			SmTimeSeriesServiceManager* tsMgr = SmTimeSeriesServiceManager::GetInstance();
			tsMgr->OnReqRegisterSiseSocket(socket);
		}
			break;
		case SmProtocol::req_chart_data_resend:
			OnReqChartDataResend(json_object);
			break;
		case SmProtocol::req_update_quote:
			OnReqUpdateQuote(json_object);
			break;
		case SmProtocol::req_update_hoga:
			OnReqUpdateHoga(json_object);
			break;
		case SmProtocol::req_update_chart_data:
			OnReqUpdateChartData(json_object);
			break;
		case SmProtocol::req_chart_data_onebyone:
			OnReqChartDataOneByOne(json_object, socket);
			break;
		case SmProtocol::req_register_user:
			OnReqRegisterUser(json_object);
			break;
		case SmProtocol::req_account_list:
			OnReqAccountList(json_object);
			break;
		case SmProtocol::req_accepted_order_list:
			OnReqAcceptedList(json_object);
			break;
		case SmProtocol::req_filled_order_list:
			OnReqFilledList(json_object);
			break;
		case SmProtocol::req_position_list:
			OnReqPositionList(json_object);
			break;
		case SmProtocol::req_order_list:
			OnReqOrderList(json_object);
			break;
		case SmProtocol::req_chart_data_resend_onebyone:
			OnReqResendChartDataOneByOne(json_object);
			break;
		case SmProtocol::req_cycle_data_resend_onebyone:
			OnReqCycleDataResendOneByOne(json_object);
			break;
		case SmProtocol::req_reset_account:
			OnReqResetAccount(json_object);
			break;
		case SmProtocol::req_recent_hoga_data_all:
			OnReqRecentHogaDataAll(json_object);
			break;
		default:
			break;
		}
	}
	catch (std::exception e) {
		std::string error = e.what();
	}
}


void SmProtocolManager::OnReqLogin(nlohmann::json& obj, SmWebsocketSession* socket)
{
	try {
		std::string id = obj["user_id"];
		std::string pwd = obj["pwd"];
		SmUserManager* userMgr = SmUserManager::GetInstance();
		userMgr->OnLogin(id, pwd, socket);
	}
	catch (std::exception e) {
		std::string error = e.what();
	}
}

void SmProtocolManager::OnLogout(nlohmann::json& obj)
{
	try {
		auto user_info = obj["user_info"];
		std::string id = user_info["id"];
		SmUserManager* userMgr = SmUserManager::GetInstance();
		userMgr->OnLogout(id);
	}
	catch (std::exception e) {
		std::string error = e.what();
	}
}

void SmProtocolManager::SendResult(std::string user_id, int result_code, std::string result_msg)
{
	json res = {
		{"result_code", result_code},
		{"result_msg", result_msg}
	};
	SmUserManager* userMgr = SmUserManager::GetInstance();
	userMgr->SendResultMessage(user_id, res.dump(4));
}


void SmProtocolManager::SendResult(std::string user_id, SmProtocol protocol, int result_code, std::string result_msg)
{
	json res = {
		{"res_id", (int)protocol},
		{"result_code", result_code},
		{"result_msg", result_msg}
	};
	SmUserManager* userMgr = SmUserManager::GetInstance();
	userMgr->SendResultMessage(user_id, res.dump(4));
}

void SmProtocolManager::SendResult(int session_id, SmProtocol protocol, int result_code, std::string result_msg)
{
	json res = {
		{"res_id", (int)protocol},
		{"result_code", result_code},
		{"result_msg", result_msg}
	};

	SmGlobal* global = SmGlobal::GetInstance();
	std::shared_ptr<SmSessionManager> sessMgr = global->GetSessionManager();
	sessMgr->send(session_id, res.dump(4));
}

void SmProtocolManager::SendResult(int session_id, int result_code, std::string result_msg)
{

}
/// <summary>
/// 클라이언트에서 차트 데이터를 하나씩 보낼때 대응하는 함수
/// 차트 리스트에 저장하고 바로 앱으로 보낸다.
/// </summary>
/// <param name="obj"></param>
void SmProtocolManager::OnReqResendChartDataOneByOne(nlohmann::json& obj)
{
	int req_session_id = obj["req_session_id"];
	std::string symbol_code = obj["symbol_code"];
	SmChartType chart_type = (SmChartType)obj["chart_type"];
	int cycle = obj["cycle"];
	int current_count = obj["current_count"];
	int total_count = obj["total_count"];
	std::string date = obj["date"];
	std::string time = obj["time"];
	int o = obj["o"];
	int h = obj["h"];
	int l = obj["l"];
	int c = obj["c"];
	int v = obj["v"];

	SmChartDataItem data;
	data.current_count = current_count;
	data.total_count = total_count;
	data.symbolCode = symbol_code;
	data.chartType = chart_type;
	data.cycle = cycle;
	data.date = date;
	data.time = time;
	data.date_time = date + time;
	data.h = h;
	data.l = l;
	data.o = o;
	data.c = c;
	data.v = v;

	char buffer[4096];
	sprintf(buffer, "SendChartDataOnebyOne%s : %s\n", date.c_str() , time.c_str());
	OutputDebugString(buffer);
	LOG_F(INFO, "OnReqResendCharrtDataOneByOne %s", data.GetDataKey().c_str());

	// 차트데이터에 추가한다.
	std::shared_ptr<SmChartData> chart_data = SmChartDataManager::GetInstance()->AddChartData(data);
	chart_data->AddData(data);

	if (total_count == current_count) {
		chart_data->Received(true);
	}

	SmTimeSeriesServiceManager* tsMgr = SmTimeSeriesServiceManager::GetInstance();
	tsMgr->SendChartData(req_session_id, data);
}

void SmProtocolManager::OnReqCycleDataResendOneByOne(nlohmann::json& obj)
{
	std::string symbol_code = obj["symbol_code"];
	SmChartType chart_type = (SmChartType)obj["chart_type"];
	int cycle = obj["cycle"];
	std::string date = obj["date"];
	std::string time = obj["time"];
	int o = obj["o"];
	int h = obj["h"];
	int l = obj["l"];
	int c = obj["c"];
	int v = obj["v"];

	SmChartDataItem data;
	data.symbolCode = symbol_code;
	data.chartType = chart_type;
	data.cycle = cycle;
	data.date = date;
	data.time = time;
	data.date_time = date + time;
	data.h = h;
	data.l = l;
	data.o = o;
	data.c = c;
	data.v = v;

	char buffer[4096];
	sprintf(buffer, "SendChartDataOnebyOne%s : %s\n", date.c_str(), time.c_str());
	OutputDebugString(buffer);

	// 차트데이터에 추가한다.
	std::shared_ptr<SmChartData> chart_data = SmChartDataManager::GetInstance()->AddChartData(data);
	// 최소 차트 데이터를 한번은 받은 데이터만 업데이트를 시작한다.
	if (chart_data->Received())
		chart_data->AddData(data);

	SmTimeSeriesServiceManager* tsMgr = SmTimeSeriesServiceManager::GetInstance();
	tsMgr->BroadcastChartData(data);
}


void SmProtocolManager::OnReqResetAccount(nlohmann::json& obj)
{
	std::string account_no = obj["account_no"];
	int session_id = obj["session_id"];
	int result = SmAccountManager::GetInstance()->ResetAccount(account_no);
	std::string msg = "";
	if (result == -1) {
		msg = "계좌 정보가 없습니다.";
	}
	else {
		msg = "계좌가 초기화 되었습니다.";
	}

	SendResult(session_id, result, msg);
}

void SmProtocolManager::OnRegisterSymbol(nlohmann::json& obj)
{
	try {
		std::string id = obj["user_id"];
		std::string symCode = obj["symbol_code"];
		SmRealtimeSymbolServiceManager* rtlSymMgr = SmRealtimeSymbolServiceManager::GetInstance();
		rtlSymMgr->RegisterSymbol(id, symCode);

		SendResult(id, SmProtocol::res_register_symbol, 0, "register symbol success!");
	}
	catch (std::exception e) {
		std::string error = e.what();
	}
}

void SmProtocolManager::OnUnregisterSymbol(nlohmann::json& obj)
{
	try {
		std::string id = obj["user_id"];
		std::string symCode = obj["symbol_code"];
		SmRealtimeSymbolServiceManager* rtlSymMgr = SmRealtimeSymbolServiceManager::GetInstance();
		rtlSymMgr->UnregisterSymbol(id, symCode);

		SendResult(id, SmProtocol::res_unregister_symbol, 0, "register symbol success!");
	}
	catch (std::exception e) {
		std::string error = e.what();
	}
}

void SmProtocolManager::OnRegisterChartCycleData(nlohmann::json& obj)
{
	try {
		std::string id = obj["user_id"];
		std::string symCode = obj["symbol_code"];
		std::string chart_type = obj["chart_type"];
		std::string cycle = obj["cycle"];
		std::string count = obj["count"];
		SmChartDataRequest req;
		req.reqType = SmChartDataReqestType::CYCLE;
		req.user_id = id;
		req.symbolCode = symCode;
		req.chartType = (SmChartType)std::stoi(chart_type);
		req.cycle = std::stoi(cycle);
		req.count = std::stoi(count);
		req.next = 0;
		SmTimeSeriesServiceManager* timeSvcMgr = SmTimeSeriesServiceManager::GetInstance();
		timeSvcMgr->OnRegisterCycleDataRequest(std::move(req));
		SendResult(id, SmProtocol::res_register_chart_cycle_data,  0, "Success!");
	}
	catch (std::exception e) {
		std::string error = e.what();
	}
}

void SmProtocolManager::OnUnregisterChartCycleData(nlohmann::json& obj)
{
	try {
		std::string id = obj["id"];
		std::string symCode = obj["symbol_code"];
		std::string chart_type = obj["chart_type"];
		std::string cycle = obj["cycle"];
		SendResult(id, SmProtocol::res_unregister_chart_cycle_data, 0, "success!");
	}
	catch (std::exception e) {
		std::string error = e.what();
	}
}

void SmProtocolManager::OnOrder(nlohmann::json& obj)
{
	try {
		std::string id = obj["user_id"];
		std::string account_no = obj["account_no"];
		std::string password = obj["password"];
		std::string symbol_code = obj["symbol_code"];
		SmPositionType position_type = (SmPositionType)obj["position_type"];
		SmPriceType price_type = (SmPriceType)obj["price_type"];
		SmFilledCondition filled_condition = (SmFilledCondition)obj["filled_condition"];
		int order_price = obj["order_price"];
		int order_amount = obj["order_amount"];
		int ori_order_no = obj["ori_order_no"];
		int request_id = obj["request_id"];
		SmOrderType order_type = (SmOrderType)obj["order_type"];
		std::string fund_name = obj["fund_name"];
		std::string system_name = obj["system_name"];
		std::string strategy_name = obj["strategy_name"];

		SmOrderRequest request;
		request.UserID = id;
		request.AccountNo = account_no;
		request.SymbolCode = symbol_code;
		request.Password = password;
		request.Position = position_type;
		request.PriceType = price_type;
		request.FillCondition = filled_condition;
		request.OrderPrice = order_price;
		request.OrderAmount = order_amount;
		request.OriOrderNo = ori_order_no;
		request.RequestID = request_id;
		request.OrderType = order_type;
		request.FundName = fund_name;
		request.SystemName = system_name;
		request.StrategyName = strategy_name;
		SmTotalOrderManager* totOrderMgr = SmTotalOrderManager::GetInstance();
		totOrderMgr->OnRequestOrder(std::move(request));
	}
	catch (std::exception e) {
		std::string error = e.what();
	}
}

void SmProtocolManager::OnReqChartData(nlohmann::json& obj)
{
	try {
		std::string chart_id = obj["chart_id"];
		std::string id = obj["user_id"];
		std::string symCode = obj["symbol_code"];
		int chart_type = obj["chart_type"];
		int cycle = obj["cycle"];
		int count = obj["count"];
		SmChartDataRequest req;
		req.reqType = SmChartDataReqestType::FIRST;
		req.user_id = id;
		req.symbolCode = symCode;
		req.chartType = (SmChartType)chart_type;
		req.cycle = cycle;
		req.count = count;
		req.next = 0;
		SmMongoDBManager* mongoMgr = SmMongoDBManager::GetInstance();
		mongoMgr->SendChartDataFromDB(std::move(req));
	}
	catch (std::exception e) {
		std::string error = e.what();
	}
}

void SmProtocolManager::OnReqChartData(std::string message)
{
	
}

void SmProtocolManager::OnReqChartData(nlohmann::json& obj, SmWebsocketSession* socket)
{
	try {
		if (!socket)
			return;

		std::string chart_id = obj["chart_id"];
		std::string id = obj["user_id"];
		std::string symCode = obj["symbol_code"];
		int chart_type = obj["chart_type"];
		int cycle = obj["cycle"];
		int count = obj["count"];
		std::string data_key = SmChartData::MakeDataKey(symCode, chart_type, cycle);
		// 요청이 일별 데이터이면 먼저 데이터베이스에서 찾아 본다.
		if (chart_type == 3) {
			std::shared_ptr<SmSymbol> symbol = SmSymbolManager::GetInstance()->FindSymbol(symCode);
			if (symbol) {
				SmChartDataRequest req;
				req.symbolCode = symCode;
				req.chartType = (SmChartType)chart_type;
				req.cycle = cycle;
				req.count = count;
				req.product_code = symbol->CategoryCode();
				req.session_id = socket->SessionID();
				SmMongoDBManager::GetInstance()->LoadDailyChartData(req);
				SmTimeSeriesServiceManager::GetInstance()->ResendChartDataRequest(req);
				LOG_F(INFO, "OnReqChartData data_count = 0: %s", data_key.c_str());
				return;
			}
		}

		LOG_F(INFO, "OnReqChartData : %s", data_key.c_str());
		std::shared_ptr<SmChartData> chart_data = SmChartDataManager::GetInstance()->AddChartData(symCode, chart_type, cycle);
		size_t data_count = chart_data->GetDataCount();
		// 데이터가 없으면 증권사 서버에 요청을 한다.
		if (data_count == 0) {
			SmChartDataRequest req;
			req.symbolCode = symCode;
			req.chartType = (SmChartType)chart_type;
			req.cycle = cycle;
			req.count = count;
			req.session_id = socket->SessionID();
			SmTimeSeriesServiceManager::GetInstance()->ResendChartDataRequest(req);
			SmMongoDBManager::GetInstance()->SaveChartDataRequest(req);
			LOG_F(INFO, "OnReqChartData data_count = 0: %s", data_key.c_str());
		} 
		else { // 데이터가 있으면 바로 보낸다.
			LOG_F(INFO, "OnReqChartData direct send : %s", data_key.c_str());
			SmTimeSeriesServiceManager::GetInstance()->SendChartData(socket->SessionID(), chart_data);
		}
	}
	catch (std::exception e) {
		std::string error = e.what();
	}
}

void SmProtocolManager::OnReqSiseData(nlohmann::json& obj)
{
	try {
		std::string id = obj["user_id"];
		std::string symCode = obj["symbol_code"];
		SmSiseDataRequest req;
		req.symbol_code = symCode;
		req.user_id = id;
		SmTimeSeriesServiceManager* timeSvcMgr = SmTimeSeriesServiceManager::GetInstance();
		timeSvcMgr->OnSiseDataRequest(std::move(req));
	}
	catch (std::exception e) {
		std::string error = e.what();
	}
}

void SmProtocolManager::OnReqHogaData(nlohmann::json& obj)
{
	try {
		std::string id = obj["user_id"];
		std::string symCode = obj["symbol_code"];
		SmHogaDataRequest req;
		req.symbol_code = symCode;
		req.user_id = id;
		SmTimeSeriesServiceManager* timeSvcMgr = SmTimeSeriesServiceManager::GetInstance();
		timeSvcMgr->OnHogaDataRequest(std::move(req));
	}
	catch (std::exception e) {
		std::string error = e.what();
	}
}

void SmProtocolManager::OnReqSymbolMaster(nlohmann::json& obj)
{
	try {
		std::string id = obj["user_id"];
		std::string symCode = obj["symbol_code"];
		SmSymbolMasterRequest req;
		req.symbol_code = symCode;
		req.user_id = id;
		SmTimeSeriesServiceManager* timeSvcMgr = SmTimeSeriesServiceManager::GetInstance();
		timeSvcMgr->OnSymbolMasterRequest(std::move(req));
	}
	catch (std::exception e) {
		std::string error = e.what();
	}
}

void SmProtocolManager::OnReqSymbolMasterAll(nlohmann::json& obj)
{
	try {
		SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
		std::map<std::string, std::shared_ptr<SmSymbol>>& symbolMap = symMgr->GetSymbolMap();
		for (auto it = symbolMap.begin(); it != symbolMap.end(); ++it) {
			std::string id = obj["user_id"];
			std::string symCode = it->second->SymbolCode();
			SmSymbolMasterRequest req;
			req.symbol_code = symCode;
			req.user_id = id;
			SmTimeSeriesServiceManager* timeSvcMgr = SmTimeSeriesServiceManager::GetInstance();
			timeSvcMgr->OnSymbolMasterRequest(std::move(req));
		}
	}
	catch (std::exception e) {
		std::string error = e.what();
	}
}

void SmProtocolManager::OnReqSiseDataAll(nlohmann::json& obj)
{
	try {
		SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
		std::map<std::string, std::shared_ptr<SmSymbol>>& symbolMap = symMgr->GetSymbolMap();
		for (auto it = symbolMap.begin(); it != symbolMap.end(); ++it) {
			std::string id = obj["user_id"];
			std::string symCode = it->second->SymbolCode();
			SmSiseDataRequest req;
			req.symbol_code = symCode;
			req.user_id = id;
			SmTimeSeriesServiceManager* timeSvcMgr = SmTimeSeriesServiceManager::GetInstance();
			timeSvcMgr->OnSiseDataRequest(std::move(req));
		}
	}
	catch (std::exception e) {
		std::string error = e.what();
	}
}

void SmProtocolManager::OnReqRecentSiseDataAll(nlohmann::json& obj)
{
	try {
		int session_id = obj["session_id"];
		std::string content = "";
		SmMarketManager* symMgr = SmMarketManager::GetInstance();
		std::vector<std::shared_ptr<SmSymbol>> symVec = symMgr->GetRecentMonthSymbolList();
		int total = (int)symVec.size();
		int current = 1;
		for (size_t i = 0; i < symVec.size(); ++i) {
			std::string symCode = symVec[i]->SymbolCode();
			std::shared_ptr<SmSymbol> sym = SmSymbolManager::GetInstance()->FindSymbol(symCode);
			if (sym)
				content = sym->GetQuoteByJson(total, current);
			else // 심볼이 없을 때는 빈값을 전송한다.
				content = SmSymbol::GetDummyQuoteByJson(total, current);
			current++;
			// 바로 전송한다.
			SmGlobal* global = SmGlobal::GetInstance();
			std::shared_ptr<SmSessionManager> sessMgr = global->GetSessionManager();
			sessMgr->send(session_id, content);
		}
	}
	catch (std::exception e) {
		std::string error = e.what();
	}
}


void SmProtocolManager::OnReqRecentHogaDataAll(nlohmann::json& obj)
{
	try {
		int session_id = obj["session_id"];
		std::string content = "";
		SmMarketManager* symMgr = SmMarketManager::GetInstance();
		std::vector<std::shared_ptr<SmSymbol>> symVec = symMgr->GetRecentMonthSymbolList();
		int total = (int)symVec.size();
		int current = 1;
		for (size_t i = 0; i < symVec.size(); ++i) {
			std::string symCode = symVec[i]->SymbolCode();
			std::shared_ptr<SmSymbol> sym = SmSymbolManager::GetInstance()->FindSymbol(symCode);
			if (sym)
				content = sym->GetHogaByJson(total, current);
			else // 심볼이 없을 때는 빈값을 전송한다.
				content = SmSymbol::GetDummyHogaByJson(total, current);
			current++;
			// 바로 전송한다.
			SmGlobal* global = SmGlobal::GetInstance();
			std::shared_ptr<SmSessionManager> sessMgr = global->GetSessionManager();
			sessMgr->send(session_id, content);
		}
	}
	catch (std::exception e) {
		std::string error = e.what();
	}
}


void SmProtocolManager::OnReqRegisterRecentRealtimeSiseAll(nlohmann::json& obj)
{
	try {
		SmMarketManager* symMgr = SmMarketManager::GetInstance();
		std::vector<std::shared_ptr<SmSymbol>> symVec = symMgr->GetRecentMonthSymbolList();
		for (auto it = symVec.begin(); it != symVec.end(); ++it) {
			std::string id = obj["user_id"];
			std::string symCode = (*it)->SymbolCode();
			SmRealtimeSymbolServiceManager* rtlSymMgr = SmRealtimeSymbolServiceManager::GetInstance();
			rtlSymMgr->RegisterSymbol(id, symCode);
		}
	}
	catch (std::exception e) {
		std::string error = e.what();
	}
}

void SmProtocolManager::OnReqMarketList(nlohmann::json& obj)
{
	try {
		SmMarketManager* marketMgr = SmMarketManager::GetInstance();
		int session_id = obj["session_id"];
		marketMgr->SendMarketList(session_id);
	}
	catch (std::exception e) {
		std::string error = e.what();
	}
}

void SmProtocolManager::OnReqSymbolListByCategory(nlohmann::json& obj)
{
	try {
		SmMarketManager* marketMgr = SmMarketManager::GetInstance();
		int session_id = obj["session_id"];
		marketMgr->SendSymbolListByCategory(session_id);
	}
	catch (std::exception e) {
		std::string error = e.what();
	}
}

void SmProtocolManager::OnReqChartDataResend(nlohmann::json& obj)
{
	int service_req_id = obj["service_req_id"];
	int session_id = obj["req_session_id"];
	std::string user_id = obj["user_id"];
	std::string symbol_code = obj["symbol_code"];
	SmChartType chart_type = (SmChartType)obj["chart_type"];
	int cycle = obj["cycle"];
	int count = obj["count"];
	SmChartDataRequest req;
	req.reqType = SmChartDataReqestType::FIRST;
	req.user_id = user_id;
	req.session_id = session_id;
	req.symbolCode = symbol_code;
	req.chartType = (SmChartType)chart_type;
	req.cycle = cycle;
	req.count = count;
	req.next = 0;
	SmMongoDBManager* mongoMgr = SmMongoDBManager::GetInstance();
	mongoMgr->SendChartDataOneByOne(std::move(req));
}

void SmProtocolManager::OnReqUpdateQuote(nlohmann::json& obj)
{
	obj["res_id"] = (int)SmProtocol::res_realtime_sise;
	std::string symbol_code = obj["symbol_code"];
	std::shared_ptr<SmSymbol> sym = SmSymbolManager::GetInstance()->FindSymbol(symbol_code);
	if (sym) {
		int gap_from_preday = obj["gap_from_preday"];
		std::string sign_to_preday = obj["sign_to_preday"];
		std::string ratio_to_preday = obj["ratio_to_preday"];
		int open = obj["open"];
		int high = obj["high"];
		int low = obj["low"];
		int close = obj["close"];
		int acc_volume = obj["acc_volume"];
		std::string time = obj["time"];
		sym->Quote.OriginTime = time;
		sym->Quote.accVolume = acc_volume;
		sym->Quote.Open = open;
		sym->Quote.High = high;
		sym->Quote.Low = low;
		sym->Quote.Close = close;
		sym->Quote.accVolume = acc_volume;
		sym->Quote.GapFromPreDay = gap_from_preday;
		sym->Quote.RatioToPreday = ratio_to_preday;
		sym->Quote.SignToPreDay = sign_to_preday;
	}

	std::string content = obj.dump(4);
	SmGlobal* global = SmGlobal::GetInstance();
	std::shared_ptr<SmSessionManager> sessMgr = global->GetSessionManager();
	sessMgr->send(content);
}

void SmProtocolManager::OnReqUpdateHoga(nlohmann::json& obj)
{
	obj["res_id"] = (int)SmProtocol::res_realtime_hoga;
	std::string symbol_code = obj["symbol_code"];
	std::shared_ptr<SmSymbol> sym = SmSymbolManager::GetInstance()->FindSymbol(symbol_code);
	if (sym) {
		sym->Hoga.Time = obj["time"];
		sym->Hoga.DomesticDate = obj["domestic_date"];
		sym->Hoga.DomesticTime = obj["domestic_time"];
		sym->Hoga.TotBuyQty = obj["tot_buy_qty"];
		sym->Hoga.TotSellQty = obj["tot_sell_qty"];
		sym->Hoga.TotBuyCnt = obj["tot_buy_cnt"];
		sym->Hoga.TotSellCnt = obj["tot_sell_cnt"];

		for (int i = 0; i < 5; i++) {
			sym->Hoga.Ary[i].BuyPrice = obj["hoga_items"][i]["buy_price"];
			sym->Hoga.Ary[i].BuyCnt = obj["hoga_items"][i]["buy_cnt"];
			sym->Hoga.Ary[i].BuyQty = obj["hoga_items"][i]["buy_qty"];
			sym->Hoga.Ary[i].SellPrice = obj["hoga_items"][i]["sell_price"];
			sym->Hoga.Ary[i].SellCnt = obj["hoga_items"][i]["sell_cnt"];
			sym->Hoga.Ary[i].SellQty = obj["hoga_items"][i]["sell_qty"];
		}
	}

	std::string content = obj.dump(4);
	SmGlobal* global = SmGlobal::GetInstance();
	std::shared_ptr<SmSessionManager> sessMgr = global->GetSessionManager();
	sessMgr->send(content);
}

void SmProtocolManager::OnReqUpdateChartData(nlohmann::json& obj)
{
	std::string symbol_code = obj["symbol_code"];
	SmChartType chart_type = (SmChartType)obj["chart_type"];
	int cycle = obj["cycle"];
	int count = 3;
	SmChartDataRequest req;
	req.reqType = SmChartDataReqestType::FIRST;
	req.symbolCode = symbol_code;
	req.chartType = (SmChartType)chart_type;
	req.cycle = cycle;
	req.count = count;
	SmMongoDBManager* mongoMgr = SmMongoDBManager::GetInstance();
	//mongoMgr->SendChartCycleData(std::move(req));
}

void SmProtocolManager::OnReqRegisterUser(nlohmann::json& obj)
{
	std::string user_id = obj["user_id"];
	std::string password = obj["password"];
	int session_id = obj["session_id"];
	// 사용자 정보를 데이터베이스에 저장한다.
	SmMongoDBManager* mongoMgr = SmMongoDBManager::GetInstance();
	mongoMgr->SaveUserInfo(user_id, password);

	// 계좌를 만들고 데이터베이스에 저장한다.
	SmAccountManager* acntMgr = SmAccountManager::GetInstance();
	// 해외 계좌를 먼저 만든다.
	acntMgr->CreateAccount(user_id, password, 0);
	// 국내 계좌도 만든다.
	acntMgr->CreateAccount(user_id, password, 1);

	// 등록 결과 메시지를 보낸다.
	obj["res_id"] = (int)SmProtocol::res_register_user;
	obj["message"] = "Registered a user successfully!";
	obj["user_id"] = user_id;
	obj["password"] = password;
	std::string content = obj.dump(4);
	SmGlobal* global = SmGlobal::GetInstance();
	std::shared_ptr<SmSessionManager> sessMgr = global->GetSessionManager();
	sessMgr->send(session_id, content);
}

void SmProtocolManager::OnReqUnregisterUser(nlohmann::json& obj)
{
	std::string user_id = obj["user_id"];
	SmMongoDBManager* mongoMgr = SmMongoDBManager::GetInstance();
	mongoMgr->RemoveUserInfo(user_id);

	obj["res_id"] = (int)SmProtocol::res_unregister_user;
	obj["message"] = "Unregistered a user successfully!";
	std::string content = obj.dump(4);
	SmGlobal* global = SmGlobal::GetInstance();
	std::shared_ptr<SmSessionManager> sessMgr = global->GetSessionManager();
	sessMgr->send(content);
}

void SmProtocolManager::OnReqChartDataOneByOne(nlohmann::json& obj, SmWebsocketSession* socket)
{
	try {
		if (!socket)
			return;

		std::string chart_id = obj["chart_id"];
		std::string id = obj["user_id"];
		std::string symCode = obj["symbol_code"];
		int chart_type = obj["chart_type"];
		int cycle = obj["cycle"];
		int count = obj["count"];
		SmChartDataRequest req;
		req.reqType = SmChartDataReqestType::FIRST;
		req.user_id = id;
		req.session_id = socket->SessionID();
		req.symbolCode = symCode;
		req.chartType = (SmChartType)chart_type;
		req.cycle = cycle;
		req.count = count;
		req.next = 0;
		SmMongoDBManager* mongoMgr = SmMongoDBManager::GetInstance();
		mongoMgr->SendChartDataFromDB(std::move(req));
	}
	catch (std::exception e) {
		std::string error = e.what();
	}
}

void SmProtocolManager::OnReqAccountList(nlohmann::json& obj)
{
	try {
		
		std::string user_id = obj["user_id"];
		int session_id = obj["session_id"];
		SmAccountManager::GetInstance()->SendAccountList(session_id, user_id);
	}
	catch (std::exception e) {
		std::string error = e.what();
	}
}

void SmProtocolManager::OnReqAcceptedList(nlohmann::json& obj)
{
	try {

		std::string user_id = obj["user_id"];
		int session_id = obj["session_id"];
		auto account_no_list = obj["account_no_list"];
		std::vector<std::string> account_vec;
		for (size_t i = 0; i < account_no_list.size(); ++i) {
			account_vec.push_back(account_no_list[i]);
		}
		
		SmTotalOrderManager::GetInstance()->SendAcceptedOrderList(session_id, account_vec);
	}
	catch (std::exception e) {
		std::string error = e.what();
	}
}

void SmProtocolManager::OnReqFilledList(nlohmann::json& obj)
{
	try {

		std::string user_id = obj["user_id"];
		int session_id = obj["session_id"];
		auto account_no_list = obj["account_no_list"];
		std::vector<std::string> account_vec;
		for (size_t i = 0; i < account_no_list.size(); ++i) {
			account_vec.push_back(account_no_list[i]);
		}
		SmTotalOrderManager::GetInstance()->SendFilledOrderList(session_id, account_vec);
	}
	catch (std::exception e) {
		std::string error = e.what();
	}
}

void SmProtocolManager::OnReqOrderList(nlohmann::json& obj)
{
	try {

		std::string user_id = obj["user_id"];
		int session_id = obj["session_id"];
		std::string account_no = obj["account_no"];
		std::string request_date = obj["request_date"];
		SmTotalOrderManager::GetInstance()->SendOrderList(session_id, account_no, request_date);
	}
	catch (std::exception e) {
		std::string error = e.what();
	}
}

void SmProtocolManager::OnReqPositionList(nlohmann::json& obj)
{
	try {

		std::string user_id = obj["user_id"];
		int session_id = obj["session_id"];
		auto account_no_list = obj["account_no_list"];
		std::vector<std::string> account_vec;
		for (size_t i = 0; i < account_no_list.size(); ++i) {
			account_vec.push_back(account_no_list[i]);
		}
		SmTotalPositionManager::GetInstance()->SendPositionList(session_id, account_vec);
	}
	catch (std::exception e) {
		std::string error = e.what();
	}
}
