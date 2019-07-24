#pragma once
#include <string>
#include <map>
struct SmPosition;
struct SmOrder;
class SmAccount
{
private:
	std::string _AccountNo;
	std::string _AccountName;
	std::string _OwnerName;
	std::map<std::string, SmPosition*> _PositionMap;
	// 총자산 - 기초자산은 1억원
	double _TotalBalance = 100000000;
public:
	std::string AccountNo() const { return _AccountNo; }
	void AccountNo(std::string val) { _AccountNo = val; }
	std::string AccountName() const { return _AccountName; }
	void AccountName(std::string val) { _AccountName = val; }
	std::string OwnerName() const { return _OwnerName; }
	void OwnerName(std::string val) { _OwnerName = val; }
	SmPosition* FindPosition(std::string symCode);
	SmPosition* CreatePosition(SmOrder* order);
};

