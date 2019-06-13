#pragma once

#include "Stdafx.h"
#include "ScoreView.h"
#include "CardControl.h"

//#define DEBUG_GAME
//////////////////////////////////////////////////////////////////////////
//消息定义

#define IDM_START					(WM_USER+100)						//开始消息
#define IDM_OUT_CARD				(WM_USER+101)						//出牌消息
#define IDM_PASS_CARD				(WM_USER+102)						//放弃出牌
#define IDM_AUTO_OUTCARD			(WM_USER+104)                       //提示消息
#define IDM_EMPEROR					(WM_USER+105)						//皇帝
#define IDM_HOUSECARL				(WM_USER+106)						//侍卫

//////////////////////////////////////////////////////////////////////////

//游戏视图
class CGameClientView : public CGameFrameView
{
	//游戏变量
protected:
	WORD							m_wChairID;							//保皇派
	bool							m_bDif;								//皇帝，侍卫一同

	//配置变量
protected:
	bool							m_bDeasilOrder;						//出牌顺序

	//状态变量
public:
	bool							m_bPass[GAME_PLAYER];				//放弃数组
	BYTE							m_bCardCount[GAME_PLAYER];			//扑克数目
	BYTE							m_wCurrentEnthronement;				//当前即位
	BYTE							m_wCurrentHousecarl;				//当前侍卫
	
	//位置信息
public:
	CPoint							m_ptHeart[GAME_PLAYER];				//心战位置
	CPoint							m_ptEmp_Hou[2];							//皇，侍卫位置
	bool							m_bGamePlaying;
	bool							m_bOutHousecarl;
	//位图变量
protected:
	CSkinImage						m_ImageBack;						//背景资源
	CSkinImage						m_ImageCenter;						//背景资源
	CSkinImage						m_ImageHousecarl;
	CSkinImage						m_ImageEmperor;
	CSkinImage						m_ImageFarmer;

	//按钮控件
public:
	CSkinButton						m_btStart;							//开始按钮
	CSkinButton						m_btOutCard;						//出牌按钮
	CSkinButton						m_btPassCard;						//放弃按钮
	CSkinButton						m_btAutoOutCard;					//提示按钮
	CSkinButton						m_btEnthronement;					//即位 
	CSkinButton						m_btDemise;							//让位 
	CSkinButton						m_btByOneself;						//独保 
	CSkinButton						m_btNotByOneself;					//不独保 
	CSkinButton						m_btRebelled;						//造反
	CSkinButton						m_btNotRebelled;					//不造反
	CSkinButton						m_btKeep;							//明保
	CSkinButton						m_btHideKeep;						//暗保
	//扑克控件
public:
	CCardControl					m_HandCardControl;					//手上扑克
	CCardControl					m_UserCardControl[GAME_PLAYER];				//扑克视图
//	CCardControl					m_LeaveCardControl[2];				//结束扑克

	//控件组件
public:
	CScoreView						m_ScoreView;						//积分视图

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
	//功能按钮函数
	void SetBT(WORD wChairID,BYTE bIndex);
	
	//消息映射
protected:
	//即位
	afx_msg void OnEnthronement();
	//让位
	afx_msg void OnDemise();
	//独保
	afx_msg void OnByOneself();
	//不独保
	afx_msg void OnNotByOneself();
	//造反
	afx_msg void OnReballed();
	//不造反
	afx_msg void OnNotReballed();
	//明保
	afx_msg void OnKeep();
	//暗保
	afx_msg void OnHideKeep();
	//开始按钮
	afx_msg void OnStart();
	//出牌按钮
	afx_msg void OnOutCard();
	//放弃按钮
	afx_msg void OnPassCard();
	//出牌提示
	afx_msg void OnAutoOutCard();
	//建立函数
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标消息
	afx_msg void OnRButtonUp(UINT nFlags, CPoint Point);
	//鼠标双击
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
