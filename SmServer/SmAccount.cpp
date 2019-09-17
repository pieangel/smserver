#include "pch.h"
#include "SmAccount.h"
#include "SmSymbolManager.h"
#include "SmSymbol.h"
#include "SmOrder.h"
#include "SmPosition.h"
#include "SmServiceDefine.h"

#include "Json/json.hpp"
using namespace nlohmann;


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
	
	return account.dump();
}

SmPosition* SmAccount::FindPosition(std::string symCode)
{
	auto it = _PositionMap.find(symCode);
	if (it != _PositionMap.end()) {
		return it->second;
	}

	return nullptr;
}

SmPosition* SmAccount::CreatePosition(SmOrder* order)
{
	if (!order)
		return nullptr;
	SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
	std::shared_ptr<SmSymbol> sym = symMgr->FindSymbol(order->SymbolCode);
	if (!sym)
		return nullptr;

	SmPosition* position = new SmPosition();
	position->AccountNo = order->AccountNo;
	position->SymbolCode = order->SymbolCode;
	position->Position = order->Position;
	position->OpenQty = order->FilledQty;
	position->AvgPrice = order->FilledPrice / pow(10, sym->Decimal());
	position->CurPrice = order->FilledPrice / pow(10, sym->Decimal());
	position->FundName = order->FundName;
	_PositionMap[order->SymbolCode] = position;

	return position;
}
