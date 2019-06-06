#pragma once
#include <string>
#include "SmOrderDefine.h"
struct SmPosition
{
	// �ɺ� �ڵ�
	std::string SymbolCode;
	// �ݵ� �̸�
	std::string FundName;
	// ���� ��ȣ
	std::string AccountNo;
	// ������
	SmPositionType Position = SmPositionType::None; 
	// �������� �ܰ�
	int OpenQty;
	double Fee; //		������
	double TradePL; //	�Ÿż���
	double	AvgPrice = 0.0f;	/*��հ�*/
	double	CurPrice = 0.0f;	/*���簡*/
	double	OpenPL = 0.0f;		/*�򰡼���*/
};

