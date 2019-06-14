#pragma once
#include <string>
#include "SmChartDefine.h"
#include <vector>
#include <set>
#include <list>
class SmChartData
{
private:
	std::string _SymbolCode;
	SmChartType _ChartType = SmChartType::MIN;
	int _Cycle = 0;
	std::list<SmChartDataItem> _DataItemList;
	// 차트를 요청하는 사용자 아이디 목록
	std::set<std::string> _UserList;
	void GetChartDataFromDB();
	void GetChartDataFromServer();
	size_t _DataQueueSize = 4;
	// 등록된 사용자들에게 차트 정기 데이터를 보내준다.
	void SendCyclicChartDataToUsers();
public:
	// 차트 데이터가 새로 도착했음을 알린다.
	void OnChartDataUpdated();
	void PushChartDataItem(SmChartDataItem data);
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
	// 타이머가 불릴때 호출 되는 함수
	/// <summary>
	/// 타이머가 불릴 때 사용자 목록을 참조하여 차트 데이터를 조작하여 클라이언트에게 전송한다.
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
	int DataQueueSize() const { return _DataQueueSize; }
	void DataQueueSize(int val) { _DataQueueSize = val; }
};

