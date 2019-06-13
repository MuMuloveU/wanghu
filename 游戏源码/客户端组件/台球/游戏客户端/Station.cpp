#include "StdAfx.h"
#include "Station.h"
#include "Math.h"
#include "GameClientDlg.h"
#include "GameClientView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CStation, CWnd)
	ON_WM_PAINT()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
END_MESSAGE_MAP()

#define TY_TILE			0x01			//平铺
#define TY_HCENTER		0x02			//平居中
#define TY_VCENTER		0x04			//立居中
#define TY_TOP			0x00			//上对齐
#define TY_LEFT			0x00			//左对齐
#define TY_RIGHT		0x08			//右对齐
#define TY_BOTTON		0x20			//下对齐
#define TY_TRAN			0x40			//透明处理
extern struct DrawInfoStruct			g_DrawInfo;
//构造函数
CStation::CStation()
{
	m_iPower=0;
	m_iBaseMoney=0L;
	m_iXBallPos=0.0;
	m_iYBallPos=0.0;
	m_iXAnglePos=41;
	m_iYAnglePos=0.0;
	iPos=0;
	m_iTargetBall[0]=255;
	m_iTargetBall[1]=255;
	m_bAddMode=true;
	m_bPowerTimer=false;
	m_IsTrain=TRUE;
	::memset(m_iPoint,0,sizeof(m_iPoint));
	m_Cursor=AfxGetApp()->LoadCursor(IDC_CURSOR);
	m_NormalCursor=AfxGetApp()->LoadStandardCursor(IDC_ARROW);
}

//重画函数
void CStation::OnPaint() 
{
	CPaintDC dc(this); 

	//建立缓冲
	CDC BufferDC;
	CRect ClientRect;
	CFont LittleFont, * pOldFont=NULL;
	CBitmap BufferBmp, * pOldBmp=NULL;
	GetClientRect(&ClientRect);
	BufferBmp.CreateCompatibleBitmap(&dc,ClientRect.Width(),ClientRect.Height());
	BufferDC.CreateCompatibleDC(&dc);
	pOldBmp=BufferDC.SelectObject(&BufferBmp);
	LittleFont.CreateFont(-12,0,0,0,400,NULL,NULL,NULL,134,3,2,1,2,TEXT("宋体"));
	pOldFont=BufferDC.SelectObject(&LittleFont);

	//绘画背景
	DrawBmp(&BufferDC,0,0,IDB_ST_BACK,TY_TILE,&ClientRect);
	DrawBmp(&BufferDC,230,0,IDB_USERINFO,TY_TRAN,&ClientRect);
	DrawBmp(&BufferDC,620,0,IDB_USERINFO,TY_TRAN,&ClientRect);
	DrawBmp(&BufferDC,0,5,IDB_ST_LOGO,TY_HCENTER|TY_VCENTER,&ClientRect);
	DrawBmp(&BufferDC,-70,0,IDB_ST_BALL_CTL,TY_VCENTER|TY_RIGHT,&ClientRect);
	DrawBmp(&BufferDC,(int)m_iXBallPos-156,30-(int)m_iYBallPos,IDB_ST_BALL_POINT,TY_TRAN|TY_RIGHT,&ClientRect);
	//绘画用户
	TCHAR szBuffer[20];
	BufferDC.SetBkMode(TRANSPARENT);
	BufferDC.SetTextColor(RGB(0,0,0));
	BufferDC.SetTextAlign(TA_LEFT|TA_TOP);
	BufferDC.TextOut(ClientRect.Width()/2-220,15,TEXT("目标球"));
	BufferDC.TextOut(ClientRect.Width()/2-220,36,TEXT("  得分"));
	const tagUserData *pUserData = g_DrawInfo.pPlayView->GetUserInfo(0);
	if(pUserData)
	{
		BufferDC.TextOut(ClientRect.Width()/2-260,STATION_WND_HIGHT-27,pUserData->szName);
		g_DrawInfo.pPlayView->DrawUserFace(&BufferDC,
								pUserData->wFaceID,
								ClientRect.Width()/2-260,
								8,
								pUserData->cbUserStatus==US_OFFLINE);
		if (m_iTargetBall[0]!=255) 
			DrawBmp(&BufferDC,ClientRect.Width()/2-165,8,IDB_BALL_A,TY_TRAN,&ClientRect,m_iTargetBall[0]*((int)(BALL_RADII_A)*2+1),0,(int)(BALL_RADII_A)*2,BALL_BACK_COLOR);
		BufferDC.SetTextAlign(TA_LEFT|TA_TOP);
		sprintf(szBuffer,TEXT("%d 分"),m_iPoint[0]);
		BufferDC.SetTextColor(RGB(0,0,0));
		BufferDC.TextOut(ClientRect.Width()/2-170,36,szBuffer);
		WORD wUserTimer = g_DrawInfo.pPlayView->GetUserTimer(0);
		if(wUserTimer) 
		{
			g_DrawInfo.pPlayView->DrawUserTimer(&BufferDC,
								ClientRect.Width()/2-138,
								10,
								wUserTimer);
		}
		if(pUserData->cbUserStatus==US_READY&&!m_IsTrain) 
		{
			g_DrawInfo.pPlayView->DrawUserReady(&BufferDC,
								ClientRect.Width()/2-145,
								13);
		}
	}
	BufferDC.SetTextAlign(TA_LEFT|TA_TOP);
	BufferDC.TextOut(ClientRect.Width()/2+180,15,TEXT("目标球"));
	BufferDC.TextOut(ClientRect.Width()/2+180,36,TEXT("  得分"));
	pUserData = g_DrawInfo.pPlayView->GetUserInfo(1);
	if(pUserData)
	{
		BufferDC.TextOut(ClientRect.Width()/2+130,STATION_WND_HIGHT-27,pUserData->szName);
		g_DrawInfo.pPlayView->DrawUserFace(&BufferDC,
								pUserData->wFaceID,
								ClientRect.Width()/2+130,
								8,
								pUserData->cbUserStatus==US_OFFLINE);
		if (m_iTargetBall[1]!=255) 
			DrawBmp(&BufferDC,ClientRect.Width()/2+220,8,IDB_BALL_A,TY_TRAN,&ClientRect,m_iTargetBall[1]*((int)(BALL_RADII_A)*2+1),0,(int)(BALL_RADII_A)*2,BALL_BACK_COLOR);
		BufferDC.SetTextAlign(TA_LEFT|TA_TOP);
		sprintf(szBuffer,TEXT("%d 分"),m_iPoint[1]);
		BufferDC.SetTextColor(RGB(0,0,0));
		BufferDC.TextOut(ClientRect.Width()/2+228,36,szBuffer);
		WORD wUserTimer = g_DrawInfo.pPlayView->GetUserTimer(1);
		if(wUserTimer) 
		{
			g_DrawInfo.pPlayView->DrawUserTimer(&BufferDC,
								ClientRect.Width()/2+250,
								10,
								wUserTimer);
		}
		if(pUserData->cbUserStatus==US_READY&&!m_IsTrain) 
		{
			g_DrawInfo.pPlayView->DrawUserReady(&BufferDC,
								ClientRect.Width()/2+258,
								13);
		}
	}
	//画力量
	BufferDC.MoveTo(ClientRect.Width()-131,56);
	BufferDC.LineTo(ClientRect.Width()-131+(int)m_iXAnglePos,(int)m_iYAnglePos+56);
	BufferDC.SetTextColor(RGB(255,0,255));
	CString strAngle;
	strAngle.Format("%d 度",(int)((acos(m_iXAnglePos/41)*180)/3.14));
	BufferDC.TextOut(ClientRect.Width()-111,10,strAngle);
	BufferDC.FillSolidRect(ClientRect.Width()-182,58,97,5,RGB(0,0,0));
	BufferDC.FillSolidRect(ClientRect.Width()-182,58,(int)(g_DrawInfo.m_iPowerPos*100),5,RGB(255,100,25));
	BufferDC.FillSolidRect(ClientRect.Width()-182,58,iPos,5,RGB(255,0,255));
	//画主界面
	dc.BitBlt(0,0,ClientRect.Width(),ClientRect.Height(),&BufferDC,0,0,SRCCOPY);
	//清理
	BufferDC.SelectObject(pOldBmp);
	BufferDC.SelectObject(pOldFont);
	return;
}

//设置光标
BOOL CStation::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (g_DrawInfo.bStation==ME_HIT_BALL)
	{
		CPoint	MousePoint;
		CRect	ClientRect;
		GetClientRect(&ClientRect);
		::GetCursorPos(&MousePoint);
		ScreenToClient(&MousePoint);
		double iXMid=(double)(MousePoint.x-(ClientRect.Width()-164));
		double iYMid=(double)(MousePoint.y-32);
		if ((iXMid*iXMid+iYMid*iYMid)<=int(PIC_BALL_RADII*PIC_BALL_RADII)) 
		{
			SetCursor(m_Cursor);
			return TRUE;
		}
		iXMid=(double)(MousePoint.x-(ClientRect.Width()-131));
		iYMid=(double)(MousePoint.y-56);
		if (iXMid>0&&iYMid<0&&(iXMid*iXMid+iYMid*iYMid)<41*40) 
		{
			SetCursor(m_Cursor);
			return TRUE;
		}
	}

	SetCursor(m_NormalCursor);
	return TRUE;
}

//绘画图片
void CStation::DrawBmp(CDC * pDC, int x, int y, UINT uBmpID, UINT uType, CRect * pRect, int xSrc, int ySrc, int iWidth, COLORREF rcTranColor)
{
	CSkinImage	  m_BmpImage;
	if(!m_BmpImage.IsNull())
		m_BmpImage.Detach();
	HINSTANCE hInstance = ::AfxGetInstanceHandle();
	m_BmpImage.LoadFromResource(hInstance,uBmpID);
	int m_BmpWidth=m_BmpImage.GetWidth();
	int m_BmpHeight=m_BmpImage.GetHeight();
	if (uType==TY_TILE)
	{
		for (y=0;y<pRect->Height();y+=m_BmpHeight)
		{
			for (x=0;x<pRect->Width();x+=m_BmpWidth)
				m_BmpImage.BitBlt(pDC->m_hDC,x,y,m_BmpWidth,m_BmpHeight,xSrc,ySrc,SRCCOPY);
		}
	}
	else
	{
		if (uType&TY_HCENTER) x=(pRect->Width()-m_BmpWidth)/2;
		else if (uType&TY_RIGHT) x=pRect->Width()-m_BmpWidth+x;
		if (uType&TY_VCENTER) y=(pRect->Height()-m_BmpHeight)/2;
		else if (uType&TY_BOTTON) y=pRect->Height()-m_BmpHeight+y;
		if (uType&TY_TRAN)
		{
			m_BmpImage.AlphaDrawImage(pDC,x,y,(iWidth==0)?m_BmpWidth:iWidth,m_BmpHeight,
					xSrc,ySrc,rcTranColor);
		}
		else
			m_BmpImage.BitBlt(pDC->m_hDC,x,y,m_BmpWidth,m_BmpHeight,xSrc,ySrc,SRCCOPY);
	}
	return;
}

//设置击球点
void CStation::SetBallPos(double iXBallPos, double iYBallPos,bool bIsAngle)
{
	if(!bIsAngle)
	{
		m_iXBallPos=iXBallPos;
		m_iYBallPos=iYBallPos;
	}
	else
	{
		g_DrawInfo.Degree=(int)(asin(m_iYAnglePos/41));
		m_iXAnglePos=iXBallPos;
		m_iYAnglePos=iYBallPos;
	}
	Invalidate(FALSE);
	return;
}

