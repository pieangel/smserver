#pragma once
#include "Global/TemplateSingleton.h"
#include <string>
#include <map>
#include <vector>
class SmMarket;
class SmCategory;
class SmSymbol;
class SmMarketManager : public TemplateSingleton<SmMarketManager>
{
public:
	SmMarketManager();
	~SmMarketManager();
	void ReadSymbolsFromFile();
	/// <summary>
	/// �̸����� ���� ��ü�� �߰��Ѵ�.
	/// </summary>
	/// <param name="name"></param>
	/// <returns>������ ���� ��ü�� �����ش�.</returns>
	SmMarket* AddMarket(std::string name);
	SmCategory* FindCategory(std::string mrkt_name, std::string cat_code);
	SmMarket* FindMarket(std::string mrkt_name);
	void AddCategoryMarket(std::string cat_code, std::string mrkt_name);
	SmCategory* FindCategory(std::string cat_code);
	std::vector<SmSymbol*> GetRecentMonthSymbolList();
	void SendMarketList(std::string user_id);
	void SendSymbolListByCategory(std::string user_id);
	int GetTotalCategoryCount();
	int GetTotalSymbolCount();
private:
	void SendSymbolMaster(std::string user_id, SmSymbol* sym);
	std::vector<SmMarket*> _MarketList;
	/// ǰ���� ���� ���� �̸� ���� ǥ
	/// <summary>
	/// �� ǰ�� ���Ͽ� �����̸��� ��Ī���� �ش�.
	/// Ű : ǰ�� �ڵ� 
	/// �� : ���� �̸�
	/// </summary>
	std::map<std::string, std::string> _CategoryToMarketMap;
};

