#include "SmSessionManager.h"
#include "SmWebSocketSession.h"
#include "SmUserManager.h"
#include "SmRealtimeSymbolServiceManager.h"
#include "SmSymbolManager.h"
#include "SmSymbol.h"
#include "SmUser.h"

SmSessionManager::
SmSessionManager(std::string doc_root)
	: doc_root_(std::move(doc_root))
{
}

void
SmSessionManager::
join(SmWebsocketSession* session)
{
	std::lock_guard<std::mutex> lock(mutex_);
	sessions_.insert(session);
}

void
SmSessionManager::
leave(SmWebsocketSession* session)
{
	std::lock_guard<std::mutex> lock(mutex_);
	SmUserManager* userMgr = SmUserManager::GetInstance();
	userMgr->ResetUserBySocket(session);
	sessions_.erase(session);
}

// Broadcast a message to all websocket client sessions
void
SmSessionManager::
send(std::string message)
{
	// Put the message in a shared pointer so we can re-use it for each client
	auto const ss = boost::make_shared<std::string const>(std::move(message));

	// Make a local list of all the weak pointers representing
	// the sessions, so we can do the actual sending without
	// holding the mutex:
	std::vector<std::weak_ptr<SmWebsocketSession>> v;
	{
		std::lock_guard<std::mutex> lock(mutex_);
		v.reserve(sessions_.size());
		for (auto p : sessions_)
			v.emplace_back(p->weak_from_this());
	}

	// For each session in our local list, try to acquire a strong
   // pointer. If successful, then send the message on that session.
	for (auto const& wp : v)
		if (auto sp = wp.lock()) {		
			sp->send(ss);
		}

}

void SmSessionManager::AddUser(std::string id, std::string pwd, SmWebsocketSession* sess)
{
	SmUserManager* userMgr = SmUserManager::GetInstance();
	SmUser* user = userMgr->AddUser(id, pwd, sess);
	SmRealtimeSymbolServiceManager* rtlSymSvcMgr = SmRealtimeSymbolServiceManager::GetInstance();
	SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
	SmSymbol* sym = symMgr->FindSymbol("CLN19");
	rtlSymSvcMgr->Symbol(sym);
	rtlSymSvcMgr->Register(user);
}

void SmSessionManager::DeleteUser(std::string id)
{
	SmRealtimeSymbolServiceManager* rtlSymSvcMgr = SmRealtimeSymbolServiceManager::GetInstance();
	rtlSymSvcMgr->Unregister(id);
}
