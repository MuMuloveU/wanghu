#ifndef CMD_BACCARAT_HEAD_FILE
#define CMD_BACCARAT_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//公共宏定义

#define KIND_ID						2008									//游戏 I D
#define GAME_PLAYER					100									//游戏人数
#define GAME_NAME					TEXT("百家乐")						//游戏名字

//玩家索引
#define ID_XIAN_JIA					1									//闲家索引
#define ID_PING_JIA					2									//平家索引
#define ID_ZHUANG_JIA				3									//庄家索引
#define ID_XIAN_TIAN_WANG			4									//闲天王
#define ID_ZHUANG_TIAN_WANG			5									//庄天王
#define ID_TONG_DIAN_PING			6									//同点平

//记录信息
struct tagServerGameRecord
{
	WORD							wWinner;							//胜利玩家
	LONG							lTieScore;							//买平总注
	LONG							lBankerScore;						//买庄总注
	LONG							lPlayerScore;						//买闲总注
	BYTE							cbPlayerCount;						//闲家点数
	BYTE							cbBankerCount;						//庄家点数
};

//////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_GAME_START			100									//游戏开始
#define SUB_S_PLACE_JETTON			101									//用户下注
#define SUB_S_GAME_END				102									//游戏结束
#define SUB_S_APPLY_BANKER			103									//申请庄家
#define SUB_S_CHANGE_BANKER			104									//切换庄家
#define SUB_S_CHANGE_USER_SCORE		105									//更新积分
#define SUB_S_SEND_RECORD			106									//游戏记录
#define SUB_S_PLACE_JETTON_FAIL		107									//下注失败
#define SUB_S_GAME_SCORE			108									//发送积分

//失败结构
struct CMD_S_PlaceJettonFail
{
	LONG							lJettonArea;						//下注区域
	LONG							lPlaceScore;						//当前下注
	LONG							lMaxLimitScore;						//限制大小
	LONG							lFinishPlaceScore;					//已下注额
};

//更新积分
struct CMD_S_ChangeUserScore
{
	WORD							wChairID;							//椅子号码
	LONG							lScore;								//玩家积分

	//庄家信息
	WORD							wCurrentBankerChairID;				//当前庄家
	BYTE							cbBankerTime;						//庄家局数
	LONG							lCurrentBankerScore;				//庄家分数
};

//申请庄家
struct CMD_S_ApplyBanker
{
	CHAR							szAccount[ 32 ];					//申请玩家
	LONG							lScore;								//玩家金币
	bool							bApplyBanker;						//申请标识
};

//切换庄家
struct CMD_S_ChangeBanker
{
	WORD							wChairID;							//椅子号码
	BYTE							cbBankerTime;						//庄家局数
	LONG							lBankerScore;						//庄家分数
	LONG							lBankerTreasure;					//庄家金币

	//我的下注
	LONG							lAreaLimitScore;					//区域限制
};

//游戏状态
struct CMD_S_StatusFree
{
	//全局信息
	BYTE							cbTimeLeave;						//剩余时间

	//下注信息
	LONG							lTieScore;							//买平总注
	LONG							lBankerScore;						//买庄总注
	LONG							lPlayerScore;						//买闲总注
	LONG							lTieSamePointScore;					//同点平注
	LONG							lPlayerKingScore;					//闲天王注
	LONG							lBankerKingScore;					//庄天王注

	//我的下注
	LONG							lMeMaxScore;						//最大下注
	LONG							lMeTieScore;						//买平总注
	LONG							lMeBankerScore;						//买庄总注
	LONG							lMePlayerScore;						//买闲总注
	LONG							lMeTieKingScore;					//同点平注
	LONG							lMePlayerKingScore;					//闲天王注
	LONG							lMeBankerKingScore;					//庄天王注

	//庄家信息
	WORD							wCurrentBankerChairID;				//当前庄家
	BYTE							cbBankerTime;						//庄家局数
	LONG							lCurrentBankerScore;				//庄家分数
	LONG							lApplyBankerCondition;				//申请条件
	LONG							lAreaLimitScore;					//区域限制
	LONG							lBankerTreasure;					//庄家金币
};

//游戏状态
struct CMD_S_StatusPlay
{
	//下注信息
	LONG							lTieScore;							//买平总注
	LONG							lBankerScore;						//买庄总注
	LONG							lPlayerScore;						//买闲总注
	LONG							lTieSamePointScore;					//同点平注
	LONG							lPlayerKingScore;					//闲天王注
	LONG							lBankerKingScore;					//庄天王注

	//我的下注
	LONG							lMeMaxScore;						//最大下注
	LONG							lMeTieScore;						//买平总注
	LONG							lMeBankerScore;						//买庄总注
	LONG							lMePlayerScore;						//买闲总注
	LONG							lMeTieKingScore;					//同点平注
	LONG							lMePlayerKingScore;					//闲天王注
	LONG							lMeBankerKingScore;					//庄天王注

	//扑克信息
 	BYTE							cbCardCount[2];						//扑克数目
	BYTE							cbTableCardArray[2][3];				//桌面扑克

	//庄家信息
	WORD							wCurrentBankerChairID;				//当前庄家
	BYTE							cbBankerTime;						//庄家局数
	LONG							lCurrentBankerScore;				//庄家分数
	LONG							lApplyBankerCondition;				//申请条件
	LONG							lAreaLimitScore;					//区域限制
	LONG							lBankerTreasure;					//庄家金币
};

//游戏开始
struct CMD_S_GameStart
{
	BYTE							cbCardCount[2];						//扑克数目
    BYTE							cbTableCardArray[2][3];				//桌面扑克
	LONG							lApplyBankerCondition;				//申请条件

	//庄家信息
	WORD							wBankerChairID;						//庄家号码
	LONG							lBankerScore;						//庄家积分
	BYTE							cbBankerTime;						//做庄次数
};

//用户下注
struct CMD_S_PlaceJetton
{
	WORD							wChairID;							//用户位置
	BYTE							cbJettonArea;						//筹码区域
	LONG							lJettonScore;						//加注数目
};

//游戏结束
struct CMD_S_GameEnd
{
	//下局信息
	LONG							lMeMaxScore;						//最大下注
	BYTE							cbTimeLeave;						//剩余时间

	//成绩记录
	BYTE							cbWinner;							//胜利玩家
	BYTE							cbKingWinner;						//天王胜利
	LONG							lBankerTreasure;					//庄家金币

	LONG							lBankerTotalScore;					//庄家成绩
	LONG							lBankerScore;						//庄家成绩
	INT								nBankerTime;						//做庄次数
};

//游戏得分
struct CMD_S_GameScore
{
	//成绩记录
	BYTE							cbWinner;							//胜利玩家
	BYTE							cbKingWinner;						//天王胜利
	LONG							lMeGameScore;						//我的成绩
	LONG							lMeReturnScore;						//返还注额
	LONG							lBankerScore;						//庄家成绩

	//下注信息
	LONG							lDrawTieScore;						//买平总注
	LONG							lDrawBankerScore;					//买庄总注
	LONG							lDrawPlayerScore;					//买闲总注
	LONG							lDrawTieSamPointScore;				//同点平注
	LONG							lDrawPlayerKingScore;				//闲天王注
	LONG							lDrawBankerKingScore;				//庄天王注

	//我的下注
	LONG							lMeTieScore;						//买平总注
	LONG							lMeBankerScore;						//买庄总注
	LONG							lMePlayerScore;						//买闲总注
	LONG							lMeTieKingScore;					//同点平注
	LONG							lMePlayerKingScore;					//闲天王注
	LONG							lMeBankerKingScore;					//庄天王注
};

//////////////////////////////////////////////////////////////////////////
//客户端命令结构

#define SUB_C_PLACE_JETTON			1									//用户下注
#define SUB_C_APPLY_BANKER			2									//申请庄家

//用户下注
struct CMD_C_PlaceJetton
{
	BYTE							cbJettonArea;						//筹码区域
	LONG							lJettonScore;						//加注数目
};

//申请庄家
struct CMD_C_ApplyBanker
{
	bool							bApplyBanker;						//申请标识
};

//////////////////////////////////////////////////////////////////////////

#endif