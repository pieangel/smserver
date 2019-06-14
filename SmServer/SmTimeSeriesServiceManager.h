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
	int SendDataSplitSize() const { return _SendDataSplitSize; }
	void SendDataSplitSize(int val) { _SendDataSplitSize = val; }
private:
	void SendChartData(std::vector<SmSimpleChartDataItem>& dataVec, SmChartDataRequest req, int totalCount, int startIndex, int endIndex);
	std::map<std::string, SmChartDataRequest> _HistoryDataReqMap;
	std::map<std::string, CppTime::timer_id> _CycleDataReqTimerMap;
	int _SendDataSplitSize = 20;
	CppTime::Timer _Timer;
	std::map<std::string, SmChartData*> _CycleDataReqMap;
	SmChartData* AddCycleDataReq(SmChartDataRequest data_req);
	void RegisterTimer(SmChartData* chartData);
};

