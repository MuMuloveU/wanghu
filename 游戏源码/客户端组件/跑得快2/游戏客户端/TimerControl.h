#ifndef TIMER_CONTROL_HEAD_FILE
#define TIMER_CONTROL_HEAD_FILE

#pragma once

#include "Stdafx.h"

//枚举定义

//消息定义
#define	IDM_TIMER_OUT			(WM_USER+400)						//时钟到时事件

//扑克控件
class CTimerControl : public CWnd
{
	//配置变量
protected:
	bool							m_bDisplay;							//显示标志

	//位置变量
protected:
	CPoint							m_BenchmarkPos;						//基准位置

	//运行变量
protected:
	DWORD							m_dwCurrentIndex;					//当前索引
	int								m_nTimer;							//当前时间
	BOOL							m_bNotify;							//是否发到时通知
	BOOL							m_bPause;							//是否暂停

	//内部数据
protected:
	CWnd *							m_pSinkWindow;						//回调窗口

	//资源变量
protected:
	CSkinImage				m_ImageBack;						//图片资源
	CSkinImage				m_ImageArrow;						//箭头资源
	CSize					m_Size;								//大小

	//函数定义
public:
	//构造函数
	CTimerControl();
	//析构函数
	virtual ~CTimerControl();

	//配置函数
public:
	void SetBitmap(UINT uIdBack,UINT uIdArrow);
	//设置窗口
	void SetSinkWindow(CWnd * pSinkWindow);

	//控件控制
public:
	//设置显示
	void ShowTimer(bool bDisplay);
	//设置倒数时钟,direction为方向,secord为秒数,bNotify是到时是否通知
	void SetTimerUp(int direction,int secord,BOOL bNotify);
	void SetPause(BOOL bPause);
	void SetBenchmarkPos(int nXPos, int nYPos);

	//内部函数
private:
	//调整位置
	void RectifyControl();

	//消息函数
protected:
	//重画函数
	afx_msg void OnPaint();
	//建立消息
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//timer消息
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif