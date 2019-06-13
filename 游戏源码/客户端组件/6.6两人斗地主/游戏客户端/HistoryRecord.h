#ifndef HISTORY_RECORD_HEAD_FILE
#define HISTORY_RECORD_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////
//宏定义


//////////////////////////////////////////////////////////////////////////
//结构定义

//积分信息
struct tagHistoryScore
{
	LONGLONG						lTurnScore;								//积分信息
	LONGLONG						lCollectScore;							//积分信息
};

//////////////////////////////////////////////////////////////////////////

//历史积分
class CHistoryScore
{
	//变量定义
protected:
	tagHistoryScore					m_HistoryScore[GAME_PLAYER];			//积分信息

	//函数定义
public:
	//构造函数
	CHistoryScore();
	//析构函数
	virtual ~CHistoryScore();

	//功能函数
public:
	//重置数据
	VOID ResetData();
	//获取积分
	tagHistoryScore * GetHistoryScore(WORD wChairID);

	//事件函数
public:
	//用户进入
	VOID OnEventUserEnter(WORD wChairID);
	//用户离开
	VOID OnEventUserLeave(WORD wChairID);
	//用户积分
	VOID OnEventUserScore(WORD wChairID, LONGLONG lGameScore);
};

//////////////////////////////////////////////////////////////////////////

#endif