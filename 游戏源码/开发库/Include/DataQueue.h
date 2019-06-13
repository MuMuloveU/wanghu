#ifndef DATA_QUEUE_HEAD_FILE
#define DATA_QUEUE_HEAD_FILE

#pragma once

#include "ComService.h"

//////////////////////////////////////////////////////////////////////////
//结构定义

//数据列头
struct tagDataHead
{
	WORD							wDataSize;							//数据大小
	WORD							wIdentifier;						//类型标识
};

//负荷信息
struct tagBurthenInfo
{
	DWORD							dwDataSize;							//数据大小
	DWORD							dwBufferSize;						//缓冲长度
	DWORD							dwDataPacketCount;					//数据包数
};

//////////////////////////////////////////////////////////////////////////

//数据队列
class COM_SERVICE_CLASS CDataQueue
{
	//查询变量
protected:
	DWORD							m_dwInsertPos;						//插入位置
	DWORD							m_dwTerminalPos;					//结束位置
	DWORD							m_dwDataQueryPos;					//查询位置

	//数据变量
protected:
	DWORD							m_dwDataSize;						//数据大小
	DWORD							m_dwDataPacketCount;				//数据包数

	//缓冲变量
protected:
	DWORD							m_dwBufferSize;						//缓冲长度
	LPBYTE							m_pDataQueueBuffer;					//缓冲指针

	//函数定义
public:
	//构造函数
	CDataQueue();
	//析构函数
	virtual ~CDataQueue();

	//功能函数
public:
	//负荷信息
	bool GetBurthenInfo(tagBurthenInfo & BurthenInfo);

	//数据管理
public:
	//删除数据
	VOID RemoveData(bool bFreeMemroy);
	//插入数据
	bool InsertData(WORD wIdentifier, VOID * const pBuffer, WORD wDataSize);
	//提取数据
	bool DistillData(tagDataHead & DataHead, VOID * pBuffer, WORD wBufferSize);
};

//////////////////////////////////////////////////////////////////////////

#endif