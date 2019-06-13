#include "stdafx.h"
#include "ShowHand.h"
#include ".\showhand.h"

//定时器标识
#define IDI_START				200									//开始定时器
#define IDI_COUNT				201									//开始定时器

#define TIMER_START				5									//开始定时器

IMPLEMENT_DYNAMIC(ShowHand, CDialog)

BEGIN_MESSAGE_MAP(ShowHand, CDialog)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()

//构造函数
ShowHand::ShowHand(): CDialog(IDD_DIALOG1)
{
	//加载资源
	m_ImageBack.SetLoadInfo(IDB_SHOWHAND_COMFIRE,AfxGetInstanceHandle());
	m_bTimes = 0;
}

//析构函数
ShowHand::~ShowHand()
{
}

//控件绑定
void ShowHand::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Control(pDX, IDOK, m_btOK);
}

//初始化函数
BOOL ShowHand::OnInitDialog()
{
	__super::OnInitDialog();	

	//设置界面
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);

	//设置按钮
	HINSTANCE Hinstance = AfxGetInstanceHandle();
	m_btOK.SetButtonImage(IDB_BT_YES1,Hinstance,false);
	m_btCancel.SetButtonImage(IDB_BT_NO1,Hinstance,false);

	//居中窗口
	CenterWindow(GetParent());

	//移动窗口
	CImageHandle HandleBack(&m_ImageBack);
	SetWindowPos(NULL,200,200,m_ImageBack.GetWidth(),m_ImageBack.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);

	//设置定时器
	m_bTimes = TIMER_START;
	SetTimer(IDI_START,TIMER_START*1000,NULL);
	SetTimer(IDI_COUNT,1000,NULL);

	return TRUE;
}

//重画函数
void ShowHand::OnPaint() 
{
	CPaintDC dc(this); 

	//绘画背景
	CImageHandle ImageHandle(&m_ImageBack);
	m_ImageBack.AlphaDrawImage(&dc,0,0,RGB(255,255,255));

	//设置 DC
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(0,0,0));
	dc.SelectObject(CSkinResourceManager::GetDefaultFont());

	//创建字体
	CFont ViewFont;
	ViewFont.CreateFont(-15,0,0,0,400,0,0,0,134,3,2,1,1,TEXT("Arial"));
	CFont *pOldFont=dc.SelectObject(&ViewFont);

	TCHAR tCh[128]=TEXT("");
	_snprintf(tCh,sizeof(tCh),TEXT("%ld"),m_bTimes);

	CRect rcScore(85,10,85+65,10+19);
	dc.DrawText(tCh,lstrlen(tCh),&rcScore,DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS);

	//还原字体
	dc.SelectObject(pOldFont);
	ViewFont.DeleteObject();

	return;
}

//绘画背景
BOOL ShowHand::OnEraseBkgnd(CDC * pDC)
{
	Invalidate(FALSE);
	UpdateWindow();
	return TRUE;
}

//时间消息
void ShowHand::OnTimer(UINT nIDEvent)
{
	if(nIDEvent==IDI_START)
	{
		KillTimer(IDI_START);
		KillTimer(IDI_COUNT);
		PostMessage(WM_CLOSE);
	}
	else if(nIDEvent==IDI_COUNT)
	{
		m_bTimes--;
		Invalidate(FALSE);
		UpdateWindow();
	}

	CDialog::OnTimer(nIDEvent);
}

//鼠标消息
void ShowHand::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags,Point);

	//模拟标题
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(Point.x,Point.y));

	return;
}

void ShowHand::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	OnOK();
}

void ShowHand::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}

///////////////////////////////////////////////////////////////////////////