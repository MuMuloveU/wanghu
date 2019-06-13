#ifndef JETON_CONTROL_HEAD_FILE
#define JETON_CONTROL_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//位置定义
#define JETOM_IMAGE_USER				0								//筹码位置
#define JETOM_IMAGE_SECOND				1								//筹码位置
#define JETOM_IMAGE_THIRDLY				2								//筹码位置
#define JETOM_IMAGE_FOURTHLY			3								//筹码位置
#define JETOM_IMAGE_FIFTHLY				4								//筹码位置

//////////////////////////////////////////////////////////////////////////

//筹码控件
class CJetonControl
{
	//状态变量
protected:
	LONG							m_lJeton;							//筹码数目

	//绘画变量
protected:
	int								m_nImagePos;						//筹码位置
	CPoint							m_BenchmarkPos;						//基准位置

	//辅助变量
protected:
	static bool						m_bLoad;							//初始标志
	static CSkinImage				m_ImageJeton;						//筹码图片

	//函数定义
public:
	//构造函数
	CJetonControl();
	//析构函数
	virtual ~CJetonControl();

	//功能函数
public:
	//获取筹码
	LONG GetJeton();
	//设置筹码
	void SetJeton(LONG lJeton);
	//设置索引
	void SetImagePos(int nImagePos);

	//绘画控制
public:
	//绘画筹码
	void DrawJetonControl(CDC * pDC);
	//绘画位置
	bool SetBenchmarkPos(int nXPos, int nYPos);
};

//////////////////////////////////////////////////////////////////////////

#endif
