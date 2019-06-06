#pragma once
#include "SmOrderManager.h"
struct SmOrder;
class SmSymbolOrderManager : public SmOrderManager
{
public:
	SmSymbolOrderManager();
	~SmSymbolOrderManager();
	virtual void OnOrderAccepted(SmOrder* order);
	virtual void OnOrderFilled(SmOrder* order);
private:
	void CalcPosition(SmOrder* order);
};

