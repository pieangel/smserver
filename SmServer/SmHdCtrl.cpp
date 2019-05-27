// VtHdCtrl.cpp : implementation file
//

#include "pch.h"
#include "SmServer.h"
#include "SmHdCtrl.h"
#include "afxdialogex.h"
#include "Log/loguru.hpp"
#include "Util/VtStringUtil.h"
#include "SmHdClient.h"
#include <ctime>

// VtHdCtrl dialog

IMPLEMENT_DYNAMIC(SmHdCtrl, CDialogEx)

SmHdCtrl::SmHdCtrl(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_HD_CTRL, pParent)
{

}

SmHdCtrl::~SmHdCtrl()
{
	if (m_CommAgent.GetSafeHwnd()) {
		if (m_CommAgent.CommGetConnectState() == 1) {
			if (m_sUserId != "")
				m_CommAgent.CommLogout(m_sUserId);
		}

		m_CommAgent.CommTerminate(TRUE);
		m_CommAgent.DestroyWindow();
	}
}

void SmHdCtrl::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

int SmHdCtrl::Init()
{
	if (!m_CommAgent.GetSafeHwnd()) {
		m_CommAgent.Create("HDF CommAgent", WS_CHILD, CRect(0, 0, 0, 0), this, 2286);
	}
	else
		return -1000;

	if (m_CommAgent.GetSafeHwnd()) {
		int nRet = m_CommAgent.CommInit(1);

		if (nRet < 0) {
			//AfxMessageBox("통신프로그램 실행 오류");
			LOG_F(INFO, _T("통신프로그램 실행 오류"));
		}
		else {
			//AfxMessageBox("통신프로그램 실행 성공");
			LOG_F(INFO, _T("통신프로그램 실행 성공"));
		}

		return nRet;
	}

	return -1000;
}

int SmHdCtrl::LogIn(CString id, CString pwd, CString cert)
{
	m_sUserId = id;
	m_sUserPw = pwd;
	m_sAuthPw = cert;
	const char* pLoginSuccess[] = { "로그인 성공"	, "Login Successful" };
	const char* pLoginFail[] = { "로그인 실패"	, "Login Failure" };

	int nRet = m_CommAgent.CommLogin(m_sUserId, m_sUserPw, m_sAuthPw);
	if (nRet > 0) {
		//AfxMessageBox(pLoginSuccess[0]);

		//로긴후 반드시 호출...
		m_CommAgent.CommAccInfo();
		LOG_F(INFO, pLoginSuccess[0]);
	}
	else
	{
		CString strRet;
		strRet.Format("[%d]", nRet);
		//AfxMessageBox(pLoginFail[0] + strRet);
		LOG_F(INFO, _T("%s"), strRet);
	}

	return nRet;
}

void SmHdCtrl::LogIn()
{
	int nRet = m_CommAgent.CommLogin(m_sUserId, m_sUserPw, m_sAuthPw);
	if (nRet > 0)
	{
		//AfxMessageBox("로그인 성공");

		// 로긴후 반드시 호출...
		m_CommAgent.CommAccInfo();
		//m_CommAgent.CommReqMakeCod("all", 0);

		//GetAccountInfo();
	}
	else
	{
		CString strRet;
		strRet.Format("[%d]", nRet);
		AfxMessageBox("로그인 실패" + strRet);
	}
}

int SmHdCtrl::LogOut(CString id)
{
	return m_CommAgent.CommLogout(id);
}

void SmHdCtrl::LogOut()
{
	// 로그아웃한다.
	int nRet = m_CommAgent.CommLogout(m_sUserId);

	CString strRet;
	strRet.Format("[%d]", nRet);
	if (nRet < 0) {
		AfxMessageBox("로그아웃 실패" + strRet);
	}
	else {
		AfxMessageBox("로그아웃 성공" + strRet);
	}
}

void SmHdCtrl::RegisterProduct(std::string symCode)
{
	std::string key = VtStringUtil::PadRight(symCode, ' ', 32);
	int nRealType = 76;
	m_CommAgent.CommSetBroad(key.c_str(), nRealType);
	nRealType = 82;
	m_CommAgent.CommSetBroad(key.c_str(), nRealType);
}

void SmHdCtrl::UnregisterProduct(std::string symCode)
{
	int nRealType = 76;
	m_CommAgent.CommRemoveBroad(symCode.c_str(), nRealType);
	nRealType = 82;
	m_CommAgent.CommRemoveBroad(symCode.c_str(), nRealType);
}

void SmHdCtrl::GetChartData(SmChartDataRequest req)
{
	std::string temp;
	std::string reqString;

	temp = VtStringUtil::PadRight(req.symbolCode, ' ', 32);
	reqString.append(temp);

	time_t rawtime;
	struct tm* timeinfo;
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, sizeof(buffer), "%Y%m%d", timeinfo);
	std::string str(buffer);
	//reqString.append(curDate);
	//reqString.append(curDate);
	reqString.append(_T("99999999"));
	reqString.append(_T("99999999"));
	reqString.append(_T("9999999999"));

	if (req.next == 0)
		reqString.append(_T("0"));
	else
		reqString.append(_T("1"));

	if (req.chartType == SmChartType::TICK)
		reqString.append("1");
	else if (req.chartType == SmChartType::MIN)
		reqString.append("2");
	else if (req.chartType == SmChartType::DAY)
		reqString.append("3");
	else
		reqString.append("2");

	temp = VtStringUtil::PadLeft(req.cycle, '0', 2);
	reqString.append(temp);

	temp = VtStringUtil::PadLeft(req.count, '0', 5);
	reqString.append(temp);


	CString sTrCode = "o51200";
	CString sInput = reqString.c_str();
	CString strNextKey = _T("");
	//int nRqID = m_CommAgent.CommRqData(sTrCode, sInput, sInput.GetLength(), "");

	CString sReqFidInput = "000001002003004005006007008009010011012013014015";
	//CString strNextKey = m_CommAgent.CommGetNextKey(nRqID, "");
	int nRqID = m_CommAgent.CommFIDRqData(sTrCode, sInput, sReqFidInput, sInput.GetLength(), strNextKey);

}

BEGIN_MESSAGE_MAP(SmHdCtrl, CDialogEx)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(SmHdCtrl, CDialogEx)
	ON_EVENT(SmHdCtrl, (UINT)-1, 3, OnDataRecv, VTS_BSTR VTS_I4)
	ON_EVENT(SmHdCtrl, (UINT)-1, 4, OnGetBroadData, VTS_BSTR VTS_I4)
	ON_EVENT(SmHdCtrl, (UINT)-1, 5, OnGetMsg, VTS_BSTR VTS_BSTR)
	ON_EVENT(SmHdCtrl, (UINT)-1, 6, OnGetMsgWithRqId, VTS_I4 VTS_BSTR VTS_BSTR)

END_EVENTSINK_MAP()

// VtHdCtrl message handlers

void SmHdCtrl::OnDataRecv(CString sTrCode, LONG nRqID)
{
	int i = 0;
}

void SmHdCtrl::OnGetBroadData(CString strKey, LONG nRealType)
{
	if (!_Client)
		return;
	switch (nRealType)
	{
	case 76: // hoga
		_Client->OnRcvdAbroadHoga(strKey, nRealType);
		break;
	case 82: // sise
		_Client->OnRcvdAbroadSise(strKey, nRealType);
		break;
	default:
		break;
	}
}

void SmHdCtrl::OnGetMsg(CString strCode, CString strMsg)
{
	int i = 0;
}

void SmHdCtrl::OnGetMsgWithRqId(int nRqId, CString strCode, CString strMsg)
{
	int i = 0;
}