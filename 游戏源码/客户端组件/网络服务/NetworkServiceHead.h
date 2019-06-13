#ifndef NETWORK_SERVICE_HEAD_HEAD_FILE
#define NETWORK_SERVICE_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//平台文件
#include "..\..\模板库\Template.h"
#include "..\..\公共文件\Constant.h"
#include "..\..\公共文件\GlobalDef.h"

//////////////////////////////////////////////////////////////////////////
//公共定义

//导出定义
#ifndef NETWORK_SERVICE_CLASS
	#ifdef  NETWORK_SERVICE_DLL
		#define NETWORK_SERVICE_CLASS _declspec(dllexport)
	#else
		#define NETWORK_SERVICE_CLASS _declspec(dllimport)
	#endif
#endif

//模块定义
#ifndef _DEBUG
	#define NETWORK_SERVICE_DLL_NAME	TEXT("NetworkService.dll")		//组件 DLL 名字
#else
	#define NETWORK_SERVICE_DLL_NAME	TEXT("NetworkServiceD.dll")		//组件 DLL 名字
#endif

//////////////////////////////////////////////////////////////////////////
//公共定义

//关闭原因
#define SHUT_REASON_INSIDE			0									//内部原因
#define SHUT_REASON_NORMAL			1									//正常关闭
#define SHUT_REASON_REMOTE			2									//远程关闭
#define SHUT_REASON_TIME_OUT		3									//网络超时
#define SHUT_REASON_EXCEPTION		4									//异常关闭

//////////////////////////////////////////////////////////////////////////
//公共定义

//网络状态
#define SOCKET_STATUS_IDLE			0									//空闲状态
#define SOCKET_STATUS_WAIT			1									//等待状态
#define SOCKET_STATUS_CONNECT		2									//连接状态

//连接错误
#define CONNECT_SUCCESS				0									//连接成功
#define CONNECT_FAILURE				1									//连接失败
#define CONNECT_EXCEPTION			2									//参数异常
#define CONNECT_PROXY_FAILURE		3									//连接成功
#define CONNECT_PROXY_USER_INVALID	4									//用户无效

//////////////////////////////////////////////////////////////////////////
//枚举定义

//代理类型
enum enProxyServerType
{
	ProxyType_None,					//没有代理
	ProxyType_Http,					//HTTP 代理
	ProxyType_Socks4,				//SOCKS4 代理
	ProxyType_Socks5,				//SOCKS5 代理
};

//////////////////////////////////////////////////////////////////////////
//结构定义

//代理信息
struct tagProxyServerInfo
{
	WORD							wProxyPort;							//代理端口
	TCHAR							szUserName[32];						//代理用户
	TCHAR							szPassword[32];						//代理密码
	TCHAR							szHttpDomain[32];					//代理用户
	TCHAR							szProxyServer[32];					//代理地址
};

//代理描述
struct tagProxyTypeDescribe
{
	TCHAR							szDescribe[32];						//描述字符
	enProxyServerType				ProxyServerType;					//代理类型
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_ITCPSocket INTERFACE_VERSION(1,1)
	static const GUID IID_ITCPSocket={0x91446684,0x621c,0x4546,0x86,0x59,0x22,0xa9,0x5a,0x7c,0x20,0xb9};
#else
	#define VER_ITCPSocket INTERFACE_VERSION(1,1)
	static const GUID IID_ITCPSocket={0xc8a0b91a,0xcb6f,0x4749,0xa3,0x44,0xa7,0x06,0x32,0x11,0xbf,0x79};
#endif

//网络接口
interface ITCPSocket : public IUnknownEx
{
	//标识接口
public:
	//获取标识
	virtual WORD __cdecl GetSocketID()=NULL;
	//设置标识
	virtual VOID __cdecl SetSocketID(WORD wSocketID)=NULL;

	//配置接口
public:
	//设置接口
	virtual bool __cdecl SetTCPSocketSink(IUnknownEx * pIUnknownEx)=NULL;
	//获取接口
	virtual VOID * __cdecl GetTCPSocketSink(const IID & Guid, DWORD dwQueryVer)=NULL;

