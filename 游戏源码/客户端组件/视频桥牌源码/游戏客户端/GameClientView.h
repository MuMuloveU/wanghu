#include "../../../开发库/Include/SkinButton.h"

#pragma once

#include "Stdafx.h"
#include "ScoreView.h"
//#include "CallCardWnd.h"
#include "CardControl.h"
#include "ConcealCardView.h"
#include "CallScore.h"
#include "CallScoreInfo.h"


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

//////////////////////////////////////////////////////////////////////////

//游戏视图
class CGameClientView : public CGameFrameView
{
	//显示变量
protected:
	bool							m_bDeasilOrder;						//出牌顺序
	bool							m_bLastTurnCard;					//上轮扑克

	//游戏变量
protected:
	WORD							m_wBankerUser;						//庄家用户
	int								m_cbJuKuang;						//局况
	BYTE							m_cbMainColor;						//主牌花色
	WORD							m_wCallCardUser;					//叫牌用户
	BYTE							m_cbValueOrder[2];					//得分
	BYTE							m_cbMainValue;						//主牌数值

	//状态变量
public:
	bool							m_bLandTitle;						//地主标志
	bool							m_bPass[GAME_PLAYER];				//放弃数组
	BYTE							m_bScore[GAME_PLAYER];				//用户叫分
	BYTE							m_bCardCount[GAME_PLAYER];			//扑克数目
	WORD							m_wLandUser;						//地主用户
	BYTE							m_cbLandScore;						//地主分数
	WORD							m_bMeChair;							//自己位置

	//位置信息
protected:
	CSize							m_SizeBanker;						//庄家大小
	CSize							m_SizeStation;						//状态大小
	CPoint							m_PointBanker[GAME_PLAYER];			//庄家位置

	//位图变量
protected:
	CSkinImage						m_ImageBack;						//背景资源
	CSkinImage						m_ImageBackGround;					//背景资源
	CSkinImage						m_ImageLastTurn;					//上轮标志
	CSkinImage						m_ImageColorFlag;					//花色标志
	//CSkinImage						m_ImageStationBack;					//状态资源
	CSkinImage						m_ImageStationTitle;				//状态资源
	CSkinImage						m_ImageUserFlag;					//用户标志

	//按钮控件
public:
	CSkinButton						m_btStart;							//开始按钮
	CSkinButton						m_btOutCard;						//出牌按钮
	CSkinButton						m_btOutPrompt;						//提示按钮
	
	//功能按钮
public:
	CSkinButton						m_btLastTurnCard;					//上轮扑克
	CSkinButton						m_btStusteeControl;					//拖管控制

	//扑克控件
public:
	CCardControl					m_CardScore;						//得分扑克
	CCardControl					m_HandCardControl;					//手上扑克
	CCardControl					m_UserCardControl[4];				//扑克视图
	CCardControl					m_HandOutCardControl[4][4];			//明手扑克

	//控件组件
public:
	CScoreView						m_ScoreView;						//积分视图
	CCallScoreInfo					m_CallScoreInfo;					//叫分状态窗口
	CToolTipCtrl					m_ToolTipCtrl;						//提示控件
	CCallScore						m_CallScore;						//叫分窗口

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

	//配置函数
public:
	//用户顺序
	void SetUserOrder(bool bDeasilOrder);
	//自己位置
	void SetMeChair(WORD wMeChair);
	//获取顺序
	bool IsDeasilOrder() { return m_bDeasilOrder; }

	//游戏信息
public:
	//庄家用户
	void SetBankerUser(WORD wBankerUser);
	//设置局况
	void SetJuKuang(int nJuKuang);
	//设置定约信息
	void SetDingYue(BYTE cbMainValue,BYTE cbMainColor);
	//设置上轮
	void SetLastTurnCard(bool bLastTurnCard);
	//设置得分
	void SetValueOrder(BYTE cbValueBanker, BYTE cbValueOther);
	//设置放弃
	void SetPassFlag(WORD wChairID, bool bPass);

	//辅助函数
protected:
	//艺术字体
	void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos);

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
