#pragma once
#include <string>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include "Global/TemplateSingleton.h"
class SmSymbol;
class SmUser;
class SmRealtimeSymbolServiceManager : public TemplateSingleton<SmRealtimeSymbolServiceManager>
{
private:
	std::mutex _mutex;
	std::shared_ptr<SmSymbol> _Symbol = nullptr;
	std::map<std::string, SmUser*> _UserMap;
	typedef std::map<SmUser*, SmUser*> SmUserMap;
	std::map<std::shared_ptr<SmSymbol>, SmUserMap> _ServiceMap;
public:
	void RegisterAllRecentSymbol();
	void RegisterSymbol(std::string user_id, std::string symCode);
	void UnregisterSymbol(std::string user_id, std::string symCode);
	void UnregisterAllSymbol(std::string user_id);


	void Register(SmUser* user);
	void Unregister(std::string user_id);
	void SendInfo();
	std::shared_ptr<SmSymbol> Symbol() const { return _Symbol; }
	void Symbol(std::shared_ptr<SmSymbol> val) { _Symbol = val; }
	void BroadcastSise();
	void BroadcastHoga();
private:
	void RegisterSymbol(SmUser* user, std::shared_ptr<SmSymbol> sym);
	void UnregisterSymbol(SmUser* user, std::shared_ptr<SmSymbol> sym);
	void UnregisterAllSymbol(SmUser* user);

	void SendSise(std::shared_ptr<SmSymbol> sym, SmUserMap& userMap);
	void SendHoga(std::shared_ptr<SmSymbol> sym, SmUserMap& userMap);
};

