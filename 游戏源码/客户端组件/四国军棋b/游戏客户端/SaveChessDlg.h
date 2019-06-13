#pragma once
#include "Resource.h"

// CSaveChessDlg 对话框

class CSaveChessDlg : public CDialog
{

public:
	CSaveChessDlg(CWnd * pParent=NULL);   // 标准构造函数
	virtual ~CSaveChessDlg();

	// 对话框数据
	enum { IDD = IDD_SAVE_CHESS_DLG };
	CButton				m_Ok;
	CButton				m_Cancel;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	short m_iClose;
};
