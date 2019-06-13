#pragma once

#include "Stdafx.h"
#include "Resource.h"

//////////////////////////////////////////////////////////////////////////

//游戏配置
class CGameOption : public CDialog
{
	//变量定义
public:
	bool							m_bEnableSound;						//启用声音
	bool							m_bDoubleMouse;						//双击下棋
	bool							m_bShowChessID;						//显示手数
	bool							m_bAutoSaveManual;					//自动保存

	//资源变量
public:
	CBrush							m_Brush;							//背景画刷
	CSkinImage						m_ImageBack;						//背景资源

	//控件变量
public:
	CSkinButton						m_btOK;								//确定按钮
	CSkinButton						m_btCancel;							//取消按钮

	//函数定义
public:
	//构造函数
	CGameOption();
	//析构函数
	virtual ~CGameOption();

	//重载函数
protected:
	//控件绑定
	virtual void DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定消息
	virtual void OnOK();

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
