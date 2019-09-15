#include "pch.h"
#include "SmTotalPositionManager.h"
#include "SmPosition.h"
#include "SmOrder.h"
#include "SmSymbolManager.h"
#include "SmSymbol.h"
#include "SmAccountPositionManager.h"
#include "Util/VtStringUtil.h"

SmTotalPositionManager::~SmTotalPositionManager()
{
	for (auto it = _AccountPositionManagerMap.begin(); it != _AccountPositionManagerMap.end(); ++it) {
		delete it->second;
	}
}

std::shared_ptr<SmPosition> SmTotalPositionManager::CreatePosition(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return nullptr;
	SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
	SmSymbol* sym = symMgr->FindSymbol(order->SymbolCode);
	if (!sym)
		return nullptr;
	std::pair<std::string, std::string> date_time = VtStringUtil::GetCurrentDateTime();

	std::shared_ptr<SmPosition> position = std::shared_ptr<SmPosition>();
	position->CreatedDate = date_time.first;
	position->CreatedTime = date_time.second;
	position->AccountNo = order->AccountNo;
	position->SymbolCode = order->SymbolCode;
	position->Position = order->Position;
	position->OpenQty = order->FilledQty;
	position->AvgPrice = order->FilledPrice / pow(10, sym->Decimal());
	position->CurPrice = order->FilledPrice / pow(10, sym->Decimal());
	position->FundName = order->FundName;

	AddPosition(position);

	return position;
}

std::shared_ptr<SmPosition> SmTotalPositionManager::FindPosition(std::string acntNo, std::string symbolCode)
{
	auto it = _AccountPositionManagerMap.find(acntNo);
	if (it == _AccountPositionManagerMap.end())
		return nullptr;
	SmAccountPositionManager* acntPosiMgr = it->second;
	return acntPosiMgr->FindPosition(symbolCode);
}

void SmTotalPositionManager::AddPosition(std::shared_ptr<SmPosition> posi)
{
	if (!posi)
		return;
	SmAccountPositionManager* acntPosiMgr = FindAddAccountPositionManager(posi->AccountNo);
	acntPosiMgr->AddPosition(posi);
}

SmAccountPositionManager* SmTotalPositionManager::FindAddAccountPositionManager(std::string accountNo)
{
	auto it = _AccountPositionManagerMap.find(accountNo);
	if (it != _AccountPositionManagerMap.end())
		return it->second;
	SmAccountPositionManager* acntPosiMgr = new SmAccountPositionManager();
	acntPosiMgr->AccountNo(accountNo);
	_AccountPositionManagerMap[accountNo] = acntPosiMgr;
	return acntPosiMgr;
}
