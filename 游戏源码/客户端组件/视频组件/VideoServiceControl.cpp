#include "Stdafx.h"
#include "Resource.h"
#include "VideoServiceControl.h"

//////////////////////////////////////////////////////////////////////////

//窗口标识
#define IDC_VIDEO_SNED				100									//发送窗口
#define IDC_VIDEO_RCEC				100									//接收窗口
#define IDC_VIDEO_SR				100									//视频窗口

//按钮标识
#define IDC_FLOWER					103									//送花按钮
#define IDC_VIDEO					104									//视频按钮
#define IDC_AUDIO					105									//音频按钮
#define IDC_CLONE					106									//克隆按钮
#define IDC_MORE					107									//更多按钮


//位置标识
#define X_SMALL_POS					10									//起始坐标
#define Y_SMALL_POS					4									//起始坐标
#define X_LARGE_POS					8									//起始坐标
#define Y_LARGE_POS					5									//起始坐标

//宽度标识
#define W_SMALL_POS					80									//宽度坐标
#define H_SMALL_POS					60									//高度坐标
#define W_LARGE_POS					160									//宽度坐标
#define H_LARGE_POS					120									//高度坐标

//////////////////////////////////////////////////////////////////////////

//菜单命令
#define	IDM_UM_COPY_NAME			WM_USER+100							//拷贝名字
#define	IDM_UM_SET_CHAT				WM_USER+101							//设置聊天
#define	IDM_UM_CANECL_CHAT			WM_USER+102							//取消聊天
#define	IDM_UM_ALLOW_LOOKON			WM_USER+103							//旁观控制
#define	IDM_UM_ENJOIN_LOOKON		WM_USER+104							//旁观控制
#define	IDM_UM_KICK_TABLE_USER		WM_USER+105							//踢走用户


//鲜花菜单
#define	IDM_UM_FLOWER				WM_USER+306							//赠送礼物


//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CVideoServiceControl, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_FLOWER,OnBnClickedFlower)
	ON_BN_CLICKED(IDC_VIDEO,OnBnClickedVideo)
	ON_BN_CLICKED(IDC_AUDIO,OnBnClickedAudio)
	ON_BN_CLICKED(IDC_CLONE,OnBnClickedClone)
	ON_BN_CLICKED(IDC_MORE,OnBnClickedMore)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CVideoServiceControl, CWnd)
	ON_EVENT(CVideoServiceControl, IDC_VIDEO_SR, 202, OnEventVideoEnable, VTS_NONE)
	ON_EVENT(CVideoServiceControl, IDC_VIDEO_SR, 201, VolumeOutAV, VTS_PI4)
END_EVENTSINK_MAP()

//////////////////////////////////////////////////////////////////////////

//静态变量
CVideoServiceManager * CVideoServiceManager::m_pVideoServiceManager=NULL;//管理对象
HINSTANCE CVideoServiceManager::m_hResInstance=NULL;
tagVideoFrameImage CVideoServiceManager::m_VideoFrameImage;
tagPropertyMenuImage CVideoServiceManager::m_PropertyMenuImage;

//////////////////////////////////////////////////////////////////////////

//构造函数
CVideoServiceControl::CVideoServiceControl()
{
	//内核变量
	m_bInitVideo=false;

	//控制变量
	m_bSmallMode=false;
	m_bLocalMode=false;
	m_bEnableVideo=true;
	m_bEnableAudio=true;

	//状态变量
	m_fVolumePercent=0.0f;

	//设备标志
	m_bVidicon=false;
	m_bMicrophone=true;
	m_bOccupy=true;

	//用户信息
	m_dwUserID=0L;
	ZeroMemory(m_szAccounts,sizeof(m_szAccounts));

	//送花变量
	m_dwTargerUserID = 0;
	m_pUserData=NULL;

	return;
}

//析构函数
CVideoServiceControl::~CVideoServiceControl()
{
	//释放控件
	if ( m_FlowerEffectControl.m_hWnd != NULL )
		m_FlowerEffectControl.DestroyWindow();
}

//消息解释
BOOL CVideoServiceControl::PreTranslateMessage(MSG * pMsg)
{
	//提示消息
	if (m_ToolTipCtrl.m_hWnd!=NULL)
	{
		m_ToolTipCtrl.RelayEvent(pMsg);
	}

	return __super::PreTranslateMessage(pMsg);
}

//大小模式
bool CVideoServiceControl::SetSmallMode(bool bSmallMode)
{
	//设置变量
	if (m_bSmallMode!=bSmallMode)
	{
		//设置变量
		m_bSmallMode=bSmallMode;

		//调整窗口
		RectifyControl();
	}

	return true;
}

//设置用户
bool CVideoServiceControl::SetVideoUserInfo(DWORD dwUserID, LPCTSTR pszAccounts)
{
	//设置变量
	m_dwUserID=dwUserID;
	lstrcpyn(m_szAccounts,pszAccounts,CountArray(m_szAccounts));

	//刷新窗口
	if (m_hWnd!=NULL) Invalidate(FALSE);

	return true;
}

//配置视频
bool CVideoServiceControl::InitVideoService(bool bLocalMode, bool bSmallMode)
{
	//效验状态
	ASSERT((m_hWnd!=NULL)&&(m_bInitVideo==false));
	if ((m_hWnd==NULL)||(m_bInitVideo==true)) return false;

	//获取对象
	ASSERT(CVideoServiceManager::GetInstance()!=NULL);
	CVideoServiceManager * pVideoServiceManager=CVideoServiceManager::GetInstance();

	//旁观判断
	if ( pVideoServiceManager != NULL && pVideoServiceManager->m_pIClientKernel != NULL && pVideoServiceManager->m_pIClientKernel->IsLookonMode() ) return false;

	//设置变量
	m_bInitVideo=true;
	m_bSmallMode=bSmallMode;
	m_bLocalMode=bLocalMode;

	//加载资源
	UpdateSkinResource();

	//创建窗口
	if (m_bLocalMode==true)
	{
		//创建视频
		CRect rcCreate(0,0,0,0);
		m_VideoSend.Create(NULL,NULL,WS_CHILD,rcCreate,this,IDC_VIDEO_SNED);

		//设置设备
		if ((m_VideoSend.m_hWnd!=NULL)&&(m_VideoSend.HasVideoDrv()!=FALSE))
		{
			//设置变量
			m_bVidicon=true;

			//设置设备
			m_VideoSend.SetVideoMode(0);
			m_VideoSend.SetVideoRate(0);
		}
	}
	else
	{
		//创建视频
		CRect rcCreate(0,0,0,0);
		m_VideoRece.Create(NULL,NULL,WS_CHILD,rcCreate,this,IDC_VIDEO_RCEC);
	}

	//调整窗口
	RectifyControl();

	//音波区域	
	if (m_bSmallMode==true)
	{
		//语音图片
		CImageHandle HandleFrame(&m_ImageSmallFrame);
		CImageHandle ImageVolume(&m_ImageSmallVolume);

		m_rcPaintVolume.left = (HandleFrame->GetWidth()-ImageVolume->GetWidth())/2;
		m_rcPaintVolume.top = H_SMALL_POS+8;
		m_rcPaintVolume.right = m_rcPaintVolume.left + ImageVolume->GetWidth();
		m_rcPaintVolume.bottom = m_rcPaintVolume.top + ImageVolume->GetHeight();
	}
	else
	{
		//语音图片
		CImageHandle HandleFrame(&m_ImageLargeFrame);
		CImageHandle ImageVolume(&m_ImageLargeVolume);

		m_rcPaintVolume.left = (HandleFrame->GetWidth()-ImageVolume->GetWidth())/2;;
		m_rcPaintVolume.top = H_LARGE_POS+10;
		m_rcPaintVolume.right = m_rcPaintVolume.left + ImageVolume->GetWidth();
		m_rcPaintVolume.bottom = m_rcPaintVolume.top + ImageVolume->GetHeight();
	}
	return true;
}

//显示视频
bool CVideoServiceControl::OnShowLocalVD(bool bShow)
{
	//显示界面
	if ( bShow )
	{
		if ( m_bLocalMode )
		{
			//显示视频
			m_VideoSend.OpenSend();

			return true;
		}
	}
	else
	{
		if ( m_bLocalMode )
		{
			//关闭视频
			m_VideoSend.CloseSend();
			m_VideoSend.ShowWindow(SW_HIDE);
			
			return true;
		}
	}

	return false;
}

//界面重置
bool CVideoServiceControl::OnEventGameReset()
{
	//设置变量
	m_dwUserID=0L;
	ZeroMemory(m_szAccounts,sizeof(m_szAccounts));

	//设置控件
	if (m_VideoRece.m_hWnd!=NULL)
	{
		m_VideoRece.CloseRece();
		m_VideoRece.ShowWindow(SW_HIDE);
	}

	//关闭通道
	if (m_VideoSend.m_hWnd!=NULL)
	{
	}

	return true;
}

//用户离开
bool CVideoServiceControl::OnEventUserLeave(DWORD dwRemoteUserID)
{
	//状态效验
	ASSERT(m_bInitVideo==true);
	if (m_bInitVideo==false) return false;

	//本地模式
	if (m_bLocalMode==true)
	{
		//状态效验
		if (m_VideoSend.m_hWnd==NULL) return false;

		//删除通道
		m_VideoSend.DeleteP2PSocket(dwRemoteUserID);

		return true;
	}

	//远程模式
	if ((m_bLocalMode==false)&&(m_dwUserID==dwRemoteUserID))
	{
		//状态效验
		if (m_VideoRece.m_hWnd==NULL) return false;

		//关闭视频
		m_VideoRece.CloseRece();

		//隐藏窗口
		if (m_VideoRece.m_hWnd!=NULL) m_VideoRece.ShowWindow(SW_HIDE);

		//设置用户
		SetVideoUserInfo(0L,TEXT(""));

		return true;
	}

	return false;
}

//用户进入
bool CVideoServiceControl::OnEventUserEnter(DWORD dwRemoteUserID)
{
	//状态效验
	ASSERT(m_bInitVideo==true);
	if (m_bInitVideo==false) return false;

	//本地模式
	if (m_bLocalMode==true)
	{
		//获取对象
		ASSERT(CVideoServiceManager::GetInstance()!=NULL);
		CVideoServiceManager * pVideoServiceManager=CVideoServiceManager::GetInstance();

		//创建通道
		unsigned char cRet = m_VideoSend.InsertP2PSocket(pVideoServiceManager->m_dwLocalUserID,dwRemoteUserID,pVideoServiceManager->m_dwVideoServer,
			pVideoServiceManager->m_wVideoPort);

		//成功判断
		if ( cRet == 0 )
		{
			//显示消息
			TCHAR szMsg[] = TEXT("与视频服务器连接失败，您将不能进行视频！");
			IMessageProxy * pIMessageProxy = pVideoServiceManager->m_pIMessageProxy;
			if ( pIMessageProxy != NULL ) pIMessageProxy->InsertSystemString(szMsg, lstrlen(szMsg));

			return false;
		}

		//网络信息
		WORD wNatPort=0,wLocalPort=0;
		DWORD dwNatAddr=0L,dwLocalAddr=0L;
		if (m_VideoSend.m_hWnd!=NULL) m_VideoSend.GetP2PInfo(dwRemoteUserID,&dwLocalAddr,&wLocalPort,&dwNatAddr,&wNatPort);

		//效验数据
 		if ((wNatPort==0L)||(dwNatAddr==0L)) return false;
		if ((wLocalPort==0L)||(dwLocalAddr==0L)) return false;

		//发送请求
		if ((pVideoServiceManager!=NULL)&&(pVideoServiceManager->m_pIClientKernel!=NULL))
		{
			//变量定义
			CMD_C_VideoConfig VideoConfig;
			ZeroMemory(&VideoConfig,sizeof(VideoConfig));

			//设置变量
			VideoConfig.wNatPort=wNatPort;
			VideoConfig.dwNatAddr=dwNatAddr;
			VideoConfig.wLocalPort=wLocalPort;
			VideoConfig.dwLocalAddr=dwLocalAddr;
			VideoConfig.dwRemoteUserID=dwRemoteUserID;

			//发送数据
			IClientKernel * pIClientKernel=pVideoServiceManager->m_pIClientKernel;
			pIClientKernel->SendSocketData(MDM_GF_VIDEO,SUB_GF_C_VIDEO_CONFIG,&VideoConfig,sizeof(VideoConfig));

			return true;
		}

		//错误断言
		ASSERT(FALSE);

		return false;
	}

	return true;
}

//用户配置
bool CVideoServiceControl::OnEventUserConfig(DWORD dwRemoteUserID, tagVideoNetwordInfo * pVideoNetwordInfo)
{
	//状态效验
	ASSERT(m_bInitVideo==true);
	if (m_bInitVideo==false) return false;

	//获取对象
	ASSERT(CVideoServiceManager::GetInstance()!=NULL);
	CVideoServiceManager * pVideoServiceManager=CVideoServiceManager::GetInstance();

	//远程模式
	if ((m_bLocalMode==false)&&(m_dwUserID==dwRemoteUserID))
	{
		//窗口效验
		ASSERT(m_VideoRece.m_hWnd!=NULL);
		if (m_VideoRece.IsWindowVisible()) return true;

		//打开视频
		m_VideoRece.OpenRece();

		//创建通道
		unsigned char cRet = m_VideoRece.CreateP2P(pVideoServiceManager->m_dwLocalUserID,dwRemoteUserID,pVideoServiceManager->m_dwVideoServer,
			pVideoServiceManager->m_wVideoPort,pVideoNetwordInfo->dwLocalAddr,pVideoNetwordInfo->wLocalPort,pVideoNetwordInfo->dwNatAddr,
			pVideoNetwordInfo->wNatPort);

		//成功判断
		if ( cRet == 0 )
		{
			//显示消息
			TCHAR szMsg[] = TEXT("与视频服务器连接失败，您将不能进行视频！");
			IMessageProxy * pIMessageProxy = pVideoServiceManager->m_pIMessageProxy;
			if ( pIMessageProxy != NULL ) pIMessageProxy->InsertSystemString(szMsg, lstrlen(szMsg));

			return false;
		}

		return true;
	}

	//错误断言
	ASSERT(FALSE);

	return false;
}

//调整窗口
VOID CVideoServiceControl::RectifyControl()
{
	//窗口判断
	if (m_hWnd==NULL) return;

	//调整窗口
	if (m_bInitVideo==true)
	{
		//加载资源
		CImageHandle HandleFrame;
		if (m_bSmallMode==true) HandleFrame.AttachResource(&m_ImageSmallFrame);
		if (m_bSmallMode==false) HandleFrame.AttachResource(&m_ImageLargeFrame);

		//调整窗口
		INT nFrameWidth=HandleFrame->GetWidth();
		INT nFrameHeight=HandleFrame->GetHeight();
		SetWindowPos(NULL,0,0,nFrameWidth,nFrameHeight,SWP_NOZORDER|SWP_NOMOVE|SWP_NOCOPYBITS);

		//移动控件
		if (m_bSmallMode==true)
		{
			if (m_VideoSend.m_hWnd!=NULL) m_VideoSend.SetWindowPos(NULL,X_SMALL_POS,Y_SMALL_POS,W_SMALL_POS,H_SMALL_POS,SWP_NOZORDER|SWP_NOACTIVATE);
			if (m_VideoRece.m_hWnd!=NULL) m_VideoRece.SetWindowPos(NULL,X_SMALL_POS,Y_SMALL_POS,W_SMALL_POS,H_SMALL_POS,SWP_NOZORDER|SWP_NOACTIVATE);
		}
		else
		{
			if (m_VideoSend.m_hWnd!=NULL) m_VideoSend.SetWindowPos(NULL,X_LARGE_POS,Y_LARGE_POS,W_LARGE_POS,H_LARGE_POS,SWP_NOZORDER|SWP_NOACTIVATE);
			if (m_VideoRece.m_hWnd!=NULL) m_VideoRece.SetWindowPos(NULL,X_LARGE_POS,Y_LARGE_POS,W_LARGE_POS,H_LARGE_POS,SWP_NOZORDER|SWP_NOACTIVATE);
		}

		//变量定义
		INT nSpace=m_bSmallMode?1:10;
		INT nXCenterPos=HandleFrame->GetWidth()/2;
		INT nYPos=m_bSmallMode?H_SMALL_POS+13:H_LARGE_POS+18;
		const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;

		//按钮位置
		CRect rcButton;
		m_btFlower.GetWindowRect(&rcButton);

		//移动按钮
		HDWP hDwp=BeginDeferWindowPos(32);
		DeferWindowPos(hDwp,m_btFlower,NULL,nXCenterPos-rcButton.Width()*5/2-nSpace*2,nYPos,0,0,uFlags);
		DeferWindowPos(hDwp,m_btVideo,NULL,nXCenterPos-rcButton.Width()*3/2-nSpace,nYPos,0,0,uFlags);
		DeferWindowPos(hDwp,m_btAudio,NULL,nXCenterPos-rcButton.Width()/2,nYPos,0,0,uFlags);
		DeferWindowPos(hDwp,m_btClone,NULL,nXCenterPos+rcButton.Width()/2+nSpace,nYPos,0,0,uFlags);
		DeferWindowPos(hDwp,m_btMore,NULL,nXCenterPos+rcButton.Width()*3/2+nSpace*2,nYPos,0,0,uFlags);

		////鲜花窗口
		//if ( m_FlowerEffectControl.m_hWnd != NULL )
		//{
		//	if ( m_bSmallMode == true )
		//	{
		//		CRect rcFlowerEffect;
		//		rcFlowerEffect.SetRect(X_SMALL_POS,Y_SMALL_POS,W_SMALL_POS+10,H_SMALL_POS+2);
		//		DeferWindowPos(hDwp,m_FlowerEffectControl,NULL,rcFlowerEffect.left,rcFlowerEffect.top,rcFlowerEffect.Width(),rcFlowerEffect.Height(),SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS);
		//	}
		//	else
		//	{
		//		CRect rcFlowerEffect;
		//		rcFlowerEffect.SetRect(X_LARGE_POS,Y_LARGE_POS,W_LARGE_POS+5,H_LARGE_POS+5);
		//		DeferWindowPos(hDwp,m_FlowerEffectControl,NULL,rcFlowerEffect.left,rcFlowerEffect.top,rcFlowerEffect.Width(),rcFlowerEffect.Height(),SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS);
		//	}
		//}

		EndDeferWindowPos(hDwp);
	}
	else
	{
		//缩小窗口
		SetWindowPos(NULL,0,0,0,0,SWP_NOZORDER|SWP_NOMOVE|SWP_NOCOPYBITS);
	}

	//重画窗口
	Invalidate(TRUE);

	return;
}

