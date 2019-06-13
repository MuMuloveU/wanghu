#include "Stdafx.h"
#include "Resource.h"
#include "DlgUserRequest.h"

//////////////////////////////////////////////////////////////////////////

//时间标识
#define IDI_REQUEST_TIME			10									//时间标识

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgUserRequest, CDialog)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgUserRequest::CDlgUserRequest() : CDialog(IDD_USER_REQUEST)
{
	//设置变量
	m_cbRequestType=0;
	m_nResidualTime=30L;

	//加载资源
	m_ImageViewBack.SetLoadInfo(IDB_REQUEST_VIEW,AfxGetInstanceHandle());

	return;
}

//析构函数
CDlgUserRequest::~CDlgUserRequest()
{
}

//控件绑定
VOID CDlgUserRequest::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btOk);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
}

//消息解释
BOOL CDlgUserRequest::PreTranslateMessage(MSG * pMsg)
{
	//按键过虑
	if (pMsg->message==WM_KEYDOWN)
	{
		if ((pMsg->wParam==VK_ESCAPE)||(pMsg->wParam==VK_RETURN)) return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//初始化函数
BOOL CDlgUserRequest::OnInitDialog()
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
	m_btOk.SetButtonImage(IDB_REQUEST_BT_OK,hResInstance,false);
	m_btCancel.SetButtonImage(IDB_REQUEST_BT_CANCEL,hResInstance,false);

	//设置时间
	SetTimer(IDI_REQUEST_TIME,1000,NULL);

	return FALSE;
}

//确定消息
VOID CDlgUserRequest::OnOK()
{
	//销毁窗口
	DestroyWindow();

	//查询接口
	//CGlobalUnits * pGlobalUnits=CGlobalUnits::GetGlobalUnits();
	//IGameFrameEngine * pIGameFrameEngine=(IGameFrameEngine *)pGlobalUnits->QueryGlobalModule(MODULE_GAME_FRAME_ENGINE,IID_IGameFrameEngine,VER_IGameFrameEngine);

	//发送消息
	//ASSERT(pIGameFrameEngine!=NULL);
	/*if (pIGameFrameEngine!=NULL) */AfxGetMainWnd()->SendMessage(IDM_USER_REQUEST_RESPOND,m_cbRequestType,TRUE);

	return;
}

//确定消息
VOID CDlgUserRequest::OnCancel()
{
	//销毁窗口
	DestroyWindow();

	//查询接口
	//CGlobalUnits * pGlobalUnits=CGlobalUnits::GetGlobalUnits();
	//IGameFrameEngine * pIGameFrameEngine=(IGameFrameEngine *)pGlobalUnits->QueryGlobalModule(MODULE_GAME_FRAME_ENGINE,IID_IGameFrameEngine,VER_IGameFrameEngine);

	//发送消息
	//ASSERT(pIGameFrameEngine!=NULL);
	/*if (pIGameFrameEngine!=NULL) */AfxGetMainWnd()->SendMessage(IDM_USER_REQUEST_RESPOND,m_cbRequestType,FALSE);
	
	return;
}

//重画函数
VOID CDlgUserRequest::OnPaint()
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

	//和棋请求
	if (m_cbRequestType==REQUEST_PEACE)
	{
		//加载资源
		CSkinImage RequestStringImage;
		RequestStringImage.LoadFromResource(AfxGetInstanceHandle(),IDB_REQUEST_PEACE_STRING);

		//绘画信息
		INT nImageWidth=RequestStringImage.GetWidth();
		INT nImageHeight=RequestStringImage.GetHeight();
		RequestStringImage.BitBlt(DCBuffer,(rcClient.Width()-nImageWidth)/2,88);
	}

	//悔棋请求
	if (m_cbRequestType==REQUEST_REGRET)
	{
		//加载资源
		CSkinImage RequestStringImage;
		RequestStringImage.LoadFromResource(AfxGetInstanceHandle(),IDB_REQUEST_REGRET_STRING);

		//绘画信息
		INT nImageWidth=RequestStringImage.GetWidth();
		INT nImageHeight=RequestStringImage.GetHeight();
		RequestStringImage.BitBlt(DCBuffer,(rcClient.Width()-nImageWidth)/2,88);
	}

	//剩余时间
	TCHAR szTimeLeave[32]=TEXT("");
	_sntprintf(szTimeLeave,CountChar(szTimeLeave),TEXT("此对话框在 %d 秒后自动关闭"),m_nResidualTime);
	DCBuffer.TextOut(rcClient.Width()/2,rcClient.bottom-85,szTimeLeave,lstrlen(szTimeLeave));

	//绘画界面
	dc.BitBlt(0,0,rcClient.Width(),rcClient.Height(),&DCBuffer,0,0,SRCCOPY);

	//清理资源
	DCBuffer.DeleteDC();
	ImageBuffer.DeleteObject();

	return;
}

//时间消息
VOID CDlgUserRequest::OnTimer(UINT nIDEvent)
{
	if (nIDEvent==IDI_REQUEST_TIME)
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
BOOL CDlgUserRequest::OnEraseBkgnd(CDC * pDC)
{
	//更新窗口
	Invalidate(FALSE);
	UpdateWindow();

	return TRUE;
}

//鼠标消息
VOID CDlgUserRequest::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags,Point);

	//消息模拟
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(Point.x,Point.y));

	return;
}

//////////////////////////////////////////////////////////////////////////
