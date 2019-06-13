#ifndef ATTEMPER_ENGINE_SINK_HEAD_FILE
#define ATTEMPER_ENGINE_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "InitParamter.h"

//////////////////////////////////////////////////////////////////////////

//绑定参数
struct tagBindParameter
{
	DWORD							dwSocketID;							//网络标识
	DWORD							dwClientIP;							//连接地址
	DWORD							dwActiveTime;						//激活时间
};

//////////////////////////////////////////////////////////////////////////

//调度引擎钩子
class CAttemperEngineSink : public IAttemperEngineSink
{
	//友元定义
	friend class CCenterService;

	//配置变量
protected:
	CInitParamter *					m_pInitParamter;					//配置参数
	tagBindParameter *				m_pBindParameter;					//连接信息

	//接口变量
protected:
	ITimerEngine *					m_pITimerEngine;					//时间引擎
	IDataBaseEngine *				m_pIDataBaseEngine;					//数据引擎
	ITCPNetworkEngine *				m_pITCPNetworkEngine;				//网络引擎

	//组件定义
protected:
	CServerListCenterHelper			m_ServerListCenter;					//列表组件

	//函数定义
public:
	//构造函数
	CAttemperEngineSink();
	//析构函数
	virtual ~CAttemperEngineSink();

	//基础接口
public:
	//释放对象
	virtual void __cdecl Release() { return; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//异步接口
public:
	//启动事件
	virtual bool __cdecl OnAttemperEngineStart(IUnknownEx * pIUnknownEx);
	//停止事件
	virtual bool __cdecl OnAttemperEngineStop(IUnknownEx * pIUnknownEx);

	//事件接口
public:
	//控制事件
	virtual bool __cdecl OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize) { return true; }
	//自定事件
	virtual bool __cdecl OnEventAttemperData(WORD wRequestID, VOID * pData, WORD wDataSize) { return true; }

	//内核事件
public:
	//时间事件
	virtual bool __cdecl OnEventTimer(DWORD dwTimerID, WPARAM wBindParam) { return true; }
	//数据库事件
	virtual bool __cdecl OnEventDataBase(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize) { return true; }

	//连接事件
public:
	//连接事件
	virtual bool __cdecl OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode) { return true; }
	//关闭事件
	virtual bool __cdecl OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason) { return true; }
	//读取事件
	virtual bool __cdecl OnEventTCPSocketRead(WORD wServiceID, CMD_Command Command, VOID * pData, WORD wDataSize) { return true; }

	//网络事件
public:
	//应答事件
	virtual bool __cdecl OnEventTCPNetworkBind(DWORD dwClientIP, DWORD dwSocketID);
	//关闭事件
	virtual bool __cdecl OnEventTCPNetworkShut(DWORD dwClientIP, DWORD dwActiveTime, DWORD dwSocketID);
	//读取事件
	virtual bool __cdecl OnEventTCPNetworkRead(CMD_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//网络函数
private:
	//列表消息处理
	bool OnSocketMainServerList(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//房间消息处理
	bool OnSocketMainServerManager(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
};

//////////////////////////////////////////////////////////////////////////

#endif