//重画函数
VOID CVideoServiceControl::OnPaint()
{
	CPaintDC dcPaint(this);

	//配置判断
	ASSERT(m_bInitVideo==true);
	if (m_bInitVideo==false) return;

	//获取大小
	CRect rcClient;
	GetClientRect(&rcClient);

	//内存DC
	CDC dcBuffer;
	dcBuffer.CreateCompatibleDC(&dcPaint);
	CBitmap BufferBmp;
	BufferBmp.CreateCompatibleBitmap(&dcPaint, rcClient.Width(), rcClient.Height());
	dcBuffer.SelectObject(&BufferBmp);

	//区域判断
	CRect rcClipBox;
	dcPaint.GetClipBox(rcClipBox);
	if ( rcClipBox != m_rcPaintVolume )
	{
		//绘画背景
		if (m_bSmallMode==true)
		{
			CImageHandle HandleFrame(&m_ImageSmallFrame);
			m_ImageSmallFrame.BitBlt(dcBuffer,0,0);

			//语音图片
			CImageHandle ImageVolume(&m_ImageSmallVolume);

			INT nXPos=(HandleFrame->GetWidth()-m_ImageSmallVolume.GetWidth())/2;
			INT nYPos=H_SMALL_POS+8;
			m_ImageSmallVolume.BitBlt(dcBuffer,nXPos, nYPos);
		}
		else
		{
			CImageHandle HandleFrame(&m_ImageLargeFrame);
			m_ImageLargeFrame.BitBlt(dcBuffer,0,0);

			//语音图片
			CImageHandle ImageVolume(&m_ImageLargeVolume);

			INT nXPos=(HandleFrame->GetWidth()-m_ImageLargeVolume.GetWidth())/2;
			INT nYPos=H_LARGE_POS+10;
			m_ImageLargeVolume.BitBlt(dcBuffer,nXPos, nYPos);
		}

		//视频标识
		if (m_bVidicon==false)
		{
			if (m_bSmallMode==true)
			{
				CImageHandle HandlePhoto(&m_ImageSmallPhoto);
				m_ImageSmallPhoto.BitBlt(dcBuffer,X_SMALL_POS+(W_SMALL_POS-m_ImageSmallPhoto.GetWidth())/2,Y_SMALL_POS+(H_SMALL_POS-m_ImageSmallPhoto.GetHeight())/2);
			}
			else
			{
				CImageHandle HandlePhoto(&m_ImageLargePhoto);
				m_ImageLargePhoto.BitBlt(dcBuffer,X_LARGE_POS+(W_LARGE_POS-m_ImageLargePhoto.GetWidth())/2,Y_LARGE_POS+(H_LARGE_POS-m_ImageLargePhoto.GetHeight())/2);
			}
		}

		//判断头像
		CVideoServiceManager *pVideoServiceManager = CVideoServiceManager::GetInstance();
		ASSERT(pVideoServiceManager!=NULL);
		IClientKernel *pIClientKernel = pVideoServiceManager->m_pIClientKernel;
		IUserFaceRes *pIUserFaceRes = pVideoServiceManager->m_pIUserFaceRes;
		if ( pIClientKernel != NULL  && pIUserFaceRes!=NULL)
		{
			//绘画头像
			const tagUserData * pUserData = pIClientKernel->SearchUserItem(m_dwUserID);
			if(pUserData!=NULL)
			{
				CRect rect;
				if(m_bSmallMode)
				{
					rect.left = 10;
					rect.right = rcClient.Width()-rect.left*2;
					rect.top = 4;
					rect.bottom = m_rcPaintVolume.top-7;
				}
				else
				{
					rect.left = 7;
					rect.right = rcClient.Width()-rect.left*2;
					rect.top = 5;
					rect.bottom = m_rcPaintVolume.top-9;
				}
				if(pUserData->dwCustomFaceVer!=0)
				{
					pIUserFaceRes->DrawCustomBigFace(&dcBuffer,rect.left,rect.top,pUserData->dwUserID,pUserData->dwCustomFaceVer,
					rect.right,rect.bottom);
				}
				else if(pUserData->cbGender==1)
				{
					CImageHandle ImageFemale1(&m_ImageSmallDefMale);
					CImageHandle ImageFemale2(&m_ImageLargeDefMale);
					if(m_bSmallMode)m_ImageSmallDefMale.AlphaDrawImage(&dcBuffer,rect.left,rect.top,m_ImageSmallDefMale.GetWidth(),m_ImageSmallDefMale.GetHeight(),0,0,RGB(255,0,255));
					if(!m_bSmallMode)m_ImageLargeDefMale.AlphaDrawImage(&dcBuffer,rect.left,rect.top,m_ImageLargeDefMale.GetWidth(),m_ImageLargeDefMale.GetHeight(),0,0,RGB(255,0,255));
				}
				else 
				{
					CImageHandle ImageFemale1(&m_ImageSmallDefFemale);
					CImageHandle ImageFemale2(&m_ImageLargeDefFemale);
					if(m_bSmallMode)m_ImageSmallDefFemale.AlphaDrawImage(&dcBuffer,rect.left,rect.top,m_ImageSmallDefFemale.GetWidth(),m_ImageSmallDefFemale.GetHeight(),0,0,RGB(255,0,255));
					if(!m_bSmallMode)m_ImageLargeDefFemale.AlphaDrawImage(&dcBuffer,rect.left,rect.top,m_ImageLargeDefFemale.GetWidth(),m_ImageLargeDefFemale.GetHeight(),0,0,RGB(255,0,255));
				}
			}
		}

		dcPaint.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &dcBuffer, 0, 0, SRCCOPY);
	}
	else
	{
		if (m_bSmallMode==true)
		{
			//语音图片
			CImageHandle ImageVolume(&m_ImageSmallVolume);
			CImageHandle ImageVolumeEx(&m_ImageSmallVolumeEx);

			m_ImageSmallVolume.BitBlt(dcBuffer,m_rcPaintVolume.left, m_rcPaintVolume.top);
			m_ImageSmallVolumeEx.BitBlt(dcBuffer,m_rcPaintVolume.left, m_rcPaintVolume.top,int(m_rcPaintVolume.Width()*m_fVolumePercent),m_rcPaintVolume.Height(),0,0);
		}
		else
		{
			//语音图片
			CImageHandle ImageVolume(&m_ImageLargeVolume);
			CImageHandle ImageVolumeEx(&m_ImageLargeVolumeEx);

			m_ImageLargeVolume.BitBlt(dcBuffer,m_rcPaintVolume.left, m_rcPaintVolume.top);
			m_ImageLargeVolumeEx.BitBlt(dcBuffer,m_rcPaintVolume.left, m_rcPaintVolume.top,int(m_rcPaintVolume.Width()*m_fVolumePercent),m_rcPaintVolume.Height(),0,0);
		}

		dcPaint.BitBlt(rcClipBox.left, rcClipBox.top, rcClipBox.Width(), rcClipBox.Height(), &dcBuffer, rcClipBox.left, rcClipBox.top, SRCCOPY);
	}

	//清理资源
	dcBuffer.DeleteDC();
	BufferBmp.DeleteObject();	

	return;
}

//绘画背景
BOOL CVideoServiceControl::OnEraseBkgnd(CDC * pDC)
{
	Invalidate(FALSE);
	UpdateWindow();

	return TRUE;
}

//创建函数
INT CVideoServiceControl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//修改窗口属性
	ModifyStyle(0,WS_CLIPCHILDREN);

	//创建按钮
	CRect rcCreate(0,0,0,0);
	m_btMore.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_MORE);
	m_btVideo.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_VIDEO);
	m_btAudio.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_AUDIO);
	m_btClone.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_CLONE);
	m_btFlower.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_FLOWER);

	//建立提示
	m_ToolTipCtrl.Create(this);
	m_ToolTipCtrl.Activate(TRUE);
	m_ToolTipCtrl.AddTool(&m_btMore, TEXT("查看信息"));
	m_ToolTipCtrl.AddTool(&m_btVideo, TEXT("禁止视频"));
	m_ToolTipCtrl.AddTool(&m_btAudio, TEXT("禁止语音"));
	m_ToolTipCtrl.AddTool(&m_btClone, TEXT("缩放视频"));
	m_ToolTipCtrl.AddTool(&m_btFlower, TEXT("赠送鲜花"));

	//创建控件
	m_FlowerEffectControl.CreateControl(this);

	//调整窗口
	RectifyControl();

	return 0;
}

//送花按钮
VOID CVideoServiceControl::OnBnClickedFlower()
{
	//获取目标
	m_dwTargerUserID = GetVideoUserID();
	if ( m_dwTargerUserID == 0 ) return;

	//菜单命令
	UINT uCommands[FLOWER_COUNT] ;
	for ( WORD wCmdIdx = 0; wCmdIdx < FLOWER_COUNT; ++wCmdIdx )
		uCommands[wCmdIdx] = IDM_UM_FLOWER + wCmdIdx;

	//菜单位图
	CBitmap MenuBmps[FLOWER_COUNT];
	AfxSetResourceHandle(CVideoServiceManager::m_hResInstance);
	tagPropertyMenuImage & PropertyMenuImage=CVideoServiceManager::m_PropertyMenuImage;

	//加载位图
	for(WORD i=0;i<FLOWER_COUNT;i++)
	{
		MenuBmps[i].LoadBitmap( PropertyMenuImage.uMenuGift[i] );
	}

	AfxSetResourceHandle(GetModuleHandle(NULL));
    
    //创建菜单
	CMenu GiftMenu;
	GiftMenu.CreatePopupMenu();

	//设置菜单
	for ( UINT uMenuItem = 0; uMenuItem < FLOWER_COUNT; ++uMenuItem )
	{
		//菜单信息
		CString strMenuItem = g_FlowerDescribe[uMenuItem].szName;
		CString strGiftPay;
		strGiftPay.Format("%ld", CPropertyBar::m_FlowerInfo[uMenuItem].lPrice);
		strMenuItem += strGiftPay;

		//增加菜单
		GiftMenu.AppendMenu( 0, uCommands[uMenuItem], strMenuItem);
		GiftMenu.SetMenuItemBitmaps(uCommands[uMenuItem], MF_BYCOMMAND,&MenuBmps[uMenuItem], &MenuBmps[uMenuItem] );
	}	

	//获取位置
	CPoint MousePoint;
	GetCursorPos(&MousePoint);

	GiftMenu.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON,MousePoint.x,MousePoint.y,this);

	//释放资源
	for ( INT nBmpIdx = 0; nBmpIdx < FLOWER_COUNT; ++nBmpIdx ) MenuBmps[nBmpIdx].DeleteObject();
	GiftMenu.DestroyMenu();

	return;
}

