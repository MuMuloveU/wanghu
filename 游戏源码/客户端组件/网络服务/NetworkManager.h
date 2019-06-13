#ifndef NETWORK_MANAGER_HEAD_FILE
#define NETWORK_MANAGER_HEAD_FILE

#pragma once

#include "NetworkServiceHead.h"

//////////////////////////////////////////////////////////////////////////

//网络管理
class CNetworkManager : public INetworkManager
{
	//函数定义
public:
	//构造函数
	CNetworkManager();
	//析构函数
	virtual ~CNetworkManager();

	//基础接口
public:
	//释放对象
	virtual VOID __cdecl Release() { return; }
	//接口查询
	virtual VOID * __cdecl QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//性能查询
public:
	//枚举代理
	virtual bool __cdecl EnumProxyServerType(WORD wIndex, tagProxyTypeDescribe & ProxyTypeDescribe);
	//代理测试
	virtual DWORD __cdecl ProxyServerTesting(enProxyServerType ProxyServerType, const tagProxyServerInfo & ProxyServerInfo);
};

//////////////////////////////////////////////////////////////////////////

#endif