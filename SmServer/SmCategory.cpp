#include "pch.h"
#include "SmCategory.h"
#include "SmSymbol.h"
#include "SmSymbolManager.h"

SmCategory::SmCategory()
{
}


SmCategory::~SmCategory()
{
	
}

std::shared_ptr<SmSymbol> SmCategory::AddSymbol(std::string symCode)
{
	SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
	std::shared_ptr<SmSymbol> sym = std::make_shared<SmSymbol>();
	sym->SymbolCode(symCode);
	_SymbolList.push_back(sym);
	symMgr->AddSymbol(sym);
	return sym;
}

std::shared_ptr<SmSymbol> SmCategory::GetRecentMonthSymbol()
{
	if (_SymbolList.size() == 0)
		return nullptr;
	return _SymbolList[0];
}

