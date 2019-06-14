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
	void StartCollectChartData();
	void StartCollectSiseData();
	void GetChartData(SmChartDataRequest&& data_req);
private:
	CppTime::Timer _Timer;
	CppTime::timer_id _ChartDataTimerId;
	CppTime::timer_id _SiseTimerId;
	CppTime::timer_id _MinTimerId;
	void OnTimer();
	void OnEveryMinute();
	void OnSiseTimer();
	size_t _Index = 0;
	size_t _SiseIndex = 0;
};

