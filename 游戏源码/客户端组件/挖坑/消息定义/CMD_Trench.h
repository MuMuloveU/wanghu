#ifndef CMD_TRENCH_HEAD_FILE
#define CMD_TRENCH_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//公共宏定义

//游戏属性
#define KIND_ID						98							//游戏 I D
#define GAME_PLAYER					3									//游戏人数
#define GAME_NAME					TEXT("挖坑游戏")					//游戏名字

//游戏类型
#define GAME_GENRE					(GAME_GENRE_SCORE|GAME_GENRE_GOLD|GAME_GENRE_MATCH)

//游戏状态
#define GS_WK_FREE					GS_FREE								//等待开始
#define GS_WK_SCORE					GS_PLAYING							//叫分状态
#define GS_WK_PLAYING				GS_PLAYING+1						//游戏进行

//////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_SEND_CARD				100									//发牌命令
#define SUB_S_CAVERN_SCORE			101									//叫分命令
#define SUB_S_GAME_START			102									//游戏开始
#define SUB_S_OUT_CARD				103									//用户出牌
#define SUB_S_PASS_CARD				104									//放弃出牌
#define SUB_S_GAME_END				105									//游戏结束

//游戏状态
struct CMD_S_StatusFree
{
	LONG							lBaseScore;							//基础积分
};

//游戏状态
struct CMD_S_StatusScore
{
	LONG							lBaseScore;							//基础积分
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							bCavernScore;						//挖坑分数
	BYTE							bScoreInfo[3];						//叫分信息
	BYTE							bCardData[16];						//手上扑克
};

//游戏状态
struct CMD_S_StatusPlay
{
	LONG							lBaseScore;							//基础积分
	WORD							wCavernUser;						//坑主玩家
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							bCavernScore;						//挖坑分数
	WORD							wLastOutUser;						//出牌的人
	BYTE							bBackCard[4];						//底牌扑克
	BYTE							bCardData[20];						//手上扑克
	BYTE							bCardCount[3];						//扑克数目
	BYTE							bTurnCardCount;						//基础出牌
	BYTE							bTurnCardData[20];					//出牌列表
};

//发送扑克
struct CMD_S_SendCard
{
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							bCardData[16];						//扑克列表
};

//用户叫分
struct CMD_S_CavernScore
{
	WORD							bCavernUser;						//叫分玩家
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							bCavernScore;						//上次叫分
	BYTE							bCurrentScore;						//当前叫分
};

//游戏开始
struct CMD_S_GameStart
{
	WORD				 			wCavernUser;						//挖坑玩家
	BYTE							bCavernScore;						//挖坑分数
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							bBackCard[4];						//底牌扑克
};

//用户出牌
struct CMD_S_OutCard
{
	BYTE							bCardCount;							//出牌数目
	WORD				 			wCurrentUser;						//当前玩家
	WORD							wOutCardUser;						//出牌玩家
	BYTE							bCardData[20];						//扑克列表
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
	LONG							lGameScore[3];						//游戏积分
	BYTE							bCardCount[3];						//扑克数目
	BYTE							bCardData[52];						//扑克列表 
};

//////////////////////////////////////////////////////////////////////////
//客户端命令结构

#define SUB_C_CAVERN_SCORE			1									//用户叫分
#define SUB_C_OUT_CART				2									//用户出牌
#define SUB_C_PASS_CARD				3									//放弃出牌

//用户叫分
struct CMD_C_CavernScore
{
	BYTE							bCavernScore;						//挖坑分数
};

//出牌数据包
struct CMD_C_OutCard
{
	BYTE							bCardCount;							//出牌数目
	BYTE							bCardData[20];						//扑克列表
};

//////////////////////////////////////////////////////////////////////////

#endif