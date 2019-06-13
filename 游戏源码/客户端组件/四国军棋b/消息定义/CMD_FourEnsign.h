#ifndef CMD_CHESS_HEAD_FILE
#define CMD_CHESS_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//公共宏定义

#ifdef VIDEO_GAME
#define KIND_ID						606									//视频游戏ID
#else 
#define KIND_ID						207						//游戏 I D
#endif

#define GAME_PLAYER					4									//游戏人数

#ifdef VIDEO_GAME
#define GAME_NAME					TEXT("视频四国军棋")					//游戏名字
#else 
#define GAME_NAME					TEXT("四国军棋")					//游戏名字
#endif

#define GAME_GENRE					GAME_GENRE_SCORE					//游戏类型

#define IS_VIDEO_GAME				0									//视频类型

//////////////////////////////////////////////////////////////////////////
//游戏状态
/*
#define GS_WK_FREE				    	GS_FREE								//等待开始
#define GS_WK_PLAYING			    	GS_PLAYING						//游戏进行*/



//////////////////////////////////////////////////////////////////////////

//游戏定义
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	服务器和客户端的坐标规定：和屏幕坐标一样，X从左到右，Y从上到下，
	在客户端：自己的棋盘坐标永远是:X:6~10 , Y:11~16
	数组下标：为了和坐标轴统一，游戏中棋子数组第一下标为列(表示X轴)，第二下标为行（表示Y轴）

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

//棋子颜色
#define CHESS_COLOR_NO				0									//错误棋子
#define CHESS_COLOR_GREEN			1									//绿色棋子
#define CHESS_COLOR_BLACK			2									//黑色棋子
#define CHESS_COLOR_BLUE			3									//蓝色棋子
#define CHESS_COLOR_RED				4									//红色棋子


//显示模式
#define CHESS_NO					0									//没有棋子
#define CHESS_BACK					1									//显示背面
#define CHESS_FORCE					2									//显示正面


//棋子定义
#define CHESS_SI_LING				12									//司令棋子
#define CHESS_JUN_ZHANG				11									//军长棋子
#define CHESS_SHI_ZHANG				10									//师长棋子
#define CHESS_LV_ZHANG				9									//旅长棋子
#define CHESS_TUAN_ZHANG			8									//团长棋子
#define CHESS_YING_ZHANG			7									//营长棋子
#define CHESS_LIAN_ZHANG			6									//连长棋子
#define CHESS_PAI_ZHANG				5									//排长棋子
#define CHESS_GONG_BING				4									//工兵棋子
#define CHESS_ZHA_DAN				3									//炸弹棋子
#define CHESS_DI_LEI				2									//地雷棋子
#define CHESS_JUN_QI				1									//军旗棋子
#define CHESS_NULL					0									//没有棋子

//////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////

//棋子定义
struct tagChessItem
{
	BYTE							bColor;								//棋子颜色
	BYTE							bChessID;							//棋子标识
	BYTE							bChessMode;							//显示模式
};

//棋谱结构
struct tagChessManual
{
	tagChessItem					tagSourceChess;						//起始棋子						

	tagChessItem					tagDesChess;						//目的棋子	
};

//////////////////////////////////////////////////////////////////////////

#define SUB_S_GAME_START			100									//游戏开始
#define	SUB_S_FINISH_PLACEMENT		101									//完成布局
#define SUB_S_MOVE_CHESS			102									//放置棋子
#define SUB_S_ONE_LOSE				103									//某人没棋
#define SUB_S_GAME_END				104									//游戏结束
#define SUB_S_GAME_TIME_OUT			105									//玩家超时
#define SUB_S_RECORD				106									//历史记录
#define SUB_S_CHESS_MANUAL			107									//发送棋谱
#define SUB_S_PLACEMENT				108									//发送布局
#define SUB_S_REQUEST_PEACE			109									//请求和棋
#define SUB_S_PEACE_FAIL			110									//和棋失败
#define SUB_S_USER_UP				111									//玩家起来

//游戏状态
struct CMD_S_StatusPlay
{
	WORD							wCurrentUser;						//当前用户
	tagChessItem					Chess[17][17];						//棋子信息
	LONG							lStepCount;							//当前步数
};

//玩家起来
struct CMD_S_UserUp
{
	WORD							wUser ;								//起来玩家
};

//游戏开始
struct CMD_S_GameStart
{
	WORD							wCurrentUser;						//当前玩家
};

//放置棋子
struct CMD_S_MoveChess
{
	BYTE							bXSourcePos;						//棋子位置
	BYTE							bYSourcePos;						//棋子位置
	BYTE							bXTargetPos;						//棋子位置
	BYTE							bYTargetPos;						//棋子位置
	BYTE							bSourceColor;						//棋子颜色
	BYTE							bTargetColor;						//棋子颜色
	WORD				 			wCurrentUser;						//当前玩家
	WORD							wMoveUser;							//走棋玩家
};

//游戏结束
struct CMD_S_GameEnd
{
	LONG							lUserScore[GAME_PLAYER];			//用户积分
};

//完成布局
struct CMD_S_FinishPlacement
{
	WORD							wCurrentUser;						//当前玩家
	BYTE							bColor;								//棋子颜色
	BYTE							bChessID[5][6];						//棋子标识
};

//某人没棋
struct CMD_S_OneLose
{
	WORD							wLoseUser;							//没棋玩家
	WORD							wCurrentUser;						//当前玩家
};

//玩家超时
struct CMD_S_GameTimeOut
{
	WORD							wCurrentUser;						//当前玩家
};

//请求和棋
struct CMD_S_RuquestPeace
{
	WORD							wRequestUser;						//请求玩家
};
//////////////////////////////////////////////////////////////////////////
//客户端命令结构

#define SUB_C_MOVE_CHESS			1									//移动棋子
#define SUB_C_FINISH_PLACEMENT		2									//完成布局
#define SUB_C_GAME_TIME_OUT			3									//玩家超时
#define SUB_C_USER_SURRENDER		4									//玩家认输
#define SUB_C_REQUEST_PEACE			5									//请求和棋
#define SUB_C_PEACE_ANSWER			6									//和棋应答


//和棋应答
struct CMD_C_PeaceAnswer
{
	bool							bAgree;								//和棋应答
};

//移动棋子
struct CMD_C_MoveChess
{
	BYTE							bXSourcePos;						//棋子位置
	BYTE							bYSourcePos;						//棋子位置
	BYTE							bXTargetPos;						//棋子位置
	BYTE							bYTargetPos;						//棋子位置
};

//完成布局
struct CMD_C_FinishPlacement
{
	BYTE							bColor;								//棋子颜色
	BYTE							bChessID[5][6];						//棋子标识
};
//////////////////////////////////////////////////////////////////////////

#endif