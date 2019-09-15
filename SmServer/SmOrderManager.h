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
	// 전체 주문 맵
	std::map<int, std::shared_ptr<SmOrder>> _OrderMap;
	// 접수된 주문 맵
	std::map<int, std::shared_ptr<SmOrder>> _AcceptedOrderMap;
	// 체결되어 남아 있는 주문 목록
	// 체결되어 청산되면 _SettledMap으로 옮겨 간다.
	std::list<std::shared_ptr<SmOrder>> _RemainOrderMap;
	// 완전히 청산된 주문 목록
	std::vector<std::shared_ptr<SmOrder>> _SettledMap;
	std::shared_ptr<SmOrder> AddOrder(int orderNo);
	void AddOrder(std::shared_ptr<SmOrder> order);
	std::shared_ptr<SmOrder> FindOrder(int order_no);
	virtual void OnOrder(std::shared_ptr<SmOrder> order);
	virtual void OnOrderAccepted(std::shared_ptr<SmOrder> order);
	virtual void OnOrderFilled(std::shared_ptr<SmOrder> order);
};

