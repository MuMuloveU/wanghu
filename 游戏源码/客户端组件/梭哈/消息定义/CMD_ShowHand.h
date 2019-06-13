#ifndef CMD_SHOWHAND_HEAD_FILE
#define CMD_SHOWHAND_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//公共宏定义

#define KIND_ID							201									//游戏标识
#define GAME_PLAYER						4									//游戏人数
#define GAME_NAME						TEXT("梭哈游戏")					//游戏名字

//////////////////////////////////////////////////////////////////////////
//命令码定义

#define SUB_C_ADD_GOLD					1									//用户加注
#define SUB_C_GIVE_UP					2									//放弃跟注

#define SUB_S_GAME_START				100									//游戏开始
#define SUB_S_ADD_GOLD					101									//加注结果
#define SUB_S_GIVE_UP					102									//放弃跟注
#define SUB_S_SEND_CARD					103									//发牌消息
#define SUB_S_GAME_END					104									//游戏结束

//////////////////////////////////////////////////////////////////////////
//消息结构体

//游戏状态
struct CMD_S_StatusFree
{
	LONG								dwBasicGold;						//基础金币
};

//游戏状态
struct CMD_S_StatusPlay
{
	WORD				 				wCurrentUser;						//当前玩家
	BYTE								bPlayStatus[GAME_PLAYER];			//游戏状态
	BYTE								bTableCardCount[GAME_PLAYER];		//扑克数目
	BYTE								bTableCardArray[GAME_PLAYER][5];	//扑克数组
	LONG								lMaxGold;							//最大下注
	LONG								lBasicGold;							//基础金币
	LONG								lTurnMaxGold;						//最大下注
	LONG								lTurnBasicGold;						//最少下注
	LONG								lTableGold[2*GAME_PLAYER];			//桌面金币
};

//游戏开始
struct CMD_S_GameStart
{
	BYTE								bFundusCard;						//底牌扑克
	WORD				 				wCurrentUser;						//当前玩家
	LONG								lMaxGold;							//最大下注
	LONG								lTurnMaxGold;						//最大下注
	LONG								lTurnBasicGold;						//最少下注
	LONG								lBasicGold;							//单元下注
	BYTE								bCardData[GAME_PLAYER];				//用户扑克
};

//用户放弃
struct CMD_S_GiveUp
{
	WORD								wUserChairID;						//放弃用户
};

//用户加注
struct CMD_C_AddGold
{
	LONG								lGold;								//加注数目
};

//加注结果
struct CMD_S_AddGold
{
	WORD								wLastChairID;						//上一用户
	WORD								wCurrentUser;						//当前用户
	LONG								lLastAddGold;						//加注数目
	LONG								lCurrentLessGold;					//最少加注
};

//发牌数据包
struct CMD_R_SendCard
{
	LONG								lMaxGold;							//最大下注
	WORD								wCurrentUser;						//当前用户
	WORD								wStartChairId;						//
	BYTE								cbSendCardCount;					//
	BYTE								bUserCard[GAME_PLAYER][2];			//用户扑克
};

//游戏结束
struct CMD_S_GameEnd
{
	LONG								lTax;								//游戏税收
	LONG								lGameGold[4];						//游戏得分
	BYTE								bUserCard[4];						//用户扑克
};

//////////////////////////////////////////////////////////////////////////

#endif