#include "Utilities.h"
#include <map>
#include <shlobj.h>
#include <vector>
#include <codecvt>

#define LABEL(x) {x, L ## #x}

std::map<UINT, std::wstring> labels = {
	LABEL(BIF_RETURNONLYFSDIRS),
	LABEL(BIF_DONTGOBELOWDOMAIN),
	LABEL(BIF_STATUSTEXT),
	LABEL(BIF_RETURNFSANCESTORS),
	LABEL(BIF_EDITBOX),
	LABEL(BIF_VALIDATE),
	LABEL(BIF_NEWDIALOGSTYLE),
	LABEL(BIF_BROWSEINCLUDEURLS),
	LABEL(BIF_UAHINT),
	LABEL(BIF_NONEWFOLDERBUTTON),
	LABEL(BIF_NOTRANSLATETARGETS),
	LABEL(BIF_BROWSEFORCOMPUTER),
	LABEL(BIF_BROWSEFORPRINTER),
	LABEL(BIF_BROWSEINCLUDEFILES),
	LABEL(BIF_SHAREABLE),
	LABEL(BIF_BROWSEFILEJUNCTIONS)
};

std::vector<std::wstring> BIFFlagsToWString(UINT flags)
{
	std::vector<std::wstring> result;
	UINT mask = 1;
	for (int i = 0; i<31; i++) {
		UINT curFlag = (flags & mask);
		bool curFlagEnabled = curFlag > 0;
		if(curFlagEnabled)
		{
			auto labelsIter = labels.find(curFlag);
			result.push_back(labelsIter->second);
		}
		mask = mask << 1;
	}
	return result;
}

std::wstring string_to_wide_string(const std::string& str)
{
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.from_bytes(str);
}

std::string wide_string_to_string(const std::wstring& wstr)
{
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.to_bytes(wstr);
}
