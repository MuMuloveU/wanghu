#ifndef CMD_BLACKJACK_FILE
#define CMD_BLACKJACK_FILE

//////////////////////////////////////////////////////////////////////////
//公共宏定义

#define KIND_ID							21						//游戏 I D
#define GAME_PLAYER						4									//游戏人数
#define GAME_NAME						TEXT("黑杰克")						//游戏名字
#define GAME_GENRE						(GAME_GENRE_GOLD|GAME_GENRE_MATCH)	//游戏类型
#define MIN_TAX_NUM						100L									//最低到此数字才收税

//版本定义
#define		MAXCLIENTVER			    MAKELONG(0,4)
#define		LESSCLIENTVER			    0
 
//游戏状态定义
#define GS_FREE  						0									//空闲状态
#define	GS_PLAYING				        100									//游戏状态
#define GS_ADDGOLDING   				101									//加注状态

//结束原因
#define GO_NORMAL_FINISH			    0x10								//正常结束

//玩家状态
#define NO_CARD					        0									//无牌状态
#define INIT_CARD						1									//初始状态
#define CUT_CARD						2                                   //分牌状态
#define STOP_CARD						3									//停牌状态
#define	BRUST_CARD						4									//爆牌状态
#define	DOUBLE_GOLD						5									//双倍状态

//////////////////////////////////////////////////////////////////////////
//服务器命令结构

//命令码定义

#define SUB_S_GAME_START				100									//游戏开始
#define SUB_S_ADD_GOLD					101									//下注结果
#define SUB_S_INIT_CARD					102									//初始发牌
#define SUB_S_CUT_CARD					103 								//分牌消息
#define SUB_S_CUT_CARD_MARK 			104 								//切换分牌
#define SUB_S_SEND_CARD					105									//发牌消息
#define SUB_S_STOP_CARD				    106									//用户停牌
#define SUB_S_BRUST_CARD				107									//爆牌消息
#define SUB_S_BANKER_SEND_CARD   		108									//庄家发牌
#define SUB_S_DOUBLE_GOLD   	    	109									//庄家发牌
#define SUB_S_USER_LEFT     	    	110									//闲家强退
#define SUB_S_GAME_END					111									//游戏结束

//消息结构体

//空闲状态
struct CMD_S_StatusFree
{
	LONG								dwBasicGold;						//基础金币
};

//加注状态
struct CMD_S_StatusAddGold
{
	WORD				 				wCurrentUser;						//当前玩家
	WORD						        wBankerStation;				    	//庄家位置
	BOOL						        bAddGoldEnd[GAME_PLAYER];	    	//加注结束
	LONG								dwBasicGold;						//基础金币
	LONG								lMaxGold;							//最大下注
	LONG								lBasicGold;							//基础金币
	LONG								lTableGold[2*GAME_PLAYER];			//桌面金币
};

//游戏状态
struct CMD_S_StatusPlay
{
	WORD				 				wCurrentUser;						//当前玩家
	WORD						        wBankerStation;				    	//庄家位置
	BYTE								cbPlayStatus[GAME_PLAYER];			//游戏状态
	BYTE								cbTableCardCount[GAME_PLAYER];		//扑克数目
	BYTE								cbTableCardArray[GAME_PLAYER][13];	//扑克数组
	LONG								lMaxGold;							//最大下注
	LONG								lBasicGold;							//基础金币
	LONG								lTableGold[2*GAME_PLAYER];			//桌面金币
	BOOL								bBankerGetCard;						//庄家要牌
};

//游戏开始
struct CMD_S_GameStart
{	
	WORD						        wBankerStation;				    	//庄家位置
	BYTE								cbFundusCard;						//底牌扑克
	WORD				 				wCurrentUser;						//当前玩家
	LONG								lMaxGold;							//最大下注
	LONG								lTurnBasicGold;						//最少下注
	BYTE								cbCardData[GAME_PLAYER];			//用户扑克
};

//下注结果
struct CMD_S_AddGold
{
	BOOL                                bDoubleGold;                        //是否双倍
	WORD								wCurrentUser;						//当前用户
	LONG								lLastAddGold;						//下注数目
};

//初始发牌
struct CMD_S_InitCard
{
	BYTE								cbFundusCard;						//底牌扑克
	BOOL								bCanCutCard;                        //能否分牌
	BYTE								cbCardData[GAME_PLAYER];			//用户扑克
	BYTE								cbCardSum;	            			//扑克总值
};

//发牌数据包
struct CMD_S_SendCard
{
	WORD								wCurrentUser;						//当前用户
	BYTE								cbUserCard;						    //用户扑克
	WORD                                wGetCardUser;                       //要牌玩家
	BYTE								cbCardSum;	            			//扑克总值
	BOOL								bCutCard;	            			//用户分牌（0:左手牌，1:右手牌）
};

//用户分牌
struct CMD_S_CutCard
{
	WORD								wCutCardUser;						//分牌用户
	BYTE								cbFirstCard;					   	//首张扑克
	BYTE								cbSecondCard;						//次张扑克	
	BYTE								cbFirstCardSum;					   	//首张扑克
	BYTE								cbSecondCardSum;					//次张扑克	
};

//用户停牌
struct CMD_S_StopCard
{
	WORD								wStopUser;							//停牌用户
	BOOL								bDoubleBrsut;						//双倍爆牌
};

//用户爆牌
struct CMD_S_BrustCard
{
	WORD								wBrustUser;							//爆牌用户
};

//用户强退
struct CMD_S_UserLeft
{
	WORD								wUserChairID;						//强退用户
	BOOL								bAddGoldEnd;						//加注结束
};

//游戏结束
struct CMD_S_GameEnd
{
	LONG								lTax;								//游戏税收
	LONG								lGameGold[GAME_PLAYER];				//游戏得分
	BYTE								cbUserCard[GAME_PLAYER];			//用户扑克
	BYTE								cbCutCard[GAME_PLAYER];		    	//分牌扑克
	BYTE								cbCardSum[GAME_PLAYER];	   	    	//扑克总值
	BYTE								cbCutCardSum[GAME_PLAYER];	     	//扑克总值
};

//////////////////////////////////////////////////////////////////////////
//客户端命令结构

//命令码定义
#define SUB_C_ADD_GOLD					1									//用户下注
#define SUB_C_CANCEL_GOLD				2   								//取消下注
#define SUB_C_GET_CARD					3									//用户要牌
#define SUB_C_STOP_CARD					4									//用户停牌
#define SUB_C_DOUBLE_GOLD				5									//用户双倍
#define SUB_C_CUT_CARD	    			6									//用户分牌

//消息结构体

//用户下注
struct CMD_C_AddGold
{	
	BOOL                                bDoubleGold;                        //是否双倍
	LONG								lGold;								//下注数目
};

//用户要牌
struct CMD_C_GetCard
{
	BOOL								bCutCard;	            			//用户分牌（0:左手牌，1:右手牌）
};

//用户要牌
struct CMD_C_StopCard
{
	BOOL								bCutCard;	            			//用户分牌（0:左手牌，1:右手牌）
};

//////////////////////////////////////////////////////////////////////////

#endif