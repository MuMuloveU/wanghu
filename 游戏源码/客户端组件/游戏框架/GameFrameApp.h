#ifndef GAME_FRAME_APP_HEAD_FILE
#define GAME_FRAME_APP_HEAD_FILE

#pragma once

#include "GameFrame.h"
#include "GameFrameDlg.h"

//////////////////////////////////////////////////////////////////////////

//应用程序类
class GAME_FRAME_CLASS CGameFrameApp : public CWinApp
{
	//变量定义
private:
	CGameFrameDlg					* m_pGameFrame;					//框架窗口

	//函数定义
public:
	//构造函数
	CGameFrameApp();
	//析构函数
	virtual ~CGameFrameApp();

	//重载函数
public:
	//初始化函数
	virtual BOOL InitInstance();
	//退出消息
	virtual int ExitInstance();

	//继承函数
public:
	//创建主窗口
	virtual CGameFrameDlg * GetGameFrameDlg()=NULL;

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif