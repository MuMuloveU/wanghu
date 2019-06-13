#include "StdAfx.h"
#include "Direct.h"
#include "DownLoadService.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

//定时器标识
#define IDI_UPDATE_TIMER					10							//更新定时器

//获取错误 HTML
const TCHAR szErrorHtml1[]=TEXT("<html>");
const TCHAR szErrorHtml2[]=TEXT("<!DOCTYPE HTML PUBLIC");

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDownLoadMission, CSkinDialogEx)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_RETRY, OnBnClickedReTry)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CDownLoadThread::CDownLoadThread()
{
	//设置变量
	m_bPreparative=false;
	m_szTempFile[0]=0;
	m_szLocalFile[0]=0;
	m_dwDownLoadSize=0;
	m_dwTotalFileSize=0;
	m_pHttpFile=NULL;
	m_pHttpConnection=NULL;
	m_pDownLoadRequest=NULL;
	m_DownLoadResult=enDownLoadResult_Noknow;
	m_DownLoadStatus=enDownLoadStatus_Unknow;

	return;
}

//析构函数
CDownLoadThread::~CDownLoadThread()
{
	DownLoadCleanUp();
	return;
}

//初始化线程
bool CDownLoadThread::InitThread(tagDownLoadRequest * pDownLoadRequest)
{
	ASSERT(pDownLoadRequest!=NULL);
	m_pDownLoadRequest=pDownLoadRequest;
	return true;
}

//下载状态
void CDownLoadThread::GetDownLoadStatus(tagDownLoadStatus & DownLoadStatus)
{
	//锁定资源
	CThreadLock ThreadLockHandle(m_CriticalSection);

	//设置变量
	DownLoadStatus.DownLoadStatus=m_DownLoadStatus;
	switch (m_DownLoadStatus)
	{
	case enDownLoadStatus_Unknow:
	case enDownLoadStatus_Ready:
		{
			DownLoadStatus.wProgress=0;
			lstrcpyn(DownLoadStatus.szStatus,TEXT("正在获取文件..."),CountArray(DownLoadStatus.szStatus));
			break;
		}
	case enDownLoadStatus_DownLoadIng:
		{
			DownLoadStatus.wProgress=(WORD)(m_dwDownLoadSize*100L/m_dwTotalFileSize);
			_snprintf(DownLoadStatus.szStatus,sizeof(DownLoadStatus.szStatus),
				TEXT("已复制：%ld KB （共 %ld KB）"),m_dwDownLoadSize/1000L,m_dwTotalFileSize/1000L);
			break;
		}
	case enDownLoadStatus_Finish:
		{
			DownLoadStatus.wProgress=100;
			lstrcpyn(DownLoadStatus.szStatus,TEXT("下载成功"),CountArray(DownLoadStatus.szStatus));
			break;
		}
	case enDownLoadStatus_Fails:
		{
			DownLoadStatus.wProgress=0;
			_snprintf(DownLoadStatus.szStatus,sizeof(DownLoadStatus.szStatus),TEXT("下载失败，错误号：%ld"),m_DownLoadResult);
			break;
		}
	default: 
		{
			ASSERT(FALSE);
			memset(&DownLoadStatus,0,sizeof(DownLoadStatus));
			break;
		}
	}

	return;
}

//目标文件
LPCTSTR CDownLoadThread::GetDownLoadFileName()
{
	ASSERT(m_DownLoadStatus==enDownLoadStatus_Finish);
	return m_szLocalFile;
}

//运行函数
bool CDownLoadThread::OnEventThreadRun()
{
	try
	{
		if (m_bPreparative==true)
		{
			//读取文件
			TCHAR szBuffer[4096];
			UINT uReadCount=m_pHttpFile->Read(szBuffer,sizeof(szBuffer));
			if (uReadCount>0)
			{
				//写入文件
				m_LocalFile.Write(szBuffer,uReadCount);

				//锁定资源
				CThreadLock ThreadLockHandle(m_CriticalSection);

				//设置变量
				m_dwDownLoadSize+=uReadCount;
				m_DownLoadStatus=enDownLoadStatus_DownLoadIng;

				return true;
			}
			else 
			{
				//关闭文件
				m_LocalFile.Close();

				//移动文件
				_snprintf(m_szLocalFile,sizeof(m_szLocalFile),TEXT("%s\\%s"),m_pDownLoadRequest->szLocalPath,m_pDownLoadRequest->szFileName);
				DeleteFile(m_szLocalFile);
				if (MoveFileWithProgress(m_szTempFile,m_szLocalFile,NULL, NULL, MOVEFILE_WRITE_THROUGH|MOVEFILE_REPLACE_EXISTING)==FALSE)

				//锁定资源
				CThreadLock ThreadLockHandle(m_CriticalSection);

				//设置变量
				m_DownLoadStatus=enDownLoadStatus_Finish;

				//下载清理
				DownLoadCleanUp();

				return false;
			}
		}
		else 
		{
			//锁定资源
			CThreadLock ThreadLockHandle(m_CriticalSection);

			//设置变量
			m_DownLoadStatus=enDownLoadStatus_Ready;

			//下载准备
			DownLoadPreparative();

			return true;
		}
	}
	catch (enDownLoadResult DownLoadResult)
	{
		//锁定资源
		CThreadLock ThreadLockHandle(m_CriticalSection);

		//设置变量
		m_DownLoadStatus=enDownLoadStatus_Fails;
		m_DownLoadResult=DownLoadResult;
	}
	catch (CInternetException * pInternetException)
	{
		//清理异常
		pInternetException->Delete();

		//锁定资源
		CThreadLock ThreadLockHandle(m_CriticalSection);

		//设置变量
		m_DownLoadStatus=enDownLoadStatus_Fails;
		m_DownLoadResult=enDownLoadResult_Exception;
	}
	catch (...) 
	{
		//锁定资源
		CThreadLock ThreadLockHandle(m_CriticalSection);

		//设置变量
		m_DownLoadStatus=enDownLoadStatus_Fails;
		m_DownLoadResult=enDownLoadResult_Exception;
	}

	//清理处理
	DownLoadCleanUp();

	return false;
}

//关闭事件
bool CDownLoadThread::OnEventThreadConclude()
{
	DownLoadCleanUp();
	return true;
}

//下载准备
void CDownLoadThread::DownLoadPreparative()
{
	//效验状态
	ASSERT(m_pHttpFile==NULL);
	ASSERT(m_bPreparative==false);

	//设置变量
	m_szTempFile[0]=0;
	m_szLocalFile[0]=0;
	m_dwDownLoadSize=0;
	m_dwTotalFileSize=0;

	//建立目录
	int nExcursion=0;
	TCHAR szDirectory[MAX_PATH]=TEXT("");
	lstrcpyn(szDirectory,m_pDownLoadRequest->szLocalPath,CountArray(szDirectory));
	do
	{
		if (szDirectory[nExcursion]==0) 
		{
			::CreateDirectory(szDirectory,NULL);
			break;
		}
		if (szDirectory[nExcursion]==TEXT('\\'))
		{
			szDirectory[nExcursion]=0;
			::CreateDirectory(szDirectory,NULL);
			szDirectory[nExcursion]=TEXT('\\');
		}
		nExcursion++;
	} while (true);

	//判断下载内容
	try
	{
		//读取内容
		UINT nReadCount=0;
		TCHAR szDownBuffer[4096];
		m_pHttpFile=(CHttpFile *)m_InternetSession.OpenURL(m_pDownLoadRequest->szDownLoadUrl,1,INTERNET_FLAG_TRANSFER_ASCII|INTERNET_FLAG_DONT_CACHE);
		nReadCount=m_pHttpFile->Read(szDownBuffer,sizeof(szDownBuffer));
		if (nReadCount<sizeof(szErrorHtml1)) throw 0;
		if (nReadCount<sizeof(szErrorHtml2)) throw 0;
		if (memcmp(szErrorHtml1,szDownBuffer,lstrlen(szErrorHtml1))==0) throw 0;
		if (memcmp(szErrorHtml2,szDownBuffer,lstrlen(szErrorHtml2))==0) throw 0;

		//读取大小
		ASSERT(m_pHttpFile!=NULL);
		BOOL bSuccess=m_pHttpFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH|HTTP_QUERY_FLAG_NUMBER,m_dwTotalFileSize);
		if (bSuccess==FALSE) throw enDownLoadResult_InternetReadError;

		//本地文件
		_snprintf(m_szTempFile,sizeof(m_szTempFile),TEXT("%s\\%s.DTM"),m_pDownLoadRequest->szLocalPath,m_pDownLoadRequest->szFileName);
		if (m_LocalFile.Open(m_szTempFile,CFile::modeReadWrite|CFile::modeCreate)==FALSE) throw enDownLoadResult_CreateFileFails;

		//写入文件
		m_LocalFile.Write(szDownBuffer,nReadCount);

		//锁定资源
		CThreadLock ThreadLockHandle(m_CriticalSection);

		//设置变量
		m_dwDownLoadSize+=nReadCount;
		m_DownLoadStatus=enDownLoadStatus_DownLoadIng;

		//设置变量
		m_bPreparative=true;

		return;
	}
	catch (...)
	{ 
		SafeDelete(m_pHttpFile);
		throw enDownLoadResult_InternetReadError;
	}

	return;
}

//下载完成
void CDownLoadThread::DownLoadCleanUp()
{
	//设置变量
	m_dwDownLoadSize=0;
	m_dwTotalFileSize=0;
	m_bPreparative=false;

	//清理资源
	if (m_pHttpFile!=NULL) 
	{
		m_pHttpFile->Close();
		SafeDelete(m_pHttpFile);
	}
	if (m_pHttpConnection!=NULL)
	{
		m_pHttpConnection->Close();
		SafeDelete(m_pHttpConnection);
	}
	if (m_LocalFile!=INVALID_HANDLE_VALUE) m_LocalFile.Close();

	//删除文件
	if (m_szTempFile[0]!=0) DeleteFile(m_szTempFile);

	return;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CDownLoadMission::CDownLoadMission(IDownLoadMissionSink * pIDownLoadMissionSink) : CSkinDialogEx(IDD_DOWN_LOAD)
{
	//设置变量
	m_dwMissionID=0;
	m_dwDownLoadType=0;
	m_pIDownLoadMissionSink=pIDownLoadMissionSink;
	m_DownLoadThread.InitThread(&m_DownLoadRequest);
	memset(&m_DownLoadRequest,0,sizeof(m_DownLoadRequest));

	return;
}

//析构函数
CDownLoadMission::~CDownLoadMission()
{
}

//控件绑定
void CDownLoadMission::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RETRY, m_btReTry);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Control(pDX, IDC_PROGRESS, m_ProgressCtrl);
}

//初始化函数
BOOL CDownLoadMission::OnInitDialog()
{
	__super::OnInitDialog();

	//设置描述
	SetDlgItemText(IDC_DESCRIPTION,m_DownLoadRequest.szDescribe);

	//设置定时器
	SetTimer(IDI_UPDATE_TIMER,300,NULL);

	return TRUE;
}

//消息过虑
BOOL CDownLoadMission::PreTranslateMessage(MSG * pMsg)
{
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE)) return TRUE;
	return __super::PreTranslateMessage(pMsg);
}

//取消消息
void CDownLoadMission::OnCancel()
{
	//关闭界面
	DestroyWindow();

	//停止下载
	m_DownLoadThread.ConcludeThread(INFINITE);

	//事件通知
	m_pIDownLoadMissionSink->OnMissionFinish(enDownLoadStatus_Fails,this);

	return;
}

//重试按钮
void CDownLoadMission::OnBnClickedReTry()
{
	//设置界面
	m_btReTry.EnableWindow(FALSE);

	//重新下载
	m_DownLoadThread.StartThread();

	//设置时间
	SetTimer(IDI_UPDATE_TIMER,300,NULL);

	return;
}

//定时器消息
void CDownLoadMission::OnTimer(UINT_PTR nIDEvent)
{
	__super::OnTimer(nIDEvent);

	//更新状态
	if (nIDEvent==IDI_UPDATE_TIMER)
	{
		//设置控件
		tagDownLoadStatus DownLoadStatus;
		m_DownLoadThread.GetDownLoadStatus(DownLoadStatus);
		m_ProgressCtrl.SetPos(DownLoadStatus.wProgress);
		SetDlgItemText(IDC_STATUS,DownLoadStatus.szStatus);

		//状态切换
		switch (DownLoadStatus.DownLoadStatus)
		{
		case enDownLoadStatus_Fails:
			{
				//设置界面
				KillTimer(IDI_UPDATE_TIMER);
				m_btReTry.EnableWindow(TRUE);
				
				//事件通知
				if (m_DownLoadRequest.bDisplay==false)
				{
					DestroyWindow();
					m_pIDownLoadMissionSink->OnMissionFinish(enDownLoadStatus_Fails,this);
				}

				break;
			}
		case enDownLoadStatus_Finish:
			{
				DestroyWindow();
				LPCTSTR pszLocalFile=m_DownLoadThread.GetDownLoadFileName();
				ShellExecute(NULL,TEXT("open"),pszLocalFile,NULL,NULL,SW_SHOWDEFAULT);

				//事件通知
				m_pIDownLoadMissionSink->OnMissionFinish(enDownLoadStatus_Finish,this);

				break;
			}
		}

		return;
	}

	return;
}

//显示界面
bool CDownLoadMission::DisplayDownLoadFace(bool bDisplay)
{
	//设置变量
	m_DownLoadRequest.bDisplay=bDisplay;

	//显示界面
	if (m_hWnd!=NULL) ShowWindow(bDisplay?SW_SHOW:SW_HIDE);

	return true;
}

//对比请求
bool CDownLoadMission::CompareRequest(tagDownLoadRequest * pDownLoadRequest)
{
	if (lstrcmp(m_DownLoadRequest.szDownLoadUrl,pDownLoadRequest->szDownLoadUrl)!=0) return false;
	return true;
}

//开始下载
bool CDownLoadMission::StartDownLoad(DWORD dwMissionID, DWORD dwDownLoadType, tagDownLoadRequest * pDownLoadRequest)
{
	//设置变量
	m_dwMissionID=dwMissionID;
	m_dwDownLoadType=dwDownLoadType;
	CopyMemory(&m_DownLoadRequest,pDownLoadRequest,sizeof(m_DownLoadRequest));
	if (m_DownLoadRequest.szLocalPath[0]==0) GetCurrentDirectory(sizeof(m_DownLoadRequest.szLocalPath),m_DownLoadRequest.szLocalPath);

	//启动下载
	m_DownLoadThread.StartThread();

	//创建界面
	AfxSetResourceHandle(GetModuleHandle(IDOWN_LOAD_DLL_NAME));
	Create(IDD_DOWN_LOAD,AfxGetMainWnd());
	AfxSetResourceHandle(GetModuleHandle(NULL));
	if (m_DownLoadRequest.bDisplay==true) ShowWindow(SW_SHOW);

	return true;
}

//停止下载
bool CDownLoadMission::StopDownLoad()
{
	//关闭界面
	DestroyWindow();

	//停止线程
	m_DownLoadThread.ConcludeThread(INFINITE);

	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CDownLoadService::CDownLoadService()
{
	m_dwMissionID=0;
	m_pIDownLoadServiceSink=NULL;
}

//析构函数
CDownLoadService::~CDownLoadService()
{
	//清理任务
	CDownLoadMission * pDownLoadMission=NULL;
	for (INT_PTR i=0;i<m_DownLoadMissionActive.GetCount();i++)
	{
		pDownLoadMission=m_DownLoadMissionActive[i];
		pDownLoadMission->StopDownLoad();
		SafeDelete(pDownLoadMission);
	}
	for (INT_PTR i=0;i<m_DownLoadMissionRelease.GetCount();i++)
	{
		pDownLoadMission=m_DownLoadMissionRelease[i];
		SafeDelete(pDownLoadMission);
	}
	m_DownLoadMissionActive.RemoveAll();
	m_DownLoadMissionRelease.RemoveAll();
	
	return;
}

//接口查询
void * __cdecl CDownLoadService::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDownLoadService,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDownLoadService,Guid,dwQueryVer);
	return NULL;
}

