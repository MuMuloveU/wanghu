#ifndef CARD_CONTROL_HEAD_FILE
#define CARD_CONTROL_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

//常量定义
#define INVALID_ITEM				0xFFFF								//无效子项

//属性定义
#define MAX_CARD_COUNT				41									//扑克数目
#define SPACE_CARD_DATA				255									//间距扑克

//数值掩码
#define	CARD_MASK_COLOR				0xF0								//花色掩码
#define	CARD_MASK_VALUE				0x0F								//数值掩码

//间距定义
#define DEF_X_DISTANCE				17									//默认间距
#define DEF_Y_DISTANCE				17									//默认间距
#define DEF_SHOOT_DISTANCE			20									//默认间距

//间距定义
#define DEF_X_DISTANCE_SMALL		17									//默认间距
#define DEF_Y_DISTANCE_SMALL		17									//默认间距
#define DEF_SHOOT_DISTANCE_SMALL	20									//默认间距

//////////////////////////////////////////////////////////////////////////
//枚举定义

//X 排列方式
enum enXCollocateMode 
{ 
	enXLeft,						//左对齐
	enXCenter,						//中对齐
	enXRight,						//右对齐
};

//Y 排列方式
enum enYCollocateMode 
{ 
	enYTop,							//上对齐
	enYCenter,						//中对齐
	enYBottom,						//下对齐
};

//////////////////////////////////////////////////////////////////////////

//扑克结构
struct tagCardItem
{
	bool							bShoot;								//弹起标志
	BYTE							cbCardData;							//扑克数据
};

//////////////////////////////////////////////////////////////////////////

//扑克控件
class CCardControl
{
	//状态变量
protected:
	bool							m_bSmallMode;						//小牌模式
	bool							m_bHorizontal;						//显示方向
	bool							m_bPositively;						//响应标志
	bool							m_bDisplayItem;						//显示标志

	//扑克数据
protected:
	WORD							m_wCardCount;						//扑克数目
	tagCardItem						m_CardItemArray[MAX_CARD_COUNT];	//扑克数据

	//选择变量
protected:
	WORD							m_wEndIndex;						//结束索引
	WORD							m_wStartIndex;						//开始索引

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
	CSkinImage						m_ImageCard;						//图片资源

	//函数定义
public:
	//构造函数
	CCardControl();
	//析构函数
	virtual ~CCardControl();

	//扑克控制
public:
	//设置扑克
	bool SetCardData(WORD wCardCount);
	//设置模式
	bool SetCardMode(bool bSmallMode);
	//设置扑克
	bool SetCardData(const BYTE cbCardData[], WORD wCardCount);
	//设置扑克
	bool SetCardItem(const tagCardItem CardItemArray[], WORD wCardCount);

	//弹起扑克
public:
	//设置扑克
	bool SetShootIndex(WORD wStartIndex, WORD wEndIndex);
	//设置扑克
	bool SetSelectIndex(WORD wStartIndex, WORD wEndIndex);
	//设置扑克
	bool SetShootCard(const BYTE cbCardData[], WORD wCardCount);

	//扑克控制
public:
	//删除扑克
	bool RemoveShootItem();

	//获取扑克
public:
	//获取扑克
	tagCardItem * GetCardFromIndex(WORD wIndex);
	//获取扑克
	tagCardItem * GetCardFromPoint(CPoint & MousePoint);

	//获取扑克
public:
	//扑克数目
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