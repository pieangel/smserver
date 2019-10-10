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
#include "SmAccount.h"
#include "SmSessionManager.h"
#include "SmPosition.h"
#include "SmOrder.h"
#include "SmAccountManager.h"
#include "SmTotalOrderManager.h"
#include "SmTotalPositionManager.h"
#include "SmOrderNumberGenerator.h"
#include "Json/json.hpp"
#include "SmConfigManager.h"
#include "Xml/pugixml.hpp"
#include <windows.h>

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

#define DebugOut( s )            \
{                             \
   std::ostringstream os_;    \
   os_ << s;                   \
}

SmMongoDBManager::SmMongoDBManager()
{
	InitDatabase();
	SmConfigManager* configMgr = SmConfigManager::GetInstance();
	std::string appPath = configMgr->GetApplicationPath();
	std::string configPath = appPath;
	configPath.append("\\Config\\Config.xml");
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(configPath.c_str());
	pugi::xml_node app = doc.first_child();
	pugi::xml_node domestic_list_node = doc.child("application").child("domestic_list");
	pugi::xml_node domestic_node = domestic_list_node.first_child();
	while (domestic_node) {
		std::string code = domestic_node.text().as_string();
		_DomesticList.insert(code);
		domestic_node = domestic_node.next_sibling();
	}
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

	if (_URI) {
		delete _URI;
		_URI = nullptr;
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

				if (product_code.length() > 2 && std::isdigit(product_code.at(2))) {
					auto it = _DomesticList.find(product_code);
					if (it == _DomesticList.end())
						continue;
				}
				
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
			std::shared_ptr<SmSymbol> symbol = foundCategory->AddSymbol(symbol_code);
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
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db[data_req.GetDataKey()];
		
		SmTimeSeriesServiceManager* tsMgr = SmTimeSeriesServiceManager::GetInstance();
		long long data_count = coll.count_documents({});
		// 데이터가 없거나 요청한 갯수보다 적으면 서버에 요청을 한다.
		if (data_count == 0 || data_count < data_req.count) {
			if (tsMgr->SisiSocket()) {
				//SendChartDataOneByOne(data_req);
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
				//SendChartDataOneByOne(data_req);
				tsMgr->ResendChartDataRequest(data_req);
				return;
			}
		}
		
		SendChartDataOneByOne(data_req);
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::SendChartDataOneByOne(SmChartDataRequest data_req)
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
		
		opts.sort(make_document(kvp("date_time", -1)));
		
		auto item_count = db[data_req.GetDataKey()].count_documents({});
		if (item_count == 0)
			return;

		int limit = item_count > data_req.count ? data_req.count : item_count;
		// 과거의 것이 앞에 오도록 한다.
		//opts.sort(make_document(kvp("date_time", 1)));
		opts.limit(limit);

		mongocxx::cursor cursor = coll.find({}, opts);
		SmChartDataManager* chartDataMgr = SmChartDataManager::GetInstance();
		std::shared_ptr<SmChartData> chart_data = chartDataMgr->AddChartData(data_req);
		int count = 1;
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
			data.current_count = count;
			data.total_count = limit;
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

			char buffer[4096];
			sprintf(buffer, "SendChartDataOnebyOne%s\n", date_time.c_str());
			OutputDebugString(buffer);

			SmTimeSeriesServiceManager* tsMgr = SmTimeSeriesServiceManager::GetInstance();
			tsMgr->SendChartData(data_req, data);
			count++;
			
		}

		
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
		std::shared_ptr<SmChartData> chart_data = chartDataMgr->AddChartData(data_req);
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
		std::shared_ptr<SmChartData> chart_data = chartDataMgr->AddChartData(data_req);
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

std::tuple<int, int, int> SmMongoDBManager::GetAccountNo()
{
	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["account_no"];


		bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
			coll.find_one(bsoncxx::builder::stream::document{} << "current_no" << "current_no" << finalize);
		if (maybe_result) {
			std::string message = bsoncxx::to_json(*maybe_result);
			auto json_object = json::parse(message);
			int first = json_object["first"];
			int second = json_object["second"];
			int last = json_object["last"];
			return std::make_tuple(first, second, last);
		}
		else {
			return std::make_tuple(100, 1000, 1000);
		}

	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}

	return std::make_tuple(100, 1000, 1000);
}

void SmMongoDBManager::SaveUserInfo(std::string user_id, std::string pwd)
{
	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["user_list"];

		builder::stream::document builder{};

		bsoncxx::stdx::optional<bsoncxx::document::value> found_user =
			coll.find_one(bsoncxx::builder::stream::document{} << "user_id" << user_id << finalize);
		if (found_user) {
			coll.update_one(bsoncxx::builder::stream::document{} << "user_id" << user_id << finalize,
				bsoncxx::builder::stream::document{} << "$set"
				<< open_document
				<< "password" << pwd
				<< close_document << finalize);
		}
		else {
			bsoncxx::document::value doc_value = builder
				<< "user_id" << user_id
				<< "password" << pwd
				<< bsoncxx::builder::stream::finalize;
			auto res = db["user_list"].insert_one(std::move(doc_value));
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

std::pair<std::string, std::string> SmMongoDBManager::GetUserInfo(std::string user_id)
{
	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["user_list"];


		bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
			coll.find_one(bsoncxx::builder::stream::document{} << "user_id" << user_id << finalize);
		if (maybe_result) {
			std::string message = bsoncxx::to_json(*maybe_result);
			auto json_object = json::parse(message);
			std::string pwd = json_object["password"];
			return std::make_pair(user_id, pwd);
		}
		else {
			return std::make_pair("", "");
		}

	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}

	return std::make_pair("", "");
}

bool SmMongoDBManager::RemoveUserInfo(std::string user_id)
{
	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["user_list"];


		bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
			coll.find_one(bsoncxx::builder::stream::document{} << "user_id" << user_id << finalize);
		if (maybe_result) {
			coll.delete_one(bsoncxx::builder::stream::document{} << "user_id" << user_id << finalize);
			return true;
		}
		else {
			return false;
		}

	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}

	return false;
}


void SmMongoDBManager::SaveAccountInfo(std::shared_ptr<SmAccount> acnt)
{
	if (!acnt)
		return;

	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["account_list"];

		builder::stream::document builder{};

		bsoncxx::stdx::optional<bsoncxx::document::value> fount_acnt =
			coll.find_one(bsoncxx::builder::stream::document{} << "account_no" << acnt->AccountNo() << finalize);
		if (fount_acnt) {
			coll.update_one(bsoncxx::builder::stream::document{} << "account_no" << acnt->AccountNo() << finalize,
				bsoncxx::builder::stream::document{} << "$set"
				<< open_document
				<< "account_no" << acnt->AccountNo()
				<< "user_id" << acnt->UserID()
				<< "account_name" << acnt->AccountName()
				<< "password" << acnt->Password()
				<< "initial_balance" << acnt->InitialBalance()
				<< "trade_profit_loss" << acnt->TradePL()
				<< "open_profit_loss" << acnt->OpenPL()
				<< close_document << finalize);
		}
		else {
			bsoncxx::document::value doc_value = builder
				<< "account_no" << acnt->AccountNo()
				<< "user_id" << acnt->UserID()
				<< "account_name" << acnt->AccountName()
				<< "password" << acnt->Password()
				<< "initial_balance" << acnt->InitialBalance()
				<< "trade_profit_loss" << acnt->TradePL()
				<< "open_profit_loss" << acnt->OpenPL()
				<< bsoncxx::builder::stream::finalize;
			auto res = db["account_list"].insert_one(std::move(doc_value));
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

// 매매수익과 평가 손익을 업데이트 한다.
void SmMongoDBManager::UpdateAccountInfo(std::shared_ptr<SmAccount> acnt)
{
	if (!acnt)
		return;
	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["account_list"];

		builder::stream::document builder{};

		bsoncxx::stdx::optional<bsoncxx::document::value> fount_acnt =
			coll.find_one(bsoncxx::builder::stream::document{} 
				<< "account_no" << acnt->AccountNo() 
				<< finalize);
		if (fount_acnt) {
			coll.update_one(bsoncxx::builder::stream::document{} << "account_no" << acnt->AccountNo() << finalize,
				bsoncxx::builder::stream::document{} << "$set"
				<< open_document
				<< "trade_profit_loss" << acnt->TradePL()
				<< "open_profit_loss" << acnt->OpenPL()
				<< close_document << finalize);
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

std::vector<std::shared_ptr<SmAccount>> SmMongoDBManager::GetAccountList(std::string user_id)
{
	std::vector<std::shared_ptr<SmAccount>> account_list;

	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["account_list"];

		builder::stream::document builder{};

		mongocxx::cursor cursor = coll.find(bsoncxx::builder::stream::document{} 
			<< "user_id" << user_id << finalize);
		for (auto doc : cursor) {
			std::string message = bsoncxx::to_json(doc);
			auto json_object = json::parse(message);
			std::shared_ptr<SmAccount> acnt = std::make_shared<SmAccount>();
			std::string account_no = json_object["account_no"];
			std::string user_id = json_object["user_id"];
			std::string account_name = json_object["account_name"];
			std::string password = json_object["password"];
			double initial_valance = json_object["initial_balance"];
			double trade_profit_loss = json_object["trade_profit_loss"];
			double open_profit_loss = json_object["open_profit_loss"];
			acnt->AccountNo(account_no);
			acnt->AccountName(account_name);
			acnt->UserID(user_id);
			acnt->Password(password);
			acnt->InitialBalance(initial_valance);
			acnt->TradePL(trade_profit_loss);
			acnt->OpenPL(open_profit_loss);
			account_list.push_back(acnt);
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}

	return account_list;
}

std::shared_ptr<SmAccount> SmMongoDBManager::GetAccount(std::string account_no)
{

	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["account_list"];

		builder::stream::document builder{};

		mongocxx::cursor cursor = coll.find(bsoncxx::builder::stream::document{}
		<< "account_no" << account_no << finalize);
		for (auto doc : cursor) {
			std::string message = bsoncxx::to_json(doc);
			auto json_object = json::parse(message);
			std::shared_ptr<SmAccount> acnt = std::make_shared<SmAccount>();
			std::string account_no = json_object["account_no"];
			std::string user_id = json_object["user_id"];
			std::string account_name = json_object["account_name"];
			std::string password = json_object["password"];
			double initial_valance = json_object["initial_balance"];
			double trade_profit_loss = json_object["trade_profit_loss"];
			double open_profit_loss = json_object["open_profit_loss"];
			acnt->AccountNo(account_no);
			acnt->AccountName(account_name);
			acnt->UserID(user_id);
			acnt->Password(password);
			acnt->InitialBalance(initial_valance);
			acnt->TradePL(trade_profit_loss);
			acnt->OpenPL(open_profit_loss);
			return acnt;
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::AddPosition(std::shared_ptr<SmPosition> posi)
{
	if (!posi)
		return;

	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["position_list"];

		builder::stream::document builder{};

		bsoncxx::stdx::optional<bsoncxx::document::value> found_posi =
			coll.find_one(bsoncxx::builder::stream::document{} 
				<< "account_no" << posi->AccountNo 
				<< "symbol_code" << posi->SymbolCode
				<< finalize);
		if (found_posi) {
			coll.update_one(bsoncxx::builder::stream::document{}
				<< "account_no" << posi->AccountNo
				<< "symbol_code" << posi->SymbolCode
				<< finalize,
				bsoncxx::builder::stream::document{} << "$set"
				<< open_document
				<< "created_date" << posi->CreatedDate
				<< "created_time" << posi->CreatedTime
				<< "account_no" << posi->AccountNo
				<< "symbol_code" << posi->SymbolCode
				<< "position_type" << (int)posi->Position
				<< "open_qty" << posi->OpenQty
				<< "fee" << posi->Fee
				<< "trade_profitloss" << posi->TradePL
				<< "average_price" << posi->AvgPrice
				<< "current_price" << posi->CurPrice
				<< "open_profitloss" << posi->OpenPL
				<< close_document << finalize);
		}
		else {
			bsoncxx::document::value doc_value = builder
				<< "created_date" << posi->CreatedDate
				<< "created_time" << posi->CreatedTime
				<< "account_no" << posi->AccountNo
				<< "symbol_code" << posi->SymbolCode
				<< "position_type" << (int)posi->Position
				<< "open_qty" << posi->OpenQty
				<< "fee" << posi->Fee
				<< "trade_profitloss" << posi->TradePL
				<< "average_price" << posi->AvgPrice
				<< "current_price" << posi->CurPrice
				<< "open_profitloss" << posi->OpenPL
				<< bsoncxx::builder::stream::finalize;
			auto res = db["position_list"].insert_one(std::move(doc_value));
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::UpdatePosition(std::shared_ptr<SmPosition> posi)
{
	if (!posi)
		return;

	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["position_list"];

		builder::stream::document builder{};

		bsoncxx::stdx::optional<bsoncxx::document::value> found_posi =
			coll.find_one(bsoncxx::builder::stream::document{}
				<< "account_no" << posi->AccountNo
				<< "symbol_code" << posi->SymbolCode
				<< finalize);
		if (found_posi) {
			// 잔고가 있을 때는 포지션을 업데이트 한다.
			if (posi->OpenQty != 0) {
				coll.update_one(bsoncxx::builder::stream::document{}
					<< "account_no" << posi->AccountNo
					<< "symbol_code" << posi->SymbolCode
					<< finalize,
					bsoncxx::builder::stream::document{} << "$set"
					<< open_document
					<< "created_date" << posi->CreatedDate
					<< "created_time" << posi->CreatedTime
					<< "account_no" << posi->AccountNo
					<< "symbol_code" << posi->SymbolCode
					<< "position_type" << (int)posi->Position
					<< "open_qty" << posi->OpenQty
					<< "fee" << posi->Fee
					<< "trade_profitloss" << posi->TradePL
					<< "average_price" << posi->AvgPrice
					<< "current_price" << posi->CurPrice
					<< "open_profitloss" << posi->OpenPL
					<< close_document << finalize);
			}
			else { // 잔고가 없을 때는 포지션을 없앤다.
				coll.delete_one(bsoncxx::builder::stream::document{} 
					<< "account_no" << posi->AccountNo
					<< "symbol_code" << posi->SymbolCode
					<< finalize);
			}
		}
		else { // 포지션이 없을 때는 삽입해 준다.
			bsoncxx::document::value doc_value = builder
				<< "created_date" << posi->CreatedDate
				<< "created_time" << posi->CreatedTime
				<< "account_no" << posi->AccountNo
				<< "symbol_code" << posi->SymbolCode
				<< "position_type" << (int)posi->Position
				<< "open_qty" << posi->OpenQty
				<< "fee" << posi->Fee
				<< "trade_profitloss" << posi->TradePL
				<< "average_price" << posi->AvgPrice
				<< "current_price" << posi->CurPrice
				<< "open_profitloss" << posi->OpenPL
				<< bsoncxx::builder::stream::finalize;
			auto res = db["position_list"].insert_one(std::move(doc_value));
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}


void SmMongoDBManager::AddOrder(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;

	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["order_list"];

		builder::stream::document builder{};
		// 주문은 날짜와 주문번호로 구분을 한다.
		// 날짜가 변경되면 주문은 다시 시작된다.
		bsoncxx::stdx::optional<bsoncxx::document::value> found_order =
			coll.find_one(bsoncxx::builder::stream::document{}
				<< "order_date" << order->OrderDate
				<< "order_no" << order->OrderNo
				<< finalize);
		if (found_order) {
			coll.update_one(bsoncxx::builder::stream::document{}
				<< "order_date" << order->OrderDate
				<< "order_no" << order->OrderNo
				<< finalize,
				bsoncxx::builder::stream::document{} << "$set"
				<< open_document
				<< "order_date" << order->OrderDate
				<< "order_time" << order->OrderTime
				<< "order_no" << order->OrderNo
				<< "account_no" << order->AccountNo
				<< "symbol_code" << order->SymbolCode
				<< "order_amount" << order->OrderAmount
				<< "price_type" << (int)order->PriceType
				<< "order_price" << order->OrderPrice
				<< "position_type" << (int)order->Position
				<< "order_type" << (int)order->OrderType
				<< "ori_order_no" << order->OriOrderNo
				<< "filled_date" << order->FilledDate
				<< "filled_time" << order->FilledTime
				<< "filled_condition" << (int)order->FilledCondition
				<< "filled_price" << order->FilledPrice
				<< "filled_qty" << order->FilledQty
				<< "order_state" << (int)order->OrderState
				<< "symbol_decimal" << order->SymbolDecimal 
				<< "remain_qty" << order->RemainQty
				<< "strategy_name" << order->StrategyName
				<< "system_name" << order->SystemName
				<< "fund_name" << order->FundName
				<< close_document << finalize);
		}
		else {
			bsoncxx::document::value doc_value = builder
				<< "order_date" << order->OrderDate
				<< "order_time" << order->OrderTime
				<< "order_no" << order->OrderNo
				<< "account_no" << order->AccountNo
				<< "symbol_code" << order->SymbolCode
				<< "order_amount" << order->OrderAmount
				<< "price_type" << (int)order->PriceType
				<< "order_price" << order->OrderPrice
				<< "position_type" << (int)order->Position
				<< "order_type" << (int)order->OrderType
				<< "ori_order_no" << order->OriOrderNo
				<< "filled_date" << order->FilledDate
				<< "filled_time" << order->FilledTime
				<< "filled_condition" << (int)order->FilledCondition
				<< "filled_price" << order->FilledPrice
				<< "filled_qty" << order->FilledQty
				<< "order_state" << (int)order->OrderState
				<< "symbol_decimal" << order->SymbolDecimal
				<< "remain_qty" << order->RemainQty
				<< "strategy_name" << order->StrategyName
				<< "system_name" << order->SystemName
				<< "fund_name" << order->FundName
				<< bsoncxx::builder::stream::finalize;
			auto res = db["order_list"].insert_one(std::move(doc_value));
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::OnAcceptedOrder(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;

	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["order_list"];

		builder::stream::document builder{};
		// 주문은 날짜와 주문번호로 구분을 한다.
		// 날짜가 변경되면 주문은 다시 시작된다.
		bsoncxx::stdx::optional<bsoncxx::document::value> found_order =
			coll.find_one(bsoncxx::builder::stream::document{}
				<< "order_date" << order->OrderDate
				<< "order_no" << order->OrderNo
				<< finalize);
		if (found_order) {
			coll.update_one(bsoncxx::builder::stream::document{}
				<< "order_date" << order->OrderDate
				<< "order_no" << order->OrderNo
				<< finalize,
				bsoncxx::builder::stream::document{} << "$set"
				<< open_document
				<< "order_state" << (int)order->OrderState
				<< close_document << finalize);
		}
		
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::OnFilledOrder(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;

	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["order_list"];

		builder::stream::document builder{};
		// 주문은 날짜와 주문번호로 구분을 한다.
		// 날짜가 변경되면 주문은 다시 시작된다.
		bsoncxx::stdx::optional<bsoncxx::document::value> found_order =
			coll.find_one(bsoncxx::builder::stream::document{}
				<< "order_date" << order->OrderDate
				<< "order_no" << order->OrderNo
				<< finalize);
		if (found_order) {
			coll.update_one(bsoncxx::builder::stream::document{}
				<< "order_date" << order->OrderDate
				<< "order_no" << order->OrderNo
				<< finalize,
				bsoncxx::builder::stream::document{} << "$set"
				<< open_document
				<< "filled_date" << order->FilledDate
				<< "filled_time" << order->FilledTime
				<< "filled_price" << order->FilledPrice
				<< "filled_qty" << order->FilledQty
				<< "remain_qty" << order->RemainQty
				<< "order_state" << (int)order->OrderState
				<< close_document << finalize);
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::ChangeOrderState(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;

	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["order_list"];

		builder::stream::document builder{};
		// 주문은 날짜와 주문번호로 구분을 한다.
		// 날짜가 변경되면 주문은 다시 시작된다.
		bsoncxx::stdx::optional<bsoncxx::document::value> found_order =
			coll.find_one(bsoncxx::builder::stream::document{}
				<< "order_date" << order->OrderDate
				<< "order_no" << order->OrderNo
				<< finalize);
		if (found_order) {
			coll.update_one(bsoncxx::builder::stream::document{}
				<< "order_date" << order->OrderDate
				<< "order_no" << order->OrderNo
				<< finalize,
				bsoncxx::builder::stream::document{} << "$set"
				<< open_document
				<< "order_state" << (int)order->OrderState
				<< "remain_qty" << order->RemainQty
				<< close_document << finalize);
		}

	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

std::vector<std::shared_ptr<SmOrder>> SmMongoDBManager::GetAcceptedOrderList(std::string account_no)
{
	std::vector<std::shared_ptr<SmOrder>> order_list;

	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["order_list"];

		builder::stream::document builder{};

		// 현재 날짜에 해당하는 것만 가져온다.
		mongocxx::cursor cursor = coll.find(bsoncxx::builder::stream::document{}
			<< "account_no" << account_no
			<< "order_state" << (int)SmOrderState::Accepted
			<< finalize);
		for (auto doc : cursor) {
			std::string message = bsoncxx::to_json(doc);
			auto json_object = json::parse(message);
			std::shared_ptr<SmOrder> order = std::make_shared<SmOrder>();

			order->AccountNo = json_object["account_no"];
			order->OrderType = json_object["order_type"];
			order->Position = json_object["position_type"];
			order->PriceType = json_object["price_type"];
			order->SymbolCode = json_object["symbol_code"];
			order->OrderPrice = json_object["order_price"];
			order->OrderNo = json_object["order_no"];
			order->OrderAmount = json_object["order_amount"];
			order->OriOrderNo = json_object["ori_order_no"];
			order->FilledDate = json_object["filled_date"];
			order->FilledTime = json_object["filled_time"];
			order->OrderDate = json_object["order_date"];
			order->OrderTime = json_object["order_time"];
			order->FilledQty = json_object["filled_qty"];
			order->FilledPrice = json_object["filled_price"];
			order->OrderState = json_object["order_state"];
			order->FilledCondition = json_object["filled_condition"];
			order->SymbolDecimal = json_object["symbol_decimal"];
			order->RemainQty = json_object["remain_qty"];
			order->StrategyName = json_object["strategy_name"];
			order->SystemName = json_object["system_name"];
			order->FundName = json_object["fund_name"];

			order_list.push_back(order);
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}

	return order_list;
}

std::vector<std::shared_ptr<SmOrder>> SmMongoDBManager::GetFilledOrderList(std::string account_no)
{
	std::vector<std::shared_ptr<SmOrder>> order_list;

	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["order_list"];

		builder::stream::document builder{};

		// 현재 날짜에 해당하는 것만 가져온다.
		mongocxx::cursor cursor = coll.find(bsoncxx::builder::stream::document{}
			<< "account_no" << account_no
			<< "order_state" << (int)SmOrderState::Filled
			<< finalize);
		for (auto doc : cursor) {
			std::string message = bsoncxx::to_json(doc);
			auto json_object = json::parse(message);
			std::shared_ptr<SmOrder> order = std::make_shared<SmOrder>();

			order->AccountNo = json_object["account_no"];
			order->OrderType = json_object["order_type"];
			order->Position = json_object["position_type"];
			order->PriceType = json_object["price_type"];
			order->SymbolCode = json_object["symbol_code"];
			order->OrderPrice = json_object["order_price"];
			order->OrderNo = json_object["order_no"];
			order->OrderAmount = json_object["order_amount"];
			order->OriOrderNo = json_object["ori_order_no"];
			order->FilledDate = json_object["filled_date"];
			order->FilledTime = json_object["filled_time"];
			order->OrderDate = json_object["order_date"];
			order->OrderTime = json_object["order_time"];
			order->FilledQty = json_object["filled_qty"];
			order->FilledPrice = json_object["filled_price"];
			order->OrderState = json_object["order_state"];
			order->FilledCondition = json_object["filled_condition"];
			order->SymbolDecimal = json_object["symbol_decimal"];
			order->RemainQty = json_object["remain_qty"];
			order->StrategyName = json_object["strategy_name"];
			order->SystemName = json_object["system_name"];
			order->FundName = json_object["fund_name"];

			order_list.push_back(order);
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}

	return order_list;
}

std::vector<std::shared_ptr<SmOrder>> SmMongoDBManager::GetOrderList(std::string account_no)
{
	std::vector<std::shared_ptr<SmOrder>> order_list;

	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["order_list"];

		builder::stream::document builder{};

		std::pair<std::string, std::string> date_time = VtStringUtil::GetCurrentDateTime();
		// 현재 날짜에 해당하는 것만 가져온다.
		mongocxx::cursor cursor = coll.find(bsoncxx::builder::stream::document{}
			<< "order_date" << date_time.first
			<< "account_no" << account_no
			<< finalize);
		for (auto doc : cursor) {
			std::string message = bsoncxx::to_json(doc);
			auto json_object = json::parse(message);
			std::shared_ptr<SmOrder> order = std::make_shared<SmOrder>();

			order->AccountNo = json_object["account_no"];
			order->OrderType = json_object["order_type"];
			order->Position = json_object["position_type"];
			order->PriceType = json_object["price_type"];
			order->SymbolCode = json_object["symbol_code"];
			order->OrderPrice = json_object["order_price"];
			order->OrderNo = json_object["order_no"];
			order->OrderAmount = json_object["order_amount"];
			order->OriOrderNo = json_object["ori_order_no"];
			order->FilledDate = json_object["filled_date"];
			order->FilledTime = json_object["filled_time"];
			order->OrderDate = json_object["order_date"];
			order->OrderTime = json_object["order_time"];
			order->FilledQty = json_object["filled_qty"];
			order->FilledPrice = json_object["filled_price"];
			order->OrderState = json_object["order_state"];
			order->FilledCondition = json_object["filled_condition"];
			order->SymbolDecimal = json_object["symbol_decimal"];
			order->RemainQty = json_object["remain_qty"];
			order->StrategyName = json_object["strategy_name"];
			order->SystemName = json_object["system_name"];
			order->FundName = json_object["fund_name"];
			
			order_list.push_back(order);
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}

	return order_list;
}

std::vector<std::shared_ptr<SmOrder>> SmMongoDBManager::GetOrderList(std::string date, std::string account_no)
{
	std::vector<std::shared_ptr<SmOrder>> order_list;

	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["order_list"];

		builder::stream::document builder{};

		// 현재 날짜에 해당하는 것만 가져온다.
		mongocxx::cursor cursor = coll.find(bsoncxx::builder::stream::document{}
			<< "order_date" << date
			<< "account_no" << account_no
			<< finalize);
		for (auto doc : cursor) {
			std::string message = bsoncxx::to_json(doc);
			auto json_object = json::parse(message);
			std::shared_ptr<SmOrder> order = std::make_shared<SmOrder>();

			order->AccountNo = json_object["account_no"];
			order->OrderType = json_object["order_type"];
			order->Position = json_object["position_type"];
			order->PriceType = json_object["price_type"];
			order->SymbolCode = json_object["symbol_code"];
			order->OrderPrice = json_object["order_price"];
			order->OrderNo = json_object["order_no"];
			order->OrderAmount = json_object["order_amount"];
			order->OriOrderNo = json_object["ori_order_no"];
			order->FilledDate = json_object["filled_date"];
			order->FilledTime = json_object["filled_time"];
			order->OrderDate = json_object["order_date"];
			order->OrderTime = json_object["order_time"];
			order->FilledQty = json_object["filled_qty"];
			order->FilledPrice = json_object["filled_price"];
			order->OrderState = json_object["order_state"];
			order->FilledCondition = json_object["filled_condition"];
			order->SymbolDecimal = json_object["symbol_decimal"];
			order->RemainQty = json_object["remain_qty"];
			order->StrategyName = json_object["strategy_name"];
			order->SystemName = json_object["system_name"];
			order->FundName = json_object["fund_name"];

			order_list.push_back(order);
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}

	return order_list;
}

std::vector<std::shared_ptr<SmOrder>> SmMongoDBManager::GetOrderList(std::string account_no, int count, int skip)
{
	std::vector<std::shared_ptr<SmOrder>> order_list;

	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["order_list"];

		// @begin: cpp-query-sort
		mongocxx::options::find opts;
		// 최신것이 앞에 오도록 한다.
		opts.sort(make_document(kvp("order_no", -1)));
		// 과거의 것이 앞에 오도록 한다.
		//opts.sort(make_document(kvp("date_time", 1)));
		opts.limit(count);
		opts.skip(skip);

		builder::stream::document builder{};

		// 현재 날짜에 해당하는 것만 가져온다.
		mongocxx::cursor cursor = coll.find(
			bsoncxx::builder::stream::document{}
			<< "account_no" << account_no
			<< finalize, opts);
		for (auto doc : cursor) {
			std::string message = bsoncxx::to_json(doc);
			auto json_object = json::parse(message);
			std::shared_ptr<SmOrder> order = std::make_shared<SmOrder>();

			order->AccountNo = json_object["account_no"];
			order->OrderType = json_object["order_type"];
			order->Position = json_object["position_type"];
			order->PriceType = json_object["price_type"];
			order->SymbolCode = json_object["symbol_code"];
			order->OrderPrice = json_object["order_price"];
			order->OrderNo = json_object["order_no"];
			order->OrderAmount = json_object["order_amount"];
			order->OriOrderNo = json_object["ori_order_no"];
			order->FilledDate = json_object["filled_date"];
			order->FilledTime = json_object["filled_time"];
			order->OrderDate = json_object["order_date"];
			order->OrderTime = json_object["order_time"];
			order->FilledQty = json_object["filled_qty"];
			order->FilledPrice = json_object["filled_price"];
			order->OrderState = json_object["order_state"];
			order->FilledCondition = json_object["filled_condition"];
			order->SymbolDecimal = json_object["symbol_decimal"];
			order->RemainQty = json_object["remain_qty"];
			order->StrategyName = json_object["strategy_name"];
			order->SystemName = json_object["system_name"];
			order->FundName = json_object["fund_name"];

			order_list.push_back(order);
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}

	return order_list;
}

std::vector<std::shared_ptr<SmPosition>> SmMongoDBManager::GetPositionList(std::string account_no)
{
	std::vector<std::shared_ptr<SmPosition>> posi_list;

	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["position_list"];

		builder::stream::document builder{};

		std::pair<std::string, std::string> date_time = VtStringUtil::GetCurrentDateTime();
		// 현재 날짜에 해당하는 것만 가져온다.
		mongocxx::cursor cursor = coll.find(bsoncxx::builder::stream::document{}
			<< "created_date" << date_time.first
			<< "account_no" << account_no
			<< finalize);
		for (auto doc : cursor) {
			std::string message = bsoncxx::to_json(doc);
			auto json_object = json::parse(message);
			std::shared_ptr<SmPosition> posi = std::make_shared<SmPosition>();

			posi->CreatedDate = json_object["created_date"];
			posi->CreatedTime = json_object["created_time"];
			posi->SymbolCode = json_object["symbol_code"];
			posi->AccountNo = json_object["account_no"];
			posi->Position = json_object["position_type"];
			posi->OpenQty = json_object["open_qty"];
			posi->Fee =json_object["fee"];
			posi->TradePL = json_object["trade_profitloss"];
			posi->AvgPrice = json_object["average_price"];
			posi->CurPrice = json_object["current_price"];
			posi->OpenPL = json_object["open_profitloss"];
			
			posi_list.push_back(posi);
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}

	return posi_list;
}

std::vector<std::shared_ptr<SmPosition>> SmMongoDBManager::GetPositionList(std::string date, std::string account_no)
{
	std::vector<std::shared_ptr<SmPosition>> posi_list;

	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["position_list"];

		builder::stream::document builder{};

		// 현재 날짜에 해당하는 것만 가져온다.
		mongocxx::cursor cursor = coll.find(bsoncxx::builder::stream::document{}
			<< "created_date" << date
			<< "account_no" << account_no
			<< finalize);
		for (auto doc : cursor) {
			std::string message = bsoncxx::to_json(doc);
			auto json_object = json::parse(message);
			std::shared_ptr<SmPosition> posi = std::make_shared<SmPosition>();

			posi->CreatedDate = json_object["created_date"];
			posi->CreatedTime = json_object["created_time"];
			posi->SymbolCode = json_object["symbol_code"];
			posi->AccountNo = json_object["account_no"];
			posi->Position = json_object["position_type"];
			posi->OpenQty = json_object["open_qty"];
			posi->Fee = json_object["fee"];
			posi->TradePL = json_object["trade_profitloss"];
			posi->AvgPrice = json_object["average_price"];
			posi->CurPrice = json_object["current_price"];
			posi->OpenPL = json_object["open_profitloss"];

			posi_list.push_back(posi);
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}

	return posi_list;
}

std::shared_ptr<SmPosition> SmMongoDBManager::GetPosition(std::string account_no, std::string symbol_code)
{
	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["position_list"];

		builder::stream::document builder{};

		std::pair<std::string, std::string> date_time = VtStringUtil::GetCurrentDateTime();
		// 현재 날짜에 해당하는 것만 가져온다.
		mongocxx::cursor cursor = coll.find(bsoncxx::builder::stream::document{}
			<< "created_date" << date_time.first
			<< "account_no" << account_no
			<< "symbol_code" << symbol_code
			<< finalize);
		for (auto doc : cursor) {
			std::string message = bsoncxx::to_json(doc);
			auto json_object = json::parse(message);
			std::shared_ptr<SmPosition> posi = std::make_shared<SmPosition>();

			posi->CreatedDate = json_object["created_date"];
			posi->CreatedTime = json_object["created_time"];
			posi->SymbolCode = json_object["symbol_code"];
			posi->AccountNo = json_object["account_no"];
			posi->Position = json_object["position_type"];
			posi->OpenQty = json_object["open_qty"];
			posi->Fee = json_object["fee"];
			posi->TradePL = json_object["trade_profitloss"];
			posi->AvgPrice = json_object["average_price"];
			posi->CurPrice = json_object["current_price"];
			posi->OpenPL = json_object["open_profitloss"];

			return posi;
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

std::shared_ptr<SmPosition> SmMongoDBManager::GetPosition(std::string date, std::string account_no, std::string symbol_code)
{
	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["position_list"];

		builder::stream::document builder{};

		// 현재 날짜에 해당하는 것만 가져온다.
		mongocxx::cursor cursor = coll.find(bsoncxx::builder::stream::document{}
			<< "created_date" << date
			<< "account_no" << account_no
			<< "symbol_code" << symbol_code
			<< finalize);
		for (auto doc : cursor) {
			std::string message = bsoncxx::to_json(doc);
			auto json_object = json::parse(message);
			std::shared_ptr<SmPosition> posi = std::make_shared<SmPosition>();

			posi->CreatedDate = json_object["created_date"];
			posi->CreatedTime = json_object["created_time"];
			posi->SymbolCode = json_object["symbol_code"];
			posi->AccountNo = json_object["account_no"];
			posi->Position = json_object["position_type"];
			posi->OpenQty = json_object["open_qty"];
			posi->Fee = json_object["fee"];
			posi->TradePL = json_object["trade_profitloss"];
			posi->AvgPrice = json_object["average_price"];
			posi->CurPrice = json_object["current_price"];
			posi->OpenPL = json_object["open_profitloss"];

			return posi;
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::LoadAccountList()
{
	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["account_list"];

		SmAccountManager* acntMgr = SmAccountManager::GetInstance();

		builder::stream::document builder{};

		mongocxx::cursor cursor = coll.find({});
		for (auto doc : cursor) {
			std::string message = bsoncxx::to_json(doc);
			auto json_object = json::parse(message);
			std::shared_ptr<SmAccount> acnt = std::make_shared<SmAccount>();
			std::string account_no = json_object["account_no"];
			std::string user_id = json_object["user_id"];
			std::string account_name = json_object["account_name"];
			std::string password = json_object["password"];
			double initial_valance = json_object["initial_valance"];
			double trade_profit_loss = json_object["trade_profit_loss"];
			double open_profit_loss = json_object["open_profit_loss"];
			acnt->AccountNo(account_no);
			acnt->AccountName(account_name);
			acnt->UserID(user_id);
			acnt->Password(password);
			acnt->InitialBalance(initial_valance);
			acnt->TradePL(trade_profit_loss);
			acnt->OpenPL(open_profit_loss);
			acntMgr->AddAccount(acnt);
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::LoadFilledOrderList()
{
	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["order_list"];

		builder::stream::document builder{};

		SmTotalOrderManager* totalOrderMgr = SmTotalOrderManager::GetInstance();

		// 현재 날짜에 해당하는 것만 가져온다.
		mongocxx::cursor cursor = coll.find(bsoncxx::builder::stream::document{}
			<< "order_state" << (int)SmOrderState::Filled
			<< finalize);
		for (auto doc : cursor) {
			std::string message = bsoncxx::to_json(doc);
			auto json_object = json::parse(message);
			std::shared_ptr<SmOrder> order = std::make_shared<SmOrder>();

			order->AccountNo = json_object["account_no"];
			order->OrderType = json_object["order_type"];
			order->Position = json_object["position_type"];
			order->PriceType = json_object["price_type"];
			order->SymbolCode = json_object["symbol_code"];
			order->OrderPrice = json_object["order_price"];
			order->OrderNo = json_object["order_no"];
			SmOrderNumberGenerator::SetID(order->OrderNo);
			order->OrderAmount = json_object["order_amount"];
			order->OriOrderNo = json_object["ori_order_no"];
			order->FilledDate = json_object["filled_date"];
			order->FilledTime = json_object["filled_time"];
			order->OrderDate = json_object["order_date"];
			order->OrderTime = json_object["order_time"];
			order->FilledQty = json_object["filled_qty"];
			order->FilledPrice = json_object["filled_price"];
			order->OrderState = json_object["order_state"];
			order->FilledCondition = json_object["filled_condition"];
			order->SymbolDecimal = json_object["symbol_decimal"];
			order->RemainQty = json_object["remain_qty"];
			order->StrategyName = json_object["strategy_name"];
			order->SystemName = json_object["system_name"];
			order->FundName = json_object["fund_name"];

			totalOrderMgr->AddFilledOrder(order);
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::LoadAcceptedOrderList()
{
	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["order_list"];

		builder::stream::document builder{};

		SmTotalOrderManager* totalOrderMgr = SmTotalOrderManager::GetInstance();

		// 현재 날짜에 해당하는 것만 가져온다.
		mongocxx::cursor cursor = coll.find(bsoncxx::builder::stream::document{}
			<< "order_state" << (int)SmOrderState::Accepted
			<< finalize);
		for (auto doc : cursor) {
			std::string message = bsoncxx::to_json(doc);
			auto json_object = json::parse(message);
			std::shared_ptr<SmOrder> order = std::make_shared<SmOrder>();

			order->AccountNo = json_object["account_no"];
			order->OrderType = json_object["order_type"];
			order->Position = json_object["position_type"];
			order->PriceType = json_object["price_type"];
			order->SymbolCode = json_object["symbol_code"];
			order->OrderPrice = json_object["order_price"];
			order->OrderNo = json_object["order_no"];
			SmOrderNumberGenerator::SetID(order->OrderNo);
			order->OrderAmount = json_object["order_amount"];
			order->OriOrderNo = json_object["ori_order_no"];
			order->FilledDate = json_object["filled_date"];
			order->FilledTime = json_object["filled_time"];
			order->OrderDate = json_object["order_date"];
			order->OrderTime = json_object["order_time"];
			order->FilledQty = json_object["filled_qty"];
			order->FilledPrice = json_object["filled_price"];
			order->OrderState = json_object["order_state"];
			order->FilledCondition = json_object["filled_condition"];
			order->SymbolDecimal = json_object["symbol_decimal"];
			order->RemainQty = json_object["remain_qty"];
			order->StrategyName = json_object["strategy_name"];
			order->SystemName = json_object["system_name"];
			order->FundName = json_object["fund_name"];

			totalOrderMgr->AddAcceptedOrder(order);
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::LoadPositionList()
{
	try
	{
		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		mongocxx::collection coll = db["position_list"];

		builder::stream::document builder{};

		std::pair<std::string, std::string> date_time = VtStringUtil::GetCurrentDateTime();

		SmTotalPositionManager* tpMgr = SmTotalPositionManager::GetInstance();
		
		// 잔고가 0보다 큰 모든 포지션을 가져온다.
		mongocxx::cursor cursor = coll.find(
			bsoncxx::builder::stream::document{} << "i" << open_document <<
			"$gt" << 0
			<< close_document << finalize);
		for (auto doc : cursor) {
			std::string message = bsoncxx::to_json(doc);
			auto json_object = json::parse(message);
			std::shared_ptr<SmPosition> posi = std::make_shared<SmPosition>();

			posi->CreatedDate = json_object["created_date"];
			posi->CreatedTime = json_object["created_time"];
			posi->SymbolCode = json_object["symbol_code"];
			posi->FundName = json_object["fund_name"];
			posi->AccountNo = json_object["account_no"];
			posi->Position = json_object["position_type"];
			posi->OpenQty = json_object["open_qty"];
			posi->Fee = json_object["fee"];
			posi->TradePL = json_object["trade_pl"];
			posi->AvgPrice = json_object["avg_price"];
			posi->CurPrice = json_object["cur_price"];
			posi->OpenPL = json_object["open_pl"];

			tpMgr->AddPosition(posi);
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::SaveChartDataRequest(SmChartDataRequest req)
{
	try
	{
		//std::lock_guard<std::mutex> lock(_mutex);

		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		// 먼저 시장이 있는지 검색한다. 
		// 그리고 시장 속에 상품이 있는지 검색한다.
		mongocxx::collection coll = db["chart_data_request"];

		builder::stream::document builder{};

		bsoncxx::stdx::optional<bsoncxx::document::value> fount_no =
			coll.find_one(bsoncxx::builder::stream::document{} << "symbol_code" << req.symbolCode << "chart_type" << (int)req.chartType << "chart_cycle" << req.cycle << finalize);
		if (!fount_no)
		{
			bsoncxx::document::value doc_value = builder
				<< "symbol_code" << req.symbolCode
				<< "chart_type" << (int)req.chartType
				<< "chart_cycle" << req.cycle
				<< bsoncxx::builder::stream::finalize;
			auto res = db["chart_data_request"].insert_one(std::move(doc_value));
		}
	}
	catch (std::exception e) {
		std::string error;
		error = e.what();
	}
}

void SmMongoDBManager::SaveAccountNo(int first, int second, int last)
{
	try
	{
		//std::lock_guard<std::mutex> lock(_mutex);

		auto c = _ConnPool->acquire();

		auto db = (*c)["andromeda"];
		using namespace bsoncxx;

		// 먼저 시장이 있는지 검색한다. 
		// 그리고 시장 속에 상품이 있는지 검색한다.
		mongocxx::collection coll = db["account_no"];

		builder::stream::document builder{};

		bsoncxx::stdx::optional<bsoncxx::document::value> fount_no =
			coll.find_one(bsoncxx::builder::stream::document{} << "current_no" << "current_no" << finalize);
		if (fount_no) {
			coll.update_one(bsoncxx::builder::stream::document{} << "current_no" << "current_no" << finalize,
				bsoncxx::builder::stream::document{} << "$set"
				<< open_document
				<< "current_no" << "current_no"
				<< "first" << first
				<< "second" << second
				<< "last" << last
				<< close_document << finalize);
		}
		else {
			bsoncxx::document::value doc_value = builder
				<< "current_no" << "current_no"
				<< "first" << first
				<< "second" << second
				<< "last" << last
				<< bsoncxx::builder::stream::finalize;
			auto res = db["account_no"].insert_one(std::move(doc_value));
		}
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
				std::vector<std::shared_ptr<SmSymbol>>& sym_list = cat->GetSymbolList();
				for (size_t k = 0; k < sym_list.size(); ++k) {
					std::shared_ptr<SmSymbol> sym = sym_list[k];
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
	_URI = new mongocxx::uri{ "mongodb://localhost:27017/?minPoolSize=3&maxPoolSize=10" };
	_Client = new mongocxx::client(mongocxx::uri{});
	_ConnPool = new mongocxx::pool(mongocxx::uri{ "mongodb://localhost:27017/?minPoolSize=3&maxPoolSize=10" });
}
