#ifndef CMD_GOBANG_HEAD_FILE
#define CMD_GOBANG_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//基础定义

#define KIND_ID						203									//游戏 I D
#define GAME_PLAYER					2									//游戏人数
#define GAME_NAME					TEXT("五子棋")						//游戏名字

//////////////////////////////////////////////////////////////////////////
//游戏定义

//棋子颜色
#define NO_CHESS					0									//没有棋子
#define BLACK_CHESS					1									//黑色棋子
#define WHITE_CHESS					2									//白色棋子

//请求状态
#define GU_WAIT_PEACE				0x01								//等待求和
#define GU_WAIT_REGRET				0x02								//等待悔棋

//失败原因
#define FR_COUNT_LIMIT				1									//次数限制
#define FR_PLAYER_OPPOSE			2									//玩家反对

//////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_GAME_START			100									//游戏开始
#define SUB_S_PLACE_CHESS			101									//放置棋子
#define SUB_S_REGRET_REQ			102									//悔棋请求
#define SUB_S_REGRET_FAILE			103									//悔棋失败
#define SUB_S_REGRET_RESULT			104									//悔棋结果
#define SUB_S_PEACE_REQ				105									//和棋请求
#define SUB_S_PEACE_ANSWER			106									//和棋应答
#define SUB_S_BLACK_TRADE			107									//交换对家
#define SUB_S_GAME_END				108									//游戏结束
#define SUB_S_CHESS_MANUAL			109									//棋谱信息

//游戏状态
struct CMD_S_StatusFree
{
	WORD							wBlackUser;							//黑棋玩家
};

//游戏状态
struct CMD_S_StatusPlay
{
	WORD							wGameClock;							//局时时间
	WORD							wBlackUser;							//黑棋玩家
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							cbRestrict;							//是否禁手
	BYTE							cbTradeUser;						//是否对换
	BYTE							cbDoubleChess;						//是否双打
	WORD							wLeftClock[2];						//剩余时间
	WORD							cbBegStatus[2];						//请求状态
};

//游戏开始
struct CMD_S_GameStart
{
	WORD							wGameClock;							//局时时间
	WORD							wBlackUser;							//黑棋玩家
	BYTE							cbRestrict;							//是否禁手
	BYTE							cbTradeUser;						//是否对换
	BYTE							cbDoubleChess;						//是否双打
};

//放置棋子
struct CMD_S_PlaceChess
{
	BYTE							cbXPos;								//棋子位置
	BYTE							cbYPos;								//棋子位置
	WORD				 			wPlaceUser;							//放棋玩家
	WORD				 			wCurrentUser;						//当前玩家
	WORD							wLeftClock[2];						//局时时间
};

//悔棋失败
struct CMD_S_RegretFaile
{
	BYTE							cbFaileReason;						//失败原因
};

//悔棋结果
struct CMD_S_RegretResult
{
	WORD							wRegretUser;						//悔棋玩家
	WORD							wCurrentUser;						//当前玩家
	WORD							wRegretCount;						//悔棋数目
	WORD							wLeftClock[2];						//局时时间
};

//游戏结束
struct CMD_S_GameEnd
{
	WORD							wWinUser;							//胜利玩家
	LONG							lUserScore[2];						//用户积分
};

//////////////////////////////////////////////////////////////////////////
//客户端命令结构

#define SUB_C_PLACE_CHESS			1									//放置棋子
#define SUB_C_REGRET_REQ			2									//悔棋请求
#define SUB_C_REGRET_ANSWER			3									//悔棋应答
#define SUB_C_PEACE_REQ				4									//和棋请求
#define SUB_C_PEACE_ANSWER			5									//和棋应答
#define SUB_C_GIVEUP_REQ			6									//认输请求
#define SUB_C_TRADE_REQ				7									//交换请求

//放置棋子
struct CMD_C_PlaceChess
{
	BYTE							cbXPos;								//棋子位置
	BYTE							cbYPos;								//棋子位置
};

//悔棋响应
struct CMD_C_RegretAnswer
{
	BYTE							cbApprove;							//同意标志
};

//和棋响应
struct CMD_C_PeaceAnswer
{
	BYTE							cbApprove;							//同意标志
};

//////////////////////////////////////////////////////////////////////////

#endif