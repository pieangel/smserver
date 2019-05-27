#include "pch.h"
#include "SmHdClient.h"
#include "SmHdCtrl.h"
#include "resource.h"

SmHdClient::SmHdClient()
{
	_HdCtrl = new SmHdCtrl();
	_HdCtrl->Create(IDD_HD_CTRL);
	_HdCtrl->ShowWindow(SW_HIDE);
	_HdCtrl->Init();
	_HdCtrl->Client(this);
}


SmHdClient::~SmHdClient()
{
	if (_HdCtrl) {
		_HdCtrl->DestroyWindow();
		delete _HdCtrl;
		_HdCtrl = nullptr;
	}
}

int SmHdClient::Login(std::string id, std::string pwd, std::string cert)
{
	return _HdCtrl->LogIn(id.c_str(), pwd.c_str(), cert.c_str());
}

void SmHdClient::RegisterProduct(std::string symCode)
{
	_HdCtrl->RegisterProduct(symCode);
}

void SmHdClient::UnregisterProduct(std::string symCode)
{
	_HdCtrl->UnregisterProduct(symCode);
}

void SmHdClient::GetChartData(SmChartDataRequest req)
{
	_HdCtrl->GetChartData(req);
}

void SmHdClient::OnRcvdAbroadHoga(CString& strKey, LONG& nRealType)
{

}

void SmHdClient::OnRcvdAbroadSise(CString& strKey, LONG& nRealType)
{

}
