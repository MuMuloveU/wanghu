#include "Stdafx.h"
#include "Resource.h"
#include "CompanionListCtrl.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

#define IMAGE_FRIEND_ITEM				6					//好友图标
#define IMAGE_DETEST_ITEM				12					//厌恶图标

//////////////////////////////////////////////////////////////////////////
//静态变量

CImageList		CCompanionListCtrl::m_ImageList;			//图片资源

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CCompanionListCtrl, CSkinListCtrl)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CCompanionListCtrl::CCompanionListCtrl()
{
}

//析构函数
CCompanionListCtrl::~CCompanionListCtrl()
{
}

//绑定消息
void CCompanionListCtrl::PreSubclassWindow()
{
	//加载背景图
	if (m_ImageList.GetSafeHandle()==NULL)
	{
		CBitmap Image;
		Image.LoadBitmap(IDB_LIST_IMAGE);
		m_ImageList.Create(24,16,ILC_COLOR16|ILC_MASK,0,0);
		m_ImageList.Add(&Image,RGB(255,0,255));
	}
	SetImageList(&m_ImageList,LVSIL_SMALL);

	//设置颜色
	SetTextColor(RGB(10,10,10));
	SetBkColor(RGB(245,255,205));
	SetTextBkColor(RGB(245,255,205));

	//加入列表
	InsertColumn(0,TEXT("关系名"),LVCFMT_LEFT,100);
	InsertColumn(1,TEXT("游戏 ID"),LVCFMT_LEFT,80);
	InsertColumn(2,TEXT("备注信息"),LVCFMT_LEFT,200);

	__super::PreSubclassWindow();
}

//加入关系
bool CCompanionListCtrl::InsertCompanionItem(const tagCompanionItem * pCompanionItem)
{
	//效验参数
	ASSERT(pCompanionItem!=NULL);
	if (pCompanionItem==NULL) return false;

	//计算位图
	int nImageStation=0;
	if (pCompanionItem->Companion==enCompanion_Friend) nImageStation=IMAGE_FRIEND_ITEM;
	else if (pCompanionItem->Companion==enCompanion_Detest) nImageStation=IMAGE_DETEST_ITEM;
	else return false;

	//插入数据
	TCHAR szBuffer[32]=TEXT("");
	int iItem=InsertItem(LVIF_IMAGE|LVIF_TEXT|LVIF_PARAM,0,pCompanionItem->szAccounts,0,0,nImageStation,(LPARAM)pCompanionItem);
	SetItem(iItem,1,LVIF_TEXT,ltoa(pCompanionItem->dwGameID,szBuffer,10),0,0,0,0);
	SetItem(iItem,2,LVIF_TEXT,pCompanionItem->szNote,0,0,0,0);

	return true;
}

//更新关系
bool CCompanionListCtrl::UpdateCompanionItem(const tagCompanionItem * pCompanionItem)
{
	//效验参数
	ASSERT(pCompanionItem!=NULL);
	if (pCompanionItem==NULL) return false;
	
	//查找关系
	LVFINDINFO FindInfo;
	memset(&FindInfo,0,sizeof(FindInfo));
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pCompanionItem;
	int iItem=FindItem(&FindInfo);
	if (iItem==-1) return false;

	//计算位图
	int nImageStation=0;
	if (pCompanionItem->Companion==enCompanion_Friend) nImageStation=IMAGE_FRIEND_ITEM;
	else if (pCompanionItem->Companion==enCompanion_Detest) nImageStation=IMAGE_DETEST_ITEM;
	else return false;

	//更新关系
	TCHAR szBuffer[32]=TEXT("");
	SetItem(iItem,0,LVIF_IMAGE|LVIF_TEXT,pCompanionItem->szAccounts,nImageStation,0,0,0);
	SetItemText(iItem,1,ltoa(pCompanionItem->dwGameID,szBuffer,10));
	SetItemText(iItem,2,pCompanionItem->szNote);

	return true;
}

//删除关系
bool CCompanionListCtrl::DeleteCompanionItem(const tagCompanionItem * pCompanionItem)
{
	//效验参数
	ASSERT(pCompanionItem!=NULL);
	if (pCompanionItem==NULL) return false;
	
	//查找关系
	LVFINDINFO FindInfo;
	memset(&FindInfo,0,sizeof(FindInfo));
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pCompanionItem;
	int iItem=FindItem(&FindInfo);
	if (iItem!=-1)
	{
		DeleteItem(iItem);
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
