#pragma once
//
// Utility to convert from UTF8 string to MFC TCHAR string.
//

class UTF8toTCHAR
{
public:
	UTF8toTCHAR(const char* utf8_string) : t_string(0), needFree(false)
	{
		if (0 == utf8_string)
			t_string = 0;
		else if (0 == *utf8_string)
			t_string = _T("");
		else if ((sizeof(TCHAR) == sizeof(char)) && isPureAscii(utf8_string))
			// No conversion needed for pure ASCII text
			t_string = (TCHAR*)utf8_string;
		else
		{
			// Either TCHAR = Unicode (2 bytes), or utf8_string contains non-ASCII characters.
			// Needs conversion
			needFree = true;

			// Convert to Unicode (2 bytes)
			int string_len = (int)strlen(utf8_string);
			wchar_t* buffer = new wchar_t[string_len + 1];
			MultiByteToWideChar(CP_UTF8, 0, utf8_string, -1, buffer, string_len + 1);
			buffer[string_len] = 0;

#ifdef _UNICODE
			t_string = buffer;
#else
			// TCHAR is MBCS - need to convert back to MBCS
			t_string = new char[string_len * 2 + 2];
			WideCharToMultiByte(CP_ACP, 0, buffer, -1, t_string, string_len * 2 + 1, 0, 0);
			t_string[string_len * 2 + 1] = 0;
			delete[] buffer;
#endif
		}

	}

	operator const TCHAR* ()
	{
		return t_string;
	}

	~UTF8toTCHAR()
	{
		if (needFree)
			delete[] t_string;
	}

private:
	TCHAR* t_string;
	bool needFree;

	//
	// helper utility to test if a string contains only ASCII characters
	//
	bool isPureAscii(const char* s)
	{
		while (*s != 0) { if (*(s++) & 0x80) return false; }
		return true;
	}

	//disable assignment
	UTF8toTCHAR(const UTF8toTCHAR& rhs);
	UTF8toTCHAR& operator=(const UTF8toTCHAR& rhs);
};

//
// Utility to convert from MFC TCHAR string to UTF8 string
//
class TCHARtoUTF8
{
public:
	TCHARtoUTF8(const TCHAR* t_string) : utf8_string(0), needFree(false)
	{
		if (0 == t_string)
			utf8_string = 0;
		else if (0 == *t_string)
			utf8_string = "";
		else if ((sizeof(TCHAR) == sizeof(char)) && isPureAscii((char*)t_string))
			// No conversion needed for pure ASCII text
			utf8_string = (char*)t_string;
		else
		{
			// TCHAR is non-ASCII. Needs conversion.

			needFree = true;
			int string_len = (int)_tcslen(t_string);

#ifndef _UNICODE
			// Convert to Unicode if not already in unicode.
			wchar_t* w_string = new wchar_t[string_len + 1];
			MultiByteToWideChar(CP_ACP, 0, t_string, -1, w_string, string_len + 1);
			w_string[string_len] = 0;
#else
			wchar_t* w_string = (wchar_t*)t_string;
#endif

			// Convert from Unicode (2 bytes) to UTF8
			utf8_string = new char[string_len * 3 + 1];
			WideCharToMultiByte(CP_UTF8, 0, w_string, -1, utf8_string, string_len * 3 + 1, 0, 0);
			utf8_string[string_len * 3] = 0;

			if (w_string != (wchar_t*)t_string)
				delete[] w_string;
		}

	}

	operator const char* ()
	{
		return utf8_string;
	}

	~TCHARtoUTF8()
	{
		if (needFree)
			delete[] utf8_string;
	}

private:
	char* utf8_string;
	bool needFree;

	//
	// helper utility to test if a string contains only ASCII characters
	//
	bool isPureAscii(const char* s)
	{
		while (*s != 0) { if (*(s++) & 0x80) return false; }
		return true;
	}

	//disable assignment
	TCHARtoUTF8(const TCHARtoUTF8& rhs);
	TCHARtoUTF8& operator=(const TCHARtoUTF8& rhs);
};

