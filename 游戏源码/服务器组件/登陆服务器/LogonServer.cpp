#include "Stdafx.h"
#include "LogonServer.h"
#include "LogonServerDlg.h"

//////////////////////////////////////////////////////////////////////////

//应用程序对象
CLogonServerApp theApp;

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CLogonServerApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CLogonServerApp::CLogonServerApp()
{
}

//初始化函数
BOOL CLogonServerApp::InitInstance()
{
	__super::InitInstance();

	//初始化组件
	AfxInitRichEdit2();
	InitCommonControls();
	AfxEnableControlContainer();

	//设置注册表
	SetRegistryKey(szSystemRegKey);

	//获取目录
	GetModuleFileName(AfxGetInstanceHandle(),m_szDirWork,sizeof(m_szDirWork));
	int nModuleLen=lstrlen(m_szDirWork);
	int nProcessLen=lstrlen(m_pszExeName)+lstrlen(TEXT(".EXE"));
	m_szDirWork[nModuleLen-nProcessLen]=0;

	//创建目录
	lstrcat(m_szDirWork, TEXT("CustomFace"));
	CreateDirectory(m_szDirWork, NULL) ;

	//显示主窗口
	CLogonServerDlg ServerDlg;
	m_pMainWnd=&ServerDlg;
	ServerDlg.DoModal();

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
