#include "StdAfx.h"
#include "GameClient.h"
#include "CardControl.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CCardControl::CCardControl()
{
	//状态变量
	m_bHorizontal=true;
	m_bPositively=false;
	m_bDisplayItem=false;

	//扑克数据
	m_cbCardCount=0;
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
	m_ImageCard.SetLoadInfo(IDB_CARD,AfxGetInstanceHandle());
	m_ImageCardMask.SetLoadInfo(IDB_CARD_MASK,AfxGetInstanceHandle());
	m_ImageWin.SetLoadInfo(IDB_MAX,AfxGetInstanceHandle());

	//获取大小
	CImageHandle HandleImage(&m_ImageCard);
	m_CardSize.SetSize(m_ImageCard.GetWidth()/13,m_ImageCard.GetHeight()/5);

	return;
}

//析构函数
CCardControl::~CCardControl()
{
}

//设置扑克
bool CCardControl::SetCardData(BYTE cbCardCount)
{
	//效验参数
	ASSERT(cbCardCount<=CountArray(m_CardItemArray));
	if (cbCardCount>CountArray(m_CardItemArray)) return false;

	//设置变量
	m_cbCardCount=cbCardCount;
	ZeroMemory(m_CardItemArray,sizeof(m_CardItemArray));

	return true;
}

//设置扑克
bool CCardControl::SetCardData(const BYTE cbCardData[], BYTE cbCardCount)
{
	//效验参数
	ASSERT(cbCardCount<=CountArray(m_CardItemArray));
	if (cbCardCount>CountArray(m_CardItemArray)) return false;

	//设置变量
	m_cbCardCount=cbCardCount;

	//设置扑克
	for (WORD i=0;i<cbCardCount;i++)
	{
		m_CardItemArray[i].bShoot=false;
		m_CardItemArray[i].bEffect = false;
		m_CardItemArray[i].bMy = false;
		m_CardItemArray[i].cbCardData=cbCardData[i];
	}

	return true;
}

//设置扑克
bool CCardControl::SetShootCard(const BYTE cbCardData[], BYTE cbCardCount)
{
	//变量定义
	bool bChangeStatus=false;

	//收起扑克
	for (WORD i=0;i<m_cbCardCount;i++) 
	{
		if (m_CardItemArray[i].bShoot==true)
		{
			bChangeStatus=true;
			m_CardItemArray[i].bShoot=false;
		}
	}

	//弹起扑克
	for (WORD i=0;i<cbCardCount;i++)
	{
		for (WORD j=0;j<m_cbCardCount;j++)
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

//设置特效
bool CCardControl::SetCardEffect(const BYTE cbCardData[],BYTE cbCardCount)
{
	//变量定义
	BYTE bTempCount=0;

	//弹起扑克
	for (WORD i=0;i<cbCardCount;i++)
	{
		for (WORD j=0;j<m_cbCardCount;j++)
		{
			if ((m_CardItemArray[j].bEffect==false)&&(m_CardItemArray[j].cbCardData==cbCardData[i])) 
			{
				bTempCount++;
				m_CardItemArray[j].bEffect=true;
				break;
			}
		}
	}

	return (bTempCount==cbCardCount);
}

//设置标志
bool CCardControl::SetMyCard(const BYTE cbCardData[],BYTE cbCardCount)
{
	//变量定义
	BYTE bTempCount=0;

	//标志扑克
	for (WORD i=0;i<cbCardCount;i++)
	{
		for (WORD j=0;j<m_cbCardCount;j++)
		{
			if ((m_CardItemArray[j].bMy==false)&&(m_CardItemArray[j].cbCardData==cbCardData[i])) 
			{
				bTempCount++;
				m_CardItemArray[j].bMy=true;
				break;
			}
		}
	}

	return (bTempCount==cbCardCount);
}

//设置扑克
bool CCardControl::SetCardItem(const tagCardItem CardItemArray[], BYTE cbCardCount)
{
	//效验参数
	ASSERT(cbCardCount<=CountArray(m_CardItemArray));
	if (cbCardCount>CountArray(m_CardItemArray)) return false;

	//设置扑克
	m_cbCardCount=cbCardCount;
	CopyMemory(m_CardItemArray,CardItemArray,cbCardCount*sizeof(tagCardItem));

	return true;
}

//获取扑克
tagCardItem * CCardControl::GetCardFromIndex(BYTE cbIndex)
{
	return (cbIndex<m_cbCardCount)?&m_CardItemArray[cbIndex]:NULL;
}

//获取扑克
tagCardItem * CCardControl::GetCardFromPoint(CPoint & MousePoint)
{
	WORD wIndex=SwitchCardPoint(MousePoint);
	return (wIndex!=INVALID_ITEM)?&m_CardItemArray[wIndex]:NULL;
}

//获取扑克
WORD CCardControl::GetCardData(BYTE cbCardData[], BYTE cbBufferCount)
{
	if(cbBufferCount==0) return 0;
	//效验参数
	ASSERT(cbBufferCount>=m_cbCardCount);
	if (cbBufferCount<m_cbCardCount) return 0;

	//拷贝扑克
	for (WORD i=0;i<m_cbCardCount;i++) cbCardData[i]=m_CardItemArray[i].cbCardData;

	return m_cbCardCount;
}

//获取扑克
WORD CCardControl::GetShootCard(BYTE cbCardData[], BYTE cbBufferCount)
{
	//变量定义
	WORD wShootCount=0;

	//拷贝扑克
	for (WORD i=0;i<m_cbCardCount;i++) 
	{
		//效验参数
		ASSERT(cbBufferCount>wShootCount);
		if (cbBufferCount<=wShootCount) break;

		//拷贝扑克
		if (m_CardItemArray[i].bShoot==true) cbCardData[wShootCount++]=m_CardItemArray[i].cbCardData;
	}

	return wShootCount;
}

//获取扑克
WORD CCardControl::GetCardData(tagCardItem CardItemArray[], BYTE cbBufferCount)
{
	//效验参数
	ASSERT(cbBufferCount>=m_cbCardCount);
	if (cbBufferCount<m_cbCardCount) return 0;

	//拷贝扑克
	CopyMemory(CardItemArray,m_CardItemArray,sizeof(tagCardItem)*m_cbCardCount);

	return m_cbCardCount;
}

//设置距离
VOID CCardControl::SetCardDistance(UINT nXDistance, UINT nYDistance, UINT nShootDistance)
{
	//设置变量
	m_nXDistance=nXDistance;
	m_nYDistance=nYDistance;
	m_nShootDistance=nShootDistance;

	return;
}

//获取中心
VOID CCardControl::GetCenterPoint(CPoint & CenterPoint)
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
VOID CCardControl::SetBenchmarkPos(INT nXPos, INT nYPos, enXCollocateMode XCollocateMode, enYCollocateMode YCollocateMode)
{
	//设置变量
	m_BenchmarkPos.x=nXPos;
	m_BenchmarkPos.y=nYPos;
	m_XCollocateMode=XCollocateMode;
	m_YCollocateMode=YCollocateMode;

	return;
}

//基准位置
VOID CCardControl::SetBenchmarkPos(const CPoint & BenchmarkPos, enXCollocateMode XCollocateMode, enYCollocateMode YCollocateMode)
{
	//设置变量
	m_BenchmarkPos=BenchmarkPos;
	m_XCollocateMode=XCollocateMode;
	m_YCollocateMode=YCollocateMode;

	return;
}

//绘画扑克
VOID CCardControl::DrawCardControl(CDC * pDC,bool ISbEffect)
{
	//加载位图
	CImageHandle HandleCard(&m_ImageCard);

	//扑克掩图
	CImageHandle ImageCardMaskHandle(&m_ImageCardMask);
	CImageHandle ImageWinHandle(&m_ImageWin);

	//掩图大小
	int nCardMaskWidth = m_ImageCardMask.GetWidth();
	int nCardMaskHeight = m_ImageCardMask.GetHeight();

	//获取位置
	CPoint OriginPoint;
	GetOriginPoint(OriginPoint);

	//变量定义
	INT nXDrawPos=0,nYDrawPos=0;
	INT nXImagePos=0,nYImagePos=0;

	//绘画扑克
	for (WORD i=0;i<m_cbCardCount;i++)
	{
		//获取扑克
		bool bShoot=m_CardItemArray[i].bShoot;
		bool bEffect = m_CardItemArray[i].bEffect;
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
			nXImagePos=m_CardSize.cx*2;
			nYImagePos=m_CardSize.cy*4;
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
		if(bEffect&&ISbEffect)
			m_ImageCardMask.AlphaDrawImage(pDC,OriginPoint.x+nXDrawPos-2,OriginPoint.y+nYDrawPos-2,nCardMaskWidth,nCardMaskHeight,0,0,RGB(255,0,255));

		//扑克标志
		if(m_CardItemArray[i].bMy)
			m_ImageWin.AlphaDrawImage(pDC,OriginPoint.x+nXDrawPos,OriginPoint.y+m_CardSize.cy+((m_YCollocateMode==enYTop)?(m_CardSize.cy/2-m_ImageWin.GetHeight()):0)-m_ImageWin.GetHeight(),
				m_ImageWin.GetWidth(),m_ImageWin.GetHeight(),0,0,RGB(255,0,255));
	}

	return;
}

//光标消息
bool CCardControl::OnEventSetCursor(CPoint Point)
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
VOID CCardControl::GetControlSize(CSize & ControlSize)
{
	//获取大小
	if (m_bHorizontal==true)
	{
		ControlSize.cy=m_CardSize.cy+m_nShootDistance;
		ControlSize.cx=(m_cbCardCount>0)?(m_CardSize.cx+(m_cbCardCount-1)*m_nXDistance):0;
	}
	else
	{
		ControlSize.cx=m_CardSize.cx;
		ControlSize.cy=(m_cbCardCount>0)?(m_CardSize.cy+(m_cbCardCount-1)*m_nYDistance):0;
	}

	return;
}

//获取原点
VOID CCardControl::GetOriginPoint(CPoint & OriginPoint)
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
WORD CCardControl::SwitchCardPoint(CPoint & MousePoint)
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
		if (wCardIndex>=m_cbCardCount) wCardIndex=(m_cbCardCount-1);

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
//////////////////////////////////////////////////////////////////////////
