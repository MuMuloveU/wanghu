#ifndef CMD_LAND_HEAD_FILE
#define CMD_LAND_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//公共宏定义
#define KIND_ID						180								//游戏 I D
#define GAME_PLAYER					5									//游戏人数
#define PLAYER_CARDS				33									//每个玩家手上的牌数
#define GAME_NAME					TEXT("保皇")						//游戏名字
#define GAME_GENRE					(GAME_GENRE_SCORE|GAME_GENRE_GOLD)	//游戏类型

//游戏状态
#define GS_WK_FREE					GS_FREE								//等待开始

#define GS_WK_PLAYING				GS_PLAYING+1						//游戏进行

//////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_SEND_CARD				100									//发牌命令
#define SUB_S_GAME_START			102									//游戏开始
#define SUB_S_OUT_CARD				103									//用户出牌
#define SUB_S_PASS_CARD				104									//放弃出牌
#define SUB_S_GAME_END				105									//游戏结束
#define SUB_S_ENTHRONEMENT			106									//new即位

//new即位
struct CMD_S_Enthronement
{
	WORD							bCurrentEnthronement;				//new即位
};

//游戏状态
struct CMD_S_StatusFree
{
	LONG							lBaseScore;							//基础积分
};

//游戏状态
struct CMD_S_StatusScore
{
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							bCardData[PLAYER_CARDS];						//手上扑克
};

//游戏状态
struct CMD_S_StatusPlay
{
	LONG							lBaseScore;							//基础积分
	WORD							wLastOutUser;						//出牌的人
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							bCardData[PLAYER_CARDS];						//手上扑克
	BYTE							bCardCount[GAME_PLAYER];						//扑克数目
	BYTE							bTurnCardCount;						//基础出牌
	BYTE							bTurnCardData[PLAYER_CARDS];					//出牌列表
};

//发送扑克
struct CMD_S_SendCard
{
	WORD				 			wCurrentUser;						//当前玩家
	WORD							wCurrentEnthronement;				//当前即位
	WORD							wCurrentHousecarl;					//当前侍卫
	BYTE							bCardData[PLAYER_CARDS];						//扑克列表
};


//用户出牌
struct CMD_S_OutCard
{
	BYTE							bCardCount;							//出牌数目
	WORD				 			wCurrentUser;						//当前玩家
	WORD							wOutCardUser;						//出牌玩家
	BYTE							bCardData[PLAYER_CARDS];						//扑克列表
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
	LONG							lGameScore[GAME_PLAYER];						//游戏积分
	BYTE							bCardCount[GAME_PLAYER];						//扑克数目
	BYTE							bCardData[165];						//扑克列表 
};

//////////////////////////////////////////////////////////////////////////
//客户端命令结构

#define SUB_C_OUT_CART				2									//用户出牌
#define SUB_C_PASS_CARD				3									//放弃出牌
#define SUB_C_CHAIRID				4									//保皇派
//保皇派
struct CMD_C_ChairID
{
	bool IsChair;
	bool IsGet;
};

//出牌数据包
struct CMD_C_OutCard
{
	BYTE							bCardCount;							//出牌数目
	BYTE							bCardData[PLAYER_CARDS];						//扑克列表
};

//////////////////////////////////////////////////////////////////////////

#endif