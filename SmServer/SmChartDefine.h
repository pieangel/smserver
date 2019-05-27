#pragma once
#include <string>
enum class SmChartType
{
	NONE = 0,
	DAY,
	WEEK,
	MONTH,
	HOUR,
	MIN,
	TICK
};

struct SmChartDataRequest
{
	std::string symbolCode;
	SmChartType chartType;
	int cycle;
	int count;
	int next;
	int reqKey;
	// ������ 0 : ������, 1 : �����
	int seq;
	bool domestic;
};