#include "Stdafx.h"
#include "CenterServer.h"
#include "CenterServerDlg.h"

//////////////////////////////////////////////////////////////////////////

//应用程序对象
CCenterServerApp theApp;

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CCenterServerApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

CCenterServerApp::CCenterServerApp()
{
}

//初始化函数
BOOL CCenterServerApp::InitInstance()
{
	__super::InitInstance();

	//初始化组件
	AfxInitRichEdit2();
	InitCommonControls();
	AfxEnableControlContainer();

	//设置注册表
	SetRegistryKey(szSystemRegKey);

	//显示主窗口
	CCenterServerDlg ServerDlg;
	m_pMainWnd=&ServerDlg;
	ServerDlg.DoModal();

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
