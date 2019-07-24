#include "pch.h"
#include "SmAccountPositionManager.h"
#include "SmPosition.h"

SmAccountPositionManager::~SmAccountPositionManager()
{
	for (auto it = _PositionMap.begin(); it != _PositionMap.end(); ++it)
		delete it->second;
}

SmPosition* SmAccountPositionManager::FindPosition(std::string symbolCode)
{
	auto it = _PositionMap.find(symbolCode);
	if (it != _PositionMap.end()) {
		return it->second;
	}

	return nullptr;
}

void SmAccountPositionManager::AddPosition(SmPosition* posi)
{
	if (!posi)
		return;
	_PositionMap[posi->SymbolCode] = posi;
}
