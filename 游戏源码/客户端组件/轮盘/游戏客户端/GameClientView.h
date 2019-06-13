#pragma once

#include "Stdafx.h"
#include "ScoreView.h"
#include "dxgameview.h"

//////////////////////////////////////////////////////////////////////////
//消息定义

#define IDM_START					(WM_USER+100)						//开始消息

//////////////////////////////////////////////////////////////////////////

//游戏视图
class CGameClientView : public CGameFrameView
{
	//游戏变量
protected:


	//配置变量
protected:
	bool							m_bDeasilOrder;						//出牌顺序

	//状态变量
public:


	//位置信息
public:

	//位图变量
protected:


	//按钮控件
public:


	//扑克控件
public:


	//控件组件
public:
	CScoreView						m_ScoreView;						//积分视图
	CDXGameView						m_DxDialog;							//d3d窗口

	//函数定义
public:
	//构造函数
	CGameClientView();
	//析构函数
	virtual ~CGameClientView();

	//继承函数
private:
	//重置界面
	virtual void ResetGameView();
	//调整控件
	virtual void RectifyGameView(int nWidth, int nHeight);
	//绘画界面
	virtual void DrawGameView(CDC * pDC, int nWidth, int nHeight);

	//配置函数
public:
	//用户顺序
	void SetUserOrder(bool bDeasilOrder);
	//获取顺序
	bool IsDeasilOrder() { return m_bDeasilOrder; }

	//功能函数
public:
	void SetUserCome(BYTE bHistoryInfo[HISTORY_NUMBER],long lMinBetScore,long lMaxBetWinScore,long lMaxUserScore,bool bLookOnUser=false);
	void SetGameStart(BYTE bTargetNumber);
	void SetUserChip(long lAllChip[256]);
	void SetRollStop(BYTE bWinNumber);
	void SetGameEnd(long lScore);
	void SetCellScore(long lCellScore);
	//消息映射
protected:
	//开始按钮
	afx_msg void OnStart();

	//建立函数
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	afx_msg void OnTimer(UINT_PTR nIDEvent);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
