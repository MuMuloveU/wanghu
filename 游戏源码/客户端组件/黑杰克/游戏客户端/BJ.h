
#pragma once

#ifndef __AFXWIN_H__
	#error 在包含用于 PCH 的此文件之前包含“stdafx.h”
#endif

#include "Resource.h"
#include "BJGameDlg.h"

//////////////////////////////////////////////////////////////////////////

//应用程序类
class CBJApp : public CGameFrameApp
{
	//函数定义
public:
	//构造函数
	CBJApp();
	//析构函数
	virtual ~CBJApp();

	//继承函数
public:
	//创建主窗口
	virtual CGameFrameDlg * GetGameFrameDlg() { return new CBJGameDlg; }
	
};

//应用程序对象
extern CBJApp theApp;

