#include "pch.h"
#include "SmAccount.h"
#include "SmSymbolManager.h"
#include "SmSymbol.h"
#include "SmOrder.h"
#include "SmPosition.h"
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
	SmSymbol* sym = symMgr->FindSymbol(order->SymbolCode);
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
