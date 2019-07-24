#pragma once
#include "SmOrderManager.h"
#include <string>
struct SmOrder;
class SmSymbolOrderManager : public SmOrderManager
{
public:
	SmSymbolOrderManager();
	~SmSymbolOrderManager();
	virtual void OnOrderAccepted(SmOrder* order);
	virtual void OnOrderFilled(SmOrder* order);
	virtual void OnOrder(SmOrder* order);
	std::string SymbolCode() const { return _SymbolCode; }
	void SymbolCode(std::string val) { _SymbolCode = val; }
private:
	std::string _SymbolCode;
	void CalcPosition(SmOrder* order);
	int CalcRemain(SmOrder* newOrder);
	int CalcTotalRemain();
};

