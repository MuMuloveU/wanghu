#ifndef SKIN_LISTCTRL_HEAD_FILE
#define SKIN_LISTCTRL_HEAD_FILE

#pragma once

#include "SkinImage.h"
#include "SkinControls.h"
//#include "SkinAttribute.h"
#include "SkinResourceManager.h"

//类说明
class CSkinListCtrl;
class CSkinHeaderCtrl;

//////////////////////////////////////////////////////////////////////////

//属性名字
//#define SKIN_LIST_CRHEADTEXT			TEXT("列头字体颜色")			// COLORREF 类型 （读写）
//#define SKIN_LIST_STRIMAGEHEADPATH		TEXT("列头背景图")				// CString 类型 （读写）

//////////////////////////////////////////////////////////////////////////

//列表头回调接口
interface IHeaderCtrlSink
{
	//鼠标右键单击
	virtual void __cdecl OnHeaderRButtonDown(CSkinHeaderCtrl * pHeaderCtrl, UINT nFlags, CPoint Point)=NULL;
	//鼠标右键单击
	virtual void __cdecl OnHeaderRButtonUp(CSkinHeaderCtrl * pHeaderCtrl, UINT nFlags, CPoint Point)=NULL;
	//鼠标左键单击
	virtual void __cdecl OnHeaderLButtonDown(CSkinHeaderCtrl * pHeaderCtrl, UINT nFlags, CPoint Point)=NULL;
	//鼠标左键单击
	virtual void __cdecl OnHeaderLButtonUp(CSkinHeaderCtrl * pHeaderCtrl, UINT nFlags, CPoint Point)=NULL;
};

//////////////////////////////////////////////////////////////////////////

//列表类属性
class CSkinListAttribute : public CSkinResourceManager
{
	friend class CSkinListCtrl;
	friend class CSkinHeaderCtrl;

	//设置部分
public:
	COLORREF							m_crHeadText;					//字体颜色

	//GDI 对象
public:
	CSkinImage							m_ImageHead;					//列头位图

	//函数定义
protected:
	//构造函数
	CSkinListAttribute();
	//析构函数
	virtual ~CSkinListAttribute();

	//接口函数
public:
	//加载配置
	bool UpdateSkinResource(ISkinResource * pISkinResource);
};

//////////////////////////////////////////////////////////////////////////

//标题头标志
enum enHeaderSortType
{
	Type_NoSort,						//没有排序
	Type_Ascend,						//升序排序
	Type_Descale,						//降序排序
};

//////////////////////////////////////////////////////////////////////////

//换皮肤列表头
class SKIN_CONTROL_CLASS CSkinHeaderCtrl : public CHeaderCtrl
{
	//静态变量
protected:
	int									m_iSortCol;						//当前排列
	enHeaderSortType					m_SortType;						//排序标志
	IHeaderCtrlSink						* m_pIHeaderCtrlSink;			//回调接口

	//内部变量
protected:
	CDC									m_BufferDC;						//缓冲 DC

	//属性变量
public:
	static CSkinListAttribute			m_SkinAttribute;				//列表属性

	//函数定义
public:
	//构造函数
	CSkinHeaderCtrl();
	//析构函数
	virtual ~CSkinHeaderCtrl();

	//重载函数
protected:
	//绘画函数
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	//功能函数
public:
	//坐标转换
	int PointToColIndex(CPoint Point);
	//获取标题
	LPCTSTR GetItemText(int iCol, TCHAR szBuffer[], int nSize);
	//设置标志
	void SetSortArrow(int iSortCol, enHeaderSortType SortType);
	//获取标志
	void GetSortArrow(int & iSortCol, enHeaderSortType & SortType);
	//获取回调接口
	IHeaderCtrlSink * GetHeaderCtrlSink() { return m_pIHeaderCtrlSink; }
	//设置回调接口
	void SetHeaderCtrlSink(IHeaderCtrlSink * pIHeaderCtrlSink) { m_pIHeaderCtrlSink=pIHeaderCtrlSink; }

	//查询函数
public:
	//获取属性接口
	static CSkinListAttribute * GetSkinAttribute() { return &m_SkinAttribute; }

	//消息函数
protected:
	//重画消息
	afx_msg void OnPaint();
	//绘画背景
	afx_msg BOOL OnEraseBkgnd(CDC * pDC);
	//鼠标消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//鼠标消息
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//鼠标消息
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//鼠标消息
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//列表类
class SKIN_CONTROL_CLASS CSkinListCtrl : public CListCtrl
{
	//变量定义
protected:
	CSkinHeaderCtrl						m_SkinHeadCtrl;					//列头控件
	
	//函数定义
public:
	//构造函数
	CSkinListCtrl();
	//析构函数
	virtual ~CSkinListCtrl();

	//重载函数
public:
	//控件绑定
	virtual void PreSubclassWindow();

	//功能函数
public:
	//获取列表头
	CSkinHeaderCtrl & GetHeadCtrl() { return m_SkinHeadCtrl; }
	//设置标志
	void SetSortArrow(int iSortCol, enHeaderSortType SortType) { return m_SkinHeadCtrl.SetSortArrow(iSortCol,SortType); }
	//获取标志
	void GetSortArrow(int & iSortCol, enHeaderSortType & SortType) { m_SkinHeadCtrl.GetSortArrow(iSortCol,SortType); };
	//获取回调接口
	IHeaderCtrlSink * GetSinkSink() { return m_SkinHeadCtrl.GetHeaderCtrlSink(); }
	//设置回调接口
	void SetHeaderCtrlSink(IHeaderCtrlSink * pIHeaderCtrlSink) { m_SkinHeadCtrl.SetHeaderCtrlSink(pIHeaderCtrlSink); }

	//消息函数
protected:
	//建立消息
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//绘画背景
	afx_msg BOOL OnEraseBkgnd(CDC * pDC);

	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CSkinListCtrl)
};

//////////////////////////////////////////////////////////////////////////

#endif