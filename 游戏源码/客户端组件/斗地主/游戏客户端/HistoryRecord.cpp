#include "StdAfx.h"
#include "HistoryRecord.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

//////////////////////////////////////////////////////////////////////////

//构造函数
CHistoryScore::CHistoryScore()
{
	//设置变量
	ZeroMemory(m_HistoryScore,sizeof(m_HistoryScore));

	return;
}

//析构函数
CHistoryScore::~CHistoryScore()
{
}

//重置数据
VOID CHistoryScore::ResetData()
{
	//设置变量
	ZeroMemory(m_HistoryScore,sizeof(m_HistoryScore));

	return;
}

//获取积分
tagHistoryScore * CHistoryScore::GetHistoryScore(WORD wChairID)
{
	//效验参数
	ASSERT(wChairID<GAME_PLAYER);
	if (wChairID>=GAME_PLAYER) return NULL;

	//获取积分
	return &m_HistoryScore[wChairID];
}

//用户进入
VOID CHistoryScore::OnEventUserEnter(WORD wChairID)
{
	//效验椅子
	ASSERT(wChairID<GAME_PLAYER);
	if (wChairID>=GAME_PLAYER) return;
	
	//设置变量
	m_HistoryScore[wChairID].lTurnScore=0;
	m_HistoryScore[wChairID].lCollectScore=0;

	return;
}

//用户离开
VOID CHistoryScore::OnEventUserLeave(WORD wChairID)
{
	//效验椅子
	ASSERT(wChairID<GAME_PLAYER);
	if (wChairID>=GAME_PLAYER) return;
	
	//设置变量
	m_HistoryScore[wChairID].lTurnScore=0;
	m_HistoryScore[wChairID].lCollectScore=0;

	return;
}

//用户积分
VOID CHistoryScore::OnEventUserScore(WORD wChairID, LONGLONG lGameScore)
{
	//效验参数
	ASSERT(wChairID<GAME_PLAYER);
	if (wChairID>=GAME_PLAYER) return;

	//设置变量
	m_HistoryScore[wChairID].lTurnScore=lGameScore;
	m_HistoryScore[wChairID].lCollectScore+=lGameScore;

	return;
}

//////////////////////////////////////////////////////////////////////////
