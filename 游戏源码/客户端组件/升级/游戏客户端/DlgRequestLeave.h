#ifndef DLG_REQUEST_LEAVE_HEAD_FILE
#define DLG_REQUEST_LEAVE_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameClientView.h"

//////////////////////////////////////////////////////////////////////////

//请求离开
class CDlgRequestLeave : public CSkinDialogEx
{
	//变量定义
protected:
	CWnd 							*m_pGameFrameDlg;					//引擎接口

	//控件变量
public:
	CSkinButton						m_btOk;								//确定按钮
	CSkinButton						m_btCancel;							//取消按钮

	//函数定义
public:
	//构造函数
	CDlgRequestLeave();
	//析构函数
	virtual ~CDlgRequestLeave();

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
	//设置接口
	void SetGameFrameEngine(CWnd * pIGameFrameDlg) { m_pGameFrameDlg=pIGameFrameDlg; }

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif