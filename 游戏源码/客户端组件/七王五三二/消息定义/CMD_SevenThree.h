#ifndef CMD_EATINGALONE_HEAD_FILE
#define CMD_EATINGALONE_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//公共宏定义

#define KIND_ID						601								//游戏 I D
#define GAME_PLAYER					5									//游戏人数
#define GAME_NAME					TEXT("七王游戏")					//游戏名字
#define GAME_GENRE					(GAME_GENRE_SCORE|GAME_GENRE_GOLD)	//游戏类型

//游戏状态
#define GS_RF_FREE					GS_FREE								//等待开始
#define GS_RF_BANKER				GS_PLAYING+1						//选庄状态
#define GS_RF_PLAYING				GS_PLAYING							//游戏状态


//////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_GAME_START			101									//游戏开始
#define SUB_S_OUT_CARD				102									//用户出牌
#define SUB_S_PASS_CARD				103									//放弃出牌
#define SUB_S_GAME_END				104									//游戏结束
#define SUB_S_DISPLAYTEN            105                                 //用户摆牌
#define SUB_S_JIU                   106                                 //用户揪人
#define SUB_S_BANK					107									//选庄
#define SUB_S_RECRUIT_CARD			108									//补牌

//游戏状态
struct CMD_S_StatusFree
{
	LONG							lBaseScore;							//基础积分
};

//游戏状态
struct CMD_S_StatusPlay
{
	LONG							lBaseScore;							//基础积分
	WORD							wBankerUser;						//庄家用户
	WORD							wLastOutUser;						//出牌的人
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							bCardData[10];						//手上扑克
	BYTE							bCardCount[4];						//扑克数目
	BYTE							bTurnCardCount;						//基础出牌
	BYTE							bTurnCardData[10];					//出牌列表
	LONG                            lAllTurnScore[GAME_PLAYER];			//总局得分
	LONG                            lLastTurnScore[GAME_PLAYER];		//上局得分
};
//用户摆十
struct CMD_S_DisplayTen
{
	WORD                            wChairID;                           //摆十用户
	BYTE                            bTenStatus;                         //摆十类型
																
};

//发送扑克
struct CMD_S_GameStart
{
	WORD							wBanker	;								//最大扑克
	WORD							wCurrentUser;							//当前玩家
	BYTE							bCardData[6];							//扑克列表
};
struct CMD_S_Bank
{
	BYTE							Moster;								//最大扑克
	BYTE							bCardData[GAME_PLAYER];				//扑克列表
};
//用户出牌
struct CMD_S_OutCard
{
	BYTE							bCardCount;							//扑克数目
	WORD				 			wCurrentUser;						//当前玩家
	WORD							wOutCardUser;						//出牌玩家
	BYTE							bCardData[10];						//扑克列表
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
	LONG							lGameScore[GAME_PLAYER];			//游戏积分
	BYTE							bCardCount[GAME_PLAYER];			//扑克数目
	BYTE							bCardData[50];						//扑克列表 
};
//补牌结构
struct CMD_S_RecruitCard
{
	WORD							wCurrentUser;						//当前用户
	BYTE							bRecruitCard[5];					//补牌数据
	BYTE							bRecruitCount;
	BYTE							bOneTurnScoreCard[12];				//一轮的数值牌
	BYTE							bOneTurnScoreCardCount;				//分值牌数
};

//////////////////////////////////////////////////////////////////////////
//客户端命令结构

#define SUB_C_OUT_CART				2									//用户出牌
#define SUB_C_PASS_CARD				3									//放弃出牌
#define SUB_C_BAIPAI                4                                    //用户摆牌                  
#define SUB_C_TOUXIANG              5                                   //用户投降
#define SUB_C_JIU					6									//用户揪人
#define SUB_C_FOURTEN               7									//四个十
#define SUB_C_DISPLAYTEN            8                                   //亮十

//出牌数据包
struct CMD_C_OutCard
{
	BYTE							bCardCount;							//出牌数目
	BYTE							bCardData[10];						//扑克列表
};
struct CMD_C_DisplayTen
{
	BYTE                            bDisplayStatus;                          //亮十的类型
};

//////////////////////////////////////////////////////////////////////////

#endif