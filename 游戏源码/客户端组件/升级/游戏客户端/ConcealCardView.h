#ifndef CONCEAL_CARD_VIEW_HEAD_FILE
#define CONCEAL_CARD_VIEW_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "Resource.h"
#include "CardControl.h"

//////////////////////////////////////////////////////////////////////////

//积分视图类
class CConcealCardView : public CWnd
{
	//结束信息
protected:
	BYTE							m_cbConcealCount;					//暗藏数目
	BYTE							m_cbConcealCard[8];					//暗藏扑克

	//资源变量
protected:
	CSize							m_CardSize;							//扑克大小
	CSkinImage						m_ImageBack;						//背景图案
	CSkinImage						m_ImageCard;						//扑克位图

	//函数定义
public:
	//构造函数
	CConcealCardView();
	//析构函数
	virtual ~CConcealCardView();
	
	//功能函数
public:
	//设置信息
	void SetConcealCard(BYTE cbConcealCard[], BYTE cbConcealCount);

	//消息函数
protected:
	//重画函数
	afx_msg void OnPaint();
	//建立消息
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif