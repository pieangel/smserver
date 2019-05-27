#include "SmRealtimeSymbolServiceManager.h"
#include "SmUser.h"
#include "SmWebsocketSession.h"
void SmRealtimeSymbolServiceManager::Register(SmUser* user)
{
	if (!user)
		return;
	_UserMap[user->Id()] = user;
}

void SmRealtimeSymbolServiceManager::Unregister(std::string user_id)
{
	auto it = _UserMap.find(user_id);
	if (it != _UserMap.end()) {
		_UserMap.erase(it);
	}
}

void SmRealtimeSymbolServiceManager::SendInfo()
{
	if (!_Symbol)
		return;
	std::string message = "test message";
	// Put the message in a shared pointer so we can re-use it for each client
	auto const ss = boost::make_shared<std::string const>(std::move(message));

	// Make a local list of all the weak pointers representing
	// the sessions, so we can do the actual sending without
	// holding the mutex:
	std::vector<std::weak_ptr<SmWebsocketSession>> v;
	{
		std::lock_guard<std::mutex> lock(_mutex);
		v.reserve(_UserMap.size());
		for (auto it = _UserMap.begin(); it != _UserMap.end(); ++it) {
			SmUser* user = it->second;
			v.emplace_back(user->Socket()->weak_from_this());
		}
	}

	// For each session in our local list, try to acquire a strong
   // pointer. If successful, then send the message on that session.
	for (auto const& wp : v)
		if (auto sp = wp.lock())
			sp->send(ss);
}