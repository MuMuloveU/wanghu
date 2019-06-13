#ifndef SKIN_FRAMEWND_HEAD_FILE
#define SKIN_FRAMEWND_HEAD_FILE

#pragma once

#include "SkinControls.h"
#include "SkinWndObject.h"

//////////////////////////////////////////////////////////////////////////

//框架类
class SKIN_CONTROL_CLASS CSkinFrameWnd : public CFrameWnd, public CSkinWndObject
{
	//函数定义
public:
	//构造函数
	CSkinFrameWnd();
	//析构函数
	virtual ~CSkinFrameWnd();

	//消息映射
protected:
	//建立消息
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标消息
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//鼠标消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//鼠标消息
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//鼠标消息
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	//鼠标消息
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//鼠标消息
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//鼠标消息
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//绘画消息
	afx_msg void OnPaint();
	//改变消息
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	//绘画背景
	afx_msg BOOL OnEraseBkgnd(CDC * pDC);
	//位置改变
	afx_msg void OnWindowPosChanged(WINDOWPOS * lpwndpos);
	//控件颜色
	afx_msg HBRUSH OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor);
	//标题消息
	LRESULT	OnSetTextMesage(WPARAM wParam, LPARAM lParam);

	//消息映射
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNCREATE(CSkinFrameWnd)
};

//////////////////////////////////////////////////////////////////////////

#endif