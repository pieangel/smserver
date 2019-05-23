#include "pch.h"
#include "SmMarketManager.h"
#include "SmConfigManager.h"
#include <string>
#include "Xml/tinyxml2.h"
#include "Xml/pugixml.hpp"
#include "SmSymbolReader.h"
#include "SmMarket.h"
#include "SmCategory.h"
SmMarketManager::SmMarketManager()
{
}


SmMarketManager::~SmMarketManager()
{
	for (auto it = _MarketList.begin(); it != _MarketList.end(); ++it) {
		delete* it;
	}
}

void SmMarketManager::ReadSymbolsFromFile()
{
	SmSymbolReader* symReader = SmSymbolReader::GetInstance();
	SmConfigManager* configMgr = SmConfigManager::GetInstance();
	std::string appPath = configMgr->GetApplicationPath();
	std::string configPath = appPath;
	configPath.append(_T("\\Config\\Config.xml"));
	std::string dataPath = appPath;
	dataPath.append(_T("\\Data\\"));
	pugi::xml_document doc;

	pugi::xml_parse_result result = doc.load_file(configPath.c_str());
	pugi::xml_node app = doc.first_child();
	pugi::xml_node sym_file_list = app.first_child();
	pugi::xml_node abroad_list = sym_file_list.first_child();
	int index = 0;
	for (auto it = abroad_list.begin(); it != abroad_list.end(); ++it) {
		std::string file_name = it->text().as_string();
		TRACE(file_name.c_str());
		std::string file_path = dataPath + file_name;
		symReader->ReadSymbolFromFile(index++, file_path);
	}
}

SmMarket* SmMarketManager::AddMarket(std::string name)
{
	SmMarket* found_market = FindMarket(name);
	if (found_market)
		return found_market;

	SmMarket* market = new SmMarket();
	market->Name(name);
	_MarketList.emplace_back(market);
	return market;
}

SmCategory* SmMarketManager::FindCategory(std::string mrkt_name, std::string cat_code)
{
	SmMarket* cur_market = FindMarket(mrkt_name);
	if (!cur_market)
		return nullptr;
	return cur_market->FindCategory(cat_code);
}

SmCategory* SmMarketManager::FindCategory(std::string cat_code)
{
	auto it = _CategoryToMarketMap.find(cat_code);
	if (it != _CategoryToMarketMap.end()) {
		std::string market_name = it->second;
		SmMarket* mrkt = FindMarket(market_name);
		if (mrkt)
			return mrkt->FindCategory(cat_code);
		else
			return nullptr;
	}

	return nullptr;
}

SmMarket* SmMarketManager::FindMarket(std::string mrkt_name)
{
	for (auto it = _MarketList.begin(); it != _MarketList.end(); ++it) {
		SmMarket* mrkt = *it;
		if (mrkt->Name().compare(mrkt_name) == 0) {
			return mrkt;
		}
	}

	return nullptr;
}

void SmMarketManager::AddCategoryMarket(std::string cat_code, std::string mrkt_name)
{
	_CategoryToMarketMap[cat_code] = mrkt_name;
}
