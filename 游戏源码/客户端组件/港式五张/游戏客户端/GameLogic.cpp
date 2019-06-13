#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////

//扑克数据
BYTE CGameLogic::m_cbCardListData[32]=
{
	0x01,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,							//方块 A - K
	0x11,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,							//梅花 A - K
	0x21,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,							//红桃 A - K
	0x31,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,							//黑桃 A - K
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
BYTE CGameLogic::GetCardType(BYTE cbCardData[], BYTE cbCardCount)
{
	//简单牌形
	switch (cbCardCount)
	{
	case 1: //单牌
		{
			return CT_SINGLE;
		}
	case 2:	//对牌
		{
			return (GetCardLogicValue(cbCardData[0])==GetCardLogicValue(cbCardData[1]))?CT_ONE_DOUBLE:CT_SINGLE;
		}
	}

	//五条类型
	if (cbCardCount==5)
	{
		//变量定义
		bool cbSameColor=true,bLineCard=true;
		BYTE cbFirstColor=GetCardColor(cbCardData[0]);
		BYTE cbFirstValue=GetCardLogicValue(cbCardData[0]);

		//牌形分析
		for (BYTE i=1;i<cbCardCount;i++)
		{
			//数据分析
			if (GetCardColor(cbCardData[i])!=cbFirstColor) cbSameColor=false;
			if (cbFirstValue!=(GetCardLogicValue(cbCardData[i])+i)) bLineCard=false;

			//结束判断
			if ((cbSameColor==false)&&(bLineCard==false)) break;
		}

		//顺子类型
		if ((cbSameColor==false)&&(bLineCard==true)) return CT_SHUN_ZI;
		
		//同花类型
		if ((cbSameColor==true)&&(bLineCard==false)) return CT_TONG_HUA;

		//同花顺类型
		if ((cbSameColor==true)&&(bLineCard==true)) return CT_TONG_HUA_SHUN;
	}

	//扑克分析
	tagAnalyseResult AnalyseResult;
	AnalysebCardData(cbCardData,cbCardCount,AnalyseResult);

	//类型判断
	if (AnalyseResult.cbFourCount==1) return CT_TIE_ZHI;
	if (AnalyseResult.cbDoubleCount==2) return CT_TWO_DOUBLE;
	if ((AnalyseResult.cbDoubleCount==1)&&(AnalyseResult.cbThreeCount==1)) return CT_HU_LU;
	if ((AnalyseResult.cbThreeCount==1)&&(AnalyseResult.cbDoubleCount==0)) return CT_THREE_TIAO;
	if ((AnalyseResult.cbDoubleCount==1)&&(AnalyseResult.cbSignedCount==3)) return CT_ONE_DOUBLE;

	return CT_SINGLE;
}

//排列扑克
void CGameLogic::SortCardList(BYTE cbCardData[], BYTE cbCardCount)
{
	//转换数值
	BYTE cbLogicValue[MAX_COUNT];
	for (BYTE i=0;i<cbCardCount;i++) cbLogicValue[i]=GetCardLogicValue(cbCardData[i]);	

	//排序操作
	bool bSorted=true;
	BYTE cbTempData,bLast=cbCardCount-1;
	do
	{
		bSorted=true;
		for (BYTE i=0;i<bLast;i++)
		{
			if ((cbLogicValue[i]<cbLogicValue[i+1])||
				((cbLogicValue[i]==cbLogicValue[i+1])&&(cbCardData[i]<cbCardData[i+1])))
			{
				//交换位置
				cbTempData=cbCardData[i];
				cbCardData[i]=cbCardData[i+1];
				cbCardData[i+1]=cbTempData;
				cbTempData=cbLogicValue[i];
				cbLogicValue[i]=cbLogicValue[i+1];
				cbLogicValue[i+1]=cbTempData;
				bSorted=false;
			}	
		}
		bLast--;
	} while(bSorted==false);

	return;
}

//混乱扑克
void CGameLogic::RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//混乱准备
	BYTE cbCardData[CountArray(m_cbCardListData)];
	CopyMemory(cbCardData,m_cbCardListData,sizeof(m_cbCardListData));

	//混乱扑克
	BYTE bRandCount=0,bPosition=0;
	do
	{
		bPosition=rand()%(CountArray(m_cbCardListData)-bRandCount);
		cbCardBuffer[bRandCount++]=cbCardData[bPosition];
		cbCardData[bPosition]=cbCardData[CountArray(m_cbCardListData)-bRandCount];
	} while (bRandCount<cbBufferCount);

	return;
}

//逻辑数值
BYTE CGameLogic::GetCardLogicValue(BYTE cbCardData)
{
	//扑克属性
	BYTE bCardColor=GetCardColor(cbCardData);
	BYTE bCardValue=GetCardValue(cbCardData);

	//转换数值
	return (bCardValue<=2)?(bCardValue+13):bCardValue;
}

//对比扑克
bool CGameLogic::CompareCard(BYTE cbFirstData[], BYTE cbNextData[], BYTE cbCardCount)
{
	//获取类型
	BYTE cbNextType=GetCardType(cbNextData,cbCardCount);
	BYTE cbFirstType=GetCardType(cbFirstData,cbCardCount);

	//类型判断
	if (cbFirstType!=cbNextType) return (cbFirstType>cbNextType);

	//简单类型
	switch(cbFirstType)
	{
	case CT_SINGLE:			//单牌
		{
			//对比数值
			for (BYTE i=0;i<cbCardCount;i++)
			{
				BYTE cbNextValue=GetCardLogicValue(cbNextData[i]);
				BYTE cbFirstValue=GetCardLogicValue(cbFirstData[i]);
				if (cbFirstValue!=cbNextValue) return cbFirstValue>cbNextValue;
			}

			//对比花色
			return GetCardColor(cbFirstData[0])>GetCardColor(cbNextData[0]);
		}
	case CT_HU_LU:			//葫芦
	case CT_TIE_ZHI:		//铁支
	case CT_ONE_DOUBLE:		//对子
	case CT_TWO_DOUBLE:		//两对
	case CT_THREE_TIAO:		//三条
		{
			//分析扑克
			tagAnalyseResult AnalyseResultNext;
			tagAnalyseResult AnalyseResultFirst;
			AnalysebCardData(cbNextData,cbCardCount,AnalyseResultNext);
			AnalysebCardData(cbFirstData,cbCardCount,AnalyseResultFirst);

			//四条数值
			if (AnalyseResultFirst.cbFourCount>0)
			{
				BYTE cbNextValue=AnalyseResultNext.cbFourLogicVolue[0];
				BYTE cbFirstValue=AnalyseResultFirst.cbFourLogicVolue[0];
				return cbFirstValue>cbNextValue;
			}

			//三条数值
			if (AnalyseResultFirst.cbThreeCount>0)
			{
				BYTE cbNextValue=AnalyseResultNext.cbThreeLogicVolue[0];
				BYTE cbFirstValue=AnalyseResultFirst.cbThreeLogicVolue[0];
				return cbFirstValue>cbNextValue;
			}

			//对子数值
			for (BYTE i=0;i<AnalyseResultFirst.cbDoubleCount;i++)
			{
				BYTE cbNextValue=AnalyseResultNext.cbDoubleLogicVolue[i];
				BYTE cbFirstValue=AnalyseResultFirst.cbDoubleLogicVolue[i];
				if (cbFirstValue!=cbNextValue) return cbFirstValue>cbNextValue;
			}

			//散牌数值
			for (BYTE i=0;i<AnalyseResultFirst.cbSignedCount;i++)
			{
				BYTE cbNextValue=AnalyseResultNext.cbSignedLogicVolue[i];
				BYTE cbFirstValue=AnalyseResultFirst.cbSignedLogicVolue[i];
				if (cbFirstValue!=cbNextValue) return cbFirstValue>cbNextValue;
			}

			//对子花色
			if (AnalyseResultFirst.cbDoubleCount>0)
			{
				BYTE cbNextColor=GetCardColor(AnalyseResultNext.cbDoubleCardData[0]);
				BYTE cbFirstColor=GetCardColor(AnalyseResultFirst.cbDoubleCardData[0]);
				return cbFirstColor>cbNextColor;
			}

			//散牌花色
			if (AnalyseResultFirst.cbSignedCount>0)
			{
				BYTE cbNextColor=GetCardColor(AnalyseResultNext.cbSignedCardData[0]);
				BYTE cbFirstColor=GetCardColor(AnalyseResultFirst.cbSignedCardData[0]);
				return cbFirstColor>cbNextColor;
			}

			break;
		}
	case CT_SHUN_ZI:		//顺子
	case CT_TONG_HUA:		//同花
	case CT_TONG_HUA_SHUN:	//同花顺
		{
			//数值判断
			BYTE cbNextValue=GetCardLogicValue(cbNextData[0]);
			BYTE cbFirstValue=GetCardLogicValue(cbFirstData[0]);
			if (cbFirstValue!=cbNextValue) return (cbFirstValue>cbNextValue);

			//花色判断
			BYTE cbNextColor=GetCardColor(cbNextData[0]);
			BYTE cbFirstColor=GetCardColor(cbFirstData[0]);

			return (cbFirstColor>cbNextColor);
		}
	}

	return false;
}

