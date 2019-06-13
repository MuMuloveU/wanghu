#ifndef USER_MANAGER_HEAD_FILE
#define USER_MANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "IGamePlaza.h"

//////////////////////////////////////////////////////////////////////////

//用户信息类
class CUserItem : public IUserItem
{
	friend class CClientUserManager;

	//变量定义
public:
	tagUserData						m_UserData;						//用户信息

	//内部变量
private:
	bool							m_bActive;						//激活有效

	//函数定义
public:
	//构造函数
	CUserItem();
	//析构函数
	virtual ~CUserItem();

	//基础接口
public:
	//释放对象
	virtual void __cdecl Release() { delete this; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//对象接口
public:
	//访问判断
	virtual bool __cdecl IsActive() { return m_bActive; }

	//属性接口
public:
	//游戏局数
	virtual LONG __cdecl GetUserPlayCount();
	//用户 I D
	virtual DWORD __cdecl GetUserID() { return m_UserData.dwUserID; }
	//用户名字
	virtual LPCTSTR __cdecl GetUserName() { return m_UserData.szName; }
	//获取用户
	virtual tagUserData * __cdecl GetUserData() { return &m_UserData; }

	//设置接口
public:
	//设置积分
	virtual void __cdecl SetUserScore(const tagUserScore * pUserScore);
	//设置状态
	virtual void __cdecl SetUserStatus(const tagUserStatus * pUserStatus);

	//功能函数
public:
	//清理数据
	void CleanData();
};

//////////////////////////////////////////////////////////////////////////

//类说明
typedef CArrayTemplate<CUserItem *> CUserItemPtrArray;

//用户管理类
class CClientUserManager : public IClientUserManager
{
	//变量定义
protected:
	CUserItemPtrArray				m_UserItemActive;					//活动数组
	static CUserItemPtrArray		m_UserItemStorage;					//存储数组

	//外部接口
protected:
	IUserManagerSink				* m_pIUserManagerSink;				//回调接口

	//函数定义
public:
	//构造函数
	CClientUserManager(void);
	//析构函数
	virtual ~CClientUserManager(void);

	//基础接口
public:
	//释放对象
	virtual void __cdecl Release() { delete this; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//管理接口
public:
	//设置回调接口
	virtual bool __cdecl SetUserManagerSink(IUnknownEx * pIUnknownEx);
	//增加用户
	virtual IUserItem * __cdecl ActiveUserItem(tagUserData & UserData);
	//删除用户
	virtual bool __cdecl DeleteUserItem(IUserItem * pIUserItem);
	//更新积分
	virtual bool __cdecl UpdateUserItemScore(IUserItem * pIUserItem, const tagUserScore * pUserScore);
	//更新状态
	virtual bool __cdecl UpdateUserItemStatus(IUserItem * pIUserItem, const tagUserStatus * pUserStatus);

	//信息接口
public:
	//获取人数
	virtual DWORD __cdecl GetOnLineCount() { return (DWORD)m_UserItemActive.GetCount(); }

	//查找接口
public:
	//枚举用户
	virtual IUserItem * __cdecl EnumUserItem(WORD wEnumIndex);
	//查找用户
	virtual IUserItem * __cdecl SearchUserByUserID(DWORD dwUserID);
	//查找用户
	virtual IUserItem * __cdecl SearchUserByGameID(DWORD dwGameID);
};

//////////////////////////////////////////////////////////////////////////

#endif