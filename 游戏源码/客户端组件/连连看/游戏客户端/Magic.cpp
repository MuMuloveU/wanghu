// Magic.cpp : 实现文件
//

#include "stdafx.h"
#include "Resource.h"
#include "Magic.h"
#include ".\magic.h"


// CMagic 对话框

IMPLEMENT_DYNAMIC(CMagic, CDialog)
CMagic::CMagic(CWnd* pParent /*=NULL*/)
	: CDialog(CMagic::IDD, pParent)
{
}

CMagic::~CMagic()
{
}

void CMagic::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMagic, CDialog)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


// CMagic 消息处理程序

int CMagic::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	HINSTANCE hInstance = ::AfxGetInstanceHandle();
	m_ImageBall.LoadFromResource(hInstance,IDB_MAGICBALL);
	m_ImageBg.SetLoadInfo(IDB_MAGICBG,AfxGetInstanceHandle());

	CImageHandle ImageHandle(&m_ImageBg);
	SetWindowPos(NULL,0,0,m_ImageBg.GetWidth(),m_ImageBg.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);
	return 0;
}

void CMagic::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	dc.SetBkMode(TRANSPARENT);
	CImageHandle ImageHandle(&m_ImageBg);
	m_ImageBg.BitBlt(dc,0,0);
	for(int i=0; i<7; i++)
	{
		m_ImageBall.BitBlt(
			dc.m_hDC, 
			3+i*50, 3,
			50,
			50,
			50*i,
			0,
			SRCCOPY);
	}
}

void CMagic::OnLButtonDown(UINT nFlags, CPoint point)
{
	int result = 0;
	if(point.x<=0 && point.x>=356)
		return;
	if(point.y<=0 && point.y>=55)
		return;
	result = (point.x-3) / 50+1;

	::SendMessage(this->GetParent()->m_hWnd,ID_MAGIC,result,0);
}

void CMagic::OnMouseMove(UINT nFlags, CPoint point)
{
	CDialog::OnMouseMove(nFlags, point);
}
