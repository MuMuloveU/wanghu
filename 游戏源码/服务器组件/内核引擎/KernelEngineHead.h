#ifndef KERNEL_ENGINE_HEAD_HEAD_FILE
#define KERNEL_ENGINE_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//包含文件

#include <Afxmt.h>
#include <Comutil.h>
#include <ICrsint.h>
#include <Process.h>
#include <WinSock2.h>

//平台文件
#include "..\..\模板库\Template.h"
#include "..\..\公共文件\Constant.h"
#include "..\..\公共文件\GlobalDef.h"
#include "..\..\共享组件\公共服务\ComService.h"

//////////////////////////////////////////////////////////////////////////
//ADO 定义

#import "MSADO15.DLL" rename_namespace("ADOCG") rename("EOF","EndOfFile")
using namespace ADOCG;

typedef _com_error						CComError;						//COM 错误
typedef _variant_t						CDBVarValue;					//数据库数值

//////////////////////////////////////////////////////////////////////////
//导出定义

//导出定义
#ifndef KERNEL_ENGINE_CLASS
	#ifdef  KERNEL_ENGINE_DLL
		#define KERNEL_ENGINE_CLASS _declspec(dllexport)
	#else
		#define KERNEL_ENGINE_CLASS _declspec(dllimport)
	#endif
#endif

//模块定义
#ifndef _DEBUG
	#define KERNEL_ENGINE_DLL_NAME	TEXT("KernelEngine.dll")			//组件 DLL 名字
#else
	#define KERNEL_ENGINE_DLL_NAME	TEXT("KernelEngineD.dll")			//组件 DLL 名字
#endif

//////////////////////////////////////////////////////////////////////////
//系统常量

//常量定义
#define TIME_CELL					200									//时间单元
#define TIMES_INFINITY				DWORD(-1)							//无限次数
#define MAX_ASYNCHRONISM_DATA		8192								//异步数据

//////////////////////////////////////////////////////////////////////////
//网络定义

//连接错误
#define CONNECT_SUCCESS				0									//连接成功
#define CONNECT_FAILURE				1									//连接失败
#define CONNECT_EXCEPTION			2									//参数异常

//关闭原因
#define SHUT_REASON_INSIDE			0									//内部原因
#define SHUT_REASON_NORMAL			1									//正常关闭
#define SHUT_REASON_REMOTE			2									//远程关闭
#define SHUT_REASON_TIME_OUT		3									//网络超时
#define SHUT_REASON_EXCEPTION		4									//异常关闭

//////////////////////////////////////////////////////////////////////////
//枚举定义

//输出等级
enum enTraceLevel
{
	TraceLevel_Info					=0,									//信息消息
	TraceLevel_Normal				=1,									//普通消息
	TraceLevel_Warning				=2,									//警告消息
	TraceLevel_Exception			=3,									//异常消息
	TraceLevel_Debug				=4,									//调试消息
};

//SQL 异常类型
enum enSQLException
{
	SQLException_None				=0,									//没有异常
	SQLException_Connect			=1,									//连接错误
	SQLException_Syntax				=2,									//语法错误
};

//连接状态
enum enTCPSocketStatus
{
	TCPSocketStatus_Idle,			//空闲状态
	TCPSocketStatus_Connecting,		//连接状态
	TCPSocketStatus_Connected,		//成功连接
};

//////////////////////////////////////////////////////////////////////////
//事件定义

//事件标识
#define EVENT_TIMER					0x0001								//时间事件
#define EVENT_CONTROL				0x0002								//控制事件
#define EVENT_DATABASE				0x0003								//数据库事件

//网络事件
#define EVENT_TCP_SOCKET_READ		0x0004								//读取事件
#define EVENT_TCP_SOCKET_CLOSE		0x0005								//关闭事件
#define EVENT_TCP_SOCKET_CONNECT	0x0006								//连接事件

//网络事件
#define EVENT_TCP_NETWORK_ACCEPT	0x0007								//应答事件
#define EVENT_TCP_NETWORK_READ		0x0008								//读取事件
#define EVENT_TCP_NETWORK_CLOSE		0x0009								//关闭事件

//事件掩码
#define EVENT_MASK_KERNEL			0x00FF								//内核事件
#define EVENT_MASK_CUSTOM			0xFF00								//自定义事件

//////////////////////////////////////////////////////////////////////////

//控制事件
struct NTY_ControlEvent
{
	WORD							wControlID;							//控制标识
};

//定时器事件
struct NTY_TimerEvent
{
	DWORD							dwTimerID;							//时间标识
	WPARAM							dwBindParameter;					//绑定参数
};

//数据库事件
struct NTY_DataBaseEvent
{
	WORD							wRequestID;							//请求标识
	DWORD							dwContextID;						//对象标识
};

//读取事件
struct NTY_TCPSocketReadEvent
{
	WORD							wDataSize;							//数据大小
	WORD							wServiceID;							//服务标识
	CMD_Command						Command;							//命令信息
};

//关闭事件
struct NTY_TCPSocketCloseEvent
{
	WORD							wServiceID;							//服务标识
	BYTE							cbShutReason;						//关闭原因
};

//连接事件
struct NTY_TCPSocketConnectEvent
{
	INT								nErrorCode;							//错误代码
	WORD							wServiceID;							//服务标识
};

//应答事件
struct NTY_TCPNetworkAcceptEvent
{
	DWORD							dwSocketID;							//网络标识
	DWORD							dwClientIP;							//连接地址
};

//读取事件
struct NTY_TCPNetworkReadEvent
{
	WORD							wDataSize;							//数据大小
	DWORD							dwSocketID;							//网络标识
	CMD_Command						Command;							//命令信息
};

//关闭事件
struct NTY_TCPNetworkCloseEvent
{
	DWORD							dwSocketID;							//网络标识
	DWORD							dwClientIP;							//连接地址
	DWORD							dwActiveTime;						//连接时间
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IDataBaseException INTERFACE_VERSION(1,1)
	static const GUID IID_IDataBaseException={0x008be9d3,0x2305,0x40da,0x00ae,0xd1,0x61,0x7a,0xd2,0x2a,0x47,0xfc};
#else
	#define VER_IDataBaseException INTERFACE_VERSION(1,1)
	static const GUID IID_IDataBaseException={0x428361ed,0x9dfa,0x43d7,0x008f,0x26,0x17,0x06,0x47,0x6b,0x2a,0x51};
#endif

//数据库异常
interface IDataBaseException : public IUnknownEx
{
	//异常代码
	virtual HRESULT __cdecl GetExceptionResult()=NULL;
	//异常描述
	virtual LPCTSTR __cdecl GetExceptionDescribe()=NULL;
	//异常类型
	virtual enSQLException __cdecl GetExceptionType()=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_ITraceService INTERFACE_VERSION(1,1)
	static const GUID IID_ITraceService={0xe4096162,0x8134,0x4d2c,0x00b6,0x4f,0x09,0x5d,0xcc,0xca,0xe0,0x81};
#else
	#define VER_ITraceService INTERFACE_VERSION(1,1)
	static const GUID IID_ITraceService={0xe5f636c6,0xabb5,0x4752,0x00bb,0xc8,0xcd,0xb1,0x76,0x58,0xf5,0x2d};
#endif

//事件输出
interface ITraceService : public IUnknownEx
{
	//字串输出
	virtual bool __cdecl TraceString(LPCTSTR pszString, enTraceLevel TraceLevel)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_ITraceServiceManager INTERFACE_VERSION(1,1)
	static const GUID IID_ITraceServiceManager={0x6d14efe6,0x892a,0x4a48,0x0092,0xc9,0xdb,0xea,0x92,0xdd,0xd5,0x13};
#else
	#define VER_ITraceServiceManager INTERFACE_VERSION(1,1)
	static const GUID IID_ITraceServiceManager={0x8bfc36db,0x5ba2,0x42ba,0x0081,0xb0,0x87,0xb0,0x1c,0x9e,0xaf,0xfe};
#endif

//输出管理
interface ITraceServiceManager : public IUnknownEx
{
	//状态管理
public:
	//追踪状态
	virtual bool __cdecl IsEnableTrace(enTraceLevel TraceLevel)=NULL;
	//追踪控制
	virtual bool __cdecl EnableTrace(enTraceLevel TraceLevel, bool bEnableTrace)=NULL;

	//服务配置
public:
	//设置接口
	virtual bool __cdecl SetTraceService(IUnknownEx * pIUnknownEx)=NULL;
	//获取接口
	virtual VOID * __cdecl GetTraceService(REFGUID Guid, DWORD dwQueryVer)=NULL;

	//服务接口
public:
	//字串输出
	virtual bool __cdecl TraceString(LPCTSTR pszString, enTraceLevel TraceLevel)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IServiceModule INTERFACE_VERSION(1,1)
	static const GUID IID_IServiceModule={0x49084dea,0x4420,0x4bea,0x0080,0x64,0xfa,0x37,0xe3,0x42,0xf3,0x1c};
#else
	#define VER_IServiceModule INTERFACE_VERSION(1,1)
	static const GUID IID_IServiceModule={0x05980504,0xa2f2,0x4b0f,0x009b,0x54,0x51,0x54,0x1e,0x05,0x5c,0xff};
#endif

//服务模块
interface IServiceModule : public IUnknownEx
{
	//启动服务
	virtual bool __cdecl StartService()=NULL;
	//停止服务
	virtual bool __cdecl ConcludeService()=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IAsynchronismEngine INTERFACE_VERSION(1,1)
	static const GUID IID_IAsynchronismEngine={0xd64208cf,0xdae9,0x45e5,0x0089,0xd1,0x07,0xdd,0x83,0x13,0xa1,0xd0};
#else
	#define VER_IAsynchronismEngine INTERFACE_VERSION(1,1)
	static const GUID IID_IAsynchronismEngine={0xdc8132ee,0x146e,0x4ace,0x008f,0xc1,0x9c,0x65,0x2a,0x88,0x35,0x93};
#endif

//异步引擎
interface IAsynchronismEngine : public IServiceModule
{
	//设置模块
	virtual bool __cdecl SetAsynchronismSink(IUnknownEx * pIUnknownEx)=NULL;
	//投递数据
	virtual bool __cdecl PostAsynchronismData(WORD wIdentifier, VOID * pData, WORD wDataSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IAsynchronismEngineSink INTERFACE_VERSION(1,1)
	static const GUID IID_IAsynchronismEngineSink={0x55215681,0x858a,0x46f6,0x0084,0xec,0x84,0x9e,0xc8,0x7d,0x82,0x35};
#else
	#define VER_IAsynchronismEngineSink INTERFACE_VERSION(1,1)
	static const GUID IID_IAsynchronismEngineSink={0x2edf5c9e,0x2cac,0x461d,0x00a7,0x82,0x2e,0x2f,0xe1,0x91,0x80,0xf8};
#endif

//异步钩子
interface IAsynchronismEngineSink : public IUnknownEx
{
	//启动事件
	virtual bool __cdecl OnAsynchronismEngineStart()=NULL;
	//停止事件
	virtual bool __cdecl OnAsynchronismEngineStop()=NULL;
	//异步数据
	virtual bool __cdecl OnAsynchronismEngineData(WORD wIdentifier, VOID * pData, WORD wDataSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IDataBase INTERFACE_VERSION(1,1)
	static const GUID IID_IDataBase={0x7fbbbffa,0xedf4,0x43d2,0x008a,0xb7,0x28,0x87,0x3b,0xd0,0xf5,0x3f};
#else
	#define VER_IDataBase INTERFACE_VERSION(1,1)
	static const GUID IID_IDataBase={0xa2e38a78,0x1e4f,0x4de4,0x00a5,0xd1,0xb9,0x19,0x9b,0xce,0x41,0xae};
#endif

//数据库接口
interface IDataBase : public IUnknownEx
{
	//连接接口
public:
	//打开连接
	virtual VOID __cdecl OpenConnection()=NULL;
	//关闭连接
	virtual VOID __cdecl CloseConnection()=NULL;
	//连接信息
	virtual bool __cdecl SetConnectionInfo(DWORD dwDBAddr, WORD wPort, LPCTSTR szDBName, LPCTSTR szUser, LPCTSTR szPassword)=NULL;
	//连接信息
	virtual bool __cdecl SetConnectionInfo(LPCTSTR szDBAddr, WORD wPort, LPCTSTR szDBName, LPCTSTR szUser, LPCTSTR szPassword)=NULL;

	//参数接口
public:
	//清除参数
	virtual VOID __cdecl ClearParameters()=NULL;
	//获取参数
	virtual VOID __cdecl GetParameter(LPCTSTR pszParamName, CDBVarValue & DBVarValue)=NULL;
	//插入参数
	virtual VOID __cdecl AddParameter(LPCTSTR pszName, DataTypeEnum Type, ParameterDirectionEnum Direction, LONG lSize, CDBVarValue & DBVarValue)=NULL;

	//控制接口
public:
	//切换记录
	virtual VOID __cdecl NextRecordset()=NULL;
	//关闭记录
	virtual VOID __cdecl CloseRecordset()=NULL;
	//绑定对象
	virtual VOID __cdecl BindToRecordset(CADORecordBinding * pBind)=NULL;

	//记录接口
public:
	//往下移动
	virtual VOID __cdecl MoveToNext()=NULL;
	//移到开头
	virtual VOID __cdecl MoveToFirst()=NULL;
	//是否结束
	virtual bool __cdecl IsRecordsetEnd()=NULL;
	//获取数目
	virtual LONG __cdecl GetRecordCount()=NULL;
	//返回数值
	virtual LONG __cdecl GetReturnValue()=NULL;
	//获取数据
	virtual VOID __cdecl GetRecordsetValue(LPCTSTR pszItem, CDBVarValue & DBVarValue)=NULL;

	//控制接口
public:
	//存储过程
	virtual VOID __cdecl ExecuteProcess(LPCTSTR pszSPName, bool bRecordset)=NULL;
	//执行语句
	virtual VOID __cdecl ExecuteSentence(LPCTSTR pszCommand, bool bRecordset)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IDataBaseEngine INTERFACE_VERSION(1,1)
	static const GUID IID_IDataBaseEngine={0x886e92ac,0x555c,0x4e8f,0x009f,0xb5,0xcb,0xee,0x6f,0x61,0xe8,0x12};
#else
	#define VER_IDataBaseEngine INTERFACE_VERSION(1,1)
	static const GUID IID_IDataBaseEngine={0xe9cb190f,0x3c36,0x4182,0x0094,0x2a,0xab,0xa7,0x35,0x26,0x2d,0xea};
#endif

//数据库引擎
interface IDataBaseEngine : public IServiceModule
{
	//配置接口
public:
	//配置模块
	virtual bool __cdecl SetDataBaseEngineSink(IUnknownEx * pIUnknownEx)=NULL;

	//控制事件
public:
	//时间事件
	virtual bool __cdecl PostDataBaseTimer(DWORD dwTimerID, WPARAM dwBindParameter)=NULL;
	//控制事件
	virtual bool __cdecl PostDataBaseControl(WORD wControlID, VOID * pData, WORD wDataSize)=NULL;
	//请求事件
	virtual bool __cdecl PostDataBaseRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IDataBaseEngineSink INTERFACE_VERSION(1,1)
	static const GUID IID_IDataBaseEngineSink={0x295c258b,0xfad0,0x41ac,0x0099,0x19,0xc5,0x64,0x54,0xec,0x3a,0xf9};
#else
	#define VER_IDataBaseEngineSink INTERFACE_VERSION(1,1)
	static const GUID IID_IDataBaseEngineSink={0x0ed26ed6,0x69d7,0x4f5b,0x00b0,0xca,0x17,0xae,0xab,0xba,0x06,0xdf};
#endif

//数据库钩子
interface IDataBaseEngineSink : public IUnknownEx
{
	//系统事件
public:
	//启动事件
	virtual bool __cdecl OnDataBaseEngineStart(IUnknownEx * pIUnknownEx)=NULL;
	//停止事件
	virtual bool __cdecl OnDataBaseEngineStop(IUnknownEx * pIUnknownEx)=NULL;

	//内核事件
public:
	//时间事件
	virtual bool __cdecl OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter)=NULL;
	//控制事件
	virtual bool __cdecl OnDataBaseEngineControl(WORD wControlID, VOID * pData, WORD wDataSize)=NULL;
	//请求事件
	virtual bool __cdecl OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_ITCPNetworkEngine INTERFACE_VERSION(1,1)
	static const GUID IID_ITCPNetworkEngine={0xfea410c2,0x2a86,0x4af0,0x009e,0x7f,0x55,0xa4,0x1c,0x0b,0xdd,0xc6};
#else
	#define VER_ITCPNetworkEngine INTERFACE_VERSION(1,1)
	static const GUID IID_ITCPNetworkEngine={0x0f1d9f08,0xa572,0x4cc4,0x009b,0xca,0x16,0x4a,0xf1,0xd5,0x9d,0x06};
#endif

//网络引擎
interface ITCPNetworkEngine : public IServiceModule
{
	//配置接口
public:
	//设置接口
	virtual bool __cdecl SetTCPNetworkEngineEvent(IUnknownEx * pIUnknownEx)=NULL;
	//设置参数
	virtual bool __cdecl SetServiceParameter(WORD wServicePort, WORD wMaxConnect)=NULL;
	
	//发送接口
public:
	//发送函数
	virtual bool __cdecl SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID)=NULL;
	//发送函数
	virtual bool __cdecl SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;
	//批量发送
	virtual bool __cdecl SendDataBatch(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;

	//控制接口
public:
	//关闭连接
	virtual bool __cdecl CloseSocket(DWORD dwSocketID)=NULL;
	//设置关闭
	virtual bool __cdecl ShutDownSocket(DWORD dwSocketID)=NULL;
	//允许群发
	virtual bool __cdecl AllowBatchSend(DWORD dwSocketID, bool bAllowBatch)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_ITCPSocketService INTERFACE_VERSION(1,1)
	static const GUID IID_ITCPSocketService={0x0f8a5c14,0xab92,0x467c,0xb6,0x7b,0x6d,0x8a,0xcf,0x64,0x52,0xd7};
#else
	#define VER_ITCPSocketService INTERFACE_VERSION(1,1)
	static const GUID IID_ITCPSocketService={0x709a4449,0xad77,0x4b3d,0xb4,0xd6,0x8d,0x0b,0x28,0x65,0xec,0xae};
#endif

//网络接口
interface ITCPSocketService : public IServiceModule
{
	//配置接口
public:
	//配置函数
	virtual bool __cdecl SetServiceID(WORD wServiceID)=NULL;
	//设置接口
	virtual bool __cdecl SetTCPSocketEvent(IUnknownEx * pIUnknownEx)=NULL;

	//功能接口
public:
	//关闭连接
	virtual bool __cdecl CloseSocket()=NULL;
	//连接地址
	virtual bool __cdecl Connect(DWORD dwServerIP, WORD wPort)=NULL;
	//连接地址
	virtual bool __cdecl Connect(LPCTSTR szServerIP, WORD wPort)=NULL;
	//发送函数
	virtual bool __cdecl SendData(WORD wMainCmdID, WORD wSubCmdID)=NULL;
	//发送函数
	virtual bool __cdecl SendData(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_ITimerEngine INTERFACE_VERSION(1,1)
	static const GUID IID_ITimerEngine={0xc90cfc81,0xee01,0x4654,0x008c,0x9a,0x58,0xa9,0x4b,0x78,0x76,0xb6};
#else
	#define VER_ITimerEngine INTERFACE_VERSION(1,1)
	static const GUID IID_ITimerEngine={0x496401ae,0x6fb0,0x4e9f,0x0090,0x98,0x44,0x9d,0x9c,0xb2,0xbd,0x97};
#endif

//定时器引擎
interface ITimerEngine : public IServiceModule
{
	//配置接口
public:
	//设置接口
	virtual bool __cdecl SetTimerEngineEvent(IUnknownEx * pIUnknownEx)=NULL;

	//功能接口
public:
	//设置定时器
	virtual bool __cdecl SetTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter)=NULL; 
	//删除定时器
	virtual bool __cdecl KillTimer(DWORD dwTimerID)=NULL;
	//删除定时器
	virtual bool __cdecl KillAllTimer()=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_ITimerEngineEvent INTERFACE_VERSION(1,1)
	static const GUID IID_ITimerEngineEvent={0x995c4868,0x81b8,0x4c52,0x00a2,0x15,0x71,0x97,0x0d,0x16,0xaf,0xb7};
#else
	#define VER_ITimerEngineEvent INTERFACE_VERSION(1,1)
	static const GUID IID_ITimerEngineEvent={0xeb78a125,0x62fc,0x4811,0x00b6,0xf2,0x59,0x26,0x88,0x04,0xc3,0x02};
#endif

//定时器事件
interface ITimerEngineEvent : public IUnknownEx
{
	//接口定义
public:
	//定时器事件
	virtual bool __cdecl OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IDataBaseEngineEvent INTERFACE_VERSION(1,1)
	static const GUID IID_IDataBaseEngineEvent={0x344ea7f2,0x633b,0x4580,0x0080,0xa9,0xa9,0x4a,0x46,0x12,0xce,0x73};
#else
	#define VER_IDataBaseEngineEvent INTERFACE_VERSION(1,1)
	static const GUID IID_IDataBaseEngineEvent={0xc29c7131,0xe84b,0x4553,0x00a8,0x38,0x12,0xee,0x07,0xdd,0x0e,0xa3};
#endif

//数据库事件
interface IDataBaseEngineEvent : public IUnknownEx
{
	//接口定义
public:
	//数据库结果
	virtual bool __cdecl OnEventDataBaseResult(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_ITCPSocketEvent INTERFACE_VERSION(1,1)
	static const GUID IID_ITCPSocketEvent={0x0400c2de,0x69b1,0x4136,0x00af,0x30,0x2d,0x2d,0x1e,0x35,0x51,0x24};
#else
	#define VER_ITCPSocketEvent INTERFACE_VERSION(1,1)
	static const GUID IID_ITCPSocketEvent={0x6f5bdb91,0xf72a,0x425d,0x0087,0x03,0x39,0xbc,0xf7,0x1e,0x0b,0x03};
#endif

//网络事件
interface ITCPSocketEvent : public IUnknownEx
{
	//连接事件
	virtual bool __cdecl OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode)=NULL;
	//关闭事件
	virtual bool __cdecl OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason)=NULL;
	//读取事件
	virtual bool __cdecl OnEventTCPSocketRead(WORD wServiceID, CMD_Command Command, VOID * pData, WORD wDataSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_ITCPNetworkEngineEvent INTERFACE_VERSION(1,1)
	static const GUID IID_ITCPNetworkEngineEvent={0x9759ffb3,0x5bec,0x4199,0x0091,0xef,0x49,0x5b,0xca,0xdc,0x00,0x98};
#else
	#define VER_ITCPNetworkEngineEvent INTERFACE_VERSION(1,1)
	static const GUID IID_ITCPNetworkEngineEvent={0xb7e6da53,0xfca5,0x4d90,0x0085,0x48,0xfe,0x05,0xf6,0xb4,0xc0,0xef};
#endif

//网络事件
interface ITCPNetworkEngineEvent : public IUnknownEx
{
	//接口定义
public:
	//应答事件
	virtual bool __cdecl OnEventTCPNetworkBind(DWORD dwSocketID, DWORD dwClientIP)=NULL;
	//关闭事件
	virtual bool __cdecl OnEventTCPNetworkShut(DWORD dwSocketID, DWORD dwClientIP, DWORD dwActiveTime)=NULL;
	//读取事件
	virtual bool __cdecl OnEventTCPNetworkRead(DWORD dwSocketID, CMD_Command Command, VOID * pData, WORD wDataSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IAttemperEngine INTERFACE_VERSION(1,1)
	static const GUID IID_IAttemperEngine={0x4d5d2424,0x40fd,0x4747,0x86,0xd8,0x8f,0xca,0x6b,0x96,0xea,0x0b};
#else
	#define VER_IAttemperEngine INTERFACE_VERSION(1,1)
	static const GUID IID_IAttemperEngine={0x0b070b2c,0x9d72,0x42d2,0xa5,0x70,0xba,0x2c,0xbf,0x6f,0xbb,0x1c};
#endif

//调度引擎
interface IAttemperEngine : public IServiceModule
{
	//配置接口
public:
	//网络接口
	virtual bool __cdecl SetNetworkEngine(IUnknownEx * pIUnknownEx)=NULL;
	//回调接口
	virtual bool __cdecl SetAttemperEngineSink(IUnknownEx * pIUnknownEx)=NULL;

	//控制事件
public:
	//自定事件
	virtual bool __cdecl OnEventCustom(WORD wRequestID, VOID * pData, WORD wDataSize)=NULL;
	//控制事件
	virtual bool __cdecl OnEventControl(WORD wControlID, VOID * pData, WORD wDataSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IAttemperEngineSink INTERFACE_VERSION(1,1)
	static const GUID IID_IAttemperEngineSink={0x133d1f30,0x54ce,0x4360,0x0084,0x50,0x87,0x29,0xe0,0x95,0xaa,0xbb};
#else
	#define VER_IAttemperEngineSink INTERFACE_VERSION(1,1)
	static const GUID IID_IAttemperEngineSink={0x831b9001,0x4450,0x45dd,0x0091,0x37,0x0d,0x26,0x16,0xe3,0x75,0x32};
#endif

//调度钩子
interface IAttemperEngineSink : public IUnknownEx
{
	//异步接口
public:
	//启动事件
	virtual bool __cdecl OnAttemperEngineStart(IUnknownEx * pIUnknownEx)=NULL;
	//停止事件
	virtual bool __cdecl OnAttemperEngineStop(IUnknownEx * pIUnknownEx)=NULL;

	//事件接口
public:
	//控制事件
	virtual bool __cdecl OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize)=NULL;
	//自定事件
	virtual bool __cdecl OnEventAttemperData(WORD wRequestID, VOID * pData, WORD wDataSize)=NULL;

	//内核事件
public:
	//时间事件
	virtual bool __cdecl OnEventTimer(DWORD dwTimerID, WPARAM wBindParam)=NULL;
	//数据库事件
	virtual bool __cdecl OnEventDataBase(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)=NULL;

	//连接事件
public:
	//连接事件
	virtual bool __cdecl OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode)=NULL;
	//关闭事件
	virtual bool __cdecl OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason)=NULL;
	//读取事件
	virtual bool __cdecl OnEventTCPSocketRead(WORD wServiceID, CMD_Command Command, VOID * pData, WORD wDataSize)=NULL;

	//网络事件
public:
	//应答事件
	virtual bool __cdecl OnEventTCPNetworkBind(DWORD dwClientIP, DWORD dwSocketID)=NULL;
	//关闭事件
	virtual bool __cdecl OnEventTCPNetworkShut(DWORD dwClientIP, DWORD dwActiveTime, DWORD dwSocketID)=NULL;
	//读取事件
	virtual bool __cdecl OnEventTCPNetworkRead(CMD_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)=NULL;
};

//////////////////////////////////////////////////////////////////////////
//组件辅助类

DECLARE_MODULE_HELPER(DataBase,KERNEL_ENGINE_DLL_NAME,"CreateDataBase")
DECLARE_MODULE_HELPER(TimerEngine,KERNEL_ENGINE_DLL_NAME,"CreateTimerEngine")
DECLARE_MODULE_HELPER(DataBaseEngine,KERNEL_ENGINE_DLL_NAME,"CreateDataBaseEngine")
DECLARE_MODULE_HELPER(AttemperEngine,KERNEL_ENGINE_DLL_NAME,"CreateAttemperEngine")
DECLARE_MODULE_HELPER(TCPSocketService,KERNEL_ENGINE_DLL_NAME,"CreateTCPSocketService")
DECLARE_MODULE_HELPER(TCPNetworkEngine,KERNEL_ENGINE_DLL_NAME,"CreateTCPNetworkEngine")
DECLARE_MODULE_HELPER(AsynchronismEngine,KERNEL_ENGINE_DLL_NAME,"CreateAsynchronismEngine")
DECLARE_MODULE_HELPER(TraceServiceManager,KERNEL_ENGINE_DLL_NAME,"CreateTraceServiceManager")

//////////////////////////////////////////////////////////////////////////

//导出文件
#include "DataBaseAide.h"
#include "TraceService.h"

//////////////////////////////////////////////////////////////////////////

#endif