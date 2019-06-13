#include "Stdafx.h"
#include <Afxdllx.h>

//组件头文件
#include "ListService.h"

//////////////////////////////////////////////////////////////////////////

static AFX_EXTENSION_MODULE ListServiceDLL={NULL,NULL};

//DLL 导出主函数
extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);
	if (dwReason==DLL_PROCESS_ATTACH)
	{
		if (!AfxInitExtensionModule(ListServiceDLL,hInstance)) return 0;
		new CDynLinkLibrary(ListServiceDLL);
	}
	else if (dwReason==DLL_PROCESS_DETACH)
	{
		AfxTermExtensionModule(ListServiceDLL);
	}
	return 1;
}

//////////////////////////////////////////////////////////////////////////
