#ifndef DLG_INDIVIDUAL_INFO_HEAD_FILE
#define DLG_INDIVIDUAL_INFO_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "Resource.h"

//////////////////////////////////////////////////////////////////////////

//登录对话框
class CDlgIndividualInfo : public CSkinDialogEx
{
	//控件变量
public:
	CWebBrowser							m_BrowerAD;						//浏览窗口

	//函数定义
public:
	//构造函数
	CDlgIndividualInfo();
	//析构函数
	virtual ~CDlgIndividualInfo();

	//重载函数
protected:
	//控件绑定
	virtual void DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif