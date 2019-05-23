#pragma once
#include "Global/TemplateSingleton.h"
#include <string>
#include <map>
#include <vector>
class SmMarket;
class SmCategory;
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
private:
	std::vector<SmMarket*> _MarketList;
	/// ǰ���� ���� ���� �̸� ���� ǥ
	/// <summary>
	/// �� ǰ�� ���Ͽ� �����̸��� ��Ī���� �ش�.
	/// Ű : ǰ�� �ڵ� 
	/// �� : ���� �̸�
	/// </summary>
	std::map<std::string, std::string> _CategoryToMarketMap;
};