//菜单命令
BOOL CVideoServiceControl::OnCommand(WPARAM wParam, LPARAM lParam)
{
	//提取命令
	UINT uCmd = LOWORD(wParam);

	if ( IDM_UM_FLOWER <= uCmd && uCmd <= IDM_UM_FLOWER + FLOWER_COUNT )
	{
		//索引转换
		UINT uBaseCmd = IDM_UM_FLOWER;
		UINT uFlowerIdx = uCmd - uBaseCmd;

		//构造信息
		CString strMsg;
		LONG lValue = CPropertyBar::m_FlowerInfo[uFlowerIdx].lPrice;
		strMsg.Format(TEXT("你选择了赠送［%s］，将会从您的银行扣除[%ld]游戏币。\n按“是”继续\n按“否”取消这次操作"),g_FlowerDescribe[uFlowerIdx].szName, lValue, lValue );
		if(IDYES!=ShowInformation(strMsg,0,MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2))
			return TRUE;

		//存在判断
		DWORD dwRcvUserID = GetVideoUserID();
		if ( dwRcvUserID == 0 || m_dwTargerUserID != dwRcvUserID )
		{
			ShowInformation( TEXT("对方已经离开，赠送失败！"), 0, MB_ICONINFORMATION );
			return TRUE;
		}

		//获取对象
		ASSERT(CVideoServiceManager::GetInstance()!=NULL);
		CVideoServiceManager * pVideoServiceManager=CVideoServiceManager::GetInstance();
		IClientKernel *pIClientKernel = pVideoServiceManager->m_pIClientKernel;
		ASSERT( pIClientKernel != NULL );

		//构造数据
		CMD_GF_Gift Gift;
		ZeroMemory( &Gift, sizeof( Gift ) );

		Gift.cbSendLocation = LOCATION_GAME_ROOM;
		Gift.wFlowerCount=1;
		Gift.dwRcvUserID = GetVideoUserID();
		Gift.dwSendUserID = pVideoServiceManager->m_dwLocalUserID;
		Gift.wGiftID = uFlowerIdx;

		//发送消息
		pIClientKernel->SendSocketData( MDM_GF_PRESENT,SUB_GF_FLOWER,&Gift, sizeof(Gift));

		return TRUE;
	}

	//获取对象
	ASSERT(CVideoServiceManager::GetInstance()!=NULL);
	CVideoServiceManager * pVideoServiceManager=CVideoServiceManager::GetInstance();
	ASSERT( pVideoServiceManager != NULL ) ;

	//发送数据
	IClientKernel * pIClientKernel=pVideoServiceManager->m_pIClientKernel;
	ASSERT( pIClientKernel != NULL ) ;

	switch (LOWORD(wParam))
	{
	case IDM_UM_COPY_NAME:		//拷贝名字
		{
			if ( m_pUserData == NULL ) return TRUE;

			//寻找玩家
			if ((m_pUserData==NULL)||(m_pUserData->dwUserID==0)) return TRUE;

			//打开剪切板
			if (OpenClipboard()==FALSE) return TRUE;
			if (EmptyClipboard()==FALSE) 
			{
				CloseClipboard();
				return TRUE;
			}

			//复制数据
			HANDLE hData=GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT,NAME_LEN);
			if (hData==NULL) 
			{
				CloseClipboard();
				return TRUE;
			}
			LPTSTR szMemName=(LPTSTR)GlobalLock(hData);
			lstrcpy(szMemName,m_pUserData->szName);
			SetClipboardData(CF_TEXT,hData);
			GlobalUnlock(hData);
			CloseClipboard();

			return TRUE;
		}
	case IDM_UM_KICK_TABLE_USER:	//踢走用户
		{
			//寻找玩家
			if ((m_pUserData==NULL)||(m_pUserData->dwUserID==0)) return TRUE;

			//发送消息
			CMD_GF_KickTableUser KickTableUser;
			KickTableUser.dwUserID=m_pUserData->dwUserID;
			pIClientKernel->SendSocketData(MDM_GF_FRAME,SUB_GF_KICK_TABLE_USER,&KickTableUser,sizeof(KickTableUser));

			return TRUE;
		}
	}

	return __super::OnCommand(wParam,lParam);
}

//视频按钮
VOID CVideoServiceControl::OnBnClickedVideo()
{
	//占用判断
	if ( m_bOccupy ) return ;

	//设置变量
	m_bEnableVideo=!m_bEnableVideo;

	//设置按钮
	HINSTANCE hInstance=CVideoServiceManager::m_hResInstance;
	tagVideoFrameImage & VideoFrameImage=CVideoServiceManager::m_VideoFrameImage;
	if (!m_bSmallMode) m_btVideo.SetButtonImage((m_bEnableVideo)?VideoFrameImage.uBtBigSee:VideoFrameImage.uBtBigNosee,hInstance,false);
	if (m_bSmallMode) m_btVideo.SetButtonImage((m_bEnableVideo)?VideoFrameImage.uBtSmallSee:VideoFrameImage.uBtSmallNosee,hInstance,false);

	if ( m_bEnableVideo ) m_ToolTipCtrl.AddTool(&m_btVideo, TEXT("禁止视频"));
	else m_ToolTipCtrl.AddTool(&m_btVideo, TEXT("启用视频"));

	//控制设备
	if (m_VideoSend.m_hWnd!=NULL)
	{
		m_VideoSend.EnableVideo(m_bEnableVideo);
		m_VideoSend.ShowWindow(m_bEnableVideo?SW_SHOW:SW_HIDE);

		if ( m_bLocalMode )
		{
			//打开视频
			if ( m_bEnableVideo )
			{
				//获取对象
				ASSERT(CVideoServiceManager::GetInstance()!=NULL);
				CVideoServiceManager * pVideoServiceManager=CVideoServiceManager::GetInstance();

				//发送数据
				IClientKernel * pIClientKernel=pVideoServiceManager->m_pIClientKernel;
				if ( pIClientKernel != NULL ) pIClientKernel->SendSocketData(MDM_GF_VIDEO,SUB_GF_C_VIDEO_OPEN);
			}
			else
			{
				//获取对象
				ASSERT(CVideoServiceManager::GetInstance()!=NULL);
				CVideoServiceManager * pVideoServiceManager=CVideoServiceManager::GetInstance();

				//发送数据
				IClientKernel * pIClientKernel=pVideoServiceManager->m_pIClientKernel;
				if ( pIClientKernel != NULL ) pIClientKernel->SendSocketData(MDM_GF_VIDEO,SUB_GF_C_VIDEO_CLOSE);
			}
		}
	}

	//控制设备
	if (m_VideoRece.m_hWnd!=NULL)
	{
		m_VideoRece.EnableVideo(m_bEnableVideo);
		m_VideoRece.ShowWindow(m_bEnableVideo?SW_SHOW:SW_HIDE);
	}

	//重画控件
	Invalidate(FALSE);

	return;
}

//关闭视频
bool CVideoServiceControl::OnEventCloseVideo()
{
	if ( m_bLocalMode && m_VideoSend.m_hWnd != NULL ) m_VideoSend.ShowWindow( SW_HIDE );
	else if ( m_VideoRece.m_hWnd != NULL ) m_VideoRece.ShowWindow( SW_HIDE );

	//使能按钮
	m_btVideo.EnableWindow(FALSE);
	m_btAudio.EnableWindow(FALSE);
	m_btClone.EnableWindow(FALSE);

	return true;
}

//打开视频
bool CVideoServiceControl::OnEventOpenVideo()
{
	if ( m_bLocalMode && m_VideoSend.m_hWnd != NULL ) m_VideoSend.ShowWindow( SW_SHOW );
	else if ( m_VideoRece.m_hWnd != NULL ) m_VideoRece.ShowWindow( SW_SHOW );

	//设置按钮
	m_btVideo.EnableWindow(TRUE);
	m_btAudio.EnableWindow(TRUE);
	m_btClone.EnableWindow(TRUE);
	m_btFlower.EnableWindow(TRUE);

	return true;
}

//赠送鲜花
bool CVideoServiceControl::OnEventFlower(UINT uFlowerEffectID)
{
	//合法判断
	if ( m_FlowerEffectControl.m_hWnd == NULL ) return false;

	//显示动画
	m_FlowerEffectControl.StartFlowerEffect(uFlowerEffectID);
	m_FlowerEffectControl.ShowWindow(SW_SHOW);
	m_FlowerEffectControl.BringWindowToTop();

	return true;
}

//更新资源
bool CVideoServiceControl::UpdateSkinResource()
{
	//加载资源
	HINSTANCE hResInstance=CVideoServiceManager::m_hResInstance;
	ASSERT(hResInstance!=NULL);
	if(hResInstance==NULL)return false;
	tagVideoFrameImage & VideoFrameImage=CVideoServiceManager::m_VideoFrameImage;

	m_ImageSmallPhoto.SetLoadInfo(VideoFrameImage.uSmallPhoto,hResInstance,false);
	m_ImageLargePhoto.SetLoadInfo(VideoFrameImage.uBigPhoto,hResInstance,false);
	m_ImageSmallFrame.SetLoadInfo(VideoFrameImage.uSmallFrame,hResInstance,false);
	m_ImageLargeFrame.SetLoadInfo(VideoFrameImage.uBigFrame,hResInstance,false);

	m_ImageLargeVolume.SetLoadInfo(VideoFrameImage.uBigVolume,hResInstance,false);
	m_ImageLargeVolumeEx.SetLoadInfo(VideoFrameImage.uBigVolumeEx,hResInstance,false);
	m_ImageSmallVolumeEx.SetLoadInfo(VideoFrameImage.uSmallVolumeEx,hResInstance,false);
	m_ImageSmallVolume.SetLoadInfo(VideoFrameImage.uSmallVolumeEx,hResInstance,false);	
	m_ImageSmallDefFemale.SetLoadInfo(VideoFrameImage.uSmallFemale,hResInstance,false);
	m_ImageSmallDefMale.SetLoadInfo(VideoFrameImage.uSmallMale,hResInstance,false);
	m_ImageLargeDefFemale.SetLoadInfo(VideoFrameImage.uBigFemale,hResInstance,false);
	m_ImageLargeDefMale.SetLoadInfo(VideoFrameImage.uBigMale,hResInstance,false);

	//设置按钮
	m_btMore.SetButtonImage(m_bSmallMode?VideoFrameImage.uBtSmallMore:VideoFrameImage.uBtBigMore,hResInstance,false);
	m_btClone.SetButtonImage(m_bSmallMode?VideoFrameImage.uBtSmallResore:VideoFrameImage.uBtBigResore,hResInstance,false);
	m_btFlower.SetButtonImage(m_bSmallMode?VideoFrameImage.uBtSmallFlower:VideoFrameImage.uBtBigFlower,hResInstance,false);

	if (!m_bSmallMode) m_btVideo.SetButtonImage((m_bEnableVideo)?VideoFrameImage.uBtBigSee:VideoFrameImage.uBtBigNosee,hResInstance,false);
	if (m_bSmallMode) m_btVideo.SetButtonImage((m_bEnableVideo)?VideoFrameImage.uBtSmallSee:VideoFrameImage.uBtSmallNosee,hResInstance,false);
	if (!m_bSmallMode) m_btAudio.SetButtonImage((m_bEnableAudio)?VideoFrameImage.uBtBigTalk:VideoFrameImage.uBtBigNotalk,hResInstance,false);
	if (m_bSmallMode) m_btAudio.SetButtonImage((m_bEnableAudio)?VideoFrameImage.uBtSmallTalk:VideoFrameImage.uBtSmallNotalk,hResInstance,false);

	return true;
}

//音频按钮
VOID CVideoServiceControl::OnBnClickedAudio()
{
	//设置变量
	m_bEnableAudio=!m_bEnableAudio;

	//设置按钮
	HINSTANCE hInstance=CVideoServiceManager::m_hResInstance;
	tagVideoFrameImage & VideoFrameImage=CVideoServiceManager::m_VideoFrameImage;
	if (!m_bSmallMode) m_btAudio.SetButtonImage((m_bEnableAudio)?VideoFrameImage.uBtBigTalk:VideoFrameImage.uBtBigNotalk,hInstance,false);
	if (m_bSmallMode) m_btAudio.SetButtonImage((m_bEnableAudio)?VideoFrameImage.uBtSmallTalk:VideoFrameImage.uBtSmallNotalk,hInstance,false);

	//控制设备
	if (m_VideoSend.m_hWnd!=NULL) m_VideoSend.EnableAudio(m_bEnableAudio);
	if (m_VideoRece.m_hWnd!=NULL) m_VideoRece.EnableAudio(m_bEnableAudio);

	if ( m_bEnableAudio ) m_ToolTipCtrl.AddTool(&m_btAudio, TEXT("禁止语音"));
	else m_ToolTipCtrl.AddTool(&m_btAudio, TEXT("启用语音"));

	return;
}

//克隆按钮
VOID CVideoServiceControl::OnBnClickedClone()
{
	//占用判断
	if ( m_bOccupy ) return ;

	//本地模式
	if (m_bLocalMode==true)
	{
		ASSERT(m_VideoSend.m_hWnd!=NULL);
		if (m_VideoSend.m_hWnd!=NULL) m_VideoSend.ScaleVideo();
	}

	//远程模式
	if (m_bLocalMode==false)
	{
		ASSERT(m_VideoRece.m_hWnd!=NULL);
		if (m_VideoRece.m_hWnd!=NULL) m_VideoRece.ScaleVideo();
	}

	return;
}

