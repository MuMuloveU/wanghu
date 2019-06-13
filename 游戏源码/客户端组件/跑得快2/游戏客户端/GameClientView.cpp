#include "StdAfx.h"
#include "Resource.h"
#include "GameClientView.h"
#include ".\gameclientview.h"

#define	BASEOFFESTX	30	//牌与x轴边缘的间距
#define BASEOFFESTY	30	//牌与y轴边缘的间距
#define OUTOFFEST	20	//出的牌与本身的牌的间距
#define BTNOFFEST	30	//出牌按钮跳过的高度
#define OUTPISA		20	//准备出的牌的高度
#define BANKSIZE	20	//当前出牌者图标大小
#ifdef _TWICE
	#define PISAOFFEST	15	//每只牌之间的间距
#else
	#define PISAOFFEST	20	//每只牌之间的间距
#endif

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameView)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_OUT_CARD, OnOutCard)
	ON_BN_CLICKED(IDC_PASS_CARD, OnPassCard)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientView::CGameClientView(void) : CGameFrameView(true,24)
{
	ResetGameView();
	//加载资源
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_ImageDisk.SetLoadInfo(IDB_VIEW_BACK,AfxGetInstanceHandle());
	m_ImageCenter.SetLoadInfo(IDB_VIEW_CENTER,AfxGetInstanceHandle());
	m_ImageFocus.SetLoadInfo(IDB_BANKER,hInstance);
	m_ImagePass.SetLoadInfo(IDB_BOMB,hInstance);
	if (GetSystemMetrics(SM_CXSCREEN)>=1024)
		m_ImagePisa.SetLoadInfo(IDB_CARD,AfxGetInstanceHandle());
	else
		m_ImagePisa.SetLoadInfo(IDB_CARD_800,AfxGetInstanceHandle());
	//获取大小
	CImageHandle CardImageHandle(&m_ImagePisa);
	m_szPisa.cx=m_ImagePisa.GetWidth()/13;
	m_szPisa.cy=m_ImagePisa.GetHeight()/5;
	m_bStart=FALSE;
	return;
}

//析构函数
CGameClientView::~CGameClientView(void)
{
}

//建立消息
int CGameClientView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	m_ScoreView.Create(IDD_GAME_SCORE,this);

	CRect CreateRect(0,0,0,0);
	m_btStart.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_START);
	m_btOutCard.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_OUT_CARD);
	m_btPassCard.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_PASS_CARD);

	//设置按钮
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_btStart.SetButtonImage(IDB_START,hInstance,false);
	m_btOutCard.SetButtonImage(IDB_OUT_CARD,hInstance,false);
	m_btPassCard.SetButtonImage(IDB_PASS,hInstance,false);

	//读取配置
	m_bDeasilOrder=AfxGetApp()->GetProfileInt(TEXT("GameOption"),TEXT("DeasilOrder"),TRUE)?true:false;

	m_Timer.SetBitmap(IDB_TIMERBACK,IDB_TIMERARROW);
	m_Timer.SetSinkWindow(AfxGetMainWnd());
	m_Timer.Create(NULL,NULL,WS_CHILD,CreateRect,this,21);
	m_Timer.ShowWindow(SW_SHOW);

	return 0;
}

//用户顺序
void CGameClientView::SetUserOrder(bool bDeasilOrder)
{
	//状态判断
	if (m_bDeasilOrder==bDeasilOrder) return;

	//设置变量
	m_bDeasilOrder=bDeasilOrder;
	AfxGetApp()->WriteProfileInt(TEXT("GameOption"),TEXT("DeasilOrder"),m_bDeasilOrder?TRUE:FALSE);

	//设置界面
	CRect rcClient;
	GetClientRect(&rcClient);
	RectifyGameView(rcClient.Width(),rcClient.Height());

	//刷新界面
	UpdateGameView(NULL);

	return;
}

//重置界面
void CGameClientView::ResetGameView()
{
	for(int i=0;i<GAME_PLAYER;i++)
	{
		m_nPlayerLeftCount[i]=0;
		m_OutPisaArray[i].RemoveAll();
		m_bOutPass[i]=FALSE;
	}
	for(int i=0;i<PISACOUNT;i++)
		m_bOutSelect[i]=FALSE;
	m_nOwnerIndex=-1;
	m_nFocusIndex=-1;
	m_bReady=FALSE;
	m_PisaArray.RemoveAll();
	m_rtPisa.SetRectEmpty();
	//隐藏控件
	if (IsWindow(m_ScoreView.m_hWnd)) m_ScoreView.ShowWindow(SW_HIDE);
	return;
}

//调整控件
void CGameClientView::RectifyGameView(int nWidth, int nHeight)
{
	//移动按钮
	CRect rcButton;
	HDWP hDwp=BeginDeferWindowPos(3);
	m_btStart.GetWindowRect(&rcButton);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;
	DeferWindowPos(hDwp,m_btStart,NULL,(nWidth-rcButton.Width())/2,(nHeight-rcButton.Height())/2+120,0,0,uFlags);
	int nbtHeight=nHeight-50-m_szPisa.cy-20-rcButton.Height()-20;
	DeferWindowPos(hDwp,m_btOutCard,NULL,(nWidth-rcButton.Width())/2-60,nbtHeight,0,0,uFlags);
	DeferWindowPos(hDwp,m_btPassCard,NULL,(nWidth-rcButton.Width())/2+60,nbtHeight,0,0,uFlags);
	EndDeferWindowPos(hDwp);
	//积分视图
	CRect rcControl;
	m_ScoreView.GetWindowRect(&rcControl);
	m_ScoreView.SetWindowPos(NULL,(nWidth-rcControl.Width())/2,(nHeight-rcControl.Height()-100)/2,0,0,SWP_NOZORDER|SWP_NOSIZE);
	//m_Timer.SetBenchmarkPos(nWidth/2,nHeight/2);
	return;
}

void CGameClientView::SetTimerUp(BOOL bUp)
{
	if (bUp)
		m_Timer.SetTimerUp(m_nFocusIndex,15,m_nFocusIndex==2);
	else
		m_Timer.ShowTimer(FALSE);
}

//绘画界面
void CGameClientView::DrawGameView(CDC * pDC, int nWidth, int nHeight)
{
	CDC Mem;
    Mem.CreateCompatibleDC(pDC);
	CBitmap Bmp;
	Bmp.CreateCompatibleBitmap(pDC,nWidth,nHeight);
    CBitmap* pOldBmp=Mem.SelectObject(&Bmp);

	//绘画背景
	DrawViewImage(&Mem,m_ImageDisk,enMode_Spread);
	DrawViewImage(&Mem,m_ImageCenter,enMode_Centent);

	//加载位图
	CImageHandle HandleCard(&m_ImagePisa);

/*2：left:
	x=BASEOFFESTX;
	y=(nHeight-m_szPisa.cy)/2;//-getcount/2
	step+=y,20
0：right:
	x=nWidth-m_szPisa.cx-BASEOFFESTX;
	y=(nHeight-m_szPisa.cy)/2;//-getcount/2
	step+=y,20
3：top:
	x=(nWidth-m_szPisa.cx)/2;//-getcount/2
	y=BASEOFFESTY;
	step+=x,20
1：bottom:
	x=(nWidth-m_szPisa.cx)/2;//-getcount/2
	y=nWidth-m_szPisa.cy-BASEOFFESTY;
	step+=x,20

2：outleft:
	x=BASEOFFESTX+m_szPisa.cx+OUTOFFEST;
	y=(nHeight-m_szPisa.cy)/2;
	step+=x,20
0：outright:
	x=nWidth-m_szPisa.cx-BASEOFFESTX-OUTOFFEST;//-getcount
	y=(nHeight-m_szPisa.cy)/2;
	step+=x,20
3：outtop:
	x=(nWidth-m_szPisa.cx)/2;//-getcount/2
	y=BASEOFFESTY+m_szPisa.cy+OUTOFFEST;
	step+=x,20
1：outbottom:
	x=(nWidth-m_szPisa.cx)/2;//-getcount/2
	y=nWidth-m_szPisa.cy*2-BASEOFFESTY-20-OUTOFFEST-BTNOFFEST;
	step+=x,20*/
	//显示各个位置的牌
	//依次是普通牌的基准x,基准y,递增量在x还是y,基准偏移的总数乘数
	int BaseStartPoint[][4]={
		{(nWidth-m_szPisa.cx)/2,BASEOFFESTY,0,2},//top
		{BASEOFFESTX+10,(nHeight-m_szPisa.cy)/2,1,2},//left
		{(nWidth-m_szPisa.cx)/2,nHeight-m_szPisa.cy-BASEOFFESTY-20,0,2},//bottom
		{nWidth-m_szPisa.cx-BASEOFFESTX+10,(nHeight-m_szPisa.cy)/2,1,2}//right
	};

	//显示各个位置出的牌
	//依次是普通牌的基准x,基准y,递增量在x还是y,基准偏移的总数乘数
	int OutStartPoint[][4]={
		{(nWidth-m_szPisa.cx)/2,BASEOFFESTY+m_szPisa.cy+OUTOFFEST,0,2},//top
		{BASEOFFESTX+m_szPisa.cx+OUTOFFEST+10,(nHeight-m_szPisa.cy)/2-BANKSIZE,0,0},//left
		{(nWidth-m_szPisa.cx)/2,nWidth-m_szPisa.cy*2-BASEOFFESTY-20-OUTOFFEST-BTNOFFEST,0,2},//bottom
		{nWidth-m_szPisa.cx*2-BASEOFFESTX-OUTOFFEST+10,(nHeight-m_szPisa.cy)/2-BANKSIZE,0,1}//right
	};

	int TmpPoint[4];
	if (!m_bDeasilOrder)
	{
		//如果是逆时针方向出牌,把数组中的left和right的数组调转
		memcpy(&TmpPoint,&BaseStartPoint[1],sizeof(TmpPoint));
		memcpy(&BaseStartPoint[1],&BaseStartPoint[3],sizeof(TmpPoint));
		memcpy(&BaseStartPoint[3],&TmpPoint,sizeof(TmpPoint));
		memcpy(&TmpPoint,&OutStartPoint[1],sizeof(TmpPoint));
		memcpy(&OutStartPoint[1],&OutStartPoint[3],sizeof(TmpPoint));
		memcpy(&OutStartPoint[3],&TmpPoint,sizeof(TmpPoint));
	}

	CRect rtPisa;
	rtPisa.SetRectEmpty();
	int iStartPointX,iStartPointY;
	//保存自己的牌数
	m_nPlayerLeftCount[2]=(int)m_PisaArray.GetCount();
	for(int i=0;i<GAME_PLAYER;i++)
	{
		//确定牌的起始坐标
		if (BaseStartPoint[i][2]==0)
		{
			if (BaseStartPoint[i][3]!=0)
				iStartPointX=BaseStartPoint[i][0]-PISAOFFEST*(m_nPlayerLeftCount[i]-1)/BaseStartPoint[i][3];
			else
				iStartPointX=BaseStartPoint[i][0];
			iStartPointY=BaseStartPoint[i][1];
		}
		else
		{
			iStartPointX=BaseStartPoint[i][0];
			if (BaseStartPoint[i][3]!=0)
				iStartPointY=BaseStartPoint[i][1]-PISAOFFEST*(m_nPlayerLeftCount[i]-1)/BaseStartPoint[i][3];
			else
				iStartPointY=BaseStartPoint[i][1];
		}
		//绘画牌
		if (i==2)
		{
			//自己的牌
			//显示自己的牌
			for(int j=0;j<m_PisaArray.GetCount();j++)
			{
				struct tagPisa pisa=m_PisaArray.GetAt(j);
				//大王、小王显示
				if (pisa.row==14 && pisa.line==1)
				{
					pisa.row=1;
					pisa.line=5;
				}
				if (pisa.row==14 && pisa.line==2)
				{
					pisa.row=2;
					pisa.line=5;
				}
				//绘画扑克
				m_ImagePisa.BitBlt(Mem,iStartPointX+j*PISAOFFEST,iStartPointY-m_bOutSelect[j]*OUTPISA,m_szPisa.cx,m_szPisa.cy,(pisa.row-1)*m_szPisa.cx,(pisa.line-1)*m_szPisa.cy,SRCCOPY);
			}
			//if (m_nFocusIndex==2)
			//这里改为任何时候都可以上下牌
			rtPisa.SetRect(iStartPointX,iStartPointY-OUTPISA,iStartPointX+(m_nPlayerLeftCount[i]-1)*PISAOFFEST+m_szPisa.cx,iStartPointY+m_szPisa.cy);
		}
		else
		{
			if (BaseStartPoint[i][2]==0)
			{
				for(int j=0;j<m_nPlayerLeftCount[i];j++)
					m_ImagePisa.BitBlt(Mem,iStartPointX+j*PISAOFFEST,iStartPointY,m_szPisa.cx,m_szPisa.cy,2*m_szPisa.cx,4*m_szPisa.cy,SRCCOPY);
			}
			else
			{
				for(int j=0;j<m_nPlayerLeftCount[i];j++)
					m_ImagePisa.BitBlt(Mem,iStartPointX,iStartPointY+j*PISAOFFEST,m_szPisa.cx,m_szPisa.cy,2*m_szPisa.cx,4*m_szPisa.cy,SRCCOPY);
			}
		}
		//确定已出牌的起始坐标
		if (OutStartPoint[i][2]==0)
		{
			if (OutStartPoint[i][3]!=0)
				iStartPointX=OutStartPoint[i][0]-PISAOFFEST*((int)m_OutPisaArray[i].GetCount()-1)/OutStartPoint[i][3];
			else
				iStartPointX=OutStartPoint[i][0];
			iStartPointY=OutStartPoint[i][1];
		}
		else
		{
			iStartPointX=OutStartPoint[i][0];
			if (OutStartPoint[i][3]!=0)
				iStartPointY=OutStartPoint[i][1]-PISAOFFEST*((int)m_OutPisaArray[i].GetCount()-1)/OutStartPoint[i][3];
			else
				iStartPointY=OutStartPoint[i][1];
		}
		if (m_bOutPass[i])
		{
			//Pass
			CImageHandle ImageHandle(&m_ImagePass);
			iStartPointX-=m_ImagePass.GetWidth()/2;
			iStartPointY-=m_ImagePass.GetHeight()/2;
			m_ImagePass.AlphaDrawImage(&Mem,iStartPointX,iStartPointY,RGB(255,0,255));
		}
		else
		{
			//绘画已出的牌
			for(int j=0;j<m_OutPisaArray[i].GetCount();j++)
			{
				struct tagPisa pisa=m_OutPisaArray[i].GetAt(j);
				//大王、小王显示
				if (pisa.row==14 && pisa.line==1)
				{
					pisa.row=1;
					pisa.line=5;
				}
				if (pisa.row==14 && pisa.line==2)
				{
					pisa.row=2;
					pisa.line=5;
				}
				m_ImagePisa.BitBlt(Mem,iStartPointX+j*PISAOFFEST,iStartPointY,m_szPisa.cx,m_szPisa.cy,(pisa.row-1)*m_szPisa.cx,(pisa.line-1)*m_szPisa.cy,SRCCOPY);
			}
		}
	}
	m_rtPisa=rtPisa;

	//绘画焦点及判断按钮
	if (m_bReady)
	{
		switch(m_nFocusIndex)
		{
			case 2://自己
				if (!m_btOutCard.IsWindowVisible()) m_btOutCard.ShowWindow(SW_SHOW);
				if (!m_btPassCard.IsWindowVisible()) m_btPassCard.ShowWindow(SW_SHOW);
				break;
			case 0://上方
			case 1://左方
			case 3://右方
			{
				if (m_btOutCard.IsWindowVisible()) m_btOutCard.ShowWindow(SW_HIDE);
				if (m_btPassCard.IsWindowVisible()) m_btPassCard.ShowWindow(SW_HIDE);
				CImageHandle ImageHandle(&m_ImageFocus);
								//	top		left				bottom	right
				int nOffWidth[]=	{0,		-150,				0,		150};
				int nOffHeight[]=	{-80,	m_szPisa.cy/2+10,	0,		m_szPisa.cy/2+10};
								//	top		right				bottom	left
				int nOffWidth1[]=	{0,		150,				0,		-150};
				int nOffHeight1[]=	{-80,	m_szPisa.cy/2+10,	0,		m_szPisa.cy/2+10};
				if (m_bDeasilOrder)
					m_ImageFocus.AlphaDrawImage(&Mem,(nWidth-m_ImageFocus.GetWidth())/2+nOffWidth[m_nFocusIndex]+50,(nHeight-m_ImageFocus.GetHeight())/2+nOffHeight[m_nFocusIndex],RGB(255,0,255));
				else
					m_ImageFocus.AlphaDrawImage(&Mem,(nWidth-m_ImageFocus.GetWidth())/2+nOffWidth1[m_nFocusIndex]+50,(nHeight-m_ImageFocus.GetHeight())/2+nOffHeight1[m_nFocusIndex],RGB(255,0,255));
			}
				break;
		}
		if (m_btStart.IsWindowVisible()) m_btStart.ShowWindow(SW_HIDE);
		if (m_ScoreView.IsWindowVisible()) m_ScoreView.ShowWindow(SW_HIDE);
	}
	else
	{
		if (!m_btStart.IsWindowVisible()) m_btStart.ShowWindow(SW_SHOW);
		if (m_btOutCard.IsWindowVisible()) m_btOutCard.ShowWindow(SW_HIDE);
		if (m_btPassCard.IsWindowVisible()) m_btPassCard.ShowWindow(SW_HIDE);
	}

	//绘画用户
	TCHAR szBuffer[32];
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		int index;
		if (m_nOwnerIndex==-1)
			//还没有开始的时候，直接按原始排列
			index=i;
		else
			//开始之后，取本地方式的排列数据
			index=(i+GAME_PLAYER-m_nOwnerIndex)%GAME_PLAYER;
		const tagUserData *pUserData=GetUserInfo(index);

						//	top					left				bottom			right
		int nOffWidth[]=	{nWidth/2-PISAOFFEST*7-PISAOFFEST/2-80,70,nWidth/2+PISAOFFEST*6+PISAOFFEST/2+m_szPisa.cx+20,nWidth-70};
		int nOffHeight[]=	{50,nHeight/2+PISAOFFEST*6+PISAOFFEST/2+m_szPisa.cy/2+10,nHeight-100,nHeight/2-PISAOFFEST*6-PISAOFFEST/2-m_szPisa.cy/2-40};
						//	top					right				bottom			left
		int nOffWidth1[]=	{nWidth/2-PISAOFFEST*7-PISAOFFEST/2-100,
												nWidth-150,			nWidth/2+PISAOFFEST*6+PISAOFFEST/2+m_szPisa.cx+20,
																					150};
		int nOffHeight1[]=	{150,				nHeight/2-PISAOFFEST*6-PISAOFFEST/2-m_szPisa.cy/2-40,
																	nHeight-160,	nHeight/2+PISAOFFEST*6+PISAOFFEST/2+m_szPisa.cy/2+20};
		//绘画用户
		if (pUserData!=NULL)
		{
			//用户名字
			Mem.SetTextColor(RGB(220,220,220));
			Mem.SetBkMode(TRANSPARENT);
			Mem.SetTextAlign(TA_LEFT);
			if (m_bDeasilOrder)
				Mem.TextOut(nOffWidth[index],nOffHeight[index],pUserData->szName,lstrlen(pUserData->szName));
			else
				Mem.TextOut(nOffWidth1[index],nOffHeight1[index],pUserData->szName,lstrlen(pUserData->szName));

			//剩余扑克
			//if (pUserData->cbUserStatus>=GS_WK_PLAYING)
			if (m_nPlayerLeftCount[index]!=0)
			{
				_snprintf(szBuffer,sizeof(szBuffer),TEXT("（%ld 张）"),m_nPlayerLeftCount[index]);
				if (m_bDeasilOrder)
					Mem.TextOut(nOffWidth[index]-5,nOffHeight[index]+18,szBuffer,lstrlen(szBuffer));
				else
					Mem.TextOut(nOffWidth1[index]-5,nOffHeight1[index]+18,szBuffer,lstrlen(szBuffer));
			}

			//其他信息
			if (pUserData->cbUserStatus==US_READY)
			{
				if (m_bDeasilOrder)
					DrawUserReady(&Mem,nOffWidth[index]-50,nOffHeight[index]+5);
				else
					DrawUserReady(&Mem,nOffWidth1[index]-50,nOffHeight1[index]+5);
			}
		/*	if (m_bDeasilOrder)
				DrawUserFace(&Mem,pUserData->wFaceID,nOffWidth[index]-36,nOffHeight[index],pUserData->cbUserStatus==US_OFFLINE);
			else
				DrawUserFace(&Mem,pUserData->wFaceID,nOffWidth1[index]-36,nOffHeight1[index],pUserData->cbUserStatus==US_OFFLINE);*/

						if (pUserData->cbUserStatus==US_READY) DrawUserReady(pDC,m_ptReady[i].x,m_ptReady[i].y);
			DrawUserFace(pDC,pUserData,m_ptFace[i].x,m_ptFace[i].y,pUserData->cbUserStatus==US_OFFLINE);
		}
	}
	pDC->BitBlt(0,0,nWidth,nHeight,&Mem,0,0,SRCCOPY);

	Mem.SelectObject(pOldBmp);
	Bmp.DeleteObject();
	Mem.DeleteDC();
	return;
}

void CGameClientView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_bReady)
	{
		if (!m_rtPisa.IsRectEmpty() && m_rtPisa.PtInRect(point))
		{
			//按下对应的牌
			int count=(point.x-m_rtPisa.left)/PISAOFFEST;
			if (count>=m_PisaArray.GetCount()) count=(int)m_PisaArray.GetCount()-1;
			m_bOutSelect[count]=!m_bOutSelect[count];
			UpdateGameView(&m_rtPisa);
		}
/*		//FIXME
		else
		{
			m_nFocusIndex=(m_nFocusIndex+1)%GAME_PLAYER;
			UpdateGameView(NULL);
		}*/
	}

	CGameFrameView::OnLButtonDown(nFlags, point);
}

//开始按钮
void CGameClientView::OnStart()
{
	m_bReady=TRUE;
	AfxGetMainWnd()->SendMessage(WM_USERREADY);
	UpdateGameView(NULL);
//FIXME,temp for use
/*	struct tagPisa Pisa;
	for(int i=0;i<PISACOUNT;i++)
	{
		Pisa.line=rand()%4+1;
		Pisa.row=rand()%PISACOUNT+1;
		m_PisaArray.Add(Pisa);
		m_OutPisaArray[0].Add(Pisa);
		m_OutPisaArray[1].Add(Pisa);
		m_OutPisaArray[2].Add(Pisa);
		m_OutPisaArray[3].Add(Pisa);
	}
	m_nPlayerLeftCount[0]=PISACOUNT;
	m_nPlayerLeftCount[1]=PISACOUNT;
	m_nPlayerLeftCount[2]=PISACOUNT;
	m_nPlayerLeftCount[3]=PISACOUNT;
	m_nFocusIndex=0;
	UpdateGameView(NULL);*/
}

//出牌按钮
void CGameClientView::OnOutCard()
{
	//按下“出牌”按钮
	struct tagSetOutPisa out;
	memset(&out,0,sizeof(tagSetOutPisa));
	//out.nChairId=(m_nOwnerIndex+GAME_PLAYER-2)%GAME_PLAYER;//在服务器上的相对偏移
	int count=0;
	for(int i=0;i<PISACOUNT;i++)
	{
		if (m_bOutSelect[i])
			out.tOutPisa[count++]=m_PisaArray.GetAt(i);
	}
	if (count>0)
	{
		out.nCount=count;
		AfxGetMainWnd()->SendMessage(WM_OUTPISA,(WPARAM)&out);
	}
}

//放弃按钮
void CGameClientView::OnPassCard()
{
	struct tagSetOutPisa out;
	memset(&out,0,sizeof(tagSetOutPisa));
	//out.nChairId=(m_nOwnerIndex+GAME_PLAYER-2)%GAME_PLAYER;//在服务器上的相对偏移
	AfxGetMainWnd()->SendMessage(WM_OUTPISA,(WPARAM)&out);
}

void CGameClientView::SetTimerPause(BOOL bPause)
{
	m_Timer.SetPause(bPause);
}

int CGameClientView::SetHeiTao3Out()
{
	//恢复其他牌的显示
	for(int i=0;i<PISACOUNT;i++)
		m_bOutSelect[i]=FALSE;
	for(int i=0;i<m_PisaArray.GetCount();i++)
	{
		const tagPisa pisa=m_PisaArray.GetAt(i);
		//如果是黑桃3
		if (pisa.line==4 && pisa.row==3)
		{
			m_bOutSelect[i]=TRUE;
			UpdateGameView(NULL);
			return i;
		}
	}
	UpdateGameView(NULL);
	return -1;
}