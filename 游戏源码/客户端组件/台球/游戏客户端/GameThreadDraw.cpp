#include "StdAfx.h"
#include "GameThreadDraw.h"
#include "GameClientView.h"
#include "math.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
DrawInfoStruct			g_DrawInfo;

//构造函数
CGameThreadDraw::CGameThreadDraw(CWnd * pGameView) : CThreadDraw(pGameView)
{
}
 CGameThreadDraw::~CGameThreadDraw()
 {

 }

bool CGameThreadDraw::InitDraw(CDC * pDrawDC)
 {
	 GDIModeInitThread();
	 return CThreadDraw::InitDraw(pDrawDC);
 }
	//销毁绘画
 bool CGameThreadDraw::UnInitDraw()
 {
	return CThreadDraw::UnInitDraw();
 }
	//绘画函数
void CGameThreadDraw::ThreadDrawView(CDC * pDC, int nClinetWith, int nClinetHeight)
 {	
	 pDC->FillSolidRect(0,0,nClinetWith,nClinetHeight,RGB(125,125,125));
	 //return;
	 GDIModeDraw(pDC);
 }

//绘画函数
void CGameThreadDraw::GDIModeDraw(CDC * pDC)
{
	//设置变量
	int i,j;
	int iXBasePoint=g_DrawInfo.pPlayView->GetXBase();
	int iYBasePoint=g_DrawInfo.pPlayView->GetYBase();
	for (int x=0;x<g_DrawInfo.ClientRect.bottom;x+=m_BackImage.GetHeight())
	{
	for (i=0;i<(g_DrawInfo.ClientRect.right/m_BackImage.GetWidth()+1);i++)
	{
		m_BackImage.BitBlt(pDC->m_hDC,i*m_BackImage.GetWidth(),x,g_DrawInfo.ClientRect.right,g_DrawInfo.ClientRect.bottom,0,0,SRCCOPY);
	}
	}
		//绘画背景
	m_DeskImage.BitBlt(pDC->m_hDC,iXBasePoint-(int)(DESK_SIDE*g_DrawInfo.dwTimes),iYBasePoint-(int)(DESK_SIDE*g_DrawInfo.dwTimes),
		g_DrawInfo.BackBmpInfo.bmWidth,g_DrawInfo.BackBmpInfo.bmHeight,
		0,0,SRCCOPY);
	//画球
	for (i=0;i<g_DrawInfo.pDesk->m_iBallCount;i++)
	{
		if (g_DrawInfo.pBallList[i].m_bAccess)
		{
			m_BallImage.AlphaDrawImage(pDC,
				(int)(g_DrawInfo.pBallList[i].m_xPos*g_DrawInfo.dwTimes+iXBasePoint-BALL_RADII*g_DrawInfo.dwTimes),
				(int)(g_DrawInfo.pBallList[i].m_yPos*g_DrawInfo.dwTimes+iYBasePoint-(BALL_RADII)*g_DrawInfo.dwTimes),
				(int)((double)BALL_RADII*2*g_DrawInfo.dwTimes),
				(int)((double)(BALL_RADII)*2*g_DrawInfo.dwTimes),
				(BallBmpInfo.bmWidth)*g_DrawInfo.pBallList[i].m_bColor,
				0,
				BALL_BACK_COLOR);
		}
	}
	//画设置球
	if (g_DrawInfo.bStation==WATI_SET_BALL)
	{
		::GetCursorPos(&g_DrawInfo.CursorPos);
		g_DrawInfo.pPlayView->ScreenToClient(&g_DrawInfo.CursorPos);
		m_BallImage.AlphaDrawImage(pDC,
			(int)((double)g_DrawInfo.CursorPos.x-(BALL_RADII)),
			(int)((double)g_DrawInfo.CursorPos.y-(BALL_RADII)),
			(int)((double)BALL_RADII*2*g_DrawInfo.dwTimes),
			(int)((double)BALL_RADII*2*g_DrawInfo.dwTimes),
			0,
			0,
			BALL_BACK_COLOR);
		
		if (g_DrawInfo.bOkSet==false)
		{
			pDC->SetTextColor(RGB(255,0,0));
			pDC->SetTextAlign(TA_LEFT|TA_TOP);
			pDC->SelectObject(&g_DrawInfo.LittleFont);
			pDC->TextOut(g_DrawInfo.CursorPos.x+(int)(BALL_RADII),g_DrawInfo.CursorPos.y+(int)(BALL_RADII),TEXT("NO"),2);
		}
	}
	//画袋
	m_BagImage.AlphaDrawImage(pDC,
		iXBasePoint+(g_DrawInfo.BackBmpInfo.bmWidth-BagBmpInfo.bmWidth)/2-30,
		iYBasePoint+g_DrawInfo.BackBmpInfo.bmHeight-(int)(DESK_SIDE*g_DrawInfo.dwTimes)+5,
		BagBmpInfo.bmWidth,
		BagBmpInfo.bmHeight,
		0,
		0,
		RGB(0,128,128));
	//画袋球
	int iXBagBase=iXBasePoint+(g_DrawInfo.BackBmpInfo.bmWidth-BagBmpInfo.bmWidth)/2;
	int iYBagBase=iYBasePoint+g_DrawInfo.BackBmpInfo.bmHeight-(int)(DESK_SIDE*g_DrawInfo.dwTimes)+7;
	for (i=g_DrawInfo.pDesk->m_BallBag.m_iBallCount-1;i>=0;i--)
	{
		if(i>=0)
		{
			m_BallInBagImage.AlphaDrawImage(pDC,
				iXBagBase+g_DrawInfo.pDesk->m_BallBag.m_bBallStation[i]-(int)((BALL_RADII_A)*g_DrawInfo.dwTimes)-25,
				iYBagBase+5,
				(int)((BALL_RADII)*2.0*g_DrawInfo.dwTimes),
				(int)((BALL_RADII)*2.0*g_DrawInfo.dwTimes),
				BallInBagBmpInfo.bmWidth*g_DrawInfo.pDesk->m_BallBag.m_pBall[i]->m_bColor,
				0,
				BALL_BACK_COLOR);
		}
	}
	//画控制信息
	if ((g_DrawInfo.bStation==ME_HIT_BALL)||(g_DrawInfo.bStation==OTHER_HIT_BALL))
	{
		//获取光标
		if (g_DrawInfo.bStation==ME_HIT_BALL)
		{
			::GetCursorPos(&g_DrawInfo.CursorPos);
			g_DrawInfo.pPlayView->ScreenToClient(&g_DrawInfo.CursorPos);
		}
		else
		{
			for (i=0;i<50;i++)
			{
				if (g_DrawInfo.CursorPos.x>g_DrawInfo.OtherPolePos.x) g_DrawInfo.CursorPos.x--;
				else if (g_DrawInfo.CursorPos.x<g_DrawInfo.OtherPolePos.x) g_DrawInfo.CursorPos.x++;
				if (g_DrawInfo.CursorPos.y>g_DrawInfo.OtherPolePos.y) g_DrawInfo.CursorPos.y--;
				else if (g_DrawInfo.CursorPos.y<g_DrawInfo.OtherPolePos.y) g_DrawInfo.CursorPos.y++;
			}
			if(m_iPower>g_DrawInfo.iPower) m_iPower-=1.0;
				else if(m_iPower<g_DrawInfo.iPower) m_iPower+=1.0;
		}
		//获取光标
		double XMid=(double)g_DrawInfo.CursorPos.x-(g_DrawInfo.pBallList[0].m_xPos*g_DrawInfo.dwTimes+(double)iXBasePoint);
		double YMid=(double)g_DrawInfo.CursorPos.y-(g_DrawInfo.pBallList[0].m_yPos*g_DrawInfo.dwTimes+(double)iYBasePoint);
		double iMid=sqrt(XMid*XMid+YMid*YMid);
		double sina=-YMid/iMid;
		double cosa=-XMid/iMid;
		if ((YMid==0.0)&&(XMid==0.0)) 
		{ 
			sina=0.0;
			cosa=-1.0; 
		}
		//画棍
		COLORREF crPole;
		for (i=0;i<PoleBmpInfo.bmWidth;i++)
		{
			for (j=0;j<PoleBmpInfo.bmHeight;j++)
			{
				crPole=m_PoleImage.GetPixel(i,j);
				if (crPole!=RGB(255,0,255)) 
				{
						double x=(i)*cosa-(j-6)*sina;
						double y=(i)*sina+(j-6)*cosa;
						pDC->SetPixel(
						(int)((double)(iXBasePoint+g_DrawInfo.pBallList[0].m_xPos)*g_DrawInfo.dwTimes+x+(g_DrawInfo.iPower*8.0*BALL_RADII/100.0+2.0*BALL_RADII)*cosa),
						(int)((double)(iYBasePoint+g_DrawInfo.pBallList[0].m_yPos)*g_DrawInfo.dwTimes+y+(g_DrawInfo.iPower*8.0*BALL_RADII/100.0+2.0*BALL_RADII)*sina),
						crPole);

				}
			}
		}
		//画目标球
		if(g_DrawInfo.bStation==OTHER_HIT_BALL)
		{
			m_BallImage.AlphaDrawImage(pDC,
				(int)((double)g_DrawInfo.CursorPos.x-(BALL_RADII)),
				(int)((double)g_DrawInfo.CursorPos.y-(BALL_RADII)),
				(int)((double)BALL_RADII*2*g_DrawInfo.dwTimes),
				(int)((double)BALL_RADII*2*g_DrawInfo.dwTimes),
				0,
				0,
				BALL_BACK_COLOR);
		}
		//画线
		CPen m_Pen;
		//m_Pen.CreatePen(2,1,RGB(200,200,200));
		m_Pen.CreatePen(2,1,RGB(255,255,0));
		CPen *m_OldPen=pDC->SelectObject(&m_Pen);

		if(g_DrawInfo.CursorPos.x>iXBasePoint&&g_DrawInfo.CursorPos.y>iYBasePoint&&
			g_DrawInfo.CursorPos.y<iYBasePoint+g_DrawInfo.BackBmpInfo.bmHeight-2*DESK_SIDE&&
			g_DrawInfo.CursorPos.x<(iXBasePoint+g_DrawInfo.BackBmpInfo.bmWidth-2*DESK_SIDE))
		//if ((g_DrawInfo.CursorPos.x>=0)&&(g_DrawInfo.CursorPos.x<g_DrawInfo.ClientRect.right)&&
		//	(g_DrawInfo.CursorPos.y>0)&&(g_DrawInfo.CursorPos.y<g_DrawInfo.ClientRect.bottom))
		{
			pDC->MoveTo(g_DrawInfo.CursorPos.x,g_DrawInfo.CursorPos.y);
			pDC->LineTo((int)((double)(g_DrawInfo.pBallList[0].m_xPos)*g_DrawInfo.dwTimes)+iXBasePoint,
				(int)((double)(g_DrawInfo.pBallList[0].m_yPos)*g_DrawInfo.dwTimes)+iYBasePoint);
		}
		pDC->SelectObject(m_OldPen);
	}

	//写信息字符
	if (g_DrawInfo.pDesk->m_iTextPos!=0)	
	{
		pDC->SetTextColor(RGB(255,255,255));
		pDC->SetTextAlign(TA_CENTER|TA_TOP);
		pDC->SelectObject(&g_DrawInfo.BigFont);
		pDC->TextOut(g_DrawInfo.ClientRect.right/2,g_DrawInfo.pDesk->m_iTextPos,
			g_DrawInfo.pDesk->m_szText,lstrlen(g_DrawInfo.pDesk->m_szText));
	}
		
	//写入显示帧
	static TCHAR szBuffer[50];
	g_DrawInfo.dwSpeed=this->GetSpeed();
	sprintf(szBuffer,TEXT("每秒：%ld 帧"),g_DrawInfo.dwSpeed);
	pDC->SetTextAlign(TA_LEFT|TA_TOP);
	pDC->SelectObject(&g_DrawInfo.LittleFont);
	pDC->SetTextColor(RGB(255,255,255));
	pDC->TextOut(50,g_DrawInfo.ClientRect.bottom-25,szBuffer,lstrlen(szBuffer));
	if(g_DrawInfo.m_StarPos<6)
	{
		BYTE  m_PosChangeX=17;
		BYTE  m_PosChangeY=14;
		switch(g_DrawInfo.m_StarPos)
		{
		case 0:
			break;
		case 1:
			m_PosChangeX+=9;
			m_PosChangeY+=5;
			break;
		case 2:
			m_PosChangeX+=12;
			break;
		case 3:
			m_PosChangeX+=2;
			m_PosChangeY+=17;
			break;
		case 4:
			m_PosChangeX+=8;
			m_PosChangeY+=11;
			break;
		case 5:
			m_PosChangeX+=13;
			m_PosChangeY+=17;
			break;
		}
		m_AppendImage.AlphaDrawImage(pDC,
			iXBasePoint-(int)(DESK_SIDE*g_DrawInfo.dwTimes)+g_DrawInfo.pDesk->m_BagPoint[g_DrawInfo.m_StarPos].x+m_PosChangeX,
			iYBasePoint-(int)(DESK_SIDE*g_DrawInfo.dwTimes)+g_DrawInfo.pDesk->m_BagPoint[g_DrawInfo.m_StarPos].y+m_PosChangeY,
				//50,
				//pDrawInfo->ClientRect.bottom-50,
				32,32,(m_StarFlash++)>2?32:0,0,RGB(255,0,255));
		if(m_StarFlash>5)
			m_StarFlash=0;
	}
	if (g_DrawInfo.bStation>BALL_RUN_ING)
		g_DrawInfo.pPlayView->m_Station.Invalidate(FALSE);
}
	//return;
