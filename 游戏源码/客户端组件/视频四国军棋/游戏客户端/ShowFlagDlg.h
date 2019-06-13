#pragma once
#include "Resource.h"

#define FLAG_WIDTH					30									//标志宽度
#define FLAG_HEIGTH					30									//标志高度
// CShowFlagDlg dialog
class CChessBorad ;

class CShowFlagDlg : public CDialog
{
	DECLARE_DYNAMIC(CShowFlagDlg)

public:
	CShowFlagDlg(CWnd* pParent = NULL);   // standard constructor
	CShowFlagDlg(CChessBorad *Parent) ;
	virtual ~CShowFlagDlg();

// Dialog Data
	enum { IDD = IDD_FLAGDLG };
public:
	CSkinImage						m_ImageFlag;						//标志图案

	//功能函数
public:
	//坐标转换
	void SwitchMousePos(const CPoint &point , BYTE & bXPos, BYTE & bYPos) ;

private:
	CChessBorad						*m_pParent ;						//父类指针

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
};
