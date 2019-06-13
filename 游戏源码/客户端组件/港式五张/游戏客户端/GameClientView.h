#pragma once

#include "Stdafx.h"
#include "ScoreView.h"
#include "CardControl.h"
#include "JettonControl.h"
#include "GoldControl.h"

//////////////////////////////////////////////////////////////////////////
//消息定义

//消息定义
#define IDM_START					WM_USER+100							//开始消息
#define IDM_NO_ADD					WM_USER+101							//不加消息
#define IDM_FOLLOW					WM_USER+102							//跟注消息
#define IDM_GIVE_UP					WM_USER+103							//放弃消息
#define IDM_ADD_SCORE				WM_USER+104							//加注消息
#define IDM_SHOW_HAND				WM_USER+105							//梭哈消息
#define IDM_SEND_CARD_FINISH		WM_USER+106							//发牌完成

//////////////////////////////////////////////////////////////////////////
//结构定义

//发牌子项
struct tagSendCardItem
{
	WORD							wChairID;							//发牌用户
	BYTE							cbCardData;							//发牌数据
};

//数组说明
typedef CArrayTemplate<tagSendCardItem,tagSendCardItem &> CSendCardItemArray;

//////////////////////////////////////////////////////////////////////////

//游戏视图
class CGameClientView : public CGameFrameView
{
	//数据变量
protected:
	LONG							m_lTableScore[GAME_PLAYER];			//下注数目
	LONG							m_lCellScore;						//单元注

	//动画变量
protected:
	CPoint							m_SendCardPos;						//发牌位置
	CPoint							m_SendCardCurPos;					//发牌当前位置
	INT								m_nStepCount;						//步数
	INT								m_nXStep;							//步长
	INT								m_nYStep;							//步长
	CSendCardItemArray				m_SendCardItemArray;				//发牌数组

	//按钮控件
public:
	CSkinButton						m_btStart;							//开始按钮
	CSkinButton						m_btNoAdd;							//不加按钮
	CSkinButton						m_btGiveUp;							//放弃按钮
	CSkinButton						m_btFollow;							//跟注按钮
	CSkinButton						m_btShowHand;						//梭哈按钮
	CSkinButton						m_btAddScore;						//加注按钮

	//位置信息
protected:
	CPoint							m_ptTableScore[GAME_PLAYER];		//当前下注信息

	//控件变量
public:
	CScoreView						m_ScoreView;						//成绩窗口
	CCardControl					m_CardControl[GAME_PLAYER];			//用户扑克
	CJettonControl					m_PlayerJeton[GAME_PLAYER+1];		//玩家筹码
	CGoldControl					m_GoldControl;						//下注框

	//界面变量
protected:
	CSkinImage						m_ImageCard;						//扑克资源
	CSkinImage						m_ImageViewBack;					//背景资源
	CSkinImage						m_ImageViewFill;					//填充资源
	CPngImage						m_PngNumber;						//数字位图

	//函数定义
public:
	//构造函数
	CGameClientView();
	//析构函数
	virtual ~CGameClientView();

	//重载函数
protected:
	//命令函数
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

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
	//动画判断
	bool IsDispatchCard();
	//完成发牌
	void FinishDispatchCard();
	//发送扑克
	void DispatchUserCard(WORD wChairID, BYTE cbCardData);
	//设置下注
	void SetUserTableScore(WORD wChairID, LONG lTableScore);
	//设置单元注
	void SetCellScore( LONG lCellScore );

	//辅助函数
protected:
	//艺术字体
	void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos);
	//艺术字体
	void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, LPRECT lpRect);

	//消息映射
protected:
	//建立函数
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//定时器消息
	afx_msg void OnTimer(UINT nIDEvent);
	//test
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
