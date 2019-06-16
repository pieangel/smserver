#pragma once
#include "Global/TemplateSingleton.h"
#include <string>
#include "Json/json.hpp"
#include "SmServiceDefine.h"
class SmWebsocketSession;
class SmProtocolManager : public TemplateSingleton<SmProtocolManager>
{
public:
	SmProtocolManager();
	~SmProtocolManager();

	void OnMessage(std::string message, SmWebsocketSession* socket);
private:
	void ParseMessage(std::string message, SmWebsocketSession* socket);
	void SendResult(std::string user_id, int result_code, std::string result_msg);
	void OnLogin(nlohmann::json& obj, SmWebsocketSession* socket);
	void OnLogout(nlohmann::json& obj);
	void OnRegisterSymbol(nlohmann::json& obj);
	void OnUnregisterSymbol(nlohmann::json& obj);
	void OnRegisterChartCycleData(nlohmann::json& obj);
	void OnUnregisterChartCycleData(nlohmann::json& obj);
	void OnOrderNew(nlohmann::json& obj);
	void OnOrderModify(nlohmann::json& obj);
	void OnOrderCancel(nlohmann::json& obj);
	void OnReqChartData(nlohmann::json& obj);
	void OnReqSiseData(nlohmann::json& obj);
	void SendResult(std::string user_id, SmProtocol protocol, int result_code, std::string result_msg);
};
