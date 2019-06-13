#ifndef CMD_HK_FIVE_CARD_HEAD_FILE
#define CMD_HK_FIVE_CARD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//公共宏定义

#define KIND_ID							3									//游戏 I D
#define GAME_PLAYER						5									//游戏人数
#define GAME_NAME						TEXT("诈金花")						//游戏名字
#define GAME_GENRE						(GAME_GENRE_GOLD|GAME_GENRE_MATCH)	//游戏类型
#define MAX_COUNT						3									//扑克数目

//结束原因
#define GER_NO_PLAYER					0x10								//没有玩家
#define GER_COMPARECARD					0x20								//比牌结束
#define GER_OPENCARD					0x30								//开牌结束

//游戏状态
#define GS_T_FREE					GS_FREE									//等待开始
#define GS_T_SCORE					GS_PLAYING								//叫分状态
#define GS_T_PLAYING				GS_PLAYING+1							//游戏进行

//////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_GAME_START				100									//游戏开始
#define SUB_S_ADD_SCORE					101									//加注结果
#define SUB_S_GIVE_UP					102									//放弃跟注
#define SUB_S_COMPARE_CARD				105									//比牌跟注
#define SUB_S_LOOK_CARD					106									//看牌跟注
#define SUB_S_SEND_CARD					103									//发牌消息
#define SUB_S_GAME_END					104									//游戏结束
#define SUB_S_PLAYER_EXIT				107									//用户强退
#define SUB_S_OPEN_CARD					108									//开牌消息
#define SUB_S_WAIT_COMPARE				109									//等待比牌

//游戏状态
struct CMD_S_StatusFree
{
	LONG								lCellScore;							//基础积分
};

//游戏状态
struct CMD_S_StatusPlay
{
	//加注信息
	LONG								lMaxCellScore;						//单元上限
	LONG								lCellScore;							//单元下注
	LONG								lCurrentTimes;						//当前倍数
	LONG								lUserMaxScore;						//用户分数上限

	//状态信息
	WORD								wBankerUser;						//庄家用户
	WORD				 				wCurrentUser;						//当前玩家
	BYTE								cbPlayStatus[GAME_PLAYER];			//游戏状态
	bool								bMingZhu[GAME_PLAYER];				//看牌状态
	LONG								lTableScore[GAME_PLAYER];			//下注数目

	//扑克信息
	BYTE								cbHandCardData[3];					//扑克数据

	//状态信息
	bool								bCompareState;						//比牌状态								
};

//游戏开始
struct CMD_S_GameStart
{
	//下注信息
	LONG								lMaxScore;							//最大下注
	LONG								lCellScore;							//单元下注
	LONG								lCurrentTimes;						//当前倍数
	LONG								lUserMaxScore;						//分数上限

	//用户信息
	WORD								wBankerUser;						//庄家用户
	WORD				 				wCurrentUser;						//当前玩家
};

//用户下注
struct CMD_S_AddScore
{
	WORD								wCurrentUser;						//当前用户
	WORD								wAddScoreUser;						//加注用户
	WORD								wCompareState;						//比牌状态
	LONG								lAddScoreCount;						//加注数目
	LONG								lCurrentTimes;						//当前倍数
};

//用户放弃
struct CMD_S_GiveUp
{
	WORD								wGiveUpUser;						//放弃用户
};

//比牌数据包
struct CMD_S_CompareCard
{
	WORD								wCurrentUser;						//当前用户
	WORD								wCompareUser[2];					//比牌用户
	WORD								wLostUser;							//输牌用户
};

//看牌数据包
struct CMD_S_LookCard
{
	WORD								wLookCardUser;						//看牌用户
	BYTE								cbCardData[MAX_COUNT];				//用户扑克
};

//开牌数据包
struct CMD_S_OpenCard
{
	WORD								wWinner;							//胜利用户
};

//游戏结束
struct CMD_S_GameEnd
{
	LONG								lGameTax;							//游戏税收
	LONG								lGameScore[GAME_PLAYER];			//游戏得分
	BYTE								cbCardData[GAME_PLAYER][3];			//用户扑克
	WORD								wCompareUser[GAME_PLAYER][4];		//比牌用户
	WORD								wEndState;							//结束状态
};

//用户退出
struct CMD_S_PlayerExit
{
	WORD								wPlayerID;							//退出用户
};

//等待比牌
struct CMD_S_WaitCompare
{
	WORD								wCompareUser;						//比牌用户
};

//////////////////////////////////////////////////////////////////////////

//客户端命令结构
#define SUB_C_ADD_SCORE					1									//用户加注
#define SUB_C_GIVE_UP					2									//放弃消息
#define SUB_C_COMPARE_CARD				3									//比牌消息
#define SUB_C_LOOK_CARD					4									//看牌消息
#define SUB_C_OPEN_CARD					5									//开牌消息
#define SUB_C_WAIT_COMPARE				6									//等待比牌
#define SUB_C_FINISH_FLASH				7									//完成动画

//用户加注
struct CMD_C_AddScore
{
	LONG								lScore;								//加注数目
	WORD								wState;								//当前状态
};

//比牌数据包
struct CMD_C_CompareCard
{	
	WORD								wCompareUser;						//比牌用户
};

//////////////////////////////////////////////////////////////////////////

#endif