void CGameThreadDraw::GDIModeInitThread()
{

	//绘画背景
	m_StarFlash=0;
	HINSTANCE hInstance = ::AfxGetInstanceHandle();
	m_BackImage.LoadFromResource(hInstance,IDB_BKG);
	m_DeskImage.LoadFromResource(hInstance,IDB_BACK);
	g_DrawInfo.BackBmpInfo.bmWidth=m_DeskImage.GetWidth();
	g_DrawInfo.BackBmpInfo.bmHeight=m_DeskImage.GetHeight();
	m_BallImage.LoadFromResource(hInstance,IDB_BALL);
	BallBmpInfo.bmWidth=m_BallImage.GetWidth()/9;
	BallBmpInfo.bmHeight=m_BallImage.GetHeight();
	m_BallAImage.LoadFromResource(hInstance,IDB_BALL_A);
	BallABmpInfo.bmWidth=m_BallAImage.GetWidth()/9;
	BallABmpInfo.bmHeight=m_BallAImage.GetHeight();
	m_PoleImage.LoadFromResource(hInstance,IDB_POLE);
	PoleBmpInfo.bmWidth=m_PoleImage.GetWidth();
	PoleBmpInfo.bmHeight=m_PoleImage.GetHeight();
	m_BagImage.LoadFromResource(hInstance,IDB_BALL_BAG);
	BagBmpInfo.bmWidth=m_BagImage.GetWidth();
	BagBmpInfo.bmHeight=m_BagImage.GetHeight();
	g_DrawInfo.pDesk->m_BallBag.m_iBagLength=m_BagImage.GetWidth()-10;
	m_AppendImage.LoadFromResource(hInstance,IDB_STAR);
	m_BallInBagImage.LoadFromResource(hInstance,IDB_BALL_INBAG);
	BallInBagBmpInfo.bmWidth=m_BallInBagImage.GetWidth()/9;
	BallInBagBmpInfo.bmHeight=m_BallInBagImage.GetHeight();
	//画笔和字体
	m_iPower=0.0;
	CPen Pen(PS_DOT,1,RGB(255,255,255));
	g_DrawInfo.BigFont.CreateFont(-28,0,0,0,400,NULL,NULL,NULL,134,3,2,1,2,TEXT("黑体"));
	g_DrawInfo.LittleFont.CreateFont(-12,0,0,0,400,NULL,NULL,NULL,134,3,2,1,2,TEXT("宋体"));
}
