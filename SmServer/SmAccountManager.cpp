#include "pch.h"
#include "SmAccountManager.h"
#include "SmAccount.h"
#include "SmMongoDBManager.h"
SmAccountManager::SmAccountManager()
{

}

SmAccountManager::~SmAccountManager()
{

}

std::shared_ptr<SmAccount> SmAccountManager::AddAccount(std::string accountNo, std::string accountName, std::string userID)
{
	std::shared_ptr<SmAccount> acnt = FindAccount(accountNo);
	if (acnt)
		return acnt;

	acnt = std::make_shared<SmAccount>();
	acnt->AccountNo(accountNo);
	acnt->AccountName(accountName);
	acnt->UserID(userID);
	_AccountMap[accountNo] = acnt;
	return acnt;
}

void SmAccountManager::AddAccount(std::shared_ptr<SmAccount> account)
{
	if (!account)
		return;
	_AccountMap[account->AccountNo()] = account;

}

std::shared_ptr<SmAccount> SmAccountManager::FindAccount(std::string accountNo)
{
	auto it = _AccountMap.find(accountNo);
	if (it != _AccountMap.end()) {
		return it->second;
	}

	return nullptr;
}

std::shared_ptr<SmAccount> SmAccountManager::FindAddAccount(std::string accountNo)
{
	auto it = _AccountMap.find(accountNo);
	if (it != _AccountMap.end())
		return it->second;
	std::shared_ptr<SmAccount> acnt = std::make_shared<SmAccount>();
	acnt->AccountNo(accountNo);
	_AccountMap[accountNo] = acnt;
	return acnt;
}

std::string SmAccountManager::GenAccountNo()
{
	return _NumGen.GetNewAccountNumber();
}

void SmAccountManager::LoadAccountFromDB()
{
	SmMongoDBManager* mongoMgr = SmMongoDBManager::GetInstance();
	mongoMgr->LoadAccountList();
}
