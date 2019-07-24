#pragma once
#include <string>
#include <map>
struct SmPosition;
class SmAccountPositionManager
{
public:
	~SmAccountPositionManager();
	SmPosition* FindPosition(std::string symbolCode);
	void AddPosition(SmPosition* posi);
	std::string AccountNo() const { return _AccountNo; }
	void AccountNo(std::string val) { _AccountNo = val; }
private:
	std::string _AccountNo;
	std::map<std::string, SmPosition*> _PositionMap;
};

