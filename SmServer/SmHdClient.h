#pragma once
#include "Global/TemplateSingleton.h"
#include <string>
#include "SmChartDefine.h"
#include <atlstr.h>
class SmHdCtrl;
class SmSymbol;
class SmHdClient : public TemplateSingleton<SmHdClient>
{
public:
	SmHdClient();
	~SmHdClient();
	int Login(std::string id, std::string pwd, std::string cert);
	void RegisterProduct(std::string symCode);
	void UnregisterProduct(std::string symCode);
	void GetChartData(SmChartDataRequest req);
	void OnRcvdAbroadHoga(SmSymbol* sym);
	void OnRcvdAbroadSise(SmSymbol* sym);
	void DownloadMasterFiles(std::string param);
	void GetSisiData(std::string symCode);
private:
	SmHdCtrl* _HdCtrl = nullptr;
};

