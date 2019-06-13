#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////
//静态变量

//扑克数据
const BYTE	CGameLogic::m_bCardListData[108]=
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
/*
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

	//天王判断
	if ((bCardCount==4)&&(bCardData[0]==0x42)&&(bCardData[3]==0x41)) return CT_MISSILE_CARD;

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
}*/

//排列扑克
void CGameLogic::SortCardList(BYTE bCardData[], BYTE bCardCount)
{
	//转换数值
	BYTE bLogicVolue[33];
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
	BYTE bDeleteCount=0,bTempCardData[33];
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
	if (bCardColor==0x40) return bCardValue+15;
	return (bCardValue<=2)?(bCardValue+13):bCardValue;
}
/*
//对比扑克
bool CGameLogic::CompareCard(const BYTE bFirstList[], const BYTE bNextList[], BYTE bFirstCount, BYTE bNextCount)
{
	//获取类型
	BYTE bNextType=GetCardType(bNextList,bNextCount);
	BYTE bFirstType=GetCardType(bFirstList,bFirstCount);

	//类型判断
	if (bFirstType==CT_INVALID) return false;
	if (bFirstType==CT_MISSILE_CARD) return true;

	//炸弹判断
	if ((bFirstType==CT_BOMB_CARD)&&(bNextType!=CT_BOMB_CARD)) return true;
	if ((bFirstType!=CT_BOMB_CARD)&&(bNextType==CT_BOMB_CARD)) return false;

	//规则判断
	if ((bFirstType!=bNextType)||(bFirstCount!=bNextCount)) return false;

	//开始对比
	switch (bNextType)
	{
	case CT_SINGLE:
	case CT_DOUBLE:
	case CT_THREE:
	case CT_ONE_LINE:
	case CT_DOUBLE_LINE:
	case CT_THREE_LINE:
	case CT_BOMB_CARD:
		{
			BYTE bFirstLogicValue=GetCardLogicValue(bFirstList[0]);
			BYTE bNextLogicValue=GetCardLogicValue(bNextList[0]);
			return bFirstLogicValue>bNextLogicValue;
		}
	case CT_THREE_LINE_TAKE_DOUBLE:
		{
			tagAnalyseResult NextResult;
			tagAnalyseResult FirstResult;
			AnalysebCardData(bNextList,bNextCount,NextResult);
			AnalysebCardData(bFirstList,bFirstCount,FirstResult);
			return FirstResult.bThreeLogicVolue[0]>NextResult.bThreeLogicVolue[0];
		}
	case CT_FOUR_LINE_TAKE_ONE:
	case CT_FOUR_LINE_TAKE_DOUBLE:
		{
			tagAnalyseResult NextResult;
			tagAnalyseResult FirstResult;
			AnalysebCardData(bNextList,bNextCount,NextResult);
			AnalysebCardData(bFirstList,bFirstCount,FirstResult);
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
*/

//获取类型
BYTE CGameLogic::GetCardType(const BYTE bCardData[], BYTE bCardCount)
{
	//简单牌形
	switch (bCardCount)
	{
	case 1:		//单牌
		{
			return CT_SINGLE;
		}
	case 2:		//对牌
		{
			BYTE bLastLogicValue=GetCardLogicValue(bCardData[1]);
			BYTE bFirstLogicValue=GetCardLogicValue(bCardData[0]);
			return (bFirstLogicValue==bLastLogicValue)?CT_DOUBLE:CT_ERROR;
		}
	case 3:		//三头
		{
			BYTE bLastLogicValue=GetCardLogicValue(bCardData[2]);
			BYTE bFirstLogicValue=GetCardLogicValue(bCardData[0]);
			if (bFirstLogicValue==bLastLogicValue) return CT_THREE_TOU;
			break;
		}
	default:	//炸弹
		{
			BYTE bFirstLogicValue=GetCardLogicValue(bCardData[0]);
			BYTE bLastLogicValue=GetCardLogicValue(bCardData[bCardCount-1]);
			if ((bFirstLogicValue==bLastLogicValue)) return CT_BOMB_CARD;
			else if(GetCardColor(bCardData[0]) == GetCardColor(bCardData[bCardCount-1]) && GetCardColor(bCardData[0]) ==0x40) return CT_MISSILE_CARD;
			break;
		}
	}

	//分析扑克
	tagAnalyseResult AnalyseResult;
	AnalysebCardData(bCardData,bCardCount,AnalyseResult);


	//俘奴判断
	if (AnalyseResult.bThreeCount==1)
	{

		if ((AnalyseResult.bDoubleCount==AnalyseResult.bThreeCount)&&(bCardCount==(AnalyseResult.bThreeCount*3+AnalyseResult.bDoubleCount*2))) return CT_THREE_LINE_TAKE_DOUBLE;

	}

	//两连判断
	if ((AnalyseResult.bDoubleCount>=3)&&(AnalyseResult.bDoubleCount*2==bCardCount))
	{
		//逻辑调整
		BYTE bLogicValue=AnalyseResult.bDoubleLogicVolue[0];
		BYTE bStartID =1;
		if(bLogicValue ==15){
			bLogicValue = GetCardLogicValue(AnalyseResult.bDoubleLogicVolue[1]);
			bStartID =2;
			if(bLogicValue ==14){
				bLogicValue = GetCardLogicValue(AnalyseResult.bDoubleLogicVolue[2]);
				bStartID =3;
			}
		}

		//连牌判断
		if (bLogicValue<15)
		{
			//连牌判断
			bool bSeriesCard=false;
			for (BYTE i=bStartID;i<AnalyseResult.bDoubleCount;i++)
			{
				if (AnalyseResult.bDoubleLogicVolue[i]!=(bLogicValue-(i-bStartID+1))) break;
			}

			//两连判断
			if (i==AnalyseResult.bDoubleCount) return CT_DOUBLE_LINE;
		}
	}
	//三连判断
	if ((AnalyseResult.bThreeCount>=3)&&(AnalyseResult.bThreeCount*3==bCardCount))
	{
		//逻辑调整
		BYTE bLogicValue=AnalyseResult.bThreeLogicVolue[0];
		BYTE bStartID =1;
		if(bLogicValue ==15){
			bLogicValue = GetCardLogicValue(AnalyseResult.bThreeLogicVolue[1]);
			bStartID =2;
			if(bLogicValue ==14){
				bLogicValue = GetCardLogicValue(AnalyseResult.bThreeLogicVolue[2]);
				bStartID =3;
			}
		}


		//连牌判断
		if (bLogicValue<15)
		{
			//连牌判断
			bool bSeriesCard=false;
			for (BYTE i=bStartID;i<AnalyseResult.bThreeCount;i++)
			{
				if (AnalyseResult.bThreeLogicVolue[i]!=(bLogicValue-(i-bStartID+1))) break;
			}

			//两连判断
			if (i==AnalyseResult.bThreeCount) return CT_THREE_LINE;
		}
	}


	//单连判断
	if ((AnalyseResult.bSignedCount>=5)&&(AnalyseResult.bSignedCount==bCardCount))
	{
		//逻辑调整
		BYTE bLogicValue=GetCardLogicValue(bCardData[0]);
		BYTE bStartID =1;
		if(bLogicValue ==15){
			bLogicValue = GetCardLogicValue(bCardData[1]);
			bStartID =2;
			if(bLogicValue ==14){
				bLogicValue = GetCardLogicValue(bCardData[2]);
				bStartID =3;
			}
		}

		//连牌判断
		if (bLogicValue<15)
		{
			//连牌判断
			bool bSeriesCard=false;
			for (BYTE i=bStartID;i<AnalyseResult.bSignedCount;i++)
			{
				if (GetCardLogicValue(bCardData[i])!=(bLogicValue-(i-bStartID+1))) break;
			}

			//单连判断
			if (i==AnalyseResult.bSignedCount) return CT_SINGLE_LINE;
		} 
	}

	return CT_ERROR;
}

