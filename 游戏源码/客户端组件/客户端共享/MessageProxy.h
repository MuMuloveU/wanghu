#pragma once

#include "Expression.h"
#include "ClientShare.h"

//////////////////////////////////////////////////////////////////////////

//信息代理组件
class CMessageProxy : public IMessageProxy
{
	//变量定义
protected:
	CWnd							*m_pParentWnd;						//父窗口
	CSkinRichEdit					* m_pSkinRichEdit;					//消息组件
	tagMessageOption				m_MessageOption;					//配置信息
	TCHAR							m_szPath[MAX_PATH];					//当前路径
    
	//函数定义
public:
	//构造函数
	CMessageProxy(void);
	//析构函数
	virtual ~CMessageProxy(void);

	//基础接口
public:
	//释放对象
	virtual VOID __cdecl Release() { if (IsValid()) delete this; }
	//是否有效
	virtual bool __cdecl IsValid() { return AfxIsValidAddress(this,sizeof(CMessageProxy))?true:false; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//配置接口
public:
	//设置句柄
	virtual bool __cdecl SetRichEditHwnd(CSkinRichEdit * pSkinRichEdit);
	//设置颜色
	virtual bool __cdecl SetMessageOption(tagMessageOption & MessageOption);

	//控制接口
public:
	//清除屏幕
	virtual bool __cdecl CleanScreen();
	//设置滚动
	virtual bool __cdecl SetAutoScroll(bool bScroll);
	//加载消息
	virtual bool __cdecl LoadMessage(LPCTSTR pszFileName);
	//保存信息
	virtual bool __cdecl SaveMessage(LPCTSTR pszFileName);
	//获取指针
	virtual CSkinRichEdit * __cdecl GetRichEdit(){return m_pSkinRichEdit;}

	//信息接口
public:
	//插入图片
	virtual bool __cdecl InsertImage(CBitmap * pBitmap);
	//插入时间
	virtual bool __cdecl InsertSystemTime(COLORREF crColor);
	//用户名字
	virtual bool __cdecl InsertUserName(LPCTSTR pszUserName);
	//系统消息
	virtual bool __cdecl InsertSystemString(LPCTSTR pszString, UINT uSize);
	//特色消息
	virtual bool __cdecl InsertShtickString(LPCTSTR pszString, COLORREF crColor, UINT uSize, bool bNewLine);
	//普通消息
	virtual bool __cdecl InsertGeneralString(LPCTSTR pszString, COLORREF crColor, UINT uSize, bool bNewLine);
	//用户聊天
	virtual bool __cdecl InsertUserChat(LPCTSTR pszSendName, LPCTSTR pszString, COLORREF crColor, UINT uSize);
	//用户聊天
	virtual bool __cdecl InsertUserChat(LPCTSTR pszSendName, LPCTSTR pszRecvName, LPCTSTR pszString, COLORREF crColor, UINT uSize);

	//内部函数
private:
	//解释插入
	bool TranslateInsert(LPCTSTR pszString, COLORREF crColor, UINT uSize, LPCTSTR pszFontName);
	//解释字符
	int TranslateChar(LPCTSTR szCharString, int iCharLength, int & iPassCharCount);
};

//////////////////////////////////////////////////////////////////////////
