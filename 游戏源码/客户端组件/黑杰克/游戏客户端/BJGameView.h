#pragma once

#include "Stdafx.h"
#include "GoldView.h"
#include "ScoreView.h"
#include "CardControl.h"
#include "GoldControl.h"
#include "..\消息定义\CMD_BlackJack.h"

//#define DEBUG_GAME
//游戏视图
class CBJGameView : public CGameFrameView
{
	//友元定义
	friend class CBJGameDlg;

	//视图变量
protected:
	LONG							m_lMaxGold;							//最高下注
	LONG							m_lBasicGold;						//最少分数

	//界面面量
protected:
	CSize							m_ImageSize;						//背景大小
	CSkinImage						m_ImageBack;						//游戏背景
	CSkinImage						m_ImageTitle;						//标题背景
	CSkinImage						m_ImageBanker;	    				//庄家标志
	CSkinImage						m_ImageArrowHead;    				//分牌标识
	CSkinImage						m_ImageCenter;
	CRect							m_Rct_Play;							//游戏区域

	//标识状态
protected:
	CSkinImage						m_ImageAddGoldMark;    				//加注状态
	CSkinImage						m_ImageGetCardMark;    				//要牌状态
	CSkinImage						m_ImageStopMark;    				//停牌状态
	CSkinImage						m_ImageBrustMark;    				//爆牌状态

    //位置变量
protected:
	POINT							m_ptMark[GAME_PLAYER];              //游戏庄家
	POINT							m_ptCardSum[GAME_PLAYER];		    //扑克总值
	CPoint							m_GoldStation[GAME_PLAYER*2];		//筹码位置
	WORD 						    m_wBankerStation;					//庄家位置

	//游戏变量
protected:
	BYTE 						    m_cbCutCardMark;					//分牌标识
	BYTE                            m_cbTopCardSum[GAME_PLAYER];        //扑克总值
	BYTE                            m_cbBottomCardSum[GAME_PLAYER];     //扑克总值
	BYTE							m_cbPlayStatus[GAME_PLAYER];		//游戏状态

	//控件变量
public:
	CGoldView						m_GoldView[GAME_PLAYER*2];			//用户筹码
	CCardControl					m_CardControl[GAME_PLAYER*2];		//扑克控件
	CScoreView						m_ScoreView;						//积分视图
	CGoldControl					m_GoldControl;						//加注视图

	//控件变量
protected:
	CSkinButton						m_btAddGold;						//下注按钮
	CSkinButton						m_btBegin;							//开始按钮
	CSkinButton						m_btGiveUp;							//放弃按钮
	CSkinButton						m_btGetCard;						//要牌按钮
	CSkinButton						m_btStop;							//停牌按钮
	CSkinButton						m_btDouble;							//双倍按钮
	CSkinButton						m_btCutCard;						//分牌按钮

	//函数定义
public:
	//构造函数
	CBJGameView();
	//析构函数
	virtual ~CBJGameView();

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

	//消息映射
protected:
	//开始按钮
	afx_msg void OnStart();
	//放弃按钮
	afx_msg void OnGiveUp();
	//下注按钮
	afx_msg void OnAddGold();
	//要牌按钮
	afx_msg void OnGetCard();
	//停牌按钮
	afx_msg void OnStopCard();
	//双倍按钮
	afx_msg void OnDoubleCard();
	//双倍按钮
	afx_msg void OnCutCard();
	//建立函数
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()

};

//////////////////////////////////////////////////////////////////////////
