#ifndef CMD_LAND_HEAD_FILE
#define CMD_LAND_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//公共宏定义
#ifdef VIDEO_GAME
#define KIND_ID						523									//游戏 I D
#else
#define KIND_ID						135									//游戏 I D
#endif

#define GAME_PLAYER					3									//游戏人数
#define GAME_NAME					TEXT("四川斗地主")						//游戏名字

//游戏状态
#define GS_WK_FREE					GS_FREE								//等待开始
#define GS_WK_SCORE					GS_PLAYING							//叫分状态
#define GS_WK_PLAYING				GS_PLAYING+1						//游戏进行

//游戏术语
#define BACK_CATCH					0x01								//闷抓
#define MING_CATCH					0x02								//明抓
#define LOOK_CARD					0x03								//看牌
#define DOUBLE_SCORE				0x04								//倒牌
#define CALL_BANKER					0x05								//叫庄
#define CALL_SCORE					0x06								//叫分
#define CALL_TWO_SCORE				0x07								//叫大分
#define PASS_ACTION					0x08								//放弃

//////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_SEND_CARD				100									//发牌命令
#define SUB_S_CHOICE_LOOK			106									//叫庄命令
#define SUB_S_LAND_SCORE			101									//叫分命令
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
	WORD							wFirstUser;							//首叫玩家
	WORD							wBankerUser;						//庄家用户
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							bCardData[20];						//手上扑克
	BYTE							bBackCard[3];						//底牌扑克
	BYTE				 			bMingCard;							//首叫明牌
	BYTE							bCurrentState;						//当前状态
	BYTE							bCallScoreTimes[3];					//叫分次数
	BYTE							bBackCatchTag;						//闷抓标志
	bool							bUserTrustee[GAME_PLAYER];			//玩家托管
};

//游戏状态
struct CMD_S_StatusPlay
{
	LONG							lBaseScore;							//基础积分
	WORD							wFirstUser;							//首叫玩家
	WORD							wBankerUser;						//坑主玩家
	WORD				 			wCurrentUser;						//当前玩家
	WORD							wLastOutUser;						//出牌的人
	WORD							wOutBombCount;						//炸弹数目
	BYTE				 			bMingCard;							//首叫明牌
	BYTE							bCallScoreTimes[3];					//叫分次数
	BYTE							bBackCard[3];						//底牌扑克
	BYTE							bCardData[20];						//手上扑克
	BYTE							bCardCount[3];						//扑克数目
	BYTE							bTurnCardCount;						//基础出牌
	BYTE							bTurnCardData[20];					//出牌列表
	BYTE							bScoreTimes[3];						//分数倍数
	bool							bUserTrustee[GAME_PLAYER];			//玩家托管
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
};

//用户叫庄
struct CMD_S_CallBanker
{
	WORD				 			wCurrentUser;						//当前玩家
	BYTE				 			bMingCard;							//首叫明牌
};

//用户叫分
struct CMD_S_LandScore
{
	WORD							bLandUser;							//叫分玩家
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							bLandScore;							//上次叫分
	BYTE							bCurrentState;						//当前状态
	BYTE							bUserCallTimes;						//用户分数
	WORD							wBankerUser;						//庄家用户
};

//游戏开始
struct CMD_S_GameStart
{
	//WORD				 			wLandUser;							//地主玩家
	BYTE							bLandScore;							//地主分数
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							bBackCard[3];						//底牌扑克
	WORD							wUserMultiple[3];					//用户倍数
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
	BYTE							bBackCard[3];						//底牌扑克 
};

//////////////////////////////////////////////////////////////////////////
//客户端命令结构

#define SUB_C_LAND_SCORE			1									//用户叫分
#define SUB_C_OUT_CART				2									//用户出牌
#define SUB_C_PASS_CARD				3									//放弃出牌
#define SUB_C_TRUSTEE				4									//托管消息

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

//////////////////////////////////////////////////////////////////////////

#endif
