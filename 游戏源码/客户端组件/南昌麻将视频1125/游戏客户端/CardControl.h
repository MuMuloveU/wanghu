#ifndef CARD_CONTROL_HEAD_FILE
#define CARD_CONTROL_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "Resource.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////
//	扑克定义

//	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//万子
//	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//索子
//	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//同子
//	0x31,0x32,0x33,0x34,0x35,0x36,0x37,									//番子
//	0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,							//花子

//////////////////////////////////////////////////////////////////////////

//扑克定义
#define HEAP_FULL_COUNT				34									//堆立全牌

//////////////////////////////////////////////////////////////////////////

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

//方向枚举
enum enDirection
{
	Direction_East,					//东向
	Direction_South,				//南向
	Direction_West,					//西向
	Direction_North,				//北向
};

//////////////////////////////////////////////////////////////////////////

//扑克图片
class CCardListImage
{
	//位置变量
protected:
	int								m_nItemWidth;						//子项高度
	int								m_nItemHeight;						//子项宽度
	int								m_nViewWidth;						//子项高度
	int								m_nViewHeight;						//子项宽度

	//资源变量
protected:
	CSkinImage						m_CardListImage;					//图片资源

	//函数定义
public:
	//构造函数
	CCardListImage();
	//析构函数
	virtual ~CCardListImage();

	//信息函数
public:
	//获取宽度
	int GetViewWidth() { return m_nViewWidth; }
	//获取高度
	int GetViewHeight() { return m_nViewHeight; }

	//资源管理
public:
	//加载资源
	bool LoadResource(UINT uResourceID, int nViewWidth, int nViewHeight);
	//释放资源
	bool DestroyResource();

	//功能函数
public:
	//获取位置
	inline int GetImageIndex(BYTE cbCardData);
	//绘画扑克
	inline bool DrawCardItem(CDC * pDestDC, BYTE cbCardData, int xDest, int yDest);
};

//////////////////////////////////////////////////////////////////////////

//扑克资源
class CCardResource
{
	//用户扑克
public:
	CSkinImage						m_ImageUserTop;						//图片资源
	CSkinImage						m_ImageUserLeft;					//图片资源
	CSkinImage						m_ImageUserRight;					//图片资源
	CCardListImage					m_ImageUserBottom;					//图片资源
	CCardListImage					m_ImageUserKingBottom;				//图片资源
	CCardListImage					m_ImageUserViceKingBottom;			//图片资源

	//桌子扑克
public:
	CCardListImage					m_ImageTableTop;					//图片资源
	CCardListImage					m_ImageTableLeft;					//图片资源
	CCardListImage					m_ImageTableRight;					//图片资源
	CCardListImage					m_ImageTableBottom;					//图片资源
	CCardListImage					m_ImageTableBottomBig;				//图片资源

	CCardListImage					m_ImageUpDown;						//图片资源

	//正精
	CCardListImage					m_ImageTableKingTop;				//上边正精
	CCardListImage					m_ImageTableKingLeft;				//左边正精
	CCardListImage					m_ImageTableKingRight;				//右边正精
	CCardListImage					m_ImageTableKingBottom;				//下边正精	
	CCardListImage					m_ImageTableKingBottomBig;			//下边正精	

	//副精
	CCardListImage					m_ImageTableViceKingTop;			//上边副精
	CCardListImage					m_ImageTableViceKingLeft;			//左边副精
	CCardListImage					m_ImageTableViceKingRight;			//右边副精
	CCardListImage					m_ImageTableViceKingBottom;			//下边副精	
	CCardListImage					m_ImageTableViceKingBottomBig;		//下边副精	

	//牌堆扑克
public:
	CSkinImage						m_ImageHeapSingleV;					//图片资源
	CSkinImage						m_ImageHeapSingleH;					//图片资源
	CSkinImage						m_ImageHeapDoubleV;					//图片资源
	CSkinImage						m_ImageHeapDoubleH;					//图片资源

	CCardListImage					m_ImageKingTop;						//图片资源
	CCardListImage					m_ImageKingBottom;					//图片资源
	CCardListImage					m_ImageKingLeft;					//图片资源
	CCardListImage					m_ImageKingRight;					//图片资源


	//函数定义
public:
	//构造函数
	CCardResource();
	//析构函数
	virtual ~CCardResource();

	//功能函数
public:
	//加载资源
	bool LoadResource();
	//消耗资源
	bool DestroyResource();
};

//////////////////////////////////////////////////////////////////////////

//堆立扑克
class CHeapCard
{
	//控制变量
protected:
	CPoint							m_ControlPoint;						//基准位置
	enDirection						m_CardDirection;					//扑克方向

	//扑克变量
protected:
	WORD							m_wFullCount;						//全满数目
	WORD							m_wMinusHeadCount;					//头部空缺
	WORD							m_wMinusLastCount;					//尾部空缺
	WORD							m_wKingPos;							//精牌位置		
	BYTE							m_cbKingCardData;					//精牌的值

	//函数定义
public:
	//构造函数
	CHeapCard();
	//析构函数
	virtual ~CHeapCard();

	//功能函数
public:
	//绘画扑克
	void DrawCardControl(CDC * pDC);
	//设置扑克
	bool SetCardData(WORD wMinusHeadCount, WORD wMinusLastCount, WORD wFullCount);
	//设置精牌位置
	void SetKingPos(WORD wKingPos) { m_wKingPos = wKingPos-1; }
	//设置正精
	void SetKingCardData(BYTE cbCardData){m_cbKingCardData=cbCardData;}

	//控件控制
public:
	//设置方向
	void SetDirection(enDirection Direction) { m_CardDirection=Direction; }
	//基准位置
	void SetControlPoint(int nXPos, int nYPos) { m_ControlPoint.SetPoint(nXPos,nYPos); }
};

//////////////////////////////////////////////////////////////////////////

//组合扑克
class CWeaveCard
{
	//控制变量
protected:
	BYTE							m_bDisplayItem;						//显示标志
	CPoint							m_ControlPoint;						//基准位置
	enDirection						m_CardDirection;					//扑克方向
	BYTE							m_cbKingCardIndx[2];				//精牌的值


	//扑克数据
protected:
	WORD							m_wCardCount;						//扑克数目
	BYTE							m_cbCardData[4];					//扑克数据

	//函数定义
public:
	//构造函数
	CWeaveCard();
	//析构函数
	virtual ~CWeaveCard();

	//功能函数
public:
	//绘画扑克
	void DrawCardControl(CDC * pDC);
	//绘画扑克
	void DrawCardControl(CDC * pDC, int nXPos, int nYPos);
	//设置扑克
	bool SetCardData(const BYTE cbCardData[], WORD wCardCount);

	//控件控制
public:
	//设置显示
	void SetDisplayItem(BYTE bDisplayItem) { m_bDisplayItem=bDisplayItem; }
	//设置方向
	void SetDirection(enDirection Direction) { m_CardDirection=Direction; }
	//基准位置
	void SetControlPoint(int nXPos, int nYPos) { m_ControlPoint.SetPoint(nXPos,nYPos); }
	//设置王牌
	void SetKingCardIndex(BYTE cbKingCardIndex[2])	{CopyMemory(m_cbKingCardIndx,cbKingCardIndex,sizeof(m_cbKingCardIndx));}


	//内部函数
private:
	//获取扑克
	inline BYTE GetCardData(WORD wIndex);
};

//////////////////////////////////////////////////////////////////////////

//用户扑克
class CUserCard
{
	//扑克数据
protected:
	WORD							m_wCardCount;						//扑克数目
	bool							m_bCurrentCard;						//当前标志

	//控制变量
protected:
	CPoint							m_ControlPoint;						//基准位置
	enDirection						m_CardDirection;					//扑克方向

	//函数定义
public:
	//构造函数
	CUserCard();
	//析构函数
	virtual ~CUserCard();

	//功能函数
public:
	//绘画扑克
	void DrawCardControl(CDC * pDC);
	//设置扑克
	bool SetCurrentCard(bool bCurrentCard);
	//设置扑克
	bool SetCardData(WORD wCardCount, bool bCurrentCard);

	//控件控制
public:
	//设置方向
	void SetDirection(enDirection Direction) { m_CardDirection=Direction; }
	//基准位置
	void SetControlPoint(int nXPos, int nYPos) { m_ControlPoint.SetPoint(nXPos,nYPos); }
};

//////////////////////////////////////////////////////////////////////////

//丢弃扑克
class CDiscardCard
{
	//扑克数据
protected:
	WORD							m_wCardCount;						//扑克数目
	BYTE							m_cbCardData[35];					//扑克数据
	BYTE							m_cbKingCardIndex[2];					//精牌的值

	//控制变量
protected:
	CPoint							m_ControlPoint;						//基准位置
	enDirection						m_CardDirection;					//扑克方向

	//函数定义
public:
	//构造函数
	CDiscardCard();
	//析构函数
	virtual ~CDiscardCard();

	//功能函数
public:
	//绘画扑克
	void DrawCardControl(CDC * pDC);
	//增加扑克
	bool AddCardItem(BYTE cbCardData);
	//设置扑克
	bool SetCardData(const BYTE cbCardData[], WORD wCardCount);

