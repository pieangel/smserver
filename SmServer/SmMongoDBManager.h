#pragma once
#include "Global/TemplateSingleton.h"
#include <mongocxx\config\config.hpp>
#include "SmChartDefine.h"
namespace mongocxx
{
	MONGOCXX_INLINE_NAMESPACE_BEGIN
		class instance;
		class client;
		class pool;
	MONGOCXX_INLINE_NAMESPACE_END
};

class SmMongoDBManager : public TemplateSingleton<SmMongoDBManager>
{
public:
	SmMongoDBManager();
	~SmMongoDBManager();
	void Test();
	void ReadSymbol();
	void LoadMarketList();
	void LoadSymbolList();
	void SendChartDataFromDB(SmChartDataRequest&& data_req);
	void SendChartData(SmChartDataRequest data_req);
	void SendQuote(std::string symbol_code);
	void SendHoga(std::string symbol_code);
	void SendChartCycleData(SmChartDataRequest data_req);
private:
	void SaveMarketsToDatabase();
	void SaveSymbolsToDatabase();
	void InitDatabase();
	mongocxx::instance* _Instance = nullptr;
	mongocxx::client* _Client = nullptr;
	mongocxx::pool* _ConnPool = nullptr;
	int _SendDataSplitSize = 20;
	bool _SendingHoga = false;
	std::mutex _mutex;
};
