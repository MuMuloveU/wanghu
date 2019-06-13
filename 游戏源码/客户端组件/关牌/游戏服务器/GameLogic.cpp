#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////
//静态变量

//索引变量
const BYTE cbIndexCount=5;

//扑克数据
const BYTE	CGameLogic::m_cbCardData[FULL_COUNT]=
{
	     0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,			//方块 A - K
		 0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,			//梅花 A - K
		 0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,			//红桃 A - K
	0x31,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,	//黑桃 A - K
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
BYTE CGameLogic::GetCardType(const BYTE cbCardData[], BYTE cbCardCount)
{
	//简单牌型
	switch (cbCardCount)
	{
	case 0:	//空牌
		{
			return CT_ERROR;
		}
	case 1: //单牌
		{
			return CT_SINGLE;
		}
	case 2:	//对牌
		{
			if (GetCardValue(cbCardData[0])==GetCardValue(cbCardData[1])) return CT_DOUBLE;
			return CT_ERROR;
		}
	case 3:	//三条
		{
			if (GetCardValue(cbCardData[0])==GetCardValue(cbCardData[2])) return CT_THREE;
			return CT_ERROR;
		}
	}

	//分析扑克
	tagAnalyseResult AnalyseResult;
	AnalysebCardData(cbCardData,cbCardCount,AnalyseResult);

	//炸弹类型
	if ((AnalyseResult.cbBlockCount[3]==1)&&((cbCardCount==4)||(cbCardCount==5))) return CT_BOMB;
	if ((AnalyseResult.cbBlockCount[2]==1)&&(cbCardCount==4)&&((AnalyseResult.cbCardData[2][0]&MASK_VALUE)==1)) return CT_BOMB;
	if ((AnalyseResult.cbBlockCount[2]==1)&&(cbCardCount==4)&&((AnalyseResult.cbCardData[2][0]&MASK_VALUE)==0x0D)) return CT_BOMB;

	//单连类型
	if ((cbCardCount>=5)&&(AnalyseResult.cbBlockCount[0]==cbCardCount))
	{
		BYTE cbSignedCount=AnalyseResult.cbBlockCount[0];
		if (IsStructureLink(AnalyseResult.cbCardData[0],cbSignedCount,1)==true) return CT_SINGLE_LINK;
	}

	//对连类型
	if ((cbCardCount>=4)&&((AnalyseResult.cbBlockCount[1]*2)==cbCardCount))
	{
		BYTE cbDoubleCount=AnalyseResult.cbBlockCount[1]*2;
		if (IsStructureLink(AnalyseResult.cbCardData[1],cbDoubleCount,2)==true) return CT_DOUBLE_LINK;
	}

	//三连类型
	if ((cbCardCount>=6)&&((AnalyseResult.cbBlockCount[2]*3)==cbCardCount))
	{
		BYTE cbThreeCount=AnalyseResult.cbBlockCount[2]*3;
		if (IsStructureLink(AnalyseResult.cbCardData[2],cbThreeCount,3)==true) return CT_THREE_LINK;
	}

	//三带二型
	if ((cbCardCount==5)&&((AnalyseResult.cbBlockCount[1]*2+AnalyseResult.cbBlockCount[2]*3)==cbCardCount))
	{
		if (AnalyseResult.cbBlockCount[1]!=AnalyseResult.cbBlockCount[2]) return CT_ERROR;
		if (IsStructureLink(AnalyseResult.cbCardData[1],AnalyseResult.cbBlockCount[1]*2,2)==false) return CT_ERROR;
		if (IsStructureLink(AnalyseResult.cbCardData[2],AnalyseResult.cbBlockCount[2]*3,3)==false) return CT_ERROR;
		return CT_THREE_DOUBLE_LINK;
	}

	return CT_ERROR;
}

//排列扑克
VOID CGameLogic::SortCardList(BYTE cbCardData[], BYTE cbCardCount)
{
	//数目过虑
	if (cbCardCount==0) return;

	//转换数值
	BYTE cbSortValue[MAX_COUNT];
	for (BYTE i=0;i<cbCardCount;i++) 
	{
		cbSortValue[i]=GetCardLogicValue(cbCardData[i]);
	}

	//排序操作
	bool bSorted=true;
	BYTE cbSwitchData=0,cbLast=cbCardCount-1;
	do
	{
		bSorted=true;
		for (BYTE i=0;i<cbLast;i++)
		{
			if ((cbSortValue[i]<cbSortValue[i+1])||
				((cbSortValue[i]==cbSortValue[i+1])&&(cbCardData[i]<cbCardData[i+1])))
			{
				//设置标志
				bSorted=false;

				//扑克数据
				cbSwitchData=cbCardData[i];
				cbCardData[i]=cbCardData[i+1];
				cbCardData[i+1]=cbSwitchData;

				//排序权位
				cbSwitchData=cbSortValue[i];
				cbSortValue[i]=cbSortValue[i+1];
				cbSortValue[i+1]=cbSwitchData;
			}	
		}
		cbLast--;
	} while(bSorted==false);

	return;
}

//混乱扑克
VOID CGameLogic::RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//混乱准备
	BYTE cbCardData[CountArray(m_cbCardData)];
	CopyMemory(cbCardData,m_cbCardData,sizeof(m_cbCardData));

	//混乱扑克
	BYTE cbRandCount=0,cbPosition=0;
	do
	{
		cbPosition=rand()%(cbBufferCount-cbRandCount);
		cbCardBuffer[cbRandCount++]=cbCardData[cbPosition];
		cbCardData[cbPosition]=cbCardData[cbBufferCount-cbRandCount];
	} while (cbRandCount<cbBufferCount);

	return;
}

//删除扑克
bool CGameLogic::RemoveCard(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount)
{
	//检验数据
	ASSERT(cbRemoveCount<=cbCardCount);

	//定义变量
	BYTE cbDeleteCount=0,cbTempCardData[MAX_COUNT];
	if (cbCardCount>CountArray(cbTempCardData)) return false;
	CopyMemory(cbTempCardData,cbCardData,cbCardCount*sizeof(cbCardData[0]));

	//置零扑克
	for (BYTE i=0;i<cbRemoveCount;i++)
	{
		for (BYTE j=0;j<cbCardCount;j++)
		{
			if (cbRemoveCard[i]==cbTempCardData[j])
			{
				cbDeleteCount++;
				cbTempCardData[j]=0;
				break;
			}
		}
	}
	if (cbDeleteCount!=cbRemoveCount) return false;

	//清理扑克
	BYTE cbCardPos=0;
	for (BYTE i=0;i<cbCardCount;i++)
	{
		if (cbTempCardData[i]!=0) cbCardData[cbCardPos++]=cbTempCardData[i];
	}

	return true;
}

//逻辑数值
BYTE CGameLogic::GetCardLogicValue(BYTE cbCardData)
{
	//扑克属性
	BYTE cbCardColor=GetCardColor(cbCardData);
	BYTE cbCardValue=GetCardValue(cbCardData);

	//转换数值
	return (cbCardValue<=2)?(cbCardValue+13):cbCardValue;
}

//对比扑克
bool CGameLogic::CompareCard(BYTE cbFirstCard, BYTE cbNextCard)
{
	//获取数值
	BYTE cbNextValue=GetCardLogicValue(cbNextCard);
	BYTE cbFirstValue=GetCardLogicValue(cbFirstCard);

	return (cbFirstValue>cbNextValue);
}

//对比扑克
bool CGameLogic::CompareCard(const BYTE cbFirstCard[], const BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount)
{
	//类型判断
	BYTE cbNextType=GetCardType(cbNextCard,cbNextCount);
	BYTE cbFirstType=GetCardType(cbFirstCard,cbFirstCount);

	//不同类型
	if (cbFirstType!=cbNextType)
	{
		//炸弹类型
		if (cbNextType==CT_BOMB) return false;
		if (cbFirstType==CT_BOMB) return true;
		return false;
	}

	//相同类型
	switch (cbFirstType)
	{
	case CT_SINGLE:				//单牌类型
	case CT_DOUBLE:				//对子类型
	case CT_THREE:				//三条类型
	case CT_SINGLE_LINK:		//单连类型
	case CT_DOUBLE_LINK:		//姐妹对型
	case CT_THREE_LINK:			//三连类型
		{
			//数目判断
			if (cbFirstCount!=cbNextCount) return false;

			//扑克对比
			return CompareCard(cbFirstCard[0],cbNextCard[0]);
		}
	case CT_BOMB:				//炸弹类型
		{
			return CompareCard(cbFirstCard[1],cbNextCard[1]);
		}
	case CT_THREE_DOUBLE_LINK:	//三带对型
		{
			//分析扑克
			tagAnalyseResult AnalyseResultNext;
			tagAnalyseResult AnalyseResultFirst;
			AnalysebCardData(cbNextCard,cbNextCount,AnalyseResultNext);
			AnalysebCardData(cbFirstCard,cbFirstCount,AnalyseResultFirst);

			//对比扑克
			return CompareCard(AnalyseResultFirst.cbCardData[2][0],AnalyseResultNext.cbCardData[2][0]);
		}
	}

	//错误断言
	ASSERT(FALSE);
	
	return false;
}

//出牌搜索
bool CGameLogic::SearchOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResult & OutCardResult)
{
	//设置结果
	ZeroMemory(&OutCardResult,sizeof(OutCardResult));

	//构造扑克
	BYTE cbCardData[MAX_COUNT];
	BYTE cbCardCount=cbHandCardCount;
	CopyMemory(cbCardData,cbHandCardData,sizeof(BYTE)*cbHandCardCount);

	//排列扑克
	SortCardList(cbCardData,cbCardCount);

	return false;
}

//构造扑克
BYTE CGameLogic::MakeCardData(BYTE cbValueIndex, BYTE cbColorIndex)
{
	//构造扑克
	switch (cbValueIndex)
	{
	case 11:	//A 扑克
		{
			return (cbColorIndex<<4)|0x01;
		}
	case 12:	//2 扑克
		{
			return (cbColorIndex<<4)|0x02;
		}
	case 13:	//小鬼扑克
		{
			return (cbColorIndex<<4)|0x01;
		}
	case 14:	//大鬼扑克
		{
			return (cbColorIndex<<4)|0x02;
		}
	default:	//其他扑克
		{
			return (cbColorIndex<<4)|(cbValueIndex+3);
		}
	}

	return 0x00;
}

//是否连牌
bool CGameLogic::IsStructureLink(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbCellCount)
{
	//数目判断
	ASSERT((cbCardCount%cbCellCount)==0);
	if ((cbCardCount%cbCellCount)!=0) return false;
	
	//变量定义
	BYTE cbBlockCount=cbCardCount/cbCellCount;
	BYTE cbFirstValue=GetCardLogicValue(cbCardData[0]);

	//特殊处理
	if (cbFirstValue==15)
	{
		for (BYTE i=1;i<cbBlockCount;i++)
		{
			//获取数值
			BYTE cbLogicValue=GetCardLogicValue(cbCardData[i*cbCellCount]);

			//连牌判断
			if ((cbLogicValue==14)&&(i!=1)) return false;
			if ((cbLogicValue!=14)&&((cbLogicValue+i)!=(cbBlockCount+2))) return false;
		}

		return true;
	}

	//分析处理
	for (BYTE i=1;i<cbBlockCount;i++)
	{
		if (cbFirstValue!=(GetCardLogicValue(cbCardData[i*cbCellCount])+i)) return false;
	}

	return true;
}

//分析扑克
VOID CGameLogic::AnalysebCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResult & AnalyseResult)
{
	//设置结果
	ZeroMemory(&AnalyseResult,sizeof(AnalyseResult));

	//扑克分析
	for (BYTE i=0;i<cbCardCount;i++)
	{
		//变量定义
		BYTE cbSameCount=1,cbCardValueTemp=0;
		BYTE cbLogicValue=GetCardLogicValue(cbCardData[i]);

		//搜索同牌
		for (BYTE j=i+1;j<cbCardCount;j++)
		{
			//获取扑克
			if (GetCardLogicValue(cbCardData[j])!=cbLogicValue) break;

			//设置变量
			cbSameCount++;
		}

		//设置结果
		switch (cbSameCount)
		{
		case 1:		//单张
			{
				BYTE cbIndex=AnalyseResult.cbBlockCount[0]++;
				AnalyseResult.cbCardData[0][cbIndex*cbSameCount]=cbCardData[i];
				break;
			}
		case 2:		//两张
			{
				BYTE cbIndex=AnalyseResult.cbBlockCount[1]++;
				AnalyseResult.cbCardData[1][cbIndex*cbSameCount]=cbCardData[i];
				AnalyseResult.cbCardData[1][cbIndex*cbSameCount+1]=cbCardData[i+1];
				break;
			}
		case 3:		//三张
			{
				BYTE cbIndex=AnalyseResult.cbBlockCount[2]++;
				AnalyseResult.cbCardData[2][cbIndex*cbSameCount]=cbCardData[i];
				AnalyseResult.cbCardData[2][cbIndex*cbSameCount+1]=cbCardData[i+1];
				AnalyseResult.cbCardData[2][cbIndex*cbSameCount+2]=cbCardData[i+2];
				break;
			}
		case 4:		//四张
			{
				BYTE cbIndex=AnalyseResult.cbBlockCount[3]++;
				AnalyseResult.cbCardData[3][cbIndex*cbSameCount]=cbCardData[i];
				AnalyseResult.cbCardData[3][cbIndex*cbSameCount+1]=cbCardData[i+1];
				AnalyseResult.cbCardData[3][cbIndex*cbSameCount+2]=cbCardData[i+2];
				AnalyseResult.cbCardData[3][cbIndex*cbSameCount+3]=cbCardData[i+3];
				break;
			}
		}

		//设置索引
		i+=cbSameCount-1;
	}

	return;
}

//分析分布
VOID CGameLogic::AnalysebDistributing(const BYTE cbCardData[], BYTE cbCardCount, tagDistributing & Distributing)
{
	//设置变量
	ZeroMemory(&Distributing,sizeof(Distributing));

	//设置变量
	for (BYTE i=0;i<cbCardCount;i++)
	{
		if (cbCardData[i]==0) continue;

		//获取属性
		BYTE cbCardColor=GetCardColor(cbCardData[i]);
		BYTE cbLogicValue=GetCardLogicValue(cbCardData[i]);

		//分布信息
		Distributing.cbCardCount++;
		Distributing.cbDistributing[cbLogicValue-3][cbIndexCount]++;
		Distributing.cbDistributing[cbLogicValue-3][cbCardColor>>4]++;
	}

	return;
}

//////////////////////////////////////////////////////////////////////////
