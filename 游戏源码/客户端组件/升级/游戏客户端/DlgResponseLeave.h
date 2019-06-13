#ifndef DLG_RESPONSE_LEAVE_HEAD_FILE
#define DLG_RESPONSE_LEAVE_HEAD_FILE

#pragma once


#include "Stdafx.h"
#include "GameClientView.h"

//////////////////////////////////////////////////////////////////////////

//请求离开回应
class CDlgResponseLeave : public CSkinDialogEx
{
	//定义变量
protected:
	WORD							m_wRequestUser;						//请求用户
	CWnd							*m_pGameFrameDlg;					//回调
	WORD							m_wTimer;							//定时时间
	CString							m_strCaption;						//对话框标题

	//控件变量
public:
	CSkinButton						m_btOk;								//确定按钮
	CSkinButton						m_btCancel;							//取消按钮

	//函数定义
public:
	//构造函数
	CDlgResponseLeave(CWnd * pParentWnd=NULL);
	//析构函数
	virtual ~CDlgResponseLeave();

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定函数
	virtual VOID OnOK();
	//取消消息
	virtual VOID OnCancel();

	//功能函数
public:
	//设置消息
	void ShowRequestMessage(WORD wChairId,LPCTSTR pszCaption,LPCTSTR pszMessage);
	//定时器
	afx_msg void OnTimer(UINT nIDEvent);
	//设置回调
	void SetGameFrameDlg( CWnd *pWnd ) { m_pGameFrameDlg = pWnd; }
	//获取用户ID
	WORD GetRequestUser() { return m_wRequestUser; }

	DECLARE_MESSAGE_MAP()
};


//////////////////////////////////////////////////////////////////////////

#endif