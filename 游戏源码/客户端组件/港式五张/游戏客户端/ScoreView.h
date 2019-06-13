#ifndef SCORE_VIEW_HEAD_FILE
#define SCORE_VIEW_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "Resource.h"

//////////////////////////////////////////////////////////////////////////

//积分视图类
class CScoreView : public CWnd
{
	//变量定义
protected:
	LONG							m_lGameTax[GAME_PLAYER];			//游戏税收
	LONG							m_lScore[GAME_PLAYER];				//游戏得分
	TCHAR							m_szUserName[GAME_PLAYER][NAME_LEN];//用户名字

	//资源变量
protected:
	CSkinImage						m_ImageBack;						//背景图案
	CSkinImage						m_ImageWinLose;						//胜负标志

	//函数定义
public:
	//构造函数
	CScoreView();
	//析构函数
	virtual ~CScoreView();
	
	//功能函数
public:
	//重置积分
	void ResetScore();
	//设置税收
	void SetGameTax(WORD wChairID,LONG lGameTax);
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

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif