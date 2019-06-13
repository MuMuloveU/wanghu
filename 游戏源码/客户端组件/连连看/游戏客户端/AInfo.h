#pragma once


// CAInfo

class CAInfo : public CWnd
{
	DECLARE_DYNAMIC(CAInfo)

public:
	CAInfo();
	virtual ~CAInfo();

protected:
	CString							m_Text;
	int								m_TextColor;

	CFont							m_Font;
	CSkinImage						m_Bg;
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void SetWindowText(LPCTSTR str);
	afx_msg void OnPaint();
	void SetTextColor(int mem);
};


