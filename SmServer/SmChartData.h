#pragma once
#include <string>
#include "SmChartDefine.h"
#include <vector>
#include <set>
class SmChartData
{
private:
	std::string _SymbolCode;
	SmChartType _ChartType = SmChartType::MIN;
	int _Cycle = 0;
	std::vector< SmChartDataItem> _DataItemVec;
	// ��Ʈ�� ��û�ϴ� ����� ���̵� ���
	std::set<std::string> _UserList;
	void GetChartDataFromDB();
public:
	size_t GetUserCount() {
		return _UserList.size();
	}
	void RemoveUser(std::string user_id);
	void AddUser(std::string user_id) {
		_UserList.insert(user_id);
	}
	std::string SymbolCode() const { return _SymbolCode; }
	void SymbolCode(std::string val) { _SymbolCode = val; }
	SmChartType ChartType() const { return _ChartType; }
	void ChartType(SmChartType val) { _ChartType = val; }
	int Cycle() const { return _Cycle; }
	void Cycle(int val) { _Cycle = val; }
	// Ÿ�̸Ӱ� �Ҹ��� ȣ�� �Ǵ� �Լ�
	/// <summary>
	/// Ÿ�̸Ӱ� �Ҹ� �� ����� ����� �����Ͽ� ��Ʈ �����͸� �����Ͽ� Ŭ���̾�Ʈ���� �����Ѵ�.
	/// </summary>
	void OnTimer();
	std::string GetDataKey() {
		std::string key = _SymbolCode;
		key.append(":");
		key.append(std::to_string((int)_ChartType));
		key.append(":");
		key.append(std::to_string(_Cycle));
		return key;
	}
};

