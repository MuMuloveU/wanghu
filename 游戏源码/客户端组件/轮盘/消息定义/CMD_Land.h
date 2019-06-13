#ifndef CMD_LAND_HEAD_FILE
#define CMD_LAND_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//公共宏定义

#define KIND_ID						404									//游戏 I D
#define GAME_PLAYER					100									//游戏人数
#define GAME_NAME					TEXT("轮盘")						//游戏名字

#define	HISTORY_NUMBER				13									//历史记录个数
#define	MAX_BET_POS					256									//最多下注位置
//游戏状态
#define GS_WK_FREE					GS_FREE								//等待开始


//////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_GAME_START			100									//游戏开始
#define SUB_S_USER_CHIP				101									//用户下注
#define SUB_S_STOP_ROLL				102									//停止旋转
#define SUB_S_GAME_END				105									//游戏结束

//空闲状态
struct CMD_S_StatusFree
{
	LONG							lCellScore;		//基础分数
	LONG							lUserScore;		//用户分数
	BYTE							cbTimeLeave;	//剩余时间
	BYTE							bHistoryInfo[HISTORY_NUMBER];		//历史信息
};

//游戏开始
struct CMD_S_GameStart
{
	BYTE							bWinNumber;	//胜利数字
};

//用户下注
struct CMD_S_UserChipIn
{
	LONG							lAllChips[256];	//所有下注
};

//胜利数字
struct CMD_S_StopRoll
{
	BYTE							bWinNumber;	//胜利数字
};

//游戏结束
struct CMD_S_GameEnd
{
	BYTE							bWinNumber;						//胜利数字
	LONG							lGameScore[GAME_PLAYER];		//胜利分数
	LONG							lMaxBetWinScore[GAME_PLAYER];		//最大下注
	LONG							lMaxUserScore[GAME_PLAYER];		//用户分数
	BYTE							bHistoryInfo[HISTORY_NUMBER];	//历史信息
};

//////////////////////////////////////////////////////////////////////////
//客户端命令结构

#define SUB_C_CHIP_IN			10									//用户叫分
#define SUB_C_ROLL_END			20									//胜利数字
#define SUB_C_GET_STOP			30									//得到停止消息

//用户叫分
struct CMD_C_ChipIn
{
	int							ChipsInfo[256];							//地主分数
};

//旋转结束
struct CMD_C_RollEnd
{
	BYTE						WinNumber;	
};


//////////////////////////////////////////////////////////////////////////

#endif