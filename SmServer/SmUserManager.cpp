#include "SmUserManager.h"
#include "SmUser.h"
#include "SmWebsocketSession.h"
SmUserManager::SmUserManager()
{

}

SmUserManager::~SmUserManager()
{
	std::lock_guard<std::mutex> lock(_mutex);
	for (auto it = _UserMap.begin(); it != _UserMap.end(); ++it) {
		delete it->second;
	}
}

SmUser* SmUserManager::FindUserBySocket(SmWebsocketSession* socket)
{
	if (!socket)
		return nullptr;
	auto it = _SocketToUserMap.find(socket);
	if (it != _SocketToUserMap.end()) {
		return it->second;
	}

	return nullptr;
}

SmUser* SmUserManager::AddUser(std::string id, SmWebsocketSession* socket)
{
	std::lock_guard<std::mutex> lock(_mutex);
	if (!socket)
		return nullptr;

	SmUser* user = new SmUser();
	user->Id(id);
	user->Socket(socket);
	_UserMap[id] = user;
	_SocketToUserMap[socket] = user;
	return user;
}

SmUser* SmUserManager::AddUser(std::string id, std::string pwd, SmWebsocketSession* socket)
{
	std::lock_guard<std::mutex> lock(_mutex);

	if (!socket)
		return nullptr;
	SmUser* user = FindUser(id);
	if (user)
		return user;

	user = new SmUser();
	user->Id(id);
	user->Password(pwd);
	user->Socket(socket);
	_UserMap[id] = user;
	_SocketToUserMap[socket] = user;
	return user;
}

void SmUserManager::RemoveUser(std::string id)
{
	auto it = _UserMap.find(id);
	if (it != _UserMap.end()) {
		_UserMap.erase(it);
	}
}

void SmUserManager::SendBroadcastMessage(std::string message)
{
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
			if (user->Connected())
				v.emplace_back(user->Socket()->weak_from_this());
		}
	}

	// For each session in our local list, try to acquire a strong
   // pointer. If successful, then send the message on that session.
	for (auto const& wp : v)
		if (auto sp = wp.lock())
			sp->send(ss);

}

SmUser* SmUserManager::FindUser(std::string id)
{
	auto it = _UserMap.find(id);
	if (it != _UserMap.end()) {
		return it->second;
	}

	return nullptr;
}

void SmUserManager::ResetUserBySocket(SmWebsocketSession* socket)
{
	std::lock_guard<std::mutex> lock(_mutex);

	auto it = _SocketToUserMap.find(socket);
	if (it != _SocketToUserMap.end()) {
		it->second->Reset();
	}
}
