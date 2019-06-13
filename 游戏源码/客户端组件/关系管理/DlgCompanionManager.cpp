#include "Stdafx.h"
#include "Resource.h"
#include "DlgCompanionManager.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgCompanionManager, CSkinDialogEx)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgCompanionManager::CDlgCompanionManager() : CSkinDialogEx(IDD_COMPANION_MANAGER)
{
	m_pICompanionManager=NULL;
}

//析构函数
CDlgCompanionManager::~CDlgCompanionManager()
{
}

//控件绑定
void CDlgCompanionManager::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX,IDCANCEL,m_btClose);
	DDX_Control(pDX,IDC_FRIEND_LIST,m_FriendListCtrl);
	DDX_Control(pDX,IDC_DETEST_LIST,m_DetestListCtrl);
	DDX_Control(pDX,IDC_COMPANION_TREE,m_CompanionTreeCtrl);
}

//初始化函数
BOOL CDlgCompanionManager::OnInitDialog()
{
	__super::OnInitDialog();

	//设置标题
	SetWindowText(TEXT("用户资料管理"));

	//加入信息
	WORD wIndex=0;
	const tagCompanionItem * pCompanionItem=NULL;
	do
	{
		//枚举关系
		pCompanionItem=m_pICompanionManager->EmunCompanionItem(wIndex++);
		if (pCompanionItem==NULL) break;

		//控制界面
		m_CompanionTreeCtrl.InsertCompanionItem(pCompanionItem);
		if (pCompanionItem->Companion==enCompanion_Friend) m_FriendListCtrl.InsertCompanionItem(pCompanionItem);
		if (pCompanionItem->Companion==enCompanion_Detest) m_DetestListCtrl.InsertCompanionItem(pCompanionItem);
	} while (true);

	//展开列表
	m_CompanionTreeCtrl.ExpandListItem();

	return TRUE;
}

//取消消息
void CDlgCompanionManager::OnCancel()
{
	DestroyWindow();
	return;
}

//设置接口
bool CDlgCompanionManager::SetCompanionManager(IUnknownEx * pIUnknownEx)
{
	ASSERT(pIUnknownEx!=NULL);
	m_pICompanionManager=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ICompanionManager);
	ASSERT(m_pICompanionManager!=NULL);
	return (m_pICompanionManager!=NULL);
}

//加入关系
bool CDlgCompanionManager::InsertCompanionItem(const tagCompanionItem * pCompanionItem)
{
	if (GetSafeHwnd()!=NULL)
	{
		m_CompanionTreeCtrl.InsertCompanionItem(pCompanionItem);
		if (pCompanionItem->Companion==enCompanion_Friend)
		{
			m_FriendListCtrl.InsertCompanionItem(pCompanionItem);
		}
		else if (pCompanionItem->Companion==enCompanion_Detest) 
		{
			m_DetestListCtrl.InsertCompanionItem(pCompanionItem);
		}
	}

	return true;
}

//更新关系
bool CDlgCompanionManager::UpdateCompanionItem(const tagCompanionItem * pCompanionItem, bool bChangeCompanion)
{
	if (GetSafeHwnd()!=NULL)
	{
		if (bChangeCompanion==true)
		{
			//删除老数据
			m_FriendListCtrl.DeleteCompanionItem(pCompanionItem);
			m_DetestListCtrl.DeleteCompanionItem(pCompanionItem);
			m_CompanionTreeCtrl.DeleteCompanionItem(pCompanionItem);

			//插入新数据
			m_CompanionTreeCtrl.InsertCompanionItem(pCompanionItem);
			if (pCompanionItem->Companion==enCompanion_Friend) m_FriendListCtrl.InsertCompanionItem(pCompanionItem);
			if (pCompanionItem->Companion==enCompanion_Detest) m_DetestListCtrl.InsertCompanionItem(pCompanionItem);
		}
		else
		{
			if (pCompanionItem->Companion==enCompanion_Friend) m_FriendListCtrl.UpdateCompanionItem(pCompanionItem);
			if (pCompanionItem->Companion==enCompanion_Detest) m_DetestListCtrl.UpdateCompanionItem(pCompanionItem);
		}
	}

	return true;
}

//删除关系
bool CDlgCompanionManager::DeleteCompanionItem(const tagCompanionItem * pCompanionItem)
{
	if (GetSafeHwnd()!=NULL)
	{
		m_CompanionTreeCtrl.DeleteCompanionItem(pCompanionItem);
		if (pCompanionItem->Companion==enCompanion_Friend)
		{
			m_FriendListCtrl.DeleteCompanionItem(pCompanionItem);
		}
		else if (pCompanionItem->Companion==enCompanion_Detest) 
		{
			m_DetestListCtrl.DeleteCompanionItem(pCompanionItem);
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
