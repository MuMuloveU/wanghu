// GameClient.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error 在包含用于 PCH 的此文件之前包含“stdafx.h”
#endif

#include "resource.h"		// 主符号
#include "GameClientDlg.h"


// CGameClientApp:
// 有关此类的实现，请参阅 GameClient.cpp
//

class CGameClientApp : public CGameFrameApp
{
public:
	CGameClientApp();
	virtual ~CGameClientApp();

public:
	//创建主窗口
	virtual CGameFrameDlg * GetGameFrameDlg() 
	{ 
		return new CGameClientDlg; 
	}
};

extern CGameClientApp theApp;
