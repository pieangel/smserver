#include "pch.h"
#include "SmChartData.h"
#include "SmUserManager.h"
#include "SmTimeSeriesDBManager.h"
#include "Json/json.hpp"
#include "Log/loguru.hpp"
#include "SmHdClient.h"
#include "SmUtil.h"

using namespace nlohmann;
void SmChartData::GetChartDataFromDB()
{
	SmTimeSeriesDBManager* dbMgr = SmTimeSeriesDBManager::GetInstance();
	std::string query_string;// = "show series on abroad_future";
	query_string.append("SELECT \"c\", \"h\", \"l\", \"o\", \"v\" FROM \"");
	query_string.append(GetDataKey());
	query_string.append("\" WHERE ");
	std::string resp = dbMgr->ExecQuery(query_string);
	CString msg;
	msg.Format("resp len = %d", resp.length());
	TRACE(msg);
	try
	{
		auto json_object = json::parse(resp);
		auto it = json_object.find("error");
		if (it != json_object.end()) {
			std::string err_msg = json_object["error"];
			TRACE(err_msg.c_str());
			LOG_F(INFO, "Query Error", err_msg);
			return;
		}
		auto series = json_object["results"][0]["series"];
		if (series.is_null()) {
			
			return;
		}
		auto a = json_object["results"][0]["series"][0]["values"];
		
	}
	catch (const std::exception& e)
	{
		std::string error = e.what();
		LOG_F(INFO, "Query Error", error);
	}
}

void SmChartData::GetChartDataFromServer()
{
	SmChartDataRequest req;
	req.symbolCode = _SymbolCode;
	req.chartType = _ChartType;
	req.cycle = _Cycle;
	req.count = _DataQueueSize;
	req.next = 0;
	SmHdClient* client = SmHdClient::GetInstance();
	client->GetChartData(req);
}

void SmChartData::SendCyclicChartDataToUsers()
{
	CString msg;
	msg.Format("차트데이터 업데이트 됨 %d\n", 0);
	TRACE(msg);
}

int SmChartData::GetCycleByTimeDif()
{
	if (_DataItemList.size() < 2)
		return 0;
	auto it = _DataItemList.begin();
	SmChartDataItem newItem = *it++;
	SmChartDataItem oldItem = *it;

	int i = 0;

	std::string new_datetime = newItem.date + newItem.time;
	std::string old_datetime = oldItem.date + oldItem.time;

	double seconds = SmUtil::GetDifTimeBySeconds(new_datetime, old_datetime);

	return (int)seconds;
}

void SmChartData::OnChartDataUpdated()
{
	SendCyclicChartDataToUsers();
}

void SmChartData::PushChartDataItem(SmChartDataItem data)
{
	CString msg;
	msg.Format(_T("pushed data :: date = %s, o = %d, h = %d, l = %d, c = %d, v = %d\n"), data.date.c_str(), data.o, data.h, data.l, data.c, data.v);
	TRACE(msg);

	_DataItemList.push_back(data);
	if (_DataItemList.size() > _DataQueueSize) {
		_DataItemList.pop_front();
	}

	GetCycleByTimeDif();
}

void SmChartData::RemoveUser(std::string user_id)
{
	auto it = _UserList.find(user_id);
	if (it != _UserList.end()) {
		_UserList.erase(it);
	}
}

void SmChartData::OnTimer()
{
	if (_UserList.size() == 0)
		return;
	GetChartDataFromServer();
}
