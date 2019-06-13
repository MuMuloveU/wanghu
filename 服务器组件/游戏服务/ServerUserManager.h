#ifndef SERVER_USER_MANAGER_HEAD_FILE
#define SERVER_USER_MANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameServiceExport.h"

//////////////////////////////////////////////////////////////////////////

//用户信息项
class CServerUserItem : public IServerUserItem
{
	//友元定义
	friend class CServerUserManager;
	friend class CAttemperEngineSink;

	//基本信息
protected:
	bool								m_bAcitve;						//激活标志
	bool								m_bAndroid;						//机器用户
	LONG								m_lRevenue;						//游戏税收
	WORD								m_wServerType;					//游戏类型
	tagUserRule							m_UserRule;						//用户规则
	tagUserScore						m_ScoreBorn;					//原本积分
	tagUserScore						m_ScoreModify;					//变更积分
	tagServerUserData					m_ServerUserData;				//用户信息

	//辅助信息
protected:
	WORD								m_wUserIndex;					//用户索引
	DWORD								m_dwClientIP;					//连接地址
	DWORD								m_dwLogonTime;					//登录时间
	DWORD								m_dwPlayTimeCount;				//游戏时间
	TCHAR								m_szPassword[PASS_LEN];			//用户密码

	//函数定义
public:
	//构造函数
	CServerUserItem(void);
	//析构函数
	virtual ~CServerUserItem(void);

	//基础接口
public:
	//释放对象
	virtual void __cdecl Release() { delete this; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//信息接口
public:
	//是否激活
	virtual bool __cdecl IsActive() { return m_bAcitve; }
	//用户索引
	virtual WORD __cdecl GetUserIndex() { return m_wUserIndex; }
	//获取地址
	virtual DWORD __cdecl GetClientIP() { return m_dwClientIP; }
	//获取密码
	virtual LPCTSTR __cdecl GetPassword() { return m_szPassword; }
	//获取规则
	virtual const tagUserRule * __cdecl GetUserRule() { return &m_UserRule; }
	//获取积分
	virtual const tagUserScore * __cdecl GetUserScore() { return &m_ServerUserData.UserScoreInfo; }
	//获取信息
	virtual tagServerUserData * __cdecl GetUserData() { return &m_ServerUserData; };

	//属性接口
public:
	//用户名字
	virtual LPCTSTR __cdecl GetAccounts() { return m_ServerUserData.szAccounts; }
	//用户 I D
	virtual DWORD __cdecl GetUserID() { return m_ServerUserData.dwUserID; }
	//桌子号码
	virtual WORD __cdecl GetTableID() { return m_ServerUserData.wTableID; }
	//椅子号码
	virtual WORD __cdecl GetChairID() { return m_ServerUserData.wChairID; }
	//用户状态
	virtual BYTE __cdecl GetUserStatus() { return m_ServerUserData.cbUserStatus; }
	//用户胜率
	virtual WORD __cdecl GetUserWinRate();
	//用户输率
	virtual WORD __cdecl GetUserLostRate();
	//用户和率
	virtual WORD __cdecl GetUserDrawRate();
	//用户逃率
	virtual WORD __cdecl GetUserFleeRate();

	//统计接口
public:
	//获取税收
	virtual LONG __cdecl GetUserRevenue() { return m_lRevenue; }
	//游戏时间
	virtual DWORD __cdecl GetPlayTimeCount() { return m_dwPlayTimeCount; }
	//在线时间
	virtual DWORD __cdecl GetOnlineTimeCount() { return (DWORD)time(NULL)-m_dwLogonTime; }

	//功能接口
public:
	//设置规则
	virtual bool __cdecl SetUserRule(tagUserRule & UserRule);
	//游戏类型
	virtual bool __cdecl SetServerType(WORD wServerType);
	//积分信息
	virtual bool __cdecl GetUserScoreInfo(tagUserScore & ScoreModifyInfo);
	//修改积分
    virtual bool __cdecl WriteScore(tagScoreInfo & ScoreInfo, DWORD dwPlayTimeCount);
	//设置状态
	virtual bool __cdecl SetUserStatus(BYTE cbUserStatus, WORD wTableID, WORD wChairID);
	//修改金币
	virtual bool __cdecl ModifyBankStorageGold(LONG lStorageCount);
	//积分信息
	virtual tagUserScore* __cdecl GetUserScoreModifyInfo(){return &m_ScoreModify;}

	//控制用户
public:
	//控制状态
	virtual bool __cdecl IsAndroidUser() { return m_bAndroid; }
	//设置控制
	virtual VOID __cdecl SetAndroidUser(bool bAndroidUser) { m_bAndroid=bAndroidUser; }

	//功能函数
public:
	//初始化
	void InitServerUserItem();
};

//////////////////////////////////////////////////////////////////////////

//类说明
typedef CArrayTemplate<CServerUserItem *> CServerUserItemArray;

//服务器用户管理
class CServerUserManager : public IServerUserManager
{
	//变量定义
protected:
	CServerUserItemArray				m_UserItemStore;				//存储用户
	CServerUserItemArray				m_OnLineUserItem;				//在线用户
	CServerUserItemArray				m_OffLineUserItem;				//断线用户

	//函数定义
public:
	//构造函数
	CServerUserManager(void);
	//析构函数
	virtual ~CServerUserManager(void);

	//基础接口
public:
	//释放对象
	virtual void __cdecl Release() { delete this; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//管理接口
public:
	//重置用户
	virtual bool __cdecl ResetUserManager();
	//切换用户
	virtual bool __cdecl SwitchOnLineUserItem(IServerUserItem * pIServerUserItem, DWORD dwClientIP, WORD wUserIndex);
	//激活用户
	virtual bool __cdecl ActiveOffLineUserItem(IServerUserItem * pIServerUserItem, DWORD dwClientIP, WORD wUserIndex);
	//激活用户
	virtual IServerUserItem * __cdecl ActiveUserItem(tagServerUserData & ServerUserData, DWORD dwClientIP, WORD wUserIndex, TCHAR szPassword[PASS_LEN],WORD wServerType);
	//删除用户
	virtual bool __cdecl DeleteUserItem(IServerUserItem * pIServerUserItem);
	//注册断线
	virtual bool __cdecl RegOffLineUserItem(IServerUserItem * pIServerUserItem);
	//删除断线
	virtual bool __cdecl UnRegOffLineUserItem(IServerUserItem * pIServerUserItem);

	//查找接口
public:
	//枚举用户
	virtual IServerUserItem * __cdecl EnumOnLineUser(WORD wEnumIndex);
	//枚举用户
	virtual IServerUserItem * __cdecl EnumOffLineUser(WORD wEnumIndex);
	//查找用户
	virtual IServerUserItem * __cdecl SearchOnLineUser(DWORD dwUserID);
	//查找用户
	virtual IServerUserItem * __cdecl SearchOffLineUser(DWORD dwUserID);
	//查找用户
	virtual IServerUserItem * __cdecl SearchOnLineUser(LPCTSTR pszAccounts);
	//查找用户
	virtual IServerUserItem * __cdecl SearchOffLineUser(LPCTSTR pszAccounts);

	//信息接口
public:
	//在线人数
	virtual DWORD __cdecl GetOnLineCount() { return (DWORD)m_OnLineUserItem.GetCount(); }
	//断线人数
	virtual DWORD __cdecl GetOffLineCount() { return (DWORD)m_OffLineUserItem.GetCount(); }
};

//////////////////////////////////////////////////////////////////////////

#endif