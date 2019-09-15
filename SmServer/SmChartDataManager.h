#pragma once
#include <string>
#include <map>
#include <memory>
#include "Global/TemplateSingleton.h"
#include "SmChartDefine.h"
class SmChartData;
class SmChartDataManager : public TemplateSingleton<SmChartDataManager>
{
public:
	SmChartDataManager();
	~SmChartDataManager();
	std::shared_ptr<SmChartData> AddChartData(SmChartDataRequest data_req);
	void AddChartData(std::shared_ptr<SmChartData> chart_data);
	std::shared_ptr<SmChartData> FindChartData(std::string data_key);
	std::shared_ptr<SmChartData> AddChartData(SmChartDataItem data_item);
private:
	std::map<std::string, std::shared_ptr<SmChartData>> _ChartDataMap;
};

