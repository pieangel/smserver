#include "pch.h"
#include "SmTimeSeriesCollector.h"
#include "SmMarketManager.h"
#include <chrono>
#include <vector>
#include "SmSymbol.h"
#include "SmHdClient.h"
#include "Json/json.hpp"
#include <ctime>
#include "SmTimeSeriesDBManager.h"
#include "Util/VtStringUtil.h"
#include "SmUserManager.h"
#include "SmTimeSeriesServiceManager.h"
#include "SmSymbolManager.h"
#include "SmMarketManager.h"
#include "SmUtil.h"
using namespace nlohmann;


SmTimeSeriesCollector::SmTimeSeriesCollector()
{
	
}

SmTimeSeriesCollector::~SmTimeSeriesCollector()
{
	
}

void SmTimeSeriesCollector::CollectRecentMonthSymbolChartData()
{
	SmMarketManager* mrktMgr = SmMarketManager::GetInstance();
	std::vector<SmSymbol*> sym_vec = mrktMgr->GetRecentMonthSymbolList();
	if (_Index >= sym_vec.size())
		return;
	SmSymbol* sym = sym_vec[_Index];
	SmChartDataRequest req;
	req.symbolCode = sym->SymbolCode();
	req.chartType = SmChartType::MIN;
	req.cycle = 1;
	req.count = 1500;
	req.next = 0;
	SmHdClient* client = SmHdClient::GetInstance();
	client->GetChartData(req);
	_Index++;
	if (_Index == sym_vec.size()) {
		_Timer.remove(_TimerId);
	}
}

void SmTimeSeriesCollector::OnChartDataItem(SmChartDataItem&& data_item)
{
	SmTimeSeriesDBManager* dbMgr = SmTimeSeriesDBManager::GetInstance();
	dbMgr->OnChartDataItem(std::move(data_item));
}

void SmTimeSeriesCollector::OnCompleteChartData(SmChartDataRequest&& data_req)
{
	SmTimeSeriesServiceManager* tsSvcMgr = SmTimeSeriesServiceManager::GetInstance();
	tsSvcMgr->OnChartDataReceived(std::move(data_req));
}

void SmTimeSeriesCollector::StartCollectData()
{
	int waitTime = 2;
	_TimerId = _Timer.add(std::chrono::seconds(waitTime - 1), [this](CppTime::timer_id) { OnTimer(); }, std::chrono::seconds(3));
}

void SmTimeSeriesCollector::GetChartData(SmChartDataRequest&& data_req)
{
	SmHdClient* client = SmHdClient::GetInstance();
	client->GetChartData(data_req);
}

void SmTimeSeriesCollector::OnTimer()
{
	CollectRecentMonthSymbolChartData();
}

void SmTimeSeriesCollector::OnEveryMinute()
{
	SmTimeSeriesDBManager* dbMgr = SmTimeSeriesDBManager::GetInstance();
	SmMarketManager* mrktMgr = SmMarketManager::GetInstance();
	std::vector<SmSymbol*> symVec = mrktMgr->GetRecentMonthSymbolList();
	std::string curTime = SmUtil::GetUTCDateTimeStringForNowMin();
	std::string prevTime = SmUtil::GetUTCDateTimeStringForPreMin(2);

	for (auto it = symVec.begin(); it != symVec.end(); ++it) {
		SmSymbol* sym = *it;
		std::string  meas = sym->SymbolCode() + "_quote";

		std::string query_string = ""; // "select * from \"chart_data\" where \"symbol_code\" = \'CLN19\' AND \"chart_type\" = \'5\' AND \"cycle\" = \'1\'";
		std::string str_cycle = std::to_string(1);
		std::string str_chart_type = std::to_string(5);
		query_string.append("SELECT FIRST(\"c\"),  LAST(\"c\"), MIN(\"c\"), MAX(\"c\") FROM \"");
		query_string.append(meas);
		query_string.append("\" WHERE time >= \'");
		query_string.append(prevTime);
		query_string.append("\' AND time <= \'");
		query_string.append(curTime);
		query_string.append("\' GROUP BY time(1m) fill(previous)");
		std::string resp = dbMgr->ExecQuery(query_string);
		CString msg;
		msg.Format(_T("resp length = %d"), resp.length());
		TRACE(msg);
	}
}