	//信息接口
public:
	//网络状态
	virtual BYTE __cdecl GetSocketStatus()=NULL;
	//发送间隔
	virtual DWORD __cdecl GetLastSendTick()=NULL;
	//接收间隔
	virtual DWORD __cdecl GetLastRecvTick()=NULL;
	//发送数目
	virtual DWORD __cdecl GetSendPacketCount()=NULL;
	//接收数目
	virtual DWORD __cdecl GetRecvPacketCount()=NULL;

	//代理接口
public:
	//代理测试
	virtual DWORD __cdecl ProxyServerTesting()=NULL;
	//设置代理
	virtual bool __cdecl SetProxyServerInfo(enProxyServerType ProxyServerType, const tagProxyServerInfo & ProxyServerInfo)=NULL;

	//操作接口
public:
	//关闭连接
	virtual VOID __cdecl CloseSocket()=NULL;
	//连接操作
	virtual DWORD __cdecl Connect(DWORD dwServerIP, WORD wPort)=NULL;
	//连接操作
	virtual DWORD __cdecl Connect(LPCTSTR szServerIP, WORD wPort)=NULL;
	//发送函数
	virtual DWORD __cdecl SendData(WORD wMainCmdID, WORD wSubCmdID)=NULL;
	//发送函数
	virtual DWORD __cdecl SendData(WORD wMainCmdID, WORD wSubCmdID, VOID * const pData, WORD wDataSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_ITCPSocketSink INTERFACE_VERSION(1,1)
	static const GUID IID_ITCPSocketSink={0xdb0f09dd,0x65e4,0x441f,0x0094,0xb1,0xfe,0x80,0x8c,0x1d,0xf2,0x65};
#else
	#define VER_ITCPSocketSink INTERFACE_VERSION(1,1)
	static const GUID IID_ITCPSocketSink={0x231416e8,0x74b0,0x472d,0x009e,0xc3,0x4a,0x22,0xe5,0x21,0x09,0x37};
#endif

//网络接口
interface ITCPSocketSink : public IUnknownEx
{
	//网络事件
public:
	//连接事件
	virtual bool __cdecl OnEventTCPSocketLink(WORD wSocketID, INT nErrorCode)=NULL;
	//关闭事件
	virtual bool __cdecl OnEventTCPSocketShut(WORD wSocketID, BYTE cbShutReason)=NULL;
	//读取事件
	virtual bool __cdecl OnEventTCPSocketRead(WORD wSocketID, CMD_Command Command, VOID * pData, WORD wDataSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_INetworkManager INTERFACE_VERSION(1,1)
	static const GUID IID_INetworkManager={0x701dfa0e,0x3e66,0x44b7,0x00ae,0x7c,0xf4,0x5e,0x59,0x95,0xfb,0x50};
#else
	#define VER_INetworkManager INTERFACE_VERSION(1,1)
	static const GUID IID_INetworkManager={0xcb58785e,0x7657,0x4d6b,0x0081,0xc0,0x2a,0x82,0x25,0x48,0xf4,0x96};
#endif

//网络管理接口
interface INetworkManager : public IUnknownEx
{
	//枚举代理
	virtual bool __cdecl EnumProxyServerType(WORD wIndex, tagProxyTypeDescribe & ProxyTypeDescribe)=NULL;
	//代理测试
	virtual DWORD __cdecl ProxyServerTesting(enProxyServerType ProxyServerType, const tagProxyServerInfo & ProxyServerInfo)=NULL;
};

//////////////////////////////////////////////////////////////////////////
//组件辅助类

DECLARE_MODULE_HELPER(TCPSocket,NETWORK_SERVICE_DLL_NAME,"CreateTCPSocket")
DECLARE_MODULE_HELPER(NetworkManager,NETWORK_SERVICE_DLL_NAME,"CreateNetworkManager")

//////////////////////////////////////////////////////////////////////////

#endif