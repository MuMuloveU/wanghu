#ifndef CHANNEL_MODULE_HEAD_FILE
#define CHANNEL_MODULE_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//平台头文件
#include "Template.h"
#include "GlobalDef.h"

//组件头文件
#include "ComService.h"

//////////////////////////////////////////////////////////////////////////
//公共宏定义

//导出定义
#ifndef CHANNEL_CLASS
	#ifdef  CHANNEL_DLL
		#define CHANNEL_CLASS _declspec(dllexport)
	#else
		#define CHANNEL_CLASS _declspec(dllimport)
	#endif
#endif

//模块定义
#ifdef _DEBUG
	#define CHANNEL_MODULE_DLL_NAME	TEXT("ChannelModuleD.dll")			//组件 DLL 名字
#else
	#define CHANNEL_MODULE_DLL_NAME	TEXT("ChannelModule.dll")			//组件 DLL 名字
#endif

//内核主命令码
#define IPC_MIAN_IPC_KERNEL					0							//内核命令

//内核子命令码
#define IPC_SUB_IPC_CLIENT_CONNECT			1							//连接通知
#define IPC_SUB_IPC_SERVER_ACCEPT			2							//应答通知
#define IPC_SUB_IPC_CLIENT_CLOSE			3							//关闭通知

//////////////////////////////////////////////////////////////////////////

#define VER_IChannelMessageSink INTERFACE_VERSION(1,1)
static const GUID IID_IChannelMessageSink={0x7a672dca,0x141e,0x45a6,0x95,0xc4,0x43,0x79,0x23,0x7f,0x29,0x73};

//信道消息钩子接口
interface IChannelMessageSink : public IUnknownEx
{
	//信道数据处理
	virtual bool __cdecl OnChannelMessage(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_IChannelEventSink INTERFACE_VERSION(1,1)
static const GUID IID_IChannelEventSink={0xb87334af,0x3f52,0x4e51,0x9d,0xc6,0x51,0x93,0x24,0x66,0x9e,0x2e};

//信道事件钩子接口
interface IChannelEventSink : public IUnknownEx
{
	//连接消息
	virtual bool __cdecl OnChannelConnect(HWND hWndSend)=NULL;
	//应答消息
	virtual bool __cdecl OnChannelAccept(HWND hWndSend)=NULL;
	//断开消息
	virtual bool __cdecl OnChannelClose(HWND hWndSend, bool bInitiative)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_IChannelService INTERFACE_VERSION(1,1)
static const GUID IID_IChannelService={0x4d0151e9,0xbd65,0x4a4c,0xa6,0xf8,0x25,0x74,0x68,0x17,0x6e,0x74};

//信道模块接口
interface IChannelService : public IUnknownEx
{
	//设置事件
	virtual bool __cdecl SetChannelEventSink(IUnknownEx * pIUnknownEx)=NULL;
	//设置消息
	virtual bool __cdecl SetChannelMessageSink(IUnknownEx * pIUnknownEx)=NULL;
	//获取事件
	virtual void * __cdecl GetChannelEventSink(const IID & Guid, DWORD dwQueryVer)=NULL;
	//获取消息
	virtual void * __cdecl GetChannelMessageSink(const IID & Guid, DWORD dwQueryVer)=NULL;
	//是否连接
	virtual bool __cdecl IsConnect(bool bEfficacy)=NULL;
	//创建函数
	virtual bool __cdecl CreateChannel(HWND hWndServer)=NULL;
	//关闭函数
	virtual bool __cdecl CloseChannel(bool bNotifyServer, bool bNotifySink)=NULL;
	//发送数据
	virtual bool __cdecl SendData(WORD wMainCmdID, WORD wSubCmdID)=NULL;
	//发送数据
	virtual bool __cdecl SendData(WORD wMainCmdID, WORD wSubCmdID, void * pBuffer, WORD wDataSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////

//信道对象辅助类
class CChannelServiceHelper : public CTempldateHelper<IChannelService>
{
	//函数定义
public:
	//构造函数
	CChannelServiceHelper(void) : CTempldateHelper<IChannelService>(IID_IChannelService,
		VER_IChannelService,CHANNEL_MODULE_DLL_NAME,TEXT("CreateChannelService"))
	{
	}
};

//////////////////////////////////////////////////////////////////////////

//导出组件文件
#include "IPCControls.h"

//////////////////////////////////////////////////////////////////////////

#endif