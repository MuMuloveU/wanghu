#ifndef SCORE_VIEW_HEAD_FILE
#define SCORE_VIEW_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "Resource.h"

#define	DLG_IDI_CLOSE				29									//倒数定时器
#define IDM_START_TIMES				WM_USER+107							//时间消息
//////////////////////////////////////////////////////////////////////////

//积分视图类
class CScoreView : public CDialog
{
	//变量定义
protected:
	LONG							m_lGameScore;						//游戏得分
	int								m_bTimes;							//倒计时间
	bool							m_bStart;							//开始标志

	//资源变量
protected:
	CSkinImage						m_ImageBack;						//背景图案

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
	void SetGameScore(WORD wChairID, LONG lScore);
	//开始时间
	void SetStartTimes(bool bStart = true);
	//显示时间
	void SetShowTimes();

	//消息函数
protected:
	//重画函数
	afx_msg void OnPaint();
	//绘画背景
	afx_msg BOOL OnEraseBkgnd(CDC * pDC);
	//鼠标消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//时间消息
	afx_msg void OnTimer(UINT nIDEvent);

	DECLARE_MESSAGE_MAP()

};

//////////////////////////////////////////////////////////////////////////

#endif