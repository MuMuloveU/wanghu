#include "StdAfx.h"
#include "Resource.h"
#include "CardControl.h"
#include ".\cardcontrol.h"

//////////////////////////////////////////////////////////////////////////
//静态变量

bool					CCardControl::m_bLoad=false;					//加载标志
CSize					CCardControl::m_CardSize;						//扑克大小
CSkinImage				CCardControl::m_ImageCard;						//图片资源

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CCardControl, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CCardControl::CCardControl()
{
	//配置变量
	m_bDisplay=false;
	m_bHorizontal=true;
	m_bPositively=false;
	m_bChange=false;

	//间隔变量
	m_dwCardHSpace=20;
	m_dwCardVSpace=19;
	m_dwShootAltitude=20;

	//位置变量
	m_XCollocateMode=enXCenter;
	m_YCollocateMode=enYCenter;
	m_BenchmarkPos.SetPoint(0,0);

	//运行变量
	m_dwCurrentIndex=0xFFFFFFFF;
	m_dwSelectIndex=0xFFFFFFFF;

	//内部数据
	m_pSinkWindow=NULL;

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

//设置窗口
void CCardControl::SetSinkWindow(CWnd * pSinkWindow)
{
	//设置变量
	m_pSinkWindow=pSinkWindow;

	return;
}

//设置方向
void CCardControl::SetDirection(bool bHorizontal)
{
	//状态判断
	if (m_bHorizontal==bHorizontal) return;

	//设置变量
	m_bHorizontal=bHorizontal;

	//调整控件
	if (m_hWnd!=NULL) RectifyControl();

	return;
}

//弹起扑克
void CCardControl::ShootAllCard(bool bShoot)
{
	//变量定义
	bool bRectify=false;
	tagCardItem * pCardItem=NULL;
	DWORD dwCardCount=(DWORD)m_CardDataItem.GetCount();

	//设置扑克
	for (DWORD i=0;i<dwCardCount;i++)
	{
		//获取扑克
		pCardItem=&m_CardDataItem[i];

		//扑克分析
		if (pCardItem->bShoot!=bShoot)
		{
			bRectify=true;
			pCardItem->bShoot=bShoot;
		}
	}

	//调整控件
	if (bRectify==true) RectifyControl();

	return;
}

//设置显示
void CCardControl::SetDisplayFlag(bool bDisplay)
{
	//状态判断
	if (m_bDisplay==bDisplay) return;

	//设置变量
	m_bDisplay=bDisplay;

	//重画控件
	if (m_hWnd!=NULL) Invalidate(FALSE);

	return;
}

//设置响应
void CCardControl::SetPositively(bool bPositively)
{
	//设置变量
	m_bPositively=bPositively;

	return;
}

//设置间距
void CCardControl::SetCardSpace(DWORD dwCardHSpace, DWORD dwCardVSpace, DWORD dwShootAltitude)
{
	//设置变量
	m_dwCardHSpace=dwCardHSpace;
	m_dwCardVSpace=dwCardVSpace;
	m_dwShootAltitude=dwShootAltitude;

	//调整控件
	if (m_hWnd!=NULL) RectifyControl();

	return;
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

//获取数目
DWORD CCardControl::GetCardCount()
{
	return (DWORD)m_CardDataItem.GetCount();
}

//获取扑克
DWORD CCardControl::GetShootCard(BYTE bCardData[], DWORD dwMaxCount)
{
	//变量定义
	DWORD bShootCount=0L;
	DWORD dwCardCount=(DWORD)m_CardDataItem.GetCount();

	//搜索扑克
	tagCardItem * pCardItem=NULL;
	for (DWORD i=0;i<dwCardCount;i++)
	{
		//获取扑克
		pCardItem=&m_CardDataItem[i];

		//扑克分析
		if (pCardItem->bShoot==true) bCardData[bShootCount++]=pCardItem->bCardData;
	}

	return bShootCount;
}

//设置扑克
DWORD CCardControl::SetCardData(const BYTE bCardData[], DWORD dwCardCount)
{
	//设置扑克
	m_CardDataItem.SetSize(dwCardCount);
	for (DWORD i=0;i<dwCardCount;i++)
	{
		m_CardDataItem[i].bShoot	=	false;
		m_CardDataItem[i].bCardData	=	bCardData[i];
		m_CardDataItem[i].bSelect	=	false;
		m_CardDataItem[i].bMark		=	false;
	}

	//调整控件
	if (m_hWnd!=NULL) RectifyControl();

	return dwCardCount;
}

//获取扑克
DWORD CCardControl::GetCardData(tagCardItem CardItemArray[], DWORD dwMaxCount)
{
	//效验参数
	ASSERT(dwMaxCount>=(DWORD)m_CardDataItem.GetCount());

	//拷贝扑克
	DWORD dwCardCount=(DWORD)m_CardDataItem.GetCount();
	tagCardItem * pCardItemData=m_CardDataItem.GetData();
	CopyMemory(CardItemArray,pCardItemData,dwCardCount*sizeof(tagCardItem));

	return dwCardCount;
}

//设置扑克
DWORD CCardControl::SetCardItem(const tagCardItem CardItemArray[], DWORD dwCardCount)
{
	//设置扑克
	m_CardDataItem.SetSize(dwCardCount);
	CopyMemory(m_CardDataItem.GetData(),CardItemArray,dwCardCount*sizeof(tagCardItem));

	//调整控件
	if (m_hWnd!=NULL) RectifyControl();
 
	return 0;
}
//设置弹起扑克
DWORD CCardControl::SetShootCard(const BYTE bCardDataIndex[], DWORD dwCardCount)
{
	for(DWORD i=0;i<dwCardCount;i++)
	{
		
		tagCardItem * pCardItem=&m_CardDataItem[bCardDataIndex[i]];
		pCardItem->bShoot=true;
		if (m_hWnd!=NULL) RectifyControl();
	}
	return 0;
}

//设置弹起扑克
DWORD CCardControl::SetShootCardData(const BYTE bCardData [], DWORD dwCardCount)
{
	ASSERT(dwCardCount<=(DWORD)m_CardDataItem.GetCount());
	//设置变量
	tagCardItem * pCardItem;
	for(DWORD i=0;i<dwCardCount;i++)
	{
		for(DWORD j=0;j<(DWORD)m_CardDataItem.GetCount();j++)
		{
			pCardItem=&m_CardDataItem[j];
			if(pCardItem->bCardData==bCardData[i]&&!pCardItem->bShoot)
			{
				pCardItem->bShoot=true;
				break;
			}
		}
		
	}

	if (m_hWnd!=NULL) RectifyControl();
	return 0;
}


//调整位置
void CCardControl::RectifyControl()
{
	//变量定义
	DWORD dwCardCount=(DWORD)m_CardDataItem.GetCount();

	//计算大小
	CSize ControlSize;
	if (m_bHorizontal==true)
	{
		ControlSize.cy=m_CardSize.cy+m_dwShootAltitude+6;
		ControlSize.cx=(dwCardCount>0)?(m_CardSize.cx+(dwCardCount-1)*m_dwCardHSpace):0;
		ControlSize.cx+=6;
	}
	else
	{
		ControlSize.cx=m_CardSize.cx;
		ControlSize.cy=(dwCardCount>0)?(m_CardSize.cy+(dwCardCount-1)*m_dwCardVSpace):0;
	}

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

	//变量定义
	CRgn CardRegion,SignedRegion;
	tagCardItem * pCardItem=NULL;

	//设置区域
	CardRegion.CreateRectRgn(0,0,0,0);
	for (DWORD i=0;i<dwCardCount;i++)
	{
		//获取扑克
		pCardItem=&m_CardDataItem[i];

		//计算位置
		if (m_bHorizontal==true)
		{
			nXPos=m_dwCardHSpace*i+3;
			nYPos=(pCardItem->bShoot==false)?m_dwShootAltitude:0;
			nYPos+=3;
		}
		else 
		{
			nXPos=0;
			nYPos=m_dwCardVSpace*i;
		}

		//合并区域
		//扑克大小
		CSize     CardSize;
		if(pCardItem->bSelect)
		{
			CardSize.cx	=	m_CardSize.cx+6;
			CardSize.cy	=	m_CardSize.cy+6;
			nYPos-=3;
			nXPos-=3;
		}
		else
		{
            CardSize.cx=m_CardSize.cx;
			CardSize.cy=m_CardSize.cy;
		}
		SignedRegion.CreateRectRgn(nXPos,nYPos,nXPos+CardSize.cx,nYPos+CardSize.cy);
		
		CardRegion.CombineRgn(&CardRegion,&SignedRegion,RGN_OR);
		SignedRegion.DeleteObject();
	}

	//设置区域
	SetWindowRgn(CardRegion,TRUE);
	m_CardRegion.DeleteObject();
	m_CardRegion.Attach(CardRegion.Detach());

	//重画界面
	Invalidate(TRUE);

	return;
}

//索引切换
DWORD CCardControl::SwitchCardPoint(const CPoint & MousePoint)
{
	if (m_bHorizontal==true)
	{
		//变量定义
		DWORD dwCardCount=(DWORD)m_CardDataItem.GetCount();
		
		//最小位置
		if(MousePoint.x<=(LONG(m_dwCardHSpace+3)))
			return   0;

		//最大位置
		if(MousePoint.x>(LONG((dwCardCount-1)*m_dwCardHSpace+m_CardSize.cx+6)))
			return  0x0FFFFFFF;
		
		//获取索引
		DWORD dwCardIndex=(MousePoint.x-3)/m_dwCardHSpace;
		
		if (dwCardIndex>=dwCardCount) dwCardIndex=(dwCardCount-1);
		
		return dwCardIndex;
	}

	return DWORD(-1);
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
	for (INT i=0;i<m_CardDataItem.GetCount();i++)
	{
		//获取扑克
		tagCardItem * pCardItem=&m_CardDataItem[i];

		//图片位置
		if ((m_bDisplay==true)&&(pCardItem->bCardData!=0))
		{
			dwXImagePos=((pCardItem->bCardData&CARD_MASK_VALUE)-1)*m_CardSize.cx;
			dwYImagePos=((pCardItem->bCardData&CARD_MASK_COLOR)>>4)*m_CardSize.cy;
		}
		else
		{
			dwXImagePos=m_CardSize.cx*2;
			dwYImagePos=m_CardSize.cy*4;
		}

		//屏幕位置
		if (m_bHorizontal==true) 
		{
			dwXScreenPos=m_dwCardHSpace*i+3;
			dwYScreenPos=(pCardItem->bShoot==false)?m_dwShootAltitude:0;
			dwYScreenPos+=3;
		}
		else
		{
			dwXScreenPos=0;
			dwYScreenPos=m_dwCardVSpace*i;
		}

		//绘画扑克
		m_ImageCard.BitBlt(BufferDC,dwXScreenPos,dwYScreenPos,m_CardSize.cx,m_CardSize.cy,dwXImagePos,dwYImagePos);
        
		//画选择框
		if(pCardItem->bSelect)
		{
			CDC cDCBuffer;
			CBitmap bitmap;
			bitmap.LoadBitmap(IDB_SELECT);
			cDCBuffer.CreateCompatibleDC(&dc);
			cDCBuffer.SelectObject(bitmap);
			int xDest,yDest;
			xDest=m_dwSelectIndex*m_dwCardHSpace;
			yDest=m_dwShootAltitude;
			if(pCardItem->bShoot)
				yDest=0;
			BufferDC.TransparentBlt(xDest,yDest,79,104,&cDCBuffer,0,0,79,104,RGB(255,255,255));
		}
        
		//标记扑克
		if(pCardItem->bMark)
		{
			CBitmap		bitmap;
			CDC			lBufferDC;
			//装载位图
			bitmap.LoadBitmap(IDB_DIAMOND);
			lBufferDC.CreateCompatibleDC(&dc);
			lBufferDC.SelectObject(bitmap);
	
			int xDest,yDest;
			xDest=i*m_dwCardHSpace+4;
			if(!pCardItem->bShoot)
				yDest=m_CardSize.cy/3+m_dwShootAltitude;
			else
				yDest=m_CardSize.cy/3;
			BufferDC.TransparentBlt(xDest,yDest,15,14,&lBufferDC,0,0,15,14,RGB(255,255,255));
		}
		
	}

	//绘画界面
	dc.BitBlt(0,0,rcClient.Width(),rcClient.Height(),&BufferDC,0,0,SRCCOPY);

	//清理资源
	BufferDC.DeleteDC();
	BufferImage.DeleteObject();
    
	return;
}

//建立消息
int CCardControl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//设置背景
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);

	return 0;
}

