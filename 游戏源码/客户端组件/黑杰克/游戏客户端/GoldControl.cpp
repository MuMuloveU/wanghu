#include "Stdafx.h"
#include "Math.h"
#include "Resource.h"
#include "GoldControl.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

#define CELL_WIDTH							19							//单元宽度
#define LESS_WIDTH							70							//最小宽度
#define SPACE_WIDTH							12							//空袭宽度
#define BUTTON_WIDTH						0							//功能区域
#define CONTROL_HEIGHT						50							//控件高度

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGoldControl, CWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGoldControl::CGoldControl()
{
	//界面变量
	m_nWidth=0;
	m_nCellCount=0;
	m_AppendWidth=0;

	//设置变量
	m_lMaxGold=0L;
	memset(m_lGoldCell,0,sizeof(m_lGoldCell));

	//加载资源
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_ImageLeft.SetLoadInfo(IDB_CONTROL_BACK_LEFT,hInstance);
	m_ImageMid.SetLoadInfo(IDB_CONTROL_BACK_MID,hInstance);
	m_ImageRight.SetLoadInfo(IDB_CONTROL_BACK_RIGHT,hInstance);
}

//析构函数
CGoldControl::~CGoldControl()
{
}

//获取金币
LONG CGoldControl::GetGold()
{
	LONG lGold=0L;
	for (LONG i=0;i<CountArray(m_lGoldCell);i++) lGold+=m_lGoldCell[i]*(LONG)pow((double)(double)10L,i);
	return lGold;
}

//设置金币
void CGoldControl::SetGold(LONG lGold)
{
	//调整参数
	if (lGold>m_lMaxGold) lGold=m_lMaxGold;
	memset(m_lGoldCell,0,sizeof(m_lGoldCell));

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

	//设置界面
	m_AppendWidth=0;
	m_nWidth=(m_nCellCount+(m_nCellCount-1)/3)*CELL_WIDTH+SPACE_WIDTH*2+BUTTON_WIDTH;
	if (m_nWidth<LESS_WIDTH) 
	{
		m_AppendWidth=LESS_WIDTH-m_nWidth;
		m_nWidth=LESS_WIDTH;
	}
	MoveWindow(m_BasicPoint.x-m_nWidth,m_BasicPoint.y-CONTROL_HEIGHT,m_nWidth,CONTROL_HEIGHT);

	//重画界面
	Invalidate(FALSE);

	return;
}

//设置位置
void CGoldControl::SetBasicPoint(int nXPos, int nYPos)
{
	//设置变量
	m_BasicPoint.x = nXPos;
	m_BasicPoint.y = nYPos;

	//调整界面
	RectifyControl();

	return;
}

//调整控件
void CGoldControl::RectifyControl()
{
	MoveWindow(m_BasicPoint.x-m_nWidth,m_BasicPoint.y-CONTROL_HEIGHT,m_nWidth,CONTROL_HEIGHT);
	return;
}

//绘画金币
void CGoldControl::DrawGoldCell(CDC * pDC, int nXPos, int nYPox, LONG lGold)
{
	if (lGold<10L) 
	{
		TCHAR szBuffer[12];
		_snprintf(szBuffer,sizeof(szBuffer),TEXT("%d"),lGold);
		pDC->TextOut(nXPos,nYPox,szBuffer,lstrlen(szBuffer));
	}
	else
	{
		LPCTSTR pszBuffer=TEXT("，");
		pDC->TextOut(nXPos,nYPox,pszBuffer,lstrlen(pszBuffer));
	}

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
	CImageHandle ImageMidHandle(&m_ImageMid);
	CImageHandle ImageLeftHandle(&m_ImageLeft);
	CImageHandle ImageRightHandle(&m_ImageRight);

	//绘画背景
	m_ImageLeft.BitBlt(BackFaceDC,0,0);
	m_ImageMid.StretchBlt(BackFaceDC,m_ImageLeft.GetWidth(),0,ClientRect.Width()-m_ImageLeft.GetWidth()-m_ImageRight.GetWidth(),m_ImageMid.GetHeight());
	m_ImageRight.BitBlt(BackFaceDC,ClientRect.Width()-m_ImageRight.GetWidth(),0);

	//创建字体
	CFont GlodFont;
	GlodFont.CreateFont(-22,0,0,0,400,0,0,0,0,3,2,1,34,TEXT("Arial Black"));
	CFont * pOldFont=BackFaceDC.SelectObject(&GlodFont);

	//设置 DC
	BackFaceDC.SetBkMode(TRANSPARENT);
	BackFaceDC.SetTextColor(RGB(250,250,0));

	//绘画金币
	int nXExcursion=ClientRect.Width()-SPACE_WIDTH-BUTTON_WIDTH;
	for (int i=0;i<m_nCellCount;i++)
	{
		//绘画逗号
		if ((i!=0)&&(i%3)==0)
		{
			nXExcursion-=CELL_WIDTH;
			DrawGoldCell(&BackFaceDC,nXExcursion,0,10);
		}

		//绘画数字
		nXExcursion-=CELL_WIDTH;
		DrawGoldCell(&BackFaceDC,nXExcursion,0,m_lGoldCell[i]);
	}

	//清理资源
	BackFaceDC.SelectObject(pOldFont);
	GlodFont.DeleteObject();

	//提示信息
	TCHAR szBuffer[50];
	sprintf(szBuffer,TEXT("最多：%ld"),m_lMaxGold);
	BackFaceDC.SetTextColor(RGB(200,200,200));
	BackFaceDC.SelectObject(CSkinResourceManager::GetDefaultFont());
	BackFaceDC.TextOut(10,31,szBuffer,lstrlen(szBuffer));

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
	int nHeadWidth=SPACE_WIDTH+m_AppendWidth;
	if ((point.y<=2)||(point.y>=26)) return;
	if ((point.x<=nHeadWidth)||(point.x>=(CELL_WIDTH*nViewCellCount+nHeadWidth))) return;

	//按钮测试
	int iCellPos=(nViewCellCount-(point.x-nHeadWidth)/CELL_WIDTH)-1;
	if ((iCellPos==3)||(iCellPos==7)) return;
	if (iCellPos>3) iCellPos=iCellPos-(iCellPos-1)/3;

	//计算限制
	ASSERT((iCellPos>=0)&&(iCellPos<CountArray(m_lGoldCell)));
	if (m_lGoldCell[iCellPos]!=9L)
	{
		LONG lAddGold=(LONG)pow((double)(double)10L,iCellPos);
		if ((GetGold()+lAddGold)>m_lMaxGold) return;
	}

	//设置变量
	m_lGoldCell[iCellPos]=(m_lGoldCell[iCellPos]+1)%10;

	//重画界面
	Invalidate(FALSE);

	return;
}

//右键按下消息
void CGoldControl::OnRButtonUp(UINT nFlags, CPoint point)
{
	//变量定义
	int nViewCellCount = (m_nCellCount - 1) / 3 + m_nCellCount;

	//位置过虑
	int nHeadWidth = SPACE_WIDTH + m_AppendWidth;
	if ((point.y <= 2)||(point.y >= 26)) return;
	if ((point.x <= nHeadWidth)||(point.x >= (CELL_WIDTH*nViewCellCount + nHeadWidth))) return;

	//按钮测试
	int iCellPos = (nViewCellCount - (point.x - nHeadWidth) / CELL_WIDTH) - 1;
	if ((iCellPos == 3)||(iCellPos == 7)) return;
	if (iCellPos > 3) iCellPos = iCellPos - (iCellPos - 1) / 3;

	//计算限制
	ASSERT((iCellPos >= 0) && (iCellPos < CountArray(m_lGoldCell)));
	if (m_lGoldCell[iCellPos] == 0L)
	{
		LONG lAddGold = 9L * (LONG)pow((double)(double)10L,iCellPos);
		if ((GetGold() + lAddGold) > m_lMaxGold) return;
	}

	//设置变量
	m_lGoldCell[iCellPos] = (m_lGoldCell[iCellPos] + 9) % 10;

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

	//设置光标
	int nHeadWidth = SPACE_WIDTH + m_AppendWidth;
	int nViewCellCount = (m_nCellCount - 1) / 3 + m_nCellCount;
	if ((point.y > 2) && (point.y < 26) && (point.x>nHeadWidth) && (point.x < (CELL_WIDTH*nViewCellCount+nHeadWidth)))
	{
		int iCellPos = (m_nCellCount - (point.x - nHeadWidth) / CELL_WIDTH) - 1;
		if ((iCellPos != 3) && (iCellPos != 7))
		{
			SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_CARD_CUR)));
			return TRUE;
		}
	}

	return __super::OnSetCursor(pWnd, nHitTest, message);
}

//////////////////////////////////////////////////////////////////////////
