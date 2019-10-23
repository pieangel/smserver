#include "pch.h"
#include "SmAccount.h"
#include "SmSymbolManager.h"
#include "SmSymbol.h"
#include "SmOrder.h"
#include "SmPosition.h"
#include "SmServiceDefine.h"
#include "SmFee.h"
#include "SmTotalPositionManager.h"
#include "Json/json.hpp"
using namespace nlohmann;

void SmAccount::Reset()
{
	_InitialBalance = 100000000;
	// �Ÿŷ� �̷���� ���� - ���� �Ÿŷ� ���� ����
	_TradePL = 0.0;
	// ���� û����� ���� �ֹ����� ���� �� ����
	_OpenPL = 0.0;
	// ������ �Ÿ� ���� - �ʱ� �ں��ݿ��� ���۵� ���� ��� ������ ���� ����
	_TotalTradePL = 0.0;
	// ���� ���� �����Ḧ ��Ÿ����. ���� ������� �ٸ��� Ȯ���� �� �ִ�.
	_Fee = 0.0;
}

double SmAccount::GetSymbolFee(std::string symbol_code)
{
	double sum = 0.0;
	auto it = SymbolFeeMap.find(symbol_code);
	if (it != SymbolFeeMap.end()) {
		SmFeeVector& fee_vec = it->second;
		for (auto its = fee_vec.begin(); its != fee_vec.end(); ++its) {
			sum += (*its)->Fee;
		}
	}

	return sum;
}

double SmAccount::GetTotalFee()
{
	double sum = 0.0;
	// �ɺ��� ������ ��ȸ
	for(auto it = SymbolFeeMap.begin(); it != SymbolFeeMap.end(); ++it) {
		SmFeeVector& fee_vec = it->second;
		// �ɺ� ������ ��� ��ȸ
		for (auto its = fee_vec.begin(); its != fee_vec.end(); ++its) {
			sum += (*its)->Fee;
		}
	}

	return sum;
}

void SmAccount::AddFee(std::string symbol_code, std::shared_ptr<SmFee> fee)
{
	// ��ü �����Ḧ ������Ʈ ���ش�.
	_Fee += fee->Fee;
	auto it = SymbolFeeMap.find(symbol_code);
	if (it != SymbolFeeMap.end()) {
		SmFeeVector& fee_vec = it->second;
		fee_vec.push_back(fee);
	}
	else {
		SmFeeVector fee_vec;
		fee_vec.push_back(fee);
		SymbolFeeMap.insert(std::make_pair(symbol_code, fee_vec));
	}
}

std::string SmAccount::GetAccountInfoByJSon()
{
	json account;
	account["res_id"] = SmProtocol::res_account_list;
	account["account_no"] = _AccountNo;
	account["account_name"] = _AccountName;
	account["user_id"] = _UserID;
	account["password"] = _Password;
	account["initial_balance"] = _InitialBalance;
	account["trade_profit_loss"] = _TradePL;
	account["open_profit_loss"] = _OpenPL;
	account["total_trade_profit_loss"] = _TotalTradePL;
	
	return account.dump();
}