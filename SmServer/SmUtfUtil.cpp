#include "pch.h"
#include "SmUtfUtil.h"

std::string SmUtfUtil::AnsiToUtf8(char* ansi)
{
	WCHAR unicode[1500];
	char utf8[1500];

	memset(unicode, 0, sizeof(unicode));
	memset(utf8, 0, sizeof(utf8));

	::MultiByteToWideChar(CP_ACP, 0, ansi, -1, unicode, sizeof(unicode));
	::WideCharToMultiByte(CP_UTF8, 0, unicode, -1, utf8, sizeof(utf8), NULL, NULL);

	return std::string(utf8);
}

std::string SmUtfUtil::Utf8ToAnsi(char* utf8)
{
	WCHAR* unicode = NULL;
	char* ansi = NULL;

	unicode = (WCHAR*)calloc(1, strlen(utf8) * 2);
	::MultiByteToWideChar(CP_UTF8, 0, utf8, -1, unicode, strlen(utf8) * 2);

	//ansi = (char*)calloc(1, _tcslen(unicode)*2);
	ansi = (char*)calloc(1, wcslen(unicode) * 2);  /* �׽�Ʈ�� ������Ʈ�� ���� ���� �Ӽ��� MultiByte �̹Ƿ� _tcslen()�� strlen()���� ���ǵȴ�. strlen()�� char*�� ���ڷ� �����Ƿ� ������ ������ �����. */
	//::WideCharToMultiByte(CP_ACP, 0, unicode, -1, ansi, _tcslen(unicode)*2, NULL, NULL); 
	::WideCharToMultiByte(CP_ACP, 0, unicode, -1, ansi, wcslen(unicode) * 2, NULL, NULL);

	return std::string(ansi);
}
