#pragma once
#include <string>
#include "SmChartDefine.h"
#include <vector>
class SmChartData
{
private:
	std::string _SymbolCode;
	SmChartType _ChartType = SmChartType::MIN;
	int _Cycle = 0;
	std::vector< SmChartDataItem> _DataItemVec;
public:
	std::string SymbolCode() const { return _SymbolCode; }
	void SymbolCode(std::string val) { _SymbolCode = val; }
	SmChartType ChartType() const { return _ChartType; }
	void ChartType(SmChartType val) { _ChartType = val; }
	int Cycle() const { return _Cycle; }
	void Cycle(int val) { _Cycle = val; }
	std::string GetDataKey() {
		std::string key = _SymbolCode;
		key.append(":");
		key.append(std::to_string((int)_ChartType));
		key.append(":");
		key.append(std::to_string(_Cycle));
		return key;
	}
};

