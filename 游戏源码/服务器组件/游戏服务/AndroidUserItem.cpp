#include "StdAfx.h"
#include "AndroidUserItem.h"

//////////////////////////////////////////////////////////////////////////

//静态变量
CTimerItemArray CAndroidUserItem::m_TimerItemStorage;					//库存数组

//////////////////////////////////////////////////////////////////////////

//构造函数
CAndroidUserItem::CAndroidUserItem()
{
	//状态变量
	m_bStartClient=false;
	m_cbGameStatus=GS_FREE;

	//用户状态
	m_CurrentUserStatus.cbUserStatus=US_NULL;
	m_CurrentUserStatus.wChairID=INVALID_CHAIR;
	m_CurrentUserStatus.wTableID=INVALID_TABLE;

	//绑定变量
	m_dwUserID=0L;
	m_pIServerUserItem=NULL;

	//索引变量
	m_wRountID=1;
	m_wAndroidIndex=INVALID_WORD_INDEX;

	//接口变量
	m_pIServerUserManager=NULL;
	m_pIAndroidUserManager=NULL;
	m_pIAndroidUserItemSink=NULL;

	return;
}

//析构函数
CAndroidUserItem::~CAndroidUserItem()
{
	//删除时间
	KillGameTimer(0);

	//释放对象
	if (m_pIAndroidUserItemSink!=NULL)
	{
		m_pIAndroidUserItemSink->Release();
		m_pIAndroidUserItemSink=NULL;
	}

	return;
}

//接口查询
VOID * __cdecl CAndroidUserItem::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAndroidUserItem,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAndroidUserItem,Guid,dwQueryVer);
	return NULL;
}

//获取 I D
DWORD __cdecl CAndroidUserItem::GetUserID()
{
	return m_dwUserID;
}

//桌子号码
WORD __cdecl CAndroidUserItem::GetTableID()
{
	//效验状态
	ASSERT(m_pIServerUserItem!=NULL);
	if (m_pIServerUserItem==NULL) return INVALID_TABLE;

	return m_pIServerUserItem->GetTableID();
}

//椅子号码
WORD __cdecl CAndroidUserItem::GetChairID()
{
	//效验状态
	ASSERT(m_pIServerUserItem!=NULL);
	if (m_pIServerUserItem==NULL) return INVALID_CHAIR;

	return m_pIServerUserItem->GetChairID();
}

//获取自己
IServerUserItem * __cdecl CAndroidUserItem::GetMeUserItem()
{
	return m_pIServerUserItem;
}

//游戏用户
IServerUserItem * __cdecl CAndroidUserItem::GetTableUserItem(WORD wChariID)
{
	return NULL;
}

//发送函数
bool __cdecl CAndroidUserItem::SendSocketData(WORD wSubCmdID)
{
	//状态效验
	ASSERT(m_pIServerUserItem!=NULL);
	if (m_pIServerUserItem==NULL) return false;

	//发送数据
	DWORD dwAndroidID=MAKELONG(m_wAndroidIndex,m_wRountID);
	m_pIAndroidUserManager->SendDataToServer(dwAndroidID,MDM_GF_GAME,wSubCmdID,NULL,0);

	return true;
}

//发送函数
bool __cdecl CAndroidUserItem::SendSocketData(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//状态效验
	ASSERT(m_pIServerUserItem!=NULL);
	if (m_pIServerUserItem==NULL) return false;

	//发送数据
	DWORD dwAndroidID=MAKELONG(m_wAndroidIndex,m_wRountID);
	m_pIAndroidUserManager->SendDataToServer(dwAndroidID,MDM_GF_GAME,wSubCmdID,pData,wDataSize);

	return true;
}

//删除时间
bool __cdecl CAndroidUserItem::KillGameTimer(UINT nTimerID)
{
	//删除时间
	if (nTimerID!=0)
	{
		//寻找子项
		for (INT_PTR i=0;i<m_TimerItemActive.GetCount();i++)
		{
			//获取时间
			tagTimerItem * pTimerItem=m_TimerItemActive[i];

			//删除判断
			if (pTimerItem->nTimerID==nTimerID)
			{
				m_TimerItemActive.RemoveAt(i);
				m_TimerItemStorage.Add(pTimerItem);

				return true;
			}
		}
	}
	else
	{
		m_TimerItemStorage.Append(m_TimerItemActive);
		m_TimerItemActive.RemoveAll();
	}

	return false;
}

