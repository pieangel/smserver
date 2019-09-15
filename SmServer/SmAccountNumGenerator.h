#pragma once
#include <string>
class SmAccountNumGenerator
{
public:
	std::string GetNewAccountNumber();
	void SaveAccountNumber();
private:
	int _First = 100;
	int _Second = 1000;
	int _Last = 1000;
	int _Level = 0;
};

