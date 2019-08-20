#include "pch.h"
#include "SmChartData.h"
#include "SmUserManager.h"
#include "SmTimeSeriesDBManager.h"
#include "Json/json.hpp"
#include "Log/loguru.hpp"
#include "SmHdClient.h"
#include "SmUtil.h"
#include "Util/VtStringUtil.h"
#include "SmServiceDefine.h"

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
	req.reqType = SmChartDataReqestType::FIRST;
	req.symbolCode = _SymbolCode;
	req.chartType = _ChartType;
	req.cycle = _Cycle;
	req.count = _DataQueueSize;
	req.next = 0;
	SmHdClient* client = SmHdClient::GetInstance();
	client->GetChartData(req);
}

void SmChartData::GetCyclicDataFromServer()
{
	SmChartDataRequest req;
	req.reqType = SmChartDataReqestType::CYCLE;
	req.symbolCode = _SymbolCode;
	req.chartType = _ChartType;
	req.cycle = _Cycle;
	req.count = _CycleDataSize;
	req.next = 0;
	SmHdClient* client = SmHdClient::GetInstance();
	client->GetChartData(req);
}

void SmChartData::SendCyclicChartDataToUsers()
{
	CString msg;
	msg.Format("��Ʈ������ ������Ʈ �� %d\n", 0);
	//TRACE(msg);

	json send_object;
	send_object["res_id"] = SmProtocol::res_chart_cycle_data;
	send_object["symbol_code"] = _SymbolCode;
	send_object["chart_type"] = (int)_ChartType;
	send_object["cycle"] = _Cycle;
	send_object["total_count"] = _CycleDataSize;
	auto it = _DataItemList.begin();
	std::advance(it, _DataItemList.size() - _CycleDataSize);
	int k = 0;
	// ���� �ֱٵ����͸� ������. ���� �ֱ� �����͸� ���� �ִ´�.
	for (; it != _DataItemList.end(); ++it) {
		SmChartDataItem item = *it;
		std::string time = item.date + item.time;
		CString msg;
		msg.Format("send cycle data ::symbol = %s, dt = %s , o = %d, h = %d, l= %d, c = %d\n", _SymbolCode.c_str(), time.c_str(), item.o, item.h, item.l, item.c);
		TRACE(msg);
		std::string date_time = VtStringUtil::GetLocalTimeByDatetimeString(time);
		send_object["data"][k++] = {
			{ "date_time",  date_time },
			{ "high", item.h },
			{ "low",  item.l },
			{ "open",  item.o },
			{ "close",  item.c },
			{ "volume",  item.v }
		};
		if (k == _CycleDataSize)
			break;
	}

	std::string content = send_object.dump(4);
	SmUserManager* userMgr = SmUserManager::GetInstance();
	// ��ϵ� �����鿡�� ��Ʈ �����͸� ������.
	for (auto it = _UserList.begin(); it != _UserList.end(); ++it) {
		userMgr->SendResultMessage(*it, content);
	}
}

std::pair<int, int> SmChartData::GetCycleByTimeDif()
{
	if (_DataItemList.size() < 2)
		return std::make_pair(0,0);
	// �Ǹ����� ��Ҹ� �����´�.
	auto it = _DataItemList.rbegin();
	SmChartDataItem newItem = *it++;
	SmChartDataItem oldItem = *it;

	std::string new_datetime = newItem.date + newItem.time;
	std::string old_datetime = oldItem.date + oldItem.time;
	// �ֱ⸦ �ʷ� ȯ����
	double cycleSeconds = SmUtil::GetDifTimeBySeconds(new_datetime, old_datetime);
	// ���� �ð����� ������ �ð����� ����ؾ� �ϴ� �ð��� �ʷ� ȯ����
	double waitSeconds = SmUtil::GetDifTimeForNow(new_datetime);
	return std::make_pair((int)cycleSeconds, (int)waitSeconds);
}

void SmChartData::OnChartDataUpdated()
{
	// ���⼭ ���ο� �����͸� ������.
	SendCyclicChartDataToUsers();
}

void SmChartData::PushChartDataItemToBack(SmChartDataItem data)
{
	CString msg;
	

	_DataItemList.push_back(data);
	if (_DataItemList.size() > _DataQueueSize) {
		_DataItemList.pop_front();
	}

	//msg.Format(_T("pushed data :: size = %d, code = %s, date = %s, time = %s, o = %d, h = %d, l = %d, c = %d, v = %d\n"), _DataItemList.size(), SymbolCode().c_str(), data.date.c_str(), data.time.c_str(), data.o, data.h, data.l, data.c, data.v);
	//TRACE(msg);

	GetCycleByTimeDif();
}

void SmChartData::PushChartDataItemToFront(SmChartDataItem data)
{
	CString msg;


	_DataItemList.push_front(data);
	if (_DataItemList.size() > _DataQueueSize) {
		_DataItemList.pop_back();
	}

	//msg.Format(_T("pushed data :: size = %d, code = %s, date = %s, time = %s, o = %d, h = %d, l = %d, c = %d, v = %d\n"), _DataItemList.size(), SymbolCode().c_str(), data.date.c_str(), data.time.c_str(), data.o, data.h, data.l, data.c, data.v);
	//TRACE(msg);

	GetCycleByTimeDif();
}

void SmChartData::UpdateChartData(SmChartDataItem data)
{
	// ������ �����Ͱ� ���� ���� ������Ʈ ���� �ʴ´�.
	if (data.o == 0 || data.h == 0 || data.l == 0 || data.c == 0)
		return;

	CString msg;
	//msg.Format(_T("UpdateChartData:: size = %d, code = %s, date = %s, time = %s, o = %d, h = %d, l = %d, c = %d, v = %d\n"), _DataItemList.size(), SymbolCode().c_str(), data.date.c_str(), data.time.c_str(), data.o, data.h, data.l, data.c, data.v);
	//TRACE(msg);
	for (auto it = _DataItemList.rbegin(); it != _DataItemList.rend(); ++it) {
		SmChartDataItem& item = *it;
		// �����Ͱ� ������ ���� �����ϰ� ���� ������.
		if (item.date.compare(data.date) == 0 && item.time.compare(data.time) == 0) {
			item.h = data.h;
			item.l = data.l;
			item.o = data.o;
			item.c = data.c;
			item.v = data.v;
			return;
		}
	}

	// �����Ͱ� ������ ���ο� �����ͷ� �����ϰ� �� ��(���� �ֽ� ������)�� ���δ�.
	_DataItemList.push_back(data);
	if (_DataItemList.size() > _DataQueueSize) {
		_DataItemList.pop_front();
	}
	int k = 0;
	for (auto it = _DataItemList.rbegin(); it != _DataItemList.rend(); ++it) {
		SmChartDataItem item = *it;
		msg.Format(_T(">>>>>>>>>>>>>>>>>>> size = %d, code = %s, date = %s, time = %s, o = %d, h = %d, l = %d, c = %d, v = %d\n"), _DataItemList.size(), SymbolCode().c_str(), item.date.c_str(), item.time.c_str(), item.o, item.h, item.l, item.c, item.v);
		//TRACE(msg);
		if (k == 3)
			break;
		k++;
	}
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
	GetCyclicDataFromServer();
}
