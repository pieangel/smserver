#pragma once
#include "Global/TemplateSingleton.h"
#include "SmOrderManager.h"
#include "SmOrderDefine.h"
#include "SmServiceDefine.h"
#include <map>
#include <string>
struct SmOrder;
class SmAccountOrderManager;
class SmTotalOrderManager : public TemplateSingleton<SmTotalOrderManager>, public SmOrderManager
{
public:
	SmTotalOrderManager();
	virtual ~SmTotalOrderManager();
	void OnRequestOrder(SmOrderRequest&& req);
private:
	SmOrder* CreateOrder();
	void OnOrderNew(SmOrder* order);
	void OnOrderModify(SmOrder* order);
	void OnOrderCancel(SmOrder* order);
	void CheckFilled(SmOrder* order);
	void SendResponse(SmOrder* order);
	void SendRemain(SmOrder* order);
	void SendResponse(SmOrder* order, SmProtocol protocol);
	void SendError(SmOrderError ErrorCode);
	void SendError(SmOrderError ErrorCode, SmOrderRequest& req);
	void SendError(SmOrderError ErrorCode, SmOrder& order);
	virtual void OnOrderAccepted(SmOrder* order);
	virtual void OnOrderFilled(SmOrder* order);
	virtual void OnOrder(SmOrder* order);
	SmAccountOrderManager* FindAddOrderManager(std::string acntNo);
	std::map<std::string, SmAccountOrderManager*> _OrderManagerMap;
	void SendConfirmModify(SmOrder* order);
	void SendConfirmCancel(SmOrder* order);
};

