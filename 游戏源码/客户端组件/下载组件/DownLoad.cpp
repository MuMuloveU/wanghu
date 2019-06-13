#include "Stdafx.h"
#include <Afxdllx.h>

//组件头文件
#include "DownLoad.h"

//////////////////////////////////////////////////////////////////////////

static AFX_EXTENSION_MODULE DownLoadDLL={NULL,NULL};

//DLL 导出主函数
extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);
	if (dwReason==DLL_PROCESS_ATTACH)
	{
		if (!AfxInitExtensionModule(DownLoadDLL,hInstance)) return 0;
		new CDynLinkLibrary(DownLoadDLL);
	}
	else if (dwReason==DLL_PROCESS_DETACH)
	{
		AfxTermExtensionModule(DownLoadDLL);
	}
	return 1;
}

//////////////////////////////////////////////////////////////////////////
