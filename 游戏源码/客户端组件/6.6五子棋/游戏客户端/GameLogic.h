#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//棋谱结构
struct tagChessManual
{
	BYTE							cbXPos;								//下棋位置
	BYTE							cbYPos;								//下棋位置
	BYTE							cbColor;							//棋子颜色
};

//////////////////////////////////////////////////////////////////////////

//游戏逻辑类
class CGameLogic
{
	//棋谱变量
public:
	BYTE							m_cbStepCount[2];					//棋子数目
	BYTE							m_cbIdentifier[15][15];				//棋子标识
	BYTE							m_cbChessBorad[15][15];				//棋子信息

	//棋谱变量
public:
	CArrayTemplate<tagChessManual>	m_ChessManualInfo;					//棋谱信息

	//函数定义
public:
	//构造函数
	CGameLogic();
	//析构函数
	virtual ~CGameLogic();

	//信息函数
public:
	//获取步数
	BYTE GetStepCount(BYTE cbColor);
	//获取棋谱
	const tagChessManual * GetLastChessManual();

	//控制函数
public:
	//初始逻辑
	void InitGameLogic();
	//悔棋控制
	bool RegretChess(WORD wStepCount);
	//放置棋子
	BYTE PlaceChess(BYTE cbXPos, BYTE cbYPos, BYTE cbColor);

	//逻辑函数
public:
	//胜利判断
	bool WinEstimate(BYTE cbXPos, BYTE cbYPos);
};

//////////////////////////////////////////////////////////////////////////

#endif