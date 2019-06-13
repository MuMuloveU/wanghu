#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////
//静态变量

//扑克数据
const BYTE	CGameLogic::m_bCardListData[55]=
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//方块 A - K
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//梅花 A - K
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//红桃 A - K
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,	//黑桃 A - K
	0x41,0x42,0x43														//大小王,赖子
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
	case 2:	//对牌和火箭
		{
			if ((bCardData[0]==0x42)&&(bCardData[1]==0x41)) 
				return CT_MISSILE_CARD;
			return (GetCardLogicValue(bCardData[0])==GetCardLogicValue(bCardData[1]))?CT_DOUBLE:CT_INVALID;
		}
	case 3:
	case 4:	//连牌和炸弹
		{
			BYTE bLogicValue=GetCardLogicValue(bCardData[0]);
			for (BYTE i=1;i<bCardCount;i++)
			{
				if (bLogicValue!=GetCardLogicValue(bCardData[i])) break;
			}
			if (i==bCardCount) return (bCardCount==3)?CT_THREE:CT_BOMB_CARD;
			if (bCardCount==3) return CT_INVALID;
			break;
		}
	}

	//其他牌型
	if (bCardCount>=4)
	{
		//分析扑克
		tagAnalyseResult AnalyseResult;
		AnalysebCardData(bCardData,bCardCount,AnalyseResult);

		//四牌判断
		if (AnalyseResult.bFourCount>0)
		{
			if ((AnalyseResult.bFourCount==1)&&(bCardCount==5)) return CT_FOUR_LINE_TAKE_ONE;
			if ((AnalyseResult.bFourCount==1)&&(AnalyseResult.bDoubleCount==1)&&(bCardCount==6)) return CT_FOUR_LINE_TAKE_ONE;
			if ((AnalyseResult.bFourCount==1)&&(AnalyseResult.bDoubleCount==2)&&(bCardCount==8)) return CT_FOUR_LINE_TAKE_DOUBLE;
			if ((AnalyseResult.bFourCount==1)&&(bCardCount==6)) return CT_FOUR_LINE_TAKE_DOUBLE;

			return CT_INVALID;
		}

		//三牌判断
		if (AnalyseResult.bThreeCount>0)
		{
			//连牌判断
			if (AnalyseResult.bThreeCount>1)
			{
				if (AnalyseResult.bThreeLogicVolue[0]==15) return CT_INVALID;
				for (BYTE i=1;i<AnalyseResult.bThreeCount;i++)
				{
					if (AnalyseResult.bThreeLogicVolue[i]!=(AnalyseResult.bThreeLogicVolue[0]-i)) return CT_INVALID;
				}
			}

			//牌形判断
			if (AnalyseResult.bThreeCount*3==bCardCount) return CT_THREE_LINE;
			if (AnalyseResult.bThreeCount==(bCardCount-AnalyseResult.bThreeCount*3)) return CT_THREE_LINE_TAKE_ONE;
			if ((AnalyseResult.bDoubleCount==AnalyseResult.bThreeCount)&&(bCardCount==(AnalyseResult.bThreeCount*3+AnalyseResult.bDoubleCount*2))) return CT_THREE_LINE_TAKE_DOUBLE;

			return CT_INVALID;
		}

		//两张类型
		if (AnalyseResult.bDoubleCount>=3)
		{
			//二连判断
			if (AnalyseResult.bDoubleLogicVolue[0]!=15)
			{
				for (BYTE i=1;i<AnalyseResult.bDoubleCount;i++)
				{
					if (AnalyseResult.bDoubleLogicVolue[i]!=(AnalyseResult.bDoubleLogicVolue[0]-i)) return CT_INVALID;
				}
				if (AnalyseResult.bDoubleCount*2==bCardCount) return CT_DOUBLE_LINE;
			}

			return CT_INVALID;
		}
		
		//单张判断
		if ((AnalyseResult.bSignedCount>=5)&&(AnalyseResult.bSignedCount==bCardCount))
		{
			BYTE bLogicValue=GetCardLogicValue(bCardData[0]);
			if (bLogicValue>=15) return CT_INVALID;
			for (BYTE i=1;i<AnalyseResult.bSignedCount;i++)
			{
				if (GetCardLogicValue(bCardData[i])!=(bLogicValue-i)) return CT_INVALID;
			}

			return CT_ONE_LINE;
		}
		
		return CT_INVALID;
	}

	return CT_INVALID;
}

