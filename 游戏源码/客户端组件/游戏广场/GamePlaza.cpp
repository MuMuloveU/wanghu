#include "Stdafx.h"
#include "GamePlaza.h"
#include "GameFrame.h"
#include "GlobalUnits.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGamePlazaApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//应用程序对象说明
CGamePlazaApp theApp;

//构造函数
CGamePlazaApp::CGamePlazaApp()
{
}

//初始化函数
BOOL CGamePlazaApp::InitInstance()
{
	__super::InitInstance();

	//判断是否重入
#ifndef ADMIN_PLAZA
#ifndef _DEBUG
	bool bExistIGame=false;
	CMutex Mutex(FALSE,MAIN_DLG_CLASSNAME,NULL);
	if (Mutex.Lock(0)==FALSE) bExistIGame=true;
	CWnd * pIGameWnd=CWnd::FindWindow(MAIN_DLG_CLASSNAME,NULL);
	if (pIGameWnd!=NULL) 
	{
		bExistIGame=true;
		if (pIGameWnd->IsIconic()) pIGameWnd->ShowWindow(SW_RESTORE);
		pIGameWnd->SetActiveWindow();
		pIGameWnd->BringWindowToTop();
		pIGameWnd->SetForegroundWindow();
	}
	if (bExistIGame==true) return FALSE;
#endif
#endif

	//程序初始化
	AfxOleInit();
	AfxInitRichEdit();
	AfxInitRichEdit2();
	InitCommonControls();
	AfxEnableControlContainer();

	//设置注册表
	SetRegistryKey(szSystemRegKey);

	////加载界面库
	//CSkinAttribute::LoadSkinAttribute(QUERY_OBJECT_INTERFACE(m_SkinRecordFile,IUnknownEx));

	//加载配置
	g_GlobalOption.LoadOptionParameter();

	//全局信息
	if (g_GlobalUnits.InitGlobalUnits()==false)
	{
		ShowInformation(TEXT("游戏广场初始化失败，程序即将退出！"),0,MB_ICONSTOP);
		return FALSE;
	}

	//变量定义
	WNDCLASS WndClasss;
	ZeroMemory(&WndClasss,sizeof(WndClasss));

	//注册窗口
	WndClasss.style=CS_DBLCLKS;
	WndClasss.lpfnWndProc=DefWindowProc;
	WndClasss.lpszClassName=MAIN_DLG_CLASSNAME;
	WndClasss.hInstance=AfxGetInstanceHandle();
	WndClasss.hIcon=LoadIcon(MAKEINTRESOURCE(IDR_MAINFRAME));
	WndClasss.hCursor=LoadStandardCursor(MAKEINTRESOURCE(IDC_ARROW));
	if (AfxRegisterClass(&WndClasss)==FALSE) AfxThrowResourceException();

	//建立窗口
	CGameFrame * pMainFrame=new CGameFrame();
	pMainFrame->Create(MAIN_DLG_CLASSNAME,TEXT(""),WS_MINIMIZEBOX|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,CRect(0,0,0,0));
	m_pMainWnd=pMainFrame;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

