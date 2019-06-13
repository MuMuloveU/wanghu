#include "StdAfx.h"
#include "IPCControls.h"
#include "ChannelService.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

#define IDI_IPC_DATA				1									//IPC 数据
#define WM_IPC_MESSAGE				(WM_USER+100)						//IPC 消息
#define IPCPacketSize(IPCPacket)	(sizeof(tagIPCPacket)-sizeof(IPCPacket.cbBuffer))

//////////////////////////////////////////////////////////////////////////
//结构体定义

//IPC 数据项
struct tagIPCPacket
{
	IPC_Head						Head;								//数据包头
	HWND							hWndSend;							//发送窗口
	BYTE							cbBuffer[IPC_PACKAGE];				//数据缓冲
};

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CChannelService, CWnd)
	ON_WM_COPYDATA()
	ON_MESSAGE(WM_IPC_MESSAGE,OnIPCMessage)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CChannelService::CChannelService()
{
	//设置变量
	m_hWndServer=NULL;
	m_pIChannelEventSink=NULL;
	m_pIChannelMessageSink=NULL;

	//设置接口
	m_IPCRecvCopyData.SetChannelMessageSink(QUERY_ME_INTERFACE(IUnknownEx));

	return;
}

//析构函数
CChannelService::~CChannelService()
{
	CloseChannel(true,false);
	return;
}

//接口查询
void * __cdecl CChannelService::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IChannelService,Guid,dwQueryVer);
	QUERYINTERFACE(IChannelMessageSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IChannelService,Guid,dwQueryVer);
	return NULL;
}

//IPC 消息
BOOL CChannelService::OnCopyData(CWnd * pWnd, COPYDATASTRUCT * pCopyDataStruct)
{
	HWND hWndSend=pWnd->m_hWnd;
	if ((hWndSend==m_hWndServer)&&m_IPCRecvCopyData.OnCopyData(hWndSend,pCopyDataStruct)) return TRUE;
	return __super::OnCopyData(pWnd, pCopyDataStruct);
}

//设置事件
bool __cdecl CChannelService::SetChannelEventSink(IUnknownEx * pIUnknownEx)
{
	if (pIUnknownEx!=NULL)
	{
		m_pIChannelEventSink=(IChannelEventSink *)pIUnknownEx->QueryInterface(IID_IChannelEventSink,VER_IChannelEventSink);
		ASSERT(m_pIChannelEventSink!=NULL);
		return (m_pIChannelEventSink!=NULL);
	}
	else
	{
		m_pIChannelEventSink=NULL;
	}

	return true;
}

//设置消息
bool __cdecl CChannelService::SetChannelMessageSink(IUnknownEx * pIUnknownEx)
{
	if (pIUnknownEx!=NULL)
	{
		m_pIChannelMessageSink=(IChannelMessageSink *)pIUnknownEx->QueryInterface(IID_IChannelMessageSink,VER_IChannelMessageSink);
		ASSERT(m_pIChannelMessageSink!=NULL);
		return (m_pIChannelMessageSink!=NULL);
	}
	else
	{
		m_pIChannelMessageSink=NULL;
	}

	return true;
}

//获取事件
void * __cdecl CChannelService::GetChannelEventSink(const IID & Guid, DWORD dwQueryVer)
{
	if (m_pIChannelEventSink==NULL) return NULL;
	return m_pIChannelEventSink->QueryInterface(Guid,dwQueryVer);
}

//获取消息
void * __cdecl CChannelService::GetChannelMessageSink(const IID & Guid, DWORD dwQueryVer)
{
	if (m_pIChannelMessageSink==NULL) return NULL;
	return m_pIChannelMessageSink->QueryInterface(Guid,dwQueryVer);
}

//初始化
bool __cdecl CChannelService::CreateChannel(HWND hWndServer)
{
	//创建窗口
	m_hWndServer=hWndServer;
	m_DataStorage.RemoveData(false);
	if (Create(NULL,NULL,WS_CHILD,CRect(0,0,0,0),GetDesktopWindow(),10)==FALSE) return false;

	//发送连接通知
	return SendData(IPC_MIAN_IPC_KERNEL,IPC_SUB_IPC_CLIENT_CONNECT);
}

//是否连接
bool __cdecl CChannelService::IsConnect(bool bEfficacy)
{
	if (m_hWndServer==NULL) return false;
	if (bEfficacy==true) return IsWindow(m_hWndServer)?true:false;
	return true;
}

//关闭函数
bool __cdecl CChannelService::CloseChannel(bool bNotifyServer, bool bNotifySink)
{
	//判断状态
	if (m_hWndServer==NULL) return false;

	//发送关闭消息
	if (bNotifyServer==true) SendData(IPC_MIAN_IPC_KERNEL,IPC_SUB_IPC_CLIENT_CLOSE);
	if ((bNotifySink==true)&&(m_pIChannelEventSink!=NULL)) m_pIChannelEventSink->OnChannelClose(m_hWndServer,true);

	//设置变量
	DestroyWindow();
	m_hWndServer=NULL;
	m_DataStorage.RemoveData(false);

	return true;
}

//发送数据
bool __cdecl CChannelService::SendData(WORD wMainCmdID, WORD wSubCmdID)
{
	if (m_hWndServer==NULL) return false;
	CIPCSendCopyData IPCSendCopyData(m_hWndServer,m_hWnd);
	return IPCSendCopyData.SendData(wMainCmdID,wSubCmdID);
}

//发送数据
bool __cdecl CChannelService::SendData(WORD wMainCmdID, WORD wSubCmdID, void * pBuffer, WORD wDataSize)
{
	if (m_hWndServer==NULL) return false;
	CIPCSendCopyData IPCSendCopyData(m_hWndServer,m_hWnd);
	return IPCSendCopyData.SendData(wMainCmdID,wSubCmdID,pBuffer,wDataSize);
}

//信道数据处理
bool __cdecl CChannelService::OnChannelMessage(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend)
{
	//效验数据
	ASSERT(wDataSize<=IPC_PACKAGE);
	if (wDataSize>IPC_PACKAGE) return false;
	if (hWndSend!=m_hWndServer) return false;

	//构造数据
	tagIPCPacket IPCPacket;
	IPCPacket.hWndSend=hWndSend;
	CopyMemory(&IPCPacket.Head,pHead,sizeof(IPC_Head));
	if (wDataSize>0)
	{
		ASSERT(pIPCBuffer!=NULL);
		CopyMemory(IPCPacket.cbBuffer,pIPCBuffer,wDataSize);
	}

	//插入数据
	WORD wAddSize=IPCPacketSize(IPCPacket)+wDataSize;
	m_DataStorage.InsertData(IDI_IPC_DATA,&IPCPacket,wAddSize);

	//投递消息
	PostMessage(WM_IPC_MESSAGE,(WPARAM)wDataSize,(LPARAM)wDataSize);

	return true;
}

//IPC 消息
LRESULT CChannelService::OnIPCMessage(WPARAM wParam, LPARAM lParam)
{
	//提取数据
	tagDataHead DataHead;
	tagIPCPacket IPCPacket;
	bool bSuccess=m_DataStorage.DistillData(DataHead,&IPCPacket,sizeof(IPCPacket));

	//效验数据
	ASSERT(bSuccess==true);
	ASSERT(DataHead.wDataSize>=IPCPacketSize(IPCPacket));
	ASSERT(DataHead.wDataSize==IPCPacket.Head.wDataSize+IPCPacketSize(IPCPacket));
	if (DataHead.wDataSize<IPCPacketSize(IPCPacket)) return 0;
	if (DataHead.wDataSize!=IPCPacket.Head.wDataSize+IPCPacketSize(IPCPacket)) return 0;

	//处理数据
	try
	{
		if (IPCPacket.Head.wMainCmdID==IPC_MIAN_IPC_KERNEL)	
		{
			//内核消息
			switch (IPCPacket.Head.wSubCmdID)
			{
			case IPC_SUB_IPC_CLIENT_CONNECT:	//连接消息
				{
					//处理通知
					if (m_pIChannelEventSink!=NULL) m_pIChannelEventSink->OnChannelConnect(IPCPacket.hWndSend);

					//发送通知
					CIPCSendCopyData SendCopyData;
					SendCopyData.SetClientHwnd(m_hWnd);
					SendCopyData.SetServerHwnd(IPCPacket.hWndSend);
					SendCopyData.SendData(IPC_MIAN_IPC_KERNEL,IPC_SUB_IPC_SERVER_ACCEPT);

					break;
				}
			case IPC_SUB_IPC_SERVER_ACCEPT:		//应答消息
				{
					//处理通知
					if (m_pIChannelEventSink!=NULL)	m_pIChannelEventSink->OnChannelAccept(IPCPacket.hWndSend);

					break;
				}
			case IPC_SUB_IPC_CLIENT_CLOSE:		//关闭消息
				{
					//关闭连接
					CloseChannel(false,true);

					break;
				}
			default: ASSERT(0);
			}
		}
		else	//外围消息
		{
			if (m_pIChannelMessageSink!=NULL)
			{
				WORD wDataSize=IPCPacket.Head.wDataSize;
				m_pIChannelMessageSink->OnChannelMessage(&IPCPacket.Head,IPCPacket.cbBuffer,wDataSize,IPCPacket.hWndSend);
			}
		}
	}
	catch (...)	{ }

	return 0;
}

//////////////////////////////////////////////////////////////////////////

//建立对象函数
extern "C" __declspec(dllexport) void * __cdecl CreateChannelService(const GUID & Guid, DWORD dwInterfaceVer)
{
	//建立对象
	CChannelService * pChannelService=NULL;
	try
	{
		pChannelService=new CChannelService();
		if (pChannelService==NULL) throw TEXT("创建失败");
		void * pObject=pChannelService->QueryInterface(Guid,dwInterfaceVer);
		if (pObject==NULL) throw TEXT("接口查询失败");
		return pObject;
	}
	catch (...) {}

	//清理对象
	SafeDelete(pChannelService);
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
