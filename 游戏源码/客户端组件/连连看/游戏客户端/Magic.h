#pragma once

#define					ID_MAGIC						3335

// CMagic 对话框

class CMagic : public CDialog
{
	DECLARE_DYNAMIC(CMagic)

public:
	CMagic(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMagic();

// 对话框数据
	enum { IDD = IDD_MAGIC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	CSkinImage m_ImageBall;
	CSkinImage m_ImageBg;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
