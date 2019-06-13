#ifndef SERVER_LIST_HEAD_FILE
#define SERVER_LIST_HEAD_FILE

#pragma once

#include "Afxtempl.h"
#include "GameServiceExport.h"

//////////////////////////////////////////////////////////////////////////

//数组定义
typedef CArrayTemplate<tagGameType *> CGameTypePtrArray;
typedef CArrayTemplate<tagGameKind *> CGameKindPtrArray;
typedef CArrayTemplate<tagGameServer *> CGameServerPtrArray;
typedef CArrayTemplate<tagGameStation *> CGameStationPtrArray;

//////////////////////////////////////////////////////////////////////////

//列表信息
class CServerListInfo
{
	//变量定义
public:
	CGameTypePtrArray				m_GameTypePtrArrayActive;			//类型数组
	CGameTypePtrArray				m_GameTypePtrArrayBuffer;			//类型数组
	CGameKindPtrArray				m_GameKindPtrArrayActive;			//种类数组
	CGameKindPtrArray				m_GameKindPtrArrayBuffer;			//种类数组
	CGameServerPtrArray				m_GameServerPtrArrayActive;			//房间数组
	CGameServerPtrArray				m_GameServerPtrArrayBuffer;			//房间数组
	CGameStationPtrArray			m_GameStationPtrArrayActive;		//站点数组
	CGameStationPtrArray			m_GameStationPtrArrayBuffer;		//站点数组

	//函数定义
public:
	//构造函数
	CServerListInfo();
	//析构函数
	virtual ~CServerListInfo();

	//维护函数
public:
	//重置信息
	bool ResetInfo();
	//插入类型
	bool InsertGameType(tagGameType * pGameType);
	//插入种类
	bool InsertGameKind(tagGameKind * pGameKind);
	//插入站点
	bool InsertGameStation(tagGameStation * pGameStation);
	//插入房间
	bool InsertGameServer(tagGameServer * pGameServer);
	//拷贝信息
	bool CopyServerListInfo(CServerListInfo & ServerListInfoScr);
};

//////////////////////////////////////////////////////////////////////////

//列表管理
class CServerList
{
	//变量定义
protected:
	CServerListInfo					m_ServerListInfoBuffer;				//列表信息
	CServerListInfo					m_ServerListInfoActive;				//列表信息
	BYTE							m_cbSendBuffer[SOCKET_PACKET];		//发送缓冲

	//接口指针
protected:
	ITCPNetworkEngine *				m_pITCPNetworkEngine;				//网络引擎

public:
	//构造函数
	CServerList();
	//析构函数
	virtual ~CServerList();

	//配置接口
public:
	//设置接口
	virtual bool __cdecl SetSocketEngine(IUnknownEx * pIUnknownEx);

	//发送接口
public:
	//发送类型
	virtual bool __cdecl SendGameTypeList(DWORD dwSocketID);
	//发送种类
	virtual bool __cdecl SendGameKindList(DWORD dwSocketID);
	//发送站点
	virtual bool __cdecl SendGameStationList(DWORD dwSocketID);
	//发送站点
	virtual bool __cdecl SendGameStationList(DWORD dwSocketID, WORD wKindID);
	//发送房间
	virtual bool __cdecl SendGameServerList(DWORD dwSocketID);
	//发送房间
	virtual bool __cdecl SendGameServerList(DWORD dwSocketID, WORD wKindID);

	//更新接口
public:
	//更新准备
	virtual bool __cdecl ResetServerListBuffer();
	//激活缓冲
	virtual bool __cdecl ActiveServerListBuffer();
	//追加类型
	virtual bool __cdecl AppendGameTypeBuffer(tagGameType * pGameType, DWORD dwCount);
	//追加种类
	virtual bool __cdecl AppendGameKindBuffer(tagGameKind * pGameKind, DWORD dwCount);
	//追加站点
	virtual bool __cdecl AppendGameStationBuffer(tagGameStation * pGameStation, DWORD dwCount);
	//追加房间
	virtual bool __cdecl AppendGameServerBuffer(tagGameServer * pGameServer, DWORD dwCount);

	//功能接口
public:
	//获取人数
	virtual WORD __cdecl GetOnLineCountInfo(tagOnLineCountInfo OnLineCountInfo[], WORD wBufferCount);
};

//////////////////////////////////////////////////////////////////////////

#endif