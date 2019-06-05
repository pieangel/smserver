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
	int cycle = 0;
	int count = 0;
	int next = 0;
	int reqKey = 0;
	// ������ 0 : ������, 1 : �����
	int seq = 0;
	bool domestic = false;
	std::string GetDataKey() {
		std::string key = symbolCode;
		key.append(":");
		key.append(std::to_string((int)chartType));
		key.append(":");
		key.append(std::to_string(cycle));
		return key;
	}
};

struct SmChartDataItem {
	std::string symbolCode;
	SmChartType chartType;
	int cycle = 0;
	std::string date;
	std::string time;
	int o = 0;
	int h = 0;
	int l = 0;
	int c = 0;
	int v = 0;
	std::string GetDataKey() {
		std::string key = symbolCode;
		key.append(":");
		key.append(std::to_string((int)chartType));
		key.append(":");
		key.append(std::to_string(cycle));
		return key;
	}
};