//设置时间
bool __cdecl CAndroidUserItem::SetGameTimer(UINT nTimerID, UINT nElapse)
{
	//寻找子项
	for (INT_PTR i=0;i<m_TimerItemActive.GetCount();i++)
	{
		//获取时间
		tagTimerItem * pTimerItem=m_TimerItemActive[i];

		//设置判断
		if (pTimerItem->nTimerID==nTimerID)
		{
			pTimerItem->nTimeLeave=nElapse;
			return true;
		}
	}

	//变量定义
	tagTimerItem * pTimerItem=NULL;
	WORD wStorageCount=(WORD)m_TimerItemStorage.GetCount();

	//库存子项
	if (wStorageCount>0)
	{
		//获取对象
		pTimerItem=m_TimerItemStorage[wStorageCount-1];

		//加入数组
		m_TimerItemActive.Add(pTimerItem);
		m_TimerItemStorage.RemoveAt(wStorageCount-1);
	}

	//创建子项
	if (pTimerItem==NULL)
	{
		try
		{
			//创建对象
			pTimerItem=new tagTimerItem;
			if (pTimerItem==NULL) return false;

			//加入数组
			m_TimerItemActive.Add(pTimerItem);
		}
		catch (...)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	//设置变量
	pTimerItem->nTimerID=nTimerID;
	pTimerItem->nTimeLeave=nElapse;

	return true;
}

//发送准备
bool __cdecl CAndroidUserItem::SendUserReady(VOID * pData, WORD wDataSize)
{
	//状态效验
	ASSERT((m_pIServerUserItem!=NULL)&&(m_pIServerUserItem->GetUserStatus()==US_SIT));
	if ((m_pIServerUserItem==NULL)||(m_pIServerUserItem->GetUserStatus()!=US_SIT)) return false;

	//发送准备
	DWORD dwAndroidID=MAKELONG(m_wAndroidIndex,m_wRountID);
	m_pIAndroidUserManager->SendDataToServer(dwAndroidID,MDM_GF_FRAME,SUB_GF_USER_READY,pData,wDataSize);

	return true;
}

//发送聊天
bool __cdecl CAndroidUserItem::SendChatMessage(DWORD dwTargetUserID, LPCTSTR pszChatMessage, COLORREF crFontColor)
{
	//效验用户
	ASSERT(m_pIServerUserItem!=NULL);
	if (m_pIServerUserItem==NULL) return false;

	//构造信息
	CMD_GF_UserChat UserChat;
	lstrcpyn(UserChat.szChatMessage,pszChatMessage,CountArray(UserChat.szChatMessage));

	//构造数据
	UserChat.crFontColor=crFontColor;
	UserChat.dwTargetUserID=dwTargetUserID;
	UserChat.dwSendUserID=m_pIServerUserItem->GetUserID();
	UserChat.wChatLength=CountStringBuffer(UserChat.szChatMessage);

	//发送数据
	DWORD dwAndroidID=MAKELONG(m_wAndroidIndex,m_wRountID);
	WORD wSendSize=sizeof(UserChat)-sizeof(UserChat.szChatMessage)+UserChat.wChatLength*sizeof(WCHAR);
	m_pIAndroidUserManager->SendDataToServer(dwAndroidID,MDM_GF_FRAME,SUB_GF_USER_CHAT,&UserChat,wSendSize);

	return true;
}

//时间事件
bool CAndroidUserItem::OnTimerPulse(DWORD dwTimerID, WPARAM dwBindParameter)
{
	//寻找子项
	for (INT_PTR i=0;i<m_TimerItemActive.GetCount();)
	{
		//变量定义
		tagTimerItem * pTimerItem=m_TimerItemActive[i];

		//时间处理
		if (pTimerItem->nTimeLeave<=1L)
		{
			//设置数组
			m_TimerItemActive.RemoveAt(i);
			m_TimerItemStorage.Add(pTimerItem);

			//时间通知
			ASSERT(m_pIAndroidUserItemSink!=NULL);
			if (m_pIAndroidUserItemSink!=NULL) m_pIAndroidUserItemSink->OnEventTimer(pTimerItem->nTimerID);
		}
		else
		{
			//设置变量
			i++;
			pTimerItem->nTimeLeave--;
		}
	}


	return true;
}

//网络消息
bool CAndroidUserItem::OnSocketRead(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//登录失败
	if ((wMainCmdID==MDM_GR_LOGON)&&(wSubCmdID==SUB_GR_LOGON_ERROR))
	{
		return OnSocketSubLogonFailed(pData,wDataSize);
	}

	//登录完成
	if ((wMainCmdID==MDM_GR_LOGON)&&(wSubCmdID==SUB_GR_LOGON_FINISH))
	{
		return OnSocketSubLogonFinish(pData,wDataSize);
	}

	//用户状态
	if ((wMainCmdID==MDM_GR_USER)&&(wSubCmdID==SUB_GR_USER_STATUS))
	{
		return OnSocketSubUserStatus(pData,wDataSize);
	}

	//游戏消息
	if ((wMainCmdID==MDM_GF_GAME)||(wMainCmdID==MDM_GF_FRAME))
	{
		return OnSocketGameMessage(wMainCmdID,wSubCmdID,pData,wDataSize);
	}

	return true;
}

//游戏消息
bool CAndroidUserItem::OnSocketGameMessage(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//游戏处理
	if (wMainCmdID==MDM_GF_GAME)
	{
		ASSERT(m_pIAndroidUserItemSink!=NULL);
		return m_pIAndroidUserItemSink->OnEventGameMessage(wSubCmdID,pData,wDataSize);
	}

	//框架处理
	if (wMainCmdID==MDM_GF_FRAME)
	{
		//默认处理
		switch (wSubCmdID)
		{
		case SUB_GF_OPTION:				//游戏状态
			{
				//效验参数
				ASSERT(wDataSize==sizeof(CMD_GF_Option));
				if (wDataSize!=sizeof(CMD_GF_Option)) return false;

				//消息处理
				CMD_GF_Option * pOption=(CMD_GF_Option *)pData;

				//设置变量
				m_cbGameStatus=pOption->bGameStatus;

				return true;
			}
		case SUB_GF_SCENE:				//游戏场景
			{
				//游戏处理
				bool bLookonUser=(m_pIServerUserItem->GetUserStatus()==US_LOOKON);
				return m_pIAndroidUserItemSink->OnEventGameScene(m_cbGameStatus,bLookonUser,pData,wDataSize);
			}
		}
	}

	return true;
}

//登录失败
bool CAndroidUserItem::OnSocketSubLogonFailed(VOID * pData, WORD wDataSize)
{
	//关闭处理
	DWORD dwAndroidID=MAKELONG(m_wAndroidIndex,m_wRountID);
	m_pIAndroidUserManager->DeleteAndroidUserItem(dwAndroidID);

	return true;
}

//登录完成
bool CAndroidUserItem::OnSocketSubLogonFinish(VOID * pData, WORD wDataSize)
{
	//设置变量
	m_cbGameStatus=GS_FREE;
	m_pIServerUserItem=m_pIServerUserManager->SearchOnLineUser(m_dwUserID);

	//状态效验
	ASSERT(m_pIServerUserItem!=NULL);
	if (m_pIServerUserItem==NULL) return false;

	//用户状态
	m_CurrentUserStatus.wChairID=m_pIServerUserItem->GetChairID();
	m_CurrentUserStatus.wTableID=m_pIServerUserItem->GetTableID();
	m_CurrentUserStatus.cbUserStatus=m_pIServerUserItem->GetUserStatus();

	//规则变量
	//CMD_GR_UserRule UserRule;
	//ZeroMemory(&UserRule,sizeof(UserRule));

	//重入判断
	if (m_pIServerUserItem->GetTableID()!=INVALID_TABLE)
	{
		StartGameClient();
	}

	return true;
}

//用户状态
bool CAndroidUserItem::OnSocketSubUserStatus(VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GR_UserStatus));
	if (wDataSize<sizeof(CMD_GR_UserStatus)) return false;

	//变量定义
	CMD_GR_UserStatus * pUserStatus=(CMD_GR_UserStatus *)pData;

	//玩家判断
	if (pUserStatus->dwUserID!=m_dwUserID) return true;

	//用户状态
	tagUserStatus LastUserStatus;
	CopyMemory(&LastUserStatus,&m_CurrentUserStatus,sizeof(LastUserStatus));

	//设置状态
	m_CurrentUserStatus.wChairID=pUserStatus->wChairID;
	m_CurrentUserStatus.wTableID=pUserStatus->wTableID;
	m_CurrentUserStatus.cbUserStatus=pUserStatus->cbUserStatus;

	//开始切换
	if ((LastUserStatus.cbUserStatus!=US_READY)&&(m_CurrentUserStatus.cbUserStatus==US_READY))
	{
	}

	//关闭判断
	if ((m_bStartClient==true)&&(m_CurrentUserStatus.wTableID==INVALID_TABLE))
	{
		//关闭游戏
		CloseGameClient();

		return true;
	}

	//启动判断
	if ((m_bStartClient==false)&&(m_CurrentUserStatus.wTableID!=INVALID_TABLE))
	{
		//启动游戏
		StartGameClient();

		return true;
	}

	return true;
}

//启动游戏
VOID CAndroidUserItem::StartGameClient()
{
	//状态判断
	ASSERT((m_bStartClient==false)&&(m_pIServerUserItem!=NULL));
	ASSERT((m_pIServerUserItem->GetTableID()!=INVALID_TABLE)&&(m_pIServerUserItem->GetChairID()!=INVALID_CHAIR));

	//设置变量
	m_bStartClient=true;

	//变量定义
	CMD_GF_Info Info;
	ZeroMemory(&Info,sizeof(Info));

	//构造数据
	Info.bAllowLookon=FALSE;

	//发送数据
	DWORD dwAndroidID=MAKELONG(m_wAndroidIndex,m_wRountID);
	m_pIAndroidUserManager->SendDataToServer(dwAndroidID,MDM_GF_FRAME,SUB_GF_INFO,&Info,sizeof(Info));

	return;
}

//关闭游戏
VOID CAndroidUserItem::CloseGameClient()
{
	//状态判断
	ASSERT((m_bStartClient==true)&&(m_pIServerUserItem!=NULL));
	ASSERT((m_pIServerUserItem->GetTableID()==INVALID_TABLE)&&(m_pIServerUserItem->GetChairID()==INVALID_CHAIR));

	//删除时间
	KillGameTimer(0);

	//设置变量
	m_bStartClient=false;
	m_cbGameStatus=GS_FREE;

	//游戏复位
	if (m_pIAndroidUserItemSink!=NULL) m_pIAndroidUserItemSink->RepositUserItemSink();

	return;
}

//复位数据
VOID CAndroidUserItem::RepositUserItem()
{
	//状态变量
	m_bStartClient=false;
	m_cbGameStatus=GS_FREE;

	//用户状态
	m_CurrentUserStatus.cbUserStatus=US_NULL;
	m_CurrentUserStatus.wChairID=INVALID_CHAIR;
	m_CurrentUserStatus.wTableID=INVALID_TABLE;

	//绑定变量
	m_dwUserID=0L;
	m_pIServerUserItem=NULL;

	//删除时间
	m_TimerItemStorage.Append(m_TimerItemActive);
	m_TimerItemActive.RemoveAll();

	//索引变量
	m_wRountID=__max(1,m_wRountID+1);

	//复位游戏
	if (m_pIAndroidUserItemSink!=NULL) m_pIAndroidUserItemSink->RepositUserItemSink();

	return;
}

//////////////////////////////////////////////////////////////////////////
