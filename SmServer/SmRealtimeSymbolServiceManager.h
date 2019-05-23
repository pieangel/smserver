#pragma once
#include <string>
#include <map>
#include <memory>
#include <mutex>
#include <string>
class SmSymbol;
class SmUser;
class SmRealtimeSymbolServiceManager
{
private:
	std::mutex _mutex;
	SmSymbol* _Symbol = nullptr;
	std::map<std::string, SmUser*> _UserMap;
public:
	void Register(SmUser* user);
	void Unregister(std::string user_id);
	void SendInfo();
	SmSymbol* Symbol() const { return _Symbol; }
	void Symbol(SmSymbol* val) { _Symbol = val; }
};

