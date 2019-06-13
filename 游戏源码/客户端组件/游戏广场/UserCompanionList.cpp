#include "StdAfx.h"
#include "Resource.h"
#include "GlobalUnits.h"
#include "UserCompanionList.h"

//////////////////////////////////////////////////////////////////////////

//索引定义
#define IMAGE_FRIEND				1									//好友图标
#define IMAGE_DETEST				1									//厌恶图标
#define IMAGE_NORMAL				1									//陌生图标
#define IMAGE_FRIEND_ITEM			5									//好友图标
#define IMAGE_DETEST_ITEM			5									//厌恶图标
#define IMAGE_NORMAL_ITEM			5									//陌生图标

//常量定义
#define ITEM_SIZE					18									//子项高度
#define WN_SET_LINE_COLOR			(TV_FIRST+40)						//设置颜色

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CUserCompanionList, CTreeCtrl)
	ON_WM_CREATE()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CUserCompanionList::CUserCompanionList()
{
	//设置变量
	m_hItemRoot=NULL;
	m_hItemDetest=NULL;
	m_hItemFriend=NULL;
	m_hItemNormal=NULL;

	return;
}

//析构函数
CUserCompanionList::~CUserCompanionList()
{
}

//插入通知
bool __cdecl CUserCompanionList::OnCompanionInsert(tagCompanionItem * pCompanionInfo)
{
	//获取树项
	ASSERT(pCompanionInfo!=NULL);
	HTREEITEM hTreeItem=SearchCompanionInfo(NULL,pCompanionInfo);

	//事件处理
	if (hTreeItem==NULL) InsertCompanionInfo(pCompanionInfo);
	else UpdateCompanionInfo(hTreeItem,pCompanionInfo);

	return true;
}

//更新通知
bool __cdecl CUserCompanionList::OnCompanionUpdate(tagCompanionItem * pCompanionInfo)
{
	//获取树项
	ASSERT(pCompanionInfo!=NULL);
	HTREEITEM hTreeItem=SearchCompanionInfo(NULL,pCompanionInfo);

	//事件处理
	if (hTreeItem==NULL) InsertCompanionInfo(pCompanionInfo);
	else UpdateCompanionInfo(hTreeItem,pCompanionInfo);

	//更新界面
	Invalidate(FALSE);
	UpdateWindow();

	return true;
}

//删除通知
bool __cdecl CUserCompanionList::OnCompanionDelete(tagCompanionItem * pCompanionInfo)
{
	//删除处理
	ASSERT(pCompanionInfo!=NULL);
	DeleteCompanionInfo(pCompanionInfo);

	return true;
}

//加入关系
bool CUserCompanionList::InsertCompanionInfo(const tagCompanionItem * pCompanionInfo)
{
	//效验参数
	ASSERT(pCompanionInfo!=NULL);
	if (pCompanionInfo==NULL) return false;

	//变量定义
	INT nImageIndex=0;
	HTREEITEM hParentTreeItem=NULL;

	//插入准备
	switch (pCompanionInfo->Companion)
	{
	case enCompanion_Friend:		//游戏好友
		{
			hParentTreeItem=m_hItemFriend;
			nImageIndex=IMAGE_FRIEND_ITEM;
			break;
		}
	case enCompanion_Detest:		//厌恶玩家
		{
			hParentTreeItem=m_hItemDetest;
			nImageIndex=IMAGE_DETEST_ITEM;
			break;
		}
	default:						//默认玩家
		{
			hParentTreeItem=m_hItemNormal;
			nImageIndex=IMAGE_NORMAL_ITEM;
			break;
		}
	}

	//生成标题
	TCHAR szTreeTitle[128]=TEXT("");
	_sntprintf(szTreeTitle,CountArray(szTreeTitle),TEXT("%s [ %ld ]"),pCompanionInfo->szAccounts,pCompanionInfo->dwGameID);

	//变量定义
	TV_INSERTSTRUCT InsertStruct;
	ZeroMemory(&InsertStruct,sizeof(InsertStruct));

	//构造变量
	InsertStruct.hInsertAfter=TVI_LAST;
	InsertStruct.hParent=hParentTreeItem;
	InsertStruct.item.iImage=nImageIndex;
	InsertStruct.item.iSelectedImage=nImageIndex;
	InsertStruct.item.pszText=szTreeTitle;
	InsertStruct.item.lParam=(LPARAM)pCompanionInfo;
	InsertStruct.item.cchTextMax=CountArray(szTreeTitle);
	InsertStruct.item.mask=TVIF_IMAGE|TVIF_SELECTEDIMAGE|TVIF_TEXT|TVIF_PARAM;

	//插入子项
	InsertItem(&InsertStruct);

	return true;
}

//删除关系
bool CUserCompanionList::DeleteCompanionInfo(tagCompanionItem * pCompanionInfo)
{
	//搜索树项
	HTREEITEM hTreeItem=SearchCompanionInfo(NULL,pCompanionInfo);

	//删除树项
	if (hTreeItem!=NULL)
	{
		DeleteItem(hTreeItem);
		return true;
	}

	return false;
}

//更新信息
bool CUserCompanionList::UpdateCompanionInfo(HTREEITEM hTreeItem, tagCompanionItem * pCompanionInfo)
{
	//效验参数
	ASSERT(hTreeItem!=NULL);
	if (hTreeItem==NULL) return false;

	//更新处理
	if (GetTreeItemCompanion(hTreeItem)==pCompanionInfo->Companion)
	{
		//生成标题
		TCHAR szTreeTitle[128]=TEXT("");
		_sntprintf(szTreeTitle,CountArray(szTreeTitle),TEXT("%s [ %ld ]"),pCompanionInfo->szAccounts,pCompanionInfo->dwGameID);

		//更新树项
		SetItemText(hTreeItem,szTreeTitle);
	}
	else
	{
		//删除树项
		DeleteItem(hTreeItem);
		InsertCompanionInfo(pCompanionInfo);
	}
	
	return true;
}

//获取关系
BYTE CUserCompanionList::GetTreeItemCompanion(HTREEITEM hTreeItem)
{
	//效验参数
	ASSERT(hTreeItem!=NULL);
	if (hTreeItem==NULL) return enCompanion_UnKnow;

	//上溯搜索
	do
	{
		//目标判断
		if (hTreeItem==m_hItemDetest) return enCompanion_Detest;
		if (hTreeItem==m_hItemFriend) return enCompanion_Friend;
		if (hTreeItem==m_hItemNormal) return enCompanion_UnKnow;

		//获取父项
		hTreeItem=GetParentItem(hTreeItem);

	} while (hTreeItem!=NULL);

	//错误断言
	ASSERT(FALSE);

	return enCompanion_UnKnow;
}

//寻找树项
HTREEITEM CUserCompanionList::SearchCompanionInfo(HTREEITEM hRootTreeItem, tagCompanionItem * pCompanionInfo)
{
	//获取父项
	if (hRootTreeItem==NULL) hRootTreeItem=m_hItemRoot;
	if (hRootTreeItem==NULL) return NULL;

	//变量定义
	HTREEITEM hTreeItemTemp=NULL;
	DWORD_PTR dwBindParam=(DWORD_PTR)pCompanionInfo;

	//循环查找
	do
	{
		//绑定判断
		if (GetItemData(hRootTreeItem)==dwBindParam) return hRootTreeItem;

		//子项搜索
		hTreeItemTemp=GetChildItem(hRootTreeItem);
		if (hTreeItemTemp!=NULL)
		{
			hTreeItemTemp=SearchCompanionInfo(hTreeItemTemp,pCompanionInfo);
			if (hTreeItemTemp!=NULL) return hTreeItemTemp;
		}

		//下一树项
		hRootTreeItem=GetNextItem(hRootTreeItem,TVGN_NEXT);

	} while (hRootTreeItem!=NULL);

	return NULL;
}

//创建消息
INT CUserCompanionList::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//设置控件
	SetItemHeight(20);
	SetTextColor(RGB(0,0,0));
	SetBkColor(RGB(230,249,255));
	ModifyStyle(0,TVS_HASBUTTONS|TVS_HASLINES|TVS_SHOWSELALWAYS|TVS_TRACKSELECT);
	SendMessage(WN_SET_LINE_COLOR,0,(LPARAM)RGB(72,79,63));

	//加载图片
	if (m_ImageCompanionList.GetSafeHandle()==NULL)
	{
		CBitmap ServerImage;
		ServerImage.LoadBitmap(IDB_SERVER_LIST_IMAGE);
		m_ImageCompanionList.Create(18,18,ILC_COLOR16|ILC_MASK,0,0);
		m_ImageCompanionList.Add(&ServerImage,RGB(255,0,255));
		SetImageList(&m_ImageCompanionList,LVSIL_NORMAL);
	}

	//设置列表
	m_hItemRoot=InsertItem(TEXT("好友管理"),0,0);
	m_hItemFriend=InsertItem(TEXT("游戏好友"),IMAGE_FRIEND,IMAGE_FRIEND,m_hItemRoot);
	m_hItemDetest=InsertItem(TEXT("黑名单"),IMAGE_DETEST,IMAGE_DETEST,m_hItemRoot);
	m_hItemNormal=InsertItem(TEXT("陌生人"),IMAGE_NORMAL,IMAGE_NORMAL,m_hItemRoot);

	//加载列表
	WORD wIndex=0;
	do
	{
		//获取关系
		const tagCompanionItem * pCompanionInfo=g_GlobalUnits.m_CompanionManager->EmunCompanionItem(wIndex++);
 
		//终止判断
		if (pCompanionInfo==NULL) break;

		//过虑本身
		if(g_GlobalUnits.GetGolbalUserData().dwUserID==pCompanionInfo->dwUserID)continue;

		//插入子项
		InsertCompanionInfo(pCompanionInfo);

	} while (true);

	//展开列表
	Expand(m_hItemRoot,TVE_EXPAND);
	Expand(m_hItemFriend,TVE_EXPAND);

	return 0;
}

//////////////////////////////////////////////////////////////////////////
