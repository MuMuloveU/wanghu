#ifndef GAME_VIEW_HEAD_FILE
#define GAME_VIEW_HEAD_FILE

#pragma once

#include "StdAfx.h"
#include "Ball.h"
#include "Station.h"
#include "GameThreadDraw.h"
#include "PeaceDlg.h"
#include "ScoreWnd.h"
#include "GameRuleSet.h"


#define BACK_COLOR				RGB(15,64,85)			//底色

//状态定义
#define NO_STATE				0						//没有状态
#define BALL_RUN_ING			1						//运行中
#define ME_HIT_BALL				2						//玩家打球
#define OTHER_HIT_BALL			3						//对方打球
#define WATI_SET_BALL			4						//设置球位置
#define  WIDTH_SCORE                275
#define  HEIGHT_SCORE               172


#define STATION_WND_HIGHT			80			//状态窗口高度 
#define PIC_BALL_RADII				21		//图球半径
#define ID_TIMER					10			//定时器 

class CGameClientView :public CGameFrameView
{
	friend class CGameClientDlg;
	friend class CStation;
	friend class CGameThreadDraw;

	//变量定义
public:
	bool					m_bMouseDown;				//鼠标是否按下
	HCURSOR					m_hMouse;					//鼠标光标
	CScoreWnd					m_ScoreWnd;				//显示得分的窗口
	//HANDLE					m_hGDIThreadHandle;			//线程句柄

public:
	//
	//构造析构
	//
	CGameClientView(void);
	virtual ~CGameClientView(void);
	private:
	virtual void ResetGameView();
											//重置界面
	virtual void RectifyGameView(int nWidth, 
									int nHeight);
											//调整控件
	virtual void DrawGameView(CDC * pDC, 
								int nWidth, 
								int nHeight);
											//绘画界面
private:
	CSkinButton		m_GiveUp;
	CSkinButton		m_Replace;
	CSkinButton		m_Concession;
	CSkinButton		m_Begin;
	CPeaceDlg		m_PeaceDlg;				//求和对话框
	//CGameRuleSet				m_RuleInfo;					//规则设置
	CGameThreadDraw     m_ThreadDraw;
	
	//CRect			m_Rct_Play;
	CRect           m_ClientRect;
	//控件变量
public:
	CStation				m_Station;					//状态窗口
		//函数定义
public:
	//获取 X 相对偏移
	int GetXBase(); 
	//获取 Y 相对偏移
	int GetYBase();

	//重载函数
public:

	//消息函数
protected:
	//建立函数
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//设置光标
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//鼠标起来
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//鼠标按下
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	void		OnBegin(void);
	void		OnGiveUp(void);
	void		OnReplace(void);
	void		OnConcession(void);
DECLARE_MESSAGE_MAP()

};


#endif
