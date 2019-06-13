#include "Stdafx.h"
#include "Math.h"
#include "Resource.h"
#include "GoldControl.h"
#include ".\goldcontrol.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

#define CELL_WIDTH							23							//单元宽度
#define LESS_WIDTH							197							//最小宽度
#define CONTROL_HEIGHT						75							//控件高度
#define LEFT_WIDTH							65							//左边宽度
#define RIGHT_WIDTH							65							//右边宽度	
#define IMAGE_HEIGHT						39							//图片高度
#define IMAGE_WIDTH							23							//图片宽度

//按钮标识
#define  IDC_MAXSCORE						101							//最大下注
#define  IDC_MINSCORE						102							//最小下注
#define  IDC_OK								103							//确定按纽
#define  IDC_CANCEL							104							//取消按纽	

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGoldControl, CWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_SETCURSOR()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_MINSCORE,OnMinScore)
	ON_BN_CLICKED(IDC_MAXSCORE,OnMaxScore)
	ON_BN_CLICKED(IDC_OK,OnOKScore)
	ON_BN_CLICKED(IDC_CANCEL,OnCancelScore)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGoldControl::CGoldControl()
{
	//界面变量
	m_nWidth=0;
	m_nCellCount=0;

	//设置变量
	m_lMaxGold = 0L;
	m_lMinGold = 0L;
	ZeroMemory(m_lGoldCell,sizeof(m_lGoldCell));

	//加载资源
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_ImageLeft.SetLoadInfo(IDB_LEFT,hInstance);
	m_ImageMid.SetLoadInfo(IDB_MID,hInstance);
	m_ImageRight.SetLoadInfo(IDB_RIGHT,hInstance);
	m_ImageNumber.SetLoadInfo(IDB_NUMBER,hInstance);
}

//析构函数
CGoldControl::~CGoldControl()
{
}

//获取金币
LONG CGoldControl::GetGold()
{
	LONG lGold=0L;
	for (LONG i=0;i<CountArray(m_lGoldCell);i++) 
		lGold+=m_lGoldCell[i]*(LONG)pow(10L,i);
	return lGold;
}

//设置金币
void CGoldControl::SetGold(LONG lGold)
{
	//调整参数
	if (lGold>m_lMaxGold) lGold=m_lMaxGold;
	ZeroMemory(m_lGoldCell,sizeof(m_lGoldCell));

	//设置变量
	int nIndex=0;
	while (lGold>0L)
	{
		m_lGoldCell[nIndex++]=lGold%10L;
		lGold/=10L;
	}
	//调整界面
	RectifyControl();
	return;
}

//设置用户最小下注
void CGoldControl::SetMinGold(LONG lMinGold)
{
	//校验改变
	if (lMinGold>m_lMaxGold) return;
	if((lMinGold == m_lMinGold)||(lMinGold <0L)) return;
	ZeroMemory(m_lGoldCell,sizeof(m_lGoldCell));

	//设置变量
	m_lMinGold = lMinGold;
	//设置变量
	int nIndex=0;
	while (lMinGold>0L)
	{
		m_lGoldCell[nIndex++]=lMinGold%10L;
		lMinGold/=10L;
	}
	//调整界面
	RectifyControl();

	return;
}

//设置用户最高下注数
void CGoldControl::SetMaxGold(LONG lMaxGold)
{
	//效验改变
	if (m_lMaxGold==lMaxGold) return;

	//设置变量
	m_lMaxGold=lMaxGold;
	if (m_lMaxGold>9999999L) m_lMaxGold=9999999L;
	ZeroMemory(m_lGoldCell,sizeof(m_lGoldCell));

	//计算单元
	m_nCellCount=0;
	while (lMaxGold>0L)
	{
		lMaxGold/=10L;
		m_nCellCount++;
	}
	m_nCellCount=__min(CountArray(m_lGoldCell),__max(m_nCellCount,1));

	//设置界面
	m_nWidth=(m_nCellCount+(m_nCellCount-1)/3)*CELL_WIDTH+LEFT_WIDTH+RIGHT_WIDTH;

	MoveWindow(m_BasicPoint.x-m_nWidth,m_BasicPoint.y-CONTROL_HEIGHT,m_nWidth,CONTROL_HEIGHT);
	return;
}


//设置位置
void CGoldControl::SetBasicPoint(int nXPos, int nYPos)
{
	//设置变量
	m_BasicPoint.x=nXPos;
	m_BasicPoint.y=nYPos;

	//调整界面
	RectifyControl();

	return;
}

//调整控件
void CGoldControl::RectifyControl()
{
	MoveWindow(m_BasicPoint.x-m_nWidth,m_BasicPoint.y-CONTROL_HEIGHT,m_nWidth,CONTROL_HEIGHT);

	CRect rcButton;
	HDWP hDwp=BeginDeferWindowPos(32);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;

	//最大按钮
	m_btMaxScore.GetWindowRect(&rcButton);
	DeferWindowPos(hDwp,m_btMaxScore,NULL,6,7,0,0,uFlags);

	//最小按钮
	DeferWindowPos(hDwp,m_btMinScore,NULL,6,CONTROL_HEIGHT-rcButton.Height()-9,0,0,uFlags);

	//确定按纽
	DeferWindowPos(hDwp,m_btOK,NULL,m_nWidth-rcButton.Width()-6,7,0,0,uFlags);

	//加注按钮
	DeferWindowPos(hDwp,m_btCancel,NULL, m_nWidth-rcButton.Width()-6,CONTROL_HEIGHT-rcButton.Height()-9,0,0,uFlags);


	//结束移动
	EndDeferWindowPos(hDwp);
	return;
}

//绘画金币
void CGoldControl::DrawGoldCell(CDC * pDC, int nXPos, int nYPox, LONG lGold)
{
	CImageHandle ImageNumberHandle(&m_ImageNumber);
	if (lGold<10L) 
	{

		m_ImageNumber.BitBlt(pDC->m_hDC,nXPos,nYPox,IMAGE_WIDTH,IMAGE_HEIGHT,lGold*IMAGE_WIDTH,0,SRCCOPY);
	}
	else
	{
		m_ImageNumber.BitBlt(pDC->m_hDC,nXPos,nYPox,IMAGE_WIDTH,IMAGE_HEIGHT,10*IMAGE_WIDTH,0,SRCCOPY);
	}

	return;
}

//重画函数
void CGoldControl::OnPaint()
{
	if(GetGold()>=m_lMinGold)m_btOK.EnableWindow(TRUE);
	else m_btOK.EnableWindow(FALSE);

	CPaintDC dc(this);

	//获取位置
	CRect ClientRect;
	GetClientRect(&ClientRect);

	//建立缓冲图
	CDC BackFaceDC;
	CBitmap BufferBmp;
	BufferBmp.CreateCompatibleBitmap(&dc,ClientRect.Width(),ClientRect.Height());
	BackFaceDC.CreateCompatibleDC(&dc);
	BackFaceDC.SelectObject(&BufferBmp);

	//加载资源
	CImageHandle ImageMidHandle(&m_ImageMid);
	CImageHandle ImageLeftHandle(&m_ImageLeft);
	CImageHandle ImageRightHandle(&m_ImageRight);

	//绘画背景
	m_ImageLeft.BitBlt(BackFaceDC,0,0);
	m_ImageMid.StretchBlt(BackFaceDC,m_ImageLeft.GetWidth(),0,ClientRect.Width()-m_ImageLeft.GetWidth()-m_ImageRight.GetWidth(),m_ImageMid.GetHeight());
	m_ImageRight.BitBlt(BackFaceDC,ClientRect.Width()-m_ImageRight.GetWidth(),0);

	//绘画金币
	int nXExcursion=ClientRect.Width()-RIGHT_WIDTH;
	for (int i=0;i<m_nCellCount;i++)
	{
		//绘画逗号
		if ((i!=0)&&(i%3)==0)
		{
			nXExcursion-=IMAGE_WIDTH;
			DrawGoldCell(&BackFaceDC,nXExcursion,12,10);
		}

		//绘画数字
		nXExcursion-=IMAGE_WIDTH;
		DrawGoldCell(&BackFaceDC,nXExcursion,12,m_lGoldCell[i]);
	}

	//绘画界面
	dc.BitBlt(0,0,ClientRect.Width(),ClientRect.Height(),&BackFaceDC,0,0,SRCCOPY);

	return;
}

//左键按下消息
void CGoldControl::OnLButtonUp(UINT nFlags, CPoint point)
{
	//变量定义
	int nViewCellCount=(m_nCellCount-1)/3+m_nCellCount;

	//位置过虑
	int nHeadWidth=LEFT_WIDTH;
	if ((point.y<=5)||(point.y>=65)) return;
	if ((point.x<=nHeadWidth)||(point.x>=(CELL_WIDTH*nViewCellCount+nHeadWidth))) return;

	//按钮测试
	int iCellPos=(nViewCellCount-(point.x-nHeadWidth)/CELL_WIDTH)-1;
	if ((iCellPos==3)||(iCellPos==7)) return;
	if (iCellPos>3) iCellPos=iCellPos-(iCellPos-1)/3;

	//计算限制
	ASSERT((iCellPos>=0)&&(iCellPos<CountArray(m_lGoldCell)));
	if (m_lGoldCell[iCellPos]!=9L)
	{
		LONG lAddGold=(LONG)pow(10L,iCellPos);
		if ((GetGold()+lAddGold)>m_lMaxGold) return;
	}
	//if (m_lGoldCell[iCellPos]==9L)
	//{
	//	LONG lAddGold=(LONG)pow(10L,iCellPos)*9;
	//	if((GetGold()-lAddGold <m_lMinGold)) return;
	//}

	//设置变量
	m_lGoldCell[iCellPos]=(m_lGoldCell[iCellPos]+1)%10;
	int iCellPosTemp=(nViewCellCount-(point.x-nHeadWidth)/CELL_WIDTH)-1;

	if(iCellPosTemp>3)
		InvalidateRect(CRect(m_nWidth-(iCellPosTemp+1)*IMAGE_WIDTH -RIGHT_WIDTH,5,m_nWidth-iCellPosTemp*IMAGE_WIDTH-RIGHT_WIDTH,12+IMAGE_HEIGHT));
	else
		InvalidateRect(CRect(m_nWidth-(iCellPos+1)*IMAGE_WIDTH -RIGHT_WIDTH,5,m_nWidth-iCellPos*IMAGE_WIDTH-RIGHT_WIDTH,12+IMAGE_HEIGHT));
	
	return;
}

