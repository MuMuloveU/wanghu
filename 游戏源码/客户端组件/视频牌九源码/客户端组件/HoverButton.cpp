// HoverButton.cpp : implementation file
//

#include "stdafx.h"
#include "HoverButton.h"
#include "resource.h"
#include "winDIB.h"

#include <gdiplus.h>
using namespace Gdiplus;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CHoverButtonEx

CHoverButtonEx::CHoverButtonEx()
{
	m_bHover = FALSE;
	m_bTracking = FALSE;
	m_bHorizontal = FALSE;
	m_track = NULL;
	m_ToolTip = NULL;
	m_ButtonSize.cx = 0;
	m_ButtonSize.cy = 0;
	m_bAllowMove = FALSE;

	m_pBkGrnd = NULL;
	m_bLoadBkgrnd = FALSE;
	m_bRegionSet = FALSE;

	m_pRaw = NULL;
	m_nRawWidth = 0;
	m_nRawHeight = 0;
}

CHoverButtonEx::~CHoverButtonEx()
{
	delete m_ToolTip;

	if (m_pBkGrnd)
		delete m_pBkGrnd;
}
IMPLEMENT_DYNAMIC(CHoverButtonEx, CBitmapButton)

BEGIN_MESSAGE_MAP(CHoverButtonEx, CBitmapButton)
	//{{AFX_MSG_MAP(CHoverButtonEx)
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_WM_RBUTTONDOWN()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////
 //	CHoverButtonEx message handlers

BOOL CHoverButtonEx::OnEraseBkgnd(CDC* pDC)
{
	return 1;
}


void CHoverButtonEx::OnMouseMove(UINT nFlags, CPoint point) 
{
	//	TODO: Add your message handler code here and/or call default
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE|TME_HOVER;
		tme.dwHoverTime = 1;
		m_bTracking = _TrackMouseEvent(&tme);
		m_point = point;
	}
	CBitmapButton::OnMouseMove(nFlags, point);
}

BOOL CHoverButtonEx::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (m_ToolTip != NULL)
		if (::IsWindow(m_ToolTip->m_hWnd)) // Incase m_ToolTip isn't NULL, check to see if its a valid window
			m_ToolTip->RelayEvent(pMsg);		
	return CButton::PreTranslateMessage(pMsg);
}

// Set the tooltip with a string resource
void CHoverButtonEx::SetToolTipText(UINT nId, BOOL bActivate)
{
	// load string resource
	m_tooltext.LoadString(nId);
	// If string resource is not empty
	if (m_tooltext.IsEmpty() == FALSE) SetToolTipText(m_tooltext, bActivate);

}

// Set the tooltip with a CString
void CHoverButtonEx::SetToolTipText(CString spText, BOOL bActivate)
{
	// We cannot accept NULL pointer
	if (spText.IsEmpty()) return;

	// Initialize ToolTip
	InitToolTip();
	m_tooltext = spText;

	// If there is no tooltip defined then add it
	if (m_ToolTip->GetToolCount() == 0)
	{
		CRect rectBtn; 
		GetClientRect(rectBtn);
		m_ToolTip->AddTool(this, m_tooltext, rectBtn, 1);
	}

	// Set text for tooltip
	m_ToolTip->UpdateTipText(m_tooltext, this, 1);
	m_ToolTip->SetDelayTime(2000);
	m_ToolTip->Activate(bActivate);
}

void CHoverButtonEx::InitToolTip()
{
	if (m_ToolTip == NULL)
	{
		m_ToolTip = new CToolTipCtrl;
		// Create ToolTip control
		m_ToolTip->Create(this);
		m_ToolTip->Activate(TRUE);
	}
} // End of InitToolTip

// Activate the tooltip
void CHoverButtonEx::ActivateTooltip(BOOL bActivate)
{
	// If there is no tooltip then do nothing
	if (m_ToolTip->GetToolCount() == 0) return;

	// Activate tooltip
	m_ToolTip->Activate(bActivate);
} // End of EnableTooltip



void CHoverButtonEx::SetRegion(HRGN hRgn)
{
	m_WinRgn.CreateRectRgn(0, 0, 1, 1);
	CombineRgn(m_WinRgn, hRgn, 0, RGN_COPY); 
	m_bRegionSet = TRUE; 

	if (m_bRegionSet)
		SetWindowRgn(m_WinRgn, FALSE);
}


void CHoverButtonEx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your code to draw the specified item

	if ((m_ButtonSize.cx == 0) || (m_ButtonSize.cy == 0)) // If our Button size if 0, we have no bitmap so draw Regular...
	{
		UINT style = GetButtonStyle();
		style &= ~BS_OWNERDRAW;
		SetButtonStyle(style); //No Bitmap so remove OwnerDraw and Refresh
		return;
	}

	CDC *mydc=CDC::FromHandle(lpDrawItemStruct->hDC);

	CDC * pMemDC = new CDC;
	pMemDC -> CreateCompatibleDC(mydc);

	// If this is the first time drawing the button, or the background
	// has been refreshed, we need to "read" the background and save
	// it for transparency mixing (aka alpha blending).
	if (m_pRaw && m_nRawWidth && m_nRawHeight && !m_bLoadBkgrnd)
	{
		unsigned int bkPix;
		COLORREF c;

		int bkWidth;
		int bkHeight;

		// Get the size of one image
		if (!m_bHorizontal)
		{
			bkWidth = m_nRawWidth;
			bkHeight = m_nRawHeight/4;
		}
		else
		{
			bkWidth = m_nRawWidth/4;
			bkHeight = m_nRawHeight;
		}

		if (m_pBkGrnd)
			delete m_pBkGrnd;

		// Create array to hold background pixels
		m_pBkGrnd = new unsigned int[bkWidth*bkHeight];

		if (!m_pBkGrnd)
			return;

		unsigned int* pBkGrnd = m_pBkGrnd;

		for (int iY = 0; iY < bkHeight; iY++)
		{
			for (int iX = 0; iX < bkWidth; iX++, pBkGrnd++)
			{
				// Get the background pixel
				c = mydc->GetPixel(iX, iY);
				bkPix = (unsigned int)c;
				bkPix |= 0xff000000;		// Set pixel opaque
				// Set correct order of RGB
				bkPix = (bkPix&0xff00ff00) | ((bkPix>>16)&0xff) | ((bkPix<<16)&0xff0000);

				// Save pixel in array
				*pBkGrnd = bkPix;
			}
		}

		m_bLoadBkgrnd = TRUE;
	}

	CBitmap * pOldBitmap;
	pOldBitmap = pMemDC -> SelectObject(&mybitmap);
	
	CPoint point(0,0);	


	// Depending on wether images are horizontal or vertical, pulls out correct image

	// For now, I only draw the button transparent if a region is set.  This could be
	// updated to always draw transparent
	if(lpDrawItemStruct->itemState & ODS_SELECTED)
	{
		if (!m_bHorizontal)
		{
			if (m_bRegionSet)
				DrawTransparentBitmap(mydc,0,0,m_ButtonSize.cx,m_ButtonSize.cy,pMemDC,0,m_ButtonSize.cy);
			else
				//mydc->BitBlt(0,0,m_ButtonSize.cx,m_ButtonSize.cy,pMemDC,0,m_ButtonSize.cy,SRCCOPY);
				mydc->StretchBlt(0,0, lpDrawItemStruct->rcItem.right-lpDrawItemStruct->rcItem.left,
					lpDrawItemStruct->rcItem.bottom-lpDrawItemStruct->rcItem.top,
					pMemDC, 0, m_ButtonSize.cy, m_ButtonSize.cx,m_ButtonSize.cy, SRCCOPY );
		}
		else
		{
			if (m_bRegionSet)
				DrawTransparentBitmap(mydc,0,0,m_ButtonSize.cx,m_ButtonSize.cy,pMemDC,m_ButtonSize.cx, 0);
			else
				//mydc->BitBlt(0,0,m_ButtonSize.cx,m_ButtonSize.cy,pMemDC,m_ButtonSize.cx,0,SRCCOPY);
				mydc->StretchBlt(0,0, lpDrawItemStruct->rcItem.right-lpDrawItemStruct->rcItem.left,
					lpDrawItemStruct->rcItem.bottom-lpDrawItemStruct->rcItem.top,
					pMemDC,m_ButtonSize.cx,0, m_ButtonSize.cx,m_ButtonSize.cy, SRCCOPY );
		}
	}
	else
	{
		if(m_bHover)
		{
			if (!m_bHorizontal)
			{
				if (m_bRegionSet)
					DrawTransparentBitmap(mydc,0,0,m_ButtonSize.cx,m_ButtonSize.cy,pMemDC,0,m_ButtonSize.cy*2);
				else
					//mydc->BitBlt(0,0,m_ButtonSize.cx,m_ButtonSize.cy,pMemDC,0,m_ButtonSize.cy*2,SRCCOPY);
					mydc->StretchBlt(0,0, lpDrawItemStruct->rcItem.right-lpDrawItemStruct->rcItem.left,
					lpDrawItemStruct->rcItem.bottom-lpDrawItemStruct->rcItem.top,
					pMemDC,0,m_ButtonSize.cy*2, m_ButtonSize.cx,m_ButtonSize.cy, SRCCOPY );
			}
			else
			{
				if (m_bRegionSet)
					DrawTransparentBitmap(mydc,0,0,m_ButtonSize.cx,m_ButtonSize.cy,pMemDC,m_ButtonSize.cx*2,0);
				else
				//mydc->BitBlt(0,0,m_ButtonSize.cx,m_ButtonSize.cy,pMemDC,m_ButtonSize.cx*2,0,SRCCOPY);
					mydc->StretchBlt(0,0, lpDrawItemStruct->rcItem.right-lpDrawItemStruct->rcItem.left,
						lpDrawItemStruct->rcItem.bottom-lpDrawItemStruct->rcItem.top,
						pMemDC,m_ButtonSize.cx*2,0, m_ButtonSize.cx,m_ButtonSize.cy, SRCCOPY );
			}
		}else
		{
			if (IsWindowEnabled())
			{
				if (m_bRegionSet)
					DrawTransparentBitmap(mydc,0,0,m_ButtonSize.cx,m_ButtonSize.cy,pMemDC,0,0);
				else
					//mydc->BitBlt(0,0,m_ButtonSize.cx,m_ButtonSize.cy,pMemDC,0,0,SRCCOPY);
					mydc->StretchBlt(0,0, lpDrawItemStruct->rcItem.right-lpDrawItemStruct->rcItem.left,
						lpDrawItemStruct->rcItem.bottom-lpDrawItemStruct->rcItem.top,
						pMemDC,0,0, m_ButtonSize.cx,m_ButtonSize.cy, SRCCOPY );
			}
			else
			{
				if (!m_bHorizontal)
				{
					if (m_bRegionSet)
						DrawTransparentBitmap(mydc,0,0,m_ButtonSize.cx,m_ButtonSize.cy,pMemDC,0,m_ButtonSize.cy*3);
					else
						//mydc->BitBlt(0,0,m_ButtonSize.cx,m_ButtonSize.cy,pMemDC,0,m_ButtonSize.cy*3,SRCCOPY);
						mydc->StretchBlt(0,0, lpDrawItemStruct->rcItem.right-lpDrawItemStruct->rcItem.left,
						lpDrawItemStruct->rcItem.bottom-lpDrawItemStruct->rcItem.top,
						pMemDC,0,m_ButtonSize.cy*3, m_ButtonSize.cx,m_ButtonSize.cy, SRCCOPY );
				}
				else
				{
					if (m_bRegionSet)
						DrawTransparentBitmap(mydc,0,0,m_ButtonSize.cx,m_ButtonSize.cy,pMemDC,m_ButtonSize.cx*3,0);
					else
						//mydc->BitBlt(0,0,m_ButtonSize.cx,m_ButtonSize.cy,pMemDC,m_ButtonSize.cx*3,0,SRCCOPY);
						mydc->StretchBlt(0,0, lpDrawItemStruct->rcItem.right-lpDrawItemStruct->rcItem.left,
							lpDrawItemStruct->rcItem.bottom-lpDrawItemStruct->rcItem.top,
							pMemDC,m_ButtonSize.cx*3,0, m_ButtonSize.cx,m_ButtonSize.cy, SRCCOPY );
				}
			}
		}	
	}
	// clean up
	pMemDC -> SelectObject(pOldBitmap);
	delete pMemDC;
}

// This routine loads raw bitmap data from a resource.
// The width and height must be defined as there is no bitmap header
//
// Note that the resource type is "RAW" here, but can be changed to 
// any name, or passed as a parameter if desired
BOOL CHoverButtonEx::LoadRaw(UINT rawid, long nWidth, long nHeight)
{
	UINT style = GetButtonStyle();//If Bitmaps are being loaded and the BS_OWNERDRAW is not set
						// then reset style to OwnerDraw.
	if (!(style & BS_OWNERDRAW))
	{
		style |= BS_OWNERDRAW;
		SetButtonStyle(style);
	}

	m_pRaw = NULL;
	CString resName;
	resName.Format("#%d", rawid);
	HGLOBAL hRaw = LoadResource(AfxGetResourceHandle(), 
		FindResource(AfxGetResourceHandle(), resName, "RAW"));
	if (!hRaw)
		return FALSE;

	m_pRaw = (unsigned int*)LockResource(hRaw);

	if (!m_pRaw)
		return FALSE;

	m_nRawWidth = nWidth;
	m_nRawHeight = nHeight;

	// The bitmap should contain four images: normal, selected, hover, and disabled.
	// The images must be the same size within the bitmap, but can be laid out
	// horizontally or vertically.
	if (!m_bHorizontal)
	{
		m_ButtonSize.cy=nHeight/4;
		m_ButtonSize.cx=nWidth;
	}
	else
	{
		m_ButtonSize.cy=nHeight;
		m_ButtonSize.cx=nWidth/4;
	}

	SetWindowPos( NULL, 0,0, m_ButtonSize.cx,m_ButtonSize.cy,SWP_NOMOVE   |SWP_NOOWNERZORDER   );

	return TRUE;
}

// Load a bitmap from the resources in the button, 
// the bitmap has to have 4 buttonsstates next to each other: Up/Down/Hover/Disabled
BOOL CHoverButtonEx::LoadBitmap(UINT bitmapid)
{
	UINT style = GetButtonStyle();//If Bitmaps are being loaded and the BS_OWNERDRAW is not set
						// then reset style to OwnerDraw.
	if (!(style & BS_OWNERDRAW))
	{
		style |= BS_OWNERDRAW;
		SetButtonStyle(style);
	}
	mybitmap.Attach(::LoadImage(::AfxGetInstanceHandle(),MAKEINTRESOURCE(bitmapid), IMAGE_BITMAP,0,0,LR_DEFAULTCOLOR));
	BITMAP	bitmapbits;
	mybitmap.GetBitmap(&bitmapbits);
	/* This checks to see which is bigger, the height or width and divides the 
	opposite by 4 to calculate whether the images are laid out vertical or horizontal*/
	if (!m_bHorizontal)
	{
		m_ButtonSize.cy=bitmapbits.bmHeight/4;
		m_ButtonSize.cx=bitmapbits.bmWidth;
	}
	else
	{
		m_ButtonSize.cy=bitmapbits.bmHeight;
		m_ButtonSize.cx=bitmapbits.bmWidth/4;
	}
	SetWindowPos( NULL, 0,0, m_ButtonSize.cx,m_ButtonSize.cy,SWP_NOMOVE   |SWP_NOOWNERZORDER   );

	return TRUE;
}

// Load a bitmap from the resources in the button, 
// the bitmap has to have 4 buttonsstates next to each other: Up/Down/Hover/Disabled
BOOL CHoverButtonEx::LoadBitmapFromFile(LPCTSTR lpszResourceName)
{
	UINT style = GetButtonStyle();//If Bitmaps are being loaded and the BS_OWNERDRAW is not set
						// then reset style to OwnerDraw.
	if (!(style & BS_OWNERDRAW))
	{
		style |= BS_OWNERDRAW;
		SetButtonStyle(style);
	}
	mybitmap.Attach(::LoadImage(AfxGetInstanceHandle(), 
                         lpszResourceName,
                         IMAGE_BITMAP,
                         0,0,
                         LR_LOADFROMFILE | LR_CREATEDIBSECTION));
	BITMAP	bitmapbits;
	mybitmap.GetBitmap(&bitmapbits);
	/* This checks to see which is bigger, the height or width and divides the 
	opposite by 4 to calculate whether the images are laid out vertical or horizontal*/
	if (!m_bHorizontal)
	{
		m_ButtonSize.cy=bitmapbits.bmHeight/4;
		m_ButtonSize.cx=bitmapbits.bmWidth;
	}
	else
	{
		m_ButtonSize.cy=bitmapbits.bmHeight;
		m_ButtonSize.cx=bitmapbits.bmWidth/4;
	}
	SetWindowPos( NULL, 0,0, m_ButtonSize.cx,m_ButtonSize.cy,SWP_NOMOVE   |SWP_NOOWNERZORDER   );
	return TRUE;
}

// Load a bitmap from the resources in the button, 
// the bitmap has to have 4 buttonsstates next to each other: Up/Down/Hover/Disabled
BOOL CHoverButtonEx::LoadBitmap(LPCTSTR lpszResourceName)
{
	UINT style = GetButtonStyle();//If Bitmaps are being loaded and the BS_OWNERDRAW is not set
						// then reset style to OwnerDraw.
	if (!(style & BS_OWNERDRAW))
	{
		style |= BS_OWNERDRAW;
		SetButtonStyle(style);
	}
	mybitmap.Attach(::LoadImage(AfxGetInstanceHandle(), 
                         lpszResourceName,
                         IMAGE_BITMAP,
                         0,0,
                         LR_DEFAULTCOLOR));
	BITMAP	bitmapbits;
	mybitmap.GetBitmap(&bitmapbits);
	/* This checks to see which is bigger, the height or width and divides the 
	opposite by 4 to calculate whether the images are laid out vertical or horizontal*/
	if (!m_bHorizontal)
	{
		m_ButtonSize.cy=bitmapbits.bmHeight/4;
		m_ButtonSize.cx=bitmapbits.bmWidth;
	}
	else
	{
		m_ButtonSize.cy=bitmapbits.bmHeight;
		m_ButtonSize.cx=bitmapbits.bmWidth/4;
	}
	SetWindowPos( NULL, 0,0, m_ButtonSize.cx,m_ButtonSize.cy,SWP_NOMOVE   |SWP_NOOWNERZORDER   );
	return TRUE;
}

// This is the routine to create anti-aliasing between the button bitmap and the
// background bitmap.  The routine works pixel by pixel based on the formula:
//
//	displayColor = sourceColor ?alpha / 255 + backgroundColor ?(255 ?alpha) / 255
//
// where the alpha is the source alpha.  Return value is the displayed pixel.
static	inline		unsigned int	alphaBlend(const unsigned int bg, const unsigned int src)
{
	// src * alpha + dst * (1-alpha), where alpha is src's alpha, resulting alpha is dst alpha
	unsigned int	a = src >> 24;	// sourceColor alpha

	// If source pixel is transparent, just return the background
	if (0 == a) return bg;

	unsigned int	rb = (((src & 0x00ff00ff) * a) + ((bg & 0x00ff00ff) * (0xff - a))) & 0xff00ff00;
	unsigned int	g  = (((src & 0x0000ff00) * a) + ((bg & 0x0000ff00) * (0xff - a))) & 0x00ff0000;

	return (src & 0xff000000) | ((rb | g) >> 8);
}

// Draws the button bitmap at given coordinates using alpha blending with background
void CHoverButtonEx::DrawTransparentBitmap(CDC* pDC,
                                   int xStart,  int yStart,
                                   int wWidth,  int wHeight,
                                   CDC* pTmpDC,
                                   int xSource, // = 0
                                   int ySource)  // = 0)
{

	unsigned int srcPix;
	unsigned int destPix;
	unsigned int* src;

	// We must have both the src and background bitmaps!
	ASSERT(m_pRaw);
	ASSERT(m_bLoadBkgrnd);

	if (!m_bLoadBkgrnd)
		return;

	// Get width and height and starting point within raw data
	unsigned int nWidth;
	unsigned int nHeight;
	if (!m_bHorizontal)
	{
		nWidth = m_nRawWidth;
		nHeight = m_nRawHeight/4;
		src = m_pRaw + ySource;
	}
	else
	{
		nWidth = m_nRawWidth/4;
		nHeight = m_nRawHeight;
		src = m_pRaw + xSource;
	}

	unsigned int* dispBuffer = new unsigned int[nWidth*nHeight];
	unsigned int* pDst = dispBuffer;
	unsigned int* pBG = m_pBkGrnd;

	if (!dispBuffer)
		return;

	for(unsigned int iRow = 0; iRow < nHeight; iRow++)
	{
		for(unsigned int iColumn = xSource; iColumn < nWidth+xSource; iColumn++, src++, pDst++, pBG++)
		{
			destPix = *pBG;
			srcPix = (*src&0xff00ff00) | ((*src>>16)&0xff) | ((*src<<16)&0xff0000);
			*pDst = alphaBlend(destPix, srcPix);
		}

		if (m_bHorizontal)
			src+=m_nRawWidth-nWidth;
	}

	// I use winDIB class to display, but a MemDC could also be used.  I wanted
	// to include winDIB because of it's many other display features.
	CRect rect(0, 0, nWidth, nHeight);
	winDIB	dib(*pDC);
	dib.frameBuffer((unsigned char *) dispBuffer, 0, nWidth, nHeight, 32);
	dib.srcRect() = rect;
	dib.dstRect() = rect;
	dib.copyToDisplay();

	delete[] dispBuffer;
}

void CHoverButtonEx::SetHorizontal(bool ImagesAreLaidOutHorizontally)
{
	// If true than the images are next to each other horizontally, else they are vertical
	m_bHorizontal = ImagesAreLaidOutHorizontally;
}

LRESULT CHoverButtonEx::OnMouseHover(WPARAM wparam, LPARAM lparam) 
{
	// TODO: Add your message handler code here and/or call default
	m_bHover=TRUE;
	/* This line corrects a problem with the tooltips not displaying when 
	the mouse passes over them, if the parent window has not been clicked yet.
	Normally this isn't an issue, but when developing multi-windowed apps, this 
	bug would appear. Setting the ActiveWindow to the parent is a solution to that.
	*/
	::SetActiveWindow(GetParent()->GetSafeHwnd());
	Invalidate();
	DeleteToolTip();
	// Create a new Tooltip with new Button Size and Location
	SetToolTipText(m_tooltext);
	if (m_ToolTip != NULL)
		if (::IsWindow(m_ToolTip->m_hWnd))
			//Display ToolTip
			m_ToolTip->Update();

	return 0;
}


LRESULT CHoverButtonEx::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	m_bTracking = FALSE;
	m_bHover=FALSE;
	Invalidate();
	return 0;
}

void CHoverButtonEx::OnRButtonDown( UINT nFlags, CPoint point )
{
	if (IsMoveable() == TRUE)
	{
		m_point = point;
		CRect rect;
		GetWindowRect(rect);//Tell the tracker where we are
		ScreenToClient(rect);
		m_track = new CRectTracker(&rect, CRectTracker::dottedLine | 
								CRectTracker::resizeInside |
								CRectTracker::hatchedBorder);
		if (nFlags & MK_CONTROL) // If Ctrl + Right-Click then Resize object
		{
			GetWindowRect(rect);
			GetParent()->ScreenToClient(rect);
			m_track->TrackRubberBand(GetParent(), rect.TopLeft());
			m_track->m_rect.NormalizeRect();
		}
		else // If not Ctrl + Right-Click, then Move Object
		{
			m_track->Track(GetParent(), point);
		}
		rect = LPRECT(m_track->m_rect); 
		MoveWindow(&rect,TRUE);//Move Window to our new position
		m_track = NULL;
		rect = NULL;
	}
	CBitmapButton::OnRButtonDown(nFlags, point);
}

void CHoverButtonEx::DeleteToolTip()
{
	// Destroy Tooltip incase the size of the button has changed.
	if (m_ToolTip != NULL)
	{
		delete m_ToolTip;
		m_ToolTip = NULL;
	}
}
