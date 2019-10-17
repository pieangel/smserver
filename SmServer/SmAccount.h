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

	// ����� ���̵� - �ݵ�� �̸��Ϸ� �Ѵ�.
	std::string _UserID;
	// �����ڻ��� 1���
	double _InitialBalance = 100000000;
	// �Ÿŷ� �̷���� ���� - ���� �Ÿŷ� ���� ����
	double _TradePL = 0.0;
	// ���� û����� ���� �ֹ����� ���� �� ����
	double _OpenPL = 0.0;
	// ������ �Ÿ� ���� - �ʱ� �ں��ݿ��� ���۵� ���� ��� ������ ���� ����
	double _TotalTradePL = 0.0;
	// ���� ���� �����Ḧ ��Ÿ����. ���� ������� �ٸ��� Ȯ���� �� �ִ�.
	double _Fee = 0.0;
	typedef std::vector<std::shared_ptr<SmFee>> SmFeeVector;
	// key : symbol code, value : SmFeeVector
	std::map<std::string, SmFeeVector> SymbolFeeMap;
public:
	void UpdateFee(double fee) {
		_Fee += fee;
	}
	// �Ÿ� ������ ������Ʈ �Ѵ�.
	void UpdateTradePL(double trade_pl) {
		// ���� ��¥ ���� �Ÿ� ����
		_TradePL += trade_pl;
		// ��¥�� ������� ���� �Ÿ� ����
		_TotalTradePL += trade_pl;
		// �Ÿ� ������ ������ ���� ��Ź��
		_InitialBalance += trade_pl;
	}
	// ���¸� ������ �ʱ�ȭ ��Ų��. ���� �Ÿſ� �Ҵ�� �ݾ��� �ٽ� �����Ѵ�.
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