//更多按钮
VOID CVideoServiceControl::OnBnClickedMore()
{
	//获取位置
	CPoint MousePoint;
	GetCursorPos(&MousePoint);

	//获取对象
	ASSERT(CVideoServiceManager::GetInstance()!=NULL);
	CVideoServiceManager * pVideoServiceManager=CVideoServiceManager::GetInstance();
	ASSERT( pVideoServiceManager != NULL ) ;

	//发送数据
	IClientKernel * pIClientKernel=pVideoServiceManager->m_pIClientKernel;
	ASSERT( pIClientKernel != NULL ) ;

	//获取用户
	TCHAR szBuffer[512]=TEXT("");
	m_pUserData=pIClientKernel->SearchUserItem(m_dwUserID);
	if(m_pUserData==NULL) return;

	//预先计算
	double dFleeRate=0.0;
	LONG lPlayCount=m_pUserData->lWinCount+m_pUserData->lLostCount+m_pUserData->lDrawCount+m_pUserData->lFleeCount;
	if (m_pUserData->lFleeCount>0)
	{
		dFleeRate=(double)(m_pUserData->lFleeCount*100)/(double)lPlayCount;
		if (dFleeRate<0.01) dFleeRate=0.0;
	}

	//构造菜单
	CMenu UserInfoMenu;
	UserInfoMenu.CreatePopupMenu();

	//功能菜单
	UserInfoMenu.AppendMenu(0,IDM_UM_COPY_NAME,TEXT("拷贝用户名"));
	UserInfoMenu.AppendMenu(MF_SEPARATOR);

	//用户称谓
	LPCTSTR pszMemberOrder[]={TEXT("普通会员"),TEXT("中级会员"),TEXT("高级贵宾")};
	if ((m_pUserData->cbMemberOrder>0)&&(m_pUserData->cbMemberOrder<CountArray(pszMemberOrder)))
	{
		_snprintf(szBuffer,sizeof(szBuffer),TEXT("用户名：%s [ %s ]"),m_pUserData->szName,pszMemberOrder[m_pUserData->cbMemberOrder]);
	}
	else _snprintf(szBuffer,sizeof(szBuffer),TEXT("用户名：%s"),m_pUserData->szName);
	UserInfoMenu.AppendMenu(0,0,szBuffer);

	//用户 ID
	_snprintf(szBuffer,sizeof(szBuffer),TEXT("游戏 ID：%ld"),m_pUserData->dwGameID);
	UserInfoMenu.AppendMenu(0,0,szBuffer);

	//用户经验
	_snprintf(szBuffer,sizeof(szBuffer),TEXT("经验值：%ld"),m_pUserData->lExperience);
	UserInfoMenu.AppendMenu(0,0,szBuffer);

	//用户积分
	_snprintf(szBuffer,sizeof(szBuffer),TEXT("积分：%ld  总局数：%ld  逃跑率：%5.2f%%"),m_pUserData->lScore,lPlayCount,dFleeRate);
	UserInfoMenu.AppendMenu(0,0,szBuffer);

	//获取用户
	const tagUserData * pMeUserData=pIClientKernel->GetMeUserInfo();

	//踢人菜单
	if (pMeUserData->dwUserID!=m_pUserData->dwUserID)
	{
		//控制菜单
		bool bEnable=(pMeUserData->cbMemberOrder>0);
		if ((bEnable==true)&&(m_pUserData->cbMemberOrder>pMeUserData->cbMemberOrder)) bEnable=false;
		if ((bEnable==true)&&(m_pUserData->cbMasterOrder>0)&&(pMeUserData->cbMasterOrder==0)) bEnable=false;

		//插入菜单
		_snprintf(szBuffer,sizeof(szBuffer),TEXT("把 [ %s ] 踢出游戏房间"),m_pUserData->szName);
		UserInfoMenu.AppendMenu((bEnable==true)?MF_ENABLED:MF_GRAYED,IDM_UM_KICK_TABLE_USER,szBuffer);
	}

	//显示菜单
	UserInfoMenu.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON,MousePoint.x,MousePoint.y,this);

	return;
}

//使能通知
VOID CVideoServiceControl::OnEventVideoEnable()
{
	//占用设置
	m_bOccupy = false;

	//本地模式
	if (m_VideoSend.m_hWnd!=NULL) m_VideoSend.ShowWindow(SW_SHOW);
	if (m_VideoRece.m_hWnd!=NULL) m_VideoRece.ShowWindow(SW_SHOW);

	return;
}

//音频函数
void CVideoServiceControl::VolumeOutAV(unsigned long *Volume)
{
	//获取对象
	ASSERT(CVideoServiceManager::GetInstance()!=NULL);
	CVideoServiceManager * pVideoServiceManager=CVideoServiceManager::GetInstance();

	//计算比例
	m_fVolumePercent = *Volume / 500.f;
	if ( m_fVolumePercent > 1 ) m_fVolumePercent=1.0f;

	//更新界面
	InvalidateRect(&m_rcPaintVolume);
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CVideoServiceManager::CVideoServiceManager()
{
	//视频信息
	m_wVideoPort=0;
	m_dwVideoServer=0L;
	m_dwLocalUserID=0L;
	ZeroMemory( m_bOffLine, sizeof( m_bOffLine ) );

	//组件变量
	m_pIClientKernel=NULL;
	m_pIUserFaceRes =NULL;
	m_pIMessageProxy=NULL;
	ZeroMemory(m_VideoServiceControl,sizeof(m_VideoServiceControl));

	//设置对象
	ASSERT(m_pVideoServiceManager==NULL);
	if (m_pVideoServiceManager==NULL) m_pVideoServiceManager=this;

	return;
}

//析构函数
CVideoServiceManager::~CVideoServiceManager()
{
}

//设置接口
bool CVideoServiceManager::SetClientKernel(IUnknownEx * pIUnknownEx)
{
	//设置接口
	if (pIUnknownEx!=NULL)
	{
		//查询接口
		ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IClientKernel)!=NULL);
		m_pIClientKernel=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IClientKernel);

		//成功判断
		if (m_pIClientKernel==NULL) return false;
	}
	else m_pIClientKernel=NULL;

	return true;
}

//设置接口
bool CVideoServiceManager::SetUserFaceRes(IUnknownEx * pIUnknownEx)
{
	ASSERT(pIUnknownEx!=NULL);
	m_pIUserFaceRes=(IUserFaceRes *)pIUnknownEx->QueryInterface(IID_IUserFaceRes,VER_IUserFaceRes);
	return (m_pIUserFaceRes!=NULL);
}

//服务信息
bool CVideoServiceManager::SetVideoServiceInfo(DWORD dwLoaclUserID, DWORD dwVideoServer, WORD wVideoPort)
{
	//设置变量
	m_wVideoPort=wVideoPort;
	m_dwVideoServer=dwVideoServer;
	m_dwLocalUserID=dwLoaclUserID;

	return true;
}

//设置对象
bool CVideoServiceManager::SetVideoServiceControl(WORD wVideoIndex, CVideoServiceControl * pVideoServiceControl)
{
	//获取对象
	ASSERT(wVideoIndex<MAX_CHAIR);
	if (wVideoIndex>=MAX_CHAIR) return false;

	//设置对象
	m_VideoServiceControl[wVideoIndex]=pVideoServiceControl;

	return true;
}

