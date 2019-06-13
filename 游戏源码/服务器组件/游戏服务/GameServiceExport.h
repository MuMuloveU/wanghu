#ifndef GAME_SERVICE_EXPORT_HEAD_FILE
#define GAME_SERVICE_EXPORT_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//平台头文件
#include "..\..\模板库\Template.h"
#include "..\..\公共文件\GlobalDef.h"
#include "..\..\公共文件\GlobalField.h"
#include "..\..\公共文件\GlobalFrame.h"
#include "..\..\公共文件\GlobalRight.h"
#include "..\..\公共文件\GlobalService.h"

//命令码文件
#include "..\..\消息定义\CMD_Game.h"
#include "..\..\消息定义\CMD_Video.h"
#include "..\..\消息定义\CMD_Plaza.h"
#include "..\..\消息定义\CMD_Center.h"

//组件头文件
#include "..\..\服务器组件\内核引擎\KernelEngineHead.h"
#include "..\..\客户端组件\鲜花道具\PropertyModule.h"

//////////////////////////////////////////////////////////////////////////
//公共宏定义

//导出定义
#ifndef GAME_SERVICE_CLASS
	#ifdef  GAME_SERVICE_DLL
		#define GAME_SERVICE_CLASS _declspec(dllexport)
	#else
		#define GAME_SERVICE_CLASS _declspec(dllimport)
	#endif
#endif

//模块定义
#ifdef _DEBUG
	#define GAME_SERVICE_DLL_NAME	TEXT("GameServiceD.dll")				//组件 DLL 名字
#else
	#define GAME_SERVICE_DLL_NAME	TEXT("GameService.dll")					//组件 DLL 名字
#endif

//////////////////////////////////////////////////////////////////////////

//宏定义
#define IDI_MAX_TIME_ID					30									//极限定时器

//结束原因
#define GER_NORMAL						0x00								//常规结束
#define GER_DISMISS						0x01								//游戏解散
#define GER_USER_LEFT					0x02								//用户离开

//索引定义
#define INDEX_SOCKET					(WORD)(0x0000)						//网络索引
#define INDEX_ANDROID					(WORD)(0x2000)						//机器索引
#define INVALID_WORD_INDEX				(WORD)(0xFFFF)						//无效索引

//////////////////////////////////////////////////////////////////////////
//调度时间

//时间范围
#define IDI_ATTEMPER_START				1									//调度起始
#define IDI_ATTEMPER_CONCLUDE			99									//调度结束

//时间标识
#define IDI_CONNECT_CENTER_SERVER		(IDI_ATTEMPER_START+1)				//连接服务器
#define IDI_UPDATE_SERVER_LIST			(IDI_ATTEMPER_START+2)				//更新列表
#define IDI_UPDATE_ONLINE_COUNT			(IDI_ATTEMPER_START+3)				//更新人数
#define IDI_SEND_SYSTEM_MESSAGE			(IDI_ATTEMPER_START+4)				//系统消息

//机器标识
#define IDI_LOAD_ANDROID_INFO			(IDI_ATTEMPER_START+5)				//用户信息
#define IDI_DISTRIBUTE_ANDROID			(IDI_ATTEMPER_START+6)				//分配用户

//时间定义
#define TIME_RECONNECT					15000L								//重连时间
#define TIME_UPDATE_LIST				30000L								//更新时间
#define TIME_UPDATE_ONLINE				30000L								//更新人数

//////////////////////////////////////////////////////////////////////////
//机器时间

//时间范围
#define IDI_ANDROID_START				100									//机器起始
#define IDI_ANDROID_CONCLUDE			199									//机器结束

//时间标识
#define IDI_ANDROID_INOUT				(IDI_ANDROID_START+1)				//进出时间
#define IDI_ANDROID_USER_PULSE			(IDI_ANDROID_START+2)				//脉冲时间

//机器时间
#define IDI_ANDROID_USER_TIEM			1									//起始标识
#define IDI_ANDROID_ITEM_SINK			500									//起始标识

//辅助时间
#define TIME_LOAD_ANDROID_INFO			180L								//加载用户
#define TIME_DISTRIBUTE_ANDROID			15L									//分配用户

//用户时间
#define TIME_ANDROID_INOUT				10L									//登录时间
#define TIME_ANDROID_SERVICE			7200L								//服务时间
#define TIME_ANDROID_USER_PULSE			1L									//用户脉冲

//////////////////////////////////////////////////////////////////////////
//桌子时间

//时间范围
#define IDI_TABLE_FRAME_START			200									//桌子起始
#define IDI_TABLE_FRAME_CONCLUDE		5000								//桌子结束

//时间常量
#define RANGE_TABLE_TIME				300									//标识范围

//////////////////////////////////////////////////////////////////////////
//枚举定义

//开始模式
enum enStartMode
{
	enStartMode_FullReady,				//满人开始
	enStartMode_AllReady,				//所有准备
	enStartMode_Symmetry,				//对称开始
	enStartMode_TimeControl,			//时间控制
};

//分数类型
enum enScoreKind
{
	enScoreKind_Win,					//胜
	enScoreKind_Lost,					//输
	enScoreKind_Draw,					//和
	enScoreKind_Flee,					//逃
	enScoreKind_Service,				//服务
	enScoreKind_Present,				//赠送
};

//////////////////////////////////////////////////////////////////////////
//结构定义

//积分信息
struct tagScoreInfo
{
	LONG								lScore;								//游戏积分
	LONG								lRevenue;							//游戏积分
	enScoreKind							ScoreKind;							//分数类型
};

//用户规则
struct tagUserRule
{
	bool								bPassword;							//设置密码
	bool								bLimitWin;							//限制胜率
	bool								bLimitFlee;							//限制逃跑
	bool								bLimitScore;						//限制分数
	bool								bCheckSameIP;						//效验地址
	WORD								wWinRate;							//胜率限制
	WORD								wFleeRate;							//限制逃跑
	LONG								lMaxScore;							//最高分数 
	LONG								lLessScore;							//最低分数
	TCHAR								szPassword[PASS_LEN];				//桌子密码
};

//道具信息
struct tagProperty
{
	INT									nPropertyID;						//道具ID
	DWORD								dwPropCount;						//可用数目
	DWORD								dwConsumedCount;					//使用数目
};

//用户信息结构
struct tagServerUserData
{
	//属性信息
	WORD								wFaceID;							//头像索引
	DWORD								dwCustomFaceVer;					//自定头像
	DWORD								dwUserID;							//用户 I D
	DWORD								dwGameID;							//游戏 I D
	DWORD								dwGroupID;							//社团索引
	DWORD								dwUserRight;						//用户等级
	LONG								lLoveliness;						//用户魅力
	DWORD								dwMasterRight;						//管理权限
	TCHAR								szAccounts[NAME_LEN];				//用户帐号
	TCHAR								szGroupName[GROUP_LEN];				//社团名字
	TCHAR								szUnderWrite[UNDER_WRITE_LEN];		//个性签名

	//用户属性
	BYTE								cbGender;							//用户性别
	BYTE								cbMemberOrder;						//会员等级
	BYTE								cbMasterOrder;						//管理等级

	//状态信息
	WORD								wTableID;							//桌子号码
	WORD								wChairID;							//椅子位置
	BYTE								cbUserStatus;						//用户状态

	//分数信息
	LONG								lStorageScore;						//存储积分
	tagUserScore						UserScoreInfo;						//用户分数

	//扩展信息
	tagProperty							PropertyInfo[PROPERTY_COUNT];		//道具信息
};

//桌子参数结构
struct tagTableFrameParameter
{
	IUnknownEx							* pIGameServiceFrame;				//服务框架
	IUnknownEx							* pIGameServiceManager;				//服务管理
	const tagGameServiceAttrib			* pGameServiceAttrib;				//桌子属性
	const tagGameServiceOption			* pGameServiceOption;				//配置参数
};

//服务参数结构
struct tagGameServiceParameter
{
	TCHAR								szModule[32];						//模块名字
	DWORD								dwCenterAddr;						//中心地址
	tagDataBaseInfo						GameUserDBInfo;						//数据配置
	tagDataBaseInfo						GameScoreDBInfo;					//数据配置
	tagGameServiceOption				GameServiceOption;					//服务配置
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IServerUserItem INTERFACE_VERSION(1,1)
	static const GUID IID_IServerUserItem={0x0400256a,0x6287,0x4a82,0x92,0xbf,0x53,0xcf,0x51,0x62,0x76,0x21};
#else
	#define VER_IServerUserItem INTERFACE_VERSION(1,1)
	static const GUID IID_IServerUserItem={0xe4ce78b7,0x9758,0x4b85,0xa5,0xa9,0x75,0x2f,0x66,0x63,0x32,0x5a};
#endif

//服务器用户接口
interface IServerUserItem : public IUnknownEx
{
	//信息接口
public:
	//是否激活
	virtual bool __cdecl IsActive()=NULL;
	//用户索引
	virtual WORD __cdecl GetUserIndex()=NULL;
	//获取地址
	virtual DWORD __cdecl GetClientIP()=NULL;
	//获取密码
	virtual LPCTSTR __cdecl GetPassword()=NULL;
	//获取规则
	virtual const tagUserRule * __cdecl GetUserRule()=NULL;
	//获取积分
	virtual const tagUserScore * __cdecl GetUserScore()=NULL;
	//获取信息
	virtual tagServerUserData * __cdecl GetUserData()=NULL;

	//属性接口
public:
	//用户名字
	virtual LPCTSTR __cdecl GetAccounts()=NULL;
	//用户 I D
	virtual DWORD __cdecl GetUserID()=NULL;
	//桌子号码
	virtual WORD __cdecl GetTableID()=NULL;
	//椅子号码
	virtual WORD __cdecl GetChairID()=NULL;
	//用户状态
	virtual BYTE __cdecl GetUserStatus()=NULL;
	//用户胜率
	virtual WORD __cdecl GetUserWinRate()=NULL;
	//用户输率
	virtual WORD __cdecl GetUserLostRate()=NULL;
	//用户和率
	virtual WORD __cdecl GetUserDrawRate()=NULL;
	//用户逃率
	virtual WORD __cdecl GetUserFleeRate()=NULL;

	//统计接口
public:
	//获取税收
	virtual LONG __cdecl GetUserRevenue()=NULL;
	//游戏时间
	virtual DWORD __cdecl GetPlayTimeCount()=NULL;
	//在线时间
	virtual DWORD __cdecl GetOnlineTimeCount()=NULL;

	//功能接口
public:
	//设置规则
	virtual bool __cdecl SetUserRule(tagUserRule & UserRule)=NULL;
	//积分信息
	virtual bool __cdecl GetUserScoreInfo(tagUserScore & ScoreModifyInfo)=NULL;
	//修改积分
    virtual bool __cdecl WriteScore(tagScoreInfo & ScoreInfo, DWORD dwPlayTimeCount)=NULL;
	//设置状态
	virtual bool __cdecl SetUserStatus(BYTE cbUserStatus, WORD wTableID, WORD wChairID)=NULL;
	//修改金币
	virtual bool __cdecl ModifyBankStorageGold(LONG lStorageCount)=NULL;
	//积分信息
	virtual tagUserScore* __cdecl GetUserScoreModifyInfo()=NULL;

	//功能接口
public:
	//控制状态
	virtual bool __cdecl IsAndroidUser()=NULL;
	//设置控制
	virtual VOID __cdecl SetAndroidUser(bool bAndroidUser)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IServerUserManager INTERFACE_VERSION(1,1)
	static const GUID IID_IServerUserManager={0x580b63b7,0x4b4f,0x4c6d,0xb7,0x49,0x5b,0x9f,0x46,0xc6,0x36,0xee};
#else
	#define VER_IServerUserManager INTERFACE_VERSION(1,1)
	static const GUID IID_IServerUserManager={0x8614dbc0,0xee75,0x4a82,0xb8,0xe1,0xdf,0x22,0x41,0x39,0x6f,0x49};
#endif

//服务器用户管理接口
interface IServerUserManager : public IUnknownEx
{
	//管理接口
public:
	//重置用户
	virtual bool __cdecl ResetUserManager()=NULL;
	//切换用户
	virtual bool __cdecl SwitchOnLineUserItem(IServerUserItem * pIServerUserItem, DWORD dwClientIP, WORD wUserIndex)=NULL;
	//激活用户
	virtual bool __cdecl ActiveOffLineUserItem(IServerUserItem * pIServerUserItem, DWORD dwClientIP, WORD wUserIndex)=NULL;
	//激活用户
	virtual IServerUserItem * __cdecl ActiveUserItem(tagServerUserData & ServerUserData, DWORD dwClientIP, WORD wUserIndex, TCHAR szPassword[PASS_LEN],WORD wServerType)=NULL;
	//删除用户
	virtual bool __cdecl DeleteUserItem(IServerUserItem * pIServerUserItem)=NULL;
	//注册断线
	virtual bool __cdecl RegOffLineUserItem(IServerUserItem * pIServerUserItem)=NULL;
	//删除断线
	virtual bool __cdecl UnRegOffLineUserItem(IServerUserItem * pIServerUserItem)=NULL;

	//查找接口
public:
	//枚举用户
	virtual IServerUserItem * __cdecl EnumOnLineUser(WORD wEnumIndex)=NULL;
	//枚举用户
	virtual IServerUserItem * __cdecl EnumOffLineUser(WORD wEnumIndex)=NULL;
	//查找用户
	virtual IServerUserItem * __cdecl SearchOnLineUser(DWORD dwUserID)=NULL;
	//查找用户
	virtual IServerUserItem * __cdecl SearchOffLineUser(DWORD dwUserID)=NULL;

	//信息接口
public:
	//在线人数
	virtual DWORD __cdecl GetOnLineCount()=NULL;
	//断线人数
	virtual DWORD __cdecl GetOffLineCount()=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_ITableFrame INTERFACE_VERSION(1,1)
	static const GUID IID_ITableFrame={0x2b4e7113,0xdf01,0x4398,0xac,0xe0,0x51,0x7f,0x0a,0x60,0xea,0xfd};
#else
	#define VER_ITableFrame INTERFACE_VERSION(1,1)
	static const GUID IID_ITableFrame={0xe65d2d8c,0xe55a,0x4b7c,0x8d,0xde,0x43,0xcc,0x69,0xdf,0x0f,0x9f};
#endif

//游戏桌子接口
interface ITableFrame : public IUnknownEx
{
	//信息接口
public:
	//桌子号码
	virtual WORD __cdecl GetTableID()=NULL;
	//游戏人数
	virtual WORD __cdecl GetChairCount()=NULL;
	//游戏时间
	virtual DWORD __cdecl GetPlayTimeCount()=NULL;

	//用户接口
public:
	//枚举用户
	virtual IServerUserItem * __cdecl EnumLookonUserItem(WORD wIndex)=NULL;
	//获取用户
	virtual IServerUserItem * __cdecl GetServerUserItem(WORD wChairID)=NULL;

	//状态接口
public:
	//获取状态
	virtual BYTE __cdecl GetGameStatus()=NULL;
	//设置状态
	virtual void __cdecl SetGameStatus(BYTE bGameStatus)=NULL;
	//获取配置
	virtual const tagGameServiceOption * __cdecl GetGameServiceOption()=NULL;

	//定时器接口
public:
	//设置定时器
	virtual bool __cdecl SetGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM wBindParam)=NULL;
	//删除定时器
	virtual bool __cdecl KillGameTimer(DWORD dwTimerID)=NULL;

	//网络接口
public:
	//发送数据
	virtual bool __cdecl SendUserData(IServerUserItem * pIServerUserItem, WORD wSubCmdID)=NULL;
	//发送数据
	virtual bool __cdecl SendUserData(IServerUserItem * pIServerUserItem, WORD wSubCmdID, void * pData, WORD wDataSize)=NULL;
	//发送数据
	virtual bool __cdecl SendTableData(WORD wChairID, WORD wSubCmdID)=NULL;
	//发送数据
	virtual bool __cdecl SendTableData(WORD wChairID, WORD wSubCmdID, void * pData, WORD wDataSize)=NULL;
	//发送数据
	virtual bool __cdecl SendLookonData(WORD wChairID, WORD wSubCmdID)=NULL;
	//发送数据
	virtual bool __cdecl SendLookonData(WORD wChairID, WORD wSubCmdID, void * pData, WORD wDataSize)=NULL;

	//消息接口
public:
	//发送房间消息
	virtual bool __cdecl SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType)=NULL;
	//发送游戏消息
	virtual bool __cdecl SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType)=NULL;

	//游戏接口
public:
	//解散游戏
	virtual bool __cdecl DismissGame()=NULL;
	//结束游戏
	virtual bool __cdecl ConcludeGame()=NULL;
	//发送场景
	virtual bool __cdecl SendGameScene(IServerUserItem * pIServerUserItem, void * pData, WORD wDataSize)=NULL;

	//写分接口
public:
	//写入积分
	virtual bool __cdecl WriteUserScore(WORD wChairID, LONG lScore, LONG lRevenue, enScoreKind ScoreKind, LONG lPlayTimeCount=-1)=NULL;
	//写入积分
	virtual bool __cdecl WriteUserScore(IServerUserItem * pIServerUserItem, LONG lScore, LONG lRevenue, enScoreKind ScoreKind, LONG lPlayTimeCount=-1)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_ITableFrameManager INTERFACE_VERSION(1,1)
	static const GUID IID_ITableFrameManager={0xab394013,0x708c,0x4cbe,0xa4,0xa7,0xe1,0x08,0x24,0x3c,0x91,0xdd};
#else
	#define VER_ITableFrameManager INTERFACE_VERSION(1,1)
	static const GUID IID_ITableFrameManager={0x4dba0eb4,0xd92b,0x471f,0xa5,0x50,0xb4,0xea,0x2a,0x6b,0xca,0x74};
#endif

//框架管理接口
interface ITableFrameManager : public ITableFrame
{
	//配置接口
public:
	//初始化
	virtual bool __cdecl InitTableFrame(WORD wTableID, tagTableFrameParameter * pTableFrameParameter)=NULL;

	//信息接口
public:
	//锁定状态
	virtual bool __cdecl IsTableLocked()=NULL;
	//游戏状态
	virtual bool __cdecl IsGameStarted()=NULL;
	//游戏状态
	virtual bool __cdecl IsUserPlaying(IServerUserItem * pIServerUserItem)=NULL;

	//执行接口
public:
	//坐下动作
	virtual bool __cdecl PerformSitDownAction(WORD wChairID, IServerUserItem * pIServerUserItem,LPCTSTR szPassword)=NULL;
	//旁观动作
	virtual bool __cdecl PerformLookonAction(WORD wChairID, IServerUserItem * pIServerUserItem,LPCTSTR szPassword)=NULL;
	//离开动作
	virtual bool __cdecl PerformStandUpAction(IServerUserItem * pIServerUserItem)=NULL;

	//用户事件
public:
	//断线事件
	virtual bool __cdecl OnUserOffLine(WORD wChairID)=NULL;
	//重进事件
	virtual bool __cdecl OnUserReConnect(WORD wChairID)=NULL;
	//请求断线
	virtual bool __cdecl OnUserReqOffLine(WORD wChairID)=NULL;