//获取数目
INT_PTR __cdecl CDownLoadService::GetDownLoadMissionCount()
{
	return m_DownLoadMissionActive.GetCount();
}

//设置接口
bool __cdecl CDownLoadService::SetDownLoadServiceSink(IUnknownEx * pIUnknownEx)
{
	ASSERT(pIUnknownEx!=NULL);
	m_pIDownLoadServiceSink=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IDownLoadServiceSink);
	ASSERT(m_pIDownLoadServiceSink!=NULL);
	return (m_pIDownLoadServiceSink!=NULL);
}

//下载请求
DWORD __cdecl CDownLoadService::AddDownLoadRequest(DWORD dwDownLoadType, tagDownLoadRequest * pDownLoadRequest)
{
	//效验参数
	ASSERT(pDownLoadRequest!=NULL);
	if (pDownLoadRequest==NULL) return 0;

	//寻找下载
	CDownLoadMission * pDownLoadMission=SearchMission(pDownLoadRequest);
	if (pDownLoadMission!=NULL)
	{
		if (pDownLoadRequest->bDisplay==true) pDownLoadMission->DisplayDownLoadFace(pDownLoadRequest->bDisplay);
		return pDownLoadMission->GetMissionID();
	}

	//创建下载
	INT_PTR nReleaseCount=m_DownLoadMissionRelease.GetCount();
	if (nReleaseCount>0)
	{
		pDownLoadMission=m_DownLoadMissionRelease[nReleaseCount-1];
		m_DownLoadMissionRelease.RemoveAt(nReleaseCount-1);
	}
	else
	{
		try { pDownLoadMission=new CDownLoadMission(this); }
		catch (...) { return 0; }
	}
	ASSERT(pDownLoadMission!=NULL);
	if (pDownLoadMission==NULL) return 0;
	m_DownLoadMissionActive.Add(pDownLoadMission);

	//配置下载
	m_dwMissionID++;
	pDownLoadMission->StartDownLoad(m_dwMissionID,dwDownLoadType,pDownLoadRequest);

	return pDownLoadMission->GetMissionID();
}

//下载通知
void CDownLoadService::OnMissionFinish(enDownLoadStatus DownLoadStatus, CDownLoadMission * pDownLoadMission)
{
	//寻找下载
	for (INT_PTR i=0;i<m_DownLoadMissionActive.GetCount();i++)
	{
		if (pDownLoadMission==m_DownLoadMissionActive[i]) break;
	}
	ASSERT(i!=m_DownLoadMissionActive.GetCount());
	if (i==m_DownLoadMissionActive.GetCount()) return;

	//清理下载
	m_DownLoadMissionActive.RemoveAt(i);
	m_DownLoadMissionRelease.Add(pDownLoadMission);

	return;
}

//查找下载
CDownLoadMission * CDownLoadService::SearchMission(tagDownLoadRequest * pDownLoadRequest)
{
	CDownLoadMission * pDownLoadMission=NULL;
	for (INT_PTR i=0;i<m_DownLoadMissionActive.GetCount();i++)
	{
		pDownLoadMission=m_DownLoadMissionActive[i];
		if (pDownLoadMission->CompareRequest(pDownLoadRequest)==true) return pDownLoadMission;
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////

//建立对象函数
extern "C" __declspec(dllexport) void * __cdecl CreateDownLoadService(const GUID & Guid, DWORD dwInterfaceVer)
{
	//建立对象
	CDownLoadService * pDownLoadService=NULL;
	try
	{
		pDownLoadService=new CDownLoadService();
		if (pDownLoadService==NULL) throw TEXT("创建失败");
		void * pObject=pDownLoadService->QueryInterface(Guid,dwInterfaceVer);
		if (pObject==NULL) throw TEXT("接口查询失败");
		return pObject;
	}
	catch (...) {}

	//清理对象
	SafeDelete(pDownLoadService);
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
