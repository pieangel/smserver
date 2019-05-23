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
