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

	// 계좌에서 현재 포지션을 가져온다.
	SmPosition* posi = acnt->FindPosition(order->SymbolCode);

	double curClose = sym->Quote.Close / pow(10, sym->Decimal());
	// 주문 포지션에 따른 부호 결정
	int buho = order->Position == SmPositionType::Buy ? 1 : -1;
	if (!posi) { // 포지션이 없는 경우
		posi = acnt->CreatePosition(order);
		posi->OpenQty = buho * order->FilledQty;
		posi->OpenPL = posi->OpenQty * (curClose - posi->AvgPrice) * sym->Seungsu();
		// 잔고 수량에 넣어 준다. 매도는 음수, 매수는 양수, 포지션 없으면 0
		order->RemainQty = buho * order->FilledQty;
		return;
	}
	else { // 포지션이 있는 경우
		double filledPrice = order->FilledPrice / pow(10, sym->Decimal());
		if (order->Position == SmPositionType::Sell) { //체결수량이 매도	
			if (posi->OpenQty > 0) { // 보유수량이 매수			
				if (posi->OpenQty >= order->FilledQty) { //보유수량이 크거나 같은 경우
					posi->OpenQty = posi->OpenQty - order->FilledQty;
					posi->TradePL += double(-order->FilledQty * (posi->AvgPrice - filledPrice) * sym->Seungsu());
					// 들어온 주문은 상쇄 된다.
					order->OrderState = SmOrderState::Settled;
					order->RemainQty = 0;
					if (posi->OpenQty == 0) { // 매수 포지션 청산
						posi->AvgPrice = 0;
						posi->OpenPL = 0;
					}
				}
				else { //체결수량이 큰 경우
					posi->TradePL += double(posi->OpenQty * (filledPrice - posi->AvgPrice) * sym->Seungsu());
					posi->AvgPrice = filledPrice;
					posi->OpenQty = posi->OpenQty - order->FilledQty;
					posi->OpenPL = posi->OpenQty * (curClose - posi->AvgPrice) * sym->Seungsu();
					// 보유수량과 상쇄되고 남은 갯수가 잔고가 된다. - 여기서는 잔고가 매도 포지션이 된다.
					order->RemainQty = posi->OpenQty - order->FilledQty;
				}
			}
			else { // 보유수량이 매도 ( 보유수량이매도/체결수량이매도 인 경우)
				posi->AvgPrice = double((posi->OpenQty * posi->AvgPrice - order->FilledQty * filledPrice) / (posi->OpenQty - order->FilledQty));
				posi->OpenQty = posi->OpenQty - order->FilledQty;
				posi->OpenPL = posi->OpenQty * (curClose - posi->AvgPrice) * sym->Seungsu();
				// 이경우 포지션이 같으므로 더해 주지 않는다.
				// 잔고 수량에 넣어 준다. 매도는 음수, 매수는 양수, 포지션 없으면 0
				order->RemainQty = buho * order->FilledQty;
			}
		}
		else { //체결수량이 매수
			if (posi->OpenQty >= 0) { // 보유수량이 매수/체결수량이매수 인 경우
				posi->AvgPrice = double((posi->OpenQty * posi->AvgPrice + order->FilledQty * filledPrice) / (posi->OpenQty + order->FilledQty));
				posi->OpenQty = posi->OpenQty + order->FilledQty;
				posi->OpenPL = posi->OpenQty * (curClose - posi->AvgPrice) * sym->Seungsu();
				// 이경우 포지션이 같으므로 더해 주지 않는다.
				// 잔고 수량에 넣어 준다. 매도는 음수, 매수는 양수, 포지션 없으면 0
				order->RemainQty = buho * order->FilledQty;
			}
			else { //보유수량이 매도
				if (abs(posi->OpenQty) >= order->FilledQty) { //보유수량이 큰경우
					posi->OpenQty = posi->OpenQty + order->FilledQty;
					posi->TradePL += double(order->FilledQty * (posi->AvgPrice - filledPrice) * sym->Seungsu());
					order->OrderState = SmOrderState::Settled;
					// 들어온 주문은 상쇄되고 포지션을 상실한다.
					order->RemainQty = 0;
					if (posi->OpenQty == 0) { // 매도 포지션 청산
						posi->AvgPrice = 0;
						posi->OpenPL = 0;
					}
				}
				else { //체결수량이 큰 경우				
					posi->TradePL += double(posi->OpenQty * (filledPrice - posi->AvgPrice) * sym->Seungsu());
					posi->AvgPrice = filledPrice;
					posi->OpenQty = posi->OpenQty + order->FilledQty;
					posi->OpenPL = posi->OpenQty * (curClose - posi->AvgPrice) * sym->Seungsu();
					// 이경우 기존의 매도 포지션 잔고 갯수가 상쇄되고 남은 주문의 잔고는 매수 포지션이 된다.
					order->RemainQty = posi->OpenQty + order->FilledQty;
				}
			}
		}
	}

	// 잔고 수량에 따라 포지션을 다시 결정해 준다.
	if (posi->OpenQty > 0) { // 매수 포지션 설정 : 양수임
		posi->Position = SmPositionType::Buy;
	}
	else if (posi->OpenQty < 0) { // 매도 포지션 설정 : 음수임
		posi->Position = SmPositionType::Sell;
	}
	else { // 모든 포지션 상실 0임
		posi->Position = SmPositionType::None;
	}
}
\