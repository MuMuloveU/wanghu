#pragma once

#include "Stdafx.h"
#include "GoldView.h"
#include "ScoreView.h"
#include "CardControl.h"

//////////////////////////////////////////////////////////////////////////

//消息定义
#define IDM_START						WM_USER+100						//开始消息
#define IDM_ADD_GOLD					WM_USER+101						//加注消息
#define IDM_FOLLOW						WM_USER+102						//跟注消息
#define IDM_GIVE_UP						WM_USER+103						//放弃消息
#define IDM_SHOWHAND					WM_USER+104						//梭哈消息
#define IDM_SEND_CARD_FINISH			WM_USER+106						//发牌完成

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
	//友元定义
	friend class CGameClientDlg;

	//视图变量
protected:
	LONG							m_lMaxGold;							//最高下注
	LONG							m_lBasicGold;						//最少分数
	CPoint							m_GoldStation[8];					//筹码位置
	bool							m_bShowScoreControl;				//

	//动画变量
protected:
	CPoint							m_SendCardPos;						//发牌位置
	CPoint							m_SendCardCurPos;					//发牌当前位置
	INT								m_nStepCount;						//步数
	INT								m_nXStep;							//步长
	INT								m_nYStep;							//步长
	CSendCardItemArray				m_SendCardItemArray;				//发牌数组

	//界面变量
protected:
	CSkinImage						m_ImageCardBack;					//扑克资源
	CSkinImage						m_ImageTitle;						//标题位图
	CSkinImage						m_ImageViewFill;					//背景位图
	CSkinImage						m_ImageViewCenter;					//背景位图
	CSkinImage						m_ImageScoreControlBack;			//
	CSkinImage						m_ImageScoreTable;					//
	CPngImage						m_PngBkTopLeft;						//
	CPngImage						m_PngBkTopRight;					//
	CPngImage						m_PngBkBottomLeft;					//
	CPngImage						m_PngBkBottomRight;					//

	//控件变量
public:
	CSkinButton						m_btStart;							//开始按钮
	CSkinButton						m_btFollow;							//跟注按钮
	CSkinButton						m_btGiveUp;							//放弃按钮
	CSkinButton						m_btShowHand;						//梭哈按钮
	CSkinButton						m_btAddTimes3;						//
	CSkinButton						m_btAddTimes2;						//
	CSkinButton						m_btAddTimes1;						//
	CGoldView						m_GoldView[8];						//用户筹码
	CGoldView						m_TotalGoldView;					//
	CScoreView						m_ScoreView;						//积分视图
	CCardControl					m_CardControl[4];					//扑克控件

	//控件位置
protected:
	CPoint							m_ptScoreControl;					//

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
	//金币标题
	void SetGoldTitleInfo(DWORD lMaxGold, DWORD lBasicGold);
	//用户筹码
	void SetUserGoldInfo(WORD wViewChairID, bool bTableGold, DWORD dwGold);
	//动画判断
	bool IsDispatchCard();
	//完成发牌
	void FinishDispatchCard();
	//发送扑克
	void DispatchUserCard(WORD wChairID, BYTE cbCardData);
	//
	VOID ShowScoreControl( bool bShow );

	//消息映射
protected:
	//开始按钮
	afx_msg void OnStart();
	//跟注按钮
	afx_msg void OnFollow();
	//放弃按钮
	afx_msg void OnGiveUp();
	//加注按钮
	afx_msg void OnAddTimes1();
	//加注按钮
	afx_msg void OnAddTimes2();
	//加注按钮
	afx_msg void OnAddTimes3();
	//梭哈按钮
	afx_msg void OnShowHand();
	//建立函数
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//定时器消息
	afx_msg void OnTimer(UINT nIDEvent);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
