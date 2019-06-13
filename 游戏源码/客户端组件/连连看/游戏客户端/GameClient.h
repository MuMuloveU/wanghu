#pragma once

#ifndef __AFXWIN_H__
	#error 在包含用于 PCH 的此文件之前包含“stdafx.h”
#endif

#include "Resource.h"
#include "GameClientDlg.h"

//////////////////////////////////////////////////////////////////////////

//应用程序类
class CGameClientApp : public CWinApp
{
private:
	CGameClientDlg					* m_pGameFrame;					//框架窗口
//	CSkinRecordFile					m_SkinRecordFile;				//界面设置

	//函数定义
public:
	//构造函数
	CGameClientApp();
	//析构函数
	virtual ~CGameClientApp();

	//继承函数
public:
	//创建主窗口
	virtual CGameClientDlg * GetGameFrameDlg() { return new CGameClientDlg; }

//重载函数
public:
	//初始化函数
	virtual BOOL InitInstance();
	//退出消息
	virtual int ExitInstance();
};

//应用程序对象
extern CGameClientApp theApp;

//////////////////////////////////////////////////////////////////////////
