#include "pch.h"
#include "SmOrderManager.h"
#include "SmOrder.h"
SmOrderManager::SmOrderManager()
{

}

SmOrderManager::~SmOrderManager()
{

}

void SmOrderManager::AddFilledOrder(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;
	auto it = _AcceptedOrderMap.find(order->OrderNo);
	if (it != _AcceptedOrderMap.end()) {
		_AcceptedOrderMap.erase(it);
	}
}

void SmOrderManager::AddAcceptedOrder(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;

	_AcceptedOrderMap[order->OrderNo] = order;
}

void SmOrderManager::OnOrderAccepted(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;

	_AcceptedOrderMap[order->OrderNo] = order;
}

void SmOrderManager::OnOrderFilled(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;
	auto it = _AcceptedOrderMap.find(order->OrderNo);
	if (it != _AcceptedOrderMap.end()) {
		_AcceptedOrderMap.erase(it);
	}
}

std::shared_ptr<SmOrder> SmOrderManager::AddOrder(int orderNo)
{
	std::shared_ptr<SmOrder> order = nullptr;
	auto it = _OrderMap.find(orderNo);
	if (it != _OrderMap.end())
		return it->second;
	_OrderMap[orderNo] = order;
	return order;
}

void SmOrderManager::AddOrder(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;
	auto it = _OrderMap.find(order->OrderNo);
	if (it != _OrderMap.end())
		return;
	_OrderMap[order->OrderNo] = order;
}

std::shared_ptr<SmOrder> SmOrderManager::FindOrder(int order_no)
{
	auto it = _OrderMap.find(order_no);
	if (it != _OrderMap.end()) {
		return it->second;
	}

	return nullptr;
}

void SmOrderManager::OnOrder(std::shared_ptr<SmOrder> order)
{
	if (!order)
		return;
	AddOrder(order);
}
