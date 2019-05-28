#include "pch.h"
#include "SmLogManager.h"
#include "SmConfigManager.h"
#include "Util/VtStringUtil.h"
#include <filesystem>
#include <fstream>
#include "Log/loguru.hpp"
namespace fs = std::filesystem;

SmLogManager::SmLogManager()
{
}


SmLogManager::~SmLogManager()
{
}

void SmLogManager::InitLog()
{
	SmConfigManager* configMgr = SmConfigManager::GetInstance();
	std::string fileName = _T("SmServer.log");
	std::string appPath;
	appPath = configMgr->GetApplicationPath();
	appPath.append(_T("\\"));

	std::string dirName = appPath;

	// ���ó�¥ ���丮 �̸��� �����.
	dirName.append(VtStringUtil::getTimeStr());
	if (!fs::exists(dirName)) { // ���丮�� �������� ���� ���
						   // ���� ��¥�� ���丮 ����
		fs::create_directory(dirName);
		// �� ���丮 �ؿ� ������ �����.
		dirName.append(_T("\\"));
		dirName.append(fileName);
		std::ofstream outfile(dirName);
		outfile.close();
	}
	appPath.append(VtStringUtil::getTimeStr());
	appPath.append(_T("\\"));
	appPath.append(fileName);

	// Put every log message in "everything.log":
	loguru::add_file(appPath.c_str(), loguru::Append, loguru::Verbosity_MAX);

	// Only log INFO, WARNING, ERROR and FATAL to "latest_readable.log":
	//loguru::add_file("latest_readable.log", loguru::Truncate, loguru::Verbosity_INFO);

	// Only show most relevant things on stderr:
	loguru::g_stderr_verbosity = 1;

	LOG_F(INFO, _T("Log manager was created!"));
}
