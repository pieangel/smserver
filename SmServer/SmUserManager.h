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
	SmUser* FindUserBySocket(SmWebsocketSession* socket);
	SmUser* AddUser(std::string id, SmWebsocketSession* socket);
	SmUser* AddUser(std::string id, std::string pwd, SmWebsocketSession* socket);
	std::string CheckUserInfo(std::string id, std::string pwd, SmWebsocketSession* socket);
	void SendBroadcastMessage(std::string message);
	SmUser* FindUser(std::string id);
	void ResetUserBySocket(SmWebsocketSession* socket);
	void SendResultMessage(std::string user_id, std::string message);
	void Logout(std::string id);
	bool IsExistUser(std::string id);
	void OnLogin(std::string id, std::string pwd, SmWebsocketSession* socket);
	void OnLogout(std::string id);
	int GetSendBufferQueueSize(std::string user_id);
private:
	void AddUserToDatabase(std::string id, std::string pwd);
	void RemoveUser(std::string id);
	void ClearAllService(SmUser* user);
	void SendLoginResult(std::string user_id, std::string msg);
	void SendLogoutResult(std::string user_id);
};

