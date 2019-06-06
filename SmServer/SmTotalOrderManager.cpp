#include "pch.h"
#include "SmTotalOrderManager.h"
#include "SmOrderNumberGenerator.h"
#include "SmOrder.h"
#include "SmSymbolManager.h"
#include "SmSymbol.h"
#include "SmAccountOrderManager.h"

SmTotalOrderManager::SmTotalOrderManager()
{

}

SmTotalOrderManager::~SmTotalOrderManager()
{
	for (auto it = _OrderMap.begin(); it != _OrderMap.end(); ++it) {
		delete it->second;
	}

	for (auto it = _OrderManagerMap.begin(); it != _OrderManagerMap.end(); ++it) {
		delete it->second;
	}
}

void SmTotalOrderManager::OnOrder(SmOrderRequest&& req)
{
	int order_no = SmOrderNumberGenerator::GetID();
	SmOrder* order = AddOrder(order_no);
	order->AccountNo = req.AccountNo;
	order->SymbolCode = req.SymbolCode;
	order->UserID = req.UserID;
	order->Position = req.Position;
	order->PriceType = req.PriceType;
	order->FilledCondition = req.FillCondition;
	order->OrderPrice = req.OrderPrice;
	order->OrderAmount = req.OrderAmount;
	order->OriOrderNo = req.OriOrderNo;
	order->RequestID = req.RequestID;
	order->OrderType = req.OrderType;
	order->FundName = req.FundName;
	order->StrategyName = req.StrategyName;
	order->ParentAccountNo = req.ParentAccountNo;
	order->SystemName = req.SystemName;
	// �ֹ����� ���� ����
	order->OrderState = SmOrderState::Ledger;
	SendResponse(order, SmOrderCode::OrderCreated);
	switch (req.OrderType)
	{
	case SmOrderType::New:
		OnOrderNew(order);
		break;
	case SmOrderType::Modify:
		OnOrderModify(order);
		break;
	case SmOrderType::Cancel:
		OnOrderCancel(order);
		break;
	default:
		break;
	}
}

void SmTotalOrderManager::OnOrderNew(SmOrder* order)
{
	if (!order)
		return;
	CheckFilled(order);
}

void SmTotalOrderManager::OnOrderModify(SmOrder* order)
{
	if (!order)
		return;
	CheckFilled(order);
}

void SmTotalOrderManager::OnOrderCancel(SmOrder* order)
{
	if (!order)
		return;
	CheckFilled(order);
}

void SmTotalOrderManager::CheckFilled(SmOrder* order)
{
	if (!order)
		return;
	SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
	SmSymbol* sym = symMgr->FindSymbol(order->SymbolCode);
	if (!sym) {
		SendResponse(order, SmOrderCode::NotFountSymbol);
		return;
	}
	// ���尡 �ֹ� ó��
	if (order->PriceType == SmPriceType::Market) {
		OnOrderAccepted(order);
		SendResponse(order, SmOrderCode::OrderAccepted);
		OnOrderFilled(order);
		SendResponse(order, SmOrderCode::OrderFilled);
	} //������ �ֹ� ó��
	else if (order->PriceType == SmPriceType::Price) {
		if (order->Position == SmPositionType::Buy) {
			if (order->OrderPrice >= sym->Quote.Close) {
				OnOrderFilled(order);
				SendResponse(order, SmOrderCode::OrderFilled);
			}
			else {
				OnOrderAccepted(order);
				SendResponse(order, SmOrderCode::OrderAccepted);
			}
		}
		else if (order->Position == SmPositionType::Sell) {
			if (order->OrderPrice <= sym->Quote.Close) {
				OnOrderFilled(order);
				SendResponse(order, SmOrderCode::OrderFilled);
			}
			else {
				OnOrderAccepted(order);
				SendResponse(order, SmOrderCode::OrderAccepted);
			}
		}
	}
	else {
		SendResponse(order, SmOrderCode::UnknownPriceType);
	}
	SendResponse(order, SmOrderCode::PositionCalculated);
}

void SmTotalOrderManager::SendResponse(SmOrder* order, SmOrderCode code)
{
	if (!order)
		return;
}

void SmTotalOrderManager::OnOrderAccepted(SmOrder* order)
{
	if (!order)
		return;

	SmAccountOrderManager* acntOrderMgr = FindAddOrderManager(order->AccountNo);
	// �ֹ����� ����
	order->OrderState = SmOrderState::Accepted;
	acntOrderMgr->OnOrderAccepted(order);
	SmOrderManager::OnOrderAccepted(order);
}

void SmTotalOrderManager::OnOrderFilled(SmOrder* order)
{
	if (!order)
		return;

	SmAccountOrderManager* acntOrderMgr = FindAddOrderManager(order->AccountNo);
	// �ֹ����� ü��
	order->OrderState = SmOrderState::Filled;
	acntOrderMgr->OnOrderFilled(order);
	SmOrderManager::OnOrderFilled(order);
}

SmAccountOrderManager* SmTotalOrderManager::FindAddOrderManager(std::string acntNo)
{
	SmAccountOrderManager* acntMgr = nullptr;
	auto it = _OrderManagerMap.find(acntNo);
	if (it == _OrderManagerMap.end()) {
		acntMgr = new SmAccountOrderManager();
		_OrderManagerMap[acntNo] = acntMgr;
		return acntMgr;
	}

	return it->second;
}
