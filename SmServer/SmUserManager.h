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
	SmUser* AddUser(std::string id, std::string pwd, SmWebsocketSession* socket);
	void DeleteUser(std::string id);
	void DeleteUser(SmWebsocketSession* socket);
	void SendBroadcastMessage(std::string message);
	SmUser* FindUser(std::string id);
private:
	void RemoveUser(std::string id);
};
