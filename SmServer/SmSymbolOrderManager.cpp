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

	SmTotalPositionManager* posiMgr = SmTotalPositionManager::GetInstance();
	// ���¿��� ���� �������� �����´�.
	std::shared_ptr<SmPosition> posi = posiMgr->FindPosition(order->AccountNo, order->SymbolCode);

	double curClose = sym->Quote.Close / pow(10, sym->Decimal());
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
					posi->TradePL += double(-order->FilledQty * (posi->AvgPrice - filledPrice) * sym->Seungsu());
					posi->OpenPL = posi->OpenQty * (curClose - posi->AvgPrice) * sym->Seungsu();
				}
				else { //ü������� ū ���
					posi->TradePL += double(posi->OpenQty * (filledPrice - posi->AvgPrice) * sym->Seungsu());
					posi->AvgPrice = filledPrice;
					posi->OpenQty = posi->OpenQty - order->FilledQty;
					posi->OpenPL = posi->OpenQty * (curClose - posi->AvgPrice) * sym->Seungsu();
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
					posi->TradePL += double(order->FilledQty * (posi->AvgPrice - filledPrice) * sym->Seungsu());
					posi->OpenPL = posi->OpenQty * (curClose - posi->AvgPrice) * sym->Seungsu();
				}
				else { //ü������� ū ���				
					posi->TradePL += double(posi->OpenQty * (filledPrice - posi->AvgPrice) * sym->Seungsu());
					posi->AvgPrice = filledPrice;
					posi->OpenQty = posi->OpenQty + order->FilledQty;
					posi->OpenPL = posi->OpenQty * (curClose - posi->AvgPrice) * sym->Seungsu();
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
	}
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
