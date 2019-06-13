#ifndef JETTON_CONTROL_HEAD_FILE
#define JETTON_CONTROL_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//属性定义
#define JETTON_COUNT				14									//筹码数目

//绘画模式
enum enDrawMode
{
	enDrawMode_Layer,			//层叠模式
	enDrawMode_Rand				//随机模式
};

//绘画筹码信息
struct tagJettonDraw
{
	WORD	wDrawCount;
	WORD	wDrawIndex;
};

//////////////////////////////////////////////////////////////////////////

//筹码控件
class CJettonControl
{
	//变量定义
protected:
	LONG							m_lScore;							//筹码数目
	LONG							m_lDrawScore;						//
	CPoint							m_BenchmarkPos;						//基准位置
	enDrawMode						m_DrawMode;							//绘画模式

	//位图变量
protected:
	CPngImage						m_PngJetton;						//筹码位图

	//位置变量
protected:
	CArrayTemplate<tagJettonDraw>	m_JetonDraw;						//绘画筹码
	CArrayTemplate<CPoint>			m_DrawExcusions;					//绘画位置

	//函数定义
protected:
	//调整控件
	void RectifyControl();
public:
	//构造函数
	CJettonControl();
	//析构函数
	virtual ~CJettonControl();

	//功能函数
public:
	//获取筹码
	LONG GetScore() { return m_lScore; }
	//设置筹码
	VOID SetScore(LONG lScore);
	//加注
	void AddScore(LONG lScore);
	//设置绘画模式
	void SetDrawMode( enDrawMode DrawMode );

	//位置函数
public:
	//基准位置
	VOID SetBenchmarkPos(INT nXPos, INT nYPos);
	//获取位置
	CPoint GetBenchmarkPos() { return m_BenchmarkPos; }

	//控制函数
public:
	//绘画控件
	VOID DrawJettonControl(CDC * pDC);
};

//////////////////////////////////////////////////////////////////////////

#endif