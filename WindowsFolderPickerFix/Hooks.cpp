#include "Hooks.h"
#include "Tools.h"

#pragma warning(disable: 4996)

/*
 	General strategy: Try to use new IFileDialog dialog, fall back to old SHBrowseForFolder dialog if compatibility is not guaranteed.
    If the old dialog is used, we can at least inject the BIF_USENEWUI flag into ulFlags for a better UI experience. (Although this cannot be done if CoInitializeEx was run with COINIT_MULTITHREADED)

  	=> Check lpfn
 		-> lpfn != NULL? display the old dialog. (Or implement a wrapper if possible?)
 	=> Check ulFlags
 		-> are incompatible flags present? show old dialog
 	=> Show new dialog

	current state ulFlags:
		BIF_RETURNONLYFSDIRS
			=> UNSUPPORTED
		BIF_DONTGOBELOWDOMAIN
			=> UNSUPPORTED
		BIF_STATUSTEXT
			=> UNSUPPORTED
		BIF_RETURNFSANCESTORS
			=> UNSUPPORTED
		BIF_EDITBOX
			=> COMPATIBLE
		BIF_VALIDATE
			=> UNSUPPORTED
		BIF_NEWDIALOGSTYLE
			=> COMPATIBLE
		BIF_BROWSEINCLUDEURLS
			=> UNSUPPORTED
		BIF_UAHINT
			=> UNSUPPORTED
		BIF_NONEWFOLDERBUTTON
			=> UNSUPPORTED
		BIF_NOTRANSLATETARGETS
			=> UNSUPPORTED
		BIF_BROWSEFORCOMPUTER
			=> UNSUPPORTED
		BIF_BROWSEFORPRINTER
			=> UNSUPPORTED
		BIF_BROWSEINCLUDEFILES
			=> UNSUPPORTED
		BIF_SHAREABLE
			=> UNSUPPORTED
		BIF_BROWSEFILEJUNCTIONS
			=> UNSUPPORTED
*/

//All flags that are compatible with the modern dialog
UINT CompatibleFlags = BIF_EDITBOX & BIF_NEWDIALOGSTYLE;

PIDLIST_ABSOLUTE ShowModernDialog(LPBROWSEINFOW lpbiw, LPBROWSEINFOA lpbia)
{
	PIDLIST_ABSOLUTE returnVal = NULL;

	HRESULT initResult = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	IFileOpenDialog *pfd;
	HRESULT result = CoCreateInstance(__uuidof(FileOpenDialog), NULL, CLSCTX_ALL, __uuidof(pfd), reinterpret_cast<void**>(&pfd));
	if (SUCCEEDED(result))
	{
		UINT flags;
		HWND parentWindow = NULL;
		LPCITEMIDLIST pidlRoot = NULL;
		if(lpbiw != NULL)
		{
			if (lpbiw->lpszTitle != NULL)
			{
				pfd->SetTitle(lpbiw->lpszTitle);
			}
			flags = lpbiw->ulFlags;
			parentWindow = lpbiw->hwndOwner;
			pidlRoot = lpbiw->pidlRoot;
		}else
		{
			std::wstring title = string_to_wide_string(lpbia->lpszTitle);
			pfd->SetTitle(title.c_str()); //TODO: is it safe to pass this pointer? If the string is not copied, this might be dangerous

			flags = lpbia->ulFlags;
			parentWindow = lpbia->hwndOwner;
			pidlRoot = lpbia->pidlRoot;
		}

		/*if (pidlRoot != NULL)
		{
			LPSHELLFOLDER psfDesktop = NULL; // namespace root for parsing the path
			result = SHGetDesktopFolder(&psfDesktop);

			//LPSHELLFOLDER psfFolder = NULL;
			//result = psfDesktop->BindToObject(lpbiw->pidlRoot, NULL, IID_IShellFolder, (void**)&psfFolder);

			IShellItem* psfFolder;
			result = psfDesktop->BindToObject(lpbiw->pidlRoot, NULL, IID_IShellItem, (void**)&psfFolder);

			pfd->SetDefaultFolder(psfFolder);
		}*/

		DWORD dwOptions;
		if (SUCCEEDED(pfd->GetOptions(&dwOptions)))
		{
			dwOptions |= FOS_PICKFOLDERS;

			/*if(flags & BIF_RETURNONLYFSDIRS != 0)
			{
				dwOptions |= FOS_FORCEFILESYSTEM;
			}*/

			pfd->SetOptions(dwOptions);
		}
		else
		{
			MessageBox(NULL, L"GetOptions() failed", NULL, NULL);
		}

		if (SUCCEEDED(pfd->Show(parentWindow)))
		{
			IShellItem *psi;
			if (SUCCEEDED(pfd->GetResult(&psi)))
			{
				LPOLESTR path = NULL;
				if (SUCCEEDED(psi->GetDisplayName(SIGDN_NORMALDISPLAY, &path)))
				{
					if (lpbiw != NULL && lpbiw->pszDisplayName != NULL)
					{
						wcsncpy_s(lpbiw->pszDisplayName, MAX_PATH, path, MAX_PATH);
						lpbiw->pszDisplayName[MAX_PATH - 1] = 0;
					}
					else if (lpbia->pszDisplayName != NULL)
					{
						memset(lpbia->pszDisplayName, 0, MAX_PATH);
						wide_string_to_string(path).copy(lpbia->pszDisplayName, MAX_PATH);
						lpbia->pszDisplayName[MAX_PATH - 1] = 0;
					}
					CoTaskMemFree(path);

					if (!SUCCEEDED(SHGetIDListFromObject(psi, &returnVal)))
					{
						MessageBox(NULL, L"SHGetIDListFromObject() failed", NULL, NULL);
					}
				}else
				{
					MessageBox(NULL, L"GetDisplayName() failed", NULL, NULL);
				}

				psi->Release();
			}
			else
			{
				MessageBox(NULL, L"GetResult() failed", NULL, NULL);
			}
		}
		pfd->Release();
	}
	else
	{
		MessageBox(NULL, L"CoCreateInstance() failed", NULL, NULL);
	}

	if (initResult != RPC_E_CHANGED_MODE && SUCCEEDED(initResult))
	{
		CoUninitialize(); //All successfull CoInitializeEx calls must be balanced by a corresponding CoUninitialize
	}

	return returnVal;
}

PIDLIST_ABSOLUTE ShowOldDialog(LPBROWSEINFOW lpbiw, LPBROWSEINFOA lpbia)
{
	HRESULT initResult = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	PIDLIST_ABSOLUTE returnVal;
	if(lpbiw != NULL)
	{
		if (initResult != RPC_E_CHANGED_MODE) //If setting COINIT_APARTMENTTHREADED did not fail
		{
			lpbiw->ulFlags |= BIF_USENEWUI;//Enable slightly better ui
		}
		returnVal = SHBrowseForFolderW_Original(lpbiw);
	}else
	{
		if (initResult != RPC_E_CHANGED_MODE) //If setting COINIT_APARTMENTTHREADED did not fail
		{
			lpbia->ulFlags |= BIF_USENEWUI;//Enable slightly better ui
		}
		returnVal = SHBrowseForFolderA_Original(lpbia);
	}
	
	if(initResult != RPC_E_CHANGED_MODE && SUCCEEDED(initResult))
	{
		CoUninitialize(); //All successfull CoInitializeEx calls must be balanced by a corresponding CoUninitialize
	}

	return returnVal;
}

PIDLIST_ABSOLUTE ShowDialog(LPBROWSEINFOW lpbiw, LPBROWSEINFOA lpbia)
{
	BFFCALLBACK lpfn = (lpbiw != NULL) ? lpbiw->lpfn : lpbia->lpfn;
	UINT ulFlags = (lpbiw != NULL) ? lpbiw->ulFlags : lpbia->ulFlags;
	LPCITEMIDLIST pidlroot = (lpbiw != NULL) ? lpbiw->pidlRoot : lpbia->pidlRoot;

	if (lpfn != NULL || pidlroot != NULL || (ulFlags & ~CompatibleFlags) != 0)
	{
		return ShowOldDialog(lpbiw, lpbia);
	}
	return ShowModernDialog(lpbiw, lpbia);
}

PIDLIST_ABSOLUTE WINAPI DetourSHBrowseForFolderW(LPBROWSEINFOW lpbi)
{
	return ShowDialog(lpbi, NULL);
}

PIDLIST_ABSOLUTE WINAPI DetourSHBrowseForFolderA(LPBROWSEINFOA lpbi)
{
	return ShowDialog(NULL, lpbi);
}