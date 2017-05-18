#pragma once
#include <windows.h>
#include <vector>

std::vector<std::wstring> BIFFlagsToWString(UINT flags);

std::wstring string_to_wide_string(const std::string& str);
std::string wide_string_to_string(const std::wstring& wstr);

inline void ShowDebugMessage(const wchar_t* title, const wchar_t* format...)
{
#if _DEBUG
	va_list args;
	va_start(args, format);
	wchar_t* buffer = new wchar_t[1024];
	vswprintf_s(buffer, 1024, format, args);
	MessageBox(NULL, buffer, title, MB_OK);
	delete[] buffer;
	va_end(args);
#endif
}
