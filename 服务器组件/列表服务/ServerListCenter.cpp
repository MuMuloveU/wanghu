#include "StdAfx.h"
#include "ServerListCenter.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CServerListCenter::CServerListCenter(void)
{
	//初始化
	m_TypeItemMap.InitHashTable(GetMaxPrime(20));
	m_KindItemMap.InitHashTable(GetMaxPrime(50));
	m_ServerItemMap.InitHashTable(GetMaxPrime(500));
	m_StationItemMap.InitHashTable(GetMaxPrime(200));

	return;
}

//析构函数
CServerListCenter::~CServerListCenter(void)
{
	//删除种类
	tagGameTypeItem * pGameTypeItem=NULL;
	for (INT_PTR i=0;i<m_TypeItemStore.GetCount();i++)
	{
		pGameTypeItem=m_TypeItemStore[i];
		SafeDelete(pGameTypeItem);
	}
	m_TypeItemStore.RemoveAll();

	//删除类型
	tagGameKindItem * pGameKindItem=NULL;
	for (INT_PTR i=0;i<m_KindItemStore.GetCount();i++)
	{
		pGameKindItem=m_KindItemStore[i];
		SafeDelete(pGameKindItem);
	}
	m_KindItemStore.RemoveAll();

	//删除房间
	tagGameServerItem * pGameServerItem=NULL;
	for (INT_PTR i=0;i<m_ServerItemStore.GetCount();i++)
	{
		pGameServerItem=m_ServerItemStore[i];
		SafeDelete(pGameServerItem);
	}
	m_ServerItemStore.RemoveAll();

	//删除站点
	tagGameStationItem * pGameStationItem=NULL;
	for (INT_PTR i=0;i<m_StationItemStore.GetCount();i++)
	{
		pGameStationItem=m_StationItemStore[i];
		SafeDelete(pGameStationItem);
	}
	m_StationItemStore.RemoveAll();

	return;
}

//接口查询
void * __cdecl CServerListCenter::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IServerListCenter,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IServerListCenter,Guid,dwQueryVer);
	return NULL;
}

//插入种类
bool __cdecl CServerListCenter::InsertGameType(tagGameType * pGameType)
{
	//效验参数
	ASSERT(pGameType!=NULL);
	if (pGameType==NULL) return false;

	//查找现存
	tagGameTypeItem * pGameTypeItem=NULL;
	if (m_TypeItemMap.Lookup(pGameType->wTypeID,pGameTypeItem)==TRUE) return false;

	try
	{
		//生成数据
		INT_PTR nStroeCount=m_TypeItemStore.GetCount();
		if (nStroeCount>0)
		{
			pGameTypeItem=m_TypeItemStore[nStroeCount-1];
			m_TypeItemStore.RemoveAt(nStroeCount-1);
		}
		else
		{
			pGameTypeItem=new tagGameTypeItem;
			if (pGameTypeItem==NULL) return false;
		}

		//插入数据
		CopyMemory(&pGameTypeItem->GameType,pGameType,sizeof(tagGameType));
		m_TypeItemMap[pGameType->wTypeID]=pGameTypeItem;

		return true;
	}
	catch (...) {}

	return false;
}

//插入类型
bool __cdecl CServerListCenter::InsertGameKind(tagGameKind * pGameKind)
{
	//效验参数
	ASSERT(pGameKind!=NULL);
	if (pGameKind==NULL) return false;

	//查找现存
	tagGameKindItem * pGameKindItem=NULL;
	if (m_KindItemMap.Lookup(pGameKind->wKindID,pGameKindItem)==TRUE) return false;

	try
	{
		//生成数据
		INT_PTR nStroeCount=m_KindItemStore.GetCount();
		if (nStroeCount>0)
		{
			pGameKindItem=m_KindItemStore[nStroeCount-1];
			m_KindItemStore.RemoveAt(nStroeCount-1);
		}
		else
		{
			pGameKindItem=new tagGameKindItem;
			if (pGameKindItem==NULL) return false;
		}

		//插入数据
		memset(pGameKindItem,0,sizeof(pGameKindItem));
		CopyMemory(&pGameKindItem->GameKind,pGameKind,sizeof(tagGameKind));
		m_KindItemMap[pGameKind->wKindID]=pGameKindItem;

		return true;
	}
	catch (...) {}

	return false;
}

//插入房间
bool __cdecl CServerListCenter::InsertGameServer(tagGameServer * pGameServer)
{
	//效验参数
	ASSERT(pGameServer!=NULL);
	if (pGameServer==NULL) return false;

	//查找现存
	tagGameServerItem * pGameServerItem=NULL;;
	DWORD dwKey=MAKELONG(pGameServer->wKindID,pGameServer->wServerID);
	if (m_ServerItemMap.Lookup(dwKey,pGameServerItem)==TRUE) return false;

	try
	{
		//生成数据
		INT_PTR nStroeCount=m_ServerItemStore.GetCount();
		if (nStroeCount>0)
		{
			pGameServerItem=m_ServerItemStore[nStroeCount-1];
			m_ServerItemStore.RemoveAt(nStroeCount-1);
		}
		else
		{
			pGameServerItem=new tagGameServerItem;
			if (pGameServerItem==NULL) return false;
		}

		//插入数据
		pGameServerItem->dwUpdateTime=(DWORD)time(NULL);
		CopyMemory(&pGameServerItem->GameServer,pGameServer,sizeof(tagGameServer));
		m_ServerItemMap[dwKey]=pGameServerItem;

		//错误事件
		TCHAR szBuffer[256]=TEXT("");
		_snprintf(szBuffer,sizeof(szBuffer),TEXT("【%s  KindID：%ld StationID：%ld  ServerID：%ld】注册成功"),
			pGameServer->szServerName,pGameServer->wKindID,pGameServer->wStationID,pGameServer->wServerID);
		CTraceService::TraceString(szBuffer,TraceLevel_Debug);

		//更新人数
		UpdateKindOnLineCount(pGameServer->wKindID);

		return true;
	}
	catch (...) {}

	return false;
}

//插入站点
bool __cdecl CServerListCenter::InsertGameStation(tagGameStation * pGameStation)
{
	//效验参数
	ASSERT(pGameStation!=NULL);
	if (pGameStation==NULL) return false;

	//查找现存
	tagGameStationItem * pGameStationItem=NULL;
	DWORD dwKey=MAKELONG(pGameStation->wKindID,pGameStation->wStationID);
	if (m_StationItemMap.Lookup(dwKey,pGameStationItem)==TRUE) return false;

	try
	{
		//生成数据
		INT_PTR nStroeCount=m_StationItemStore.GetCount();
		if (nStroeCount>0)
		{
			pGameStationItem=m_StationItemStore[nStroeCount-1];
			m_StationItemStore.RemoveAt(nStroeCount-1);
		}
		else
		{
			pGameStationItem=new tagGameStationItem;
			if (pGameStationItem==NULL) return false;
		}

		//插入数据
		CopyMemory(&pGameStationItem->GameStation,pGameStation,sizeof(tagGameStation));
		m_StationItemMap[dwKey]=pGameStationItem;

		return true;
	}
	catch (...) {}

	return false;
}

//删除种类
bool __cdecl CServerListCenter::DeleteGameType(WORD wTypeID)
{
	//查找种类
	WORD wKey=0;
	tagGameTypeItem * pGameTypeItem=NULL;
	if (m_TypeItemMap.Lookup(wKey,pGameTypeItem)==FALSE) return false;

	//删除数据
	m_TypeItemMap.RemoveKey(wKey);
	m_TypeItemStore.Add(pGameTypeItem);

	return true;
}

//删除类型
bool __cdecl CServerListCenter::DeleteGameKind(WORD wKindID)
{
	//查找类型
	WORD wKey=0;
	tagGameKindItem * pGameKindItem=NULL;
	if (m_KindItemMap.Lookup(wKey,pGameKindItem)==FALSE) return false;

	//删除数据
	m_KindItemMap.RemoveKey(wKey);
	m_KindItemStore.Add(pGameKindItem);

	return true;
}

//删除房间
bool __cdecl CServerListCenter::DeleteGameServer(WORD wKindID, WORD wServerID)
{
	//查找房间
	tagGameServerItem * pGameServerItem=NULL;
	DWORD dwKey=MAKELONG(wKindID,wServerID);
	if (m_ServerItemMap.Lookup(dwKey,pGameServerItem)==FALSE) return false;

	//删除数据
	m_ServerItemMap.RemoveKey(dwKey);
	m_ServerItemStore.Add(pGameServerItem);

	//更新人数
	UpdateKindOnLineCount(wKindID);

	return false;
}

//删除站点
bool __cdecl CServerListCenter::DeleteGameStation(WORD wKindID, WORD wStationID)
{
	//查找站点
	tagGameStationItem * pGameStationItem=NULL;
	DWORD dwKey=MAKELONG(wKindID,wStationID);
	if (m_StationItemMap.Lookup(dwKey,pGameStationItem)==FALSE) return false;

	//删除数据
	m_StationItemMap.RemoveKey(dwKey);
	m_StationItemStore.Add(pGameStationItem);

	return false;
}

//枚举种类
tagGameTypeItem * const __cdecl CServerListCenter::EmunGameTypeItem(POSITION & Pos)
{
	//调整参数
	if (Pos==NULL) Pos=m_TypeItemMap.GetStartPosition();
	if (Pos==NULL) return NULL;

	//查找房间
	WORD wKey=0;
	tagGameTypeItem * pGameTypeItem=NULL;
	m_TypeItemMap.GetNextAssoc(Pos,wKey,pGameTypeItem);

	return pGameTypeItem;
}

//枚举类型
tagGameKindItem * const __cdecl CServerListCenter::EmunGameKindItem(POSITION & Pos)
{
	//调整参数
	if (Pos==NULL) Pos=m_KindItemMap.GetStartPosition();
	if (Pos==NULL) return NULL;

	//查找房间
	WORD wKey=0;
	tagGameKindItem * pGameKindItem=NULL;
	m_KindItemMap.GetNextAssoc(Pos,wKey,pGameKindItem);

	return pGameKindItem;
}

//枚举房间
tagGameServerItem * const __cdecl CServerListCenter::EmunGameServerItem(POSITION & Pos)
{
	//调整参数
	if (Pos==NULL) Pos=m_ServerItemMap.GetStartPosition();
	if (Pos==NULL) return NULL;

	//查找房间
	DWORD dwKey=0;
	tagGameServerItem * pGameServerItem=NULL;
	m_ServerItemMap.GetNextAssoc(Pos,dwKey,pGameServerItem);

	return pGameServerItem;
}

//枚举站点
tagGameStationItem * const __cdecl CServerListCenter::EmunGameStationItem(POSITION & Pos)
{
	//调整参数
	if (Pos==NULL) Pos=m_StationItemMap.GetStartPosition();
	if (Pos==NULL) return NULL;

	//查找站点
	DWORD dwKey=0;
	tagGameStationItem * pGameStationItem=NULL;
	m_StationItemMap.GetNextAssoc(Pos,dwKey,pGameStationItem);

	return pGameStationItem;
}

//查找种类
tagGameTypeItem * const __cdecl CServerListCenter::SearchGameType(WORD wTypeID)
{
	tagGameTypeItem * pGameTypeItem=NULL;
	m_TypeItemMap.Lookup(wTypeID,pGameTypeItem);
	return pGameTypeItem;
}

//查找类型
tagGameKindItem * const __cdecl CServerListCenter::SearchGameKind(WORD wKindID)
{
	tagGameKindItem * pGameKindItem=NULL;
	m_KindItemMap.Lookup(wKindID,pGameKindItem);
	return pGameKindItem;
}

//查找房间
tagGameServerItem * const __cdecl CServerListCenter::SearchGameServer(WORD wKindID, WORD wServerID)
{
	tagGameServerItem * pGameServerItem=NULL;
	m_ServerItemMap.Lookup(MAKELONG(wKindID,wServerID),pGameServerItem);
	return pGameServerItem;
}

//查找站点
tagGameStationItem * const __cdecl CServerListCenter::SearchGameStation(WORD wKindID, WORD wStationID)
{
	tagGameStationItem * pGameStationItem=NULL;
	m_StationItemMap.Lookup(MAKELONG(wKindID,wStationID),pGameStationItem);
	return pGameStationItem;
}

//重置列表
void __cdecl CServerListCenter::ResetServerList()
{
	//删除种类
	tagGameTypeItem * pGameTypeItem=NULL;
	POSITION Pos=m_TypeItemMap.GetStartPosition();
	while (Pos!=NULL)
	{
		WORD wKey=0;
		m_TypeItemMap.GetNextAssoc(Pos,wKey,pGameTypeItem);
		m_TypeItemStore.Add(pGameTypeItem);
	}
	m_TypeItemMap.RemoveAll();

	//删除类型
	tagGameKindItem * pGameKindItem=NULL;
	Pos=m_KindItemMap.GetStartPosition();
	while (Pos!=NULL)
	{
		WORD wKey=0;
		m_KindItemMap.GetNextAssoc(Pos,wKey,pGameKindItem);
		m_KindItemStore.Add(pGameKindItem);
	}
	m_KindItemMap.RemoveAll();

	//删除房间
	Pos=m_ServerItemMap.GetStartPosition();
	tagGameServerItem * pGameServerItem=NULL;
	while (Pos!=NULL)
	{
		DWORD dwKey=0;
		m_ServerItemMap.GetNextAssoc(Pos,dwKey,pGameServerItem);
		m_ServerItemStore.Add(pGameServerItem);
	}
	m_ServerItemMap.RemoveAll();

	//删除站点
	Pos=m_StationItemMap.GetStartPosition();
	tagGameStationItem * pGameStationItem=NULL;
	while (Pos!=NULL)
	{
		DWORD dwKey=0;
		m_StationItemMap.GetNextAssoc(Pos,dwKey,pGameStationItem);
		m_StationItemStore.Add(pGameStationItem);
	}
	m_StationItemMap.RemoveAll();

	return;
}

//加载列表
bool __cdecl CServerListCenter::LoadServerList(tagDataBaseInfo & DataBaseInfo)
{
	//重置列表
	ResetServerList();

	//创建数据库
	CDataBaseAide  ServerInfoDBAide;
	CDataBaseHelper	ServerInfoDBModule;

	//创建对象
	if (ServerInfoDBModule.CreateInstance()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	try
	{
		//转化地址
		TCHAR szDataBaseAdr[16]=TEXT("");
		BYTE * pcbDataBaseAdr=(BYTE *)&DataBaseInfo.dwDataBaseAddr;
		_snprintf(szDataBaseAdr,sizeof(szDataBaseAdr),TEXT("%d.%d.%d.%d"),pcbDataBaseAdr[0],pcbDataBaseAdr[1],pcbDataBaseAdr[2],pcbDataBaseAdr[3]);

		//连接数据库
		ServerInfoDBModule->SetConnectionInfo(szDataBaseAdr,DataBaseInfo.wDataBasePort,DataBaseInfo.szDataBaseName,DataBaseInfo.szDataBaseUser,DataBaseInfo.szDataBasePass);

		//发起连接
		ServerInfoDBModule->OpenConnection();
		ServerInfoDBAide.SetDataBase(ServerInfoDBModule.GetInterface());

		//加载信息
		LoadGameType(ServerInfoDBAide);
		LoadGameKind(ServerInfoDBAide);
		LoadGameStation(ServerInfoDBAide);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		return false;
	}

	return false;
}

//更新人数
bool __cdecl CServerListCenter::UpdateServerOnLineCount(WORD wKindID, WORD wServerID, DWORD dwOnLineCount)
{
	tagGameServerItem * pGameServerItem=SearchGameServer(wKindID,wServerID);
	if (pGameServerItem!=NULL)
	{
		//更新房间
		pGameServerItem->dwUpdateTime=(DWORD)time(NULL);
		pGameServerItem->GameServer.dwOnLineCount=dwOnLineCount;

		//更新人数
		UpdateKindOnLineCount(wKindID);

		return true;
	}

	return false;
}

//加载类型
void CServerListCenter::LoadGameType(CDataBaseAide & DataBaseAide)
{
	//执行命令
	DataBaseAide.ResetParameter();
	DataBaseAide.ExecuteProcess(TEXT("GSP_LoadGameTypeItem"),true);

	//变量定义
	tagGameType GameTypeRead;
	ZeroMemory(&GameTypeRead,sizeof(GameTypeRead));

	//获取数据
	IDataBase * pIDataBase=(IDataBase *)DataBaseAide.GetDataBase(IID_IDataBase,VER_IDataBase);

	//读取数据
	while (pIDataBase->IsRecordsetEnd()==false)
	{
		//读取信息
		GameTypeRead.wTypeID=DataBaseAide.GetValue_WORD(TEXT("TypeID"));
		GameTypeRead.wSortID=DataBaseAide.GetValue_WORD(TEXT("SortID"));
		DataBaseAide.GetValue_String(TEXT("TypeName"),GameTypeRead.szTypeName,CountArray(GameTypeRead.szTypeName));

		//移动记录
		pIDataBase->MoveToNext();

		//插入信息
		InsertGameType(&GameTypeRead);
	}

	//关闭记录
	pIDataBase->CloseRecordset();

	return;
}

//加载种类
void CServerListCenter::LoadGameKind(CDataBaseAide & DataBaseAide)
{
	//执行命令
	DataBaseAide.ResetParameter();
	DataBaseAide.ExecuteProcess(TEXT("GSP_LoadGameKindItem"),true);

	//变量定义
	tagGameKind GameKindRead;
	ZeroMemory(&GameKindRead,sizeof(GameKindRead));

	//获取数据
	IDataBase * pIDataBase=(IDataBase *)DataBaseAide.GetDataBase(IID_IDataBase,VER_IDataBase);

	//读取信息
	while (pIDataBase->IsRecordsetEnd()==false)
	{
		//读取信息
		GameKindRead.wTypeID=DataBaseAide.GetValue_WORD(TEXT("TypeID"));
		GameKindRead.wKindID=DataBaseAide.GetValue_WORD(TEXT("KindID"));
		GameKindRead.wSortID=DataBaseAide.GetValue_WORD(TEXT("SortID"));
		GameKindRead.dwMaxVersion=DataBaseAide.GetValue_DWORD(TEXT("MaxVersion"));
		DataBaseAide.GetValue_String(TEXT("KindName"),GameKindRead.szKindName,CountArray(GameKindRead.szKindName));
		DataBaseAide.GetValue_String(TEXT("ProcessName"),GameKindRead.szProcessName,CountArray(GameKindRead.szProcessName));

		//移动记录
		pIDataBase->MoveToNext();

		//插入信息
		InsertGameKind(&GameKindRead);
	}

	//关闭记录
	pIDataBase->CloseRecordset();

	return;
}

//加载站点
void CServerListCenter::LoadGameStation(CDataBaseAide & DataBaseAide)
{
	//执行命令
	DataBaseAide.ResetParameter();
	DataBaseAide.ExecuteProcess(TEXT("GSP_LoadGameNodeItem"),true);

	//变量定义
	tagGameStation GameStationRead;
	ZeroMemory(&GameStationRead,sizeof(GameStationRead));

	//获取数据
	IDataBase * pIDataBase=(IDataBase *)DataBaseAide.GetDataBase(IID_IDataBase,VER_IDataBase);

	//读取信息
	while (pIDataBase->IsRecordsetEnd()==false)
	{
		//读取信息
		GameStationRead.wSortID=DataBaseAide.GetValue_WORD(TEXT("SortID"));
		GameStationRead.wKindID=DataBaseAide.GetValue_WORD(TEXT("KindID"));
		GameStationRead.wJoinID=DataBaseAide.GetValue_WORD(TEXT("JoinID"));
		GameStationRead.wStationID=DataBaseAide.GetValue_WORD(TEXT("NodeID"));
		DataBaseAide.GetValue_String(TEXT("NodeName"),GameStationRead.szStationName,CountArray(GameStationRead.szStationName));

		//插入信息
		InsertGameStation(&GameStationRead);

		//移动记录
		pIDataBase->MoveToNext();
	}

	//关闭记录
	pIDataBase->CloseRecordset();

	return;
}

//获取质数
UINT CServerListCenter::GetMaxPrime(UINT uLessNum)
{
	for (UINT uPrime=uLessNum;uPrime>2;uPrime--)
	{
		for(UINT u2=2;u2<=uPrime/2;u2++) if (uPrime%u2==0) break;
		if (u2>uPrime/2) break;
	}

	return uPrime;
}

//更新人数
void CServerListCenter::UpdateKindOnLineCount(WORD wKindID)
{
	tagGameKindItem * pGameKindItem=SearchGameKind(wKindID);
	if (pGameKindItem!=NULL)
	{
		POSITION Pos=NULL;
		DWORD dwKindAllLineCount=0L;
		tagGameServerItem * pGameServerItem=NULL;
		do
		{
			pGameServerItem=EmunGameServerItem(Pos);
			if (pGameServerItem==NULL) break;
			if (pGameServerItem->GameServer.wKindID==wKindID) dwKindAllLineCount+=pGameServerItem->GameServer.dwOnLineCount;
		} while (Pos!=NULL);
		pGameKindItem->GameKind.dwOnLineCount=dwKindAllLineCount;
	}

	return;
}

//////////////////////////////////////////////////////////////////////////

//建立对象函数
extern "C" __declspec(dllexport) void * __cdecl CreateServerListCenter(const GUID & Guid, DWORD dwInterfaceVer)
{
	//建立对象
	CServerListCenter * pServerListCenter=NULL;
	try
	{
		pServerListCenter=new CServerListCenter();
		if (pServerListCenter==NULL) throw TEXT("创建失败");
		void * pObject=pServerListCenter->QueryInterface(Guid,dwInterfaceVer);
		if (pObject==NULL) throw TEXT("接口查询失败");
		return pObject;
	}
	catch (...) {}

	//清理对象
	SafeDelete(pServerListCenter);
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
