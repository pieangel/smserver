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
	virtual void AddFilledOrder(std::shared_ptr<SmOrder> order);
	virtual void AddAcceptedOrder(std::shared_ptr<SmOrder> order);
	// ����Ȯ�ε� �ֹ����� ������.
	void SendAcceptedOrderList(int session_id, std::string account_no);
	// ü���(�̹� û��� �ֹ��� ü��) �ֹ����� ������.
	void SendFilledOrderList(int session_id, std::string account_no);
	// �ֹ� ����� �ֽ� �� ���� ������.
	void SendOrderList(int session_id, std::string account_no, int count = 50);
	void SendResponse(std::shared_ptr<SmOrder> order, SmProtocol protocol);
	void SendOrderList(int session_id, std::string account_no, std::string date_time);
private:
	std::shared_ptr<SmOrder> CreateOrder();
	void OnOrderNew(std::shared_ptr<SmOrder> order);
	void OnOrderModify(std::shared_ptr<SmOrder> order);
	void OnOrderCancel(std::shared_ptr<SmOrder> order);
	void CheckFilled(std::shared_ptr<SmOrder> order);
	void SendResponse(std::shared_ptr<SmOrder> order);
	void SendRemain(std::shared_ptr<SmOrder> order);
	void SendError(SmOrderError ErrorCode);
	void SendError(SmOrderError ErrorCode, SmOrderRequest& req);
	void SendError(SmOrderError ErrorCode, SmOrder& order);
	virtual void OnOrderAccepted(std::shared_ptr<SmOrder> order);
	virtual void OnOrderFilled(std::shared_ptr<SmOrder> order);
	virtual void OnOrder(std::shared_ptr<SmOrder> order);
	SmAccountOrderManager* FindAddOrderManager(std::string acntNo);
	std::map<std::string, SmAccountOrderManager*> _OrderManagerMap;
	void SendConfirmModify(std::shared_ptr<SmOrder> order);
	void SendConfirmCancel(std::shared_ptr<SmOrder> order);
};

