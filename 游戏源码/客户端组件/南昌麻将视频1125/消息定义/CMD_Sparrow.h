#ifndef CMD_SPARROW_HEAD_FILE
#define CMD_SPARROW_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//公共宏定义

#define KIND_ID						557									//游戏 I D
#define GAME_PLAYER					4									//游戏人数
#define GAME_NAME					TEXT("南昌麻将")					//游戏名字
#define GAME_GENRE					(GAME_GENRE_SCORE|GAME_GENRE_MATCH|GAME_GENRE_GOLD)	//游戏类型

//游戏状态
#define GS_MJ_FREE					GS_FREE								//空闲状态
#define GS_MJ_BATCH					(GS_PLAYING+1)						//洗澡状态
#define GS_MJ_SICE					(GS_PLAYING+2)						//筛子状态	
#define GS_MJ_PLAY					(GS_PLAYING+3)						//游戏状态

#define MAX_COUNT					14  								//最大数目
#define MUST_LEFT_COUNT				36									//预留数目	

//////////////////////////////////////////////////////////////////////////

//组合子项
struct CMD_WeaveItem
{
	BYTE							cbWeaveKind;						//组合类型
	BYTE							cbCenterCard;						//中心扑克
	BYTE							cbPublicCard;						//公开标志
	WORD							wProvideUser;						//供应用户
};

//////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_GAME_START			100									//游戏开始
#define SUB_S_OUT_CARD				101									//出牌命令
#define SUB_S_SEND_CARD				102									//发送扑克
#define SUB_S_OPERATE_NOTIFY		104									//操作提示
#define SUB_S_OPERATE_RESULT		105									//操作命令
#define SUB_S_GAME_END				106									//游戏结束
#define SUB_S_BATCHRESULT			107									//洗澡
#define SUB_S_TRUSTEE				108									//用户托管
#define SUB_S_BATCH					109									//洗澡命令
//游戏状态
struct CMD_S_StatusFree
{
	LONG							lCellScore;							//基础金币
	WORD							wBankerUser;						//庄家用户
	bool							bTrustee[GAME_PLAYER];						//是否托管
	LONG                            lAllTurnScore;					//总局得分
	LONG                            lLastTurnScore;					//上局得分

};

//游戏状态
struct CMD_S_StatusBatch
{
	LONG							lCellScore;							//基础金币
	BYTE							bComplete;							//完成标志
	bool							bTrustee[GAME_PLAYER];						//是否托管

	//历史积分
	LONG                            lAllTurnScore[4];					//总局得分
	LONG                            lLastTurnScore[4];					//上局得分

};

//游戏状态
struct CMD_S_StatusPlay
{
	//游戏变量
	LONG							lCellScore;							//单元积分
	WORD							wSiceCount;							//骰子点数
	WORD							wSiceCount2;						//骰子点数
	WORD							wSiceCount3;						//骰子点数
	WORD							wBankerUser;						//庄家用户
	WORD							wCurrentUser;						//当前用户
	BYTE							cbChip[GAME_PLAYER];				//洗澡分数
	bool							bTrustee[GAME_PLAYER];						//是否托管

	
	//风牌记录
	BYTE							cbFengCardData[8];					//风牌记录
	BYTE							cbFengCardCount;					//风牌记录

	//状态变量
	BYTE							cbActionCard;						//动作扑克
	WORD							wActionMask;						//动作掩码
	BYTE							cbLeftCardCount;					//剩余数目

	//出牌信息
	WORD							wOutCardUser;						//出牌用户
	BYTE							cbOutCardData;						//出牌扑克
	BYTE							cbDiscardCount[4];					//丢弃数目
	BYTE							cbDiscardCard[4][55];				//丢弃记录

	//扑克数据
	BYTE							cbCardCount[GAME_PLAYER];			//扑克数目
	BYTE							cbKingCardIndex[2];						//王牌扑克
	BYTE							cbCardData[GAME_PLAYER][MAX_COUNT];				//扑克列表
	BYTE							cbSendCardData;								//发送扑克

	//组合扑克
	BYTE							cbWeaveCount[4];					//组合数目
	CMD_WeaveItem					WeaveItemArray[4][4];				//组合扑克
	
	//历史积分
	LONG                            lAllTurnScore[4];					//总局得分
	LONG                            lLastTurnScore[4];					//上局得分
	
	//上下番
	LONG							lUpDownScore[GAME_PLAYER];			//上下翻得分	
	BYTE							cbUpDownKingCardData;				//上下番财神
	BYTE							cbFloor;							//楼层	

};

//游戏开始
struct CMD_S_GameStart
{
	WORD							wSiceCount;							//骰子点数
	WORD							wSiceCount2;						//骰子点数2
	WORD							wSiceCount3;						//骰子点数3
	WORD							wBankerUser;						//庄家用户
	WORD							wCurrentUser;						//当前用户
	BYTE							wUserAction;						//用户动作
	BYTE							cbChip[GAME_PLAYER];				//洗澡分数
	BYTE							cbKingCardIndex[2];					//王牌扑克
	BYTE							cbCardData[GAME_PLAYER][MAX_COUNT];				//扑克列表
	bool							bTrustee[GAME_PLAYER];				//是否托管
	LONG							lUpDownScore[GAME_PLAYER];			//上下翻得分	
	BYTE							cbUpDownKingCardData;				//上下番财神
	BYTE							cbFloor;							//楼层	
};
struct CMD_S_BatchResult
{
	BYTE							cbBatchResult[GAME_PLAYER];						//庄家用户
};

//出牌命令
struct CMD_S_OutCard
{
	WORD							wOutCardUser;						//出牌用户
	BYTE							cbOutCardData;						//出牌扑克
};

//发送扑克
struct CMD_S_SendCard
{
	BYTE							cbCardData;							//扑克数据
	WORD							wActionMask;						//动作掩码
	WORD							wCurrentUser;						//当前用户
	bool							cbIsNotGang;							//是否杠牌
};

//听牌命令
struct CMD_S_ListenCard
{
	WORD							wListenUser;						//听牌用户
};

//操作提示
struct CMD_S_OperateNotify
{
	WORD							wResumeUser;						//还原用户
	WORD							wActionMask;						//动作掩码
	BYTE							cbActionCard;						//动作扑克
};

//操作命令
struct CMD_S_OperateResult
{
	WORD							wOperateUser;						//操作用户
	WORD							wProvideUser;						//供应用户
	WORD							wOperateCode;						//操作代码
	BYTE							cbOperateCard;						//操作扑克
};

//游戏结束
struct CMD_S_GameEnd
{

	//税收
	LONG							lGameTax;							//游戏税收

	//冲关
	BYTE							cbChongguan[GAME_PLAYER];			//冲关数目	

	//霸王精
	BYTE							cbBaWangKing[GAME_PLAYER];			//霸王精

	//结束信息
	WORD							wProvideUser;						//供应用户
	BYTE							cbChiHuCard;						//供应扑克
	DWORD							dwChiHuKind[GAME_PLAYER];			//胡牌类型
	DWORD							dwChiHuRight[GAME_PLAYER];			//胡牌类型

	//积分信息
	LONG							lGameScore[GAME_PLAYER];			//游戏积分
	LONG							lUpDownScore[GAME_PLAYER];			//上下番积分
	LONG							lGameScoreEx[GAME_PLAYER];			//奖励分
	LONG							lGameHuScore[GAME_PLAYER];			//胡牌分

	//扑克信息
	BYTE							cbCardCount[GAME_PLAYER];			//扑克数目
	BYTE							cbCardData[GAME_PLAYER][MAX_COUNT];	//扑克数据


	LONG							lAllTurnScore[GAME_PLAYER];			//总局得分
	LONG							lLastTurnScore[GAME_PLAYER];		//上局得分	

};
//用户托管
struct CMD_S_Trustee
{
	bool							bTrustee;							//是否托管
	WORD							wChairID;							//托管用户
};
//////////////////////////////////////////////////////////////////////////
//客户端命令结构

#define SUB_C_OUT_CARD				1									//出牌命令
#define SUB_C_BATCH					2									//洗澡开始
#define SUB_C_OPERATE_CARD			3									//操作扑克
#define SUB_C_TRUSTEE				4									//用户托管
#define SUB_C_BATCH_END				5									//洗澡完毕	
#define SUB_C_PASS_KING				6									//经过精牌							

//出牌命令
struct CMD_C_OutCard
{
	BYTE							cbCardData;							//扑克数据
};

//洗澡
struct CMD_C_BATCH
{
	BYTE							cbChipTimes;						//买顶倍数
	WORD							wCurrentUser;						//当前用户
};

//操作命令
struct CMD_C_OperateCard
{
	WORD							wOperateCode;						//操作代码
	BYTE							cbOperateCard;						//操作扑克
};
//用户托管
struct CMD_C_Trustee
{
	bool							bTrustee;							//是否托管	
};

//////////////////////////////////////////////////////////////////////////

#endif