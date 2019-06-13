#ifndef DLG_HINT_HEAD_FILE
#define DLG_HINT_HEAD_FILE

#pragma once

#include "Stdafx.h"
//#include "GameClient.h"

//////////////////////////////////////////////////////////////////////////

//用户请求
class CDlgHint : public CDialog
{
	//变量定义
public:
	LONG							m_nResidualTime;					//剩余时间
	CSkinImage						m_ImageViewBack;					//背景资源

	//控件变量
protected:
	CSkinButton						m_btOk;								//按钮控件

	//函数定义
public:
	//构造函数
	CDlgHint();
	//析构函数
	virtual ~CDlgHint();

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定消息
	virtual VOID OnOK();

	//消息映射
protected:
	//重画函数
	VOID OnPaint();
	//时间消息
	VOID OnTimer(UINT nIDEvent);
	//绘画背景
	BOOL OnEraseBkgnd(CDC * pDC);
	//鼠标消息
	VOID OnLButtonDown(UINT nFlags, CPoint Point);

	//辅助函数
public:
	//设置时间
	VOID InitTimerCount();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif
