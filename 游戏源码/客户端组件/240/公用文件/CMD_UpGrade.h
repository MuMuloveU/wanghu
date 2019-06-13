#ifndef CMD_SHOWHAND_HEAD_FILE
#define CMD_SHOWHAND_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//公共宏定义

#define KIND_ID							301									//游戏 I D
#define GAME_PLAYER						4									//游戏人数
#define GAME_NAME						TEXT("240游戏")					//游戏名字
#define GAME_GENRE						GAME_GENRE_SCORE					//游戏类型


//游戏状态
#define	GS_SEND_CARD					GS_FREE+10
#define GS_WAIT_PUBLIC					GS_FREE+11				//等待亮牌
#define	GS_OUT_CARD						GS_FREE+14				//出牌

#define								TOTAL_CARDS_CNT		92
#define								BACK_CARD_CNT		0
#define								PLAYER_CARD_CNT		(TOTAL_CARDS_CNT-BACK_CARD_CNT)/GAME_PLAYER	  

//扑克花色
#define UG_FANG_KUAI				0x00			//方块	0000 0000
#define UG_MEI_HUA					0x10			//梅花	0001 0000
#define UG_HONG_TAO					0x20			//红桃	0010 0000
#define UG_HEI_TAO					0x30			//黑桃	0011 0000
#define UG_NT_CARD					0x40			//普通主牌	0100 0000
#define UG_NT_CARD_EX				0x50			//常主
#define UG_NT_CARD_KING				0x60			//大小王
#define UG_ERROR_HUA				0xF0			//错误  1111 0000


//亮，反牌消息
#define IDM_PUBLIC_CARD					WM_USER+500

//命令码
#define	SUB_CS_FINISH_ASHIBIT				49				//完成粘牌

#define	SUB_SC_BEGIN_SEND					51				//开始发牌
#define SUB_SC_SEND_CARD					52				//发牌信息
#define SUB_NT_CARD							53				//用户反牌
#define SUB_SC_SEND_FINISH					54				//发牌完成
#define SUB_CS_SEND_FINISH					57				//发牌完成

#define SUB_SC_BEGIN_OUT					59				//开始游戏
#define SUB_OUT_CARD						60				//用户出牌
#define SUB_SC_BATCH_OUT					62				//批量出牌（最后一轮）
#define SUB_SC_GAME_POINT					63				//用户分数
#define	SUB_SC_GAME_END						65				//游戏结束


#define SUB_SC_THROW_CARD					67				//甩牌


#define	SUB_CS_LOOK_INFO					66				//是否允许旁观

//////////////////////////////////////////////////////////////////////////


struct GS_FreeStruct
{
	 char	buf[2];
};

struct GS_SendStruct
{
	WORD	wBanker;									//庄家
	WORD	wPublic;									//亮牌者
	
	BYTE	bPublicCardFlower;							//亮牌花色
	BYTE	bPublicCardCnt;								//亮牌
	BYTE	bPublicCard[PLAYER_CARD_CNT];	
	BYTE	bAllMeCardCnt;								
	BYTE	bAllMeCard[PLAYER_CARD_CNT];


	BOOL	bIsCanLook;									//是否允许旁观

	
	char	buf[2];
};

struct GS_WaitStruct
{
	WORD	wBanker;									//庄家
	WORD	wPublic;									//亮牌者

	BYTE	bPublicCardFlower;							//亮牌花色
	BYTE	bPublicCardCnt;								//亮牌
	BYTE	bPublicCard[PLAYER_CARD_CNT];				
	BYTE	bMeCardCnt;	
	BYTE	bMeCard[PLAYER_CARD_CNT+BACK_CARD_CNT];
	
	BOOL	bIsCanLook;									//是否允许旁观

	char	buf[2];
};

struct GS_GiveStruct
{
	WORD	wBanker;									//庄家
	WORD	wPublic;									//亮牌者
	BYTE	bPublicCardFlower;							//亮牌花色

	BYTE	bMeCardCnt;	
	BYTE	bMeCard[PLAYER_CARD_CNT+BACK_CARD_CNT];

	BOOL	bIsCanLook;									//是否允许旁观
	
	char	buf[2];
};

