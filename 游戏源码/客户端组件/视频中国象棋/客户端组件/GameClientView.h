#ifndef GAME_CLIENT_HEAD_FILE
#define GAME_CLIENT_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "ChessBorad.h"
#include "ManualList.h"
#include "ScoreControl.h"
#include "DlgRuleOption.h"
#include "DlgUserRequest.h"
#include "DlgChessManual.h"
#include "DlgHint.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

////环绕位图
//struct tagEncircleImage
//{
//	CSkinImage						ImageTL;							//位图对象
//	CSkinImage						ImageTM;							//位图对象
//	CSkinImage						ImageTR;							//位图对象
//	CSkinImage						ImageML;							//位图对象
//	CSkinImage						ImageMR;							//位图对象
//	CSkinImage						ImageBL;							//位图对象
//	CSkinImage						ImageBM;							//位图对象
//	CSkinImage						ImageBR;							//位图对象
//};
//
////环绕信息
//struct tagEncircleInfo
//{
//	INT								nTBorder;							//上边框高
//	INT								nBBorder;							//下边框高
//	INT								nLBorder;							//左边框宽
//	INT								nRBorder;							//右边框宽
//};

//视图位置
#define MYSELF_VIEW_ID				1									//视图位置

//控制消息
#define IDM_START					(WM_USER+200)						//开始消息
#define IDM_PEACE					(WM_USER+201)						//求和消息
#define IDM_REGRET					(WM_USER+202)						//悔棋消息
#define IDM_GIVEUP					(WM_USER+203)						//认输消息
#define IDM_MANUAL					(WM_USER+204)						//棋谱消息
#define IDM_PRESERVE				(WM_USER+205)						//保存消息

//棋谱消息
#define IDM_ORIENTATION_MANUAL		(WM_USER+210)						//定位棋谱

//////////////////////////////////////////////////////////////////////////

//状态信息
struct tagStatusInfo
{
	WORD							wDrawTimeCount;						//局时时间
	WORD							wStepTimeCount;						//步时时间
	WORD							wSecondTimeCount;					//读秒时间
};

//////////////////////////////////////////////////////////////////////////

//游戏视图
class CGameClientView : public CGameFrameView
{
	//游戏变量
protected:
	INT								m_nFrameWidth;						//常规大小
	INT								m_nFrameHeight;						//常规大小
	WORD							m_wBankerUser;						//庄家用户
	WORD							m_wCurrentUser;						//当前用户
	tagStatusInfo					m_RuleStatusInfo;					//配置状态
	tagStatusInfo					m_UserStatusInfo[2];				//用户状态

	//控制按钮
public:
	CSkinButton						m_btStart;							//开始按钮
	CSkinButton						m_btPeace;							//求和按钮
	CSkinButton						m_btManual;							//打谱按钮
	CSkinButton						m_btRegret;							//悔棋按钮
	CSkinButton						m_btGiveUp;							//认输按钮
	CSkinButton						m_btPreserve;						//保存按钮

	//棋谱按钮
public:
	CSkinButton						m_btManualHead;						//棋谱按钮
	CSkinButton						m_btManualLast;						//棋谱按钮
	CSkinButton						m_btManualOpen;						//棋谱按钮
	CSkinButton						m_btManualNext;						//棋谱按钮
	CSkinButton						m_btManualTail;						//棋谱按钮

	//游戏控件
public:
	CManualList						m_ManualList;						//棋谱列表
	CChessBorad						m_ChessBorad;						//棋盘控件
	CToolTipCtrl					m_ToolTipCtrl;						//提示控件
	CScoreControl					m_ScoreControl;						//游戏成绩
	CDlgRuleOption					m_DlgRuleOption;					//规则设置
	CDlgChessManual					m_DlgChessManual;					//棋谱视图
	CDlgUserRequest					m_UserRequestPeace;					//和棋请求
	CDlgUserRequest					m_UserRequestRegret;				//悔棋请求
	CDlgHint						m_UserHint;							//提示语言

