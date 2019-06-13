#pragma once

#include "Stdafx.h"
#include "..\消息定义\CMD_QuickRun.h"
#include "ScoreView.h"
#include "TimerControl.h"

//////////////////////////////////////////////////////////////////////////
//消息定义
#define IDC_START						100								//开始按钮
#define IDC_OUT_CARD					101								//出牌按钮
#define IDC_PASS_CARD					102								//PASS按钮

//////////////////////////////////////////////////////////////////////////

//游戏视图
class CGameClientView : public CGameFrameView
{
	//友元定义
	friend class CGameClientDlg;

	//界面变量
protected:
	//桌面图片
	CSkinImage						m_ImageDisk;
	CSkinImage						m_ImageCenter;
	//当前出牌者
	CSkinImage						m_ImageFocus;
	//pass图片
	CSkinImage						m_ImagePass;
	//牌图片
	CSkinImage						m_ImagePisa;
	//牌的大小
	CSize							m_szPisa;
	CSkinButton						m_btStart;							//开始按钮
	CSkinButton						m_btOutCard;						//出牌按钮
	CSkinButton						m_btPassCard;						//Pass按钮
	CTimerControl					m_Timer;							//计时器
	//玩家自己的牌
	CArrayTemplate<struct tagPisa,struct tagPisa&> m_PisaArray;
	//各人出的牌
	CArrayTemplate<struct tagPisa,struct tagPisa&> m_OutPisaArray[GAME_PLAYER];
	//其他玩家剩下的牌数,其中自己索引的值不使用
	int m_nPlayerLeftCount[GAME_PLAYER];
	//其他玩家是否Pass
	BOOL m_bOutPass[GAME_PLAYER];
	int m_nOwnerIndex;
	int m_nFocusIndex;
	//是否准备好
	BOOL m_bReady;
	//自己的牌的矩形
	CRect m_rtPisa;
	//被选中的牌
	BOOL m_bOutSelect[PISACOUNT];
	//出牌顺序
	bool m_bDeasilOrder;
	//是否刚开始
	BOOL m_bStart;

	//控件变量
public:
	CScoreView						m_ScoreView;						//积分视图

	//函数定义
public:
	//构造函数
	CGameClientView();
	//析构函数
	virtual ~CGameClientView();

	//用户顺序
	void SetUserOrder(bool bDeasilOrder);
	//获取顺序
	bool IsDeasilOrder() { return m_bDeasilOrder; }
	//显示计时器
	void SetTimerUp(BOOL bUp);
	//设置计时器暂停计数
	void SetTimerPause(BOOL bPause);
	//设置黑桃3准备出牌
	int SetHeiTao3Out();

	//继承函数
private:
	//重置界面
	virtual void ResetGameView();
	//调整控件
	virtual void RectifyGameView(int nWidth, int nHeight);
	//绘画界面
	virtual void DrawGameView(CDC * pDC, int nWidth, int nHeight);
	//消息映射
protected:
	//建立函数
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//开始按钮
	afx_msg void OnStart();
	//出牌按钮
	afx_msg void OnOutCard();
	//放弃按钮
	afx_msg void OnPassCard();

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

//////////////////////////////////////////////////////////////////////////
