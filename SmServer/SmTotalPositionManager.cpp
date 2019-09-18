#include "pch.h"
#include "SmTotalPositionManager.h"
#include "SmPosition.h"
#include "SmOrder.h"
#include "SmSymbolManager.h"
#include "SmSymbol.h"
#include "SmAccountPositionManager.h"
#include "Util/VtStringUtil.h"
#include "SmMongoDBManager.h"
#include "SmServiceDefine.h"
#include "SmGlobal.h"
#include "SmSessionManager.h"
#include "SmMongoDBManager.h"

#include "Json/json.hpp"
using namespace nlohmann;
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
	std::shared_ptr<SmSymbol> sym = symMgr->FindSymbol(order->SymbolCode);
	if (!sym)
		return nullptr;
	std::pair<std::string, std::string> date_time = VtStringUtil::GetCurrentDateTime();

	std::shared_ptr<SmPosition> position = std::make_shared<SmPosition>();
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
	// 여기서 데이터 베이스에 포지션을 저장해 준다.
	SmMongoDBManager::GetInstance()->AddPosition(posi);
}

void SmTotalPositionManager::SendPositionList(int session_id, std::string account_no)
{
	std::vector<std::shared_ptr<SmPosition>> position_list = SmMongoDBManager::GetInstance()->GetPositionList(account_no);
	json send_object;
	send_object["res_id"] = SmProtocol::res_position_list;
	send_object["total_position_count"] = (int)position_list.size();
	for (size_t j = 0; j < position_list.size(); ++j) {
		std::shared_ptr<SmPosition> position = position_list[j];
		send_object["position"][j] = {
			{ "created_date",  position->CreatedDate },
			{ "created_time", position->CreatedTime },
			{ "symbol_code",  position->SymbolCode },
			{ "position_type",  (int)position->Position },
			{ "account_no",  position->AccountNo },
			{ "open_qty",  position->OpenQty },
			{ "fee",  position->Fee },
			{ "trade_profitloss",  position->TradePL },
			{ "average_price",  position->AvgPrice },
			{ "current_price",  position->CurPrice },
			{ "open_profitloss",  position->OpenPL }
		};

	}

	std::shared_ptr<SmSessionManager> session_mgr = SmGlobal::GetInstance()->GetSessionManager();
	session_mgr->send(session_id, send_object.dump());
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
