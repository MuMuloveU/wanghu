#ifndef SKIN_RICH_EDIT_HEAD_FILE
#define SKIN_RICH_EDIT_HEAD_FILE

#pragma once

#include "SkinControls.h"


//////////////////////////////////////////////////////////////////////////

//图像 OLE 类
class SKIN_CONTROL_CLASS CImageDataObject : IDataObject
{
	//内核变量
protected:
	ULONG							m_ulRefCnt;							//引用计数
	STGMEDIUM 						m_StgMedium;						//对象信息
	FORMATETC 						m_FormatEtc;						//对象信息

	//接口变量
protected:
	IStorage *						m_pIStorage;						//对象接口
	IOleObject *					m_pIOleObject;						//对象接口

	//函数定义
public:
	//构造函数
	CImageDataObject();
	//析构函数
	virtual ~CImageDataObject();

	//基础接口
public:
	//增加引用
	virtual ULONG STDMETHODCALLTYPE AddRef();
	//释放对象
	virtual ULONG STDMETHODCALLTYPE Release();
	//查询接口
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, VOID * * ppvObject);

	//对象接口
public:
	//获取数据
	virtual HRESULT STDMETHODCALLTYPE GetData(FORMATETC * pformatetcIn, STGMEDIUM * pStgMedium);
	//设置数据
	virtual HRESULT STDMETHODCALLTYPE SetData(FORMATETC * pFormatEtc, STGMEDIUM * pStgMedium, BOOL fRelease);
	
	//其他接口
public:
	virtual HRESULT STDMETHODCALLTYPE DUnadvise(DWORD dwConnection) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE QueryGetData(FORMATETC * pFormatEtc) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE EnumDAdvise(IEnumSTATDATA * * ppenumAdvise) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE GetDataHere(FORMATETC * pFormatEtc, STGMEDIUM *  pStgMedium ) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC * * ppenumFormatEtc ) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE GetCanonicalFormatEtc(FORMATETC * pFormatEtcIn, FORMATETC * pFormatEtcOut) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE DAdvise(FORMATETC * pFormatEtc, DWORD advf, IAdviseSink * pAdvSink, DWORD * pdwConnection) { return E_NOTIMPL; }

	//配置函数
public:
	//设置图片
	bool SetImage(HBITMAP hBitmap);
	//加载图片
	bool LoadImage(LPCTSTR pszFileName);
	//加载图片
	bool LoadImage(HINSTANCE hInstance, LPCTSTR lpBitmapName);

	//图片函数
public:
	//获取图片
	HBITMAP GetBitmap() { return m_StgMedium.hBitmap; }
	//获取信息
	bool IntercalateReObject(REOBJECT & ReObject, IOleClientSite * pIOleClientSite);
};

//////////////////////////////////////////////////////////////////////////

using namespace ImageOleLib;
class _AFX_RICHEDITEX_STATE
{
public:
    _AFX_RICHEDITEX_STATE();
    virtual ~_AFX_RICHEDITEX_STATE();
    HINSTANCE m_hInstRichEdit20 ;
};


//消息框类
class SKIN_CONTROL_CLASS CSkinRichEdit : public CRichEditCtrl
{
	//接口变量
protected:
	IRichEditOle *					m_pIRichEditOle;					//OLE 指针

	//函数定义
public:
	//构造函数
	CSkinRichEdit();
	//析构函数
	virtual ~CSkinRichEdit();

	//重载函数
protected:
	//控件绑定
	virtual VOID PreSubclassWindow();

	//功能函数
public:
	//清除屏幕
	bool CleanScreen();
	//加载消息
	bool LoadMessage(LPCTSTR pszFileName);
	//保存信息
	bool SaveMessage(LPCTSTR pszFileName);

	//图片插入
public:
	//插入图片
	bool InsertImage(CBitmap * pBitmap);
	//插入图片
	void InsertImage( CString strPicPath );
	//插入对象
	bool InsertDataObject(CImageDataObject * pImageDataObject);

	//字串插入
public:
	//插入字串
	bool InsertString(LPCTSTR pszString, COLORREF crTextColor);
	//插入字串
	bool InsertString(LPCTSTR pszString, COLORREF crTextColor, COLORREF crBackColor);

	//超级链接
public:
	//插入字串
	bool InsertHyperLink(LPCTSTR pszString);
	//插入字串
	bool InsertHyperLink(LPCTSTR pszString, COLORREF crBackColor);

	//辅助函数
private:
	//插入字串
	bool InsertString(LPCTSTR pszString, CHARFORMAT2 & CharFormat);
	//加载回调
	static DWORD CALLBACK LoadCallBack(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG * pcb);
	//保存回调
	static DWORD CALLBACK SaveCallBack(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG * pcb);

	//内部函数
private:
	//字符判断
	bool EfficacyUrlChar(TCHAR chChar);
	//地址判断
	bool EfficacyUrlString(LPCTSTR pszUrl);

	//菜单命令
protected:
	//删除信息
	VOID OnClearAll();
	//全部选择
	VOID OnSelectAll();
	//拷贝字符
	VOID OnCopyString();
	//保存信息
	VOID OnSaveString();
	//删除字符
	VOID OnDeleteString();

	//消息映射
protected:
	//销毁消息
	VOID OnDestroy();
	//创建消息
	INT OnCreate(LPCREATESTRUCT lpCreateStruct);
	//右键消息
	VOID OnRButtonDown(UINT nFlags, CPoint point);
	//设置光标
	BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif