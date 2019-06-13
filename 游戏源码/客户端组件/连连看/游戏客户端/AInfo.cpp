// AInfo.cpp : 实现文件
//

#include "stdafx.h"
#include "AInfo.h"
#include ".\ainfo.h"


// CAInfo

IMPLEMENT_DYNAMIC(CAInfo, CWnd)
CAInfo::CAInfo()
{
	m_Bg.SetLoadInfo(IDB_PROPINFO,AfxGetInstanceHandle());
}

CAInfo::~CAInfo()
{
}


BEGIN_MESSAGE_MAP(CAInfo, CWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CAInfo 消息处理程序


int CAInfo::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);
	//移动窗口
	CImageHandle ImageHandle(&m_Bg);
	SetWindowPos(NULL,0,0,m_Bg.GetWidth(),m_Bg.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);

	m_Font.CreateFont(-12,0,0,0,400,NULL,NULL,NULL,134,3,2,1,2,TEXT("宋体"));

	return 0;
}

void CAInfo::SetWindowText(LPCTSTR str)
{
	m_Text = str;
	Invalidate(false);
}

void CAInfo::OnPaint()
{
	CPaintDC dc(this);
	dc.SetBkMode(TRANSPARENT);
	dc.SelectObject(&m_Font);
	CImageHandle ImageHandle(&m_Bg);
	m_Bg.BitBlt(dc,0,0);
	switch(m_TextColor)
	{
	case 0:
		dc.SetTextColor(RGB(0,0,0));
		break;
	case 1:
		dc.SetTextColor(RGB(255,255,0));
		break;
	case 2:
		dc.SetTextColor(RGB(255,0,255));
		break;
	default:
		break;
	}
	dc.TextOut(4,3,m_Text);
}

void CAInfo::SetTextColor(int mem)
{
	m_TextColor = mem;
}
