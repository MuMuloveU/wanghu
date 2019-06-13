#pragma once

#include "Stdafx.h"
#include "ChessBorad.h"
#include "DlgUserRequest.h"
#include "ScoreView.h"

//////////////////////////////////////////////////////////////////////////////////
//消息定义

#define IDM_PLACEMENT				(WM_APP+100)						//完成布局
#define WM_HIT_CHESS_BORAD			(WM_APP+101)						//点击棋盘
#define WM_CHANGE_CURSOR			(WM_APP+102)						//改变鼠标
#define WM_RESET_GAME				(WM_APP+103)						//重新游戏
#define WM_SAVE_PLACEMENT			(WM_APP+104)						//保存布局
#define WM_LOAD_PLACEMENT			(WM_APP+105)						//装载布局
#define	IDM_LOAD_HISTORY			(WM_APP+106)						//载入棋局
#define	IDM_PREV_STEP				(WM_APP+107)						//上一步棋
#define	IDM_NEXT_STEP				(WM_APP+108)						//下一步棋
#define	IDM_TURN_CHESS				(WM_APP+109)						//旋转棋盘
#define IDM_LOSE					(WM_APP+110)						//认输消息
#define IDM_GIVEUP					(WM_APP+111)						//放弃消息
#define IDM_PEACE					(WM_APP+112)						//和棋消息


//////////////////////////////////////////////////////////////////////////////////


//游戏视图
class CGameClientView : public CGameFrameView
{
	//变量定义
protected:
	BYTE							m_bFrameX ;							//边框位置
	BYTE							m_bFrameY ;							//边框位置
	bool							m_bDeasilOrder;						//走棋顺序
	LONG							m_lCurrentStepCount;				//当前步数

	//界面变量
protected:
	CSkinImage						m_ImageBack;						//背景图案


	//控件变量
public:
	CChessBorad						m_ChessBoard;						//游戏棋盘
	CDlgUserRequest					m_PeaceRequest;						//和棋请求

	CSkinButton						m_btPlacement;						//布局按钮
	CSkinButton						m_btSavePlacement;					//保存布局
	CSkinButton						m_btLoadPlacement;					//载入按钮
	CSkinButton						m_btResetGame;						//重新游戏
	CSkinButton						m_btLoadHistory;					//载入棋局
	CSkinButton						m_btPrevStep;						//上一步棋
	CSkinButton				    	m_btNextStep;						//下一步棋
	CSkinButton						m_btTurn;							//旋转棋盘
	CSkinButton						m_btLose;							//认输按钮
	CSkinButton						m_btGiveUp;							//放弃按钮
	CSkinButton						m_btPeace;							//和棋按钮

	CScoreView						m_ScoreView;						//积分视图

		#ifdef VIDEO_GAME
	//视频组件
private:
	CVideoServiceManager			m_VedioServiceManager;				//视频对象
	CVideoServiceControl			m_DlgVedioService[GAME_PLAYER];		//视频组件
#endif

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

	//功能函数
public:
	//和棋请求
	void ShowPeaceRequest();
	//艺术字体
	void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos) ;
	//绘画边框
	void DrawChessFrame(BYTE bXPos , BYTE bYPos) ;
	//走棋顺序
	void SetUserOrder(bool bDeasilOrder) ;
	//走棋顺序
	bool IsDeasilOrder(bool bDeasilOrder) ;
	//得到边框
	void GetChessFrame(BYTE &bXPos , BYTE &bYPos) ;
	//设置步数
	void SetStepCount(LONG lStepCount) ;

	//内部函数 
private:
	//更新状态
	void UpdateUserInfoArea();

	//控件消息
protected:
	//布局按钮
	void OnHitPlacement();
	//重新游戏
	void OnHitResetGame() ;
	//保存布局
	void OnSavePlacement() ;
	//装载布局
	void OnHitLoadPlacement() ;
	//载入棋局
	void OnHitLoadHistory();
	//上一步棋			
	void OnHitPrevStep();
	//下一步棋				
	void OnHitNextStep();
	//旋转棋盘				
	void OnHitTurn();	
	//认输按钮
	void OnHitLose();
	//放弃按钮
	void OnHitGiveUp();
	//请求和棋
	void OnHitPeace();

	//消息映射
protected:
	//建立函数
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};

//////////////////////////////////////////////////////////////////////////
