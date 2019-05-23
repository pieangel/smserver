#pragma once
#include <map>
#include <string>
class SmUser;
class SmRealtimeRegisterManager
{
public:
	SmRealtimeRegisterManager();
	~SmRealtimeRegisterManager();
	void AddUser(SmUser* user);
	void RemoveUser(std::string user_id);
private:
	std::map<std::string, SmUser*> _UserMap;
};

