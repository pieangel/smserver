#pragma once
#include <string>
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
};

