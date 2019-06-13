#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//区域标志
#define AREA_UNKNOW					0									//未知区域
#define AREA_CHESS					1									//棋子区域
#define AREA_BURROW					2									//洞穴区域

//点目标志
#define CT_UNKNOW					0x00								//未被统计
#define CT_BLACE_CHESS				0x01								//黑棋区域
#define CT_WHITE_CHESS				0x02								//白棋区域

//////////////////////////////////////////////////////////////////////////

//区域结果
struct tagAreaResult
{
	WORD							wChessCount;						//棋子数目
	WORD							wBurrowCount;						//洞穴数目
	WORD							wChessList[361];					//区域棋子
	BYTE							cbAreaResult[19][19];				//区域信息
};

//点目结果
struct tagCountResult
{
	WORD							wCommomCount;						//公共数目
	WORD							wChessCount[2];						//棋子数目
	BYTE							cbCountInfo[19][19];				//点目结果
};

//点目辅助
struct tagCountAssistant
{
	BYTE							cbCurIndex;							//当前索引
	BYTE							cbChessMask;						//棋子掩码
	WORD							wChessCount[2];						//棋子数目
	WORD							wChessList[2][361];					//区域棋子
};

//提子结构
struct tagChessItem
{
	BYTE							cbXPos;								//棋子位置
	BYTE							cbYPos;								//棋子位置
	WORD							wIdentifier;						//棋子标识
};

//棋谱结构
struct tagChessManual
{
	BYTE							cbXPos;								//下棋位置
	BYTE							cbYPos;								//下棋位置
	BYTE							cbColor;							//棋子颜色
	WORD							wDeadChessCount;					//围棋数目
};

//////////////////////////////////////////////////////////////////////////

//游戏逻辑类
class CGameLogic
{
	//状态变量
protected:
	WORD							m_wStepCount[2];					//棋步数目
	WORD							m_wTakeChessCount[2];				//提子数目
	BYTE							m_cbLastChessBorad[2][19][19];		//历史信息

	//棋盘变量
public:
	WORD							m_wIdentifier[19][19];				//棋子标识
	BYTE							m_cbChessBorad[19][19];				//棋子信息

	//棋谱变量
public:
	CArrayTemplate<tagChessItem>	m_DeadChessInfo;					//提子信息
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
	WORD GetStepCount(BYTE cbColor);
	//提子数目
	WORD GetTakeChessCount(BYTE cbColor);
	//获取棋谱
	const tagChessManual * GetLastChessManual();

	//控制函数
public:
	//初始逻辑
	void InitGameLogic();
	//悔棋控制
	bool RegretChess(WORD wStepCount);
	//点目统计
    bool GetChessCountInfo(tagCountResult & CountResult);
	//放置棋子
	WORD PlaceChess(BYTE cbXPos, BYTE cbYPos, BYTE cbColor, WORD wDeadChess[361], WORD & wDeadChessCount);

	//逻辑函数
protected:
	//提子处理
	WORD TakeOutChess(BYTE cbXPos, BYTE cbYPos, WORD wDeadChess[361]);
	//获取区域
	void GetChessArea(BYTE cbXPos, BYTE cbYPos, tagAreaResult & AreaResult);
	//十字区域
	void GetCrossArea(BYTE cbXPos, BYTE cbYPos, tagAreaResult & AreaResult);
	//十字扩张
	void GetCrossExpand(BYTE cbXPos, BYTE cbYPos, tagCountResult & CountResult, tagCountAssistant & CountAssistant);
};

//////////////////////////////////////////////////////////////////////////

#endif