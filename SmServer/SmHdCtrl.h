#pragma once
#include "HDCtrl/hdfcommagent.h"
#include <string>
#include "SmChartDefine.h"
// VtHdCtrl dialog
class SmHdClient;
class SmHdCtrl : public CDialogEx
{
	DECLARE_DYNAMIC(SmHdCtrl)

public:
	SmHdCtrl(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SmHdCtrl();
	CHDFCommAgent m_CommAgent;
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HD_CTRL };
#endif

	SmHdClient* Client() const { return _Client; }
	void Client(SmHdClient* val) { _Client = val; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	DECLARE_EVENTSINK_MAP()
	afx_msg void OnDataRecv(CString sTrCode, LONG nRqID);
	afx_msg void OnGetBroadData(CString strKey, LONG nRealType);
	afx_msg void OnGetMsg(CString strCode, CString strMsg);
	afx_msg void OnGetMsgWithRqId(int nRqId, CString strCode, CString strMsg);
private:
	CString m_sUserId;
	CString m_sUserPw;
	CString m_sAuthPw;
	CString m_sAcntPw;
	SmHdClient* _Client = nullptr;
public:
	int Init();
	int LogIn(CString id, CString pwd, CString cert);
	int LogOut(CString id);
	void LogIn();
	void LogOut();
	void RegisterProduct(std::string symCode);
	void UnregisterProduct(std::string symCode);
	void GetChartData(SmChartDataRequest req);
};
