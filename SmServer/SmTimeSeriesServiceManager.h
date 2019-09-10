#pragma once
#include "Global/TemplateSingleton.h"
#include "SmChartDefine.h"
#include <vector>
#include <map>
#include <string>
#include "Timer/cpptime.h"
#include "SmServieReqNumGenerator.h"

class SmChartData;
class SmWebsocketSession;
class SmTimeSeriesServiceManager : public TemplateSingleton<SmTimeSeriesServiceManager>
{
public:
	SmTimeSeriesServiceManager();
	~SmTimeSeriesServiceManager();
	void OnUnregisterCycleDataRequest(SmChartDataRequest&& data_req);
	void OnRegisterCycleDataRequest(SmChartDataRequest&& data_req);
	void OnChartDataRequest(SmChartDataRequest&& data_req);
	void OnChartDataReceived(SmChartDataRequest&& data_req);
	void OnSiseDataRequest(SmSiseDataRequest&& sise_req);
	void OnHogaDataRequest(SmHogaDataRequest&& hoga_req);
	void OnSymbolMasterRequest(SmSymbolMasterRequest&& master_req);
	int SendDataSplitSize() const { return _SendDataSplitSize; }
	void SendDataSplitSize(int val) { _SendDataSplitSize = val; }
	void OnCompleteChartData(SmChartDataRequest data_req, SmChartData* chart_data);
	SmWebsocketSession* SisiSocket() const { return _SisiSocket; }
	void SisiSocket(SmWebsocketSession* val) { _SisiSocket = val; }
	void ClearSiseSocket(SmWebsocketSession* session);
	void OnReqRegisterSiseSocket(SmWebsocketSession* socket);
	void ResendChartDataRequest(SmChartDataRequest req);
	void SendChartData(SmChartDataRequest data_req, SmChartData* chart_data);
private:
	SmServieReqNumGenerator _SvcNoGen;
	void RegisterCycleChartDataRequest(SmChartDataRequest data_req);
	void SendChartData(std::vector<SmSimpleChartDataItem>& dataVec, SmChartDataRequest req, int totalCount, int startIndex, int endIndex);
	std::map<int, SmChartDataRequest> _HistoryDataReqMap;
	std::map<std::string, CppTime::timer_id> _CycleDataReqTimerMap;
	int _SendDataSplitSize = 20;
	CppTime::Timer _Timer;
	std::map<std::string, SmChartData*> _CycleDataReqMap;
	SmChartData* AddCycleDataReq(SmChartDataRequest data_req);
	void RegisterTimer(SmChartData* chartData);
	void SendChartDataFromDB(SmChartDataRequest&& data_req);
	// ���ǻ糪 ������ ���� ��ü�� ���� �����͸� ��û�Ѵ�.
	void GetChartDataFromSourceServer(SmChartDataRequest&& data_req);
	
	SmWebsocketSession* _SisiSocket = nullptr;
	void SendRequestToSiseServer(std::string message);
	std::mutex _mutex;
};

