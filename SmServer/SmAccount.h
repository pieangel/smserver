#pragma once
#include <string>
#include <map>
#include <vector>
#include "SmFee.h"
struct SmPosition;
struct SmOrder;
struct SmFee;
class SmAccount
{
private:
	std::string _AccountNo;
	std::string _AccountName;
	std::string _Password;

	// 사용자 아이디 - 반드시 이메일로 한다.
	std::string _UserID;
	// 기초자산은 1억원
	double _InitialBalance = 100000000;
	// 매매로 이루어진 수익 - 당일 매매로 인한 수익
	double _TradePL = 0.0;
	// 현재 청산되지 않은 주문으로 인한 평가 손익
	double _OpenPL = 0.0;
	// 누적된 매매 수익 - 초기 자본금에서 시작된 이후 모든 수익을 더한 값임
	double _TotalTradePL = 0.0;
	// 당일 통합 수수료를 나타낸다. 누적 수수료는 다르게 확인할 수 있다.
	double _Fee = 0.0;
	typedef std::vector<std::shared_ptr<SmFee>> SmFeeVector;
	// key : symbol code, value : SmFeeVector
	std::map<std::string, SmFeeVector> SymbolFeeMap;
public:
	void UpdateFee(double fee) {
		_Fee += fee;
	}
	// 매매 손익을 업데이트 한다.
	void UpdateTradePL(double trade_pl) {
		// 오늘 날짜 누적 매매 수익
		_TradePL += trade_pl;
		// 날짜에 상관없는 누적 매매 수익
		_TotalTradePL += trade_pl;
		// 매매 수익이 더해진 계좌 예탁금
		_InitialBalance += trade_pl;
	}
	// 계좌를 완전히 초기화 시킨다. 모의 매매에 할당된 금액을 다시 설정한다.
	void Reset();
	double GetSymbolFee(std::string symbol_code);
	double GetTotalFee();
	void AddFee(std::string symbol_code, std::shared_ptr<SmFee> fee);
	std::string GetAccountInfoByJSon();
	std::string AccountNo() const { return _AccountNo; }
	void AccountNo(std::string val) { _AccountNo = val; }
	std::string AccountName() const { return _AccountName; }
	void AccountName(std::string val) { _AccountName = val; }
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
	double TotalTradePL() const { return _TotalTradePL; }
	void TotalTradePL(double val) { _TotalTradePL = val; }
	double Fee() const { return _Fee; }
	void Fee(double val) { _Fee = val; }
};

