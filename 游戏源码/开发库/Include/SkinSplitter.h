#ifndef SKIN_SPLITTER_HEAD_FILE
#define SKIN_SPLITTER_HEAD_FILE

#pragma once

#include "SkinImage.h"
#include "SkinButton.h"
#include "SkinControls.h"
//#include "SkinAttribute.h"
#include "SkinResourceManager.h"

//////////////////////////////////////////////////////////////////////////

//属性名字
//#define SKIN_SPLITTER_IMAGEHOR_PATH		TEXT("横向背景图")				// CString 类型 （读写）
//#define SKIN_SPLITTER_IMAGEVOR_PATH		TEXT("竖向背景图")				// CString 类型 （读写）

//拆分条属性
class CSkinSplitterAttribute : public CSkinResourceManager
{
	friend class CSkinSplitter;

	//GDI 对象
public:
	CSkinImage							m_ImageHor;						//拆分位图
	CSkinImage							m_ImageVor;						//拆分位图

	//函数定义
protected:
	//构造函数
	CSkinSplitterAttribute();
	//析构函数
	virtual ~CSkinSplitterAttribute();

	//接口函数
public:
	//加载配置
	bool UpdateSkinResource(ISkinResource * pISkinResource);
};

//////////////////////////////////////////////////////////////////////////

//类说明
class CSkinSplitter;

//拆分条类型
enum enSplitterType
{
	HorSpliter,
	VorSpliter,
};

//拆分条处理接口
interface ISplitterSink
{
	//按钮消息
	virtual bool __cdecl OnSplitterButton(CSkinSplitter * pSkinSplitter, CSkinButton * pSplitterButton)=NULL;
	//拆分条消息
	virtual bool __cdecl OnSplitterEvent(CSkinSplitter * pSkinSplitter, UINT uSplitID, int nXPos, int nYPos)=NULL;
};

//////////////////////////////////////////////////////////////////////////

//拆分条类
class SKIN_CONTROL_CLASS CSkinSplitter : public CWnd
{
	//变量定义
protected:
	int									m_nMaxPos;							//最大位置
	int									m_nLessPos;							//最小位置
	bool								m_bEnalbe;							//启用标志
	bool								m_bMouseDown;						//是否按下
	CRect								m_rcSplitter;						//拆分矩形
	enSplitterType						m_SplitterType;						//拆分条类型
	ISplitterSink						* m_pISplitterSink;					//拆分事件

	//控件变量
protected:
	CSkinButton							m_SplitterButton;					//拆分按钮

	//拆分条属性
public:
	static CSkinSplitterAttribute		m_SkinAttribute;					//拆分条属性

	//函数定义
public:
	//构造函数
	CSkinSplitter(enSplitterType SplitterType, ISplitterSink * pISplitterSink=NULL);
	//析构函数
	virtual ~CSkinSplitter();

	//重载函数
protected:
	//绑定函数
	virtual VOID PreSubclassWindow();

	//配置函数
public:
	//设置拆分范围
	void SetSplitterRange(int nLessPos, int nMaxPos);
	//设置拆分类型
	void SetSplitterType(enSplitterType SplitterType);
	//设置回调接口
	void SetSplitterSink(ISplitterSink * pISplitterSink);

	//功能函数
public:
	//设置启用
	void EnableSplitter(bool bEnable);
	//隐藏按钮
	bool HideSplitterButton();
	//显示按钮
	bool ShowSplitterButton(UINT uButtonImage, HINSTANCE hInstance);
	//按钮大小
	CSize GetButtonSize();

	//查询函数
public:
	//获取拆分类型
	enSplitterType GetSplitterType() { return m_SplitterType; }
	//获取回调接口
	ISplitterSink * GetSplitterSink() { return m_pISplitterSink; };
	//设置拆分范围
	void GetSplitterRange(int & nLessPos, int & nMaxPos) { nLessPos=m_nLessPos; nMaxPos=m_nMaxPos; }
	//获取属性接口
	static CSkinSplitterAttribute * GetSkinAttribute() { return &m_SkinAttribute; }

	//消息函数
protected:
	//重画消息
	afx_msg void OnPaint();
	//按键消息
	afx_msg void OnSplitterButton();
	//绘画背景
	afx_msg BOOL OnEraseBkgnd(CDC * pDC);
	//失去焦点
	afx_msg void OnKillFocus(CWnd * pNewWnd);
	//鼠标消息
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//鼠标消息
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//鼠标消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//位置消息
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//光标消息
	afx_msg BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT message);

	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CSkinSplitter)
};

//////////////////////////////////////////////////////////////////////////

#endif