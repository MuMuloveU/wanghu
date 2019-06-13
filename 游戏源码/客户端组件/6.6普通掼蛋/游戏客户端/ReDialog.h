#pragma once

#include"Resource.h"

#define IDQUIT			100							//强退
// CReDialog 对话框

class CReDialog : public CSkinDialogEx
{
	DECLARE_DYNAMIC(CReDialog)

public:
	CReDialog();   // 标准构造函数
	virtual ~CReDialog();

// 对话框数据
	enum { IDD = IDD_REQUEST };
public:


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_bCause;
	CSkinButton						m_btOK;								//确定按钮
	CSkinButton						m_btCancel;							//取消按钮

	afx_msg void OnClose();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
};
