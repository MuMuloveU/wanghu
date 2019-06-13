#include "Stdafx.h"
#include "Resource.h"
#include "CompanionTreeCtrl.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

#define IMAGE_FRIEND					1					//好友图标
#define IMAGE_DETEST					1					//厌恶图标
#define IMAGE_FRIEND_ITEM				5					//好友图标
#define IMAGE_DETEST_ITEM				5					//厌恶图标

#define WN_SET_LINE_COLOR				(TV_FIRST+40)		//设置颜色消息

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CCompanionTreeCtrl, CTreeCtrl)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CCompanionTreeCtrl::CCompanionTreeCtrl()
{
	m_hItemDetest=NULL;
	m_hItemFriend=NULL;
}

//析构函数
CCompanionTreeCtrl::~CCompanionTreeCtrl()
{
}

//控件绑定
void CCompanionTreeCtrl::PreSubclassWindow()
{
	//加载图片
	if (m_ImageList.GetSafeHandle()==NULL)
	{
		CBitmap TreeImage;
		AfxSetResourceHandle(GetModuleHandle(COMPANION_DLL_NAME));
		TreeImage.LoadBitmap(IDB_TREE_IMAGE);
		AfxSetResourceHandle(GetModuleHandle(NULL));
		m_ImageList.Create(18,18,ILC_COLOR16|ILC_MASK,0,0);
		m_ImageList.Add(&TreeImage,RGB(255,0,255));
	}
	SetImageList(&m_ImageList,LVSIL_NORMAL);

	//设置控件
	SetItemHeight(20);
	SetTextColor(RGB(10,10,10));
	SetBkColor(RGB(245,255,205));
	SendMessage(WN_SET_LINE_COLOR,0,(LPARAM)RGB(69,69,69));

	//插入子项
	TV_INSERTSTRUCT InsertInf;
	memset(&InsertInf,0,sizeof(InsertInf));
	InsertInf.item.cchTextMax=128;
	InsertInf.hInsertAfter=TVI_LAST;
	InsertInf.item.mask=TVIF_IMAGE|TVIF_SELECTEDIMAGE|TVIF_TEXT|TVIF_PARAM;

	//游戏好友
	InsertInf.item.iImage=IMAGE_FRIEND;
	InsertInf.item.iSelectedImage=IMAGE_FRIEND;
	InsertInf.item.pszText=TEXT("游戏好友");
	m_hItemFriend=InsertItem(&InsertInf);

	//厌恶玩家
	InsertInf.item.iImage=IMAGE_DETEST;
	InsertInf.item.iSelectedImage=IMAGE_DETEST;
	InsertInf.item.pszText=TEXT("游戏黑名单");
	m_hItemDetest=InsertItem(&InsertInf);

	__super::PreSubclassWindow();
}

//展开列表
bool CCompanionTreeCtrl::ExpandListItem()
{
	//展开列表
	Expand(m_hItemDetest,TVE_EXPAND);
	Expand(m_hItemFriend,TVE_EXPAND);

	return true;
}

//加入关系
bool CCompanionTreeCtrl::InsertCompanionItem(const tagCompanionItem * pCompanionItem)
{
	//效验参数
	ASSERT(pCompanionItem!=NULL);
	if (pCompanionItem==NULL) return false;

	//计算位图
	int nImageStation=0;
	HTREEITEM hParentTreeItem=NULL;
	if (pCompanionItem->Companion==enCompanion_Friend) 
	{
		hParentTreeItem=m_hItemFriend;
		nImageStation=IMAGE_FRIEND_ITEM;
	}
	else if (pCompanionItem->Companion==enCompanion_Detest) 
	{
		hParentTreeItem=m_hItemDetest;
		nImageStation=IMAGE_DETEST_ITEM;
	}
	else return false;

	//生成标题
	TCHAR szTreeTitle[128]=TEXT("");
	_snprintf(szTreeTitle,sizeof(szTreeTitle),TEXT("%s [ %ld ]"),pCompanionItem->szAccounts,pCompanionItem->dwUserID);

	//插入关系
	TV_INSERTSTRUCT InsertInf;
	memset(&InsertInf,0,sizeof(InsertInf));
	InsertInf.hInsertAfter=TVI_LAST;
	InsertInf.hParent=hParentTreeItem;
	InsertInf.item.iImage=nImageStation;
	InsertInf.item.iSelectedImage=nImageStation;
	InsertInf.item.pszText=szTreeTitle;
	InsertInf.item.lParam=(LPARAM)pCompanionItem;
	InsertInf.item.cchTextMax=CountArray(szTreeTitle);
	InsertInf.item.mask=TVIF_IMAGE|TVIF_SELECTEDIMAGE|TVIF_TEXT|TVIF_PARAM;
	HTREEITEM hTreeItem=InsertItem(&InsertInf);

	//展开控件
	Expand(hParentTreeItem,TVE_EXPAND);

	return true;
}

//删除关系
bool CCompanionTreeCtrl::DeleteCompanionItem(const tagCompanionItem * pCompanionItem)
{
	HTREEITEM hTreeItem=SearchCompanionItem(NULL,(DWORD_PTR)pCompanionItem);
	if (hTreeItem!=NULL)
	{
		DeleteItem(hTreeItem);
		return true;
	}
	return false;
}

//寻找项
HTREEITEM CCompanionTreeCtrl::SearchCompanionItem(HTREEITEM hRootTreeItem, DWORD_PTR dwParam)
{
	//获取父项
	if (hRootTreeItem==NULL) hRootTreeItem=GetRootItem();
	if (hRootTreeItem==NULL) return NULL;

	//循环查找
	HTREEITEM hTreeItemTemp=NULL;
	do
	{
		if (GetItemData(hRootTreeItem)==dwParam) return hRootTreeItem;
		hTreeItemTemp=GetChildItem(hRootTreeItem);
		if (hTreeItemTemp!=NULL)
		{
			hTreeItemTemp=SearchCompanionItem(hTreeItemTemp,dwParam);
			if (hTreeItemTemp!=NULL) return hTreeItemTemp;
		}
		hRootTreeItem=GetNextItem(hRootTreeItem,TVGN_NEXT);
	} while (hRootTreeItem!=NULL);

	return NULL;
}

//////////////////////////////////////////////////////////////////////////

