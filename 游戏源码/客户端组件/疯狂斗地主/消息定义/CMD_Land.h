#ifndef CMD_LAND_HEAD_FILE
#define CMD_LAND_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//公共宏定义
#ifdef VIDEO_GAME
#define KIND_ID						522									//游戏 I D
#else
#define KIND_ID						102									//游戏 I D
#endif

#define GAME_PLAYER					3									//游戏人数
#define GAME_NAME					TEXT("疯狂斗地主")						//游戏名字

//游戏状态
#define GS_WK_FREE					GS_FREE								//等待开始
#define GS_WK_SCORE					GS_PLAYING							//叫分状态
#define GS_WK_DOUBLE_SCORE			GS_PLAYING+1						//加倍状态
#define GS_WK_PLAYING				GS_PLAYING+2						//游戏进行

//阶段变量
#define CSD_NORMAL		0
#define CSD_SNATCHLAND	1
#define CSD_BRIGHTCARD	2
#define CSD_GAMESTART	3

//倍数变量
#define BRIGHT_START_TIME				8									//明牌开始
#define BRIGHT_CARD_TIME				4									//明牌倍数

//////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_SEND_CARD				100									//发牌命令
#define SUB_S_LAND_SCORE			101									//叫分命令
#define SUB_S_GAME_START			102									//游戏开始
#define SUB_S_OUT_CARD				103									//用户出牌
#define SUB_S_PASS_CARD				104									//放弃出牌
#define SUB_S_GAME_END				105									//游戏结束
#define SUB_S_BRIGHT_START			106									//明牌开始
#define SUB_S_BRIGHT_CARD			107									//玩家明牌
#define SUB_S_DOUBLE_SCORE			108									//加倍命令
#define SUB_S_USER_DOUBLE			109									//加倍命令


//游戏状态
struct CMD_S_StatusFree
{
	LONG							lBaseScore;							//基础积分
	bool							bBrightStart[GAME_PLAYER];			//玩家明牌
};

//游戏状态
struct CMD_S_StatusScore
{
	BYTE							bLandScore;							//地主分数
	LONG							lBaseScore;							//基础积分
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							bScoreInfo[3];						//叫分信息
	BYTE							bCardData[3][20];					//手上扑克
	bool							bUserTrustee[GAME_PLAYER];			//玩家托管
	BYTE							bCallScorePhase;					//叫牌阶段
	BYTE							bBrightTime;						//明牌倍数
	bool							bUserBrightCard[GAME_PLAYER];		//玩家亮牌
};

//加倍状态
struct CMD_S_StatusDoubleScore
{
	WORD							wLandUser;							//坑主玩家
	LONG							lBaseScore;							//基础积分
	BYTE							bLandScore;							//地主分数
	BYTE							bBackCard[3];						//底牌扑克
	BYTE							bCardData[3][20];					//手上扑克
	BYTE							bCardCount[3];						//扑克数目
	bool							bUserTrustee[GAME_PLAYER];			//玩家托管
	bool							bAllowDouble;						//允许加倍
	bool							bDoubleUser[GAME_PLAYER];			//加倍玩家
	bool							bUserBrightCard[GAME_PLAYER];		//玩家亮牌

};

//游戏状态
struct CMD_S_StatusPlay
{
	WORD							wLandUser;							//坑主玩家
	WORD							wBombTime;							//炸弹倍数
	LONG							lBaseScore;							//基础积分
	BYTE							bLandScore;							//地主分数
	WORD							wLastOutUser;						//出牌的人
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							bBackCard[3];						//底牌扑克
	BYTE							bCardData[3][20];					//手上扑克
	BYTE							bCardCount[3];						//扑克数目
	BYTE							bTurnCardCount;						//基础出牌
	BYTE							bTurnCardData[20];					//出牌列表
	bool							bUserTrustee[GAME_PLAYER];			//玩家托管
	BYTE							bBrightTime;						//明牌倍数
	bool							bUserBrightCard[GAME_PLAYER];		//玩家亮牌
};

//发送扑克
struct CMD_S_SendCard
{
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							bCardData[17];						//扑克列表
};

//发送扑克
struct CMD_S_SendAllCard
{
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							bCardData[GAME_PLAYER][20];			//扑克列表
	BYTE							bBackCardData[3];					//底牌扑克
	bool							bUserBrightCard[GAME_PLAYER];		//玩家亮牌
};

//用户叫分
struct CMD_S_LandScore
{
	WORD							bLandUser;							//叫分玩家
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							bLandScore;							//上次叫分
	BYTE							bCurrentScore;						//当前叫分
	BYTE							bPreCallScorePhase;					//之前阶段
	BYTE							bCallScorePhase;					//叫牌阶段
	bool							bBrightCardUser[GAME_PLAYER];		//明牌玩家
};

//游戏开始
struct CMD_S_GameStart
{
	WORD				 			wLandUser;							//地主玩家
	BYTE							bLandScore;							//地主分数
	WORD				 			wCurrentUser;						//当前玩家	
	BYTE							bBrightCard;						//明牌标志
	bool							bUserBrightCard[GAME_PLAYER];		//玩家亮牌
	BYTE							bBackCardData[3];					//底牌扑克
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
	BYTE							bCardData[54];						//扑克列表 
};

//明牌开始
struct CMD_S_BrightStart
{
	WORD							wBrightUser;						//明牌玩家
	bool							bBright;							//明牌标识
};

//玩家明牌
struct CMD_S_BrightCard
{
	WORD							wBrightUser;						//明牌玩家
	BYTE							cbBrightTime;						//明牌倍数
	BYTE							cbCurrenBrightTime;					//当前倍数
	BYTE							cbCallScorePhase;					//叫分阶段
};

//加倍结构
struct CMD_S_DoubleScore
{
	bool							bAllowDouble;						//允许加倍
	BYTE							bBackCard[3];						//底牌扑克
	WORD				 			wLandUser;							//地主玩家
	BYTE							bCurrentScore;						//当前叫分
};

//加倍结构
struct CMD_S_UserDouble
{
	WORD							wDoubleUser;						//加倍玩家
	bool							bDoubleScore;						//是否加倍
	BYTE							bCurrentScore;						//当前叫分
};

//////////////////////////////////////////////////////////////////////////
//客户端命令结构

#define SUB_C_LAND_SCORE			1									//用户叫分
#define SUB_C_OUT_CART				2									//用户出牌
#define SUB_C_PASS_CARD				3									//放弃出牌
#define SUB_C_TRUSTEE				4									//托管消息
#define SUB_C_BRIGHT_START			5									//明牌开始
#define SUB_C_BRIGHT				6									//玩家明牌
#define SUB_C_DOUBLE_SCORE			7									//加倍命令

//托管结构
struct CMD_C_UserTrustee {
	WORD							wUserChairID;						//玩家椅子
	bool							bTrustee;							//托管标识
};

//用户叫分
struct CMD_C_LandScore
{
	BYTE							bLandScore;							//地主分数
};

//出牌数据包
struct CMD_C_OutCard
{
	BYTE							bCardCount;							//出牌数目
	BYTE							bCardData[20];						//扑克列表
};

//玩家明牌
struct CMD_C_BrightCard
{
	BYTE							cbBrightCardTime;					//明牌倍数
};

//玩家加倍
struct CMD_C_DoubleScore
{
	bool							bDoubleScore;						//玩家加倍
};

//////////////////////////////////////////////////////////////////////////

#endif