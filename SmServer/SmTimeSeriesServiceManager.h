#pragma once
#include "Global/TemplateSingleton.h"
#include "SmChartDefine.h"
#include <vector>
#include <map>
#include <string>
#include "Timer/cpptime.h"

class SmChartData;
class SmTimeSeriesServiceManager : public TemplateSingleton<SmTimeSeriesServiceManager>
{
public:
	SmTimeSeriesServiceManager();
	~SmTimeSeriesServiceManager();
	void OnUnregisterCycleDataRequest(SmChartDataRequest&& data_req);
	void OnRegisterCycleDataRequest(SmChartDataRequest&& data_req);
	void OnChartDataRequest(SmChartDataRequest&& data_req);
	void OnChartDataReceived(SmChartDataRequest&& data_req);
	void OnSiseDataRequest(SmSiseDataRequest&& sise_req);
	void OnHogaDataRequest(SmHogaDataRequest&& hoga_req);
	void OnSymbolMasterRequest(SmSymbolMasterRequest&& master_req);
	int SendDataSplitSize() const { return _SendDataSplitSize; }
	void SendDataSplitSize(int val) { _SendDataSplitSize = val; }
	void OnCompleteChartData(SmChartDataRequest&& data_req, SmChartData* chart_data);
private:
	void SendChartData(std::vector<SmSimpleChartDataItem>& dataVec, SmChartDataRequest req, int totalCount, int startIndex, int endIndex);
	std::map<std::string, SmChartDataRequest> _HistoryDataReqMap;
	std::map<std::string, CppTime::timer_id> _CycleDataReqTimerMap;
	int _SendDataSplitSize = 20;
	CppTime::Timer _Timer;
	std::map<std::string, SmChartData*> _CycleDataReqMap;
	SmChartData* AddCycleDataReq(SmChartDataRequest data_req);
	void RegisterTimer(SmChartData* chartData);
	void SendChartDataFromDB(SmChartDataRequest&& data_req);
	void GetChartDataFromAnotherServer(SmChartDataRequest&& data_req);
	void SendChartData(SmChartDataRequest&& data_req, SmChartData* chart_data);
};