//分析扑克
void CGameLogic::AnalysebCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResult & AnalyseResult)
{
	//设置结果
	ZeroMemory(&AnalyseResult,sizeof(AnalyseResult));

	//扑克分析
	for (BYTE i=0;i<cbCardCount;i++)
	{
		//变量定义
		BYTE cbSameCount=1;
		BYTE cbSameCardData[4]={cbCardData[i],0,0,0};
		BYTE cbLogicValue=GetCardLogicValue(cbCardData[i]);

		//获取同牌
		for (int j=i+1;j<cbCardCount;j++)
		{
			//逻辑对比
			if (GetCardLogicValue(cbCardData[j])!=cbLogicValue) break;

			//设置扑克
			cbSameCardData[cbSameCount++]=cbCardData[j];
		}

		//保存结果
		switch (cbSameCount)
		{
		case 1:		//单张
			{
				AnalyseResult.cbSignedLogicVolue[AnalyseResult.cbSignedCount]=cbLogicValue;
				CopyMemory(&AnalyseResult.cbSignedCardData[(AnalyseResult.cbSignedCount++)*cbSameCount],cbSameCardData,cbSameCount);
				break;
			}
		case 2:		//两张
			{
				AnalyseResult.cbDoubleLogicVolue[AnalyseResult.cbDoubleCount]=cbLogicValue;
				CopyMemory(&AnalyseResult.cbDoubleCardData[(AnalyseResult.cbDoubleCount++)*cbSameCount],cbSameCardData,cbSameCount);
				break;
			}
		case 3:		//三张
			{
				AnalyseResult.cbThreeLogicVolue[AnalyseResult.cbThreeCount]=cbLogicValue;
				CopyMemory(&AnalyseResult.cbThreeCardData[(AnalyseResult.cbThreeCount++)*cbSameCount],cbSameCardData,cbSameCount);
				break;
			}
		case 4:		//四张
			{
				AnalyseResult.cbFourLogicVolue[AnalyseResult.cbFourCount]=cbLogicValue;
				CopyMemory(&AnalyseResult.cbFourCardData[(AnalyseResult.cbFourCount++)*cbSameCount],cbSameCardData,cbSameCount);
				break;
			}
		}

		//设置递增
		i+=cbSameCount-1;
	}

	return;
}

//////////////////////////////////////////////////////////////////////////
