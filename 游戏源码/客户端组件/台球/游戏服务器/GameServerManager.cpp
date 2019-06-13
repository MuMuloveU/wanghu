#include "StdAfx.h"
#include "Tableframesink.h"
#include "GameServerManager.h"


//////////////////////////////////////////////////////////////////////////

//全局变量
static CGameServiceManager			g_GameServiceManager;				//管理变量

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameServiceManager::CGameServiceManager(void)
{
	//设置属性
	m_GameServiceAttrib.wKindID=KIND_ID;
	m_GameServiceAttrib.wChairCount=GAME_PLAYER;
	lstrcpyn(m_GameServiceAttrib.szKindName,GAME_NAME,CountArray(m_GameServiceAttrib.szKindName));
	lstrcpyn(m_GameServiceAttrib.szDescription,TEXT("台球游戏服务组件"),CountArray(m_GameServiceAttrib.szDescription));
	m_GameServiceAttrib.cbJoinInGame = FALSE;
	lstrcpyn(m_GameServiceAttrib.szDataBaseName,TEXT( "QPBilliardsDB" ),CountArray(m_GameServiceAttrib.szDataBaseName));	
	lstrcpyn(m_GameServiceAttrib.szClientModuleName,TEXT("Billiards.EXE"),CountArray(m_GameServiceAttrib.szClientModuleName));
	lstrcpyn(m_GameServiceAttrib.szServerModuleName,TEXT("BilliardsServer.DLL"),CountArray(m_GameServiceAttrib.szServerModuleName));

	return;
}

//析构函数
CGameServiceManager::~CGameServiceManager(void)
{
}

//接口查询
void * __cdecl CGameServiceManager::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IGameServiceManager,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IGameServiceManager,Guid,dwQueryVer);
	return NULL;
}

//创建游戏桌
void * __cdecl CGameServiceManager::CreateTableFrameSink(const IID & Guid, DWORD dwQueryVer)
{
	//建立对象
	CTableFrameSink * pTableFrameSink=NULL;
	try
	{
		pTableFrameSink=new CTableFrameSink();
		if (pTableFrameSink==NULL) throw TEXT("创建失败");
		void * pObject=pTableFrameSink->QueryInterface(Guid,dwQueryVer);
		if (pObject==NULL) throw TEXT("接口查询失败");
		return pObject;
	}
	catch (...) {}

	//清理对象
	SafeDelete(pTableFrameSink);

	return NULL;
}

//获取属性
void __cdecl CGameServiceManager::GetGameServiceAttrib(tagGameServiceAttrib & GameServiceAttrib)
{
	GameServiceAttrib=m_GameServiceAttrib;
	return;
}

//参数修改
bool __cdecl CGameServiceManager::RectifyServiceOption(tagGameServiceOption * m_pGameServiceOption)
{
	//效验参数
	ASSERT(m_pGameServiceOption!=NULL);
	if (m_pGameServiceOption==NULL) return false;

	//积分调整
	m_pGameServiceOption->lCellScore=__max(1L,m_pGameServiceOption->lCellScore);
	m_pGameServiceOption->lLessScore=__max(m_pGameServiceOption->lCellScore*10L,m_pGameServiceOption->lLessScore);
	if (m_pGameServiceOption->lRestrictScore!=0L) m_pGameServiceOption->lRestrictScore=__max(m_pGameServiceOption->lRestrictScore,m_pGameServiceOption->lLessScore);

	return true;
}

//////////////////////////////////////////////////////////////////////////

//建立对象函数
extern "C" __declspec(dllexport) void * __cdecl CreateGameServiceManager(const GUID & Guid, DWORD dwInterfaceVer)
{
	return g_GameServiceManager.QueryInterface(Guid,dwInterfaceVer);
}

//////////////////////////////////////////////////////////////////////////
