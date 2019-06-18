#include "SmUtil.h"
#include <stdarg.h> 
#include "pch.h"

std::tuple<int, int, int> SmUtil::GetLocalTime()
{
	time_t now = time(0);
	tm timeinfo;
	localtime_s(&timeinfo, &now);

	return std::make_tuple(timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
}

std::vector<int> SmUtil::GetLocalDate()
{
	time_t now = time(0);
	tm timeinfo;
	localtime_s(&timeinfo, &now);

	std::vector<int> datevec;
	datevec.push_back(timeinfo.tm_year - 1900);
	datevec.push_back(timeinfo.tm_mon + 1);
	datevec.push_back(timeinfo.tm_mday);
	return datevec;
}

std::vector<int> SmUtil::GetTime(std::string datetime_string)
{
	int year = std::stoi(datetime_string.substr(0, 4));
	int month = std::stoi(datetime_string.substr(4, 2));
	int day = std::stoi(datetime_string.substr(6, 2));
	int hour = 0;
	int min = 0;
	int sec = 0;
	if (datetime_string.length() > 8) {
		hour = std::stoi(datetime_string.substr(8, 2));
		min = std::stoi(datetime_string.substr(10, 2));
		sec = std::stoi(datetime_string.substr(12, 2));
	}
	
	std::vector<int> result;
	result.push_back(year);
	result.push_back(month);
	result.push_back(day);
	result.push_back(hour);
	result.push_back(min);
	result.push_back(sec);
	return result;
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

double SmUtil::GetDifTimeBySeconds(std::string newTime, std::string oldTime)
{
	struct tm new_time;
	struct tm old_time;
	double seconds;

	std::vector<int> newVec = GetTime(newTime);
	std::vector<int> oldVec = GetTime(oldTime);

	new_time.tm_year = newVec[0] - 1900;
	new_time.tm_mon = newVec[1] - 1;    //months since January - [0,11]
	new_time.tm_mday = newVec[2];          //day of the month - [1,31] 
	new_time.tm_hour = newVec[3];         //hours since midnight - [0,23]
	new_time.tm_min = newVec[4];          //minutes after the hour - [0,59]
	new_time.tm_sec = newVec[5];          //seconds after the minute - [0,59]

	old_time.tm_year = oldVec[0] - 1900;
	old_time.tm_mon = oldVec[1] - 1;    //months since January - [0,11]
	old_time.tm_mday = oldVec[2];          //day of the month - [1,31] 
	old_time.tm_hour = oldVec[3];         //hours since midnight - [0,23]
	old_time.tm_min = oldVec[4];          //minutes after the hour - [0,59]
	old_time.tm_sec = oldVec[5];          //seconds after the minute - [0,59]

	seconds = difftime(mktime(&new_time), mktime(&old_time));

	return seconds;
}

double SmUtil::GetDifTimeForNow(std::string srcTime)
{
	time_t now;
	struct tm new_time;
	double seconds;

	time(&now);  /* get current time; same as: now = time(NULL)  */

	std::vector<int> newVec = GetTime(srcTime);

	new_time.tm_year = newVec[0] - 1900;
	new_time.tm_mon = newVec[1] - 1;    //months since January - [0,11]
	new_time.tm_mday = newVec[2];          //day of the month - [1,31] 
	new_time.tm_hour = newVec[3];         //hours since midnight - [0,23]
	new_time.tm_min = newVec[4];          //minutes after the hour - [0,59]
	new_time.tm_sec = newVec[5];          //seconds after the minute - [0,59]


	seconds = difftime(now, mktime(&new_time));

	return seconds;
}

std::string SmUtil::AnsiToUtf8(char* ansi)
{
	WCHAR unicode[1500];
	char utf8[1500];

	memset(unicode, 0, sizeof(unicode));
	memset(utf8, 0, sizeof(utf8));

	::MultiByteToWideChar(CP_ACP, 0, ansi, -1, unicode, sizeof(unicode));
	::WideCharToMultiByte(CP_UTF8, 0, unicode, -1, utf8, sizeof(utf8), NULL, NULL);

	return std::string(utf8);
}

std::string SmUtil::Utf8ToAnsi(char* utf8)
{
	WCHAR* unicode = NULL;
	char* ansi = NULL;

	unicode = (WCHAR*)calloc(1, strlen(utf8) * 2);
	::MultiByteToWideChar(CP_UTF8, 0, utf8, -1, unicode, strlen(utf8) * 2);

	//ansi = (char*)calloc(1, _tcslen(unicode)*2);
	ansi = (char*)calloc(1, wcslen(unicode) * 2);  /* 테스트한 프로젝트의 문자 집합 속성이 MultiByte 이므로 _tcslen()은 strlen()으로 정의된다. strlen()은 char*을 인자로 받으므로 컴파일 오류가 생긴다. */
	//::WideCharToMultiByte(CP_ACP, 0, unicode, -1, ansi, _tcslen(unicode)*2, NULL, NULL); 
	::WideCharToMultiByte(CP_ACP, 0, unicode, -1, ansi, wcslen(unicode) * 2, NULL, NULL);

	return std::string(ansi);
}
