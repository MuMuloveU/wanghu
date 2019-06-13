#ifndef GLG_USER_REQUEST_HEAD_FILE
#define GLG_USER_REQUEST_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "Resource.h"

//////////////////////////////////////////////////////////////////////////

//请求对话框
class CDlgUserRequest : public CSkinDialogEx
{
	//配置定义
protected:
	UINT							m_nElapse;							//倒计时间
	UINT							m_uMessage;							//通知消息
	CString							m_strMessage;						//提示消息

	//辅助变量
protected:
	UINT							m_nResidualTime;					//剩余时间

	//控件变量
protected:
	CSkinButton						m_btApprove;						//同意按钮
	CSkinButton						m_btCancel;							//取消按钮

	//函数定义
public:
	//构造函数
	CDlgUserRequest();
	//析构函数
	virtual ~CDlgUserRequest();

	//重载函数
protected:
	//控件绑定
	virtual void DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定消息
	virtual void OnOK();
	//取消消息
	virtual void OnCancel();

	//功能函数
public:
	//配置请求
	void InitRequest(UINT uMessage, UINT nElapse, LPCTSTR pszMessage);

	//消息映射
protected:
	//同意按钮
	afx_msg void OnBnClickedApprove();
	//定时器消息
	afx_msg void OnTimer(UINT nIDEvent);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif