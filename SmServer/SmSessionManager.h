#pragma once

#include <boost/smart_ptr.hpp>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_set>
#include "Global/TemplateSingleton.h"

// Forward declaration
class SmWebsocketSession;

// Represents the shared server state
class SmSessionManager
{
	std::string const doc_root_;

	// This mutex synchronizes all access to sessions_
	std::mutex mutex_;

	// Keep a list of all the connected clients
	std::unordered_set<SmWebsocketSession*> sessions_;


public:
	~SmSessionManager();

	explicit
		SmSessionManager(std::string doc_root);

	std::string const&
		doc_root() const noexcept
	{
		return doc_root_;
	}

	void join(SmWebsocketSession* session);
	void leave(SmWebsocketSession* session);
	void send(std::string message);

private:
	void AddUser(std::string id, std::string pwd, SmWebsocketSession* sess);
	void DeleteUser(std::string id);
	void CloseAllSocket();
};