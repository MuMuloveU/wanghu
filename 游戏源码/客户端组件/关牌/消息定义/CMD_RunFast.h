#ifndef CMD_RUNFAST_HEAD_FILE
#define CMD_RUNFAST_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//公共宏定义

#ifdef VIDEO_GAME
	#define KIND_ID						516									//游戏 I D
#else
	#define KIND_ID						112									//游戏 I D
#endif

#define GAME_PLAYER					2									//游戏人数
#define GAME_NAME					TEXT("关牌")						//游戏名字
#define GAME_GENRE					(GAME_GENRE_SCORE|GAME_GENRE_GOLD)	//游戏类型

//游戏状态
#define GS_RF_FREE					GS_FREE								//等待开始
#define GS_RF_PLAYING				GS_PLAYING							//叫分状态

//////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_GAME_START			101									//游戏开始
#define SUB_S_OUT_CARD				102									//用户出牌
#define SUB_S_PASS_CARD				103									//放弃出牌
#define SUB_S_GAME_END				104									//游戏结束
#define SUB_S_AUTOMATISM			105

//玩家托管事件
struct CMD_S_UserAutomatism
{
	bool							bAutoStatus[2];						//托管状态
};

//游戏状态
struct CMD_S_StatusFree
{
	LONG							lBaseScore;							//基础积分
	bool							bAutoStatus[2];						//托管状态
};

//游戏状态
struct CMD_S_StatusPlay
{
	LONG							lBaseScore;							//基础积分
	BYTE							cbBombCount;						//炸弹数目
	WORD							wBankerUser;						//庄家用户
	WORD							wLastOutUser;						//出牌的人
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							bCardData[17];						//手上扑克
	BYTE							bCardCount[2];						//扑克数目
	BYTE							bTurnCardCount;						//基础出牌
	BYTE							bTurnCardData[17];					//出牌列表
	bool							bAutoStatus[2];						//托管状态
};

//发送扑克
struct CMD_S_GameStart
{
	LONG							lBaseScore;							//基础积分
	WORD							wBankerUser;						//庄家用户
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							cbCardData[17];						//扑克列表
};

//用户出牌
struct CMD_S_OutCard
{
	BYTE							bCardCount;							//扑克数目
	WORD				 			wCurrentUser;						//当前玩家
	WORD							wOutCardUser;						//出牌玩家
	BYTE							bCardData[17];						//扑克列表
};

//放弃出牌
struct CMD_S_PassCard
{
	BYTE							bNewTurn;							//一轮开始
	WORD				 			wPassUser;							//放弃玩家
	WORD				 			wCurrentUser;						//当前玩家
};

//游戏结束
struct CMD_S_GameEnd
{
	LONG							lGameTax;							//游戏税收
	LONG							lGameScore[2];						//游戏积分
	BYTE							bCardCount[2];						//扑克数目
	BYTE							bCardData[34];						//扑克列表 
};

//////////////////////////////////////////////////////////////////////////
//客户端命令结构

#define SUB_C_OUT_CART				1									//用户出牌
#define SUB_C_PASS_CARD				2									//放弃出牌
#define SUB_C_AUTOMATISM			4

struct CMD_C_Automatism
{
	bool							bAutomatism;
};

//出牌数据包
struct CMD_C_OutCard
{
	BYTE							bCardCount;							//出牌数目
	BYTE							bCardData[17];						//扑克列表
};

//////////////////////////////////////////////////////////////////////////

#endif