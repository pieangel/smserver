#pragma once
#include "Global/TemplateSingleton.h"
#include <string>
class SmWebsocketSession;
class SmMessageManager : public TemplateSingleton<SmMessageManager>
{
public:
	SmMessageManager();
	~SmMessageManager();

	void OnMessage(std::string message, SmWebsocketSession* socket);
private:
	void ParseMessage(std::string message, SmWebsocketSession* socket);
	void OnLogin(std::string message, SmWebsocketSession* socket);
	void SendResult(std::string user_id, int result_code, std::string result_msg);
	void OnRegisterSymbol(std::string message);
	void OnRegisterSymbolCycle(std::string message);
};

