#pragma once

#include "Stdafx.h"
#include "ScoreView.h"
#include "CardControl.h"

//////////////////////////////////////////////////////////////////////////
//消息定义

#define IDM_START					(WM_USER+100)						//开始消息
#define IDM_OUT_CARD				(WM_USER+101)						//出牌消息
#define IDM_PASS_CARD				(WM_USER+102)						//放弃出牌
#define IDM_OUT_PROMPT				(WM_USER+103)						//提示出牌
#define IDM_SORT_BY_COLOR			(WM_USER+104)						//花色排序
#define IDM_SORT_BY_VALUE			(WM_USER+105)						//大小排序
#define IDM_TRUSTEE_CONTROL			(WM_USER+106)						//托管控制
//////////////////////////////////////////////////////////////////////////

//游戏视图
class CGameClientView : public CGameFrameView
{
	//配置变量
protected:
	bool							m_bDeasilOrder;						//出牌顺序

	//状态变量
public:
	bool							m_bPass[GAME_PLAYER];				//放弃数组
	BYTE							m_bCardCount[GAME_PLAYER];			//扑克数目
	CPoint							m_PointUserPass[GAME_PLAYER];		//放弃位置
	CPoint							m_PointTrustee[GAME_PLAYER];		//托管位置
	WORD							m_wWinOrder[GAME_PLAYER];			//胜利列表
	bool							m_bTrustee[GAME_PLAYER];			//是否托管

	//位图变量
protected:
	CSkinImage						m_ImageViewFill;					//背景位图
	CSkinImage						m_ImageViewBack;					//背景位图
	CSkinImage						m_ImageUserPass;					//放弃标志
	CSkinImage						ImageWinOrder;						//名次位图
	CPngImage						m_ImageTrustee;						//托管标志	


	//按钮控件
public:
	CSkinButton						m_btStart;							//开始按钮
	CSkinButton						m_btOutCard;						//出牌按钮
	CSkinButton						m_btPassCard;						//放弃按钮
	//CSkinButton						m_btOutPrompt;						//提示按钮
	CSkinButton						m_btSortByColor;					//花色排序
	CSkinButton						m_btSortByValue;					//大小排序
	CSkinButton						m_btStusteeControl;					//拖管控制

	//CSkinButton						m_btShowVideo;						//显示视频

	//扑克控件
public:
	CCardControl					m_HandCardControl[4];				//手上扑克
	CCardControl					m_UserCardControl[4];				//扑克视图

	//控件组件
public:
	CScoreView						m_ScoreView;						//积分视图

#ifdef VIDEO_GAME
	//视频组件
private:
	CVideoServiceControl 			m_DlgVedioService[4];				//视频窗口
#endif

	//名次位置
private:
	CPoint							m_PointWinOrder[GAME_PLAYER];		//输赢名次

	//辅助函数
protected:
	//艺术字体
	void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos);

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
	//设置放弃
	void SetPassFlag(WORD wChairID, bool bPass);
	//扑克数目
	void SetCardCount(WORD wChairID, BYTE bCardCount);
	//设置名次
	bool SetUserWinOrder(WORD wChairID, WORD wWinOrder);
	//设置托管
	void SetTrustee(WORD wTrusteeUser,bool bTrustee);


	//消息映射
protected:
	//开始按钮
	afx_msg void OnStart();
	//出牌按钮
	afx_msg void OnOutCard();
	//放弃按钮
	afx_msg void OnPassCard();
	//提示按钮
	afx_msg void OnOutPrompt();
	//花色排序
	afx_msg void OnSortByColor();
	//大小排序
	afx_msg void OnSortByValue();
	//拖管控制
	afx_msg void OnStusteeControl();
	//显示视频
	//afx_msg void OnShowVideo();
	//建立函数
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标消息
	afx_msg void OnRButtonUp(UINT nFlags, CPoint Point);
	//鼠标双击
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
