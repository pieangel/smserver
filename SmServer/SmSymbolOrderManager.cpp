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
	// 접수된 가격을 넣어 준다.
	order->AcceptedPrice = sym->Quote.Close;
	// 주문 상태를 변경한다.
	order->OrderState = SmOrderState::Accepted;
	SmMongoDBManager::GetInstance()->OnAcceptedOrder(order);
	// 접수 확인 목록에 넣어 준다.
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
	// 체결 날짜를 넣어 준다.
	std::pair<std::string, std::string> date_time = VtStringUtil::GetCurrentDateTime();
	order->FilledDate = date_time.first;
	order->FilledTime = date_time.second;
	// 체결 가격을 넣어 준다. 체결 가격은 현재 가격이다.
	order->FilledPrice = sym->Quote.Close;
	// 신규 주문상태 체결
	order->OrderState = SmOrderState::Filled;
	// 주문 갯수를 체결 갯수에 넣어 준다.
	order->FilledQty = order->OrderAmount;
	// 잔고 갯수를 넣어 준다.
	int buho = order->Position == SmPositionType::Buy ? 1 : -1;
	order->RemainQty = order->FilledQty * buho;
	// 여기서 주문 상태를 데이터베이스에 저장해 준다.
	SmMongoDBManager::GetInstance()->OnFilledOrder(order);
	// 포지션을 계산한다.
	CalcPosition(order);
	SmOrderManager::OnOrderFilled(order);
}

void SmSymbolOrderManager::OnOrder(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;
	// 원장 접수 상태
	order->OrderState = SmOrderState::Ledger;
	std::pair<std::string, std::string> date_time = VtStringUtil::GetCurrentDateTime();
	// 주문 날짜를 넣어 준다.
	order->OrderDate = date_time.first;
	order->OrderTime = date_time.second;
	// 여기서 데이터 베이스에 주문을 저장해 준다.
	SmMongoDBManager::GetInstance()->AddOrder(order);
	// 주문 목록에 넣어 준다.
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
	// 계좌에서 현재 포지션을 가져온다.
	std::shared_ptr<SmPosition> posi = posiMgr->FindPosition(order->AccountNo, order->SymbolCode);

	double curClose = sym->Quote.Close / pow(10, sym->Decimal());
	// 주문 포지션에 따른 부호 결정
	int buho = order->Position == SmPositionType::Buy ? 1 : -1;
	if (!posi) { // 포지션이 없는 경우
		posi = posiMgr->CreatePosition(order);
		posi->OpenQty = buho * order->FilledQty;
		posi->OpenPL = posi->OpenQty * (curClose - posi->AvgPrice) * sym->Seungsu();
	}
	else { // 포지션이 있는 경우
		double filledPrice = order->FilledPrice / pow(10, sym->Decimal());
		if (order->Position == SmPositionType::Sell) { //체결수량이 매도	
			if (posi->OpenQty > 0) { // 보유수량이 매수			
				if (posi->OpenQty >= order->FilledQty) { //보유수량이 크거나 같은 경우
					posi->OpenQty = posi->OpenQty - order->FilledQty;
					posi->TradePL += double(-order->FilledQty * (posi->AvgPrice - filledPrice) * sym->Seungsu());
					posi->OpenPL = posi->OpenQty * (curClose - posi->AvgPrice) * sym->Seungsu();
				}
				else { //체결수량이 큰 경우
					posi->TradePL += double(posi->OpenQty * (filledPrice - posi->AvgPrice) * sym->Seungsu());
					posi->AvgPrice = filledPrice;
					posi->OpenQty = posi->OpenQty - order->FilledQty;
					posi->OpenPL = posi->OpenQty * (curClose - posi->AvgPrice) * sym->Seungsu();
				}
			}
			else { // 보유수량이 매도 ( 보유수량이매도/체결수량이매도 인 경우)
				posi->AvgPrice = double((posi->OpenQty * posi->AvgPrice - order->FilledQty * filledPrice) / (posi->OpenQty - order->FilledQty));
				posi->OpenQty = posi->OpenQty - order->FilledQty;
				posi->OpenPL = posi->OpenQty * (curClose - posi->AvgPrice) * sym->Seungsu();
			}
		}
		else { //체결수량이 매수
			if (posi->OpenQty >= 0) { // 보유수량이 매수/체결수량이매수 인 경우
				posi->AvgPrice = double((posi->OpenQty * posi->AvgPrice + order->FilledQty * filledPrice) / (posi->OpenQty + order->FilledQty));
				posi->OpenQty = posi->OpenQty + order->FilledQty;
				posi->OpenPL = posi->OpenQty * (curClose - posi->AvgPrice) * sym->Seungsu();
			}
			else { //보유수량이 매도
				if (abs(posi->OpenQty) >= order->FilledQty) { //보유수량이 큰경우
					posi->OpenQty = posi->OpenQty + order->FilledQty;
					posi->TradePL += double(order->FilledQty * (posi->AvgPrice - filledPrice) * sym->Seungsu());
					posi->OpenPL = posi->OpenQty * (curClose - posi->AvgPrice) * sym->Seungsu();
				}
				else { //체결수량이 큰 경우				
					posi->TradePL += double(posi->OpenQty * (filledPrice - posi->AvgPrice) * sym->Seungsu());
					posi->AvgPrice = filledPrice;
					posi->OpenQty = posi->OpenQty + order->FilledQty;
					posi->OpenPL = posi->OpenQty * (curClose - posi->AvgPrice) * sym->Seungsu();
				}
			}
		}
	}
	// 포지션 현재가를 넣어준다.
	posi->CurPrice = curClose;
	// 주문 잔고를 계산한다.
	int totalRemain = CalcRemain(order);
	if (totalRemain == posi->OpenQty) {
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
	else { // 주문 잔고와 포지션 잔고가 맞지 않을 경우 심각한 오류이다.
		int error = -1;
		error = error;
	}
	// 여기서 데이터베이스의 포지션을 상태를 업데이트한다.
	SmMongoDBManager::GetInstance()->UpdatePosition(posi);
}
int SmSymbolOrderManager::CalcRemain(std::shared_ptr<SmOrder> newOrder)
{
	// 잔고 주문이 없다면 맨뒤에 추가하고 나간다.
	// 잔고 주문은 오래된 주문이 맨 앞에 있다.
	if (_RemainOrderMap.size() == 0) {
		_RemainOrderMap.push_back(newOrder);
		return CalcTotalRemain();
	}
	// 잔고 주문이 남아 있을 경우 처리
	for (auto it = _RemainOrderMap.begin(); it != _RemainOrderMap.end(); ++it) {
		std::shared_ptr<SmOrder> oldOrder = *it;
		
		// 현재 들어온 주문이 맨 마지막 주문과 같으면 바로 나간다.
		if (oldOrder->Position == newOrder->Position) {
			break;
		}
		// 들어온 주문과 현재 주문과 갯수가 같을 경우
		if (std::abs(newOrder->RemainQty) == std::abs(oldOrder->RemainQty)) {
			newOrder->RemainQty = 0;
			oldOrder->RemainQty = 0;
			newOrder->OrderState = SmOrderState::Settled;
			// 여기서 주문 상태를 데이터베이스에 저장해 준다.
			SmMongoDBManager::GetInstance()->ChangeOrderState(newOrder);
			// 새로운 주문이 청산 시킨 주문은 목록에 넣어 준다.
			newOrder->SettledOrders.push_back(oldOrder->OrderNo);
			oldOrder->OrderState = SmOrderState::Settled;
			// 여기서 주문 상태를 데이터베이스에 저장해 준다.
			SmMongoDBManager::GetInstance()->ChangeOrderState(oldOrder);
			// 잔고에서 지워준다.
			_RemainOrderMap.erase(it);
			break;
		} 
		// 신규 주문잔고가 더 클 경우 - 이 경우 신규 주문의 잔고는 줄어든다.
		// 기존 주문은 잔고가 0이 된다.
		else if (std::abs(newOrder->RemainQty) > std::abs(oldOrder->RemainQty)) {
			newOrder->RemainQty += oldOrder->RemainQty;
			oldOrder->RemainQty = 0;
			// 새로운 주문이 청산 시킨 주문은 목록에 넣어 준다.
			newOrder->SettledOrders.push_back(oldOrder->OrderNo);
			oldOrder->OrderState = SmOrderState::Settled;
			// 여기서 주문 상태를 데이터베이스에 저장해 준다.
			SmMongoDBManager::GetInstance()->ChangeOrderState(oldOrder);
			// 기존 주문을 목록에서 지워준다.
			it = _RemainOrderMap.erase(it);
			// iterator를 하나 후퇴시킨다.
			--it;
		} 
		// 신규 주문잔고가 더 작을 경우 - 이 경우 신규 주문은 잔고가 0이된다.
		// 기존 주문은 신규 주문으로 잔고가 줄어든다.
		else if (std::abs(newOrder->RemainQty) < std::abs(oldOrder->RemainQty)) {
			newOrder->RemainQty = 0;
			oldOrder->RemainQty += newOrder->RemainQty;
			newOrder->OrderState = SmOrderState::Settled;
			// 여기서 주문 상태를 데이터베이스에 저장해 준다.
			SmMongoDBManager::GetInstance()->ChangeOrderState(newOrder);
			break;
		}
	}

	// 신규 주문이 잔고가 남아 있다면 잔고 목록에 추가한다.
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
