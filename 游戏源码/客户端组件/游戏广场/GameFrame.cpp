#include "Stdafx.h"
#include "GameFrame.h"
#include "DlgOption.h"
#include "GlobalUnits.h"
#include "DlgIndividualInfo.h"

//////////////////////////////////////////////////////////////////////////

//框架偏移
#define FRAME_EXALTATION			24									//提升高度

//屏幕位置
#define LESS_SCREEN_X				1024								//最小宽度
#define LESS_SCREEN_Y				720									//最小高度

//按钮标示
#define IDC_BT_MIN					1000								//最小按钮
#define IDC_BT_MAX					1001								//最小按钮
#define IDC_BT_CLOSE				1002								//关闭按钮
#define IDC_BT_EXCHANGE_SINK		1003								//换肤按钮
#define IDC_BT_BUTTON_1				1004								//功能按钮
#define IDC_BT_BUTTON_2				1005								//功能按钮
#define IDC_BT_BUTTON_3				1006								//功能按钮
#define IDC_BT_BUTTON_4				1007								//功能按钮
#define IDC_BT_BUTTON_5				1008								//功能按钮

//控件标识
#define IDC_BT_PLAZA				1038								//游戏广场
#define IDC_BT_SWITCH_ACCOUNTS		1009								//切换帐号
#define IDC_BT_SYSTEM_OPTION		1010								//系统配置
#define IDC_BT_SELF_OPTION			1011								//个人配置
#define IDC_BT_QUIT_GAME			1012								//退出游戏
#define IDC_BT_VIEW_ITEM			1013								//房间控件
#define IDC_BT_LIST_CONTROL_1		21015								//控制按钮
#define IDC_BT_LIST_CONTROL_2		21016								//控制按钮
#define IDC_BROWER_AD				1017								//见面控制
#define IDC_SPLITTER				1018								//分条控制
#define IDC_SERVER_ITEM_VIEW		1019								//列表控制
#define IDC_COMPANION_LIST			1020								//好友列表
	
//消息定义
#define WM_CLOSE_ROOM_VIEW_ITEM		WM_USER+10							//关闭消息
#define WM_SETUP_FINISH				WM_USER+100							//安装完成

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameFrame, CFrameWnd)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_CLOSE()
	ON_WM_CREATE()
	ON_WM_NCHITTEST()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SETTINGCHANGE()
	ON_WM_GETMINMAXINFO()
	ON_MESSAGE(WM_HOTKEY,OnHotKeyMessage)
	ON_MESSAGE(WM_SETUP_FINISH,OnMessageSetupFinish)
	ON_MESSAGE(WM_CLOSE_ROOM_VIEW_ITEM,OnCloseRoomViewItem)
	ON_WM_NCMOUSEMOVE()
//	ON_WM_MOUSEMOVE()
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()



//////////////////////////////////////////////////////////////////////////
//构造函数
CGameFrame::CGameFrame():m_Splitter(VorSpliter)
{
	//设置变量
	m_bRectify=false;
	m_bHideGameList=false;
	m_wShowListType=IDC_BT_LIST_CONTROL_1;
	m_pActiveViewItem=NULL;
	m_pRoomViewItemCreate=NULL;
	memset(m_pRoomViewItem,0,sizeof(m_pRoomViewItem));

	//界面状态
	m_bMaxShow=false;
	m_rcNormalSize.SetRect(0,0,0,0);

	return;
}

//析构函数
CGameFrame::~CGameFrame()
{
}

//按钮消息
bool CGameFrame::OnSplitterButton(CSkinSplitter * pSkinSplitter, CSkinButton * pSplitterButton)
{
	//列表消息
	ControlServerList(ServerListControl_Turn);

	return true;
}

//拆分条消息
bool CGameFrame::OnSplitterEvent(CSkinSplitter * pSkinSplitter, UINT uSplitID, int nXPos, int nYPos)
{
	//显示控件
	if (m_bHideGameList==true)
	{
		//设置变量
		m_bRectify=true;
		m_bHideGameList=false;

		//界面控制
		m_UserInfoView.ShowWindow(SW_SHOW);
		OnCommand(m_wShowListType,0);

		//设置按钮
		m_Splitter.ShowSplitterButton(GetPlatformRes().uBtHideGameList,GetResInstanceHandle());
	}

	//调整控件
	CRect rcClient;
	GetClientRect(&rcClient);
	RectifyControl(rcClient.Width(),rcClient.Height());

	return true;
}

//左键单击
void __cdecl CGameFrame::OnTreeLeftClick(CListItem *pListItem, HTREEITEM hTreeItem, CTreeCtrl * pTreeCtrl)
{
	//效验参数
	if (pListItem==NULL) 
	{
		return;
	}

	//消息处理
	switch (pListItem->GetItemGenre())
	{
	case ItemGenre_Inside:
		{
			//变量定义
			CListInside * pListInside=(CListInside *)pListItem;
			tagGameInside * pGameInside=pListInside->GetItemInfo();

			WebBrowse(TEXT("http://www.dc173.com"),false);

			return;
		}
	case ItemGenre_Kind:		//游戏类型
		{
			//变量定义
			CListKind * pListKind=(CListKind *)pListItem;
			tagGameKind * pGameKind=pListKind->GetItemInfo();

			//连接规则
			TCHAR szRuleUrl[256]=TEXT("");
			_snprintf(szRuleUrl,sizeof(szRuleUrl),TEXT("http://www.dc173.com/GameRule.asp?KindID=%ld"),pGameKind->wKindID);
			WebBrowse(szRuleUrl,true);

			return;
		}
	}

	return;
}

//右键单击
void __cdecl CGameFrame::OnTreeRightClick(CListItem *pListItem, HTREEITEM hTreeItem, CTreeCtrl *pTreeCtrl)
{
	return;
}

//左键双击
void __cdecl CGameFrame::OnTreeLeftDBClick(CListItem *pListItem, HTREEITEM hTreeItem, CTreeCtrl *pTreeCtrl)
{
	//效验参数
	if(pListItem==NULL) return;

	//消息处理
	switch (pListItem->GetItemGenre())
	{
	case ItemGenre_Kind:	//游戏类型
		{
			//变量定义
			CListKind * pListKind=(CListKind *)pListItem;
			tagGameKind * pGameKind=pListKind->GetItemInfo();

			//安装判断
			if ((pListKind->m_bInstall==false)&&(pListKind->m_GameKind.dwMaxVersion!=0L))
			{
				//构造提示
				TCHAR szBuffer[512]=TEXT("");
				_snprintf(szBuffer,sizeof(szBuffer),TEXT("【%s】还没有安装，现在是否下载？"),pGameKind->szKindName);

				//提示信息
				if (ShowMessageBox(szBuffer,MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON1)==IDYES)
				{
					g_GlobalAttemper.DownLoadClient(pGameKind->szKindName,pGameKind->wKindID,true);
				}
			}

			return;
		}
	case ItemGenre_Server:	//游戏房间
		{
			CListServer * pListServer=(CListServer *)pListItem;
			CreateRoomViewItem(pListServer);
			return;
		}
	}

	return;
}

//右键双击
void __cdecl CGameFrame::OnTreeRightDBClick(CListItem *pListItem, HTREEITEM hTreeItem, CTreeCtrl *pTreeCtrl)
{
	return;
}

//选择改变
void __cdecl CGameFrame::OnTreeSelectChanged(CListItem *pListItem, HTREEITEM hTreeItem, CTreeCtrl *pTreeCtrl)
{
	return;
}

//子项展开
void __cdecl CGameFrame::OnTreeItemexpanded(CListItem *pListItem, HTREEITEM hTreeItem, CTreeCtrl * pTreeCtrl)
{
	return;
}

//消息解释
BOOL CGameFrame::PreTranslateMessage(MSG * pMsg)
{
	m_ToolTipCtrl.RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

//命令函数
BOOL CGameFrame::OnCommand(WPARAM wParam, LPARAM lParam)
{
	//变量定义
	UINT nCommandID=LOWORD(wParam);

	//切换按钮
	if ((nCommandID>=IDC_BT_VIEW_ITEM)&&(nCommandID<(IDC_BT_VIEW_ITEM+CountArray(m_btViewItem))))
	{
		ActiveRoomViewItem(m_pRoomViewItem[nCommandID-IDC_BT_VIEW_ITEM]);
		return TRUE;
	}

	//消息处理
	switch(nCommandID)
	{
	case IDC_BT_MIN	:					//最小化窗
		{
			ShowWindow(SW_MINIMIZE);

			return TRUE;
		}
	case IDC_BT_MAX		:				//最大化窗
		{
			if (m_bMaxShow==true) RestoreWindow();
			else MaxSizeWindow();

			return TRUE;
		}
	case IDC_BT_BUTTON_1:				//功能按钮
	case IDC_BT_BUTTON_2:				//功能按钮
	case IDC_BT_BUTTON_4:				//功能按钮
	case IDC_BT_BUTTON_5:				//功能按钮
		{
			if(IDC_BT_BUTTON_1==nCommandID)WebBrowse(TEXT("http://www.dc173.com/index.asp"),true);
			if(IDC_BT_BUTTON_2==nCommandID)WebBrowse(TEXT("http://www.dc173.com/PayBuy.asp"),true);
			if(IDC_BT_BUTTON_4==nCommandID)WebBrowse(TEXT("http://www.dc173.com/user/ExChange.asp"),true);
			if(IDC_BT_BUTTON_5==nCommandID)WebBrowse(TEXT("http://www.dc173.com/bbs/index.asp?action=frameon"),true);

			return TRUE;
		}
	case IDC_BT_BUTTON_3:				//上传头像
		{
			//创建窗体
			if ( m_DlgCustomFace.m_hWnd == NULL )
			{
				m_DlgCustomFace.Create(IDD_CUSTOM_FACE, this);
			}

			//显示窗体
			m_DlgCustomFace.CenterWindow();
			m_DlgCustomFace.ShowWindow(SW_SHOW);
			m_DlgCustomFace.SetActiveWindow();
			m_DlgCustomFace.SetForegroundWindow();

			return TRUE;
		}
	case IDC_BT_PLAZA	:				//大厅按钮
		{
			ActivePlazaViewItem();

			return TRUE;
		}
	case IDC_BT_EXCHANGE_SINK:			//换肤按钮
		{
			g_GlobalUnits.m_PlatformResourceModule->SetResourceType(RESOURCE_RED);
			UpdateSkinResource();
			m_DlgGamePlaza.UpdateSkinResource();
			m_UserInfoView.UpdateSkinResource();
			for (int i=0;i<CountArray(m_pRoomViewItem);i++)
			{
				if (m_pRoomViewItem[i]==NULL) break;
				m_pRoomViewItem[i]->UpdateSkinResource();
			}

			return TRUE;
		}
	case IDC_BT_CLOSE	:				//关闭房间
	case IDC_BT_QUIT_GAME	:			//退出游戏
		{
			CloseCurrentViewItem();

			return TRUE;
		}
	case IDC_BT_SYSTEM_OPTION	:		//系统配置
		{
			//系统设置
			ShowSystemOption();

			return TRUE;
		}
	case IDC_BT_SELF_OPTION:			//个人配置
		{
			CDlgIndividualInfo DlgIndividualInfo;
			DlgIndividualInfo.DoModal();

			return TRUE;
		}
	case IDC_BT_SWITCH_ACCOUNTS :		//切换按钮
		{
			//状态判断
			tagGlobalUserData & GlobalUserData=g_GlobalUnits.GetGolbalUserData();
			if (GlobalUserData.dwUserID==0L) return TRUE;

			//切换询问
			if (m_pRoomViewItem[0]!=NULL)
			{
				const TCHAR szMessage[]=TEXT("切换用户帐号，将会关闭所有游戏房间，确实要切换用户帐号吗？ ");
				int iResult=ShowMessageBox(szMessage,MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2);
				if (iResult!=IDYES) return TRUE;
			}
			else
			{
				const TCHAR szMessage[]=TEXT("确实要注销当前用户，切换用户帐号吗？ ");
				int iResult=ShowMessageBox(szMessage,MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2);
				if (iResult!=IDYES) return TRUE;
			}

			//关闭房间
			ActivePlazaViewItem();
			CloseAllRoomViewItem();

			//删除记录
			g_GlobalUnits.DeleteUserCookie();

			//设置变量
			memset(&GlobalUserData,0,sizeof(GlobalUserData));

			//投递消息
			m_DlgGamePlaza.SendLogonMessage();

			return TRUE;
		}
	case IDC_BT_LIST_CONTROL_1:			//列表按钮
	case IDC_BT_LIST_CONTROL_2:			//列表按钮
		{
			//隐藏列表
			if(m_bHideGameList)
			{
				//隐藏按钮
				m_btListControl1.ShowWindow(SW_HIDE);
				m_btListControl2.ShowWindow(SW_HIDE);
				m_ServerItemView.ShowWindow(SW_HIDE);
				m_UserCompanionList.ShowWindow(SW_HIDE);
			}
			//显示列表
			else
			{
				//设置变量
				m_wShowListType = nCommandID;

				//设置按钮
				m_btListControl1.ShowWindow(SW_SHOW);
				m_btListControl2.ShowWindow(SW_SHOW);
				UINT uControl1=((IDC_BT_LIST_CONTROL_1==m_wShowListType)?GetPlatformRes().uFrameServiceBtShow1:GetPlatformRes().uFrameServiceBtHide1);
				UINT uControl2=((IDC_BT_LIST_CONTROL_2==m_wShowListType)?GetPlatformRes().uFrameServiceBtShow2:GetPlatformRes().uFrameServiceBtHide2);
				m_btListControl1.SetButtonImage(uControl1,GetResInstanceHandle(),false);
				m_btListControl2.SetButtonImage(uControl2,GetResInstanceHandle(),false);

				//判断显示
				m_ServerItemView.ShowWindow((m_wShowListType==IDC_BT_LIST_CONTROL_1)?SW_SHOW:SW_HIDE);
				m_UserCompanionList.ShowWindow((m_wShowListType==IDC_BT_LIST_CONTROL_2)?SW_SHOW:SW_HIDE);
			}

			return TRUE;
		}
	}

	return __super::OnCommand(wParam,lParam);
}

//显示网页
void CGameFrame::WebBrowse(LPCTSTR lpszUrl, bool bForceBrowse)
{
	//激活广场
	ActivePlazaViewItem();

	//打开网页
	if (&m_DlgGamePlaza!=NULL) 
	{
		m_DlgGamePlaza.WebBrowse(lpszUrl,bForceBrowse);
	}

	return;
}

//系统设置
void CGameFrame::ShowSystemOption()
{
	//获取参数
	CGameOption * pGameOption=NULL;
	CServerOption * pServerOption=NULL;
	if (m_pActiveViewItem!=&m_DlgGamePlaza)
	{
		for (int i=0;i<CountArray(m_pRoomViewItem);i++)
		{
			if (m_pRoomViewItem[i]==NULL) break;
			if (m_pActiveViewItem==m_pRoomViewItem[i])
			{
				pGameOption=m_pRoomViewItem[i]->GetGameOption();
				pServerOption=m_pRoomViewItem[i]->GetServerOption();
				break;
			}
		}
	}

	//显示窗口
	CDlgOption DlgOption(pGameOption,pServerOption);
	INT_PTR iResult=DlgOption.DoModal();
	if (iResult==IDOK)
	{
		for (int i=0;i<CountArray(m_pRoomViewItem);i++)
		{
			if (m_pRoomViewItem[i]==NULL) break;
			m_pRoomViewItem[i]->SendUserRule();
		}
	}

	return;
}

//显示消息
int CGameFrame::ShowMessageBox(LPCTSTR pszMessage, UINT nType)
{
	INT nResult=ShowInformationEx(pszMessage,0,nType,TEXT("游戏大厅"));
	return nResult;
}

//列表控制
bool CGameFrame::ControlServerList(enServerListControl ServerListControl)
{
	//位置控制
	bool bHideGameList=false;
	switch (ServerListControl)
	{
	case ServerListControl_Hide: { bHideGameList=true; break; }
	case ServerListControl_Show: { bHideGameList=false; break; }
	case ServerListControl_Turn: { bHideGameList=!m_bHideGameList; break; }
	}

	//调整判断
	if (bHideGameList==m_bHideGameList) return false;

	//设置变量
	m_bRectify=false;
	m_bHideGameList=bHideGameList;

	//界面控制
	m_UserInfoView.ShowWindow((bHideGameList==false)?SW_SHOW:SW_HIDE);
	OnCommand(m_wShowListType,0);

	//设置按钮
	UINT uControlId = ((m_bHideGameList)?GetPlatformRes().uBtShowGameList:GetPlatformRes().uBtHideGameList);
	m_Splitter.ShowSplitterButton(uControlId,GetResInstanceHandle());

	//调整界面
	CRect rcClient;
	GetClientRect(&rcClient);
	RectifyControl(rcClient.Width(),rcClient.Height());

	return true;
}

//资源句柄
HINSTANCE CGameFrame::GetResInstanceHandle()
{
	return g_GlobalUnits.m_PlatformResourceModule->GetResInstance();
}

//获取资源
tagPlatformFrameImage CGameFrame::GetPlatformRes()
{
	return g_GlobalUnits.m_PlatformFrameImage;
}

//更新资源
bool CGameFrame::UpdateSkinResource()
{
	//获取资源
	tagPlatformFrameImage & PlatformFrameImage=GetPlatformRes();
	HINSTANCE hResInstance=GetResInstanceHandle();

	//加载资源
	m_EncircleFrame.ImageML.SetLoadInfo(PlatformFrameImage.uFrameML,hResInstance);
	m_EncircleFrame.ImageMR.SetLoadInfo(PlatformFrameImage.uFrameMR,hResInstance);
	m_EncircleFrame.ImageTL.SetLoadInfo(PlatformFrameImage.uFrameTL,hResInstance);
	m_EncircleFrame.ImageTM.SetLoadInfo(PlatformFrameImage.uFrameTM,hResInstance);
	m_EncircleFrame.ImageTR.SetLoadInfo(PlatformFrameImage.uFrameTR,hResInstance);
	m_EncircleFrame.ImageBL.SetLoadInfo(PlatformFrameImage.uFrameBL,hResInstance);
	m_EncircleFrame.ImageBM.SetLoadInfo(PlatformFrameImage.uFrameBM,hResInstance);
	m_EncircleFrame.ImageBR.SetLoadInfo(PlatformFrameImage.uFrameBR,hResInstance);

	//服务器列表
	m_EncircleServer.ImageML.SetLoadInfo(PlatformFrameImage.uServerML,hResInstance);
	m_EncircleServer.ImageMR.SetLoadInfo(PlatformFrameImage.uServerMR,hResInstance);
	m_EncircleServer.ImageTL.SetLoadInfo(PlatformFrameImage.uServerTL,hResInstance);
	m_EncircleServer.ImageTM.SetLoadInfo(PlatformFrameImage.uServerTM,hResInstance);
	m_EncircleServer.ImageTR.SetLoadInfo(PlatformFrameImage.uServerTR,hResInstance);
	m_EncircleServer.ImageBL.SetLoadInfo(PlatformFrameImage.uServerBL,hResInstance);
	m_EncircleServer.ImageBM.SetLoadInfo(PlatformFrameImage.uServerBM,hResInstance);
	m_EncircleServer.ImageBR.SetLoadInfo(PlatformFrameImage.uServerBR,hResInstance);

	//获取信息
	CSkinAide::GetEncircleInfo(m_ImageInfoFrame,m_EncircleFrame);
	CSkinAide::GetEncircleInfo(m_ImageInfoServer,m_EncircleServer);

	//拆分条控件
	UINT uControlId = ((m_bHideGameList)?PlatformFrameImage.uBtShowGameList:PlatformFrameImage.uBtHideGameList);
	m_Splitter.ShowSplitterButton(uControlId,hResInstance);

	//设置图片
	m_btMin.SetButtonImage(PlatformFrameImage.uBtFrameMin,hResInstance,false);
	m_btMax.SetButtonImage((m_bMaxShow)?PlatformFrameImage.uBtFrameResore:PlatformFrameImage.uBtFrameMax,hResInstance,false);
	m_btClose.SetButtonImage(PlatformFrameImage.uBtFrameClose,hResInstance,false);
	m_btExChangeSkin.SetButtonImage(PlatformFrameImage.uBtFrameClose,hResInstance,false);
	m_btButton1.SetButtonImage(PlatformFrameImage.uFrameNavigation[0],hResInstance,false);
	m_btButton2.SetButtonImage(PlatformFrameImage.uFrameNavigation[1],hResInstance,false);
	m_btButton3.SetButtonImage(PlatformFrameImage.uFrameNavigation[2],hResInstance,false);
	m_btButton4.SetButtonImage(PlatformFrameImage.uFrameNavigation[3],hResInstance,false);
	m_btButton5.SetButtonImage(PlatformFrameImage.uFrameNavigation[4],hResInstance,false);
	m_btSwitchUser.SetButtonImage(PlatformFrameImage.uBtFrameSwitch,hResInstance,false);
	m_btGlobalOption.SetButtonImage(PlatformFrameImage.uBtFrameOption,hResInstance,false);
	m_btSelfOption.SetButtonImage(PlatformFrameImage.uBtFrameSelfSet,hResInstance,false);

	//列表按钮
	UINT uControl1=((IDC_BT_LIST_CONTROL_1==m_wShowListType)?PlatformFrameImage.uFrameServiceBtShow1:PlatformFrameImage.uFrameServiceBtHide1);
	UINT uControl2=((IDC_BT_LIST_CONTROL_2==m_wShowListType)?PlatformFrameImage.uFrameServiceBtShow2:PlatformFrameImage.uFrameServiceBtHide2);
	m_btListControl1.SetButtonImage(uControl1,hResInstance,false);
	m_btListControl2.SetButtonImage(uControl2,hResInstance,false);

	//设置图片
	for (int i=0;i<CountArray(m_pRoomViewItem);i++)
	{
		if(m_pActiveViewItem==m_pRoomViewItem[i])break;
	}
	CSkinButton * pTempButton[]={&m_btViewItem[0],&m_btViewItem[1],&m_btViewItem[2],&m_btGamePlaza};
	ASSERT((CountArray(m_btViewItem)+1)==CountArray(pTempButton));
	for (WORD j=0;j<CountArray(pTempButton);j++)
	{
		if(i==j)
		{
			pTempButton[j]->SetTextColor(RGB(0,0,0),RGB(133,215,255),true);
			pTempButton[j]->SetButtonImage(PlatformFrameImage.uBtFrameItemActive,hResInstance,false);
		}
		else
		{
			pTempButton[j]->SetTextColor(RGB(245,245,245),RGB(20,20,20),true);
			pTempButton[j]->SetButtonImage(PlatformFrameImage.uBtFrameItemNormal,hResInstance,false);
		}
	}
	return true;
}

//激活广场
void CGameFrame::ActivePlazaViewItem()
{
	//获取窗口
	HWND hWndActive=NULL;
	if (m_pActiveViewItem!=NULL) hWndActive=m_pActiveViewItem->GetWindowHandle();
	if (m_DlgGamePlaza.GetSafeHwnd()!=hWndActive)
	{
		m_pActiveViewItem=&m_DlgGamePlaza;
		m_DlgGamePlaza.ShowWindow(SW_SHOW);
		if (hWndActive!=NULL) ::ShowWindow(hWndActive,SW_HIDE);

		//显示列表
		if(!ControlServerList(ServerListControl_Show))
		{
			//调整界面
			CRect rcClient;
			GetClientRect(&rcClient);
			RectifyControl(rcClient.Width(),rcClient.Height());
		}
	}

	return;
}

//激活房间
void CGameFrame::ActiveRoomViewItem(CRoomViewItem * pRoomViewItem)
{
	//效验参数
	ASSERT(pRoomViewItem!=NULL);
	if (pRoomViewItem==NULL) return;

	//判断创建
	if (m_pRoomViewItemCreate==pRoomViewItem)
	{
		m_pRoomViewItemCreate=NULL;
		MoveMemory(m_pRoomViewItem+1,m_pRoomViewItem,sizeof(m_pRoomViewItem[0])*(CountArray(m_pRoomViewItem)-1));
		m_pRoomViewItem[0]=pRoomViewItem;
	}

	//激活房间
	HWND hWndActive=NULL;
	if (m_pActiveViewItem!=NULL) hWndActive=m_pActiveViewItem->GetWindowHandle();
	if (pRoomViewItem->GetSafeHwnd()!=hWndActive)
	{
		m_pActiveViewItem=pRoomViewItem;
		pRoomViewItem->ShowWindow(SW_SHOW);
		if (hWndActive!=NULL) ::ShowWindow(hWndActive,SW_HIDE);

		//隐藏列表
		if(!ControlServerList(ServerListControl_Hide))
		{
			//调整界面
			CRect rcClient;
			GetClientRect(&rcClient);
			RectifyControl(rcClient.Width(),rcClient.Height());
		}
	}

	//更新控制
	UpdateControlButton();

	return;
}

//查找房间
CRoomViewItem * CGameFrame::SearchRoomViewItem(WORD wKindID, WORD wServerID)
{
	tagGameServer * pGameServer=NULL;
	for (int i=0;i<CountArray(m_pRoomViewItem);i++)
	{
		if (m_pRoomViewItem[i]==NULL) return NULL;
		pGameServer=m_pRoomViewItem[i]->GetServerInfo();
		if ((pGameServer->wKindID==wKindID)&&(pGameServer->wServerID==wServerID)) return m_pRoomViewItem[i];
	}

	return NULL;
}

//进入房间
CRoomViewItem * CGameFrame::CreateRoomViewItem(CListServer * pListServer)
{
	//判断状态
	tagGlobalUserData & GlobalUserData=g_GlobalUnits.GetGolbalUserData();
	if (GlobalUserData.dwUserID==0L)
	{
		ShowMessageBox(TEXT("你还没有登录，请先登录游戏广场！"),MB_ICONQUESTION);
		return NULL;
	}

	//效验参数
	ASSERT(pListServer!=NULL);
	CListKind * pListKind=pListServer->GetListKind();
	tagGameKind * pGameKind=pListKind->GetItemInfo();
	tagGameServer * pGameServer=pListServer->GetItemInfo();

	//判断连接
	if (m_pRoomViewItemCreate!=NULL)
	{
		tagGameServer * pGameServerCreate=m_pRoomViewItemCreate->GetServerInfo();
		if ((pGameServer->wKindID==pGameServerCreate->wKindID)&&
			(pGameServer->wServerID==pGameServerCreate->wServerID))
			return NULL;
	}

	//寻找房间
	CRoomViewItem * pRoomViewItem=SearchRoomViewItem(pGameServer->wKindID,pGameServer->wServerID);
	if (pRoomViewItem!=NULL) 
	{
		ActiveRoomViewItem(pRoomViewItem);
		return pRoomViewItem;
	}

	//进入房间数目
	if (m_pRoomViewItem[CountArray(m_pRoomViewItem)-1]!=NULL)
	{
		ShowMessageBox(TEXT("进入的游戏房间太多了，请关闭部分游戏房间再试！"),MB_ICONINFORMATION);
		return NULL;
	}

	//获取版本
	CWinFileInfo WinFileInfo;
	if (WinFileInfo.OpenWinFile(pGameKind->szProcessName)==false)
	{
		TCHAR szBuffer[512]=TEXT("");
		_snprintf(szBuffer,sizeof(szBuffer),TEXT("【%s】还没有安装，现在是否下载？"),pGameKind->szKindName);
		int nResult=ShowMessageBox(szBuffer,MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON1);
		if (nResult==IDYES) g_GlobalAttemper.DownLoadClient(pGameKind->szKindName,pGameKind->wKindID,true);
		return NULL;
	}

	//获取版本
	DWORD dwFileVerMS=0L,dwFileVerLS=0L;
	WinFileInfo.GetFileVersion(dwFileVerMS,dwFileVerLS);

	//版本分析
	BYTE cbFileVer1=(BYTE)(HIWORD(dwFileVerMS));
	BYTE cbFileVer2=(BYTE)(LOWORD(dwFileVerMS));
	BYTE cbFileVer3=(BYTE)(HIWORD(dwFileVerLS));
	BYTE cbFileVer4=(BYTE)(LOWORD(dwFileVerLS));
	BYTE cbListVer1=(BYTE)(LOWORD(LOWORD(pGameKind->dwMaxVersion)));
	BYTE cbListVer2=(BYTE)(HIBYTE(LOWORD(pGameKind->dwMaxVersion)));
	BYTE cbListVer3=(BYTE)(LOBYTE(HIWORD(pGameKind->dwMaxVersion)));
	BYTE cbListVer4=(BYTE)(HIBYTE(HIWORD(pGameKind->dwMaxVersion)));

	//判断版本
	if ((cbFileVer1!=cbListVer1)||(cbFileVer2!=cbListVer2)||(cbFileVer3!=cbListVer3))
	{
		TCHAR szBuffer[512]=TEXT("");
		_snprintf(szBuffer,sizeof(szBuffer),TEXT("【%s】已经更新为 %ld.%ld.%ld.%ld 版本，你的版本不能继续使用，现在是否下载？"),pGameKind->szKindName,
			cbListVer1,cbListVer2,cbListVer3,cbListVer4);
		int nResult=ShowMessageBox(szBuffer,MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON1);
		if (nResult==IDYES) g_GlobalAttemper.DownLoadClient(pGameKind->szKindName,pGameKind->wKindID,true);
		return NULL;
	}

	//兼容版本
	if (cbFileVer4!=cbListVer4)
	{
		TCHAR szBuffer[512]=TEXT("");
		_snprintf(szBuffer,sizeof(szBuffer),TEXT("【%s】已经更新为 %ld.%ld.%ld.%ld 版本，现在是否下载升级？"),pGameKind->szKindName,
			cbListVer1,cbListVer2,cbListVer3,cbListVer4);
		int nResult=ShowMessageBox(szBuffer,MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON1);
		if (nResult==IDYES) 
		{
			g_GlobalAttemper.DownLoadClient(pGameKind->szKindName,pGameKind->wKindID,true);
			return NULL;
		}
	}

	//创建房间
	try
	{
		pRoomViewItem=new CRoomViewItem;
		if (pRoomViewItem==NULL) return NULL;
		pRoomViewItem->Create(IDD_GAME_ROOM,AfxGetMainWnd());
		if (pRoomViewItem->InitRoomViewItem(pListServer)==false) throw TEXT("游戏房间初始化失败！");
	}
	catch (...) { SafeDelete(pRoomViewItem); }
	if (pRoomViewItem==NULL)
	{
		ShowMessageBox(TEXT("游戏房间创建失败"),MB_ICONQUESTION);
		return NULL;
	}

	//连接游戏
	bool bSuccess=pRoomViewItem->ConnectGameServer();
	if (bSuccess==false)
	{
		pRoomViewItem->DestroyWindow();
		SafeDelete(pRoomViewItem);
		return NULL;
	}

	//设置变量
	m_pRoomViewItemCreate=pRoomViewItem;

	//设置提示
	CString strBuffer;
	strBuffer.Format(TEXT("正在连接房间，请稍候..."));
	g_GlobalAttemper.ShowStatusMessage(strBuffer,pRoomViewItem);
	
	return m_pRoomViewItemCreate;
}

//关闭房间
void CGameFrame::CloseRoomViewItem(IRoomViewItem * pIRoomViewItem)
{
	//效验参数
	ASSERT(pIRoomViewItem!=NULL);
	if (pIRoomViewItem==NULL) return;

	//设置变量
	if (m_pRoomViewItemCreate!=pIRoomViewItem)
	{
		//关闭提示
		if (pIRoomViewItem->QueryCloseRoom()==false) return;

		//寻找房间
		for (int i=0;i<CountArray(m_pRoomViewItem);i++)
		{
			if (m_pRoomViewItem[i]==NULL) break;
			if (m_pRoomViewItem[i]==pIRoomViewItem) 
			{
				//移动数组
				if ((i+1)!=CountArray(m_pRoomViewItem))
				{
					int nMoveSize=(CountArray(m_pRoomViewItem)-i-1)*sizeof(m_pRoomViewItem[0]);
					MoveMemory(m_pRoomViewItem+i,m_pRoomViewItem+i+1,nMoveSize);
				}
				m_pRoomViewItem[CountArray(m_pRoomViewItem)-1]=NULL;

				//激活视图
				if (m_pActiveViewItem==pIRoomViewItem)
				{
					int nActivePos=CountArray(m_pRoomViewItem);
					for (int i=CountArray(m_pRoomViewItem)-1;i>=0;i--)
					{
						if (m_pRoomViewItem[i]!=NULL)
						{
							nActivePos=i;
							break;
						}
					}
					if (nActivePos==CountArray(m_pRoomViewItem)) ActivePlazaViewItem();
					else ActiveRoomViewItem(m_pRoomViewItem[nActivePos]);
				}

				//更新控制
				UpdateControlButton();

				break;
			}
		}
	}
	else m_pRoomViewItemCreate=NULL;

	//关闭房间
	pIRoomViewItem->CloseRoomViewItem();
	PostMessage(WM_CLOSE_ROOM_VIEW_ITEM,(WPARAM)pIRoomViewItem,(LPARAM)pIRoomViewItem);

	return;
}

//关闭房间
void CGameFrame::CloseAllRoomViewItem()
{
	//关闭房间
	IRoomViewItem * pIRoomViewItem=NULL;
	for (int i=0;i<CountArray(m_pRoomViewItem);i++)
	{
		if (m_pRoomViewItem[i]==NULL) break;

		//关闭房间
		pIRoomViewItem=QUERY_OBJECT_PTR_INTERFACE(m_pRoomViewItem[i],IRoomViewItem);
		pIRoomViewItem->CloseRoomViewItem();
		PostMessage(WM_CLOSE_ROOM_VIEW_ITEM,(WPARAM)pIRoomViewItem,(LPARAM)pIRoomViewItem);

		//设置变量
		m_pRoomViewItem[i]=NULL;
	}

	//关闭创建房间
	if (m_pRoomViewItemCreate!=NULL) 
	{
		//关闭房间
		g_GlobalAttemper.DestroyStatusWnd(NULL);
		m_pRoomViewItemCreate->CloseRoomViewItem();
		pIRoomViewItem=QUERY_OBJECT_PTR_INTERFACE(m_pRoomViewItemCreate,IRoomViewItem);
		PostMessage(WM_CLOSE_ROOM_VIEW_ITEM,(WPARAM)pIRoomViewItem,(LPARAM)pIRoomViewItem);

		//设置变量
		m_pRoomViewItemCreate=NULL;
	}

	//更新控制
	UpdateControlButton();

	return;
}

//关闭当前
void CGameFrame::CloseCurrentViewItem()
{
	//房间判断
	ASSERT(m_pActiveViewItem!=NULL);
	if (m_pActiveViewItem!=&m_DlgGamePlaza)
	{
		IRoomViewItem * pIRoomViewItem=QUERY_OBJECT_PTR_INTERFACE(m_pActiveViewItem,IRoomViewItem);
		ASSERT(pIRoomViewItem!=NULL);
		CloseRoomViewItem(pIRoomViewItem);
	}
	else AfxGetMainWnd()->PostMessage(WM_CLOSE,0,0); 

	return;
}

//更新控制
void CGameFrame::UpdateControlButton()
{
	//更新按钮
	for (int i=0;i<CountArray(m_btViewItem);i++)
	{
		if (m_pRoomViewItem[i]!=NULL)
		{
			tagGameKind * pGameKind=m_pRoomViewItem[i]->GetKindInfo();
			m_btViewItem[i].ShowWindow(SW_SHOW);
			m_btViewItem[i].SetWindowText(pGameKind->szKindName);
		}
		else m_btViewItem[i].ShowWindow(SW_HIDE);
	}

	return;
}

//调整控件
void CGameFrame::RectifyControl(int nWidth, int nHeight)
{
	//状态判断
	if (m_DlgGamePlaza.m_bInitDialog==false) return;
	if ((nWidth==0)||(nHeight==0)) return;

	//变量定义
	const int nSplitterWidth=__max(6,m_Splitter.GetButtonSize().cx);

	//获取位置
	CRect rcSplitter;
	m_Splitter.GetWindowRect(&rcSplitter);
	ScreenToClient(&rcSplitter);

	//调整拆分条
	if(!m_bHideGameList)
	{
		int nLessPos=nWidth/4,nMaxPos=nWidth/2;
		if (m_bRectify==false)
		{
			m_bRectify=true;
			rcSplitter.left=nLessPos;
			rcSplitter.right=rcSplitter.left+nSplitterWidth;
		}

		//设置拆分范围
		m_Splitter.SetSplitterRange(nLessPos,nMaxPos);
		if ((nWidth!=0)&&(nHeight!=0))
		{
			if (rcSplitter.left<nLessPos) 
			{
				rcSplitter.left=nLessPos;
				rcSplitter.right=rcSplitter.left+nSplitterWidth;
			}
			if (rcSplitter.right>nMaxPos)
			{
				rcSplitter.right=nMaxPos;
				rcSplitter.left=rcSplitter.right-nSplitterWidth;
			}
		}
	}
	else
	{
		//拆分位置
		rcSplitter.left=0;
		rcSplitter.right=rcSplitter.left+nSplitterWidth;
	}

	//位置调整
	rcSplitter.top=m_ImageInfoFrame.nTBorder-FRAME_EXALTATION;
	rcSplitter.bottom=nHeight-m_ImageInfoFrame.nBBorder;

	//分条控件
	HDWP hDwp=BeginDeferWindowPos(32);
	UINT uUserTitleHeight=m_UserInfoView.GetTitleHeight();
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS;
	DeferWindowPos(hDwp,m_Splitter,NULL,rcSplitter.left,rcSplitter.top,rcSplitter.Width(),rcSplitter.Height(),uFlags);

	//列表控件
	if(!m_bHideGameList)
	{
		DeferWindowPos(hDwp,m_ServerItemView,NULL,m_ImageInfoFrame.nLBorder+m_ImageInfoServer.nLBorder,m_ImageInfoFrame.nTBorder+m_ImageInfoServer.nTBorder+uUserTitleHeight-FRAME_EXALTATION,
			rcSplitter.right-m_ImageInfoServer.nLBorder-m_ImageInfoServer.nRBorder-m_ImageInfoFrame.nLBorder,nHeight-m_ImageInfoServer.nTBorder-m_ImageInfoServer.nBBorder-m_ImageInfoFrame.nTBorder-uUserTitleHeight-m_ImageInfoFrame.nBBorder+FRAME_EXALTATION,uFlags);
		DeferWindowPos(hDwp,m_UserCompanionList,NULL,m_ImageInfoFrame.nLBorder+m_ImageInfoServer.nLBorder,m_ImageInfoFrame.nTBorder+m_ImageInfoServer.nTBorder+uUserTitleHeight-FRAME_EXALTATION,
			rcSplitter.right-m_ImageInfoServer.nLBorder-m_ImageInfoServer.nRBorder-m_ImageInfoFrame.nLBorder,nHeight-m_ImageInfoServer.nTBorder-m_ImageInfoServer.nBBorder-m_ImageInfoFrame.nTBorder-uUserTitleHeight-m_ImageInfoFrame.nBBorder+FRAME_EXALTATION,uFlags);
	}

	//信息控件
	DeferWindowPos(hDwp,m_UserInfoView,NULL,m_ImageInfoFrame.nLBorder,m_ImageInfoFrame.nTBorder-FRAME_EXALTATION,rcSplitter.right-m_ImageInfoFrame.nLBorder,uUserTitleHeight,uFlags);

	//获取位置
	CRect rcItemRect;
	m_btGamePlaza.GetWindowRect(&rcItemRect);

	//移动控件
	INT nYPos = m_ImageInfoFrame.nTBorder-FRAME_EXALTATION;
	DeferWindowPos(hDwp,m_btGamePlaza,NULL,rcSplitter.right+5,nYPos,0,0,uFlags|SWP_NOSIZE);
	for (int i=0;i<CountArray(m_btViewItem);i++) DeferWindowPos(hDwp,m_btViewItem[i],NULL,rcSplitter.right+rcItemRect.Width()+rcItemRect.Width()*i+5,nYPos,0,0,uFlags|SWP_NOSIZE);

	//页面控件
	HWND hWndActive=m_pActiveViewItem->GetWindowHandle();
	DeferWindowPos(hDwp,hWndActive,NULL,rcSplitter.right,nYPos+rcItemRect.Height(),nWidth-m_ImageInfoFrame.nRBorder-rcSplitter.right,
		nHeight-(nYPos+rcItemRect.Height())-m_ImageInfoFrame.nBBorder,uFlags);

	//其他控件
	DeferWindowPos(hDwp,m_btMin,NULL,nWidth-102,3,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btMax,NULL,nWidth-72,3,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btClose,NULL,nWidth-42,3,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btExChangeSkin,NULL,nWidth-132,3,0,0,uFlags|SWP_NOSIZE);

	//列表按钮
	m_btListControl1.GetWindowRect(&rcItemRect);
	INT nY = m_ImageInfoFrame.nTBorder-FRAME_EXALTATION+uUserTitleHeight+5;
	DeferWindowPos(hDwp,m_btListControl1,NULL,m_ImageInfoFrame.nLBorder+5,nY,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btListControl2,NULL,m_ImageInfoFrame.nLBorder+5,nY+rcItemRect.Height()+1,0,0,uFlags|SWP_NOSIZE);

	//按钮位置
	CRect rcButton;
	m_btButton1.GetWindowRect(&rcButton);

	//间隔计算
	int nEndPos=105;
	int nBeginPos=560;
	int nWindth=nWidth-nBeginPos-rcButton.Width()*5-nEndPos;
	int nButtonSpace=__min((nWidth-nBeginPos-rcButton.Width()*5-nEndPos)/4,30);

	//广告控件
	DeferWindowPos(hDwp,m_BrowerAD,NULL,300,5,250,52,uFlags);

	//导航按钮
	DeferWindowPos(hDwp,m_btButton1,NULL,nWidth-rcButton.Width()*5-nButtonSpace*4-nEndPos,6,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btButton2,NULL,nWidth-rcButton.Width()*4-nButtonSpace*3-nEndPos,6,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btButton3,NULL,nWidth-rcButton.Width()*3-nButtonSpace*2-nEndPos,6,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btButton4,NULL,nWidth-rcButton.Width()*2-nButtonSpace*1-nEndPos,6,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btButton5,NULL,nWidth-rcButton.Width()*1-nButtonSpace*0-nEndPos,6,0,0,uFlags|SWP_NOSIZE);

	//获取位置
	CRect rcButtonRect;
	m_btSelfOption.GetWindowRect(&rcButtonRect);

	//移动控件
	nYPos = m_ImageInfoFrame.nTBorder-FRAME_EXALTATION-4;
	CButton * pButton[]={/*&m_btQuitGame,*/&m_btSelfOption,&m_btGlobalOption,&m_btSwitchUser};
	for (int i=0;i<CountArray(pButton);i++) DeferWindowPos(hDwp,pButton[i]->m_hWnd,NULL,nWidth-(rcButtonRect.Width()+6)*(i+1)-8,nYPos,0,0,uFlags|SWP_NOSIZE);

	//结束调整
	LockWindowUpdate();
	EndDeferWindowPos(hDwp);
	UnlockWindowUpdate();

	//更换皮肤
	for (int i=0;i<CountArray(m_btViewItem);i++)
	{
		if(m_pActiveViewItem==m_pRoomViewItem[i])break;
	}
	CSkinButton * pTempButton[]={&m_btViewItem[0],&m_btViewItem[1],&m_btViewItem[2],&m_btGamePlaza};
	ASSERT((CountArray(m_btViewItem)+1)==CountArray(pTempButton));
	for (WORD j=0;j<CountArray(pTempButton);j++)
	{
		if(i==j)
		{
			pTempButton[j]->SetTextColor(RGB(0,0,0),RGB(133,215,255),true);
			pTempButton[j]->SetButtonImage(GetPlatformRes().uBtFrameItemActive,GetResInstanceHandle(),false);
		}
		else
		{
			pTempButton[j]->SetTextColor(RGB(245,245,245),RGB(20,20,20),true);
			pTempButton[j]->SetButtonImage(GetPlatformRes().uBtFrameItemNormal,GetResInstanceHandle(),false);
		}
	}

	return;
}

//最大窗口
bool CGameFrame::MaxSizeWindow()
{
	//状态判断
	if (m_bMaxShow==false)
	{
		//默认位置
		GetWindowRect(&m_rcNormalSize);

		//设置按钮
		m_btMax.SetButtonImage(GetPlatformRes().uBtFrameResore,GetResInstanceHandle(),false);

		//获取位置
		CRect rcArce;
		SystemParametersInfo(SPI_GETWORKAREA,0,&rcArce,SPIF_SENDCHANGE);

		//移动窗口
		m_bMaxShow=true;
		LockWindowUpdate();
		SetWindowPos(NULL,rcArce.left-2,rcArce.top-2,rcArce.Width()+4,rcArce.Height()+4,SWP_NOZORDER);
		UnlockWindowUpdate();
	}

	return true;
}

//还原窗口
bool CGameFrame::RestoreWindow()
{
	//状态判断
	if (m_bMaxShow==true)
	{
		//设置按钮
		m_btMax.SetButtonImage(GetPlatformRes().uBtFrameMax,GetResInstanceHandle(),false);

		//移动窗口
		m_bMaxShow=false;
		LockWindowUpdate();
		SetWindowPos(NULL,m_rcNormalSize.left,m_rcNormalSize.top,m_rcNormalSize.Width(),m_rcNormalSize.Height(),SWP_NOZORDER);
		UnlockWindowUpdate();
	}

	return true;
}

//建立消息
int CGameFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//设置属性
	ModifyStyle(WS_CAPTION|WS_BORDER,0,0);

	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//设置标题
	CString strTitle;
	strTitle.LoadString(AfxGetInstanceHandle(),IDS_MAIN_TITLE);
	SetWindowText(strTitle);

	//广告控件
	m_BrowerAD.Create(NULL,NULL,WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),this,IDC_BROWER_AD,NULL);
	m_BrowerAD.Navigate(TEXT("http://www.dc173.com/AD/GamePlazaAD.asp"),NULL,NULL,NULL,NULL);

	//拆分条控件
	m_Splitter.Create(NULL,NULL,WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),this,IDC_SPLITTER,NULL);
	m_Splitter.SetSplitterSink(this);

	//用户视图
	m_UserInfoView.Create(IDD_USER_INFO,this);
	m_UserInfoView.ShowWindow(SW_SHOW);

	//服务器视图
	m_ServerItemView.Create(WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),this,IDC_SERVER_ITEM_VIEW);
	m_ServerItemView.InitServerItemView(this);
	g_GlobalUnits.m_ServerListManager.InitServerListManager(&m_ServerItemView);

	//好友列表
	m_wShowListType = IDC_BT_LIST_CONTROL_1;
	m_UserCompanionList.Create(WS_CHILD,CRect(0,0,0,0),this,IDC_COMPANION_LIST);

	//创建按钮
	m_btMin.Create(NULL,WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BT_MIN);
	m_btMax.Create(NULL,WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BT_MAX);
	m_btClose.Create(NULL,WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BT_CLOSE);
	m_btExChangeSkin.Create(NULL,WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BT_EXCHANGE_SINK);
	m_btExChangeSkin.ShowWindow(SW_HIDE);
	m_btButton1.Create(TEXT(""),WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BT_BUTTON_1);
	m_btButton2.Create(TEXT(""),WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BT_BUTTON_2);
	m_btButton3.Create(TEXT(""),WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BT_BUTTON_3);
	m_btButton4.Create(TEXT(""),WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BT_BUTTON_4);
	m_btButton5.Create(TEXT(""),WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BT_BUTTON_5);
	m_btGamePlaza.Create(TEXT("游戏大厅"),WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BT_PLAZA);
	m_btSelfOption.Create(TEXT(""),WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BT_SELF_OPTION);
	m_btSwitchUser.Create(TEXT(""),WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BT_SWITCH_ACCOUNTS);
	m_btGlobalOption.Create(TEXT(""),WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BT_SYSTEM_OPTION);
	m_btListControl1.Create(NULL,WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BT_LIST_CONTROL_1);
	m_btListControl2.Create(NULL,WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BT_LIST_CONTROL_2);
	for (WORD i=0;i<CountArray(m_btViewItem);i++)
	{
		CRect rcCreate(0,0,0,0);
		m_btViewItem[i].Create(NULL,WS_CHILD,rcCreate,this,IDC_BT_VIEW_ITEM+i);
	}

	//皮肤资源
	UpdateSkinResource();

	//建立提示控件
	m_ToolTipCtrl.Create(this);
	m_ToolTipCtrl.Activate(TRUE);
	m_ToolTipCtrl.AddTool(GetDlgItem(IDC_BT_MIN),TEXT("最小化游戏广场"));
	m_ToolTipCtrl.AddTool(GetDlgItem(IDC_BT_CLOSE),TEXT("退出游戏广场"));

	//创建控件
	m_DlgGamePlaza.Create(IDD_GAME_PLAZA,this);
	ActivePlazaViewItem();

	//注册热键
	g_GlobalUnits.RegisterHotKey(m_hWnd,IDI_HOT_KEY_BOSS,g_GlobalOption.m_wBossHotKey);

	//窗口位置
	CRect rcArce;
	SystemParametersInfo(SPI_GETWORKAREA,0,&rcArce,SPIF_SENDCHANGE);

	//显示窗口
	MaxSizeWindow();
	ShowWindow(SW_SHOW);

	//默认位置
	m_rcNormalSize.left=(rcArce.Width()-LESS_SCREEN_X)/2;
	m_rcNormalSize.top=(rcArce.Height()-LESS_SCREEN_Y)/2;
	m_rcNormalSize.right=(rcArce.Width()+LESS_SCREEN_X)/2;
	m_rcNormalSize.bottom=(rcArce.Height()+LESS_SCREEN_Y)/2;

	return 0;
}

//绘画消息
void CGameFrame::OnPaint()
{
	CPaintDC dc(this);

	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//游戏列表
	if (!m_bHideGameList)
	{
		//获取位置
		CRect rcSplitter;
		m_Splitter.GetWindowRect(&rcSplitter);
		ScreenToClient(&rcSplitter);

		//绘画列表
		CRect rcServer;
		rcServer.right=rcSplitter.right;
		rcServer.left=m_ImageInfoFrame.nLBorder;
		rcServer.bottom=rcClient.Height()-m_ImageInfoFrame.nBBorder;
		rcServer.top=m_ImageInfoFrame.nTBorder+m_UserInfoView.GetTitleHeight()-FRAME_EXALTATION;
		CSkinAide::DrawEncircleFrame(&dc,rcServer,m_EncircleServer);
	}

	//绘画框架
	CSkinAide::DrawEncircleFrame(&dc,rcClient,m_EncircleFrame);

	return;
}

//绘画背景
BOOL CGameFrame::OnEraseBkgnd(CDC * pDC)
{
	Invalidate(FALSE);
	UpdateWindow();
	return TRUE;
}

//设置改变
void CGameFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	__super::OnSettingChange(uFlags, lpszSection);

	//调整框架大小
	CRect rcClient;
	SystemParametersInfo(SPI_GETWORKAREA,0,&rcClient,SPIF_SENDCHANGE);
	MoveWindow(&rcClient,TRUE);

	return;
}

//光标信息
void CGameFrame::OnNcMouseMove(UINT nHitTest, CPoint point)
{
	//去掉菜单
	LONG lStyle = GetWindowLong(m_hWnd,GWL_STYLE);
	if((lStyle&WS_SYSMENU)!=0)ModifyStyle(WS_SYSMENU,0,0);

	__super::OnNcMouseMove(nHitTest, point);
}

//激活事件
void CGameFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	//增加菜单
	LONG lStyle = GetWindowLong(m_hWnd,GWL_STYLE);
	if((lStyle&WS_SYSMENU)==0)ModifyStyle(0,WS_SYSMENU,0);

	__super::OnActivate(nState, pWndOther, bMinimized);
}

//关闭消息
void CGameFrame::OnClose()
{
	//关闭提示
	if (g_GlobalUnits.GetGolbalUserData().dwUserID!=0L)
	{
		int nCode=ShowInformation(TEXT("您确实要关闭游戏广场吗？"),0,MB_YESNO|MB_DEFBUTTON2|MB_ICONQUESTION);
		if (nCode!=IDYES) return;
	}

	//隐藏界面
	ShowWindow(SW_HIDE);

	//关闭房间
	CloseAllRoomViewItem();

	//保存配置
	g_GlobalOption.SaveOptionParameter();
	g_GlobalUnits.m_CompanionManager->SaveCompanion();

	__super::OnClose();
}

//获取最大位置
void CGameFrame::OnGetMinMaxInfo(MINMAXINFO FAR * lpMMI) 
{
	//设置变量
	lpMMI->ptMinTrackSize.x=LESS_SCREEN_X;
	lpMMI->ptMinTrackSize.y=LESS_SCREEN_Y;

	__super::OnGetMinMaxInfo(lpMMI);
}

//位置消息
void CGameFrame::OnSize(UINT nType, int cx, int cy) 
{
	__super::OnSize(nType, cx, cy);

	//调整拆分条
	RectifyControl(cx,cy);

	return;
}

//按键测试
UINT CGameFrame::OnNcHitTest(CPoint Point)
{
	//按钮测试
	if (m_bMaxShow==false)
	{
		//获取位置
		CRect rcClient;
		GetClientRect(&rcClient);

		//转换位置
		CPoint ClientPoint=Point;
		ScreenToClient(&ClientPoint);

		//左面位置
		if (ClientPoint.x<=m_ImageInfoFrame.nRBorder)
		{
			if (ClientPoint.y<=m_ImageInfoFrame.nTBorder-FRAME_EXALTATION) return HTTOPLEFT;
			if (ClientPoint.y>=(rcClient.Height()-m_ImageInfoFrame.nRBorder)) return HTBOTTOMLEFT;
			return HTLEFT;
		}

		//右面位置
		if (ClientPoint.x>=(rcClient.Width()-m_ImageInfoFrame.nRBorder))
		{
			if (ClientPoint.y<=m_ImageInfoFrame.nTBorder-FRAME_EXALTATION) return HTTOPRIGHT;
			if (ClientPoint.y>=(rcClient.Height()-m_ImageInfoFrame.nRBorder)) return HTBOTTOMRIGHT;
			return HTRIGHT;
		}

		//上下位置
		if (ClientPoint.y<=m_ImageInfoFrame.nRBorder) return HTTOP;
		if (ClientPoint.y>=(rcClient.Height()-m_ImageInfoFrame.nRBorder)) return HTBOTTOM;
	}

	return __super::OnNcHitTest(Point);
}

