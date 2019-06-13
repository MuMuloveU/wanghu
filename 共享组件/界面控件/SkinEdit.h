#pragma once

#ifndef SKIN_EDIT_HEAD_FILE
#define SKIN_EDIT_HEAD_FILE

#include "SkinControls.h"
#include "SkinResourceManager.h"

//属性名字
//#define SKIN_EDIT_CRFOCUSTX			TEXT("编辑框焦点字色")				// COLORREF 类型 （读写）
//#define SKIN_EDIT_CRFOCUSBK			TEXT("编辑框焦点背景色")			// COLORREF 类型 （读写）
//#define SKIN_EDIT_CRNOFOCUSTX		TEXT("编辑框正常字色")				// COLORREF 类型 （读写）
//#define SKIN_EDIT_CRNOFOCUSBK		TEXT("编辑框正常背景色")			// COLORREF 类型 （读写）
//#define SKIN_EDIT_CRDISFOCUSTX		TEXT("编辑框禁止字色")				// COLORREF 类型 （读写）
//#define SKIN_EDIT_CRDISFOCUSBK		TEXT("编辑框禁止背景色")			// COLORREF 类型 （读写）

//////////////////////////////////////////////////////////////////////////

//编辑框属性
class CSkinEditAttribute : public CSkinResourceManager
{
	friend class CSkinEdit;

	//设置部分
public:
	COLORREF							m_crFocusTX;					//字体颜色
	COLORREF							m_crFocusBK;					//背景颜色
	COLORREF							m_crNoFocusTX;					//字体颜色
	COLORREF							m_crNoFocusBK;					//背景颜色
	COLORREF							m_crDisFocusTX;					//字体颜色
	COLORREF							m_crDisFocusBK;					//背景颜色

	//GDI 对象
public:
	CBrush								m_brFocus;						//背景刷
	CBrush								m_brNoFocus;					//背景刷
	CBrush								m_brDisbale;					//背景刷

	//函数定义
protected:
	//构造函数
	CSkinEditAttribute();
	//析构函数
	virtual ~CSkinEditAttribute();

	//接口函数
public:
	//加载配置
	bool UpdateSkinResource(ISkinResource * pISkinResource);
};

//////////////////////////////////////////////////////////////////////////

//编辑框
class SKIN_CONTROL_CLASS CSkinEdit : public CEdit
{
	//变量定义
protected:
	bool							m_bFocus;						//焦点标志

	//静态变量
public:
	static CSkinEditAttribute		m_SkinAttribute;				//界面属性

	//函数定义
public:
	//构造函数
	CSkinEdit();
	//析构函数
	virtual ~CSkinEdit();

	//查询函数
public:
	//获取属性接口
	static CSkinEditAttribute * GetSkinAttribute() { return &m_SkinAttribute; }

	//消息函数 
protected:
	//绘画控件
	afx_msg HBRUSH CtlColor(CDC * pDC, UINT nCtlColor);
	//得到焦点
	afx_msg void OnSetFocus(CWnd * pOldWnd);
	//失去焦点
	afx_msg void OnKillFocus(CWnd * pNewWnd);

	//消息映射
protected:
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CSkinEdit)
};

//////////////////////////////////////////////////////////////////////////

#endif