//对比扑克
bool CGameLogic::CompareCard(const BYTE bFirstList[], const BYTE bNextList[], BYTE bFirstCount, BYTE bNextCount)
{
	//获取类型，1>2 返回true，1<2 返回false
	BYTE bNextType=GetCardType(bNextList,bNextCount);
	BYTE bFirstType=GetCardType(bFirstList,bFirstCount);

	//类型判断
	if (bFirstType==CT_ERROR) return false;

	if(bFirstType ==CT_MISSILE_CARD)return true;
	else if(bNextType==CT_MISSILE_CARD)return false;
	//炸弹判断
	if ((bFirstType==CT_BOMB_CARD)&&(bNextType!=CT_BOMB_CARD)) return true;
	if ((bFirstType!=CT_BOMB_CARD)&&(bNextType==CT_BOMB_CARD)) return false;

	//炸弹对比
	if ((bFirstType==CT_BOMB_CARD)&&(bNextType==CT_BOMB_CARD))
	{
		//四王炸弹
		if (bFirstCount==4)
		{
			BYTE bLastLogicValue=GetCardLogicValue(bFirstList[3]);
			BYTE bFirstLogicValue=GetCardLogicValue(bFirstList[0]);
			if ((bFirstLogicValue==16)&&(bLastLogicValue==16)) return true;
		}

		//四王炸弹
		if (bNextCount==4)
		{
			BYTE bLastLogicValue=GetCardLogicValue(bNextList[3]);
			BYTE bFirstLogicValue=GetCardLogicValue(bNextList[0]);
			if ((bFirstLogicValue==16)&&(bLastLogicValue==16)) return false;
		}

		//数目对比
		if (bFirstCount>bNextCount) return true;

		//数值对比
		if (bFirstCount==bNextCount)
		{
			BYTE bNextLogicValue=GetCardLogicValue(bNextList[0]);
			BYTE bFirstLogicValue=GetCardLogicValue(bFirstList[0]);
			return (bFirstLogicValue>bNextLogicValue);
		}

		return false;
	}

	//规则判断
	if ((bFirstType!=bNextType)||(bFirstCount!=bNextCount)) return false;

	//开始对比
	switch (bNextType)
	{
	case CT_SINGLE:
	case CT_DOUBLE:
	case CT_THREE_TOU:
		{
			BYTE bNextLogicValue=GetCardLogicValue(bNextList[0]);
			BYTE bFirstLogicValue=GetCardLogicValue(bFirstList[0]);
			return bFirstLogicValue>bNextLogicValue;
		}
	case CT_SINGLE_LINE:
	case CT_DOUBLE_LINE:
	case CT_THREE_LINE:
		{
			BYTE bNextLogicValue=GetCardLogicValue(bNextList[0]);
			BYTE bFirstLogicValue=GetCardLogicValue(bFirstList[0]);
			if(bNextLogicValue == 15 && bFirstLogicValue != 15) return true;
			else if(bFirstLogicValue == 15 && bNextLogicValue != 15) return false;
			else if(bFirstLogicValue ==15 && bNextLogicValue ==15){
				BYTE bFirstLogicValidValue=0, bNextLogicValidValue=0;
				for(int i=1; i<bFirstCount; i++){
					if(GetCardLogicValue(bFirstList[i]) <14){
						bFirstLogicValidValue = GetCardLogicValue(bFirstList[i]);
						break;
					}
				}
				for(int i=1; i<bNextCount; i++){
					if(GetCardLogicValue(bNextList[i]) <14){
						bNextLogicValidValue = GetCardLogicValue(bNextList[i]);
						break;
					}
				}
				return bFirstLogicValidValue >bNextLogicValidValue; 
			}
			return bFirstLogicValue>bNextLogicValue;
		}
	case CT_THREE_LINE_TAKE_DOUBLE:
		{
			BYTE bFirstLogicValue = GetCardLogicValue(bFirstList[0]);
			BYTE bCount =1;
			BYTE bFirstLogicValidValue=0, bNextLogicValidValue=0;
			for(int i=1; i<bFirstCount; i++){
				if(bFirstLogicValue == GetCardLogicValue(bFirstList[i])){
					bCount++;
				} else {
					bCount =1;
					bFirstLogicValue = GetCardLogicValue(bFirstList[i]);
				}
				if(bCount ==3){
					bFirstLogicValidValue = GetCardLogicValue(bFirstList[i]);
					break;
				}
			}
			BYTE bNextLogicValue = GetCardLogicValue(bNextList[0]);
			bCount =1;
			for(int i=1; i<bNextCount; i++){
				if(bNextLogicValue == GetCardLogicValue(bNextList[i])){
					bCount++;
				} else {
					bCount =1;
					bNextLogicValue = GetCardLogicValue(bNextList[i]);
				}
				if(bCount ==3){
					bNextLogicValidValue = GetCardLogicValue(bNextList[i]);
					break;
				}
			}
			ASSERT(bFirstLogicValidValue &&bNextLogicValidValue);
			return bFirstLogicValidValue>bNextLogicValidValue;;
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
					AnalyseResult.bDoubleLogicVolue[AnalyseResult.bDoubleCount++]=bLogicValue;
					break;
				}
			case 3:		//三张
				{
					AnalyseResult.bThreeLogicVolue[AnalyseResult.bThreeCount++]=bLogicValue;
					break;
				}
			case 4:		//四张
				{
					AnalyseResult.bFourLogicVolue[AnalyseResult.bFourCount++]=bLogicValue;
					break;
				}
			}
		}

		//设置变量
		if ((bCardValueTemp!=bLogicValue)&&(i!=(bCardCount-1)))
		{
			bSameCount=1;
			bLogicValue=bCardValueTemp;
		}
	}

	//单牌数目
	BYTE bOtherCount=AnalyseResult.bDoubleCount*2+AnalyseResult.bThreeCount*3+AnalyseResult.bFourCount*4;
	AnalyseResult.bSignedCount=bCardCount-bOtherCount;

	return;
}
//////////////////////////////////////////////////////////////////////////