	//系统事件
public:
	//定时器事件
	virtual bool __cdecl OnEventTimer(DWORD dwTimerID, WPARAM wBindParam)=NULL;
	//游戏事件
	virtual bool __cdecl OnEventSocketGame(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)=NULL;
	//框架事件
	virtual bool __cdecl OnEventSocketFrame(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)=NULL;
	//视频事件处理
	virtual bool __cdecl OnEventSocketVideo(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_ITableFrameControl INTERFACE_VERSION(1,2)
	static const GUID IID_ITableFrameControl={0x3d1bbba0,0x9311,0x4dc9,0x97,0x04,0x62,0x36,0xd7,0xa9,0x29,0xb5};
#else
	#define VER_ITableFrameControl INTERFACE_VERSION(1,2)
	static const GUID IID_ITableFrameControl={0xb2590d93,0xe8fc,0x48fa,0x94,0x44,0x73,0xd8,0x98,0x96,0x88,0x82};
#endif

//控制接口
interface ITableFrameControl : public IUnknownEx
{
	//开始游戏
	virtual bool __cdecl StartGame()=NULL;
	//开始判断
	virtual bool __cdecl StartVerdict()=NULL;
	//调换位置
	virtual bool __cdecl SwitchUserChair(WORD wSourceID[], WORD wTargetID[], WORD wSwitchCount)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_ITableFrameSink INTERFACE_VERSION(1,1)
	static const GUID IID_ITableFrameSink={0xc9aea5dc,0xb7a8,0x47bf,0xb5,0x48,0xcb,0x4b,0xbb,0x3a,0x39,0x6a};
#else
	#define VER_ITableFrameSink INTERFACE_VERSION(1,1)
	static const GUID IID_ITableFrameSink={0x2b4a4b35,0x837a,0x4e84,0x86,0xcb,0xa0,0x01,0x09,0xa8,0xe9,0x9f};
#endif

//游戏桌子回调接口
interface ITableFrameSink : public IUnknownEx
{
	//管理接口
public:
	//初始化
	virtual bool __cdecl InitTableFrameSink(IUnknownEx * pIUnknownEx)=NULL;
	//复位桌子
	virtual void __cdecl RepositTableFrameSink()=NULL;

	//信息接口
public:
	//开始模式
	virtual enStartMode __cdecl GetGameStartMode()=NULL;
	//游戏状态
	virtual bool __cdecl IsUserPlaying(WORD wChairID)=NULL;

	//游戏事件
public:
	//游戏开始
	virtual bool __cdecl OnEventGameStart()=NULL;
	//游戏结束
	virtual bool __cdecl OnEventGameEnd(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)=NULL;
	//发送场景
	virtual bool __cdecl SendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE bGameStatus, bool bSendSecret)=NULL;

	//事件接口
public:
	//定时器事件
	virtual bool __cdecl OnTimerMessage(WORD wTimerID, WPARAM wBindParam)=NULL;
	//游戏消息
	virtual bool __cdecl OnGameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)=NULL;
	//框架消息
	virtual bool __cdecl OnFrameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_ITableUserAction INTERFACE_VERSION(1,1)
	static const GUID IID_ITableUserAction={0x72c11395,0x1ee9,0x4dc2,0xa8,0x66,0x0e,0x3f,0xfb,0xca,0xbb,0xaf};
#else
	#define VER_ITableUserAction INTERFACE_VERSION(1,1)
	static const GUID IID_ITableUserAction={0xfaf49c38,0x0d59,0x4ab8,0xb2,0x3b,0x03,0xde,0x56,0x7d,0x00,0xd1};
#endif

//动作事件
interface ITableUserAction : public IUnknownEx
{
	//用户断线
	virtual bool __cdecl OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem)=NULL;
	//用户重入
	virtual bool __cdecl OnActionUserReConnect(WORD wChairID, IServerUserItem * pIServerUserItem)=NULL;
	//用户坐下
	virtual bool __cdecl OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)=NULL;
	//用户起来
	virtual bool __cdecl OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)=NULL;
	//用户同意
	virtual bool __cdecl OnActionUserReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IAndroidUserItem INTERFACE_VERSION(1,1)
	static const GUID IID_IAndroidUserItem={0x8eab7240,0xd355,0x4b93,0x9f,0xb3,0x03,0x00,0xac,0xde,0x50,0xfb};
#else
	#define VER_IAndroidUserItem INTERFACE_VERSION(1,1)
	static const GUID IID_IAndroidUserItem={0xc83ac2b5,0x9e18,0x4909,0xb0,0x7e,0xd5,0x7c,0x56,0x9b,0x16,0x4b};
#endif

//机器人接口
interface IAndroidUserItem : public IUnknownEx
{
	//状态函数
public:
	//获取状态
	virtual BYTE __cdecl GetGameStatus()=NULL;
	//设置状态
	virtual VOID __cdecl SetGameStatus(BYTE cbGameStatus)=NULL;

	//信息接口
public:
	//获取 I D
	virtual DWORD __cdecl GetUserID()=NULL;
	//桌子号码
	virtual WORD __cdecl GetTableID()=NULL;
	//椅子号码
	virtual WORD __cdecl GetChairID()=NULL;

	//功能接口
public:
	//获取自己
	virtual IServerUserItem * __cdecl GetMeUserItem()=NULL;
	//游戏用户
	virtual IServerUserItem * __cdecl GetTableUserItem(WORD wChariID)=NULL;

	//网络接口
public:
	//发送函数
	virtual bool __cdecl SendSocketData(WORD wSubCmdID)=NULL;
	//发送函数
	virtual bool __cdecl SendSocketData(WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;

	//功能接口
public:
	//删除时间
	virtual bool __cdecl KillGameTimer(UINT nTimerID)=NULL;
	//设置时间
	virtual bool __cdecl SetGameTimer(UINT nTimerID, UINT nElapse)=NULL;
	//发送准备
	virtual bool __cdecl SendUserReady(VOID * pData, WORD wDataSize)=NULL;
	//发送聊天
	virtual bool __cdecl SendChatMessage(DWORD dwTargetUserID, LPCTSTR pszChatMessage, COLORREF crFontColor)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IAndroidUserItemSink INTERFACE_VERSION(1,1)
	static const GUID IID_IAndroidUserItemSink={0xf87ebc8b,0xb80d,0x4a6d,0x9b,0x7b,0x40,0x2a,0x3e,0xd6,0x30,0x42};
#else
	#define VER_IAndroidUserItemSink INTERFACE_VERSION(1,1)
	static const GUID IID_IAndroidUserItemSink={0x43ef04c6,0x04e9,0x4603,0x92,0xff,0x97,0xda,0x1f,0x70,0x31,0x62};
#endif

//机器人接口
interface IAndroidUserItemSink : public IUnknownEx
{
	//控制接口
public:
	//初始接口
	virtual bool __cdecl InitUserItemSink(IUnknownEx * pIUnknownEx)=NULL;
	//重置接口
	virtual bool __cdecl RepositUserItemSink()=NULL;

	//游戏事件
public:
	//时间消息
	virtual bool __cdecl OnEventTimer(UINT nTimerID)=NULL;
	//游戏消息
	virtual bool __cdecl OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;
	//游戏消息
	virtual bool __cdecl OnEventFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;
	//场景消息
	virtual bool __cdecl OnEventGameScene(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize)=NULL;

	//用户事件
public:
	//用户进入
	virtual VOID __cdecl OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)=NULL;
	//用户离开
	virtual VOID __cdecl OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)=NULL;
	//用户积分
	virtual VOID __cdecl OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)=NULL;
	//用户状态
	virtual VOID __cdecl OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)=NULL;
	//用户段位
	virtual VOID __cdecl OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IAndroidUserManager INTERFACE_VERSION(1,1)
	static const GUID IID_IAndroidUserManager={0xbf347008,0x96d7,0x4ca7,0xb2,0x25,0xe9,0xbe,0x0e,0x04,0x94,0xfd};
#else
	#define VER_IAndroidUserManager INTERFACE_VERSION(1,1)
	static const GUID IID_IAndroidUserManager={0xe79d8438,0xe020,0x4bf4,0x80,0x5e,0x8b,0x7a,0x10,0xdc,0xd0,0xad};
#endif

//机器人接口
interface IAndroidUserManager : public IUnknownEx
{
	//控制接口
public:
	//停止服务
	virtual bool __cdecl StopService()=NULL;
	//启动服务
	virtual bool __cdecl StartService()=NULL;

	//配置接口
public:
	//设置库存
	virtual bool __cdecl SetAndroidStock(DWORD dwAndroidStock[], WORD wStockCount)=NULL;

	//管理接口
public:
	//删除机器人
	virtual bool __cdecl DeleteAndroidUserItem(DWORD dwAndroidID)=NULL;
	//切换机器人
	virtual IAndroidUserItem * __cdecl SwitchAndroidUserItem(IServerUserItem * pIServerUserItem)=NULL;
	//创建机器人
	virtual IAndroidUserItem * __cdecl CreateAndroidUserItem(DWORD dwUserID, TCHAR szPassword[PASS_LEN])=NULL;

	//事件接口
public:
	//脉冲事件
	virtual bool __cdecl OnEventTimerPulse(DWORD dwTimerID, WPARAM dwBindParameter)=NULL;

	//网络接口
public:
	//发送数据
	virtual bool __cdecl SendDataToClient(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;
	//发送数据
	virtual bool __cdecl SendDataToClient(DWORD dwAndroidID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;
	//发送数据
	virtual bool __cdecl SendDataToServer(DWORD dwAndroidID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IGameServiceFrame INTERFACE_VERSION(1,1)
	static const GUID IID_IGameServiceFrame={0x1a009277,0x069d,0x42a2,0xb9,0xde,0x5b,0x39,0x29,0x0b,0xf9,0x9c};
#else
	#define VER_IGameServiceFrame INTERFACE_VERSION(1,1)
	static const GUID IID_IGameServiceFrame={0x4d4395f9,0xb18e,0x4b30,0xad,0xc0,0xfb,0x09,0x02,0xd6,0xf9,0x40};
#endif

//游戏服务框架
interface IGameServiceFrame : public IUnknownEx
{
	//状态接口
public:
	//关闭查询
	virtual bool __cdecl IsShallClose()=NULL;
	//私聊查询
	virtual bool __cdecl IsAllowWisper()=NULL;
	//聊天查询
	virtual bool __cdecl IsAllowRoomChat()=NULL;
	//聊天查询
	virtual bool __cdecl IsAllowGameChat()=NULL;
	//进入查询
	virtual bool __cdecl IsAllowEnterRoom()=NULL;
	//进入查询
	virtual bool __cdecl IsAllowEnterGame()=NULL;

	//状态通知
public:
	//发送状态
	virtual bool __cdecl SendTableStatus(WORD wTableID)=NULL;
	//发送分数
	virtual bool __cdecl SendUserScore(IServerUserItem * pIServerUserItem)=NULL;
	//发送状态
	virtual bool __cdecl SendUserStatus(IServerUserItem * pIServerUserItem)=NULL;
	//发送道具
	virtual bool __cdecl SendResidualProperty(IServerUserItem * pIServerUserItem)=NULL;

	//网络接口
public:
	//发送数据
	virtual bool __cdecl SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID)=NULL;
	//发送数据
	virtual bool __cdecl SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize)=NULL;

	//定时器接口
public:
	//设置定时器
	virtual bool __cdecl SetTableTimer(WORD wTableID, DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM wBindParam)=NULL; 
	//删除定时器
	virtual bool __cdecl KillTableTimer(WORD wTableID, DWORD dwTimerID)=NULL;

	//管理接口
public:
	//删除用户
	virtual bool __cdecl DeleteUserItem(IServerUserItem * pIServerUserItem)=NULL;
	//输出信息
	virtual void __cdecl ExportInformation(LPCTSTR pszString, enTraceLevel TraceLevel)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IGameServiceManager INTERFACE_VERSION(1,1)
	static const GUID IID_IGameServiceManager={0x5843d536,0xc6b7,0x4c9d,0x00aa,0xbc,0xce,0x05,0x2f,0x17,0x56,0x84};
#else
	#define VER_IGameServiceManager INTERFACE_VERSION(1,1)
	static const GUID IID_IGameServiceManager={0x1a17e988,0xaed4,0x4bcd,0x0082,0x4d,0x77,0xf7,0x1a,0x4d,0x62,0x21};
#endif

//游戏服务器管理接口
interface IGameServiceManager : public IUnknownEx
{
	//参数接口
public:
	//获取属性
	virtual void __cdecl GetGameServiceAttrib(tagGameServiceAttrib & GameServiceAttrib)=NULL;
	//修改参数
	virtual bool __cdecl RectifyServiceOption(tagGameServiceOption * pGameServiceOption)=NULL;

	//创建接口
public:
	//创建桌子
	virtual VOID * __cdecl CreateTableFrameSink(REFGUID Guid, DWORD dwQueryVer)=NULL;
	//创建机器
	virtual VOID * __cdecl CreateAndroidUserItemSink(REFGUID Guid, DWORD dwQueryVer)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IGameService INTERFACE_VERSION(1,1)
	static const GUID IID_IGameService={0xcefa9ee9,0xb11f,0x47f9,0x8b,0xe1,0x92,0x70,0x25,0xfe,0x5d,0x57};
#else
	#define VER_IGameService INTERFACE_VERSION(1,1)
	static const GUID IID_IGameService={0x93376fd3,0xc55d,0x45d4,0xbd,0x43,0x3d,0x20,0x15,0x72,0x2e,0xb9};
#endif

//游戏服务接口
interface IGameService : public IUnknownEx
{
	//服务状态
	virtual bool __cdecl IsService()=NULL;
	//开始服务
	virtual bool __cdecl StartService(tagGameServiceParameter * pGameServiceParameter)=NULL;
	//停止服务
	virtual bool __cdecl StopService()=NULL;
};

//////////////////////////////////////////////////////////////////////////

//游戏服务组件辅助类
class CGameServiceHelper : public CTempldateHelper<IGameService>
{
	//函数定义
public:
	//构造函数
	CGameServiceHelper(void) : CTempldateHelper<IGameService>(IID_IGameService,
		VER_IGameService,GAME_SERVICE_DLL_NAME,TEXT("CreateGameService")) { }
};

//////////////////////////////////////////////////////////////////////////

//游戏服务组件辅助类
class CGameServiceManagerHelper : public CTempldateHelper<IGameServiceManager>
{
	//函数定义
public:
	//构造函数
	CGameServiceManagerHelper(void) : CTempldateHelper<IGameServiceManager>(IID_IGameServiceManager,
		VER_IGameServiceManager,GAME_SERVICE_DLL_NAME,TEXT("CreateGameServiceManager")) { }
};

//////////////////////////////////////////////////////////////////////////

#endif