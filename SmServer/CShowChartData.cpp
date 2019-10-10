// CShowChartData.cpp : implementation file
//

#include "pch.h"
#include "SmServer.h"
#include "CShowChartData.h"
#include "afxdialogex.h"
#include "SmChartDataManager.h"
#include "SmChartData.h"

// CShowChartData dialog

IMPLEMENT_DYNAMIC(CShowChartData, CDialogEx)

CShowChartData::CShowChartData(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SHOW_CHART_DATA, pParent)
{
	int i = 0;
	i = i + 1;
}

CShowChartData::~CShowChartData()
{
}

void CShowChartData::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRID, m_Grid);
	DDX_Control(pDX, IDC_COMBO_CHART_DATA, m_CombolChartData);
}


BEGIN_MESSAGE_MAP(CShowChartData, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBO_CHART_DATA, &CShowChartData::OnCbnSelchangeComboChartData)
END_MESSAGE_MAP()


// CShowChartData message handlers


BOOL CShowChartData::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_Grid.GetDefaultCell(FALSE, FALSE)->SetBackClr(RGB(0xFF, 0xFF, 0xE0));

	UpdateData();

	m_Grid.SetEditable(m_bEditable);

	m_nFixCols = 1;
	m_nFixRows = 1;
	m_nCols = 8;
	m_nRows = 2000;

	//m_Grid.SetAutoSizeStyle();

	m_Grid.SetRowCount(m_nRows);
	m_Grid.SetColumnCount(m_nCols);
	m_Grid.SetFixedRowCount(m_nFixRows);
	m_Grid.SetFixedColumnCount(m_nFixCols);

	std::map<std::string, std::shared_ptr<SmChartData>> datamap = SmChartDataManager::GetInstance()->GetChartDataMap();
	for (auto it = datamap.begin(); it != datamap.end(); ++it) {
		std::shared_ptr<SmChartData> item = it->second;
		m_CombolChartData.AddString(item->GetDataKey().c_str());
	}

	if (datamap.size() > 0) {
		m_CombolChartData.SetCurSel(0);
	}

	m_Grid.SetItemText(0, 0, "index");
	m_Grid.SetItemText(0, 1, "date");
	m_Grid.SetItemText(0, 2, "time");
	m_Grid.SetItemText(0, 3, "open");
	m_Grid.SetItemText(0, 4, "high");
	m_Grid.SetItemText(0, 5, "low");
	m_Grid.SetItemText(0, 6, "close");
	m_Grid.SetItemText(0, 7, "volume");
	m_Grid.SetColumnWidth(0, 60);
	m_Grid.SetColumnWidth(1, 100);
	m_Grid.SetColumnWidth(2, 100);
	m_Grid.SetColumnWidth(3, 80);
	m_Grid.SetColumnWidth(4, 80);
	m_Grid.SetColumnWidth(5, 80);
	m_Grid.SetColumnWidth(6, 80);
	m_Grid.SetColumnWidth(7, 80);

	// fill rows/cols with text
	for (int row = 1; row < m_Grid.GetRowCount(); row++)
	{
		for (int col = 0; col < m_Grid.GetColumnCount(); col++)
		{
			CString text;
			
			switch (col)
			{
			case 0:
				text.Format("%d", row);
				m_Grid.SetItemText(row, col, text);
				break;
			default:
				break;
			}
		}
	}

	
	UpdateData(FALSE);


	m_Grid.SetFixedColumnSelection(TRUE);
	m_Grid.SetFixedRowSelection(TRUE);
	m_Grid.EnableColumnHide();


	//m_Grid.AutoSize();

	m_Grid.SetCompareFunction(CGridCtrl::pfnCellNumericCompare);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CShowChartData::OnCbnSelchangeComboChartData()
{
	int sel = m_CombolChartData.GetCurSel();
	if (sel == -1)
		return;

	ClearGrid();

	CString item_text;
	m_CombolChartData.GetLBText(sel, item_text);
	std::shared_ptr<SmChartData> chart_data = SmChartDataManager::GetInstance()->FindChartData((LPCTSTR)item_text);
	if (chart_data) {
		std::multimap<std::string, SmChartDataItem>& data_map = chart_data->GetDataMap();
		size_t row_count = data_map.size();
		int row = 1;
		for (auto it = data_map.begin(); it != data_map.end(); ++it) {
			SmChartDataItem& item = it->second;
			for (int col = 0; col < m_Grid.GetColumnCount(); col++)
			{
				CString text;

				switch (col)
				{
				case 1:
					text.Format("%s", item.date.c_str());
					m_Grid.SetItemText(row, col, text);
					break;
				case 2:
					text.Format("%s", item.time.c_str());
					m_Grid.SetItemText(row, col, text);
					break;
				case 3:
					text.Format("%d", item.o);
					m_Grid.SetItemText(row, col, text);
					break;
				case 4:
					text.Format("%d", item.h);
					m_Grid.SetItemText(row, col, text);
					break;
				case 5:
					text.Format("%d", item.l);
					m_Grid.SetItemText(row, col, text);
					break;
				case 6:
					text.Format("%d", item.c);
					m_Grid.SetItemText(row, col, text);
					break;
				case 7:
					text.Format("%d", item.v);
					m_Grid.SetItemText(row, col, text);
					break;
				default:
					break;
				}
				CGridCellBase* pCell = m_Grid.GetCell(row, col);
				if (pCell) {
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				}
				m_Grid.RedrawCell(row, col);
			}
			row++;
		}
	}
}

void CShowChartData::ClearGrid()
{
	for (int row = 1; row < m_nRows; ++row) {
		for (int col = 1; col < m_nCols; ++col) {
			m_Grid.SetItemText(row, col, "");
			m_Grid.RedrawCell(row, col);
		}
	}
}
