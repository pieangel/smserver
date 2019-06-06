#pragma once
#include "Global/TemplateSingleton.h"
#include "SmOrderManager.h"
#include "SmOrderDefine.h"
#include <map>
#include <string>
struct SmOrder;
class SmAccountOrderManager;
class SmTotalOrderManager : public TemplateSingleton<SmTotalOrderManager>, public SmOrderManager
{
public:
	SmTotalOrderManager();
	virtual ~SmTotalOrderManager();
	void OnOrder(SmOrderRequest&& req);
private:
	void OnOrderNew(SmOrder* order);
	void OnOrderModify(SmOrder* order);
	void OnOrderCancel(SmOrder* order);
	void CheckFilled(SmOrder* order);
	void SendResponse(SmOrder* order, SmOrderCode code);
	virtual void OnOrderAccepted(SmOrder* order);
	virtual void OnOrderFilled(SmOrder* order);
	SmAccountOrderManager* FindAddOrderManager(std::string acntNo);
	std::map<std::string, SmAccountOrderManager*> _OrderManagerMap;
};

