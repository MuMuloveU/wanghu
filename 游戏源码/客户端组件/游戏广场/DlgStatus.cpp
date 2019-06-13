#include "Stdafx.h"
#include "Resource.h"
#include "DlgStatus.h"
#include "GlobalUnits.h"

//定时器 ID
#define IDT_STATUS_IMANGE					1					//状态图标
#define STATUS_IMAGE_COUNT					8					//状态数目

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgStatus, CDialog)
	ON_BN_CLICKED(IDC_CLOSE, OnBnClickedClose)
	ON_WM_PAINT()
	ON_WM_TIMER()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgStatus::CDlgStatus() : CDialog(IDD_STATUS)
{
	m_wImagePos=0;
	m_pNotifyWnd=NULL;
	return;
}

//析构函数
CDlgStatus::~CDlgStatus()
{
}

//控件绑定
void CDlgStatus::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CLOSE, m_btCancel);
}

//重画消息
void CDlgStatus::OnPaint()
{
	CPaintDC dc(this);

	//获取位置
	CRect rcClientRect;
	GetClientRect(&rcClientRect);

	//绘画背景
	CImage ImageBuffer;
	ImageBuffer.Create(rcClientRect.Width(),rcClientRect.Height(),16);
	CDC * pDCBuffer=CDC::FromHandle(ImageBuffer.GetDC());
	pDCBuffer->FillSolidRect(0,0,rcClientRect.Width(),rcClientRect.Height(),CSkinDialog::m_SkinAttribute.m_crBackGround);

	//绘画状态
	CImage ImageStatus;
	ImageStatus.LoadFromResource(AfxGetInstanceHandle(),IDB_CONNECT_STATUS);
	CDC * pDCImage=CDC::FromHandle(ImageStatus.GetDC());
	AlphaDrawImage(pDCBuffer,10,10,32,32,pDCImage,m_wImagePos*32,0,32,32,RGB(255,0,255));

	//绘画消息
	CFont DrawFont;
	DrawFont.CreateFont(-12,0,0,0,400,0,0,0,134,3,2,1,2,TEXT("宋体"));
	CFont * pOldFont=pDCBuffer->SelectObject(&DrawFont);
	pDCBuffer->SetTextColor(RGB(10,10,10));
	pDCBuffer->SetBkMode(TRANSPARENT);
	pDCBuffer->TextOut(60,20,m_strMessage);
	pDCBuffer->SelectObject(pOldFont);
	DrawFont.DeleteObject();

	//绘画界面
	ImageBuffer.BitBlt(dc,0,0);

	//清理资源
	ImageStatus.ReleaseDC();
	ImageBuffer.ReleaseDC();
	ImageStatus.Destroy();
	ImageBuffer.Destroy();

	return;
}

//定时器消息
void CDlgStatus::OnTimer(UINT nIDEvent)
{
	if (nIDEvent==IDT_STATUS_IMANGE)
	{
		CRect rcImageRect;
		rcImageRect.SetRect(10,10,42,42);
		m_wImagePos=(m_wImagePos+1)%STATUS_IMAGE_COUNT;
		InvalidateRect(&rcImageRect,FALSE);
		return;
	}
	__super::OnTimer(nIDEvent);
}

//设置消息
bool CDlgStatus::ShowStatusMessage(LPCTSTR pszMessage, CWnd * pNotifyWnd)
{
	//创建窗口
	if (m_hWnd==NULL)
	{
		Create(IDD_STATUS);
		SetTimer(IDT_STATUS_IMANGE,200,NULL);
	}

	//设置消息
	m_pNotifyWnd=pNotifyWnd;
	m_strMessage=pszMessage;
	Invalidate(FALSE);

	//显示窗口
	ShowWindow(SW_SHOW);
	SetForegroundWindow();

	return true;
}

//销毁窗口
bool CDlgStatus::DestroyStatusWnd(CWnd * pNotifyWnd)
{
	if ((m_pNotifyWnd==pNotifyWnd)||(pNotifyWnd==NULL))
	{
		DestroyWindow();
		return true;
	}
	return false;
}

//取消按钮
void CDlgStatus::OnBnClickedClose()
{
	if (m_pNotifyWnd==NULL) return;
	if (IsWindow(m_pNotifyWnd->m_hWnd)==FALSE) return;
	m_pNotifyWnd->PostMessage(WM_COMMAND,IDM_CANCEL_CONNECT,0);
	return;
}

//////////////////////////////////////////////////////////////////////////
