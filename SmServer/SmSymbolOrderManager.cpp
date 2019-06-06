#include "SmSymbolOrderManager.h"
#include "SmOrder.h"
#include "SmOrderManager.h"
#include "SmAccountManager.h"
#include "SmAccount.h"
#include "SmPosition.h"
#include "SmSymbolManager.h"
#include "SmSymbol.h"
#include <math.h>
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
	SmOrderManager::OnOrderAccepted(order);
}
void SmSymbolOrderManager::OnOrderFilled(SmOrder* order)
{
	if (!order)
		return;
	CalcPosition(order);
	SmOrderManager::OnOrderFilled(order);
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

	// ���¿��� ���� �������� �����´�.
	SmPosition* posi = acnt->FindPosition(order->SymbolCode);

	double curClose = sym->Quote.Close / pow(10, sym->Decimal());
	// �ֹ� �����ǿ� ���� ��ȣ ����
	int buho = order->Position == SmPositionType::Buy ? 1 : -1;
	if (!posi) { // �������� ���� ���
		posi = acnt->CreatePosition(order);
		posi->OpenQty = buho * order->FilledQty;
		posi->OpenPL = posi->OpenQty * (curClose - posi->AvgPrice) * sym->Seungsu();
		// �ܰ� ������ �־� �ش�. �ŵ��� ����, �ż��� ���, ������ ������ 0
		order->RemainQty = buho * order->FilledQty;
		return;
	}
	else { // �������� �ִ� ���
		double filledPrice = order->FilledPrice / pow(10, sym->Decimal());
		if (order->Position == SmPositionType::Sell) { //ü������� �ŵ�	
			if (posi->OpenQty > 0) { // ���������� �ż�			
				if (posi->OpenQty >= order->FilledQty) { //���������� ũ�ų� ���� ���
					posi->OpenQty = posi->OpenQty - order->FilledQty;
					posi->TradePL += double(-order->FilledQty * (posi->AvgPrice - filledPrice) * sym->Seungsu());
					// ���� �ֹ��� ��� �ȴ�.
					order->OrderState = SmOrderState::Settled;
					order->RemainQty = 0;
					if (posi->OpenQty == 0) { // �ż� ������ û��
						posi->AvgPrice = 0;
						posi->OpenPL = 0;
					}
				}
				else { //ü������� ū ���
					posi->TradePL += double(posi->OpenQty * (filledPrice - posi->AvgPrice) * sym->Seungsu());
					posi->AvgPrice = filledPrice;
					posi->OpenQty = posi->OpenQty - order->FilledQty;
					posi->OpenPL = posi->OpenQty * (curClose - posi->AvgPrice) * sym->Seungsu();
					// ���������� ���ǰ� ���� ������ �ܰ� �ȴ�. - ���⼭�� �ܰ� �ŵ� �������� �ȴ�.
					order->RemainQty = posi->OpenQty - order->FilledQty;
				}
			}
			else { // ���������� �ŵ� ( ���������̸ŵ�/ü������̸ŵ� �� ���)
				posi->AvgPrice = double((posi->OpenQty * posi->AvgPrice - order->FilledQty * filledPrice) / (posi->OpenQty - order->FilledQty));
				posi->OpenQty = posi->OpenQty - order->FilledQty;
				posi->OpenPL = posi->OpenQty * (curClose - posi->AvgPrice) * sym->Seungsu();
				// �̰�� �������� �����Ƿ� ���� ���� �ʴ´�.
				// �ܰ� ������ �־� �ش�. �ŵ��� ����, �ż��� ���, ������ ������ 0
				order->RemainQty = buho * order->FilledQty;
			}
		}
		else { //ü������� �ż�
			if (posi->OpenQty >= 0) { // ���������� �ż�/ü������̸ż� �� ���
				posi->AvgPrice = double((posi->OpenQty * posi->AvgPrice + order->FilledQty * filledPrice) / (posi->OpenQty + order->FilledQty));
				posi->OpenQty = posi->OpenQty + order->FilledQty;
				posi->OpenPL = posi->OpenQty * (curClose - posi->AvgPrice) * sym->Seungsu();
				// �̰�� �������� �����Ƿ� ���� ���� �ʴ´�.
				// �ܰ� ������ �־� �ش�. �ŵ��� ����, �ż��� ���, ������ ������ 0
				order->RemainQty = buho * order->FilledQty;
			}
			else { //���������� �ŵ�
				if (abs(posi->OpenQty) >= order->FilledQty) { //���������� ū���
					posi->OpenQty = posi->OpenQty + order->FilledQty;
					posi->TradePL += double(order->FilledQty * (posi->AvgPrice - filledPrice) * sym->Seungsu());
					order->OrderState = SmOrderState::Settled;
					// ���� �ֹ��� ���ǰ� �������� ����Ѵ�.
					order->RemainQty = 0;
					if (posi->OpenQty == 0) { // �ŵ� ������ û��
						posi->AvgPrice = 0;
						posi->OpenPL = 0;
					}
				}
				else { //ü������� ū ���				
					posi->TradePL += double(posi->OpenQty * (filledPrice - posi->AvgPrice) * sym->Seungsu());
					posi->AvgPrice = filledPrice;
					posi->OpenQty = posi->OpenQty + order->FilledQty;
					posi->OpenPL = posi->OpenQty * (curClose - posi->AvgPrice) * sym->Seungsu();
					// �̰�� ������ �ŵ� ������ �ܰ� ������ ���ǰ� ���� �ֹ��� �ܰ�� �ż� �������� �ȴ�.
					order->RemainQty = posi->OpenQty + order->FilledQty;
				}
			}
		}
	}

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
\