struct GS_OutStruct
{
	WORD	wBanker;									//庄家
	WORD	wPublic;									//亮牌者
	WORD	wCurSeat;									//当前给牌者
	WORD	wThrowSeat;									// 甩牌者（就是一轮的首先出牌者)
	BYTE	bPublicCardFlower;							//亮牌花色

	BYTE	bPublicCardCnt;
	BYTE	bPublicCard[2];								//具体亮牌
	BYTE	bFirstCardCnt;
	BYTE	bFirstCard[PLAYER_CARD_CNT];

	BYTE	bScoreCardCnt[GAME_PLAYER];
	BYTE	bScoreCard[GAME_PLAYER][25];				//分牌
	BYTE	bMeCardCnt;	
	BYTE	bMeCard[PLAYER_CARD_CNT];
	BYTE	bOutCardCnt[GAME_PLAYER];
	BYTE	bOutCard[GAME_PLAYER][PLAYER_CARD_CNT];		   //出牌
	
	int		nCurScore[GAME_PLAYER];							//当前得分
	BOOL	bIsCanLook;									   //是否允许旁观

	BOOL	bIsHaveScore;
	BYTE	bMaxCardCnt;
	BYTE	bMaxCard[PLAYER_CARD_CNT];		   //最大牌
	BYTE	bOutHandCnt;									//出了几手牌
	BYTE	bLastOutCardCnt[PLAYER_CARD_CNT][GAME_PLAYER];
	BYTE	bLastOutCard[PLAYER_CARD_CNT][GAME_PLAYER][PLAYER_CARD_CNT];	 //上一轮出牌
	
	char	buf[2];
};

struct ServerBeginSend
{
	BOOL				bIsResend;						//是否是重新开始发送
	BYTE				bAllCard[PLAYER_CARD_CNT];		//一次发送完所有玩家的牌
	BOOL				bIsCanLook;						//是否允许旁观
};


//亮主牌数据包  （发向服务器）
struct ClientNtStruct
{
	BYTE				bCardCount;						//反牌数目
	BYTE				bCard[2];						//反牌
};

//亮主牌数据包  （发向客户端）
struct ServerNtStruct
{
	WORD				wNTSeat;						//反牌者
	BYTE				bHuaKind;						//牌标号
	BYTE				bCardCount;						//反牌数目
	BYTE				bCard[2];						//反牌
};



//开始出牌
struct ServerBeginOut
{
	WORD	wFirstOutSeat;									//首先出牌者
	
};

//用户甩牌失败数据包 （发向客户端）
struct ServerThrowCard
{
	WORD				wNextSeat;						//下一个出牌者
	BYTE				iCardCount;						//扑克数目
	BYTE				bDeskStation;					//当前出牌者
	BYTE				iResultCount;					//结果扑克数目
	BYTE				iCardList[78];					//扑克信息
};


//出牌（客户端）
struct ClientOutCard
{
	WORD	wSeat;											//出牌者
	BYTE	bOutCardCnt;									//出牌个数
	BYTE	bOutCard[PLAYER_CARD_CNT];						//出的牌
};
struct ServerOutCard
{
	WORD	wSeat;											//出牌的人
	WORD	wNextSeat;										//下一个出牌者
	BOOL	bIsLastOut;										//是否是最后一个出牌
	BYTE	bOutCardCnt;									//出牌个数
	BYTE	bOutCard[PLAYER_CARD_CNT];						//出的牌
};

//最后出牌（显示剩下全部的牌）
struct ServerBatchOut
{
	BYTE	bOutCardCnt[GAME_PLAYER];						//出牌个数
	BYTE	bOutCard[GAME_PLAYER][PLAYER_CARD_CNT];			//出的牌
};

struct ServerGamePoint
{
	WORD	wWiner;											//赢分者
	BYTE	bScoreValue;									//分值
	BYTE	bScoreCardCnt;									
	BYTE	bScoreCard[PLAYER_CARD_CNT];
};
struct ClientLookInfo
{
	BOOL	bIsCanLook;
};
//游戏结束
struct ServerGameEnd
{
	BOOL	bIsForceExit;									//是否是强退
	int		iScore[GAME_PLAYER];							//最后得分(赢家的得分写入数据库)
	char	buf[2];
};

//////////////////////////////////////////////////////////////////////////

#endif