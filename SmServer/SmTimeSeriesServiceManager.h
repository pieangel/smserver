#pragma once
#include "Global/TemplateSingleton.h"
#include "SmChartDefine.h"
#include <vector>
// user->symbol->charttype->cycle
class SmTimeSeriesServiceManager : public TemplateSingleton<SmTimeSeriesServiceManager>
{
public:
	SmTimeSeriesServiceManager();
	~SmTimeSeriesServiceManager();
	void OnChartDataRequest(SmChartDataRequest&& data_req);
private:
	void SendChartData(std::vector<SmSimpleChartDataItem>& dataVec, SmChartDataRequest req, int totalCount, int startIndex, int endIndex);
};

