#ifndef COM_SERVICE_HEAD_FILE
#define COM_SERVICE_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//系统头文件
#include "AfxHtml.h"

//平台头文件
#include "..\..\模板库\Template.h"

//////////////////////////////////////////////////////////////////////////
//公共宏定义

//导出定义
#ifndef COM_SERVICE_CLASS
	#ifdef  COM_SERVICE_DLL
		#define COM_SERVICE_CLASS _declspec(dllexport)
	#else
		#define COM_SERVICE_CLASS _declspec(dllimport)
	#endif
#endif

//模块定义
#ifdef _DEBUG
	#define COM_SERVICE_DLL_NAME	TEXT("ComServiceD.dll")			//组件 DLL 名字
#else
	#define COM_SERVICE_DLL_NAME	TEXT("ComService.dll")			//组件 DLL 名字
#endif

//////////////////////////////////////////////////////////////////////////

//导出类头文件
#include "Encrypt.h"
#include "DataQueue.h"
#include "WebBrowser.h"
#include "HtmlBrowser.h"
#include "WinFileInfo.h"
#include "ServiceThread.h"

//////////////////////////////////////////////////////////////////////////

#endif