#ifndef HISTORY_RECORD_HEAD_FILE
#define HISTORY_RECORD_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

//动作标志
#define AF_NULL						0										//没有动作
#define AF_PASS						1										//放弃动作
#define AF_OUT_CARD					2										//出牌动作

//消息定义
#define WM_ENABLE_HISTORY			(WM_USER+500)							//启用消息

//////////////////////////////////////////////////////////////////////////
//结构定义

//积分信息
struct tagHistoryScore
{
	LONGLONG						lTurnScore;								//积分信息
	LONGLONG						lCollectScore;							//积分信息
};

//////////////////////////////////////////////////////////////////////////

//历史扑克
class CHistoryCard
{
	//变量定义
protected:
	bool							m_bHistoryCard;							//历史标志
	BYTE							m_cbEventFlag[GAME_PLAYER][2];			//动作标志
	BYTE							m_cbCardCount[GAME_PLAYER][2];			//扑克数目
	BYTE							m_cbCardData[GAME_PLAYER][2][MAX_COUNT];//扑克数据

	//函数定义
public:
	//构造函数
	CHistoryCard();
	//析构函数
	virtual ~CHistoryCard();

	//功能函数
public:
	//重置数据
	VOID ResetData();
	//获取动作
	BYTE GetCurrentEvent(WORD wChairID);
	//获取动作
	BYTE GetHistoryEvent(WORD wChairID);
	//获取扑克
	BYTE GetCurrentCard(WORD wChairID, BYTE cbCardBuffer[], BYTE cbBufferCount);
	//获取扑克
	BYTE GetHistoryCard(WORD wChairID, BYTE cbCardBuffer[], BYTE cbBufferCount);

	//事件函数
public:
	//保存事件
	VOID OnEventSaveData(WORD wChairID);
	//用户放弃
	VOID OnEventUserPass(WORD wChairID);
	//用户出牌
	VOID OnEventUserOutCard(WORD wChairID, BYTE cbCardData[], BYTE cbCardCount);
};

//////////////////////////////////////////////////////////////////////////

#endif