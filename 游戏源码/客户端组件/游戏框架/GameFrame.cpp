#include "Stdafx.h"
#include <AfxDLLx.h>
#include "GameFrame.h"

//////////////////////////////////////////////////////////////////////////

ULONG_PTR							g_lGdiPlusToken=0L;					//GID+ 标识

static AFX_EXTENSION_MODULE GameFrameDLL={NULL,NULL};

//////////////////////////////////////////////////////////////////////////

//DLL 主函数
extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);
	if (dwReason==DLL_PROCESS_ATTACH)
	{
		if (!AfxInitExtensionModule(GameFrameDLL, hInstance)) return 0;
		new CDynLinkLibrary(GameFrameDLL);

		GdiplusStartupInput StartupInput;
		GdiplusStartup(&g_lGdiPlusToken,&StartupInput,NULL);
	}
	else if (dwReason==DLL_PROCESS_DETACH)
	{
		GdiplusShutdown(g_lGdiPlusToken);
		AfxTermExtensionModule(GameFrameDLL);
	}
	return 1;
}

//////////////////////////////////////////////////////////////////////////
