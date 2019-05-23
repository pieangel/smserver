#pragma once
#include "Global/TemplateSingleton.h"
#include <string>
class SmHdCtrl;
class SmHdClient : public TemplateSingleton<SmHdClient>
{
public:
	SmHdClient();
	~SmHdClient();
	int Login(std::string id, std::string pwd, std::string cert);
private:
	SmHdCtrl* _HdCtrl = nullptr;
};

