#include "SmUtil.h"
#include <stdarg.h> 

std::tuple<int, int, int> SmUtil::GetLocalTime()
{
	time_t now = time(0);
	tm timeinfo;
	localtime_s(&timeinfo, &now);

	return std::make_tuple(timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
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

std::vector<int> SmUtil::GetUtcDateTime()
{
	time_t now = time(0);
	tm* gmtm = gmtime(&now);

	std::vector<int> date_time;
	date_time.push_back(gmtm->tm_year + 1900);
	date_time.push_back(gmtm->tm_mon + 1);
	date_time.push_back(gmtm->tm_mday);
	date_time.push_back(gmtm->tm_hour);
	date_time.push_back(gmtm->tm_min);
	date_time.push_back(gmtm->tm_sec);

	return date_time;
}


std::string Format(const char* fmt, ...)
{
	char textString[MAX_BUFFER * 5] = { '\0' };

	// -- Empty the buffer properly to ensure no leaks.
	memset(textString, '\0', sizeof(textString));

	va_list args;
	va_start(args, fmt);
	vsnprintf(textString, MAX_BUFFER * 5, fmt, args);
	va_end(args);
	std::string retStr = textString;
	return retStr;
}

std::string SmUtil::GetUTCDateTimeString()
{
	time_t now = time(0);
	tm* gmtm = gmtime(&now);

	std::vector<int> date_time;
	date_time.push_back(gmtm->tm_year + 1900);
	date_time.push_back(gmtm->tm_mon + 1);
	date_time.push_back(gmtm->tm_mday);
	date_time.push_back(gmtm->tm_hour);
	date_time.push_back(gmtm->tm_min);
	date_time.push_back(gmtm->tm_sec);

	std::string result = Format("%04d-%02d-%02dT%02d:%02d:%02dZ", date_time[0], date_time[1], date_time[2], date_time[3], date_time[4], date_time[5]);

	return result;
}

std::string SmUtil::GetUTCDateTimeStringForNowMin()
{
	time_t now = time(0);
	tm* gmtm = gmtime(&now);

	std::vector<int> date_time;
	date_time.push_back(gmtm->tm_year + 1900);
	date_time.push_back(gmtm->tm_mon + 1);
	date_time.push_back(gmtm->tm_mday);
	date_time.push_back(gmtm->tm_hour);
	date_time.push_back(gmtm->tm_min);
	date_time.push_back(0);

	std::string result = Format("%04d-%02d-%02dT%02d:%02d:%02dZ", date_time[0], date_time[1], date_time[2], date_time[3], date_time[4], date_time[5]);

	return result;
}

std::string SmUtil::GetUTCDateTimeStringForPreMin(int previousMinLen)
{
	time_t now = time(0);
	tm* gmtm = gmtime(&now);

	std::vector<int> date_time;
	date_time.push_back(gmtm->tm_year + 1900);
	date_time.push_back(gmtm->tm_mon + 1);
	date_time.push_back(gmtm->tm_mday);
	int hour = gmtm->tm_hour;
	int min = gmtm->tm_min - previousMinLen;
	if (min < 0) {
		hour--;
		min = 60 - min;
	}
	else if (min > 60) {
		hour++;
		min = min - 60;
	}
	date_time.push_back(hour);
	date_time.push_back(gmtm->tm_min - previousMinLen);
	date_time.push_back(0);

	std::string result = Format("%04d-%02d-%02dT%02d:%02d:%02dZ", date_time[0], date_time[1], date_time[2], date_time[3], date_time[4], date_time[5]);

	return result;
}
