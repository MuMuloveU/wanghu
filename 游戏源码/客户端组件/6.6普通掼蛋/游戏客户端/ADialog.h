#pragma once
#include "Resource.h"

// CADialog 对话框

class CADialog : public CSkinDialogEx
{
	DECLARE_DYNAMIC(CADialog)

public:
	CADialog();   // 标准构造函数
	virtual ~CADialog();

// 对话框数据
	enum { IDD = IDD_ANSWER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_bCause;
	CString m_title;
	CSkinButton						m_btOK;								//确定按钮
	CSkinButton						m_btCancel;							//取消按钮
	virtual BOOL OnInitDialog();
};