//设置资源
void CVideoServiceManager::SetSkinResource(HINSTANCE hResInstance,tagVideoFrameImage *pVideoFrameImage,tagPropertyMenuImage *pPropertyMenuImage)
{
	//资源变量
	m_hResInstance=hResInstance;
	CopyMemory(&m_VideoFrameImage,pVideoFrameImage,sizeof(m_VideoFrameImage));
	CopyMemory(&m_PropertyMenuImage,pPropertyMenuImage,sizeof(m_PropertyMenuImage));

	return ;
}

//界面重置
bool CVideoServiceManager::OnEventGameReset()
{
	//界面重置
	for (WORD i=0;i<MAX_CHAIR;i++)
	{
		if (m_VideoServiceControl[i]!=NULL)
		{
			m_VideoServiceControl[i]->OnEventGameReset();
		}
	}

	return true;
}

//用户离开
bool CVideoServiceManager::OnEventUserLeave(WORD wVideoIndex, DWORD dwRemoteUserID)
{
	//事件通知
	ASSERT(wVideoIndex<MAX_CHAIR);
	if (wVideoIndex>=MAX_CHAIR) return false;

	//效验参数
	ASSERT(m_VideoServiceControl[wVideoIndex]!=NULL);
	ASSERT(m_VideoServiceControl[wVideoIndex]->GetVideoUserID()==dwRemoteUserID);

	//发送组件
	for (WORD i=0;i<MAX_CHAIR;i++)
	{
		if (m_VideoServiceControl[i]!=NULL)
		{
			DWORD dwVideoUserID=m_VideoServiceControl[i]->GetVideoUserID();
			if ((dwVideoUserID==m_dwLocalUserID)||(dwVideoUserID==dwRemoteUserID))
			{
				m_VideoServiceControl[i]->OnEventUserLeave(dwRemoteUserID);
			}
		}
	}

	return true;
}

//用户进入
bool CVideoServiceManager::OnEventUserEnter(WORD wVideoIndex, DWORD dwRemoteUserID)
{
	//事件通知
	ASSERT(wVideoIndex<MAX_CHAIR);
 	if (wVideoIndex>=MAX_CHAIR) return false;

	//效验参数
	ASSERT(m_VideoServiceControl[wVideoIndex]!=NULL);
	ASSERT(m_VideoServiceControl[wVideoIndex]->GetVideoUserID()==0L);

	//设置用户
	if (m_VideoServiceControl[wVideoIndex]!=NULL)
	{
		m_VideoServiceControl[wVideoIndex]->SetVideoUserInfo(dwRemoteUserID,TEXT("视频用户"));
	}

	//增加用户
	if (dwRemoteUserID!=m_dwLocalUserID)
	{
		for (WORD i=0;i<MAX_CHAIR;i++)
		{
			if (m_VideoServiceControl[i]!=NULL)
			{
				DWORD dwVideoUserID=m_VideoServiceControl[i]->GetVideoUserID();
				if (dwVideoUserID==m_dwLocalUserID)
					if ( m_VideoServiceControl[i]->OnEventUserEnter(dwRemoteUserID) )
					{
						//设置变量
						m_bOffLine[wVideoIndex] = false;
						break;
					}
			}
		}
	}

	return true;
}

//用户配置
bool CVideoServiceManager::OnEventUserConfig(WORD wVideoIndex, DWORD dwRemoteUserID, tagVideoNetwordInfo * pVideoNetwordInfo)
{
	//事件通知
	ASSERT(wVideoIndex<MAX_CHAIR);
	if (wVideoIndex>=MAX_CHAIR) return false;

	//效验参数
	ASSERT(m_VideoServiceControl[wVideoIndex]!=NULL);
	ASSERT(m_VideoServiceControl[wVideoIndex]->GetVideoUserID()==dwRemoteUserID);

	//设置用户
	if (m_VideoServiceControl[wVideoIndex]!=NULL)
	{
		m_VideoServiceControl[wVideoIndex]->OnEventUserConfig(dwRemoteUserID, pVideoNetwordInfo);
	}

	return true;
}

//显示视频
bool CVideoServiceManager::OnShowLocalVD(bool bShow)
{
	for (WORD i=0;i<MAX_CHAIR;i++)
	{
		if (m_VideoServiceControl[i]!=NULL)
		{
			m_VideoServiceControl[i]->OnShowLocalVD(bShow);
		}
	}

	return false;
}

//更新用户
bool CVideoServiceManager::OnUpdateUserItem( WORD wVideoIndex, BYTE cbUserStatus, DWORD dwRemoteUserID )
{
	//参数效验
	ASSERT(wVideoIndex<MAX_CHAIR);
	ASSERT(m_VideoServiceControl[wVideoIndex]!=NULL);

	//参数效验
	if (wVideoIndex>=MAX_CHAIR) return false;
	if (m_VideoServiceControl[wVideoIndex]==NULL) return false;	

	if (cbUserStatus==US_OFFLINE )
	{
		//设置变量
		m_bOffLine[wVideoIndex]=true;
		return OnEventUserLeave(wVideoIndex,dwRemoteUserID);
	}
	else if (cbUserStatus==US_PLAY)
	{
		//断线判断
		if (m_bOffLine[wVideoIndex]) OnEventUserEnter(wVideoIndex,dwRemoteUserID);

	}

	return false;
}

//关闭视频
bool CVideoServiceManager::OnEventCloseVideo(WORD wVideoIndex)
{
	//合法验证
	ASSERT( wVideoIndex < MAX_CHAIR );
	ASSERT( m_VideoServiceControl[wVideoIndex] != NULL );
	if ( wVideoIndex >= MAX_CHAIR ) return false;
	if ( m_VideoServiceControl[wVideoIndex] == NULL ) return false;

	m_VideoServiceControl[wVideoIndex]->OnEventCloseVideo();

	return true;
}

//打开视频
bool CVideoServiceManager::OnEventOpenVideo(WORD wVideoIndex)
{
	//合法验证
	ASSERT( wVideoIndex < MAX_CHAIR );
	ASSERT( m_VideoServiceControl[wVideoIndex] != NULL );
	if ( wVideoIndex >= MAX_CHAIR ) return false;
	if ( m_VideoServiceControl[wVideoIndex] == NULL ) return false;

	m_VideoServiceControl[wVideoIndex]->OnEventOpenVideo();
	
	return true;
}

//赠送鲜花
bool CVideoServiceManager::OnEventFlower(WORD wVideoIndex, UINT uFlowerEffectID)
{
	//合法验证
	ASSERT( wVideoIndex < MAX_CHAIR );
	ASSERT( m_VideoServiceControl[wVideoIndex] != NULL );
	if ( wVideoIndex >= MAX_CHAIR ) return false;
	if ( m_VideoServiceControl[wVideoIndex] == NULL ) return false;

	return m_VideoServiceControl[wVideoIndex]->OnEventFlower(uFlowerEffectID);
}

//设置接口
bool CVideoServiceManager::SetMessageProxy(IUnknownEx * pIUnknownEx)
{
	ASSERT(pIUnknownEx!=NULL);
	m_pIMessageProxy=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IMessageProxy);
	ASSERT(m_pIMessageProxy!=NULL);
	return (m_pIMessageProxy!=NULL);
}

//////////////////////////////////////////////////////////////////////////
