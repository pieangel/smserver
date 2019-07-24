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
	// ��ü �ֹ� ��
	std::map<int, SmOrder*> _OrderMap;
	// ������ �ֹ� ��
	std::map<int, SmOrder*> _AcceptedOrderMap;
	// ü��Ǿ� ���� �ִ� �ֹ� ���
	// ü��Ǿ� û��Ǹ� _SettledMap���� �Ű� ����.
	std::list<SmOrder*> _RemainOrderMap;
	// ������ û��� �ֹ� ���
	std::vector<SmOrder*> _SettledMap;
	SmOrder* AddOrder(int orderNo);
	void AddOrder(SmOrder* order);
	SmOrder* FindOrder(int order_no);
	virtual void OnOrder(SmOrder* order);
	virtual void OnOrderAccepted(SmOrder* order);
	virtual void OnOrderFilled(SmOrder* order);
};

