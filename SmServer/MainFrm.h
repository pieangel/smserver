
// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include "Scheduler/Scheduler.h"
class SmChartServer;
class Bosma::Scheduler;

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame() noexcept;
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CToolBar          m_wndToolBar;
	CStatusBar        m_wndStatusBar;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnServerStart();
	void DbTest();
	void ReadSymbols();
private:
	SmChartServer* _ChartServer = nullptr;
	Bosma::Scheduler* _Scheduler = nullptr;
	void ScheduleTest();
	void InitHdClient();
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};

