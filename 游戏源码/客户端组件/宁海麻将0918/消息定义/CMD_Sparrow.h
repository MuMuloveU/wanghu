#ifndef CMD_SPARROW_HEAD_FILE
#define CMD_SPARROW_HEAD_FILE
//////////////////////////////////////////////////////////////////////////
//公共宏定义
#define MAX_COUNT					14									//最大数目
#define KIND_ID						322									//游戏 I D
#define GAME_PLAYER					4									//游戏人数
#define GAME_REMAIN					18									//留顿牌数	
#define GAME_NAME					TEXT("宁海麻将")					//游戏名字
#define GAME_GENRE					(GAME_GENRE_SCORE|GAME_GENRE_MATCH|GAME_GENRE_GOLD)	//游戏类型

//游戏状态
#define GS_MJ_FREE					GS_FREE								//空闲状态
#define GS_MJ_PLAY					(GS_PLAYING+1)						//游戏状态

//////////////////////////////////////////////////////////////////////////

//组合子项
struct CMD_WeaveItem
{
	BYTE							cbWeaveKind;						//组合类型
	BYTE							cbCenterCard;						//中心扑克
	BYTE							cbPublicCard;						//公开标志
	WORD							wProvideUser;						//供应用户
};

//////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_GAME_START			100									//游戏开始
#define SUB_S_OUT_CARD				101									//出牌命令
#define SUB_S_SEND_CARD				102									//发送扑克
#define SUB_S_LISTEN_CARD			103									//听牌命令
#define SUB_S_OPERATE_NOTIFY		104									//操作提示
#define SUB_S_OPERATE_RESULT		105									//操作命令
#define SUB_S_GAME_END				106									//游戏结束
#define SUB_S_TRUSTEE				107									//用户托管
#define SUB_S_FORCE_SEND_CARD		108									//强迫发牌	
#define SUB_S_FORCE_OUT_CARD		109									//强迫出牌

//游戏状态
struct CMD_S_StatusFree
{
	LONG							lCellScore;							//基础金币
	WORD							wBankerUser;						//庄家用户
	bool							bTrustee[GAME_PLAYER];						//是否托管
};

//游戏状态
struct CMD_S_StatusPlay
{
	//游戏变量
	LONG							lCellScore;							//单元积分
	WORD							wSiceCount;							//骰子点数
	WORD							wBankerUser;						//庄家用户
	WORD							wCurrentUser;						//当前用户

	//状态变量
	BYTE							cbActionCard;						//动作扑克
	BYTE							cbActionMask;						//动作掩码
	BYTE							cbLeftCardCount;					//剩余数目
	bool							bTrustee[GAME_PLAYER];						//是否托管

	//出牌信息
	WORD							wOutCardUser;						//出牌用户
	BYTE							cbOutCardData;						//出牌扑克
	BYTE							cbDiscardCount[4];					//丢弃数目
	BYTE							cbDiscardCard[4][65];				//丢弃记录
	BYTE							cbUserWindCount[GAME_PLAYER];		//风牌记录
	BYTE							cbUserWindCardData[GAME_PLAYER][8]; //风牌记录

	//扑克数据
	BYTE							cbCardCount;						//扑克数目
	BYTE							cbCardData[MAX_COUNT];				//扑克列表

	//组合扑克
	BYTE							cbWeaveCount[4];					//组合数目
	CMD_WeaveItem					WeaveItemArray[4][4];				//组合扑克
};

//游戏开始
struct CMD_S_GameStart
{
	WORD							wSiceCount;							//骰子点数
	WORD							wBankerUser;						//庄家用户
	WORD							wCurrentUser;						//当前用户
	BYTE							cbUserAction;						//用户动作
	BYTE							cbSunWindCount;						//总花番	
	BYTE							cbCardData[MAX_COUNT*GAME_PLAYER];	//扑克列表
	BYTE							cbLeftCardCount;					//剩余牌数
	bool							bFirst;								//是否首发
	BYTE							cbRepertoryCard[144];				//所有牌
	bool							bTrustee[GAME_PLAYER];						//是否托管
};

//出牌命令
struct CMD_S_OutCard
{
	WORD							wOutCardUser;						//出牌用户
	BYTE							cbOutCardData;						//出牌扑克
};

//发送扑克
struct CMD_S_SendCard
{
	BYTE							cbCardData;							//扑克数据
	BYTE							cbActionMask;						//动作掩码
	WORD							wCurrentUser;						//当前用户
	bool							bGang;								//是否杠牌
};

//听牌命令
struct CMD_S_ListenCard
{
	WORD							wListenUser;						//听牌用户
};

//操作提示
struct CMD_S_OperateNotify
{
	WORD							wResumeUser;						//还原用户
	BYTE							cbActionMask;						//动作掩码
	BYTE							cbActionCard;						//动作扑克
};

//操作命令
struct CMD_S_OperateResult
{
	WORD							wOperateUser;						//操作用户
	WORD							wProvideUser;						//供应用户
	BYTE							cbOperateCode;						//操作代码
	BYTE							cbOperateCard;						//操作扑克
};

//游戏结束
struct CMD_S_GameEnd
{
	LONG							lGameTax;							//游戏税收
	BYTE							cbProvideCard;						//吃胡扑克
	WORD							wProvideUser;						//点炮用户
	BYTE							cbHaiDiCard;						//海底扑克
	LONG							lGameScore[4];						//游戏积分
	WORD							wChiHuKind[4];						//胡牌类型
	BYTE							cbCardCount[4];						//扑克数目
	BYTE							cbCardData[4][MAX_COUNT];			//扑克数据
	WORD							wChiHuRight[4];						//胡牌权位
	WORD							wHuCount[4];						//胡牌胡数
	BYTE							cbHua[4];							//胡牌花数	
};
//用户托管
struct CMD_S_Trustee
{
	bool							bTrustee;							//是否托管
	WORD							wChairID;							//托管用户
};
//////////////////////////////////////////////////////////////////////////
//客户端命令结构

#define SUB_C_OUT_CARD				1									//出牌命令
#define SUB_C_LISTEN_CARD			2									//听牌命令
#define SUB_C_OPERATE_CARD			3									//操作扑克
#define SUB_C_FINISH				4									//筛子完毕
#define SUB_C_TRUSTEE				5									//用户托管

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
//用户托管
struct CMD_C_Trustee
{
	bool							bTrustee;							//是否托管	
};
//////////////////////////////////////////////////////////////////////////

#endif