#include "Stdafx.h"
#include "Resource.h"
#include "SkinButton.h"

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CSkinButton, CButton)

BEGIN_MESSAGE_MAP(CSkinButton, CButton)
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CRgnButton, CButton)

BEGIN_MESSAGE_MAP(CRgnButton, CButton)
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//静态变量
CSkinButtonAttribute				CSkinButton::m_SkinAttribute;			//按钮属性

//////////////////////////////////////////////////////////////////////////

//构造函数
CSkinButtonAttribute::CSkinButtonAttribute() 
{
	//初始化变量
	m_crTextColor=RGB(0,0,0);

	return;
}

//析构函数
CSkinButtonAttribute::~CSkinButtonAttribute() 
{
}

//加载配置
bool CSkinButtonAttribute::UpdateSkinResource(ISkinResource * pISkinResource)
{
	//获取信息
	tagSkinButtonResource SkinButtonResource;
	if (pISkinResource!=NULL) pISkinResource->GetResourceInfo(SkinButtonResource);

	//设置变量
	m_crTextColor=SkinButtonResource.crButtonText;
	m_ImageBack.SetLoadInfo(SkinButtonResource.LoadInfoButton);

	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CSkinButton::CSkinButton()
{
	m_hIcon=NULL;
	m_bExpand=true;
	m_bHovering=false;
	m_bShowTextFrame=false;
	m_pIMouseEvent=NULL;
	m_crTextColor=m_SkinAttribute.m_crTextColor;
	m_crTextFrameColor=DEF_TEXT_FRAME_COLOR;
}

//析构函数
CSkinButton::~CSkinButton()
{
}

//对象附加到现有窗口
void CSkinButton::PreSubclassWindow()
{
	__super::PreSubclassWindow();
	SetButtonStyle(GetButtonStyle()|BS_OWNERDRAW);
	SendMessage(WM_SETFONT,(WPARAM)CSkinResourceManager::GetDefaultFont(),TRUE);
	return;
}

//建立消息
int CSkinButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;
	SetButtonStyle(GetButtonStyle()|BS_OWNERDRAW);
	SendMessage(WM_SETFONT,(WPARAM)CSkinResourceManager::GetDefaultFont(),TRUE);
	return 0;
}

//设置颜色
bool CSkinButton::SetTextColor(COLORREF crTextColor,COLORREF crTextFrameColor,bool bShowFrame)
{
	m_crTextColor=crTextColor;
	m_bShowTextFrame=bShowFrame;
	m_crTextFrameColor=crTextFrameColor;

	if (GetSafeHwnd()) Invalidate(FALSE);
	return true;
}

//设置图标
bool CSkinButton::SetButtonIcon(HICON hIcon)
{
	m_hIcon=hIcon;
	if (GetSafeHwnd()) Invalidate(FALSE);
	return true;
}

//加载位图
bool CSkinButton::SetButtonImage(LPCTSTR pszFileName, bool bExpandImage)
{
	//效验参数
	ASSERT(pszFileName);
	if (pszFileName==NULL) return false;

	//加载位图
	m_bExpand=bExpandImage;
	m_ImageBack.SetLoadInfo(pszFileName);

	//调整位置
	if (m_bExpand==false) FixButtonSize();
	if (GetSafeHwnd()) Invalidate(FALSE);

	return true;
}

//加载位图
bool CSkinButton::SetButtonImage(UINT uBitmapID, HINSTANCE hInstance, bool bExpandImage)
{
	//加载位图
	ASSERT(uBitmapID!=0);
	if (uBitmapID==0) return false;
	m_bExpand=bExpandImage;
	m_ImageBack.SetLoadInfo(uBitmapID,hInstance);

	//调整位置
	if (m_bExpand==false) FixButtonSize();
	if (GetSafeHwnd()) Invalidate(FALSE);

	return true;
}

//获取位图参数
bool CSkinButton::GetButtonLoadInfo(tagImageLoadInfo & LoadInfo)
{
	if (m_ImageBack.IsSetLoadInfo()) return m_ImageBack.GetLoadInfo(LoadInfo);
	return m_SkinAttribute.m_ImageBack.GetLoadInfo(LoadInfo);
}

//默认按钮
void CSkinButton::SetDefaultButton(bool bResetIcon, bool bResetColor, bool bResetImage)
{
	//恢复默认
	if (bResetImage) 
	{
		m_bExpand=true;
		m_ImageBack.RemoveLoadInfo();
	}
	if (bResetIcon)	m_hIcon=NULL;
	if (bResetColor)
	{
		m_bShowTextFrame = false;
		m_crTextColor=m_SkinAttribute.m_crTextColor;
		m_crTextFrameColor = DEF_TEXT_FRAME_COLOR;
	}

	//刷新界面
	if (GetSafeHwnd()) Invalidate(FALSE);

	return;
}

//调整位置
bool CSkinButton::FixButtonSize()
{
	if (m_ImageBack.IsSetLoadInfo()&&GetSafeHwnd())
	{
		CImageHandle ImageHandle(&m_ImageBack);
		SetWindowPos(NULL,0,0,ImageHandle->GetWidth()/5,ImageHandle->GetHeight(),SWP_NOMOVE);
		return true;
	}
	return false;
}

//鼠标移动消息
void CSkinButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bHovering==false)
	{
		//注册消息
		m_bHovering=true;
		Invalidate(FALSE);
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
LRESULT CSkinButton::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	m_bHovering=false;
	Invalidate(FALSE);
	return 0;
}

//背景函数
BOOL CSkinButton::OnEraseBkgnd(CDC * pDC)
{
	Invalidate(FALSE);
	UpdateWindow();
	return TRUE;
}

//界面绘画函数
void CSkinButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	//定义变量
	CRect ClientRect;
	GetClientRect(&ClientRect);
	bool bDisable=((lpDrawItemStruct->itemState&ODS_DISABLED)!=0);
	bool bButtonDown=((lpDrawItemStruct->itemState&ODS_SELECTED)!=0);

	//设置 DC
	HDC hDC=lpDrawItemStruct->hDC;

	//获取文字
	CString strText;
	GetWindowText(strText);

	//加载背景图
	CImageHandle ImageHandle;
	if (m_ImageBack.IsSetLoadInfo()) ImageHandle.AttachResource(&m_ImageBack);
	else ImageHandle.AttachResource(&m_SkinAttribute.m_ImageBack);
	if (ImageHandle.IsResourceValid())
	{
		//计算位图位置
		int iPartWidth=ImageHandle->GetWidth()/5,iDrawPos=0;
		if (bDisable) iDrawPos=iPartWidth*4;
		else if (bButtonDown) iDrawPos=iPartWidth;
		else if (m_bHovering) iDrawPos=iPartWidth*3;

		//绘画背景图
		if (m_bExpand==false) ImageHandle->BitBlt(hDC,0,0,ClientRect.Width(),ClientRect.Height(),iDrawPos,0,SRCCOPY);
		else ImageHandle->StretchBlt(hDC,0,0,ClientRect.Width(),ClientRect.Height(),iDrawPos,0,iPartWidth,ImageHandle->GetHeight(),SRCCOPY);
	}
	else
	{
		//绘画默认界面
		CDC * pDC=CDC::FromHandle(hDC);
		pDC->FillSolidRect(&ClientRect,GetSysColor(COLOR_BTNFACE));
		if (bButtonDown) pDC->Draw3dRect(&ClientRect,GetSysColor(COLOR_WINDOWFRAME),GetSysColor(COLOR_3DHILIGHT));
		else pDC->Draw3dRect(&ClientRect,GetSysColor(COLOR_3DHILIGHT),GetSysColor(COLOR_WINDOWFRAME));
	}

	//绘画图标
	if (bButtonDown) ClientRect.top+=2;
	if (m_hIcon)
	{
		DrawIconEx(hDC,ClientRect.left+6,ClientRect.top+(ClientRect.Height()-16)/2+1,m_hIcon,16,16,0,NULL,DI_NORMAL);
		ClientRect.left+=22;
	}

	//绘画字体
	ClientRect.top+=1;
	::SetBkMode(hDC,TRANSPARENT);
	if(!m_bShowTextFrame)
	{
		if (bDisable) ::SetTextColor(hDC,GetSysColor(COLOR_GRAYTEXT));
		else ::SetTextColor(hDC,m_crTextColor);
		DrawText(hDC,strText,strText.GetLength(),ClientRect,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	}
	//艺术字体
	else
	{
		CDC * pDC=CDC::FromHandle(hDC);
		DrawTextString(pDC,strText,m_crTextColor,m_crTextFrameColor,ClientRect);
	}

	return;
}

