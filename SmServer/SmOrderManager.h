#pragma once
#include <map>
#include <list>
#include <vector>
struct SmOrder;
class SmOrderManager
{
public:
	SmOrderManager();
	virtual ~SmOrderManager();
protected:
	// 전체 주문 맵
	std::map<int, SmOrder*> _OrderMap;
	// 접수된 주문 맵
	std::map<int, SmOrder*> _AcceptedOrderMap;
	// 체결되어 남아 있는 주문 목록
	// 체결되어 청산되면 _SettledMap으로 옮겨 간다.
	std::list<SmOrder*> _RemainOrderMap;
	// 완전히 청산된 주문 목록
	std::vector<SmOrder*> _SettledMap;
	SmOrder* AddOrder(int orderNo);
	void AddOrder(SmOrder* order);
	SmOrder* FindOrder(int order_no);
	virtual void OnOrder(SmOrder* order);
	virtual void OnOrderAccepted(SmOrder* order);
	virtual void OnOrderFilled(SmOrder* order);
};

