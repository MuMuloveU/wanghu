#pragma once

#ifndef __AFXWIN_H__
	#error 在包含用于 PCH 的此文件之前包含“stdafx.h”
#endif

#include "Resource.h"

//////////////////////////////////////////////////////////////////////////

//应用程序对象
class CCenterServerApp : public CWinApp
{
	//函数定义
public:
	//构造函数 
	CCenterServerApp();

	//重载函数
public:
	//初始化函数
	virtual BOOL InitInstance();

	//消息映射
protected:
	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
