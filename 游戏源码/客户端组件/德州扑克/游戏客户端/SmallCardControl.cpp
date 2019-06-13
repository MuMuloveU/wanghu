#include "StdAfx.h"
#include "GameClient.h"
#include "SmallCardControl.h"

//////////////////////////////////////////////////////////////////////////

CSkinImage		 CSmallCardControl::m_ImageCard;
//////////////////////////////////////////////////////////////////////////

//构造函数
CSmallCardControl::CSmallCardControl()
{
	//状态变量
	m_bHorizontal=true;
	m_bPositively=false;
	m_bDisplayItem=false;

	//扑克数据
	m_wCardCount=0;
	ZeroMemory(m_CardItemArray,sizeof(m_CardItemArray));

	//间隔变量
	m_nXDistance=DEF_X_DISTANCE;
	m_nYDistance=DEF_Y_DISTANCE;
	m_nShootDistance=DEF_SHOOT_DISTANCE;

	//位置变量
	m_YCollocateMode=enYTop;
	m_XCollocateMode=enXLeft;
	m_BenchmarkPos.SetPoint(0,0);

	//加载资源
	m_ImageCard.SetLoadInfo(IDB_SMALL_CARD,AfxGetInstanceHandle());

	//获取大小
	CImageHandle HandleImage(&m_ImageCard);
	m_CardSize.SetSize(m_ImageCard.GetWidth(),m_ImageCard.GetHeight());

	return;
}

//析构函数
CSmallCardControl::~CSmallCardControl()
{
}

//设置扑克
bool CSmallCardControl::SetCardData(WORD wCardCount)
{
	//ASSERT(wCardCount>=m_wCardCount);
	//if(m_wCardCount==2 && (wCardCount>=m_wCardCount))return false;

	//效验参数
	ASSERT(wCardCount<=CountArray(m_CardItemArray));
	if (wCardCount>CountArray(m_CardItemArray)) return false;

	//设置变量
	m_wCardCount=wCardCount;
	ZeroMemory(m_CardItemArray,sizeof(m_CardItemArray));

	return true;
}

//设置扑克
bool CSmallCardControl::SetCardData(const BYTE cbCardData[], WORD wCardCount)
{
	//效验参数
	ASSERT(wCardCount<=CountArray(m_CardItemArray));
	if (wCardCount>CountArray(m_CardItemArray)) return false;

	//设置变量
	m_wCardCount=wCardCount;

	//设置扑克
	for (WORD i=0;i<wCardCount;i++)
	{
		m_CardItemArray[i].bShoot=false;
		m_CardItemArray[i].cbCardData=cbCardData[i];
	}

	return true;
}

//设置扑克
bool CSmallCardControl::SetShootCard(const BYTE cbCardData[], WORD wCardCount)
{
	//变量定义
	bool bChangeStatus=false;

	//收起扑克
	for (WORD i=0;i<m_wCardCount;i++) 
	{
		if (m_CardItemArray[i].bShoot==true)
		{
			bChangeStatus=true;
			m_CardItemArray[i].bShoot=false;
		}
	}

	//弹起扑克
	for (WORD i=0;i<wCardCount;i++)
	{
		for (WORD j=0;j<m_wCardCount;j++)
		{
			if ((m_CardItemArray[j].bShoot==false)&&(m_CardItemArray[j].cbCardData==cbCardData[i])) 
			{
				bChangeStatus=true;
				m_CardItemArray[j].bShoot=true;
				break;
			}
		}
	}

	return bChangeStatus;
}

//设置扑克
bool CSmallCardControl::SetCardItem(const tagCardItem CardItemArray[], WORD wCardCount)
{
	//效验参数
	ASSERT(wCardCount<=CountArray(m_CardItemArray));
	if (wCardCount>CountArray(m_CardItemArray)) return false;

	//设置扑克
	m_wCardCount=wCardCount;
	CopyMemory(m_CardItemArray,CardItemArray,wCardCount*sizeof(tagCardItem));

	return true;
}

//获取扑克
tagCardItem * CSmallCardControl::GetCardFromIndex(WORD wIndex)
{
	return (wIndex<m_wCardCount)?&m_CardItemArray[wIndex]:NULL;
}

//获取扑克
tagCardItem * CSmallCardControl::GetCardFromPoint(CPoint & MousePoint)
{
	WORD wIndex=SwitchCardPoint(MousePoint);
	return (wIndex!=INVALID_ITEM)?&m_CardItemArray[wIndex]:NULL;
}

//获取扑克
WORD CSmallCardControl::GetCardData(BYTE cbCardData[], WORD wBufferCount)
{
	//效验参数
	ASSERT(wBufferCount>=m_wCardCount);
	if (wBufferCount<m_wCardCount) return 0;

	//拷贝扑克
	for (WORD i=0;i<m_wCardCount;i++) cbCardData[i]=m_CardItemArray[i].cbCardData;

	return m_wCardCount;
}

//获取扑克
WORD CSmallCardControl::GetShootCard(BYTE cbCardData[], WORD wBufferCount)
{
	//变量定义
	WORD wShootCount=0;

	//拷贝扑克
	for (WORD i=0;i<m_wCardCount;i++) 
	{
		//效验参数
		ASSERT(wBufferCount>wShootCount);
		if (wBufferCount<=wShootCount) break;

		//拷贝扑克
		if (m_CardItemArray[i].bShoot==true) cbCardData[wShootCount++]=m_CardItemArray[i].cbCardData;
	}

	return wShootCount;
}

//获取扑克
WORD CSmallCardControl::GetCardData(tagCardItem CardItemArray[], WORD wBufferCount)
{
	//效验参数
	ASSERT(wBufferCount>=m_wCardCount);
	if (wBufferCount<m_wCardCount) return 0;

	//拷贝扑克
	CopyMemory(CardItemArray,m_CardItemArray,sizeof(tagCardItem)*m_wCardCount);

	return m_wCardCount;
}

//设置距离
VOID CSmallCardControl::SetCardDistance(UINT nXDistance, UINT nYDistance, UINT nShootDistance)
{
	//设置变量
	m_nXDistance=nXDistance;
	m_nYDistance=nYDistance;
	m_nShootDistance=nShootDistance;

	return;
}

//获取中心
VOID CSmallCardControl::GetCenterPoint(CPoint & CenterPoint)
{
	//获取原点
	CPoint OriginPoint;
	GetOriginPoint(OriginPoint);

	//获取位置
	CSize ControlSize;
	GetControlSize(ControlSize);

	//设置中心
	CenterPoint.x=OriginPoint.x+ControlSize.cx/2;
	CenterPoint.y=OriginPoint.y+ControlSize.cy/2;

	return;
}

//基准位置
VOID CSmallCardControl::SetBenchmarkPos(INT nXPos, INT nYPos, enXCollocateMode XCollocateMode, enYCollocateMode YCollocateMode)
{
	//设置变量
	m_BenchmarkPos.x=nXPos;
	m_BenchmarkPos.y=nYPos;
	m_XCollocateMode=XCollocateMode;
	m_YCollocateMode=YCollocateMode;

	return;
}

//基准位置
VOID CSmallCardControl::SetBenchmarkPos(const CPoint & BenchmarkPos, enXCollocateMode XCollocateMode, enYCollocateMode YCollocateMode)
{
	//设置变量
	m_BenchmarkPos=BenchmarkPos;
	m_XCollocateMode=XCollocateMode;
	m_YCollocateMode=YCollocateMode;

	return;
}

//绘画扑克
VOID CSmallCardControl::DrawCardControl(CDC * pDC)
{
	//加载位图
	CImageHandle HandleCard(&m_ImageCard);

	//获取位置
	CPoint OriginPoint;
	GetOriginPoint(OriginPoint);

	//变量定义
	INT nXDrawPos=0,nYDrawPos=0;
	INT nXImagePos=0,nYImagePos=0;

	//绘画扑克
	for (WORD i=0;i<m_wCardCount;i++)
	{
		//获取扑克
		bool bShoot=m_CardItemArray[i].bShoot;
		BYTE cbCardData=m_CardItemArray[i].cbCardData;

		//间隙过滤
		if (cbCardData==SPACE_CARD_DATA) continue;

		//图片位置
		if ((m_bDisplayItem==true)&&(cbCardData!=0))
		{
			if ((cbCardData==0x4E)||(cbCardData==0x4F))
			{
				nXImagePos=((cbCardData&CARD_MASK_VALUE)%14)*m_CardSize.cx;
				nYImagePos=((cbCardData&CARD_MASK_COLOR)>>4)*m_CardSize.cy;
			}
			else
			{
				nXImagePos=((cbCardData&CARD_MASK_VALUE)-1)*m_CardSize.cx;
				nYImagePos=((cbCardData&CARD_MASK_COLOR)>>4)*m_CardSize.cy;
			}
		}
		else
		{
			nXImagePos=0;
			nYImagePos=0;
		}

		//屏幕位置
		if (m_bHorizontal==true)
		{
			nXDrawPos=m_nXDistance*i;
			nYDrawPos=(bShoot==false)?m_nShootDistance:0;
		}
		else
		{
			nXDrawPos=0;
			nYDrawPos=m_nYDistance*i;
		}

		//绘画扑克
		m_ImageCard.AlphaDrawImage(pDC,OriginPoint.x+nXDrawPos,OriginPoint.y+nYDrawPos,m_CardSize.cx,m_CardSize.cy,nXImagePos,nYImagePos,RGB(255,0,255));
	}

	return;
}

//光标消息
bool CSmallCardControl::OnEventSetCursor(CPoint Point)
{
	//光标处理
	if (m_bPositively==true)
	{
		//获取索引
		WORD wHoverItem=SwitchCardPoint(Point);

		//更新判断
		if (wHoverItem!=INVALID_ITEM)
		{
			SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_CARD_CUR)));
			return true;
		}
	}

	return false;
}

//获取大小
VOID CSmallCardControl::GetControlSize(CSize & ControlSize)
{
	//获取大小
	if (m_bHorizontal==true)
	{
		ControlSize.cy=m_CardSize.cy+m_nShootDistance;
		ControlSize.cx=(m_wCardCount>0)?(m_CardSize.cx+(m_wCardCount-1)*m_nXDistance):0;
	}
	else
	{
		ControlSize.cx=m_CardSize.cx;
		ControlSize.cy=(m_wCardCount>0)?(m_CardSize.cy+(m_wCardCount-1)*m_nYDistance):0;
	}

	return;
}

//获取原点
VOID CSmallCardControl::GetOriginPoint(CPoint & OriginPoint)
{
	//获取位置
	CSize ControlSize;
	GetControlSize(ControlSize);

	//横向位置
	switch (m_XCollocateMode)
	{
	case enXLeft:	{ OriginPoint.x=m_BenchmarkPos.x; break; }
	case enXCenter: { OriginPoint.x=m_BenchmarkPos.x-ControlSize.cx/2; break; }
	case enXRight:	{ OriginPoint.x=m_BenchmarkPos.x-ControlSize.cx; break; }
	}

	//竖向位置
	switch (m_YCollocateMode)
	{
	case enYTop:	{ OriginPoint.y=m_BenchmarkPos.y; break; }
	case enYCenter: { OriginPoint.y=m_BenchmarkPos.y-ControlSize.cy/2; break; }
	case enYBottom: { OriginPoint.y=m_BenchmarkPos.y-ControlSize.cy; break; }
	}

	return;
}

//索引切换
WORD CSmallCardControl::SwitchCardPoint(CPoint & MousePoint)
{
	//获取位置
	CSize ControlSize;
	CPoint OriginPoint;
	GetControlSize(ControlSize);
	GetOriginPoint(OriginPoint);

	//基准位置
	INT nXPos=MousePoint.x-OriginPoint.x;
	INT nYPos=MousePoint.y-OriginPoint.y;

	//横向模式
	if (m_bHorizontal==true)
	{
		//越界判断
		if ((nXPos<=0)||(nXPos>=ControlSize.cx)) return INVALID_ITEM;
		if ((nYPos<=0)||(nYPos>=ControlSize.cy)) return INVALID_ITEM;

		//计算索引
		WORD wCardIndex=nXPos/m_nXDistance;
		if (wCardIndex>=m_wCardCount) wCardIndex=(m_wCardCount-1);

		//扑克搜索
		for (WORD i=0;i<=wCardIndex;i++)
		{
			//变量定义
			WORD wCurrentIndex=wCardIndex-i;

			//横向测试
			if (nXPos>=(INT)(wCurrentIndex*m_nXDistance+m_CardSize.cx)) break;

			//竖向测试
			bool bShoot=m_CardItemArray[wCurrentIndex].bShoot;
			if ((bShoot==true)&&(nYPos<=m_CardSize.cy)) return wCurrentIndex;
			if ((bShoot==false)&&(nYPos>=(INT)m_nShootDistance)) return wCurrentIndex;
		}
	}

	return INVALID_ITEM;
}

// 在指定位置绘制一张牌, bCardData为0时绘制背面
void CSmallCardControl::DrawOneCard(CDC* pDC, BYTE bCardData,int nX , int nY  )
{
	//变量定义

	COLORREF clrTrans = RGB( 255, 0, 255 );
	CImageHandle HandleCard(&m_ImageCard);
    
	m_ImageCard.AlphaDrawImage( pDC, nX, nY, SMALL_CARD_WIDTH, SMALL_CARD_HEIGHT
			, 0, 0
			, clrTrans );
	return;

}
//////////////////////////////////////////////////////////////////////////
