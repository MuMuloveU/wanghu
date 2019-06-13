#pragma once

#include "Stdafx.h"
#include "GameCard.h"
#include "NtList.h"
#include "SmallGameCard.h"
#include "HintInfo.h"
#include "ScoreView.h"
#include "GameCardEx.h"


//////////////////////////////////////////////////////////////////////////
#define IDM_BEGIN			WM_USER+120					//开始按钮消息
#define	IDM_HINT			WM_USER+121					//提示消息
#define IDM_GIVE_BACK		WM_USER+123					//留底牌按钮
#define	IDM_CHANGE_BACK		WM_USER+131					//换底
#define	IDM_GIVE_CARD		WM_USER+119					//回牌
#define IDM_OUT_CARD		WM_USER+124					//用户出牌
#define IDM_LOOK_LAST		WM_USER+125					//看上轮扑克
#define IDM_BACK_CARD		WM_USER+126					//底牌信息
#define	IDM_GIVE_UP			WM_USER+127					//放弃等待亮牌

#define IDM_AI  			WM_USER+130					//提前结束按钮
//////////////////////////////////////////////////////////////////////////

//游戏视图
class CGameClientView : public CGameFrameView
{
	//友元定义
	friend class CGameClientDlg;
protected:
	//////////////////////////
	//控件
	CSkinButton		m_btBegin;						//开始
	CSkinButton		m_btOutCard;					//出牌
	CSkinButton		m_btHint;						//提示


	CSkinButton		m_btAI;							//托管
	CSkinButton		m_btLastTurn;					//上一轮

	//视图
	CSkinImage		m_ImageBack;					//游戏背景
	CSkinImage		m_ImageBackCenter;				//中心背景

	CSkinImage		m_ImageNT;						//庄家标识
	CSkinImage		m_ImageNTLogo;					//亮主标识
	CSkinImage		m_ImageScore;					//分牌
	CSkinImage		m_ImagePublicCard;				//亮主牌背景

	CSkinImage		m_ImageFlower;					//分牌

	CSkinImage		m_ImageFullTimer;				//满时间条
	CSkinImage		m_ImageEmptyTimer;				//空
	float			m_Timer_Scale;					//时间

	//信息位置
	POINT			m_PtNt[GAME_PLAYER];			//庄家标识位置
	POINT			m_PtNtLogo[GAME_PLAYER];		//亮牌

	POINT			m_PtScoreCard[GAME_PLAYER];		//分牌位置

	POINT			m_PtPublicCard;					//亮牌位置

public:
	CRect			m_PlayRect;						//玩家活动区域
	CRect			m_Rct_DymTimer;					//时间条区域
	CRect			m_Rct_Info;						//提示信息区域

public:
	int				m_nCurScore[GAME_PLAYER];		//当局所检的分数

	bool			m_bSunShi;						//是否顺时针

	WORD			m_wBankerChair;					//庄家座位（均是物理座位）
	WORD			m_wViewBankerChair;				//视图位置

	WORD			m_wPublicChair;					//亮牌的座位
	WORD			m_wViewPublicChair;				//视图位置
	BYTE			m_bPublicCard;					//亮牌花色

	CGameCardEx		m_MeCard;						//我的扑克
	CGameCard		m_OutCard[GAME_PLAYER];			//出的扑克
	CSmallGameCard	m_PublicCard;					//所亮牌


	CSmallGameCard	m_Score_Card[GAME_PLAYER];		//分牌				

	CNtList			m_NtList;
	CHintInfo		m_dlgHintInfo;					//提示信息
	CScoreWnd       m_ScoreWnd;						//分数框


	//界面变量
protected:

	//控件变量
public:

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
	virtual void RectifyGameView(int cx, int cy);
	//绘画界面
	virtual void DrawGameView(CDC * pDC, int nWidth, int nHeight);

	
	//载入资源
	void	LoadResource();
public:
	//隐藏所有按钮
	void	HideAllButton();
	//显示亮牌信息
	void	SetShowInfo(const CString &str,bool bIsShow);

	//消息映射
protected:
	//建立函数
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	afx_msg void OnHitBegin();
	afx_msg void OnHitBackCard();

	afx_msg void OnHitOutCard();
	afx_msg void OnHint();
	afx_msg void OnHitLastTurn();
	afx_msg void OnHitLookBack();
	afx_msg void OnQuit_();
	afx_msg void OnAI();

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

//////////////////////////////////////////////////////////////////////////
