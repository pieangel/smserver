#pragma once
#include <string>
#include <map>
#include "Global/TemplateSingleton.h"
#include "SmChartDefine.h"
class SmChartData;
class SmChartDataManager : public TemplateSingleton<SmChartDataManager>
{
public:
	SmChartDataManager();
	~SmChartDataManager();
	SmChartData* AddChartData(SmChartDataRequest data_req);
	void AddChartData(SmChartData* chart_data);
	SmChartData* FindChartData(std::string data_key);
	SmChartData* PushChartData(SmChartDataItem data);
private:
	SmChartData* AddChartData(SmChartDataItem data_item);
	std::map<std::string, SmChartData*> _ChartDataMap;
};

