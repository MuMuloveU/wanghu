#pragma once


#include "Resource.h"


class CPeaceDlg : public CDialog
{
	DECLARE_DYNAMIC(CPeaceDlg)

public:
	void    SetOk();
	CPeaceDlg(CWnd* pParent = NULL);   
	virtual ~CPeaceDlg();

	enum { IDD = IDD_PEACE };
	//bool		m_Mode;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL	OnInitDialog();
	afx_msg void	OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void	OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};


//-----------------------------------------------
//					the end
//-----------------------------------------------
