#pragma once
#include <string>
#include <vector>
#include <algorithm>

class VtStringUtil
{
public:
	VtStringUtil();
	~VtStringUtil();
	static std::string getTimeStr();
	static bool endsWith(const std::string& s, const std::string& suffix);
	static std::vector<std::string> split(const std::string& s, const std::string& delimiter, const bool& removeEmptyEntries = false);
	static std::string PadLeft(double input, char padding, int len, int decimal);
	static std::string PadRight(double input, char padding, int len, int decimal);
	static std::string PadLeft(int input, char padding, int len);
	static std::string PadRight(int input, char padding, int len);
	static std::string PadLeft(std::string input, char padding, int len);
	static std::string PadRight(std::string input, char padding, int len);
};

