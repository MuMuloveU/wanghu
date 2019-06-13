#include "StdAfx.h"
#include "UserManager.h"

//////////////////////////////////////////////////////////////////////////

//静态变量
CUserItemPtrArray		CClientUserManager::m_UserItemStorage;			//存储数组

//////////////////////////////////////////////////////////////////////////

//构造函数
CUserItem::CUserItem() 
{ 
	CleanData(); 
}

//析构函数
CUserItem::~CUserItem() 
{
}

//接口查询
void * __cdecl CUserItem::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IUserItem,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IUserItem,Guid,dwQueryVer);
	return NULL;
}

//游戏局数
LONG CUserItem::GetUserPlayCount()
{
	return m_UserData.lWinCount+m_UserData.lLostCount+m_UserData.lDrawCount+m_UserData.lFleeCount;
}

//设置积分
void __cdecl CUserItem::SetUserScore(const tagUserScore * pUserScore)
{
	//效验参数
	ASSERT(m_bActive==true);
	ASSERT(pUserScore!=NULL);

	//设置变量
	m_UserData.lScore=pUserScore->lScore;
	m_UserData.lWinCount=pUserScore->lWinCount;
	m_UserData.lLostCount=pUserScore->lLostCount;
	m_UserData.lDrawCount=pUserScore->lDrawCount;
	m_UserData.lFleeCount=pUserScore->lFleeCount;
	m_UserData.lExperience=pUserScore->lExperience;

	return;
}

//设置状态
void __cdecl CUserItem::SetUserStatus(const tagUserStatus * pUserStatus)
{
	//效验参数
	ASSERT(m_bActive==true);
	ASSERT(pUserStatus!=NULL);

	//设置变量
	m_UserData.wTableID=pUserStatus->wTableID;
	m_UserData.wChairID=pUserStatus->wChairID;
	m_UserData.cbUserStatus=pUserStatus->cbUserStatus;

	return;
}

//清理数据
void CUserItem::CleanData() 
{
	m_bActive=false;
	memset(&m_UserData,0,sizeof(m_UserData));
	m_UserData.wTableID=INVALID_TABLE;
	m_UserData.wChairID=INVALID_CHAIR;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CClientUserManager::CClientUserManager(void)
{
	m_pIUserManagerSink=NULL;
}

//接口查询
void * __cdecl CClientUserManager::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IClientUserManager,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IClientUserManager,Guid,dwQueryVer);
	return NULL;
}

//析构函数
CClientUserManager::~CClientUserManager(void)
{
	//释放用户
	m_UserItemStorage.Append(m_UserItemActive);
	m_UserItemActive.RemoveAll();

	return;
}

//增加用户
IUserItem * __cdecl CClientUserManager::ActiveUserItem(tagUserData & UserData)
{
	//寻找用户
	CUserItem * pUserItem=NULL;
	INT_PTR nStorageCount=m_UserItemStorage.GetCount();
	if (nStorageCount>0)
	{
		pUserItem=m_UserItemStorage[nStorageCount-1];
		ASSERT(pUserItem!=NULL);
		m_UserItemStorage.RemoveAt(nStorageCount-1);
	}
	else
	{
		pUserItem=new CUserItem;
		ASSERT(pUserItem!=NULL);
		if (pUserItem==NULL) return false;
	}

	//拷贝数据
	pUserItem->m_bActive=true;
	CopyMemory(&pUserItem->m_UserData,&UserData,sizeof(UserData));
	m_UserItemActive.Add(pUserItem);

	//更新信息
	ASSERT(m_pIUserManagerSink!=NULL);
	m_pIUserManagerSink->OnUserItemAcitve(pUserItem);

	return pUserItem;
}

//删除用户
bool __cdecl CClientUserManager::DeleteUserItem(IUserItem * pIUserItem)
{
	//查找用户
	CUserItem * pUserItemActive=NULL;
	for (INT_PTR i=0;i<m_UserItemActive.GetCount();i++)
	{
		pUserItemActive=m_UserItemActive[i];
		ASSERT(pUserItemActive!=NULL);
		if (pIUserItem==pUserItemActive)
		{
			//删除用户
			m_UserItemActive.RemoveAt(i);
			m_UserItemStorage.Add(pUserItemActive);
			m_pIUserManagerSink->OnUserItemDelete(pUserItemActive);
			pUserItemActive->CleanData();
			return true;
		}
	}
	ASSERT(FALSE);

	return false;
}

//更新积分
bool __cdecl CClientUserManager::UpdateUserItemScore(IUserItem * pIUserItem, const tagUserScore * pUserScore)
{
	//效验参数
	ASSERT(pUserScore!=NULL);
	if (pUserScore==NULL) return false;

	//设置数据
	pIUserItem->SetUserScore(pUserScore);

	//通知更新
	ASSERT(m_pIUserManagerSink!=NULL);
	m_pIUserManagerSink->OnUserItemUpdate(pIUserItem);

	return true;
}

//更新状态
bool __cdecl CClientUserManager::UpdateUserItemStatus(IUserItem * pIUserItem, const tagUserStatus * pUserStatus)
{
	//设置数据
	pIUserItem->SetUserStatus(pUserStatus);

	//通知更新
	ASSERT(m_pIUserManagerSink!=NULL);
	m_pIUserManagerSink->OnUserItemUpdate(pIUserItem);

	return true;
}

//枚举用户
IUserItem * __cdecl CClientUserManager::EnumUserItem(WORD wEnumIndex)
{
	if (wEnumIndex<m_UserItemActive.GetCount())
	{
		CUserItem * pUserItem=m_UserItemActive[wEnumIndex];
		ASSERT(pUserItem!=NULL);
		ASSERT(pUserItem->m_bActive==true);
		return pUserItem;
	}
	return NULL;
}

//查找用户
IUserItem * __cdecl CClientUserManager::SearchUserByUserID(DWORD dwUserID)
{
	CUserItem * pUserItem=NULL;
	for (INT_PTR i=0;i<m_UserItemActive.GetCount();i++)
	{
		pUserItem=m_UserItemActive[i];
		ASSERT(pUserItem!=NULL);
		ASSERT(pUserItem->m_bActive==true);
		if (pUserItem->m_UserData.dwUserID==dwUserID) return pUserItem;
	}
	return NULL;
}

//查找用户
IUserItem * __cdecl CClientUserManager::SearchUserByGameID(DWORD dwGameID)
{
	CUserItem * pUserItem=NULL;
	for (INT_PTR i=0;i<m_UserItemActive.GetCount();i++)
	{
		pUserItem=m_UserItemActive[i];
		ASSERT(pUserItem!=NULL);
		ASSERT(pUserItem->m_bActive==true);
		if (pUserItem->m_UserData.dwGameID==dwGameID) return pUserItem;
	}
	return NULL;
}

//设置回调接口
bool CClientUserManager::SetUserManagerSink(IUnknownEx * pIUnknownEx)
{
	ASSERT(pIUnknownEx!=NULL);
	m_pIUserManagerSink=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IUserManagerSink);
	ASSERT(m_pIUserManagerSink!=NULL);
	return (m_pIUserManagerSink!=NULL);
}

//////////////////////////////////////////////////////////////////////////
