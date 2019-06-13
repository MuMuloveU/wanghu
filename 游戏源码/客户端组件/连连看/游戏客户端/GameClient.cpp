#include "Stdafx.h"
#include "GameClient.h"
#include "GameClientDlg.h"
#include "GlobalOption.h"

//应用程序对象
CGameClientApp theApp;

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientApp::CGameClientApp()
{
	m_pGameFrame=NULL;
}

//析构函数
CGameClientApp::~CGameClientApp() 
{
}

//初始化函数
BOOL CGameClientApp::InitInstance()
{
	__super::InitInstance();

	//内部初始化
	AfxOleInit();
	AfxInitRichEdit2();
	InitCommonControls();
	AfxEnableControlContainer();

	//设置随机种子
	srand((UINT)time(NULL));

	//设置工作目录
	TCHAR szDirWork[MAX_PATH];
	GetModuleFileName(AfxGetInstanceHandle(),szDirWork,sizeof(szDirWork));
	int nModuleLen=lstrlen(szDirWork);
	int nProcessLen=lstrlen(AfxGetApp()->m_pszExeName)+lstrlen(TEXT(".EXE"));
	szDirWork[nModuleLen-nProcessLen]=0;
	SetCurrentDirectory(szDirWork);

	////初始化界面库
	//IUnknownEx * pIUnknownEx=(IUnknownEx *)m_SkinRecordFile.QueryInterface(IID_IUnknownEx,VER_IUnknownEx);
	//CSkinResourceManager::LoadSkinResource(pIUnknownEx);
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
		m_pGameFrame->Create(IDD_GAME_FRAME,NULL);
	}
	catch (LPCTSTR pszMesssage)
	{
		CString strBuffer;
		strBuffer.Format(TEXT("由于 [ %s ] 游戏程序即将退出。"),pszMesssage);
		AfxMessageBox(strBuffer,MB_ICONSTOP);
		if (m_pGameFrame!=NULL) SafeDelete(m_pGameFrame);
		return FALSE;
	}

	return TRUE;
}

//退出消息
int CGameClientApp::ExitInstance()
{
	//删除窗口
	if (m_pGameFrame!=NULL) SafeDelete(m_pGameFrame);
	return __super::ExitInstance();
}

//////////////////////////////////////////////////////////////////////////
