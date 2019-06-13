#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//棋盘定义
#define COUNT_X						9									//棋格数目
#define COUNT_Y						10									//棋格数目
#define COUNT_CHESS					16									//棋子数目				

//棋子颜色
#define CHESS_NULL					0									//没有棋子
#define CHESS_BLACK					1									//黑色棋子
#define CHESS_WHITE					2									//白色棋子

//棋子定义
#define CHESS_GENERAL				1									//将
#define CHESS_KAVASS				2									//士
#define CHESS_ELEPHANT				3									//象
#define CHESS_HORSE					4									//马
#define CHESS_ROOK					5									//车
#define CHESS_CANNON				6									//炮
#define CHESS_SOLDIER				7									//兵

//////////////////////////////////////////////////////////////////////////

//棋子定义
struct tagChessItem
{
	BYTE							cbXPos;								//棋子坐标
	BYTE							cbYPos;								//棋子坐标
	BYTE							cbColor;							//棋子颜色
	BYTE							cbChess;							//棋子权位
	BYTE							cbChessID;							//棋子标识
};

//棋谱结构
struct tagChessManual
{
	BYTE							cbXSourceChessPos;					//棋子位置
	BYTE							cbYSourceChessPos;					//棋子位置
	BYTE							cbXTargetChessPos;					//棋子位置
	BYTE							cbYTargetChessPos;					//棋子位置
};

//捉棋结果
struct tagCatchChessInfo
{
	BYTE							cbSourceCount;						//捉子数目
	tagChessItem *					pTargetChessItem;					//捉棋指针
	tagChessItem *					pSourceChessItem[18];				//捉子指针
};

//将军结果
struct tagGeneralChessInfo
{
	BYTE							cbSourceCount;						//将军数目
	tagChessItem *					pSourceChessItem[18];				//将军指针
};

//走棋结果
struct tagMoveChessResult
{
	bool							bCatchChess;						//捉子标志
	bool							bMinatoryKing;						//将军标志
};

//结束信息
struct tagConcludeInfo
{
	bool							bGameConclude;						//结束标志
	BYTE							cbWinnerColor;						//胜利玩家
	BYTE							cbReason;							//结束理由
};

//////////////////////////////////////////////////////////////////////////

//棋谱定义
typedef	CArrayTemplate<tagChessManual,tagChessManual &>	CChessManualArray;

//游戏逻辑类
class CGameLogic
{
	//棋盘变量
public:
	tagChessItem					m_ChessItem[2][COUNT_CHESS];		//棋子信息
	tagChessItem *					m_ChessBorad[COUNT_X][COUNT_Y];		//棋盘信息
	//tagChessItem *					m_KeppChessBorad[COUNT_X][COUNT_Y];	//棋盘信息

	//状态变量
public:
	WORD							m_wNullStep;						//空走步数
	tagCatchChessInfo				m_CatchChessInfo[2];				//捉子信息
	tagGeneralChessInfo				m_GeneralChessInfo[2];				//将军信息

	//棋谱变量
public:
	CChessManualArray				m_ChessManualArray;					//棋谱信息

	//函数定义
public:
	//构造函数
	CGameLogic();
	//析构函数
	virtual ~CGameLogic();

	//信息函数
public:
	//获取步数
	WORD GetStepCount(BYTE cbColor);
	//获取棋谱
	tagChessManual * GetLastChessManual();
	//获取棋子
	tagChessItem * GetChessItem(BYTE cbXPos, BYTE cbYPos);

	//管理函数
public:
	//重置棋盘
	VOID ResetChessBorad();
	//历史状态
	VOID GetHistoryStatus(tagChessItem ChessItem[2][COUNT_CHESS], tagChessItem * ChessBorad[COUNT_X][COUNT_Y], WORD wStepCount);

	//控制函数
public:
	//悔棋控制
	bool RegretChess(WORD wTargetStep);
	//移动棋子
	bool MoveChess(BYTE cbXSourcePos, BYTE cbYSourcePos, BYTE cbXTargetPos, BYTE cbYTargetPos, tagMoveChessResult & MoveChessResult, LPTSTR pszManualName);

	//逻辑函数
public:
	//结束判断
	bool IsGameFinish(BYTE cbMoveColor, tagConcludeInfo & ConcludeInfo);
	//效验步法
	bool IsWalkLegality(const tagChessItem * pSourceChessItem, BYTE cbXTargetPos, BYTE cbYTargetPos);
	//判断将军
	bool IsKillGeneral(BYTE cbXSourcePos, BYTE cbYSourcePos, BYTE cbXTargetPos, BYTE cbYTargetPos);
	//判断死棋
	bool IsLoseChess(BYTE cbColor);

	//内部函数
private:
	//获取索引
	inline BYTE GetColorIndex(BYTE cbChessColor);
	//获取偏移
	inline BYTE GetExcursion(BYTE cbSourcePos, BYTE cbTargetPos);

	//内部控制
private:
	//复位棋盘
	bool InitChessBorad(tagChessItem ChessItem[2][COUNT_CHESS], tagChessItem * ChessBorad[COUNT_X][COUNT_Y]);
	//棋谱名字
	bool GetManualItemName(BYTE cbXSourcePos, BYTE cbYSourcePos, BYTE cbXTargetPos, BYTE cbYTargetPos, LPTSTR pszManualName);
	//执行走棋
	bool PerformMoveChess(BYTE cbXSourcePos, BYTE cbYSourcePos, BYTE cbXTargetPos, BYTE cbYTargetPos, tagMoveChessResult & MoveChessResult);

	//辅助函数
private:
	//威胁棋子
	tagChessItem * GetMinatoryChess(BYTE cbXTargetPos, BYTE cbYTargetPos);
	//有根判断
	tagChessItem * GetProtectedChess(BYTE cbXTargetPos, BYTE cbYTargetPos);
	//阻塞分析
	bool ObstructAnalyse(BYTE cbXSourcePos, BYTE cbYSourcePos, BYTE cbXTargetPos, BYTE cbYTargetPos);
};

//////////////////////////////////////////////////////////////////////////

#endif
