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
	SmTimeSeriesDBManager* dbMgr = SmTimeSeriesDBManager::GetInstance();
	std::string query_string;// = "show series on abroad_future";
	query_string.append("select * from \"");
	query_string.append(data_req.GetDataKey());
	query_string.append("\"");
	std::string resp = dbMgr->ExecQuery(query_string);
	CString msg;
	try
	{
		auto json_object = json::parse(resp);
		auto a = json_object["results"][0]["series"][0]["values"];
		for (size_t i = 0; i < a.size(); i++) {
			auto val = a[i];
			std::string time = val[0];
			int h = 0, l = 0, o = 0, c = 0, v = 0;
			h = val[1];
			l = val[4];
			o = val[5];
			c = val[6];
			//v = val[7];

			std::string local_time = VtStringUtil::GetLocalTime(time);

			msg.Format(_T("index = %d, date_time = %s, lc = %s, h = %d, l = %d, o = %d, c = %d\n"), i, time.c_str(), local_time.c_str(), h, l, o, c);
			TRACE(msg);
		}
	}
	catch (const std::exception& e)
	{
		std::string error = e.what();
	}
}

void SmTimeSeriesCollector::GetChartFromDatabase(SmChartDataRequest&& data_req)
{
	SmTimeSeriesDBManager* dbMgr = SmTimeSeriesDBManager::GetInstance();
	std::string query_string;// = "show series on abroad_future";
	query_string.append("select \"c\", \"o\" from \"");
	query_string.append(data_req.GetDataKey());
	query_string.append("\" ");
	query_string.append("where time >= '2019-06-05T07:12:00Z'");
	std::string resp = dbMgr->ExecQuery(query_string);
	CString msg;

	try
	{
		auto json_object = json::parse(resp);
		auto a = json_object["results"][0]["series"][0]["values"];
		for (size_t i = 0; i < a.size(); i++) {
			auto val = a[i];
			std::string time = val[0];
			int h = 0, l = 0, o = 0, c = 0, v = 0;
			h = val[1];
			//l = val[4];
			//o = val[5];
			//c = val[6];
			//v = val[7];

			std::string local_time = VtStringUtil::GetLocalTime(time);

			msg.Format(_T("index = %d, date_time = %s, lc = %s, h = %d, l = %d, o = %d, c = %d\n"), i, time.c_str(), local_time.c_str(), h, l, o, c);
			TRACE(msg);
		}
	}
	catch (const std::exception& e)
	{
		std::string error = e.what();
	}
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
