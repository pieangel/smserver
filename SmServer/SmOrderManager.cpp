#include "pch.h"
#include "SmOrderManager.h"
#include "SmOrder.h"
SmOrderManager::SmOrderManager()
{

}

SmOrderManager::~SmOrderManager()
{

}

void SmOrderManager::OnOrderAccepted(SmOrder* order)
{
	if (!order)
		return;

	_AcceptedOrderMap[order->OrderNo] = order;
}

void SmOrderManager::OnOrderFilled(SmOrder* order)
{
	if (!order)
		return;
	auto it = _AcceptedOrderMap.find(order->OrderNo);
	if (it != _AcceptedOrderMap.end()) {
		_AcceptedOrderMap.erase(it);
	}

	_FilledOrderMap[order->OrderNo] = order;
}

SmOrder* SmOrderManager::AddOrder(int orderNo)
{
	SmOrder* order = nullptr;
	auto it = _OrderMap.find(orderNo);
	if (it != _OrderMap.end())
		return it->second;
	order = new SmOrder();
	order->OrderNo = orderNo;
	_OrderMap[orderNo] = order;
	return order;
}

void SmOrderManager::AddOrder(SmOrder* order)
{
	if (!order)
		return;
	_OrderMap[order->OrderNo] = order;
}
