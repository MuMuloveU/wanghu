#ifndef SCORE_VIEW_HEAD_FILE
#define SCORE_VIEW_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "Resource.h"
#include "CardControl.h"

//////////////////////////////////////////////////////////////////////////

//结束信息
struct tagScoreViewInfo
{
	WORD							wGameScore;							//游戏得分
	WORD							wConcealTime;						//扣底倍数
	WORD							wConcealScore;						//底牌积分
	BYTE							cbConcealCount;						//暗藏数目
	BYTE							cbConcealCard[8];					//暗藏扑克
};

//////////////////////////////////////////////////////////////////////////

//积分视图类
class CScoreView : public CWnd
{
	//结束信息
protected:
	WORD							m_wGameScore;						//游戏得分
	WORD							m_wConcealTime;						//扣底倍数
	WORD							m_wConcealScore;					//底牌积分
	BYTE							m_cbConcealCount;					//暗藏数目
	BYTE							m_cbConcealCard[8];					//暗藏扑克

	//积分信息
protected:
	LONG							m_lScore[GAME_PLAYER];				//游戏得分
	TCHAR							m_szUserName[GAME_PLAYER][NAME_LEN];//用户名字

	//资源变量
protected:
	CSize							m_CardSize;							//扑克大小
	CSkinImage						m_ImageBack;						//背景图案
	CSkinImage						m_ImageCard;						//扑克位图
	CSkinImage						m_ImageWinLose;						//胜负标志

	//函数定义
public:
	//构造函数
	CScoreView();
	//析构函数
	virtual ~CScoreView();
	
	//功能函数
public:
	//设置信息
	void SetScoreViewInfo(tagScoreViewInfo & ScoreViewInfo);
	//设置积分
	void SetGameScore(WORD wChairID, LPCTSTR pszUserName, LONG lScore);

	//消息函数
protected:
	//重画函数
	afx_msg void OnPaint();
	//建立消息
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint Point);
	//鼠标激活
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif