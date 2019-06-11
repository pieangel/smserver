#pragma once
#include <string>
#include <map>
#include "Global/TemplateSingleton.h"
class SmChartData;
class SmChartDataManager : public TemplateSingleton<SmChartDataManager>
{
public:
	SmChartDataManager();
	~SmChartDataManager();
	void AddChartData(SmChartData* chart_data);
	SmChartData* FindChartData(std::string data_key);
private:
	std::map<std::string, SmChartData*> _ChartDataMap;
};

