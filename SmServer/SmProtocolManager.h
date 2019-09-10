#pragma once
#include "Global/TemplateSingleton.h"
#include <string>
#include "Json/json.hpp"
#include "SmServiceDefine.h"
#include <mutex>
class SmWebsocketSession;
class SmProtocolManager : public TemplateSingleton<SmProtocolManager>
{
public:
	SmProtocolManager();
	~SmProtocolManager();

	void OnMessage(std::string message, SmWebsocketSession* socket);
private:
	void OnReqChartData(std::string message);
	void ParseMessage(std::string message, SmWebsocketSession* socket);
	void SendResult(std::string user_id, int result_code, std::string result_msg);
	void OnLogin(nlohmann::json& obj, SmWebsocketSession* socket);
	void OnLogout(nlohmann::json& obj);
	void OnRegisterSymbol(nlohmann::json& obj);
	void OnUnregisterSymbol(nlohmann::json& obj);
	void OnRegisterChartCycleData(nlohmann::json& obj);
	void OnUnregisterChartCycleData(nlohmann::json& obj);
	void OnOrder(nlohmann::json& obj);
	void OnReqChartData(nlohmann::json& obj);
	void OnReqChartData(nlohmann::json& obj, SmWebsocketSession* socket);
	void OnReqSiseData(nlohmann::json& obj);
	void OnReqHogaData(nlohmann::json& obj);
	void OnReqSymbolMaster(nlohmann::json& obj);
	void OnReqSymbolMasterAll(nlohmann::json& obj);
	void OnReqSiseDataAll(nlohmann::json& obj);
	void OnReqRecentSiseDataAll(nlohmann::json& obj);
	void OnReqRegisterRecentRealtimeSiseAll(nlohmann::json& obj);
	void OnReqMarketList(nlohmann::json& obj);
	void OnReqSymbolListByCategory(nlohmann::json& obj);
	void OnReqChartDataResend(nlohmann::json& obj);
	void SendResult(std::string user_id, SmProtocol protocol, int result_code, std::string result_msg);
};

