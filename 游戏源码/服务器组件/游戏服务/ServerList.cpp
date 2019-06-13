#include "StdAfx.h"
#include "ServerList.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CServerListInfo::CServerListInfo()
{
}

//析构函数
CServerListInfo::~CServerListInfo()
{
}

//重置信息
bool CServerListInfo::ResetInfo()
{
	m_GameTypePtrArrayBuffer.Append(m_GameTypePtrArrayActive);
	m_GameTypePtrArrayActive.RemoveAll();
	m_GameKindPtrArrayBuffer.Append(m_GameKindPtrArrayActive);
	m_GameKindPtrArrayActive.RemoveAll();
	m_GameStationPtrArrayBuffer.Append(m_GameStationPtrArrayActive);
	m_GameStationPtrArrayActive.RemoveAll();
	m_GameServerPtrArrayBuffer.Append(m_GameServerPtrArrayActive);
	m_GameServerPtrArrayActive.RemoveAll();

	return true;
}

//插入类型
bool CServerListInfo::InsertGameType(tagGameType * pGameType)
{
	//生成数据
	tagGameType * pGameTypeInsert=NULL;
	INT_PTR nBufferCount=m_GameTypePtrArrayBuffer.GetCount();
	if (nBufferCount>0)
	{
		pGameTypeInsert=m_GameTypePtrArrayBuffer[nBufferCount-1];
		m_GameTypePtrArrayBuffer.RemoveAt(nBufferCount-1);
	}
	else
	{
		pGameTypeInsert=new tagGameType;
		if (pGameTypeInsert==NULL) return false;
	}
	ASSERT(pGameTypeInsert!=NULL);
	CopyMemory(pGameTypeInsert,pGameType,sizeof(tagGameType));

	//插入数据
	bool bInsert=false;
	tagGameType * pGameTypeTemp=NULL;
	for (INT_PTR i=0;i<m_GameTypePtrArrayActive.GetCount();i++)
	{
		pGameTypeTemp=m_GameTypePtrArrayActive[i];
		ASSERT(pGameTypeTemp!=NULL);
		if (pGameTypeInsert->wSortID<=pGameTypeTemp->wSortID)
		{
			bInsert=true;
			m_GameTypePtrArrayActive.InsertAt(i,pGameTypeInsert);
			break;
		}
	}
 	if (bInsert==false) m_GameTypePtrArrayActive.Add(pGameTypeInsert);

	return true;
}

//插入种类
bool CServerListInfo::InsertGameKind(tagGameKind * pGameKind)
{
	//生成数据
	tagGameKind * pGameKindInsert=NULL;
	INT_PTR nBufferCount=m_GameKindPtrArrayBuffer.GetCount();
	if (nBufferCount>0)
	{
		pGameKindInsert=m_GameKindPtrArrayBuffer[nBufferCount-1];
		m_GameKindPtrArrayBuffer.RemoveAt(nBufferCount-1);
	}
	else
	{
		pGameKindInsert=new tagGameKind;
		if (pGameKindInsert==NULL) return false;
	}
	ASSERT(pGameKindInsert!=NULL);
	CopyMemory(pGameKindInsert,pGameKind,sizeof(tagGameKind));

	//插入数据
	bool bInsert=false;
	tagGameKind * pGameKindTemp=NULL;
	for (INT_PTR i=0;i<m_GameKindPtrArrayActive.GetCount();i++)
	{
		pGameKindTemp=m_GameKindPtrArrayActive[i];
		ASSERT(pGameKindTemp!=NULL);
		if (pGameKindInsert->wSortID<=pGameKindTemp->wSortID)
		{
			bInsert=true;
			m_GameKindPtrArrayActive.InsertAt(i,pGameKindInsert);
			break;
		}
	}
	if (bInsert==false) m_GameKindPtrArrayActive.Add(pGameKindInsert);

	return true;
}

//插入站点
bool CServerListInfo::InsertGameStation(tagGameStation * pGameStation)
{
	//生成数据
	tagGameStation * pGameStationInsert=NULL;
	INT_PTR nBufferCount=m_GameStationPtrArrayBuffer.GetCount();
	if (nBufferCount>0)
	{
		pGameStationInsert=m_GameStationPtrArrayBuffer[nBufferCount-1];
		m_GameStationPtrArrayBuffer.RemoveAt(nBufferCount-1);
	}
	else
	{
		pGameStationInsert=new tagGameStation;
		if (pGameStationInsert==NULL) return false;
	}
	ASSERT(pGameStationInsert!=NULL);
	CopyMemory(pGameStationInsert,pGameStation,sizeof(tagGameStation));

	//插入数据
	bool bInsert=false;
	tagGameStation * pGameStationTemp=NULL;
	for (INT_PTR i=0;i<m_GameStationPtrArrayActive.GetCount();i++)
	{
		pGameStationTemp=m_GameStationPtrArrayActive[i];
		ASSERT(pGameStationTemp!=NULL);
		if ((pGameStationInsert->wJoinID<=pGameStationTemp->wJoinID)
			&&(pGameStationInsert->wSortID<=pGameStationTemp->wSortID))
		{
			bInsert=true;
			m_GameStationPtrArrayActive.InsertAt(i,pGameStationInsert);
			break;
		}
	}
	if (bInsert==false) m_GameStationPtrArrayActive.Add(pGameStationInsert);

	return true;
}

//插入房间
bool CServerListInfo::InsertGameServer(tagGameServer * pGameServer)
{
	//生成数据
	tagGameServer * pGameServerInsert=NULL;
	INT_PTR nBufferCount=m_GameServerPtrArrayBuffer.GetCount();
	if (nBufferCount>0)
	{
		pGameServerInsert=m_GameServerPtrArrayBuffer[nBufferCount-1];
		m_GameServerPtrArrayBuffer.RemoveAt(nBufferCount-1);
	}
	else
	{
		pGameServerInsert=new tagGameServer;
		if (pGameServerInsert==NULL) return false;
	}
	ASSERT(pGameServerInsert!=NULL);
	CopyMemory(pGameServerInsert,pGameServer,sizeof(tagGameServer));

	//插入数据
	bool bInsert=false;
	tagGameServer * pGameServerTemp=NULL;
	for (INT_PTR i=0;i<m_GameServerPtrArrayActive.GetCount();i++)
	{
		pGameServerTemp=m_GameServerPtrArrayActive[i];
		ASSERT(pGameServerTemp!=NULL);
		if (pGameServerInsert->wSortID<=pGameServerTemp->wSortID)
		{
			bInsert=true;
			m_GameServerPtrArrayActive.InsertAt(i,pGameServerInsert);
			break;
		}
	}
	if (bInsert==false) m_GameServerPtrArrayActive.Add(pGameServerInsert);

	return true;
}

//拷贝信息
bool CServerListInfo::CopyServerListInfo(CServerListInfo & ServerListInfoScr)
{
	//重置列表
	ResetInfo();

	//拷贝类型
	tagGameType * pGameType=NULL;
	INT_PTR nTypeScrCount=ServerListInfoScr.m_GameTypePtrArrayActive.GetCount();
	if (nTypeScrCount>m_GameTypePtrArrayBuffer.GetCount())
	{
		for (INT_PTR i=m_GameTypePtrArrayBuffer.GetCount();i<nTypeScrCount;i++)
		{
			pGameType=new tagGameType;
			memset(pGameType,0,sizeof(tagGameType));
			m_GameTypePtrArrayBuffer.Add(pGameType);
		}
	}
	for (INT_PTR i=0;i<nTypeScrCount;i++)
	{
		pGameType=m_GameTypePtrArrayBuffer[i];
		CopyMemory(pGameType,ServerListInfoScr.m_GameTypePtrArrayActive[i],sizeof(tagGameType));
		m_GameTypePtrArrayActive.Add(pGameType);
	}
	m_GameTypePtrArrayBuffer.RemoveAt(0,nTypeScrCount);

	//拷贝种类
	tagGameKind * pGameKind=NULL;
	INT_PTR nKindScrCount=ServerListInfoScr.m_GameKindPtrArrayActive.GetCount();
	if (nKindScrCount>m_GameKindPtrArrayBuffer.GetCount())
	{
		for (INT_PTR i=m_GameKindPtrArrayBuffer.GetCount();i<nKindScrCount;i++)
		{
			pGameKind=new tagGameKind;
			memset(pGameKind,0,sizeof(tagGameKind));
			m_GameKindPtrArrayBuffer.Add(pGameKind);
		}
	}
	for (INT_PTR i=0;i<nKindScrCount;i++)
	{
		pGameKind=m_GameKindPtrArrayBuffer[i];
		CopyMemory(pGameKind,ServerListInfoScr.m_GameKindPtrArrayActive[i],sizeof(tagGameKind));
		m_GameKindPtrArrayActive.Add(pGameKind);
	}
	m_GameKindPtrArrayBuffer.RemoveAt(0,nKindScrCount);

	//拷贝站点
	tagGameStation * pGameStation=NULL;
	INT_PTR nStationScrCount=ServerListInfoScr.m_GameStationPtrArrayActive.GetCount();
	if (nStationScrCount>m_GameStationPtrArrayBuffer.GetCount())
	{
		for (INT_PTR i=m_GameStationPtrArrayBuffer.GetCount();i<nStationScrCount;i++)
		{
			pGameStation=new tagGameStation;
			memset(pGameStation,0,sizeof(tagGameStation));
			m_GameStationPtrArrayBuffer.Add(pGameStation);
		}
	}
	for (INT_PTR i=0;i<nStationScrCount;i++)
	{
		pGameStation=m_GameStationPtrArrayBuffer[i];
		CopyMemory(pGameStation,ServerListInfoScr.m_GameStationPtrArrayActive[i],sizeof(tagGameStation));
		m_GameStationPtrArrayActive.Add(pGameStation);
	}
	m_GameStationPtrArrayBuffer.RemoveAt(0,nStationScrCount);

	//拷贝房间
	tagGameServer * pGameServer=NULL;
	INT_PTR nServerScrCount=ServerListInfoScr.m_GameServerPtrArrayActive.GetCount();
	if (nServerScrCount>m_GameServerPtrArrayBuffer.GetCount())
	{
		for (INT_PTR i=m_GameServerPtrArrayBuffer.GetCount();i<nServerScrCount;i++)
		{
			pGameServer=new tagGameServer;
			memset(pGameServer,0,sizeof(tagGameServer));
			m_GameServerPtrArrayBuffer.Add(pGameServer);
		}
	}
	for (INT_PTR i=0;i<nServerScrCount;i++)
	{
		pGameServer=m_GameServerPtrArrayBuffer[i];
		CopyMemory(pGameServer,ServerListInfoScr.m_GameServerPtrArrayActive[i],sizeof(tagGameServer));
		m_GameServerPtrArrayActive.Add(pGameServer);
	}
	m_GameServerPtrArrayBuffer.RemoveAt(0,nServerScrCount);

	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CServerList::CServerList()
{
	m_pITCPNetworkEngine=NULL;
	memset(m_cbSendBuffer,0,sizeof(m_cbSendBuffer));
}

//析构函数
CServerList::~CServerList()
{
}

//设置接口
bool __cdecl CServerList::SetSocketEngine(IUnknownEx * pIUnknownEx)
{
	ASSERT(pIUnknownEx!=NULL);
	m_pITCPNetworkEngine=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITCPNetworkEngine);
	ASSERT(m_pITCPNetworkEngine!=NULL);
	return (m_pITCPNetworkEngine!=NULL);
}

//发送类型
bool __cdecl CServerList::SendGameTypeList(DWORD dwSocketID)
{
	//效验参数
	ASSERT(m_pITCPNetworkEngine!=NULL);

	//构造数据
	WORD wSendSize=0;
	tagGameType * pGameType=NULL;
	DWORD dwTypeCount=(DWORD)m_ServerListInfoActive.m_GameTypePtrArrayActive.GetCount();
	for (DWORD i=0;i<dwTypeCount;i++)
	{
		if ((wSendSize+sizeof(tagGameType))>sizeof(m_cbSendBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_TYPE,m_cbSendBuffer,wSendSize);
			wSendSize=0;
		}
		pGameType=m_ServerListInfoActive.m_GameTypePtrArrayActive[i];
		CopyMemory(m_cbSendBuffer+wSendSize,pGameType,sizeof(tagGameType));
		wSendSize+=sizeof(tagGameType);
	}
	if (wSendSize>0) m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_TYPE,m_cbSendBuffer,wSendSize);

	return true;
}

//发送种类
bool __cdecl CServerList::SendGameKindList(DWORD dwSocketID)
{
	//效验参数
	ASSERT(m_pITCPNetworkEngine!=NULL);

	//构造数据
	WORD wSendSize=0;
	tagGameKind * pGameKind=NULL;
	DWORD dwKindCount=(DWORD)m_ServerListInfoActive.m_GameKindPtrArrayActive.GetCount();
	for (DWORD i=0;i<dwKindCount;i++)
	{
		if ((wSendSize+sizeof(tagGameKind))>sizeof(m_cbSendBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_KIND,m_cbSendBuffer,wSendSize);
			wSendSize=0;
		}
		pGameKind=m_ServerListInfoActive.m_GameKindPtrArrayActive[i];
		CopyMemory(m_cbSendBuffer+wSendSize,pGameKind,sizeof(tagGameKind));
		wSendSize+=sizeof(tagGameKind);
	}
	if (wSendSize>0) m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_KIND,m_cbSendBuffer,wSendSize);

	return true;
}

//发送站点
bool __cdecl CServerList::SendGameStationList(DWORD dwSocketID)
{
	//效验参数
	ASSERT(m_pITCPNetworkEngine!=NULL);

	//构造数据
	WORD wSendSize=0;
	tagGameStation * pGameStation=NULL;
	DWORD dwStationCount=(DWORD)m_ServerListInfoActive.m_GameStationPtrArrayActive.GetCount();
	for (DWORD i=0;i<dwStationCount;i++)
	{
		if ((wSendSize+sizeof(tagGameStation))>sizeof(m_cbSendBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_STATION,m_cbSendBuffer,wSendSize);
			wSendSize=0;
		}
		pGameStation=m_ServerListInfoActive.m_GameStationPtrArrayActive[i];
		CopyMemory(m_cbSendBuffer+wSendSize,pGameStation,sizeof(tagGameStation));
		wSendSize+=sizeof(tagGameStation);
	}
	if (wSendSize>0) m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_STATION,m_cbSendBuffer,wSendSize);

	return true;
}

//发送站点
bool __cdecl CServerList::SendGameStationList(DWORD dwSocketID, WORD wKindID)
{
	//效验参数
	ASSERT(m_pITCPNetworkEngine!=NULL);

	//构造数据
	WORD wSendSize=0;
	tagGameStation * pGameStation=NULL;
	DWORD dwStationCount=(DWORD)m_ServerListInfoActive.m_GameStationPtrArrayActive.GetCount();
	for (DWORD i=0;i<dwStationCount;i++)
	{
		if ((wSendSize+sizeof(tagGameStation))>sizeof(m_cbSendBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_STATION,m_cbSendBuffer,wSendSize);
			wSendSize=0;
		}
		pGameStation=m_ServerListInfoActive.m_GameStationPtrArrayActive[i];
		if (pGameStation->wKindID==wKindID)
		{
			CopyMemory(m_cbSendBuffer+wSendSize,pGameStation,sizeof(tagGameStation));
			wSendSize+=sizeof(tagGameStation);
		}
	}
	if (wSendSize>0) m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_STATION,m_cbSendBuffer,wSendSize);

	return true;
}

//发送房间
bool __cdecl CServerList::SendGameServerList(DWORD dwSocketID)
{
	//效验参数
	ASSERT(m_pITCPNetworkEngine!=NULL);

	//构造数据
	WORD wSendSize=0;
	tagGameServer * pGameServer=NULL;
	DWORD dwServerCount=(DWORD)m_ServerListInfoActive.m_GameServerPtrArrayActive.GetCount();
	for (DWORD i=0;i<dwServerCount;i++)
	{
		if ((wSendSize+sizeof(tagGameServer))>sizeof(m_cbSendBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_SERVER,m_cbSendBuffer,wSendSize);
			wSendSize=0;
		}
		pGameServer=m_ServerListInfoActive.m_GameServerPtrArrayActive[i];
		CopyMemory(m_cbSendBuffer+wSendSize,pGameServer,sizeof(tagGameServer));
		wSendSize+=sizeof(tagGameServer);
	}
	if (wSendSize>0) m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_SERVER,m_cbSendBuffer,wSendSize);

	return true;
}

//发送房间
bool __cdecl CServerList::SendGameServerList(DWORD dwSocketID, WORD wKindID)
{
	//效验参数
	ASSERT(m_pITCPNetworkEngine!=NULL);

	//构造数据
	WORD wSendSize=0;
	tagGameServer * pGameServer=NULL;
	DWORD dwServerCount=(DWORD)m_ServerListInfoActive.m_GameServerPtrArrayActive.GetCount();
	for (DWORD i=0;i<dwServerCount;i++)
	{
		if ((wSendSize+sizeof(tagGameServer))>sizeof(m_cbSendBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_SERVER,m_cbSendBuffer,wSendSize);
			wSendSize=0;
		}
		pGameServer=m_ServerListInfoActive.m_GameServerPtrArrayActive[i];
		if (pGameServer->wKindID==wKindID)
		{
			CopyMemory(m_cbSendBuffer+wSendSize,pGameServer,sizeof(tagGameServer));
			wSendSize+=sizeof(tagGameServer);
		}
	}
	if (wSendSize>0) m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_SERVER,m_cbSendBuffer,wSendSize);
	m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_FINISH);

	return true;
}

//更新准备
bool __cdecl CServerList::ResetServerListBuffer()
{
	m_ServerListInfoBuffer.ResetInfo();
	return true;
}

//激活缓冲
bool __cdecl CServerList::ActiveServerListBuffer()
{
	m_ServerListInfoActive.CopyServerListInfo(m_ServerListInfoBuffer);
	return true;
}

//追加类型
bool CServerList::AppendGameTypeBuffer(tagGameType * pGameType, DWORD dwCount)
{
	for (DWORD i=0;i<dwCount;i++) m_ServerListInfoBuffer.InsertGameType(pGameType+i);
	return true;
}

//追加种类
bool CServerList::AppendGameKindBuffer(tagGameKind * pGameKind, DWORD dwCount)
{
	for (DWORD i=0;i<dwCount;i++) m_ServerListInfoBuffer.InsertGameKind(pGameKind+i);
	return true;
}

//追加站点
bool CServerList::AppendGameStationBuffer(tagGameStation * pGameStation, DWORD dwCount)
{
	for (DWORD i=0;i<dwCount;i++) m_ServerListInfoBuffer.InsertGameStation(pGameStation+i);
	return true;
}

//追加房间
bool CServerList::AppendGameServerBuffer(tagGameServer * pGameServer, DWORD dwCount)
{
	for (DWORD i=0;i<dwCount;i++) m_ServerListInfoBuffer.InsertGameServer(pGameServer+i);
	return true;
}

//获取人数//修改
WORD __cdecl CServerList::GetOnLineCountInfo(tagOnLineCountInfo OnLineCountInfo[], WORD wBufferCount)
{
	//变量定义
	WORD wKindOnLineCount=(WORD)m_ServerListInfoActive.m_GameKindPtrArrayActive.GetCount();

	//类型人数
	for (INT_PTR i=0;i<__min(wKindOnLineCount,wBufferCount);i++)
	{
		//获取对象
		tagGameKind * pGameKind=m_ServerListInfoActive.m_GameKindPtrArrayActive[i];

		//设置变量
		OnLineCountInfo[i].wKindID=pGameKind->wKindID;
		OnLineCountInfo[i].dwOnLineCount=pGameKind->dwOnLineCount;
	}

	return __min(wKindOnLineCount,wBufferCount);
}

//////////////////////////////////////////////////////////////////////////
