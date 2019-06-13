#include "StdAfx.h"
#include "Resource.h"
#include "CardControl.h"
#include "..\\游戏服务器\\GameLogic.h"
#include "DrawFunc.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

//公共定义
#define POS_SHOOT					5									//弹起象素
#define POS_SPACE					8									//分隔间隔
#define ITEM_COUNT					43									//子项数目
#define INVALID_ITEM				0xFFFF								//无效索引

//扑克大小
#define CARD_WIDTH					42									//扑克宽度
#define CARD_HEIGHT					60									//扑克高度

//////////////////////////////////////////////////////////////////////////

//构造函数
CCardListImage::CCardListImage()
{
	//位置变量
	m_nItemWidth=0;
	m_nItemHeight=0;
	m_nViewWidth=0;
	m_nViewHeight=0;

	return;
}

//析构函数
CCardListImage::~CCardListImage()
{
}

//加载资源
bool CCardListImage::LoadResource(UINT uResourceID, int nViewWidth, int nViewHeight)
{
	//加载资源
	m_CardListImage.LoadFromResource(AfxGetInstanceHandle(),uResourceID);

	//设置变量
	m_nViewWidth=nViewWidth;
	m_nViewHeight=nViewHeight;
	m_nItemHeight=m_CardListImage.GetHeight();
	m_nItemWidth=m_CardListImage.GetWidth()/ITEM_COUNT;

	return true;
}

//释放资源
bool CCardListImage::DestroyResource()
{
	//设置变量
	m_nItemWidth=0;
	m_nItemHeight=0;

	//释放资源
	m_CardListImage.Destroy();

	return true;
}

//获取位置
int CCardListImage::GetImageIndex(BYTE cbCardData)
{
	//背景判断
	if (cbCardData==0) return 0;

	//计算位置
	BYTE cbValue=cbCardData&MASK_VALUE;
	BYTE cbColor=(cbCardData&MASK_COLOR)>>4;
	return (cbColor>0x02)?(cbValue+27):(cbColor*9+cbValue);
}

//绘画扑克
bool CCardListImage::DrawCardItem(CDC * pDestDC, BYTE cbCardData, int xDest, int yDest)
{
	//效验状态
	ASSERT(m_CardListImage.IsNull()==false);
	ASSERT((m_nItemWidth!=0)&&(m_nItemHeight!=0));

	//绘画子项
	int nImageXPos=GetImageIndex(cbCardData)*m_nItemWidth;
	m_CardListImage.AlphaDrawImage(pDestDC,xDest,yDest,m_nItemWidth,m_nItemHeight,nImageXPos,0,RGB(255,0,255));

	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CCardResource::CCardResource()
{
}

//析构函数
CCardResource::~CCardResource()
{
}

//加载资源
bool CCardResource::LoadResource()
{
	//变量定义
	HINSTANCE hInstance=AfxGetInstanceHandle();

	//用户扑克
	m_ImageUserTop.LoadFromResource(hInstance,IDB_CARD_USER_TOP);
	m_ImageUserLeft.LoadFromResource(hInstance,IDB_CARD_USER_LEFT);
	m_ImageUserRight.LoadFromResource(hInstance,IDB_CARD_USER_RIGHT);
	m_ImageUserBottom.LoadResource(IDB_CARD_USER_BOTTOM,CARD_WIDTH,CARD_HEIGHT);

	//桌子扑克
	m_ImageTableTop.LoadResource(IDB_CARD_TABLE_TOP,26,25);
	m_ImageTableLeft.LoadResource(IDB_CARD_TABLE_LEFT,33,21);
	m_ImageTableRight.LoadResource(IDB_CARD_TABLE_RIGHT,33,21);
	m_ImageTableBottom.LoadResource(IDB_CARD_TABLE_BOTTOM,26,25);

	//牌堆扑克
	m_ImageBackH.LoadFromResource(hInstance,IDB_CARD_BACK_H);
	m_ImageBackV.LoadFromResource(hInstance,IDB_CARD_BACK_V);
	m_ImageHeapSingleV.LoadFromResource(hInstance,IDB_CARD_HEAP_SINGLE_V);
	m_ImageHeapSingleH.LoadFromResource(hInstance,IDB_CARD_HEAP_SINGLE_H);
	m_ImageHeapDoubleV.LoadFromResource(hInstance,IDB_CARD_HEAP_DOUBLE_V);
	m_ImageHeapDoubleH.LoadFromResource(hInstance,IDB_CARD_HEAP_DOUBLE_H);

	//财神的弟弟
	m_ImageKingBrotherTop.LoadResource(IDB_KING_BROTHER_TOP, 17, 37);
	m_ImageKingBrotherBottom.LoadResource(IDB_KING_BROTHER_BOTTOM, 17, 37);
	m_ImageKingBrotherLeft.LoadResource(IDB_KING_BROTHER_LEFT, 22, 33);
	m_ImageKingBrotherRight.LoadResource(IDB_KING_BROTHER_RIGHT, 22, 33);

	return true;
}

//消耗资源
bool CCardResource::DestroyResource()
{
	//用户扑克
	m_ImageUserTop.Destroy();
	m_ImageUserLeft.Destroy();
	m_ImageUserRight.Destroy();
	m_ImageUserBottom.DestroyResource();

	//桌子扑克
	m_ImageTableTop.DestroyResource();
	m_ImageTableLeft.DestroyResource();
	m_ImageTableRight.DestroyResource();
	m_ImageTableBottom.DestroyResource();

	//牌堆扑克
	m_ImageBackH.Destroy();
	m_ImageBackV.Destroy();
	m_ImageHeapSingleV.Destroy();
	m_ImageHeapSingleH.Destroy();
	m_ImageHeapDoubleV.Destroy();
	m_ImageHeapDoubleH.Destroy();

	//财神的弟弟
	m_ImageKingBrotherTop.DestroyResource();
	m_ImageKingBrotherBottom.DestroyResource();
	m_ImageKingBrotherLeft.DestroyResource();
	m_ImageKingBrotherRight.DestroyResource();

	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CHeapCard::CHeapCard()
{
	//控制变量
	m_ControlPoint.SetPoint(0,0);
	m_CardDirection=Direction_East;

	//扑克变量
	m_wFullCount=0;
	m_wMinusHeadCount=0;
	m_wMinusLastCount=0;
	m_wKingBrotherPos = (WORD)-1;

	return;
}

//析构函数
CHeapCard::~CHeapCard()
{
}

//绘画扑克
void CHeapCard::DrawCardControl(CDC * pDC)
{
	switch (m_CardDirection)
	{
	case Direction_East:	//东向
		{
			//绘画扑克
			if ((m_wFullCount-m_wMinusHeadCount-m_wMinusLastCount)>0)
			{
				//变量定义
				int nXPos=0,nYPos=0;
				WORD wHeapIndex=m_wMinusHeadCount/2;
				WORD wDoubleHeap=(m_wMinusHeadCount+1)/2;
				WORD wDoubleLast=(m_wFullCount-m_wMinusLastCount)/2;
				WORD wFinallyIndex=(m_wFullCount-m_wMinusLastCount)/2;

				//头部扑克
				if (m_wMinusHeadCount%2!=0)
				{
					nXPos=m_ControlPoint.x;
					nYPos=m_ControlPoint.y+wHeapIndex*13+9;
					g_CardResource.m_ImageHeapSingleV.AlphaDrawImage(pDC,nXPos,nYPos,RGB(255,0,255));
				}

				//中间扑克
				for (WORD i=wDoubleHeap;i<wFinallyIndex;i++)
				{
					nXPos=m_ControlPoint.x;
					nYPos=m_ControlPoint.y+i*13;
					g_CardResource.m_ImageHeapDoubleV.AlphaDrawImage(pDC,nXPos,nYPos,RGB(255,0,255));
					if(i == (HEAP_FULL_COUNT/2-1-m_wKingBrotherPos))
					{
						g_CardResource.m_ImageKingBrotherRight.DrawCardItem(pDC, m_pDataCtrl->GetKingBrotherData(), nXPos, nYPos);
					}
				}

				//尾部扑克
				if (m_wMinusLastCount%2!=0)
				{
					nXPos=m_ControlPoint.x;
					nYPos=m_ControlPoint.y+wFinallyIndex*13+9;
					g_CardResource.m_ImageHeapSingleV.AlphaDrawImage(pDC,nXPos,nYPos,RGB(255,0,255));
				}
			}
			
			break;
		}
	case Direction_South:	//南向
		{
			//绘画扑克
			if ((m_wFullCount-m_wMinusHeadCount-m_wMinusLastCount)>0)
			{
				//变量定义
				int nXPos=0,nYPos=0;
				WORD wHeapIndex=m_wMinusLastCount/2;
				WORD wDoubleHeap=(m_wMinusLastCount+1)/2;
				WORD wDoubleLast=(m_wFullCount-m_wMinusHeadCount)/2;
				WORD wFinallyIndex=(m_wFullCount-m_wMinusHeadCount)/2;

				//尾部扑克
				if (m_wMinusLastCount%2!=0)
				{
					nYPos=m_ControlPoint.y+11;
					nXPos=m_ControlPoint.x+wHeapIndex*17;
					g_CardResource.m_ImageHeapSingleH.AlphaDrawImage(pDC,nXPos,nYPos,RGB(255,0,255));
				}

				//中间扑克
				for (WORD i=wDoubleHeap;i<wFinallyIndex;i++)
				{
					nYPos=m_ControlPoint.y;
					nXPos=m_ControlPoint.x+i*17;
					g_CardResource.m_ImageHeapDoubleH.AlphaDrawImage(pDC,nXPos,nYPos,RGB(255,0,255));
					if(i == m_wKingBrotherPos)
					{
						g_CardResource.m_ImageKingBrotherBottom.DrawCardItem(pDC, m_pDataCtrl->GetKingBrotherData(), nXPos, nYPos);
					}
				}

				//头部扑克
				if (m_wMinusHeadCount%2!=0)
				{
					nYPos=m_ControlPoint.y+11;
					nXPos=m_ControlPoint.x+wFinallyIndex*17;
					g_CardResource.m_ImageHeapSingleH.AlphaDrawImage(pDC,nXPos,nYPos,RGB(255,0,255));
				}
			}
			
			break;
		}
	case Direction_West:	//西向
		{
			//绘画扑克
			if ((m_wFullCount-m_wMinusHeadCount-m_wMinusLastCount)>0)
			{
				//变量定义
				int nXPos=0,nYPos=0;
				WORD wHeapIndex=m_wMinusLastCount/2;
				WORD wDoubleHeap=(m_wMinusLastCount+1)/2;
				WORD wDoubleLast=(m_wFullCount-m_wMinusHeadCount)/2;
				WORD wFinallyIndex=(m_wFullCount-m_wMinusHeadCount)/2;

				//尾部扑克
				if (m_wMinusLastCount%2!=0)
				{
					nXPos=m_ControlPoint.x;
					nYPos=m_ControlPoint.y+wHeapIndex*13+9;
					g_CardResource.m_ImageHeapSingleV.AlphaDrawImage(pDC,nXPos,nYPos,RGB(255,0,255));
				}

				//中间扑克
				for (WORD i=wDoubleHeap;i<wFinallyIndex;i++)
				{
					nXPos=m_ControlPoint.x;
					nYPos=m_ControlPoint.y+i*13;
					g_CardResource.m_ImageHeapDoubleV.AlphaDrawImage(pDC,nXPos,nYPos,RGB(255,0,255));
					if(i == m_wKingBrotherPos)
					{
						g_CardResource.m_ImageKingBrotherLeft.DrawCardItem(pDC, m_pDataCtrl->GetKingBrotherData(), nXPos, nYPos);
					}
				}

				//头部扑克
				if (m_wMinusHeadCount%2!=0)
				{
					nXPos=m_ControlPoint.x;
					nYPos=m_ControlPoint.y+wFinallyIndex*13+9;
					g_CardResource.m_ImageHeapSingleV.AlphaDrawImage(pDC,nXPos,nYPos,RGB(255,0,255));
				}
			}
			
			break;
		}
	case Direction_North:	//北向
		{
			//绘画扑克
			if ((m_wFullCount-m_wMinusHeadCount-m_wMinusLastCount)>0)
			{
				//变量定义
				int nXPos=0,nYPos=0;
				WORD wHeapIndex=m_wMinusHeadCount/2;
				WORD wDoubleHeap=(m_wMinusHeadCount+1)/2;
				WORD wDoubleLast=(m_wFullCount-m_wMinusLastCount)/2;
				WORD wFinallyIndex=(m_wFullCount-m_wMinusLastCount)/2;

				//头部扑克
				if (m_wMinusHeadCount%2!=0)
				{
					nYPos=m_ControlPoint.y+11;
					nXPos=m_ControlPoint.x+wHeapIndex*17;
					g_CardResource.m_ImageHeapSingleH.AlphaDrawImage(pDC,nXPos,nYPos,RGB(255,0,255));
				}

				//中间扑克
				for (WORD i=wDoubleHeap;i<wFinallyIndex;i++)
				{
					nYPos=m_ControlPoint.y;
					nXPos=m_ControlPoint.x+i*17;
					g_CardResource.m_ImageHeapDoubleH.AlphaDrawImage(pDC,nXPos,nYPos,RGB(255,0,255));
					if(i == (HEAP_FULL_COUNT/2-1-m_wKingBrotherPos))
					{
						g_CardResource.m_ImageKingBrotherTop.DrawCardItem(pDC, m_pDataCtrl->GetKingBrotherData(), nXPos, nYPos);
					}
				}

				//尾部扑克
				if (m_wMinusLastCount%2!=0)
				{
					nYPos=m_ControlPoint.y+11;
					nXPos=m_ControlPoint.x+wFinallyIndex*17;
					g_CardResource.m_ImageHeapSingleH.AlphaDrawImage(pDC,nXPos,nYPos,RGB(255,0,255));
				}
			}
			
			break;
		}
	}

	return;
}

//设置扑克
bool CHeapCard::SetCardData(WORD wMinusHeadCount, WORD wMinusLastCount, WORD wFullCount)
{
	//设置变量
	m_wFullCount=wFullCount;
	m_wMinusHeadCount=wMinusHeadCount;
	m_wMinusLastCount=wMinusLastCount;

	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CWeaveCard::CWeaveCard()
{
	//状态变量
	m_bDisplayItem=true;
	m_ControlPoint.SetPoint(0,0);
	m_CardDirection=Direction_South;
	m_cbDirectionCardPos = 1;

	//扑克数据
	m_wCardCount=0;
	ZeroMemory(&m_cbCardData,sizeof(m_cbCardData));

	return;
}

//析构函数
CWeaveCard::~CWeaveCard()
{
}

//绘画扑克
void CWeaveCard::DrawCardControl(CDC * pDC)
{
	//if(m_wCardCount < 4 && m_bDisplayItem == false)
	//{
	//	assert(0 && "非杠竟然藏牌！！");
	//}
	//显示判断
	if (m_wCardCount==0) return;

	//变量定义
	int nXScreenPos=0,nYScreenPos=0;
	int nItemWidth=0,nItemHeight=0,nItemWidthEx=0,nItemHeightEx=0;

	//绘画扑克
	switch (m_CardDirection)
	{
	case Direction_East:	//东向
		{
			//变量定义
			nItemWidth=g_CardResource.m_ImageTableRight.GetViewWidth();
			nItemHeight=g_CardResource.m_ImageTableRight.GetViewHeight();
			nItemWidthEx=g_CardResource.m_ImageTableTop.GetViewWidth();
			nItemHeightEx=g_CardResource.m_ImageTableTop.GetViewHeight();

			//第三扑克
			nYScreenPos = m_ControlPoint.y;			
			if(m_cbDirectionCardPos == 2)
			{
				nXScreenPos=m_ControlPoint.x+nItemWidthEx;
				g_CardResource.m_ImageTableTop.DrawCardItem(pDC,GetCardData(2),nXScreenPos,nYScreenPos);
				nYScreenPos += nItemHeightEx;
			}
			else
			{
				nXScreenPos=m_ControlPoint.x+nItemWidthEx*2-nItemWidth;
				g_CardResource.m_ImageTableRight.DrawCardItem(pDC,GetCardData(2),nXScreenPos,nYScreenPos);
				nYScreenPos += nItemHeight;
			}
			

			//第二扑克
			nXScreenPos=m_ControlPoint.x+nItemWidthEx*2-nItemWidth;
			g_CardResource.m_ImageTableRight.DrawCardItem(pDC,GetCardData(1),nXScreenPos,nYScreenPos);
			//第四扑克
			if (m_wCardCount==4)
			{
				g_CardResource.m_ImageTableRight.DrawCardItem(pDC,GetCardData(3),nXScreenPos,nYScreenPos-12);
			}
			nYScreenPos += nItemHeight;

			//第一扑克
			if(m_cbDirectionCardPos == 0)
			{
				nXScreenPos=m_ControlPoint.x+nItemWidthEx;
				g_CardResource.m_ImageTableBottom.DrawCardItem(pDC,GetCardData(0),nXScreenPos,nYScreenPos);
			}
			else
			{
				nXScreenPos=m_ControlPoint.x+nItemWidthEx*2-nItemWidth;
				g_CardResource.m_ImageTableRight.DrawCardItem(pDC,GetCardData(0),nXScreenPos,nYScreenPos);

			}
			
			break;
		}
	case Direction_South:	//南向
		{
			//变量定义
			nItemWidth		= g_CardResource.m_ImageTableBottom.GetViewWidth();
			nItemHeight		= g_CardResource.m_ImageTableBottom.GetViewHeight();
			nItemWidthEx	= g_CardResource.m_ImageTableRight.GetViewWidth();
			nItemHeightEx	= g_CardResource.m_ImageTableRight.GetViewHeight();
			
			//第三扑克
			nXScreenPos = m_ControlPoint.x;
			if(m_cbDirectionCardPos == 2)
			{
				nXScreenPos -= nItemWidthEx;
				nYScreenPos=m_ControlPoint.y+nItemHeightEx;
				g_CardResource.m_ImageTableLeft.DrawCardItem(pDC,GetCardData(2),nXScreenPos,nYScreenPos);
			}
			else
			{
				nXScreenPos -= nItemWidth;
				nYScreenPos=m_ControlPoint.y+nItemHeightEx*2-nItemHeight;
				g_CardResource.m_ImageTableBottom.DrawCardItem(pDC,GetCardData(2),nXScreenPos,nYScreenPos);
			}			

			//第二扑克
			nXScreenPos -= nItemWidth;
			nYScreenPos=m_ControlPoint.y+nItemHeightEx*2-nItemHeight;
			g_CardResource.m_ImageTableBottom.DrawCardItem(pDC,GetCardData(1),nXScreenPos,nYScreenPos);
			//第四扑克
			if (m_wCardCount==4)
			{
				g_CardResource.m_ImageTableBottom.DrawCardItem(pDC,GetCardData(3),nXScreenPos,nYScreenPos-12);
			}


			//第一扑克
			if(m_cbDirectionCardPos == 0)
			{
				nXScreenPos -= nItemWidthEx;
				nYScreenPos=m_ControlPoint.y+nItemHeightEx;
				g_CardResource.m_ImageTableRight.DrawCardItem(pDC,GetCardData(0),nXScreenPos,nYScreenPos);
			}
			else
			{
				nXScreenPos -= nItemWidth;
				nYScreenPos=m_ControlPoint.y+nItemHeightEx*2-nItemHeight;
				g_CardResource.m_ImageTableBottom.DrawCardItem(pDC,GetCardData(0),nXScreenPos,nYScreenPos);
			}
			

			break;
		}
	case Direction_West:	//西向
		{
			//变量定义
			nItemWidth=g_CardResource.m_ImageTableLeft.GetViewWidth();
			nItemHeight=g_CardResource.m_ImageTableLeft.GetViewHeight();
			nItemWidthEx=g_CardResource.m_ImageTableBottom.GetViewWidth();
			nItemHeightEx=g_CardResource.m_ImageTableBottom.GetViewHeight();

			
			if(m_cbDirectionCardPos != 1)
			{
				//第一扑克
				nYScreenPos=m_ControlPoint.y-nItemHeightEx-nItemHeight*2;
				if(m_cbDirectionCardPos == 0)
				{
					nXScreenPos=m_ControlPoint.x-nItemWidthEx*2;
					g_CardResource.m_ImageTableBottom.DrawCardItem(pDC,GetCardData(0),nXScreenPos,nYScreenPos);
					nYScreenPos += nItemHeightEx;
				}
				else
				{
					nXScreenPos=m_ControlPoint.x-nItemWidthEx*2;
					g_CardResource.m_ImageTableLeft.DrawCardItem(pDC,GetCardData(0),nXScreenPos,nYScreenPos);
					nYScreenPos += nItemHeight;
				}
				//第二扑克
				nXScreenPos=m_ControlPoint.x-nItemWidthEx*2;
				g_CardResource.m_ImageTableLeft.DrawCardItem(pDC,GetCardData(1),nXScreenPos,nYScreenPos);
				//第四扑克
				if (m_wCardCount==4)
				{
					g_CardResource.m_ImageTableLeft.DrawCardItem(pDC,GetCardData(3),nXScreenPos,nYScreenPos-12);
				}
				nYScreenPos += nItemHeight;

				//第三扑克
				if(m_cbDirectionCardPos == 2)
				{
					nXScreenPos=m_ControlPoint.x-nItemWidthEx*2;
					g_CardResource.m_ImageTableBottom.DrawCardItem(pDC,GetCardData(2),nXScreenPos,nYScreenPos);
				}
				else
				{
					nXScreenPos=m_ControlPoint.x-nItemWidthEx*2;
					g_CardResource.m_ImageTableLeft.DrawCardItem(pDC,GetCardData(2),nXScreenPos,nYScreenPos);
				}
				
			}
			else
			{			
				//第一扑克
				nYScreenPos = m_ControlPoint.y-nItemHeight*3;
				//nXScreenPos=m_ControlPoint.x-nItemWidth;
				nXScreenPos=m_ControlPoint.x-nItemWidthEx*2;
				g_CardResource.m_ImageTableLeft.DrawCardItem(pDC,GetCardData(0),nXScreenPos,nYScreenPos);
				//第二扑克
				nYScreenPos += nItemHeight;
				g_CardResource.m_ImageTableLeft.DrawCardItem(pDC,GetCardData(1),nXScreenPos,nYScreenPos);
				//第四扑克
				if (m_wCardCount==4)
				{
					g_CardResource.m_ImageTableLeft.DrawCardItem(pDC,GetCardData(3),nXScreenPos,nYScreenPos-12);
				}
				//第三扑克
				nYScreenPos += nItemHeight;
				g_CardResource.m_ImageTableLeft.DrawCardItem(pDC,GetCardData(2),nXScreenPos,nYScreenPos);
			}

			break;
		}
	case Direction_North:	//北向
		{
			//变量定义
			nItemWidth		= g_CardResource.m_ImageTableTop.GetViewWidth();
			nItemHeight		= g_CardResource.m_ImageTableTop.GetViewHeight();
			nItemWidthEx	= g_CardResource.m_ImageTableLeft.GetViewWidth();
			nItemHeightEx	= g_CardResource.m_ImageTableLeft.GetViewHeight();

			//第三扑克
			nXScreenPos = m_ControlPoint.x;
			if(m_cbDirectionCardPos == 2)
			{
				nYScreenPos = m_ControlPoint.y-nItemHeight;
				g_CardResource.m_ImageTableRight.DrawCardItem(pDC,GetCardData(2),nXScreenPos,nYScreenPos);
				nXScreenPos += nItemWidthEx;
			}
			else
			{
				nYScreenPos = m_ControlPoint.y-nItemHeight;
				g_CardResource.m_ImageTableTop.DrawCardItem(pDC,GetCardData(2),nXScreenPos,nYScreenPos);
				nXScreenPos += nItemWidth;
			}
			
			//第二扑克
			nYScreenPos = m_ControlPoint.y-nItemHeight;
			g_CardResource.m_ImageTableTop.DrawCardItem(pDC,GetCardData(1),nXScreenPos,nYScreenPos);			
			//第四扑克
			if (m_wCardCount==4)
			{
				g_CardResource.m_ImageTableTop.DrawCardItem(pDC,GetCardData(3),nXScreenPos,nYScreenPos-12);
			}
			nXScreenPos += nItemWidth;


			//第一扑克
			if(m_cbDirectionCardPos == 0)
			{
				nYScreenPos=m_ControlPoint.y-nItemHeight;			
				g_CardResource.m_ImageTableLeft.DrawCardItem(pDC,GetCardData(0),nXScreenPos,nYScreenPos);
			}
			else
			{
				nYScreenPos=m_ControlPoint.y-nItemHeight;		
				g_CardResource.m_ImageTableTop.DrawCardItem(pDC,GetCardData(0),nXScreenPos,nYScreenPos);
			}
			

			break;
		}
	}

	return;
}

//设置扑克
bool CWeaveCard::SetCardData(const BYTE cbCardData[], WORD wCardCount)
{
	//效验大小
	ASSERT(wCardCount<=CountArray(m_cbCardData));
	if (wCardCount>CountArray(m_cbCardData)) return false;

	//设置扑克
	m_wCardCount=wCardCount;
	CopyMemory(m_cbCardData,cbCardData,sizeof(BYTE)*wCardCount);

	return true;
}

//获取扑克
BYTE CWeaveCard::GetCardData(WORD wIndex)
{
	ASSERT(wIndex<CountArray(m_cbCardData));
	return ((m_bDisplayItem==true)||(wIndex==3))?m_cbCardData[wIndex]:0;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CUserCard::CUserCard()
{
	//扑克数据
	m_wCardCount=0;
	m_bCurrentCard=false;

	//控制变量
	m_ControlPoint.SetPoint(0,0);
	m_CardDirection=Direction_East;

	return;
}

//析构函数
CUserCard::~CUserCard()
{
}

//绘画扑克
void CUserCard::DrawCardControl(CDC * pDC)
{
	switch (m_CardDirection)
	{
	case Direction_East:	//东向
		{
			//当前扑克
			if (m_bCurrentCard==true)
			{
				int nXPos=m_ControlPoint.x;
				int nYPos=m_ControlPoint.y;
				g_CardResource.m_ImageUserRight.AlphaDrawImage(pDC,nXPos,nYPos,RGB(255,0,255));
			}

			//正常扑克
			if (m_wCardCount>0)
			{
				int nXPos=0,nYPos=0;
				for (WORD i=0;i<m_wCardCount;i++)
				{
					nXPos=m_ControlPoint.x;
					nYPos=m_ControlPoint.y+i*25+40;
					g_CardResource.m_ImageUserRight.AlphaDrawImage(pDC,nXPos,nYPos,RGB(255,0,255));
				}
			}

			break;
		}
	case Direction_West:	//西向
		{
			//正常扑克
			if (m_wCardCount>0)
			{
				int nXPos=0,nYPos=0;
				for (WORD i=0;i<m_wCardCount;i++)
				{
					nXPos=m_ControlPoint.x;
					nYPos=m_ControlPoint.y-(m_wCardCount-i-1)*25-92;
					g_CardResource.m_ImageUserLeft.AlphaDrawImage(pDC,nXPos,nYPos,RGB(255,0,255));
				}
			}

			//当前扑克
			if (m_bCurrentCard==true)
			{
				int nXPos=m_ControlPoint.x;
				int nYPos=m_ControlPoint.y-49;
				g_CardResource.m_ImageUserLeft.AlphaDrawImage(pDC,nXPos,nYPos,RGB(255,0,255));
			}

			break;
		}
	case Direction_North:	//北向
		{
			//当前扑克
			if (m_bCurrentCard==true)
			{
				int nXPos=m_ControlPoint.x;
				int nYPos=m_ControlPoint.y;
				g_CardResource.m_ImageUserTop.AlphaDrawImage(pDC,nXPos,nYPos,RGB(255,0,255));
			}

			//正常扑克
			if (m_wCardCount>0)
			{
				int nXPos=0,nYPos=0;
				for (WORD i=0;i<m_wCardCount;i++)
				{
					nYPos=m_ControlPoint.y;
					nXPos=m_ControlPoint.x+i*25+40;
					g_CardResource.m_ImageUserTop.AlphaDrawImage(pDC,nXPos,nYPos,RGB(255,0,255));
				}
			}

			break;
		}
	}

	return;
}

//设置扑克
bool CUserCard::SetCurrentCard(bool bCurrentCard)
{
	//设置变量
	m_bCurrentCard=bCurrentCard;

	return true;
}

//设置扑克
bool CUserCard::SetCardData(WORD wCardCount, bool bCurrentCard)
{
	//设置变量
	m_wCardCount=wCardCount;
	m_bCurrentCard=bCurrentCard;

	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CDiscardCard::CDiscardCard()
{
	//扑克数据
	m_wCardCount=0;
	ZeroMemory(m_cbCardData,sizeof(m_cbCardData));

	//控制变量
	m_ControlPoint.SetPoint(0,0);
	m_CardDirection=Direction_East;

	return;
}

//析构函数
CDiscardCard::~CDiscardCard()
{
}

//绘画扑克
void CDiscardCard::DrawCardControl(CDC * pDC)
{
	//绘画控制
	switch (m_CardDirection)
	{
	case Direction_East:	//东向
		{
			//绘画扑克
			for (WORD i=0;i<m_wCardCount;i++)
			{
				int nXPos=m_ControlPoint.x+(i/11)*33;
				int nYPos=m_ControlPoint.y+(i%11)*21;
				g_CardResource.m_ImageTableRight.DrawCardItem(pDC,m_cbCardData[i],nXPos,nYPos);
			}

			break;
		}
	case Direction_South:	//南向
		{
			//绘画扑克
			for (WORD i=0;i<m_wCardCount;i++)
			{
				int nXPos=m_ControlPoint.x-(i%11)*26;
				int nYPos=m_ControlPoint.y+(i/11)*26;
				g_CardResource.m_ImageTableBottom.DrawCardItem(pDC,m_cbCardData[i],nXPos,nYPos);
			}

			break;
		}
	case Direction_West:	//西向
		{
			//绘画扑克
			for (WORD i=0;i<m_wCardCount;i++)
			{
				int nXPos=m_ControlPoint.x-((m_wCardCount-1-i)/11+1)*33;
				int nYPos=m_ControlPoint.y-((m_wCardCount-1-i)%11+1)*21;
				g_CardResource.m_ImageTableLeft.DrawCardItem(pDC,m_cbCardData[m_wCardCount-i-1],nXPos,nYPos);
			}

			break;
		}
	case Direction_North:	//北向
		{
			//绘画扑克
			for (WORD i=0;i<m_wCardCount;i++)
			{
				int nXPos=m_ControlPoint.x+((m_wCardCount-1-i)%11)*26;
				int nYPos=m_ControlPoint.y-((m_wCardCount-1-i)/11+1)*25-11;
				g_CardResource.m_ImageTableTop.DrawCardItem(pDC,m_cbCardData[m_wCardCount-i-1],nXPos,nYPos);
			}

			break;
		}
	}

	return;
}

//增加扑克
bool CDiscardCard::AddCardItem(BYTE cbCardData)
{
	//清理扑克
	if (m_wCardCount>=CountArray(m_cbCardData))
	{
		m_wCardCount--;
		MoveMemory(m_cbCardData,m_cbCardData+1,CountArray(m_cbCardData)-1);
	}

	//设置扑克
	m_cbCardData[m_wCardCount++]=cbCardData;

	return true;
}

//设置扑克
bool CDiscardCard::SetCardData(const BYTE cbCardData[], WORD wCardCount)
{
	//效验大小
	ASSERT(wCardCount<=CountArray(m_cbCardData));
	if (wCardCount>CountArray(m_cbCardData)) return false;

	//设置扑克
	m_wCardCount=wCardCount;
	CopyMemory(m_cbCardData,cbCardData,sizeof(m_cbCardData[0])*wCardCount);

	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CTableCard::CTableCard()
: m_pDataCtrl(0)
{
	//扑克数据
	m_wCardCount=0;
	ZeroMemory(m_cbCardData,sizeof(m_cbCardData));

	//控制变量
	m_ControlPoint.SetPoint(0,0);
	m_CardDirection=Direction_East;

	return;
}

//析构函数
CTableCard::~CTableCard()
{
}

//绘画扑克
void CTableCard::DrawCardControl(CDC * pDC)
{
	ASSERT(m_pDataCtrl != 0);
	if(m_pDataCtrl == 0)
	{
		return;
	}

	RECT rcKing;
	//绘画控制
	switch (m_CardDirection)
	{
	case Direction_East:	//东向
		{
			//绘画扑克
			for (WORD i=0;i<m_wCardCount;i++)
			{
				int nXPos=m_ControlPoint.x-33;
				int nYPos=m_ControlPoint.y+i*21;
				BYTE cbCardData = m_cbCardData[m_wCardCount-i-1];
				if(cbCardData == CARD_KING_DATA_MASK)
				{
					cbCardData = m_pDataCtrl->GetKingCardData();
					g_CardResource.m_ImageTableRight.DrawCardItem(pDC,cbCardData,nXPos,nYPos);
					rcKing.left		= nXPos+2;
					rcKing.top		= nYPos+1;
					rcKing.right	= nXPos+g_CardResource.m_ImageTableRight.GetViewWidth()-1;
					rcKing.bottom	= nYPos+g_CardResource.m_ImageTableRight.GetViewHeight();					
					CDrawFunc::DrawAlphaRect(pDC, &rcKing, RGB(255,255,0), 0.6f);

				}
				else
				{
					g_CardResource.m_ImageTableRight.DrawCardItem(pDC,cbCardData,nXPos,nYPos);
				}
			}

			break;
		}
	case Direction_South:	//南向
		{
			//绘画扑克
			for (WORD i=0;i<m_wCardCount;i++)
			{
				int nYPos=m_ControlPoint.y-36;
				int nXPos=m_ControlPoint.x-(m_wCardCount-i)*26;
				BYTE cbCardData = m_cbCardData[i];
				if(cbCardData == CARD_KING_DATA_MASK)
				{
					cbCardData = m_pDataCtrl->GetKingCardData();
					g_CardResource.m_ImageTableBottom.DrawCardItem(pDC,cbCardData,nXPos,nYPos);
					rcKing.left		= nXPos+2;
					rcKing.top		= nYPos+1;
					rcKing.right	= nXPos+g_CardResource.m_ImageTableBottom.GetViewWidth()-1;
					rcKing.bottom	= nYPos+g_CardResource.m_ImageTableBottom.GetViewHeight();
					CDrawFunc::DrawAlphaRect(pDC, &rcKing, RGB(255,255,0), 0.6f);

				}
				else
				{
					g_CardResource.m_ImageTableBottom.DrawCardItem(pDC,cbCardData,nXPos,nYPos);
				}
			}

			break;
		}
	case Direction_West:	//西向
		{
			//绘画扑克
			for (WORD i=0;i<m_wCardCount;i++)
			{
				int nXPos=m_ControlPoint.x;
				int nYPos=m_ControlPoint.y-(m_wCardCount-i)*21;
				BYTE cbCardData = m_cbCardData[i];
				if(cbCardData == CARD_KING_DATA_MASK)
				{
					cbCardData = m_pDataCtrl->GetKingCardData();
					g_CardResource.m_ImageTableLeft.DrawCardItem(pDC,cbCardData,nXPos,nYPos);
					rcKing.left		= nXPos+2;
					rcKing.top		= nYPos+1;
					rcKing.right	= nXPos+g_CardResource.m_ImageTableLeft.GetViewWidth()-1;
					rcKing.bottom	= nYPos+g_CardResource.m_ImageTableLeft.GetViewHeight();				
					CDrawFunc::DrawAlphaRect(pDC, &rcKing, RGB(255,255,0), 0.6f);

				}
				else
				{
					g_CardResource.m_ImageTableLeft.DrawCardItem(pDC,cbCardData,nXPos,nYPos);
				}
			}

			break;
		}
	case Direction_North:	//北向
		{
			//绘画扑克
			for (WORD i=0;i<m_wCardCount;i++)
			{
				int nYPos=m_ControlPoint.y;
				int nXPos=m_ControlPoint.x+i*26;
				BYTE cbCardData = m_cbCardData[m_wCardCount-i-1];
				if(cbCardData == CARD_KING_DATA_MASK)
				{
					cbCardData = m_pDataCtrl->GetKingCardData();
					g_CardResource.m_ImageTableTop.DrawCardItem(pDC,cbCardData,nXPos,nYPos);
					rcKing.left		= nXPos+2;
					rcKing.top		= nYPos+1;
					rcKing.right	= nXPos+g_CardResource.m_ImageTableTop.GetViewWidth()-1;
					rcKing.bottom	= nYPos+g_CardResource.m_ImageTableTop.GetViewHeight();
					CDrawFunc::DrawAlphaRect(pDC, &rcKing, RGB(255,255,0), 0.6f);

				}
				else
				{
					g_CardResource.m_ImageTableTop.DrawCardItem(pDC,cbCardData,nXPos,nYPos);
				}
			}

			break;
		}
	}

	return;
}

//设置扑克
bool CTableCard::SetCardData(const BYTE cbCardData[], WORD wCardCount)
{
	//效验大小
	ASSERT(wCardCount<=CountArray(m_cbCardData));
	if (wCardCount>CountArray(m_cbCardData)) return false;

	//设置扑克
	m_wCardCount=wCardCount;
	CopyMemory(m_cbCardData,cbCardData,sizeof(m_cbCardData[0])*wCardCount);

	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CCardControl::CCardControl(IDataCtrl* pDataCtrl)
:	m_pDataCtrl(pDataCtrl)
{
	//状态变量
	m_bPositively=false;
	m_bDisplayItem=false;

	//位置变量
	m_XCollocateMode=enXCenter;
	m_YCollocateMode=enYCenter;
	m_BenchmarkPos.SetPoint(0,0);

	//扑克数据
	m_wCardCount=0;
	m_wHoverItem=INVALID_ITEM;
	ZeroMemory(&m_CurrentCard,sizeof(m_CurrentCard));
	ZeroMemory(&m_CardItemArray,sizeof(m_CardItemArray));

	//加载设置
	m_ControlPoint.SetPoint(0,0);
	m_ControlSize.cy=CARD_HEIGHT+POS_SHOOT;
	m_ControlSize.cx=(CountArray(m_CardItemArray)+1)*CARD_WIDTH+POS_SPACE;

	return;
}

//析构函数
CCardControl::~CCardControl()
{
}

//基准位置
void CCardControl::SetBenchmarkPos(int nXPos, int nYPos, enXCollocateMode XCollocateMode, enYCollocateMode YCollocateMode)
{
	//设置变量
	m_BenchmarkPos.x=nXPos;
	m_BenchmarkPos.y=nYPos;
	m_XCollocateMode=XCollocateMode;
	m_YCollocateMode=YCollocateMode;

	//横向位置
	switch (m_XCollocateMode)
	{
	case enXLeft:	{ m_ControlPoint.x=m_BenchmarkPos.x; break; }
	case enXCenter: { m_ControlPoint.x=m_BenchmarkPos.x-m_ControlSize.cx/2; break; }
	case enXRight:	{ m_ControlPoint.x=m_BenchmarkPos.x-m_ControlSize.cx; break; }
	}

	//竖向位置
	switch (m_YCollocateMode)
	{
	case enYTop:	{ m_ControlPoint.y=m_BenchmarkPos.y; break; }
	case enYCenter: { m_ControlPoint.y=m_BenchmarkPos.y-m_ControlSize.cy/2; break; }
	case enYBottom: { m_ControlPoint.y=m_BenchmarkPos.y-m_ControlSize.cy; break; }
	}

	return;
}

//基准位置
void CCardControl::SetBenchmarkPos(const CPoint & BenchmarkPos, enXCollocateMode XCollocateMode, enYCollocateMode YCollocateMode)
{
	//设置变量
	m_BenchmarkPos=BenchmarkPos;
	m_XCollocateMode=XCollocateMode;
	m_YCollocateMode=YCollocateMode;

	//横向位置
	switch (m_XCollocateMode)
	{
	case enXLeft:	{ m_ControlPoint.x=m_BenchmarkPos.x; break; }
	case enXCenter: { m_ControlPoint.x=m_BenchmarkPos.x-m_ControlSize.cx/2; break; }
	case enXRight:	{ m_ControlPoint.x=m_BenchmarkPos.x-m_ControlSize.cx; break; }
	}

	//竖向位置
	switch (m_YCollocateMode)
	{
	case enYTop:	{ m_ControlPoint.y=m_BenchmarkPos.y; break; }
	case enYCenter: { m_ControlPoint.y=m_BenchmarkPos.y-m_ControlSize.cy/2; break; }
	case enYBottom: { m_ControlPoint.y=m_BenchmarkPos.y-m_ControlSize.cy; break; }
	}

	return;
}

//获取扑克
BYTE CCardControl::GetHoverCard()
{
	//获取扑克
	if (m_wHoverItem!=INVALID_ITEM)
	{
		if (m_wHoverItem==CountArray(m_CardItemArray))
		{
			return m_CurrentCard.cbCardData;
		}
		return m_CardItemArray[m_wHoverItem].cbCardData;
	}

	return 0;
}

//设置扑克
bool CCardControl::SetCurrentCard(BYTE cbCardData)
{
	//设置变量
	m_CurrentCard.bShoot=false;
	m_CurrentCard.cbCardData=cbCardData;

	return true;
}

//设置扑克
bool CCardControl::SetCurrentCard(tagCardItem CardItem)
{
	//设置变量
	m_CurrentCard.bShoot=CardItem.bShoot;
	m_CurrentCard.cbCardData=CardItem.cbCardData;

	return true;
}

//设置扑克
bool CCardControl::SetCardData(const BYTE cbCardData[], WORD wCardCount, BYTE cbCurrentCard)
{
	//效验大小
	ASSERT(wCardCount<=CountArray(m_CardItemArray));
	if (wCardCount>CountArray(m_CardItemArray)) 
	{
		return false;
	}

	//当前扑克
	m_CurrentCard.bShoot=false;
	m_CurrentCard.cbCardData = cbCurrentCard;

	//设置扑克
	m_wCardCount=wCardCount;
	for (WORD i=0;i<m_wCardCount;i++)
	{
		m_CardItemArray[i].bShoot=false;
		m_CardItemArray[i].cbCardData=cbCardData[i];
	}
	////设置王牌
	//if(cbCurrentCard == CARD_KING_DATA_MASK)
	//{
	//}

	return true;
}

//设置扑克
bool CCardControl::SetCardItem(const tagCardItem CardItemArray[], WORD wCardCount)
{
	//效验大小
	ASSERT(wCardCount<=CountArray(m_CardItemArray));
	if (wCardCount>CountArray(m_CardItemArray)) return 0;

	//设置扑克
	m_wCardCount=wCardCount;
	for (WORD i=0;i<m_wCardCount;i++)
	{
		m_CardItemArray[i].bShoot=CardItemArray[i].bShoot;
		m_CardItemArray[i].cbCardData=CardItemArray[i].cbCardData;
	}

	return true;
}

//绘画扑克
void CCardControl::DrawCardControl(CDC * pDC)
{
	ASSERT(m_pDataCtrl != 0);
	//绘画准备
	int nXExcursion=m_ControlPoint.x+(CountArray(m_CardItemArray)-m_wCardCount)*CARD_WIDTH;

	//绘画扑克
	for (WORD i=0;i<m_wCardCount;i++)
	{
		//计算位置
		int nXScreenPos=nXExcursion+CARD_WIDTH*i;
		int nYScreenPos=m_ControlPoint.y+(((m_CardItemArray[i].bShoot==false)&&(m_wHoverItem!=i))?POS_SHOOT:0);

		//绘画扑克
		//BYTE cbCardData = (m_bDisplayItem==true)?m_CardItemArray[i].cbCardData:0;
		BYTE cbCardData = (m_pDataCtrl->IsEnableLookOn() == false && m_pDataCtrl->IsLookOn() ) ? 0 : m_CardItemArray[i].cbCardData;
		//绘画王牌
		if(cbCardData == CARD_KING_DATA_MASK)
		{
			cbCardData = m_pDataCtrl->GetKingCardData();
			g_CardResource.m_ImageUserBottom.DrawCardItem(pDC,cbCardData,nXScreenPos,nYScreenPos);
			RECT rcKing;
			rcKing.left		= nXScreenPos+2;
			rcKing.top		= nYScreenPos+5;
			rcKing.right	= nXScreenPos+g_CardResource.m_ImageUserBottom.GetViewWidth();
			rcKing.bottom	= nYScreenPos+g_CardResource.m_ImageUserBottom.GetViewHeight()-3;				
			CDrawFunc::DrawAlphaRect(pDC, &rcKing, RGB(255,255,0), 0.6f);
		}
		else
		{
			g_CardResource.m_ImageUserBottom.DrawCardItem(pDC,cbCardData,nXScreenPos,nYScreenPos);
		}

	}

	//当前扑克
	if (m_CurrentCard.cbCardData!=0)
	{
		//计算位置
		int nXScreenPos=m_ControlPoint.x+m_ControlSize.cx-CARD_WIDTH;
		int nYScreenPos=m_ControlPoint.y+(((m_CurrentCard.bShoot==false)&&(m_wHoverItem!=CountArray(m_CardItemArray)))?POS_SHOOT:0);

		//绘画扑克
		//BYTE cbCardData=(m_bDisplayItem==true)?m_CurrentCard.cbCardData:0;
		BYTE cbCardData = (m_pDataCtrl->IsEnableLookOn() == false && m_pDataCtrl->IsLookOn() ) ? 0 : m_CurrentCard.cbCardData;
		//绘画王牌
		if(cbCardData == CARD_KING_DATA_MASK)
		{
			cbCardData = m_pDataCtrl->GetKingCardData();
			g_CardResource.m_ImageUserBottom.DrawCardItem(pDC,cbCardData,nXScreenPos,nYScreenPos);
			RECT rcKing;
			rcKing.left		= nXScreenPos+2;
			rcKing.top		= nYScreenPos+5;
			rcKing.right	= nXScreenPos+g_CardResource.m_ImageUserBottom.GetViewWidth();
			rcKing.bottom	= nYScreenPos+g_CardResource.m_ImageUserBottom.GetViewHeight()-3;				
			CDrawFunc::DrawAlphaRect(pDC, &rcKing, RGB(255,255,0), 0.6f);
		}
		else
		{
			g_CardResource.m_ImageUserBottom.DrawCardItem(pDC,cbCardData,nXScreenPos,nYScreenPos);
		}		
	}

	return;
}

//索引切换
WORD CCardControl::SwitchCardPoint(CPoint & MousePoint)
{
	//基准位置
	int nXPos=MousePoint.x-m_ControlPoint.x;
	int nYPos=MousePoint.y-m_ControlPoint.y;

	//范围判断
	if ((nXPos<0)||(nXPos>m_ControlSize.cx)) return INVALID_ITEM;
	if ((nYPos<POS_SHOOT)||(nYPos>m_ControlSize.cy)) return INVALID_ITEM;

	//牌列子项
	if (nXPos<CARD_WIDTH*CountArray(m_CardItemArray))
	{
		WORD wViewIndex=(WORD)(nXPos/CARD_WIDTH)+m_wCardCount;
		if (wViewIndex>=CountArray(m_CardItemArray)) return wViewIndex-CountArray(m_CardItemArray);
		return INVALID_ITEM;
	}

	//当前子项
	if ((m_CurrentCard.cbCardData!=0)&&(nXPos>=(m_ControlSize.cx-CARD_WIDTH))) return CountArray(m_CardItemArray);

	return INVALID_ITEM;
}

//光标消息
bool CCardControl::OnEventSetCursor(CPoint Point, bool & bRePaint)
{
	//获取索引
	WORD wHoverItem=SwitchCardPoint(Point);

	//响应判断
	if ((m_bPositively==false)&&(m_wHoverItem!=INVALID_ITEM))
	{
		bRePaint=true;
		m_wHoverItem=INVALID_ITEM;
	}

	//更新判断
	if ((wHoverItem!=m_wHoverItem)&&(m_bPositively==true))
	{
		bRePaint=true;
		m_wHoverItem=wHoverItem;
		SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_CARD_CUR)));
	}

	return (wHoverItem!=INVALID_ITEM);
}

//////////////////////////////////////////////////////////////////////////

//变量声明
CCardResource						g_CardResource;						//扑克资源

//////////////////////////////////////////////////////////////////////////