//默认回调函数
LRESULT CSkinButton::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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

//艺术字体
void CSkinButton::DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, LPRECT lpRect)
{
	//变量定义
	int nStringLength=lstrlen(pszString);
	int nXExcursion[8]={1,1,1,0,-1,-1,-1,0};
	int nYExcursion[8]={-1,0,1,1,1,0,-1,-1};

	//绘画边框
	pDC->SetTextColor(crFrame);
	CRect rcDraw;
	for (int i=0;i<CountArray(nXExcursion);i++)
	{
		rcDraw.CopyRect(lpRect);
		rcDraw.OffsetRect(nXExcursion[i],nYExcursion[i]);
		pDC->DrawText(pszString,nStringLength,&rcDraw,DT_VCENTER|DT_CENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	}

	//绘画字体
	rcDraw.CopyRect(lpRect);
	pDC->SetTextColor(crText);
	pDC->DrawText(pszString,nStringLength,&rcDraw,DT_VCENTER|DT_CENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

	return;
}

//艺术字体
void CSkinButton::DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos)
{
	//变量定义
	int nStringLength=lstrlen(pszString);
	int nXExcursion[8]={1,1,1,0,-1,-1,-1,0};
	int nYExcursion[8]={-1,0,1,1,1,0,-1,-1};

	//绘画边框
	pDC->SetTextColor(crFrame);
	for (int i=0;i<CountArray(nXExcursion);i++)
	{
		pDC->TextOut(nXPos+nXExcursion[i],nYPos+nYExcursion[i],pszString,nStringLength);
	}

	//绘画字体
	pDC->SetTextColor(crText);
	pDC->TextOut(nXPos,nYPos,pszString,nStringLength);

	return;
}
//////////////////////////////////////////////////////////////////////////

//构造函数
CRgnButton::CRgnButton()
{
	m_bHovering=false;
	m_pIMouseEvent=NULL;
	m_crTrans=RGB(255,0,255);
	m_crTextColor=CSkinButton::m_SkinAttribute.m_crTextColor;
}

//析构函数
CRgnButton::~CRgnButton()
{
}

//对象附加到现有窗口
void CRgnButton::PreSubclassWindow()
{
	__super::PreSubclassWindow();
	SetButtonStyle(GetButtonStyle()|BS_OWNERDRAW);
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);
	SendMessage(WM_SETFONT,(WPARAM)CSkinResourceManager::GetDefaultFont(),TRUE);
	return;
}

//建立消息
int CRgnButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;
	SetButtonStyle(GetButtonStyle()|BS_OWNERDRAW);
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);
	SendMessage(WM_SETFONT,(WPARAM)CSkinResourceManager::GetDefaultFont(),TRUE);
	return 0;
}

//加载位图
bool CRgnButton::LoadRgnImage(HINSTANCE hInstance, UINT uBitmapID, COLORREF crTrans)
{
	m_ImageBack.SetLoadInfo(uBitmapID,hInstance);
	return CreateControlRgn(crTrans);
}

