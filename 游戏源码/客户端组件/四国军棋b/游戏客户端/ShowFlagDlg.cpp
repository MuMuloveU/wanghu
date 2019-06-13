// ShowFlagDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ShowFlagDlg.h"
#include "ChessBorad.h"
#include ".\showflagdlg.h"

// CShowFlagDlg dialog

IMPLEMENT_DYNAMIC(CShowFlagDlg, CDialog)
CShowFlagDlg::CShowFlagDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CShowFlagDlg::IDD, pParent)
{
}

CShowFlagDlg::CShowFlagDlg(CChessBorad *pParent): CDialog(CShowFlagDlg::IDD , NULL)
{
	m_pParent = pParent ;
	//加载资源
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_ImageFlag.SetLoadInfo(IDB_FLAG , hInstance) ;

}
CShowFlagDlg::~CShowFlagDlg()
{
}

void CShowFlagDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}
//坐标转换
void CShowFlagDlg::SwitchMousePos(const CPoint &point , BYTE & bXPos, BYTE & bYPos)
{
	bXPos = (BYTE)(point.x / FLAG_WIDTH) ;
	bYPos = (BYTE)(point.y / FLAG_HEIGTH) ;

	if(point.x > (bXPos+1)*FLAG_WIDTH)  ++bXPos ;
	if(point.y > (bYPos+1)*FLAG_HEIGTH) ++bYPos ;

	ASSERT(bXPos>=0 && bXPos<=5 && bYPos>=0 && bYPos<=5) ;
	if(bXPos<0 || bXPos>=5 || bYPos<0 || bYPos>=5) 
	{
		bXPos = 255 ;
		bYPos = 255 ;
		return ;
	}
}


BEGIN_MESSAGE_MAP(CShowFlagDlg, CDialog)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CShowFlagDlg message handlers

void CShowFlagDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	CDialog::OnLButtonUp(nFlags, point);
	CPoint RealPoint = point ;
	RealPoint.x = RealPoint.x - 5 ;
	RealPoint.y = RealPoint.y + 8 ;
	BYTE bXPos , bYPos ;
	SwitchMousePos(RealPoint , bXPos , bYPos) ;

	if(255==bXPos || 255==bYPos) return ;
	if(0==bYPos) return ;

	if(255!=bXPos && 255!=bYPos)
	{
		//清除图案
		if(4==bXPos && 4==bYPos)
			m_pParent->SetFlagImage(bXPos , bYPos , false) ;
		//设置图案
		else
			m_pParent->SetFlagImage(bXPos , bYPos , true) ;
	}    
	ShowWindow(SW_HIDE) ;
}

void CShowFlagDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	CDialog::OnMouseMove(nFlags, point);
	CWnd::OnMouseMove(nFlags, point);

	CPoint RealPoint = point ;
	RealPoint.x = RealPoint.x - 5 ;
	RealPoint.y = RealPoint.y + 8 ;

	BYTE bXPos , bYPos ;
	SwitchMousePos(RealPoint , bXPos , bYPos) ;
	if(255==bXPos || 255==bYPos) return ;
	if(0==bYPos) return ;

	static CRect PreRect ;

	CRect rect(bXPos*FLAG_WIDTH+5 , bYPos*FLAG_HEIGTH-8 , (bXPos+1)*FLAG_WIDTH+5 , (bYPos+1)*FLAG_HEIGTH-8) ;

	//擦除边框
	if(PreRect!=rect) InvalidateRect(&PreRect , TRUE) ;

	PreRect = rect ;

	//重画边框
	CDC *pDC = GetDC() ;
	pDC->Draw3dRect(&rect , RGB(225,0,0),RGB(225,0,0)) ;
	ReleaseDC(pDC) ;

}

void CShowFlagDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	//标志图案
	CImageHandle FlagHandle(&m_ImageFlag) ;
	m_ImageFlag.BitBlt(dc.GetSafeHdc() , 0 , 0 , m_ImageFlag.GetWidth() , m_ImageFlag.GetHeight() , 0 , 0) ;

}
