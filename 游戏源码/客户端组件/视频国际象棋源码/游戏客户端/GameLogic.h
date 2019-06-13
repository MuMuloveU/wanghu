#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//走棋类型
enum enMoveType
{
	enMoveType_Error,			//走棋错误
	enMoveType_Normal,			//普通走棋
	enMoveType_SoldierUp,		//兵的升变
	enMoveType_KingRook,		//王车易位
};

//////////////////////////////////////////////////////////////////////////

//游戏逻辑类
class CGameLogic
{
	//棋谱变量
public:
	tagChessItem *					m_pItemLately[2];					//最近走棋
	CArrayTemplate<tagChessManual>	m_ChessManualInfo;					//棋谱信息

	//棋盘变量
public:
	WORD							m_wStepCount[2];					//棋子步数
	tagChessItem					m_ChessItem[2][16];					//棋子信息
	tagChessItem *					m_ChessBorad[8][8];					//棋盘信息

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
	const tagChessManual * GetLastChessManual();
	//获取棋子
	const tagChessItem * GetChessItem(BYTE cbXPos, BYTE cbYPos);

	//管理函数
public:
	//重置棋盘
	void ResetChessBorad();
	//更新棋盘
	void UpdateChessBorad();
	//设置棋谱
	void InsertChessManual(tagChessManual ChessManual[], WORD wManualCount);

	//控制函数
public:
	//悔棋控制
	bool RegretChess(WORD wStepCount);
	//移动棋子
	enMoveType MoveChess(BYTE cbXSourcePos, BYTE cbYSourcePos, BYTE cbXTargetPos, BYTE cbYTargetPos, BYTE cbSwitchChess);

	//逻辑函数
public:
	//结束判断
	bool IsGameFinish(BYTE cbColor);
	//杀王判断
	bool IsKingDanger(BYTE cbColor);
	//危险判断
	bool IsChessDanger(BYTE cbXTargetPos, BYTE cbYTargetPos);
	//效验步法
	bool IsWalkLegality(BYTE cbXSourcePos, BYTE cbYSourcePos, BYTE cbXTargetPos, BYTE cbYTargetPos);

	//内部函数
private:
	//获取索引
	inline BYTE GetColorIndex(BYTE cbChessColor);
	//获取偏移
	inline BYTE GetExcursion(BYTE cbSourcePos, BYTE cbTargetPos);
	//阻塞分析
	inline bool ObstructAnalyse(BYTE cbXSourcePos, BYTE cbYSourcePos, BYTE cbXTargetPos, BYTE cbYTargetPos);
};

//////////////////////////////////////////////////////////////////////////
