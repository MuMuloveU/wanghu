#pragma once

#include "Stdafx.h"
#include "GameRule.h"
#include "ChessBorad.h"
#include "ChessManual.h"
#include "GameScoreWnd.h"
#include "ChessCountInfo.h"
#include "DlgUserRequest.h"

//////////////////////////////////////////////////////////////////////////
//消息定义

#define IDM_START					(WM_USER+100)						//开始消息
#define IDM_COUNT					(WM_USER+101)						//点目消息
#define IDM_PASS					(WM_USER+102)						//通过消息
#define IDM_REGRET					(WM_USER+103)						//悔棋消息
#define IDM_PEACE					(WM_USER+104)						//求和消息
#define IDM_GIVEUP					(WM_USER+105)						//认输消息
#define IDM_PRESERVE				(WM_USER+106)						//保存消息
#define IDM_STUDY					(WM_USER+107)						//研究消息
#define IDM_REGRET_ANSWER			(WM_USER+108)						//悔棋应答
#define IDM_PEACE_ANSWER			(WM_USER+109)						//求和应答

//////////////////////////////////////////////////////////////////////////

//状态信息
struct tagStatusInfo
{
	WORD							wStepCount;							//棋步数目
	WORD							wUseTimeCount;						//用时时间
	WORD							wLimitTimeCount;					//限时时间
	WORD							wTakeChessCount;					//提子数目
};

//////////////////////////////////////////////////////////////////////////

//游戏视图
class CGameClientView : public CGameFrameView
{
	//游戏变量
protected:
	WORD							m_wBlackUser;						//黑棋用户
	tagStatusInfo					m_StatusInfo[2];					//游戏信息

	//位置信息
protected:
	CSize							m_UserInfoSize;						//信息大小

	//位图变量
protected:
	CSkinImage						m_ImageBack;						//背景图案
	CSkinImage						m_ImageUserBlack;					//用户信息
	CSkinImage						m_ImageUserWhite;					//用户信息

	//控件变量
public:
	CSkinButton						m_btStart;							//开始按钮
	CSkinButton						m_btCount;							//点目按钮
	CSkinButton						m_btPass;							//通过按钮
	CSkinButton						m_btRegret;							//悔棋按钮
	CSkinButton						m_btPeace;							//求和按钮
	CSkinButton						m_btGiveUp;							//认输按钮
	CSkinButton						m_btPreserve;						//保存按钮
	CSkinButton						m_btStudy;							//研究按钮
	CGameRule						m_GameRule;							//游戏规则
	CChessBorad						m_ChessBorad;						//游戏棋盘
	CChessManual					m_ChessManual;						//棋谱窗口
	CGameScoreWnd					m_GameScoreWnd;						//成绩窗口
	CDlgUserRequest					m_PeaceRequest;						//和棋请求
	CDlgUserRequest					m_RegretRequest;					//悔棋请求
	CChessCountInfo					m_ChessCountInfo;					//点目信息

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
	//和棋请求
	void ShowPeaceRequest();
	//悔棋请求
	void ShowRegretRequest();
	//设置黑棋
	void SetBlackUser(WORD wBlackUser);
	//设置信息
	void SetUserStatusInfo(WORD wViewChairID, tagStatusInfo & StatusInfo);

	//内部函数 
private:
	//更新状态
	void UpdateUserInfoArea();
	//用户时钟
	void DrawUserClock(CDC * pDC, int nXPos, int nYPos, WORD wClockCount);

	//控件消息
protected:
	//点击棋盘
	LRESULT OnHitChessBorad(WPARAM wParam, LPARAM lParam);

	//消息映射
protected:
	//开始按钮
	afx_msg void OnStart();
	//点目按钮
	afx_msg void OnCount();
	//通过按钮
	afx_msg void OnPass();
	//悔棋按钮
	afx_msg void OnRegret();
	//求和按钮
	afx_msg void OnPeace();
	//认输按钮
	afx_msg void OnGiveUp();
	//保存按钮
	afx_msg void OnPreserve();
	//研究按钮
	afx_msg void OnStudy();
	//建立函数
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()

};

//////////////////////////////////////////////////////////////////////////
