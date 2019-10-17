#pragma once
#include <string>
#include "SmOrderDefine.h"
struct SmPosition
{
	// 포지션이 만들어진 날짜
	std::string CreatedDate;
	/// <summary>
	/// 포지션이 만들어진 시간
	/// </summary>
	std::string CreatedTime;
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
	// 당일 수수료만 나타낸다. 포지션별 누적 수수료는 다른 경로를 통해 확인할 수 있다.
	double Fee = 0.0f; //		수수료
	double TradePL = 0.0f; //	매매손익
	double	AvgPrice = 0.0f;	/*평균가*/
	double	CurPrice = 0.0f;	/*현재가*/
	double	OpenPL = 0.0f;		/*평가손익*/
};

