#ifndef GAME_SERVICE_HEAD_FILE
#define GAME_SERVICE_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "DataBaseSink.h"
#include "AttemperEngineSink.h"

//////////////////////////////////////////////////////////////////////////

//服务状态
enum enServiceStatus
{
	ServiceStatus_Stop,				//停止状态
	ServiceStatus_Service,			//服务状态
};

//////////////////////////////////////////////////////////////////////////

//游戏服务类
class CGameService : public IGameService
{
	//变量定义
protected:
	enServiceStatus					m_ServiceStatus;					//运行状态

	//组件变量
protected:
	CDataBaseSink					m_DataBaseEngineSink;				//数据库钩子
	CAttemperEngineSink				m_AttemperEngineSink;				//调度钩子

	//内核组件
protected:
	CTimerEngineHelper				m_TimerEngine;						//时间引擎
	CAttemperEngineHelper			m_AttemperEngine;					//调度引擎
	CDataBaseEngineHelper			m_DataBaseEngine;					//数据引擎
	CTCPNetworkEngineHelper			m_TCPNetworkEngine;					//网络引擎
	CTCPSocketServiceHelper			m_TCPSocketCorrespond;				//协调服务

	//配置变量
protected:
	TCHAR							m_szModule[32];						//模块名字
	tagDataBaseInfo					m_GameUserDBInfo;					//数据配置
	tagDataBaseInfo					m_GameScoreDBInfo;					//数据配置
	tagGameServiceAttrib			m_GameServiceAttrib;				//服务属性
	tagGameServiceOption			m_GameServiceOption;				//服务配置

	//组件接口
protected:
	HINSTANCE						m_hDllInstance;						//游戏实例
	IGameServiceManager	*			m_pIGameServiceManager;				//管理接口

	//函数定义
public:
	//构造函数
	CGameService(void);
	//析构函数
	virtual ~CGameService(void);

	//基础接口
public:
	//释放对象
	virtual void __cdecl Release() { delete this; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//管理接口
public:
	//开始服务
	virtual bool __cdecl StartService(tagGameServiceParameter * pGameServiceParameter);
	//停止服务
	virtual bool __cdecl StopService();
	//服务状态
	virtual bool __cdecl IsService() { return m_ServiceStatus==ServiceStatus_Service; }

	//内部函数
private:
	//释放模块
	bool UnLoadGameServiceModule();
	//加载模块
	bool LoadGameServiceModule(LPCTSTR pszModuleName);
	//显示错误
	void ShowErrorMessasge(LPCTSTR pszString, enTraceLevel TraceLevel);
};

//////////////////////////////////////////////////////////////////////////

#endif