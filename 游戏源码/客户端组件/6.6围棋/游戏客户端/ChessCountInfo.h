#pragma once

#include "Stdafx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////
//消息定义

#define IDM_CHESS_COUNT_RESPONSION	(WM_USER+400)						//应答消息

//////////////////////////////////////////////////////////////////////////

//游戏规则
class CChessCountInfo : public CDialog
{
	//变量定义
protected:
	float							m_fBlackCount;						//黑棋数目
	float							m_fWhiteCount;						//白棋数目
	float							m_fCommomCount;						//公共数目

	//资源变量
public:
	CBrush							m_Brush;							//背景画刷
	CSkinImage						m_ImageBack;						//背景资源

	//控件变量
public:
	CSkinButton						m_btOk;								//按钮控件
	CSkinButton						m_btCancel;							//按钮控件

	//函数定义
public:
	//构造函数
	CChessCountInfo();
	//析构函数
	virtual ~CChessCountInfo();

	//重载函数
protected:
	//控件绑定
	virtual void DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定消息
	virtual void OnOK();
	//取消消息
	virtual void OnCancel();

	//功能函数
public:
	//设置信息
	void SetChessCountInfo(float fBlackCount, float fWhiteCount, float fCommomCount);

	//消息映射
protected:
	//重画函数
	afx_msg void OnPaint();
	//销毁消息
	afx_msg void OnDestroy();
	//绘画背景
	afx_msg BOOL OnEraseBkgnd(CDC * pDC);
	//鼠标消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//控件颜色
	afx_msg HBRUSH OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
