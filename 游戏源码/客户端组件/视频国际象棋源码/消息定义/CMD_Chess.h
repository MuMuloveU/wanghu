#ifndef CMD_CHESS_HEAD_FILE
#define CMD_CHESS_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//公共宏定义

#ifdef VIDEO_GAME
#define KIND_ID						601									//视频游戏ID
#else 
#define KIND_ID						201									//游戏 I D
#endif

#define GAME_PLAYER					2									//游戏人数

#ifdef VIDEO_GAME
#define GAME_NAME					TEXT("视频国际象棋")					//游戏名字
#else 
#define GAME_NAME					TEXT("国际象棋")					//游戏名字
#endif

#define GAME_GENRE					GAME_GENRE_SCORE					//游戏类型

#define IS_VIDEO_GAME				0									//视频类型

//////////////////////////////////////////////////////////////////////////
//游戏定义

//棋子颜色
#define NO_CHESS					0									//没有棋子
#define BLACK_CHESS					1									//黑色棋子
#define WHITE_CHESS					2									//白色棋子

//棋子定义
#define CHESS_KING					1									//王
#define CHESS_QUEEN					2									//后
#define CHESS_ROOK					3									//车
#define CHESS_ELEPHANT				4									//象
#define CHESS_HORSE					5									//马
#define CHESS_SOLDIER				6									//兵

//请求状态
#define GU_WAIT_PEACE				0x01								//等待求和
#define GU_WAIT_REGRET				0x02								//等待悔棋

//失败原因
#define FR_COUNT_LIMIT				1									//次数限制
#define FR_PLAYER_OPPOSE			2									//玩家反对

//棋子定义
struct tagChessItem
{
	BYTE							cbXPos;								//棋子坐标
	BYTE							cbYPos;								//棋子坐标
	BYTE							cbColor;							//棋子颜色
	BYTE							cbChess;							//棋子权位
	BYTE							cbChessID;							//棋子标识
	WORD							wWalkCount;							//行走步数
};

//棋谱结构
struct tagChessManual
{
	//移动棋子
	BYTE							cbSourceID;							//棋子标识
	BYTE							cbSourceColor;						//棋子颜色
	BYTE							cbSourceChess;						//棋子权位
	BYTE							cbXSourceChessPos;					//棋子位置
	BYTE							cbYSourceChessPos;					//棋子位置
	BYTE							cbXTargetChessPos;					//棋子位置
	BYTE							cbYTargetChessPos;					//棋子位置

	//关联棋子
	BYTE							cbUniteID;							//棋子标识
	BYTE							cbUniteColor;						//棋子颜色
	BYTE							cbUniteChess;						//棋子权位
	BYTE							cbSwitchChess;						//升变棋子
	BYTE							cbXUniteChessPos;					//棋子位置
	BYTE							cbYUniteChessPos;					//棋子位置
};

//////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_GAME_START			100									//游戏开始
#define SUB_S_MOVE_CHESS			101									//移动棋子
#define SUB_S_REGRET_REQ			102									//悔棋请求
#define SUB_S_REGRET_FAILE			103									//悔棋失败
#define SUB_S_REGRET_RESULT			104									//悔棋结果
#define SUB_S_PEACE_REQ				105									//和棋请求
#define SUB_S_PEACE_ANSWER			106									//和棋应答
#define SUB_S_GAME_END				107									//游戏结束
#define SUB_S_CHESS_MANUAL			108									//棋谱信息

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
	WORD							wLeftClock[2];						//剩余时间
	WORD							cbBegStatus[2];						//请求状态
	WORD							wChessStepCount[2];					//棋子步数
	tagChessItem					ChessItemArray[2][16];				//棋子信息
};

//游戏开始
struct CMD_S_GameStart
{
	WORD							wGameClock;							//局时时间
	WORD							wBlackUser;							//黑棋玩家
};

//放置棋子
struct CMD_S_MoveChess
{
	BYTE							cbXSourcePos;						//棋子位置
	BYTE							cbYSourcePos;						//棋子位置
	BYTE							cbXTargetPos;						//棋子位置
	BYTE							cbYTargetPos;						//棋子位置
	BYTE							cbSwitchChess;						//变换棋子
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
	BYTE							cbRegretCount;						//悔棋数目
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

#define SUB_C_MOVE_CHESS			1									//移动棋子
#define SUB_C_REGRET_REQ			2									//悔棋请求
#define SUB_C_REGRET_ANSWER			3									//悔棋应答
#define SUB_C_PEACE_REQ				4									//和棋请求
#define SUB_C_PEACE_ANSWER			5									//和棋应答
#define SUB_C_GIVEUP_REQ			6									//认输请求

//移动棋子
struct CMD_C_MoveChess
{
	BYTE							cbXSourcePos;						//棋子位置
	BYTE							cbYSourcePos;						//棋子位置
	BYTE							cbXTargetPos;						//棋子位置
	BYTE							cbYTargetPos;						//棋子位置
	BYTE							cbSwitchChess;						//变换棋子
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