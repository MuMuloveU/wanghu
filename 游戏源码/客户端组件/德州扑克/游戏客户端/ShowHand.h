#ifndef SHOWHAND_VIEW_HEAD_FILE
#define SHOWHAND_VIEW_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

#pragma once

#include "Stdafx.h"
#include "Resource.h"

class ShowHand : public CDialog
{
	DECLARE_DYNAMIC(ShowHand)

public:
	BYTE		m_bTimes;		//时间数目

	//资源变量
protected:
	CSkinImage						m_ImageBack;						//背景图案

	//控件变量
public:
	CSkinButton						m_btOK;						//继续按钮
	CSkinButton						m_btCancel;						//退出按钮

public:
	//构造函数
	ShowHand();   
	//析构函数
	virtual ~ShowHand();

	//重载函数
protected:
	//控件绑定
	virtual void DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();

	//消息函数
protected:
	//重画函数
	afx_msg void OnPaint();
	//绘画背景
	afx_msg BOOL OnEraseBkgnd(CDC * pDC);
	//时间消息
	afx_msg void OnTimer(UINT nIDEvent);
	//鼠标消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint Point);
	//
	afx_msg void OnBnClickedOk();
	//
	afx_msg void OnBnClickedCancel();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif