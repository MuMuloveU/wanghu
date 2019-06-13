#include "StdAfx.h"
#include "GameClient.h"
#include "CardControl.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CCardControl::CCardControl()
{
	//状态变量
	m_bSmallMode=false;
	m_bHorizontal=true;
	m_bPositively=false;
	m_bDisplayItem=false;

	//选择变量
	m_wEndIndex=INVALID_ITEM;
	m_wStartIndex=INVALID_ITEM;

	//扑克数据
	m_wCardCount=0;
	ZeroMemory(m_CardItemArray,sizeof(m_CardItemArray));

	//位置变量
	m_YCollocateMode=enYTop;
	m_XCollocateMode=enXLeft;
	m_BenchmarkPos.SetPoint(0,0);

	//间隔变量
	m_nXDistance=(m_bSmallMode==false)?DEF_X_DISTANCE:DEF_X_DISTANCE_SMALL;
	m_nYDistance=(m_bSmallMode==false)?DEF_Y_DISTANCE:DEF_Y_DISTANCE_SMALL;
	m_nShootDistance=(m_bSmallMode==false)?DEF_SHOOT_DISTANCE:DEF_SHOOT_DISTANCE_SMALL;

	//加载资源
	HINSTANCE hResInstance=AfxGetInstanceHandle();
	m_ImageCard.SetLoadInfo((m_bSmallMode==false)?IDB_GAME_CARD:IDB_GAME_CARD_SMALL,hResInstance);

	//获取大小
	CImageHandle HandleCard(&m_ImageCard);
	m_CardSize.SetSize(m_ImageCard.GetWidth()/13,m_ImageCard.GetHeight()/5);

	return;
}

//析构函数
CCardControl::~CCardControl()
{
}

//设置扑克
bool CCardControl::SetCardData(WORD wCardCount)
{
	//效验参数
	ASSERT(wCardCount<=CountArray(m_CardItemArray));
	if (wCardCount>CountArray(m_CardItemArray)) return false;

	//设置变量
	m_wCardCount=wCardCount;
	ZeroMemory(m_CardItemArray,sizeof(m_CardItemArray));

	return true;
}

//设置模式
bool CCardControl::SetCardMode(bool bSmallMode)
{
	//设置模式
	if (m_bSmallMode!=bSmallMode)
	{
		//设置变量
		m_bSmallMode=bSmallMode;

		//设置间距
		m_nXDistance=(m_bSmallMode==false)?DEF_X_DISTANCE:DEF_X_DISTANCE_SMALL;
		m_nYDistance=(m_bSmallMode==false)?DEF_Y_DISTANCE:DEF_Y_DISTANCE_SMALL;
		m_nShootDistance=(m_bSmallMode==false)?DEF_SHOOT_DISTANCE:DEF_SHOOT_DISTANCE_SMALL;

		//加载资源
		HINSTANCE hResInstance=AfxGetInstanceHandle();
		m_ImageCard.SetLoadInfo((m_bSmallMode==false)?IDB_GAME_CARD:IDB_GAME_CARD_SMALL,hResInstance);

		//获取大小
		CImageHandle HandleCard(&m_ImageCard);
		m_CardSize.SetSize(m_ImageCard.GetWidth()/13,m_ImageCard.GetHeight()/5);
	}

	return true;
}

//设置扑克
bool CCardControl::SetCardData(const BYTE cbCardData[], WORD wCardCount)
{
	//效验参数
	ASSERT(wCardCount<=CountArray(m_CardItemArray));
	if (wCardCount>CountArray(m_CardItemArray)) return false;

	//设置变量
	m_wEndIndex=INVALID_ITEM;
	m_wStartIndex=INVALID_ITEM;

	//扑克数目
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
bool CCardControl::SetCardItem(const tagCardItem CardItemArray[], WORD wCardCount)
{
	//效验参数
	ASSERT(wCardCount<=CountArray(m_CardItemArray));
	if (wCardCount>CountArray(m_CardItemArray)) return false;

	//设置变量
	m_wEndIndex=INVALID_ITEM;
	m_wStartIndex=INVALID_ITEM;

	//设置扑克
	m_wCardCount=wCardCount;
	CopyMemory(m_CardItemArray,CardItemArray,wCardCount*sizeof(tagCardItem));

	return true;
}

//设置扑克
bool CCardControl::SetShootIndex(WORD wStartIndex, WORD wEndIndex)
{
	//设置变量
	m_wEndIndex=INVALID_ITEM;
	m_wStartIndex=INVALID_ITEM;

	//设置扑克
	for (WORD i=0;i<m_wCardCount;i++)
	{
		if ((i>=wStartIndex)&&(i<=wEndIndex))
		{
			m_CardItemArray[i].bShoot=true;
		}
	}

	return true;
}

//设置扑克
bool CCardControl::SetSelectIndex(WORD wStartIndex, WORD wEndIndex)
{
	//设置变量
	m_wEndIndex=wEndIndex;
	m_wStartIndex=wStartIndex;

	return true;
}

//设置扑克
bool CCardControl::SetShootCard(const BYTE cbCardData[], WORD wCardCount)
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

//删除扑克
bool CCardControl::RemoveShootItem()
{
	//设置变量
	m_wEndIndex=INVALID_ITEM;
	m_wStartIndex=INVALID_ITEM;

	//保存扑克
	tagCardItem	CardItemArray[MAX_CARD_COUNT];
	CopyMemory(CardItemArray,m_CardItemArray,sizeof(tagCardItem)*m_wCardCount);

	//删除扑克
	WORD wRemoveCount=0;
	for (WORD i=0;i<m_wCardCount;i++)
	{
		if (CardItemArray[i].bShoot==true)
		{
			wRemoveCount++;
			CardItemArray[i].cbCardData=0x00;
		}
	}

	//设置扑克
	if (wRemoveCount>0)
	{
		//设置扑克
		WORD wInsertCount=0;
		for (WORD i=0;i<m_wCardCount;i++)
		{
			if (CardItemArray[i].cbCardData!=0x00)
			{
				m_CardItemArray[wInsertCount++]=CardItemArray[i];
			}
		}

		//设置变量
		m_wCardCount=m_wCardCount-wRemoveCount;
	}

	return true;
}

//获取扑克
tagCardItem * CCardControl::GetCardFromIndex(WORD wIndex)
{
	return (wIndex<m_wCardCount)?&m_CardItemArray[wIndex]:NULL;
}

//获取扑克
tagCardItem * CCardControl::GetCardFromPoint(CPoint & MousePoint)
{
	WORD wIndex=SwitchCardPoint(MousePoint);
	return (wIndex!=INVALID_ITEM)?&m_CardItemArray[wIndex]:NULL;
}

//获取扑克
WORD CCardControl::GetCardData(BYTE cbCardData[], WORD wBufferCount)
{
	//效验参数
	ASSERT(wBufferCount>=m_wCardCount);
	if (wBufferCount<m_wCardCount) return 0;

	//拷贝扑克
	for (WORD i=0;i<m_wCardCount;i++) cbCardData[i]=m_CardItemArray[i].cbCardData;

	return m_wCardCount;
}

//获取扑克
WORD CCardControl::GetShootCard(BYTE cbCardData[], WORD wBufferCount)
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
WORD CCardControl::GetCardData(tagCardItem CardItemArray[], WORD wBufferCount)
{
	//效验参数
	ASSERT(wBufferCount>=m_wCardCount);
	if (wBufferCount<m_wCardCount) return 0;

	//拷贝扑克
	CopyMemory(CardItemArray,m_CardItemArray,sizeof(tagCardItem)*m_wCardCount);

	return m_wCardCount;
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
VOID CCardControl::DrawCardControl(CDC * pDC)
{
	//加载位图
	CPngImage ImageCardMask;
	CImageHandle HandleCard(&m_ImageCard);

	//获取位置
	CPoint OriginPoint;
	GetOriginPoint(OriginPoint);

	//变量定义
	INT nXDrawPos=0,nYDrawPos=0;
	INT nXImagePos=0,nYImagePos=0;

	//加载资源
	CPngImage ImageCardSelect;
	if ((m_wStartIndex<m_wCardCount)&&(m_wEndIndex<m_wCardCount))
	{
		ImageCardSelect.LoadImage(AfxGetInstanceHandle(),TEXT("CARD_SELECT"));
	}

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

		//绘画选择
		if ((m_wStartIndex<=i)&&(i<=m_wEndIndex))
		{
			CSize SizeSelectImage(ImageCardSelect.GetWidth(),ImageCardSelect.GetHeight());
			ImageCardSelect.DrawImage(pDC,OriginPoint.x+nXDrawPos,OriginPoint.y+nYDrawPos,SizeSelectImage.cx,SizeSelectImage.cy,0,0);
		}
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
	//横向模式
	if ((m_bHorizontal==true)&&(m_wCardCount>0))
	{
		//获取位置
		CSize ControlSize;
		CPoint OriginPoint;
		GetControlSize(ControlSize);
		GetOriginPoint(OriginPoint);

		//基准位置
		INT nXPos=MousePoint.x-OriginPoint.x;
		INT nYPos=MousePoint.y-OriginPoint.y;

		//越界判断
		if ((nXPos<0)||(nXPos>ControlSize.cx)) return INVALID_ITEM;
		if ((nYPos<0)||(nYPos>ControlSize.cy)) return INVALID_ITEM;

		//计算索引
		WORD wCardIndex=nXPos/m_nXDistance;
		if (wCardIndex>=m_wCardCount) wCardIndex=(m_wCardCount-1);

		//扑克搜索
		for (WORD i=0;i<=wCardIndex;i++)
		{
			//变量定义
			WORD wCurrentIndex=wCardIndex-i;

			//横向测试
			if (nXPos>(INT)(wCurrentIndex*m_nXDistance+m_CardSize.cx)) break;

			//竖向测试
			bool bShoot=m_CardItemArray[wCurrentIndex].bShoot;
			if ((bShoot==true)&&(nYPos<=m_CardSize.cy)) return wCurrentIndex;
			if ((bShoot==false)&&(nYPos>=(INT)m_nShootDistance)) return wCurrentIndex;
		}
	}

	return INVALID_ITEM;
}

//移动扑克
bool CCardControl::MoveCardItem(WORD wTargerItem)
{
	//效验参数
	ASSERT(wTargerItem<m_wCardCount);
	if (wTargerItem>=m_wCardCount) return false;

	//统计扑克
	WORD wShootCount=0;
	for (WORD i=0;i<m_wCardCount;i++)
	{
		if (m_CardItemArray[i].bShoot==true)
		{
			wShootCount++;
		}
	}

	//移动扑克
	if ((wShootCount>0)&&(wShootCount<m_wCardCount))
	{
		//调整索引
		wTargerItem=__min(m_wCardCount-wShootCount,wTargerItem);

		//变量定义
		WORD wShootIndex=wTargerItem;
		WORD wNormalIndex=(wTargerItem==0)?(wTargerItem+wShootCount):0;

		//提取扑克
		tagCardItem CardItemTemp[MAX_CARD_COUNT];
		CopyMemory(CardItemTemp,m_CardItemArray,sizeof(tagCardItem)*m_wCardCount);

		//调整扑克
		for (WORD i=0;i<m_wCardCount;i++)
		{
			if (CardItemTemp[i].bShoot==false)
			{
				//设置扑克
				m_CardItemArray[wNormalIndex++]=CardItemTemp[i];

				//调整索引
				if (wNormalIndex==wTargerItem) wNormalIndex=wTargerItem+wShootCount;
			}
			else
			{
				//设置扑克
				m_CardItemArray[wShootIndex++]=CardItemTemp[i];
			}
		}

		return true;
	}

	return false;
}
//交换扑克
bool CCardControl::SwitchCardItem(WORD wSourceItem, WORD wTargerItem)
{
	//效验参数
	if (wSourceItem==wTargerItem) return false;
	if ((wSourceItem>=m_wCardCount)||(wTargerItem>=m_wCardCount)) return false;

	//保存扑克
	tagCardItem CardItem=m_CardItemArray[wSourceItem];

	//移动扑克
	if (wSourceItem>wTargerItem)
	{
		WORD wMoveCount=wSourceItem-wTargerItem;
		MoveMemory(&m_CardItemArray[wTargerItem+1],&m_CardItemArray[wTargerItem],sizeof(tagCardItem)*wMoveCount);
	}
	else
	{
		WORD wMoveCount=wTargerItem-wSourceItem;
		MoveMemory(&m_CardItemArray[wSourceItem],&m_CardItemArray[wSourceItem+1],sizeof(tagCardItem)*wMoveCount);
	}

	//插入目标
	m_CardItemArray[wTargerItem]=CardItem;

	return true;
}
//////////////////////////////////////////////////////////////////////////
