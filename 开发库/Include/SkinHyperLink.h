#ifndef SKIN_HYPER_LINK_HEAD_FILE
#define SKIN_HYPER_LINK_HEAD_FILE

#pragma once

#include "SkinControls.h"
#include "SkinResourceManager.h"

//////////////////////////////////////////////////////////////////////////

//属性名字
//#define SKIN_LINK_CRHOVERTEXT			TEXT("盘旋字体颜色")			// COLORREF 类型 （读写）
//#define SKIN_LINK_CRVISITEDTEXT			TEXT("访问字体颜色")			// COLORREF 类型 （读写）
//#define SKIN_LINK_CRNORMALTEXT			TEXT("正常字体颜色")			// COLORREF 类型 （读写）
//#define SKIN_LINK_CRBACKGROUND			TEXT("字体背景颜色")			// COLORREF 类型 （读写）

//列表类属性
class CSkinHyperLinkAttribute : public CSkinResourceManager
{
	friend class CSkinHyperLink;

	//设置部分
public:
	COLORREF							m_crHoverText;					//字体颜色
	COLORREF							m_crVisitedText;				//字体颜色
	COLORREF							m_crNormalText;					//字体颜色
	COLORREF							m_crBackColor;					//背景颜色

	//函数定义
protected:
	//构造函数
	CSkinHyperLinkAttribute();
	//析构函数
	virtual ~CSkinHyperLinkAttribute();

	//接口函数
public:
	//加载配置
	bool UpdateSkinResource(ISkinResource * pISkinResource);
};

//////////////////////////////////////////////////////////////////////////

//超连接类
class SKIN_CONTROL_CLASS CSkinHyperLink : public CStatic
{
	//变量定义
protected:
   	bool								m_bHovering;					//盘旋标志
	bool								m_bVisited;                     //是否访问
    bool								m_bUnderline;					//是否划线
    bool								m_bAutoAdjust;					//自动调整
    CString								m_strURL;						//连接地址
	COLORREF							m_crBackColor;					//背景颜色

	//接口指针
protected:
	IMouseEvent							* m_pIMouseEvent;				//鼠标事件

	//控件变量
protected:
    CToolTipCtrl						m_ToolTip;						//提示控件

	//属性变量
public:
	static CSkinHyperLinkAttribute		m_SkinAttribute;				//超连接属性

	//函数定义
public:
	//构造函数
	CSkinHyperLink();
	//析构函数
	virtual ~CSkinHyperLink();

	//配置函数
public:
	//设置鼠标回调接口
	void SetMouseEventSink(IUnknownEx * pIUnknownEx);
	//获取鼠标回调接口
	void * GetMouseEventSink(const IID & Guid, DWORD dwQueryVer);

	//查询函数
public:
	//获取调整
	bool GetAutoAdjust() { return m_bAutoAdjust; }
	//获取访问
	bool GetVisited() { return m_bVisited; }
	//获取下划线
	bool GetUnderline() { return m_bUnderline; }
	//获取 URL
	LPCTSTR GetHyperLinkUrl() { return m_strURL; }

	//设置函数
public:
	//设置访问
    void SetVisited(bool bVisited);
	//设置下划线
    void SetUnderline(bool bUnderline);
	//设置调整
    void SetAutoAdjust(bool bAutoAdjust);
	//设置 URL
	void SetHyperLinkUrl(LPCTSTR pszUrl);
	//背景颜色
	void SetBackGroundColor(COLORREF crColor);

	//重载函数
protected:
	//控件子类化
	virtual void PreSubclassWindow();
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//默认回调函数
	LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	//查询函数
public:
	//获取属性接口
	static CSkinHyperLinkAttribute * GetSkinAttribute() { return &m_SkinAttribute; }

	//内部函数
protected:
	//初始化
	void InitHyperLink();
	//更新提示
	void UpdateToolTip();
	//调整位置
	void AdjustHyperLink();

	//消息函数
protected:
	//建立消息
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//绘画消息
	afx_msg void OnPaint();
	//单击控件
	afx_msg void OnStnClicked();
	//鼠标移动
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//光标消息
    afx_msg BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT message);
	//鼠标离开
	afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
	//标题消息
	afx_msg LRESULT	OnSetTextMesage(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CSkinHyperLink)
};

//////////////////////////////////////////////////////////////////////////

#endif