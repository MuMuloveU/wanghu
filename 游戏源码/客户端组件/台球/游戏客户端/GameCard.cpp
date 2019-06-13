//***********************************************
//File		: GameCard.cpp 
//Time		: 2004-06-04
//Author	: 鲁辉
//Comment	: 麻将牌类
//***********************************************
#include "Stdafx.h"
#include "Resource.h"
#include "GameCard.h"
#include "define.h"
#include ".\gamecard.h"


BEGIN_MESSAGE_MAP(CGameCard, CWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_NCMOUSEMOVE()
END_MESSAGE_MAP()


CGameCard::CGameCard()
{
	m_bInCardCount=0;
	m_bEatCardCount=0;
	m_bOutCardCount=0;
	m_bHideCardCount=0;
	m_bTrainCardCount=0;
	m_MouseMove=false;
	m_IsCanDel=false;
	m_HitCard=250;
}

CGameCard::~CGameCard()
{
}

//建立消息
int CGameCard::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct)==-1) return -1;
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);
	return 0;
}

//重画函数
void CGameCard::OnPaint()
{
	if(m_bEatCardCount>0)
		DrawEatCard();
	if(m_bInCardCount>0)
		DrawInCard();
	if(m_bOutCardCount>0)
		DrawOutCard();
	if(m_bHideCardCount>0)
		DrawHideCard();
	if(m_bTrainCardCount>0)
		DrawTrainCard();
	if(m_bCenterCardCount==1)
		DrawCenterCard();
}

//鼠标消息
void CGameCard::OnLButtonUp(UINT nFlags, CPoint Point)
{
	BYTE m_OldHitCard=0;
	CWnd  *pWnd = ::AfxGetMainWnd();
	if(m_bPlayerWhich==SWX_MODE_ME&&m_IsCanDel)
	{
		m_OldHitCard=HitCardTest(Point);
		if(m_OldHitCard!=250)
		{
			if(!pWnd)
			{
				return ;
			}
			m_IsCanDel=false;
			::SendMessage(pWnd->m_hWnd, UM_OUTCARD, 0, m_OldHitCard);
		}
	}
	if(m_bTrainCardCount>0)
	{
		m_OldHitCard=(BYTE)(Point.y/HEIGHT_MEINCARD);
		::SendMessage(pWnd->m_hWnd, UM_EATTRAINCARD, 0, m_OldHitCard);
	}
	return;
}

//鼠标消息
void CGameCard::OnMouseMove(UINT nFlags, CPoint Point)
{
	if(m_bInCardCount>0&&m_bPlayerWhich==SWX_MODE_ME)
	{
		BYTE m_OldHitCard=HitCardTest(Point);
		if(m_OldHitCard==250)
		{
			if(m_HitCard!=250)
			{
				m_MouseMove=false;
				m_HitCard=m_OldHitCard;
				FixInCardControl();
			}
		}
		else
		{
			if(m_HitCard!=m_OldHitCard)
			{
				
				m_MouseMove=true;
				m_HitCard=m_OldHitCard;
				FixInCardControl();
			}
		}
	}
	Invalidate(FALSE);
	return;
}
//设置手中的牌的信息
void CGameCard::SetCardInInfo(BYTE bCardList[],BYTE bCardCount,XPlayerWhich bPlayerWhich,CPoint BasePoint)
{
	if (bCardCount>m_CardInArray.GetSize())
		m_CardInArray.SetSize(bCardCount);
	if (bCardList!=NULL) 
		CopyMemory(m_CardInArray.GetData(),bCardList,sizeof(BYTE)*bCardCount);
	m_PtInBasePoint=BasePoint;
	m_bInCardCount=bCardCount;
	m_bPlayerWhich=bPlayerWhich;
	FixInCardControl();
	Invalidate(FALSE);
}
//设置吃，碰的牌的信息
void CGameCard::SetCardEatInfo(BYTE bCardList[],BYTE bCardCount,XPlayerWhich bPlayerWhich,CPoint BasePoint)
{ 
	if (bCardCount>m_CardEatArray.GetSize())
		m_CardEatArray.SetSize(bCardCount);
	if (bCardList!=NULL) 
		CopyMemory(m_CardEatArray.GetData(),bCardList,sizeof(BYTE)*bCardCount);
	m_Logic.SortCard(m_CardEatArray.GetData(),sizeof(BYTE)*bCardCount);
	m_PtEatBasePoint=BasePoint;
	m_bEatCardCount=bCardCount;
	m_bPlayerWhich=bPlayerWhich;
	FixEatCardControl();
	Invalidate(FALSE);
}
//设置打出的牌的信息
void CGameCard::SetCardOutInfo(BYTE bCardList[],BYTE bCardCount,XPlayerWhich bPlayerWhich,CPoint BasePoint)
{
	if (bCardCount>m_CardOutArray.GetSize())
		m_CardOutArray.SetSize(bCardCount);
	if (bCardList!=NULL) 
		CopyMemory(m_CardOutArray.GetData(),bCardList,sizeof(BYTE)*bCardCount);
	m_PtOutBasePoint=BasePoint;
	m_bOutCardCount=bCardCount;
	m_bPlayerWhich=bPlayerWhich;
	FixOutCardControl();
	Invalidate(FALSE);
}
//设置未揭的牌的信息
void CGameCard::SetCardHideInfo(BYTE bCardCount,CPoint BasePoint)
{
	m_PtHideBasePoint=BasePoint;
	if(bCardCount>CARDHIDE_NUM)
	{
		if(bCardCount%2==0)
			m_bHideCardCount=CARDHIDE_NUM;
		else
			m_bHideCardCount=CARDHIDE_NUM-1;
	}
	else
		m_bHideCardCount=bCardCount;
	FixHideCardControl();
	Invalidate(FALSE);
}
//设置吃的牌的信息
void CGameCard::SetCardCenterInfo(BYTE bCardList[],BYTE bCardCount,CPoint BasePoint)
{
	CRect CardRect;
	m_bCenterCardCount=bCardCount;
	if(bCardCount==1)
	{
		CardRect.left=BasePoint.x;
		CardRect.top=BasePoint.y;
		CardRect.right=CardRect.left+WIDTH_MEINCARD;
		CardRect.bottom=CardRect.top+HEIGHT_MEINCARD;
		m_CenterCard=bCardList[0];
	}
	else
	{
		CardRect.right=CardRect.left;
		CardRect.bottom=CardRect.top;
	}
	MoveWindow(&CardRect);
	Invalidate(FALSE);
}
//设置刚出的在中间牌的信息
void CGameCard::SetCardTrainInfo(BYTE bCardList[],BYTE bCardCount,CPoint BasePoint)
{
	memset(m_TrainCard,0,TRAINCARD_NUM*TRAINCARD_NUM);
	m_PtTrainBasePoint=BasePoint;
	m_bTrainCardCount=bCardCount;
	memset(m_TrainCard,0,bCardCount*TRAINCARD_NUM);
	for(int i=0;i<bCardCount;i++)
	{
		CopyMemory(&m_TrainCard[i*TRAINCARD_NUM],&bCardList[i*TRAINCARD_NUM],TRAINCARD_NUM);
		m_Logic.SortCard(&m_TrainCard[i*TRAINCARD_NUM],sizeof(BYTE)*TRAINCARD_NUM);
	}
	FixTrainCardControl();
	Invalidate(FALSE);
}
void CGameCard::DrawInCard()
{
	CSize CardSize;
	switch(m_bPlayerWhich)
	{
	case SWX_MODE_ME:
		CardSize.cx=WIDTH_MEINCARD*m_bInCardCount;		
		CardSize.cy=HEIGHT_MEINCARD+CARDUP_HEIGHT;
		break;
	case SWX_MODE_LEFT:
		CardSize.cx=WIDTH_LEFTINCARD;		
		CardSize.cy=HEIGHT_LEFTINCARD_P*(m_bInCardCount-1)+HEIGHT_LEFTINCARD_A;
		break;
	case SWX_MODE_FRONT:
		CardSize.cx=WIDTH_FRONTINCARD*m_bInCardCount;		
		CardSize.cy=HEIGHT_FRONTINCARD;
		break;
	case SWX_MODE_RIGHT:
		CardSize.cx=WIDTH_RIGHTINCARD;		
		CardSize.cy=HEIGHT_RIGHTINCARD_P*(m_bInCardCount-1)+HEIGHT_RIGHTINCARD_A;
		break;
	}
	//建立缓冲
	CPaintDC dc(this);
	CDC BufferDC;
	CBitmap BufferBmp;
	BufferDC.CreateCompatibleDC(&dc);
	BufferBmp.CreateCompatibleBitmap(&dc,CardSize.cx,CardSize.cy);
	CBitmap * pOldBmp=BufferDC.SelectObject(&BufferBmp);

	//绘制麻将
	int iXPos=0;
	CSkinImage * pActiveHandle=NULL;
	pActiveHandle=&m_CardPic[0];
	CImageHandle HandleCrad(pActiveHandle);
	for (BYTE i=0; i<m_bInCardCount; i++)
	{
		BYTE bCard=0;
		switch(m_bPlayerWhich)
		{
		case SWX_MODE_ME:
			bCard=m_CardInArray.GetAt(i);
			iXPos=GetCardXPos(bCard)*WIDTH_MEINCARD;
			if(m_HitCard==i)
			{
				pActiveHandle->BitBlt(BufferDC.m_hDC,
								i*WIDTH_MEINCARD,
								m_MouseMove?0:CARDUP_HEIGHT,
								WIDTH_MEINCARD,
								HEIGHT_MEINCARD,
								iXPos,
								0);
			}
			else
			{
				pActiveHandle->BitBlt(BufferDC.m_hDC,
								i*WIDTH_MEINCARD,
								CARDUP_HEIGHT,
								WIDTH_MEINCARD,
								HEIGHT_MEINCARD,
								iXPos,
								0);
			}
			break;
		case SWX_MODE_LEFT:
			pActiveHandle->BitBlt(BufferDC.m_hDC,
							0,
							i*HEIGHT_LEFTINCARD_P,
							WIDTH_LEFTINCARD,
							HEIGHT_LEFTINCARD_A,
							0,
							0);
			break;
		case SWX_MODE_FRONT:
			pActiveHandle->BitBlt(BufferDC.m_hDC,
								(m_bInCardCount-i-1)*WIDTH_FRONTINCARD,
								0,
								WIDTH_FRONTINCARD,
								HEIGHT_FRONTINCARD,
								0,
								0);
			break;
		case SWX_MODE_RIGHT:
			pActiveHandle->BitBlt(BufferDC.m_hDC,
								0,
								i*HEIGHT_RIGHTINCARD_P,
								WIDTH_RIGHTINCARD,
								HEIGHT_RIGHTINCARD_A,
								0,
								0);
			break;
		}
		
	}
	//刷新界面
	dc.BitBlt(0,0,CardSize.cx,CardSize.cy,&BufferDC,0,0,SRCCOPY);
	//清理资源
	BufferDC.SelectObject(pOldBmp);
	BufferDC.DeleteDC();
	BufferBmp.DeleteObject();
}
void CGameCard::DrawCenterCard()
{
	CSize CardSize;
	CardSize.cx=WIDTH_MEINCARD;		
	CardSize.cy=HEIGHT_MEINCARD;
	//建立缓冲
	CPaintDC dc(this);
	CDC BufferDC;
	CBitmap BufferBmp;
	BufferDC.CreateCompatibleDC(&dc);
	BufferBmp.CreateCompatibleBitmap(&dc,CardSize.cx,CardSize.cy);
	CBitmap * pOldBmp=BufferDC.SelectObject(&BufferBmp);
	//绘制麻将
	int iXPos=0;
	CSkinImage * pActiveHandle=NULL;
	pActiveHandle=&m_CardPic[0];
	CImageHandle HandleCrad(pActiveHandle);
	iXPos=GetCardXPos(m_CenterCard)*WIDTH_MEINCARD;
	pActiveHandle->BitBlt(BufferDC.m_hDC,
								0,
								0,
								WIDTH_MEINCARD,
								HEIGHT_MEINCARD,
								iXPos,
								0);

	//刷新界面
	dc.BitBlt(0,0,CardSize.cx,CardSize.cy,&BufferDC,0,0,SRCCOPY);
	//清理资源
	BufferDC.SelectObject(pOldBmp);
	BufferDC.DeleteDC();
	BufferBmp.DeleteObject();
}
void CGameCard::DrawEatCard()
{
	CSize CardSize;
	switch(m_bPlayerWhich)
	{
	case SWX_MODE_ME:
		CardSize.cx=WIDTH_MEEATCARD*m_bEatCardCount;		
		CardSize.cy=HEIGHT_MEEATCARD;
		break;
	case SWX_MODE_LEFT:
		CardSize.cx=WIDTH_LEFTEATCARD;		
		CardSize.cy=HEIGHT_LEFTEATCARD_P*(m_bEatCardCount-1)+HEIGHT_LEFTEATCARD_A;
		break;
	case SWX_MODE_FRONT:
		CardSize.cx=WIDTH_FRONTEATCARD*m_bEatCardCount;		
		CardSize.cy=HEIGHT_FRONTEATCARD;
		break;
	case SWX_MODE_RIGHT:
		CardSize.cx=WIDTH_RIGHTEATCARD;		
		CardSize.cy=HEIGHT_RIGHTEATCARD_P*(m_bEatCardCount-1)+HEIGHT_RIGHTEATCARD_A;
		break;
	}
	//建立缓冲
	CPaintDC dc(this);
	CDC BufferDC;
	CBitmap BufferBmp;
	BufferDC.CreateCompatibleDC(&dc);
	BufferBmp.CreateCompatibleBitmap(&dc,CardSize.cx,CardSize.cy);
	CBitmap * pOldBmp=BufferDC.SelectObject(&BufferBmp);
	//绘制麻将

	int iXPos=0;
	CSkinImage * pActiveHandle=NULL;
	pActiveHandle=&m_CardPic[1];
	CImageHandle HandleCrad(pActiveHandle);
	for (BYTE i=0; i<m_bEatCardCount; i++)
	{
		BYTE bCard=m_CardEatArray.GetAt(i);
		switch(m_bPlayerWhich)
		{
		case SWX_MODE_ME:
			iXPos=GetCardXPos(bCard)*WIDTH_MEEATCARD;
			pActiveHandle->BitBlt(BufferDC.m_hDC,
								i*WIDTH_MEEATCARD,
								0,
								WIDTH_MEEATCARD,
								HEIGHT_MEEATCARD,
								iXPos,
								0);
			break;
		case SWX_MODE_LEFT:
			iXPos=GetCardXPos(bCard)*WIDTH_LEFTEATCARD;
			pActiveHandle->BitBlt(BufferDC.m_hDC,
								0,
								i*HEIGHT_LEFTEATCARD_P,
								WIDTH_LEFTEATCARD,
								HEIGHT_LEFTEATCARD_A,
								iXPos,
								0);
			break;
		case SWX_MODE_FRONT:
			iXPos=GetCardXPos(bCard)*WIDTH_FRONTEATCARD;
		
			pActiveHandle->BitBlt(BufferDC.m_hDC,
								(m_bEatCardCount-i-1)*WIDTH_FRONTEATCARD,
								0,
								WIDTH_FRONTEATCARD,
								HEIGHT_FRONTEATCARD,
								iXPos,
								0);
			break;
		case SWX_MODE_RIGHT:
			iXPos=GetCardXPos(bCard)*WIDTH_RIGHTEATCARD;
		
			pActiveHandle->BitBlt(BufferDC.m_hDC,
								0,
								(m_bEatCardCount-i-1)*HEIGHT_RIGHTEATCARD_P,
								WIDTH_RIGHTEATCARD,
								(i==0?HEIGHT_RIGHTEATCARD_A:HEIGHT_RIGHTEATCARD_P),
								iXPos,
								0);
			break;
		}
	}
	//刷新界面
	dc.BitBlt(0,0,CardSize.cx,CardSize.cy,&BufferDC,0,0,SRCCOPY);
	//清理资源
	BufferDC.SelectObject(pOldBmp);
	BufferDC.DeleteDC();
	BufferBmp.DeleteObject();
}
void CGameCard::DrawOutCard()
{
	CSize CardSize;
	switch(m_bPlayerWhich)
	{
	case SWX_MODE_ME:
			CardSize.cx=WIDTH_MEOUTCARD*CARDOUT_NUM;		
			CardSize.cy=HEIGHT_MEOUTCARD*(m_bOutCardCount/CARDOUT_NUM+1);
		break;
	case SWX_MODE_LEFT:
			CardSize.cx=WIDTH_LEFTOUTCARD*(m_bOutCardCount/CARDOUT_NUM+1);		
			CardSize.cy=HEIGHT_LEFTOUTCARD_P*(CARDOUT_NUM-1)+HEIGHT_LEFTOUTCARD_A;
		break;
	case SWX_MODE_FRONT:
			CardSize.cx=WIDTH_FRONTOUTCARD*CARDOUT_NUM;		
			CardSize.cy=HEIGHT_FRONTOUTCARD*(m_bOutCardCount/CARDOUT_NUM+1);
		break;
	case SWX_MODE_RIGHT:
			CardSize.cx=WIDTH_RIGHTOUTCARD*(m_bOutCardCount/CARDOUT_NUM+1);		
			CardSize.cy=HEIGHT_RIGHTOUTCARD_P*(CARDOUT_NUM-1)+HEIGHT_RIGHTOUTCARD_A;
		break;
	}
	//建立缓冲
	CPaintDC dc(this);
	CDC BufferDC;
	CBitmap BufferBmp;
	BufferDC.CreateCompatibleDC(&dc);
	BufferBmp.CreateCompatibleBitmap(&dc,CardSize.cx,CardSize.cy);
	CBitmap * pOldBmp=BufferDC.SelectObject(&BufferBmp);
	//绘制麻将

	int iXPos=0;
	CSkinImage * pActiveHandle=NULL;
	pActiveHandle=&m_CardPic[1];
	CImageHandle HandleCrad(pActiveHandle);
	for (BYTE i=0; i<m_bOutCardCount; i++)
	{
		BYTE bCard=m_CardOutArray.GetAt(i);

		switch(m_bPlayerWhich)
		{
		case SWX_MODE_ME:
			iXPos=GetCardXPos(bCard)*WIDTH_MEOUTCARD;
			
			pActiveHandle->BitBlt(BufferDC.m_hDC,
								(i%CARDOUT_NUM)*WIDTH_MEOUTCARD,
								(m_bOutCardCount/CARDOUT_NUM-i/CARDOUT_NUM)*HEIGHT_MEOUTCARD,
								WIDTH_MEOUTCARD,
								HEIGHT_MEOUTCARD,
								iXPos,
								0);
			break;
		case SWX_MODE_LEFT:
			iXPos=GetCardXPos(bCard)*WIDTH_LEFTOUTCARD;
		
			pActiveHandle->BitBlt(BufferDC.m_hDC,
								(i/CARDOUT_NUM)*WIDTH_LEFTOUTCARD,
								(i%CARDOUT_NUM)*HEIGHT_LEFTOUTCARD_P,
								WIDTH_LEFTOUTCARD,
								HEIGHT_LEFTOUTCARD_A,
								iXPos,
								0);
			break;
		case SWX_MODE_FRONT:
			iXPos=GetCardXPos(bCard)*WIDTH_FRONTOUTCARD;
		
			pActiveHandle->BitBlt(BufferDC.m_hDC,
								(CARDOUT_NUM-i%CARDOUT_NUM-1)*(WIDTH_FRONTOUTCARD),
								i/CARDOUT_NUM*HEIGHT_FRONTOUTCARD,
								WIDTH_FRONTOUTCARD,
								HEIGHT_FRONTOUTCARD,
								iXPos,
								0);
			break;
		case SWX_MODE_RIGHT:
			iXPos=GetCardXPos(bCard)*WIDTH_RIGHTOUTCARD;
		
			pActiveHandle->BitBlt(BufferDC.m_hDC,
								(m_bOutCardCount/CARDOUT_NUM-i/CARDOUT_NUM)*WIDTH_RIGHTOUTCARD,
								(CARDOUT_NUM-i%CARDOUT_NUM-1)*HEIGHT_RIGHTOUTCARD_P,
								WIDTH_RIGHTOUTCARD,
								(i%CARDOUT_NUM==0?HEIGHT_RIGHTOUTCARD_A:HEIGHT_RIGHTOUTCARD_P),
								iXPos,
								0);
			break;
		}
	}
	//刷新界面
	dc.BitBlt(0,0,CardSize.cx,CardSize.cy,&BufferDC,0,0,SRCCOPY);
	//清理资源
	BufferDC.SelectObject(pOldBmp);
	BufferDC.DeleteDC();
	BufferBmp.DeleteObject();
}
//绘制未揭牌窗口
void CGameCard::DrawHideCard()
{
	CSize CardSize;
	CardSize.cx=WIDTH_HIDECARD*((m_bHideCardCount+1)/2);		
	CardSize.cy=HEIGHT_HIDECARD+TOP_HIDECARD;
	
	//建立缓冲
	CPaintDC dc(this);
	CDC BufferDC,BufferBackDC;
	CBitmap BufferBmp;

	BufferDC.CreateCompatibleDC(&dc);
	BufferBmp.CreateCompatibleBitmap(&dc,CardSize.cx,CardSize.cy);
	CBitmap * pOldBmp=BufferDC.SelectObject(&BufferBmp);

	//绘制麻将
	CSkinImage * pActiveHandle=NULL;
	pActiveHandle=&m_CardPic[1];
	CImageHandle HandleCrad(pActiveHandle);
	for (BYTE i=0; i<m_bHideCardCount; i++)
	{
		if(m_bHideCardCount%2!=0)
			pActiveHandle->BitBlt(BufferDC.m_hDC,
							((m_bHideCardCount-i)/2)*WIDTH_HIDECARD,
							(1-i%2)*TOP_HIDECARD,
							WIDTH_HIDECARD,
							HEIGHT_HIDECARD,
							0,
							0);
		else
			pActiveHandle->BitBlt(BufferDC.m_hDC,
							((m_bHideCardCount-i-1)/2)*WIDTH_HIDECARD,
							(1-i%2)*TOP_HIDECARD,
							WIDTH_HIDECARD,
							HEIGHT_HIDECARD,
							0,
							0);
	}
	//刷新界面
	dc.BitBlt(0,0,CardSize.cx,CardSize.cy,&BufferDC,0,0,SRCCOPY);
	//清理资源*/
	BufferDC.SelectObject(pOldBmp);
	BufferDC.DeleteDC();
	BufferBmp.DeleteObject();
}
//绘制吃牌窗口
void CGameCard::DrawTrainCard()
{
	CSize CardSize;
	CardSize.cx=WIDTH_MEINCARD*TRAINCARD_NUM;		
	CardSize.cy=HEIGHT_MEINCARD*m_bTrainCardCount;
	
	//建立缓冲
	CPaintDC dc(this);
	CDC BufferDC,BufferBackDC;
	CBitmap BufferBmp;

	BufferDC.CreateCompatibleDC(&dc);
	BufferBmp.CreateCompatibleBitmap(&dc,CardSize.cx,CardSize.cy);
	CBitmap * pOldBmp=BufferDC.SelectObject(&BufferBmp);

	//绘制麻将
	int iXPos=0;
	CSkinImage * pActiveHandle=NULL;
	pActiveHandle=&m_CardPic[0];
	CImageHandle HandleCrad(pActiveHandle);
	for (BYTE i=0; i<m_bTrainCardCount; i++)
	{
		for (BYTE j=0; j<TRAINCARD_NUM; j++)
		{
			BYTE bCard=m_TrainCard[i*TRAINCARD_NUM+j];
			iXPos=GetCardXPos(bCard)*WIDTH_MEINCARD;
			pActiveHandle->BitBlt(BufferDC.m_hDC,
							j*WIDTH_MEINCARD,
							i*HEIGHT_MEINCARD,
							WIDTH_MEINCARD,
							HEIGHT_MEINCARD,
							iXPos,
							0);
		}
	}
	//刷新界面
	dc.BitBlt(0,0,CardSize.cx,CardSize.cy,&BufferDC,0,0,SRCCOPY);
	//清理资源*/
	BufferDC.SelectObject(pOldBmp);
	BufferDC.DeleteDC();
	BufferBmp.DeleteObject();
}
void CGameCard::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	if(!m_CardPic[0].IsNull())
	{
		m_CardPic[0].Detach();
	}
	if(!m_CardPic[1].IsNull())
	{
		m_CardPic[1].Detach();
	}
	//装载图片
	HINSTANCE hInstance = ::AfxGetInstanceHandle();
	if(m_bCenterCardCount==1)
	{
		m_CardPic[0].SetLoadInfo(IDB_MCARD,hInstance);
		m_CardPic[1].SetLoadInfo(IDB_MHCARD,hInstance);
	}
	if(m_bHideCardCount>0)
	{
		m_CardPic[0].SetLoadInfo(IDB_HBCARD,hInstance);
		m_CardPic[1].SetLoadInfo(IDB_HCARD,hInstance);
	}
	else
	{
		if(m_bTrainCardCount>0)
		{
			m_CardPic[0].SetLoadInfo(IDB_MCARD,hInstance);
			m_CardPic[1].SetLoadInfo(IDB_MHCARD,hInstance);
		}
		else
		{
			switch(m_bPlayerWhich)
			{
			case SWX_MODE_ME:
				m_CardPic[0].SetLoadInfo(IDB_MCARD,hInstance);
				m_CardPic[1].SetLoadInfo(IDB_MHCARD,hInstance);
				break;
			case SWX_MODE_LEFT:
				m_CardPic[0].SetLoadInfo(IDB_LCARD,hInstance);
				m_CardPic[1].SetLoadInfo(IDB_LHCARD,hInstance);
				break;
			case SWX_MODE_FRONT:
				m_CardPic[0].SetLoadInfo(IDB_FCARD,hInstance);
				m_CardPic[1].SetLoadInfo(IDB_FHCARD,hInstance);
				break;
			case SWX_MODE_RIGHT:
				m_CardPic[0].SetLoadInfo(IDB_RCARD,hInstance);
				m_CardPic[1].SetLoadInfo(IDB_RHCARD,hInstance);
				break;
			}
		}
	}
	// TODO: 在此处添加消息处理程序代码
}

//按键测试
BYTE CGameCard::HitCardTest(CPoint Point)
{
	if(Point.x<1||Point.x>m_bInCardCount*WIDTH_MEINCARD-1||Point.y<1||Point.y>(HEIGHT_MEINCARD+CARDUP_HEIGHT-1))
		return 250;
	return (BYTE)((Point.x)/WIDTH_MEINCARD);
}
//调整手中牌的窗口位置
void CGameCard::FixInCardControl()
{
	//设置区域
	CRect CardRect;
	if(m_bInCardCount>0)
	{
		CRgn   m_CardViewRgn;					//手中麻将区域
		switch(m_bPlayerWhich)
		{
		case SWX_MODE_ME:
			CardRect.left=m_PtInBasePoint.x;
			CardRect.bottom=m_PtInBasePoint.y;
			CardRect.right=CardRect.left+WIDTH_MEINCARD*m_bInCardCount;
			CardRect.top=CardRect.bottom-HEIGHT_MEINCARD-CARDUP_HEIGHT;
			break;
		case SWX_MODE_LEFT:
			CardRect.left=m_PtInBasePoint.x;
			CardRect.top=m_PtInBasePoint.y;
			CardRect.right=CardRect.left+WIDTH_LEFTINCARD;
			CardRect.bottom=CardRect.top+HEIGHT_LEFTINCARD_P*(m_bInCardCount-1)+HEIGHT_LEFTINCARD_A;
			break;
		case SWX_MODE_FRONT:
			CardRect.right=m_PtInBasePoint.x;
			CardRect.top=m_PtInBasePoint.y;
			CardRect.left=CardRect.right-WIDTH_FRONTINCARD*m_bInCardCount;
			CardRect.bottom=CardRect.top+HEIGHT_FRONTINCARD;
			break;
		case SWX_MODE_RIGHT:
			CardRect.right=m_PtInBasePoint.x;
			CardRect.bottom=m_PtInBasePoint.y;
			CardRect.left=CardRect.right-WIDTH_RIGHTINCARD;
			CardRect.top=CardRect.bottom-HEIGHT_RIGHTINCARD_P*(m_bInCardCount-1)-HEIGHT_RIGHTINCARD_A;
			break;
		}
		MoveWindow(&CardRect);
		if(m_bPlayerWhich==SWX_MODE_ME)
		{
			//设置弹起牌的区域
			CRgn AllRgn,SignedRgn;
			AllRgn.CreateRectRgn(0,0,0,0);
			for (BYTE i=0;i<m_bInCardCount;i++)
			{
				if(m_HitCard==i)
				{
					SignedRgn.CreateRoundRectRgn(i*WIDTH_MEINCARD,m_MouseMove?0:CARDUP_HEIGHT,(i+1)*WIDTH_MEINCARD+1,m_MouseMove?HEIGHT_MEINCARD:HEIGHT_MEINCARD+CARDUP_HEIGHT,2,2);
					AllRgn.CombineRgn(&AllRgn,&SignedRgn,RGN_OR);
					SignedRgn.DeleteObject();
				}
				else
				{
					SignedRgn.CreateRoundRectRgn(i*WIDTH_MEINCARD,CARDUP_HEIGHT,(i+1)*WIDTH_MEINCARD+1,HEIGHT_MEINCARD+CARDUP_HEIGHT,2,2);
					AllRgn.CombineRgn(&AllRgn,&SignedRgn,RGN_OR);
					SignedRgn.DeleteObject();
				}
			}
			//设置区域
			SetWindowRgn(AllRgn,TRUE);
			m_CardViewRgn.DeleteObject();
			m_CardViewRgn.Attach(AllRgn.Detach());
		}
	}
	else
	{
		CardRect.left=CardRect.right;
		CardRect.bottom=CardRect.top;
		MoveWindow(&CardRect);
	}
}
//调整吃的牌的窗口位置
void CGameCard::FixEatCardControl()
{
	CRect CardRect;
	if(m_bEatCardCount>0)
	{
		//设置区域
		CRgn   m_CardViewRgn;					//吃的麻将区域
		switch(m_bPlayerWhich)
		{
		case SWX_MODE_ME:
			CardRect.left=m_PtEatBasePoint.x;
			CardRect.bottom=m_PtEatBasePoint.y;
			CardRect.right=CardRect.left+WIDTH_MEEATCARD*m_bEatCardCount;
			CardRect.top=CardRect.bottom-HEIGHT_MEEATCARD;
			break;
		case SWX_MODE_LEFT:
			CardRect.left=m_PtEatBasePoint.x;
			CardRect.top=m_PtEatBasePoint.y;
			CardRect.right=CardRect.left+WIDTH_LEFTEATCARD;
			CardRect.bottom=CardRect.top+HEIGHT_LEFTEATCARD_P*(m_bEatCardCount-1)+HEIGHT_LEFTEATCARD_A;
			break;
		case SWX_MODE_FRONT:
			CardRect.right=m_PtEatBasePoint.x;
			CardRect.top=m_PtEatBasePoint.y;
			CardRect.left=CardRect.right-WIDTH_FRONTEATCARD*m_bEatCardCount;
			CardRect.bottom=CardRect.top+HEIGHT_FRONTEATCARD;
			break;
		case SWX_MODE_RIGHT:
			CardRect.right=m_PtEatBasePoint.x;
			CardRect.bottom=m_PtEatBasePoint.y;
			CardRect.left=CardRect.right-WIDTH_RIGHTEATCARD;
			CardRect.top=CardRect.bottom-HEIGHT_RIGHTEATCARD_P*(m_bEatCardCount-1)-HEIGHT_RIGHTEATCARD_A;
			break;
		}
		MoveWindow(&CardRect);
	}
	else
	{
		CardRect.left=CardRect.right;
		CardRect.bottom=CardRect.top;
		MoveWindow(&CardRect);
	}
}
//调整打出的牌的窗口位置
void CGameCard::FixOutCardControl()
{
	CRect CardRect;
	if(m_bOutCardCount>0)
	{
		//设置区域
		CRgn   m_CardViewRgn;					//打出去的麻将区域
		switch(m_bPlayerWhich)
		{
		case SWX_MODE_ME:
			CardRect.left=m_PtOutBasePoint.x;
			CardRect.bottom=m_PtOutBasePoint.y;
			CardRect.right=CardRect.left+WIDTH_MEOUTCARD*CARDOUT_NUM;
			CardRect.top=CardRect.bottom-HEIGHT_MEOUTCARD*(m_bOutCardCount/CARDOUT_NUM+1);
			break;
		case SWX_MODE_LEFT:
			CardRect.left=m_PtOutBasePoint.x;
			CardRect.top=m_PtOutBasePoint.y;
			CardRect.bottom=CardRect.top+HEIGHT_LEFTOUTCARD_P*(CARDOUT_NUM-1)+HEIGHT_LEFTOUTCARD_A;
			CardRect.right=CardRect.left+WIDTH_LEFTOUTCARD*(m_bOutCardCount/CARDOUT_NUM+1);
			break;
		case SWX_MODE_FRONT:
			CardRect.right=m_PtOutBasePoint.x;
			CardRect.top=m_PtOutBasePoint.y;
			CardRect.left=CardRect.right-WIDTH_FRONTOUTCARD*CARDOUT_NUM;
			CardRect.bottom=CardRect.top+HEIGHT_FRONTOUTCARD*(m_bOutCardCount/CARDOUT_NUM+1);
			break;
		case SWX_MODE_RIGHT:
			CardRect.right=m_PtOutBasePoint.x;
			CardRect.bottom=m_PtOutBasePoint.y;
			CardRect.top=CardRect.bottom-HEIGHT_RIGHTOUTCARD_P*(CARDOUT_NUM-1)-HEIGHT_RIGHTOUTCARD_A;
			CardRect.left=CardRect.right-WIDTH_RIGHTOUTCARD*(m_bOutCardCount/CARDOUT_NUM+1);
			break;
		}
		MoveWindow(&CardRect);
		CRgn AllRgn,SignedRgn;
		AllRgn.CreateRectRgn(0,0,0,0);
		for (BYTE i=0;i<m_bOutCardCount;i++)
		{
			switch(m_bPlayerWhich)
			{
			case SWX_MODE_ME:
				SignedRgn.CreateRoundRectRgn((i%CARDOUT_NUM)*WIDTH_MEOUTCARD,(m_bOutCardCount/CARDOUT_NUM-i/CARDOUT_NUM)*HEIGHT_MEOUTCARD,(i%CARDOUT_NUM+1)*WIDTH_MEOUTCARD+1,(m_bOutCardCount/CARDOUT_NUM-i/CARDOUT_NUM+1)*HEIGHT_MEOUTCARD,2,2);
				AllRgn.CombineRgn(&AllRgn,&SignedRgn,RGN_OR);
				SignedRgn.DeleteObject();
				break;
			case SWX_MODE_LEFT:
				SignedRgn.CreateRoundRectRgn((i/CARDOUT_NUM)*WIDTH_LEFTOUTCARD,(i%CARDOUT_NUM)*HEIGHT_LEFTOUTCARD_P,(i/CARDOUT_NUM+1)*WIDTH_LEFTOUTCARD,(i%CARDOUT_NUM)*HEIGHT_LEFTOUTCARD_P+HEIGHT_LEFTOUTCARD_A,2,2);
				AllRgn.CombineRgn(&AllRgn,&SignedRgn,RGN_OR);
				SignedRgn.DeleteObject();
				break;
			case SWX_MODE_FRONT:
				SignedRgn.CreateRoundRectRgn((CARDOUT_NUM-i%CARDOUT_NUM-1)*WIDTH_FRONTOUTCARD,(i/CARDOUT_NUM)*HEIGHT_FRONTOUTCARD,(CARDOUT_NUM-i%CARDOUT_NUM)*WIDTH_FRONTOUTCARD,(i/CARDOUT_NUM+1)*HEIGHT_FRONTOUTCARD,2,2);
				AllRgn.CombineRgn(&AllRgn,&SignedRgn,RGN_OR);
				SignedRgn.DeleteObject();
				break;
			case SWX_MODE_RIGHT:
				SignedRgn.CreateRoundRectRgn((m_bOutCardCount/CARDOUT_NUM-i/CARDOUT_NUM)*WIDTH_RIGHTOUTCARD,(CARDOUT_NUM-i%CARDOUT_NUM-1)*HEIGHT_RIGHTOUTCARD_P,(m_bOutCardCount/CARDOUT_NUM-i/CARDOUT_NUM+1)*WIDTH_RIGHTOUTCARD,(CARDOUT_NUM-i%CARDOUT_NUM-1)*HEIGHT_RIGHTOUTCARD_P+HEIGHT_RIGHTOUTCARD_A,2,2);
				AllRgn.CombineRgn(&AllRgn,&SignedRgn,RGN_OR);
				SignedRgn.DeleteObject();
				break;
			}
		}
		//设置区域
		SetWindowRgn(AllRgn,TRUE);
		m_CardViewRgn.DeleteObject();
		m_CardViewRgn.Attach(AllRgn.Detach());
	}
	else
	{
		CardRect.left=CardRect.right;
		CardRect.bottom=CardRect.top;
		MoveWindow(&CardRect);
	}
}
//调整未揭的牌的窗口位置
void CGameCard::FixHideCardControl()
{
	CRect CardRect;
	if(m_bHideCardCount>0)
	{
		//设置区域
		CRgn   m_CardViewRgn;					//吃的麻将区域
		CardRect.left=m_PtHideBasePoint.x;
		CardRect.top=m_PtHideBasePoint.y;
		CardRect.right=CardRect.left+WIDTH_HIDECARD*((m_bHideCardCount+1)/2);
		CardRect.bottom=CardRect.top+HEIGHT_HIDECARD+TOP_HIDECARD;
		MoveWindow(&CardRect);

		//设置弹起牌的区域
		CRgn AllRgn,SignedRgn;
		AllRgn.CreateRectRgn(0,0,0,0);
		for (BYTE i=0;i<m_bHideCardCount;i++)
		{
			if(m_bHideCardCount%2!=0)
				SignedRgn.CreateRoundRectRgn(((m_bHideCardCount-i)/2)*WIDTH_HIDECARD,(1-i%2)*TOP_HIDECARD,((m_bHideCardCount-i)/2+1)*WIDTH_HIDECARD+1,(1-i%2)*TOP_HIDECARD+HEIGHT_HIDECARD+1,2,2);
			else
				SignedRgn.CreateRoundRectRgn(((m_bHideCardCount-i-1)/2)*WIDTH_HIDECARD,(1-i%2)*TOP_HIDECARD,((m_bHideCardCount-i-1)/2+1)*WIDTH_HIDECARD+1,(1-i%2)*TOP_HIDECARD+HEIGHT_HIDECARD+1,2,2);
			AllRgn.CombineRgn(&AllRgn,&SignedRgn,RGN_OR);
			SignedRgn.DeleteObject();
		}
		//设置区域
		SetWindowRgn(AllRgn,TRUE);
		m_CardViewRgn.DeleteObject();
		m_CardViewRgn.Attach(AllRgn.Detach());
	}
	else
	{
		CardRect.left=CardRect.right;
		CardRect.top=CardRect.bottom; 
		MoveWindow(&CardRect);
	}
}

//调整吃的牌的窗口位置
void CGameCard::FixTrainCardControl()
{
	CRect CardRect;
	if(m_bTrainCardCount>0)
	{
		//设置区域
		CardRect.left=m_PtTrainBasePoint.x;
		CardRect.top=m_PtTrainBasePoint.y;
		CardRect.right=CardRect.left+WIDTH_MEINCARD*TRAINCARD_NUM;
		CardRect.bottom=CardRect.top+HEIGHT_MEINCARD*m_bTrainCardCount;
		MoveWindow(&CardRect);
	}
	else
	{
		CardRect.left=CardRect.right;
		CardRect.top=CardRect.bottom; 
		MoveWindow(&CardRect);
	}
}
