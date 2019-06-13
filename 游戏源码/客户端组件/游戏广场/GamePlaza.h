#pragma once

#ifndef __AFXWIN_H__
	#error 在包含用于 PCH 的此文件之前包含“stdafx.h”
#endif

#include "GlobalUnits.h"

//////////////////////////////////////////////////////////////////////////

//应用程序对象
class CGamePlazaApp : public CWinApp
{
//	//变量定义
//public:
//	CSkinRecordFile							m_SkinRecordFile;				//界面设置

	//函数定义
public:
	//构造函数
	CGamePlazaApp();

	//重载函数
public:
	//初始化函数
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//应用程序对象说明
extern CGamePlazaApp theApp;

//////////////////////////////////////////////////////////////////////////
