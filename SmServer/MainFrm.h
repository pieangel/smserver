
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
	void RegisterProduct();
	void ClearAllResource();
	void GetChartData();
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnClose();
	afx_msg void OnServerStartschedule();
	afx_msg void OnServerGetchartdata();
	afx_msg void OnServerCollectchartdata();
	afx_msg void OnServerUsertest();
	afx_msg void OnServerGetutctime();
	afx_msg void OnServerGetmindata();
	afx_msg void OnServerGetsise();
	afx_msg void OnServerGethoga();
	afx_msg void OnServerRegisterallrecentsymbol();
	afx_msg void OnServerDeletemeasure();
	afx_msg void OnServerCreatedatabase();
	afx_msg void OnServerShowchartdata();
};


