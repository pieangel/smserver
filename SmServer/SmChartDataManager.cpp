#include "pch.h"
#include "SmChartDataManager.h"
#include "SmChartData.h"

SmChartDataManager::SmChartDataManager()
{

}

SmChartDataManager::~SmChartDataManager()
{
}

void SmChartDataManager::AddChartData(std::shared_ptr<SmChartData> chart_data)
{
	if (!chart_data)
		return;

	auto it = _ChartDataMap.find(chart_data->GetDataKey());
	if (it == _ChartDataMap.end()) {
		_ChartDataMap[chart_data->GetDataKey()] = chart_data;
	}
}

std::shared_ptr<SmChartData> SmChartDataManager::AddChartData(SmChartDataRequest data_req)
{
	std::shared_ptr<SmChartData> chartData = FindChartData(data_req.GetDataKey());
	if (!chartData) {
		chartData = std::make_shared<SmChartData>();
		chartData->SymbolCode(data_req.symbolCode);
		chartData->ChartType(data_req.chartType);
		chartData->Cycle(data_req.cycle);
		_ChartDataMap[data_req.GetDataKey()] = chartData;
	}

	return chartData;
}

std::shared_ptr<SmChartData> SmChartDataManager::AddChartData(SmChartDataItem data_item)
{
	std::shared_ptr<SmChartData> chartData = FindChartData(data_item.GetDataKey());
	if (!chartData) {
		chartData = std::make_shared<SmChartData>();
		chartData->SymbolCode(data_item.symbolCode);
		chartData->ChartType(data_item.chartType);
		chartData->Cycle(data_item.cycle);
		_ChartDataMap[data_item.GetDataKey()] = chartData;
	}

	return chartData;
}

std::shared_ptr<SmChartData> SmChartDataManager::FindChartData(std::string data_key)
{
	auto it = _ChartDataMap.find(data_key);
	if (it != _ChartDataMap.end()) {
		return it->second;
	}

	return nullptr;
}

