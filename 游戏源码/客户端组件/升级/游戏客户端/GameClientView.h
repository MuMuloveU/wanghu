#pragma once

#include "Stdafx.h"
#include "ScoreView.h"
#include "CallCardWnd.h"
#include "CardControl.h"
#include "ConcealCardView.h"

//////////////////////////////////////////////////////////////////////////
//消息定义

//按钮消息
#define IDM_START					(WM_USER+100)						//开始消息
#define IDM_OUT_CARD				(WM_USER+101)						//出牌消息
#define IDM_OUT_PROMPT				(WM_USER+102)						//提示消息
#define IDM_SEND_CONCEAL			(WM_USER+103)						//留底消息

//功能按钮
#define IDM_CONCEAL_CARD			(WM_USER+110)						//查看底牌
#define IDM_REQUEST_LEAVE			(WM_USER+111)						//请求离开
#define IDM_LAST_TURN_CARD			(WM_USER+112)						//上轮扑克
#define IDM_TRUSTEE_CONTROL			(WM_USER+113)						//托管控制
#define IDM_RESPONSE_LEAVE			(WM_USER+114)						//请求离开回应

//test
#define IDM_TEST					(WM_USER+15)	

//////////////////////////////////////////////////////////////////////////

//游戏视图
class CGameClientView : public CGameFrameView
{
	//显示变量
protected:
	bool							m_bWaitConceal;						//等待标志
	bool							m_bLastTurnCard;					//上轮扑克

	//游戏变量
protected:
	WORD							m_wCardScore;						//玩家得分
	WORD							m_wBankerUser;						//庄家用户
	bool							m_bStrustee[GAME_PLAYER];			//托管标志
	//叫牌信息
protected:
	BYTE							m_cbCallColor;						//叫牌花色
	BYTE							m_cbCallCount;						//叫牌数目
	BYTE							m_cbMainValue;						//主牌数值
	WORD							m_wCallCardUser;					//叫牌用户
	BYTE							m_cbValueOrder[2];					//主牌数值

	//得分信息
protected:
	WORD							m_cbScoreCardCount;					//扑克数目
	BYTE							m_cbScoreCardData[2*12];			//扑克数据

	//位置信息
protected:
	CPoint							m_PointBanker[GAME_PLAYER];			//庄家位置
	CPoint							m_ptStrustee[GAME_PLAYER];			//托管标志

	//位图变量
protected:
	CSkinImage						m_ImageBack;						//背景资源
	CSkinImage						m_ImageCenter;						//居中背景
	CSkinImage						m_ImageBanker;						//庄家资源
	CSkinImage						m_ImageLastTurn;					//上轮标志
	CSkinImage						m_ImageColorFlag;					//花色标志
	CSkinImage						m_ImageWaitConceal;					//等待资源
	CSkinImage						m_ImageStatusInfo;					//得分状态
	CSkinImage						m_ImageScoreCard;					//得分牌
	CPngImage						m_PngStrustee;						//托管标志

	//视频控件
public:
#ifdef VIDEO_GAME
	CVideoServiceControl			m_DlgVedioService[4];				//视频窗口
#endif

	//按钮控件
public:
	CSkinButton						m_btStart;							//开始按钮
	CSkinButton						m_btOutCard;						//出牌按钮
	CSkinButton						m_btOutPrompt;						//提示按钮
	CSkinButton						m_btSendConceal;					//留底按钮

	//功能按钮
public:
	CSkinButton						m_btConcealCard;					//查看底牌
	CSkinButton						m_btRuquestLeave;					//请求离开
	CSkinButton						m_btLastTurnCard;					//上轮扑克
	CSkinButton						m_btStusteeControl;					//拖管控制

	//扑克控件
public:
	CCardControl					m_HandCardControl;					//手上扑克
	CCardControl					m_UserCardControl[4];				//扑克视图

	//控件组件
public:
	CScoreView						m_ScoreView;						//积分视图
	CToolTipCtrl					m_ToolTipCtrl;						//提示控件
	CCallCardWnd					m_CallCardWnd;						//叫牌窗口
	CConcealCardView				m_ConcealCardView;					//底牌窗口

	//函数定义
public:
	//构造函数
	CGameClientView();
	//析构函数
	virtual ~CGameClientView();

	//重载函数
public:
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	//继承函数
private:
	//重置界面
	virtual void ResetGameView();
	//调整控件
	virtual void RectifyGameView(int nWidth, int nHeight);
	//绘画界面
	virtual void DrawGameView(CDC * pDC, int nWidth, int nHeight);

	//游戏信息
public:
	//设置得分
	void SetCardScore(WORD wCardScore);
	//庄家用户
	void SetBankerUser(WORD wBankerUser);
	//设置等待
	void SetWaitConceal(bool bWaitConceal);
	//设置上轮
	void SetLastTurnCard(bool bLastTurnCard);
	//设置主牌
	void SetValueOrder(BYTE cbMainValue, BYTE cbValueMySelf, BYTE cbValueOther);
	//设置亮主
	void SetCallCardInfo(WORD wCallCardUser, BYTE cbCallColor, BYTE cbCallCount);
	//设置得分牌
	void SetScoreData( const BYTE cbScoreCard[],BYTE cbCardCount );
	//设置托管状态
	void SetStrustee( WORD wChairId,bool bStrustee );

	//辅助函数
protected:
	//艺术字体
	void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos);
	//艺术字体
	void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, LPRECT lpRect);


	//消息映射
protected:
	//开始按钮
	afx_msg void OnStart();
	//出牌按钮
	afx_msg void OnOutCard();
	//提示按钮
	afx_msg void OnOutPrompt();
	//留底按钮
	afx_msg void OnSendConceal();
	//查看底牌
	afx_msg void OnConcealCard();
	//请求离开
	afx_msg void OnRuquestLeave();
	//上轮扑克
	afx_msg void OnLastTurnCard();
	//拖管控制
	afx_msg void OnStusteeControl();
	//建立函数
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标消息
	afx_msg void OnRButtonUp(UINT nFlags, CPoint Point);
	//鼠标双击
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()

};

//////////////////////////////////////////////////////////////////////////