	//控件控制
public:
	//设置方向
	void SetDirection(enDirection Direction) { m_CardDirection=Direction; }
	//基准位置
	void SetControlPoint(int nXPos, int nYPos) { m_ControlPoint.SetPoint(nXPos,nYPos); }
	//设置王牌
	void SetKingCardIndex(BYTE cbKingCardIndex[2]) {CopyMemory(m_cbKingCardIndex,cbKingCardIndex,sizeof(m_cbKingCardIndex));}
	//获取位置
	CPoint GetLastCardPosition();

};

//////////////////////////////////////////////////////////////////////////

//桌面扑克
class CTableCard
{
	//扑克数据
protected:
	WORD							m_wCardCount;						//扑克数目
	BYTE							m_cbCardData[MAX_COUNT];			//扑克数据
	BYTE							m_cbKingCardIndex[2];					//王牌数据

	//控制变量
protected:
	CPoint							m_ControlPoint;						//基准位置
	enDirection						m_CardDirection;					//扑克方向

	//函数定义
public:
	//构造函数
	CTableCard();
	//析构函数
	virtual ~CTableCard();

	//功能函数
public:
	//绘画扑克
	void DrawCardControl(CDC * pDC);
	//设置扑克
	bool SetCardData(const BYTE cbCardData[], WORD wCardCount);

	//控件控制
public:
	//设置方向
	void SetDirection(enDirection Direction) { m_CardDirection=Direction; }
	//基准位置
	void SetControlPoint(int nXPos, int nYPos) { m_ControlPoint.SetPoint(nXPos,nYPos); }
	//设置王牌
	void SetKingCardIndex(BYTE cbKingCardIndex[2]);
};

//////////////////////////////////////////////////////////////////////////

//扑克结构
struct tagCardItem
{
	bool							bShoot;								//弹起标志
	BYTE							cbCardData;							//扑克数据
};

//扑克控件
class CCardControl
{
	//状态变量
protected:
	bool							m_bPositively;						//响应标志
	bool							m_bDisplayItem;						//显示标志

	//位置变量
protected:
	CPoint							m_BenchmarkPos;						//基准位置
	enXCollocateMode				m_XCollocateMode;					//显示模式
	enYCollocateMode				m_YCollocateMode;					//显示模式

	//扑克数据
protected:
	WORD							m_wCardCount;						//扑克数目
	WORD							m_wHoverItem;						//盘旋子项
	tagCardItem						m_CurrentCard;						//当前扑克
	tagCardItem						m_CardItemArray[MAX_COUNT-1];		//扑克数据
	BYTE							m_cbKingCardIndex[2];					//王牌数据

	//资源变量
protected:
	CSize							m_ControlSize;						//控件大小
	CPoint							m_ControlPoint;						//控件位置

	//函数定义
public:
	//构造函数
	CCardControl();
	//析构函数
	virtual ~CCardControl();

	//查询函数
public:
	//获取扑克
	BYTE GetHoverCard();
	//获取扑克
	BYTE GetCurrentCard() { return m_CurrentCard.cbCardData; }

	//扑克控制
public:
	//设置扑克
	bool SetCurrentCard(BYTE cbCardData);
	//设置扑克
	bool SetCurrentCard(tagCardItem CardItem);
	//设置扑克
	bool SetCardData(const BYTE cbCardData[], WORD wCardCount, BYTE cbCurrentCard);
	//设置扑克
	bool SetCardItem(const tagCardItem CardItemArray[], WORD wCardCount);
	//设置王牌
	void SetKingCardIndex(BYTE cbKingCardIndex[2]);


	//控件控制
public:
	//设置响应
	void SetPositively(bool bPositively) { m_bPositively=bPositively; }
	//设置显示
	void SetDisplayItem(bool bDisplayItem) { m_bDisplayItem=bDisplayItem; }
	//基准位置
	void SetBenchmarkPos(int nXPos, int nYPos, enXCollocateMode XCollocateMode, enYCollocateMode YCollocateMode);
	//基准位置
	void SetBenchmarkPos(const CPoint & BenchmarkPos, enXCollocateMode XCollocateMode, enYCollocateMode YCollocateMode);

	//事件控制
public:
	//绘画扑克
	void DrawCardControl(CDC * pDC);
	//光标消息
	bool OnEventSetCursor(CPoint Point, bool & bRePaint);

	//内部函数
private:
	//索引切换
	WORD SwitchCardPoint(CPoint & MousePoint);
};

//////////////////////////////////////////////////////////////////////////

//变量声明
//导出变量
extern CCardResource				g_CardResource;						//扑克资源

//////////////////////////////////////////////////////////////////////////

#endif