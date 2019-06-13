#ifndef GOLD_CONTROL_HEAD_FILE
#define GOLD_CONTROL_HEAD_FILE

#pragma once

#include "Stdafx.h"
#define IDM_MAX_SCORE				  WM_USER+120						//最大下注
#define IDM_MIN_SCORE				  WM_USER+121						//最小下注	
#define IDM_OK_SCORE				  WM_USER+122						//确定消息
#define IDM_CANCEL_SCORE			  WM_USER+123						//取消消息	
//////////////////////////////////////////////////////////////////////////

//金币控制
class CGoldControl : public CWnd
{
	//变量定义
protected:
	LONG								m_lMaxGold;						//最大金币
	LONG								m_lMinGold;						//最小金币
	LONG								m_lGoldCell[7];					//单元金币
	CPoint								m_BasicPoint;					//基础位置

	//位置变量
protected:
	int									m_nWidth;						//控件宽度
	int									m_nCellCount;					//单元数目

	//按纽控件
public:
	CSkinButton							m_btMaxScore;					//最大按钮
	CSkinButton						    m_btMinScore;					//最小按钮
	CSkinButton						    m_btOK;							//确定按纽
	CSkinButton						    m_btCancel;						//取消按纽


	//资源变量
protected:
	CSkinImage							m_ImageLeft;					//背景资源
	CSkinImage							m_ImageMid;						//背景资源
	CSkinImage							m_ImageRight;					//背景资源
	CSkinImage							m_ImageNumber;					//数字资源

	//函数定义
public:
	//构造函数
	CGoldControl();
	//析构函数
	virtual ~CGoldControl();

	//功能函数
public:
	//获取金币
	LONG GetGold();
	//设置金币
	void SetGold(LONG lGold);
	//设置限注
	void SetMaxGold(LONG lMaxGold);
	//设置最小
	void SetMinGold(LONG lMinGold);
	//设置位置
	void SetBasicPoint(int nXPos, int nYPos);

	//内部函数
private:
	//调整控件
	void RectifyControl();
	//绘画金币
	void DrawGoldCell(CDC * pDC, int nXPos, int nYPox, LONG lGold);

	//消息映射
protected:
	//重画函数
	afx_msg void OnPaint();
	//左键按下消息
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//右键按下消息
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//设置光标
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//最小下注
	afx_msg void OnMinScore();
	//最大下注
	afx_msg void OnMaxScore();
	//确定消息
	afx_msg void OnOKScore();
	//取消消息
	afx_msg void OnCancelScore();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};


//////////////////////////////////////////////////////////////////////////

#endif