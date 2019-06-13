#ifndef CMD_UPGRADE_HEAD_FILE
#define CMD_UPGRADE_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//公共宏定义

#ifdef VIDEO_GAME
	#define KIND_ID						603									//游戏 I D
#else
#define KIND_ID						26									//游戏 I D
#endif

#define GAME_PLAYER					4									//游戏人数

#ifdef VIDEO_GAME
#define GAME_NAME					TEXT("视频桥牌")					//游戏名字
#else 
#define GAME_NAME					TEXT("桥牌游戏")					//游戏名字
#endif

#define GAME_GENRE					(GAME_GENRE_GOLD|GAME_GENRE_MATCH)	//游戏类型

//游戏状态
#define GS_UG_FREE					GS_FREE								//空闲状态
#define GS_UG_CALL					(GS_PLAYING+1)						//叫牌状态
#define GS_UG_SCORE					(GS_PLAYING+2)						//叫分状态
#define GS_UG_PLAY					(GS_PLAYING+3)						//游戏状态

//////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_GAME_START			100									//游戏开始
#define SUB_S_SEND_CARD				101									//发送扑克
#define SUB_S_GAME_PLAY				102									//游戏开始
#define SUB_S_OUT_CARD				103									//用户出牌
#define SUB_S_THROW_RESULT			104									//甩牌结果
#define SUB_S_TURN_BALANCE			105									//一轮统计
#define SUB_S_GAME_END				106									//游戏结束
#define SUB_S_CALL_SCORE			107									//叫分命令

//游戏状态
struct CMD_S_StatusFree
{
	LONG							lBaseScore;							//基础积分
};


//叫分状态
struct CMD_S_StatusScore
{
	//游戏变量
	WORD							wBankerUser;						//庄家用户
	BYTE							bBossScore;							//庄家分数
	BYTE							cbPackCount;						//副数数目
	BYTE							cbMainValue;						//主牌数值
	BYTE							cbValueOrder[2];					//主牌数值

	//发牌变量
	BYTE							cbCardCount;						//扑克数目
	BYTE							cbCardData[52];						//扑克列表

	
	//叫分信息
	BYTE							bScoreInfo[4];						//叫分信息
	WORD							wCallScoreUser;						//叫分用户
	WORD				 			wCurrentUser;						//当前玩家

	//叫牌信息
	BYTE							cbCallCard;							//叫牌扑克
	BYTE							cbCallCount;						//叫牌数目
	WORD							wCallCardUser;						//叫牌用户

	
};


//游戏状态
struct CMD_S_StatusPlay
{
	//游戏变量
	BYTE							cbPackCount;						//副数数目
	BYTE							cbMainValue;						//主牌数值
	BYTE							cbMainColor;						//主牌数值
	BYTE							cbValueOrder[2];					//主牌数值

	//用户变量
	WORD							wBankerUser;						//庄家用户
	WORD							wCurrentUser;						//当前用户
	WORD							wFirstOutUser;						//出牌用户

	//叫牌信息
	BYTE							cbCallCard;							//叫牌扑克
	BYTE							cbCallCount;						//叫牌数目
	WORD							wCallCardUser;						//叫牌用户

	//扑克变量
	BYTE							cbCardCount;						//扑克数目
	BYTE							cbCardData[52];						//扑克列表

	//底牌信息
	BYTE							cbConcealCount;						//暗藏数目
	BYTE							cbConcealCard[8];					//暗藏扑克

	//出牌变量
	BYTE							cbOutCardCount[4];					//出牌数目
	BYTE							cbOutCardData[4][52];				//出牌列表

	//得分变量
	BYTE							cbScoreCardCount;					//扑克数目
	BYTE							cbScoreCardData[48];				//得分扑克
};

//等待状态
struct CMD_S_StatusWait
{
	//游戏变量
	BYTE							cbPackCount;						//副数数目
	BYTE							cbMainValue;						//主牌数值
	BYTE							cbValueOrder[2];					//主牌数值

	//用户变量
	WORD							wBankerUser;						//庄家用户
};


//游戏开始
struct CMD_S_GameStart
{
	WORD							wBankerUser;						//庄家用户
	BYTE							cbPackCount;						//副数数目
	BYTE							cbMainValue;						//主牌数值
	BYTE							cbValueOrder[2];					//主牌数值
	BYTE							cbJuKuang;							//局况
};

//发送扑克
struct CMD_S_SendCard
{
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							cbCardCount;						//扑克数目
	BYTE							cbCardData[52];						//扑克列表
};

//用户叫分
struct CMD_S_CallScore
{
	WORD							bCallScoreUser;						//叫分玩家
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							bCurrentScore;						//当前叫分
	BYTE							bCurrentColor;						//当前花色
	BYTE							bMultiples;							//游戏倍数
	BYTE							bLastScore;							//上次分数
	BYTE							bLastColor;							//上次花色
	bool							bDbl;								//加倍标志
	bool							bReDbl;								//再加倍标志
};

//游戏开始
struct CMD_S_GamePlay
{
	WORD							wCurrentUser;						//当前用户
	BYTE							m_cbLandScore;						//地主分数
	BYTE							m_cbLandColor;						//地主花色
	BYTE							bMultiples;							//游戏倍数
	BYTE							bCardCount[4];						//扑克数目
	BYTE							bCardData[4][60];					//扑克列表 
};

//用户出牌
struct CMD_S_OutCard
{
	BYTE							cbCardCount;						//扑克数目
	WORD							wOutCardUser;						//出牌玩家
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							cbCardData[52];						//扑克列表
	BYTE							bHandOutCardCount;					//明手扑克数目
	BYTE							bHandOutCardData[60];				//明手扑克列表 
	bool							bHandOutFlag[4];					//明手控制
};

//甩牌结果
struct CMD_S_ThrowResult
{
	WORD							wOutCardUser;						//出牌玩家
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							cbThrowCardCount;					//扑克数目
	BYTE							cbResultCardCount;					//扑克数目
	BYTE							cbCardDataArray[104];				//扑克数组
};

//一轮统计
struct CMD_TurnBalance
{
	WORD							wTurnWinner;						//一轮胜者
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							bBankerScore;						//庄家得分
	BYTE							bOtherScore;						//防家得分
	bool							bHandOutFlag[4];					//明手控制
};

//游戏结束
struct CMD_S_GameEnd
{
	LONG							lScore[4];							//用户得分
	WORD							wConcealTime;						//游戏倍数
	BYTE							bBankerScore;						//庄家得分
	BYTE							bOtherScore;						//防家得分
};

//////////////////////////////////////////////////////////////////////////
//客户端命令结构

#define SUB_C_OUT_CARD				1									//用户出牌
#define SUB_C_CALL_SCORE			2									//用户叫分
#define SUB_C_MULTIPLES				3									//用户加倍

//用户叫分
struct CMD_C_CallScore
{
	BYTE							bBossScore;							//地主分数
	BYTE							bBossColor;							//地主花色
};

//用户加倍
struct CMD_C_OnMultiples
{
	BYTE							bMultiples;							//游戏倍数
};

//用户出牌
struct CMD_C_OutCard
{
	BYTE							cbCardCount;						//出牌数目
	BYTE							cbCardData[52];						//扑克列表
	bool							bIsHandOut[4];							//是否明手出牌]
	bool							bIsAutoOut;							//是否自动出牌
};

//////////////////////////////////////////////////////////////////////////

#endif