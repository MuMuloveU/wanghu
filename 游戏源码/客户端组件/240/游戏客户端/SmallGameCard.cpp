// SmallGameCard.cpp : 实现文件
//

#include "stdafx.h"
#include "SmallGameCard.h"
#include "Resource.h"


BEGIN_MESSAGE_MAP(CSmallGameCard, CWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_SETCURSOR()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()


//构造函数
CSmallGameCard::CSmallGameCard()
{
	m_bCardCount=0;
	m_iCardHight=21;
	m_iCardWidth=15;
	m_iCardHSpace=10;
	m_iCardVSpace=10;
	m_iCardUpSpace=2;
	m_pHandleWnd=NULL;
	m_bHorzLook=true;
	m_bShowCard=false;
	m_bUseMouse=false;
	m_bShowTen=false;
	m_bHotHitCard=255;
	m_PtBasePoint.SetPoint(0,0);
	m_XShowMode=XShowMode::SWX_MODE_CENTER;
	m_YShowMode=YShowMode::SWY_MODE_CENTER;

	HINSTANCE hInstance = ::AfxGetInstanceHandle();

	m_CardPic[0].LoadFromResource(hInstance,IDB_CARD_SMALL);
	m_CardPic[1].LoadFromResource(hInstance,IDB_CARD_SMALL_BK);
}

//析构函数
CSmallGameCard::~CSmallGameCard()
{
}

void CSmallGameCard::SetShowTen(bool bShowTen)
{
	m_bShowTen=bShowTen;
	if(m_bShowTen) 
	{
		m_CardPic[0].Destroy();
		m_CardPic[0].LoadFromResource(AfxGetInstanceHandle(),IDB_CARD_TEN);
		m_iCardHight=m_CardPic[0].GetHeight();
		m_iCardWidth=m_CardPic[0].GetWidth()/6;
		m_iCardHSpace=m_iCardHight/2;
		m_iCardVSpace=m_iCardHight/2;
	}
}

//初始化函数
bool CSmallGameCard::InitGameCard(CWnd * pHandleWnd)
{
	m_pHandleWnd=pHandleWnd;
	return true;
}

//设置扑克参数
void CSmallGameCard::SetCardParameter(int iHSpace, int iVSpace, int iUpSpace)
{
	m_iCardHSpace=iHSpace;
	m_iCardVSpace=iVSpace;
	m_iCardUpSpace=iUpSpace;
	FixCardControl();
	return;
}

//获取大小函数
bool CSmallGameCard::GetCardSize(CSize & Size)
{
	if (m_bCardCount>0)
	{
		if (m_bHorzLook==true)
		{
			Size.cx=m_iCardWidth+(m_bCardCount-1)*m_iCardHSpace;
			Size.cy=m_iCardHight+m_iCardUpSpace;
		}
		else
		{
			Size.cx=m_iCardWidth;
			Size.cy=m_iCardHight+(m_bCardCount-1)*m_iCardVSpace;
		}
	}
	else
	{
		Size.cx=0;
		Size.cy=0;
	}
	return true;
}

//设置基准点函数
void CSmallGameCard::SetBasePoint(CPoint & BasePoint, XShowMode XShowMode, YShowMode YShowMode)
{
	m_XShowMode=XShowMode;
	m_YShowMode=YShowMode;
	m_PtBasePoint=BasePoint;
	FixCardControl();
	return;
}

//设置显示扑克
void CSmallGameCard::SetShowCard(bool bShow)
{
	m_bShowCard=bShow;
	Invalidate(FALSE);
	return;
}

//设置使用鼠标
void CSmallGameCard::SetUseMouse(bool bUserMouse)
{
	m_bUseMouse=bUserMouse;
	return;
}

//设置显示方式
void CSmallGameCard::SetLookMode(bool bHorzLook)
{
	m_bHorzLook=bHorzLook;
	return;
}

//设置扑克
bool CSmallGameCard::SetCard(BYTE bCardList[], BYTE bUpList[], BYTE bCardCount)
{
	if (bCardCount>m_CardArray.GetSize()) m_CardArray.SetSize(bCardCount);
	if (bCardCount>m_CardUpArray.GetSize()) m_CardUpArray.SetSize(bCardCount);
	if (bCardList!=NULL) CopyMemory(m_CardArray.GetData(),bCardList,sizeof(BYTE)*bCardCount);
	if (bUpList!=NULL) CopyMemory(m_CardUpArray.GetData(),bUpList,sizeof(BYTE)*bCardCount);
	else memset(m_CardUpArray.GetData(),0,sizeof(BYTE)*bCardCount);
	m_bCardCount=bCardCount;
	FixCardControl();
	return true;
}

//获取升起的扑克
BYTE CSmallGameCard::GetUpCard(BYTE bCardList[])
{
	BYTE bUpCount=0;
	for (BYTE i=0;i<m_bCardCount;i++)
	{
		if (m_CardUpArray.GetAt(i)==TRUE)
		{
			if (bCardList!=NULL) bCardList[bUpCount]=m_CardArray.GetAt(i);
			bUpCount++;
		}
	}
	return bUpCount;
}

//获取扑克信息
BYTE CSmallGameCard::GetCard(BYTE bCardList[], BYTE bUpList[])
{
	if (bCardList!=NULL) CopyMemory(bCardList,m_CardArray.GetData(),m_bCardCount*sizeof(BYTE));
	if (bUpList!=NULL) CopyMemory(bUpList,m_CardUpArray.GetData(),m_bCardCount*sizeof(BYTE));
	return m_bCardCount;
}

//按键测试
BYTE CSmallGameCard::HitCardTest(CPoint & Point)
{
	if (m_bHorzLook==true)
	{
		//寻找按键扑克
		BYTE bCardIndex=(BYTE)(Point.x/m_iCardHSpace);
		if (bCardIndex>=m_bCardCount) bCardIndex=m_bCardCount-1;

		//判断按键位置
		bool bHitThisCard=true;
		BYTE bCardUp=m_CardUpArray.GetAt(bCardIndex);
		if ((bCardUp==TRUE)&&(Point.y>m_iCardHight)) bHitThisCard=false;
		if ((bCardUp==FALSE)&&(Point.y<m_iCardUpSpace)) bHitThisCard=false;

		//寻找前面的张扑克
		if (bHitThisCard==false)
		{
			BYTE bFindCount=m_iCardWidth/m_iCardHSpace+1;
			for (BYTE i=1;i<bFindCount;i++)
			{
				if (bCardIndex==0) break;
				bCardUp=m_CardUpArray.GetAt(bCardIndex-i);
				if ((bCardUp==FALSE)&&(Point.y>m_iCardWidth)) return bCardIndex-i;
				if ((bCardUp==TRUE)&&(Point.y<20)) return bCardIndex-i;
			}
			return 255;
		}
		return bCardIndex;
	}
	return 255;
}

//删除升起的扑克
BYTE CSmallGameCard::RemoveUpCard()
{
	for (BYTE i=0;i<m_bCardCount;i++)
	{
		if (m_CardUpArray.GetAt(i)==TRUE)
		{
			m_bCardCount--;
			m_CardArray.RemoveAt(i);
			m_CardUpArray.RemoveAt(i);
			i--;
		}
	}
	FixCardControl();
	return m_bCardCount;
}

//调整窗口位置
void CSmallGameCard::FixCardControl()
{
	//获取位置
	CSize Size;
	GetCardSize(Size);

	//调整位置
	CRect CardRect;
	switch (m_XShowMode)
	{
	case SWX_MODE_LEFT: { CardRect.left=m_PtBasePoint.x; break; }
	case SWX_MODE_CENTER: {	CardRect.left=m_PtBasePoint.x-Size.cx/2; break; }
	case SWX_MODE_RIGHT: { CardRect.left=m_PtBasePoint.x-Size.cx; break; }
	}

	switch (m_YShowMode)
	{
	case SWY_MODE_TOP: { CardRect.top=m_PtBasePoint.y; break; }
	case SWY_MODE_CENTER: {	CardRect.top=m_PtBasePoint.y-Size.cy/2;	break; }
	case SWY_MODE_BOTTOM: { CardRect.top=m_PtBasePoint.y-Size.cy; break; }
	}

	//移动位置
	CardRect.right=CardRect.left+Size.cx;
	CardRect.bottom=CardRect.top+Size.cy;
	MoveWindow(&CardRect);
	Invalidate(FALSE);

	//设置区域
	CRgn AllRgn,SignedRgn;
	AllRgn.CreateRectRgn(0,0,0,0);
	for (BYTE i=0;i<m_bCardCount;i++)
	{
		if (m_bHorzLook==true)
		{
			BYTE bUpCard=m_CardUpArray.GetAt(i);
			SignedRgn.CreateRoundRectRgn(i*m_iCardHSpace,(bUpCard==TRUE)?0:m_iCardUpSpace,
				i*m_iCardHSpace+m_iCardWidth+1,(bUpCard==TRUE)?m_iCardHight+1:m_iCardUpSpace+m_iCardHight+1,2,2);
			AllRgn.CombineRgn(&AllRgn,&SignedRgn,RGN_OR);
			SignedRgn.DeleteObject();
		}
		else 
		{
			SignedRgn.CreateRoundRectRgn(0,i*m_iCardVSpace,m_iCardWidth+1,i*m_iCardVSpace+m_iCardHight+1,2,2);
			AllRgn.CombineRgn(&AllRgn,&SignedRgn,RGN_OR);
			SignedRgn.DeleteObject();
		}
	}

	//设置区域
	SetWindowRgn(AllRgn,TRUE);
	m_CardViewRgn.DeleteObject();
	m_CardViewRgn.Attach(AllRgn.Detach());

	return;
}

//建立消息
int CSmallGameCard::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct)==-1) return -1;
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);
	return 0;
}

//重画函数
void CSmallGameCard::OnPaint()
{
	CPaintDC dc(this);
	if (m_bCardCount==0) return;

	//建立缓冲
	//双缓冲画扑克
	CDC BufferDC;
	CSize CardSize;
	CBitmap BufferBmp;
	GetCardSize(CardSize);
	BufferDC.CreateCompatibleDC(&dc);
	BufferBmp.CreateCompatibleBitmap(&dc,CardSize.cx,CardSize.cy);
	CBitmap * pOldBmp=BufferDC.SelectObject(&BufferBmp);

	//绘画扑克
	int iXPos=0,iYPos=0;
	
	CSkinImage * pActiveHandle=NULL;

	for (BYTE i=0;i<m_bCardCount; i++)
	{
		BYTE bCard=m_CardArray.GetAt(i);
		BYTE bUpCard=m_CardUpArray.GetAt(i);

		if ((m_bShowCard==true)&&(m_CardArray.GetAt(i)!=0))
		{
			if ((bCard==0x4E)||(bCard==0x4F))
			{
				iYPos=0;
				iXPos=(bCard-0x4D)*m_iCardWidth;
				pActiveHandle=&m_CardPic[1];
			}
			else
			{
				pActiveHandle=&m_CardPic[0];
				iXPos=GetCardXPos(bCard)*m_iCardWidth;
				iYPos=GetCardYPos(bCard)*m_iCardHight;
			}
			if(m_bShowTen)
			{
				iYPos=0;
				if(bCard<0x40) iXPos=(bCard>>4)*m_iCardWidth;
				else iXPos=(bCard-0x4E+4)*m_iCardWidth;
				pActiveHandle=&m_CardPic[0];
			}

		}
		else
		{
			iYPos=0;
			iXPos=0;
			pActiveHandle=&m_CardPic[1];
		}

		if(m_bHorzLook==true) 
		{
			pActiveHandle->AlphaDrawImage(&BufferDC,
									i*m_iCardHSpace,
									bUpCard?0:m_iCardUpSpace,
									m_iCardWidth,
									m_iCardHight,
									iXPos,
									iYPos,
									RGB(255,0,255));
		}
		else
		{
			pActiveHandle->AlphaDrawImage(&BufferDC,
									0,
									i*m_iCardVSpace,
									m_iCardWidth,
									m_iCardHight,
									iXPos,
									iYPos,
									RGB(255,0,255));
		}
	}

	//刷新界面
	dc.BitBlt(0,0,CardSize.cx,CardSize.cy,&BufferDC,0,0,SRCCOPY);

	//清理资源
	BufferDC.SelectObject(pOldBmp);
	BufferDC.DeleteDC();
	BufferBmp.DeleteObject();

	return;
}

