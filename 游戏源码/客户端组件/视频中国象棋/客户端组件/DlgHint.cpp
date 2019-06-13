#include "Stdafx.h"
#include "Resource.h"
#include "DlgHint.h"

//////////////////////////////////////////////////////////////////////////

//时间标识
#define IDI_HINT_TIME			10									//时间标识

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgHint, CDialog)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgHint::CDlgHint() : CDialog(IDD_USER_HINT)
{
	//设置变量
	m_nResidualTime=9L;

	//加载资源
	m_ImageViewBack.SetLoadInfo(IDB_HINT_VIEW,AfxGetInstanceHandle());

	return;
}

//析构函数
CDlgHint::~CDlgHint()
{
}

//控件绑定
VOID CDlgHint::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btOk);
}

//消息解释
BOOL CDlgHint::PreTranslateMessage(MSG * pMsg)
{
	//按键过虑
	if (pMsg->message==WM_KEYDOWN)
	{
		if ((pMsg->wParam==VK_ESCAPE)||(pMsg->wParam==VK_RETURN)||(pMsg->wParam==VK_SPACE))
		{
			return TRUE;
		}
	}

	return __super::PreTranslateMessage(pMsg);
}

//初始化函数
BOOL CDlgHint::OnInitDialog()
{
	__super::OnInitDialog();

	//创建区域
	CRgn RgnImage;
	CImageHandle HandleViewBack(&m_ImageViewBack);
	m_ImageViewBack.CreateImageRegion(RgnImage,RGB(255,0,255));

	//设置窗口
	SetWindowRgn(RgnImage,TRUE);
	SetWindowPos(NULL,0,0,m_ImageViewBack.GetWidth(),m_ImageViewBack.GetHeight(),SWP_NOZORDER|SWP_NOMOVE);

	//设置控件
	HINSTANCE hResInstance=AfxGetInstanceHandle();
	m_btOk.SetButtonImage(IDB_BT_HINT_OK,hResInstance,false);

	//设置时间
	SetTimer(IDI_HINT_TIME,1000,NULL);

	return FALSE;
}

//确定消息
VOID CDlgHint::OnOK()
{
	//销毁窗口
	DestroyWindow();

	return;
}

//重画函数
VOID CDlgHint::OnPaint()
{
	CPaintDC dc(this);

	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//创建缓冲
	CDC DCBuffer;
	CBitmap ImageBuffer;
	DCBuffer.CreateCompatibleDC(&dc);
	ImageBuffer.CreateCompatibleBitmap(&dc,rcClient.Width(),rcClient.Height());

	//设置 DC
	DCBuffer.SetBkMode(TRANSPARENT);
	DCBuffer.SelectObject(&ImageBuffer);
	DCBuffer.SetTextColor(RGB(125,125,125));
	DCBuffer.SetTextAlign(TA_CENTER|TA_TOP);
	DCBuffer.SelectObject(CSkinResourceManager::GetDefaultFont());

	//绘画背景
	CImageHandle HandleViewBack(&m_ImageViewBack);
	m_ImageViewBack.BitBlt(DCBuffer,0,0);

	//剩余时间
	TCHAR szTimeLeave[32]=TEXT("");
	_sntprintf(szTimeLeave,CountChar(szTimeLeave),TEXT("此对话框在 %d 秒后自动关闭"),m_nResidualTime);
	DCBuffer.TextOut(rcClient.Width()/2,rcClient.bottom-75,szTimeLeave,lstrlen(szTimeLeave));

	//绘画界面
	dc.BitBlt(0,0,rcClient.Width(),rcClient.Height(),&DCBuffer,0,0,SRCCOPY);

	//清理资源
	DCBuffer.DeleteDC();
	ImageBuffer.DeleteObject();

	return;
}

//时间消息
VOID CDlgHint::OnTimer(UINT nIDEvent)
{
	if (nIDEvent==IDI_HINT_TIME)
	{
		//关闭判断
		if (m_nResidualTime>0)
		{
			//设置变量
			m_nResidualTime--;

			//更新界面
			Invalidate(FALSE);
		}
		else PostMessage(WM_CLOSE,0,0);

		return;
	}

	__super::OnTimer(nIDEvent);
}

//绘画背景
BOOL CDlgHint::OnEraseBkgnd(CDC * pDC)
{
	//更新窗口
	Invalidate(FALSE);
	UpdateWindow();

	return TRUE;
}

//鼠标消息
VOID CDlgHint::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags,Point);

	//消息模拟
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(Point.x,Point.y));

	return;
}

//设置时间
VOID CDlgHint::InitTimerCount()
{
	m_nResidualTime=9L;

	return;
}

//////////////////////////////////////////////////////////////////////////
