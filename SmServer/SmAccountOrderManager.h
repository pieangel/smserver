#pragma once
#include "Global/TemplateSingleton.h"
#include "SmOrderManager.h"
#include <string>
#include <map>
struct SmOrder;
class SmSymbolOrderManager;
class SmAccountOrderManager : public SmOrderManager
{
public:
	SmAccountOrderManager();
	~SmAccountOrderManager();
	virtual void OnOrderAccepted(SmOrder* order);
	virtual void OnOrderFilled(SmOrder* order);
	SmSymbolOrderManager* FindAddOrderManager(std::string symCode);
private:
	std::map<std::string, SmSymbolOrderManager*> _OrderManagerMap;
};

