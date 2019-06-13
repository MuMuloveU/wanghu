#ifndef LIST_SERVICE_HEAD_FILE
#define LIST_SERVICE_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//平台文件
#include "Template.h"
#include "Constant.h"
#include "GlobalDef.h"
#include "GlobalService.h"

//命令码文件
#include "CMD_Game.h"
#include "CMD_Plaza.h"
#include "CMD_Center.h"

//组件头文件
#include "KernelEngineHead.h"

//////////////////////////////////////////////////////////////////////////
//公共宏定义

//导出定义
#ifndef LIST_SERVICE_CLASS
	#ifdef  LIST_SERVICE_DLL
		#define LIST_SERVICE_CLASS _declspec(dllexport)
	#else
		#define LIST_SERVICE_CLASS _declspec(dllimport)
	#endif
#endif

//模块定义
#ifdef _DEBUG
	#define LIST_SERVICE_DLL_NAME	TEXT("ListServiceD.dll")				//组件 DLL 名字
#else
	#define LIST_SERVICE_DLL_NAME	TEXT("ListService.dll")					//组件 DLL 名字
#endif

//////////////////////////////////////////////////////////////////////////
//结构定义

//游戏种类
struct tagGameTypeItem
{
	tagGameType						GameType;							//种类信息
};

//游戏类型
struct tagGameKindItem
{
	tagGameKind						GameKind;							//类型信息
};

//游戏站点
struct tagGameStationItem
{
	tagGameStation					GameStation;						//站点信息
};

//游戏房间
struct tagGameServerItem
{
	DWORD							dwUpdateTime;						//更新时间
	tagGameServer					GameServer;							//房间信息
};

//////////////////////////////////////////////////////////////////////////
//接口定义

#define VER_IServerListCenter INTERFACE_VERSION(2,1)
static const GUID IID_IServerListCenter={0x80533956,0x98e0,0x43e4,0xa6,0xe,0x11,0xff,0xa,0xb9,0x3c,0x2f};

//中心列表接口
interface IServerListCenter : public IUnknownEx
{
	//插入接口
public:
	//插入种类
	virtual bool __cdecl InsertGameType(tagGameType * pGameType)=NULL;
	//插入类型
	virtual bool __cdecl InsertGameKind(tagGameKind * pGameKind)=NULL;
	//插入房间
	virtual bool __cdecl InsertGameServer(tagGameServer * pGameServer)=NULL;
	//插入站点
	virtual bool __cdecl InsertGameStation(tagGameStation * pGameStation)=NULL;

	//删除接口
public:
	//删除种类
	virtual bool __cdecl DeleteGameType(WORD wTypeID)=NULL;
	//删除类型
	virtual bool __cdecl DeleteGameKind(WORD wKindID)=NULL;
	//删除房间
	virtual bool __cdecl DeleteGameServer(WORD wKindID, WORD wServerID)=NULL;
	//删除站点
	virtual bool __cdecl DeleteGameStation(WORD wKindID, WORD wStationID)=NULL;

	//枚举接口
public:
	//枚举种类
	virtual tagGameTypeItem * const __cdecl EmunGameTypeItem(POSITION & Pos)=NULL;
	//枚举类型
	virtual tagGameKindItem * const __cdecl EmunGameKindItem(POSITION & Pos)=NULL;
	//枚举房间
	virtual tagGameServerItem * const __cdecl EmunGameServerItem(POSITION & Pos)=NULL;
	//枚举站点
	virtual tagGameStationItem * const __cdecl EmunGameStationItem(POSITION & Pos)=NULL;

	//查找接口
public:
	//查找种类
	virtual tagGameTypeItem * const __cdecl SearchGameType(WORD wTypeID)=NULL;
	//查找类型
	virtual tagGameKindItem * const __cdecl SearchGameKind(WORD wKindID)=NULL;
	//查找房间
	virtual tagGameServerItem * const __cdecl SearchGameServer(WORD wKindID, WORD wServerID)=NULL;
	//查找站点
	virtual tagGameStationItem * const __cdecl SearchGameStation(WORD wKindID, WORD wStationID)=NULL;

	//数目接口
public:
	//种类数目
	virtual DWORD __cdecl GetGameTypeCount()=NULL;
	//类型数目
	virtual DWORD __cdecl GetGameKindCount()=NULL;
	//房间数目
	virtual DWORD __cdecl GetGameServerCount()=NULL;
	//站点数目
	virtual DWORD __cdecl GetGameStationCount()=NULL;

	//维护接口
public:
	//重置列表
	virtual void __cdecl ResetServerList()=NULL;
	//加载列表
	virtual bool __cdecl LoadServerList(tagDataBaseInfo & DataBaseInfo)=NULL;
	//更新人数
	virtual bool __cdecl UpdateServerOnLineCount(WORD wKindID, WORD wServerID, DWORD dwOnLineCount)=NULL;
};

//////////////////////////////////////////////////////////////////////////

//列表服务组件辅助类
class CServerListCenterHelper : public CTempldateHelper<IServerListCenter>
{
	//函数定义
public:
	//构造函数
	CServerListCenterHelper(void) : CTempldateHelper<IServerListCenter>(IID_IServerListCenter,
		VER_IServerListCenter,LIST_SERVICE_DLL_NAME,TEXT("CreateServerListCenter")) { }
};

//////////////////////////////////////////////////////////////////////////

#endif