#include "Stdafx.h"
#include "Resource.h"
#include "SkinHyperLink.h"

//////////////////////////////////////////////////////////////////////////

//提示 ID
#define TOOLTIP_ID					100

//静态变量
CSkinHyperLinkAttribute				CSkinHyperLink::m_SkinAttribute;				//超连接属性

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CSkinHyperLink, CStatic)

BEGIN_MESSAGE_MAP(CSkinHyperLink, CStatic)
	ON_WM_PAINT()
	ON_WM_CREATE()
    ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
	ON_MESSAGE(WM_SETTEXT,OnSetTextMesage)
	ON_CONTROL_REFLECT(STN_CLICKED, OnStnClicked)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CSkinHyperLinkAttribute::CSkinHyperLinkAttribute() 
{
	//设置变量
	m_crBackColor=RGB(0,0,0);
	m_crHoverText=RGB(0,0,0);
	m_crNormalText=RGB(0,0,0);
	m_crVisitedText=RGB(0,0,0);
}

//析构函数
CSkinHyperLinkAttribute::~CSkinHyperLinkAttribute()
{
}

//加载配置
bool CSkinHyperLinkAttribute::UpdateSkinResource(ISkinResource * pISkinResource)
{
	//获取信息
	tagSkinHyperLinkResource SkinHyperLinkResource;
	if (pISkinResource!=NULL) pISkinResource->GetResourceInfo(SkinHyperLinkResource);

	//设置变量
	m_crBackColor=SkinHyperLinkResource.crBackColor;
	m_crHoverText=SkinHyperLinkResource.crHoverText;
	m_crNormalText=SkinHyperLinkResource.crNormalText;
	m_crVisitedText=SkinHyperLinkResource.crVisitedText;

	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CSkinHyperLink::CSkinHyperLink()
{
	m_crBackColor=0;
	m_bVisited=false;
	m_bHovering=false;
	m_bUnderline=true;
	m_bAutoAdjust=true;
	m_pIMouseEvent=NULL;
}

//析构函数
CSkinHyperLink::~CSkinHyperLink()
{
}

//设置鼠标回调接口
void CSkinHyperLink::SetMouseEventSink(IUnknownEx * pIUnknownEx)
{
	ASSERT(pIUnknownEx!=NULL);
	m_pIMouseEvent=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IMouseEvent);
	ASSERT(m_pIMouseEvent!=NULL);
	return;
}

//获取鼠标回调接口
void * CSkinHyperLink::GetMouseEventSink(const IID & Guid, DWORD dwQueryVer) 
{ 
	if (m_pIMouseEvent==NULL) return NULL;
	return m_pIMouseEvent->QueryInterface(Guid,dwQueryVer);
}

//消息解释
BOOL CSkinHyperLink::PreTranslateMessage(MSG * pMsg)
{
    m_ToolTip.RelayEvent(pMsg);
	return CStatic::PreTranslateMessage(pMsg);
}

//控件子类化
void CSkinHyperLink::PreSubclassWindow()
{
	__super::PreSubclassWindow();

	//初始化
	InitHyperLink();

	return;
}

//建立消息
int CSkinHyperLink::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//初始化
	InitHyperLink();

	return 0;
}

//绘画消息
void CSkinHyperLink::OnPaint()
{
	CPaintDC dc(this);

	//获取位置
	CRect ClientRect;
	GetClientRect(&ClientRect);

	//绘画背景
	dc.FillSolidRect(&ClientRect,m_crBackColor);

	//设置 DC
	dc.SetBkMode(TRANSPARENT);
	if (m_bHovering) dc.SetTextColor(m_SkinAttribute.m_crHoverText);
	else if (m_bVisited) dc.SetTextColor(m_SkinAttribute.m_crVisitedText);
	else dc.SetTextColor(m_SkinAttribute.m_crNormalText);

	//绘画字体
	CString strText;
	GetWindowText(strText);
	LONG dwStyte=GetWindowLong(m_hWnd,GWL_STYLE);
	UINT uFormat=DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS;
	if (dwStyte&SS_RIGHT) uFormat|=DT_RIGHT;
	else if (dwStyte&SS_CENTER) uFormat|=DT_CENTER;
	else uFormat|=DT_LEFT;
	dc.SelectObject(CSkinResourceManager::m_DefaultFont);
	dc.DrawText(strText,&ClientRect,uFormat);

	return;
}

//光标消息
BOOL CSkinHyperLink::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT message)
{
	HCURSOR hCursor=LoadCursor(GetModuleHandle(SKIN_CONTROL_DLL_NAME),MAKEINTRESOURCE(IDC_HAND_CUR));
	::SetCursor(hCursor);
	return TRUE;
}

//单击控件
void CSkinHyperLink::OnStnClicked()
{
	//判断状态
	if ((m_pIMouseEvent!=NULL)||(m_strURL.IsEmpty())) return;

	//打开网页
	ShellExecute(NULL,TEXT("open"),m_strURL,NULL,NULL,SW_SHOW);

	//重画控件
	if (m_bVisited==false)
	{
		m_bVisited=true;
		Invalidate();
	}

	return;
}

//鼠标移动消息
void CSkinHyperLink::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bHovering==false)
	{
		//注册消息
		m_bHovering=true;
		Invalidate();
		TRACKMOUSEEVENT TrackMouseEvent;
		TrackMouseEvent.cbSize=sizeof(TrackMouseEvent);
		TrackMouseEvent.dwFlags=TME_LEAVE;
		TrackMouseEvent.hwndTrack=GetSafeHwnd();
		TrackMouseEvent.dwHoverTime=HOVER_DEFAULT;
		_TrackMouseEvent(&TrackMouseEvent);
	}

	__super::OnMouseMove(nFlags, point);
}

//鼠标离开消息
LRESULT CSkinHyperLink::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	m_bHovering=false;
	Invalidate();
	return 0;
}

//标题消息
LRESULT	CSkinHyperLink::OnSetTextMesage(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult=Default();
	if (m_bAutoAdjust) AdjustHyperLink();
	return lResult;
}

//设置访问
void CSkinHyperLink::SetVisited(bool bVisited)
{
	if (m_bVisited!=bVisited)
	{
		m_bVisited=bVisited;
		if (GetSafeHwnd()) Invalidate();
	}
	return;
}

//设置下划线
void CSkinHyperLink::SetUnderline(bool bUnderline)
{
	if (m_bUnderline!=bUnderline)
	{
		m_bUnderline=bUnderline;
		if (GetSafeHwnd()) Invalidate();
	}
	return;
}

//设置 URL
void CSkinHyperLink::SetHyperLinkUrl(LPCTSTR pszUrl)
{
	//设置变量
	m_strURL=pszUrl;

	//建立提示
	UpdateToolTip();

	return;
}

//背景颜色
void CSkinHyperLink::SetBackGroundColor(COLORREF crColor)
{
	m_crBackColor=crColor;
	if (m_hWnd!=NULL) Invalidate(FALSE);
	return;
}

//设置调整
void CSkinHyperLink::SetAutoAdjust(bool bAutoAdjust)
{
	if (m_bAutoAdjust!=bAutoAdjust)
	{
		m_bAutoAdjust=bAutoAdjust;
		if (m_bAutoAdjust) AdjustHyperLink();
	}
	return;
}

//初始化
void CSkinHyperLink::InitHyperLink()
{
	SetWindowLong(m_hWnd,GWL_STYLE,GetStyle()|SS_NOTIFY);
	if (m_bAutoAdjust) AdjustHyperLink();
	m_crBackColor=m_SkinAttribute.m_crBackColor;
	UpdateToolTip();
	return;
}

//更新提示
void CSkinHyperLink::UpdateToolTip()
{
	if (GetSafeHwnd())
	{
		if (m_ToolTip.GetSafeHwnd()==NULL) m_ToolTip.Create(this);
		if (m_strURL.IsEmpty()==false)
		{
			CRect ClientRect;
			GetClientRect(&ClientRect);
			m_ToolTip.Activate(TRUE);
			m_ToolTip.AddTool(this,m_strURL,&ClientRect,TOOLTIP_ID);
		}
		else m_ToolTip.Activate(FALSE);
	}
	return;
}

//调整位置
void CSkinHyperLink::AdjustHyperLink()
{
	//获取标题
	CString strText;
	GetWindowText(strText);

	//获取位置
	CRect ControlRect;
	GetWindowRect(&ControlRect);
	GetParent()->ScreenToClient(&ControlRect);

	//计算长度
	CDC * pDC=GetDC();
	pDC->SelectObject(CSkinResourceManager::m_DefaultFont);
	CSize Size=pDC->GetTextExtent(strText);
	ReleaseDC(pDC);

	//移动位置
	MoveWindow(ControlRect.left,ControlRect.top,Size.cx,Size.cy);

	return;
}

//默认回调函数
LRESULT CSkinHyperLink::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (m_pIMouseEvent)
	{
		switch (message)
		{
		case WM_MOUSELEAVE:
			{
				UINT uButtonID=GetWindowLong(GetSafeHwnd(),GWL_ID);
				m_pIMouseEvent->OnEventMouseLeft(uButtonID,wParam,lParam);
				break;
			}
		case WM_MOUSEMOVE:
			{
				UINT uButtonID=GetWindowLong(GetSafeHwnd(),GWL_ID);
				m_pIMouseEvent->OnEventMouseMove(uButtonID,wParam,lParam);
				break;
			}
		case WM_LBUTTONDOWN:
			{
				UINT uButtonID=GetWindowLong(GetSafeHwnd(),GWL_ID);
				m_pIMouseEvent->OnEventLButtonDown(uButtonID,wParam,lParam);
				break;
			}
		case WM_LBUTTONUP:
			{
				UINT uButtonID=GetWindowLong(GetSafeHwnd(),GWL_ID);
				m_pIMouseEvent->OnEventLButtonUp(uButtonID,wParam,lParam);
				break;
			}
		case WM_RBUTTONDOWN:
			{
				UINT uButtonID=GetWindowLong(GetSafeHwnd(),GWL_ID);
				m_pIMouseEvent->OnEventRButtonDown(uButtonID,wParam,lParam);
				break;
			}
		case WM_RBUTTONUP:
			{
				UINT uButtonID=GetWindowLong(GetSafeHwnd(),GWL_ID);
				m_pIMouseEvent->OnEventRButtonUp(uButtonID,wParam,lParam);
				break;
			}
		}
	}
	return __super::DefWindowProc(message, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////////

