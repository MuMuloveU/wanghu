#ifndef SCORE_VIEW_HEAD_FILE
#define SCORE_VIEW_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "Resource.h"

//////////////////////////////////////////////////////////////////////////

//积分视图类
class CScoreView : public CDialog
{
	//变量定义
protected:
	LONG							m_lGameTax;							//游戏税收
	LONG							m_lGameScore[GAME_PLAYER];			//游戏得分
	TCHAR							m_szUserName[GAME_PLAYER][NAME_LEN];//用户名字

	//资源变量
protected:
	CSkinImage						m_ImageBack;						//背景图案
	CSkinImage						m_ImageWinLose;						//胜负标识

	//函数定义
public:
	//构造函数
	CScoreView(void);
	//析构函数
	virtual ~CScoreView(void);
	
	//重载函数
public:
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定消息
	virtual void OnOK() { return; }
	//取消消息
	virtual void OnCancel() { return; }

	//功能函数
public:
	//重置积分
	void ResetScore();
	//设置税收
	void SetGameTax(LONG lGameTax);
	//设置积分
	void SetGameScore(WORD wChairID, LPCTSTR pszUserName, LONG lScore);

	//消息函数
protected:
	//重画函数
	afx_msg void OnPaint();
	//绘画背景
	afx_msg BOOL OnEraseBkgnd(CDC * pDC);
	//鼠标消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif