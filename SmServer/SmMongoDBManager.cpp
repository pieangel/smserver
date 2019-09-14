#include "SmMongoDBManager.h"
#include <chrono>

#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/pool.hpp>
#include <string>
#include <iostream>
#include "SmMarketManager.h"
#include "SmSymbolReader.h"
#include "SmConfigManager.h"
#include "SmUtfUtil.h"
#include "SmMarket.h"
#include "SmCategory.h"
#include "SmSymbol.h"
#include <codecvt>
#include <locale>
#include "SmSymbolManager.h"
#include "SmTimeSeriesServiceManager.h"
#include "Util/VtStringUtil.h"
#include "SmChartDataManager.h"
#include "SmChartData.h"
#include "SmServiceDefine.h"
#include "SmGlobal.h"
#include "SmSessionManager.h"

#include "Json/json.hpp"

using namespace nlohmann;

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

SmMongoDBManager::SmMongoDBManager()
{
	InitDatabase();
}

SmMongoDBManager::~SmMongoDBManager()
{
	if (_Client) {
		delete _Client;
		_Client = nullptr;
	}

	if (_Instance) {
		delete _Instance;
		_Instance = nullptr;
	}

	if (_ConnPool) {
		delete _ConnPool;
		_ConnPool = nullptr;
	}
}

void SmMongoDBManager::Test()
{
	if (!_Client)
		return;

	auto db = (*_Client)["andromeda"];

	// TODO: fix dates

	std::string name = "company";

	// @begin: cpp-insert-a-document
	bsoncxx::document::value restaurant_doc = make_document(
		kvp("address",
			make_document(kvp("street", "2 Avenue"),
				kvp("zipcode", 10075),
				kvp("building", "1480"),
				kvp("coord", make_array(-73.9557413, 40.7720266)))),
		kvp("borough", "Manhattan"),
		kvp("cuisine", "Italian"),
		kvp("grades",
			make_array(
				make_document(kvp("date", bsoncxx::types::b_date{ std::chrono::milliseconds{12323} }),
					kvp("grade", "A"),
					kvp("score", 11)),
				make_document(
					kvp("date", bsoncxx::types::b_date{ std::chrono::milliseconds{121212} }),
					kvp("grade", "B"),
					kvp("score", 17)))),
		kvp("name", name),
		kvp("restaurant_id", "41704623"));

	// We choose to move in our document here, which transfers ownership to insert_one()
	auto res = db["andromeda"].insert_one(std::move(restaurant_doc));
	// @end: cpp-insert-a-document

	auto builder = bsoncxx::builder::stream::document{};
	bsoncxx::document::value doc_value = builder
		<< "name" << "MongoDB"
		<< "type" << "database"
		<< "count" << 1
		<< "versions" << bsoncxx::builder::stream::open_array
		<< "v3.2" << "v3.0" << "v2.6"
		<< close_array
		<< "info" << bsoncxx::builder::stream::open_document
		<< "x" << 203
		<< "y" << 102
		<< bsoncxx::builder::stream::close_document
		<< bsoncxx::builder::stream::finalize;

	res = db["database"].insert_one(std::move(doc_value));


	std::vector<bsoncxx::document::value> documents;
	for (int i = 0; i < 100; i++) {
		documents.push_back(
			bsoncxx::builder::stream::document{} << "i" << i << finalize);
	}

	mongocxx::collection coll = db["test"];
	coll.insert_many(documents);


	bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
		coll.find_one(document{} << "i" << 18 << finalize);
	if (maybe_result) {
		std::cout << bsoncxx::to_json(*maybe_result) << "\n";
	}



	mongocxx::cursor cursor = coll.find(
		document{} << "i" << open_document <<
		"$gt" << 5 <<
		"$lte" << 10
		<< close_document << finalize);
	for (auto doc : cursor) {
		std::cout << bsoncxx::to_json(doc) << "\n";
	}



	coll.update_one(document{} << "i" << 10 << finalize,
		document{} << "$set" << open_document <<
		"i" << 110 << close_document << finalize);

	bsoncxx::stdx::optional<mongocxx::result::update> result =
		coll.update_many(
			document{} << "i" << open_document <<
			"$lt" << 100 << close_document << finalize,
			document{} << "$inc" << open_document <<
			"i" << 100 << close_document << finalize);

	if (result) {
		std::cout << result->modified_count() << "\n";
	}

}

void SmMongoDBManager::ReadSymbol()
{
	SmSymbolReader* symReader = SmSymbolReader::GetInstance();
	std::string dir = symReader->GetWorkingDir();
	std::string name = dir;
	SmMarketManager* mrktMgr = SmMarketManager::GetInstance();

	//DbTest();

	SmConfigManager* configMgr = SmConfigManager::GetInstance();
	std::string appPath = configMgr->GetApplicationPath();

	mrktMgr->ReadSymbolsFromFile();

	SaveMarketsToDatabase();

	SaveSymbolsToDatabase();
}

void SmMongoDBManager::LoadMarketList()
{
	try
	{
		//std::lock_guard<std::mutex> lock(_mutex);

		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["market_list"];
		SmMarketManager* marketMgr = SmMarketManager::GetInstance();
		mongocxx::cursor cursor = coll.find({});
		for (auto doc : cursor) {
			std::string object = bsoncxx::to_json(doc);
			auto json_object = json::parse(object);
			std::string market_name = json_object["market_name"];
			market_name = SmUtfUtil::Utf8ToAnsi(market_name);

			SmMarket* newMarket = marketMgr->AddMarket(market_name);
			int market_index = json_object["market_index"];
			newMarket->Name(market_name);
			newMarket->Index(market_index);
			// Get the product list
			auto product_list = json_object["product_list"];
			int count = product_list.size();
			for (int i = 0; i < count; ++i) {
				auto product = product_list[i];
				int product_index = product["product_index"];
				std::string product_code = product["product_code"];
				std::string product_name_kr = product["product_name_kr"];
				product_name_kr = SmUtfUtil::Utf8ToAnsi(product_name_kr);
				std::string product_name_en = product["product_name_en"];
				std::string exchange_name = product["exchange_name"];
				std::string exchange_code = product["exchange_code"];
				std::string market_name2 = product["market_name"];
				market_name2 = SmUtfUtil::Utf8ToAnsi(market_name2);
				
				SmCategory* category = newMarket->AddCategory(product_code);
				category->Code(product_code);
				category->Name(product_name_en);
				category->NameKr(product_name_kr);
				category->Exchange(exchange_name);
				category->ExchangeCode(exchange_code);
				category->MarketName(market_name2);
				marketMgr->AddCategoryMarket(product_code, market_name2);
			}
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::LoadSymbolList()
{
	try
	{
		//std::lock_guard<std::mutex> lock(_mutex);

		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;
		mongocxx::collection coll = db["symbol_list"];
		SmMarketManager* marketMgr = SmMarketManager::GetInstance();
		mongocxx::cursor cursor = coll.find({});
		for (auto doc : cursor) {
			std::string object = bsoncxx::to_json(doc);
			auto json_object = json::parse(object);
			std::string market_name = json_object["market_name"];
			market_name = SmUtfUtil::Utf8ToAnsi(market_name);
			std::string product_code = json_object["product_code"];
			SmCategory* foundCategory = marketMgr->FindCategory(market_name, product_code);
			if (!foundCategory)
				continue;

			std::string symbol_code = json_object["symbol_code"];
			std::string symbol_name_kr = json_object["symbol_name_kr"];
			symbol_name_kr = SmUtfUtil::Utf8ToAnsi(symbol_name_kr);
			std::string symbol_name_en = json_object["symbol_name_en"];
			int symbol_index = json_object["symbol_index"];
			int decimal = json_object["decimal"];
			double contract_unit = json_object["contract_unit"];
			int seungsu = json_object["seungsu"];
			double tick_size = json_object["tick_size"];
			double tick_value = json_object["tick_value"];
			SmSymbol* symbol = foundCategory->AddSymbol(symbol_code);
			symbol->Index(symbol_index);
			symbol->SymbolCode(symbol_code);
			symbol->CategoryCode(product_code);
			symbol->Name(symbol_name_kr);
			symbol->NameEn(symbol_name_en);
			symbol->CtrUnit(contract_unit);
			symbol->TickSize(tick_size);
			symbol->TickValue(tick_value);
			symbol->Decimal(decimal);
			symbol->Seungsu(seungsu);
			symbol->MarketName(market_name);
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::SendChartDataFromDB(SmChartDataRequest&& data_req)
{
	try
	{
		//std::lock_guard<std::mutex> lock(_mutex);

		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db[data_req.GetDataKey()];
		
		// @begin: cpp-query-sort
		mongocxx::options::find opts;
		// 최신것이 앞에 오도록 한다.
		opts.sort(make_document(kvp("date_time", -1)));
		// 과거의 것이 앞에 오도록 한다.
		//opts.sort(make_document(kvp("date_time", 1)));
		opts.limit(data_req.count);
		SmTimeSeriesServiceManager* tsMgr = SmTimeSeriesServiceManager::GetInstance();
		int data_count = coll.count_documents({});
		// 데이터가 없거나 요청한 갯수보다 적으면 서버에 요청을 한다.
		if (data_count == 0 || data_count < data_req.count) {
			if (tsMgr->SisiSocket()) {
				tsMgr->ResendChartDataRequest(data_req);
				return;
			}
		} 

		auto dt = VtStringUtil::GetCurrentDateTimeNoSecond();
		std::string d_t = dt.first + dt.second;

		bsoncxx::stdx::optional<bsoncxx::document::value> found_symbol =
			coll.find_one(bsoncxx::builder::stream::document{} << "date_time" << d_t << finalize);
		// 최신 데이터가 현재 날짜와 같지 않으면 서버에 요청한다.
		if (!found_symbol) {
			if (tsMgr->SisiSocket()) {
				tsMgr->ResendChartDataRequest(data_req);
				return;
			}
		}
		
		SendChartData(data_req);
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::SendChartData(SmChartDataRequest data_req)
{
	try
	{
		//std::lock_guard<std::mutex> lock(_mutex);

		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db[data_req.GetDataKey()];

		// @begin: cpp-query-sort
		mongocxx::options::find opts;
		// 최신것이 앞에 오도록 한다.
		opts.sort(make_document(kvp("date_time", -1)));
		// 과거의 것이 앞에 오도록 한다.
		//opts.sort(make_document(kvp("date_time", 1)));
		opts.limit(data_req.count);
		mongocxx::cursor cursor = coll.find({}, opts);
		//int total_count = std::distance(cursor.begin(), cursor.end());
		SmChartDataManager* chartDataMgr = SmChartDataManager::GetInstance();
		SmChartData* chart_data = chartDataMgr->AddChartData(data_req);
		for (auto&& doc : cursor) {
			std::string object = bsoncxx::to_json(doc);
			auto json_object = json::parse(object);
			std::string date_time = json_object["date_time"];
			std::string date = json_object["local_date"];
			std::string time = json_object["local_time"];
			int o = json_object["o"];
			int h = json_object["h"];
			int l = json_object["l"];
			int c = json_object["c"];
			int v = json_object["v"];

			SmChartDataItem data;
			data.symbolCode = data_req.symbolCode;
			data.chartType = data_req.chartType;
			data.cycle = data_req.cycle;
			data.date = date;
			data.time = time;
			data.h = h;
			data.l = l;
			data.o = o;
			data.c = c;
			data.v = v;
			chart_data->PushChartDataItemToFront(data);	
		}

		SmTimeSeriesServiceManager* tsMgr = SmTimeSeriesServiceManager::GetInstance();
		tsMgr->SendChartData(data_req, chart_data);
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::SendQuote(std::string symbol_code)
{
	try
	{
		//std::lock_guard<std::mutex> lock(_mutex);

		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["quote"];

		
		bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
			coll.find_one(bsoncxx::builder::stream::document{} << "symbol_code" << symbol_code << finalize);
		if (maybe_result) {
			std::string message = bsoncxx::to_json(*maybe_result);
			auto json_object = json::parse(message);
			json_object["res_id"] = (int)SmProtocol::res_realtime_sise;
			std::string content = json_object.dump(4);
			SmGlobal* global = SmGlobal::GetInstance();
			std::shared_ptr<SmSessionManager> sessMgr = global->GetSessionManager();
			sessMgr->send(content);
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::SendHoga(std::string symbol_code)
{
	try
	{
		//std::lock_guard<std::mutex> lock(_mutex);

		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["hoga"];


		bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
			coll.find_one(bsoncxx::builder::stream::document{} << "symbol_code" << symbol_code << finalize);
		if (maybe_result) {
			std::string message = bsoncxx::to_json(*maybe_result);
			auto json_object = json::parse(message);
			json_object["res_id"] = (int)SmProtocol::res_realtime_hoga;
			std::string content = json_object.dump(4);
			SmGlobal* global = SmGlobal::GetInstance();
			std::shared_ptr<SmSessionManager> sessMgr = global->GetSessionManager();
			sessMgr->send(content);
		}

	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::SendChartCycleData(SmChartDataRequest data_req)
{
	try
	{
		//std::lock_guard<std::mutex> lock(_mutex);

		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db[data_req.GetDataKey()];

		// @begin: cpp-query-sort
		mongocxx::options::find opts;
		// 최신것이 앞에 오도록 한다.
		opts.sort(make_document(kvp("date_time", -1)));
		// 과거의 것이 앞에 오도록 한다.
		//opts.sort(make_document(kvp("date_time", 1)));
		opts.limit(data_req.count);

		json send_object;
		send_object["res_id"] = SmProtocol::res_chart_cycle_data;
		send_object["symbol_code"] = data_req.symbolCode;
		send_object["chart_type"] = (int)data_req.chartType;
		send_object["cycle"] = data_req.cycle;
		send_object["total_count"] = data_req.count;

		mongocxx::cursor cursor = coll.find({}, opts);
		//int total_count = std::distance(cursor.begin(), cursor.end());
		SmChartDataManager* chartDataMgr = SmChartDataManager::GetInstance();
		SmChartData* chart_data = chartDataMgr->AddChartData(data_req);
		int k = 0;
		for (auto&& doc : cursor) {
			std::string object = bsoncxx::to_json(doc);
			auto json_object = json::parse(object);
			std::string date_time = json_object["date_time"];
			int o = json_object["o"];
			int h = json_object["h"];
			int l = json_object["l"];
			int c = json_object["c"];
			int v = json_object["v"];

			send_object["data"][k++] = {
			{ "date_time",  date_time },
			{ "high", h },
			{ "low",  l },
			{ "open",  o },
			{ "close", c },
			{ "volume", v }
			};
		}

		std::string content = send_object.dump(4);
		SmGlobal* global = SmGlobal::GetInstance();
		std::shared_ptr<SmSessionManager> sessMgr = global->GetSessionManager();
		sessMgr->send(content);
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::SaveMarketsToDatabase()
{
	try
	{
		//std::lock_guard<std::mutex> lock(_mutex);

		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		// 먼저 시장이 있는지 검색한다. 
		// 그리고 시장 속에 상품이 있는지 검색한다.
		mongocxx::collection coll = db["market_list"];
		
		builder::stream::document builder{};

		SmMarketManager* marketMgr = SmMarketManager::GetInstance();
		std::vector<SmMarket*>& marketList = marketMgr->GetMarketList();
		for (size_t i = 0; i < marketList.size(); ++i) {
			SmMarket* market = marketList[i];
			bsoncxx::stdx::optional<bsoncxx::document::value> found_market =
				coll.find_one(bsoncxx::builder::stream::document{} << "market_name" << SmUtfUtil::AnsiToUtf8((char*)market->Name().c_str()) << finalize);
			if (!found_market) {
				auto in_array = builder << "product_list" << builder::stream::open_array;
				std::vector<SmCategory*>& catVec = market->GetCategoryList();
				for (size_t j = 0; j < catVec.size(); ++j) {
					SmCategory* cat = catVec[j];
					bsoncxx::stdx::optional<bsoncxx::document::value> found_product =
						coll.find_one(bsoncxx::builder::stream::document{} << "prodcut_list.product_code" << cat->Code() << finalize);
					if (!found_product) {
						in_array = in_array << builder::stream::open_document
							<< "product_index" << (int)j
							<< "product_code" << cat->Code()
							<< "product_name_kr" << SmUtfUtil::AnsiToUtf8((char*)cat->NameKr().c_str())
							<< "product_name_en" << cat->Name()
							<< "exchange_name" << cat->Exchange()
							<< "exchange_code" << cat->ExchangeCode()
							<< "market_name" << SmUtfUtil::AnsiToUtf8((char*)cat->MarketName().c_str())
							<< builder::stream::close_document;
					}
				}
				auto after_array = in_array << builder::stream::close_array;
				after_array << "market_index" << (int)i
					<< "market_name" << SmUtfUtil::AnsiToUtf8((char*)market->Name().c_str());
				bsoncxx::document::value doc = after_array << builder::stream::finalize;
				auto res = db["market_list"].insert_one(std::move(doc));
			}
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::SaveSymbolsToDatabase()
{
	try
	{
		//std::lock_guard<std::mutex> lock(_mutex);

		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		// 먼저 시장이 있는지 검색한다. 
		// 그리고 시장 속에 상품이 있는지 검색한다.
		mongocxx::collection coll = db["symbol_list"];

		builder::stream::document builder{};

		SmMarketManager* marketMgr = SmMarketManager::GetInstance();
		std::vector<SmMarket*>& marketList = marketMgr->GetMarketList();

		for (size_t i = 0; i < marketList.size(); ++i) {
			SmMarket* market = marketList[i];
			std::vector<SmCategory*>& cat_list = market->GetCategoryList();
			for (size_t j = 0; j < cat_list.size(); ++j) {
				SmCategory* cat = cat_list[j];
				std::vector<SmSymbol*>& sym_list = cat->GetSymbolList();
				for (size_t k = 0; k < sym_list.size(); ++k) {
					SmSymbol* sym = sym_list[k];
					bsoncxx::stdx::optional<bsoncxx::document::value> found_symbol =
						coll.find_one(bsoncxx::builder::stream::document{} << "symbol_list" << sym->SymbolCode() << finalize);
					if (!found_symbol) {
						bsoncxx::document::value doc_value = builder
							<< "symbol_code" << sym->SymbolCode()
							<< "symbol_index" << sym->Index()
							<< "symbol_name_kr" << SmUtfUtil::AnsiToUtf8((char*)sym->Name().c_str())
							<< "symbol_name_en" << sym->NameEn()
							<< "product_code" << sym->CategoryCode()
							<< "market_name" << SmUtfUtil::AnsiToUtf8((char*)sym->MarketName().c_str())
							<< "decimal" << sym->Decimal()
							<< "contract_unit" << sym->CtrUnit()
							<< "seungsu" << sym->Seungsu()
							<< "tick_size" << sym->TickSize()
							<< "tick_value" << sym->TickValue()
							<< bsoncxx::builder::stream::finalize;
						auto res = db["symbol_list"].insert_one(std::move(doc_value));
					}
				}
			}
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::InitDatabase()
{
	_Instance = new mongocxx::instance();
	_Client = new mongocxx::client(mongocxx::uri{});
	_ConnPool = new mongocxx::pool(mongocxx::uri{});
}