//加载位图
bool CRgnButton::LoadRgnImage(LPCTSTR pszFileName, COLORREF crTrans)
{
	m_ImageBack.SetLoadInfo(pszFileName);
	return CreateControlRgn(crTrans);
}

//创建区域
bool CRgnButton::CreateControlRgn(COLORREF crTrans)
{
	CImageHandle ImageHandle(&m_ImageBack);
	if (ImageHandle.IsResourceValid())
	{
		//创建临时图
		CSkinImage BufferImage;
		int nButtonWidth=m_ImageBack.GetWidth()/5;
		int nButtomHeight=m_ImageBack.GetHeight();
		BufferImage.Create(nButtonWidth,nButtomHeight,32);
		ImageHandle->BitBlt(BufferImage.GetDC(),0,0,nButtonWidth,nButtomHeight,0,0);
		BufferImage.ReleaseDC();

		//创建区域
		CRgn RgnControl;
		BufferImage.CreateImageRegion(RgnControl,crTrans);

		//创建区域
		if (RgnControl.GetSafeHandle()!=NULL)
		{
			//设置变量
			m_crTrans=crTrans;

			//设置按钮
			SetWindowRgn(RgnControl,TRUE);
			SetWindowPos(NULL,0,0,nButtonWidth,nButtomHeight,SWP_NOMOVE|SWP_NOACTIVATE);

			//设置区域
			m_ButtonRgn.DeleteObject();
			m_ButtonRgn.Attach(RgnControl.Detach());

			return true;
		}
	}

	return false;
}

//鼠标移动消息
void CRgnButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bHovering==false)
	{
		//注册消息
		m_bHovering=true;
		Invalidate(FALSE);
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
LRESULT CRgnButton::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	m_bHovering=false;
	Invalidate(FALSE);
	return 0;
}

//界面绘画函数
void CRgnButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	//定义变量
	CRect ClientRect;
	GetClientRect(&ClientRect);
	bool bDisable=((lpDrawItemStruct->itemState&ODS_DISABLED)!=0);
	bool bButtonDown=((lpDrawItemStruct->itemState&ODS_SELECTED)!=0);

	//加载背景图
	CImageHandle ImageHandle(&m_ImageBack);
	if (ImageHandle.IsResourceValid())
	{
		//计算位图位置
		int iPartWidth=ImageHandle->GetWidth()/5,iDrawPos=0;
		if (bDisable) iDrawPos=iPartWidth*4;
		else if (bButtonDown) iDrawPos=iPartWidth;
		else if (m_bHovering) iDrawPos=iPartWidth*3;
		else if (lpDrawItemStruct->itemState&ODS_FOCUS) iDrawPos=iPartWidth*2;

		//绘画背景图
		CDC * pDesDC=CDC::FromHandle(lpDrawItemStruct->hDC);
		m_ImageBack.AlphaDrawImage(pDesDC,0,0,iPartWidth,m_ImageBack.GetHeight(),iDrawPos,0,m_crTrans);
	}

	//获取文字
	CString strText;
	GetWindowText(strText);

	//绘画字体
	ClientRect.top+=1;
	::SetBkMode(lpDrawItemStruct->hDC,TRANSPARENT);
	if (bDisable) ::SetTextColor(lpDrawItemStruct->hDC,GetSysColor(COLOR_GRAYTEXT));
	else ::SetTextColor(lpDrawItemStruct->hDC,m_crTextColor);
	DrawText(lpDrawItemStruct->hDC,strText,strText.GetLength(),ClientRect,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

	return;
}

//默认回调函数
LRESULT CRgnButton::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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

//背景函数
BOOL CRgnButton::OnEraseBkgnd(CDC * pDC)
{
	Invalidate(FALSE);
	UpdateWindow();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
