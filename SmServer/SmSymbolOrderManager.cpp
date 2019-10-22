#include "SmSymbolOrderManager.h"
#include "SmOrder.h"
#include "SmOrderManager.h"
#include "SmAccountManager.h"
#include "SmAccount.h"
#include "SmPosition.h"
#include "SmSymbolManager.h"
#include "SmSymbol.h"
#include <math.h>
#include "SmTotalPositionManager.h"
#include "Util/VtStringUtil.h"
#include "SmMongoDBManager.h"
#include "SmTotalOrderManager.h"
#include "SmServiceDefine.h"
#include "SmFee.h"
#include "SmUserManager.h"
#include "Json/json.hpp"
#include "Log/loguru.hpp"
using namespace nlohmann;

SmSymbolOrderManager::SmSymbolOrderManager()
{

}

SmSymbolOrderManager::~SmSymbolOrderManager()
{

}
void SmSymbolOrderManager::OnOrderAccepted(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;
	if (order->SymbolCode.compare(_SymbolCode) != 0)
		return;
	SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
	std::shared_ptr<SmSymbol> sym = symMgr->FindSymbol(order->SymbolCode);
	if (!sym)
		return;
	// ������ ������ �־� �ش�.
	order->AcceptedPrice = sym->Quote.Close;
	// �ֹ� ���¸� �����Ѵ�.
	order->OrderState = SmOrderState::Accepted;
	SmMongoDBManager::GetInstance()->OnAcceptedOrder(order);
	// ���� Ȯ�� ��Ͽ� �־� �ش�.
	SmOrderManager::OnOrderAccepted(order);
}
void SmSymbolOrderManager::OnOrderFilled(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;

	SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
	std::shared_ptr<SmSymbol> sym = symMgr->FindSymbol(order->SymbolCode);
	if (!sym)
		return;
	// ü�� ��¥�� �־� �ش�.
	std::pair<std::string, std::string> date_time = VtStringUtil::GetCurrentDateTime();
	order->FilledDate = date_time.first;
	order->FilledTime = date_time.second;
	// ü�� ������ �־� �ش�. ü�� ������ ���� �����̴�.
	order->FilledPrice = sym->Quote.Close;
	// �ű� �ֹ����� ü��
	order->OrderState = SmOrderState::Filled;
	// �ֹ� ������ ü�� ������ �־� �ش�.
	order->FilledQty = order->OrderAmount;
	// �ܰ� ������ �־� �ش�.
	int buho = order->Position == SmPositionType::Buy ? 1 : -1;
	order->RemainQty = order->FilledQty * buho;
	// ���⼭ �ֹ� ���¸� �����ͺ��̽��� ������ �ش�.
	SmMongoDBManager::GetInstance()->OnFilledOrder(order);
	// �������� ����Ѵ�.
	CalcPosition(order);
	// ü�� �ֹ� ��Ͽ� �߰��Ѵ�.
	SmOrderManager::OnOrderFilled(order);
}

void SmSymbolOrderManager::OnOrder(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;
	// ���� ���� ����
	order->OrderState = SmOrderState::Ledger;
	std::pair<std::string, std::string> date_time = VtStringUtil::GetCurrentDateTime();
	// �ֹ� ��¥�� �־� �ش�.
	order->OrderDate = date_time.first;
	order->OrderTime = date_time.second;
	// ���⼭ ������ ���̽��� �ֹ��� ������ �ش�.
	SmMongoDBManager::GetInstance()->AddOrder(order);
	// �ֹ� ��Ͽ� �־� �ش�.
	SmOrderManager::OnOrder(order);
}

void SmSymbolOrderManager::CalcFee(std::shared_ptr<SmPosition> posi, std::shared_ptr<SmAccount> acnt, std::shared_ptr<SmOrder> filledOrder)
{
	if (!posi || !filledOrder || !acnt)
		return;
	std::shared_ptr<SmFee> fee = std::make_shared<SmFee>();
	fee->AccountNo = filledOrder->AccountNo;
	fee->Amount = filledOrder->FilledQty;
	std::pair<std::string, std::string> date_time = VtStringUtil::GetCurrentDateTime();
	fee->Date = date_time.first;
	fee->Time = date_time.second;
	fee->Position = filledOrder->Position == SmPositionType::Buy ? 1 : 2;
	fee->SymbolCode = filledOrder->SymbolCode;

	if (std::isdigit(posi->SymbolCode.at(2))) { // ���� ��ǰ
		posi->Fee += filledOrder->FilledQty * SmTotalOrderManager::FeeForDomestic;
		fee->Fee = filledOrder->FilledQty * SmTotalOrderManager::FeeForDomestic;
		LOG_F(INFO, "CalcFee: symbol_code = %s, fee = %.0f", posi->SymbolCode.c_str(), posi->Fee);
	}
	else { // �ؿ� ��ǰ
		posi->Fee += filledOrder->FilledQty * SmTotalOrderManager::FeeForAbroad;
		fee->Fee = filledOrder->FilledQty * SmTotalOrderManager::FeeForAbroad;
		LOG_F(INFO, "CalcFee: symbol_code = %s, fee = %.0f", posi->SymbolCode.c_str(), posi->Fee);
	}

	// ���¿� �����Ḧ �����Ѵ�.
	acnt->AddFee(fee->SymbolCode, fee);
	LOG_F(INFO, "CalcFee2: symbol_code = %s, fee = %.0f", posi->SymbolCode.c_str(), posi->Fee);
	// �����ͺ��̽��� �����Ḧ �����Ѵ�.
	SmMongoDBManager::GetInstance()->SaveFee(fee);
}

void SmSymbolOrderManager::CalcPosition(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;
	SmAccountManager* acntMgr = acntMgr->GetInstance();
	std::shared_ptr<SmAccount> acnt = acntMgr->FindAccount(order->AccountNo);
	if (!acnt)
		return;
	SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
	std::shared_ptr<SmSymbol> sym = symMgr->FindSymbol(order->SymbolCode);
	if (!sym)
		return;
	SmMongoDBManager* dbMgr = SmMongoDBManager::GetInstance();

	SmTotalPositionManager* posiMgr = SmTotalPositionManager::GetInstance();
	// ���¿��� ���� �������� �����´�.
	std::shared_ptr<SmPosition> posi = posiMgr->FindPosition(order->AccountNo, order->SymbolCode);
	// �������� �ֽ� ��¥�� �°� ������Ʈ �Ѵ�.
	posiMgr->CheckUptoDate(posi);
	double curClose = sym->Quote.Close / pow(10, sym->Decimal());
	double current_profit_loss = 0.0;
	// �ֹ� �����ǿ� ���� ��ȣ ����
	int buho = order->Position == SmPositionType::Buy ? 1 : -1;
	if (!posi) { // �������� ���� ���
		posi = posiMgr->CreatePosition(order);
		posi->OpenQty = buho * order->FilledQty;
		posi->OpenPL = posi->OpenQty * (curClose - posi->AvgPrice) * sym->Seungsu();
	}
	else { // �������� �ִ� ���
		double filledPrice = order->FilledPrice / pow(10, sym->Decimal());
		if (order->Position == SmPositionType::Sell) { //ü������� �ŵ�	
			if (posi->OpenQty > 0) { // ���������� �ż�			
				if (posi->OpenQty >= order->FilledQty) { //���������� ũ�ų� ���� ���
					posi->OpenQty = posi->OpenQty - order->FilledQty;
					// �������� �߻� - �����ͺ��̽��� �����Ѵ�.
					current_profit_loss = double(-order->FilledQty * (posi->AvgPrice - filledPrice) * sym->Seungsu());
					// ���⼭ �����ͺ��̽��� ������ ���ش�.
					posi->TradePL += current_profit_loss;
					posi->OpenPL = posi->OpenQty * (curClose - posi->AvgPrice) * sym->Seungsu();
					acnt->UpdateTradePL(current_profit_loss);
					//dbMgr->UpdateAccountInfo(acnt);
					dbMgr->SaveTradePL(acnt, posi, current_profit_loss);
				}
				else { //ü������� ū ���
					// �������� �߻� - �����ͺ��̽��� �����Ѵ�.
					current_profit_loss = double(posi->OpenQty * (filledPrice - posi->AvgPrice) * sym->Seungsu());
					posi->TradePL += current_profit_loss;
					posi->AvgPrice = filledPrice;
					posi->OpenQty = posi->OpenQty - order->FilledQty;
					posi->OpenPL = posi->OpenQty * (curClose - posi->AvgPrice) * sym->Seungsu();
					acnt->UpdateTradePL(current_profit_loss);
					//dbMgr->UpdateAccountInfo(acnt);
					dbMgr->SaveTradePL(acnt, posi, current_profit_loss);
				}
			}
			else { // ���������� �ŵ� ( ���������̸ŵ�/ü������̸ŵ� �� ���)
				posi->AvgPrice = double((posi->OpenQty * posi->AvgPrice - order->FilledQty * filledPrice) / (posi->OpenQty - order->FilledQty));
				posi->OpenQty = posi->OpenQty - order->FilledQty;
				posi->OpenPL = posi->OpenQty * (curClose - posi->AvgPrice) * sym->Seungsu();
			}
		}
		else { //ü������� �ż�
			if (posi->OpenQty >= 0) { // ���������� �ż�/ü������̸ż� �� ���
				posi->AvgPrice = double((posi->OpenQty * posi->AvgPrice + order->FilledQty * filledPrice) / (posi->OpenQty + order->FilledQty));
				posi->OpenQty = posi->OpenQty + order->FilledQty;
				posi->OpenPL = posi->OpenQty * (curClose - posi->AvgPrice) * sym->Seungsu();
			}
			else { //���������� �ŵ�
				if (abs(posi->OpenQty) >= order->FilledQty) { //���������� ū���
					posi->OpenQty = posi->OpenQty + order->FilledQty;
					// �������� �߻� - �����ͺ��̽��� �����Ѵ�.
					current_profit_loss = double(order->FilledQty * (posi->AvgPrice - filledPrice) * sym->Seungsu());
					posi->TradePL += current_profit_loss;
					posi->OpenPL = posi->OpenQty * (curClose - posi->AvgPrice) * sym->Seungsu();
					acnt->UpdateTradePL(current_profit_loss);
					//dbMgr->UpdateAccountInfo(acnt);
					dbMgr->SaveTradePL(acnt, posi, current_profit_loss);
				}
				else { //ü������� ū ���		
					// �������� �߻� - �����ͺ��̽��� �����Ѵ�.
					current_profit_loss = double(posi->OpenQty * (filledPrice - posi->AvgPrice) * sym->Seungsu());
					posi->TradePL += current_profit_loss;
					posi->AvgPrice = filledPrice;
					posi->OpenQty = posi->OpenQty + order->FilledQty;
					posi->OpenPL = posi->OpenQty * (curClose - posi->AvgPrice) * sym->Seungsu();
					acnt->UpdateTradePL(current_profit_loss);
					//dbMgr->UpdateAccountInfo(acnt);
					dbMgr->SaveTradePL(acnt, posi, current_profit_loss);
				}
			}
		}
	}
	// ������ ���簡�� �־��ش�.
	posi->CurPrice = curClose;
	// �ֹ� �ܰ� ����Ѵ�.
	int totalRemain = CalcRemain(order);
	if (totalRemain == posi->OpenQty) {
		// �ܰ� ������ ���� �������� �ٽ� ������ �ش�.
		if (posi->OpenQty > 0) { // �ż� ������ ���� : �����
			posi->Position = SmPositionType::Buy;
		}
		else if (posi->OpenQty < 0) { // �ŵ� ������ ���� : ������
			posi->Position = SmPositionType::Sell;
		}
		else { // ��� ������ ��� 0��
			posi->Position = SmPositionType::None;
		}
	}
	else { // �ֹ� �ܰ�� ������ �ܰ� ���� ���� ��� �ɰ��� �����̴�.
		int error = -1;
		error = error;
		LOG_F(INFO, "�ֹ���� ���� �߻� ���� : %s, ���� : %s", posi->AccountNo.c_str(), posi->SymbolCode.c_str());
	}

	// ü��� �ֹ��� ������. û��� �ֹ��� �̹� ó���Ǿ���.
	if (order->OrderState == SmOrderState::Filled)
		SendResponse(order, SmProtocol::res_order_filled);
	// �����ǿ� �����Ḧ �ΰ��Ѵ�.
	// ������� ���� ü��� �ֹ��� ������ ���� �ΰ��ȴ�.
	CalcFee(posi, acnt, order);
	dbMgr->UpdateAccountInfo(acnt);
	// �ܰ� ������.
	SendRemain(order, acnt);

	// ���⼭ �����ͺ��̽��� �������� ���¸� ������Ʈ�Ѵ�.
	SmMongoDBManager::GetInstance()->UpdatePosition(posi);
}
int SmSymbolOrderManager::CalcRemain(std::shared_ptr<SmOrder> newOrder)
{
	// �ܰ� �ֹ��� ���ٸ� �ǵڿ� �߰��ϰ� ������.
	// �ܰ� �ֹ��� ������ �ֹ��� �� �տ� �ִ�.
	if (_RemainOrderMap.size() == 0) {
		_RemainOrderMap.push_back(newOrder);
		return CalcTotalRemain();
	}
	// �ܰ� �ֹ��� ���� ���� ��� ó��
	for (auto it = _RemainOrderMap.begin(); it != _RemainOrderMap.end(); ++it) {
		std::shared_ptr<SmOrder> oldOrder = *it;
		
		// ���� ���� �ֹ��� �� ������ �ֹ��� ������ �ٷ� ������.
		// ���ο� �ֹ��� ����� ���� �ؿ��� �ٽ� ó���Ѵ�.
		if (oldOrder->Position == newOrder->Position) {
			break;
		}
		// ���� �ֹ��� ���� �ֹ��� ������ ���� ���
		if (std::abs(newOrder->RemainQty) == std::abs(oldOrder->RemainQty)) {
			newOrder->RemainQty = 0;
			oldOrder->RemainQty = 0;
			newOrder->OrderState = SmOrderState::Settled;
			// ���⼭ �ֹ� ���¸� �����ͺ��̽��� ������ �ش�.
			SmMongoDBManager::GetInstance()->ChangeOrderState(newOrder);
			// ���ο� �ֹ��� û�� ��Ų �ֹ��� ��Ͽ� �־� �ش�.
			newOrder->SettledOrders.push_back(oldOrder->OrderNo);
			oldOrder->OrderState = SmOrderState::Settled;
			// û�� �ֹ� �޽����� ������.
			SmTotalOrderManager::GetInstance()->SendResponse(oldOrder, SmProtocol::res_order_settled);
			// û�� �ֹ� �޽����� ������.
			SmTotalOrderManager::GetInstance()->SendResponse(newOrder, SmProtocol::res_order_settled);
			// ���⼭ �ֹ� ���¸� �����ͺ��̽��� ������ �ش�.
			SmMongoDBManager::GetInstance()->ChangeOrderState(oldOrder);
			// �ܰ��� �����ش�.
			_RemainOrderMap.erase(it);
			break;
		} 
		// �ű� �ֹ��ܰ� �� Ŭ ��� - �� ��� �ű� �ֹ��� �ܰ�� �پ���.
		// ���� �ֹ��� �ܰ� 0�� �ȴ�.
		else if (std::abs(newOrder->RemainQty) > std::abs(oldOrder->RemainQty)) {
			newOrder->RemainQty += oldOrder->RemainQty;
			oldOrder->RemainQty = 0;
			// ���ο� �ֹ��� û�� ��Ų �ֹ��� ��Ͽ� �־� �ش�.
			newOrder->SettledOrders.push_back(oldOrder->OrderNo);
			oldOrder->OrderState = SmOrderState::Settled;
			// û�� �ֹ� �޽����� ������.
			SmTotalOrderManager::GetInstance()->SendResponse(oldOrder, SmProtocol::res_order_settled);
			// ���⼭ �ֹ� ���¸� �����ͺ��̽��� ������ �ش�.
			SmMongoDBManager::GetInstance()->ChangeOrderState(oldOrder);
			// ���� �ֹ��� ��Ͽ��� �����ش�.
			it = _RemainOrderMap.erase(it);
			// iterator�� �ϳ� �����Ų��.
			--it;
		} 
		// �ű� �ֹ��ܰ� �� ���� ��� - �� ��� �ű� �ֹ��� �ܰ� 0�̵ȴ�.
		// ���� �ֹ��� �ű� �ֹ����� �ܰ� �پ���.
		else if (std::abs(newOrder->RemainQty) < std::abs(oldOrder->RemainQty)) {
			newOrder->RemainQty = 0;
			oldOrder->RemainQty += newOrder->RemainQty;
			newOrder->OrderState = SmOrderState::Settled;
			// û�� �ֹ� �޽����� ������.
			SmTotalOrderManager::GetInstance()->SendResponse(oldOrder, SmProtocol::res_order_settled);
			// ���⼭ �ֹ� ���¸� �����ͺ��̽��� ������ �ش�.
			SmMongoDBManager::GetInstance()->ChangeOrderState(newOrder);
			break;
		}
	}

	// �ű� �ֹ��� �ܰ� ���� �ִٸ� �ܰ� ��Ͽ� �߰��Ѵ�.
	if (newOrder->RemainQty != 0 && newOrder->OrderState == SmOrderState::Filled) {
		_RemainOrderMap.push_back(newOrder);
	}

	return CalcTotalRemain();
}

int SmSymbolOrderManager::CalcTotalRemain()
{
	int totalRemain = 0;
	for (auto it = _RemainOrderMap.begin(); it != _RemainOrderMap.end(); ++it) {
		totalRemain += (*it)->RemainQty;
	}

	return totalRemain;
}

void SmSymbolOrderManager::SendRemain(std::shared_ptr<SmOrder> order, std::shared_ptr<SmAccount> acnt)
{
	if (!order || !acnt)
		return;
	SmTotalPositionManager* totalPosiMgr = SmTotalPositionManager::GetInstance();
	std::shared_ptr<SmPosition> posi = totalPosiMgr->FindPosition(order->AccountNo, order->SymbolCode);

	json send_object;
	send_object["res_id"] = SmProtocol::res_symbol_position;
	send_object["created_date"] = posi->CreatedDate;
	send_object["created_time"] = posi->CreatedTime;
	send_object["symbol_code"] = posi->SymbolCode;
	send_object["fund_name"] = posi->FundName;
	send_object["account_no"] = posi->AccountNo;
	send_object["position_type"] = posi->Position;
	send_object["open_qty"] = posi->OpenQty;
	send_object["symbol_fee"] = posi->Fee;
	send_object["trade_pl"] = posi->TradePL;
	send_object["avg_price"] = posi->AvgPrice;
	send_object["cur_price"] = posi->CurPrice;
	send_object["fee"] = posi->Fee;
	send_object["open_pl"] = posi->OpenPL;
	send_object["account_fee"] = acnt->GetTotalFee();
	send_object["account_trade_pl"] = acnt->TradePL();
	send_object["account_total_trade_pl"] = acnt->TotalTradePL();

	LOG_F(INFO, "SendRemain: symbol_code = %s, fee = %.0f", posi->SymbolCode.c_str(), posi->Fee);

	std::string content = send_object.dump();
	SmUserManager* userMgr = SmUserManager::GetInstance();
	userMgr->SendResultMessage(order->UserID, content);
}

void SmSymbolOrderManager::SendResponse(std::shared_ptr<SmOrder> order, SmProtocol protocol)
{
	if (!order)
		return;

	json send_object;
	send_object["res_id"] = protocol;
	send_object["request_id"] = order->RequestID;
	send_object["account_no"] = order->AccountNo;
	send_object["order_type"] = (int)order->OrderType;
	send_object["position_type"] = (int)order->Position;
	send_object["price_type"] = (int)order->PriceType;
	send_object["symbol_code"] = order->SymbolCode;
	send_object["order_price"] = order->OrderPrice;
	send_object["order_no"] = order->OrderNo;
	send_object["order_amount"] = order->OrderAmount;
	send_object["ori_order_no"] = order->OriOrderNo;
	send_object["filled_date"] = order->FilledDate;
	send_object["filled_time"] = order->FilledTime;
	send_object["order_date"] = order->OrderDate;
	send_object["order_time"] = order->OrderTime;
	send_object["filled_qty"] = order->FilledQty;
	send_object["filled_price"] = order->FilledPrice;
	send_object["order_state"] = (int)order->OrderState;
	send_object["filled_condition"] = (int)order->FilledCondition;
	send_object["symbol_decimal"] = order->SymbolDecimal;
	send_object["remain_qty"] = order->RemainQty;
	send_object["strategy_name"] = order->StrategyName;
	send_object["system_name"] = order->SystemName;
	send_object["fund_name"] = order->FundName;
	for (size_t i = 0; i < order->SettledOrders.size(); ++i) {
		send_object["settled_orders"][i] = order->SettledOrders[i];
	}

	std::string content = send_object.dump();
	SmUserManager* userMgr = SmUserManager::GetInstance();
	userMgr->SendResultMessage(order->UserID, content);
}
