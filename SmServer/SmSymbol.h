#pragma once
#include <string>
#include <list>
struct SmQuoteItem {
	std::string	Time;	/* �ð�(HH:MM:SS)												*/
	int	ClosePrice;	/* ���簡														*/
	int	ContQty;	/* ü�ᷮ														*/
	int	MatchKind;	/* ���簡�� ȣ������ (+.�ż� -.�ŵ�)							*/
	int Decimal;
	std::string	Date;	
};

struct	SmQuote
{
	/// <summary>
	/// ��
	/// </summary>
	int High;
	/// <summary>
	/// ����
	/// </summary>
	int Low;
	/// <summary>
	/// ����
	/// </summary>
	int Close;
	/// <summary>
	/// �ð�
	/// </summary>
	int Open;
	/// <summary>
	/// ��������
	/// </summary>
	int PreClose;
	/// <summary>
	/// ����
	/// </summary>
	int Expected;
	/// <summary>
	/// ���ϰ�
	/// </summary>
	int PreDayHigh;
	/// <summary>
	/// ��������
	/// </summary>
	int PreDayLow;
	/// <summary>
	/// ���Ͻð�
	/// </summary>
	int PreDayOpen;
	/// <summary>
	/// ���ϴ�񱸺� : + (���), -(�϶�)
	/// </summary>
	std::string RatioToPredaySign;
	/// <summary>
	/// ���ϴ�� ��°�
	/// </summary>
	int GapFromPreDay;
	/// <summary>
	/// ���ϴ������
	/// </summary>
	std::string RatioToPreday;
	/// <summary>
	/// �ü�ȣ��ť
	/// </summary>
	std::list<SmQuoteItem> QuoteItemQ;
	/// <summary>
	/// �ü����������ð�
	/// </summary>
	std::string Time;
};

struct SmHoga
{
	/// <summary>
	/// �ð� - �ؿܼ����� �ؿܽð�
	/// </summary>
	std::string	Time;
	/// <summary>
	/// ������¥
	/// </summary>
	std::string DomesticDate;
	/// <summary>
	/// ���� �ð�
	/// </summary>
	std::string DomesticTime;
	/// <summary>
	/// ȣ�� ������
	/// </summary>
	struct	SmHogaItem
	{
		/// <summary>
		/// �ż�ȣ���Ǽ�
		/// </summary>
		int	BuyCnt;
		/// <summary>
		/// �ż� ȣ��
		/// </summary>
		int	BuyPrice;
		/// <summary>
		/// �ż�ȣ������
		/// </summary>
		int	BuyQty; 
		/// <summary>
		/// �ŵ�ȣ���Ǽ�
		/// </summary>
		int	SellCnt;
		/// <summary>
		/// �ŵ�ȣ��
		/// </summary>
		int	SellPrice;
		/// <summary>
		/// �ŵ�ȣ������
		/// </summary>
		int	SellQty;
	}	Ary[5];

	/// <summary>
	/// �ŵ���ȣ������
	/// </summary>
	int	TotSellQty;
	/// <summary>
	/// �ż���ȣ������
	/// </summary>
	int	TotBuyQty;
	/// <summary>
	/// �ŵ���ȣ���Ǽ�
	/// </summary>
	int	TotSellCnt;
	/// <summary>
	/// �ż���ȣ���Ǽ�
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
	/// ���� �ڵ� - ������ �����ϴ� Ű�� �ȴ�.
	/// </summary>
	std::string _SymbolCode;
	/// <summary>
	/// ���� �̸�
	/// </summary>
	std::string _Name;
	/// <summary>
	/// ���� ���� �̸�
	/// </summary>
	std::string _NameEn;
};

