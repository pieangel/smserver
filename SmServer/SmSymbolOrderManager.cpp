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
SmSymbolOrderManager::SmSymbolOrderManager()
{

}

SmSymbolOrderManager::~SmSymbolOrderManager()
{

}
void SmSymbolOrderManager::OnOrderAccepted(SmOrder* order)
{
	if (!order)
		return;
	if (order->SymbolCode.compare(_SymbolCode) != 0)
		return;
	SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
	SmSymbol* sym = symMgr->FindSymbol(order->SymbolCode);
	if (!sym)
		return;
	// ������ ������ �־� �ش�.
	order->AcceptedPrice = sym->Quote.Close;
	// �ֹ� ���¸� �����Ѵ�.
	order->OrderState = SmOrderState::Accepted;
	// ���� Ȯ�� ��Ͽ� �־� �ش�.
	SmOrderManager::OnOrderAccepted(order);
}
void SmSymbolOrderManager::OnOrderFilled(SmOrder* order)
{
	if (!order)
		return;

	SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
	SmSymbol* sym = symMgr->FindSymbol(order->SymbolCode);
	if (!sym)
		return;
	// ü�� ��¥�� �־� �ش�.
	std::pair<std::string, std::string> date_time = VtStringUtil::GetCurrentDateTime();
	order->TradeDate = date_time.first + "T" + date_time.second + "Z";
	// ü�� ������ �־� �ش�. ü�� ������ ���� �����̴�.
	order->FilledPrice = sym->Quote.Close;
	// �ű� �ֹ����� ü��
	order->OrderState = SmOrderState::Filled;
	// �ֹ� ������ ü�� ������ �־� �ش�.
	order->FilledQty = order->OrderAmount;
	// �ܰ� ������ �־� �ش�.
	int buho = order->Position == SmPositionType::Buy ? 1 : -1;
	order->RemainQty = order->FilledQty * buho;
	// �������� ����Ѵ�.
	CalcPosition(order);
	SmOrderManager::OnOrderFilled(order);
}

void SmSymbolOrderManager::OnOrder(SmOrder* order)
{
	if (!order)
		return;
	// ���� ���� ����
	order->OrderState = SmOrderState::Ledger;
	SmOrderManager::OnOrder(order);
}

void SmSymbolOrderManager::CalcPosition(SmOrder* order)
{
	if (!order)
		return;
	SmAccountManager* acntMgr = acntMgr->GetInstance();
	SmAccount* acnt = acntMgr->FindAccount(order->AccountNo);
	if (!acnt)
		return;
	SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
	SmSymbol* sym = symMgr->FindSymbol(order->SymbolCode);
	if (!sym)
		return;

	SmTotalPositionManager* posiMgr = SmTotalPositionManager::GetInstance();
	// ���¿��� ���� �������� �����´�.
	SmPosition* posi = posiMgr->FindPosition(order->AccountNo, order->SymbolCode);

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
}
int SmSymbolOrderManager::CalcRemain(SmOrder* newOrder)
{
	// �ܰ� �ֹ��� ���ٸ� �ǵڿ� �߰��ϰ� ������.
	// �ܰ� �ֹ��� ������ �ֹ��� �� �տ� �ִ�.
	if (_RemainOrderMap.size() == 0) {
		_RemainOrderMap.push_back(newOrder);
		return CalcTotalRemain();
	}
	// �ܰ� �ֹ��� ���� ���� ��� ó��
	for (auto it = _RemainOrderMap.begin(); it != _RemainOrderMap.end(); ++it) {
		SmOrder* oldOrder = *it;
		
		// ���� ���� �ֹ��� �� ������ �ֹ��� ������ �ٷ� ������.
		if (oldOrder->Position == newOrder->Position) {
			break;
		}
		// ���� �ֹ��� ���� �ֹ��� ������ ���� ���
		if (std::abs(newOrder->RemainQty) == std::abs(oldOrder->RemainQty)) {
			newOrder->RemainQty = 0;
			oldOrder->RemainQty = 0;
			newOrder->OrderState = SmOrderState::Settled;
			// ���ο� �ֹ��� û�� ��Ų �ֹ��� ��Ͽ� �־� �ش�.
			newOrder->SettledOrders.push_back(oldOrder->OrderNo);
			oldOrder->OrderState = SmOrderState::Settled;
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
