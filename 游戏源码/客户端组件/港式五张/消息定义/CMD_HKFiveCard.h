#ifndef CMD_HK_FIVE_CARD_HEAD_FILE
#define CMD_HK_FIVE_CARD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//公共宏定义


#define KIND_ID						2										//非视频游戏 I D

#define GAME_PLAYER						5									//游戏人数
#define GAME_NAME						TEXT("港式五张")					//游戏名字
#define GAME_GENRE						(GAME_GENRE_GOLD|GAME_GENRE_MATCH)	//游戏类型

//结束原因
#define GER_NO_PLAYER					0x10								//没有玩家

//////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_GAME_START				100									//游戏开始
#define SUB_S_ADD_SCORE					101									//加注结果
#define SUB_S_GIVE_UP					102									//放弃跟注
#define SUB_S_SEND_CARD					103									//发牌消息
#define SUB_S_GAME_END					104									//游戏结束

#define SUB_S_SCORE_OPTION				500									//积分信息

//游戏状态
struct CMD_S_StatusFree
{
	LONG								lCellScore;							//基础积分
};

//游戏状态
struct CMD_S_StatusPlay
{
	//标志变量
	BYTE								bShowHand;							//梭哈标志
	BYTE								bAddScore;							//加注标志

	//加注信息
	LONG								lMaxScore;							//最大下注
	LONG								lCellScore;							//单元下注
	LONG								lTurnMaxScore;						//最大下注
	LONG								lTurnLessScore;						//最小下注

	//状态信息
	WORD				 				wCurrentUser;						//当前玩家
	BYTE								cbPlayStatus[GAME_PLAYER];			//游戏状态
	LONG								lTableScore[GAME_PLAYER*2];			//下注数目

	//扑克信息
	BYTE								cbCardCount[GAME_PLAYER];			//扑克数目
	BYTE								cbHandCardData[GAME_PLAYER][5];		//桌面扑克
};

//游戏开始
struct CMD_S_GameStart
{
	//下注信息
	LONG								lMaxScore;							//最大下注
	LONG								lCellScore;							//单元下注
	LONG								lTurnMaxScore;						//最大下注
	LONG								lTurnLessScore;						//最小下注

	//用户信息
	WORD				 				wCurrentUser;						//当前玩家

	//扑克数据
	BYTE								cbObscureCard;						//底牌扑克
	BYTE								cbCardData[GAME_PLAYER];			//用户扑克
};

//用户下注
struct CMD_S_AddScore
{
	WORD								wCurrentUser;						//当前用户
	WORD								wAddScoreUser;						//加注用户
	LONG								lAddScoreCount;						//加注数目
	LONG								lTurnLessScore;						//最少加注
};

//用户放弃
struct CMD_S_GiveUp
{
	WORD								wGiveUpUser;						//放弃用户
};

//发牌数据包
struct CMD_S_SendCard
{
	WORD								wCurrentUser;						//当前用户
	WORD								wLastMostUser;						//上次最大用户
	LONG								lTurnMaxScore;						//最大下注
	LONG								lTurnLessScore;						//最小下注
	BYTE								cbSendCardCount;					//发牌数目
	BYTE								cbCardData[GAME_PLAYER][2];			//用户扑克
};

//游戏结束
struct CMD_S_GameEnd
{
	LONG								lGameTax[GAME_PLAYER];				//游戏税收
	LONG								lGameScore[GAME_PLAYER];			//游戏得分
	BYTE								cbCardData[GAME_PLAYER];			//用户扑克
};

//积分配置
struct CMD_S_ScoreOption
{
	//状态信息
	LONG								lUserCount;							//用户数目
	LONG								lBlackCount;						//用户数目

	//积分信息
	__int64								lMaxScore;							//最大变化
	__int64								lMaxWinScore;						//最大赢分
	__int64								lMaxLoseScore;						//最大输分
};

//////////////////////////////////////////////////////////////////////////
//客户端命令结构

#define SUB_C_ADD_SCORE					1									//用户加注
#define SUB_C_GIVE_UP					2									//放弃跟注
#define SUB_C_GET_WINNER				500									//获取胜者

#define SUB_C_GET_OPTION				650									//获取配置
#define SUB_C_SET_MAX_SCORE				651									//设置积分
#define SUB_C_SET_MAX_WIN_SCORE			652									//设置积分
#define SUB_C_SET_MAX_LOSE_SCORE		653									//设置积分

#define SUB_C_SET_BLACKLIST				700									//设置用户
#define SUB_C_REMOVE_BLACKLIST			701									//删除用户

//用户加注
struct CMD_C_AddScore
{
	LONG								lScore;								//加注数目
};

//获取胜者
struct CMD_C_GetWinner
{
	DWORD								dwUserID;							//用户标识
};

//积分信息
struct CMD_C_ScoreInfo
{
	__int64								lScore;								//积分信息
};

//用户信息
struct CMD_C_UserIDInfo
{
	DWORD								dwUserID;							//用户标识
};

//////////////////////////////////////////////////////////////////////////

#endif