//鼠标消息
void CGameFrame::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags,Point);

	//模拟按标题
	if ((m_bMaxShow==false)&&(Point.y<=m_ImageInfoFrame.nTBorder-FRAME_EXALTATION))
	{
		PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(Point.x,Point.y));
	}

	return;
}

//鼠标消息
void CGameFrame::OnLButtonDblClk(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDblClk(nFlags,Point);

	//状态判断
	if (Point.y>m_ImageInfoFrame.nTBorder-FRAME_EXALTATION) return;

	//控制窗口
	if (m_bMaxShow==true) RestoreWindow();
	else MaxSizeWindow();

	return;
}

//安装完成
LRESULT CGameFrame::OnMessageSetupFinish(WPARAM wParam, LPARAM lParam)
{
	g_GlobalUnits.m_ServerListManager.UpdateGameKind((WORD)wParam);
	return 0;
}

//热键消息
LRESULT CGameFrame::OnHotKeyMessage(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case IDI_HOT_KEY_BOSS:		//老板热键
		{
			//变量定义
			bool bBossCome=(IsWindowVisible()==FALSE)?false:true;

			//隐藏窗口
			if (bBossCome==false)
			{
				//还原窗口
				ShowWindow(SW_RESTORE);
				ShowWindow(SW_SHOW);

				//置顶窗口
				SetActiveWindow();
				BringWindowToTop();
				SetForegroundWindow();
			}
			else
			{
				//隐藏窗口
				ShowWindow(SW_MINIMIZE);
				ShowWindow(SW_HIDE);
			}

			//发送消息
			for (INT_PTR i=0;i<MAX_SERVER;i++)
			{
				if (m_pRoomViewItem[i]!=NULL)
				{
					m_pRoomViewItem[i]->NotifyBossCome(bBossCome);
				}
			}

			return 0;
		}
	}

	return 0;
}

//关闭房间
LRESULT CGameFrame::OnCloseRoomViewItem(WPARAM wParam, LPARAM lParam)
{
	//释放房间
	IRoomViewItem * pIRoomViewItem=(IRoomViewItem *)wParam;
	if (pIRoomViewItem!=NULL) pIRoomViewItem->Release();

	return 0;
}

//////////////////////////////////////////////////////////////////////////
