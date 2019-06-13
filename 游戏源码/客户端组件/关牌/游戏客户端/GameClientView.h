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
#define IDM_TRUSTEE_CONTROL			(WM_USER+104)						//托管控制

//////////////////////////////////////////////////////////////////////////

//游戏视图
class CGameClientView : public CGameFrameView
{
	//游戏变量
protected:
	LONG							m_lCellScore;						//基础分数
	BYTE							m_cbBombCount;						//炸弹数目
	bool							m_bAutoPlayer[GAME_PLAYER];			//托管用户
	//状态变量
public:
	bool							m_bPass[GAME_PLAYER];				//放弃数组
	BYTE							m_bCardCount[GAME_PLAYER];			//扑克数目
	CPoint							m_PointUserPass[GAME_PLAYER];		//放弃位置
	CPoint							m_PointUserBomb[GAME_PLAYER];		//炸弹位置
	CPoint							m_ptAuto[GAME_PLAYER];

	//位图变量
protected:
	CSkinImage						m_ImageBomb;						//炸弹资源
	CSkinImage						m_ImageBack;						//背景资源
	CSkinImage						m_ImageFill;
	CSkinImage						m_ImageUserPass;					//放弃标志

	//按钮控件
public:
	CSkinButton						m_btStart;							//开始按钮
	CSkinButton						m_btOutCard;						//出牌按钮
	CSkinButton						m_btPassCard;						//放弃按钮
	CSkinButton						m_btOutPrompt;						//提示按钮
	CSkinButton						m_btStusteeControl;					//拖管控制

	//扑克控件
public:
	CCardControl					m_HandCardControl[2];				//手上扑克
	CCardControl					m_UserCardControl[2];				//扑克视图

	//控件组件
public:
	CScoreView						m_ScoreView;						//积分视图
#ifdef VIDEO_GAME
	CVideoServiceControl			m_DlgVideoService[2];				//视频窗口
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
	//炸弹数目
	void SetBombCount(BYTE cbBombCount);
	//基础分数
	void SetCellScore(LONG lCellScore);
	//设置放弃
	void SetPassFlag(WORD wChairID, bool bPass);
	//扑克数目
	void SetCardCount(WORD wChairID, BYTE bCardCount);

	void SetAutoUser(bool bAutoUser[2]);
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
	//建立函数
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标消息
	afx_msg void OnRButtonUp(UINT nFlags, CPoint Point);
	//鼠标双击
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//拖管控制
	afx_msg void OnStusteeControl();
	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