//鼠标消息
void CCardControl::OnRButtonUp(UINT nFlags, CPoint Point)
{
	__super::OnRButtonUp(nFlags, Point);

	//状态判断
	if ((m_bHorizontal==false)||(m_bPositively==false)) return;

	//发送消息
	if (m_pSinkWindow!=NULL) m_pSinkWindow->PostMessage(IDM_RIGHT_HIT_CARD,0,0);

	return;
}

//鼠标消息
void CCardControl::OnLButtonUp(UINT nFlags, CPoint Point)
{
	__super::OnLButtonUp(nFlags, Point);
	

	//释放鼠标
	ReleaseCapture();

	//状态判断
	if (m_dwCurrentIndex==0xFFFFFFFF) return;
	if ((m_bHorizontal==false)||(m_bPositively==false)) return;
    
	
	//设置变量
	tagCardItem * pCardItem;
	
    m_bChange=false;
	//设置控件
	if(m_dwSelectIndex!=0xFFFFFFFF)
	{
		SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_CARD_CUR)));
		if(m_dwSelectIndex<(DWORD)m_CardDataItem.GetCount())
		{
			pCardItem=&m_CardDataItem[m_dwSelectIndex];
			pCardItem->bSelect=false;
			//错误位置
			if(pCardItem->bShoot)
			{
				//恢复初值
				m_dwSelectIndex=0xFFFFFFFF;
				//调整控件
				RectifyControl();
				return ;
			}

		}
		
		ChangePosition(m_dwSelectIndex);
		m_dwSelectIndex=0xFFFFFFFF;
		//发送消息
		if (m_pSinkWindow!=NULL) m_pSinkWindow->SendMessage(IDM_CHANGE_POSITION,0,0);
		return ;
	}
    
	pCardItem=&m_CardDataItem[m_dwCurrentIndex];
	pCardItem->bShoot=!pCardItem->bShoot;
	
	//发送消息
	if (m_pSinkWindow!=NULL) m_pSinkWindow->SendMessage(IDM_LEFT_HIT_CARD,0,0);

	//调整控件
	RectifyControl();

	return;
}

//鼠标消息
void CCardControl::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags, Point);

	//状态判断
	if ((m_bHorizontal==false)||(m_bPositively==false)) return;

	//获取索引
	m_dwCurrentIndex=SwitchCardPoint(Point);
	//判断下标
	if(m_dwCurrentIndex>=(DWORD)m_CardDataItem.GetCount())
	{	
		m_dwCurrentIndex=0xFFFFFFFF;
		return ;
	}
	tagCardItem * pCardItem=&m_CardDataItem[m_dwCurrentIndex];
	m_bChange=pCardItem->bShoot;
   

	return;
}

//光标消息
BOOL CCardControl::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT message)
{
	//设置光标
	if (m_bPositively==true)
	{
		SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_CARD_CUR)));
		return true;
	}

	return __super::OnSetCursor(pWnd,nHitTest,message);
}

//////////////////////////////////////////////////////////////////////////
//鼠标移动,拖动扑克
void CCardControl::OnMouseMove(UINT nFlags, CPoint point)
{

   CWnd::OnMouseMove(nFlags, point);
   
   //状态判断
   if ((m_bHorizontal==false)||(m_bPositively==false)) return;
   
   if(nFlags&MK_LBUTTON&&m_bChange)
   {
	   //设置光标
	   SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SELECT_CUR)));
	   //拦截鼠标
	   SetCapture();
	
	   if(SwitchCardPoint(point)!=m_dwSelectIndex)
		{
			tagCardItem * pCardItem;
			if(m_dwSelectIndex!=0xFFFFFFFF&&(m_dwSelectIndex<(DWORD)m_CardDataItem.GetCount()))
			{
				pCardItem=&m_CardDataItem[m_dwSelectIndex];
				pCardItem->bSelect=false;
			}
			m_dwSelectIndex=SwitchCardPoint(point);
			if(m_dwSelectIndex<((DWORD)m_CardDataItem.GetCount()))
			{
				pCardItem=&m_CardDataItem[m_dwSelectIndex];
				pCardItem->bSelect=true;
			}
		    //调整控件
			RectifyControl();
		}
   }
}

//改变扑克位置
void CCardControl::ChangePosition(DWORD dwIndex)
{
	//变量定义
	BYTE  bCardData[PIECES],bShootCardData[PIECES];
	DWORD bShootCount=0L,bNotShootCount=0L;
	DWORD dwCardCount=(DWORD)m_CardDataItem.GetCount();
	DWORD bPosition;
    
	//搜索扑克
	tagCardItem * pCardItem=NULL;
	for (DWORD i=0;i<dwCardCount;i++)
	{
		//获取扑克
		pCardItem=&m_CardDataItem[i];
		//记录扑克值
		if(i==dwIndex)	bPosition=bNotShootCount;
			
		//扑克分析
		if (pCardItem->bShoot==true) 
			bShootCardData[bShootCount++]=pCardItem->bCardData;
		else
			bCardData[bNotShootCount++]=pCardItem->bCardData;
	}
    
	//重置扑克
	if(dwIndex==0x0FFFFFFF)
	{
		CopyMemory(&bCardData[bNotShootCount],bShootCardData,bShootCount);
		BYTE bCard=GetMarkCard();
		SetCardData(bCardData,dwCardCount);
		if(bCard!=0)
			MarkCard(bCard,m_bSeries);
	}
	else
	{
		BYTE CardTemp[PIECES];
		CopyMemory(CardTemp,bCardData,bPosition);
		CopyMemory(&CardTemp[bPosition],bShootCardData,bShootCount);
		CopyMemory(&CardTemp[bPosition+bShootCount],&bCardData[bPosition],bNotShootCount-bPosition);
		BYTE bCard=GetMarkCard();
		SetCardData(CardTemp,dwCardCount);
		if(bCard!=0)
			MarkCard(bCard,m_bSeries);
	}

	return ;
}

//获取控件扑克
DWORD CCardControl::GetCardData(BYTE  bCardData[], BYTE bCardCount)
{
	//效验参数
	ASSERT(bCardCount>=(DWORD)m_CardDataItem.GetCount());

	//拷贝扑克
	DWORD  dwCardCount=(DWORD)m_CardDataItem.GetCount();
	tagCardItem *pCardItem;
	for(DWORD i=0;i<dwCardCount;i++)
	{
		pCardItem=&m_CardDataItem[i];
		bCardData[i]=pCardItem->bCardData;
	}


	return dwCardCount;
}


//标志可进贡扑克
void CCardControl::MarkCard(BYTE bCard,BYTE bSeries)
{
	//定义变量
	CGameLogic  lGameLogic;
	tagCardItem *pCardItem;
	
	//分析处理
	DWORD  dwCardCount=(DWORD)m_CardDataItem.GetCount();
	for(DWORD i=0;i<dwCardCount;i++)
	{
		pCardItem=&m_CardDataItem[i];
		if(lGameLogic.GetCardLogicValue(pCardItem->bCardData)==lGameLogic.GetCardLogicValue(bCard)
			&&pCardItem->bCardData!=bSeries)
		{
			pCardItem->bMark	=	true;
		}
	}

	Invalidate(true);
}

//获取标记扑克
BYTE CCardControl::GetMarkCard()
{
	tagCardItem *pCardItem;
	DWORD dwCardCount=(DWORD)m_CardDataItem.GetCount();
	for(DWORD i=0;i<dwCardCount;i++)
	{
		pCardItem=&m_CardDataItem[i];
		if(pCardItem->bMark)
		{
			return pCardItem->bCardData;
		}
	}

	return 0;
}
