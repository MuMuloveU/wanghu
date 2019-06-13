#include "Stdafx.h"
#include "GameClient.h"
#include "GameClientDlg.h"

//////////////////////////////////////////////////////////////////////////

//程序对象
CGameClientApp theApp;
#ifdef VIDEO_GAME
//视频对象
CVideoServiceManager g_VideoServiceManager;
#endif
//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientApp::CGameClientApp()
{
}

//析构函数
CGameClientApp::~CGameClientApp()
{
}

////创建窗口
//CGameFrameWnd * CGameClientApp::GetGameFrameWnd()
//{
//	return new CGameClientWnd;
//}
//
////创建引擎
//CGameFrameEngine * CGameClientApp::GetGameFrameEngine(DWORD dwSDKVersion)
//{
//	//版本检测
//	if (InterfaceVersionCompare(VERSION_FRAME_SDK,dwSDKVersion)==false) return NULL;
//
//	//创建引擎
//	return new CGameClientDlg;
//}

//////////////////////////////////////////////////////////////////////////
