#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////
//静态变量

//扑克数据
const BYTE	CGameLogic::m_cbCardData[FULL_COUNT]=
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//方块 A - K
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//梅花 A - K
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//红桃 A - K
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,	//黑桃 A - K
	0x4E,0x4F,
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
	if (IsOnlyOne(cbCardData,cbCardCount)) return CT_SINGLE;//单牌　 

	if (IsDouble(cbCardData,cbCardCount)) return CT_DOUBLE;//对牌 

	if (IsSingleContinue(cbCardData,cbCardCount)) return CT_SINGLE_LINE;//单顺  

	if (IsDoubleContinue(cbCardData,cbCardCount)) return CT_DOUBLE_LINE;//双顺 

	if (IsThree(cbCardData,cbCardCount)) return CT_THREE;//三同张炸弹

	if (IsBomb(cbCardData,cbCardCount)) return CT_FOUR;//四张炸弹

	if (IsDoubleKong(cbCardData,cbCardCount)) return CT_MISSILE_CARD;//双王

	if ( Is44A( cbCardData,cbCardCount ) )
	{
		if (cbCardCount != 3)  return CT_ERROR;
		BYTE cbFourCount = 0;   //4的数目
		BYTE cbRed = 0;         //红4的数目
		BYTE cbBlack = 0;       //黑四的数目
		BYTE cbOne = 0;         //A的数目		

		for (BYTE i=0; i<3; i++)
		{
			if (cbCardData[i] == 0x04 || cbCardData[i] == 0x14 || cbCardData[i] == 0x24||cbCardData[i] == 0x34)
			{
				++cbFourCount;
				if (cbCardData[i] == 0x04 || cbCardData[i] == 0x24)
				{
					++cbRed;
				}
				if (cbCardData[i] == 0x14 || cbCardData[i] == 0x34)
				{
					++cbBlack;
				}
			}

			if (cbCardData[i] == 0x01 || cbCardData[i] == 0x11 || cbCardData[i] == 0x21||cbCardData[i] == 0x31)
			{
				++cbOne;
			}
		}

		if (cbFourCount == 2 && cbOne == 1)
		{
			if (cbRed == 2) return CT_A44_red;
			else if (cbBlack == 2) return CT_A44_black;
			else return CT_A44_red_black;

		}
	}
	return CT_ERROR;
}

//排列扑克//可以大小或数目ST_COUNT为数目ST_ORDER大小
void CGameLogic::SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType)
{
	//数目过虑
	if (cbCardCount==0) return;

	//转换数值
	BYTE cbSortValue[MAX_COUNT];
	for (BYTE i=0;i<cbCardCount;i++) cbSortValue[i]=GetCardLogicValue(cbCardData[i]);	

	//排序操作
	bool bSorted=true;
	BYTE cbThreeCount,cbLast=cbCardCount-1;
	do
	{
		bSorted=true;
		for (BYTE i=0;i<cbLast;i++)
		{
			if ((cbSortValue[i]<cbSortValue[i+1])||
				((cbSortValue[i]==cbSortValue[i+1])&&(cbCardData[i]<cbCardData[i+1])))
			{
				//交换位置
				cbThreeCount=cbCardData[i];
				cbCardData[i]=cbCardData[i+1];
				cbCardData[i+1]=cbThreeCount;
				cbThreeCount=cbSortValue[i];
				cbSortValue[i]=cbSortValue[i+1];
				cbSortValue[i+1]=cbThreeCount;
				bSorted=false;
			}	
		}
		cbLast--;
	} while(bSorted==false);

	//数目排序
	if (cbSortType==ST_COUNT)
	{
		//分析扑克
		BYTE cbIndex=0;
		tagAnalyseResult AnalyseResult;
		AnalysebCardData(cbCardData,cbCardCount,AnalyseResult);

		//拷贝四牌
		CopyMemory(&cbCardData[cbIndex],AnalyseResult.cbFourCardData,sizeof(BYTE)*AnalyseResult.cbFourCount*4);
		cbIndex+=AnalyseResult.cbFourCount*4;

		//拷贝三牌
		CopyMemory(&cbCardData[cbIndex],AnalyseResult.cbThreeCardData,sizeof(BYTE)*AnalyseResult.cbThreeCount*3);
		cbIndex+=AnalyseResult.cbThreeCount*3;

		//拷贝对牌
		CopyMemory(&cbCardData[cbIndex],AnalyseResult.cbDoubleCardData,sizeof(BYTE)*AnalyseResult.cbDoubleCount*2);
		cbIndex+=AnalyseResult.cbDoubleCount*2;

		//拷贝单牌
		CopyMemory(&cbCardData[cbIndex],AnalyseResult.cbSignedCardData,sizeof(BYTE)*AnalyseResult.cbSignedCount);
		cbIndex+=AnalyseResult.cbSignedCount;
	}

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
	if (cbCardColor==0x40) return cbCardValue+3;///////////////李A443大
	return (cbCardValue<=3)?(cbCardValue+13):cbCardValue;///////////////李A443大
}

//对比扑克	//出牌列表//自己扑克数据//出牌数目//自己的张数
bool CGameLogic::CompareCard(const BYTE cbFirstCard[], const BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount)
{

	//获取类型
	BYTE cbNextType=GetCardType(cbNextCard,cbNextCount);//自己
	BYTE cbFirstType=GetCardType(cbFirstCard,cbFirstCount);//上个出牌的玩家

	//类型判断
	if (cbNextType == CT_ERROR) return false;
	//自己出牌
	if (cbNextType == CT_A44_red || cbNextType == CT_A44_red_black) return true;
	//上家出牌
	if ( cbFirstType == CT_A44_red || cbFirstType == CT_A44_red_black ) return false;

	//牌型一样的情况
	if(cbNextType == cbFirstType)
	{
		switch (cbFirstType)
		{
		case CT_SINGLE:		//单牌类型
		case CT_DOUBLE:		//对牌类型
		case CT_SINGLE_LINE://单连类型
		case CT_THREE:		//三条类型
		case CT_DOUBLE_LINE://对连类型
		case CT_FOUR:		//四条类型
		{
			//获取数值
			BYTE cbNextLogicValue=GetCardLogicValue(cbNextCard[0]);
			BYTE cbFirstLogicValue=GetCardLogicValue(cbFirstCard[0]);
			
			if(cbFirstCount == cbNextCount)//对比扑克
				return cbNextLogicValue>cbFirstLogicValue;
			
		}
		}
	}
	//牌型不一样的情况
	else
	{
		switch (cbFirstType)
		{
		case CT_SINGLE:		//单牌类型
		case CT_DOUBLE:		//对牌类型
			{
				if(cbNextType >= 4 && cbNextType != 5)
				return true;
			}
		case CT_SINGLE_LINE://单连类型
			{
				if(cbNextType >= 4 /*&& cbNextType != 5*/)
					return true;
				if(cbNextType == 5)
				{
					BYTE cbNextLogicValue=GetCardLogicValue(cbNextCard[0]);
					BYTE cbFirstLogicValue=GetCardLogicValue(cbFirstCard[0]);

					//对比扑克
					return cbNextLogicValue>cbFirstLogicValue;
				}
			}
		case CT_THREE:		//三条类型
		case CT_DOUBLE_LINE://对连类型
			{
				if(cbNextType >= 6)
					return true;
			}
		case CT_FOUR:		//四条类型
			{
				if(cbNextType >= 7)
					return true;
			}
		case CT_MISSILE_CARD:		//四条类型
			{
				if(cbNextType >= 8)
					return true;
			}
		case CT_A44_black:		//四条类型
			{
				if(cbNextType >= 10)
					return true;
			}
		}
	
	
	}
	return false;
}

//出牌搜索//提示出牌===参数(手上扑克//扑克数目//出牌列表//出牌数目)
bool CGameLogic::SearchOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResult & OutCardResult)
{
	//设置结果
	ZeroMemory(&OutCardResult,sizeof(OutCardResult));

	//构造扑克
	BYTE cbCardData[MAX_COUNT];
	BYTE cbCardCount=cbHandCardCount;
	CopyMemory(cbCardData,cbHandCardData,sizeof(BYTE)*cbHandCardCount);

	//排列扑克(按大小)
	SortCardList(cbCardData,cbCardCount,ST_ORDER);

	//获取类型
	BYTE cbTurnOutType=GetCardType(cbTurnCardData,cbTurnCardCount);

	if((cbTurnOutType == CT_A44_red_black)||(cbTurnOutType ==CT_A44_red))return false;

	switch (cbTurnOutType)
	{
	case CT_ERROR:					//错误类型
		{
			//获取数值
			BYTE cbLogicValue=GetCardLogicValue(cbCardData[cbCardCount-1]);

			//多牌判断
			BYTE cbSameCount=1;
			for (BYTE i=1;i<cbCardCount;i++)
			{
				if (GetCardLogicValue(cbCardData[cbCardCount-i-1])==cbLogicValue) cbSameCount++;
				else break;
			}

			//完成处理
			if (cbSameCount>1)
			{
				OutCardResult.cbCardCount=cbSameCount;
				for (BYTE j=0;j<cbSameCount;j++) OutCardResult.cbResultCard[j]=cbCardData[cbCardCount-1-j];
				return true;
			}

			//单牌处理
			OutCardResult.cbCardCount=1;
			OutCardResult.cbResultCard[0]=cbCardData[cbCardCount-1];

			return true;
		}
	case CT_SINGLE:					//单牌类型
	case CT_DOUBLE:					//对牌类型
	case CT_THREE:					//三条类型
		{
			//获取数值
			BYTE cbLogicValue=GetCardLogicValue(cbTurnCardData[0]);

			//分析扑克
			tagAnalyseResult AnalyseResult;
			AnalysebCardData(cbCardData,cbCardCount,AnalyseResult);
			//寻找单牌
			if (cbTurnCardCount<=1)
			{
				for (BYTE i=0;i<AnalyseResult.cbSignedCount;i++)
				{
					BYTE cbIndex=AnalyseResult.cbSignedCount-i-1;
					if (GetCardLogicValue(AnalyseResult.cbSignedCardData[cbIndex])>cbLogicValue)
					{
						//设置结果
						OutCardResult.cbCardCount=cbTurnCardCount;
						CopyMemory(OutCardResult.cbResultCard,&AnalyseResult.cbSignedCardData[cbIndex],sizeof(BYTE)*cbTurnCardCount);

						return true;
					}
				}
			}

			//寻找对牌
			if (cbTurnCardCount<=2)
			{
				for (BYTE i=0;i<AnalyseResult.cbDoubleCount;i++)
				{
					BYTE cbIndex=(AnalyseResult.cbDoubleCount-i-1)*2;
					if (GetCardLogicValue(AnalyseResult.cbDoubleCardData[cbIndex])>cbLogicValue)
					{
						//设置结果
						OutCardResult.cbCardCount=cbTurnCardCount;
						CopyMemory(OutCardResult.cbResultCard,&AnalyseResult.cbDoubleCardData[cbIndex],sizeof(BYTE)*cbTurnCardCount);

						return true;
					}
				}
			}

			//寻找三牌
			if (cbTurnCardCount<=4)
			{
				for (BYTE i=0;i<AnalyseResult.cbThreeCount;i++)
				{
					BYTE cbIndex=(AnalyseResult.cbThreeCount-i-1)*3;
					if (GetCardLogicValue(AnalyseResult.cbThreeCardData[cbIndex])>cbLogicValue)
					{
						//设置结果
						OutCardResult.cbCardCount=cbTurnCardCount;
						CopyMemory(OutCardResult.cbResultCard,&AnalyseResult.cbThreeCardData[cbIndex],sizeof(BYTE)*cbTurnCardCount);
						return true;
					}
				}
			}

			break;
		}
	case CT_SINGLE_LINE:			//单连类型
		{
			//长度判断
			if (cbCardCount<cbTurnCardCount) break;

			//获取数值
			BYTE cbLogicValue=GetCardLogicValue(cbTurnCardData[0]);

			//搜索连牌
			for (BYTE i=(cbTurnCardCount-1);i<cbCardCount;i++)
			{
				//获取数值
				BYTE cbHandLogicValue=GetCardLogicValue(cbCardData[cbCardCount-i-1]);

				//构造判断
				if (cbHandLogicValue>=15) break;
				if (cbHandLogicValue<=cbLogicValue) continue;

				//搜索连牌
				BYTE cbLineCount=0;
				for (BYTE j=(cbCardCount-i-1);j<cbCardCount;j++)
				{
					if ((GetCardLogicValue(cbCardData[j])+cbLineCount)==cbHandLogicValue) 
					{
						//增加连数
						OutCardResult.cbResultCard[cbLineCount++]=cbCardData[j];

						//完成判断
						if (cbLineCount==cbTurnCardCount)
						{
							OutCardResult.cbCardCount=cbTurnCardCount;
							return true;
						}
					}
				}
			}

			break;
		}
	case CT_DOUBLE_LINE:			//对连类型
		{
			//长度判断
			if (cbCardCount<cbTurnCardCount) break;

			//获取数值
			BYTE cbLogicValue=GetCardLogicValue(cbTurnCardData[0]);

			//搜索连牌
			for (BYTE i=(cbTurnCardCount-1);i<cbCardCount;i++)
			{
				//获取数值
				BYTE cbHandLogicValue=GetCardLogicValue(cbCardData[cbCardCount-i-1]);

				//构造判断
				if (cbHandLogicValue<=cbLogicValue) continue;
				if ((cbTurnCardCount>1)&&(cbHandLogicValue>=15)) break;

				//搜索连牌
				BYTE cbLineCount=0;
				for (BYTE j=(cbCardCount-i-1);j<(cbCardCount-1);j++)
				{
					if (((GetCardLogicValue(cbCardData[j])+cbLineCount)==cbHandLogicValue)
						&&((GetCardLogicValue(cbCardData[j+1])+cbLineCount)==cbHandLogicValue))
					{
						//增加连数
						OutCardResult.cbResultCard[cbLineCount*2]=cbCardData[j];
						OutCardResult.cbResultCard[(cbLineCount++)*2+1]=cbCardData[j+1];

						//完成判断
						if (cbLineCount*2==cbTurnCardCount)
						{
							OutCardResult.cbCardCount=cbTurnCardCount;
							return true;
						}
					}
				}
			}

			break;
		}
	}
	//搜索炸弹
	if ((cbCardCount==4)&&(cbTurnOutType < CT_MISSILE_CARD))
	{
		//变量定义
		BYTE cbLogicValue=0;
		if (cbTurnOutType==CT_FOUR) cbLogicValue=GetCardLogicValue(cbTurnCardData[0]);

		//排列扑克
		SortCardList(cbCardData,cbCardCount,ST_ORDER);
		//搜索炸弹
		for(int i = 0; i <  cbCardCount - 3 ;i++)
		{
			if((GetCardValue(cbCardData[i]) == GetCardValue(cbCardData[i-1])) && (GetCardValue(cbCardData[i])== GetCardValue(cbCardData[i-2]))&& (GetCardValue(cbCardData[i])== GetCardValue(cbCardData[i-3]))) 
			{
				if( GetCardLogicValue(cbCardData[i])>cbLogicValue)
				{
					OutCardResult.cbCardCount=4;
					OutCardResult.cbResultCard[0]=cbCardData[i];
					OutCardResult.cbResultCard[1]=cbCardData[i-1];
					OutCardResult.cbResultCard[2]=cbCardData[i-2];
					OutCardResult.cbResultCard[3]=cbCardData[i-3];
					return true;
				}
				else
				{
					OutCardResult.cbCardCount=4;
					OutCardResult.cbResultCard[0]=cbCardData[i];
					OutCardResult.cbResultCard[1]=cbCardData[i-1];
					OutCardResult.cbResultCard[2]=cbCardData[i-2];
					OutCardResult.cbResultCard[3]=cbCardData[i-3];
					return true;				
				}
			}
			
		}

	}
		//搜索火箭
	if ((cbCardCount>=2)&&(cbCardData[0]==0x4F)&&(cbCardData[1]==0x4E))
	{
		//设置结果
		OutCardResult.cbCardCount=2;
		OutCardResult.cbResultCard[0]=cbCardData[0];
		OutCardResult.cbResultCard[1]=cbCardData[1];

		return true;
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
				BYTE cbIndex=AnalyseResult.cbSignedCount++;
				AnalyseResult.cbSignedCardData[cbIndex*cbSameCount]=cbCardData[i];
				break;
			}
		case 2:		//两张
			{
				BYTE cbIndex=AnalyseResult.cbDoubleCount++;
				AnalyseResult.cbDoubleCardData[cbIndex*cbSameCount]=cbCardData[i];
				AnalyseResult.cbDoubleCardData[cbIndex*cbSameCount+1]=cbCardData[i+1];
				break;
			}
		case 3:		//三张
			{
				BYTE cbIndex=AnalyseResult.cbThreeCount++;
				AnalyseResult.cbThreeCardData[cbIndex*cbSameCount]=cbCardData[i];
				AnalyseResult.cbThreeCardData[cbIndex*cbSameCount+1]=cbCardData[i+1];
				AnalyseResult.cbThreeCardData[cbIndex*cbSameCount+2]=cbCardData[i+2];
				break;
			}
		case 4:		//四张
			{
				BYTE cbIndex=AnalyseResult.cbFourCount++;
				AnalyseResult.cbFourCardData[cbIndex*cbSameCount]=cbCardData[i];
				AnalyseResult.cbFourCardData[cbIndex*cbSameCount+1]=cbCardData[i+1];
				AnalyseResult.cbFourCardData[cbIndex*cbSameCount+2]=cbCardData[i+2];
				AnalyseResult.cbFourCardData[cbIndex*cbSameCount+3]=cbCardData[i+3];
				break;
			}
		}

		//设置索引
		i+=cbSameCount-1;
	}

	return;
}

//////////////////////////////////////////////////////////////////////////
//是否对牌
BOOL CGameLogic::IsDouble(const BYTE iCardList[], int iCardCount)
{
	if (iCardCount!=2) return FALSE;
	if (GetCardValue(iCardList[0]) == GetCardValue(iCardList[1]))
		return true;
	else 
		return false;
}
//是否三条
BOOL CGameLogic::IsThree(const BYTE iCardList[], int iCardCount)
{
	if (iCardCount!=3) return FALSE;
	return ((GetCardValue(iCardList[0])==GetCardValue(iCardList[1]))&&(GetCardValue(iCardList[0])==GetCardValue(iCardList[2])));
}
//是否单顺(至少3张)
BOOL CGameLogic::IsSingleContinue(const BYTE iCardList[], int iCardCount)
{
	if (iCardCount < 3) return FALSE;

	BYTE cbCardData[MAX_COUNT];
	CopyMemory(cbCardData,iCardList,sizeof(BYTE)*iCardCount);

	//排列扑克
	SortCardList(cbCardData,iCardCount,ST_ORDER);

	for (int i = 0;i < iCardCount - 1;i++)
	{
		if (GetCardValue(cbCardData[i]) == 2 || cbCardData[i] == 0x4E || cbCardData[i] == 0x4F||GetCardValue(cbCardData[i]) == 3 )
			return FALSE;
		if ((GetCardLogicValue(cbCardData[i]) % 15) != ((GetCardLogicValue(cbCardData[i+1]) + 1) % 15))
		{

			return FALSE;

		}
	}
	return TRUE;
}
//是否44毙
BOOL CGameLogic::Is44A(const BYTE iCardList[], int iCardCount)
{
	if (iCardCount != 3) return FALSE;

	int Number4 = 0;
	int NumberA = 0;
	for(int i = 0; i < 3; i++)
	{
		if(GetCardValue(iCardList[i]) == GetCardValue(0x04))
		{
			Number4++;
		}
		if(GetCardValue(iCardList[i]) == GetCardValue(0x01))
		{
			NumberA++;
		}
	}
	if( Number4 == 2 && NumberA == 1)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//是否双王
BOOL CGameLogic::IsDoubleKong(const BYTE iCardList[], int iCardCount)
{
	if(iCardCount != 2)  return FALSE;
	if((iCardList[0] == 0x4E && iCardList[1] == 0x4F)||(iCardList[0] == 0x4F && iCardList[1] == 0x4E))
		return TRUE;
	else
		return FALSE;
}
//是否炸弹
BOOL CGameLogic::IsBomb(const BYTE iCardList[], int iCardCount)
{
	if (iCardCount < 4) return FALSE;

	int pd1 = GetCardValue(iCardList[0]);

	for (int i = 0;i < iCardCount;i++)
	{
		if (GetCardValue(iCardList[i]) != pd1) return FALSE;
		else continue;
	}

	return TRUE;
}
//是否双顺 6张或6张以上，必须是以对为基础，花色不限！例如：334455，66778899等！
BOOL CGameLogic::IsDoubleContinue(const BYTE iCardList[], int iCardCount)
{
	if (iCardCount < 6 || iCardCount % 2 != 0) return FALSE;

	BYTE cbCardData[MAX_COUNT];
	CopyMemory(cbCardData,iCardList,sizeof(BYTE)*iCardCount);

	//排列扑克
	SortCardList(cbCardData,iCardCount,ST_ORDER);

	for (int i = 0;i < iCardCount - 2;i++)
	{
		if (GetCardValue(cbCardData[i]) == 2 || cbCardData[i] == 0x4E || cbCardData[i] == 0x4F||GetCardValue(cbCardData[i]) == 3 )
			return FALSE;
		if ((GetCardLogicValue(cbCardData[i]) % 15) != ((GetCardLogicValue(cbCardData[i+2]) + 1) % 15))
			return FALSE;
	}
	return TRUE;
}
//是否可以查
bool CGameLogic::CompareChaCard(const BYTE cbFirstCard[], const BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount)
{
	if(cbNextCount != 2) return false;

	if((GetCardValue(cbFirstCard[0] )== GetCardValue(cbNextCard[0]))&&(GetCardValue(cbFirstCard[0]) == GetCardValue(cbNextCard[1])))
		return TRUE;
	return false;
}
//是否可以查
bool CGameLogic::CompareDianCard(const BYTE cbFirstCard[], const BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount)
{
	if(cbNextCount != 1) return false;

	if(GetCardValue(cbFirstCard[0]) == GetCardValue(cbNextCard[0]))
		return TRUE;
	return false;
}
