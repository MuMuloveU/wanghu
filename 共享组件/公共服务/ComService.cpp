#include "Stdafx.h"
#include <afxdllx.h>

//组件头文件
#include "ComService.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////

static AFX_EXTENSION_MODULE ComServiceDLL={NULL,NULL};

//DLL 导出主函数
extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		if (!AfxInitExtensionModule(ComServiceDLL,hInstance)) return 0;
		new CDynLinkLibrary(ComServiceDLL);
	}
	else if (dwReason==DLL_PROCESS_DETACH)
	{
		AfxTermExtensionModule(ComServiceDLL);
	}
	return 1;
}

//////////////////////////////////////////////////////////////////////////
