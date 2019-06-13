#include "StdAfx.h"
#include "Resource.h"
#include "GameOption.h"
#include "CardControl.h"
#include ".\cardcontrol.h"

//////////////////////////////////////////////////////////////////////////

//静态变量
bool					CCardControl::m_bLoad=false;					//初始标志
CSize					CCardControl::m_CardSize;						//扑克大小
CSkinImage				CCardControl::m_ImageCard;						//扑克图片

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CCardControl, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CCardControl::CCardControl()
{
	//配置变量
	m_bPositively=false;
	m_bDisplayHead=false;
	m_dwCardHSpace=DEFAULT_PELS;

	//状态变量
	m_bCaptureMouse=false;

	//位置变量
	m_XCollocateMode=enXCenter;
	m_YCollocateMode=enYCenter;

	//资源变量
	if (m_bLoad==false)
	{
		//加载资源
		m_bLoad=true;
		m_ImageCard.SetLoadInfo(IDB_CARD,AfxGetInstanceHandle());

		//获取大小
		CImageHandle CardImageHandle(&m_ImageCard);
		m_CardSize.cx=m_ImageCard.GetWidth()/13;
		m_CardSize.cy=m_ImageCard.GetHeight()/5;
	}

	return;
}

//析构函数
CCardControl::~CCardControl()
{
}

//建立消息
int CCardControl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//设置背景
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);

	return 0;
}

//重画函数
void CCardControl::OnPaint()
{
	CPaintDC dc(this);

	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//加载位图
	CImageHandle HandleCard(&m_ImageCard);

	//创建位图
	CBitmap BufferImage;
	BufferImage.CreateCompatibleBitmap(&dc,rcClient.Width(),rcClient.Height());

	//创建 DC
	CDC BufferDC;
	BufferDC.CreateCompatibleDC(&dc);
	BufferDC.SelectObject(&BufferImage);

	//绘画扑克
	DWORD dwXImagePos,dwYImagePos;
	DWORD dwXScreenPos,dwYScreenPos;
	for (INT_PTR i=0;i<m_CardDataArray.GetCount();i++)
	{
		//变量定义
		BYTE cbCardData=m_CardDataArray[i];
		bool bDrawCard=((i!=0)||(m_bDisplayHead==true));

		//图片位置
		if ((bDrawCard==true)&&(cbCardData!=0))
		{
			dwXImagePos=((cbCardData&CARD_MASK_VALUE)-1)*m_CardSize.cx;
			dwYImagePos=((cbCardData&CARD_MASK_COLOR)>>4)*m_CardSize.cy;
		}
		else
		{
			dwXImagePos=m_CardSize.cx*2;
			dwYImagePos=m_CardSize.cy*4;
		}

		//屏幕位置
		dwYScreenPos=0;
		dwXScreenPos=m_dwCardHSpace*i;

		//绘画扑克
		m_ImageCard.AlphaDrawImage(&BufferDC,dwXScreenPos,dwYScreenPos,m_CardSize.cx,m_CardSize.cy,dwXImagePos,dwYImagePos,RGB(255,0,255));
	}

	//绘画界面
	dc.BitBlt(0,0,rcClient.Width(),rcClient.Height(),&BufferDC,0,0,SRCCOPY);

	//清理资源
	BufferDC.DeleteDC();
	BufferImage.DeleteObject();

	return;
}

//鼠标消息
void CCardControl::OnLButtonUp(UINT nFlags, CPoint Point)
{
	__super::OnLButtonUp(nFlags, Point);

	//状态判断
	if (m_bCaptureMouse==false) return;

	//释放捕获
	ReleaseCapture();
	m_bCaptureMouse=false;

	//刷新界面
	CRect rcInvalid(0,0,(m_CardDataArray.GetCount()==1)?m_CardSize.cx:m_dwCardHSpace,m_CardSize.cy);
	InvalidateRect(rcInvalid,FALSE);

	return;
}

//鼠标消息
void CCardControl::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags, Point);

	//状态判断
	if ((m_bPositively==false)||(m_bDisplayHead==true)) return;
	if ((Point.x>(int)m_dwCardHSpace)||(m_CardDataArray.GetCount()==0)) return;

	//变量定义
	BYTE cbCardData=m_CardDataArray[0];
	int nDrawWidth=(m_CardDataArray.GetCount()==1)?m_CardSize.cx:m_dwCardHSpace;

	//捕获鼠标
	SetCapture();
	m_bCaptureMouse=true;

	//计算位置
	DWORD dwXImagePos=((cbCardData&CARD_MASK_VALUE)-1)*m_CardSize.cx;
	DWORD dwYImagePos=((cbCardData&CARD_MASK_COLOR)>>4)*m_CardSize.cy;

	//绘画扑克
	CClientDC ClientDC(this);
	CImageHandle HandleCard(&m_ImageCard);
	m_ImageCard.AlphaDrawImage(&ClientDC,0,0,nDrawWidth,m_CardSize.cy,dwXImagePos,dwYImagePos,RGB(255,0,255));

	return;
}

//光标消息
BOOL CCardControl::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
{
	if ((m_bPositively==true)&&(m_bDisplayHead==false))
	{
		//获取光标
		CPoint MousePoint;
		GetCursorPos(&MousePoint);
		ScreenToClient(&MousePoint);

		//位置判断
		if (MousePoint.x<(int)m_dwCardHSpace)
		{
			SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_CARD_CUR)));
			return TRUE;
		}
	}
	
	return __super::OnSetCursor(pWnd,nHitTest,uMessage);
}

//获取数目
DWORD CCardControl::GetCardCount()
{
	return (WORD)m_CardDataArray.GetCount();
}

//显示扑克
void CCardControl::SetDisplayHead(bool bDisplayHead)
{
	//状态判断
	if (m_bDisplayHead==bDisplayHead) return;

	//设置变量
	m_bDisplayHead=bDisplayHead;

	//重画控件
	if (m_hWnd!=NULL) Invalidate(FALSE);

	return;
}

//设置间距
void CCardControl::SetCardSpace(DWORD dwCardSpace)
{
	//设置变量
	m_dwCardHSpace=dwCardSpace;

	//调整控件
	if (m_hWnd!=NULL) RectifyControl();

	return;
}

//设置响应
void CCardControl::SetPositively(bool bPositively)
{
	//设置变量
	m_bPositively=bPositively;

	return;
}

//设置扑克
DWORD CCardControl::GetCardData(BYTE cbCardData[], DWORD dwMaxCount)
{
	//拷贝扑克
	DWORD dwCopyCount=__min((DWORD)m_CardDataArray.GetCount(),dwMaxCount);
	CopyMemory(cbCardData,m_CardDataArray.GetData(),sizeof(BYTE)*dwCopyCount);

	return dwCopyCount;
}

//设置扑克
DWORD CCardControl::SetCardData(const BYTE cbCardData[], DWORD dwCardCount)
{
	//设置扑克
	m_CardDataArray.SetSize(dwCardCount);
	CopyMemory(m_CardDataArray.GetData(),cbCardData,sizeof(BYTE)*dwCardCount);

	//调整位置
	RectifyControl();

	return dwCardCount;
}

//基准位置
void CCardControl::SetBenchmarkPos(int nXPos, int nYPos, enXCollocateMode XCollocateMode, enYCollocateMode YCollocateMode)
{
	//设置变量
	m_BenchmarkPos.x=nXPos;
	m_BenchmarkPos.y=nYPos;
	m_XCollocateMode=XCollocateMode;
	m_YCollocateMode=YCollocateMode;

	//调整控件
	if (m_hWnd!=NULL) RectifyControl();

	return;
}

