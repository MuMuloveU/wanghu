#ifndef GOLD_VIEW_HEAD_FILE
#define GOLD_VIEW_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//筹码视图类
class CGoldView
{
	//变量定义
public:
	LONG								m_lGold;						//筹码数目
	INT									m_nGoldIndex;					//

	//辅助变量
protected:
	static bool							m_bInit;						//初始标志
	static CSkinImage					m_ImageGold;					//筹码图片

	//函数定义
public:
	//构造函数
	CGoldView();
	//析构函数
	virtual ~CGoldView();

	//功能函数
public:
	//设置筹码
	void SetGold(LONG lGold);
	//获取筹码
	LONG GetGold() { return m_lGold; };
	//绘画筹码
	void DrawGoldView(CDC * pDC, int nXPos, int nYPos);
	//设置绘画筹码索引
	VOID SetGoldIndex( INT nIndex );

	//内部函数
private:
	//整性变字符
	LPCTSTR GetGlodString(LONG lGold, TCHAR szString[]);
};

//////////////////////////////////////////////////////////////////////////

#endif