#include "pch.h"
#include "SmAccountManager.h"
#include "SmAccount.h"
SmAccountManager::SmAccountManager()
{

}

SmAccountManager::~SmAccountManager()
{

}

SmAccount* SmAccountManager::AddAccount(std::string accountNo, std::string accountName, std::string userID)
{
	SmAccount* acnt = FindAccount(accountNo);
	if (acnt)
		return acnt;

	acnt = new SmAccount();
	acnt->AccountNo(accountNo);
	acnt->AccountName(accountName);
	acnt->OwnerName(userID);
	_AccountMap[accountNo] = acnt;
	return acnt;
}

SmAccount* SmAccountManager::FindAccount(std::string accountNo)
{
	auto it = _AccountMap.find(accountNo);
	if (it != _AccountMap.end()) {
		return it->second;
	}

	return nullptr;
}
