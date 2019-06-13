#pragma once
#include "Resource.h"

// CAnswerDlg 对话框

class CAnswerDlg : public CSkinDialogEx
{
	DECLARE_DYNAMIC(CAnswerDlg)

public:
	CAnswerDlg();   // 标准构造函数
	virtual ~CAnswerDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_answer;
	CSkinButton						m_btOK;								//确定按钮
	CSkinButton						m_btCancel;							//取消按钮
	virtual BOOL OnInitDialog();
};
