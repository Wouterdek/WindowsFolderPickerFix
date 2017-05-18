#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "MinHook.h"
#include "Hooks.h"

SHBrowseForFolderW_Func SHBrowseForFolderW_Original = NULL;
SHBrowseForFolderA_Func SHBrowseForFolderA_Original = NULL;

int CreateHook()
{
	// Initialize MinHook.
	if (MH_Initialize() != MH_OK)
	{
		return 1;
	}

	// Create a hook for SHBrowseForFolderW, in disabled state.
	if (MH_CreateHook(&SHBrowseForFolderW, &DetourSHBrowseForFolderW, reinterpret_cast<LPVOID*>(&SHBrowseForFolderW_Original)) != MH_OK)
	{
		return 1;
	}

	// Create a hook for SHBrowseForFolderA, in disabled state.
	if (MH_CreateHook(&SHBrowseForFolderA, &DetourSHBrowseForFolderA, reinterpret_cast<LPVOID*>(&SHBrowseForFolderA_Original)) != MH_OK)
	{
		return 1;
	}

	return 0;
}

int EnableHook()
{
	// Enable the hook for SHBrowseForFolderW.
	if (MH_EnableHook(&SHBrowseForFolderW) != MH_OK)
	{
		return 1;
	}

	// Enable the hook for SHBrowseForFolderA.
	if (MH_EnableHook(&SHBrowseForFolderA) != MH_OK)
	{
		return 1;
	}

	return 0;
}

int DisableHook()
{
	// Disable the hook for SHBrowseForFolderW.
	if (MH_DisableHook(&SHBrowseForFolderW) != MH_OK)
	{
		return 1;
	}

	// Disable the hook for SHBrowseForFolderA.
	if (MH_DisableHook(&SHBrowseForFolderA) != MH_OK)
	{
		return 1;
	}

	// Uninitialize MinHook.
	if (MH_Uninitialize() != MH_OK)
	{
		return 1;
	}

	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		CreateHook();
		EnableHook();
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		DisableHook();
		break;
	}
	return TRUE;
}

