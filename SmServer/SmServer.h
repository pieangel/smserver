
// SmServer.h : main header file for the SmServer application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CSmServerApp:
// See SmServer.cpp for the implementation of this class
//

class CSmServerApp : public CWinApp
{
public:
	CSmServerApp() noexcept;


// Overrides
public:
	int RegisterOCX(CString strFileName);
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	UINT  m_nAppLook;
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CSmServerApp theApp;
