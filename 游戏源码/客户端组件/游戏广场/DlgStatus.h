#ifndef DLG_STATUS_HEAD_FILE
#define DLG_STATUS_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//游戏状态类
class CDlgStatus : public CDialog
{
	//变量定义
protected:
	WORD								m_wImagePos;					//位图位置					
	CWnd								* m_pNotifyWnd;					//通知窗口
	CString								m_strMessage;					//消息字符

	//控件变量
protected:
	CSkinButton							m_btCancel;						//取消按钮

	//函数定义
public:
	//构造函数
	CDlgStatus();
	//析构函数
	virtual ~CDlgStatus();

	//重载函数
protected:
	//控件绑定
	virtual void DoDataExchange(CDataExchange * pDX);
	//确定函数
	virtual void OnOK() { return; };
	//取消函数
	virtual void OnCancel() { return; };

	//功能函数
public:
	//设置消息
	bool ShowStatusMessage(LPCTSTR pszMessage, CWnd * pNotifyWnd);
	//销毁窗口
	bool DestroyStatusWnd(CWnd * pNotifyWnd);

	//消息映射
protected:
	//重画消息
	afx_msg void OnPaint();
	//取消按钮
	afx_msg void OnBnClickedClose();
	//定时器消息
	afx_msg void OnTimer(UINT nIDEvent);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif