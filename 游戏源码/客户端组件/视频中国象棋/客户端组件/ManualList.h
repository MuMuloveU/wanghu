#ifndef MANUAL_LIST_HEAD_FILE
#define MANUAL_LIST_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//棋谱列表
class CManualList : public CListBox
{
	//变量定义
protected:
	CBrush							m_BackBrush;						//背景画刷

	//函数定义
public:
	//构造函数
	CManualList();
	//析构函数
	virtual ~CManualList();

	//消息映射
protected:
	//控件颜色
	HBRUSH CtlColor(CDC * pDC, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif
