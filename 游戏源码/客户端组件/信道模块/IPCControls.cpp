#include "StdAfx.h"
#include "IPCControls.h"

//////////////////////////////////////////////////////////////////////////

//函数定义
CIPCSendCopyData::CIPCSendCopyData(HWND hWndServer, HWND hWndClient)
{
	m_hWndClient=hWndClient;
	SetServerHwnd(hWndServer);
}

//析构函数
CIPCSendCopyData::~CIPCSendCopyData()
{
}

//是否连接
bool CIPCSendCopyData::IsConnect()
{
	return IsWindow(m_hWndServer)?true:false;
}

//设置发送窗口
bool CIPCSendCopyData::SetClientHwnd(HWND hWndClient)
{
	m_hWndClient=NULL;
	if (IsWindow(hWndClient))
	{
		m_hWndClient=hWndClient;
		return true;
	}
	return false;
}

//设置接收窗口
bool CIPCSendCopyData::SetServerHwnd(HWND hWndServer)
{
	m_hWndServer=NULL;
	if (IsWindow(hWndServer))
	{
		m_hWndServer=hWndServer;
		return true;
	}
	return false;
}

//设置接收窗口
bool CIPCSendCopyData::SetServerHwnd(LPCSTR szClassName)
{
	ASSERT(szClassName!=NULL);
	m_hWndServer=FindWindow(szClassName,NULL);
	return (m_hWndServer!=NULL);
}

//发送数据
bool CIPCSendCopyData::SendData(WORD wMainCmdID, WORD wSubCmdID)
{
	if (!IsWindow(m_hWndServer)) return false;

	//构造数据
	IPC_Head IPCHead;
	IPCHead.wVersion=IPC_VER;
	IPCHead.wDataSize=sizeof(IPC_Head);
	IPCHead.wMainCmdID=wMainCmdID;
	IPCHead.wSubCmdID=wSubCmdID;
	
	//发送数据
	COPYDATASTRUCT CopyDataStruct;
	CopyDataStruct.lpData=&IPCHead;
	CopyDataStruct.cbData=IPCHead.wDataSize;
	CopyDataStruct.dwData=IPC_IDENTIFIER;
	SendMessage(m_hWndServer,WM_COPYDATA,(LPARAM)m_hWndClient,(WPARAM)&CopyDataStruct);

	return true;
}

//发送数据
bool CIPCSendCopyData::SendData(WORD wMainCmdID, WORD wSubCmdID, void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize<=IPC_PACKAGE);
	if (wDataSize>IPC_PACKAGE) return false;
	if (!IsWindow(m_hWndServer)) return false;

	//构造数据
	BYTE cbSendBuffer[IPC_PACKAGE+sizeof(IPC_Head)];
	IPC_Head * pIPCHead=(IPC_Head *)cbSendBuffer;
	pIPCHead->wVersion=IPC_VER;
	pIPCHead->wDataSize=sizeof(IPC_Head);
	pIPCHead->wMainCmdID=wMainCmdID;
	pIPCHead->wSubCmdID=wSubCmdID;
	if (pBuffer!=NULL)
	{
		pIPCHead->wDataSize+=wDataSize;
		CopyMemory(cbSendBuffer+sizeof(IPC_Head),pBuffer,wDataSize);
	}

	//发送数据
	COPYDATASTRUCT CopyDataStruct;
	CopyDataStruct.lpData=cbSendBuffer;
	CopyDataStruct.cbData=pIPCHead->wDataSize;
	CopyDataStruct.dwData=IPC_IDENTIFIER;
	SendMessage(m_hWndServer,WM_COPYDATA,(LPARAM)m_hWndClient,(WPARAM)&CopyDataStruct);

	return true;
}

//发送数据
bool CIPCSendCopyData::SendIPCData(DWORD dwIdentifier, void * pBuffer, WORD wSize)
{
	if (!IsWindow(m_hWndServer)) return false;

	COPYDATASTRUCT CopyDataStruct;
	CopyDataStruct.lpData=pBuffer;
	CopyDataStruct.cbData=wSize;
	CopyDataStruct.dwData=dwIdentifier;
	SendMessage(m_hWndServer,WM_COPYDATA,(LPARAM)m_hWndClient,(WPARAM)&CopyDataStruct);

	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CIPCRecvCopyData::CIPCRecvCopyData()
{
	m_pIChannelMessageSink=NULL;
}

//析构函数
CIPCRecvCopyData::~CIPCRecvCopyData()
{
}

//设置回调接口
bool CIPCRecvCopyData::SetChannelMessageSink(IUnknownEx * pIUnknownEx)
{
	ASSERT(pIUnknownEx!=NULL);
	m_pIChannelMessageSink=(IChannelMessageSink *)pIUnknownEx->QueryInterface(IID_IChannelMessageSink,VER_IChannelMessageSink);
	ASSERT(m_pIChannelMessageSink!=NULL);
	return (m_pIChannelMessageSink!=NULL);
}

//COPYDATA 消息处理
bool CIPCRecvCopyData::OnCopyData(HWND hWndSend, COPYDATASTRUCT * pCopyDataStruct)
{
	//效验数据
	ASSERT(m_pIChannelMessageSink!=NULL);
	if (m_pIChannelMessageSink==NULL) return false;
	if (pCopyDataStruct->dwData!=IPC_IDENTIFIER) return false;

	//提取数据
	IPC_Head IPCHead;
	WORD wDataSize=0;
	void * pIPCBuffer=NULL;
	if (!GetIPCData(IPCHead,pIPCBuffer,wDataSize,pCopyDataStruct)) return false;

	//处理数据
	return m_pIChannelMessageSink->OnChannelMessage(&IPCHead,pIPCBuffer,wDataSize,hWndSend);
}

//获取数据
bool CIPCRecvCopyData::GetIPCData(IPC_Head & IPCHead, void * & pIPCBuffer, WORD & wDataSize, PCOPYDATASTRUCT pCopyDataStruct)
{
	//效验数据
	ASSERT(pCopyDataStruct!=NULL);
	if (pCopyDataStruct->dwData!=IPC_IDENTIFIER) return false;
	if (pCopyDataStruct->cbData<sizeof(IPC_Head)) return false;

	//提取数据
	CopyMemory(&IPCHead,pCopyDataStruct->lpData,sizeof(IPC_Head));
	if (IPCHead.wVersion!=IPC_VER) return false;
	if (IPCHead.wDataSize!=pCopyDataStruct->cbData) return false;

	//叠加数据
	IPCHead.wDataSize-=sizeof(IPC_Head);
	wDataSize=IPCHead.wDataSize;
	if (wDataSize>0) pIPCBuffer=((IPC_Head *)pCopyDataStruct->lpData)+1;
	else pIPCBuffer=NULL;

	return true;
}

//////////////////////////////////////////////////////////////////////////
