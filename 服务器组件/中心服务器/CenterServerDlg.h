#pragma once

#include "Stdafx.h"
#include "Resource.h"
#include "CenterService.h"

//////////////////////////////////////////////////////////////////////////

//配置对话框
class CSystemOptionDlg : public CDialog
{
public:
	CSystemOptionDlg();
	virtual ~CSystemOptionDlg();

	//重载函数
protected:
	//初始化函数
	virtual BOOL OnInitDialog();
	//控件子类化
	virtual void DoDataExchange(CDataExchange * pDX);
	//确定函数
	virtual void OnOK();

	//功能函数
public:
	//配置参数
	void SetInitParamter(const CInitParamter & InitParamter);

	//消息映射
	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//主对话框
class CCenterServerDlg : public CDialog
{
	//组件变量
public:
	CCenterService					m_CenterService;					//登录服务
	CRichEditTrace					m_RichEditTrace;					//事件记录

	//构造函数
public:
	//构造程序
	CCenterServerDlg();
	//析构函数
	virtual ~CCenterServerDlg();

	//重载函数
protected:
	//控件子类化
	virtual void DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定消息
	virtual void OnOK();
	//取消函数
	virtual void OnCancel();
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	//消息函数
protected:
	//启动服务
	afx_msg void OnBnClickedStartService();
	//停止服务
	afx_msg void OnBnClickedStopService();
	//配置服务
	afx_msg void OnBnClickedInitService();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
