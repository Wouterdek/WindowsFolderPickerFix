#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlobj.h>
#include <cstdio>
#include <iostream>

//https://msdn.microsoft.com/en-us/library/windows/desktop/bb762598(v=vs.85).aspx
int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData) //lpData = BROWSEINFO.lParam
{
	switch(uMsg)
	{
	case BFFM_INITIALIZED:
		//lParam is NULL
	case BFFM_IUNKNOWN:
		//lParam is pointer to available IUnknown
	case BFFM_SELCHANGED:
		//lParam is a PIDL that identifies the newly selected item.
	case BFFM_VALIDATEFAILED:
		//lParam is a pointer to a string that contains the invalid name. An application can use this data in an error dialog informing the user that the name was not valid.
		//Return 0 to dismiss dialog, nonzero to keep dialog displayed.
		break;
	}

	return 0;
}

void OpenOldFolderPicker()
{
	wchar_t filepath[MAX_PATH]; //Chosen folder goes in here

	BROWSEINFO info = {};
	info.hwndOwner = NULL; //Owner window
	info.pidlRoot = NULL;
	info.lpszTitle = L"User instructions go here."; //Displayed at top of dialog
	info.pszDisplayName = filepath; //Set result buffer
	//info.ulFlags = BIF_RETURNONLYFSDIRS  | BIF_NEWDIALOGSTYLE; //Settings
	//info.lpfn = BrowseCallbackProc;
	info.lParam = NULL;
	info.iImage = NULL;
	LPITEMIDLIST result = SHBrowseForFolder(&info);
	if(result != NULL)
	{
		wprintf(L"Chosen folder: %s\n", filepath);
		wchar_t* buffer = new wchar_t[MAX_PATH];
		SHGetPathFromIDList(result, buffer);
		wprintf(L"Full path: %s\n", buffer);
		delete[] buffer;
	}
}

void OpenNewFolderPicker()
{
	CoInitialize(NULL);
	IFileOpenDialog *pfd;
	HRESULT result = CoCreateInstance(__uuidof(FileOpenDialog), NULL, CLSCTX_ALL, __uuidof(pfd), reinterpret_cast<void**>(&pfd));
	if (SUCCEEDED(result))
	{
		pfd->SetTitle(L"A Single-Selection Dialog");

		DWORD dwOptions;
		if (SUCCEEDED(pfd->GetOptions(&dwOptions)))
		{
			pfd->SetOptions(dwOptions | FOS_PICKFOLDERS);
		}
		else
		{
			MessageBox(NULL, L"GetOptions() failed", NULL, NULL);
		}

		if (SUCCEEDED(pfd->Show(NULL)))
		{
			IShellItem *psi;
			if (SUCCEEDED(pfd->GetResult(&psi)))
			{
				LPOLESTR g_path = NULL;
				if (!SUCCEEDED(psi->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &g_path)))
				{
					MessageBox(NULL, L"GetIDListName() failed", NULL, NULL);
				}
				psi->Release();
			}
		}
		pfd->Release();
	}else
	{
		printf("%lu", result);
		MessageBox(NULL, L"CoCreateInstance() failed", NULL, NULL);
	}
	CoUninitialize();
}

int main()
{
	//LoadLibrary(L"WindowsFolderPickerFix.dll");
	OpenOldFolderPicker();
	OpenNewFolderPicker();
}

