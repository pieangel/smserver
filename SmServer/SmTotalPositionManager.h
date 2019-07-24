#pragma once
#include <string>
#include <map>
#include "Global/TemplateSingleton.h"
struct SmPosition;
class SmAccountPositionManager;
struct SmOrder;
class SmTotalPositionManager : public TemplateSingleton<SmTotalPositionManager>
{
public:
	~SmTotalPositionManager();
	SmPosition* CreatePosition(SmOrder* order);
	SmPosition* FindPosition(std::string acntNo, std::string symbolCode);
private:
	std::map<std::string, SmAccountPositionManager*> _AccountPositionManagerMap;
	void AddPosition(SmPosition* posi);
	SmAccountPositionManager* FindAddAccountPositionManager(std::string accountNo);
};

