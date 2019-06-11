#pragma once
#include "Global/TemplateSingleton.h"
#include "Timer/cpptime.h"
#include "SmChartDefine.h"

class SmTimeSeriesCollector : public TemplateSingleton<SmTimeSeriesCollector>
{
public:
	SmTimeSeriesCollector();
	~SmTimeSeriesCollector();
	void CollectRecentMonthSymbolChartData();
	void OnChartDataItem(SmChartDataItem&& data_item);
	void OnCompleteChartData(SmChartDataRequest&& data_req);
	void StartCollectData();
	void GetChartData(SmChartDataRequest&& data_req);
private:
	CppTime::Timer _Timer;
	CppTime::timer_id _TimerId;
	void OnTimer();
	size_t _Index = 0;
};

