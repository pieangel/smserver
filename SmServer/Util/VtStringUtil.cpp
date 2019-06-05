#include "pch.h"
#include "VtStringUtil.h"
#include <string>
#include <sstream>
#include <chrono>
#include <ctime>
#include <time.h>
#include <iomanip>

VtStringUtil::VtStringUtil()
{
}


VtStringUtil::~VtStringUtil()
{
}

std::string VtStringUtil::getTimeStr()
{
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);

	std::stringstream ss;
	ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d");
	return ss.str();
}

bool VtStringUtil::endsWith(const std::string& s, const std::string& suffix)
{
	return s.size() >= suffix.size() &&
		s.substr(s.size() - suffix.size()) == suffix;
}

std::vector<std::string> VtStringUtil::split(const std::string& s, const std::string& delimiter, const bool& removeEmptyEntries /*= false*/)
{
	std::vector<std::string> tokens;

	for (size_t start = 0, end; start < s.length(); start = end + delimiter.length())
	{
		size_t position = s.find(delimiter, start);
		end = position != std::string::npos ? position : s.length();

		std::string token = s.substr(start, end - start);
		if (!removeEmptyEntries || !token.empty())
		{
			tokens.push_back(token);
		}
	}

	if (!removeEmptyEntries &&
		(s.empty() || endsWith(s, delimiter)))
	{
		tokens.push_back("");
	}

	return tokens;
}


std::string VtStringUtil::PadLeft(int input, char padding, int len)
{
	std::ostringstream out;
	out << std::internal << std::right << std::setfill(padding) << std::setw(len) << input;
	return out.str();
}

std::string VtStringUtil::PadLeft(std::string input, char padding, int len)
{
	std::ostringstream out;
	out << std::right << std::setfill(padding) << std::setw(len) << input;
	return out.str();
}

std::string VtStringUtil::PadLeft(double input, char padding, int len, int decimal)
{
	std::ostringstream out;
	out << std::fixed << std::setprecision(decimal) << std::right << std::setfill(padding) << std::setw(len) << input;
	return out.str();
}

std::string VtStringUtil::PadRight(int input, char padding, int len)
{
	std::ostringstream out;
	out << std::internal << std::left << std::setfill(padding) << std::setw(len) << input;
	return out.str();
}

std::string VtStringUtil::PadRight(std::string input, char padding, int len)
{
	std::ostringstream out;
	out << std::left << std::setfill(padding) << std::setw(len) << input;
	return out.str();
}

std::string VtStringUtil::PadRight(double input, char padding, int len, int decimal)
{
	std::ostringstream out;
	out << std::fixed << std::setprecision(decimal) << std::left << std::setfill(padding) << std::setw(len) << input;
	return out.str();
}

std::time_t VtStringUtil::GetUTCTimestamp(std::string datetime_string)
{
	time_t rawtime;
	struct tm* timeinfo;

	int year = std::stoi(datetime_string.substr(0, 4));
	int month = std::stoi(datetime_string.substr(4,2));
	int day = std::stoi(datetime_string.substr(6, 2));
	int hour = std::stoi(datetime_string.substr(8, 2));
	int min = std::stoi(datetime_string.substr(10, 2));
	int sec = std::stoi(datetime_string.substr(12, 2));

	/* get current timeinfo: */
	time(&rawtime); //or: rawtime = time(0);
	/* convert to struct: */
	timeinfo = localtime(&rawtime);

	/* now modify the timeinfo to the given date: */
	timeinfo->tm_year = year - 1900;
	timeinfo->tm_mon = month - 1;    //months since January - [0,11]
	timeinfo->tm_mday = day;          //day of the month - [1,31] 
	timeinfo->tm_hour = hour;         //hours since midnight - [0,23]
	timeinfo->tm_min = min;          //minutes after the hour - [0,59]
	timeinfo->tm_sec = sec;          //seconds after the minute - [0,59]

	/* call mktime: create unix time stamp from timeinfo struct */
	std::time_t date = mktime(timeinfo);

	return date;
}

std::string VtStringUtil::GetLocalTime(std::string utc_time_format)
{
	std::wstring wstrUTCTime = std::wstring(utc_time_format.begin(), utc_time_format.end());
	time_t utctime = getEpochTime(wstrUTCTime.c_str());
	struct tm tm;
	/*Convert UTC TIME To Local TIme*/
	localtime_s(&tm, &utctime);
	char CharLocalTimeofUTCTime[30];
	strftime(CharLocalTimeofUTCTime, 30, "%Y-%m-%dT%H:%M:%SZ", &tm);
	std::string strLocalTimeofUTCTime(CharLocalTimeofUTCTime);
	return strLocalTimeofUTCTime;
}

std::time_t VtStringUtil::getEpochTime(const std::wstring& dateTime)
{
	/* Standard UTC Format*/
	static const std::wstring dateTimeFormat{ L"%Y-%m-%dT%H:%M:%SZ" };

	std::wistringstream ss{ dateTime };
	std::tm dt;
	ss >> std::get_time(&dt, dateTimeFormat.c_str());

	/* Convert the tm structure to time_t value and return Epoch. */
	return _mkgmtime(&dt);
}

