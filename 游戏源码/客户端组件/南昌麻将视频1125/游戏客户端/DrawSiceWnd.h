#ifndef DRAW_WND_HEAD_FILE
#define DRAW_WND_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "Resource.h"

//////////////////////////////////////////////////////////////////////////
#define IDM_GAME_START			(WM_USER+401)						//扑克操作
#define IDM_CARD_MOVE			(WM_USER+402)						//移动声音
#define IDM_SICE_TWO			(WM_USER+403)						//移动声音

//////////////////////////////////////////////////////////////////////////

//控制窗口
class CDrawSiceWnd : public CWnd
{
	//配置变量
protected:
	CSkinImage						m_ImageBack;						//背景图案
	CSkinImage						m_ImageDrawSiceFirst;				//动作解释
	CSkinImage						m_ImageDrawSiceSecond;				//动作解释
	CSkinImage						m_ImageDrawSiceUserFirst;			//动作解释
	CSkinImage						m_ImageDrawSiceUserSecond;			//动作解释
	BYTE                            m_bSicePos;							//动画帧数
	int								m_nXPos;							//X位置
	int								m_nYPos;							//Y位置
	CDC								*ParentDC;							//CDC窗体	
	//函数定义
public:
	//构造函数
	CDrawSiceWnd();
	//析构函数
	virtual ~CDrawSiceWnd();

	//内部函数
public:
	void SetSiceInfo(CDC *dc,BYTE bMoveTime,BYTE SiceFirst,BYTE SiceSecond,BYTE SiceUserFirst,BYTE SiceUserSecond);
	void SetBenchmarkPos(int nXPos, int nYPos);
	//消息映射
protected:
	//重画函数
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT nIDEvent);
	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif