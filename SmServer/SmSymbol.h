#pragma once
#include <string>
#include <list>
#include "SmQuoteDefine.h"
#include "SmHogaDefine.h"

class SmSymbol
{
public:
	SmSymbol();
	~SmSymbol();
	std::string SymbolCode() const { return _SymbolCode; }
	void SymbolCode(std::string val) { _SymbolCode = val; }
	std::string Name() const { return _Name; }
	void Name(std::string val) { _Name = val; }
	std::string NameEn() const { return _NameEn; }
	void NameEn(std::string val) { _NameEn = val; }
	SmQuote Quote;
	SmHoga  Hoga;
	std::string GetQuoteByJson();
	std::string GetHogaByJson();
	int Decimal() const { return _Decimal; }
	void Decimal(int val) { _Decimal = val; }
	int Seungsu() const { return _Seungsu; }
	void Seungsu(int val) { _Seungsu = val; }
private:
	/// <summary>
	/// 종목 코드 - 종목을 구분하는 키가 된다.
	/// </summary>
	std::string _SymbolCode;
	/// <summary>
	/// 종목 이름
	/// </summary>
	std::string _Name;
	/// <summary>
	/// 영문 종목 이름
	/// </summary>
	std::string _NameEn;
	// 소수점 자리수
	int _Decimal;
	// 승수
	int _Seungsu;
};

