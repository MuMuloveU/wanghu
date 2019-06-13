#ifndef SCORE_CONTROL_HEAD_FILE
#define SCORE_CONTROL_HEAD_FILE

#pragma once

#include "Stdafx.h"

#define LEN_ACCOUNTS 32
//////////////////////////////////////////////////////////////////////////

//积分信息
struct tagScoreInfoC
{
	LONGLONG						lGameScore[GAME_PLAYER];				//游戏积分
	TCHAR							szUserName[GAME_PLAYER][LEN_ACCOUNTS];	//用户名字
};

//////////////////////////////////////////////////////////////////////////

//积分控件
class CScoreControl
{
	//变量定义
protected:
	bool							m_bValid;								//有效标志
	tagScoreInfoC					m_ScoreInfo;							//积分信息

	//资源变量
protected:
	CSkinImage						m_ImageWinLose;							//胜利失败
	CSkinImage						m_ImageGameScore;						//积分视图

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
	VOID SetScoreInfo(const tagScoreInfoC & ScoreInfo);
	//绘画界面
	VOID DrawScoreView(CDC * pDC, INT nXPos, INT nYPos);
};

//////////////////////////////////////////////////////////////////////////

#endif