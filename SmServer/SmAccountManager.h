#pragma once
#include "Global/TemplateSingleton.h"
#include <map>
#include <string>
#include "SmAccountNumGenerator.h"
class SmAccount;
class SmAccountManager : public TemplateSingleton<SmAccountManager>
{
public:
	SmAccountManager();
	~SmAccountManager();
	void AddAccount(std::shared_ptr<SmAccount> account);
	std::shared_ptr<SmAccount> AddAccount(std::string accountNo, std::string accountName, std::string userID);
	std::shared_ptr<SmAccount> FindAccount(std::string accountNo);
	std::shared_ptr<SmAccount> FindAddAccount(std::string accountNo);
	std::string GenAccountNo();
	void LoadAccountFromDB();
private:
	SmAccountNumGenerator _NumGen;
	std::map<std::string, std::shared_ptr<SmAccount>> _AccountMap;
};

