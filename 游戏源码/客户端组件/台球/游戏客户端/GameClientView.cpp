#include "StdAfx.h"
#include "GameClientView.h"
#include "resource.h"
#include "define.h"
#include "GameClientDlg.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameView)
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BEGIN,			OnBegin)
	ON_BN_CLICKED(IDC_GIVEUP,			OnGiveUp)
	ON_BN_CLICKED(IDC_REPLACE,			OnReplace)
	ON_BN_CLICKED(IDC_CONCESSION,		OnConcession)
END_MESSAGE_MAP()


extern struct  DrawInfoStruct			g_DrawInfo;
CGameClientView::CGameClientView(void): CGameFrameView(true,24,&m_ThreadDraw),m_ThreadDraw(this)
{
	m_bMouseDown=false;
	g_DrawInfo.pPlayView=this;
	g_DrawInfo.bStation=NO_STATE;
	m_hMouse=::LoadCursor(AfxGetInstanceHandle(),(LPCSTR)IDC_MOUSE);
	if (::GetSystemMetrics(SM_CXSCREEN)>=1024)
	{
		g_DrawInfo.dwTimes=1.0;
	}
	else
	{
		g_DrawInfo.dwTimes=0.75;
	}
	
}
CGameClientView::~CGameClientView(void)
{
}
void	CGameClientView::ResetGameView()

{
	m_ScoreWnd.ShowWindow(SW_HIDE);
}
void	CGameClientView::RectifyGameView(int nWidth, int nHeight)
{
	if(!nWidth || !nHeight)
	{
		return ;
	}
	GetClientRect(&m_ClientRect);
	g_DrawInfo.ClientRect=m_ClientRect;

	//移动信息窗口
	if (m_Station.GetSafeHwnd()) 
		m_Station.MoveWindow(0,0,nWidth,STATION_WND_HIGHT);
	m_Begin.MoveWindow(m_ClientRect.right-320,m_ClientRect.bottom-80,BTN_WIDTH,BTN_HEIGHT);
	m_GiveUp.MoveWindow(m_ClientRect.left+20,m_ClientRect.top+140,BTN_WIDTH_OTHER,BTN_HEIGHT_OTHER);
	m_Replace.MoveWindow(m_ClientRect.left+20,m_ClientRect.top+200,BTN_WIDTH_OTHER,BTN_HEIGHT_OTHER);
	m_Concession.MoveWindow(m_ClientRect.left+20,m_ClientRect.top+260,BTN_WIDTH_OTHER,BTN_HEIGHT_OTHER);
	m_ScoreWnd.MoveWindow(m_ClientRect.left+m_ClientRect.Width()/2-WIDTH_SCORE/2,m_ClientRect.top+m_ClientRect.Height()/2-HEIGHT_SCORE/2-30,WIDTH_SCORE,HEIGHT_SCORE);

}
void	CGameClientView::DrawGameView(CDC * pDC, 
										int nWidth, 
										int nHeight)
{
}
int		CGameClientView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(__super::OnCreate(lpCreateStruct) == -1) 
	{
		return -1;
	}

	//初始化窗口
	HINSTANCE hInstance = ::AfxGetInstanceHandle();
	CRect r(0,0,0,0);
	m_Station.Create(NULL,NULL,WS_CHILD|WS_VISIBLE,r,this,100);
	CSkinButton *pBtn_[] = {
							&m_Begin,
							&m_GiveUp,
							&m_Replace,
							&m_Concession,
							};
	UINT	Bmp_id_[] ={
						IDB_BEGIN,
						IDB_GIVEUP,
						IDB_REPLACE,
						IDB_CONCESSION,
					};

	TCHAR *title[] = {
						_T(""),
						_T(""),
						_T(""),
						_T(""),
						};
	UINT	control_id_[] = {
							IDC_BEGIN,
							IDC_GIVEUP,
							IDC_REPLACE,
							IDC_CONCESSION,
							};
	
	for(int i=0; i<sizeof(pBtn_)/sizeof(pBtn_[0]); i++)
	{
		if(Bmp_id_[i] != 0)
		{
			if(!pBtn_[i]->SetButtonImage(Bmp_id_[i], 
									hInstance,
									false))
			{
				return -1;
			}

			if(!pBtn_[i]->Create(title[i],
								WS_CHILD,
								r,
								this,
								control_id_[i]))
			{
				return -1;
			}
		}
		else
		{
			if(!pBtn_[i]->Create(title[i],
								WS_BORDER|WS_CHILD,
								r,
								this,
								control_id_[i]))
			{
				return -1;
			}
		}
	}
	if(!m_PeaceDlg.Create(IDD_PEACE, this->GetParent()))
	{
		return -1;
	}
	/*if(!m_RuleInfo.Create(IDD_INFO, this->GetParent()))
	{
		return -1;
	}*/
	if(!m_ScoreWnd.Create(NULL, NULL, 
							WS_CHILD, 
							r, 
							this, 
							IDC_SCORE_WND))
	{
		return -1;
	}
	m_PeaceDlg.ShowWindow(SW_HIDE);
	//m_RuleInfo.ShowWindow(SW_HIDE);
	m_Begin.ShowWindow(SW_HIDE);
	m_GiveUp.ShowWindow(SW_SHOW);
	m_Replace.ShowWindow(SW_SHOW);
	m_Concession.ShowWindow(SW_SHOW);
	m_Concession.EnableWindow(FALSE);
	m_GiveUp.EnableWindow(FALSE);
	m_Replace.EnableWindow(FALSE);
	g_DrawInfo.pPlayView=this;

	return 0;
}
//设置光标
BOOL CGameClientView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if ((g_DrawInfo.bStation==ME_HIT_BALL))
	{
		CPoint	MousePoint;
		::GetCursorPos(&MousePoint);
		ScreenToClient(&MousePoint);
		if(MousePoint.x>GetXBase()&&MousePoint.y>GetYBase()&&
			MousePoint.y<GetYBase()+g_DrawInfo.BackBmpInfo.bmHeight-2*DESK_SIDE&&
			MousePoint.x<(GetXBase()+g_DrawInfo.BackBmpInfo.bmWidth-2*DESK_SIDE))
		{
			SetCursor(m_hMouse);
			return TRUE;
		}
	}
	else if (g_DrawInfo.bStation==WATI_SET_BALL)
	{
		SetCursor(NULL);
		POINT Point;
		::GetCursorPos(&Point);
		ScreenToClient(&Point);
		g_DrawInfo.bOkSet=g_DrawInfo.pDesk->CanPlaceBall((double)(Point.x-GetXBase())/g_DrawInfo.dwTimes,
			(double)(Point.y-GetYBase())/g_DrawInfo.dwTimes,true);
	}

	return CWnd::OnSetCursor(pWnd,nHitTest,message);
}

//鼠标起来
void CGameClientView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bMouseDown)
	{
		if(point.x>GetXBase())
		{
			if(point.x<(GetXBase()-(int)(2*DESK_SIDE*g_DrawInfo.dwTimes)+g_DrawInfo.BackBmpInfo.bmWidth))
			{
				if(point.y>GetYBase())
				{
					if(point.y<(GetYBase()-(int)(2*DESK_SIDE*g_DrawInfo.dwTimes)+g_DrawInfo.BackBmpInfo.bmHeight))
					{
						m_bMouseDown=false;
						ReleaseCapture();
						AfxGetMainWnd()->SendMessage(IDM_HIT_BALL,point.x,point.y);
					}
				}
			}
		}
	}

	return;
}

//鼠标按下
void CGameClientView::OnLButtonDown(UINT nFlags, CPoint point)
{
	if ((m_bMouseDown==false)&&(g_DrawInfo.bStation==ME_HIT_BALL))
	{
		if(point.x>GetXBase()&&point.y>GetYBase()&&
			point.y<GetYBase()+g_DrawInfo.BackBmpInfo.bmHeight-2*DESK_SIDE&&
			point.x<(GetXBase()+g_DrawInfo.BackBmpInfo.bmWidth-2*DESK_SIDE))
		{
			m_bMouseDown=true;
			SetCapture();
			m_Station.BeginPower();
		}
	}
	else if (g_DrawInfo.bStation==WATI_SET_BALL)
	{
		AfxGetMainWnd()->SendMessage(IDM_PLACE_BALL,(WPARAM)&point,0);
	}

	return;
}
int CGameClientView::GetXBase()
{
	 return (int)(m_ClientRect.right-g_DrawInfo.BackBmpInfo.bmWidth)/2+(int)(DESK_SIDE*g_DrawInfo.dwTimes); 
}
int CGameClientView::GetYBase()
{
	  return (int)(m_ClientRect.bottom+STATION_WND_HIGHT-g_DrawInfo.BackBmpInfo.bmHeight-10)/2+(int)(DESK_SIDE*g_DrawInfo.dwTimes-15*g_DrawInfo.dwTimes); 
}
void	CGameClientView::OnBegin(void)
{
	CWnd  *pWnd = ::AfxGetMainWnd();
	if(!pWnd)
	{
		return ;
	}
	m_Begin.ShowWindow(SW_HIDE);
	::SendMessage(pWnd->m_hWnd, IDM_BEGIN, 0, 0);
}
void	CGameClientView::OnGiveUp(void)
{	
	CWnd  *pWnd = ::AfxGetMainWnd();
	if(!pWnd)
	{
		return ;
	}
	::SendMessage(pWnd->m_hWnd, IDM_HITGIVEUP, 0, 0);
}
void	CGameClientView::OnReplace(void)
{	
	CWnd  *pWnd = ::AfxGetMainWnd();
	if(!pWnd)
	{
		return ;
	}
	::SendMessage(pWnd->m_hWnd, IDM_HITREPLACE, 0, 0);
}
void	CGameClientView::OnConcession(void)
{	
	CWnd  *pWnd = ::AfxGetMainWnd();
	if(!pWnd)
	{
		return ;
	}
	m_Concession.EnableWindow(FALSE);
	::SendMessage(pWnd->m_hWnd, IDM_HITCONCESSION, 0, 0);
}
