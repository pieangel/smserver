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
	msg.Format(_T("resp length = %d"), resp.length());
	TRACE(msg);
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
	msg.Format("resp len = %d", resp.length());
	TRACE(msg);
	try
	{
		auto json_object = json::parse(resp);
		auto a = json_object["results"][0]["series"][0]["values"];
		int split_size = 20;
		int cur_count = 0;
		int start_index = 0;
		int end_index = 0;
		SmUserManager* userMgr = SmUserManager::GetInstance();
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
			cur_count++;
			if (cur_count % split_size == 0) {
				end_index = i;
				msg.Format(_T("total_count = %d, cur_count = %d, index = %d, si = %d, ei = %d, date_time = %s, lc = %s, h = %d, l = %d, o = %d, c = %d\n"), a.size(), cur_count, i, start_index, end_index,  time.c_str(), local_time.c_str(), h, l, o, c);
				TRACE(msg);
				json send_object;
				send_object["result"] = 0;
				send_object["symbol_code"] = data_req.symbolCode;
				send_object["chart_type"] = (int)data_req.chartType;
				send_object["cycle"] = data_req.cycle;
				send_object["total_count"] = a.size();
				send_object["cur_count"] = cur_count;
				send_object["start_index"] = start_index;
				send_object["end_index"] = end_index;
				for (int di = start_index, ci = 0; di <= end_index; ++di, ++ci) {
					auto data = a[di];
					send_object["data"][ci] = {
						{ "date_time",  VtStringUtil::GetLocalTime(data[0]) },
						{ "high", data[1] },
						{ "low",  data[4] },
						{ "open",  data[5] },
						{ "close",  data[6] },
						{ "volume",  data[7] }
					};
				}

				std::string content = send_object.dump(4);

				userMgr->SendResultMessage(data_req.user_id, content);

				start_index = i + 1;
				cur_count = 0;
			}
			else {
				if (i == a.size()) {
					msg.Format(_T("total_count = %d, cur_count = %d, index = %d, si = %d, ei = %d,  date_time = %s, lc = %s, h = %d, l = %d, o = %d, c = %d\n"), a.size(), cur_count, i, start_index, end_index, time.c_str(), local_time.c_str(), h, l, o, c);
					TRACE(msg);

					json send_object;
					send_object["result"] = 0;
					send_object["symbol_code"] = data_req.symbolCode;
					send_object["chart_type"] = (int)data_req.chartType;
					send_object["cycle"] = data_req.cycle;
					send_object["total_count"] = a.size();
					send_object["cur_count"] = cur_count;
					send_object["start_index"] = start_index;
					send_object["end_index"] = end_index;
					for (int di = start_index, ci = 0; di <= end_index; ++di, ++ci) {
						auto data = a[di];
						send_object["data"][ci] = {
							{ "date_time",  VtStringUtil::GetLocalTime(data[0]) },
							{ "high", data[1] },
							{ "low",  data[4] },
							{ "open",  data[5] },
							{ "close",  data[6] },
							{ "volume",  data[7] }
						};
					}

					userMgr->SendResultMessage(data_req.user_id, send_object.dump(4));
				}
			}
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
