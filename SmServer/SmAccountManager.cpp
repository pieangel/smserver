#include "pch.h"
#include "SmAccountManager.h"
#include "SmAccount.h"
#include "SmMongoDBManager.h"
#include <set>
#include "SmUserManager.h"
#include "SmGlobal.h"
#include "SmSessionManager.h"
#include "SmServiceDefine.h"
#include "SmUtfUtil.h"
#include "Json/json.hpp"
using namespace nlohmann;

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

std::shared_ptr<SmAccount> SmAccountManager::CreateAccount(std::string user_id, std::string password)
{
	std::string account_no = _NumGen.GetNewAccountNumber();
	std::shared_ptr<SmAccount> acnt = FindAccount(account_no);
	if (acnt)
		return acnt;

	acnt = std::make_shared<SmAccount>();
	acnt->AccountNo(account_no);
	acnt->UserID(user_id);
	_AccountMap[account_no] = acnt;

	SmMongoDBManager* mongoMgr = SmMongoDBManager::GetInstance();
	mongoMgr->SaveAccountInfo(acnt);

	return acnt;
}

void SmAccountManager::SendAccountList(int session_id, std::string user_id)
{
	std::vector<std::shared_ptr<SmAccount>> account_list = SmMongoDBManager::GetInstance()->GetAccountList(user_id);
	json send_object;
	send_object["res_id"] = SmProtocol::res_account_list;
	send_object["total_account_count"] = (int)account_list.size();
	for (size_t j = 0; j < account_list.size(); ++j) {
		std::shared_ptr<SmAccount> acnt = account_list[j];
		send_object["account"][j] = {
			{ "account_no",  acnt->AccountNo() },
			{ "account_name", SmUtfUtil::AnsiToUtf8((char*)acnt->AccountName().c_str()) },
			{ "user_id",  acnt->UserID() },
			{ "password",  acnt->Password() },
			{ "initial_balance",  acnt->InitialBalance() },
			{ "trade_profit_loss",  acnt->TradePL() },
			{ "open_profit_loss",  acnt->OpenPL() }
		};
		
	}

	std::shared_ptr<SmSessionManager> session_mgr = SmGlobal::GetInstance()->GetSessionManager();
	session_mgr->send(session_id, send_object.dump());
}
