#ifndef SCORE_CONTROL_HEAD_FILE
#define SCORE_CONTROL_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//积分信息
struct tagScoreInfo
{
	//积分信息
	LONGLONG						lGameScore[GAME_PLAYER];				//游戏积分
	TCHAR							szUserName[GAME_PLAYER][LEN_ACCOUNTS];	//用户名字

	//附加信息
	BYTE							cbReason;								//结束原因
	FLOAT							fWinRate[GAME_PLAYER];					//用户胜率
	TCHAR							szLevelName[GAME_PLAYER][32];			//用户等级
	WORD							wMeChiarID;								//本身位置
};

//////////////////////////////////////////////////////////////////////////

//积分控件
class CScoreControl : public CWnd
{
	//状态变量
protected:
	tagScoreInfo					m_GameScoreInfo;					//积分信息

	//资源变量
protected:
	CSkinImage						m_ImageWinLose;						//胜利失败
	CSkinImage						m_ImageGameScore;					//积分视图

	//函数定义
public:
	//构造函数
	CScoreControl();
	//析构函数
	virtual ~CScoreControl();

	//功能函数
public:
	//复位数据
	VOID RestorationData();
	//设置积分
	VOID SetScoreInfo(const tagScoreInfo & ScoreInfo);

	//消息映射
protected:
	//重画函数
	VOID OnPaint();
	//绘画背景
	BOOL OnEraseBkgnd(CDC * pDC);
	//创建函数
	INT OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标消息
	VOID OnLButtonDown(UINT nFlags, CPoint Point);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif
