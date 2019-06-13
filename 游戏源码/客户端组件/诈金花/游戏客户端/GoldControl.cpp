#include "Stdafx.h"
#include "Math.h"
#include "Resource.h"
#include "GoldControl.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

#define CELL_WIDTH							23							//单元宽度
#define LESS_WIDTH							70							//最小宽度
#define SPACE_WIDTH							12							//空袭宽度
#define BUTTON_WIDTH						0							//功能区域
#define CONTROL_HEIGHT						50							//控件高度

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGoldControl, CWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGoldControl::CGoldControl()
{
	m_nCellCount=0;
	m_lAllGold=0;

	//界面变量
	m_nWidth=0;
	m_lGoldCount[0]=10;
	m_lGoldCount[1]=20;
	m_lGoldCount[2]=50;
	m_AppendWidth=0;

	//设置变量
	m_lMaxGold=0L;
	m_lMinGold=0L;
	memset(m_lGoldCell,0,sizeof(m_lGoldCell));

	//加载资源
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_ImageMoney.SetLoadInfo(IDB_GOLD,hInstance);
	m_ImageBack.SetLoadInfo(IDB_ADD_INFO,hInstance);
	m_ImageNumber.SetLoadInfo(IDB_NUMBER,hInstance);
}

//析构函数
CGoldControl::~CGoldControl()
{
}

//设置筹码
void CGoldControl::SetGoldCount(LONG lCellSocre)
{
	if(lCellSocre>0L)
	{
		for (int i=0;i<3;i++)
		{
			m_lGoldCount[i]*=(lCellSocre/10);
		}
	}
	else 
	{
		m_lGoldCount[0]=10;
		m_lGoldCount[1]=20;
		m_lGoldCount[2]=50;
	}
	return;
}

//设置金币
void CGoldControl::SetGold(LONG lGold)
{
	//调整参数
	if (lGold>m_lMaxGold)m_lMaxGold= lGold;
	memset(m_lGoldCell,0,sizeof(m_lGoldCell));
	m_lAllGold=lGold;

	//设置变量
	int nIndex=0;
	while (lGold>0L)
	{
		m_lGoldCell[nIndex++]=lGold%10L;
		lGold/=10L;
	}

	//重画界面
	Invalidate(FALSE);

	return;
}

//最少单元
void CGoldControl::SetMinGold(LONG lMinGold)
{
	ASSERT(lMinGold%10==0);

	//设置变量
	m_lAllGold=lMinGold;
	m_lMinGold=lMinGold;

	//重画界面
	Invalidate(FALSE);
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
	memset(m_lGoldCell,0,sizeof(m_lGoldCell));

	//计算单元
	m_nCellCount=0;
	while (lMaxGold>0L)
	{
		lMaxGold/=10L;
		m_nCellCount++;
	}
	m_nCellCount=__min(CountArray(m_lGoldCell),__max(m_nCellCount,1));

	//重画界面
	//Invalidate(FALSE);

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
	CImageHandle ImageBack(&m_ImageBack);
	MoveWindow(m_BasicPoint.x,m_BasicPoint.y,m_ImageBack.GetWidth(),m_ImageBack.GetHeight());

	return;
}

//重画函数
void CGoldControl::OnPaint()
{
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
	CImageHandle ImageHandle1(&m_ImageBack);
	CImageHandle ImageHandle2(&m_ImageMoney);
	CImageHandle ImageHandle3(&m_ImageNumber);

	//绘画背景
	m_ImageBack.BitBlt(BackFaceDC,0,0);

	//绘画筹码
	for (int i=0;i<3;i++)
	{
		int iX=0;
		if(m_lGoldCount[0] == 100)iX=3;
		else if(m_lGoldCount[0] == 1000)iX=6;
		else if(m_lGoldCount[0] == 10000)iX=9;
		else if(m_lGoldCount[0] == 100000)iX=12;
		m_ImageMoney.AlphaDrawImage(&BackFaceDC,(i)*(m_ImageMoney.GetWidth()/15+5)+17,m_ImageMoney.GetHeight()/2+12,
			m_ImageMoney.GetWidth()/15,m_ImageMoney.GetHeight(),m_ImageMoney.GetWidth()/15*(i+iX),0,RGB(255,0,255));
	}

	//绘画金币
	int nXExcursion=ClientRect.Width()/2+20;

	//绘画数字
	LONG lGold=m_lAllGold;
	int iCount=0;
	while (lGold>0)
	{
		LONG lTemp=lGold%10;
		m_ImageNumber.AlphaDrawImage(&BackFaceDC,nXExcursion-m_ImageNumber.GetWidth()/10*(iCount++),5,
			m_ImageNumber.GetWidth()/10,m_ImageNumber.GetHeight(),
			m_ImageNumber.GetWidth()/10*lTemp,0,RGB(255,0,255));
		lGold/=10;
	}

	//绘画界面
	dc.BitBlt(0,0,ClientRect.Width(),ClientRect.Height(),&BackFaceDC,0,0,SRCCOPY);

	return;
}

//左键按下消息
void CGoldControl::OnLButtonUp(UINT nFlags, CPoint point)
{
	CImageHandle ImageHandle2(&m_ImageMoney);

	//位置过虑
	int nX = m_ImageMoney.GetWidth()/3+27;
	int nY = m_ImageMoney.GetHeight()/2+9;
	if ((point.x <=15) || (point.x>=nX)) return;
	if ((point.y>=77) || (point.y<=nY)) return;

	//按钮测试
	int iCellPos=(point.x-20)/48+1;
	if(iCellPos==4)iCellPos=3;

	//计算限制
	ASSERT((iCellPos>=0)&&(iCellPos<=3));
	LONG lAddGold=m_lGoldCount[iCellPos-1];
	if ((GetGold()+lAddGold)>m_lMaxGold) return;

	//重新设置
	m_lAllGold=GetGold()+lAddGold;

	LONG lGold=m_lAllGold;
	int nIndex=0;
	while (lGold>0L)
	{
		m_lGoldCell[nIndex++]=lGold%10L;
		lGold/=10L;
	}

	//if(m_lAllGold < m_lMinGold)
	//{
	//	SetGold(m_lMinGold);
	//	return;
	//}

	//重画界面
	Invalidate(FALSE);

	return;
}

//设置光标
BOOL CGoldControl::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT message)
{
	//获取鼠标
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(&point);

	CImageHandle ImageHandle2(&m_ImageMoney);

	//位置过虑
	int nX = m_ImageMoney.GetWidth()/3+27;
	int nY = m_ImageMoney.GetHeight()/2+9;
	if ((point.x <=15) || (point.x>=nX) || (point.y>=77) || (point.y<=nY))
		return __super::OnSetCursor(pWnd, nHitTest, message);

	//设置光标
	SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_CARD_CUR)));
	return TRUE;

}

//////////////////////////////////////////////////////////////////////////
