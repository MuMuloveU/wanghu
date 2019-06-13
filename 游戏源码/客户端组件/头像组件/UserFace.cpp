#include "Stdafx.h"
#include <afxdllx.h>

//////////////////////////////////////////////////////////////////////////

static AFX_EXTENSION_MODULE UserFaceDLL = { NULL, NULL };

//DLL 导出主函数
extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);
	if (dwReason==DLL_PROCESS_ATTACH)
	{
		if (!AfxInitExtensionModule(UserFaceDLL, hInstance)) return 0;
		new CDynLinkLibrary(UserFaceDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		AfxTermExtensionModule(UserFaceDLL);
	}
	return 1;
}

//////////////////////////////////////////////////////////////////////////
