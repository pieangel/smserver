#pragma once
#include <map>
struct SmOrder;
class SmOrderManager
{
public:
	SmOrderManager();
	virtual ~SmOrderManager();
protected:
	std::map<int, SmOrder*> _OrderMap;
	std::map<int, SmOrder*> _AcceptedOrderMap;
	std::map<int, SmOrder*> _FilledOrderMap;
	std::map<int, SmOrder*> _SettledMap;
	SmOrder* AddOrder(int orderNo);
	void AddOrder(SmOrder* order);
	virtual void OnOrderAccepted(SmOrder* order);
	virtual void OnOrderFilled(SmOrder* order);
};

