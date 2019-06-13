#pragma once


// CControlDlg 对话框

class CControlDlg : public CDialog
{
	DECLARE_DYNAMIC(CControlDlg)

	CSkinImage			m_ImageBg;

public:
	CControlDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CControlDlg();

// 对话框数据
	enum { IDD = IDD_CONTROL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
};
