
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "pch.h"
#include "framework.h"
#include "SmServer.h"

#include "MainFrm.h"

#include "SmChartServer.h"
#include "SmSymbolReader.h"
#include "SmMarketManager.h"
#include "Database/influxdb.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <iostream>
#include "Scheduler/Scheduler.h"
#include "Xml/pugixml.hpp"
#include "Json/json.hpp"

#include "SmHdClient.h"
#include "SmRealtimeRegisterManager.h"
#include "SmConfigManager.h"
#include "SmLogManager.h"
#include "SmMarketManager.h"
#include "SmMessageManager.h"
#include "SmRealtimeSymbolServiceManager.h"
#include "SmSymbolManager.h"
#include "SmSymbolReader.h"
#include "SmUserManager.h"
#include "SmScheduler.h"
#include "SmSymbol.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_COMMAND(ID_SERVER_START, &CMainFrame::OnServerStart)
	ON_WM_SHOWWINDOW()
	ON_WM_CLOSE()
	ON_COMMAND(ID_SERVER_STARTSCHEDULE, &CMainFrame::OnServerStartschedule)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame() noexcept
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
	if (_Scheduler) {
		delete _Scheduler;
	}
	if (_ChartServer) {
		delete _ChartServer;
	}
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: Delete these three lines if you don't want the toolbar to be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);


	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}

	RedrawWindow(nullptr, nullptr, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}



void CMainFrame::OnServerStart()
{
	// TODO: Add your command handler code here
	_ChartServer = new SmChartServer();

	//ScheduleTest();
	
	//InitHdClient();

	SmScheduler* timer = SmScheduler::GetInstance();
	timer->RepeatSymbolService();

}

void CMainFrame::DbTest()
{
	influxdb_cpp::server_info si("127.0.0.1", 8086, "testx", "test", "test");
	// post_http demo with resp[optional]
	string resp;
	int ret = influxdb_cpp::builder()
		.meas("test2")
		.tag("k", "v")
		.tag("x", "y")
		.field("x", 10)
		.field("y", 10.3, 2)
		.field("b", !!10)
		.timestamp(1512722735522840439)
		.post_http(si, &resp);



	auto now = std::chrono::system_clock::now();
	
	auto itt = std::chrono::system_clock::to_time_t(now);

	std::time_t lt = itt;
	auto local_field = *std::gmtime(&lt);
	local_field.tm_isdst = -1;
	auto utc = std::mktime(&local_field);


	std::ostringstream ss;
	ss << std::put_time(gmtime(&itt), "%FT%TZ");

	for (int i = 0; i < 50; i++) {
		ret = influxdb_cpp::builder()
			.meas("cli10")
			.field("h", (double)i)
			.field("l", i)
			.field("o", i)
			.field("c", i)
			.field("v", i)
			//.timestamp(itt)
			.post_http(si, &resp);
	}


	
	cout << ret << endl << resp << endl;

	// send_udp demo
	ret = influxdb_cpp::builder()
		.meas("test2")
		.tag("k", "v")
		.tag("x", "y")
		.field("x", 10)
		.field("y", 3.14e18, 3)
		.field("b", !!10)
		.timestamp(1512722735522840439)
		.send_udp("127.0.0.1", 8089);

	cout << ret << endl;

	influxdb_cpp::query(resp, "show databases", si);
	cout << resp << endl;

	

	influxdb_cpp::builder()
		.meas("foo")  // series 1
		.field("x", 10)

		.meas("bar")  // series 2
		.field("y", 10.3)
		.send_udp("127.0.0.1", 8091);

	influxdb_cpp::query(resp, "select * from cli10", si);
	cout << resp << endl;


	std::time_t t = std::time(nullptr);
	std::cout << "UTC:   " << std::put_time(std::gmtime(&t), "%c %Z") << '\n';
	std::cout << "local: " << std::put_time(std::localtime(&t), "%c %Z") << '\n';

	std::string db_name = "testx";
// 	JSON::Value v;
// 	v.read(resp);
// 	JSON::Array& a = v["results"][0]["series"][0]["values"].a();
// 	for (size_t i = 0; i < a.size(); ++i) {
// 		JSON::Array& name = a[i].a();
// 		int h, l, o, c, v;
// 		std::string time = name[0];
// 		h = name[1];
// 		l = name[2];
// 		o = name[3];
// 		c = name[4];
// 		v = name[5];
// 		CString msg;
// 		msg.Format(_T("time = %s, h=%d, l=%d, o=%d, c=%d, v=%d\n"), time.c_str(), h, l, o, c, v);
// 		TRACE(msg);
// 	}

	auto j2 = R"(
	{
		"happy": true,
		"pi": 3.141
	}
	)"_json;

	TRACE(resp.c_str());

	
}

