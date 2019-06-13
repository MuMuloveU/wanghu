#include "StdAfx.h"
#include "DrawSiceWnd2.h"

//////////////////////////////////////////////////////////////////////////
#define IDI_SICE_POS				203									//移动定时器
//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDrawSiceWnd2, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CDrawSiceWnd2::CDrawSiceWnd2()
{
	//加载资源
	m_bSicePos=0;
	m_nXPos=0;
	m_nYPos=0;
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_ImageBack.SetLoadInfo(IDB_SICE_BACK,hInstance);
	
	return;
}

//析构函数
CDrawSiceWnd2::~CDrawSiceWnd2()
{
}

//重画函数
void CDrawSiceWnd2::OnPaint()
{
	CPaintDC dc(this);

	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//创建缓冲
	CDC BufferDC;
	CBitmap BufferImage;
	BufferDC.CreateCompatibleDC(&dc);
	BufferImage.CreateCompatibleBitmap(&dc,rcClient.Width(),rcClient.Height());
	BufferDC.SelectObject(&BufferImage);
	//绘画背景
	CImageHandle BackImageHandle(&m_ImageBack);
	CImageHandle SiceImageFirstHandle(&m_ImageDrawSiceFirst);
	CImageHandle SiceImageSecondHandle(&m_ImageDrawSiceSecond);
	int nImageWidth=m_ImageBack.GetWidth();
	int nImageHeight=m_ImageBack.GetHeight();
	for (int nXPos=0;nXPos<rcClient.right;nXPos+=nImageWidth)
	{
		for (int nYPos=0;nYPos<rcClient.bottom;nYPos+=nImageHeight)
		{
			m_ImageBack.BitBlt(BufferDC,nXPos,nYPos);
		}
	}

	if(m_bSicePos<9)
	{
		m_ImageDrawSiceFirst.AlphaDrawImage(&BufferDC,30,0,m_ImageDrawSiceFirst.GetWidth()/9,m_ImageDrawSiceFirst.GetHeight(),
			m_bSicePos*m_ImageDrawSiceFirst.GetWidth()/9,0,RGB(0,0,254));
		m_ImageDrawSiceSecond.AlphaDrawImage(&BufferDC,30,0,m_ImageDrawSiceSecond.GetWidth()/9,m_ImageDrawSiceSecond.GetHeight(),
			m_bSicePos*m_ImageDrawSiceSecond.GetWidth()/9,0,RGB(0,0,254));
	}

	//绘画界面
	
	dc.BitBlt(0,0,rcClient.Width(),rcClient.Height(),&BufferDC,0,0,SRCCOPY);
	
	//清理资源
	BufferDC.DeleteDC();
	BufferImage.DeleteObject();

	return;
}

//建立消息
int CDrawSiceWnd2::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;
	CImageHandle BackImageHandle(&m_ImageBack);
	SetWindowPos(NULL,0,0,m_ImageBack.GetWidth(),m_ImageBack.GetHeight(),SWP_NOZORDER|SWP_NOREPOSITION);
	return 0;
}
//定时器消息
void CDrawSiceWnd2::OnTimer(UINT nIDEvent)
{
	if(IDI_SICE_POS==nIDEvent)
	{
		
		if(m_bSicePos>=8)
		{
			KillTimer(IDI_SICE_POS);
			m_bSicePos=0;
			Sleep(500);
			AfxGetMainWnd()->PostMessage(IDM_GAME_START2,NULL,NULL);
		}
		else
		{
			m_bSicePos++;
		}
		Invalidate();
	}
	
	__super::OnTimer(nIDEvent);
}
void CDrawSiceWnd2::SetSiceInfo(CDC *dc,BYTE bMoveTime,BYTE SiceFirst,BYTE SiceSecond)
{
	HINSTANCE hInstance=AfxGetInstanceHandle();
	ParentDC=dc;
	m_ImageBack.SetLoadInfo(IDB_SICE_BACK,hInstance);
	m_ImageDrawSiceFirst.SetLoadInfo(IDB_DRAW_SICE1+SiceFirst-1,hInstance);
	m_ImageDrawSiceSecond.SetLoadInfo(IDB_DRAW_SICE7+SiceSecond-1,hInstance);
	m_bSicePos=0;
	SetTimer(IDI_SICE_POS,bMoveTime,NULL);
	

}
//基准位置
void CDrawSiceWnd2::SetBenchmarkPos(int nXPos, int nYPos)
{
	m_nXPos=nXPos;
	m_nYPos=nYPos;
}

//////////////////////////////////////////////////////////////////////////
