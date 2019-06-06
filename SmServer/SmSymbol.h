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
	int High = 0;
	/// <summary>
	/// ����
	/// </summary>
	int Low = 0;
	/// <summary>
	/// ����
	/// </summary>
	int Close = 0;
	/// <summary>
	/// �ð�
	/// </summary>
	int Open = 0;
	/// <summary>
	/// ��������
	/// </summary>
	int PreClose = 0;
	/// <summary>
	/// ����
	/// </summary>
	int Expected = 0;
	/// <summary>
	/// ���ϰ�
	/// </summary>
	int PreDayHigh = 0;
	/// <summary>
	/// ��������
	/// </summary>
	int PreDayLow = 0;
	/// <summary>
	/// ���Ͻð�
	/// </summary>
	int PreDayOpen = 0;
	/// <summary>
	/// ���ϴ�񱸺� : + (���), -(�϶�)
	/// </summary>
	std::string RatioToPredaySign = "+";
	/// <summary>
	/// ���ϴ�� ��°�
	/// </summary>
	int GapFromPreDay = 0;
	/// <summary>
	/// ���ϴ������
	/// </summary>
	std::string RatioToPreday = "0.0";
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
	std::string	Time = "";
	/// <summary>
	/// ������¥
	/// </summary>
	std::string DomesticDate = "";
	/// <summary>
	/// ���� �ð�
	/// </summary>
	std::string DomesticTime = "";
	/// <summary>
	/// ȣ�� ������
	/// </summary>
	struct	SmHogaItem
	{
		/// <summary>
		/// �ż�ȣ���Ǽ�
		/// </summary>
		int	BuyCnt = 0;
		/// <summary>
		/// �ż� ȣ��
		/// </summary>
		int	BuyPrice = 0;
		/// <summary>
		/// �ż�ȣ������
		/// </summary>
		int	BuyQty = 0; 
		/// <summary>
		/// �ŵ�ȣ���Ǽ�
		/// </summary>
		int	SellCnt = 0;
		/// <summary>
		/// �ŵ�ȣ��
		/// </summary>
		int	SellPrice = 0;
		/// <summary>
		/// �ŵ�ȣ������
		/// </summary>
		int	SellQty = 0;
	}	Ary[5];

	/// <summary>
	/// �ŵ���ȣ������
	/// </summary>
	int	TotSellQty = 0;
	/// <summary>
	/// �ż���ȣ������
	/// </summary>
	int	TotBuyQty = 0;
	/// <summary>
	/// �ŵ���ȣ���Ǽ�
	/// </summary>
	int	TotSellCnt = 0;
	/// <summary>
	/// �ż���ȣ���Ǽ�
	/// </summary>
	int	TotBuyCnt = 0;

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
	std::string GetQuoteByJson();
	std::string GetHogaByJson();
	int Decimal() const { return _Decimal; }
	void Decimal(int val) { _Decimal = val; }
	int Seungsu() const { return _Seungsu; }
	void Seungsu(int val) { _Seungsu = val; }
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
	// �Ҽ��� �ڸ���
	int _Decimal;
	// �¼�
	int _Seungsu;
};

