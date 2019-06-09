#include "pch.h"
#include "SmTimeSeriesDBManager.h"
#include "Database/influxdb.hpp"
#include <ctime>
#include "Util/VtStringUtil.h"
#include <cstring> //memset
#include <sstream>
#include <iostream>
#include <algorithm>
#include <boost/asio.hpp>
#include "Json/json.hpp"
using namespace nlohmann;

namespace ip = boost::asio::ip;

SmTimeSeriesDBManager::SmTimeSeriesDBManager()
{
	std::vector<std::string> addr_vec = GetIPAddress("angelpie.ddns.net");
	_Ip = addr_vec[0];
	_Port = 8086;
	_DatabaseName = "abroad_future";
	_Id = "angelpie";
	_Password = "orion1";

	_ServerInfo = new influxdb_cpp::server_info(_Ip, _Port, _DatabaseName, _Id, _Password);
}

SmTimeSeriesDBManager::SmTimeSeriesDBManager(std::string ip, int port, std::string db, std::string id, std::string pwd)
{
	_Ip = ip;
	_Port = port;
	_DatabaseName = db;
	_Id = id;
	_Password = pwd;

	_ServerInfo = new influxdb_cpp::server_info(_Ip, _Port, _DatabaseName, _Id, _Password);
}

SmTimeSeriesDBManager::~SmTimeSeriesDBManager()
{
	if (_ServerInfo) {
		delete _ServerInfo;
		_ServerInfo = nullptr;
	}
}

std::string SmTimeSeriesDBManager::ExecQuery(std::string query_string)
{
	std::string resp;
	if (!_ServerInfo)
		return resp;
	influxdb_cpp::query(resp, query_string, *_ServerInfo);
	return resp;
}

void SmTimeSeriesDBManager::OnChartDataItem(SmChartDataItem&& data_item)
{
	std::string date_time = data_item.date + data_item.time;
	std::time_t utc = VtStringUtil::GetUTCTimestamp(date_time);
	std::string  meas = "chart_data";
	std::string resp;
	int ret = influxdb_cpp::builder()
		.meas(meas)
		.tag("symbol_code", data_item.symbolCode)
		.tag("chart_type", std::to_string((int)data_item.chartType))
		.tag("cycle", std::to_string(data_item.cycle))
		.field("h", data_item.h)
		.field("l", data_item.l)
		.field("o", data_item.o)
		.field("c", data_item.c)
		.field("v", data_item.v)
		.timestamp(utc * 1000000000)
		.post_http(*_ServerInfo, &resp);
}

void SmTimeSeriesDBManager::CreateDataBase(std::string db_name)
{
	std::string resp;
	if (!_ServerInfo)
		return;
	influxdb_cpp::create_db(resp, db_name, *_ServerInfo);
}

std::vector<std::string> SmTimeSeriesDBManager::GetIPAddress(std::string host_name)
{
	boost::asio::io_service io_service;
	ip::tcp::resolver resolver(io_service);

	std::string h = host_name;
	std::cout << "Host name is " << h << '\n';
	std::cout << "IP addresses are: \n";
	std::stringstream ss;
	std::vector<std::string> addr_vec;
	std::for_each(resolver.resolve({ h, "" }), {}, [ &ss, &addr_vec](const auto& re) {
		ss << re.endpoint().address();
		std::string addr = ss.str();
		addr_vec.push_back(addr);
		ss.str("");
		});
	return addr_vec;
}

void SmTimeSeriesDBManager::UserTest()
{
	std::string resp;
	
	std::time_t fixed_time = 1559941200;
	std::string id = "angelpie";
	std::string pwd = "orion1";
	std::string cert = "123456";
	int ret = influxdb_cpp::builder()
		.meas("user_info")
		.tag("id", id)
		.field("pwd", pwd)
		.field("cert", cert)
		.timestamp(fixed_time * 1000000000)
		.post_http(*_ServerInfo, &resp);
	
	

	std::string qry = "DROP MEASUREMENT \"user_info\"";
	//resp = ExecQuery(qry);
	// 태크가 있을 때는 반드시 태그외에 필드를 하나는 포함해야 한다.
	//The query selects the level description field, the location tag, and the water_level field. Note that the SELECT clause must specify at least one field when it includes a tag.
	//std::string query_string = "select \"tag_id\" from \"user_info\" where  \"tag_id\" = \'angelpie\'";
	std::string query_string = "select \"id\"  from \"user_info\" where  \"id\" = \'angelpie\'";
	resp = ExecQuery(query_string);

	std::string result = resp;

	auto json_object = json::parse(resp);
	auto values = json_object["results"][0]["series"];
	if (!values.is_null()) {
		auto columns = json_object["results"][0]["series"][0]["columns"];
		std::string str_pwd = values[0][1];
		auto b = values;
	}

	values = json_object["results"][0]["series"][0]["values"];
	std::string str_id = values[0][1];
}

void SmTimeSeriesDBManager::AddUserToDatabase(std::string id, std::string pwd)
{
	std::string resp;

	std::time_t fixed_time = 1559941200;
	//std::string id = "angelpie";
	//std::string pwd = "orion1";
	std::string cert = "123456";
	int ret = influxdb_cpp::builder()
		.meas("user_info")
		.tag("id", id)
		.field("pwd", pwd)
		.field("cert", cert)
		.timestamp(fixed_time * 1000000000)
		.post_http(*_ServerInfo, &resp);
}

std::pair<std::string, std::string> SmTimeSeriesDBManager::GetUserInfo(std::string id)
{
	// 태크가 있을 때는 반드시 태그외에 필드를 하나는 포함해야 한다.
	//The query selects the level description field, the location tag, and the water_level field. Note that the SELECT clause must specify at least one field when it includes a tag.
	//std::string query_string = "select \"tag_id\" from \"user_info\" where  \"tag_id\" = \'angelpie\'";
	std::string query_string = "select \"id\"  from \"user_info\" where  \"id\" = \'angelpie\'";
	std::string resp = ExecQuery(query_string);
	std::string str_id = "";
	std::string str_pwd = "";
	auto json_object = json::parse(resp);
	auto values = json_object["results"][0]["series"];
	if (values.is_null()) {
		return std::make_pair(str_id, str_pwd);
	}

	values = json_object["results"][0]["series"][0]["values"];
	str_id = values[0][1];
	str_pwd = values[0][2];
	return std::make_pair(str_id, str_pwd);
}

void SmTimeSeriesDBManager::GetChartData()
{
	SmTimeSeriesDBManager* dbMgr = SmTimeSeriesDBManager::GetInstance();
	std::string query_string = ""; // "select * from \"chart_data\" where \"symbol_code\" = \'CLN19\' AND \"chart_type\" = \'5\' AND \"cycle\" = \'1\'";
	std::string str_cycle = std::to_string(1);
	std::string str_chart_type = std::to_string(5);
	query_string.append("SELECT * FROM \"");
	query_string.append("chart_data");
	query_string.append("\" WHERE \"symbol_code\" = \'");
	query_string.append("CLN19");
	query_string.append("\' AND \"chart_type\" = \'");
	query_string.append(str_chart_type);
	query_string.append("\' AND \"cycle\" = \'");
	query_string.append(str_cycle);
	query_string.append("\'");
	std::string resp = dbMgr->ExecQuery(query_string);
	CString msg;
	msg.Format(_T("resp length = %d"), resp.length());
	TRACE(msg);
	try
	{
		auto json_object = json::parse(resp);
		auto a = json_object["results"][0]["series"][0]["values"];
		for (size_t i = 0; i < a.size(); i++) {
			auto val = a[i];
			std::string time = val[0];
			int h = 0, l = 0, o = 0, c = 0, v = 0;
			h = val[1];
			l = val[4];
			o = val[5];
			c = val[6];
			//v = val[7];

			std::string local_time = VtStringUtil::GetLocalTime(time);

			msg.Format(_T("index = %d, date_time = %s, lc = %s, h = %d, l = %d, o = %d, c = %d\n"), i, time.c_str(), local_time.c_str(), h, l, o, c);
			TRACE(msg);
		}
	}
	catch (const std::exception& e)
	{
		std::string error = e.what();
	}
}
