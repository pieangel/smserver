#include "SmRealtimeSymbolServiceManager.h"
#include "SmUser.h"
#include "SmWebsocketSession.h"
#include "SmSymbolManager.h"
#include "SmUserManager.h"
#include "SmSymbol.h"
#include "SmMarketManager.h"
#include "SmHdClient.h"

void SmRealtimeSymbolServiceManager::RegisterAllRecentSymbol()
{
	SmHdClient* client = SmHdClient::GetInstance();
	SmMarketManager* mrktMgr = SmMarketManager::GetInstance();
	std::vector<std::shared_ptr<SmSymbol>> symVec = mrktMgr->GetRecentMonthSymbolList();
	for (auto it = symVec.begin(); it != symVec.end(); ++it) {
		client->RegisterProduct((*it)->SymbolCode());
	}
}

void SmRealtimeSymbolServiceManager::RegisterSymbol(std::string user_id, std::string symCode)
{
	SmUserManager* userMgr = SmUserManager::GetInstance();
	std::shared_ptr<SmUser> user = userMgr->FindUser(user_id);
	SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
	std::shared_ptr<SmSymbol> sym = symMgr->FindSymbol(symCode);
	if (!user || !sym)
		return;
	RegisterSymbol(user, sym);
}

void SmRealtimeSymbolServiceManager::RegisterSymbol(std::shared_ptr<SmUser> user, std::shared_ptr<SmSymbol> sym)
{
	if (!user || !sym)
		return;
	{
		std::lock_guard<std::mutex> lock(_mutex);

		auto it = _ServiceMap.find(sym);
		if (it != _ServiceMap.end()) {
			SmUserMap& userMap = it->second;
			userMap[user] = user;
		}
		else {
			SmUserMap userMap;
			userMap[user] = user;
			_ServiceMap[sym] = userMap;
		}
	}
}

void SmRealtimeSymbolServiceManager::UnregisterSymbol(std::string user_id, std::string symCode)
{
	SmUserManager* userMgr = SmUserManager::GetInstance();
	std::shared_ptr<SmUser> user = userMgr->FindUser(user_id);
	SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
	std::shared_ptr<SmSymbol> sym = symMgr->FindSymbol(symCode);
	if (!user || !sym)
		return;
	UnregisterSymbol(user, sym);
}

void SmRealtimeSymbolServiceManager::UnregisterSymbol(std::shared_ptr<SmUser> user, std::shared_ptr<SmSymbol> sym)
{
	if (!user || !sym)
		return;
	{
		std::lock_guard<std::mutex> lock(_mutex);

		auto it = _ServiceMap.find(sym);
		if (it != _ServiceMap.end()) {
			SmUserMap& userMap = it->second;
			auto itu = userMap.find(user);
			if (itu != userMap.end()) {
				userMap.erase(itu);
			}
		}
	}
}

void SmRealtimeSymbolServiceManager::UnregisterAllSymbol(std::string user_id)
{
	SmUserManager* userMgr = SmUserManager::GetInstance();
	std::shared_ptr<SmUser> user = userMgr->FindUser(user_id);
	if (!user)
		return;
	UnregisterAllSymbol(user);
}

void SmRealtimeSymbolServiceManager::UnregisterAllSymbol(std::shared_ptr<SmUser> user)
{
	if (!user)
		return;
	{
		std::lock_guard<std::mutex> lock(_mutex);

		for (auto it = _ServiceMap.begin(); it != _ServiceMap.end(); ++it) {
			SmUserMap& userMap = it->second;
			auto itu = userMap.find(user);
			if (itu != userMap.end()) {
				userMap.erase(itu);
			}
		}
	}
}

void SmRealtimeSymbolServiceManager::Register(std::shared_ptr<SmUser> user)
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
	//if (!_Symbol)
	//	return;
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
			std::shared_ptr<SmUser> user = it->second;
			if (user->Socket())
				v.emplace_back(user->Socket()->weak_from_this());
		}
	}

	// For each session in our local list, try to acquire a strong
   // pointer. If successful, then send the message on that session.
	for (auto const& wp : v)
		if (auto sp = wp.lock())
			sp->send(ss);
}

void SmRealtimeSymbolServiceManager::BroadcastSise()
{
	for (auto it = _ServiceMap.begin(); it != _ServiceMap.end(); ++it) {
		SendSise(it->first, it->second);
	}
}

void SmRealtimeSymbolServiceManager::BroadcastHoga()
{
	for (auto it = _ServiceMap.begin(); it != _ServiceMap.end(); ++it) {
		SendHoga(it->first, it->second);
	}
}

void SmRealtimeSymbolServiceManager::SendSise(std::shared_ptr<SmSymbol> sym, SmUserMap& userMap)
{
	if (!sym || userMap.size() == 0)
		return;
	std::string message = sym->GetQuoteByJson();
	// Put the message in a shared pointer so we can re-use it for each client
	auto const ss = boost::make_shared<std::string const>(std::move(message));

	std::vector<std::weak_ptr<SmWebsocketSession>> v;
	{
		std::lock_guard<std::mutex> lock(_mutex);
		v.reserve(userMap.size());
		for (auto it = userMap.begin(); it != userMap.end(); ++it) {
			std::shared_ptr<SmUser> user = it->second;
			if (user->Connected() && user->Socket())
				v.emplace_back(user->Socket()->weak_from_this());
		}
	}

	// For each session in our local list, try to acquire a strong
	// pointer. If successful, then send the message on that session.
	for (auto const& wp : v)
		if (auto sp = wp.lock())
			sp->send(ss);
}

void SmRealtimeSymbolServiceManager::SendHoga(std::shared_ptr<SmSymbol> sym, SmUserMap& userMap)
{
	if (!sym || userMap.size() == 0)
		return;
	std::string message = sym->GetHogaByJson();
	// Put the message in a shared pointer so we can re-use it for each client
	auto const ss = boost::make_shared<std::string const>(std::move(message));

	std::vector<std::weak_ptr<SmWebsocketSession>> v;
	{
		std::lock_guard<std::mutex> lock(_mutex);
		v.reserve(userMap.size());
		for (auto it = userMap.begin(); it != userMap.end(); ++it) {
			std::shared_ptr<SmUser> user = it->second;
			if (user->Connected() && user->Socket())
				v.emplace_back(user->Socket()->weak_from_this());
		}
	}

	// For each session in our local list, try to acquire a strong
	// pointer. If successful, then send the message on that session.
	for (auto const& wp : v)
		if (auto sp = wp.lock())
			sp->send(ss);
}
