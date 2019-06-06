#pragma once
#include <string>
#include "SmOrderDefine.h"
struct SmPosition
{
	// 심볼 코드
	std::string SymbolCode;
	// 펀드 이름
	std::string FundName;
	// 계좌 번호
	std::string AccountNo;
	// 포지션
	SmPositionType Position = SmPositionType::None; 
	// 포지션의 잔고
	int OpenQty;
	double Fee; //		수수료
	double TradePL; //	매매손익
	double	AvgPrice = 0.0f;	/*평균가*/
	double	CurPrice = 0.0f;	/*현재가*/
	double	OpenPL = 0.0f;		/*평가손익*/
};

