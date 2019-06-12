#include "pch.h"
#include "SmTimeSeriesServiceManager.h"
#include "SmTimeSeriesDBManager.h"
#include "Util/VtStringUtil.h"
#include "SmUser.h"
#include "SmUserManager.h"
#include "Json/json.hpp"
#include <vector>
#include "SmHdClient.h"
#include "Log/loguru.hpp"
#include "SmChartData.h"
#include "SmUtil.h"
#include <chrono>
using namespace std::chrono;
using namespace nlohmann;

SmTimeSeriesServiceManager::SmTimeSeriesServiceManager()
{

}

SmTimeSeriesServiceManager::~SmTimeSeriesServiceManager()
{
	for (auto it = _CycleDataReqMap.begin(); it != _CycleDataReqMap.end(); ++it) {
		delete it->second;
	}
}

void SmTimeSeriesServiceManager::OnUnregisterCycleDataRequest(SmChartDataRequest&& data_req)
{
	// 기존 목록에 요청이 있는지 확인한다.
	auto it = _CycleDataReqMap.find(data_req.GetDataKey());
	if (it != _CycleDataReqMap.end()) {
		return;
	}

	SmChartData* chart_data = it->second;
	chart_data->RemoveUser(data_req.user_id);
	size_t user_count = chart_data->GetUserCount();
	if (user_count == 0) {
		// 사용자가 없을 경우에는 타이머를 정지시켜 준다.
		auto tit = _CycleDataReqTimerMap.find(data_req.GetDataKey());
		if (tit != _CycleDataReqTimerMap.end()) {
			auto timer_id = tit->second;
			_Timer.remove(timer_id);
			_CycleDataReqTimerMap.erase(tit);
		}
		// 차트 객체도 삭제해 준다.
		delete chart_data;
		_CycleDataReqMap.erase(it);
	}
}

void SmTimeSeriesServiceManager::OnRegisterCycleDataRequest(SmChartDataRequest&& data_req)
{
	auto it = _CycleDataReqMap.find(data_req.GetDataKey());
	// 이미 차트 데이터에 대한 요청이 있는 경우에는 그 차트데이터에 사용자 아이디만 추가한다.
	if (it != _CycleDataReqMap.end()) {
		it->second->AddUser(data_req.user_id);
		return;
	}
	// 차트 데이터를 등록해 준다.
	SmChartData* chart_data = AddCycleDataReq(data_req);
	// 차트 데이터 타이머 서비스를 등록해 준다.
	RegisterTimer(chart_data);
}

void SmTimeSeriesServiceManager::OnChartDataRequest(SmChartDataRequest&& data_req)
{
	SmTimeSeriesDBManager* dbMgr = SmTimeSeriesDBManager::GetInstance();
	std::string query_string;// = "show series on abroad_future";
	query_string.append("SELECT \"c\", \"h\", \"l\", \"o\", \"v\" FROM \"");
	query_string.append(data_req.GetDataKey());
	query_string.append("\" ");
	query_string.append("LIMIT ");
	query_string.append(std::to_string(data_req.count));
	//query_string.append("where time >= '2019-06-05T07:12:00Z'");
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
			_HistoryDataReqMap[data_req.GetDataKey()] = data_req;
			SmHdClient* client = SmHdClient::GetInstance();
			client->GetChartData(data_req);
			return;
		}
		auto a = json_object["results"][0]["series"][0]["values"];
		int split_size = _SendDataSplitSize;
		int cur_count = 0;
		int start_index = 0;
		int end_index = 0;
		std::vector<SmSimpleChartDataItem> dataVec;
		SmUserManager* userMgr = SmUserManager::GetInstance();
		for (size_t i = 0; i < a.size(); i++) {
			end_index = i;
			auto val = a[i];
			std::string time = val[0];
			std::string local_date_time = VtStringUtil::GetLocalTime(time);
			SmSimpleChartDataItem item;
			item.date_time = local_date_time;
			item.c = val[1];
			item.h = val[2];
			item.l = val[3];
			item.o = val[4];
			item.v = val[5];
			dataVec.push_back(item);
			cur_count++;
			if (cur_count % split_size == 0) {
				SendChartData(dataVec, data_req, a.size(), start_index, end_index);
			}
		}
		SendChartData(dataVec, data_req, a.size(), start_index, end_index - 1);
	}
	catch (const std::exception& e)
	{
		std::string error = e.what();
		LOG_F(INFO, "Query Error", error);
	}
}

void SmTimeSeriesServiceManager::OnChartDataReceived(SmChartDataRequest&& data_req)
{
	OnChartDataRequest(std::move(data_req));
}

void SmTimeSeriesServiceManager::SendChartData(std::vector<SmSimpleChartDataItem>& dataVec, SmChartDataRequest req, int totalCount, int startIndex, int endIndex)
{
	if (dataVec.size() == 0)
		return;
	json send_object;
	send_object["result"] = 0;
	send_object["symbol_code"] = req.symbolCode;
	send_object["chart_type"] = (int)req.chartType;
	send_object["cycle"] = req.cycle;
	send_object["total_count"] = totalCount;
	send_object["cur_count"] = (int)dataVec.size();
	send_object["start_index"] = startIndex;
	send_object["end_index"] = endIndex;
	for (size_t i = 0; i < dataVec.size(); ++i) {
		SmSimpleChartDataItem item = dataVec[i];
		send_object["data"][i] = {
			{ "date_time",  item.date_time },
			{ "high", item.h },
			{ "low",  item.l },
			{ "open",  item.o },
			{ "close",  item.c },
			{ "volume",  item.v }
		};
	}

	std::string content = send_object.dump(4);
	SmUserManager* userMgr = SmUserManager::GetInstance();
	userMgr->SendResultMessage(req.user_id, content);
}

SmChartData* SmTimeSeriesServiceManager::AddCycleDataReq(SmChartDataRequest data_req)
{
	SmChartData* chartData = new SmChartData();
	chartData->SymbolCode(data_req.symbolCode);
	chartData->ChartType(data_req.chartType);
	chartData->Cycle(data_req.cycle);
	chartData->AddUser(data_req.user_id);
	_CycleDataReqMap[data_req.GetDataKey()] = chartData;

	return chartData;
}

void SmTimeSeriesServiceManager::RegisterTimer(SmChartData* chartData)
{
	if (!chartData)
		return;
	auto curTime = SmUtil::GetLocalTime();
	int waitTime = 60 - std::get<2>(curTime);
	// Add to the timer.
	auto id = _Timer.add(seconds(waitTime), std::bind(&SmChartData::OnTimer, chartData), seconds(chartData->Cycle() * 60));
	// Add to the request map.
	_CycleDataReqTimerMap[chartData->GetDataKey()] = id;
}
