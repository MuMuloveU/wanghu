#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////
//静态变量

//扑克数据
const BYTE	CGameLogic::m_bCardListData[216]=
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//方块 A - K
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//梅花 A - K
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//红桃 A - K
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,	//黑桃 A - K
	0x41,0x42,
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//方块 A - K
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//梅花 A - K
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//红桃 A - K
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,	//黑桃 A - K
	0x41,0x42,
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//方块 A - K
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//梅花 A - K
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//红桃 A - K
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,	//黑桃 A - K
	0x41,0x42,
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//方块 A - K
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//梅花 A - K
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//红桃 A - K
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,	//黑桃 A - K
	0x41,0x42,
};

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameLogic::CGameLogic()
{
}

//析构函数
CGameLogic::~CGameLogic()
{
}

//获取类型
BYTE CGameLogic::GetCardType(const BYTE bCardData[], BYTE bCardCount)
{
	//开始分析
	
	switch (bCardCount)
	{
	case 1: //单牌
		{
			return CT_SINGLE;
		}
	case 2:	//对牌
		{
			if (bCardData[0] == 0x41 || bCardData[0] == 0x43)
				return CT_HAVE_D;
			else if (bCardData[0] == 0x42 || bCardData[0] == 0x44)
				return CT_HAVE_X;
			else if (GetCardLogicValue(bCardData[0])==15)
			return CT_HAVE_2;
			else
				return (GetCardLogicValue(bCardData[0])==GetCardLogicValue(bCardData[1]))?CT_DOUBLE:CT_INVALID;
		}
	
	}
	if (bCardCount > 2)
	{
		if (GetCardLogicValue(bCardData[0])==17)
			return CT_HAVE_D;
		else if (GetCardLogicValue(bCardData[0])==16)
			return CT_HAVE_X;
		else if (GetCardLogicValue(bCardData[0])==15)
			return CT_HAVE_2;
		else
			return CT_NORMAL;
	}
	return CT_INVALID;
	
}

//排列扑克
void CGameLogic::SortCardList(BYTE bCardData[], BYTE bCardCount)
{
	//转换数值
	BYTE bLogicVolue[PLAYER_CARDS];
	for (BYTE i=0;i<bCardCount;i++)	bLogicVolue[i]=GetCardLogicValue(bCardData[i]);

	//排序操作
	bool bSorted=true;
	BYTE bTempData,bLast=bCardCount-1;
	do
	{
		bSorted=true;
		for (BYTE i=0;i<bLast;i++)
		{
			if ((bLogicVolue[i]<bLogicVolue[i+1])||
			   ((bLogicVolue[i]==bLogicVolue[i+1])&&(bCardData[i]<bCardData[i+1])))
			{
				//交换位置
				bTempData=bCardData[i];
				bCardData[i]=bCardData[i+1];
				bCardData[i+1]=bTempData;
				bTempData=bLogicVolue[i];
				bLogicVolue[i]=bLogicVolue[i+1];
				bLogicVolue[i+1]=bTempData;
				bSorted=false;
			}	
		}
		bLast--;
	} while(bSorted==false);

	return;
}

//混乱扑克
void CGameLogic::RandCardList(BYTE bCardBuffer[], BYTE bBufferCount)
{
	//混乱准备
	BYTE bCardData[sizeof(m_bCardListData)];
	CopyMemory(bCardData,m_bCardListData,sizeof(m_bCardListData));

	//混乱扑克
	BYTE bRandCount=0,bPosition=0;
	do
	{
		bPosition=rand()%(bBufferCount-bRandCount);
		bCardBuffer[bRandCount++]=bCardData[bPosition];
		bCardData[bPosition]=bCardData[bBufferCount-bRandCount];
	} while (bRandCount<bBufferCount);

	return;
}

//删除扑克
bool CGameLogic::RemoveCard(const BYTE bRemoveCard[], BYTE bRemoveCount, BYTE bCardData[], BYTE bCardCount)
{
	//检验数据
	ASSERT(bRemoveCount<=bCardCount);

	//定义变量
	BYTE bDeleteCount=0,bTempCardData[PLAYER_CARDS];
	if (bCardCount>CountArray(bTempCardData)) return false;
	CopyMemory(bTempCardData,bCardData,bCardCount*sizeof(bCardData[0]));

	//置零扑克
	for (BYTE i=0;i<bRemoveCount;i++)
	{
		for (BYTE j=0;j<bCardCount;j++)
		{
			if (bRemoveCard[i]==bTempCardData[j])
			{
				bDeleteCount++;
				bTempCardData[j]=0;
				break;
			}
		}
	}
	if (bDeleteCount!=bRemoveCount) return false;

	//清理扑克
	BYTE bCardPos=0;
	for (BYTE i=0;i<bCardCount;i++)
	{
		if (bTempCardData[i]!=0) bCardData[bCardPos++]=bTempCardData[i];
	}

	return true;
}

//逻辑数值
BYTE CGameLogic::GetCardLogicValue(BYTE bCardData)
{
	//扑克属性
	BYTE bCardColor=GetCardColor(bCardData);
	BYTE bCardValue=GetCardValue(bCardData);

	//转换数值
	if (bCardColor==0x40)	return bCardValue+15;	
	return (bCardValue<=2)?(bCardValue+13):bCardValue;
}

//对比扑克
bool CGameLogic::CompareCard(const BYTE bFirstList[], const BYTE bNextList[], BYTE bFirstCount, BYTE bNextCount)
{
	//获取类型
	BYTE bNextType=GetCardType(bNextList,bNextCount);
	BYTE bFirstType=GetCardType(bFirstList,bFirstCount);

	if (bFirstType==CT_INVALID) return false;
	
	if (bFirstCount==2&&bNextType==CT_SINGLE)
	{
		if (GetCardLogicValue(bFirstList[0])==17 &&GetCardLogicValue(bFirstList[1])==17 && GetCardLogicValue(bNextList[0])==17)
			return true;
	}
	if (bFirstCount==2 && bNextType==CT_SINGLE)
	{
		if (GetCardLogicValue(bFirstList[0])==16 && GetCardLogicValue(bFirstList[1])==16 &&GetCardLogicValue(bNextList[0])==16)
			return true;
	}
	if (bFirstCount==3 && bNextCount==1)
	{
		if (GetCardLogicValue(bFirstList[0])==16 && GetCardLogicValue(bFirstList[1])==16 && GetCardLogicValue(bFirstList[2])==16 && GetCardLogicValue(bNextList[0])==17)
		{
			return true;
		}
	}

	if (bFirstCount != bNextCount)
		return false;
	
	if (bFirstType==CT_HAVE_D && bNextType==CT_HAVE_D && (GetCardLogicValue(bFirstList[bFirstCount-1]) > GetCardLogicValue(bNextList[bNextCount-1])))
		return true;
	else if (bFirstType==CT_HAVE_D && bNextType!=CT_HAVE_D && (GetCardLogicValue(bFirstList[bFirstCount-1]) > GetCardLogicValue(bNextList[bNextCount-1])))
		return true;
	else
	{
		if (bFirstType==CT_HAVE_X && bNextType==CT_HAVE_X && (GetCardLogicValue(bFirstList[bFirstCount-1]) > GetCardLogicValue(bNextList[bNextCount-1])))
			return true;
		else if (bFirstType==CT_HAVE_X && bNextType!=CT_HAVE_D && bNextType!=CT_HAVE_X && (GetCardLogicValue(bFirstList[bFirstCount-1]) > GetCardLogicValue(bNextList[bNextCount-1])))
			return true;
		else
		{
			if (bFirstType==CT_HAVE_2 && bNextType==CT_HAVE_2 && (GetCardLogicValue(bFirstList[bFirstCount-1]) > GetCardLogicValue(bNextList[bNextCount-1])))
				return true;
			else if (bFirstType==CT_HAVE_2 && bNextType!=CT_HAVE_D && bNextType!=CT_HAVE_X && bNextType!=CT_HAVE_2 && (GetCardLogicValue(bFirstList[bFirstCount-1]) > GetCardLogicValue(bNextList[bNextCount-1])))
				return true;
			else
				if (GetCardLogicValue(bFirstList[bFirstCount-1]) > GetCardLogicValue(bNextList[bNextCount-1]))
					return true;
		}
	}
	
	return false;
}
//////////////////////////////////////////////////////////////////////////