//基准位置
void CCardControl::SetBenchmarkPos(const CPoint & BenchmarkPos, enXCollocateMode XCollocateMode, enYCollocateMode YCollocateMode)
{
	//设置变量
	m_BenchmarkPos=BenchmarkPos;
	m_XCollocateMode=XCollocateMode;
	m_YCollocateMode=YCollocateMode;

	//调整控件
	if (m_hWnd!=NULL) RectifyControl();

	return;
}

//调整控件
void CCardControl::RectifyControl()
{
	//变量定义
	DWORD dwCardCount=(DWORD)m_CardDataArray.GetCount();

	//计算大小
	CSize ControlSize;
	ControlSize.cy=m_CardSize.cy;
	ControlSize.cx=(dwCardCount>0)?(m_CardSize.cx+(dwCardCount-1)*m_dwCardHSpace):0;

	//横向位置
	int nXPos=0;
	switch (m_XCollocateMode)
	{
	case enXLeft:	{ nXPos=m_BenchmarkPos.x; break; }
	case enXCenter: { nXPos=m_BenchmarkPos.x-ControlSize.cx/2; break; }
	case enXRight:	{ nXPos=m_BenchmarkPos.x-ControlSize.cx; break; }
	}

	//竖向位置
	int nYPos=0;
	switch (m_YCollocateMode)
	{
	case enYTop:	{ nYPos=m_BenchmarkPos.y; break; }
	case enYCenter: { nYPos=m_BenchmarkPos.y-ControlSize.cy/2; break; }
	case enYBottom: { nYPos=m_BenchmarkPos.y-ControlSize.cy; break; }
	}

	//移动位置
	SetWindowPos(NULL,nXPos,nYPos,ControlSize.cx,ControlSize.cy,SWP_NOZORDER);

	//设置区域
	CRgn CardRegion,SignedRegion;
	CardRegion.CreateRectRgn(0,0,0,0);
	for (DWORD i=0;i<dwCardCount;i++)
	{
		SignedRegion.CreateRoundRectRgn(i*m_dwCardHSpace,0,i*m_dwCardHSpace+m_CardSize.cx+1,m_CardSize.cy+1,2,2);
		CardRegion.CombineRgn(&CardRegion,&SignedRegion,RGN_OR);
		SignedRegion.DeleteObject();
	}

	//设置区域
	SetWindowRgn(CardRegion,TRUE);
	m_CardRegion.DeleteObject();
	m_CardRegion.Attach(CardRegion.Detach());

	//重画界面
	Invalidate(FALSE);

	return;
}

//获取牌尾位置
CPoint CCardControl::GetTailPos()
{
	CPoint ptTail;

	//变量定义
	DWORD dwCardCount=(DWORD)m_CardDataArray.GetCount();

	//计算大小
	CSize ControlSize;
	ControlSize.cy=m_CardSize.cy;
	ControlSize.cx=(dwCardCount>0)?(m_CardSize.cx+(dwCardCount-1)*m_dwCardHSpace):0;
	INT nSpaceWidth = dwCardCount*m_dwCardHSpace;

	//横向位置
	int nXPos=0;
	switch (m_XCollocateMode)
	{
	case enXLeft:	{ ptTail.x=m_BenchmarkPos.x+nSpaceWidth; break; }
	case enXCenter: { ptTail.x=m_BenchmarkPos.x-ControlSize.cx/2+nSpaceWidth; break; }
	case enXRight:	{ ptTail.x=m_BenchmarkPos.x-ControlSize.cx+nSpaceWidth; break; }
	}

	//竖向位置
	int nYPos=0;
	switch (m_YCollocateMode)
	{
	case enYTop:	{ ptTail.y=m_BenchmarkPos.y; break; }
	case enYCenter: { ptTail.y=m_BenchmarkPos.y-ControlSize.cy/2; break; }
	case enYBottom: { ptTail.y=m_BenchmarkPos.y-ControlSize.cy; break; }
	}

	return ptTail;
}

//////////////////////////////////////////////////////////////////////////

