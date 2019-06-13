#ifndef GLOBAL_SERVICE_HEAD_FILE
#define GLOBAL_SERVICE_HEAD_FILE

#pragma once

//////////////////////////////////////////////////////////////////////////

//数据库名字
const TCHAR szGameUserDB[]=TEXT("QPGameUserDB");						//用户数据库
const TCHAR	szTreasureDB[]=TEXT("QPTreasureDB");						//财富数据库
const TCHAR szServerInfoDB[]=TEXT("QPServerInfoDB");					//房间数据库

//////////////////////////////////////////////////////////////////////////

//数据库信息
struct tagDataBaseInfo
{
	WORD							wDataBasePort;						//数据库端口
	DWORD							dwDataBaseAddr;						//数据库地址
	TCHAR							szDataBaseUser[32];					//数据库用户
	TCHAR							szDataBasePass[32];					//数据库密码
	TCHAR							szDataBaseName[32];					//数据库名字
};

//////////////////////////////////////////////////////////////////////////

//游戏服务属性
struct tagGameServiceAttrib
{
	WORD							wKindID;							//名称号码
	WORD							wChairCount;						//椅子数目
	BYTE							cbJoinInGame;						//游戏加入
	TCHAR							szDataBaseName[32];					//游戏库名
	TCHAR							szDescription[128];					//模块描述
	TCHAR							szKindName[KIND_LEN];				//类型名字
	TCHAR							szServerModuleName[MODULE_LEN];		//模块名字
	TCHAR							szClientModuleName[MODULE_LEN];		//模块名字
};

//游戏服务配置
struct tagGameServiceOption
{
	//房间属性
	WORD							wKindID;							//类型标识
	WORD							wSortID;							//排序标识
	WORD							wNodeID;							//站点号码
	WORD							wServerID;							//房间号码
	WORD							wTableCount;						//桌子数目
	WORD							wServerPort;						//房间端口
	WORD							wServerType;						//房间类型
	WORD							wMaxConnect;						//最大连接
	DWORD							dwServerAddr;						//房间地址
	DWORD							dwVideoServer;						//视频地址
	DWORD							dwDataBaseAddr;						//数据地址
	TCHAR							szDataBaseName[SERVER_LEN];			//数据名字
	TCHAR							szGameRoomName[SERVER_LEN];			//房间名称

	//积分限制
	WORD							wRevenue;							//游戏税收
	LONG							lCellScore;							//单位积分
	LONG							lLessScore;							//最低积分
	LONG							lMaxScore;							//最高积分
	LONG							lRestrictScore;						//限制积分

	//比赛配置
	LONG							lMatchDraw;							//比赛局数
	BYTE							cbControlStart;						//控制开始

	//扩展配置
	BYTE							cbHideUserInfo;						//隐藏信息
	BYTE							cbUnLookOnTag;						//限制旁观
	BYTE							cbUnSameIPPlay;						//限制同IP
};

//最少连接数
#define	LESS_CONNECT_COUNT			(50L)								//最少连接
//////////////////////////////////////////////////////////////////////////

#endif