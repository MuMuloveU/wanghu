#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////
//静态变量

//扑克数据
const BYTE	CGameLogic::m_bCardListData[52]=
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//方块 A - K
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//梅花 A - K
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//红桃 A - K
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,	//黑桃 A - K
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
	//条牌判断
	switch (bCardCount)
	{
	case 1: return CT_SINGLE;
	case 2:	return (GetCardValue(bCardData[0])==GetCardValue(bCardData[1]))?CT_DOUBLE:CT_INVALID;
	case 3:
	case 4:
		{
			BYTE bCardValue=GetCardValue(bCardData[0]);
			BYTE i;
			for (i=1;i<bCardCount;i++)
			{
				if (bCardValue!=GetCardValue(bCardData[i])) break;
			}
			if (i==bCardCount) return (bCardCount==3)?CT_THREE:CT_FOUR;
			break;
		}
	}

	//连牌判断
	if (bCardCount>=3)
	{
		//连牌判断
		BYTE bSameCardCount=1;
		BYTE bCardValue=GetCardValue(bCardData[0]);
		if (bCardValue<4) return CT_INVALID;

		//相同数目
		for (BYTE i=1;i<bCardCount;i++)
		{
			if (bCardValue==GetCardValue(bCardData[i]))	bSameCardCount++;
			else break;
		}
		if ((bCardCount%bSameCardCount!=0)||(bCardCount/bSameCardCount<3)) return CT_INVALID;

		//连牌过虑
		for (int i=1;i<bCardCount/bSameCardCount;i++)
		{
			for (BYTE j=0;j<bSameCardCount;j++)
			{
				if (bCardValue!=GetCardValue(bCardData[i*bSameCardCount+j])+i) return CT_INVALID;
			}
		}

		//连牌处理
		switch (bSameCardCount)
		{
		case 1: return CT_ONE_LINE;
		case 2: return CT_DOUBLE_LINE;
		case 3: return CT_THREE_LINE;
		case 4: return CT_FOUR_LINE;
		}
	}

	return CT_INVALID;
}

//排列扑克
void CGameLogic::SortCardList(BYTE bCardData[], BYTE bCardCount)
{
	//转换数值
	BYTE bLogicVolue[20];
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
	BYTE bDeleteCount=0,bTempCardData[20];
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
	BYTE bLogicValue=GetCardValue(bCardData);
	return (bLogicValue<=3)?(bLogicValue+13):bLogicValue;
}

//对比扑克
bool CGameLogic::CompareCard(const BYTE bFirstList[], const BYTE bNextList[], BYTE bCardCount)
{
	BYTE bCardType=GetCardType(bNextList,bCardCount);
	if (GetCardType(bFirstList,bCardCount)!=bCardType) return false;
	return GetCardLogicValue(bFirstList[0])>GetCardLogicValue(bNextList[0]);
}

//////////////////////////////////////////////////////////////////////////
