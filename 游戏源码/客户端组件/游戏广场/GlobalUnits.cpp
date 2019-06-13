#include "StdAfx.h"
#include "Mmsystem.h"
#include "GlobalUnits.h"


//////////////////////////////////////////////////////////////////////////

//下载类型
#define DTP_GAME_CLIENT			1									//下载类型

//状态信息
struct tagAstatInfo
{
	ADAPTER_STATUS				AdapterStatus;						//网卡状态
	NAME_BUFFER					NameBuff[16];						//名字缓冲
};

//全局变量
CGlobalUnits					g_GlobalUnits;						//信息组件			
CGlobalAttemper					g_GlobalAttemper;					//全局调度

//////////////////////////////////////////////////////////////////////////

//构造函数
CGlobalUnits::CGlobalUnits(void)
{
	m_szDirSkin[0]=0;
	m_szDirWork[0]=0;
	m_dwPlazaVersion=VER_PLAZA_FRAME;
	memset(&m_GloblaUserData,0,sizeof(m_GloblaUserData));
	lstrcpyn(m_szStationPage,szStationPage,sizeof(m_szStationPage));

	//资源变量
	ZeroMemory(&m_PlazaViewImage,sizeof(m_PlazaViewImage));
	ZeroMemory(&m_ServerViewImage,sizeof(m_ServerViewImage));
	ZeroMemory(&m_WhisperViewImage,sizeof(m_WhisperViewImage));
	ZeroMemory(&m_ControlViewImage,sizeof(m_ControlViewImage));
	ZeroMemory(&m_PlatformFrameImage,sizeof(m_PlatformFrameImage));
	ZeroMemory(&m_ChatExpViewImage,sizeof(m_ChatExpViewImage));
	ZeroMemory(&m_PropertyViewImage,sizeof(m_PropertyViewImage));

	return;
}

//析构函数
CGlobalUnits::~CGlobalUnits(void)
{
}

//初始化函数
bool CGlobalUnits::InitGlobalUnits()
{
	//设置随机种子
	srand((UINT)time(NULL));

	//设置工作目录
	GetModuleFileName(AfxGetInstanceHandle(),m_szDirWork,sizeof(m_szDirWork));
	int nModuleLen=lstrlen(m_szDirWork);
	int nProcessLen=lstrlen(AfxGetApp()->m_pszExeName)+lstrlen(TEXT(".EXE"));
	m_szDirWork[nModuleLen-nProcessLen]=0;
	SetCurrentDirectory(m_szDirWork);

	//设置界面目录
	_snprintf(m_szDirSkin,sizeof(m_szDirSkin),TEXT("%s\\Skin"),m_szDirWork);

	//加载组件
	if (m_UserFaceRes.CreateInstance()==false) return false;
	if (m_DownLoadService.CreateInstance()==false) return false;
	if (m_CompanionManager.CreateInstance()==false) return false;
	if (m_PlatformResourceModule.CreateInstance()==false) return false;

	//获取资源
	m_PlatformResourceModule->GetPlazaViewImage(&m_PlazaViewImage);
	m_PlatformResourceModule->GetServerViewImage(&m_ServerViewImage);
	m_PlatformResourceModule->GetWhisperViewImage(&m_WhisperViewImage);
	m_PlatformResourceModule->GetControlViewImage(&m_ControlViewImage);
	m_PlatformResourceModule->GetPlatformFrameImage(&m_PlatformFrameImage);
	m_PlatformResourceModule->GetChatExpViewImage(&m_ChatExpViewImage);
	m_PlatformResourceModule->GetPropertyViewImage(&m_PropertyViewImage);

	//控件资源
	//IPlatformResource * pIPlatformResource=m_PlatformResourceModule.GetInterface();
	ISkinResource * pISkinResource=(ISkinResource *)m_PlatformResourceModule->GetSkinResource(IID_ISkinResource,VER_ISkinResource);
	if (CSkinResourceManager::LoadSkinResource(pISkinResource)==false) return false;

	//初始化组件
	m_CompanionManager->LoadCompanion();

	return true;
}

//拷贝字符
bool CGlobalUnits::CopyToClipboard(LPCTSTR pszString)
{
	//变量定义
	HWND hWnd=AfxGetMainWnd()->GetSafeHwnd();

	//打开剪切板
	if (OpenClipboard(hWnd)==FALSE) return false;
	if (EmptyClipboard()==FALSE) { CloseClipboard(); return false; }

	//复制数据
	HANDLE hData=GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT,CountStringBuffer(pszString));
	if (hData==NULL) 
	{
		CloseClipboard();
		return false;
	}

	//复制数据
	LPSTR pszMemString=(LPSTR)GlobalLock(hData);
	lstrcpy(pszMemString,pszString);
	SetClipboardData(CF_TEXT,hData);

	//环境设置
	GlobalUnlock(hData);
	CloseClipboard();

	return true;
}

//获取序列
void CGlobalUnits::GetClientSerial(tagClientSerial & ClientSerial)
{
	//获取版本
	OSVERSIONINFO OSVerInfo;
	OSVerInfo.dwOSVersionInfoSize=sizeof(OSVerInfo);
	GetVersionEx(&OSVerInfo);
	ClientSerial.dwSystemVer=MAKELONG(OSVerInfo.dwMajorVersion,OSVerInfo.dwMinorVersion);

	//网卡标识
	ZeroMemory(ClientSerial.dwComputerID,sizeof(ClientSerial.dwComputerID));
	WORD wMacSize=GetMACAddress((BYTE *)ClientSerial.dwComputerID,sizeof(ClientSerial.dwComputerID));

	//硬盘标识
	WORD wIndex=(wMacSize+sizeof(DWORD)-1)/sizeof(DWORD);
	LPCTSTR pszHardDisk[]={TEXT("C:\\"),TEXT("D:\\"),TEXT("E:\\")};
	for (WORD i=wIndex;i<CountArray(ClientSerial.dwComputerID);i++)
	{
		ASSERT(CountArray(pszHardDisk)>(i-wIndex));
		GetVolumeInformation(pszHardDisk[i-wIndex],NULL,0,&ClientSerial.dwComputerID[i],NULL,NULL,0,NULL);
	}

	return;
}

//设置主站地址
LPCTSTR CGlobalUnits::SetStationPage(LPCTSTR pszStationPage)
{
	ASSERT(pszStationPage!=NULL);
	lstrcpyn(m_szStationPage,pszStationPage,sizeof(m_szStationPage));
	return m_szStationPage;
}

//记录甜饼
bool CGlobalUnits::WriteUserCookie()
{
	//变量定义
	TCHAR szTokenData[128]=TEXT("");
	INT nCharCount=sizeof(szTokenData)/sizeof(szTokenData[0]);

	//用户 I D
	_sntprintf(szTokenData,nCharCount,TEXT("%ld"),m_GloblaUserData.dwUserID);
	CInternetSession::SetCookie(szCookieUrl,TEXT("UserID"),szTokenData);

	//游戏 I D
	_sntprintf(szTokenData,nCharCount,TEXT("%ld"),m_GloblaUserData.dwGameID);
	CInternetSession::SetCookie(szCookieUrl,TEXT("GameID"),szTokenData);

	//用户帐号
	CInternetSession::SetCookie(szCookieUrl,TEXT("Accounts"),m_GloblaUserData.szAccounts);

	//用户密码
	CInternetSession::SetCookie(szCookieUrl,TEXT("Password"),m_GloblaUserData.szPassWord);

	return true;
}

//删除甜饼
bool CGlobalUnits::DeleteUserCookie()
{
	//删除信息
	CInternetSession::SetCookie(szCookieUrl,TEXT("UserID"),TEXT(""));
	CInternetSession::SetCookie(szCookieUrl,TEXT("GameID"),TEXT(""));
	CInternetSession::SetCookie(szCookieUrl,TEXT("Accounts"),TEXT(""));
	CInternetSession::SetCookie(szCookieUrl,TEXT("Password"),TEXT(""));
	CInternetSession::SetCookie(szCookieUrl,TEXT("UserToken"),TEXT(""));

	return true;
}

//网卡地址
WORD CGlobalUnits::GetMACAddress(BYTE cbMacBuffer[], WORD wBufferSize)
{
	//变量定义
	WORD wMacSize=0;
	HINSTANCE hInstance=NULL;
	typedef BYTE __stdcall NetBiosProc(NCB * Ncb);

	try
	{
		//加载 DLL
		hInstance=LoadLibrary(TEXT("NetApi32.dll"));
		if (hInstance==NULL) throw TEXT("加载 NetApi32.dll 失败");

		//获取函数
		NetBiosProc * pNetBiosProc=(NetBiosProc *)GetProcAddress(hInstance,"Netbios");
		if (pNetBiosProc==NULL) throw TEXT("无法找到 NetBios 函数");

		//变量定义
		NCB Ncb;
		LANA_ENUM LanaEnum;
		ZeroMemory(&Ncb,sizeof(Ncb));
		ZeroMemory(&LanaEnum,sizeof(LanaEnum));

		//枚举网卡
		Ncb.ncb_command=NCBENUM;
		Ncb.ncb_length=sizeof(LanaEnum);
		Ncb.ncb_buffer=(BYTE *)&LanaEnum;
		if (pNetBiosProc(&Ncb)!=NRC_GOODRET) throw TEXT("枚举网卡失败");
		if (LanaEnum.length==0) throw TEXT("找不到任何网卡");

		//获取地址
		tagAstatInfo Adapter;
		for (BYTE i=0;i<LanaEnum.length;i++)
		{
			//重置网卡
			ZeroMemory(&Ncb,sizeof(Ncb));
			Ncb.ncb_command=NCBRESET;
			Ncb.ncb_lana_num=LanaEnum.lana[i];
			if (pNetBiosProc(&Ncb)!=NRC_GOODRET) throw TEXT("重置网卡失败");

			//获取状态
			ZeroMemory(&Ncb,sizeof(Ncb));
			Ncb.ncb_command=NCBASTAT;
			Ncb.ncb_length=sizeof(Adapter);
			Ncb.ncb_buffer=(BYTE *)&Adapter;
			Ncb.ncb_lana_num=LanaEnum.lana[i];
			strcpy((char *)Ncb.ncb_callname,"*");
			if (pNetBiosProc(&Ncb)!=NRC_GOODRET) throw TEXT("获取网卡状态失败");

			//获取地址
			if (wMacSize<wBufferSize)
			{
				//拷贝地址
				WORD wCopySize=__min((wBufferSize-wMacSize),sizeof(BYTE)*6);
				CopyMemory(cbMacBuffer,Adapter.AdapterStatus.adapter_address,wCopySize);

				//完成判断
				wMacSize+=wCopySize;
				if (wMacSize==wBufferSize) break;
			}
		}

		//释放资源
		if (hInstance!=NULL)
		{
			FreeLibrary(hInstance);
			hInstance=NULL;
		}

		return wMacSize;
	}
	catch (...) 
	{ 
		//释放资源
		if (hInstance!=NULL)
		{
			FreeLibrary(hInstance);
			hInstance=NULL;
		}
	}

	return wMacSize;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CGlobalAttemper::CGlobalAttemper(void)
{
}

//析构函数
CGlobalAttemper::~CGlobalAttemper(void)
{
}

//显示提示
bool CGlobalAttemper::ShowStatusMessage(LPCTSTR pszMessage, CWnd * pNotifyWnd)
{
	return m_DlgStatus.ShowStatusMessage(pszMessage,pNotifyWnd);
}

//关闭状态
bool CGlobalAttemper::DestroyStatusWnd(CWnd * pNotifyWnd)
{
	return m_DlgStatus.DestroyStatusWnd(pNotifyWnd);
}

//下载游戏
bool CGlobalAttemper::DownLoadClient(LPCTSTR pszKindName, WORD wKindID, bool bDisplay)
{
	//构造数据
	tagDownLoadRequest DownLoadRequest;
	memset(&DownLoadRequest,0,sizeof(DownLoadRequest));
	DownLoadRequest.bDisplay=bDisplay;
	lstrcpyn(DownLoadRequest.szDescribe,pszKindName,CountArray(DownLoadRequest.szDescribe));
	_snprintf(DownLoadRequest.szFileName,sizeof(DownLoadRequest.szFileName),TEXT("%s.EXE"),pszKindName);
	_snprintf(DownLoadRequest.szLocalPath,sizeof(DownLoadRequest.szLocalPath),TEXT("%s\\DownLoad"),g_GlobalUnits.GetWorkDirectory());
	_snprintf(DownLoadRequest.szDownLoadUrl,sizeof(DownLoadRequest.szDownLoadUrl),TEXT("http://www.dc173.com/Download.asp?KindID=%ld&LocalVersion=0&PlazaVersion=%ld"),wKindID,VER_PLAZA_FRAME);

	//投递请求
	DWORD dwDownLoadID=g_GlobalUnits.m_DownLoadService->AddDownLoadRequest(DTP_GAME_CLIENT,&DownLoadRequest);

	return true;
}

//注册热键
bool CGlobalUnits::RegisterHotKey(HWND hWnd, UINT uKeyID, WORD wHotKey)
{
	//变量定义
	BYTE cbModifiers=0;
	if (HIBYTE(wHotKey)&HOTKEYF_ALT) cbModifiers|=MOD_ALT;
	if (HIBYTE(wHotKey)&HOTKEYF_SHIFT) cbModifiers|=MOD_SHIFT;
	if (HIBYTE(wHotKey)&HOTKEYF_CONTROL) cbModifiers|=MOD_CONTROL;

	//注册热键
	BOOL bSuccess=::RegisterHotKey(hWnd,uKeyID,cbModifiers,LOBYTE(wHotKey));

	return (bSuccess==TRUE)?true:false;
}


//////////////////////////////////////////////////////////////////////////
