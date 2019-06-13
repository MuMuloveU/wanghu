#if !defined(AFX_HOVERBUTTON_H__16C6D980_BD45_11D3_BDA3_00104B133581__INCLUDED_)
#define AFX_HOVERBUTTON_H__16C6D980_BD45_11D3_BDA3_00104B133581__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HoverButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Orginally based on CHoverButton by Niek Albers
// A cool CBitmapButton derived class with 3 states (Up/Down/Hover).
//
// Following Added by Frederick Ackers:
// Movable\Resizeable Button code
// ToolTip support for Resizable\Moving Windows
// Horizontal or Vertical Image layout now possible
// Automatic Image Stretching to fit Window Size
// Draws Regular Button with ToolTip if no bitmap is loaded
// ToolTips now display OnHover even if parent window is not active
//
// Added by Bob Carboni:
// 4th state for disabled bitmap
// Region-shaped button
// Transparent button areas for region button
// Anti-aliased edges on transparent button

class CHoverButtonEx : public CBitmapButton
{
	DECLARE_DYNAMIC(CHoverButtonEx);

	// Construction
public:
	CHoverButtonEx();
	void SetToolTipText(CString spText, BOOL bActivate = TRUE);
	void SetToolTipText(UINT nId, BOOL bActivate = TRUE);
	void SetHorizontal(bool ImagesAreLaidOutHorizontally = FALSE);
	BOOL IsMoveable() { return m_bAllowMove; }
	void SetMoveable(BOOL moveAble = TRUE) { m_bAllowMove = moveAble; }
	void DeleteToolTip();

// Attributes
protected:
	void ActivateTooltip(BOOL bActivate = TRUE);
	BOOL m_bHover;						// indicates if mouse is over the button
	CSize m_ButtonSize;					// width and height of the button
	CBitmap mybitmap;
	CPoint m_point;
	CRectTracker *m_track;
	BOOL m_bTracking;
	BOOL m_bHorizontal;
	BOOL m_bAllowMove;

	BOOL  m_bRegionSet;
	CRgn  m_WinRgn;
	BOOL  m_bLoadBkgrnd;

	unsigned int*	m_pBkGrnd;
	unsigned int*	m_pRaw;
	long			m_nRawWidth;
	long			m_nRawHeight;

	void DrawTransparentBitmap (CDC* pDC,
                        int xStart, int yStart,
                        int wWidth, int wHeight,
                        CDC* pTmpDC,
                        int xSource = 0,
                        int ySource = 0);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHoverButtonEx)
	protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL LoadBitmap(UINT bitmapid);
	BOOL LoadBitmap(LPCTSTR lpszResourceName);
	BOOL LoadBitmapFromFile(LPCTSTR lpszResourceName);
	virtual ~CHoverButtonEx();

	void RefreshBkgrnd()		{ m_bLoadBkgrnd = FALSE; };
	BOOL LoadRaw(UINT rawid, long nWidth, long nHeight);
	void SetRegion(HRGN hRgn);

	// Generated message map functions
protected:
	CString m_tooltext;
	CToolTipCtrl* m_ToolTip;
	void InitToolTip();
	//{{AFX_MSG(CHoverButtonEx)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnMouseHover(WPARAM wparam, LPARAM lparam) ;
	afx_msg void OnRButtonDown( UINT nFlags, CPoint point );
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HOVERBUTTON_H__16C6D980_BD45_11D3_BDA3_00104B133581__INCLUDED_)
