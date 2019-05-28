#pragma once
#include <string>
#include <list>
struct SmQuoteItem {
	std::string	Time;	/* 시간(HH:MM:SS)												*/
	int	ClosePrice;	/* 현재가														*/
	int	ContQty;	/* 체결량														*/
	int	MatchKind;	/* 현재가의 호가구분 (+.매수 -.매도)							*/
	int Decimal;
	std::string	Date;	
};

struct	SmQuote
{
	/// <summary>
	/// 고가
	/// </summary>
	int High;
	/// <summary>
	/// 저가
	/// </summary>
	int Low;
	/// <summary>
	/// 종가
	/// </summary>
	int Close;
	/// <summary>
	/// 시가
	/// </summary>
	int Open;
	/// <summary>
	/// 전일종가
	/// </summary>
	int PreClose;
	/// <summary>
	/// 예상가
	/// </summary>
	int Expected;
	/// <summary>
	/// 전일고가
	/// </summary>
	int PreDayHigh;
	/// <summary>
	/// 전일저가
	/// </summary>
	int PreDayLow;
	/// <summary>
	/// 전일시가
	/// </summary>
	int PreDayOpen;
	/// <summary>
	/// 전일대비구분 : + (상승), -(하락)
	/// </summary>
	std::string RatioToPredaySign;
	/// <summary>
	/// 전일대비 상승값
	/// </summary>
	int GapFromPreDay;
	/// <summary>
	/// 전일대비등락율
	/// </summary>
	std::string RatioToPreday;
	/// <summary>
	/// 시세호가큐
	/// </summary>
	std::list<SmQuoteItem> QuoteItemQ;
	/// <summary>
	/// 시세수신최종시간
	/// </summary>
	std::string Time;
};

struct SmHoga
{
	/// <summary>
	/// 시간 - 해외선물은 해외시간
	/// </summary>
	std::string	Time;
	/// <summary>
	/// 국내날짜
	/// </summary>
	std::string DomesticDate;
	/// <summary>
	/// 국내 시간
	/// </summary>
	std::string DomesticTime;
	/// <summary>
	/// 호가 아이템
	/// </summary>
	struct	SmHogaItem
	{
		/// <summary>
		/// 매수호가건수
		/// </summary>
		int	BuyCnt;
		/// <summary>
		/// 매수 호가
		/// </summary>
		int	BuyPrice;
		/// <summary>
		/// 매수호가수량
		/// </summary>
		int	BuyQty; 
		/// <summary>
		/// 매도호가건수
		/// </summary>
		int	SellCnt;
		/// <summary>
		/// 매도호가
		/// </summary>
		int	SellPrice;
		/// <summary>
		/// 매도호가수량
		/// </summary>
		int	SellQty;
	}	Ary[5];

	/// <summary>
	/// 매도총호가수량
	/// </summary>
	int	TotSellQty;
	/// <summary>
	/// 매수총호가수량
	/// </summary>
	int	TotBuyQty;
	/// <summary>
	/// 매도총호가건수
	/// </summary>
	int	TotSellCnt;
	/// <summary>
	/// 매수총호가건수
	/// </summary>
	int	TotBuyCnt;

};

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

