#ifndef CMD_GAME_HEAD_FILE
#define CMD_GAME_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//服务定义
//游戏属性
#ifdef VIDEO_GAME
#define KIND_ID						504									//游戏 I D
#define GAME_NAME					TEXT("百变双扣"	)						//游戏名字
#define GAME_PLAYER					4									//游戏人数
#else
#define KIND_ID						110									//游戏 I D
#define GAME_NAME					TEXT("百变双扣"	)						//游戏名字
#define GAME_PLAYER					4									//游戏人数
#endif

//////////////////////////////////////////////////////////////////////////
//数目定义

#define MAX_COUNT					27									//最大数目
#define FULL_COUNT					108									//全牌数目

//////////////////////////////////////////////////////////////////////////

//数值掩码
#define	MASK_COLOR					0xF0								//花色掩码
#define	MASK_VALUE					0x0F								//数值掩码

//扑克类型
#define CT_ERROR					0									//错误类型
#define CT_SINGLE					1									//单牌类型
#define CT_DOUBLE					2									//对子类型
#define CT_THREE					3									//三条类型
#define CT_DOUBLE_LINK				4									//对连类型
#define CT_THREE_LINK				5									//三连类型
#define CT_SHUNZI					6									//顺子类型
#define CT_BOMB						7									//炸弹类型
#define CT_BOMB_TW					8									//天王炸弹
#define CT_BOMB_LINK				9									//排炸类型

//////////////////////////////////////////////////////////////////////////
//状态定义

//游戏状态
#define GS_RF_FREE					GS_FREE								//等待开始
#define GS_RF_PLAYING				GS_PLAYING							//游戏状态

//////////////////////////////////////////////////////////////////////////
//命令定义

#define SUB_S_GAME_START			100									//游戏函数
#define SUB_S_OUT_CARD				101									//用户出牌
#define SUB_S_PASS_CARD				102									//用户放弃
#define SUB_S_GAME_END				103									//游戏结束
#define SUB_S_TRUSTEE				104									//用户托管

//空闲状态
struct CMD_S_StatusFree
{
	//单元积分
	LONG							lCellScore;							//单元积分

	//庄家用户
	WORD							wBankerUser;						//庄家用户


};

//游戏状态
struct CMD_S_StatusPlay
{
	//游戏变量
	LONG							lCellScore;							//单元积分
	WORD							wCurrentUser;						//当前玩家

	//庄家用户
	WORD							wBankerUser;						//庄家用户

	//胜利信息
	WORD							wWinCount;							//胜利人数
	WORD							wWinOrder[GAME_PLAYER];				//胜利列表
	bool							bTrustee[GAME_PLAYER];				//是否托管


	//出牌信息
	WORD							wTurnWiner;							//胜利玩家
	BYTE							cbTurnCardCount;					//出牌数目
	BYTE							cbTurnCardData[MAX_COUNT];			//出牌数据
	BYTE							cbMagicCardData[MAX_COUNT];			//变幻扑克

	//扑克信息
	BYTE							cbHandCardData[MAX_COUNT];			//手上扑克
	BYTE							cbHandCardCount[GAME_PLAYER];		//扑克数目
	BYTE							cbPartnerCardData[MAX_COUNT];		//合作者扑克	
};

//发送扑克
struct CMD_S_GameStart
{
	WORD				 			wCurrentUser;						//当前玩家
	LONG							lCellScore;							//单元积分
	BYTE							cbCardData[MAX_COUNT];				//扑克列表
	BYTE							cbPartnerCardData[MAX_COUNT];		//合作者扑克
	bool							bTrustee[GAME_PLAYER];						//是否托管
};

//用户出牌
struct CMD_S_OutCard
{
	BYTE							cbCardCount;						//出牌数目
	WORD				 			wCurrentUser;						//当前玩家
	WORD							wOutCardUser;						//出牌玩家
	BYTE							cbCardData[MAX_COUNT];				//扑克列表
};

//放弃出牌
struct CMD_S_PassCard
{
	BYTE							cbTurnOver;							//一轮结束
	WORD				 			wCurrentUser;						//当前玩家
	WORD				 			wPassCardUser;						//放弃玩家
};

//游戏结束
struct CMD_S_GameEnd
{
	//游戏成绩
	LONG							lGameScore[GAME_PLAYER];			//游戏积分

	//扑克信息
	BYTE							cbCardCount[GAME_PLAYER];			//扑克数目
	BYTE							cbCardData[GAME_PLAYER][MAX_COUNT];	//扑克列表
	LONG							lGameTax;							//游戏税收
};
//用户托管
struct CMD_S_Trustee
{
	bool							bTrustee;							//是否托管
	WORD							wChairID;							//托管用户
};
//////////////////////////////////////////////////////////////////////////
//命令定义

#define SUB_C_OUT_CARD				1									//用户出牌
#define SUB_C_PASS_CARD				2									//用户放弃
#define SUB_C_TRUSTEE				3									//用户托管

//用户出牌
struct CMD_C_OutCard
{
	BYTE							cbCardCount;						//出牌数目
	BYTE							cbCardData[MAX_COUNT];				//扑克数据
};
//用户托管
struct CMD_C_Trustee
{
	bool							bTrustee;							//是否托管	
};
//////////////////////////////////////////////////////////////////////////

#endif