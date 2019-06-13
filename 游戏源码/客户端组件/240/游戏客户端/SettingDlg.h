//***********************************************
//File		: SettingDlg.h 
//Time		: 2005-05-09
//Author	: 
//Comment	: 
//***********************************************


#pragma once

#include "Resource.h"

class CSettingDlg : public CSkinDialogEx
{
	DECLARE_DYNAMIC(CSettingDlg)

public:
	CSettingDlg(CWnd* pParent = NULL);   
	virtual ~CSettingDlg();

	enum { IDD = IDD_SETTINGDLG };
	
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    
	virtual void OnOK();
	virtual void OnCancel();

	DECLARE_MESSAGE_MAP()

public:
	bool		m_IsSound;
	bool		m_bShunShiZhen;
	bool		m_bIsCanLook;
	bool		m_bIsCanChoose;
	

	CSkinButton		m_btOk;
	CSkinButton		m_btCancel;
};


//-----------------------------------------------
//					the end
//-----------------------------------------------
