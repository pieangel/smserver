#pragma once
#include "Global/TemplateSingleton.h"
#include <string>
#include "SmChartDefine.h"
class SmHdCtrl;
class SmHdClient : public TemplateSingleton<SmHdClient>
{
public:
	SmHdClient();
	~SmHdClient();
	int Login(std::string id, std::string pwd, std::string cert);
	void RegisterProduct(std::string symCode);
	void UnregisterProduct(std::string symCode);
	void GetChartData(SmChartDataRequest req);
	void OnRcvdAbroadHoga(CString& strKey, LONG& nRealType);
	void OnRcvdAbroadSise(CString& strKey, LONG& nRealType);
private:
	SmHdCtrl* _HdCtrl = nullptr;
};

