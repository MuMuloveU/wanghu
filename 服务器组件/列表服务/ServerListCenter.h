#ifndef SERVER_LIST_HEAD_FILE
#define SERVER_LIST_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "AfxTempl.h"
#include "ListService.h"

//////////////////////////////////////////////////////////////////////////

//类说明
typedef CArrayTemplate<tagGameTypeItem *>								CGameTypeItemArray;
typedef CArrayTemplate<tagGameKindItem *>								CGameKindItemArray;
typedef CArrayTemplate<tagGameServerItem *>								CGameServerItemArray;
typedef CArrayTemplate<tagGameStationItem *>							CGameStationItemArray;
typedef CMap<WORD,WORD,tagGameTypeItem *,tagGameTypeItem * &>			CTypeItemMap;
typedef CMap<WORD,WORD,tagGameKindItem *,tagGameKindItem * &>			CKindItemMap;
typedef CMap<DWORD,DWORD,tagGameServerItem *,tagGameServerItem * &>		CServerItemMap;
typedef CMap<DWORD,DWORD,tagGameStationItem *,tagGameStationItem * &>	CStationItemMap;

//////////////////////////////////////////////////////////////////////////

//服务器列表类
class CServerListCenter : public IServerListCenter
{
	//索引管理
protected:
	CTypeItemMap					m_TypeItemMap;						//索引数组
	CKindItemMap					m_KindItemMap;						//索引数组
	CServerItemMap					m_ServerItemMap;					//索引数组
	CStationItemMap					m_StationItemMap;					//索引数组

	//存储管理
protected:
	CGameTypeItemArray				m_TypeItemStore;					//存储数组
	CGameKindItemArray				m_KindItemStore;					//存储数组
	CGameServerItemArray			m_ServerItemStore;					//存储数组
	CGameStationItemArray			m_StationItemStore;					//存储数组

	//函数定义
public:
	//构造函数
	CServerListCenter(void);
	//析构函数
	virtual ~CServerListCenter(void);

	//基础接口
public:
	//释放对象
	virtual void __cdecl Release() { delete this; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//插入接口
public:
	//插入种类
	virtual bool __cdecl InsertGameType(tagGameType * pGameType);
	//插入类型
	virtual bool __cdecl InsertGameKind(tagGameKind * pGameKind);
	//插入房间
	virtual bool __cdecl InsertGameServer(tagGameServer * pGameServer);
	//插入站点
	virtual bool __cdecl InsertGameStation(tagGameStation * pGameStation);

	//删除接口
public:
	//删除种类
	virtual bool __cdecl DeleteGameType(WORD wTypeID);
	//删除类型
	virtual bool __cdecl DeleteGameKind(WORD wKindID);
	//删除房间
	virtual bool __cdecl DeleteGameServer(WORD wKindID, WORD wServerID);
	//删除站点
	virtual bool __cdecl DeleteGameStation(WORD wKindID, WORD wStationID);

	//枚举接口
public:
	//枚举种类
	virtual tagGameTypeItem * const __cdecl EmunGameTypeItem(POSITION & Pos);
	//枚举类型
	virtual tagGameKindItem * const __cdecl EmunGameKindItem(POSITION & Pos);
	//枚举房间
	virtual tagGameServerItem * const __cdecl EmunGameServerItem(POSITION & Pos);
	//枚举站点
	virtual tagGameStationItem * const __cdecl EmunGameStationItem(POSITION & Pos);

	//查找接口
public:
	//查找种类
	virtual tagGameTypeItem * const __cdecl SearchGameType(WORD wTypeID);
	//查找类型
	virtual tagGameKindItem * const __cdecl SearchGameKind(WORD wKindID);
	//查找房间
	virtual tagGameServerItem * const __cdecl SearchGameServer(WORD wKindID, WORD wServerID);
	//查找站点
	virtual tagGameStationItem * const __cdecl SearchGameStation(WORD wKindID, WORD wStationID);

	//数目接口
public:
	//种类数目
	virtual DWORD __cdecl GetGameTypeCount() { return (DWORD)m_TypeItemMap.GetCount(); }
	//类型数目
	virtual DWORD __cdecl GetGameKindCount() { return (DWORD)m_KindItemMap.GetCount(); }
	//房间数目
	virtual DWORD __cdecl GetGameServerCount() { return (DWORD)m_ServerItemMap.GetCount(); }
	//站点数目
	virtual DWORD __cdecl GetGameStationCount() { return (DWORD)m_StationItemMap.GetCount(); }

	//维护接口
public:
	//重置列表
	virtual void __cdecl ResetServerList();
	//加载列表
	virtual bool __cdecl LoadServerList(tagDataBaseInfo & DataBaseInfo);
	//更新人数
	virtual bool __cdecl UpdateServerOnLineCount(WORD wKindID, WORD wServerID, DWORD dwOnLineCount);

	//加载接口
private:
	//加载类型
	void LoadGameType(CDataBaseAide & DataBaseAide);
	//加载种类
	void LoadGameKind(CDataBaseAide & DataBaseAide);
	//加载站点
	void LoadGameStation(CDataBaseAide & DataBaseAide);

	//内部函数
private:
	//获取质数
	UINT GetMaxPrime(UINT uLessNum);
	//更新人数
	void UpdateKindOnLineCount(WORD wKindID);
};

//////////////////////////////////////////////////////////////////////////

#endif