	//位置变量
protected:
	CPoint							m_ptBanker[GAME_PLAYER];			//庄家位置

	//控件位置
protected:
	CRect							m_rcChessManual;					//棋谱位置
	CPoint							m_ptControlView;					//控制视图
	CPoint							m_ptUserInfoView;					//信息视图

	//资源大小
protected:
	INT								m_nManualWidth;						//棋谱宽度
	CSize							m_SizeChessBorad;					//棋盘大小
	CSize							m_SizeControlView;					//控制大小
	CSize							m_SizeUserInfoView;					//信息大小
	tagEncircleInfo					m_EncircleInfoViewBack;				//框架信息

	//资源变量
protected:
	CSkinImage						m_ImageViewFill;					//背景资源
	CSkinImage						m_ImageClockNumber;					//数字资源
	CSkinImage						m_ImageControlBack;					//控制背景
	CSkinImage						m_ImageUserInfoFlag;				//用户标志
	CSkinImage						m_ImageUserInfoView;				//用户信息
	CSkinImage						m_ImageChessManualT;				//棋谱背景
	CSkinImage						m_ImageChessManualM;				//棋谱背景
	CSkinImage						m_ImageChessManualB;				//棋谱背景
	tagEncircleImage				m_EncircleImageViewBack;			//框架信息

#ifdef VIDEO_GAME
	//控件变量
protected:
	CVideoServiceControl			m_DlgVideoService[GAME_PLAYER];		//视频窗口
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

	//控制接口
private:
	//重置界面
	virtual void ResetGameView();

	//继承函数
protected:
	//调整控件
	virtual VOID RectifyGameView(INT nWidth, INT nHeight);
	//绘画界面
	virtual VOID DrawGameView(CDC * pDC, INT nWidth, INT nHeight);

	//功能函数
public:
	//庄家用户
	bool SetBankerUser(WORD wBankerUser);
	//当前用户
	bool SetCurrentUser(WORD wCurrentUser);
	//设置规则
	bool SetGameRuleInfo(const tagGameRuleInfo & GameRuleInfo);
	//用户状态
	bool SetUserStatusInfo(WORD wChairID, const tagStatusInfo & UserStatusInfo);

	//棋盘函数
public:
	//选择棋子
	bool SelectChessItem(BYTE cbXPos, BYTE cbYPos);
	//走棋路线
	bool SetMoveChessTrace(BYTE cbXPosStart, BYTE cbYPosStart, BYTE cbXPosTerminal, BYTE cbYPosTerminal);

	//辅助函数
protected:
	//用户时钟
	VOID DrawUserClock(CDC * pDC, INT nXPos, INT nYPos, WORD wClockCount);
	//用户等级
	TCHAR *GetLevelDescribe(LONG lUserScore,TCHAR tcUserInfo[32]);
	
	//按钮消息
protected:
	//开始按钮
	VOID OnBnClickedStart();
	//求和按钮
	VOID OnBnClickedPeace();
	//悔棋按钮
	VOID OnBnClickedRegret();
	//认输按钮
	VOID OnBnClickedGiveUp();
	//打谱按钮
	VOID OnBnClickedManual();
	//保存按钮
	VOID OnBnClickedPreserve();

	//棋谱按钮
protected:
	//棋谱按钮
	VOID OnBnClickedHead();
	//棋谱按钮
	VOID OnBnClickedLast();
	//棋谱按钮
	VOID OnBnClickedOpen();
	//棋谱按钮
	VOID OnBnClickedNext();
	//棋谱按钮
	VOID OnBnClickedTail();

	//消息函数
protected:
	//选择改变
	VOID OnLbnSelchangeManualList();
	//创建函数
	INT OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标消息
	VOID OnLButtonDown(UINT nFlags, CPoint Point);
	//鼠标消息
	VOID OnLButtonDblClk(UINT nFlags, CPoint Point);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif
