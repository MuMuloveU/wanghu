#ifndef ANDROID_USER_MANAGER_HEAD_FILE
#define ANDROID_USER_MANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "AfxTempl.h"
#include "AndroidUserItem.h"
#include "GameServiceExport.h"

//////////////////////////////////////////////////////////////////////////

//数组数组
typedef CArrayTemplate<CAndroidUserItem *> CAndroidUserItemArray;
typedef CMap<DWORD,DWORD,CAndroidUserItem *, CAndroidUserItem *> CAndroidUserItemMap;

//////////////////////////////////////////////////////////////////////////

//机器人管理
class CAndroidUserManager : public IAndroidUserManager
{
	//友元定义
	friend class CAttemperEngineSink;

	//配置变量
protected:
	WORD							m_wAutoAndroidCount;				//自动数目

	//库存变量
protected:
	WORD							m_wStockCount;						//库存数目
	DWORD							m_dwAndroidStock[MAX_ANDROID];		//库存用户

	//组件接口
protected:
	ITimerEngine *					m_pITimerEngine;					//时间引擎
	IServerUserManager *			m_pIServerUserManager;				//用户管理
	IGameServiceManager *			m_pIGameServiceManager;				//服务管理
	ITCPNetworkEngineEvent *		m_pITCPNetworkEngineEvent;			//网络接口

	//系统配置
protected:
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//服务属性
	tagGameServiceOption *			m_pGameServiceOption;				//服务配置

	//用户数组
protected:
	CAndroidUserItemMap				m_AndroidUserItemMap;				//用户索引
	CAndroidUserItemArray			m_AndroidUserItemFree;				//空闲对象
	CAndroidUserItemArray			m_AndroidUserItemStorage;			//存储对象

	//函数定义
public:
	//构造函数
	CAndroidUserManager();
	//析构函数
	virtual ~CAndroidUserManager();

	//基础接口
public:
	//释放对象
	virtual VOID __cdecl Release() { return; }
	//接口查询
	virtual VOID * __cdecl QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//控制接口
public:
	//停止服务
	virtual bool __cdecl StopService();
	//启动服务
	virtual bool __cdecl StartService();

	//配置接口
public:
	//设置库存
	virtual bool __cdecl SetAndroidStock(DWORD dwAndroidStock[], WORD wStockCount);

	//管理接口
public:
	//删除机器人
	virtual bool __cdecl DeleteAndroidUserItem(DWORD dwAndroidID);
	//切换机器人
	virtual IAndroidUserItem * __cdecl SwitchAndroidUserItem(IServerUserItem * pIServerUserItem);
	//创建机器人
	virtual IAndroidUserItem * __cdecl CreateAndroidUserItem(DWORD dwUserID, TCHAR szPassword[PASS_LEN]);

	//系统事件
public:
	//脉冲事件
	virtual bool __cdecl OnEventTimerPulse(DWORD dwTimerID, WPARAM dwBindParameter);

	//网络接口
public:
	//发送数据
	virtual bool __cdecl SendDataToClient(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//发送数据
	virtual bool __cdecl SendDataToClient(DWORD dwAndroidID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//发送数据
	virtual bool __cdecl SendDataToServer(DWORD dwAndroidID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//对象管理
protected:
	//获取对象
	CAndroidUserItem * GetAndroidUserItem(WORD wIndex);
	//激活对象
	CAndroidUserItem * ActiveAndroidUserItem(DWORD dwUserID);
	//释放对象
	bool FreeAndroidUserItem(CAndroidUserItem * pAndroidUserItem);
};

//////////////////////////////////////////////////////////////////////////

#endif