#pragma once

#include "Grid/GridCtrl.h"
// CShowChartData dialog

class CShowChartData : public CDialogEx
{
	DECLARE_DYNAMIC(CShowChartData)

public:
	CShowChartData(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CShowChartData();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SHOW_CHART_DATA };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	int		m_nFixCols;
	int		m_nFixRows;
	int		m_nCols;
	int		m_nRows;
	BOOL	m_bEditable;
	CGridCtrl m_Grid;
	CComboBox m_CombolChartData;
	afx_msg void OnCbnSelchangeComboChartData();
	void ClearGrid();
};
