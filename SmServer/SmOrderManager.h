#pragma once
#include <map>
#include <list>
#include <vector>
#include <string>
#include <memory>
struct SmOrder;
class SmOrderManager
{
public:
	SmOrderManager();
	virtual ~SmOrderManager();
	virtual void AddFilledOrder(std::shared_ptr<SmOrder> order);
	virtual void AddAcceptedOrder(std::shared_ptr<SmOrder> order);
protected:
	// ��ü �ֹ� ��
	std::map<int, std::shared_ptr<SmOrder>> _OrderMap;
	// ������ �ֹ� ��
	std::map<int, std::shared_ptr<SmOrder>> _AcceptedOrderMap;
	// ü��Ǿ� ���� �ִ� �ֹ� ���
	// ü��Ǿ� û��Ǹ� _SettledMap���� �Ű� ����.
	std::list<std::shared_ptr<SmOrder>> _RemainOrderMap;
	// ������ û��� �ֹ� ���
	std::vector<std::shared_ptr<SmOrder>> _SettledMap;
	std::shared_ptr<SmOrder> AddOrder(int orderNo);
	void AddOrder(std::shared_ptr<SmOrder> order);
	std::shared_ptr<SmOrder> FindOrder(int order_no);
	virtual void OnOrder(std::shared_ptr<SmOrder> order);
	virtual void OnOrderAccepted(std::shared_ptr<SmOrder> order);
	virtual void OnOrderFilled(std::shared_ptr<SmOrder> order);
};

