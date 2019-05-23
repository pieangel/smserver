#pragma once
#include "HDCtrl/hdfcommagent.h"

// VtHdCtrl dialog

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
public:
	int Init();
	int LogIn(CString id, CString pwd, CString cert);
	int LogOut(CString id);
	void LogIn();
	void LogOut();
};
