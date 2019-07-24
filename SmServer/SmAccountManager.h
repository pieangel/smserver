#pragma once
#include "Global/TemplateSingleton.h"
#include <map>
#include <string>
class SmAccount;
class SmAccountManager : public TemplateSingleton<SmAccountManager>
{
public:
	SmAccountManager();
	~SmAccountManager();
	SmAccount* AddAccount(std::string accountNo, std::string accountName, std::string userID);
	SmAccount* FindAccount(std::string accountNo);
	SmAccount* FindAddAccount(std::string accountNo);
private:
	std::map<std::string, SmAccount*> _AccountMap;
};

