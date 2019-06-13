#ifndef SMALL_CARD_CONTROL_HEAD_FILE
#define SMALL_CARD_CONTROL_HEAD_FILE

#pragma once

#include "Stdafx.h"
#define INVALID_ITEM				0xFFFF								//无效子项


//扑克控件
class CSmallCardControl
{
	//状态变量
protected:
	bool							m_bHorizontal;						//显示方向
	bool							m_bPositively;						//响应标志
	bool							m_bDisplayItem;						//显示标志

	//扑克数据
protected:
	WORD							m_wCardCount;						//扑克数目
	tagCardItem						m_CardItemArray[MAX_CARD_COUNT];	//扑克数据

	//间隔变量
protected:
	UINT							m_nXDistance;						//横向间隔
	UINT							m_nYDistance;						//竖向间隔
	UINT							m_nShootDistance;					//弹起高度

	//位置变量
protected:
	CPoint							m_BenchmarkPos;						//基准位置
	enXCollocateMode				m_XCollocateMode;					//显示模式
	enYCollocateMode				m_YCollocateMode;					//显示模式

	//资源变量
protected:
	CSize							m_CardSize;							//扑克大小
	static CSkinImage						m_ImageCard;						//图片资源

	//函数定义
public:
	//构造函数
	CSmallCardControl();
	//析构函数
	virtual ~CSmallCardControl();

	//扑克控制
public:
	//设置扑克
	bool SetCardData(WORD wCardCount);
	//设置扑克
	bool SetCardData(const BYTE cbCardData[], WORD wCardCount);
	//设置扑克
	bool SetShootCard(const BYTE cbCardData[], WORD wCardCount);
	//设置扑克
	bool SetCardItem(const tagCardItem CardItemArray[], WORD wCardCount);

	//获取扑克
public:
	//获取扑克
	tagCardItem * GetCardFromIndex(WORD wIndex);
	//获取扑克
	tagCardItem * GetCardFromPoint(CPoint & MousePoint);

	//获取扑克
public:
	//扑克数码
	WORD GetCardCount() { return m_wCardCount; }
	//获取扑克
	WORD GetCardData(BYTE cbCardData[], WORD wBufferCount);
	//获取扑克
	WORD GetShootCard(BYTE cbCardData[], WORD wBufferCount);
	//获取扑克
	WORD GetCardData(tagCardItem CardItemArray[], WORD wBufferCount);

	//状态查询
public:
	//获取大小
	CSize GetCardSize() { return m_CardSize; }
	//查询方向
	bool GetDirection() { return m_bHorizontal; }
	//查询响应
	bool GetPositively() { return m_bPositively; }
	//查询显示
	bool GetDisplayItem() { return m_bDisplayItem; }

	//状态控制
public:
	//设置方向
	VOID SetDirection(bool bHorizontal) { m_bHorizontal=bHorizontal; }
	//设置响应
	VOID SetPositively(bool bPositively) { m_bPositively=bPositively; }
	//设置显示
	VOID SetDisplayItem(bool bDisplayItem) { m_bDisplayItem=bDisplayItem; }
	//设置距离
	VOID SetCardDistance(UINT nXDistance, UINT nYDistance, UINT nShootDistance);

	//控件控制
public:
	//获取中心
	VOID GetCenterPoint(CPoint & CenterPoint);
	//基准位置
	VOID SetBenchmarkPos(INT nXPos, INT nYPos, enXCollocateMode XCollocateMode, enYCollocateMode YCollocateMode);
	//基准位置
	VOID SetBenchmarkPos(const CPoint & BenchmarkPos, enXCollocateMode XCollocateMode, enYCollocateMode YCollocateMode);

	//事件控制
public:
	//绘画扑克
	VOID DrawCardControl(CDC * pDC);
	//光标消息
	bool OnEventSetCursor(CPoint Point);
	/*static*/ void DrawOneCard(CDC* pDC, BYTE bCardData, int nX = 0, int nY = 0 );

	//内部函数
public:
	//获取大小
	VOID GetControlSize(CSize & ControlSize);
	//获取原点
	VOID GetOriginPoint(CPoint & OriginPoint);
	//索引切换
	WORD SwitchCardPoint(CPoint & MousePoint);
};

//////////////////////////////////////////////////////////////////////////

#endif