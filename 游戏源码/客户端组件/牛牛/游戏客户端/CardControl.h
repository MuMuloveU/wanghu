#ifndef CARD_CONTROL_HEAD_FILE
#define CARD_CONTROL_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////

//枚举定义

//数值掩码
#define	CARD_MASK_COLOR				0xF0								//花色掩码
#define	CARD_MASK_VALUE				0x0F								//数值掩码

//消息定义
#define	IDM_LEFT_HIT_CARD			(WM_USER+500)						//左击扑克
#define	IDM_RIGHT_HIT_CARD			(WM_USER+501)						//右击扑克
#define IDM_SHOW_VALUE				WM_USER+105							//点数信息
#define IDM_SHOW_INFO				WM_USER+106							//提示信息
#define IDM_OX_ENABLE				WM_USER+107							//牛牛按钮
#define IDM_SORT					WM_USER+108							//扑克分类

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

//扑克结构
struct tagCardItem
{
	bool							bShoot;								//弹起标志
	BYTE							bCardData;							//扑克数据
};

//////////////////////////////////////////////////////////////////////////

//扑克控件
class CCardControl : public CWnd
{
	//配置变量
protected:
	bool							m_bDisplay;							//显示标志
	bool							m_bHorizontal;						//横放标志
	bool							m_bPositively;						//响应标志
	bool							m_bKey;								//快捷键
	bool							m_bOX;								//牛牛标志
	//bool							m_bScot;							//分类标志

	//间隔变量
protected:
	DWORD							m_dwCardHSpace;						//横向间隔
	DWORD							m_dwCardVSpace;						//竖向间隔
	DWORD							m_dwShootAltitude;					//弹起高度
	DWORD							m_dwDrawHeight;						//绘画高度	

	//位置变量
protected:
	CPoint							m_BenchmarkPos;						//基准位置
	enXCollocateMode				m_XCollocateMode;					//显示模式
	enYCollocateMode				m_YCollocateMode;					//显示模式

	//运行变量
protected:
	DWORD							m_dwCurrentIndex;					//点击索引

	//内部数据
protected:
	CRgn							m_CardRegion;						//控件区域
	CWnd *							m_pSinkWindow;						//回调窗口
	CArrayTemplate<tagCardItem>		m_CardDataItem;						//扑克数据
	CGameLogic						m_GameLogic;						//逻辑函数

	//资源变量
protected:
	static bool						m_bLoad;							//加载标志
	static CSize					m_CardSize;							//扑克大小
	static CSkinImage				m_ImageCard;						//图片资源

	//函数定义
public:
	//构造函数
	CCardControl();
	//析构函数
	virtual ~CCardControl();

	//配置函数
public:
	//设置窗口
	void SetSinkWindow(CWnd * pSinkWindow);

	//控件控制
public:
	//绘画设置
	void SetDrawHeight(DWORD dwDrawHeight);
	//设置牛牛
	void SetOX(bool bOX);
	//设置快捷键
	void SetKey(bool Key);
	//弹起扑克
	void ShootAllCard(bool bShoot);
	//设置显示
	void SetDisplayFlag(bool bDisplay);
	//设置方向
	void SetDirection(bool bHorizontal);
	//设置响应
	void SetPositively(bool bPositively);
	//设置间距
	void SetCardSpace(DWORD dwCardHSpace, DWORD dwCardVSpace=18, DWORD dwShootAltitude=18);
	//基准位置
	void SetBenchmarkPos(int nXPos, int nYPos, enXCollocateMode XCollocateMode, enYCollocateMode YCollocateMode);
	//基准位置
	void SetBenchmarkPos(const CPoint & BenchmarkPos, enXCollocateMode XCollocateMode, enYCollocateMode YCollocateMode);

	//扑克控制
public:
	//获取快捷键
	bool GetKey(){return m_bKey;}
	//获取牛牛
	bool GetOX(){return m_bOX;}
	//获取数目
	DWORD GetCardCount();
	//获取弹起扑克数目
	DWORD GetShootCount();
	//获取扑克
	DWORD GetShootCard(BYTE bCardData[], bool bShoot=true);
	//设置扑克
	DWORD SetCardData(const BYTE bCardData[], DWORD dwCardCount);
	//获取扑克
	DWORD GetCardData(BYTE cbCardData[], DWORD dwMaxCount);
	//获取位置
	CPoint GetCardPos();
	//设置扑克
	DWORD SetCardItem(const tagCardItem CardItemArray[], DWORD dwCardCount);
	//设置弹起扑克
	DWORD SetShootCard(const BYTE bCardData[], DWORD dwCardCount);

	//内部函数
private:
	//调整位置
	void RectifyControl();
	//索引切换
	DWORD SwitchCardPoint(const CPoint & MousePoint);

	//消息函数
protected:
	//重画函数
	afx_msg void OnPaint();
	//建立消息
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标消息
	afx_msg void OnLButtonUp(UINT nFlags, CPoint Point);
	//鼠标消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint Point);
	//光标消息
	afx_msg BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT message);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif
