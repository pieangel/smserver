#pragma once
#include "Global/TemplateSingleton.h"
#include <string>
#include "Json/json.hpp"
class SmWebsocketSession;
class SmMessageManager : public TemplateSingleton<SmMessageManager>
{
public:
	SmMessageManager();
	~SmMessageManager();

	void OnMessage(std::string message, SmWebsocketSession* socket);
private:
	void ParseMessage(std::string message, SmWebsocketSession* socket);
	void SendResult(std::string user_id, int result_code, std::string result_msg);
	void OnLogin(nlohmann::json& obj, SmWebsocketSession* socket);
	void OnRegisterSymbol(nlohmann::json& obj, SmWebsocketSession* socket);
	void OnRegisterSymbolCycle(nlohmann::json& obj, SmWebsocketSession* socket);
};

