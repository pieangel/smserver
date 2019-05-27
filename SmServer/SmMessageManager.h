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
};