//右键按下消息
void CGoldControl::OnRButtonUp(UINT nFlags, CPoint point)
{
	//变量定义
	int nViewCellCount=(m_nCellCount-1)/3+m_nCellCount;

	//位置过虑
	int nHeadWidth=LEFT_WIDTH;
	if ((point.y<=5)||(point.y>=65)) return;
	if ((point.x<=nHeadWidth)||(point.x>=(CELL_WIDTH*nViewCellCount+nHeadWidth))) return;

	//按钮测试
	int iCellPos=(nViewCellCount-(point.x-nHeadWidth)/CELL_WIDTH)-1;
	if ((iCellPos==3)||(iCellPos==7)) return;
	if (iCellPos>3) iCellPos=iCellPos-(iCellPos-1)/3;

	//计算限制
	ASSERT((iCellPos>=0)&&(iCellPos<CountArray(m_lGoldCell)));
	if (m_lGoldCell[iCellPos]==0L)
	{
		LONG lAddGold=9L*(LONG)pow(10L,iCellPos);
		if ((GetGold()+lAddGold)>m_lMaxGold) return;
	}
	//if (m_lGoldCell[iCellPos]!=0L)
	//{
	//	LONG lAddGold=(LONG)pow(10L,iCellPos);
	//	if ((GetGold()-lAddGold)<m_lMinGold) return;

	//}

	//设置变量
	m_lGoldCell[iCellPos]=(m_lGoldCell[iCellPos]+9)%10;

	int iCellPosTemp =(nViewCellCount-(point.x-nHeadWidth)/CELL_WIDTH)-1;

	if(iCellPosTemp>3)
		InvalidateRect(CRect(m_nWidth-(iCellPosTemp+1)*IMAGE_WIDTH -RIGHT_WIDTH,5,m_nWidth-iCellPosTemp*IMAGE_WIDTH-RIGHT_WIDTH,12+IMAGE_HEIGHT));
	else
		InvalidateRect(CRect(m_nWidth-(iCellPos+1)*IMAGE_WIDTH -RIGHT_WIDTH,5,m_nWidth-iCellPos*IMAGE_WIDTH-RIGHT_WIDTH,12+IMAGE_HEIGHT));

	return;
}

//设置光标
BOOL CGoldControl::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT message)
{
	//获取鼠标
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(&point);

	//设置光标
	int nHeadWidth=LEFT_WIDTH;
	int nViewCellCount=(m_nCellCount-1)/3+m_nCellCount;
	if ((point.y>5)&&(point.y<65)&&(point.x>nHeadWidth)&&(point.x<(CELL_WIDTH*nViewCellCount+nHeadWidth)))
	{
		int iCellPos=(m_nCellCount-(point.x-nHeadWidth)/CELL_WIDTH);
		if ((iCellPos!=3)&&(iCellPos!=7))
		{
			SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_CARD_CUR)));
			return TRUE;
		}
	}

	return __super::OnSetCursor(pWnd, nHitTest, message);
}

//////////////////////////////////////////////////////////////////////////

int CGoldControl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	//创建控件
	CRect rcCreate(0,0,0,0);
	m_btMaxScore.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_MAXSCORE);
	m_btMinScore.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_MINSCORE);
	m_btOK.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_OK);
	m_btCancel.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_CANCEL);

	//加载位图
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_btMaxScore.SetButtonImage(IDB_BT_MAX,hInstance,false);
	m_btMinScore.SetButtonImage(IDB_BT_MIN,hInstance,false);
	m_btOK.SetButtonImage(IDB_BT_OK,hInstance,false);
	m_btCancel.SetButtonImage(IDB_BT_CANCEL,hInstance,false);
	return 0;
}

//最小下注
void CGoldControl::OnMinScore()
{
	SetGold(m_lMinGold);
	Invalidate(false);
	AfxGetMainWnd()->SendMessage(IDM_MIN_SCORE,WPARAM(m_lMinGold),LPARAM(m_lMinGold));
	return ;
}

//最大下注
void CGoldControl::OnMaxScore()
{
	SetGold(m_lMaxGold);
	Invalidate(false);
	AfxGetMainWnd()->SendMessage(IDM_MAX_SCORE,WPARAM(m_lMaxGold),LPARAM(m_lMaxGold));
	return ;
}

//确定消息
void CGoldControl::OnOKScore()
{
	LONG lGold = GetGold();
	AfxGetMainWnd()->SendMessage(IDM_OK_SCORE,WPARAM(lGold),LPARAM(lGold));

	return;
}

//取消消息
void CGoldControl::OnCancelScore()
{
	AfxGetMainWnd()->SendMessage(IDM_CANCEL_SCORE,0,0);

	return;
}

void CGoldControl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	//消息模拟
	//PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));

	__super::OnLButtonDown(nFlags, point);
}
