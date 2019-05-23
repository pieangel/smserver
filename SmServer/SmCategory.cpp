#include "pch.h"
#include "SmCategory.h"
#include "SmSymbol.h"

SmCategory::SmCategory()
{
}


SmCategory::~SmCategory()
{
	for (auto it = _SymbolList.begin(); it != _SymbolList.end(); ++it) {
		delete* it;
	}
}

SmSymbol* SmCategory::AddSymbol(std::string symCode)
{
	SmSymbol* sym = new SmSymbol();
	sym->SymbolCode(symCode);
	_SymbolList.emplace_back(sym);
	return sym;
}

