#ifndef CMD_UPGRADE_HEAD_FILE
#define CMD_UPGRADE_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//公共宏定义

#ifdef	VIDEO_GAME
#define KIND_ID						500									//视频游戏 I D
#else
#define KIND_ID						103									//非视频游戏 I D

#endif
#define GAME_PLAYER					4									//游戏人数
#define GAME_NAME					TEXT("升级游戏")					//游戏名字

//游戏状态
#define GS_UG_FREE					GS_FREE								//空闲状态
#define GS_UG_CALL					(GS_PLAYING+1)						//叫牌状态
#define GS_UG_BACK					(GS_PLAYING+2)						//留底状态
#define GS_UG_PLAY					(GS_PLAYING+3)						//游戏状态
#define GS_UG_WAIT					(GS_PLAYING+4)						//等待状态


//////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_GAME_START			100									//游戏开始
#define SUB_S_SEND_CARD				101									//发送扑克
#define SUB_S_CALL_CARD				102									//用户叫牌
#define SUB_S_SEND_CONCEAL			103									//底牌扑克
#define SUB_S_GAME_PLAY				104									//游戏开始
#define SUB_S_OUT_CARD				105									//用户出牌
#define SUB_S_THROW_RESULT			106									//甩牌结果
#define SUB_S_TURN_BALANCE			107									//一轮统计
#define SUB_S_GAME_END				108									//游戏结束
#define SUB_S_STRUSTEE				109									//用户托管
#define SUB_S_REQUEST_LEAVE			110									//请求离开
#define SUB_S_BATCH_CARD			111									//批量出牌

//牌副定义
#define PACK_COUNT					2									//最大副数
#define CELL_PACK					54									//单元数目
#define MAX_COUNT					PACK_COUNT*CELL_PACK/GAME_PLAYER+8	//最大数目

//游戏状态
struct CMD_S_StatusFree
{
	LONG							lBaseScore;							//基础积分
	//托管变量
	bool							bStrustee[GAME_PLAYER];				//托管标志
};

//叫牌状态
struct CMD_S_StatusCall
{
	//游戏变量
	WORD							wBankerUser;						//庄家用户
	BYTE							cbPackCount;						//副数数目
	BYTE							cbMainValue;						//主牌数值
	BYTE							cbValueOrder[2];					//主牌数值

	//发牌变量
	BYTE							cbCardCount;						//扑克数目
	BYTE							cbCardData[MAX_COUNT];				//扑克列表

	//叫牌信息
	BYTE							cbComplete;							//完成标志
	BYTE							cbCallCard;							//叫牌扑克
	BYTE							cbCallCount;						//叫牌数目
	WORD							wCallCardUser;						//叫牌用户

	//托管变量
	bool							bStrustee[GAME_PLAYER];				//托管标志
};

//留底状态
struct CMD_S_StatusBack
{
	//游戏变量
	BYTE							cbPackCount;						//副数数目
	BYTE							cbMainValue;						//主牌数值
	BYTE							cbMainColor;						//主牌数值
	BYTE							cbValueOrder[2];					//主牌数值

	//叫牌信息
	BYTE							cbCallCard;							//叫牌扑克
	BYTE							cbCallCount;						//叫牌数目
	WORD							wCallCardUser;						//叫牌用户

	//用户变量
	WORD							wBankerUser;						//庄家用户
	WORD							wCurrentUser;						//当前用户

	//扑克变量
	BYTE							cbCardCount;						//扑克数目
	BYTE							cbCardData[MAX_COUNT];				//扑克列表

	//底牌信息
	BYTE							cbConcealCount;						//暗藏数目
	BYTE							cbConcealCard[8];					//暗藏扑克

	//托管变量
	bool							bStrustee[GAME_PLAYER];				//托管标志
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
	BYTE							cbCardData[MAX_COUNT];				//扑克列表

	//底牌信息
	BYTE							cbConcealCount;						//暗藏数目
	BYTE							cbConcealCard[8];					//暗藏扑克

	//出牌变量
	BYTE							cbOutCardCount[GAME_PLAYER];				//出牌数目
	BYTE							cbOutCardData[GAME_PLAYER][MAX_COUNT];		//出牌列表
	BYTE							cbLastTurnCard[GAME_PLAYER][MAX_COUNT];		//上次出牌
	BYTE							cbLastTurnCount[GAME_PLAYER];				//上次出牌数

	//得分变量
	BYTE							cbScoreCardCount;					//扑克数目
	BYTE							cbScoreCardData[PACK_COUNT*12];		//得分扑克

	//托管变量
	bool							bStrustee[GAME_PLAYER];				//托管标志
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
};

//发送扑克
struct CMD_S_SendCard
{
	BYTE							cbCardCount;						//扑克数目
	BYTE							cbCardData[MAX_COUNT];				//扑克列表
};

//用户叫牌
struct CMD_S_CallCard
{
	BYTE							cbCallCard;							//叫牌扑克
	BYTE							cbCallCount;						//叫牌数目
	WORD							wCallCardUser;						//叫牌用户
};

//底牌扑克
struct CMD_S_SendConceal
{
	BYTE							cbMainColor;						//主牌数值
	WORD							wBankerUser;						//庄家用户
	WORD							wCurrentUser;						//当前用户
	BYTE							cbConcealCount;						//暗藏数目
	BYTE							cbConcealCard[8];					//暗藏扑克
};

//游戏开始
struct CMD_S_GamePlay
{
	WORD							wCurrentUser;						//当前用户
	BYTE							cbConcealCount;						//暗藏数目
	BYTE							cbConcealCard[8];					//暗藏扑克
};

//用户出牌
struct CMD_S_OutCard
{
	BYTE							cbCardCount;						//扑克数目
	WORD							wOutCardUser;						//出牌玩家
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							cbCardData[MAX_COUNT];				//扑克列表
};

//甩牌结果
struct CMD_S_ThrowResult
{
	WORD							wOutCardUser;						//出牌玩家
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							cbThrowCardCount;					//扑克数目
	BYTE							cbResultCardCount;					//扑克数目
	BYTE							cbCardDataArray[2*MAX_COUNT];		//扑克数组
};

//一轮统计
struct CMD_TurnBalance
{
	WORD							wTurnWinner;						//一轮胜者
	WORD				 			wCurrentUser;						//当前玩家
};

//游戏结束
struct CMD_S_GameEnd
{
	LONG							lScore[GAME_PLAYER];				//用户得分
	WORD							wGameScore;							//游戏得分
	WORD							wConcealTime;						//扣底倍数
	WORD							wConcealScore;						//底牌积分
	BYTE							cbConcealCount;						//暗藏数目
	BYTE							cbConcealCard[8];					//暗藏扑克
};

//用户托管
struct CMD_S_Strustee
{
	WORD							wChairId;							//托管用户
	bool							bStrustee;							//是否托管
};

//请求离开
struct CMD_S_RequestLeave
{
	WORD							wChairID;							//离开用户
	TCHAR							szLeaveReason[128];					//离开原因
};

//批量出牌
struct CMD_S_BatchCard
{
	BYTE							cbCardCount;						//扑克数目
	WORD							wOutCardUser;						//出牌玩家
	WORD							wTurnWinner;						//一轮胜者
	BYTE							cbCardData[PACK_COUNT*CELL_PACK];	//扑克列表
};


//////////////////////////////////////////////////////////////////////////
//客户端命令结构

#define SUB_C_CALL_CARD				1									//用户叫牌
#define SUB_C_CALL_FINISH			2									//叫牌完成
#define SUB_C_CONCEAL_CARD			3									//底牌扑克
#define SUB_C_OUT_CARD				4									//用户出牌
#define SUB_C_STRUSTEE				5									//用户托管
#define SUB_C_REQUEST_LEAVE			6									//请求离开
#define SUB_C_RESPONSES_LEAVE		7									//回应离开

//用户叫牌
struct CMD_C_CallCard
{
	BYTE							cbCallCard;							//叫牌扑克
	BYTE							cbCallCount;						//叫牌数目
};

//底牌扑克
struct CMD_C_ConcealCard
{
	BYTE							cbConcealCount;						//暗藏数目
	BYTE							cbConcealCard[8];					//暗藏扑克
};

//用户出牌
struct CMD_C_OutCard
{
	BYTE							cbCardCount;						//出牌数目
	BYTE							cbCardData[MAX_COUNT];				//扑克列表
};

//用户托管
struct CMD_C_Strustee
{
	bool bStrustee;														//是否托管
};

//请求离开
struct CMD_C_RequestLeave
{
	TCHAR							szLeaveReason[128];					//离开原因
};

//请求回应
struct CMD_C_ResponsesLeave
{
	WORD							wChairID;							//离开用户
	BYTE							cbAgreeLeave;						//同意离开
};

//////////////////////////////////////////////////////////////////////////

#endif