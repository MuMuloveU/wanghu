#include "stdafx.h"
#include <afxdllx.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static AFX_EXTENSION_MODULE VideoServiceDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("VideoService.DLL 正在初始化！\n");
		if (!AfxInitExtensionModule(VideoServiceDLL, hInstance))
			return 0;

		new CDynLinkLibrary(VideoServiceDLL);

	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("VideoService.DLL 正在终止！\n");
		AfxTermExtensionModule(VideoServiceDLL);
	}
	return 1; 
}
