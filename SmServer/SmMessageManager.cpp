#include "pch.h"
#include "SmMessageManager.h"
#include "SmUserManager.h"
#include "Json/json.hpp"
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
	if (!socket)
		return;

	json j2 = {
	{"req_id", 1},
	{"user_info", {
		{"id", "angelpie"},
		{"pwd", "11orion!!"}
	}}
	};

	std::string received_message = j2.dump(4);

	auto j3 = json::parse(received_message);
	int req_id = j3["req_id"];
	auto user_info = j3["user_info"];
	std::string id = user_info["id"];
	std::string pwd = user_info["pwd"];
	if (req_id == 1) {
		SmUserManager* userMgr = SmUserManager::GetInstance();
		userMgr->AddUser(id, pwd, socket);
		json res = {
			{"result", 0},
			{"message", "success!"}
		};
		userMgr->SendBroadcastMessage(res.dump(4));
	}
}
