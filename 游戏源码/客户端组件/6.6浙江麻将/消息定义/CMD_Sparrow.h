#ifndef CMD_SPARROW_HEAD_FILE
#define CMD_SPARROW_HEAD_FILE

#include "..\\游戏服务器\\GameLogic.h"
//////////////////////////////////////////////////////////////////////////

//公共宏定义
#define KIND_ID						305						//游戏 I D
#define GAME_PLAYER					4									//游戏人数
#define GAME_NAME					TEXT("浙江麻将")					//游戏名字
#define GAME_GENRE					(GAME_GENRE_GOLD|GAME_GENRE_MATCH)	//游戏类型

//////////////////////////////////////////////////////////////////////////

//组合子项
struct CMD_WeaveItem
{
	BYTE							cbWeaveKind;						//组合类型
	BYTE							cbCenterCard;						//中心扑克
	WORD							wProvideUser;						//供应用户
};

//////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_GAME_START			100									//游戏开始
#define SUB_S_OUT_CARD				101									//出牌命令
#define SUB_S_OPERATE_CARD			102									//操作命令
#define SUB_S_SEND_CARD				103									//发送扑克
#define SUB_S_GAME_END				104									//游戏结束
#define SUB_S_SYS_MSG				105									//系统消息
#define SUB_S_USER_READY			106									//玩家准备
#define SUB_S_STAKE					107									//注额改变

//游戏状态
struct CMD_S_StatusFree
{
	LONG							lCellScore;							//基础金币
	LONG							lLeftScore[4];							//剩余金币
	WORD							wBankerUser;						//庄家用户
};

//游戏状态
struct CMD_S_StatusPlay
{
	//游戏变量
	LONG							lCellScore;							//单元积分
	LONG							lLeftScore[4];							//剩余金币
	WORD							wSiceCount;							//骰子点数
	WORD							wBankerUser;						//庄家用户
	WORD							wCurrentUser;						//当前用户

	//状态变量
	BYTE							cbChihu;							//吃胡标志
	BYTE							cbResponse;							//响应标志
	BYTE							cbLeftCardCount;					//剩余数目

	//出牌信息
	BYTE							cbOutCardCount;						//出牌数目
	WORD							wOutCardUser[108];					//出牌用户
	BYTE							cbOutCardData[108];					//出牌扑克

	//扑克数据
	BYTE							cbKingCardData;						//王牌扑克
	BYTE							cbCardCount;						//扑克数目
	BYTE							cbCardData[14];						//扑克列表

	//组合扑克
	BYTE							bWeaveCount[4];						//组合数目
	CMD_WeaveItem					WeaveItemArray[4][4];				//组合扑克
};

//游戏开始
struct CMD_S_GameStart
{
	WORD							wSiceCount;							//骰子点数
	WORD							wBankerUser;						//庄家用户
	WORD							wCurrentUser;						//当前用户
	BYTE							cbCardData[14];						//扑克列表
	BYTE							cbKingCardData;						//王牌扑克
};

//出牌命令
struct CMD_S_OutCard
{
	WORD							wCurrentUser;						//当前用户
	WORD							wOutCardUser;						//出牌用户
	BYTE							cbOutCardData;						//出牌扑克
	BYTE							cbCurrentCard;						//当前扑克
};

//操作命令
struct CMD_S_OperateCard
{
	WORD							wCurrentUser;						//当前用户
	WORD							wOperateUser;						//操作用户
	WORD							wProvideUser;						//提供用户
	BYTE							cbCurrentCard;						//发送扑克
	BYTE							cbOperateCode;						//操作代码
	BYTE							cbOperateCard;						//操作扑克
};

//发送扑克
struct CMD_S_SendCard
{
	BYTE							cbCardData;							//扑克数据
	WORD							wCurrentUser;						//当前用户
};

//游戏结束
struct CMD_S_GameEnd
{
	tagHuCourt						stHuCourt;							//胡牌详情
	LONG							lGameScore[4];						//游戏积分
	LONG							lLeftScore[4];						//剩余积分
	BYTE							cbCardCount[4];						//扑克数目
	BYTE							cbCardData[4][14];					//扑克数据
	BYTE							cbSelfReady;						//下次ready属性
};

//系统消息
struct CMD_S_SysMsg
{
	BYTE							cbBuf[128];							//消息内容
};

//玩家准备
struct CMD_S_UserReady
{
	WORD							wChairID;							//准备用户
};

//注额改变
struct CMD_S_Stake
{
	LONG							lCellScore;							//单位金币
	LONG							lLeftScore[4];							//剩余金币
};

//////////////////////////////////////////////////////////////////////////
//客户端命令结构

#define SUB_C_OUT_CARD				1									//出牌命令
#define SUB_C_OPERATE_CARD			2									//操作扑克
#define	SUB_C_READY					3									//ready信号

//出牌命令
struct CMD_C_OutCard
{
	BYTE							cbCardData;							//扑克数据
};

//操作命令
struct CMD_C_OperateCard
{
	BYTE							cbOperateCode;						//操作代码
	BYTE							cbOperateCard;						//操作扑克
};

//////////////////////////////////////////////////////////////////////////

#endif