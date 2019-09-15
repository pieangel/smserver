#pragma once
#include "SmOrderManager.h"
#include <string>
struct SmOrder;
class SmSymbolOrderManager : public SmOrderManager
{
public:
	SmSymbolOrderManager();
	~SmSymbolOrderManager();
	virtual void OnOrderAccepted(std::shared_ptr<SmOrder> order);
	virtual void OnOrderFilled(std::shared_ptr<SmOrder> order);
	virtual void OnOrder(std::shared_ptr<SmOrder> order);
	std::string SymbolCode() const { return _SymbolCode; }
	void SymbolCode(std::string val) { _SymbolCode = val; }
private:
	double _TotalFee = 0.0;
	std::string _SymbolCode;
	void CalcPosition(std::shared_ptr<SmOrder> order);
	int CalcRemain(std::shared_ptr<SmOrder> newOrder);
	int CalcTotalRemain();
};

