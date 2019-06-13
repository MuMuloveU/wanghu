#ifndef GOLD_CONTROL_HEAD_FILE
#define GOLD_CONTROL_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//金币控制
class CGoldControl : public CWnd
{
	//变量定义
protected:
	LONG								m_lCellScore;					//单元注
	LONG								m_lTimes;						//倍数

	//位置变量
protected:
	CPoint								m_BenchmarkPos;					//基准位置

	//资源变量
protected:
	CSkinImage							m_ImageBack;					//背景资源
	CSkinImage							m_ImageNumber;					//数字资源

	//按钮控件
protected:
	CSkinButton							m_btAddTimes1;					//1倍
	CSkinButton							m_btAddTimes2;					//2倍
	CSkinButton							m_btAddTimes3;					//3倍
	CSkinButton							m_btAddTimes4;					//4倍
	CSkinButton							m_btCancel;						//取消

	//函数定义
public:
	//构造函数
	CGoldControl();
	//析构函数
	virtual ~CGoldControl();

	//功能函数
public:
	//设置筹码
	void SetCellScore(LONG lCellSocre);
	//设置位置
	void SetBenchmarkPos(int nXPos, int nYPos);
	//设置倍数
	void SetAddTimes( LONG lTimes );
	//获取倍数
	LONG GetAddTimes() { return m_lTimes; }

	//内部函数
private:
	//调整控件
	void RectifyControl();

	//消息映射
protected:
	//重画函数
	afx_msg void OnPaint();
	//创建控件
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//1倍按钮
	afx_msg void OnBnAddTimes1();
	//2倍按钮
	afx_msg void OnBnAddTimes2();
	//3倍按钮
	afx_msg void OnBnAddTimes3();
	//4倍按钮
	afx_msg void OnBnAddTimes4();
	//取消
	afx_msg void OnBnCancel();

	DECLARE_MESSAGE_MAP()
};


//////////////////////////////////////////////////////////////////////////

#endif