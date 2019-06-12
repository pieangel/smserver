#pragma once
#include <tuple>
#include <time.h>
#include <vector>
class SmUtil {
public:
	static std::tuple<int, int, int> GetLocalTime();
	static std::vector<int> GetLocalDateTime();
};

std::tuple<int, int, int> SmUtil::GetLocalTime()
{
	time_t now = time(0);
	tm timeinfo;
	localtime_s(&timeinfo, &now);

	return std::make_tuple( timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
}

std::vector<int> SmUtil::GetLocalDateTime()
{
	time_t now = time(0);
	tm timeinfo;
	localtime_s(&timeinfo, &now);

	std::vector<int> date_time;
	date_time.push_back(timeinfo.tm_year + 1900);
	date_time.push_back(timeinfo.tm_mon + 1);
	date_time.push_back(timeinfo.tm_mday);
	date_time.push_back(timeinfo.tm_hour);
	date_time.push_back(timeinfo.tm_min);
	date_time.push_back(timeinfo.tm_sec);

	return date_time;
}
