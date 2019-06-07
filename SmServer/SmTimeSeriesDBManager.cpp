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

namespace ip = boost::asio::ip;

SmTimeSeriesDBManager::SmTimeSeriesDBManager()
{
	std::vector<std::string> addr_vec = GetIPAddress("angelpie.ddns.net");
	_Ip = addr_vec[3];
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

	std::string resp;
	int ret = influxdb_cpp::builder()
		.meas(data_item.GetDataKey())
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

	std::string h = ip::host_name();
	std::cout << "Host name is " << h << '\n';
	std::cout << "IP addresses are: \n";
	std::stringstream ss;
	std::vector<std::string> addr_vec;
	std::for_each(resolver.resolve({ h, "" }), {}, [&host_name, &ss, &addr_vec](const auto& re) {
		ss << re.endpoint().address();
		host_name = ss.str();
		addr_vec.push_back(host_name);
		ss.str("");
		});
	return addr_vec;
}
