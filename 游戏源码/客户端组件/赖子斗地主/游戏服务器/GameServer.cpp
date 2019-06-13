#include "Stdafx.h"
#include <afxdllx.h>

//////////////////////////////////////////////////////////////////////////

static AFX_EXTENSION_MODULE GameServerDLL={NULL,NULL};

//DLL 导出主函数
extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);
	if (dwReason==DLL_PROCESS_ATTACH)
	{
		if (!AfxInitExtensionModule(GameServerDLL, hInstance)) return 0;
		new CDynLinkLibrary(GameServerDLL);
	}
	else if (dwReason==DLL_PROCESS_DETACH)
	{
		AfxTermExtensionModule(GameServerDLL);
	}
	return 1;
}

//////////////////////////////////////////////////////////////////////////
