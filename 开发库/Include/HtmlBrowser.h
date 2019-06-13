#ifndef HTML_BROWSER_HEAD_FILE
#define HTML_BROWSER_HEAD_FILE

#pragma once

#include "ComService.h"

//////////////////////////////////////////////////////////////////////////

//浏览器视图
class COM_SERVICE_CLASS CHtmlBrower : public CHtmlView
{
	//变量定义
protected:
	bool							m_bEnableBrowser;					//是否启用
	CString							m_strNavigateUrl;					//页面地址

	//函数定义
public:
	//构造函数
	CHtmlBrower();
	//析构函数
	virtual ~CHtmlBrower();

	//功能函数
public:
	//转向网页
	void Navigate(LPCTSTR pszUrl);
	//启用网页
	bool EnableBrowser(bool bEnableBrowser);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif