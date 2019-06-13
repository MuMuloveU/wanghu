#ifndef CENTER_SERVICE_HEAD_FILE
#define CENTER_SERVICE_HEAD_FILE

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

//中心服务类
class CCenterService
{
	//配置变量
protected:
	CInitParamter					m_InitParamter;						//配置参数
	enServiceStatus					m_ServiceStatus;					//运行状态

	//组件变量
protected:
	CDataBaseSink					m_DataBaseEngineSink;				//数据钩子
	CAttemperEngineSink				m_AttemperEngineSink;				//调度钩子

	//内核组件
protected:
	CAttemperEngineHelper			m_AttemperEngine;					//调度引擎
	CDataBaseEngineHelper			m_DataBaseEngine;					//数据引擎
	CTCPNetworkEngineHelper			m_TCPNetworkEngine;					//网络引擎

	//函数定义
public:
	//构造函数
	CCenterService();
	//析构函数
	virtual ~CCenterService();

	//功能函数
public:
	//开始服务
	bool StartService();
	//停止服务
	bool StopService();
	//服务状态
	bool IsService() { return m_ServiceStatus==ServiceStatus_Service; }
};


//////////////////////////////////////////////////////////////////////////

#endif
