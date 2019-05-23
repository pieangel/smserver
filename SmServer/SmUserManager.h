#pragma once
#include <memory>
#include <mutex>
#include <string>
#include <unordered_set>
#include <map>
#include "Global/TemplateSingleton.h"
class SmUser;
class SmWebsocketSession;
class SmUserManager : public TemplateSingleton<SmUserManager>
{
	// This mutex synchronizes all access to sessions_
	std::mutex _mutex;

	// Keep a list of all the connected clients
	std::map<std::string, SmUser*> _UserMap;
	std::map<SmWebsocketSession*, SmUser*> _SocketToUserMap;
public:
	SmUserManager();
	~SmUserManager();
	SmUser* AddUser(std::string id, SmWebsocketSession* socket);
	void DeleteUser(std::string id);
	void SendBroadcastMessage(std::string message);
};

