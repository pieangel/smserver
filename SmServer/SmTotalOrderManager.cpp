#include "pch.h"
#include "SmTotalOrderManager.h"
#include "SmOrderNumberGenerator.h"
#include "SmOrder.h"
#include "SmSymbolManager.h"
#include "SmSymbol.h"
#include "SmAccountOrderManager.h"
#include "SmUserManager.h"
#include "Json/json.hpp"
#include "SmServiceDefine.h"
#include "SmAccountManager.h"
#include "SmAccount.h"
#include "SmTotalPositionManager.h"
#include "SmPosition.h"
using namespace std::chrono;
using namespace nlohmann;

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

void SmTotalOrderManager::OnRequestOrder(SmOrderRequest&& req)
{
	SmAccountManager* acntMgr = SmAccountManager::GetInstance();
	SmAccount* acnt = acntMgr->FindAddAccount(req.AccountNo);
	if (!acnt) {
		SendError(SmOrderError::BadAccount, req);
		return;
	}
	SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
	SmSymbol* sym = symMgr->FindSymbol(req.SymbolCode);
	if (!sym) {
		SendError(SmOrderError::BadSymbol, req);
		return;
	}
	SmOrder* order = CreateOrder();
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
	OnOrder(order);
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

SmOrder* SmTotalOrderManager::CreateOrder()
{
	int order_no = SmOrderNumberGenerator::GetID();
	SmOrder* order = new SmOrder();
	order->OrderNo = order_no;
	return order;
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
	SendConfirmModify(order);
}

void SmTotalOrderManager::OnOrderCancel(SmOrder* order)
{
	if (!order)
		return;
	SendConfirmCancel(order);
}

void SmTotalOrderManager::CheckFilled(SmOrder* order)
{
	if (!order)
		return;
	// 시장가 주문 처리
	if (order->PriceType == SmPriceType::Market) {
		OnOrderAccepted(order);
		OnOrderFilled(order);
	} //지정가 주문 처리
	else if (order->PriceType == SmPriceType::Price) {
		SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
		SmSymbol* sym = symMgr->FindSymbol(order->SymbolCode);
		if (order->Position == SmPositionType::Buy) {
			if (order->OrderPrice >= sym->Quote.Close) {
				OnOrderFilled(order);
			}
			else {
				OnOrderAccepted(order);
			}
		}
		else if (order->Position == SmPositionType::Sell) {
			if (order->OrderPrice <= sym->Quote.Close) {
				OnOrderFilled(order);
			}
			else {
				OnOrderAccepted(order);
			}
		}
	}
	else {
		SendError(SmOrderError::BadPriceType, *order);
	}
}

void SmTotalOrderManager::SendResponse(SmOrder* order)
{
	if (!order)
		return;

	CString msg;
	json send_object;
	if (order->OrderState == SmOrderState::Accepted)
		send_object["res_id"] = SmProtocol::res_order_accepted;
	else if (order->OrderState == SmOrderState::Filled)
		send_object["res_id"] = SmProtocol::res_order_filled;
	send_object["request_id"] = order->RequestID;
	send_object["account_no"] = order->AccountNo;
	send_object["order_type"] = (int)order->OrderType;
	send_object["position_type"] = order->Position;
	send_object["price_type"] = (int)order->PriceType;
	send_object["symbol_code"] = order->SymbolCode;
	send_object["order_price"] = order->OrderPrice;
	send_object["order_no"] = order->OrderNo;
	send_object["order_amount"] = order->OrderAmount;
	send_object["ori_order_no"] = order->OriOrderNo;
	send_object["trade_date"] = order->TradeDate;
	send_object["filled_qty"] = order->FilledQty;
	send_object["filled_price"] = order->FilledPrice;
	send_object["order_state"] = (int)order->OrderState;
	send_object["filled_condition"] = (int)order->FilledCondition;
	send_object["symbol_decimal"] = order->SymbolDecimal;
	send_object["remain_qty"] = order->RemainQty;
	send_object["strategy_name"] = order->StrategyName;
	send_object["system_name"] = order->SystemName;
	send_object["fund_name"] = order->FundName;

	std::string content = send_object.dump();
	SmUserManager* userMgr = SmUserManager::GetInstance();
	userMgr->SendResultMessage(order->UserID, content);
}

void SmTotalOrderManager::SendResponse(SmOrder* order, SmProtocol protocol)
{
	if (!order)
		return;

	CString msg;
	json send_object;
	send_object["res_id"] = protocol;
	send_object["request_id"] = order->RequestID;
	send_object["account_no"] = order->AccountNo;
	send_object["order_type"] = (int)order->OrderType;
	send_object["position_type"] = order->Position;
	send_object["price_type"] = (int)order->PriceType;
	send_object["symbol_code"] = order->SymbolCode;
	send_object["order_price"] = order->OrderPrice;
	send_object["order_no"] = order->OrderNo;
	send_object["order_amount"] = order->OrderAmount;
	send_object["ori_order_no"] = order->OriOrderNo;
	send_object["trade_date"] = order->TradeDate;
	send_object["filled_qty"] = order->FilledQty;
	send_object["filled_price"] = order->FilledPrice;
	send_object["order_state"] = (int)order->OrderState;
	send_object["filled_condition"] = (int)order->FilledCondition;
	send_object["symbol_decimal"] = order->SymbolDecimal;
	send_object["remain_qty"] = order->RemainQty;
	send_object["strategy_name"] = order->StrategyName;
	send_object["system_name"] = order->SystemName;
	send_object["fund_name"] = order->FundName;

	std::string content = send_object.dump();
	SmUserManager* userMgr = SmUserManager::GetInstance();
	userMgr->SendResultMessage(order->UserID, content);
}

void SmTotalOrderManager::SendRemain(SmOrder* order)
{
	if (!order)
		return;
	SmTotalPositionManager* totalPosiMgr = SmTotalPositionManager::GetInstance();
	SmPosition* posi = totalPosiMgr->FindPosition(order->AccountNo, order->SymbolCode);

	CString msg;
	json send_object;
	send_object["res_id"] = SmProtocol::res_symbol_position;
	send_object["symbol_code"] = posi->SymbolCode;
	send_object["fund_name"] = posi->FundName;
	send_object["account_no"] = posi->AccountNo;
	send_object["position_type"] = posi->Position;
	send_object["open_qty"] = posi->OpenQty;
	send_object["fee"] = posi->Fee;
	send_object["trade_pl"] = posi->TradePL;
	send_object["avg_price"] = posi->AvgPrice;
	send_object["cur_price"] = posi->CurPrice;
	send_object["open_pl"] = posi->OpenPL;

	std::string content = send_object.dump();
	SmUserManager* userMgr = SmUserManager::GetInstance();
	userMgr->SendResultMessage(order->UserID, content);
}

void SmTotalOrderManager::SendError(SmOrderError ErrorCode)
{
	json send_object;
	
	send_object["res_id"] = SmProtocol::res_order_error;
	
}

void SmTotalOrderManager::SendError(SmOrderError ErrorCode, SmOrderRequest& req)
{
	json send_object;
	send_object["res_id"] = SmProtocol::res_order_error;
	send_object["error_code"] = (int)ErrorCode;
	send_object["symbol_code"] = req.SymbolCode;
	send_object["fund_name"] = req.FundName;
	send_object["account_no"] = req.AccountNo;
	send_object["request_id"] = req.RequestID;

	std::string content = send_object.dump();
	SmUserManager* userMgr = SmUserManager::GetInstance();
	userMgr->SendResultMessage(req.UserID, content);
}

void SmTotalOrderManager::SendError(SmOrderError ErrorCode, SmOrder& order)
{
	json send_object;
	send_object["res_id"] = SmProtocol::res_order_error;
	send_object["symbol_code"] = order.SymbolCode;
	send_object["fund_name"] = order.FundName;
	send_object["account_no"] = order.AccountNo;
	send_object["request_id"] = order.RequestID;

	std::string content = send_object.dump();
	SmUserManager* userMgr = SmUserManager::GetInstance();
	userMgr->SendResultMessage(order.UserID, content);
}

void SmTotalOrderManager::OnOrderAccepted(SmOrder* order)
{
	if (!order)
		return;

	SmAccountOrderManager* acntOrderMgr = FindAddOrderManager(order->AccountNo);
	// 주문상태 접수
	acntOrderMgr->OnOrderAccepted(order);
	SmOrderManager::OnOrderAccepted(order);
	SendResponse(order, SmProtocol::res_order_accepted);
}

void SmTotalOrderManager::OnOrderFilled(SmOrder* order)
{
	if (!order)
		return;

	SmAccountOrderManager* acntOrderMgr = FindAddOrderManager(order->AccountNo);
	acntOrderMgr->OnOrderFilled(order);
	SmOrderManager::OnOrderFilled(order);
	SendResponse(order, SmProtocol::res_order_filled);
	SendRemain(order);
}

void SmTotalOrderManager::OnOrder(SmOrder* order)
{
	if (!order)
		return;
	SmAccountOrderManager* acntOrderMgr = FindAddOrderManager(order->AccountNo);
	acntOrderMgr->OnOrder(order);
	SmOrderManager::OnOrder(order);
	if (order->OrderType == SmOrderType::New)
		SendResponse(order, SmProtocol::res_order_new);
	else if (order->OrderType == SmOrderType::Modify)
		SendResponse(order, SmProtocol::res_order_modify);
	else if (order->OrderType == SmOrderType::Cancel)
		SendResponse(order, SmProtocol::res_order_cancel);
}

SmAccountOrderManager* SmTotalOrderManager::FindAddOrderManager(std::string acntNo)
{
	SmAccountOrderManager* acntMgr = nullptr;
	auto it = _OrderManagerMap.find(acntNo);
	if (it == _OrderManagerMap.end()) {
		acntMgr = new SmAccountOrderManager();
		acntMgr->AccountNo(acntNo);
		_OrderManagerMap[acntNo] = acntMgr;
		return acntMgr;
	}

	return it->second;
}

void SmTotalOrderManager::SendConfirmModify(SmOrder* order)
{
	if (!order)
		return;
	// 원래 주문을 찾는다.
	SmOrder* oriOrder = FindOrder(order->OriOrderNo);
	// 원래 주문 상태를 변경한다.
	oriOrder->OrderState = SmOrderState::ConfirmModify;
	SendResponse(order);
}

void SmTotalOrderManager::SendConfirmCancel(SmOrder* order)
{
	if (!order)
		return;
	// 원래 주문을 찾는다.
	SmOrder* oriOrder = FindOrder(order->OriOrderNo);
	// 원래 주문 상태를 변경한다.
	oriOrder->OrderState = SmOrderState::ConfirmCancel;
	SendResponse(order);
}