//鼠标右键消息
void CStation::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (g_DrawInfo.bStation==ME_HIT_BALL)
	{
		CRect	ClientRect;
		GetClientRect(&ClientRect);
		double iXMid=(double)(point.x-(ClientRect.Width()-161));
		double iYMid=(double)(32-point.y);
		if ((iXMid*iXMid+iYMid*iYMid)<=(int)(PIC_BALL_RADII*PIC_BALL_RADII))
		{
			SetBallPos(iXMid,iYMid);
			return;
		}
		iXMid=(double)(point.x-(ClientRect.Width()-131));
		iYMid=(double)(point.y-56);
		if (iXMid>0&&iYMid<0&&(iXMid*iXMid+iYMid*iYMid)<41*41)
		{
			double m_AngleXPos=41*sqrt(iXMid*iXMid/(iXMid*iXMid+iYMid*iYMid));
			double m_AngelYPos=-41*sqrt(iYMid*iYMid/(iXMid*iXMid+iYMid*iYMid));
			SetBallPos(m_AngleXPos,m_AngelYPos,true);
		}
		if(point.x>ClientRect.Width()-182&&point.x<ClientRect.Width()-82&&point.y>58&&point.y<63)
		{
			g_DrawInfo.m_iPowerPos=(point.x-ClientRect.Width()+182)/100.0;
			Invalidate(FALSE);
		}

	}
	return;
}

//设置力量
void CStation::SetPower(int iPower)
{
	m_iPower=iPower;
	g_DrawInfo.iPower=iPower;
	iPos=__max(0,__min(m_iPower,100))*97/100;
	Invalidate(FALSE);
	/*
	//画力量
	CRect ClientRect;
	GetClientRect(&ClientRect);
	CDC * pDC=GetDC();
	pDC->FillSolidRect(ClientRect.Width()-182,58,97,5,RGB(0,0,0));
	pDC->FillSolidRect(ClientRect.Width()-182,58,(int)(g_DrawInfo.m_iPowerPos*100),5,RGB(255,100,25));
	pDC->FillSolidRect(ClientRect.Width()-182,58,iPos,6,RGB(255,0,255));
	ReleaseDC(pDC);*/

	return;
}

//定时器函数 
void CStation::OnTimer(UINT nIDEvent)
{
	if (nIDEvent==ID_TIMER)
	{
		m_bAddMode?m_iPower+=5:m_iPower-=5;
		if (m_iPower>=120) m_bAddMode=false;
		if (m_iPower<=-10)  m_bAddMode=true;
		SetPower(m_iPower);
	}
	return;
}

//开始力量
void CStation::BeginPower()
{
	if (m_bPowerTimer==false)
	{
		m_bPowerTimer=true;
		SetTimer(ID_TIMER,40,NULL);
	}
	return;
}

//停止力量
void CStation::EndPower()
{
	if (m_bPowerTimer)	
	{
		m_bPowerTimer=false;
		KillTimer(ID_TIMER);
	}
	return;
}

//设置分数
void CStation::SetUserPoint(int iViewStation, int Point, bool bAdd)
{
	if ((iViewStation==0)||(iViewStation==1))
	{
		if (bAdd) m_iPoint[iViewStation]+=Point;
		else m_iPoint[iViewStation]=Point;
		Invalidate(FALSE);
	}
	return;
}

//设置目标球
void CStation::SetTargertBall(int iViewStation, BYTE bColor)
{
	if ((iViewStation==0)||(iViewStation==1))
	{
		m_iTargetBall[iViewStation]=bColor;
		m_iTargetBall[(iViewStation+1)%2]=255;
		Invalidate(FALSE);
	}
	return;
}