#include "Stdafx.h"
#include "GameClient.h"
#include "GameClientDlg.h"

//应用程序对象
CGameClientApp theApp;
#ifdef VIDEO_GAME
//视频对象
CVideoServiceManager g_VedioServiceManager;
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

//////////////////////////////////////////////////////////////////////////
