#include "Stdafx.h"
#include "Resource.h"
#include "GameFrameApp.h"
#include "GlobalOption.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameFrameApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameFrameApp::CGameFrameApp()
{
	m_pGameFrame=NULL;
}

//析构函数
CGameFrameApp::~CGameFrameApp()
{
}

//初始化函数
BOOL CGameFrameApp::InitInstance()
{
	__super::InitInstance();

	//内部初始化
	AfxOleInit();
	AfxInitRichEdit2();
	InitCommonControls();
	AfxEnableControlContainer();

	//加载资源
	if(g_GlobalOption.LoadGameFrameRes()==false)
	{
		ShowInformation(TEXT("加载资源失败!"),0,MB_ICONSTOP);
		return FALSE;
	}

	//加载配置
	g_GlobalOption.LoadOptionParameter();

	try
	{
		//创建主窗口
		m_pGameFrame=GetGameFrameDlg();
		ASSERT(m_pGameFrame!=NULL);
		if (m_pGameFrame==NULL) throw TEXT("游戏主窗口创建失败");

		//设置主窗口
		m_pMainWnd=m_pGameFrame;
		AfxSetResourceHandle(GetModuleHandle(GAME_FRAME_DLL_NAME));
		m_pGameFrame->Create(IDD_GAME_FRAME,NULL);
		AfxSetResourceHandle(GetModuleHandle(NULL));
	}
	catch (LPCTSTR pszMesssage)
	{
		CString strBuffer;
		strBuffer.Format(TEXT("由于 [ %s ] 游戏程序即将退出。"),pszMesssage);
		ShowInformation(strBuffer,0,MB_ICONSTOP);
		if (m_pGameFrame!=NULL) SafeDelete(m_pGameFrame);
		return FALSE;
	}

	return TRUE;
}

//退出消息
int CGameFrameApp::ExitInstance()
{
	//删除窗口
	if (m_pGameFrame!=NULL) SafeDelete(m_pGameFrame);
	return __super::ExitInstance();
}

//////////////////////////////////////////////////////////////////////////
