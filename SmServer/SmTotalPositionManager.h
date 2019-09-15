#pragma once
#include <string>
#include <map>
#include "Global/TemplateSingleton.h"
struct SmPosition;
class SmAccountPositionManager;
struct SmOrder;
class SmAccount;
class SmTotalPositionManager : public TemplateSingleton<SmTotalPositionManager>
{
public:
	~SmTotalPositionManager();
	std::shared_ptr<SmPosition> CreatePosition(std::shared_ptr<SmOrder> order);
	std::shared_ptr<SmPosition> FindPosition(std::string acntNo, std::string symbolCode);
	void AddPosition(std::shared_ptr<SmPosition> posi);
private:
	std::map<std::string, SmAccountPositionManager*> _AccountPositionManagerMap;
	SmAccountPositionManager* FindAddAccountPositionManager(std::string accountNo);
};

