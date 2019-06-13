#ifndef CARD_CONTROL_HEAD_FILE
#define CARD_CONTROL_HEAD_FILE

#pragma once

#include "Stdafx.h"
#define CARD_MASK_VALUE 0x0F
#define CARD_MASK_COLOR 0xF0
//////////////////////////////////////////////////////////////////////////

//X 排列方式
enum enXCollocateMode 
{ 
	enXLeft, 
	enXCenter,
	enXRight,
};

//Y 排列方式
enum enYCollocateMode 
{ 
	enYTop,
	enYCenter,
	enYBottom,
};

//////////////////////////////////////////////////////////////////////////

//扑克控件
class CCardControl : public CWnd
{
	//状态变量
protected:
	bool								m_bShowFirst;					//显示扑克
	bool								m_bPositively;					//响应标志
	bool								m_bCaptureMouse;				//捕获标志
	CArrayTemplate<BYTE>				m_CardDataArray;				//扑克数组

	//位置信息
protected:
	CRgn								m_CardRgn;						//扑克区域
	CPoint								m_BasicStation;					//基准位置
	enXCollocateMode					m_XCollocateMode;				//显示模式
	enYCollocateMode					m_YCollocateMode;				//显示模式

	//静态变量
protected:
	static bool							m_bInit;						//初始标志
	static CSkinImage					m_ImageCard;					//扑克图片
	static CSkinImage					m_ImageBack;					//扑克图片

	//函数定义
public:
	//构造函数
	CCardControl();
	//析构函数
	virtual ~CCardControl();

	//功能函数
public:
	//获取数目
	WORD GetCardCount();
	//排列扑克
	void SortCardData();
	//显示扑克
	void ShowFirstCard(bool bShowFirst);
	//设置响应
	void AllowPositively(bool bPositively);
	//设置扑克
	WORD GetCardData(BYTE bCardBuffer[], WORD wBufferSize);
	//设置扑克
	bool SetCardData(const BYTE bCardData[], WORD wCardCount);
	//设置基准点函数
	void SetBasicStation(CPoint & BasicStation, enXCollocateMode XCollocateMode, enYCollocateMode YCollocateMode);

	//内部函数
private:
	//扑克数值
	//BYTE GetCardValue(BYTE bCardData);
	//扑克位置
	WORD GetCardXPos(BYTE bCardData);
	//扑克位置
	WORD GetCardYPos(BYTE bCardData);
	//扑克花色
//BYTE GetHuaKind(BYTE bCardData) { return (bCardData-1)/13; };
	//对比扑克
	//bool CompareOnlyOne(BYTE bFirstCard, BYTE bLastCard);
	//调整位置
	void RectifyControl();

	//消息函数
protected:
	//重画函数
	afx_msg void OnPaint();
	//建立消息
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标消息
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//鼠标消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//光标消息
	afx_msg BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT message);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif