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
	// 사용자 아이디 - 반드시 이메일로 한다.
	std::string _UserID;
	std::map<std::string, SmPosition*> _PositionMap;
	// 기초자산은 1억원
	double _InitialBalance = 100000000;
	// 매매로 이루어진 수익
	double _TradePL = 0.0;
	// 현재 청산되지 않은 주문으로 인한 평가 손익
	double _OpenPL = 0.0;
	std::string _Password;
public:
	std::string GetAccountInfoByJSon();
	std::string AccountNo() const { return _AccountNo; }
	void AccountNo(std::string val) { _AccountNo = val; }
	std::string AccountName() const { return _AccountName; }
	void AccountName(std::string val) { _AccountName = val; }
	SmPosition* FindPosition(std::string symCode);
	SmPosition* CreatePosition(SmOrder* order);
	std::string UserID() const { return _UserID; }
	void UserID(std::string val) { _UserID = val; }
	std::string Password() const { return _Password; }
	void Password(std::string val) { _Password = val; }
	double InitialBalance() const { return _InitialBalance; }
	void InitialBalance(double val) { _InitialBalance = val; }
	double TradePL() const { return _TradePL; }
	void TradePL(double val) { _TradePL = val; }
	double OpenPL() const { return _OpenPL; }
	void OpenPL(double val) { _OpenPL = val; }
};

