#ifndef IPCCONTROLS_HEAD_FILE
#define IPCCONTROLS_HEAD_FILE

#pragma once

#include "ChannelModule.h"

//////////////////////////////////////////////////////////////////////////

//IPC 数据发送类
class CHANNEL_CLASS CIPCSendCopyData
{
	//变量定义
protected:
	HWND							m_hWndServer;						//接收窗口
	HWND							m_hWndClient;						//发送窗口

	//函数定义
public:
	//构造函数
	CIPCSendCopyData(HWND hWndServer=NULL, HWND hWndClient=NULL);
	//析构函数
	virtual ~CIPCSendCopyData();

	//接口函数
public:
	//是否连接
	bool IsConnect();
	//设置发送窗口
	bool SetClientHwnd(HWND hWndClient);
	//设置接收窗口
	bool SetServerHwnd(HWND hWndServer);
	//设置接收窗口
	bool SetServerHwnd(LPCSTR szClassName);
	//发送数据
	bool SendData(WORD wMainCmdID, WORD wSubCmdID);
	//发送数据
	bool SendData(WORD wMainCmdID, WORD wSubCmdID, void * pBuffer, WORD wDataSize);
	//发送数据（高级使用）
	bool SendIPCData(DWORD dwIdentifier, void * pBuffer, WORD wDataSize);
};

//////////////////////////////////////////////////////////////////////////

//IPC 数据接收类
class CHANNEL_CLASS CIPCRecvCopyData
{
	//变量定义
protected:
	IChannelMessageSink				* m_pIChannelMessageSink;				//回调接口

	//函数定义
public:
	//构造函数
	CIPCRecvCopyData();
	//析构函数
	virtual ~CIPCRecvCopyData();

	//接口函数
public:
	//设置回调接口
	bool SetChannelMessageSink(IUnknownEx * pIUnknownEx);
	//COPYDATA 消息处理
	bool OnCopyData(HWND hWndSend, COPYDATASTRUCT * pCopyDataStruct);

	//高级函数
public:
	//获取数据（高级处理）
	bool GetIPCData(IPC_Head & IPCHead, void * & pIPCBuffer, WORD & wDataSize, PCOPYDATASTRUCT pCopyDataStruct);
};

//////////////////////////////////////////////////////////////////////////

#endif
