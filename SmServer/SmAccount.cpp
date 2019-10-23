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
	// 매매로 이루어진 수익 - 당일 매매로 인한 수익
	_TradePL = 0.0;
	// 현재 청산되지 않은 주문으로 인한 평가 손익
	_OpenPL = 0.0;
	// 누적된 매매 수익 - 초기 자본금에서 시작된 이후 모든 수익을 더한 값임
	_TotalTradePL = 0.0;
	// 당일 통합 수수료를 나타낸다. 누적 수수료는 다르게 확인할 수 있다.
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
	// 심볼별 수수료 순회
	for(auto it = SymbolFeeMap.begin(); it != SymbolFeeMap.end(); ++it) {
		SmFeeVector& fee_vec = it->second;
		// 심볼 수수료 목록 순회
		for (auto its = fee_vec.begin(); its != fee_vec.end(); ++its) {
			sum += (*its)->Fee;
		}
	}

	return sum;
}

void SmAccount::AddFee(std::string symbol_code, std::shared_ptr<SmFee> fee)
{
	// 전체 수수료를 업데이트 해준다.
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