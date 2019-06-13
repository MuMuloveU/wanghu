#pragma once

#include "Stdafx.h"
#include "Resource.h"

//////////////////////////////////////////////////////////////////////////
//消息定义

#define WM_SELECT_CHESS			WM_USER+300							//选择棋子

//////////////////////////////////////////////////////////////////////////

//棋子选择
class CChessSelect : public CDialog
{
	//资源变量
protected:
	CSkinImage						m_ImageBack;						//背景资源

	//控件变量
protected:
	CSkinButton						m_btRook;							//棋子按钮
	CSkinButton						m_btHorse;							//棋子按钮
	CSkinButton						m_btQueen;							//棋子按钮
	CSkinButton						m_btElephant;						//棋子按钮

	//函数定义
public:
	//构造函数
	CChessSelect();
	//析构函数
	virtual ~CChessSelect();

	//重载函数
protected:
	//控件绑定
	virtual void DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定消息
	virtual void OnOK() { return; }
	//取消消息
	virtual void OnCancel() { return; }

	//按钮消息
protected:
	//棋子按钮
	afx_msg void OnRook();
	//棋子按钮
	afx_msg void OnHorse();
	//棋子按钮
	afx_msg void OnQueen();
	//棋子按钮
	afx_msg void OnElephant();

	//消息映射
protected:
	//重画函数
	afx_msg void OnPaint();
	//背景消息
	afx_msg BOOL OnEraseBkgnd(CDC * pDC);
	//鼠标消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
