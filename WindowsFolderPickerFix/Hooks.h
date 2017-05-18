#pragma once

#include <shlobj.h>

//Declare SHBrowseForFolder function types
typedef PIDLIST_ABSOLUTE(WINAPI *SHBrowseForFolderW_Func)(LPBROWSEINFOW lpbi);
typedef PIDLIST_ABSOLUTE(WINAPI *SHBrowseForFolderA_Func)(LPBROWSEINFOA lpbi);

//Functions pointers to the real, original Win32 SHBrowseForFolder functions
extern SHBrowseForFolderW_Func SHBrowseForFolderW_Original;
extern SHBrowseForFolderA_Func SHBrowseForFolderA_Original;

//Detour functions
PIDLIST_ABSOLUTE WINAPI DetourSHBrowseForFolderW(LPBROWSEINFOW lpbi);
PIDLIST_ABSOLUTE WINAPI DetourSHBrowseForFolderA(LPBROWSEINFOA lpbi);