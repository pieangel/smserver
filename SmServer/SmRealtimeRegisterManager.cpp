#include "SmRealtimeRegisterManager.h"
#include "SmUser.h"
SmRealtimeRegisterManager::SmRealtimeRegisterManager()
{

}

SmRealtimeRegisterManager::~SmRealtimeRegisterManager()
{

}

void SmRealtimeRegisterManager::AddUser(SmUser* user)
{
	if (!user)
		return;
	_UserMap[user->Id()] = user;
}

void SmRealtimeRegisterManager::RemoveUser(std::string user_id)
{
	auto it = _UserMap.find(user_id);
	if (it != _UserMap.end()) {
		_UserMap.erase(it);
	}
}
