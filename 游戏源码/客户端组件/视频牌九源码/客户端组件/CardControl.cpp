#include "StdAfx.h"
#include "Resource.h"
#include "CardControl.h"

//////////////////////////////////////////////////////////////////////////

//宏定义
#define CARD_SPACE					16								//扑克间距
#define CARD_WIDTH					/*61*/	70							//扑克宽度
#define CARD_HEIGHT					/*82*/	95						//扑克高度

//静态变量
bool			CCardControl::m_bInit=false;						//初始标志
CSkinImage		CCardControl::m_ImageCard;							//扑克图片
CSkinImage		CCardControl::m_ImageBack;							//扑克图片

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
	//状态变量
	m_bShowFirst=false;
	m_bPositively=false;
	m_bCaptureMouse=false;

	//位置变量
	m_XCollocateMode=enXLeft;
	m_YCollocateMode=enYTop;

	//资源变量
	if (m_bInit==false)
	{
		m_bInit=true;
		
		//if (GetSystemMetrics(SM_CXSCREEN)>=1024) 
		//{	
			m_ImageCard.SetLoadInfo(IDB_CARD,AfxGetInstanceHandle());
			m_ImageBack.SetLoadInfo(IDB_CARD_BACK,AfxGetInstanceHandle());
		//}

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
	CImage ImageBuffer;
	CImageHandle HandleCard(&m_ImageCard);
	CImageHandle HandleFundus(&m_ImageBack);
	ImageBuffer.Create(rcClient.Width(),rcClient.Height(),16);
	if (ImageBuffer.IsNull()==true) return;

	//绘画扑克
	bool bDrawCard=false;
	CDC * pBufferDC=CDC::FromHandle(ImageBuffer.GetDC());
	for (int i=0;i<m_CardDataArray.GetCount();i++)
	{
		bDrawCard=((i==0)&&(m_bShowFirst==false))?false:true;
		if ((bDrawCard==true)&&(m_CardDataArray[i]!=0))
		{
			BYTE bCardData=m_CardDataArray[i];
			m_ImageCard.AlphaDrawImage(pBufferDC,i*CARD_SPACE,0,CARD_WIDTH,CARD_HEIGHT,
				GetCardXPos(bCardData),GetCardYPos(bCardData),RGB(255,0,255));
		}
		else
		{
			m_ImageBack.AlphaDrawImage(pBufferDC,i*CARD_SPACE,0,CARD_WIDTH,CARD_HEIGHT,0,0,RGB(255,0,255));
		}
	}
	ImageBuffer.ReleaseDC();
	ImageBuffer.BitBlt(dc,0,0);

	//清理资源
	ImageBuffer.Destroy();

	return;
}

//鼠标消息
void CCardControl::OnLButtonUp(UINT nFlags, CPoint point)
{
	__super::OnLButtonUp(nFlags, point);

	//状态判断
	if (m_bCaptureMouse==false) return;

	//释放捕获
	ReleaseCapture();
	m_bCaptureMouse=false;

	//刷新界面
	CRect rcInvalid(0,0,(m_CardDataArray.GetCount()==1)?CARD_WIDTH:CARD_SPACE,CARD_HEIGHT);
	InvalidateRect(rcInvalid,FALSE);

	return;
}

//鼠标消息
void CCardControl::OnLButtonDown(UINT nFlags, CPoint point)
{
	__super::OnLButtonDown(nFlags, point);

	//状态判断
	if ((m_bPositively==false)||(m_bShowFirst==true)) return;
	if ((point.x>CARD_SPACE)||(m_CardDataArray.GetCount()==0)) return;

	//变量定义
	CClientDC ClientDC(this);
	BYTE bCardData=m_CardDataArray[0];
	CImageHandle HandleCard(&m_ImageCard);
	int nDrawWidth=(m_CardDataArray.GetCount()==1)?CARD_WIDTH:CARD_SPACE;

	//捕获鼠标
	SetCapture();
	m_bCaptureMouse=true;

	//绘画扑克
	int nXPos=GetCardXPos(bCardData);
	int nYPos=GetCardYPos(bCardData);
	m_ImageCard.AlphaDrawImage(&ClientDC,0,0,nDrawWidth,CARD_HEIGHT,nXPos,nYPos,RGB(255,0,255));

	return;
}

//光标消息
BOOL CCardControl::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT message)
{
	if ((m_bPositively==true)&&(m_bShowFirst==false))
	{
		//获取光标
		CPoint MousePoint;
		GetCursorPos(&MousePoint);
		ScreenToClient(&MousePoint);

		//位置判断
		if (MousePoint.x<CARD_SPACE)
		{
			SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_CARD_CUR)));
			return TRUE;
		}
	}
	
	return __super::OnSetCursor(pWnd,nHitTest,message);
}

//获取数目
WORD CCardControl::GetCardCount()
{
	return (WORD)m_CardDataArray.GetCount();
}

//排列扑克
void CCardControl::SortCardData()
{
	//if (m_CardDataArray.GetCount()>1)
	//{
	//	//变量定义
	//	bool bFinished=true;
	//	BYTE bCardDataTemp=0;
	//	INT_PTR nLastPos=m_CardDataArray.GetCount()-1;

	//	//排序操作
	//	do
	//	{
	//		bFinished=true;
	//		for (INT_PTR i=0;i<nLastPos;i++) 
	//		{
	//			if (CompareOnlyOne(m_CardDataArray[i],m_CardDataArray[i+1])==false)
	//			{	
	//				bCardDataTemp=m_CardDataArray[i];
	//				m_CardDataArray[i]=m_CardDataArray[i+1];
	//				m_CardDataArray[i+1]=bCardDataTemp;
	//				bFinished=false;
	//			}	
	//		}
	//		nLastPos--;
	//	} while (bFinished==false);
	//}

	return;
}

//显示扑克
void CCardControl::ShowFirstCard(bool bShowFirst)
{
	//设置变量
	m_bShowFirst=bShowFirst;

	//刷新界面
	CRect rcInvalid(0,0,(m_CardDataArray.GetCount()==1)?CARD_WIDTH:CARD_SPACE,CARD_HEIGHT);
	InvalidateRect(rcInvalid,FALSE);

	return;
}

//设置响应
void CCardControl::AllowPositively(bool bPositively)
{
	m_bPositively=bPositively;
	return;
}

//设置扑克
WORD CCardControl::GetCardData(BYTE bCardBuffer[], WORD wBufferSize)
{
	//拷贝数据
	WORD wCopyCount=(WORD)m_CardDataArray.GetCount();
	if (wCopyCount>(wBufferSize/sizeof(BYTE))) wCopyCount=wBufferSize/sizeof(BYTE);
	CopyMemory(bCardBuffer,m_CardDataArray.GetData(),wCopyCount);

	return (WORD)m_CardDataArray.GetCount();
}

//设置扑克
bool CCardControl::SetCardData(const BYTE bCardData[], WORD wCardCount)
{
	//设置扑克
	m_CardDataArray.RemoveAll();
	m_CardDataArray.SetSize(wCardCount);
	CopyMemory(m_CardDataArray.GetData(),bCardData,sizeof(BYTE)*wCardCount);

	//调整位置
	RectifyControl();

	return 0;
}

//设置基准点函数
void CCardControl::SetBasicStation(CPoint & BasicStation, enXCollocateMode XCollocateMode, enYCollocateMode YCollocateMode)
{
	//设置变量
	m_BasicStation=BasicStation;
	m_XCollocateMode=XCollocateMode;
	m_YCollocateMode=YCollocateMode;

	//调整位置
	RectifyControl();

	return;
}

//获取扑克数值
//BYTE CCardControl::GetCardValue(BYTE bCardData)
//{
//	BYTE bCardVolue=(bCardData-1)%13;
//	if (bCardVolue==0) bCardVolue=14;
//	return bCardVolue;
//}

//扑克位置
WORD CCardControl::GetCardXPos(BYTE bCardData)//这里是根据扑克数据来确定该扑克在图片中的X位置
{
	return ((bCardData&CARD_MASK_VALUE)-1)*CARD_WIDTH;
}

//扑克位置
WORD CCardControl::GetCardYPos(BYTE bCardData)//这里是根据扑克数据来确定该扑克在图片中的Y位置
{
	return ((bCardData&CARD_MASK_COLOR)>>4)*CARD_HEIGHT;//Y位置正确
}

//对比扑克
//bool CCardControl::CompareOnlyOne(BYTE bFirstCard, BYTE bLastCard)
//{
//	BYTE bFirstVolae=GetCardValue(bFirstCard);
//	BYTE bLastVolae=GetCardValue(bLastCard);
//	if (bFirstVolae==bLastVolae) return bFirstCard>bLastCard;
//	return bFirstVolae>bLastVolae;
//}

//调整窗口位置
void CCardControl::RectifyControl()
{
	//获取大小 
	INT_PTR nCardCount=(int)m_CardDataArray.GetCount();
	CSize CardSize((nCardCount>0)?(CARD_WIDTH+(int)(nCardCount-1)*CARD_SPACE):0,CARD_HEIGHT);

	//调整位置
	CRect rcCardRect;
	switch (m_XCollocateMode)
	{
	case enXLeft:	{ rcCardRect.left=m_BasicStation.x; break; }
	case enXCenter: { rcCardRect.left=m_BasicStation.x-CardSize.cx/2; break; }
	case enXRight:	{ rcCardRect.left=m_BasicStation.x-CardSize.cx; break; }
	}

	switch (m_YCollocateMode)
	{
	case enYTop:	{ rcCardRect.top=m_BasicStation.y; break; }
	case enYCenter: { rcCardRect.top=m_BasicStation.y-CardSize.cy/2; break; }
	case enYBottom: { rcCardRect.top=m_BasicStation.y-CardSize.cy; break; }
	}

	//移动位置
	rcCardRect.right=rcCardRect.left+CardSize.cx;
	rcCardRect.bottom=rcCardRect.top+CardSize.cy;
	MoveWindow(&rcCardRect,FALSE);

	//设置区域
	CRgn AllRgn,SignedRgn;
	AllRgn.CreateRectRgn(0,0,0,0);
	for (int i=0;i<nCardCount;i++)
	{
		SignedRgn.CreateRoundRectRgn(i*CARD_SPACE,0,i*CARD_SPACE+CARD_WIDTH,CARD_HEIGHT,2,2);
		AllRgn.CombineRgn(&AllRgn,&SignedRgn,RGN_OR);
		SignedRgn.DeleteObject();
	}

	//设置区域
	SetWindowRgn(AllRgn,TRUE);
	m_CardRgn.DeleteObject();
	m_CardRgn.Attach(AllRgn.Detach());

	//重画界面
	Invalidate(FALSE);

	return;
}

//////////////////////////////////////////////////////////////////////////