//获取类型(有赖子牌)
BYTE CGameLogic::GetMagicCardType(const BYTE bCardData[],BYTE bCardCount,BYTE *bMagicCard)
{
	BYTE tempCardList[21];
	BYTE tempCardType=0;
	BYTE CardType=0;

	int index;
	for(int j=0;j<bCardCount;j++)
	{
		if(bCardData[j]==0x43)
			index=j;
	}
	for(BYTE i=0x51;i<=0x5D;i++)
	{	
		CopyMemory(tempCardList,bCardData,bCardCount);
		tempCardList[index]=i;
		SortCardList(tempCardList,bCardCount);
		tempCardType=GetCardType(tempCardList,bCardCount);
		if(tempCardType>CardType)
		{
			*bMagicCard=i;
			CardType=tempCardType;
		}
		else if(tempCardType==CardType)
		{
			if((tempCardList[0]==i)&&(GetCardLogicValue(tempCardList[0])>GetCardLogicValue(*bMagicCard)))
				*bMagicCard=i;
		}
	}
	if(CardType==CT_BOMB_CARD)
		CardType=CT_BOMB_SOFT;

	return CardType;
}
//获取类型(有赖子牌)
BYTE CGameLogic::GetMagicCardType(const BYTE bCardData[],BYTE bCardCount)
{
	BYTE tempCardList[21];
	BYTE tempCardType=0;
	BYTE CardType=0;

	int index;
	for(int j=0;j<bCardCount;j++)
	{
		if(bCardData[j]==0x43)
			index=j;
	}
	for(BYTE i=1;i<=13;i++)
	{	
		CopyMemory(tempCardList,bCardData,sizeof(BYTE)*bCardCount);
		tempCardList[index]=i;
		SortCardList(tempCardList,bCardCount);
		tempCardType=GetCardType(tempCardList,bCardCount);
		if(tempCardType>CardType)
		{
			CardType=tempCardType;
		}
	}
	if(CardType==CT_BOMB_CARD)
		CardType=CT_BOMB_SOFT;
	return CardType;
}
//排列扑克
void CGameLogic::SortCardList(BYTE bCardData[], BYTE bCardCount)
{
	//转换数值
	BYTE bLogicVolue[21];
	for (BYTE i=0;i<bCardCount;i++)
	{
		bLogicVolue[i]=GetCardLogicValue(bCardData[i]);
		//赖子排最前
		if(bLogicVolue[i]==0)
			bLogicVolue[i]+=18;
	}

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
	BYTE bDeleteCount=0,bTempCardData[21];
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

//是否有赖子
bool CGameLogic::IsHadRoguishCard(const BYTE bCardData[],BYTE bCardCount)
{
	for(int i=0; i<bCardCount;i++)
	{
		if(bCardData[i]==0x43)
			return true;
	}
	return false;
}
//逻辑数值
BYTE CGameLogic::GetCardLogicValue(BYTE bCardData)
{
	//扑克属性
	BYTE bCardColor=GetCardColor(bCardData);
	BYTE bCardValue=GetCardValue(bCardData);

	//转换数值
	if (bCardColor==0x40)
	{
		//赖子不能单出,最小
		if(bCardValue==3)
			return 0;
		else
			return bCardValue+15;
	}
	return (bCardValue<=2)?(bCardValue+13):bCardValue;
}

//对比扑克
bool CGameLogic::CompareCard(const BYTE bFirstList[], const BYTE bNextList[], BYTE bFirstCount, BYTE bNextCount)
{
	//获取类型
	BYTE bNextType;
	BYTE bFirstType;
	BYTE bMagicCard=0;
	BYTE bFirstData[21];
	BYTE bNextData[21];
	memset(bFirstData,0,21);
	memset(bNextData,0,21);
	//是否有赖子
	if(IsHadRoguishCard(bFirstList,bFirstCount))
	{
		if(bFirstCount<2)
			return false;
		bFirstType=GetMagicCardType(bFirstList,bFirstCount,&bMagicCard);
		CopyMemory(bFirstData,bFirstList,bFirstCount);
		bFirstData[0]=bMagicCard;
		SortCardList(bFirstData,bFirstCount);
	}
	else
	{
		bFirstType=GetCardType(bFirstList,bFirstCount);
		CopyMemory(bFirstData,bFirstList,bFirstCount);
	}
	if(IsHadRoguishCard(bNextList,bNextCount))
	{
		bNextType=GetMagicCardType(bNextList,bNextCount,&bMagicCard);
		CopyMemory(bNextData,bNextList,bNextCount);
		bNextData[0]=bMagicCard;
		SortCardList(bNextData,bNextCount);
	}
	else
	{
		bNextType=GetCardType(bNextList,bNextCount);
		CopyMemory(bNextData,bNextList,bNextCount);
	}

	//类型判断	
	if (bFirstType==CT_INVALID) return false;

	if((bFirstType>=CT_BOMB_SOFT)&&(bNextType<CT_BOMB_SOFT))
		return true;
	if((bFirstType<CT_BOMB_SOFT)&&(bNextType>=CT_BOMB_SOFT))
		return false;
	if((bFirstType!=bNextType)&&(bFirstType>=CT_BOMB_SOFT)&&(bNextType>=CT_BOMB_SOFT))
		return bFirstType>bNextType;

	//逻辑判断
	if ((bFirstType!=bNextType)||(bFirstCount!=bNextCount)) return false;

	//char *str=new char[100];
	//sprintf(str,"%d",bNextType);
	//MessageBox(0,str,0,0);
	//delete []str;
	//开始对比
	switch (bNextType)
	{
	case CT_SINGLE:
	case CT_DOUBLE:
	case CT_THREE:
	case CT_ONE_LINE:
	case CT_DOUBLE_LINE:
	case CT_THREE_LINE:
	case CT_BOMB_SOFT:
	case CT_BOMB_CARD:
		{
			BYTE bFirstLogicValue=GetCardLogicValue(bFirstData[0]);
			BYTE bNextLogicValue=GetCardLogicValue(bNextData[0]);
			//char *str=new char[100];
			//sprintf(str,"%d::%d",bFirstLogicValue,bNextLogicValue);
			//MessageBox(0,str,0,0);
			//delete []str;
			return bFirstLogicValue>bNextLogicValue;
		}
	case CT_THREE_LINE_TAKE_ONE:
	case CT_THREE_LINE_TAKE_DOUBLE:
		{
			tagAnalyseResult NextResult;
			tagAnalyseResult FirstResult;
			AnalysebCardData(bNextData,bNextCount,NextResult);
			AnalysebCardData(bFirstData,bFirstCount,FirstResult);
			return FirstResult.bThreeLogicVolue[0]>NextResult.bThreeLogicVolue[0];
		}
	case CT_FOUR_LINE_TAKE_ONE:
	case CT_FOUR_LINE_TAKE_DOUBLE:
		{
			tagAnalyseResult NextResult;
			tagAnalyseResult FirstResult;
			AnalysebCardData(bNextData,bNextCount,NextResult);
			AnalysebCardData(bFirstData,bFirstCount,FirstResult);
			return FirstResult.bFourLogicVolue[0]>NextResult.bFourLogicVolue[0];
		}
	}
	
	return false;
}

//分析扑克
void CGameLogic::AnalysebCardData(const BYTE bCardData[], BYTE bCardCount, tagAnalyseResult & AnalyseResult)
{
	//变量定义
	BYTE bSameCount=1,bCardValueTemp=0;
	BYTE bLogicValue=GetCardLogicValue(bCardData[0]);

	//设置结果
	memset(&AnalyseResult,0,sizeof(AnalyseResult));

	//扑克分析
	for (BYTE i=1;i<bCardCount;i++)
	{
		//获取扑克
		bCardValueTemp=GetCardLogicValue(bCardData[i]);
		if (bCardValueTemp==bLogicValue) bSameCount++;

		//保存结果
		if ((bCardValueTemp!=bLogicValue)||(i==(bCardCount-1)))
		{
			switch (bSameCount)
			{
			case 2:		//两张
				{
					CopyMemory(&AnalyseResult.m_bDCardData[AnalyseResult.bDoubleCount*2],&bCardData[i-2+((i==bCardCount-1&&bCardValueTemp==bLogicValue)?1:0)],2);
					AnalyseResult.bDoubleLogicVolue[AnalyseResult.bDoubleCount++]=bLogicValue;
					break;
				}
			case 3:		//三张
				{
					CopyMemory(&AnalyseResult.m_bTCardData[AnalyseResult.bThreeCount*3],&bCardData[i-3+((i==bCardCount-1&&bCardValueTemp==bLogicValue)?1:0)],3);
					AnalyseResult.bThreeLogicVolue[AnalyseResult.bThreeCount++]=bLogicValue;
					break;
				}
			case 4:		//四张
				{
					CopyMemory(&AnalyseResult.m_bFCardData[AnalyseResult.bFourCount*4],&bCardData[i-4+((i==bCardCount-1&&bCardValueTemp==bLogicValue)?1:0)],4);
					AnalyseResult.bFourLogicVolue[AnalyseResult.bFourCount++]=bLogicValue;
					break;
				}
			}
		}

		//设置变量
		if (bCardValueTemp!=bLogicValue)
		{
			if(bSameCount==1)
			{
				if(i!=bCardCount-1)
					AnalyseResult.m_bSCardData[AnalyseResult.bSignedCount++]=bCardData[i-1];
				else
				{
					AnalyseResult.m_bSCardData[AnalyseResult.bSignedCount++]=bCardData[i-1];
					AnalyseResult.m_bSCardData[AnalyseResult.bSignedCount++]=bCardData[i];
				}
			}
			else
			{
				if(i==bCardCount-1)
					AnalyseResult.m_bSCardData[AnalyseResult.bSignedCount++]=bCardData[i];
			}
			bSameCount=1;
			bLogicValue=bCardValueTemp;

		}
	}

	//单牌数目
	BYTE bOtherCount=AnalyseResult.bDoubleCount*2+AnalyseResult.bThreeCount*3+AnalyseResult.bFourCount*4;
	return;
}

//////////////////////////////////////////////////////////////////////////
