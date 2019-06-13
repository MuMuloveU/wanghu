#include "StdAfx.h"
#include "AndroidUserManager.h"

//////////////////////////////////////////////////////////////////////////

//常量定义
#define ANDROID_LOGON_COUNT			128									//登录数目

//////////////////////////////////////////////////////////////////////////

//构造函数
CAndroidUserManager::CAndroidUserManager()
{
	//配置变量
	m_wAutoAndroidCount=ANDROID_LOGON_COUNT;

	//组件接口
	m_pITimerEngine=NULL;
	m_pIServerUserManager=NULL;
	m_pIGameServiceManager=NULL;
	m_pITCPNetworkEngineEvent=NULL;

	//系统参数
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	//库存变量
	m_wStockCount=0;
	ZeroMemory(m_dwAndroidStock,sizeof(m_dwAndroidStock));

	//设置索引
	m_AndroidUserItemMap.InitHashTable(503L);

	return;
}

//析构函数
CAndroidUserManager::~CAndroidUserManager()
{
	//清理断言
	ASSERT(m_AndroidUserItemMap.GetCount()==0L);
	ASSERT(m_AndroidUserItemFree.GetCount()==0L);
	ASSERT(m_AndroidUserItemStorage.GetCount()==0L);

	return;
}

//接口查询
VOID * __cdecl CAndroidUserManager::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAndroidUserManager,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAndroidUserManager,Guid,dwQueryVer);
	return NULL;
}

//启动服务
bool __cdecl CAndroidUserManager::StartService()
{
	//启动时间
	m_pITimerEngine->SetTimer(IDI_ANDROID_INOUT,TIME_ANDROID_INOUT*1000L,TIMES_INFINITY,0);
	m_pITimerEngine->SetTimer(IDI_ANDROID_USER_PULSE,TIME_ANDROID_USER_PULSE*1000L,TIMES_INFINITY,0);

	return true;
}

//停止服务
bool __cdecl CAndroidUserManager::StopService()
{
	//删除存储
	for (INT_PTR i=0;i<m_AndroidUserItemStorage.GetCount();i++)
	{
		CAndroidUserItem * pAndroidUserItem=m_AndroidUserItemStorage[i];
		SafeDelete(pAndroidUserItem);
	}

	//清理数组
	m_AndroidUserItemMap.RemoveAll();
	m_AndroidUserItemFree.RemoveAll();
	m_AndroidUserItemStorage.RemoveAll();

	//库存用户
	m_wStockCount=0;
	ZeroMemory(m_dwAndroidStock,sizeof(m_dwAndroidStock));

	return true;
}

//设置库存
bool __cdecl CAndroidUserManager::SetAndroidStock(DWORD dwAndroidStock[], WORD wStockCount)
{
	//清理变量
	m_wStockCount=0;
	ZeroMemory(m_dwAndroidStock,sizeof(m_dwAndroidStock));

	//设置用户
	for (WORD i=0;i<wStockCount;i++)
	{
		//数目效验
		ASSERT(m_wStockCount<CountArray(m_dwAndroidStock));
		if (m_wStockCount>=CountArray(m_dwAndroidStock)) break;

		//设置变量
		CAndroidUserItem * pAndroidUserItem=NULL;
		m_AndroidUserItemMap.Lookup(dwAndroidStock[i],pAndroidUserItem);

		//设置变量
		if (pAndroidUserItem==NULL)
		{
			DWORD dwUserID=dwAndroidStock[i];
			m_dwAndroidStock[m_wStockCount++]=dwUserID;
		}
	}

	return true;
}

//删除机器人
bool __cdecl CAndroidUserManager::DeleteAndroidUserItem(DWORD dwAndroidID)
{
	//获取对象
	WORD wIndex=LOWORD(dwAndroidID);
	CAndroidUserItem * pAndroidUserItem=GetAndroidUserItem(wIndex);

	//对象效验
	ASSERT((pAndroidUserItem!=NULL)&&(pAndroidUserItem->m_wRountID==HIWORD(dwAndroidID)));
	if ((pAndroidUserItem==NULL)||(pAndroidUserItem->m_wRountID!=HIWORD(dwAndroidID))) return false;

	//关闭事件
	//bool bSuccess=false;
	try
	{
		m_pITCPNetworkEngineEvent->OnEventTCPNetworkShut(dwAndroidID,0,0L);
	}
	catch (...)
	{
		//错误断言
		ASSERT(FALSE);
	}

	//删除对象
	FreeAndroidUserItem(pAndroidUserItem);

	return true;
}

//切换机器人
IAndroidUserItem * __cdecl CAndroidUserManager::SwitchAndroidUserItem(IServerUserItem * pIServerUserItem)
{
	return NULL;
}

//创建机器人
IAndroidUserItem * __cdecl CAndroidUserManager::CreateAndroidUserItem(DWORD dwUserID, TCHAR szPassword[PASS_LEN])
{
	//创建对象
	CAndroidUserItem * pAndroidUserItem=ActiveAndroidUserItem(dwUserID);
	if (pAndroidUserItem==NULL) return NULL;

	//属性变量
	WORD wRoundID=pAndroidUserItem->m_wRountID;
	WORD wAndroidIndex=pAndroidUserItem->m_wAndroidIndex;

	//连接模拟
	try
	{
		if (m_pITCPNetworkEngineEvent->OnEventTCPNetworkBind(MAKELONG(wAndroidIndex,wRoundID),0L)==false)
		{
			throw 0;
		}
	}
	catch (...)
	{
		ASSERT(FALSE);
		FreeAndroidUserItem(pAndroidUserItem);
		return NULL;
	}

	//变量定义
	CMD_GR_LogonByUserID LogonByUserID;
	ZeroMemory(&LogonByUserID,sizeof(LogonByUserID));

	//设置数据
	LogonByUserID.dwUserID=dwUserID;
	lstrcpyn(LogonByUserID.szPassWord,szPassword,CountArray(LogonByUserID.szPassWord));

	//构造数据
	CMD_Command Command;
	Command.wMainCmdID=MDM_GR_LOGON;
	Command.wSubCmdID=SUB_GR_LOGON_USERID;

	//消息处理
	try
	{
		//发送数据
		if (m_pITCPNetworkEngineEvent->OnEventTCPNetworkRead(MAKELONG(wAndroidIndex,wRoundID),Command,&LogonByUserID,sizeof(LogonByUserID))==false)
		{
			throw 0;
		}
	}
	catch (...) 
	{
		ASSERT(FALSE);
		DeleteAndroidUserItem(MAKELONG(wAndroidIndex,wRoundID));
		return NULL;
	}

	return pAndroidUserItem;
}

//脉冲事件
bool __cdecl CAndroidUserManager::OnEventTimerPulse(DWORD dwTimerID, WPARAM dwBindParameter)
{
	switch (dwTimerID)
	{
	case IDI_ANDROID_INOUT:			//进出处理
		{
			//变量定义
			DWORD dwUserID=0L;
			INT_PTR nActiveCount=m_AndroidUserItemMap.GetCount();
			POSITION Position=m_AndroidUserItemMap.GetStartPosition();

			//登录处理
			if ((m_wStockCount>0)&&(nActiveCount<m_wAutoAndroidCount))
			{
				//获取用户
				WORD wIndex=rand()%m_wStockCount;
				DWORD dwUserID=m_dwAndroidStock[wIndex];

				//删除存存
				m_wStockCount--;
				m_dwAndroidStock[wIndex]=m_dwAndroidStock[m_wStockCount];

				//创建机器人
				CreateAndroidUserItem(dwUserID,NULL);

				return true;
			}

			//退出处理
			while (Position!=NULL)
			{
				//获取对象
				CAndroidUserItem * pAndroidUserItem=NULL;
				m_AndroidUserItemMap.GetNextAssoc(Position,dwUserID,pAndroidUserItem);

				//退出判断
				if (pAndroidUserItem->m_pIServerUserItem!=NULL)
				{
					//获取属性
					BYTE cbUserStatus=pAndroidUserItem->m_pIServerUserItem->GetUserStatus();
					DWORD dwOnlineTimeCount=pAndroidUserItem->m_pIServerUserItem->GetOnlineTimeCount();

					//状态判断
					if (dwOnlineTimeCount<=TIME_ANDROID_SERVICE) continue;
					if ((cbUserStatus!=US_FREE)&&(cbUserStatus!=US_SIT)) continue;

					//删除用户
					WORD wRoundID=pAndroidUserItem->m_wRountID;
					WORD wAndroidIndex=pAndroidUserItem->m_wAndroidIndex;
					DeleteAndroidUserItem(MAKELONG(wAndroidIndex,wRoundID));

					return true;
				}
			}

			return true;
		}
	case IDI_ANDROID_USER_PULSE:	//用户脉冲
		{
			//变量定义
			DWORD dwUserID=0L;
			CAndroidUserItem * pAndroidUserItem=NULL;
			POSITION Position=m_AndroidUserItemMap.GetStartPosition();

			//用户处理
			while (Position!=NULL)
			{
				//获取对象
				m_AndroidUserItemMap.GetNextAssoc(Position,dwUserID,pAndroidUserItem);

				//时间处理
				try
				{
					ASSERT(pAndroidUserItem!=NULL);
					if (pAndroidUserItem->m_pIServerUserItem!=NULL) pAndroidUserItem->OnTimerPulse(dwTimerID,dwBindParameter);
				}
				catch (...)
				{
					ASSERT(FALSE);
					DeleteAndroidUserItem(MAKELONG(pAndroidUserItem->m_wAndroidIndex,pAndroidUserItem->m_wRountID));
				}
			}

			return true;
		}
	}

	return false;
}

//发送数据
bool __cdecl CAndroidUserManager::SendDataToClient(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//发送数据
	for (INT_PTR i=0;i<m_AndroidUserItemStorage.GetCount();i++)
	{
		//获取对象
		CAndroidUserItem * pAndroidUserItem=m_AndroidUserItemStorage[i];

		//状态判断
		if (pAndroidUserItem->m_dwUserID==0L) continue;
		if (pAndroidUserItem->m_pIServerUserItem==NULL) continue;

		//消息处理
		try
		{
			if (pAndroidUserItem->OnSocketRead(wMainCmdID,wSubCmdID,pData,wDataSize)==false)
			{
				throw 0;
			}
		}
		catch (...) 
		{
			//错误断言
			ASSERT(FALSE);

			//断开用户
			WORD wRoundID=pAndroidUserItem->m_wRountID;
			WORD wAndroidIndex=pAndroidUserItem->m_wAndroidIndex;
			DeleteAndroidUserItem(MAKELONG(wAndroidIndex,wRoundID));
		}
	}

	return true;
}

//发送数据
bool __cdecl CAndroidUserManager::SendDataToClient(DWORD dwAndroidID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//获取对象
	WORD wIndex=LOWORD(dwAndroidID);
	CAndroidUserItem * pAndroidUserItem=GetAndroidUserItem(wIndex);

	//对象效验
	ASSERT((pAndroidUserItem!=NULL)&&(pAndroidUserItem->m_wRountID==HIWORD(dwAndroidID)));
	if ((pAndroidUserItem==NULL)||(pAndroidUserItem->m_wRountID!=HIWORD(dwAndroidID))) return false;

	//消息处理
	try
	{
		if (pAndroidUserItem->OnSocketRead(wMainCmdID,wSubCmdID,pData,wDataSize)==false)
		{
			throw 0;
		}
	}
	catch (...) 
	{
		ASSERT(FALSE);
		DeleteAndroidUserItem(dwAndroidID);
	}

	return true;
}

//发送数据
bool __cdecl CAndroidUserManager::SendDataToServer(DWORD dwAndroidID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//构造数据
	CMD_Command Command;
	Command.wSubCmdID=wSubCmdID;
	Command.wMainCmdID=wMainCmdID;

	//消息处理
	try
	{
		if (m_pITCPNetworkEngineEvent->OnEventTCPNetworkRead(dwAndroidID,Command,pData,wDataSize)==false)
		{
			throw 0;
		}
	}
	catch (...) 
	{
		ASSERT(FALSE);
		DeleteAndroidUserItem(dwAndroidID);
	}

	return true;
}

//获取对象
CAndroidUserItem * CAndroidUserManager::GetAndroidUserItem(WORD wIndex)
{
	//效验索引
	ASSERT(wIndex>=INDEX_ANDROID);
	if (wIndex<INDEX_ANDROID) return NULL;

	//效验索引
	ASSERT((wIndex-INDEX_ANDROID)<m_AndroidUserItemStorage.GetCount());
	if (((wIndex-INDEX_ANDROID)>=m_AndroidUserItemStorage.GetCount())) return NULL;

	//获取对象
	WORD wStorageIndex=wIndex-INDEX_ANDROID;
	CAndroidUserItem * pAndroidUserItem=m_AndroidUserItemStorage[wStorageIndex];

	return pAndroidUserItem;
}

//激活对象
CAndroidUserItem * CAndroidUserManager::ActiveAndroidUserItem(DWORD dwUserID)
{
	//变量定义
	CAndroidUserItem * pAndroidUserItem=NULL;
	INT_PTR nFreeItemCount=m_AndroidUserItemFree.GetCount();

	//获取对象
	if (nFreeItemCount>0)
	{
		//获取对象
		INT_PTR nItemPostion=nFreeItemCount-1;
		pAndroidUserItem=m_AndroidUserItemFree[nItemPostion];

		//删除数组
		m_AndroidUserItemFree.RemoveAt(nItemPostion);
	}

	//创建对象
	if (pAndroidUserItem==NULL)
	{
		//数目判断
		if (m_AndroidUserItemStorage.GetCount()>=MAX_ANDROID) return NULL;

		try
		{
			//创建对象
			pAndroidUserItem=new CAndroidUserItem;
			if (pAndroidUserItem==NULL)
			{
				ASSERT(FALSE);
				return NULL;
			}

			//设置数组
			m_AndroidUserItemStorage.Add(pAndroidUserItem);

			//变量定义
			WORD wCurrentIndex=(WORD)m_AndroidUserItemStorage.GetCount()-1;
			IAndroidUserItemSink * pIAndroidUserItemSink=(IAndroidUserItemSink *)
				m_pIGameServiceManager->CreateAndroidUserItemSink(IID_IAndroidUserItemSink,VER_IAndroidUserItemSink);

			//设置用户
			if ((pIAndroidUserItemSink!=NULL)&&(pIAndroidUserItemSink->InitUserItemSink(pAndroidUserItem)==false))
			{
				pIAndroidUserItemSink->Release();
				pIAndroidUserItemSink=NULL;
			}

			//设置接口
			pAndroidUserItem->m_wAndroidIndex=wCurrentIndex+INDEX_ANDROID;
			pAndroidUserItem->m_pIServerUserManager=m_pIServerUserManager;
			pAndroidUserItem->m_pIAndroidUserItemSink=pIAndroidUserItemSink;
			pAndroidUserItem->m_pIAndroidUserManager=QUERY_ME_INTERFACE(IAndroidUserManager);
		}
		catch (...) 
		{ 
			ASSERT(FALSE);
			return NULL; 
		}
	}

	//设置变量
	pAndroidUserItem->m_dwUserID=dwUserID;

	//设置索引
	m_AndroidUserItemMap[dwUserID]=pAndroidUserItem;

	return pAndroidUserItem;
}

//释放对象
bool CAndroidUserManager::FreeAndroidUserItem(CAndroidUserItem * pAndroidUserItem)
{
	//效验参数
	ASSERT(pAndroidUserItem!=NULL);
	if (pAndroidUserItem==NULL) return false;

	//变量定义
	DWORD dwUserID=pAndroidUserItem->m_dwUserID;
	CAndroidUserItem * pAndroidUserItemMap=NULL;
	m_AndroidUserItemMap.Lookup(dwUserID,pAndroidUserItemMap);

	//对象判断
	if (pAndroidUserItemMap==pAndroidUserItem)
	{
		//复位对象
		pAndroidUserItem->RepositUserItem();

		//设置索引
		m_AndroidUserItemMap.RemoveKey(dwUserID);
		m_AndroidUserItemFree.Add(pAndroidUserItem);

		return true;
	}

	//释放失败
	ASSERT(FALSE);

	return false;
}

//////////////////////////////////////////////////////////////////////////
