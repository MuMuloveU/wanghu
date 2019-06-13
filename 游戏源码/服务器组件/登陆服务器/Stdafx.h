#pragma once

//////////////////////////////////////////////////////////////////////////
//MFC 文件

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#ifndef WINVER
#define WINVER 0x0400
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif						

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0410
#endif

#ifndef _WIN32_IE
#define _WIN32_IE 0x0400
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS

#define _AFX_ALL_WARNINGS

#include <AfxWin.h>
#include <AfxExt.h>
#include <AfxDisp.h>
#include <AfxDtctl.h>

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <AfxCmn.h>
#endif

//////////////////////////////////////////////////////////////////////////

//全局文件
#include "..\..\公共文件\GlobalField.h"
#include "..\..\公共文件\GlobalService.h"

//组件头文件
#include "..\内核引擎\KernelEngineHead.h"

//消息定义
#include "..\..\消息定义\CMD_Plaza.h"
#include "..\..\消息定义\CMD_Center.h"

//////////////////////////////////////////////////////////////////////////
