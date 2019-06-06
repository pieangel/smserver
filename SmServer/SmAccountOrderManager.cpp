#include "pch.h"
#include "SmAccountOrderManager.h"
#include "SmSymbolOrderManager.h"
#include "SmOrder.h"
SmAccountOrderManager::SmAccountOrderManager()
{

}

SmAccountOrderManager::~SmAccountOrderManager()
{
	for (auto it = _OrderManagerMap.begin(); it != _OrderManagerMap.end(); ++it) {
		delete it->second;
	}
}

void SmAccountOrderManager::OnOrderAccepted(SmOrder* order)
{
	if (!order)
		return;

	SmSymbolOrderManager* orderMgr = FindAddOrderManager(order->SymbolCode);
	orderMgr->OnOrderAccepted(order);
	SmOrderManager::OnOrderAccepted(order);
}

void SmAccountOrderManager::OnOrderFilled(SmOrder* order)
{
	if (!order)
		return;
	SmSymbolOrderManager* orderMgr = FindAddOrderManager(order->SymbolCode);
	orderMgr->OnOrderFilled(order);
	SmOrderManager::OnOrderFilled(order);
}

SmSymbolOrderManager* SmAccountOrderManager::FindAddOrderManager(std::string symCode)
{
	SmSymbolOrderManager* acntMgr = nullptr;
	auto it = _OrderManagerMap.find(symCode);
	if (it == _OrderManagerMap.end()) {
		acntMgr = new SmSymbolOrderManager();
		_OrderManagerMap[symCode] = acntMgr;
		return acntMgr;
	}

	return it->second;
}
