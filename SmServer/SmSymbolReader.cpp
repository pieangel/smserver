#include "pch.h"
#include "SmSymbolReader.h"
#include <filesystem>
#include <sstream>
#include <string>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include "SmMarket.h"
#include "SmMarketManager.h"
#include "SmCategory.h"
#include "SmSymbol.h"
#include "SmSymbolManager.h"
namespace fs = std::filesystem;
SmSymbolReader::SmSymbolReader()
{
}


SmSymbolReader::~SmSymbolReader()
{
}

std::string SmSymbolReader::GetWorkingDir()
{
	fs::path cwp = fs::current_path();
	return cwp.string();
}


void SmSymbolReader::ReadSymbolFromFile(int index, std::string fullPath)
{
	switch (index)
	{
	case 0:
		ReadMarketFile(fullPath);
		break;
	case 1:
		ReadPmFile(fullPath);
		break;
	case 2:
		ReadJmFile(fullPath);
		break;
	default:
		break;
	}
}

void SmSymbolReader::ReadMarketFile()
{
	CString msg;
	std::ifstream infile("C:\\Users\\piean\\Source\\Repos\\ReadSymbol\\Debug\\MRKT.cod");
	std::string line;
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);
		std::string market = line.substr(0, 20);
		std::string exchange = line.substr(20, 5);
		std::string pmCode = line.substr(25, 3);
		std::string enName = line.substr(28, 50);
		std::string name = line.substr(78, 50);
		msg.Format(_T("market = %s, ex = %s, pmCode = %s, name = %s, ename = %s\n"), market.c_str(), exchange.c_str(), pmCode.c_str(), name.c_str(), enName.c_str());
		TRACE(msg);
	}
}

void SmSymbolReader::ReadMarketFile(std::string fullPath)
{
	SmMarketManager* marketMgr = SmMarketManager::GetInstance();
	CString msg;
	std::ifstream infile(fullPath);
	std::string line;
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);
		std::string market_type = line.substr(0, 20);
		std::string exchange = line.substr(20, 5);
		std::string pmCode = line.substr(25, 3);
		std::string enName = line.substr(28, 50);
		std::string name = line.substr(78, 50);
		
		boost::trim_right(market_type);
		boost::trim_right(exchange);
		boost::trim_right(pmCode);
		boost::trim_right(enName);
		boost::trim_right(name);
		
		SmMarket* market = marketMgr->AddMarket(market_type);
		SmCategory* cat = market->AddCategory(pmCode);
		cat->Exchange(exchange);
		cat->Name(enName);
		cat->NameKr(name);
		msg.Format(_T("market = %s, ex = %s, pmCode = %s, name = %s, ename = %s\n"), market_type.c_str(), exchange.c_str(), pmCode.c_str(), name.c_str(), enName.c_str());
		TRACE(msg);
	}
}

void SmSymbolReader::ReadPmFile()
{
	SmMarketManager* marketMgr = SmMarketManager::GetInstance();
	CString msg;
	std::ifstream infile("C:\\Users\\piean\\Source\\Repos\\ReadSymbol\\Debug\\PMCODE.cod");
	std::string line;
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);
		std::string market = line.substr(0, 20);
		// �ŷ��� �ε��� �ڵ�
		std::string exIndexCode = line.substr(20, 4);
		// �ŷ��� �ڵ� 
		std::string exChangeCode = line.substr(24, 5);
		// ǰ�� �ڵ�
		std::string pmCode = line.substr(29, 5);
		// ���� ������ ���ڷ� ��
		std::string pmGubun = line.substr(34, 3);
		msg.Format(_T("market = %s, ex = %s, pmCode = %s, name = %s, ename = %s\n"), market.c_str(), exIndexCode.c_str(), exChangeCode.c_str(), pmCode.c_str(), pmGubun.c_str());
		TRACE(msg);
		boost::trim_right(market);
		boost::trim_right(exIndexCode);
		boost::trim_right(exChangeCode);
		boost::trim_right(pmCode);
		boost::trim_right(pmGubun);
		SmCategory* cat = marketMgr->FindCategory(market, pmCode);
		if (cat) {
			cat->ExchangeCode(exChangeCode);
			cat->ExchangeIndex(exIndexCode);
			cat->MarketCode(pmGubun);
		}
	}
}



void SmSymbolReader::ReadPmFile(std::string fullPath)
{
	SmMarketManager* marketMgr = SmMarketManager::GetInstance();
	CString msg;
	std::ifstream infile(fullPath);
	std::string line;
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);
		std::string market = line.substr(0, 20);
		std::string exIndexCode = line.substr(20, 4);
		std::string exChangeCode = line.substr(24, 5);
		std::string pmCode = line.substr(29, 5);
		std::string pmGubun = line.substr(34, 3);
		msg.Format(_T("market = %s, ex = %s, pmCode = %s, name = %s, ename = %s\n"), market.c_str(), exIndexCode.c_str(), exChangeCode.c_str(), pmCode.c_str(), pmGubun.c_str());
		TRACE(msg);

		boost::trim_right(market);
		boost::trim_right(exIndexCode);
		boost::trim_right(exChangeCode);
		boost::trim_right(pmCode);
		boost::trim_right(pmGubun);
		SmCategory* cat = marketMgr->FindCategory(market, pmCode);
		if (cat) {
			cat->ExchangeCode(exChangeCode);
			cat->ExchangeIndex(exIndexCode);
			cat->MarketCode(pmGubun);
			marketMgr->AddCategoryMarket(pmCode, market);
		}
	}
}

void SmSymbolReader::ReadJmFile()
{
	CString msg;
	std::ifstream infile("C:\\Users\\piean\\Source\\Repos\\ReadSymbol\\Debug\\JMCODE.cod");
	std::string line;
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);

		std::string Series = line.substr(0, 32);
		/* �����ڵ�                             */

		std::string ExchCd = line.substr(32, 5);
		/* �ŷ���                               */

		std::string IndexCode = line.substr(37, 4);
		/* ǰ�� �ε����ڵ�                      */

		std::string MrktCd = line.substr(41, 5);
		/* ǰ���ڵ�                             */

		std::string ExchNo = line.substr(46, 5);
		/* �ŷ��� ��ȣ                          */

		std::string Pdesz = line.substr(51, 5);
		/* �Ҽ��� ����                          */

		std::string Rdesz = line.substr(56, 5);
		/* �Ҽ��� ����2                         */

		std::string CtrtSize = line.substr(61, 20);
		/* ���ũ��                             */

		std::string TickSize = line.substr(81, 20);
		/* Tick Size                            */

		std::string TickValue = line.substr(101, 20);
		/* Tick Value                           */

		std::string MltiPler = line.substr(121, 20);
		/* �ŷ��¼�                             */

		std::string DispDigit = line.substr(141, 10);
		/* ����                                 */

		std::string SeriesNm = line.substr(151, 32);
		/* Full �����                          */

		std::string SeriesNmKor = line.substr(183, 32);
		/* Full ������ѱ�                      */

		std::string NearSeq = line.substr(215, 1);
		/* �ֱٿ���, �ֿ�����ǥ��               */

		std::string StatTp = line.substr(216, 1);
		/* �ŷ����ɿ���                         */

		std::string LockDt = line.substr(217, 8);
		/* �ű԰ŷ�������                       */

		std::string TradFrDt = line.substr(225, 8);
		/* ���ʰŷ���                           */

		std::string TradToDt = line.substr(233, 8);
		/* �����ŷ���                           */

		std::string ExprDt = line.substr(241, 8);
		/* ������, ����������                   */

		std::string RemnCnt = line.substr(249, 4);
		/* �����ϼ�                             */

		std::string HogaMthd = line.substr(253, 30);
		/* ȣ�����                             */

		std::string MinMaxRt = line.substr(283, 6);
		/* ������������                         */

		std::string BaseP = line.substr(289, 20);
		/* ���ذ�                               */

		std::string MaxP = line.substr(309, 20);
		/* ���Ѱ�                               */

		std::string MinP = line.substr(329, 20);
		/* ���Ѱ�                               */

		std::string TrstMgn = line.substr(349, 20);
		/* �ű��ֹ����ű�                       */

		std::string MntMgn = line.substr(369, 20);
		/* �������ű�                           */

		std::string CrcCd = line.substr(389, 3);
		/* ������ȭ�ڵ�                         */

		std::string BaseCrcCd = line.substr(392, 3);
		/* BASE CRC CD                          */

		std::string CounterCrcCd = line.substr(395, 3);
		/* COUNTER CRC CD                       */

		std::string PipCost = line.substr(398, 20);
		/* PIP COST                             */

		std::string BuyInt = line.substr(418, 20);
		/* �ż�����                             */

		std::string SellInt = line.substr(438, 20);
		/* �ŵ�����                             */

		std::string RoundLots = line.substr(458, 6);
		/* LOUND LOTS                           */

		std::string ScaleChiper = line.substr(464, 10);
		/* �����ڸ���                           */

		std::string decimalchiper = line.substr(474, 5);
		/* �Ҽ��� ����(KTB����)                 */

		std::string JnilVolume = line.substr(479, 10);
		/* ���ϰŷ���                           */



		msg.Format(_T("code = %s, name = %s, name_kr = %s\n"), Series.c_str(), SeriesNm.c_str(), SeriesNmKor.c_str());
		TRACE(msg);
	}
}

void SmSymbolReader::ReadJmFile(std::string fullPath)
{
	SmMarketManager* marketMgr = SmMarketManager::GetInstance();
	CString msg;
	std::ifstream infile(fullPath);
	std::string line;
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);

		std::string Series = line.substr(0, 32);
		/* �����ڵ�                             */

		std::string ExchCd = line.substr(32, 5);
		/* �ŷ���                               */

		std::string IndexCode = line.substr(37, 4);
		/* ǰ�� �ε����ڵ�                      */

		std::string MrktCd = line.substr(41, 5);
		/* ǰ���ڵ�                             */

		std::string ExchNo = line.substr(46, 5);
		/* �ŷ��� ��ȣ                          */

		std::string Pdesz = line.substr(51, 5);
		/* �Ҽ��� ����                          */

		std::string Rdesz = line.substr(56, 5);
		/* �Ҽ��� ����2                         */

		std::string CtrtSize = line.substr(61, 20);
		/* ���ũ��                             */

		std::string TickSize = line.substr(81, 20);
		/* Tick Size                            */

		std::string TickValue = line.substr(101, 20);
		/* Tick Value                           */

		std::string MltiPler = line.substr(121, 20);
		/* �ŷ��¼�                             */

		std::string DispDigit = line.substr(141, 10);
		/* ����                                 */

		std::string SeriesNm = line.substr(151, 32);
		/* Full �����                          */

		std::string SeriesNmKor = line.substr(183, 32);
		/* Full ������ѱ�                      */

		std::string NearSeq = line.substr(215, 1);
		/* �ֱٿ���, �ֿ�����ǥ��               */

		std::string StatTp = line.substr(216, 1);
		/* �ŷ����ɿ���                         */

		std::string LockDt = line.substr(217, 8);
		/* �ű԰ŷ�������                       */

		std::string TradFrDt = line.substr(225, 8);
		/* ���ʰŷ���                           */

		std::string TradToDt = line.substr(233, 8);
		/* �����ŷ���                           */

		std::string ExprDt = line.substr(241, 8);
		/* ������, ����������                   */

		std::string RemnCnt = line.substr(249, 4);
		/* �����ϼ�                             */

		std::string HogaMthd = line.substr(253, 30);
		/* ȣ�����                             */

		std::string MinMaxRt = line.substr(283, 6);
		/* ������������                         */

		std::string BaseP = line.substr(289, 20);
		/* ���ذ�                               */

		std::string MaxP = line.substr(309, 20);
		/* ���Ѱ�                               */

		std::string MinP = line.substr(329, 20);
		/* ���Ѱ�                               */

		std::string TrstMgn = line.substr(349, 20);
		/* �ű��ֹ����ű�                       */

		std::string MntMgn = line.substr(369, 20);
		/* �������ű�                           */

		std::string CrcCd = line.substr(389, 3);
		/* ������ȭ�ڵ�                         */

		std::string BaseCrcCd = line.substr(392, 3);
		/* BASE CRC CD                          */

		std::string CounterCrcCd = line.substr(395, 3);
		/* COUNTER CRC CD                       */

		std::string PipCost = line.substr(398, 20);
		/* PIP COST                             */

		std::string BuyInt = line.substr(418, 20);
		/* �ż�����                             */

		std::string SellInt = line.substr(438, 20);
		/* �ŵ�����                             */

		std::string RoundLots = line.substr(458, 6);
		/* LOUND LOTS                           */

		std::string ScaleChiper = line.substr(464, 10);
		/* �����ڸ���                           */

		std::string decimalchiper = line.substr(474, 5);
		/* �Ҽ��� ����(KTB����)                 */

		std::string JnilVolume = line.substr(479, 10);
		/* ���ϰŷ���                           */


		boost::trim_right(Series);
		boost::trim_right(SeriesNm);
		boost::trim_right(SeriesNmKor);
		boost::trim_right(MrktCd);
		msg.Format(_T("code = %s, name = %s, name_kr = %s\n"), Series.c_str(), SeriesNm.c_str(), SeriesNmKor.c_str());
		TRACE(msg);

		SmSymbolManager* symMgr = SmSymbolManager::GetInstance();
		SmCategory* cat = marketMgr->FindCategory(MrktCd);
		if (cat) {
			SmSymbol* sym = cat->AddSymbol(Series);
			sym->Name(SeriesNmKor);
			sym->NameEn(SeriesNm);
			symMgr->AddSymbol(sym);
		}
	}
}