//鼠标消息
void CSmallGameCard::OnLButtonDown(UINT nFlags, CPoint Point)
{
	if ((m_bHorzLook==true)&&(m_bUseMouse==true)) 
	{
		m_bHotHitCard=HitCardTest(Point);
	}
	return;
}

//鼠标消息
void CSmallGameCard::OnLButtonUp(UINT nFlags, CPoint Point)
{
	if ((m_bUseMouse==true)&&(m_bHotHitCard!=255))
	{
		BYTE bHitCard=HitCardTest(Point);
		if ((bHitCard==255)||(m_bHotHitCard!=bHitCard))
		{
			m_bHotHitCard=255;
			return;
		}
		BYTE bUpCard=m_CardUpArray.GetAt(bHitCard);
		m_CardUpArray.SetAt(bHitCard,(bUpCard==TRUE)?FALSE:TRUE);
		FixCardControl();

		//重画失效部分
		CRect Rect;
		Rect.SetRect(bHitCard*m_iCardHSpace,0,bHitCard*m_iCardHSpace+m_iCardWidth,m_iCardHight+m_iCardUpSpace);
		InvalidateRect(Rect,FALSE);
		//if (m_pHandleWnd!=NULL) m_pHandleWnd->PostMessage(IDM_LEFT_HITCARD);
//		AfxGetMainWnd()->SendMessage(IDM_LEFT_HITCARD);
	}
	if(!m_bUseMouse)
	{
		AfxGetMainWnd()->SendMessage(IDM_REVERT_CARD);
	}
	return;
}

//鼠标消息
void CSmallGameCard::OnRButtonUp(UINT nFlags, CPoint point)
{
	if (m_bUseMouse==true)
	{
	//	AfxGetMainWnd()->SendMessage(IDM_RIGHT_HITCARD);
	}
	return;
}

//光标消息
BOOL CSmallGameCard::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT message)
{
	if (m_bUseMouse==true)
	{
		HINSTANCE hInstance = ::AfxGetInstanceHandle();

		/*
		HCURSOR hCursor = ::LoadCursor(hInstance, MAKEINTRESOURCE(IDC_HAND));
		if(hCursor)
		{
			::SetCursor(hCursor);
		}
		*/

		return TRUE;
	}
	return __super::OnSetCursor(pWnd,nHitTest,message);
}