void TimeFuction() {
	CMainFrame* mfrm = (CMainFrame*)AfxGetMainWnd();
	mfrm->ReadSymbols();
}

void message(const std::string& s) {
	std::cout << s << std::endl;
}

void onevent(const int id) {
	std::cout << id << std::endl;
}


class Functor
{
public:
	int operator()(int a, int b)
	{
		return a < b;
	}
};

void CMainFrame::ScheduleTest()
{
	_Scheduler = new Bosma::Scheduler(12);
	std::chrono::system_clock::time_point tp =_Scheduler->at("2019-05-27 02:36:16", onevent, 1);
	std::chrono::system_clock::time_point tp2 = _Scheduler->at("2019-05-27 02:36:16", onevent, 2);

	//Functor* ff = new Functor();
	//_Scheduler->at("2017-04-19 12:31:15", ff->(1,1));
}

void CMainFrame::InitHdClient()
{

}

void CMainFrame::RegisterProduct()
{
	SmRealtimeRegisterManager* realRegMgr = SmRealtimeRegisterManager::GetInstance();
	realRegMgr->RegisterProduct("CLN19");
}

/*
#include "SmHdClient.h"
#include "SmRealtimeRegisterManager.h"
#include "SmConfigManager.h"
#include "SmLogManager.h"
#include "SmMarketManager.h"
#include "SmMessageManager.h"
#include "SmRealtimeSymbolServiceManager.h"
#include "SmSymbolManager.h"
#include "SmSymbolReader.h"
#include "SmUserManager.h"
*/
void CMainFrame::ClearAllResource()
{
	SmScheduler::DestroyInstance();
	SmRealtimeRegisterManager::DestroyInstance();
	SmHdClient::DestroyInstance();
	SmConfigManager::DestroyInstance();
	SmLogManager::DestroyInstance();
	SmMarketManager::DestroyInstance();
	SmMessageManager::DestroyInstance();
	SmRealtimeSymbolServiceManager::DestroyInstance();
	SmSymbolManager::DestroyInstance();
	SmSymbolReader::DestroyInstance();
	SmUserManager::DestroyInstance();
}

void CMainFrame::ReadSymbols()
{
	SmSymbolReader* symReader = SmSymbolReader::GetInstance();
	std::string dir = symReader->GetWorkingDir();
	std::string name = dir;
	SmMarketManager* mrktMgr = SmMarketManager::GetInstance();

	//DbTest();

	SmConfigManager* configMgr = SmConfigManager::GetInstance();
	std::string appPath = configMgr->GetApplicationPath();

	mrktMgr->ReadSymbolsFromFile();

}


void CMainFrame::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CFrameWnd::OnShowWindow(bShow, nStatus);

	SmHdClient* hdClient = SmHdClient::GetInstance();

	SmSymbolReader* symReader = SmSymbolReader::GetInstance();
	SmConfigManager* configMgr = SmConfigManager::GetInstance();
	std::string appPath = configMgr->GetApplicationPath();
	std::string configPath = appPath;
	configPath.append(_T("\\Config\\Config.xml"));
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(configPath.c_str());
	pugi::xml_node app = doc.first_child();
	pugi::xml_node login_info = doc.child("application").child("login_info");
	
	std::string id = "yjs1974";// login_info.child("id").text().as_string();
	std::string pwd = "3753yang";// login_info.child("pwd").text().as_string();
	std::string cert = "#*sm1026jw";// login_info.child("cert").text().as_string();

	int loginResult = hdClient->Login(id, pwd, cert);
	if (loginResult < 0) {
		AfxMessageBox(_T("Login Error!"));
	}

	hdClient->DownloadMasterFiles("futures");
}


void CMainFrame::OnClose()
{
	ClearAllResource();

	CFrameWnd::OnClose();
}


void CMainFrame::OnServerStartschedule()
{
	ReadSymbols();
	RegisterProduct();

	SmScheduler* timer = SmScheduler::GetInstance();
	timer->RepeatSymbolService();
}
