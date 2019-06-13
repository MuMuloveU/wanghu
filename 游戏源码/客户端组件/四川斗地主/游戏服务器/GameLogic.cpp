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

const BYTE	CGameLogic::m_cbGoodcardData[GOOD_CARD_COUTN]=
{
0x01,0x02,
0x11,0x12,
0x21,0x22,
0x31,0x32,
0x4E,0x4F,
0x07,0x08,0x09,
0x17,0x18,0x19,
0x27,0x28,0x29,
0x37,0x38,0x39,
0x0A,0x0B,0x0C,0x0D,
0x1A,0x1B,0x1C,0x1D,
0x2A,0x2B,0x2C,0x2D,
0x3A,0x3B,0x3C,0x3D
};

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameLogic::CGameLogic()
{
	//AI变量
	m_lBankerOutCardCount = 0 ;
	m_bCurrentState=0;
	ZeroMemory(m_bCallScore,sizeof(m_bCallScore));
}

//析构函数
CGameLogic::~CGameLogic()
{
#ifdef _CHECK_DUG
	file.Close() ;
#endif
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
	case 2:	//对牌火箭
		{
			//牌型判断
			if ((cbCardData[0]==0x4F)&&(cbCardData[1]==0x4E)) return CT_MISSILE_CARD;
			if (GetCardLogicValue(cbCardData[0])==GetCardLogicValue(cbCardData[1])) return CT_DOUBLE;

			return CT_ERROR;
		}
	}

	//分析扑克
	tagAnalyseResult AnalyseResult;
	if(!AnalysebCardData(cbCardData,cbCardCount,AnalyseResult)) 
		return CT_ERROR ;

	//四牌判断
	if (AnalyseResult.cbFourCount>0)
	{
		//牌型判断
		if ((AnalyseResult.cbFourCount==1)&&(cbCardCount==4)) return CT_BOMB_CARD;
		//	if ((AnalyseResult.cbFourCount==1)&&(AnalyseResult.cbSignedCount==2)&&(cbCardCount==6)) return CT_FOUR_LINE_TAKE_ONE;
		if ((AnalyseResult.cbFourCount==1)&&(AnalyseResult.cbSignedCount==2)&&(cbCardCount==6)) return CT_FOUR_LINE_TAKE_ONE;
		if ((AnalyseResult.cbFourCount==1)&&(AnalyseResult.cbDoubleCount==2)&&(cbCardCount==8)) return CT_FOUR_LINE_TAKE_TWO;

		return CT_ERROR;
	}

	//三牌判断
	if (AnalyseResult.cbThreeCount>0)
	{
		//三条类型
		if(AnalyseResult.cbThreeCount==1 && cbCardCount==3) return CT_THREE ;

		//连牌判断
		if (AnalyseResult.cbThreeCount>1)
		{
			//变量定义
			BYTE cbCardData=AnalyseResult.cbThreeCardData[0];
			BYTE cbFirstLogicValue=GetCardLogicValue(cbCardData);

			//错误过虑
			if (cbFirstLogicValue>=15) return CT_ERROR;

			//连牌判断
			for (BYTE i=1;i<AnalyseResult.cbThreeCount;i++)
			{
				BYTE cbCardData=AnalyseResult.cbThreeCardData[i*3];
				if (cbFirstLogicValue!=(GetCardLogicValue(cbCardData)+i)) return CT_ERROR;
			}
		}

		//牌形判断
		if (AnalyseResult.cbThreeCount*3==cbCardCount) return CT_THREE_LINE;
		if (AnalyseResult.cbThreeCount*4==cbCardCount) return CT_THREE_LINE_TAKE_ONE;

		return CT_ERROR;
	}

	//两张类型
	if (AnalyseResult.cbDoubleCount>=3)
	{
		//变量定义
		BYTE cbCardData=AnalyseResult.cbDoubleCardData[0];
		BYTE cbFirstLogicValue=GetCardLogicValue(cbCardData);

		//错误过虑
		if (cbFirstLogicValue>=15) return CT_ERROR;

		//连牌判断
		for (BYTE i=1;i<AnalyseResult.cbDoubleCount;i++)
		{
			BYTE cbCardData=AnalyseResult.cbDoubleCardData[i*2];
			if (cbFirstLogicValue!=(GetCardLogicValue(cbCardData)+i)) return CT_ERROR;
		}

		//二连判断
		if ((AnalyseResult.cbDoubleCount*2)==cbCardCount) return CT_DOUBLE_LINE;

		return CT_ERROR;
	}

	//单张判断
	if ((AnalyseResult.cbSignedCount>=5)&&(AnalyseResult.cbSignedCount==cbCardCount))
	{
		//变量定义
		BYTE cbCardData=AnalyseResult.cbSignedCardData[0];
		BYTE cbFirstLogicValue=GetCardLogicValue(cbCardData);

		//错误过虑
		if (cbFirstLogicValue>=15) return CT_ERROR;

		//连牌判断
		for (BYTE i=1;i<AnalyseResult.cbSignedCount;i++)
		{
			BYTE cbCardData=AnalyseResult.cbSignedCardData[i];
			if (cbFirstLogicValue!=(GetCardLogicValue(cbCardData)+i)) return CT_ERROR;
		}

		return CT_SINGLE_LINE;
	}

	return CT_ERROR;
}

//排列扑克
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
//得到好牌
void CGameLogic::GetGoodCardData(BYTE cbGoodCardData[NORMAL_COUNT])
{
	//混乱准备
	BYTE cbCardData[CountArray(m_cbGoodcardData)];
	BYTE cbCardBuffer[CountArray(m_cbGoodcardData)];
	CopyMemory(cbCardData,m_cbGoodcardData,sizeof(m_cbGoodcardData));

	//混乱扑克
	BYTE cbRandCount=0,cbPosition=0;
	BYTE cbBufferCount=CountArray(m_cbGoodcardData);
	do
	{
		cbPosition=rand()%(cbBufferCount-cbRandCount);
		cbCardBuffer[cbRandCount++]=cbCardData[cbPosition];
		cbCardData[cbPosition]=cbCardData[cbBufferCount-cbRandCount];
	} while (cbRandCount<cbBufferCount);

	//复制好牌
	CopyMemory(cbGoodCardData, cbCardBuffer, NORMAL_COUNT) ;
}

//删除好牌
bool CGameLogic::RemoveGoodCardData(BYTE cbGoodcardData[NORMAL_COUNT], BYTE cbGoodCardCount, BYTE cbCardData[FULL_COUNT], BYTE cbCardCount) 
{
	//检验数据
	ASSERT(cbGoodCardCount<=cbCardCount);
	if(cbGoodCardCount>cbCardCount)
		return false ;

	//定义变量
	BYTE cbDeleteCount=0,cbTempCardData[FULL_COUNT];
	if (cbCardCount>CountArray(cbTempCardData)) return false;
	CopyMemory(cbTempCardData,cbCardData,cbCardCount*sizeof(cbCardData[0]));

	//置零扑克
	for (BYTE i=0;i<cbGoodCardCount;i++)
	{
		for (BYTE j=0;j<cbCardCount;j++)
		{
			if (cbGoodcardData[i]==cbTempCardData[j])
			{
				cbDeleteCount++;
				cbTempCardData[j]=0;
				break;
			}
		}
	}
	ASSERT(cbDeleteCount==cbGoodCardCount) ;
	if (cbDeleteCount!=cbGoodCardCount) return false;

	//清理扑克
	BYTE cbCardPos=0;
	for (BYTE i=0;i<cbCardCount;i++)
	{
		if (cbTempCardData[i]!=0) cbCardData[cbCardPos++]=cbTempCardData[i];
	}

	return true;
}

//混乱扑克
void CGameLogic::RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount)
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
	if(cbRemoveCount>cbCardCount)
		return false ;

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

//有效判断
bool CGameLogic::IsValidCard(BYTE cbCardData)
{
	//获取属性
	BYTE cbCardColor=GetCardColor(cbCardData);
	BYTE cbCardValue=GetCardValue(cbCardData);

	//有效判断
	if ((cbCardData==0x4E)||(cbCardData==0x4F)) return true;
	if ((cbCardColor<=0x30)&&(cbCardValue>=0x01)&&(cbCardValue<=0x0D)) return true;

	return false;
}

//逻辑数值
BYTE CGameLogic::GetCardLogicValue(BYTE cbCardData)
{
	//扑克属性
	BYTE cbCardColor=GetCardColor(cbCardData);
	BYTE cbCardValue=GetCardValue(cbCardData);

#ifdef _DEBUG
	if(cbCardValue<=0 || cbCardValue>(MASK_VALUE&0x4f))
		return 0 ;
#endif

	ASSERT(cbCardValue>0 && cbCardValue<=(MASK_VALUE&0x4f)) ;


	//转换数值
	if (cbCardColor==0x40) return cbCardValue+2;
	return (cbCardValue<=2)?(cbCardValue+13):cbCardValue;
}

//对比扑克
bool CGameLogic::CompareCard(const BYTE cbFirstCard[], const BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount)
{
	//获取类型
	BYTE cbNextType=GetCardType(cbNextCard,cbNextCount);
	BYTE cbFirstType=GetCardType(cbFirstCard,cbFirstCount);

	//类型判断
	if (cbNextType==CT_ERROR) return false;
	if (cbNextType==CT_MISSILE_CARD) return true;
	if (cbFirstType==CT_MISSILE_CARD) return false ;

	//炸弹判断
	if ((cbFirstType!=CT_BOMB_CARD)&&(cbNextType==CT_BOMB_CARD)) return true;
	if ((cbFirstType==CT_BOMB_CARD)&&(cbNextType!=CT_BOMB_CARD)) return false;

	//规则判断
	if ((cbFirstType!=cbNextType)||(cbFirstCount!=cbNextCount)) return false;

	//开始对比
	switch (cbNextType)
	{
	case CT_SINGLE:
	case CT_DOUBLE:
	case CT_THREE:
	case CT_SINGLE_LINE:
	case CT_DOUBLE_LINE:
	case CT_THREE_LINE:
	case CT_BOMB_CARD:
		{
			//获取数值
			BYTE cbNextLogicValue=GetCardLogicValue(cbNextCard[0]);
			BYTE cbFirstLogicValue=GetCardLogicValue(cbFirstCard[0]);

			//对比扑克
			return cbNextLogicValue>cbFirstLogicValue;
		}
	case CT_THREE_LINE_TAKE_ONE:
		{
			//分析扑克
			tagAnalyseResult NextResult;
			tagAnalyseResult FirstResult;
			AnalysebCardData(cbNextCard,cbNextCount,NextResult);
			AnalysebCardData(cbFirstCard,cbFirstCount,FirstResult);

			//获取数值
			BYTE cbNextLogicValue=GetCardLogicValue(NextResult.cbThreeCardData[0]);
			BYTE cbFirstLogicValue=GetCardLogicValue(FirstResult.cbThreeCardData[0]);

			//对比扑克
			return cbNextLogicValue>cbFirstLogicValue;
		}
	case CT_FOUR_LINE_TAKE_ONE:
	case CT_FOUR_LINE_TAKE_TWO:
		{
			//分析扑克
			tagAnalyseResult NextResult;
			tagAnalyseResult FirstResult;
			AnalysebCardData(cbNextCard,cbNextCount,NextResult);
			AnalysebCardData(cbFirstCard,cbFirstCount,FirstResult);

			//获取数值
			BYTE cbNextLogicValue=GetCardLogicValue(NextResult.cbFourCardData[0]);
			BYTE cbFirstLogicValue=GetCardLogicValue(FirstResult.cbFourCardData[0]);

			//对比扑克
			return cbNextLogicValue>cbFirstLogicValue;
		}
	}

	return false;
}

//分析扑克
bool CGameLogic::AnalysebCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResult & AnalyseResult)
{
	//设置结果
	ZeroMemory(&AnalyseResult,sizeof(AnalyseResult));

	//扑克分析
	for (BYTE i=0;i<cbCardCount;i++)
	{
		//变量定义
		BYTE cbSameCount=1;
		BYTE cbLogicValue=GetCardLogicValue(cbCardData[i]);
		if(cbLogicValue<=0) 
			return false;

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

	return true;
}
//随机扑克
BYTE CGameLogic::GetRandomCard(void)
{
	size_t cbIndex = rand()%(sizeof(m_cbCardData)) ;
	return m_cbCardData[cbIndex] ;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////以下为AI函数

//出牌搜索
bool CGameLogic::SearchOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, WORD wOutCardUser, WORD wMeChairID, tagOutCardResult & OutCardResult)
{
	//玩家判断
	WORD wUndersideOfBanker = (m_wBankerUser+1)%GAME_PLAYER ;	//地主下家
	WORD wUpsideOfBanker = (wUndersideOfBanker+1)%GAME_PLAYER ;	//地主上家

	//初始变量
	ZeroMemory(&OutCardResult, sizeof(OutCardResult)) ;

	//先出牌
	if(cbTurnCardCount==0)
	{
		//地主出牌
		if(wMeChairID==m_wBankerUser) BankerOutCard(cbHandCardData, cbHandCardCount, OutCardResult) ;
		//地主下家
		else if(wMeChairID==wUndersideOfBanker) UndersideOfBankerOutCard(cbHandCardData, cbHandCardCount,wMeChairID,  OutCardResult) ;
		//地主上家
		else if(wMeChairID==wUpsideOfBanker) UpsideOfBankerOutCard(cbHandCardData, cbHandCardCount, wMeChairID, OutCardResult) ;
		//错误 ID
		else ASSERT(false) ;
	}
	//压牌
	else
	{	
		//地主出牌
		if(wMeChairID==m_wBankerUser) BankerOutCard(cbHandCardData, cbHandCardCount, wOutCardUser, cbTurnCardData, cbTurnCardCount, OutCardResult) ;
		//地主下家
		else if(wMeChairID==wUndersideOfBanker) UndersideOfBankerOutCard(cbHandCardData, cbHandCardCount, wOutCardUser, cbTurnCardData, cbTurnCardCount, OutCardResult) ;
		//地主上家
		else if(wMeChairID==wUpsideOfBanker) UpsideOfBankerOutCard(cbHandCardData, cbHandCardCount, wOutCardUser, cbTurnCardData, cbTurnCardCount, OutCardResult) ;
		//错误 ID
		else ASSERT(false) ;

		//反春天判断
		if (GetCardType(m_cbAllCardData[wOutCardUser], m_cbUserCardCount[wOutCardUser]) != CT_ERROR && m_lBankerOutCardCount == 1 && (OutCardResult.cbCardCount <= 0 || 
			!CompareCard(cbTurnCardData, OutCardResult.cbResultCard , cbTurnCardCount , OutCardResult.cbCardCount)))
		{
			//零下标没用
			tagOutCardTypeResult CardTypeResult[12+1] ;
			ZeroMemory(CardTypeResult, sizeof(CardTypeResult)) ;

			//出牌类型
			AnalyseOutCardType(cbHandCardData,cbHandCardCount,cbTurnCardData,cbTurnCardCount, CardTypeResult) ;

			BYTE cbOutCardType = GetCardType(cbTurnCardData, cbTurnCardCount) ;

			if (CardTypeResult[cbOutCardType].cbCardTypeCount > 0 && cbOutCardType != CT_THREE)
			{
				BYTE cbIndex = CardTypeResult[cbOutCardType].cbCardTypeCount - 1 ;
				BYTE cbCardCount = CardTypeResult[cbOutCardType].cbEachHandCardCount[cbIndex] ;

				CopyMemory(OutCardResult.cbResultCard, CardTypeResult[cbOutCardType].cbCardData[cbIndex], cbCardCount) ;
				OutCardResult.cbCardCount = cbCardCount ;
			}
			//出炸弹
			else if (CardTypeResult[CT_BOMB_CARD].cbCardTypeCount > 0)
			{
				BYTE cbIndex = CardTypeResult[CT_BOMB_CARD].cbCardTypeCount - 1 ;
				BYTE cbCardCount = CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[cbIndex] ;
				CopyMemory(OutCardResult.cbResultCard, CardTypeResult[CT_BOMB_CARD].cbCardData[cbIndex], cbCardCount) ;
				OutCardResult.cbCardCount = cbCardCount ;
			}
			else if (CardTypeResult[CT_MISSILE_CARD].cbCardTypeCount > 0)
			{
				OutCardResult.cbCardCount = 2 ;
				OutCardResult.cbResultCard[0] = 0x4F ;
				OutCardResult.cbResultCard[1] = 0x4E ;
			}
		}

		//春天判断
		if (GetCardType(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser]) != CT_ERROR && (OutCardResult.cbCardCount <= 0 || 
			!CompareCard(cbTurnCardData, OutCardResult.cbResultCard , cbTurnCardCount , OutCardResult.cbCardCount)) &&
			m_cbUserCardCount[(1 + m_wBankerUser) % GAME_PLAYER] == NORMAL_COUNT && m_cbUserCardCount[(2 + m_wBankerUser) % GAME_PLAYER] == NORMAL_COUNT)
		{
			//零下标没用
			tagOutCardTypeResult CardTypeResult[12+1] ;
			ZeroMemory(CardTypeResult, sizeof(CardTypeResult)) ;

			//出牌类型
			AnalyseOutCardType(cbHandCardData,cbHandCardCount,cbTurnCardData,cbTurnCardCount, CardTypeResult) ;

			BYTE cbOutCardType = GetCardType(cbTurnCardData, cbTurnCardCount) ;

			if (CardTypeResult[cbOutCardType].cbCardTypeCount > 0 && cbOutCardType != CT_THREE)
			{
				BYTE cbIndex = CardTypeResult[cbOutCardType].cbCardTypeCount - 1 ;
				BYTE cbCardCount = CardTypeResult[cbOutCardType].cbEachHandCardCount[cbIndex] ;

				CopyMemory(OutCardResult.cbResultCard, CardTypeResult[cbOutCardType].cbCardData[cbIndex], cbCardCount) ;
				OutCardResult.cbCardCount = cbCardCount ;
			}
			//出炸弹
			else if (CardTypeResult[CT_BOMB_CARD].cbCardTypeCount > 0)
			{
				BYTE cbIndex = CardTypeResult[CT_BOMB_CARD].cbCardTypeCount - 1 ;
				BYTE cbCardCount = CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[cbIndex] ;
				CopyMemory(OutCardResult.cbResultCard, CardTypeResult[CT_BOMB_CARD].cbCardData[cbIndex], cbCardCount) ;
				OutCardResult.cbCardCount = cbCardCount ;
			}
			else if (CardTypeResult[CT_MISSILE_CARD].cbCardTypeCount > 0)
			{
				OutCardResult.cbCardCount = 2 ;
				OutCardResult.cbResultCard[0] = 0x4F ;
				OutCardResult.cbResultCard[1] = 0x4E ;
			}
		}

	}

	//出牌次数
	if (m_wBankerUser == wMeChairID && OutCardResult.cbCardCount > 0) ++m_lBankerOutCardCount ;

	return true ;
}

//分析炸弹
void CGameLogic::GetAllBomCard(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbBomCardData[], BYTE &cbBomCardCount)
{
	BYTE cbTmpCardData[MAX_COUNT] ;
	CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount) ;

	//大小排序
	SortCardList(cbTmpCardData, cbHandCardCount, ST_ORDER);

	cbBomCardCount = 0 ;

	if(cbHandCardCount<2) return ;

	//双王炸弹
	if(0x4F==cbTmpCardData[0] && 0x4E==cbTmpCardData[1])
	{
		cbBomCardData[cbBomCardCount++] = cbTmpCardData[0] ;
		cbBomCardData[cbBomCardCount++] = cbTmpCardData[1] ;
	}

	//扑克分析
	for (BYTE i=0;i<cbHandCardCount;i++)
	{
		//变量定义
		BYTE cbSameCount=1;
		BYTE cbLogicValue=GetCardLogicValue(cbTmpCardData[i]);

		//搜索同牌
		for (BYTE j=i+1;j<cbHandCardCount;j++)
		{
			//获取扑克
			if (GetCardLogicValue(cbTmpCardData[j])!=cbLogicValue) break;

			//设置变量
			cbSameCount++;
		}

		if(4==cbSameCount)
		{
			cbBomCardData[cbBomCardCount++] = cbTmpCardData[i] ;
			cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+1] ;
			cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+2] ;
			cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+3] ;
		}

		//设置索引
		i+=cbSameCount-1;
	}
}

//分析顺子
void CGameLogic::GetAllLineCard(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbLineCardData[], BYTE &cbLineCardCount)
{
	BYTE cbTmpCard[MAX_COUNT] ;
	CopyMemory(cbTmpCard, cbHandCardData, cbHandCardCount) ;
	//大小排序
	SortCardList(cbTmpCard, cbHandCardCount, ST_ORDER) ;

	cbLineCardCount = 0 ;

	//数据校验
	if(cbHandCardCount<5) return ;

	BYTE cbFirstCard = 0 ;
	//去除2和王
	for(BYTE i=0 ; i<cbHandCardCount ; ++i)	if(GetCardLogicValue(cbTmpCard[i])<15)	{cbFirstCard = i ; break ;}

	BYTE cbSingleLineCard[12] ;
	BYTE cbSingleLineCount=0 ;
	BYTE cbLeftCardCount = cbHandCardCount ;
	bool bFindSingleLine = true ;

	//连牌判断
	while (cbLeftCardCount>=5 && bFindSingleLine)
	{
		cbSingleLineCount=1 ;
		bFindSingleLine = false ;
		BYTE cbLastCard = cbTmpCard[cbFirstCard] ;
		cbSingleLineCard[cbSingleLineCount-1] = cbTmpCard[cbFirstCard] ;
		for (BYTE i=cbFirstCard+1; i<cbLeftCardCount; i++)
		{
			BYTE cbCardData=cbTmpCard[i];

			//连续判断
			if (1!=(GetCardLogicValue(cbLastCard)-GetCardLogicValue(cbCardData)) && GetCardValue(cbLastCard)!=GetCardValue(cbCardData)) 
			{
				cbLastCard = cbTmpCard[i] ;
				if(cbSingleLineCount<5) 
				{
					cbSingleLineCount = 1 ;
					cbSingleLineCard[cbSingleLineCount-1] = cbTmpCard[i] ;
					continue ;
				}
				else break ;
			}
			//同牌判断
			else if(GetCardValue(cbLastCard)!=GetCardValue(cbCardData))
			{
				cbLastCard = cbCardData ;
				cbSingleLineCard[cbSingleLineCount] = cbCardData ;
				++cbSingleLineCount ;
			}					
		}

		//保存数据
		if(cbSingleLineCount>=5)
		{
			RemoveCard(cbSingleLineCard, cbSingleLineCount, cbTmpCard, cbLeftCardCount) ;
			memcpy(cbLineCardData+cbLineCardCount , cbSingleLineCard, sizeof(BYTE)*cbSingleLineCount) ;
			cbLineCardCount += cbSingleLineCount ;
			cbLeftCardCount -= cbSingleLineCount ;
			bFindSingleLine = true ;
		}
	}
}

//分析三条
void CGameLogic::GetAllThreeCard(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbThreeCardData[], BYTE &cbThreeCardCount)
{
	BYTE cbTmpCardData[MAX_COUNT] ;
	CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount) ;

	//大小排序
	SortCardList(cbTmpCardData, cbHandCardCount, ST_ORDER);

	cbThreeCardCount = 0 ;

	//扑克分析
	for (BYTE i=0;i<cbHandCardCount;i++)
	{
		//变量定义
		BYTE cbSameCount=1;
		BYTE cbLogicValue=GetCardLogicValue(cbTmpCardData[i]);

		//搜索同牌
		for (BYTE j=i+1;j<cbHandCardCount;j++)
		{
			//获取扑克
			if (GetCardLogicValue(cbTmpCardData[j])!=cbLogicValue) break;

			//设置变量
			cbSameCount++;
		}

		if(cbSameCount>=3)
		{
			cbThreeCardData[cbThreeCardCount++] = cbTmpCardData[i] ;
			cbThreeCardData[cbThreeCardCount++] = cbTmpCardData[i+1] ;
			cbThreeCardData[cbThreeCardCount++] = cbTmpCardData[i+2] ;	
		}

		//设置索引
		i+=cbSameCount-1;
	}
}

//分析对子
void CGameLogic::GetAllDoubleCard(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbDoubleCardData[], BYTE &cbDoubleCardCount)
{
	BYTE cbTmpCardData[MAX_COUNT] ;
	CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount) ;

	//大小排序
	SortCardList(cbTmpCardData, cbHandCardCount, ST_ORDER);

	cbDoubleCardCount = 0 ;

	//扑克分析
	for (BYTE i=0;i<cbHandCardCount;i++)
	{
		//变量定义
		BYTE cbSameCount=1;
		BYTE cbLogicValue=GetCardLogicValue(cbTmpCardData[i]);

		//搜索同牌
		for (BYTE j=i+1;j<cbHandCardCount;j++)
		{
			//获取扑克
			if (GetCardLogicValue(cbTmpCardData[j])!=cbLogicValue) break;

			//设置变量
			cbSameCount++;
		}

		if(cbSameCount>=2)
		{
			cbDoubleCardData[cbDoubleCardCount++] = cbTmpCardData[i] ;
			cbDoubleCardData[cbDoubleCardCount++] = cbTmpCardData[i+1] ;
		}

		//设置索引
		i+=cbSameCount-1;
	}
}

//分析单牌
void CGameLogic::GetAllSingleCard(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbSingleCardData[], BYTE &cbSingleCardCount)
{
	cbSingleCardCount =0 ;

	BYTE cbTmpCardData[MAX_COUNT] ;
	CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount) ;

	//大小排序
	SortCardList(cbTmpCardData, cbHandCardCount, ST_ORDER);

	//扑克分析
	for (BYTE i=0;i<cbHandCardCount;i++)
	{
		//变量定义
		BYTE cbSameCount=1;
		BYTE cbLogicValue=GetCardLogicValue(cbTmpCardData[i]);

		//搜索同牌
		for (BYTE j=i+1;j<cbHandCardCount;j++)
		{
			//获取扑克
			if (GetCardLogicValue(cbTmpCardData[j])!=cbLogicValue) break;

			//设置变量
			cbSameCount++;
		}

		if(cbSameCount==1)
		{
			cbSingleCardData[cbSingleCardCount++] = cbTmpCardData[i] ;
		}

		//设置索引
		i+=cbSameCount-1;
	}
}

//分析出牌
void CGameLogic::AnalyseOutCardType(BYTE const cbHandCardData[], BYTE const cbHandCardCount, tagOutCardTypeResult CardTypeResult[12+1])
{
	ZeroMemory(CardTypeResult, sizeof(CardTypeResult[0])*12) ;
	BYTE cbTmpCardData[MAX_COUNT] ;
	//保留扑克，防止分析时改变扑克
	BYTE cbReserveCardData[MAX_COUNT] ;
	CopyMemory(cbReserveCardData, cbHandCardData, cbHandCardCount) ;	
	SortCardList(cbReserveCardData, cbHandCardCount, ST_ORDER) ;
	CopyMemory(cbTmpCardData, cbReserveCardData, cbHandCardCount) ;

	//单牌类型
	for(BYTE i=0; i<cbHandCardCount; ++i)
	{
		BYTE Index = CardTypeResult[CT_SINGLE].cbCardTypeCount ;
		CardTypeResult[CT_SINGLE].cbCardType = CT_SINGLE ;
		CardTypeResult[CT_SINGLE].cbCardData[Index][0] = cbTmpCardData[i] ;
		CardTypeResult[CT_SINGLE].cbEachHandCardCount[Index] = 1 ;
		CardTypeResult[CT_SINGLE].cbCardTypeCount++ ;	

		ASSERT(CardTypeResult[CT_SINGLE].cbCardTypeCount<MAX_TYPE_COUNT) ;
	}

	//对牌类型
	{
		BYTE cbDoubleCardData[MAX_COUNT] ;
		BYTE cbDoubleCardcount=0; 
		GetAllDoubleCard(cbTmpCardData, cbHandCardCount, cbDoubleCardData, cbDoubleCardcount) ;
		for(BYTE i=0; i<cbDoubleCardcount; i+=2)
		{
			BYTE Index = CardTypeResult[CT_DOUBLE].cbCardTypeCount ;
			CardTypeResult[CT_DOUBLE].cbCardType = CT_DOUBLE ;
			CardTypeResult[CT_DOUBLE].cbCardData[Index][0] = cbDoubleCardData[i] ;
			CardTypeResult[CT_DOUBLE].cbCardData[Index][1] = cbDoubleCardData[i+1] ;
			CardTypeResult[CT_DOUBLE].cbEachHandCardCount[Index] = 2 ;
			CardTypeResult[CT_DOUBLE].cbCardTypeCount++ ;	

			ASSERT(CardTypeResult[CT_DOUBLE].cbCardTypeCount<MAX_TYPE_COUNT) ;
		}
	}

	//三条类型
	{
		BYTE cbThreeCardData[MAX_COUNT];
		BYTE cbThreeCardCount=0 ;
		GetAllThreeCard(cbTmpCardData, cbHandCardCount, cbThreeCardData, cbThreeCardCount) ;
		for(BYTE i=0; i<cbThreeCardCount; i+=3)
		{
			BYTE Index = CardTypeResult[CT_THREE].cbCardTypeCount ;
			CardTypeResult[CT_THREE].cbCardType = CT_THREE ;
			CardTypeResult[CT_THREE].cbCardData[Index][0] = cbThreeCardData[i] ;
			CardTypeResult[CT_THREE].cbCardData[Index][1] = cbThreeCardData[i+1] ;
			CardTypeResult[CT_THREE].cbCardData[Index][2] = cbThreeCardData[i+2] ;
			CardTypeResult[CT_THREE].cbEachHandCardCount[Index] = 3 ;
			CardTypeResult[CT_THREE].cbCardTypeCount++ ;	

			ASSERT(CardTypeResult[CT_THREE].cbCardTypeCount<MAX_TYPE_COUNT) ;
		}
	}

	//炸弹类型
	{
		BYTE cbFourCardData[MAX_COUNT];
		BYTE cbFourCardCount=0 ;
		if(cbHandCardCount>=2 && 0x4F==cbTmpCardData[0] && 0x4E==cbTmpCardData[1])
		{
			BYTE Index = CardTypeResult[CT_BOMB_CARD].cbCardTypeCount ;
			CardTypeResult[CT_BOMB_CARD].cbCardType = CT_BOMB_CARD ;
			CardTypeResult[CT_BOMB_CARD].cbCardData[Index][0] = cbTmpCardData[0] ;
			CardTypeResult[CT_BOMB_CARD].cbCardData[Index][1] = cbTmpCardData[1] ;
			CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[Index] = 2 ;
			CardTypeResult[CT_BOMB_CARD].cbCardTypeCount++ ;	
			GetAllBomCard(cbTmpCardData+2, cbHandCardCount-2, cbFourCardData, cbFourCardCount) ;
		}
		else GetAllBomCard(cbTmpCardData, cbHandCardCount, cbFourCardData, cbFourCardCount) ;
		for (BYTE i=0; i<cbFourCardCount; i+=4)
		{
			BYTE Index = CardTypeResult[CT_BOMB_CARD].cbCardTypeCount ;
			CardTypeResult[CT_BOMB_CARD].cbCardType = CT_BOMB_CARD ;
			CardTypeResult[CT_BOMB_CARD].cbCardData[Index][0] = cbFourCardData[i] ;
			CardTypeResult[CT_BOMB_CARD].cbCardData[Index][1] = cbFourCardData[i+1] ;
			CardTypeResult[CT_BOMB_CARD].cbCardData[Index][2] = cbFourCardData[i+2] ;
			CardTypeResult[CT_BOMB_CARD].cbCardData[Index][3] = cbFourCardData[i+3] ;
			CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[Index] = 4 ;
			CardTypeResult[CT_BOMB_CARD].cbCardTypeCount++ ;	

			ASSERT(CardTypeResult[CT_BOMB_CARD].cbCardTypeCount<MAX_TYPE_COUNT) ;
		}
	}
	//单连类型
	{
		//恢复扑克，防止分析时改变扑克
		CopyMemory(cbTmpCardData, cbReserveCardData, cbHandCardCount) ;

		BYTE cbFirstCard = 0 ;
		//去除2和王
		for(BYTE i=0 ; i<cbHandCardCount ; ++i)
		{
			if(GetCardLogicValue(cbTmpCardData[i])<15)
			{
				cbFirstCard = i ;
				break ;
			}
		}

		BYTE cbSingleLineCard[12] ;
		BYTE cbSingleLineCount=1 ;
		BYTE cbLeftCardCount = cbHandCardCount ;
		bool bFindSingleLine = true ;

		//连牌判断
		while (cbLeftCardCount>=5 && bFindSingleLine)
		{
			cbSingleLineCount=1 ;
			bFindSingleLine = false ;
			BYTE cbLastCard = cbTmpCardData[cbFirstCard] ;
			cbSingleLineCard[cbSingleLineCount-1] = cbTmpCardData[cbFirstCard] ;
			for (BYTE i=cbFirstCard+1; i<cbLeftCardCount; i++)
			{
				BYTE cbCardData=cbTmpCardData[i];

				//连续判断
				if (1!=(GetCardLogicValue(cbLastCard)-GetCardLogicValue(cbCardData)) && GetCardValue(cbLastCard)!=GetCardValue(cbCardData)) 
				{
					cbLastCard = cbTmpCardData[i] ;
					//是否合法
					if(cbSingleLineCount<5) 
					{
						cbSingleLineCount = 1 ;
						cbSingleLineCard[cbSingleLineCount-1] = cbTmpCardData[i] ;
						continue ;
					}
					else break ;
				}
				//同牌判断
				else if(GetCardValue(cbLastCard)!=GetCardValue(cbCardData))
				{
					cbLastCard = cbCardData ;
					cbSingleLineCard[cbSingleLineCount] = cbCardData ;
					++cbSingleLineCount ;
				}
			}

			//保存数据
			if(cbSingleLineCount>=5)
			{
				BYTE Index ;
				//所有连牌

				int nStart = 0 ;
				
				//从大到小
				nStart = cbSingleLineCount - 5 ;
				while ( 0 < nStart )
				{
					Index = CardTypeResult[CT_SINGLE_LINE].cbCardTypeCount ;
					BYTE cbThisLineCount = cbSingleLineCount-nStart ;
					CardTypeResult[CT_SINGLE_LINE].cbCardType = CT_SINGLE_LINE ;
					CopyMemory(CardTypeResult[CT_SINGLE_LINE].cbCardData[Index], cbSingleLineCard, sizeof(BYTE)*(cbThisLineCount));
					CardTypeResult[CT_SINGLE_LINE].cbEachHandCardCount[Index] = cbThisLineCount;
					CardTypeResult[CT_SINGLE_LINE].cbCardTypeCount++ ;

					ASSERT(CardTypeResult[CT_SINGLE_LINE].cbCardTypeCount<MAX_TYPE_COUNT) ;
					nStart-- ;
				}

				//从小到大
				nStart = cbSingleLineCount - 5;
				while ( 0 <= nStart )
				{
					Index = CardTypeResult[CT_SINGLE_LINE].cbCardTypeCount ;
					BYTE cbThisLineCount = cbSingleLineCount-nStart ;
					CardTypeResult[CT_SINGLE_LINE].cbCardType = CT_SINGLE_LINE ;
					CopyMemory(CardTypeResult[CT_SINGLE_LINE].cbCardData[Index], cbSingleLineCard+nStart, sizeof(BYTE)*(cbThisLineCount));
					CardTypeResult[CT_SINGLE_LINE].cbEachHandCardCount[Index] = cbThisLineCount;
					CardTypeResult[CT_SINGLE_LINE].cbCardTypeCount++ ;

					ASSERT(CardTypeResult[CT_SINGLE_LINE].cbCardTypeCount<MAX_TYPE_COUNT) ;
					nStart-- ;
				}

				RemoveCard(cbSingleLineCard, cbSingleLineCount, cbTmpCardData, cbLeftCardCount) ;
				cbLeftCardCount -= cbSingleLineCount ;
				bFindSingleLine = true ;
			}
		}

	}

	//对连类型
	{
		//恢复扑克，防止分析时改变扑克
		CopyMemory(cbTmpCardData, cbReserveCardData, cbHandCardCount) ;

		//连牌判断
		BYTE cbFirstCard = 0 ;
		//去除2和王
		for(BYTE i=0 ; i<cbHandCardCount ; ++i)	if(GetCardLogicValue(cbTmpCardData[i])<15)	{cbFirstCard = i ; break ;}

		BYTE cbLeftCardCount = cbHandCardCount-cbFirstCard ;
		bool bFindDoubleLine = true ;
		BYTE cbDoubleLineCount = 0 ;
		BYTE cbDoubleLineCard[24] ;
		//开始判断
		while (cbLeftCardCount>=6 && bFindDoubleLine)
		{
			BYTE cbLastCard = cbTmpCardData[cbFirstCard] ;
			BYTE cbSameCount = 1 ;
			cbDoubleLineCount = 0 ;
			bFindDoubleLine=false ;
			for(BYTE i=cbFirstCard+1 ; i<cbLeftCardCount+cbFirstCard ; ++i)
			{
				//搜索同牌
				while (GetCardLogicValue(cbLastCard)==GetCardLogicValue(cbTmpCardData[i]) && i<cbLeftCardCount+cbFirstCard)
				{
					++cbSameCount;
					++i ;
				}

				BYTE cbLastDoubleCardValue ;
				if(cbDoubleLineCount>0) cbLastDoubleCardValue = GetCardLogicValue(cbDoubleLineCard[cbDoubleLineCount-1]) ;
				//重新开始
				if((cbSameCount<2 || (cbDoubleLineCount>0 && (cbLastDoubleCardValue-GetCardLogicValue(cbLastCard))!=1)) && i<=cbLeftCardCount+cbFirstCard)
				{
					if(cbDoubleLineCount>=6) break ;
					//回退
					if(cbSameCount>=2) i-=cbSameCount ;
					cbLastCard = cbTmpCardData[i] ;
					cbDoubleLineCount = 0 ;
				}
				//保存数据
				else if(cbSameCount>=2)
				{
					cbDoubleLineCard[cbDoubleLineCount] = cbTmpCardData[i-cbSameCount] ;
					cbDoubleLineCard[cbDoubleLineCount+1] = cbTmpCardData[i-cbSameCount+1] ;
					cbDoubleLineCount += 2 ;

					//结尾判断
					if(i==(cbLeftCardCount+cbFirstCard-2))
						if((GetCardLogicValue(cbLastCard)-GetCardLogicValue(cbTmpCardData[i]))==1 && (GetCardLogicValue(cbTmpCardData[i])==GetCardLogicValue(cbTmpCardData[i+1])))
						{
							cbDoubleLineCard[cbDoubleLineCount] = cbTmpCardData[i] ;
							cbDoubleLineCard[cbDoubleLineCount+1] = cbTmpCardData[i+1] ;
							cbDoubleLineCount += 2 ;
							break ;
						}

				}

				cbLastCard = cbTmpCardData[i] ;
				cbSameCount = 1 ;
			}

			//保存数据
			if(cbDoubleLineCount>=6)
			{
				BYTE Index ;

				//所有连牌
				BYTE cbCurrentDoubleLineCount = 6 ;
				while ( cbCurrentDoubleLineCount < cbDoubleLineCount )
				{
					Index = CardTypeResult[CT_DOUBLE_LINE].cbCardTypeCount ;
					CardTypeResult[CT_DOUBLE_LINE].cbCardType = CT_DOUBLE_LINE ;
					CopyMemory(CardTypeResult[CT_DOUBLE_LINE].cbCardData[Index], cbDoubleLineCard, sizeof(BYTE)*cbCurrentDoubleLineCount);
					CardTypeResult[CT_DOUBLE_LINE].cbEachHandCardCount[Index] = cbCurrentDoubleLineCount;
					CardTypeResult[CT_DOUBLE_LINE].cbCardTypeCount++ ;

					cbCurrentDoubleLineCount += 2 ;

					ASSERT(CardTypeResult[CT_DOUBLE_LINE].cbCardTypeCount<MAX_TYPE_COUNT) ;
				}

				//从小到大
				if ( cbDoubleLineCount >= 6 )
				{
					//所有连牌
					int cbLeftLen = cbDoubleLineCount - 6 ;
					while ( cbLeftLen >= 0 )
					{
						Index = CardTypeResult[CT_DOUBLE_LINE].cbCardTypeCount ;
						CardTypeResult[CT_DOUBLE_LINE].cbCardType = CT_DOUBLE_LINE ;
						CopyMemory(CardTypeResult[CT_DOUBLE_LINE].cbCardData[Index], cbDoubleLineCard + cbLeftLen, sizeof( BYTE ) * ( cbDoubleLineCount - cbLeftLen ));
						CardTypeResult[CT_DOUBLE_LINE].cbEachHandCardCount[Index] = cbDoubleLineCount - cbLeftLen;
						CardTypeResult[CT_DOUBLE_LINE].cbCardTypeCount++ ;

						cbLeftLen -= 2 ;

						ASSERT(CardTypeResult[CT_DOUBLE_LINE].cbCardTypeCount<MAX_TYPE_COUNT) ;
					}
				}

				RemoveCard(cbDoubleLineCard, cbDoubleLineCount, cbTmpCardData, cbFirstCard+cbLeftCardCount) ;				
				bFindDoubleLine=true ;
				cbLeftCardCount -= cbDoubleLineCount ;
			}
		}
	}

	//三连类型
	{
		//恢复扑克，防止分析时改变扑克
		CopyMemory(cbTmpCardData, cbReserveCardData, cbHandCardCount) ;

		//连牌判断
		BYTE cbFirstCard = 0 ;
		//去除2和王
		for(BYTE i=0 ; i<cbHandCardCount ; ++i)	if(GetCardLogicValue(cbTmpCardData[i])<15)	{cbFirstCard = i ; break ;}

		BYTE cbLeftCardCount = cbHandCardCount-cbFirstCard ;
		bool bFindThreeLine = true ;
		BYTE cbThreeLineCount = 0 ;
		BYTE cbThreeLineCard[20] ;
		//开始判断
		while (cbLeftCardCount>=6 && bFindThreeLine)
		{
			BYTE cbLastCard = cbTmpCardData[cbFirstCard] ;
			BYTE cbSameCount = 1 ;
			cbThreeLineCount = 0 ;
			bFindThreeLine = false ;
			for(BYTE i=cbFirstCard+1 ; i<cbLeftCardCount+cbFirstCard ; ++i)
			{
				//搜索同牌
				while (GetCardLogicValue(cbLastCard)==GetCardLogicValue(cbTmpCardData[i]) && i<cbLeftCardCount+cbFirstCard)
				{
					++cbSameCount;
					++i ;
				}

				BYTE cbLastThreeCardValue ;
				if(cbThreeLineCount>0) cbLastThreeCardValue = GetCardLogicValue(cbThreeLineCard[cbThreeLineCount-1]) ;

				//重新开始
				if((cbSameCount<3 || (cbThreeLineCount>0&&(cbLastThreeCardValue-GetCardLogicValue(cbLastCard))!=1)) && i<=cbLeftCardCount+cbFirstCard)
				{
					if(cbThreeLineCount>=6) break ;

					if(cbSameCount>=3) i-=cbSameCount ;
					cbLastCard = cbTmpCardData[i] ;
					cbThreeLineCount = 0 ;
				}
				//保存数据
				else if(cbSameCount>=3)
				{
					cbThreeLineCard[cbThreeLineCount] = cbTmpCardData[i-cbSameCount] ;
					cbThreeLineCard[cbThreeLineCount+1] = cbTmpCardData[i-cbSameCount+1] ;
					cbThreeLineCard[cbThreeLineCount+2] = cbTmpCardData[i-cbSameCount+2] ;
					cbThreeLineCount += 3 ;

					//结尾判断
					if(i==(cbLeftCardCount+cbFirstCard-3))
						if((GetCardLogicValue(cbLastCard)-GetCardLogicValue(cbTmpCardData[i]))==1 && (GetCardLogicValue(cbTmpCardData[i])==GetCardLogicValue(cbTmpCardData[i+1])) && (GetCardLogicValue(cbTmpCardData[i])==GetCardLogicValue(cbTmpCardData[i+2])))
						{
							cbThreeLineCard[cbThreeLineCount] = cbTmpCardData[i] ;
							cbThreeLineCard[cbThreeLineCount+1] = cbTmpCardData[i+1] ;
							cbThreeLineCard[cbThreeLineCount+2] = cbTmpCardData[i+2] ;
							cbThreeLineCount += 3 ;
							break ;
						}

				}

				cbLastCard = cbTmpCardData[i] ;
				cbSameCount = 1 ;
			}

			//保存数据
			if(cbThreeLineCount>=6)
			{
				BYTE Index ;

				Index = CardTypeResult[CT_THREE_LINE].cbCardTypeCount ;
				CardTypeResult[CT_THREE_LINE].cbCardType = CT_THREE_LINE ;
				CopyMemory(CardTypeResult[CT_THREE_LINE].cbCardData[Index], cbThreeLineCard, sizeof(BYTE)*cbThreeLineCount);
				CardTypeResult[CT_THREE_LINE].cbEachHandCardCount[Index] = cbThreeLineCount;
				CardTypeResult[CT_THREE_LINE].cbCardTypeCount++ ;

				ASSERT(CardTypeResult[CT_THREE_LINE].cbCardTypeCount<MAX_TYPE_COUNT) ;

				RemoveCard(cbThreeLineCard, cbThreeLineCount, cbTmpCardData, cbFirstCard+cbLeftCardCount) ;
				bFindThreeLine=true ;
				cbLeftCardCount -= cbThreeLineCount ;
			}
		}

	}
	//三带一单
	{
		//恢复扑克，防止分析时改变扑克
		CopyMemory(cbTmpCardData, cbReserveCardData, cbHandCardCount) ;

		BYTE cbHandThreeCard[MAX_COUNT];
		BYTE cbHandThreeCount=0 ;

		//移除炸弹
		BYTE cbAllBomCardData[MAX_COUNT] ;
		BYTE cbAllBomCardCount=0 ;
		GetAllBomCard(cbTmpCardData, cbHandCardCount, cbAllBomCardData, cbAllBomCardCount) ;
		RemoveCard(cbAllBomCardData, cbAllBomCardCount, cbTmpCardData, cbHandCardCount) ;

		GetAllThreeCard(cbTmpCardData, cbHandCardCount-cbAllBomCardCount, cbHandThreeCard, cbHandThreeCount) ;

		{
			BYTE Index ;
			//去掉三条
			BYTE cbRemainCardData[MAX_COUNT] ;
			CopyMemory(cbRemainCardData, cbTmpCardData, cbHandCardCount-cbAllBomCardCount) ;
			BYTE cbRemainCardCount=cbHandCardCount-cbAllBomCardCount-cbHandThreeCount ;
			RemoveCard(cbHandThreeCard, cbHandThreeCount, cbRemainCardData, cbHandCardCount-cbAllBomCardCount) ;
			//三条带一张
			for(BYTE i=0; i<cbHandThreeCount; i+=3)
			{
				//三条带一张
				for(BYTE j=0; j<cbRemainCardCount; ++j)
				{
					Index = CardTypeResult[CT_THREE_LINE_TAKE_ONE].cbCardTypeCount ;
					CardTypeResult[CT_THREE_LINE_TAKE_ONE].cbCardType = CT_THREE_LINE_TAKE_ONE ;
					CardTypeResult[CT_THREE_LINE_TAKE_ONE].cbCardData[Index][0] = cbHandThreeCard[i] ;
					CardTypeResult[CT_THREE_LINE_TAKE_ONE].cbCardData[Index][1] = cbHandThreeCard[i+1] ;
					CardTypeResult[CT_THREE_LINE_TAKE_ONE].cbCardData[Index][2] = cbHandThreeCard[i+2] ;
					CardTypeResult[CT_THREE_LINE_TAKE_ONE].cbCardData[Index][3] = cbRemainCardData[j] ;
					CardTypeResult[CT_THREE_LINE_TAKE_ONE].cbEachHandCardCount[Index] = 4 ;
					CardTypeResult[CT_THREE_LINE_TAKE_ONE].cbCardTypeCount++ ;
				}			
			}
		}

		//三连带单
		BYTE cbLeftThreeCardCount=cbHandThreeCount ;
		bool bFindThreeLine=true ;
		BYTE cbLastIndex=0 ;
		if(GetCardLogicValue(cbHandThreeCard[0])==15) cbLastIndex=3 ;
		while (cbLeftThreeCardCount>=6 && bFindThreeLine)
		{
			BYTE cbLastLogicCard=GetCardLogicValue(cbHandThreeCard[cbLastIndex]);
			BYTE cbThreeLineCard[MAX_COUNT];
			BYTE cbThreeLineCardCount=3;
			cbThreeLineCard[0]=cbHandThreeCard[cbLastIndex];
			cbThreeLineCard[1]=cbHandThreeCard[cbLastIndex+1];
			cbThreeLineCard[2]=cbHandThreeCard[cbLastIndex+2];

			bFindThreeLine = false ;
			for(BYTE j=3+cbLastIndex; j<cbLeftThreeCardCount; j+=3)
			{
				//连续判断
				if(1!=(cbLastLogicCard-(GetCardLogicValue(cbHandThreeCard[j]))))
				{
					cbLastIndex = j ;
					if(cbLeftThreeCardCount-j>=6) bFindThreeLine = true ;

					break;
				}

				cbLastLogicCard=GetCardLogicValue(cbHandThreeCard[j]);
				cbThreeLineCard[cbThreeLineCardCount]=cbHandThreeCard[j];
				cbThreeLineCard[cbThreeLineCardCount+1]=cbHandThreeCard[j+1];
				cbThreeLineCard[cbThreeLineCardCount+2]=cbHandThreeCard[j+2];
				cbThreeLineCardCount += 3;
			}
			if(cbThreeLineCardCount>3)
			{
				BYTE Index ;

				BYTE cbRemainCard[MAX_COUNT];
				BYTE cbRemainCardCount=cbHandCardCount-cbAllBomCardCount-cbHandThreeCount ;


				//移除三条（还应该移除炸弹王等）
				CopyMemory(cbRemainCard, cbTmpCardData, (cbHandCardCount-cbAllBomCardCount)*sizeof(BYTE));
				RemoveCard(cbHandThreeCard, cbHandThreeCount, cbRemainCard, cbHandCardCount-cbAllBomCardCount) ;

				for(BYTE start=0; start<cbThreeLineCardCount-3; start+=3)
				{
					//本顺数目
					BYTE cbThisTreeLineCardCount = cbThreeLineCardCount-start ;
					//单牌个数
					BYTE cbSingleCardCount=(cbThisTreeLineCardCount)/3;

					//单牌不够
					if(cbRemainCardCount<cbSingleCardCount) continue ;

					//单牌组合
					BYTE cbComCard[5];
					BYTE cbComResCard[254][5] ;
					BYTE cbComResLen=0 ;

					Combination(cbComCard, 0, cbComResCard, cbComResLen, cbRemainCard, cbSingleCardCount, cbRemainCardCount, cbSingleCardCount);
					for(BYTE i=0; i<cbComResLen; ++i)
					{
						Index = CardTypeResult[CT_THREE_LINE_TAKE_ONE].cbCardTypeCount ;
						CardTypeResult[CT_THREE_LINE_TAKE_ONE].cbCardType = CT_THREE_LINE_TAKE_ONE ;
						//保存三条
						CopyMemory(CardTypeResult[CT_THREE_LINE_TAKE_ONE].cbCardData[Index], cbThreeLineCard+start, sizeof(BYTE)*cbThisTreeLineCardCount);
						//保存单牌
						CopyMemory(CardTypeResult[CT_THREE_LINE_TAKE_ONE].cbCardData[Index]+cbThisTreeLineCardCount, cbComResCard[i], cbSingleCardCount) ;


						CardTypeResult[CT_THREE_LINE_TAKE_ONE].cbEachHandCardCount[Index] = cbThisTreeLineCardCount+cbSingleCardCount ;
						CardTypeResult[CT_THREE_LINE_TAKE_ONE].cbCardTypeCount++ ;

						ASSERT(CardTypeResult[CT_THREE_LINE_TAKE_ONE].cbCardTypeCount<MAX_TYPE_COUNT) ;
					}

				}

				//移除三连
				bFindThreeLine = true ;
				RemoveCard(cbThreeLineCard, cbThreeLineCardCount, cbHandThreeCard, cbLeftThreeCardCount) ;
				cbLeftThreeCardCount -= cbThreeLineCardCount ;
			}
		}
	}

	//四带两单
	/*
	{
	//恢复扑克，防止分析时改变扑克
	CopyMemory(cbTmpCardData, cbReserveCardData, cbHandCardCount) ;

	BYTE cbFirstCard = 0 ;
	//去除王牌
	for(BYTE i=0 ; i<cbHandCardCount ; ++i)	if(GetCardColor(cbTmpCardData[i])!=0x40)	{cbFirstCard = i ; break ;}

	BYTE cbHandAllFourCardData[MAX_COUNT] ;
	BYTE cbHandAllFourCardCount=0;
	//抽取四张
	GetAllBomCard(cbTmpCardData+cbFirstCard, cbHandCardCount-cbFirstCard, cbHandAllFourCardData, cbHandAllFourCardCount) ;

	//移除四条
	BYTE cbRemainCard[MAX_COUNT];
	BYTE cbRemainCardCount=cbHandCardCount-cbHandAllFourCardCount ;
	CopyMemory(cbRemainCard, cbTmpCardData, cbHandCardCount*sizeof(BYTE));
	RemoveCard(cbHandAllFourCardData, cbHandAllFourCardCount, cbRemainCard, cbHandCardCount) ;

	for(BYTE Start=0; Start<cbHandAllFourCardCount; Start += 4)
	{
	BYTE Index ;
	//单牌组合
	BYTE cbComCard[5];
	BYTE cbComResCard[254][5] ;
	BYTE cbComResLen=0 ;
	//单牌组合
	Combination(cbComCard, 0, cbComResCard, cbComResLen, cbRemainCard, 2, cbRemainCardCount, 2);
	for(BYTE i=0; i<cbComResLen; ++i)
	{
	//不能带对
	if(GetCardValue(cbComResCard[i][0])==GetCardValue(cbComResCard[i][1])) continue ;

	Index=CardTypeResult[CT_FOUR_LINE_TAKE_ONE].cbCardTypeCount ;
	CardTypeResult[CT_FOUR_LINE_TAKE_ONE].cbCardType = CT_FOUR_LINE_TAKE_ONE ;
	CopyMemory(CardTypeResult[CT_FOUR_LINE_TAKE_ONE].cbCardData[Index], cbHandAllFourCardData+Start, 4) ;
	CardTypeResult[CT_FOUR_LINE_TAKE_ONE].cbCardData[Index][4] = cbComResCard[i][0] ;
	CardTypeResult[CT_FOUR_LINE_TAKE_ONE].cbCardData[Index][4+1] = cbComResCard[i][1] ;
	CardTypeResult[CT_FOUR_LINE_TAKE_ONE].cbEachHandCardCount[Index] = 6 ;
	CardTypeResult[CT_FOUR_LINE_TAKE_ONE].cbCardTypeCount++ ;

	ASSERT(CardTypeResult[CT_FOUR_LINE_TAKE_ONE].cbCardTypeCount<MAX_TYPE_COUNT) ;
	}
	}
	}*/


	//四带两对
	/*
	{
	//恢复扑克，防止分析时改变扑克
	CopyMemory(cbTmpCardData, cbReserveCardData, cbHandCardCount) ;

	BYTE cbFirstCard = 0 ;
	//去除王牌
	for(BYTE i=0 ; i<cbHandCardCount ; ++i)	if(GetCardColor(cbTmpCardData[i])!=0x40)	{cbFirstCard = i ; break ;}

	BYTE cbHandAllFourCardData[MAX_COUNT] ;
	BYTE cbHandAllFourCardCount=0;

	//抽取四张
	GetAllBomCard(cbTmpCardData+cbFirstCard, cbHandCardCount-cbFirstCard, cbHandAllFourCardData, cbHandAllFourCardCount) ;

	//移除四条
	BYTE cbRemainCard[MAX_COUNT];
	BYTE cbRemainCardCount=cbHandCardCount-cbHandAllFourCardCount ;
	CopyMemory(cbRemainCard, cbTmpCardData, cbHandCardCount*sizeof(BYTE));
	RemoveCard(cbHandAllFourCardData, cbHandAllFourCardCount, cbRemainCard, cbHandCardCount) ;

	for(BYTE Start=0; Start<cbHandAllFourCardCount; Start += 4)
	{
	//抽取对牌
	BYTE cbAllDoubleCardData[MAX_COUNT] ;
	BYTE cbAllDoubleCardCount=0 ;
	GetAllDoubleCard(cbRemainCard, cbRemainCardCount, cbAllDoubleCardData, cbAllDoubleCardCount) ;

	BYTE cbDoubleCardIndex[10]; //对牌下标
	for(BYTE i=0, j=0; i<cbAllDoubleCardCount; i+=2, ++j)
	cbDoubleCardIndex[j]=i ;

	//对牌组合
	BYTE cbComCard[5];
	BYTE cbComResCard[255][5] ;
	BYTE cbComResLen=0 ;

	//利用对牌的下标做组合，再根据下标提取出对牌
	Combination(cbComCard, 0, cbComResCard, cbComResLen, cbDoubleCardIndex, 2, cbAllDoubleCardCount/2, 2);
	for(BYTE i=0; i<cbComResLen; ++i)
	{
	BYTE Index = CardTypeResult[CT_FOUR_LINE_TAKE_TWO].cbCardTypeCount ;
	CardTypeResult[CT_FOUR_LINE_TAKE_TWO].cbCardType = CT_FOUR_LINE_TAKE_TWO ;
	CopyMemory(CardTypeResult[CT_FOUR_LINE_TAKE_TWO].cbCardData[Index], cbHandAllFourCardData+Start, 4) ;

	//保存对牌
	for(BYTE j=0, k=0; j<4; ++j, k+=2)
	{
	CardTypeResult[CT_FOUR_LINE_TAKE_TWO].cbCardData[Index][4+k] = cbAllDoubleCardData[cbComResCard[i][j]];
	CardTypeResult[CT_FOUR_LINE_TAKE_TWO].cbCardData[Index][4+k+1] = cbAllDoubleCardData[cbComResCard[i][j]+1];
	}

	CardTypeResult[CT_FOUR_LINE_TAKE_TWO].cbEachHandCardCount[Index] = 8 ;
	CardTypeResult[CT_FOUR_LINE_TAKE_TWO].cbCardTypeCount++ ;

	ASSERT(CardTypeResult[CT_FOUR_LINE_TAKE_TWO].cbCardTypeCount<MAX_TYPE_COUNT) ;
	}
	}
	}*/

}

//分析牌型
void CGameLogic::AnalyseOutCardType(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE const cbTurnCardData[], BYTE const cbTurnCardCount,tagOutCardTypeResult CardTypeResult[12+1])
{
	ZeroMemory(CardTypeResult, sizeof(CardTypeResult[0])*12) ;
	
	BYTE cbTmpCard[MAX_COUNT];
	CopyMemory(cbTmpCard, cbHandCardData, cbHandCardCount) ;
	SortCardList(cbTmpCard, cbHandCardCount, ST_ORDER) ;
	//	SortCardList(cbTurnCardData, cbTurnCardCount, ST_ORDER) ;

	BYTE cbTurnCardType = GetCardType(cbTurnCardData, cbTurnCardCount) ;

	ASSERT(cbTurnCardType!=CT_ERROR) ;
#ifdef	_DEBUG
	if(cbTurnCardType==CT_ERROR)
	{
		CString str = "\nAnalyseOutCardType:  ";
		for(BYTE i=0; i<cbTurnCardCount; ++i)
		{
			CString strTmp ;
			strTmp.Format("%d,", GetCardValue(cbTurnCardData[i])) ;
			str += strTmp ;
		}
		str += "\n\n" ;
		OutputDebugString(str) ;
	}
#endif
	if(cbTurnCardType==CT_ERROR)
		return ;

	if(cbTurnCardType!=CT_MISSILE_CARD && cbTurnCardType!=CT_BOMB_CARD)
	{
		//双王炸弹
		if(cbHandCardCount>=2 && 0x4F==cbTmpCard[0] && 0x4E==cbTmpCard[1])
		{
			BYTE Index = CardTypeResult[CT_BOMB_CARD].cbCardTypeCount;
			CardTypeResult[CT_BOMB_CARD].cbCardType = CT_BOMB_CARD ;
			CardTypeResult[CT_BOMB_CARD].cbCardData[Index][0] = cbTmpCard[0] ;
			CardTypeResult[CT_BOMB_CARD].cbCardData[Index][1] = cbTmpCard[1] ;
			CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[Index] = 2 ;
			CardTypeResult[CT_BOMB_CARD].cbCardTypeCount++;

			BYTE cbBomCardData[MAX_COUNT];
			BYTE cbBomCardCount=0;
			GetAllBomCard(cbTmpCard+2, cbHandCardCount-2, cbBomCardData, cbBomCardCount) ;
			for(BYTE i=0; i<cbBomCardCount/4; ++i)
			{
				Index = CardTypeResult[CT_BOMB_CARD].cbCardTypeCount;
				CardTypeResult[CT_BOMB_CARD].cbCardType = CT_BOMB_CARD ;
				CopyMemory(CardTypeResult[CT_BOMB_CARD].cbCardData[Index], cbBomCardData+4*i, 4) ;
				CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[Index] = 4;
				CardTypeResult[CT_BOMB_CARD].cbCardTypeCount++;

				ASSERT(CardTypeResult[CT_BOMB_CARD].cbCardTypeCount<=MAX_TYPE_COUNT) ;
			}
		}
		//炸弹牌型
		else
		{
			BYTE cbBomCardData[MAX_COUNT];
			BYTE cbBomCardCount=0;
			GetAllBomCard(cbTmpCard, cbHandCardCount, cbBomCardData, cbBomCardCount) ;
			for(BYTE i=0; i<cbBomCardCount/4; ++i)
			{
				BYTE Index = CardTypeResult[CT_BOMB_CARD].cbCardTypeCount;
				CardTypeResult[CT_BOMB_CARD].cbCardType = CT_BOMB_CARD ;
				CopyMemory(CardTypeResult[CT_BOMB_CARD].cbCardData[Index], cbBomCardData+4*i, 4) ;
				CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[Index] = 4;
				CardTypeResult[CT_BOMB_CARD].cbCardTypeCount++;

				ASSERT(CardTypeResult[CT_BOMB_CARD].cbCardTypeCount<=MAX_TYPE_COUNT) ;
			}
		}
	}

	switch(cbTurnCardType)
	{
	case CT_SINGLE:				//单牌类型
		{			
			for(BYTE i=0; i<cbHandCardCount; ++i) 
				if(GetCardLogicValue(cbTmpCard[i])>GetCardLogicValue(cbTurnCardData[0])) 
				{
					BYTE Index = CardTypeResult[CT_SINGLE].cbCardTypeCount ;
					CardTypeResult[CT_SINGLE].cbCardType = CT_SINGLE ;
					CardTypeResult[CT_SINGLE].cbCardData[Index][0] = cbTmpCard[i];
					CardTypeResult[CT_SINGLE].cbEachHandCardCount[Index] = 1;
					CardTypeResult[CT_SINGLE].cbCardTypeCount++ ;

					ASSERT(CardTypeResult[CT_SINGLE].cbCardTypeCount<=MAX_TYPE_COUNT) ;
				}
				break ;
		}
	case CT_DOUBLE:				//对牌类型
		{
			//扑克分析
			for (BYTE i=0;i<cbHandCardCount;i++)
			{
				//变量定义
				BYTE cbSameCount=1;
				BYTE cbLogicValue=GetCardLogicValue(cbTmpCard[i]);

				//搜索同牌
				for (BYTE j=i+1;j<cbHandCardCount;j++)
				{
					//获取扑克
					if (GetCardLogicValue(cbTmpCard[j])!=cbLogicValue) break;

					//设置变量
					cbSameCount++;
				}

				if(cbSameCount>=2 && GetCardLogicValue(cbTmpCard[i])>GetCardLogicValue(cbTurnCardData[0]))
				{
					BYTE Index = CardTypeResult[CT_DOUBLE].cbCardTypeCount ;
					CardTypeResult[CT_DOUBLE].cbCardType = CT_DOUBLE ;
					CardTypeResult[CT_DOUBLE].cbCardData[Index][0] = cbTmpCard[i];
					CardTypeResult[CT_DOUBLE].cbCardData[Index][1] = cbTmpCard[i+1];
					CardTypeResult[CT_DOUBLE].cbEachHandCardCount[Index] = 2;
					CardTypeResult[CT_DOUBLE].cbCardTypeCount++ ;

					ASSERT(CardTypeResult[CT_DOUBLE].cbCardTypeCount<=MAX_TYPE_COUNT) ;
				}
				//设置索引
				i+=cbSameCount-1;
			}
			break ;
		}
	case CT_THREE:				//三条类型
		{
			//扑克分析
			for (BYTE i=0;i<cbHandCardCount;i++)
			{
				//变量定义
				BYTE cbSameCount=1;
				BYTE cbLogicValue=GetCardLogicValue(cbTmpCard[i]);

				//搜索同牌
				for (BYTE j=i+1;j<cbHandCardCount;j++)
				{
					//获取扑克
					if (GetCardLogicValue(cbTmpCard[j])!=cbLogicValue) break;

					//设置变量
					cbSameCount++;
				}

				if(cbSameCount>=3 && GetCardLogicValue(cbTmpCard[i])>GetCardLogicValue(cbTurnCardData[0]))
				{
					BYTE Index = CardTypeResult[CT_THREE].cbCardTypeCount ;
					CardTypeResult[CT_THREE].cbCardType = CT_THREE ;
					CardTypeResult[CT_THREE].cbCardData[Index][0] = cbTmpCard[i];
					CardTypeResult[CT_THREE].cbCardData[Index][1] = cbTmpCard[i+1];
					CardTypeResult[CT_THREE].cbCardData[Index][2] = cbTmpCard[i+2];
					CardTypeResult[CT_THREE].cbEachHandCardCount[Index] = 3;
					CardTypeResult[CT_THREE].cbCardTypeCount++ ;

					ASSERT(CardTypeResult[CT_THREE].cbCardTypeCount<=MAX_TYPE_COUNT) ;
				}
				//设置索引
				i+=cbSameCount-1;
			}
			break ;
		}
	case CT_SINGLE_LINE:		//单连类型
		{
			BYTE cbFirstCard = 0 ;
			//去除2和王
			for(BYTE i=0 ; i<cbHandCardCount ; ++i)	if(GetCardLogicValue(cbTmpCard[i])<15)	{cbFirstCard = i ; break ;}

			BYTE cbSingleLineCard[12] ;
			BYTE cbSingleLineCount=1 ;
			BYTE cbLeftCardCount = cbHandCardCount ;
			bool bFindSingleLine = true ;

			//连牌判断
			while (cbLeftCardCount>=cbTurnCardCount && bFindSingleLine)
			{
				cbSingleLineCount=1 ;
				bFindSingleLine = false ;
				BYTE cbLastCard = cbTmpCard[cbFirstCard] ;
				cbSingleLineCard[cbSingleLineCount-1] = cbTmpCard[cbFirstCard] ;
				for (BYTE i=cbFirstCard+1; i<cbLeftCardCount; i++)
				{
					BYTE cbCardData=cbTmpCard[i];

					//连续判断
					if (1!=(GetCardLogicValue(cbLastCard)-GetCardLogicValue(cbCardData)) && GetCardValue(cbLastCard)!=GetCardValue(cbCardData)) 
					{
						cbLastCard = cbTmpCard[i] ;
						//是否合法
						if(cbSingleLineCount<cbTurnCardCount) 
						{
							cbSingleLineCount = 1 ;
							cbSingleLineCard[cbSingleLineCount-1] = cbTmpCard[i] ;
							continue ;
						}
						else break ;
					}
					//同牌判断
					else if(GetCardValue(cbLastCard)!=GetCardValue(cbCardData))
					{
						cbLastCard = cbCardData ;
						cbSingleLineCard[cbSingleLineCount] = cbCardData ;
						++cbSingleLineCount ;
					}
				}

				//保存数据
				if(cbSingleLineCount>=cbTurnCardCount && GetCardLogicValue(cbSingleLineCard[0])>GetCardLogicValue(cbTurnCardData[0]))
				{
					BYTE Index ;
					BYTE cbStart=0 ;
					//所有连牌
					while (GetCardLogicValue(cbSingleLineCard[cbStart])>GetCardLogicValue(cbTurnCardData[0]) && ((cbSingleLineCount-cbStart)>=cbTurnCardCount))
					{
						Index = CardTypeResult[CT_SINGLE_LINE].cbCardTypeCount ;
						CardTypeResult[CT_SINGLE_LINE].cbCardType = CT_SINGLE_LINE ;
						CopyMemory(CardTypeResult[CT_SINGLE_LINE].cbCardData[Index], cbSingleLineCard+cbStart, sizeof(BYTE)*cbTurnCardCount);
						CardTypeResult[CT_SINGLE_LINE].cbEachHandCardCount[Index] = cbTurnCardCount;
						CardTypeResult[CT_SINGLE_LINE].cbCardTypeCount++ ;
						cbStart++;

						ASSERT(CardTypeResult[CT_SINGLE_LINE].cbCardTypeCount<=MAX_TYPE_COUNT) ;
					}

					RemoveCard(cbSingleLineCard, cbSingleLineCount, cbTmpCard, cbLeftCardCount) ;
					cbLeftCardCount -= cbSingleLineCount ;
					bFindSingleLine = true ;
				}
			}

			break ;
		}
	case CT_DOUBLE_LINE:		//对连类型
		{
			//连牌判断
			BYTE cbFirstCard = 0 ;
			//去除2和王
			for(BYTE i=0 ; i<cbHandCardCount ; ++i)	if(GetCardLogicValue(cbTmpCard[i])<15)	{cbFirstCard = i ; break ;}

			BYTE cbLeftCardCount = cbHandCardCount-cbFirstCard ;
			bool bFindDoubleLine = true ;
			BYTE cbDoubleLineCount = 0 ;
			BYTE cbDoubleLineCard[24] ;
			//开始判断
			while (cbLeftCardCount>=cbTurnCardCount && bFindDoubleLine)
			{
				BYTE cbLastCard = cbTmpCard[cbFirstCard] ;
				BYTE cbSameCount = 1 ;
				cbDoubleLineCount = 0 ;
				bFindDoubleLine=false ;
				for(BYTE i=cbFirstCard+1 ; i<cbLeftCardCount+cbFirstCard ; ++i)
				{
					//搜索同牌
					while (GetCardValue(cbLastCard)==GetCardValue(cbTmpCard[i]) && i<cbLeftCardCount+cbFirstCard)
					{
						++cbSameCount;
						++i ;
					}

					BYTE cbLastDoubleCardValue ;
					if(cbDoubleLineCount>0) cbLastDoubleCardValue = GetCardLogicValue(cbDoubleLineCard[cbDoubleLineCount-1]) ;
					//重新开始
					if((cbSameCount<2 || (cbDoubleLineCount>0 && (cbLastDoubleCardValue-GetCardLogicValue(cbLastCard))!=1)) && i<=cbLeftCardCount+cbFirstCard)
					{
						if(cbDoubleLineCount>=cbTurnCardCount) break ;

						if(cbSameCount>=2) i-=cbSameCount ;

						cbLastCard = cbTmpCard[i] ;
						cbDoubleLineCount = 0 ;
					}
					//保存数据
					else if(cbSameCount>=2)
					{
						cbDoubleLineCard[cbDoubleLineCount] = cbTmpCard[i-cbSameCount] ;
						cbDoubleLineCard[cbDoubleLineCount+1] = cbTmpCard[i-cbSameCount+1] ;
						cbDoubleLineCount += 2 ;

						//结尾判断
						if(i==(cbLeftCardCount+cbFirstCard-2))
							if((GetCardLogicValue(cbLastCard)-GetCardLogicValue(cbTmpCard[i]))==1 && (GetCardLogicValue(cbTmpCard[i])==GetCardLogicValue(cbTmpCard[i+1])))
							{
								cbDoubleLineCard[cbDoubleLineCount] = cbTmpCard[i] ;
								cbDoubleLineCard[cbDoubleLineCount+1] = cbTmpCard[i+1] ;
								cbDoubleLineCount += 2 ;
								break ;
							}

					}

					cbLastCard = cbTmpCard[i] ;
					cbSameCount = 1 ;
				}

				//保存数据
				if(cbDoubleLineCount>=cbTurnCardCount)
				{
					BYTE Index ;
					BYTE cbStart=0 ;
					//所有连牌
					while (GetCardLogicValue(cbDoubleLineCard[cbStart])>GetCardLogicValue(cbTurnCardData[0]) && ((cbDoubleLineCount-cbStart)>=cbTurnCardCount))
					{
						Index = CardTypeResult[CT_DOUBLE_LINE].cbCardTypeCount ;
						CardTypeResult[CT_DOUBLE_LINE].cbCardType = CT_DOUBLE_LINE ;
						CopyMemory(CardTypeResult[CT_DOUBLE_LINE].cbCardData[Index], cbDoubleLineCard+cbStart, sizeof(BYTE)*cbTurnCardCount);
						CardTypeResult[CT_DOUBLE_LINE].cbEachHandCardCount[Index] = cbTurnCardCount;
						CardTypeResult[CT_DOUBLE_LINE].cbCardTypeCount++ ;
						cbStart += 2;

						ASSERT(CardTypeResult[CT_DOUBLE_LINE].cbCardTypeCount<=MAX_TYPE_COUNT) ;
					}
					RemoveCard(cbDoubleLineCard, cbDoubleLineCount, cbTmpCard, cbFirstCard+cbLeftCardCount) ;				
					bFindDoubleLine=true ;
					cbLeftCardCount -= cbDoubleLineCount ;
				}
			}

			break;
		}
	case CT_THREE_LINE:			//三连类型
		{
			//连牌判断
			BYTE cbFirstCard = 0 ;
			//去除2和王
			for(BYTE i=0 ; i<cbHandCardCount ; ++i)	if(GetCardLogicValue(cbTmpCard[i])<15)	{cbFirstCard = i ; break ;}

			BYTE cbLeftCardCount = cbHandCardCount-cbFirstCard ;
			bool bFindThreeLine = true ;
			BYTE cbThreeLineCount = 0 ;
			BYTE cbThreeLineCard[20] ;
			//开始判断
			while (cbLeftCardCount>=cbTurnCardCount && bFindThreeLine)
			{
				BYTE cbLastCard = cbTmpCard[cbFirstCard] ;
				BYTE cbSameCount = 1 ;
				cbThreeLineCount = 0 ;
				bFindThreeLine = false ;
				for(BYTE i=cbFirstCard+1 ; i<cbLeftCardCount+cbFirstCard ; ++i)
				{
					//搜索同牌
					while (GetCardValue(cbLastCard)==GetCardValue(cbTmpCard[i]) && i<cbLeftCardCount+cbFirstCard)
					{
						++cbSameCount;
						++i ;
					}

					BYTE cbLastThreeCardValue ;
					if(cbThreeLineCount>0) cbLastThreeCardValue = GetCardLogicValue(cbThreeLineCard[cbThreeLineCount-1]) ;

					//重新开始
					if((cbSameCount<3 || (cbThreeLineCount>0&&(cbLastThreeCardValue-GetCardLogicValue(cbLastCard))!=1)) && i<=cbLeftCardCount+cbFirstCard)
					{
						if(cbThreeLineCount>=cbTurnCardCount) break ;

						if(cbSameCount>=3) i-= 3 ;
						cbLastCard = cbTmpCard[i] ;
						cbThreeLineCount = 0 ;
					}
					//保存数据
					else if(cbSameCount>=3)
					{
						cbThreeLineCard[cbThreeLineCount] = cbTmpCard[i-cbSameCount] ;
						cbThreeLineCard[cbThreeLineCount+1] = cbTmpCard[i-cbSameCount+1] ;
						cbThreeLineCard[cbThreeLineCount+2] = cbTmpCard[i-cbSameCount+2] ;
						cbThreeLineCount += 3 ;

						//结尾判断
						if(i==(cbLeftCardCount+cbFirstCard-3))
							if((GetCardLogicValue(cbLastCard)-GetCardLogicValue(cbTmpCard[i]))==1 && (GetCardLogicValue(cbTmpCard[i])==GetCardLogicValue(cbTmpCard[i+1])) && (GetCardLogicValue(cbTmpCard[i])==GetCardLogicValue(cbTmpCard[i+2])))
							{
								cbThreeLineCard[cbThreeLineCount] = cbTmpCard[i] ;
								cbThreeLineCard[cbThreeLineCount+1] = cbTmpCard[i+1] ;
								cbThreeLineCard[cbThreeLineCount+2] = cbTmpCard[i+2] ;
								cbThreeLineCount += 3 ;
								break ;
							}

					}

					cbLastCard = cbTmpCard[i] ;
					cbSameCount = 1 ;
				}

				//保存数据
				if(cbThreeLineCount>=cbTurnCardCount)
				{
					BYTE Index ;
					BYTE cbStart=0 ;
					//所有连牌
					while (GetCardLogicValue(cbThreeLineCard[cbStart])>GetCardLogicValue(cbTurnCardData[0]) && ((cbThreeLineCount-cbStart)>=cbTurnCardCount))
					{
						Index = CardTypeResult[CT_THREE_LINE].cbCardTypeCount ;
						CardTypeResult[CT_THREE_LINE].cbCardType = CT_THREE_LINE ;
						CopyMemory(CardTypeResult[CT_THREE_LINE].cbCardData[Index], cbThreeLineCard+cbStart, sizeof(BYTE)*cbTurnCardCount);
						CardTypeResult[CT_THREE_LINE].cbEachHandCardCount[Index] = cbTurnCardCount;
						CardTypeResult[CT_THREE_LINE].cbCardTypeCount++ ;
						cbStart += 3;

						ASSERT(CardTypeResult[CT_THREE_LINE].cbCardTypeCount<=MAX_TYPE_COUNT) ;
					}

					RemoveCard(cbThreeLineCard, cbThreeLineCount, cbTmpCard, cbFirstCard+cbLeftCardCount) ;
					bFindThreeLine=true ;
					cbLeftCardCount -= cbThreeLineCount ;
				}
			}

			break;
		}
	case CT_THREE_LINE_TAKE_ONE://三带一单
		{
			BYTE cbTurnThreeCard[MAX_COUNT]; 
			BYTE cbTurnThreeCount=0;
			BYTE cbHandThreeCard[MAX_COUNT];
			BYTE cbHandThreeCount=0 ;
			BYTE cbSingleCardCount=cbTurnCardCount/4;

			//移除炸弹
			BYTE cbAllBomCardData[MAX_COUNT] ;
			BYTE cbAllBomCardCount=0 ;
			GetAllBomCard(cbTmpCard, cbHandCardCount, cbAllBomCardData, cbAllBomCardCount) ;
			RemoveCard(cbAllBomCardData, cbAllBomCardCount, cbTmpCard, cbHandCardCount) ;

			//三条扑克
			GetAllThreeCard(cbTurnCardData, cbTurnCardCount, cbTurnThreeCard, cbTurnThreeCount) ;

			BYTE cbFirstCard = 0 ;

			//去除2和王
			if(cbTurnThreeCount>3)
				for(BYTE i=0 ; i<cbHandCardCount-cbAllBomCardCount ; ++i)	
					if(GetCardLogicValue(cbTmpCard[i])<15)	
					{
						cbFirstCard = i ; 
						break ;
					}

					GetAllThreeCard(cbTmpCard+cbFirstCard, cbHandCardCount-cbFirstCard-cbAllBomCardCount, cbHandThreeCard, cbHandThreeCount) ;

					if(cbHandThreeCount<cbTurnThreeCount || (cbHandThreeCount>0&&GetCardLogicValue(cbHandThreeCard[0])<GetCardLogicValue(cbTurnThreeCard[0]))) return ;

					for(BYTE i=0; i<cbHandThreeCount; i+=3)
					{
						BYTE cbLastLogicCard=GetCardLogicValue(cbHandThreeCard[i]);
						BYTE cbThreeLineCard[MAX_COUNT];
						BYTE cbThreeLineCardCount=3;
						cbThreeLineCard[0]=cbHandThreeCard[i];
						cbThreeLineCard[1]=cbHandThreeCard[i+1];
						cbThreeLineCard[2]=cbHandThreeCard[i+2];
						for(BYTE j=i+3; j<cbHandThreeCount; j+=3)
						{
							//连续判断
							if(1!=(cbLastLogicCard-(GetCardLogicValue(cbHandThreeCard[j]))) || cbThreeLineCardCount==cbTurnThreeCount) break;

							cbLastLogicCard=GetCardLogicValue(cbHandThreeCard[j]);
							cbThreeLineCard[cbThreeLineCardCount]=cbHandThreeCard[j];
							cbThreeLineCard[cbThreeLineCardCount+1]=cbHandThreeCard[j+1];
							cbThreeLineCard[cbThreeLineCardCount+2]=cbHandThreeCard[j+2];
							cbThreeLineCardCount += 3;
						}
						if(cbThreeLineCardCount==cbTurnThreeCount && GetCardLogicValue(cbThreeLineCard[0])>GetCardLogicValue(cbTurnThreeCard[0]))
						{
							BYTE Index ;

							BYTE cbRemainCard[MAX_COUNT];
							CopyMemory(cbRemainCard, cbTmpCard, (cbHandCardCount-cbAllBomCardCount)*sizeof(BYTE));
							RemoveCard(cbThreeLineCard, cbTurnThreeCount, cbRemainCard, (cbHandCardCount-cbAllBomCardCount)) ;

							//单牌组合
							BYTE cbComCard[5];
							BYTE cbComResCard[254][5] ;
							BYTE cbComResLen=0 ;
							Combination(cbComCard, 0, cbComResCard, cbComResLen, cbRemainCard, cbSingleCardCount, (cbHandCardCount-cbAllBomCardCount)-cbTurnThreeCount, cbSingleCardCount);
							for(BYTE i=0; i<cbComResLen; ++i)
							{
								Index = CardTypeResult[CT_THREE_LINE_TAKE_ONE].cbCardTypeCount ;
								CardTypeResult[CT_THREE_LINE_TAKE_ONE].cbCardType = CT_THREE_LINE_TAKE_ONE;
								//保存三条
								CopyMemory(CardTypeResult[CT_THREE_LINE_TAKE_ONE].cbCardData[Index], cbThreeLineCard, sizeof(BYTE)*cbTurnThreeCount);
								//保存单牌
								CopyMemory(CardTypeResult[CT_THREE_LINE_TAKE_ONE].cbCardData[Index]+cbTurnThreeCount, cbComResCard[i], cbSingleCardCount) ;

								ASSERT(cbTurnThreeCount+cbSingleCardCount==cbTurnCardCount) ;
								CardTypeResult[CT_THREE_LINE_TAKE_ONE].cbEachHandCardCount[Index] = cbTurnCardCount ;
								CardTypeResult[CT_THREE_LINE_TAKE_ONE].cbCardTypeCount++ ;

								ASSERT(CardTypeResult[CT_THREE_LINE_TAKE_ONE].cbCardTypeCount<=MAX_TYPE_COUNT) ;
							}

						}
					}

					break;
		}
	case CT_FOUR_LINE_TAKE_ONE://四带两单
		{
			BYTE cbFirstCard = 0 ;
			//去除王牌
			for(BYTE i=0 ; i<cbHandCardCount ; ++i)	if(GetCardColor(cbTmpCard[i])!=0x40)	{cbFirstCard = i ; break ;}

			BYTE cbHandAllFourCardData[MAX_COUNT] ;
			BYTE cbHandAllFourCardCount=0;
			BYTE cbTurnAllFourCardData[MAX_COUNT];
			BYTE cbTurnAllFourCardCount=0;
			//抽取四张
			GetAllBomCard(cbTmpCard+cbFirstCard, cbHandCardCount-cbFirstCard, cbHandAllFourCardData, cbHandAllFourCardCount) ;
			GetAllBomCard(cbTurnCardData, cbTurnCardCount, cbTurnAllFourCardData, cbTurnAllFourCardCount) ;

			if(cbHandAllFourCardCount>0 && GetCardLogicValue(cbHandAllFourCardData[0])<GetCardLogicValue(cbTurnAllFourCardData[0])) return ;


			BYTE cbCanOutFourCardData[MAX_COUNT] ;
			BYTE cbCanOutFourCardCount=0 ;
			//可出的牌
			for(BYTE i=0; i<cbHandAllFourCardCount; i+=4)
			{
				if(GetCardLogicValue(cbHandAllFourCardData[i])>GetCardLogicValue(cbTurnAllFourCardData[0]))
				{
					cbCanOutFourCardData[cbCanOutFourCardCount] = cbHandAllFourCardData[i] ;
					cbCanOutFourCardData[cbCanOutFourCardCount+1] = cbHandAllFourCardData[i+1] ;
					cbCanOutFourCardData[cbCanOutFourCardCount+2] = cbHandAllFourCardData[i+2] ;
					cbCanOutFourCardData[cbCanOutFourCardCount+3] = cbHandAllFourCardData[i+3] ;
					cbCanOutFourCardCount += 4 ;
				}
			}

			if((cbHandCardCount-cbCanOutFourCardCount) < (cbTurnCardCount-cbTurnAllFourCardCount)) return ;

			BYTE cbRemainCard[MAX_COUNT];
			CopyMemory(cbRemainCard, cbTmpCard, cbHandCardCount*sizeof(BYTE));
			RemoveCard(cbCanOutFourCardData, cbCanOutFourCardCount, cbRemainCard, cbHandCardCount) ;
			for(BYTE Start=0; Start<cbCanOutFourCardCount; Start += 4)
			{
				BYTE Index ;
				//单牌组合
				BYTE cbComCard[5];
				BYTE cbComResCard[254][5] ;
				BYTE cbComResLen=0 ;
				//单牌组合
				Combination(cbComCard, 0, cbComResCard, cbComResLen, cbRemainCard, 2, cbHandCardCount-cbCanOutFourCardCount, 2);
				for(BYTE i=0; i<cbComResLen; ++i)
				{
					//不能带对
					if(GetCardValue(cbComResCard[i][0])==GetCardValue(cbComResCard[i][1])) continue ;

					Index=CardTypeResult[CT_FOUR_LINE_TAKE_ONE].cbCardTypeCount ;
					CardTypeResult[CT_FOUR_LINE_TAKE_ONE].cbCardType = CT_FOUR_LINE_TAKE_ONE ;
					CopyMemory(CardTypeResult[CT_FOUR_LINE_TAKE_ONE].cbCardData[Index], cbCanOutFourCardData+Start, 4) ;
					CardTypeResult[CT_FOUR_LINE_TAKE_ONE].cbCardData[Index][4] = cbComResCard[i][0] ;
					CardTypeResult[CT_FOUR_LINE_TAKE_ONE].cbCardData[Index][4+1] = cbComResCard[i][1] ;
					CardTypeResult[CT_FOUR_LINE_TAKE_ONE].cbEachHandCardCount[Index] = 6 ;
					CardTypeResult[CT_FOUR_LINE_TAKE_ONE].cbCardTypeCount++ ;

					ASSERT(CardTypeResult[CT_FOUR_LINE_TAKE_ONE].cbCardTypeCount<=MAX_TYPE_COUNT) ;
				}
			}


			break;
		}
	case CT_FOUR_LINE_TAKE_TWO://四带两对
		{
			BYTE cbFirstCard = 0 ;
			//去除王牌
			for(BYTE i=0 ; i<cbHandCardCount ; ++i)	if(GetCardColor(cbTmpCard[i])!=0x40)	{cbFirstCard = i ; break ;}

			BYTE cbHandAllFourCardData[MAX_COUNT] ;
			BYTE cbHandAllFourCardCount=0;
			BYTE cbTurnAllFourCardData[MAX_COUNT];
			BYTE cbTurnAllFourCardCount=0;
			//抽取四张
			GetAllBomCard(cbTmpCard+cbFirstCard, cbHandCardCount-cbFirstCard, cbHandAllFourCardData, cbHandAllFourCardCount) ;
			GetAllBomCard(cbTurnCardData, cbTurnCardCount, cbTurnAllFourCardData, cbTurnAllFourCardCount) ;

			if(cbHandAllFourCardCount>0 && GetCardLogicValue(cbHandAllFourCardData[0])<GetCardLogicValue(cbTurnAllFourCardData[0])) return ;


			BYTE cbCanOutFourCardData[MAX_COUNT] ;
			BYTE cbCanOutFourCardCount=0 ;
			//可出的牌
			for(BYTE i=0; i<cbHandAllFourCardCount; i+=4)
			{
				if(GetCardLogicValue(cbHandAllFourCardData[i])>GetCardLogicValue(cbTurnAllFourCardData[0]))
				{
					cbCanOutFourCardData[cbCanOutFourCardCount] = cbHandAllFourCardData[i] ;
					cbCanOutFourCardData[cbCanOutFourCardCount+1] = cbHandAllFourCardData[i+1] ;
					cbCanOutFourCardData[cbCanOutFourCardCount+2] = cbHandAllFourCardData[i+2] ;
					cbCanOutFourCardData[cbCanOutFourCardCount+3] = cbHandAllFourCardData[i+3] ;
					cbCanOutFourCardCount += 4 ;
				}
			}

			if((cbHandCardCount-cbCanOutFourCardCount) < (cbTurnCardCount-cbTurnAllFourCardCount)) return ;

			BYTE cbRemainCard[MAX_COUNT];
			CopyMemory(cbRemainCard, cbTmpCard, cbHandCardCount*sizeof(BYTE));
			RemoveCard(cbCanOutFourCardData, cbCanOutFourCardCount, cbRemainCard, cbHandCardCount) ;
			for(BYTE Start=0; Start<cbCanOutFourCardCount; Start += 4)
			{
				BYTE cbAllDoubleCardData[MAX_COUNT] ;
				BYTE cbAllDoubleCardCount=0 ;
				GetAllDoubleCard(cbRemainCard, cbHandCardCount-cbCanOutFourCardCount, cbAllDoubleCardData, cbAllDoubleCardCount) ;


				BYTE cbDoubleCardIndex[10]; //对牌下标
				for(BYTE i=0, j=0; i<cbAllDoubleCardCount; i+=2, ++j)
					cbDoubleCardIndex[j]=i ;

				//对牌组合
				BYTE cbComCard[5];
				BYTE cbComResCard[254][5] ;
				BYTE cbComResLen=0 ;

				//利用对牌的下标做组合，再根据下标提取出对牌
				Combination(cbComCard, 0, cbComResCard, cbComResLen, cbDoubleCardIndex, 2, cbAllDoubleCardCount/2, 2);
				for(BYTE i=0; i<cbComResLen; ++i)
				{
					BYTE Index = CardTypeResult[CT_FOUR_LINE_TAKE_TWO].cbCardTypeCount ;
					CardTypeResult[CT_FOUR_LINE_TAKE_TWO].cbCardType = CT_FOUR_LINE_TAKE_TWO ;
					CopyMemory(CardTypeResult[CT_FOUR_LINE_TAKE_TWO].cbCardData[Index], cbCanOutFourCardData+Start, 4) ;

					//保存对牌
					for(BYTE j=0, k=0; j<4; ++j, k+=2)
					{
						CardTypeResult[CT_FOUR_LINE_TAKE_TWO].cbCardData[Index][4+k] = cbAllDoubleCardData[cbComResCard[i][j]];
						CardTypeResult[CT_FOUR_LINE_TAKE_TWO].cbCardData[Index][4+k+1] = cbAllDoubleCardData[cbComResCard[i][j]+1];
					}

					CardTypeResult[CT_FOUR_LINE_TAKE_TWO].cbEachHandCardCount[Index] = 8 ;
					CardTypeResult[CT_FOUR_LINE_TAKE_TWO].cbCardTypeCount++ ;

					ASSERT(CardTypeResult[CT_FOUR_LINE_TAKE_TWO].cbCardTypeCount<=MAX_TYPE_COUNT) ;
				}
			}
			break;
		}
	case CT_BOMB_CARD:			//炸弹类型
		{
			BYTE cbAllBomCardData[MAX_COUNT] ;
			BYTE cbAllBomCardCount=0 ; 
			GetAllBomCard(cbTmpCard, cbHandCardCount, cbAllBomCardData, cbAllBomCardCount) ;
			BYTE cbFirstBom=0 ;
			BYTE Index ;
			if(cbAllBomCardCount>0 && cbAllBomCardData[0]==0x4F)
			{
				Index = CardTypeResult[CT_BOMB_CARD].cbCardTypeCount ;
				CardTypeResult[CT_BOMB_CARD].cbCardType = CT_BOMB_CARD ;
				CardTypeResult[CT_BOMB_CARD].cbCardData[Index][0] = 0x4F ;
				CardTypeResult[CT_BOMB_CARD].cbCardData[Index][1] = 0x4E ;
				CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[Index] = 2 ;
				CardTypeResult[CT_BOMB_CARD].cbCardTypeCount++ ;

				ASSERT(CardTypeResult[CT_BOMB_CARD].cbCardTypeCount<=MAX_TYPE_COUNT) ;
				cbFirstBom=2;
			}
			for(BYTE i=cbFirstBom; i<cbAllBomCardCount; i+=4)
			{
				if(GetCardLogicValue(cbAllBomCardData[i])>GetCardLogicValue(cbTurnCardData[0]))
				{
					Index = CardTypeResult[CT_BOMB_CARD].cbCardTypeCount ;
					CardTypeResult[CT_BOMB_CARD].cbCardType = CT_BOMB_CARD ;
					CopyMemory(CardTypeResult[CT_BOMB_CARD].cbCardData[Index], cbAllBomCardData+i, 4) ;
					CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[Index] = 4 ;
					CardTypeResult[CT_BOMB_CARD].cbCardTypeCount++ ;

					ASSERT(CardTypeResult[CT_BOMB_CARD].cbCardTypeCount<=MAX_TYPE_COUNT) ;
				}
			}
			break;
		}
	case CT_MISSILE_CARD:		//火箭类型
		{
			//没有比火箭更大的牌了
			break;
		}
	default:
		ASSERT(false) ;
		break;
	}

}


/********************************************************************
函数名：Combination
参数：
cbCombineCardData：存储单个的组合结果
cbResComLen：已得到的组合长度，开始调用时此参数为0
cbResultCardData：存储所有的组合结果
cbResCardLen：cbResultCardData的第一下标的长度，组合结果的个数
cbSrcCardData：需要做组合的数据
cbSrcLen：cbSrcCardData的数据数目
cbCombineLen2，cbCombineLen1：组合的长度，开始调用时两者相等。
*********************************************************************/
//组合算法
void CGameLogic::Combination(BYTE cbCombineCardData[], BYTE cbResComLen,  BYTE cbResultCardData[100][5], BYTE &cbResCardLen,BYTE cbSrcCardData[] , BYTE cbCombineLen1, BYTE cbSrcLen, const BYTE cbCombineLen2)
{

	if( cbResComLen == cbCombineLen2 )
	{
		CopyMemory(&cbResultCardData[cbResCardLen], cbCombineCardData, cbResComLen) ;
		++cbResCardLen ;

		ASSERT(cbResCardLen<255) ;

	}
	else
	{ 
		if(cbCombineLen1 >= 1 && cbSrcLen > 0 && (cbSrcLen+1) >= 0 ){
			cbCombineCardData[cbCombineLen2-cbCombineLen1] =  cbSrcCardData[0];
			++cbResComLen ;
			Combination(cbCombineCardData,cbResComLen, cbResultCardData, cbResCardLen, cbSrcCardData+1,cbCombineLen1-1, cbSrcLen-1, cbCombineLen2);

			--cbResComLen;
			Combination(cbCombineCardData,cbResComLen, cbResultCardData, cbResCardLen, cbSrcCardData+1,cbCombineLen1, cbSrcLen-1, cbCombineLen2);
		}
	}
}

//排列算法
void CGameLogic::Permutation(BYTE *list, int m, int n, BYTE result[][4], BYTE &len)
{ 
	int j,temp; 
	if(m == n){ 
		for(j = 0; j < n; j++) 
			result[len][j]=list[j]; 
		len++ ;
	} 
	else{ 
		for(j = m; j < n; j++){ 
			temp = list[m] ;
			list[m] = list[j];
			list[j] = temp ;
			Permutation(list,m+1,n,result,len); 
			temp = list[m] ;
			list[m] = list[j];
			list[j] = temp ;
		} 
	} 
} 

//单牌个数
BYTE CGameLogic::AnalyseSinleCardCount(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE const cbWantOutCardData[], BYTE const cbWantOutCardCount, BYTE cbSingleCardData[])
{
	//参数判断
	ASSERT(cbHandCardCount>0) ;
	if(cbHandCardCount<=0) 
		return MAX_COUNT + 5 ;

	BYTE cbRemainCard[MAX_COUNT] ;
	BYTE cbRemainCardCount=0 ;
	CopyMemory(cbRemainCard, cbHandCardData, cbHandCardCount) ;
	SortCardList(cbRemainCard, cbHandCardCount, ST_ORDER) ;

	//移除扑克
	if(cbWantOutCardCount!=0) RemoveCard(cbWantOutCardData, cbWantOutCardCount, cbRemainCard, cbHandCardCount) ;
	cbRemainCardCount = cbHandCardCount-cbWantOutCardCount ;

	//函数指针
	typedef void (CGameLogic::*pGetAllCardFun)(BYTE const [], BYTE const , BYTE[], BYTE &); 

	//指针数组
	pGetAllCardFun GetAllCardFunArray[4] ;
	GetAllCardFunArray[0] = GetAllBomCard ;		//炸弹函数
	GetAllCardFunArray[1] = GetAllLineCard ;	//顺子函数
	GetAllCardFunArray[2] = GetAllThreeCard ;	//三条函数
	GetAllCardFunArray[3] = GetAllDoubleCard ;	//对子函数

	//指针数组下标
	BYTE cbIndexArray[4] = {0,1,2,3} ;
	//排列结果
	BYTE cbPermutationRes[24][4] ;
	BYTE cbLen=0 ;
	//计算排列
	Permutation(cbIndexArray, 0, 4, cbPermutationRes, cbLen) ;
	ASSERT(cbLen==24) ;
	if(cbLen!=24) return MAX_COUNT + 5 ;

	//单牌数目
	BYTE cbMinSingleCardCount = MAX_COUNT + 5 ;
	//计算最小值
	for(BYTE i=0; i<24; ++i)
	{
		//保留数据
		BYTE cbTmpCardData[MAX_COUNT] ;
		BYTE cbTmpCardCount = cbRemainCardCount ;
		CopyMemory(cbTmpCardData, cbRemainCard, cbRemainCardCount) ;

		for(BYTE j=0; j<4; ++j)
		{
			BYTE Index = cbPermutationRes[i][j] ;

			//校验下标
			ASSERT(Index>=0 && Index<4) ;
			if(Index<0 || Index>=4) return MAX_COUNT + 5 ;

			pGetAllCardFun pTmpGetAllCardFun = GetAllCardFunArray[Index] ;

			//提取扑克
			BYTE cbGetCardData[MAX_COUNT] ;
			BYTE cbGetCardCount=0 ;
			//成员函数
			((*this).*pTmpGetAllCardFun)(cbTmpCardData, cbTmpCardCount, cbGetCardData, cbGetCardCount) ;

			//删除扑克
			if(cbGetCardCount!=0) RemoveCard(cbGetCardData, cbGetCardCount, cbTmpCardData, cbTmpCardCount) ;
			cbTmpCardCount -= cbGetCardCount ;
		}

		//计算单牌
		BYTE cbSingleCard[MAX_COUNT] ;
		BYTE cbSingleCardCount=0 ;
		GetAllSingleCard(cbTmpCardData, cbTmpCardCount, cbSingleCard, cbSingleCardCount) ;
		if(cbMinSingleCardCount> cbSingleCardCount)
		{
			cbMinSingleCardCount = cbSingleCardCount ;

			//保存单牌
			if(cbSingleCardData!=NULL)
			{
				CopyMemory(cbSingleCardData, cbSingleCard, cbSingleCardCount) ;
			}
		}
	}

	//带大牌判断
	if (cbWantOutCardCount > 0)
	{
		//出牌类型
		BYTE cbCardType = GetCardType(cbWantOutCardData, cbWantOutCardCount) ;
		if (cbCardType == CT_THREE_LINE_TAKE_ONE)

		for (BYTE i = 3; i < cbWantOutCardCount; ++i)
		{
			if (GetCardLogicValue(cbWantOutCardData[i]) >= 14) cbMinSingleCardCount += 3 ;
		}
	}

	//拆三条判断
	if ( GetCardType(cbWantOutCardData, cbWantOutCardCount) == CT_DOUBLE )
	{
		BYTE cbAllThreeCardData[MAX_COUNT], cbAllThreeCount ;
		BYTE cbAllLineCardData[MAX_COUNT], cbAllLineCount ;

		GetAllThreeCard(cbHandCardData, cbHandCardCount, cbAllThreeCardData, cbAllThreeCount) ;
		GetAllLineCard( cbHandCardData, cbHandCardCount, cbAllLineCardData, cbAllLineCount ) ;

		BYTE cbThreeCardValue = 0 ;
		for ( BYTE i = 0; i < cbAllThreeCount; ++i)
			for ( BYTE j = 0; j < cbWantOutCardCount; ++j)
				if ( GetCardLogicValue(cbWantOutCardData[j]) == GetCardLogicValue(cbAllThreeCardData[i]) )
				{
					cbThreeCardValue = GetCardValue( cbWantOutCardData[j] ) ;
					break ;
				}

		//是否有连牌
		bool bInLineCard = false ;
		for ( BYTE cbLineCardIndex = 0; cbLineCardIndex < cbAllLineCount; ++cbLineCardIndex )
			if ( GetCardValue( cbAllLineCardData[cbLineCardIndex] ) == cbThreeCardValue )
			{
				bInLineCard = true ;
				break ;
			}
		if ( !bInLineCard && cbThreeCardValue != 0 ) cbMinSingleCardCount += 2 ;
	}

	//拆炸判断
	if(cbWantOutCardCount!=0)
	{
		//炸弹扑克
		BYTE cbBombCard[20] ;
		BYTE cbBombCardCount=0;
		GetAllBomCard(cbHandCardData, cbHandCardCount, cbBombCard, cbBombCardCount) ;

		//出牌类型
		BYTE cbCardType = GetCardType(cbWantOutCardData, cbWantOutCardCount) ;

		if(cbBombCardCount>0 && cbCardType<CT_BOMB_CARD)
		{
			//寻找相同
			for(BYTE i = GetCardColor(cbBombCard[0]) == 4 ? 2 : 0; i<cbBombCardCount; i += 4)
				for(BYTE j=0; j<cbWantOutCardCount; ++j)
				{
					if(GetCardValue( cbBombCard[i] ) == GetCardValue( cbWantOutCardData[j] ) && GetCardLogicValue(cbWantOutCardData[j])<15 && 
						cbCardType!=CT_SINGLE_LINE && cbCardType!=CT_DOUBLE_LINE) 
						return MAX_COUNT + 5 ; //不拆炸弹

					else if ( GetCardValue( cbBombCard[i] ) == GetCardValue( cbWantOutCardData[j] ) && GetCardLogicValue(cbWantOutCardData[j])<15 && 
						( cbCardType == CT_SINGLE_LINE || cbCardType == CT_DOUBLE_LINE) ) 
						cbMinSingleCardCount += 2 ; //不拆炸弹
				}

			//多个炸弹判断
			if (cbCardType == CT_SINGLE_LINE)
			{
				BYTE cbBombCount = 0 ;

				for (BYTE i = GetCardColor(cbBombCard[0]) == 4 ? 2 : 0; i < cbBombCardCount; i += 4)
					for (BYTE j=0; j < cbWantOutCardCount; ++j)
						if ( GetCardValue( cbBombCard[i] ) == GetCardValue( cbWantOutCardData[j] ) ) ++cbBombCount ;

				if (cbBombCount >= 2) return  MAX_COUNT ; //不拆炸弹

				//三条个数
				tagAnalyseResult AnalyseResult;
				AnalysebCardData(cbHandCardData, cbHandCardCount,AnalyseResult) ;

                BYTE cbThreeCount = 0 ;

				for (BYTE i = 0; i < AnalyseResult.cbThreeCount; ++i)
					for (BYTE j = 0; j < cbWantOutCardCount; ++j)
						if (GetCardValue(cbWantOutCardData[j]) == GetCardValue(AnalyseResult.cbThreeCardData[3 * i])) ++cbThreeCount ;

				if (cbThreeCount + cbBombCount >= 2) return  MAX_COUNT + 5 ;

			}
		}
	}

	return cbMinSingleCardCount ;
}

//设置扑克
void CGameLogic::SetUserCard(WORD wChairID, BYTE cbCardData[], BYTE cbCardCount)
{
	CopyMemory(m_cbAllCardData[wChairID], cbCardData, cbCardCount*sizeof(BYTE)) ;
	m_cbUserCardCount[wChairID] = cbCardCount ;

	//排列扑克
	SortCardList(m_cbAllCardData[wChairID], cbCardCount, ST_ORDER) ;
}

//设置底牌
void CGameLogic::SetBackCard(WORD wChairID, BYTE cbBackCardData[], BYTE cbCardCount)
{
	BYTE cbTmpCount = m_cbUserCardCount[wChairID] ;
	CopyMemory(m_cbAllCardData[wChairID]+cbTmpCount, cbBackCardData, cbCardCount*sizeof(BYTE)) ;
	m_cbUserCardCount[wChairID] += cbCardCount ;

	//排列扑克
	SortCardList(m_cbAllCardData[wChairID], m_cbUserCardCount[wChairID], ST_ORDER) ;
}

//设置庄家
void CGameLogic::SetBanker(WORD wBanker) 
{
	m_wBankerUser = wBanker ;
	m_lBankerOutCardCount = 0 ;
}

//叫牌扑克
void CGameLogic::SetLandScoreCardData(BYTE cbCardData[], BYTE cbCardCount) 
{
	ASSERT(cbCardCount==MAX_COUNT) ;
	if(cbCardCount!=MAX_COUNT) return ;

	CopyMemory(m_cbLandScoreCardData, cbCardData, cbCardCount*sizeof(BYTE)) ;
	//排列扑克
	SortCardList(m_cbLandScoreCardData, cbCardCount, ST_ORDER) ;
}

//倒拉设置
void CGameLogic::SetCallScore(WORD wChairID, bool bCallScore)
{
	if ( wChairID == INVALID_CHAIR ) 
	{
		for ( WORD wIndex = 0; wIndex < GAME_PLAYER; ++wIndex ) m_bCallScore[wIndex] = bCallScore;
	}
	else m_bCallScore[wChairID] = bCallScore;
}

//删除扑克
void CGameLogic::RemoveUserCardData(WORD wChairID, BYTE cbRemoveCardData[], BYTE cbRemoveCardCount) 
{
	bool bSuccess = RemoveCard(cbRemoveCardData, cbRemoveCardCount, m_cbAllCardData[wChairID], m_cbUserCardCount[wChairID]) ;
	ASSERT(bSuccess) ;
	m_cbUserCardCount[wChairID] -= cbRemoveCardCount ;

}

//地主出牌（先出牌）
void CGameLogic::BankerOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, tagOutCardResult & OutCardResult) 
{
	//零下标没用
	tagOutCardTypeResult CardTypeResult[12+1] ;
	ZeroMemory(CardTypeResult, sizeof(CardTypeResult)) ;

	//初始变量
	ZeroMemory(&OutCardResult, sizeof(OutCardResult)) ;

	BYTE cbLineCard[MAX_COUNT] ;
	BYTE cbThreeLineCard[MAX_COUNT] ;
	BYTE cbDoubleLineCard[MAX_COUNT] ;
	BYTE cbLineCardCount;
	BYTE cbThreeLineCardCount ;
	BYTE cbDoubleLineCount ;
	GetAllLineCard(cbHandCardData, cbHandCardCount, cbLineCard, cbLineCardCount) ;
	GetAllThreeCard(cbHandCardData, cbHandCardCount, cbThreeLineCard, cbThreeLineCardCount) ;
	GetAllDoubleCard(cbHandCardData, cbHandCardCount, cbDoubleLineCard, cbDoubleLineCount) ;

	WORD wUndersideOfBanker = (m_wBankerUser+1)%GAME_PLAYER ;	//地主下家
	WORD wUpsideOfBanker = (wUndersideOfBanker+1)%GAME_PLAYER ;	//地主上家

	//判断可否出完
	BYTE cbSingleCardCount = MAX_COUNT+CT_MISSILE_CARD ;
	bool bFindBestCard = false ;
	AnalyseOutCardType(cbHandCardData, cbHandCardCount, CardTypeResult) ;
	for(BYTE cbCardType=CT_SINGLE; cbCardType<=CT_MISSILE_CARD; ++cbCardType)
		if(CardTypeResult[cbCardType].cbCardTypeCount>0)
		{
			for(LONG lIndex=0; lIndex<CardTypeResult[cbCardType].cbCardTypeCount; ++lIndex)
			{
				if(TestOutAllCard(m_wBankerUser, CardTypeResult[cbCardType].cbCardData[lIndex], CardTypeResult[cbCardType].cbEachHandCardCount[lIndex], true))
				{
					//计算单牌
					BYTE cbTmpSingleCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, CardTypeResult[cbCardType].cbCardData[lIndex], 
						CardTypeResult[cbCardType].cbEachHandCardCount[lIndex]) ;

					//结果判断
					if (cbTmpSingleCount >= MAX_COUNT) continue ;

					//炸弹优先级排后
					BYTE cbBombCardType = GetCardType(CardTypeResult[cbCardType].cbCardData[lIndex], CardTypeResult[cbCardType].cbEachHandCardCount[lIndex]) ;
					if (cbBombCardType == CT_BOMB_CARD) cbTmpSingleCount += 4 ;
					else if (cbBombCardType == CT_MISSILE_CARD) cbTmpSingleCount += 5 ;
					else if ( 15 == GetCardLogicValue( CardTypeResult[ cbCardType ].cbCardData[ lIndex ][ 0 ] ) ) cbTmpSingleCount += 2;
					else if ( 15 < GetCardLogicValue( CardTypeResult[ cbCardType ].cbCardData[ lIndex ][ 0 ] ) ) cbTmpSingleCount += 3;


					////改变权值
					//if (cbBombCardType != CT_ERROR) cbTmpSingleCount += cbBombCardType ;
					
					if (cbTmpSingleCount <= cbSingleCardCount)
					{
						//设置变量
						OutCardResult.cbCardCount=CardTypeResult[cbCardType].cbEachHandCardCount[lIndex];
						CopyMemory(OutCardResult.cbResultCard, CardTypeResult[cbCardType].cbCardData[lIndex], 
						CardTypeResult[cbCardType].cbEachHandCardCount[lIndex]*sizeof(BYTE));
						cbSingleCardCount = cbTmpSingleCount ;
						bFindBestCard = true ;
					}

				}
			}
		}
		//直接返回
		if (bFindBestCard) return ;

	//对王和两单
	if ( cbHandCardCount == 4 && GetCardLogicValue(cbHandCardData[1]) == 16 && m_cbUserCardCount[wUndersideOfBanker] == 1 && 
		GetCardLogicValue(cbHandCardData[2]) < GetCardLogicValue(m_cbAllCardData[wUndersideOfBanker][0]))
	{
		OutCardResult.cbCardCount = 1 ;
		OutCardResult.cbResultCard[0] = cbHandCardData[2] ;
		return ;
	}

	//四带牌型判断
	if ( AnalyseFourCardType(cbHandCardData, cbHandCardCount, m_cbAllCardData[wUndersideOfBanker], m_cbUserCardCount[wUndersideOfBanker], OutCardResult ) )
	{
		return ;
	}

	//对王和两单
	if ( cbHandCardCount == 4 && GetCardLogicValue(cbHandCardData[1]) == 16 && m_cbUserCardCount[wUpsideOfBanker] == 1 && 
		GetCardLogicValue(cbHandCardData[2]) < GetCardLogicValue(m_cbAllCardData[wUpsideOfBanker][0]))
	{
		OutCardResult.cbCardCount = 1 ;
		OutCardResult.cbResultCard[0] = cbHandCardData[2] ;
		return ;
	}

	//四带牌型判断
	if ( AnalyseFourCardType(cbHandCardData, cbHandCardCount, m_cbAllCardData[wUpsideOfBanker], m_cbUserCardCount[wUpsideOfBanker], OutCardResult ) )
	{
		return ;
	}

	//如果只剩顺牌和单只，则先出顺
	{
		if(cbLineCardCount+1==cbHandCardCount && CT_SINGLE==GetCardType(cbLineCard, cbLineCardCount))
		{
			OutCardResult.cbCardCount = cbLineCardCount ;
			CopyMemory(OutCardResult.cbResultCard, cbLineCard, cbLineCardCount) ;
		}
		else if(cbThreeLineCardCount+1==cbHandCardCount && CT_THREE_LINE==GetCardType(cbThreeLineCard, cbThreeLineCardCount))
		{
			OutCardResult.cbCardCount = cbThreeLineCardCount ;
			CopyMemory(OutCardResult.cbResultCard, cbThreeLineCard, cbThreeLineCardCount) ;
		}
		else if(cbDoubleLineCount+1==cbHandCardCount && CT_DOUBLE_LINE==GetCardType(cbDoubleLineCard, cbDoubleLineCount))
		{
			OutCardResult.cbCardCount = cbDoubleLineCount ;
			CopyMemory(OutCardResult.cbResultCard, cbDoubleLineCard, cbDoubleLineCount) ;
		}
		//双王炸弹和一手
		else if(cbHandCardCount>2 && cbHandCardData[0]==0x4f && cbHandCardData[1]==0x4e && CT_ERROR!=GetCardType(cbHandCardData+2, cbHandCardCount-2) &&
			GetCardType(cbHandCardData+2, cbHandCardCount-2) !=CT_FOUR_LINE_TAKE_ONE && 
			GetCardType(cbHandCardData+2, cbHandCardCount-2) !=CT_FOUR_LINE_TAKE_TWO)
		{
			OutCardResult.cbCardCount = 2 ;
			OutCardResult.cbResultCard[0] = 0x4f ;
			OutCardResult.cbResultCard[1] = 0x4e ;
		}

		if(OutCardResult.cbCardCount>0)
		{
			return ;
		}
	}

	//对王加一只
	if(cbHandCardCount==3 && GetCardColor(cbHandCardData[0])==0x40 && GetCardColor(cbHandCardData[1])==0x40)
	{
		OutCardResult.cbCardCount = 2 ;
		OutCardResult.cbResultCard[0] = 0x4f ;
		OutCardResult.cbResultCard[1] = 0x4e ;
		return ;
	}
	//对王
	else if(cbHandCardCount==2 && GetCardColor(cbHandCardData[0])==0x40 && GetCardColor(cbHandCardData[1])==0x40)
	{
		OutCardResult.cbCardCount = 2 ;
		OutCardResult.cbResultCard[0] = 0x4f ;
		OutCardResult.cbResultCard[1] = 0x4e ;
		return ;
	}
	//只剩一手牌
	else if(CT_ERROR!=GetCardType(cbHandCardData, cbHandCardCount) && CT_FOUR_LINE_TAKE_ONE!=GetCardType(cbHandCardData, cbHandCardCount) && CT_FOUR_LINE_TAKE_TWO!=GetCardType(cbHandCardData, cbHandCardCount))
	{
		OutCardResult.cbCardCount = cbHandCardCount ;
		CopyMemory(OutCardResult.cbResultCard, cbHandCardData, cbHandCardCount) ;
		return ;
	}

	//只剩一张和一手
	if(cbHandCardCount>=2)
	{
		//上家扑克
		tagOutCardTypeResult UpsideCanOutCardType1[13] ;
		ZeroMemory(UpsideCanOutCardType1, sizeof(UpsideCanOutCardType1)) ;
		tagOutCardTypeResult UpsideCanOutCardType2[13] ;
		ZeroMemory(UpsideCanOutCardType2, sizeof(UpsideCanOutCardType2)) ;

		//下家扑克
		tagOutCardTypeResult UndersideCanOutCardType1[13] ;
		ZeroMemory(UndersideCanOutCardType1, sizeof(UndersideCanOutCardType1)) ;
		tagOutCardTypeResult UndersideCanOutCardType2[13] ;
		ZeroMemory(UndersideCanOutCardType2, sizeof(UndersideCanOutCardType2)) ;

		BYTE cbFirstHandCardType = GetCardType(cbHandCardData, cbHandCardCount-1) ;
		BYTE cbSecondHandCardType = GetCardType(cbHandCardData+1, cbHandCardCount-1) ;

		//是否有炸
		BYTE cbAllBombCardData[MAX_COUNT], cbAllBombCount=0 ;
		GetAllBomCard(cbHandCardData, cbHandCardCount, cbAllBombCardData, cbAllBombCount) ;

		//没有炸弹
		if (cbAllBombCount <= 0 && cbFirstHandCardType!=CT_THREE_LINE_TAKE_ONE)
		{
		if(CT_ERROR!=cbFirstHandCardType && cbFirstHandCardType!=CT_FOUR_LINE_TAKE_ONE && cbFirstHandCardType!= CT_FOUR_LINE_TAKE_TWO)
		{
			AnalyseOutCardType(m_cbAllCardData[wUpsideOfBanker], m_cbUserCardCount[wUpsideOfBanker], cbHandCardData, cbHandCardCount-1, UpsideCanOutCardType1) ;
			AnalyseOutCardType(m_cbAllCardData[wUndersideOfBanker], m_cbUserCardCount[wUndersideOfBanker], cbHandCardData, cbHandCardCount-1, UndersideCanOutCardType1) ;
		}
		if(CT_ERROR!=cbSecondHandCardType && cbSecondHandCardType!=CT_FOUR_LINE_TAKE_ONE && cbSecondHandCardType!= CT_FOUR_LINE_TAKE_TWO)
		{
			AnalyseOutCardType(m_cbAllCardData[wUpsideOfBanker], m_cbUserCardCount[wUpsideOfBanker], cbHandCardData+1, cbHandCardCount-1, UpsideCanOutCardType2) ;
			AnalyseOutCardType(m_cbAllCardData[wUndersideOfBanker], m_cbUserCardCount[wUndersideOfBanker], cbHandCardData+1, cbHandCardCount-1, UndersideCanOutCardType2) ;
		}

		if(cbSecondHandCardType!=CT_ERROR && cbSecondHandCardType!=CT_FOUR_LINE_TAKE_ONE && cbSecondHandCardType!= CT_FOUR_LINE_TAKE_TWO && UpsideCanOutCardType2[cbSecondHandCardType].cbCardTypeCount==0 && UndersideCanOutCardType2[cbSecondHandCardType].cbCardTypeCount==0 && 
			UpsideCanOutCardType2[CT_BOMB_CARD].cbCardTypeCount==0 && UndersideCanOutCardType2[CT_BOMB_CARD].cbCardTypeCount==0)
		{
			OutCardResult.cbCardCount = cbHandCardCount-1 ;
			CopyMemory(OutCardResult.cbResultCard, cbHandCardData+1, cbHandCardCount-1) ;
			return ;
		}

		if(cbFirstHandCardType!=CT_ERROR && cbFirstHandCardType!=CT_FOUR_LINE_TAKE_ONE && cbFirstHandCardType!= CT_FOUR_LINE_TAKE_TWO && UpsideCanOutCardType1[cbFirstHandCardType].cbCardTypeCount==0 && UndersideCanOutCardType1[cbFirstHandCardType].cbCardTypeCount==0 &&
			UpsideCanOutCardType2[CT_BOMB_CARD].cbCardTypeCount==0 && UndersideCanOutCardType2[CT_BOMB_CARD].cbCardTypeCount==0)
		{
			OutCardResult.cbCardCount = cbHandCardCount-1 ;
			CopyMemory(OutCardResult.cbResultCard, cbHandCardData, cbHandCardCount-1) ;
			return ;
		}

		if(GetCardLogicValue(cbHandCardData[0])>=GetCardLogicValue(m_cbAllCardData[wUpsideOfBanker][0]) &&
			GetCardLogicValue(cbHandCardData[0])>=GetCardLogicValue(m_cbAllCardData[wUndersideOfBanker][0]) &&
			CT_ERROR!=cbSecondHandCardType && cbSecondHandCardType!=CT_FOUR_LINE_TAKE_ONE && cbSecondHandCardType!= CT_FOUR_LINE_TAKE_TWO &&
			UpsideCanOutCardType2[CT_BOMB_CARD].cbCardTypeCount==0 && UndersideCanOutCardType2[CT_BOMB_CARD].cbCardTypeCount==0)
		{
			OutCardResult.cbCardCount = 1 ;
			OutCardResult.cbResultCard[0] = cbHandCardData[0] ;
			return ;
		}

		if(CT_ERROR!=cbSecondHandCardType && cbSecondHandCardType!=CT_FOUR_LINE_TAKE_ONE && cbSecondHandCardType!= CT_FOUR_LINE_TAKE_TWO &&
			UpsideCanOutCardType2[CT_BOMB_CARD].cbCardTypeCount==0 && UndersideCanOutCardType2[CT_BOMB_CARD].cbCardTypeCount==0)
		{
			OutCardResult.cbCardCount = cbHandCardCount-1 ;
			CopyMemory(OutCardResult.cbResultCard, cbHandCardData+1, cbHandCardCount-1) ;
			return ;
		}
		}
		//还有炸弹
		else
		{
			//除去炸后的牌
			BYTE cbRemainCard[MAX_COUNT], cbRemainCount=0 ;
			CopyMemory(cbRemainCard, cbHandCardData, cbHandCardCount) ;
			cbRemainCount = cbHandCardCount ;
			RemoveCard(cbAllBombCardData, cbAllBombCount, cbRemainCard, cbRemainCount) ;
			cbRemainCount -= cbAllBombCount ;
			if (GetCardType(cbRemainCard, cbRemainCount) != CT_ERROR)
			{
				OutCardResult.cbCardCount = cbRemainCount ;
				CopyMemory(OutCardResult.cbResultCard, cbRemainCard, cbRemainCount) ;
				return ;
			}
		}
	}

	{
		{
			//分析扑克
			tagOutCardTypeResult MeOutCardTypeResult[13] ;
			ZeroMemory(MeOutCardTypeResult, sizeof(MeOutCardTypeResult)) ;
			AnalyseOutCardType(cbHandCardData, cbHandCardCount, MeOutCardTypeResult) ;

			//计算单牌
			BYTE cbMinSingleCardCount[4] ;
			cbMinSingleCardCount[0]=MAX_COUNT ;
			cbMinSingleCardCount[1]=MAX_COUNT ;
			cbMinSingleCardCount[2]=MAX_COUNT ;
			cbMinSingleCardCount[3]=MAX_COUNT ;
			BYTE cbIndex[4]={0} ;
			BYTE cbOutcardType[4]={CT_ERROR} ;
			BYTE cbMinValue=MAX_COUNT ; 
			BYTE cbMinSingleCountInFour=MAX_COUNT ;
			BYTE cbMinCardType=CT_ERROR ;
			BYTE cbMinIndex=0 ;

			//除炸弹外的牌
			for(BYTE cbCardType=CT_DOUBLE; cbCardType<CT_BOMB_CARD; ++cbCardType)
			{

				tagOutCardTypeResult const &tmpCardResult = MeOutCardTypeResult[cbCardType] ;

				//相同牌型，相同长度，单连，对连等相同牌型可能长度不一样
				BYTE cbThisHandCardCount = MAX_COUNT ;

				//上家扑克
				tagOutCardTypeResult UpsideOutCardTypeResult[13] ;
				ZeroMemory(UpsideOutCardTypeResult, sizeof(UpsideOutCardTypeResult)) ;

				//下家扑克
				tagOutCardTypeResult UndersideOutCardTypeResult[13] ;
				ZeroMemory(UndersideOutCardTypeResult, sizeof(UndersideOutCardTypeResult)) ;


				for(BYTE i=0; i<tmpCardResult.cbCardTypeCount; ++i)
				{
					//拆三条判断
					if ( cbCardType == CT_DOUBLE )
					{
						tagAnalyseResult AnalyseResult ;
						ZeroMemory( &AnalyseResult,  sizeof( AnalyseResult ) ) ;
						AnalysebCardData( cbHandCardData, cbHandCardCount, AnalyseResult ) ;
						if ( AnalyseResult.cbSignedCount + AnalyseResult.cbThreeCount * 3 == cbHandCardCount )
						{
							bool bContinue = false ;
							for ( BYTE cbThreeIndex = 0; cbThreeIndex < AnalyseResult.cbThreeCount; ++cbThreeIndex )
								if ( GetCardValue(  tmpCardResult.cbCardData[i][0] ) == GetCardValue( AnalyseResult.cbThreeCardData[3 * cbThreeIndex] ) )
								{
									bContinue = true ;
									break ;
								}
							if ( bContinue ) continue ;
						}
					}

					BYTE cbTmpCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, tmpCardResult.cbCardData[i], tmpCardResult.cbEachHandCardCount[i]) ;

					//重新分析
					if(tmpCardResult.cbEachHandCardCount[i]!=cbThisHandCardCount)
					{
						cbThisHandCardCount = tmpCardResult.cbEachHandCardCount[i] ;
						AnalyseOutCardType(m_cbAllCardData[wUpsideOfBanker], m_cbUserCardCount[wUpsideOfBanker], 
							tmpCardResult.cbCardData[i], tmpCardResult.cbEachHandCardCount[i] ,UpsideOutCardTypeResult) ;
						AnalyseOutCardType(m_cbAllCardData[wUndersideOfBanker], m_cbUserCardCount[wUndersideOfBanker], 
							tmpCardResult.cbCardData[i], tmpCardResult.cbEachHandCardCount[i] ,UndersideOutCardTypeResult) ;
					}
					BYTE cbMaxValue=0 ; 
					BYTE Index = 0 ;

					//针对顺子，三条的大牌
					BYTE cbCurrentCardType = GetCardType(tmpCardResult.cbCardData[i], cbThisHandCardCount) ;
					if (cbThisHandCardCount != cbHandCardCount && cbCurrentCardType >= CT_SINGLE_LINE && cbCurrentCardType <= CT_THREE_LINE_TAKE_ONE &&
						( GetCardLogicValue(tmpCardResult.cbCardData[i][cbThisHandCardCount-1]) > GetCardLogicValue(cbHandCardData[cbHandCardCount-2]) || 
						GetCardLogicValue(tmpCardResult.cbCardData[i][0]) >= 11 ) )
					{
						BYTE cbRemainCardData[MAX_COUNT], cbRemainCardCount ;
						CopyMemory(cbRemainCardData, cbHandCardData, cbHandCardCount) ;
						cbRemainCardCount = cbHandCardCount ;

						//移除扑克
						RemoveCard(tmpCardResult.cbCardData[i], cbThisHandCardCount, cbRemainCardData, cbRemainCardCount) ;
						cbRemainCardCount -= cbThisHandCardCount ;

						//最大扑克
						BYTE cbCurrentLargestLogicCard = GetCardLogicValue(tmpCardResult.cbCardData[i][0]) ;

						if (GetCardType(cbRemainCardData, cbRemainCardCount) == CT_ERROR && (cbCurrentCardType >= CT_THREE_LINE_TAKE_ONE && 
							cbCurrentCardType <= CT_THREE_LINE_TAKE_ONE && cbCurrentLargestLogicCard >= 11 && cbThisHandCardCount <=5 || 
							cbCurrentCardType == CT_SINGLE_LINE && cbThisHandCardCount <= 6 && cbCurrentLargestLogicCard >= 12 || 
							cbCurrentCardType >= CT_DOUBLE_LINE && cbCurrentCardType <= CT_THREE_LINE && cbCurrentLargestLogicCard >= 12 && cbThisHandCardCount <= 8))
						{
							//暂时不出
							if ( cbCurrentCardType >= CT_SINGLE_LINE && cbCurrentCardType <= CT_THREE_LINE &&
								GetCardLogicValue(tmpCardResult.cbCardData[i][cbThisHandCardCount - 1]) > GetCardLogicValue(cbHandCardData[cbHandCardCount-3]) )
								continue ;

							if ( cbCurrentCardType >= CT_THREE_LINE_TAKE_ONE && cbCurrentCardType <= CT_THREE_LINE_TAKE_ONE &&
								GetCardLogicValue(tmpCardResult.cbCardData[i][0]) > GetCardLogicValue(cbHandCardData[cbHandCardCount-3]) )
								continue ;
						}
					}

					//针对大对（不可先出）
					if (cbCardType == CT_DOUBLE && GetCardLogicValue(tmpCardResult.cbCardData[i][0]) >= 11)
					{
						BYTE cbAllSingleCardData[MAX_COUNT], cbAllSingleCount ;
						cbAllSingleCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, NULL, 0, cbAllSingleCardData) ;
						if (cbAllSingleCount >= 2 && GetCardLogicValue(cbAllSingleCardData[cbAllSingleCount-2]) < 10) continue ;
					}

					//敌方可以压住牌
					if(UpsideOutCardTypeResult[cbCardType].cbCardTypeCount>0 || UndersideOutCardTypeResult[cbCardType].cbCardTypeCount>0)
					{
						//上家跑掉
						if(UpsideOutCardTypeResult[cbCardType].cbEachHandCardCount[0] > 0 && m_cbUserCardCount[wUpsideOfBanker]<=UpsideOutCardTypeResult[cbCardType].cbEachHandCardCount[0]+1)
							continue ;

						//下家跑掉
						if(UndersideOutCardTypeResult[cbCardType].cbEachHandCardCount[0] > 0 && m_cbUserCardCount[wUndersideOfBanker]<=UndersideOutCardTypeResult[cbCardType].cbEachHandCardCount[0]+1)
							continue ;

						//自己不可以再拿回牌权
						//if(UpsideOutCardTypeResult[cbCardType].cbCardTypeCount > 0 && GetCardLogicValue(tmpCardResult.cbCardData[0][0]) < GetCardLogicValue(UpsideOutCardTypeResult[cbCardType].cbCardData[0][0]) ||
						//	UpsideOutCardTypeResult[cbCardType].cbCardTypeCount > 0 && GetCardLogicValue(tmpCardResult.cbCardData[0][0]) < GetCardLogicValue(UpsideOutCardTypeResult[cbCardType].cbCardData[0][0])) 
						//	continue ;
					}
					//是否有大牌 
					if(tmpCardResult.cbEachHandCardCount[i] != cbHandCardCount)
					{
						bool bHaveLargeCard=false ;
						for(BYTE j=0; j<tmpCardResult.cbEachHandCardCount[i]; ++j)
						{
							if(GetCardLogicValue(tmpCardResult.cbCardData[i][j])>=15) bHaveLargeCard=true ;
							if(cbCardType!=CT_SINGLE_LINE && cbCardType!=CT_DOUBLE_LINE  && GetCardLogicValue(tmpCardResult.cbCardData[i][0])==14) bHaveLargeCard=true ; 
						}

						if(bHaveLargeCard)
							continue ;
					}

					//搜索cbMinSingleCardCount[4]的最大值
					for(BYTE j=0; j<4; ++j)
					{
						if(cbMinSingleCardCount[j]>=cbTmpCount)
						{
							cbMinSingleCardCount[j] = cbTmpCount ;
							cbIndex[j] = i ;
							cbOutcardType[j] = cbCardType ;
							break ;
						}
					}

					//保存最小值
					if(cbMinSingleCountInFour>=cbTmpCount)
					{
						//最小牌型
						cbMinCardType = cbCardType ;
						//最小牌型中的最小单牌
						cbMinSingleCountInFour=cbTmpCount ;						
						//最小牌型中的最小牌
						cbMinIndex=i ;
					}
				}
			}

			if(cbMinSingleCountInFour>=AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, NULL, 0)+3 && 
				(m_cbUserCardCount[wUndersideOfBanker]>=4 && m_cbUserCardCount[wUpsideOfBanker]>=4))
				cbMinSingleCountInFour=MAX_COUNT ;

			if(cbMinSingleCountInFour!=MAX_COUNT)
			{
				BYTE Index = cbMinIndex ;

				//选择最小牌
				for(BYTE i=0; i<4; ++i)
				{
					if(cbOutcardType[i]==cbMinCardType && cbMinSingleCardCount[i]<=cbMinSingleCountInFour &&
						GetCardLogicValue(MeOutCardTypeResult[cbMinCardType].cbCardData[cbIndex[i]][0])<GetCardLogicValue(MeOutCardTypeResult[cbMinCardType].cbCardData[Index][0]))
						Index = cbIndex[i] ;
				}

				//对王加一只
				if(cbHandCardCount==3 && GetCardColor(cbHandCardData[0])==0x40 && GetCardColor(cbHandCardData[1])==0x40)
				{
					OutCardResult.cbCardCount = 2 ;
					OutCardResult.cbResultCard[0] = 0x4f ;
					OutCardResult.cbResultCard[1] = 0x4e ;
					return ;
				}
				//对王
				else if(cbHandCardCount==2 && GetCardColor(cbHandCardData[0])==0x40 && GetCardColor(cbHandCardData[1])==0x40)
				{
					OutCardResult.cbCardCount = 2 ;
					OutCardResult.cbResultCard[0] = 0x4f ;
					OutCardResult.cbResultCard[1] = 0x4e ;
					return ;
				}
				else
				{
					//设置变量
					OutCardResult.cbCardCount=MeOutCardTypeResult[cbMinCardType].cbEachHandCardCount[Index];
					CopyMemory(OutCardResult.cbResultCard,MeOutCardTypeResult[cbMinCardType].cbCardData[Index],MeOutCardTypeResult[cbMinCardType].cbEachHandCardCount[Index]*sizeof(BYTE));
					return ;
				}

				ASSERT(OutCardResult.cbCardCount>0) ;

				return ;
			}

			//还没有找到适合的牌则从大出到小
			if(OutCardResult.cbCardCount<=0 && (m_cbUserCardCount[wUndersideOfBanker] == 1 || m_cbUserCardCount[wUpsideOfBanker] == 1))
			{
				//只有一张牌时不能放走
				if(MeOutCardTypeResult[CT_SINGLE].cbCardTypeCount>0)
				{
					BYTE Index=MAX_COUNT ;
					for(BYTE i=0; i<MeOutCardTypeResult[CT_SINGLE].cbCardTypeCount; ++i)
					{
						if((m_cbUserCardCount[wUndersideOfBanker] == 1 && GetCardLogicValue(MeOutCardTypeResult[CT_SINGLE].cbCardData[i][0])>=GetCardLogicValue(m_cbAllCardData[wUndersideOfBanker][0])) ||
							(m_cbUserCardCount[wUpsideOfBanker] == 1 && GetCardLogicValue(MeOutCardTypeResult[CT_SINGLE].cbCardData[i][0])>=GetCardLogicValue(m_cbAllCardData[wUpsideOfBanker][0])))
						{
							Index=i ;
						}
						else break ;
					}

					if(MAX_COUNT!=Index)
					{
						OutCardResult.cbCardCount = MeOutCardTypeResult[CT_SINGLE].cbEachHandCardCount[Index] ;
						CopyMemory(OutCardResult.cbResultCard, MeOutCardTypeResult[CT_SINGLE].cbCardData[Index], OutCardResult.cbCardCount) ;
						return ;
					}
				}
			}
		}
	}
	BYTE cbFirstCard=0 ;
	//过滤王和2
	for(BYTE i=0; i<cbHandCardCount; ++i) 
		if(GetCardLogicValue(cbHandCardData[i])<15)
		{
			cbFirstCard = i ;
			break ;
		}

		if(cbFirstCard<cbHandCardCount-1)
			AnalyseOutCardType(cbHandCardData+cbFirstCard, cbHandCardCount-cbFirstCard, CardTypeResult) ;
		else
			AnalyseOutCardType(cbHandCardData, cbHandCardCount, CardTypeResult) ;

		//计算单牌
		BYTE cbMinSingleCardCount[4] ;
		cbMinSingleCardCount[0]=MAX_COUNT ;
		cbMinSingleCardCount[1]=MAX_COUNT ;
		cbMinSingleCardCount[2]=MAX_COUNT ;
		cbMinSingleCardCount[3]=MAX_COUNT ;
		BYTE cbIndex[4]={0} ;
		BYTE cbOutcardType[4]={CT_ERROR} ;
		BYTE cbMinValue=MAX_COUNT ; 
		BYTE cbMinSingleCountInFour=MAX_COUNT ;
		BYTE cbMinCardType=CT_ERROR ;
		BYTE cbMinIndex=0 ;

		//除炸弹外的牌
		for(BYTE cbCardType=CT_SINGLE; cbCardType<CT_BOMB_CARD; ++cbCardType)
		{
			tagOutCardTypeResult const &tmpCardResult = CardTypeResult[cbCardType] ;
			for(BYTE i=0; i<tmpCardResult.cbCardTypeCount; ++i)
			{
				//闲家可以走掉
				if ( CompareCard(tmpCardResult.cbCardData[i], m_cbAllCardData[wUndersideOfBanker], tmpCardResult.cbEachHandCardCount[i], m_cbUserCardCount[wUndersideOfBanker]) ||
					CompareCard(tmpCardResult.cbCardData[i],  m_cbAllCardData[wUpsideOfBanker], tmpCardResult.cbEachHandCardCount[i],m_cbUserCardCount[wUpsideOfBanker]))
					continue ;

				//针对顺子，三条的大牌
				if ( tmpCardResult.cbEachHandCardCount[i] != cbHandCardCount && cbCardType >= CT_SINGLE_LINE && cbCardType <= CT_THREE_LINE_TAKE_ONE &&
					( GetCardLogicValue(tmpCardResult.cbCardData[i][tmpCardResult.cbEachHandCardCount[i]-1]) > GetCardLogicValue(cbHandCardData[cbHandCardCount-2]) || 
						GetCardLogicValue(tmpCardResult.cbCardData[i][0]) >= 11 ))
				{
					BYTE cbRemainCardData[MAX_COUNT], cbRemainCardCount ;
					CopyMemory(cbRemainCardData, cbHandCardData, cbHandCardCount) ;
					cbRemainCardCount = cbHandCardCount ;

					//移除扑克
					RemoveCard(tmpCardResult.cbCardData[i], tmpCardResult.cbEachHandCardCount[i], cbRemainCardData, cbRemainCardCount) ;
					cbRemainCardCount -= tmpCardResult.cbEachHandCardCount[i] ;

					//最大扑克
					BYTE cbCurrentLargestLogicCard = GetCardLogicValue(tmpCardResult.cbCardData[i][0]) ;

					if (GetCardType(cbRemainCardData, cbRemainCardCount) == CT_ERROR && (cbCardType >= CT_THREE_LINE_TAKE_ONE && 
						cbCardType <= CT_THREE_LINE_TAKE_ONE && cbCurrentLargestLogicCard >= 11 && tmpCardResult.cbEachHandCardCount[i] <=5 || 
						cbCardType == CT_SINGLE_LINE && tmpCardResult.cbEachHandCardCount[i] <= 6 && cbCurrentLargestLogicCard >= 12 || 
						cbCardType >= CT_DOUBLE_LINE && cbCardType <= CT_THREE_LINE && cbCurrentLargestLogicCard >= 12 && tmpCardResult.cbEachHandCardCount[i] <= 8))
					{
						//暂时不出
						if ( cbCardType >= CT_SINGLE_LINE && cbCardType <= CT_THREE_LINE &&
							GetCardLogicValue(tmpCardResult.cbCardData[i][tmpCardResult.cbEachHandCardCount[i] - 1]) > GetCardLogicValue(cbHandCardData[cbHandCardCount-3]) )
							continue ;

						if ( cbCardType >= CT_THREE_LINE_TAKE_ONE && cbCardType <= CT_THREE_LINE_TAKE_ONE &&
							GetCardLogicValue(tmpCardResult.cbCardData[i][0]) > GetCardLogicValue(cbHandCardData[cbHandCardCount-3]) )
							continue ;
					}
				}

				BYTE cbTmpCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, tmpCardResult.cbCardData[i], tmpCardResult.cbEachHandCardCount[i]) ;

				BYTE cbMaxValue=0 ; 
				BYTE Index = 0 ;

				//搜索cbMinSingleCardCount[4]的最大值
				for(BYTE j=0; j<4; ++j)
				{
					if(cbMinSingleCardCount[j]>=cbTmpCount)
					{
						cbMinSingleCardCount[j] = cbTmpCount ;
						cbIndex[j] = i ;
						cbOutcardType[j] = cbCardType ;
						break ;
					}
				}

				//保存最小值
				if(cbMinSingleCountInFour>=cbTmpCount)
				{
					//最小牌型
					cbMinCardType = cbCardType ;
					//最小牌型中的最小单牌
					cbMinSingleCountInFour=cbTmpCount ;						
					//最小牌型中的最小牌
					cbMinIndex=i ;
				}
			}
		}

		if(cbMinSingleCountInFour!=MAX_COUNT)
		{
			BYTE Index = cbMinIndex ;

			//选择最小牌
			for(BYTE i=0; i<4; ++i)
			{
				if(cbOutcardType[i]==cbMinCardType && cbMinSingleCardCount[i]<=cbMinSingleCountInFour &&
					GetCardLogicValue(CardTypeResult[cbMinCardType].cbCardData[cbIndex[i]][0])<GetCardLogicValue(CardTypeResult[cbMinCardType].cbCardData[Index][0]))
					Index = cbIndex[i] ;
			}

			//对王加一只
			if(cbHandCardCount==3 && GetCardColor(cbHandCardData[0])==0x40 && GetCardColor(cbHandCardData[1])==0x40)
			{
				OutCardResult.cbCardCount = 2 ;
				OutCardResult.cbResultCard[0] = 0x4f ;
				OutCardResult.cbResultCard[1] = 0x4e ;
				return ;
			}
			//对王
			else if(cbHandCardCount==2 && GetCardColor(cbHandCardData[0])==0x40 && GetCardColor(cbHandCardData[1])==0x40)
			{
				OutCardResult.cbCardCount = 2 ;
				OutCardResult.cbResultCard[0] = 0x4f ;
				OutCardResult.cbResultCard[1] = 0x4e ;
				return ;
			}
			else
			{
				//设置变量
				OutCardResult.cbCardCount=CardTypeResult[cbMinCardType].cbEachHandCardCount[Index];
				CopyMemory(OutCardResult.cbResultCard,CardTypeResult[cbMinCardType].cbCardData[Index],CardTypeResult[cbMinCardType].cbEachHandCardCount[Index]*sizeof(BYTE));
				return ;
			}

			ASSERT(OutCardResult.cbCardCount>0) ;

			return ;
		}
		//如果只剩炸弹
		else if (CardTypeResult[CT_BOMB_CARD].cbCardTypeCount > 0) 
		{
			//BYTE Index=0 ;
			//BYTE cbLogicCardValue = GetCardLogicValue(0x4F)+1 ;
			////最小炸弹
			//for(BYTE i=0; i<CardTypeResult[CT_BOMB_CARD].cbCardTypeCount; ++i)
			//	if(cbLogicCardValue>GetCardLogicValue(CardTypeResult[CT_BOMB_CARD].cbCardData[i][0]))
			//	{
			//		cbLogicCardValue = GetCardLogicValue(CardTypeResult[CT_BOMB_CARD].cbCardData[i][0]) ;
			//		Index = i ;
			//	}

			//	//设置变量
			//	OutCardResult.cbCardCount=CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[Index];
			//	CopyMemory(OutCardResult.cbResultCard,CardTypeResult[CT_BOMB_CARD].cbCardData[Index],CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[Index]*sizeof(BYTE));

			//	return ;
		}

		BYTE cbAllSingleCardData[MAX_COUNT], cbAllSingleCardCount ;
		cbAllSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, NULL, 0, cbAllSingleCardData) ;

		if ( cbAllSingleCardCount > 0 )
		{
			//如果都没有搜索到就出最小的一张
			if ( ( 1 == m_cbUserCardCount[wUndersideOfBanker] || 1 == m_cbUserCardCount[wUpsideOfBanker] ) && cbAllSingleCardCount >= 2 ) 
			{
				OutCardResult.cbCardCount = 1 ;
				OutCardResult.cbResultCard[0] = cbAllSingleCardData[cbAllSingleCardCount-2] ;
			}
			else 
			{
				OutCardResult.cbCardCount = 1 ;
				OutCardResult.cbResultCard[0] = cbAllSingleCardData[cbAllSingleCardCount-1] ;
			}
			return ;
		}

		//如果都没有搜索到就出最小的一张
		OutCardResult.cbCardCount = 1 ;
		OutCardResult.cbResultCard[0] = cbHandCardData[cbHandCardCount-1] ;

		return ;
}

//地主出牌（后出牌）
void CGameLogic::BankerOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wOutCardUser, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResult & OutCardResult) 
{
	//初始变量
	ZeroMemory(&OutCardResult, sizeof(OutCardResult)) ;

	//零下标没用
	tagOutCardTypeResult CardTypeResult[12+1] ;
	ZeroMemory(CardTypeResult, sizeof(CardTypeResult)) ;

	//出牌类型
	BYTE cbOutCardType = GetCardType(cbTurnCardData,cbTurnCardCount) ;
	AnalyseOutCardType(cbHandCardData,cbHandCardCount,cbTurnCardData,cbTurnCardCount, CardTypeResult) ;

	WORD wUndersideUser = (m_wBankerUser+1)%GAME_PLAYER ;
	WORD wUpsideUser = (wUndersideUser+1)%GAME_PLAYER ;

	//只剩炸弹
	if(cbHandCardCount==CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[0] && (cbOutCardType<CT_BOMB_CARD || 
		GetCardLogicValue(CardTypeResult[CT_BOMB_CARD].cbCardData[0][0])>GetCardLogicValue(cbTurnCardData[0])))
	{
		OutCardResult.cbCardCount = CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[0] ;
		CopyMemory(OutCardResult.cbResultCard,  CardTypeResult[CT_BOMB_CARD].cbCardData, OutCardResult.cbCardCount) ;

		return ;
	}
	//双王炸弹和一手
	else if(cbHandCardCount>2 && cbHandCardData[0]==0x4f && cbHandCardData[1]==0x4e && CT_ERROR!=GetCardType(cbHandCardData+2, cbHandCardCount-2) &&
		CT_FOUR_LINE_TAKE_ONE != GetCardType(cbHandCardData+2, cbHandCardCount-2) && CT_FOUR_LINE_TAKE_TWO != GetCardType(cbHandCardData+2, cbHandCardCount-2))
	{
		OutCardResult.cbCardCount = 2 ;
		OutCardResult.cbResultCard[0] = 0x4f ;
		OutCardResult.cbResultCard[1] = 0x4e ;
		return  ;
	}

	//炸弹和一手
	BYTE cbRemainCard[MAX_COUNT], cbRemainCount=0;
	BYTE cbAllBombCard[MAX_COUNT], cbAllBombCount=0 ;
	GetAllBomCard(cbHandCardData, cbHandCardCount, cbAllBombCard, cbAllBombCount) ;

	//出炸判断
	if(cbAllBombCount>0)
	{
		//剩余扑克
		CopyMemory(cbRemainCard, cbHandCardData, cbHandCardCount) ;
		cbRemainCount = cbHandCardCount ;
		RemoveCard(cbAllBombCard, cbAllBombCount, cbRemainCard, cbRemainCount) ;
		cbRemainCount -= cbAllBombCount ;

		if(CT_ERROR != GetCardType(cbRemainCard, cbRemainCount) || 
			(2==cbRemainCount && GetCardLogicValue(cbRemainCard[0])>GetCardLogicValue(m_cbAllCardData[wUndersideUser][0]) &&
			GetCardLogicValue(cbRemainCard[0])>GetCardLogicValue(m_cbAllCardData[wUpsideUser][0])))
		{
			if((cbOutCardType<CT_BOMB_CARD || GetCardLogicValue(cbAllBombCard[0])>GetCardLogicValue(cbTurnCardData[0])) &&
				 ( CardTypeResult[cbOutCardType].cbCardTypeCount <= 0 || CT_ERROR != GetCardType(cbRemainCard, cbRemainCount) ) )
			{
				//双王炸弹
				if(GetCardColor(cbAllBombCard[0])==0x40)
				{
					OutCardResult.cbCardCount = 2 ;
					OutCardResult.cbResultCard[0] = 0x4f ;
					OutCardResult.cbResultCard[1] = 0x4e ;
					return ;
				}
				else
				{
					//分析闲家牌
					BYTE cbUnderSideBankerAllBombCard[MAX_COUNT], cbUnderSideBankerAllBombCardCount ;
					GetAllBomCard( m_cbAllCardData[wUndersideUser], m_cbUserCardCount[wUndersideUser], cbUnderSideBankerAllBombCard, cbUnderSideBankerAllBombCardCount) ;

					BYTE cbUpSideBankerAllBombCard[MAX_COUNT], cbUpSideBankerAllBombCardCount ;
					GetAllBomCard( m_cbAllCardData[wUpsideUser], m_cbUserCardCount[wUpsideUser], cbUpSideBankerAllBombCard, cbUpSideBankerAllBombCardCount) ;

					if ( !CompareCard( cbTurnCardData, cbRemainCard, cbTurnCardCount, cbRemainCount) ||  ( cbUnderSideBankerAllBombCardCount <= 0 && cbUpSideBankerAllBombCardCount )|| 
						( GetCardLogicValue( cbAllBombCard[0] ) > GetCardLogicValue( cbUnderSideBankerAllBombCard[0] ) && 
						GetCardLogicValue( cbAllBombCard[0] ) > GetCardLogicValue( cbUpSideBankerAllBombCard[0] )) )
					{
						OutCardResult.cbCardCount = 4 ;
						CopyMemory(OutCardResult.cbResultCard, cbAllBombCard, 4) ;
						return ;
					}
				}
			}
		}
	}

	//判断可否出完
	BYTE cbSingleCardCount = MAX_COUNT+CT_MISSILE_CARD ;
	bool bFindBestCard = false ;
	for(BYTE cbCardType=CT_SINGLE; cbCardType<=CT_MISSILE_CARD; ++cbCardType)
		if(CardTypeResult[cbCardType].cbCardTypeCount>0)
		{
			for(LONG lIndex=0; lIndex<CardTypeResult[cbCardType].cbCardTypeCount; ++lIndex)
			{
				if(TestOutAllCard(m_wBankerUser, CardTypeResult[cbCardType].cbCardData[lIndex], CardTypeResult[cbCardType].cbEachHandCardCount[lIndex], false))
				{
					//计算单牌
					BYTE cbTmpSingleCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, CardTypeResult[cbCardType].cbCardData[lIndex], 
						CardTypeResult[cbCardType].cbEachHandCardCount[lIndex]) ;

					//结果判断
					if (cbTmpSingleCount >= MAX_COUNT) continue ;

					//炸弹优先级排后
					BYTE cbBombCardType = GetCardType(CardTypeResult[cbCardType].cbCardData[lIndex], CardTypeResult[cbCardType].cbEachHandCardCount[lIndex]) ;
					if (cbBombCardType == CT_BOMB_CARD) cbTmpSingleCount += 4 ;
					else if (cbBombCardType == CT_MISSILE_CARD) cbTmpSingleCount += 5 ;

					////改变权值
					//if (cbBombCardType != CT_ERROR) cbTmpSingleCount += cbBombCardType ;

					//不出炸弹
					//BYTE cbWantOutCardType = GetCardType(CardTypeResult[cbCardType].cbCardData[lIndex], CardTypeResult[cbCardType].cbEachHandCardCount[lIndex]) ;
					//if (CardTypeResult[cbOutCardType].cbCardTypeCount > 0 && cbOutCardType < CT_BOMB_CARD && cbWantOutCardType >= CT_BOMB_CARD) continue ;

					if (cbTmpSingleCount <= cbSingleCardCount)
					{
						//设置变量
						OutCardResult.cbCardCount=CardTypeResult[cbCardType].cbEachHandCardCount[lIndex];
						CopyMemory(OutCardResult.cbResultCard, CardTypeResult[cbCardType].cbCardData[lIndex], 
						CardTypeResult[cbCardType].cbEachHandCardCount[lIndex]*sizeof(BYTE));
						cbSingleCardCount = cbTmpSingleCount ;
						bFindBestCard = true ;
					}
				}
			}
		}
		//直接返回
		if (bFindBestCard) return ;

	//取出四个最小单牌
	BYTE cbMinSingleCardCount[4] ;
	cbMinSingleCardCount[0]=MAX_COUNT ;
	cbMinSingleCardCount[1]=MAX_COUNT ;
	cbMinSingleCardCount[2]=MAX_COUNT ;
	cbMinSingleCardCount[3]=MAX_COUNT ;
	BYTE cbIndex[4]={0} ;	
	BYTE cbMinSingleCountInFour=MAX_COUNT ;

	//可出扑克（这里已经过滤掉炸弹了）
	tagOutCardTypeResult const &CanOutCard = CardTypeResult[cbOutCardType] ;

	for(BYTE i=0; i<CanOutCard.cbCardTypeCount; ++i)
	{
		//最小单牌
		BYTE cbTmpCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount,CanOutCard.cbCardData[i], CanOutCard.cbEachHandCardCount[i]) ; 
		BYTE cbMaxValue=0 ; 
		BYTE Index = 0 ;

		//搜索cbMinSingleCardCount[4]的最大值
		for(BYTE j=0; j<4; ++j)
		{
			if(cbMinSingleCardCount[j]>=cbTmpCount)
			{
				cbMinSingleCardCount[j] = cbTmpCount ;
				cbIndex[j] = i ;
				break ;
			}
		}

	}

	for(BYTE i=0; i<4; ++i)
		if(cbMinSingleCountInFour>cbMinSingleCardCount[i]) cbMinSingleCountInFour = cbMinSingleCardCount[i] ;


	//原始单牌数
	BYTE cbOriginSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount,NULL,0) ;

	if(CanOutCard.cbCardTypeCount>0 && cbMinSingleCountInFour < MAX_COUNT)
	{
		BYTE cbMinLogicCardValue = GetCardLogicValue(0x4F)+1 ;
		bool bFindCard = false ;
		BYTE cbCanOutIndex=0 ;
		for(BYTE i=0; i<4; ++i)
		{
			BYTE Index = cbIndex[i] ;

			if((cbMinSingleCardCount[i]<cbOriginSingleCardCount+3)  &&  (cbMinSingleCardCount[i]<=cbMinSingleCountInFour || cbMinSingleCardCount[i]<=cbMinSingleCountInFour+1 && 
				CanOutCard.cbCardType >= CT_THREE_LINE_TAKE_ONE && CanOutCard.cbCardType <= CT_THREE_LINE_TAKE_ONE ) &&
				cbMinLogicCardValue>GetCardLogicValue(CanOutCard.cbCardData[Index][0]))
			{
				//针对大牌
				bool bNoLargeCard = true ;

				//当出牌玩家手上牌数大于4，而且出的是小于K的牌而且不是出牌手上的最大牌时，不能出2去打
				if(m_cbUserCardCount[wOutCardUser]>=4 && cbHandCardCount>=5  && CanOutCard.cbEachHandCardCount[Index]>=2 && 
					GetCardLogicValue(CanOutCard.cbCardData[Index][0])>=15 &&
					GetCardLogicValue(cbTurnCardData[0])<13 &&
					(wOutCardUser==wUndersideUser&&GetCardLogicValue(cbTurnCardData[0])<GetCardLogicValue(m_cbAllCardData[wUndersideUser][0]) || wOutCardUser==wUpsideUser&&GetCardLogicValue(cbTurnCardData[0])<GetCardLogicValue(m_cbAllCardData[wUpsideUser][0])) && 
					CanOutCard.cbEachHandCardCount[Index]!=cbHandCardCount)
					bNoLargeCard=false ;

				//搜索有没有大牌（针对飞机带翅膀后面的带牌）
				for(BYTE k=3; k<CanOutCard.cbEachHandCardCount[Index]; ++k)
				{
					if(GetCardLogicValue(CanOutCard.cbCardData[Index][k])>=15 && 
						CanOutCard.cbEachHandCardCount[Index]!=cbHandCardCount)
						bNoLargeCard = false ;
				}
				if(bNoLargeCard)
				{
					bFindCard = true ;
					cbCanOutIndex = Index ; 
					cbMinLogicCardValue = GetCardLogicValue(CanOutCard.cbCardData[Index][0]) ;
				}
			}
		}

		if(bFindCard)
		{
			//最大牌
			BYTE cbLargestLogicCard ;
			if(wOutCardUser==wUndersideUser) cbLargestLogicCard = GetCardLogicValue(m_cbAllCardData[wUndersideUser][0]) ;
			else if(wOutCardUser==wUpsideUser) cbLargestLogicCard = GetCardLogicValue(m_cbAllCardData[wUpsideUser][0]) ;
			bool bCanOut=true ;

			//王只压2
			if(GetCardLogicValue(cbTurnCardData[0])<cbLargestLogicCard)
			{
				if(GetCardColor(CanOutCard.cbCardData[cbCanOutIndex][0])==0x40 && GetCardLogicValue(cbTurnCardData[0])<=14 && cbHandCardCount>5) 								
				{
					bCanOut = false ;
				}
			}

			//双王判断
			if(GetCardLogicValue(CanOutCard.cbCardData[cbCanOutIndex][0])>=16 && cbHandCardCount>=2 && cbHandCardData[0]==0x4F && cbHandCardData[1]==0x4E)
			{
				bool bOutMissileCard = false ;
				//一手牌和一个炸弹
				BYTE cbRemainCardData[20], cbRemainCardCount=cbHandCardCount ;
				CopyMemory(cbRemainCardData, cbHandCardData, cbHandCardCount) ;
				RemoveCard(cbRemainCardData, 2, cbRemainCardData, cbRemainCardCount) ;
				cbRemainCardCount -= 2 ;
				if(CT_ERROR!=GetCardType(cbRemainCardData, cbRemainCardCount)) bOutMissileCard = true;

				//只剩少量牌
				BYTE cbRemainLargestCard = GetCardLogicValue(cbRemainCardData[0]) ;
				if(cbRemainCardCount<5 && cbRemainCardCount>0 && GetCardLogicValue(cbRemainCardData[0])>=14) bOutMissileCard = true;

				//炸后单牌数
				BYTE cbSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, CanOutCard.cbCardData[cbCanOutIndex],
					CanOutCard.cbEachHandCardCount[cbCanOutIndex]) ;
				if(cbSingleCardCount<=1 && GetCardLogicValue(cbRemainCardData[0])>=11) bOutMissileCard = true;

				//还有小牌
				if (GetCardLogicValue(cbRemainCardData[0]) <= 10 && CT_ERROR == GetCardType(cbRemainCardData, cbRemainCardCount) && 
					(GetCardLogicValue(m_cbAllCardData[(1+m_wBankerUser) % GAME_PLAYER][0]) > 10 || GetCardLogicValue(m_cbAllCardData[(2+m_wBankerUser) % GAME_PLAYER][0]) > 10))
					bOutMissileCard = false ;

				//火箭扑克
				if(bOutMissileCard)
				{
					//优先其他炸弹
					BYTE cbIndex = CardTypeResult[CT_BOMB_CARD].cbCardTypeCount - 1 ;

					OutCardResult.cbCardCount = CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[cbIndex] ;
					CopyMemory(OutCardResult.cbResultCard, CardTypeResult[CT_BOMB_CARD].cbCardData[cbIndex], OutCardResult.cbCardCount) ;
					return ;
				}
			}

			if(bCanOut)
			{
				//设置变量
				OutCardResult.cbCardCount=CanOutCard.cbEachHandCardCount[cbCanOutIndex];
				CopyMemory(OutCardResult.cbResultCard,CanOutCard.cbCardData[cbCanOutIndex],CanOutCard.cbEachHandCardCount[cbCanOutIndex]*sizeof(BYTE));

				return ;
			}
		}

		if(cbOutCardType==CT_SINGLE)
		{
			//闲家的最大牌
			BYTE cbLargestLogicCard ;
			if(wOutCardUser==wUndersideUser) cbLargestLogicCard = GetCardLogicValue(m_cbAllCardData[wUndersideUser][0]) ;
			else if(wOutCardUser==wUpsideUser) cbLargestLogicCard = GetCardLogicValue(m_cbAllCardData[wUpsideUser][0]) ;

			if(GetCardLogicValue(cbTurnCardData[0])==14 || 
				GetCardLogicValue(cbTurnCardData[0])>=cbLargestLogicCard || 
				(GetCardLogicValue(cbTurnCardData[0])<cbLargestLogicCard-1) ||
				(wOutCardUser==wUndersideUser&&m_cbUserCardCount[wUndersideUser]<=5 || wOutCardUser==wUpsideUser&&m_cbUserCardCount[wUpsideUser]<=5))
			{
				//取一张大于等于2而且要比闲家出的牌大的牌，
				BYTE cbIndex=MAX_COUNT ;
				for(BYTE i=0; i<cbHandCardCount; ++i)
					if(GetCardLogicValue(cbHandCardData[i])>GetCardLogicValue(cbTurnCardData[0]) &&
						GetCardLogicValue(cbHandCardData[i])>=15)
					{
						cbIndex = i ;
					}
					if(cbIndex!=MAX_COUNT)
					{
						//设置变量
						OutCardResult.cbCardCount=1;
						OutCardResult.cbResultCard[0] = cbHandCardData[cbIndex] ;

						return ;
					}
			}
		}


		BYTE cbMinSingleCount=MAX_COUNT ;
		BYTE Index=0 ;
		for(BYTE i=0; i<CardTypeResult[cbOutCardType].cbCardTypeCount; ++i)
		{
			BYTE cbTmpCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, CardTypeResult[cbOutCardType].cbCardData[i], CardTypeResult[cbOutCardType].cbEachHandCardCount[i]) ;
			if(cbMinSingleCount>=cbTmpCount)
			{
				cbMinSingleCount = cbTmpCount ;
				Index = i ;
			}
		}
		//设置变量
		OutCardResult.cbCardCount=CardTypeResult[cbOutCardType].cbEachHandCardCount[Index];
		CopyMemory(OutCardResult.cbResultCard, CardTypeResult[cbOutCardType].cbCardData[Index], OutCardResult.cbCardCount) ;

		return ;
	}

	//还要考虑炸弹
	if(CardTypeResult[CT_BOMB_CARD].cbCardTypeCount>0)
	{
		tagOutCardTypeResult const &BomCard = CardTypeResult[CT_BOMB_CARD] ;
		BYTE cbMinLogicValue = GetCardLogicValue(BomCard.cbCardData[0][0]) ;
		BYTE Index = 0 ;
		for(BYTE i=0; i<BomCard.cbCardTypeCount; ++i)
		{
			if(cbMinLogicValue>GetCardLogicValue(BomCard.cbCardData[i][0]))
			{
				cbMinLogicValue = GetCardLogicValue(BomCard.cbCardData[i][0]) ;
				Index = i ;
			}
		}

		bool bOutBomb=false ;

		//另一闲家
		//WORD wOtherUser=INVALID_CHAIR ;
		//for(WORD wUser=0; wUser<GAME_PLAYER; ++wUser)
		//	if(wUser!=wOutCardUser && wUser!=m_wBankerUser) wOtherUser=wOtherUser ;
		
		//一手牌和一个炸弹
		BYTE cbRemainCardData[20], cbRemainCardCount=cbHandCardCount ;
		CopyMemory(cbRemainCardData, cbHandCardData, cbHandCardCount) ;
		RemoveCard(BomCard.cbCardData[Index], BomCard.cbEachHandCardCount[Index], cbRemainCardData, cbRemainCardCount) ;
		cbRemainCardCount -= BomCard.cbEachHandCardCount[Index] ;
		if(CT_ERROR!=GetCardType(cbRemainCardData, cbRemainCardCount)) bOutBomb = true ;

		//炸后单牌数
		BYTE cbSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, BomCard.cbCardData[Index],BomCard.cbEachHandCardCount[Index]) ;
		if(cbSingleCardCount==0 && GetCardLogicValue(cbRemainCardData[0]) > 
			GetCardLogicValue(m_cbAllCardData[wUpsideUser == wOutCardUser ? wUndersideUser : wUpsideUser][0])) bOutBomb = true ;

		//只剩一手
		BYTE cbRemainCardType = GetCardType(m_cbAllCardData[wOutCardUser], m_cbUserCardCount[wOutCardUser]) ;
		if(cbRemainCardType>CT_ERROR && cbRemainCardType<CT_FOUR_LINE_TAKE_ONE && GetCardLogicValue(m_cbAllCardData[wOutCardUser][0])<15 && 
			cbSingleCardCount < 2 && (GetCardLogicValue(cbRemainCardData[0]) >= GetCardLogicValue(m_cbAllCardData[wUndersideUser][0]) && 
			GetCardLogicValue(cbRemainCardData[0]) >= GetCardLogicValue(m_cbAllCardData[wUpsideUser][0]))) bOutBomb = true ;

		//反春天
		if (cbRemainCardType != CT_ERROR && m_lBankerOutCardCount == 1) bOutBomb = true ;

		//只剩少量牌
		BYTE cbRemainLargestCard = GetCardLogicValue(cbRemainCardData[0]) ;
		if(cbRemainCardCount<5 && cbRemainCardCount>0 && (cbRemainLargestCard!=GetCardLogicValue(BomCard.cbCardData[Index][0])) && 
			cbRemainLargestCard>GetCardLogicValue(m_cbAllCardData[wOutCardUser][0]) && cbRemainLargestCard > 14) bOutBomb = true ;

		//分析扑克
		tagAnalyseResult AnalyseResult ;
		AnalysebCardData(cbRemainCardData, cbRemainCardCount, AnalyseResult) ;

		if (m_cbUserCardCount[wOutCardUser] == 1 && (AnalyseResult.cbDoubleCount * 2 + AnalyseResult.cbThreeCount * 3 + AnalyseResult.cbFourCount * 4 + 1 >= cbRemainCardCount)) bOutBomb = true ;


		//设置变量
		if(bOutBomb)
		{
			OutCardResult.cbCardCount=BomCard.cbEachHandCardCount[Index];
			CopyMemory(OutCardResult.cbResultCard,BomCard.cbCardData[Index],BomCard.cbEachHandCardCount[Index]*sizeof(BYTE));
		}
		return ;
	}

	return ;
}

//地主上家（先出牌）
void CGameLogic::UpsideOfBankerOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wMeChairID, tagOutCardResult & OutCardResult)
{
	//初始变量
	ZeroMemory(&OutCardResult, sizeof(OutCardResult)) ;

	//地主只有一张，从大到小出牌
	if (m_cbUserCardCount[m_wBankerUser] == 1)
	{
		BYTE cbSingleCardData[MAX_COUNT] ;
		BYTE cbSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, NULL, 0, cbSingleCardData) ;

		//只剩单牌
		if (cbSingleCardCount == cbHandCardCount) 
		{
			OutCardResult.cbCardCount = 1 ;
			OutCardResult.cbResultCard[0] = cbHandCardData[0] ;

			return ;
		}
	}

	//零下标没用
	tagOutCardTypeResult CardTypeResult[12+1] ;
	ZeroMemory(CardTypeResult, sizeof(CardTypeResult)) ;

	BYTE cbLineCard[MAX_COUNT] ;
	BYTE cbThreeLineCard[MAX_COUNT] ;
	BYTE cbDoubleLineCard[MAX_COUNT] ;
	BYTE cbLineCardCount;
	BYTE cbThreeLineCardCount ;
	BYTE cbDoubleLineCount ;
	GetAllLineCard(cbHandCardData, cbHandCardCount, cbLineCard, cbLineCardCount) ;
	GetAllThreeCard(cbHandCardData, cbHandCardCount, cbThreeLineCard, cbThreeLineCardCount) ;
	GetAllDoubleCard(cbHandCardData, cbHandCardCount, cbDoubleLineCard, cbDoubleLineCount) ;

	//判断可否出完
	BYTE cbSingleCardCount = MAX_COUNT+CT_MISSILE_CARD ;
	bool bFindBestCard = false ;
	AnalyseOutCardType(cbHandCardData, cbHandCardCount, CardTypeResult) ;
	for(BYTE cbCardType=CT_SINGLE; cbCardType<=CT_MISSILE_CARD; ++cbCardType)
		if(CardTypeResult[cbCardType].cbCardTypeCount>0)
		{
			for(LONG lIndex=0; lIndex<CardTypeResult[cbCardType].cbCardTypeCount; ++lIndex)
			{
				WORD wMeChairID = (m_wBankerUser+2)%GAME_PLAYER ;
				
				if(TestOutAllCard(wMeChairID, CardTypeResult[cbCardType].cbCardData[lIndex], CardTypeResult[cbCardType].cbEachHandCardCount[lIndex], true))
				{
					//计算单牌
					BYTE cbTmpSingleCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, CardTypeResult[cbCardType].cbCardData[lIndex], 
						CardTypeResult[cbCardType].cbEachHandCardCount[lIndex]) ;

					//结果判断
					if (cbTmpSingleCount >= MAX_COUNT) continue ;

					//炸弹优先级排后
					BYTE cbBombCardType = GetCardType(CardTypeResult[cbCardType].cbCardData[lIndex], CardTypeResult[cbCardType].cbEachHandCardCount[lIndex]) ;
					if (cbBombCardType == CT_BOMB_CARD) cbTmpSingleCount += 4 ;
					else if (cbBombCardType == CT_MISSILE_CARD) cbTmpSingleCount += 5 ;
					else if ( 15 == GetCardLogicValue( CardTypeResult[ cbCardType ].cbCardData[ lIndex ][ 0 ] ) ) cbTmpSingleCount += 2;
					else if ( 15 < GetCardLogicValue( CardTypeResult[ cbCardType ].cbCardData[ lIndex ][ 0 ] ) ) cbTmpSingleCount += 3;


					////改变权值
					//if (cbBombCardType != CT_ERROR) cbTmpSingleCount += cbBombCardType ;

					if(cbTmpSingleCount <= cbSingleCardCount)
					{
						//设置变量
						OutCardResult.cbCardCount=CardTypeResult[cbCardType].cbEachHandCardCount[lIndex];
						CopyMemory(OutCardResult.cbResultCard, CardTypeResult[cbCardType].cbCardData[lIndex], 
						CardTypeResult[cbCardType].cbEachHandCardCount[lIndex]*sizeof(BYTE));
						cbSingleCardCount = cbTmpSingleCount ;

						bFindBestCard = true ;
					}
				}
			}
		}
		//直接返回
		if (bFindBestCard) return ;

		//对王和两单
		if ( cbHandCardCount == 4 && GetCardLogicValue(cbHandCardData[1]) == 16 && m_cbUserCardCount[m_wBankerUser] == 1 && 
			GetCardLogicValue(cbHandCardData[2]) < GetCardLogicValue(m_cbAllCardData[m_wBankerUser][0]))
		{
			OutCardResult.cbCardCount = 1 ;
			OutCardResult.cbResultCard[0] = cbHandCardData[2] ;
			return ;
		}

		//四带牌型判断
		if ( AnalyseFourCardType(cbHandCardData, cbHandCardCount, m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser], OutCardResult ) )
		{
			return ;
		}

	//如果有顺牌和单只或一对，而且单只或对比地主的小，则先出顺
	{
		if(cbLineCardCount+1==cbHandCardCount && CT_SINGLE==GetCardType(cbLineCard, cbLineCardCount))
		{
			OutCardResult.cbCardCount = cbLineCardCount ;
			CopyMemory(OutCardResult.cbResultCard, cbLineCard, cbLineCardCount) ;
		}
		else if(cbThreeLineCardCount+1==cbHandCardCount && CT_THREE_LINE==GetCardType(cbThreeLineCard, cbThreeLineCardCount))
		{
			OutCardResult.cbCardCount = cbThreeLineCardCount ;
			CopyMemory(OutCardResult.cbResultCard, cbThreeLineCard, cbThreeLineCardCount) ;
		}
		else if(cbDoubleLineCount+1==cbHandCardCount && CT_DOUBLE_LINE==GetCardType(cbDoubleLineCard, cbDoubleLineCount))
		{
			OutCardResult.cbCardCount = cbDoubleLineCount ;
			CopyMemory(OutCardResult.cbResultCard, cbDoubleLineCard, cbDoubleLineCount) ;
		}
		//双王炸弹和一手
		else if(cbHandCardCount>2 && cbHandCardData[0]==0x4f && cbHandCardData[1]==0x4e && CT_ERROR!=GetCardType(cbHandCardData+2, cbHandCardCount-2) &&
			CT_FOUR_LINE_TAKE_ONE != GetCardType(cbHandCardData+2, cbHandCardCount-2) && CT_FOUR_LINE_TAKE_TWO != GetCardType(cbHandCardData+2, cbHandCardCount-2))
		{
			OutCardResult.cbCardCount = 2 ;
			OutCardResult.cbResultCard[0] = 0x4f ;
			OutCardResult.cbResultCard[1] = 0x4e ;
		}

		if(OutCardResult.cbCardCount>0)
		{
			return ;
		}
	}
	//对王加一只
	if(cbHandCardCount==3 && GetCardColor(cbHandCardData[0])==0x40 && GetCardColor(cbHandCardData[1])==0x40)
	{
		OutCardResult.cbCardCount = 2 ;
		OutCardResult.cbResultCard[0] = 0x4f ;
		OutCardResult.cbResultCard[1] = 0x4e ;
		return ;
	}
	//对王
	else if(cbHandCardCount==2 && GetCardColor(cbHandCardData[0])==0x40 && GetCardColor(cbHandCardData[1])==0x40)
	{
		OutCardResult.cbCardCount = 2 ;
		OutCardResult.cbResultCard[0] = 0x4f ;
		OutCardResult.cbResultCard[1] = 0x4e ;
		return ;
	}
	//只剩一手牌
	else if(CT_ERROR!=GetCardType(cbHandCardData, cbHandCardCount) && CT_FOUR_LINE_TAKE_ONE!=GetCardType(cbHandCardData, cbHandCardCount) && CT_FOUR_LINE_TAKE_TWO!=GetCardType(cbHandCardData, cbHandCardCount))
	{
		OutCardResult.cbCardCount = cbHandCardCount ;
		CopyMemory(OutCardResult.cbResultCard, cbHandCardData, cbHandCardCount) ;
		return ;
	}

	//只剩一张和一手
	if(cbHandCardCount>=2)
	{
		//地主扑克
		tagOutCardTypeResult BankerCanOutCardType1[13] ;
		ZeroMemory(BankerCanOutCardType1, sizeof(BankerCanOutCardType1)) ;
		tagOutCardTypeResult BankerCanOutCardType2[13] ;
		ZeroMemory(BankerCanOutCardType2, sizeof(BankerCanOutCardType2)) ;

		BYTE cbFirstHandCardType = GetCardType(cbHandCardData, cbHandCardCount-1) ;
		BYTE cbSecondHandCardType = GetCardType(cbHandCardData+1, cbHandCardCount-1) ;

		//是否有炸
		BYTE cbAllBombCardData[MAX_COUNT], cbAllBombCount=0 ;
		GetAllBomCard(cbHandCardData, cbHandCardCount, cbAllBombCardData, cbAllBombCount) ;

		//没有炸弹
		if (cbAllBombCount <= 0 && cbFirstHandCardType!=CT_THREE_LINE_TAKE_ONE)
		{
		//地主可以出的牌
		if(cbFirstHandCardType!=CT_ERROR)
			AnalyseOutCardType(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser], cbHandCardData, cbHandCardCount-1, BankerCanOutCardType1) ;
		if(cbSecondHandCardType!=CT_ERROR)
			AnalyseOutCardType(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser], cbHandCardData+1, cbHandCardCount-1, BankerCanOutCardType2) ;

		if(cbSecondHandCardType!=CT_ERROR && cbSecondHandCardType!=CT_FOUR_LINE_TAKE_ONE && cbSecondHandCardType!= CT_FOUR_LINE_TAKE_TWO && 
			BankerCanOutCardType2[cbSecondHandCardType].cbCardTypeCount==0 && BankerCanOutCardType2[CT_BOMB_CARD].cbCardTypeCount==0)
		{
			OutCardResult.cbCardCount = cbHandCardCount-1 ;
			CopyMemory(OutCardResult.cbResultCard, cbHandCardData+1, cbHandCardCount-1) ;
			return ;
		}

		if(cbFirstHandCardType!=CT_ERROR && cbFirstHandCardType!=CT_FOUR_LINE_TAKE_ONE && cbFirstHandCardType!= CT_FOUR_LINE_TAKE_TWO &&
			BankerCanOutCardType1[cbFirstHandCardType].cbCardTypeCount==0 && BankerCanOutCardType2[CT_BOMB_CARD].cbCardTypeCount==0)
		{
			OutCardResult.cbCardCount = cbHandCardCount-1 ;
			CopyMemory(OutCardResult.cbResultCard, cbHandCardData, cbHandCardCount-1) ;
			return ;
		}

		if(GetCardLogicValue(cbHandCardData[0])>=GetCardLogicValue(m_cbAllCardData[m_wBankerUser][0]) &&
			CT_ERROR!=cbSecondHandCardType && cbSecondHandCardType!=CT_FOUR_LINE_TAKE_ONE && cbSecondHandCardType!= CT_FOUR_LINE_TAKE_TWO &&
			BankerCanOutCardType2[CT_BOMB_CARD].cbCardTypeCount==0)
		{
			OutCardResult.cbCardCount = 1 ;
			OutCardResult.cbResultCard[0] = cbHandCardData[0] ;
			return ;
		}

		if(CT_ERROR!=cbSecondHandCardType && cbSecondHandCardType!=CT_FOUR_LINE_TAKE_ONE && cbSecondHandCardType!= CT_FOUR_LINE_TAKE_TWO &&
			BankerCanOutCardType2[CT_BOMB_CARD].cbCardTypeCount==0)
		{
			OutCardResult.cbCardCount = cbHandCardCount-1 ;
			CopyMemory(OutCardResult.cbResultCard, cbHandCardData+1, cbHandCardCount-1) ;
			return ;
		}
		}
		//还有炸弹
		else
		{
			//除去炸后的牌
			BYTE cbRemainCard[MAX_COUNT], cbRemainCount=0 ;
			CopyMemory(cbRemainCard, cbHandCardData, cbHandCardCount) ;
			cbRemainCount = cbHandCardCount ;
			RemoveCard(cbAllBombCardData, cbAllBombCount, cbRemainCard, cbRemainCount) ;
			cbRemainCount -= cbAllBombCount ;
			if (GetCardType(cbRemainCard, cbRemainCount) != CT_ERROR)
			{
				OutCardResult.cbCardCount = cbRemainCount ;
				CopyMemory(OutCardResult.cbResultCard, cbRemainCard, cbRemainCount) ;
				return ;
			}
		}
	}

	//对牌接牌判断
	WORD wFriendID = (m_wBankerUser + 1) % GAME_PLAYER ;
	if (1 == m_cbUserCardCount[m_wBankerUser] && cbHandCardCount >= 2 && m_cbUserCardCount[wFriendID] >= 2)
	{
		tagAnalyseResult MeAnalyseResult ;
		ZeroMemory(&MeAnalyseResult, sizeof(MeAnalyseResult)) ;

		//分析扑克
		AnalysebCardData(cbHandCardData, cbHandCardCount, MeAnalyseResult) ;

		tagAnalyseResult FriendAnalyseResult ;
		ZeroMemory(&FriendAnalyseResult, sizeof(FriendAnalyseResult)) ;

		//分析扑克
		AnalysebCardData(m_cbAllCardData[wFriendID], m_cbUserCardCount[wFriendID], FriendAnalyseResult) ;

		//对牌判断
		if ((m_cbUserCardCount[wFriendID] == (FriendAnalyseResult.cbDoubleCount * 2 + FriendAnalyseResult.cbThreeCount * 3 + FriendAnalyseResult.cbFourCount * 4) || 
			m_cbUserCardCount[wFriendID] == (FriendAnalyseResult.cbDoubleCount * 2 + FriendAnalyseResult.cbThreeCount * 3 + FriendAnalyseResult.cbFourCount * 4 + 1)) &&
			MeAnalyseResult.cbDoubleCount > 0 && FriendAnalyseResult.cbDoubleCount > 0)
		{
			//最小对子
			BYTE cbMeLeastDoubleCardLogic = GetCardLogicValue(MeAnalyseResult.cbDoubleCardData[MeAnalyseResult.cbDoubleCount*2-2]) ;
			//最大对子
			BYTE cbFriendLargestDoublecardLogic = GetCardLogicValue(FriendAnalyseResult.cbDoubleCardData[0]) ;

			//出对判断
			if (cbMeLeastDoubleCardLogic < 14 && cbMeLeastDoubleCardLogic < cbFriendLargestDoublecardLogic)
			{
				OutCardResult.cbCardCount = 2 ;
				OutCardResult.cbResultCard[0] = MeAnalyseResult.cbDoubleCardData[MeAnalyseResult.cbDoubleCount*2-2] ;
				OutCardResult.cbResultCard[1] = MeAnalyseResult.cbDoubleCardData[MeAnalyseResult.cbDoubleCount*2-1] ;
				return ;
			}
          			
		}
	}

	//下家为地主，而且地主扑克少于5张
	//	if(m_cbUserCardCount[m_wBankerUser]<=5)
	{
		//分析扑克
		tagOutCardTypeResult MeOutCardTypeResult[13] ;
		ZeroMemory(MeOutCardTypeResult, sizeof(MeOutCardTypeResult)) ;
		AnalyseOutCardType(cbHandCardData, cbHandCardCount, MeOutCardTypeResult) ;

		//对家扑克
		WORD wFriendID ;
		for(WORD wChairID=0; wChairID<GAME_PLAYER; ++wChairID) 
			if(wChairID!=m_wBankerUser && wMeChairID!=wChairID) wFriendID = wChairID ;

		//计算单牌
		BYTE cbMinSingleCardCount[4] ;
		cbMinSingleCardCount[0]=MAX_COUNT ;
		cbMinSingleCardCount[1]=MAX_COUNT ;
		cbMinSingleCardCount[2]=MAX_COUNT ;
		cbMinSingleCardCount[3]=MAX_COUNT ;
		BYTE cbIndex[4]={0} ;
		BYTE cbOutcardType[4]={CT_ERROR} ;
		BYTE cbMinValue=MAX_COUNT ; 
		BYTE cbMinSingleCountInFour=MAX_COUNT ;
		BYTE cbMinCardType=CT_ERROR ;
		BYTE cbMinIndex=0 ;

		//分析地主对牌
		BYTE cbBankerDoubleCardData[MAX_COUNT], cbBankerDoubleCardCount ;
		GetAllDoubleCard(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser], cbBankerDoubleCardData, cbBankerDoubleCardCount) ;

		//除炸弹外的牌
		for(BYTE cbCardType=CT_DOUBLE; cbCardType<CT_BOMB_CARD; ++cbCardType)
		{
			tagOutCardTypeResult const &tmpCardResult = MeOutCardTypeResult[cbCardType] ;

			//相同牌型，相同长度，单连，对连等相同牌型可能长度不一样
			BYTE cbThisHandCardCount = MAX_COUNT ;

			//地主扑克
			tagOutCardTypeResult BankerCanOutCard[13] ;
			ZeroMemory(BankerCanOutCard, sizeof(BankerCanOutCard)) ;

			tagOutCardTypeResult FriendOutCardTypeResult[13] ;
			ZeroMemory(FriendOutCardTypeResult, sizeof(FriendOutCardTypeResult)) ;

			for(BYTE i=0; i<tmpCardResult.cbCardTypeCount; ++i)
			{
				//拆三条判断
				if ( cbCardType == CT_DOUBLE )
				{
					tagAnalyseResult AnalyseResult ;
					ZeroMemory( &AnalyseResult,  sizeof( AnalyseResult ) ) ;
					AnalysebCardData( cbHandCardData, cbHandCardCount, AnalyseResult ) ;
					if ( AnalyseResult.cbSignedCount + AnalyseResult.cbThreeCount * 3 == cbHandCardCount )
					{
						bool bContinue = false ;
						for ( BYTE cbThreeIndex = 0; cbThreeIndex < AnalyseResult.cbThreeCount; ++cbThreeIndex )
							if ( GetCardValue(  tmpCardResult.cbCardData[i][0] ) == GetCardValue( AnalyseResult.cbThreeCardData[3 * cbThreeIndex] ) )
							{
								bContinue = true ;
								break ;
							}
						if ( bContinue ) continue ;
					}
				}

				BYTE cbTmpCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, tmpCardResult.cbCardData[i], tmpCardResult.cbEachHandCardCount[i]) ;

				//拦截对牌
				if (cbCardType == CT_DOUBLE && cbBankerDoubleCardCount > 0 && GetCardLogicValue(cbBankerDoubleCardData[cbBankerDoubleCardCount-1]) < 10 &&
					GetCardLogicValue(tmpCardResult.cbCardData[i][0]) < GetCardLogicValue(cbBankerDoubleCardData[cbBankerDoubleCardCount-1]) &&
					GetCardLogicValue(tmpCardResult.cbCardData[0][0]) >= 10 && GetCardLogicValue(tmpCardResult.cbCardData[0][0]) < 14) continue ;

				//重新分析
				if(tmpCardResult.cbEachHandCardCount[i]!=cbThisHandCardCount)
				{
					cbThisHandCardCount = tmpCardResult.cbEachHandCardCount[i] ;
					AnalyseOutCardType(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser], 
						tmpCardResult.cbCardData[i], tmpCardResult.cbEachHandCardCount[i] ,BankerCanOutCard) ;
					AnalyseOutCardType(m_cbAllCardData[wFriendID], m_cbUserCardCount[wFriendID],
						tmpCardResult.cbCardData[i], tmpCardResult.cbEachHandCardCount[i] ,FriendOutCardTypeResult) ;
				}

				BYTE cbMaxValue=0 ; 
				BYTE Index = 0 ;

				//针对顺子，三条的大牌
				BYTE cbCurrentCardType = GetCardType(tmpCardResult.cbCardData[i], cbThisHandCardCount) ;
				if (cbThisHandCardCount != cbHandCardCount && cbCurrentCardType >= CT_SINGLE_LINE && cbCurrentCardType <= CT_THREE_LINE_TAKE_ONE &&
					( GetCardLogicValue(tmpCardResult.cbCardData[i][cbThisHandCardCount-1]) > GetCardLogicValue(cbHandCardData[cbHandCardCount-2]) || 
						GetCardLogicValue(tmpCardResult.cbCardData[i][0]) >= 11 ))
				{
					BYTE cbRemainCardData[MAX_COUNT], cbRemainCardCount ;
					CopyMemory(cbRemainCardData, cbHandCardData, cbHandCardCount) ;
					cbRemainCardCount = cbHandCardCount ;

					//移除扑克
					RemoveCard(tmpCardResult.cbCardData[i], cbThisHandCardCount, cbRemainCardData, cbRemainCardCount) ;
					cbRemainCardCount -= cbThisHandCardCount ;

					//最大扑克
					BYTE cbCurrentLargestLogicCard = GetCardLogicValue(tmpCardResult.cbCardData[i][0]) ;

					if (GetCardType(cbRemainCardData, cbRemainCardCount) == CT_ERROR && (cbCurrentCardType >= CT_THREE_LINE_TAKE_ONE && 
						cbCurrentCardType <= CT_THREE_LINE_TAKE_ONE && cbCurrentLargestLogicCard >= 11 && cbThisHandCardCount <=5 || 
						cbCurrentCardType == CT_SINGLE_LINE && cbThisHandCardCount <= 6 && cbCurrentLargestLogicCard >= 12 || 
						cbCurrentCardType >= CT_DOUBLE_LINE && cbCurrentCardType <= CT_THREE_LINE && cbCurrentLargestLogicCard >= 12 && cbThisHandCardCount <= 8))
					{
						//暂时不出
						if ( cbCurrentCardType >= CT_SINGLE_LINE && cbCurrentCardType <= CT_THREE_LINE &&
							GetCardLogicValue(tmpCardResult.cbCardData[i][cbThisHandCardCount - 1]) > GetCardLogicValue(cbHandCardData[cbHandCardCount-3]) )
							continue ;

						if ( cbCurrentCardType >= CT_THREE_LINE_TAKE_ONE && cbCurrentCardType <= CT_THREE_LINE_TAKE_ONE &&
							GetCardLogicValue(tmpCardResult.cbCardData[i][0]) > GetCardLogicValue(cbHandCardData[cbHandCardCount-3]) )
							continue ;
					}
				}

				//地主可以压牌，而且队友不可以压地主
				if((BankerCanOutCard[cbCardType].cbCardTypeCount>0&&FriendOutCardTypeResult[cbCardType].cbCardTypeCount==0) || (BankerCanOutCard[cbCardType].cbCardTypeCount>0 && FriendOutCardTypeResult[cbCardType].cbCardTypeCount>0 &&
					GetCardLogicValue(FriendOutCardTypeResult[cbCardType].cbCardData[0][0])<=GetCardLogicValue(BankerCanOutCard[cbCardType].cbCardData[0][0])))
				{
					//地主跑掉
					if( BankerCanOutCard[cbCardType].cbEachHandCardCount[0] > 0 && m_cbUserCardCount[m_wBankerUser]<=BankerCanOutCard[cbCardType].cbEachHandCardCount[0]+1)
						continue ;

					//自己不可以再拿回牌权
					if(GetCardLogicValue(tmpCardResult.cbCardData[0][0]) < GetCardLogicValue(BankerCanOutCard[cbCardType].cbCardData[0][0]) &&
						BankerCanOutCard[cbCardType].cbCardTypeCount > 0) 
						continue ;
				}
				//是否有大牌
				if(tmpCardResult.cbEachHandCardCount[i] != cbHandCardCount)
				{
					bool bHaveLargeCard=false ;
					for(BYTE j=0; j<tmpCardResult.cbEachHandCardCount[i]; ++j)
						if(GetCardLogicValue(tmpCardResult.cbCardData[i][j])>=15) bHaveLargeCard=true ;
					if(cbCardType!=CT_SINGLE_LINE && cbCardType!=CT_DOUBLE_LINE  && GetCardLogicValue(tmpCardResult.cbCardData[i][0])==14) bHaveLargeCard=true ; 

					if(bHaveLargeCard) continue ;
				}

				//地主是否可以走掉，这里都没有考虑炸弹
				if(tmpCardResult.cbEachHandCardCount[i]==m_cbUserCardCount[m_wBankerUser] && cbCardType==GetCardType(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser]) &&
					GetCardLogicValue(m_cbAllCardData[m_wBankerUser][0])>GetCardLogicValue(tmpCardResult.cbCardData[i][0])) continue ;

				//搜索cbMinSingleCardCount[4]的最大值
				for(BYTE j=0; j<4; ++j)
				{
					if(cbMinSingleCardCount[j]>=cbTmpCount)
					{
						cbMinSingleCardCount[j] = cbTmpCount ;
						cbIndex[j] = i ;
						cbOutcardType[j] = cbCardType ;
						break ;
					}
				}

				//保存最小值
				if(cbMinSingleCountInFour>=cbTmpCount)
				{
					//最小牌型
					cbMinCardType = cbCardType ;
					//最小牌型中的最小单牌
					cbMinSingleCountInFour=cbTmpCount ;						
					//最小牌型中的最小牌
					cbMinIndex=i ;
				}
			}
		}

		if(cbMinSingleCountInFour>=AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, NULL, 0)+3 && 
			m_cbUserCardCount[m_wBankerUser]>4)
			cbMinSingleCountInFour=MAX_COUNT ;

		if(cbMinSingleCountInFour!=MAX_COUNT)
		{
			BYTE Index = cbMinIndex ;

			//选择最小牌
			for(BYTE i=0; i<4; ++i)
			{
				if(cbOutcardType[i]==cbMinCardType && cbMinSingleCardCount[i]<=cbMinSingleCountInFour &&
					GetCardLogicValue(MeOutCardTypeResult[cbMinCardType].cbCardData[cbIndex[i]][0])<GetCardLogicValue(MeOutCardTypeResult[cbMinCardType].cbCardData[Index][0]))
					Index = cbIndex[i] ;
			}

			//对王加一只
			if(cbHandCardCount==3 && GetCardColor(cbHandCardData[0])==0x40 && GetCardColor(cbHandCardData[1])==0x40)
			{
				OutCardResult.cbCardCount = 2 ;
				OutCardResult.cbResultCard[0] = 0x4f ;
				OutCardResult.cbResultCard[1] = 0x4e ;
				return ;
			}
			//对王
			else if(cbHandCardCount==2 && GetCardColor(cbHandCardData[0])==0x40 && GetCardColor(cbHandCardData[1])==0x40)
			{
				OutCardResult.cbCardCount = 2 ;
				OutCardResult.cbResultCard[0] = 0x4f ;
				OutCardResult.cbResultCard[1] = 0x4e ;
				return ;
			}
			else
			{
				//设置变量
				OutCardResult.cbCardCount=MeOutCardTypeResult[cbMinCardType].cbEachHandCardCount[Index];
				CopyMemory(OutCardResult.cbResultCard,MeOutCardTypeResult[cbMinCardType].cbCardData[Index],MeOutCardTypeResult[cbMinCardType].cbEachHandCardCount[Index]*sizeof(BYTE));
				return ;
			}

			ASSERT(OutCardResult.cbCardCount>0) ;

			return ;
		}

		//如果地主扑克少于5，还没有找到适合的牌则从大出到小
		if(OutCardResult.cbCardCount<=0 && m_cbUserCardCount[m_wBankerUser]<=5)
		{
			//只有一张牌时不能放地主走
			if(m_cbUserCardCount[m_wBankerUser]==1 && MeOutCardTypeResult[CT_SINGLE].cbCardTypeCount>0)
			{
				BYTE Index=MAX_COUNT ;
				for(BYTE i=0; i<MeOutCardTypeResult[CT_SINGLE].cbCardTypeCount; ++i)
				{
					if(GetCardLogicValue(MeOutCardTypeResult[CT_SINGLE].cbCardData[i][0])>=GetCardLogicValue(m_cbAllCardData[m_wBankerUser][0]))
					{
						Index=i ;
					}
					else break ;
				}

				if(MAX_COUNT!=Index)
				{
					OutCardResult.cbCardCount = MeOutCardTypeResult[CT_SINGLE].cbEachHandCardCount[Index] ;
					CopyMemory(OutCardResult.cbResultCard, MeOutCardTypeResult[CT_SINGLE].cbCardData[Index], OutCardResult.cbCardCount) ;
					return ;
				}
			}
		}
	}

	BYTE cbFirstCard=0 ;
	//过滤王和2
	for(BYTE i=0; i<cbHandCardCount; ++i) 
		if(GetCardLogicValue(cbHandCardData[i])<15)
		{
			cbFirstCard = i ;
			break ;
		}

		if(cbFirstCard<cbHandCardCount-1)
			AnalyseOutCardType(cbHandCardData+cbFirstCard, cbHandCardCount-cbFirstCard, CardTypeResult) ;
		else
			AnalyseOutCardType(cbHandCardData, cbHandCardCount, CardTypeResult) ;

		//计算单牌
		BYTE cbMinSingleCardCount[4] ;
		cbMinSingleCardCount[0]=MAX_COUNT ;
		cbMinSingleCardCount[1]=MAX_COUNT ;
		cbMinSingleCardCount[2]=MAX_COUNT ;
		cbMinSingleCardCount[3]=MAX_COUNT ;
		BYTE cbIndex[4]={0} ;
		BYTE cbOutcardType[4]={CT_ERROR} ;
		BYTE cbMinValue=MAX_COUNT ; 
		BYTE cbMinSingleCountInFour=MAX_COUNT ;
		BYTE cbMinCardType=CT_ERROR ;
		BYTE cbMinIndex=0 ;

		//分析地主单牌
		BYTE cbBankerSingleCardData[MAX_COUNT] ;
		BYTE cbBankerSingleCardCount=AnalyseSinleCardCount(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser], NULL, 0, cbBankerSingleCardData) ;
		BYTE cbBankerSingleCardLogic = 0 ;
		if(cbBankerSingleCardCount>=2 && GetCardLogicValue(cbBankerSingleCardData[cbBankerSingleCardCount-2])<=10) cbBankerSingleCardLogic = GetCardLogicValue(cbBankerSingleCardData[cbBankerSingleCardCount-2]) ;
		else if(cbBankerSingleCardCount>=2 && GetCardLogicValue(cbBankerSingleCardData[cbBankerSingleCardCount-1])<=10) cbBankerSingleCardLogic = GetCardLogicValue(cbBankerSingleCardData[cbBankerSingleCardCount-1]) ;
		else if(cbBankerSingleCardCount>0 && GetCardLogicValue(cbBankerSingleCardData[0])<=10) cbBankerSingleCardLogic = GetCardLogicValue(cbBankerSingleCardData[0]) ;

		//分析地主对牌
		BYTE cbBankerDoubleCardData[MAX_COUNT], cbBankerDoubleCardCount ;
		GetAllDoubleCard(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser], cbBankerDoubleCardData, cbBankerDoubleCardCount) ;

		//除炸弹外的牌
		for(BYTE cbCardType=CT_SINGLE; cbCardType<CT_BOMB_CARD; ++cbCardType)
		{
			tagOutCardTypeResult const &tmpCardResult = CardTypeResult[cbCardType] ;
			for(BYTE i=0; i<tmpCardResult.cbCardTypeCount; ++i)
			{
				//不能放走地主小牌
				if(cbCardType==CT_SINGLE &&  cbBankerSingleCardCount > 0 && GetCardLogicValue(tmpCardResult.cbCardData[i][0])<cbBankerSingleCardLogic) continue ;

				//拦截对牌
				if (cbCardType == CT_DOUBLE && cbBankerDoubleCardCount > 0 && GetCardLogicValue(cbBankerDoubleCardData[cbBankerDoubleCardCount-1]) < 10 &&
					GetCardLogicValue(tmpCardResult.cbCardData[i][0]) < GetCardLogicValue(cbBankerDoubleCardData[cbBankerDoubleCardCount-1]) &&
					GetCardLogicValue(tmpCardResult.cbCardData[0][0]) >= 10 && GetCardLogicValue(tmpCardResult.cbCardData[0][0]) < 14) continue ;

				//针对顺子，三条的大牌
				if ( tmpCardResult.cbEachHandCardCount[i] != cbHandCardCount && cbCardType >= CT_SINGLE_LINE && cbCardType <= CT_THREE_LINE_TAKE_ONE &&
					( GetCardLogicValue(tmpCardResult.cbCardData[i][tmpCardResult.cbEachHandCardCount[i]-1]) > GetCardLogicValue(cbHandCardData[cbHandCardCount-2]) || 
						GetCardLogicValue(tmpCardResult.cbCardData[i][0]) >= 11 ))
				{
					BYTE cbRemainCardData[MAX_COUNT], cbRemainCardCount ;
					CopyMemory(cbRemainCardData, cbHandCardData, cbHandCardCount) ;
					cbRemainCardCount = cbHandCardCount ;

					//移除扑克
					RemoveCard(tmpCardResult.cbCardData[i], tmpCardResult.cbEachHandCardCount[i], cbRemainCardData, cbRemainCardCount) ;
					cbRemainCardCount -= tmpCardResult.cbEachHandCardCount[i] ;

					//最大扑克
					BYTE cbCurrentLargestLogicCard = GetCardLogicValue(tmpCardResult.cbCardData[i][0]) ;

					if (GetCardType(cbRemainCardData, cbRemainCardCount) == CT_ERROR && (cbCardType >= CT_THREE_LINE_TAKE_ONE && 
						cbCardType <= CT_THREE_LINE_TAKE_ONE && cbCurrentLargestLogicCard >= 11 && tmpCardResult.cbEachHandCardCount[i] <=5 || 
						cbCardType == CT_SINGLE_LINE && tmpCardResult.cbEachHandCardCount[i] <= 6 && cbCurrentLargestLogicCard >= 12 || 
						cbCardType >= CT_DOUBLE_LINE && cbCardType <= CT_THREE_LINE && cbCurrentLargestLogicCard >= 12 && tmpCardResult.cbEachHandCardCount[i] <= 8))
					{
						//暂时不出
						if ( cbCardType >= CT_SINGLE_LINE && cbCardType <= CT_THREE_LINE &&
							GetCardLogicValue(tmpCardResult.cbCardData[i][tmpCardResult.cbEachHandCardCount[i] - 1]) > GetCardLogicValue(cbHandCardData[cbHandCardCount-3]) )
							continue ;

						if ( cbCardType >= CT_THREE_LINE_TAKE_ONE && cbCardType <= CT_THREE_LINE_TAKE_ONE &&
							GetCardLogicValue(tmpCardResult.cbCardData[i][0]) > GetCardLogicValue(cbHandCardData[cbHandCardCount-3]) )
							continue ;
					}
				}

				BYTE cbTmpCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, tmpCardResult.cbCardData[i], tmpCardResult.cbEachHandCardCount[i]) ;

				BYTE cbMaxValue=0 ; 
				BYTE Index = 0 ;

				//搜索cbMinSingleCardCount[4]的最大值
				for(BYTE j=0; j<4; ++j)
				{
					if(cbMinSingleCardCount[j]>=cbTmpCount)
					{
						cbMinSingleCardCount[j] = cbTmpCount ;
						cbIndex[j] = i ;
						cbOutcardType[j] = cbCardType ;
						break ;
					}
				}

				//保存最小值
				if(cbMinSingleCountInFour>=cbTmpCount)
				{
					//最小牌型
					cbMinCardType = cbCardType ;
					//最小牌型中的最小单牌
					cbMinSingleCountInFour=cbTmpCount ;						
					//最小牌型中的最小牌
					cbMinIndex=i ;
				}
			}
		}

		if(cbMinSingleCountInFour!=MAX_COUNT)
		{
			BYTE Index = cbMinIndex ;

			//选择最小牌
			for(BYTE i=0; i<4; ++i)
			{
				if(cbOutcardType[i]==cbMinCardType && cbMinSingleCardCount[i]<=cbMinSingleCountInFour &&
					GetCardLogicValue(CardTypeResult[cbMinCardType].cbCardData[cbIndex[i]][0])<GetCardLogicValue(CardTypeResult[cbMinCardType].cbCardData[Index][0]))
					Index = cbIndex[i] ;
			}

			//对王加一只
			if(cbHandCardCount==3 && GetCardColor(cbHandCardData[0])==0x40 && GetCardColor(cbHandCardData[1])==0x40)
			{
				OutCardResult.cbCardCount = 2 ;
				OutCardResult.cbResultCard[0] = 0x4f ;
				OutCardResult.cbResultCard[1] = 0x4e ;
				return ;
			}
			//对王
			else if(cbHandCardCount==2 && GetCardColor(cbHandCardData[0])==0x40 && GetCardColor(cbHandCardData[1])==0x40)
			{
				OutCardResult.cbCardCount = 2 ;
				OutCardResult.cbResultCard[0] = 0x4f ;
				OutCardResult.cbResultCard[1] = 0x4e ;
				return ;
			}
			else
			{
				//设置变量
				OutCardResult.cbCardCount=CardTypeResult[cbMinCardType].cbEachHandCardCount[Index];
				CopyMemory(OutCardResult.cbResultCard,CardTypeResult[cbMinCardType].cbCardData[Index],CardTypeResult[cbMinCardType].cbEachHandCardCount[Index]*sizeof(BYTE));
				return ;
			}

			ASSERT(OutCardResult.cbCardCount>0) ;

			return ;
		}
		//如果只剩炸弹
		else if (CardTypeResult[CT_BOMB_CARD].cbCardTypeCount > 0) 
		{	
			//BYTE Index=0 ;
			//BYTE cbLogicCardValue = GetCardLogicValue(0x4F)+1 ;
			////最小炸弹
			//for(BYTE i=0; i<CardTypeResult[CT_BOMB_CARD].cbCardTypeCount; ++i)
			//	if(cbLogicCardValue>GetCardLogicValue(CardTypeResult[CT_BOMB_CARD].cbCardData[i][0]))
			//	{
			//		cbLogicCardValue = GetCardLogicValue(CardTypeResult[CT_BOMB_CARD].cbCardData[i][0]) ;
			//		Index = i ;
			//	}

			//	//设置变量
			//	OutCardResult.cbCardCount=CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[Index];
			//	CopyMemory(OutCardResult.cbResultCard,CardTypeResult[CT_BOMB_CARD].cbCardData[Index],CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[Index]*sizeof(BYTE));

			//	return ;
		}

		BYTE cbAllSingleCardData[MAX_COUNT], cbAllSingleCardCount ;
		cbAllSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, NULL, 0, cbAllSingleCardData) ;

		if ( cbAllSingleCardCount > 0 )
		{
			//如果都没有搜索到就出最小的一张
			if ( 1 == m_cbUserCardCount[m_wBankerUser] ) 
			{
				OutCardResult.cbCardCount = 1 ;
				OutCardResult.cbResultCard[0] = cbAllSingleCardData[0] ;
			}
			else 
			{
				OutCardResult.cbCardCount = 1 ;
				OutCardResult.cbResultCard[0] = cbAllSingleCardData[cbAllSingleCardCount-1] ;
			}
			return ;
		}

		//如果都没有搜索到就出最小的一张
		OutCardResult.cbCardCount = 1 ;
		OutCardResult.cbResultCard[0] = cbHandCardData[cbHandCardCount-1] ;
		return ;
}

//地主上家（后出牌）
void CGameLogic::UpsideOfBankerOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wOutCardUser, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResult & OutCardResult) 
{
	//零下标没用
	tagOutCardTypeResult CardTypeResult[12+1] ;
	ZeroMemory(CardTypeResult, sizeof(CardTypeResult)) ;

	//初始变量
	ZeroMemory(&OutCardResult, sizeof(OutCardResult)) ;

	//出牌类型
	BYTE cbOutCardType = GetCardType(cbTurnCardData, cbTurnCardCount) ;

	//地主只有一张，从大到小出牌
	if (m_cbUserCardCount[m_wBankerUser] == 1 && wOutCardUser != m_wBankerUser && CT_SINGLE == cbOutCardType)
	{
		BYTE cbSingleCardData[MAX_COUNT] ;
		BYTE cbSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, NULL, 0, cbSingleCardData) ;

		WORD wFriendID = (m_wBankerUser+1) % GAME_PLAYER ;
		WORD wMeID = (wFriendID+1) % GAME_PLAYER ;

		BYTE cbFriendLargestLogicCard = GetCardLogicValue(m_cbAllCardData[wFriendID][0]) ;
		BYTE cbMeLargestLogicCard = GetCardLogicValue(cbHandCardData[0]) ;
		BYTE cbTurnLogicCard = GetCardLogicValue(cbTurnCardData[0]) ;

		//只剩单牌(人性化处理)
		if (cbSingleCardCount == cbHandCardCount && cbFriendLargestLogicCard > cbTurnLogicCard && cbMeLargestLogicCard > cbTurnLogicCard) 
		{
			OutCardResult.cbCardCount = 1 ;
			OutCardResult.cbResultCard[0] = cbHandCardData[0] ;

			return ;
		}
	}

	//搜索可出牌
	tagOutCardTypeResult BankerOutCardTypeResult[13] ;
	ZeroMemory(BankerOutCardTypeResult, sizeof(BankerOutCardTypeResult)) ;

	AnalyseOutCardType(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser], BankerOutCardTypeResult) ;
	AnalyseOutCardType(cbHandCardData,cbHandCardCount,cbTurnCardData,cbTurnCardCount, CardTypeResult) ;

	//只剩炸弹
	if(cbHandCardCount==CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[0] && (cbOutCardType<CT_BOMB_CARD || 
		GetCardLogicValue(CardTypeResult[CT_BOMB_CARD].cbCardData[0][0])>GetCardLogicValue(cbTurnCardData[0])))
	{
		OutCardResult.cbCardCount = CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[0] ;
		CopyMemory(OutCardResult.cbResultCard,  CardTypeResult[CT_BOMB_CARD].cbCardData, OutCardResult.cbCardCount) ;

		return ;
	}
	//双王炸弹和一手
	else if(cbHandCardCount>2 && cbHandCardData[0]==0x4f && cbHandCardData[1]==0x4e && CT_ERROR!=GetCardType(cbHandCardData+2, cbHandCardCount-2) &&
		CT_FOUR_LINE_TAKE_ONE != GetCardType(cbHandCardData+2, cbHandCardCount-2) && CT_FOUR_LINE_TAKE_TWO != GetCardType(cbHandCardData+2, cbHandCardCount-2))
	{
		OutCardResult.cbCardCount = 2 ;
		OutCardResult.cbResultCard[0] = 0x4f ;
		OutCardResult.cbResultCard[1] = 0x4e ;
		return ;
	}

	//炸弹和一手
	BYTE cbRemainCard[MAX_COUNT], cbRemainCount=0;
	BYTE cbAllBombCard[MAX_COUNT], cbAllBombCount=0 ;
	GetAllBomCard(cbHandCardData, cbHandCardCount, cbAllBombCard, cbAllBombCount) ;

	//出炸判断
	if(cbAllBombCount>0 && m_wBankerUser==wOutCardUser)
	{
		//剩余扑克
		CopyMemory(cbRemainCard, cbHandCardData, cbHandCardCount) ;
		cbRemainCount = cbHandCardCount ;
		RemoveCard(cbAllBombCard, cbAllBombCount, cbRemainCard, cbRemainCount) ;
		cbRemainCount -= cbAllBombCount ;

		if(CT_ERROR != GetCardType(cbRemainCard, cbRemainCount) || 
			(2==cbRemainCount && GetCardLogicValue(cbRemainCard[0])>GetCardLogicValue(m_cbAllCardData[m_wBankerUser][0])))
		{
			if((cbOutCardType<CT_BOMB_CARD || GetCardLogicValue(cbAllBombCard[0])>GetCardLogicValue(cbTurnCardData[0])) &&
				( CardTypeResult[cbOutCardType].cbCardTypeCount <= 0 || CT_ERROR != GetCardType(cbRemainCard, cbRemainCount)) )
			{
				//双王炸弹
				if(GetCardColor(cbAllBombCard[0])==0x40)
				{
					OutCardResult.cbCardCount = 2 ;
					OutCardResult.cbResultCard[0] = 0x4f ;
					OutCardResult.cbResultCard[1] = 0x4e ;
					return ;
				}
				else
				{
					//分析地主牌
					BYTE cbBankerAllBombCard[MAX_COUNT], cbBankerAllBombCardCount ;
					GetAllBomCard( m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser], cbBankerAllBombCard, cbBankerAllBombCardCount) ;

					if ( !CompareCard( cbTurnCardData, cbRemainCard, cbTurnCardCount, cbRemainCount) ||  cbBankerAllBombCardCount <= 0 || 
						GetCardLogicValue( cbAllBombCard[0] ) > GetCardLogicValue( cbBankerAllBombCard[0] ) )
					{
						OutCardResult.cbCardCount = 4 ;
						CopyMemory(OutCardResult.cbResultCard, cbAllBombCard, 4) ;
						return ;
					}
				}
			}
		}
	}

	//只有一手牌
	if ( GetCardType(cbHandCardData, cbHandCardCount) != CT_FOUR_LINE_TAKE_ONE && GetCardType(cbHandCardData, cbHandCardCount) != CT_FOUR_LINE_TAKE_TWO &&
		CompareCard(cbTurnCardData, cbHandCardData, cbTurnCardCount, cbHandCardCount) )
	{
		OutCardResult.cbCardCount = cbHandCardCount ;
		CopyMemory(OutCardResult.cbResultCard, cbHandCardData, cbHandCardCount) ;

		return ;
	}

	//对牌接牌判断
	if (1 == m_cbUserCardCount[m_wBankerUser] && cbHandCardCount >= 2 && cbOutCardType == CT_DOUBLE)
	{
		tagAnalyseResult AnalyseResult ;
		ZeroMemory(&AnalyseResult, sizeof(AnalyseResult)) ;

		//分析扑克
		AnalysebCardData(cbHandCardData, cbHandCardCount, AnalyseResult) ;

		//对牌判断
		if (cbHandCardCount == (AnalyseResult.cbDoubleCount * 2 + AnalyseResult.cbThreeCount * 3 + AnalyseResult.cbFourCount * 4) || 
			cbHandCardCount == (AnalyseResult.cbDoubleCount * 2 + AnalyseResult.cbThreeCount * 3 + AnalyseResult.cbFourCount * 4 + 1))
		{
			//出对判断
			for (int nIndex = AnalyseResult.cbDoubleCount-1; nIndex>=0 ; --nIndex)
			{
				if (GetCardLogicValue(AnalyseResult.cbDoubleCardData[nIndex*2]) > GetCardLogicValue(cbTurnCardData[0]))
				{
					OutCardResult.cbCardCount = 2 ;
					OutCardResult.cbResultCard[0] = AnalyseResult.cbDoubleCardData[nIndex*2] ;
					OutCardResult.cbResultCard[1] = AnalyseResult.cbDoubleCardData[nIndex*2+1] ;
					return ;
				}
			}

			//出炸判断
			if (AnalyseResult.cbFourCount > 0) 
			{
				//最小炸弹
				BYTE cbLestBombIndex = AnalyseResult.cbFourCount-1 ;

				OutCardResult.cbCardCount = 4 ;
				OutCardResult.cbResultCard[0] = AnalyseResult.cbFourCardData[cbLestBombIndex*4] ;
				OutCardResult.cbResultCard[1] = AnalyseResult.cbFourCardData[cbLestBombIndex*4+1] ;
				OutCardResult.cbResultCard[2] = AnalyseResult.cbFourCardData[cbLestBombIndex*4+2] ;
				OutCardResult.cbResultCard[3] = AnalyseResult.cbFourCardData[cbLestBombIndex*4+3] ;

				return ;
			}
            			
		}

		//分析对家
		if ( wOutCardUser != m_wBankerUser )
		{
			tagAnalyseResult FriendAnalyseResult ;
			ZeroMemory( &FriendAnalyseResult, sizeof( FriendAnalyseResult ) ) ;

			//分析扑克
			AnalysebCardData( m_cbAllCardData[wOutCardUser], m_cbUserCardCount[wOutCardUser], FriendAnalyseResult ) ;

			//对牌判断
			if (m_cbUserCardCount[wOutCardUser] == (FriendAnalyseResult.cbDoubleCount * 2 + FriendAnalyseResult.cbThreeCount * 3 + FriendAnalyseResult.cbFourCount * 4) || 
				m_cbUserCardCount[wOutCardUser] == (FriendAnalyseResult.cbDoubleCount * 2 + FriendAnalyseResult.cbThreeCount * 3 + FriendAnalyseResult.cbFourCount * 4 + 1))
			{
				return ;
			}

			//零下标没用
			tagOutCardTypeResult FriendCardTypeResult[12+1] ;
			ZeroMemory( FriendCardTypeResult, sizeof( FriendCardTypeResult ) ) ;

			AnalyseOutCardType( m_cbAllCardData[wOutCardUser], m_cbUserCardCount[wOutCardUser], FriendCardTypeResult ) ;

			for ( BYTE cbLineCardIdx = 0; cbLineCardIdx < FriendCardTypeResult[CT_SINGLE_LINE].cbCardTypeCount; ++cbLineCardIdx )
			{
				//剩余扑克
				BYTE cbRemainCardData[MAX_COUNT], cbRemainCardCount ;

				cbRemainCardCount = m_cbUserCardCount[wOutCardUser] ;
				CopyMemory( cbRemainCardData, m_cbAllCardData[wOutCardUser], cbRemainCardCount ) ;
				RemoveCard( FriendCardTypeResult[CT_SINGLE_LINE].cbCardData[cbLineCardIdx],  FriendCardTypeResult[CT_SINGLE_LINE].cbEachHandCardCount[cbLineCardIdx], 
					cbRemainCardData, cbRemainCardCount ) ;

				cbRemainCardCount -= FriendCardTypeResult[CT_SINGLE_LINE].cbEachHandCardCount[cbLineCardIdx] ;

				//分析扑克
				AnalysebCardData( cbRemainCardData, cbRemainCardCount, FriendAnalyseResult ) ;

				//对牌判断
				if (cbRemainCardCount == (FriendAnalyseResult.cbDoubleCount * 2 + FriendAnalyseResult.cbThreeCount * 3 + FriendAnalyseResult.cbFourCount * 4) || 
					cbRemainCardCount == (FriendAnalyseResult.cbDoubleCount * 2 + FriendAnalyseResult.cbThreeCount * 3 + FriendAnalyseResult.cbFourCount * 4 + 1))
				{
					return ;
				}
			}

			for ( BYTE cbLineCardIdx = 0; cbLineCardIdx < FriendCardTypeResult[CT_DOUBLE_LINE].cbCardTypeCount; ++cbLineCardIdx )
			{
				//剩余扑克
				BYTE cbRemainCardData[MAX_COUNT], cbRemainCardCount ;

				cbRemainCardCount = m_cbUserCardCount[wOutCardUser] ;
				CopyMemory( cbRemainCardData, m_cbAllCardData[wOutCardUser], cbRemainCardCount ) ;
				RemoveCard( FriendCardTypeResult[CT_SINGLE_LINE].cbCardData[cbLineCardIdx],  FriendCardTypeResult[CT_SINGLE_LINE].cbEachHandCardCount[cbLineCardIdx], 
					cbRemainCardData, cbRemainCardCount ) ;

				cbRemainCardCount -= FriendCardTypeResult[CT_SINGLE_LINE].cbEachHandCardCount[cbLineCardIdx] ;

				//分析扑克
				AnalysebCardData( cbRemainCardData, cbRemainCardCount, FriendAnalyseResult ) ;

				//对牌判断
				if (cbRemainCardCount == (FriendAnalyseResult.cbDoubleCount * 2 + FriendAnalyseResult.cbThreeCount * 3 + FriendAnalyseResult.cbFourCount * 4) || 
					cbRemainCardCount == (FriendAnalyseResult.cbDoubleCount * 2 + FriendAnalyseResult.cbThreeCount * 3 + FriendAnalyseResult.cbFourCount * 4 + 1))
				{
					return ;
				}
			}


		}
	}

	//对家可否出完
	if( m_wBankerUser != wOutCardUser && ! CompareCard( cbTurnCardData, m_cbAllCardData[ m_wBankerUser ], cbTurnCardCount, m_cbUserCardCount[ m_wBankerUser ] ) )
	{
		//庄家扑克
		bool bBankerCanOut = false ;
		tagOutCardTypeResult BankerOutCardResult[12+1] ;
		ZeroMemory(BankerOutCardResult, sizeof(BankerOutCardResult)) ;

        //分析扑克
		AnalyseOutCardType(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser], cbTurnCardData, cbTurnCardCount, BankerOutCardResult) ;
		for(BYTE cbCardType=CT_SINGLE; cbCardType<=CT_MISSILE_CARD; ++cbCardType) if(BankerOutCardResult[cbCardType].cbCardTypeCount>0) bBankerCanOut = true ; 

		if(!bBankerCanOut)
		{
		//对家ID
		WORD wFriendChairID = (m_wBankerUser+1)%GAME_PLAYER ;

		//分析扑克
		tagOutCardTypeResult FriendCardTypeResult[12+1] ;
		ZeroMemory(FriendCardTypeResult, sizeof(FriendCardTypeResult)) ;
		AnalyseOutCardType(m_cbAllCardData[wFriendChairID], m_cbUserCardCount[wFriendChairID], FriendCardTypeResult) ;

		for(BYTE cbCardType=CT_SINGLE; cbCardType<=CT_MISSILE_CARD; ++cbCardType)
			if(FriendCardTypeResult[cbCardType].cbCardTypeCount>0)
			{
				for(LONG lIndex=0; lIndex<FriendCardTypeResult[cbCardType].cbCardTypeCount; ++lIndex)
				{
					if(TestOutAllCard(wFriendChairID, FriendCardTypeResult[cbCardType].cbCardData[lIndex], FriendCardTypeResult[cbCardType].cbEachHandCardCount[lIndex], true))
					{
						//不压对家
						return ;
					}
				}
			}
		}
	}

	//放走对家
	if (GetCardType(m_cbAllCardData[(m_wBankerUser + 1) % GAME_PLAYER], m_cbUserCardCount[(m_wBankerUser + 1) % GAME_PLAYER]) == GetCardType(cbTurnCardData, cbTurnCardCount) &&
		CompareCard(cbTurnCardData, m_cbAllCardData[(m_wBankerUser + 1) % GAME_PLAYER], cbTurnCardCount, m_cbUserCardCount[(m_wBankerUser + 1) % GAME_PLAYER]) &&
		!CompareCard(cbTurnCardData, m_cbAllCardData[m_wBankerUser], cbTurnCardCount, m_cbUserCardCount[m_wBankerUser]))
		return ;

	if (CompareCard(cbTurnCardData, m_cbAllCardData[(m_wBankerUser + 1) % GAME_PLAYER], cbTurnCardCount, m_cbUserCardCount[(m_wBankerUser + 1) % GAME_PLAYER]) &&
		!CompareCard(cbTurnCardData, m_cbAllCardData[m_wBankerUser], cbTurnCardCount, m_cbUserCardCount[m_wBankerUser])) return ;

	//判断可否出完
	BYTE cbSingleCardCount = MAX_COUNT+CT_MISSILE_CARD ;
	bool bFindBestCard = false ;
	for(BYTE cbCardType=CT_SINGLE; cbCardType<=CT_MISSILE_CARD; ++cbCardType)
		if(CardTypeResult[cbCardType].cbCardTypeCount>0)
		{
			for(LONG lIndex=0; lIndex<CardTypeResult[cbCardType].cbCardTypeCount; ++lIndex)
			{
				WORD wMeChairID = (m_wBankerUser+2)%GAME_PLAYER ;

				if(TestOutAllCard(wMeChairID, CardTypeResult[cbCardType].cbCardData[lIndex], CardTypeResult[cbCardType].cbEachHandCardCount[lIndex], false))
				{
					//计算单牌
					BYTE cbTmpSingleCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, CardTypeResult[cbCardType].cbCardData[lIndex], 
						CardTypeResult[cbCardType].cbEachHandCardCount[lIndex]) ;

					//结果判断
					if (cbTmpSingleCount >= MAX_COUNT) continue ;

					//炸弹优先级排后
					BYTE cbBombCardType = GetCardType(CardTypeResult[cbCardType].cbCardData[lIndex], CardTypeResult[cbCardType].cbEachHandCardCount[lIndex]) ;
					if (cbBombCardType == CT_BOMB_CARD) cbTmpSingleCount += 4 ;
					else if (cbBombCardType == CT_MISSILE_CARD) cbTmpSingleCount += 5 ;

					////改变权值
					//if (cbBombCardType != CT_ERROR) cbTmpSingleCount += cbBombCardType ;
					
					//不出炸弹
					//BYTE cbWantOutCardType = GetCardType(CardTypeResult[cbCardType].cbCardData[lIndex], CardTypeResult[cbCardType].cbEachHandCardCount[lIndex]) ;
					//if (CardTypeResult[cbOutCardType].cbCardTypeCount > 0 && cbOutCardType < CT_BOMB_CARD && cbWantOutCardType >= CT_BOMB_CARD) continue ;


					if(cbTmpSingleCount <= cbSingleCardCount)
					{
						//设置变量
						OutCardResult.cbCardCount=CardTypeResult[cbCardType].cbEachHandCardCount[lIndex];
						CopyMemory(OutCardResult.cbResultCard, CardTypeResult[cbCardType].cbCardData[lIndex], 
						CardTypeResult[cbCardType].cbEachHandCardCount[lIndex]*sizeof(BYTE));
						cbSingleCardCount = cbTmpSingleCount ;

						bFindBestCard = true ;
					}
				}
			}
		}
		//直接返回
		if (bFindBestCard) return ;

	//如果庄家没有此牌型了则不压对家牌
	if( m_cbUserCardCount[m_wBankerUser]<=5 && wOutCardUser!=m_wBankerUser &&
		(BankerOutCardTypeResult[cbOutCardType].cbCardTypeCount==0 ||
		GetCardLogicValue(BankerOutCardTypeResult[cbOutCardType].cbCardData[0][0])<=GetCardLogicValue(cbTurnCardData[0])) &&
		CardTypeResult[cbOutCardType].cbEachHandCardCount[0]!=cbHandCardCount)//不能一次出完
	{
		//放弃出牌
		return ;
	}

	//下家为地主，而且地主扑克少于5张
	if(m_cbUserCardCount[m_wBankerUser]<=5 && CardTypeResult[cbOutCardType].cbCardTypeCount>0 && cbOutCardType!=CT_BOMB_CARD && 
		((GetCardLogicValue(cbTurnCardData[0])<12 && wOutCardUser!=m_wBankerUser && BankerOutCardTypeResult[cbOutCardType].cbCardTypeCount>0) ||//对家出牌
		(wOutCardUser==m_wBankerUser)))//地主出牌
	{
		//防止三带等带大牌出去
		BYTE Index = cbOutCardType == CT_SINGLE ? 0 : CardTypeResult[cbOutCardType].cbCardTypeCount - 1 ;
		//寻找可以压住地主的最小一手牌
		BYTE cbThisOutTypeMinSingleCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, CardTypeResult[cbOutCardType].cbCardData[0], CardTypeResult[cbOutCardType].cbEachHandCardCount[0]) ;
		BYTE cbBestIndex = 255 ;
		for(BYTE i=0; i<CardTypeResult[cbOutCardType].cbCardTypeCount; ++i)
		{
			BYTE cbTmpSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, CardTypeResult[cbOutCardType].cbCardData[i], CardTypeResult[cbOutCardType].cbEachHandCardCount[i]) ;
			if((BankerOutCardTypeResult[cbOutCardType].cbCardTypeCount>0 && 
				GetCardLogicValue(CardTypeResult[cbOutCardType].cbCardData[i][0])>=GetCardLogicValue(BankerOutCardTypeResult[cbOutCardType].cbCardData[0][0]) ||
				BankerOutCardTypeResult[cbOutCardType].cbCardTypeCount==0) && 
				cbTmpSingleCardCount<=cbThisOutTypeMinSingleCount)
			{
				cbBestIndex = i ;
				cbThisOutTypeMinSingleCount = cbTmpSingleCardCount ;
			}

			if((BankerOutCardTypeResult[cbOutCardType].cbCardTypeCount>0 && 
				GetCardLogicValue(CardTypeResult[cbOutCardType].cbCardData[i][0])>=GetCardLogicValue(BankerOutCardTypeResult[cbOutCardType].cbCardData[0][0]) ||
				BankerOutCardTypeResult[cbOutCardType].cbCardTypeCount==0))
				Index = i ;
			else break ;
		}

		if(cbBestIndex!=255)
		{
			OutCardResult.cbCardCount = CardTypeResult[cbOutCardType].cbEachHandCardCount[cbBestIndex] ;
			CopyMemory(OutCardResult.cbResultCard, CardTypeResult[cbOutCardType].cbCardData[cbBestIndex], OutCardResult.cbCardCount) ;
		}
		else
		{
			OutCardResult.cbCardCount = CardTypeResult[cbOutCardType].cbEachHandCardCount[Index] ;
			CopyMemory(OutCardResult.cbResultCard, CardTypeResult[cbOutCardType].cbCardData[Index], OutCardResult.cbCardCount) ;
		}

		return ;
	}

	//单牌顶牌
	if (CT_SINGLE == cbOutCardType && CardTypeResult[cbOutCardType].cbCardTypeCount > 0)
	{
		BYTE cbMeSingleCardData[MAX_COUNT], cbMeSingleCardCount ;
		BYTE cbBankerSingleCardData[MAX_COUNT], cbBankerSingleCardCount ;

		//获取单牌
		cbMeSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, NULL, 0, cbMeSingleCardData) ;
		cbBankerSingleCardCount = AnalyseSinleCardCount(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser], NULL, 0, cbBankerSingleCardData) ;

		//地主还有小牌
		if (cbBankerSingleCardCount > 0 && cbMeSingleCardCount > 0 && GetCardLogicValue(cbBankerSingleCardData[cbBankerSingleCardCount-1]) <= 10)
		{
			//拦截两张
			if (cbBankerSingleCardCount >= 2 && GetCardValue(cbBankerSingleCardData[cbBankerSingleCardCount-2]) <= 10)
			{
				for (int nMeIndex = cbMeSingleCardCount-1; nMeIndex >=0 ; --nMeIndex)
					if (GetCardLogicValue(cbMeSingleCardData[nMeIndex]) > GetCardLogicValue(cbTurnCardData[0]) && 
						GetCardLogicValue(cbMeSingleCardData[nMeIndex]) >= GetCardLogicValue(cbBankerSingleCardData[cbBankerSingleCardCount-2]) &&
						GetCardLogicValue(cbMeSingleCardData[nMeIndex]) <= 15)
					{
						OutCardResult.cbCardCount = 1 ;
						OutCardResult.cbResultCard[0] = cbMeSingleCardData[nMeIndex] ;

						return ;
					}
			}	

			//拦截一张
			for (int nMeIndex = cbMeSingleCardCount-1; nMeIndex >=0 ; --nMeIndex)
				if (GetCardLogicValue(cbMeSingleCardData[nMeIndex]) > GetCardLogicValue(cbTurnCardData[0]) && 
					GetCardLogicValue(cbMeSingleCardData[nMeIndex]) >= GetCardLogicValue(cbBankerSingleCardData[cbBankerSingleCardCount-1]) &&
					GetCardLogicValue(cbMeSingleCardData[nMeIndex]) <= 15)
				{
					OutCardResult.cbCardCount = 1 ;
					OutCardResult.cbResultCard[0] = cbMeSingleCardData[nMeIndex] ;

					return ;
				}
		}
	}

	//取出四个最小单牌
	BYTE cbMinSingleCardCount[4] ;
	cbMinSingleCardCount[0]=MAX_COUNT ;
	cbMinSingleCardCount[1]=MAX_COUNT ;
	cbMinSingleCardCount[2]=MAX_COUNT ;
	cbMinSingleCardCount[3]=MAX_COUNT ;
	BYTE cbIndex[4]={0} ;	
	BYTE cbMinSingleCountInFour=MAX_COUNT ;

	//可出扑克（这里已经过滤掉炸弹了）
	tagOutCardTypeResult const &CanOutCard = CardTypeResult[cbOutCardType] ;

	for(BYTE i=0; i<CanOutCard.cbCardTypeCount; ++i)
	{
		//最小单牌
		BYTE cbTmpCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount,CanOutCard.cbCardData[i], CanOutCard.cbEachHandCardCount[i]) ; 
		BYTE cbMaxValue=0 ; 
		BYTE Index = 0 ;

		//搜索cbMinSingleCardCount[4]的最大值
		for(BYTE j=0; j<4; ++j)
		{
			if(cbMinSingleCardCount[j]>=cbTmpCount)
			{
				cbMinSingleCardCount[j] = cbTmpCount ;
				cbIndex[j] = i ;
				break ;
			}
		}

	}

	for(BYTE i=0; i<4; ++i)
		if(cbMinSingleCountInFour>cbMinSingleCardCount[i]) cbMinSingleCountInFour = cbMinSingleCardCount[i] ;


	//原始单牌数
	BYTE cbOriginSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount,NULL,0) ;

	//分析地主对牌
	BYTE cbBankerDoubleCardData[MAX_COUNT], cbBankerDoubleCardCount ;
	GetAllDoubleCard(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser], cbBankerDoubleCardData, cbBankerDoubleCardCount) ;

	//朋友出牌
	bool bFriendOut = m_wBankerUser!=wOutCardUser ;
	if(bFriendOut)
	{
		//不拦截朋友最后一手牌
		if (GetCardType(m_cbAllCardData[(1 + m_wBankerUser) % GAME_PLAYER], m_cbUserCardCount[(1 + m_wBankerUser) % GAME_PLAYER]) != CT_ERROR) return ;

		//在上面的TestOutAllCard中已对可出炸弹情况分析过
		if(CanOutCard.cbCardTypeCount>0 && CanOutCard.cbCardType < CT_BOMB_CARD)
		{
			//分析地主单牌
			BYTE cbBankerSingleCardData[MAX_COUNT] ;
			BYTE cbBankerSingleCardCount=AnalyseSinleCardCount(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser], NULL, 0, cbBankerSingleCardData) ;
			BYTE cbBankerSingleCardLogic = 0 ;

			if(cbBankerSingleCardCount>=2 && GetCardLogicValue(cbBankerSingleCardData[cbBankerSingleCardCount-2])<=10) cbBankerSingleCardLogic = GetCardLogicValue(cbBankerSingleCardData[cbBankerSingleCardCount-2]) ;
			else if(cbBankerSingleCardCount>=2 && GetCardLogicValue(cbBankerSingleCardData[cbBankerSingleCardCount-1])<=10) cbBankerSingleCardLogic = GetCardLogicValue(cbBankerSingleCardData[cbBankerSingleCardCount-1]) ;
			else if(cbBankerSingleCardCount>0 && GetCardLogicValue(cbBankerSingleCardData[0])<=10) cbBankerSingleCardLogic = GetCardLogicValue(cbBankerSingleCardData[0]) ;

			BYTE cbMinLogicCardValue = GetCardLogicValue(0x4F)+1 ;
			bool bFindCard = false ;
			BYTE cbCanOutIndex=0 ;
			for(BYTE i=0; i<4; ++i)
			{
				BYTE Index = cbIndex[i] ;

				//三带，和连牌不接对家牌
				if ( CanOutCard.cbCardType >= CT_THREE &&  CanOutCard.cbCardType <= CT_MISSILE_CARD && GetCardLogicValue(CanOutCard.cbCardData[Index][0]) >= 7 && 
					CanOutCard.cbEachHandCardCount[Index] <=5 )
					continue ;

				//单牌拦截
				bool bCanOut = false ;
				if(cbOutCardType==CT_SINGLE && cbBankerSingleCardCount > 0 && GetCardLogicValue(CanOutCard.cbCardData[Index][0]) >= cbBankerSingleCardLogic &&
					GetCardLogicValue(cbTurnCardData[0]) < 14 && cbMinSingleCardCount[i] < cbOriginSingleCardCount &&
					GetCardLogicValue(m_cbAllCardData[m_wBankerUser][0]) > GetCardLogicValue(cbTurnCardData[0]))
					bCanOut = true ;

				//拦截对牌
				//if (cbOutCardType == CT_DOUBLE && cbBankerDoubleCardCount > 0 && GetCardLogicValue(cbBankerDoubleCardData[cbBankerDoubleCardCount-1]) < 10 &&
				//	GetCardLogicValue(CanOutCard.cbCardData[Index][0]) < GetCardLogicValue(cbBankerDoubleCardData[cbBankerDoubleCardCount-1]) &&
				//	GetCardLogicValue(CanOutCard.cbCardData[0][0]) >= 10 && GetCardLogicValue(CanOutCard.cbCardData[0][0]) < 14) continue ;

				//小于J的牌，或者小于K而且是散牌
				if(bCanOut ||
					((cbMinSingleCardCount[i]<cbOriginSingleCardCount+3 &&  (cbMinSingleCardCount[i]<=cbMinSingleCountInFour || cbMinSingleCardCount[i]<=cbMinSingleCountInFour+1 && 
				CanOutCard.cbCardType >= CT_THREE_LINE_TAKE_ONE && CanOutCard.cbCardType <= CT_THREE_LINE_TAKE_ONE ) && 
					(GetCardLogicValue(CanOutCard.cbCardData[Index][0])<=11 || (cbMinSingleCardCount[i]<cbOriginSingleCardCount)&&GetCardLogicValue(CanOutCard.cbCardData[Index][0])<=13)) &&
					cbMinLogicCardValue>GetCardLogicValue(CanOutCard.cbCardData[Index][0]) && cbHandCardCount>5))
				{
					//搜索有没有大牌（针对飞机带翅膀后面的带牌）
					bool bNoLargeCard = true ;
					for(BYTE k=3; k<CanOutCard.cbEachHandCardCount[Index]; ++k)
					{
						//有大牌而且不能一次出完
						if(GetCardLogicValue(CanOutCard.cbCardData[Index][k])>=15 && 
							CanOutCard.cbEachHandCardCount[Index]!=cbHandCardCount) bNoLargeCard = false ;
					}
					if(bNoLargeCard)
					{
						bFindCard = true ;
						cbCanOutIndex = Index ; 
						cbMinLogicCardValue = GetCardLogicValue(CanOutCard.cbCardData[Index][0]) ;
					}
				}
				else if(cbHandCardCount<5 && cbMinSingleCardCount[i]<cbOriginSingleCardCount+4 && cbMinSingleCardCount[i]<=cbMinSingleCountInFour &&
					cbMinLogicCardValue>GetCardLogicValue(CanOutCard.cbCardData[Index][0]))
				{
					//能出王打自家的2
					if ( GetCardLogicValue( CanOutCard.cbCardData[Index][0] ) >= 16 && GetCardLogicValue( cbTurnCardData[0] ) >= 15 )
						continue ;

					bFindCard = true ;
					cbCanOutIndex = Index ; 
					cbMinLogicCardValue = GetCardLogicValue(CanOutCard.cbCardData[Index][0]) ;
				}
			}

			if(bFindCard)
			{

				//设置变量
				OutCardResult.cbCardCount=CanOutCard.cbEachHandCardCount[cbCanOutIndex];
				CopyMemory(OutCardResult.cbResultCard,CanOutCard.cbCardData[cbCanOutIndex],CanOutCard.cbEachHandCardCount[cbCanOutIndex]*sizeof(BYTE));

				return ;
			}
			//手上少于五张牌
			else if(cbHandCardCount<=5)
			{
				BYTE cbMinLogicCard = GetCardLogicValue(0x4f)+1 ;
				BYTE cbCanOutIndex = 0 ;
				for(BYTE i=0; i<4; ++i)
					if(cbMinSingleCardCount[i]<MAX_COUNT && cbMinSingleCardCount[i]<=cbMinSingleCountInFour && cbMinLogicCard>GetCardLogicValue(CanOutCard.cbCardData[cbIndex[i]][0]) && 
						GetCardLogicValue(CanOutCard.cbCardData[cbIndex[i]][0])<=14)
					{
						cbMinLogicCard = GetCardLogicValue(CanOutCard.cbCardData[cbIndex[i]][0]) ;
						cbCanOutIndex = cbIndex[i] ;
					}

					if(cbMinLogicCard != (GetCardLogicValue(0x4f)+1))
					{
						//设置变量
						OutCardResult.cbCardCount=CanOutCard.cbEachHandCardCount[cbCanOutIndex];
						CopyMemory(OutCardResult.cbResultCard,CanOutCard.cbCardData[cbCanOutIndex],CanOutCard.cbEachHandCardCount[cbCanOutIndex]*sizeof(BYTE));

						return ;
					}
			}

			return ;
		}
		else
		{
			return ;
		}

	}
	//地主出牌
	else
	{
		if(CanOutCard.cbCardTypeCount>0)
		{
			BYTE cbMinLogicCardValue = GetCardLogicValue(0x4F)+1 ;
			bool bFindCard = false ;
			BYTE cbCanOutIndex=0 ;
			for(BYTE i=0; i<4; ++i)
			{
				BYTE Index = cbIndex[i] ;

				if((cbMinSingleCardCount[i]<cbOriginSingleCardCount+4)  &&  (cbMinSingleCardCount[i]<=cbMinSingleCountInFour || cbMinSingleCardCount[i]<=cbMinSingleCountInFour+1 && 
				CanOutCard.cbCardType >= CT_THREE_LINE_TAKE_ONE && CanOutCard.cbCardType <= CT_THREE_LINE_TAKE_ONE ) &&
					cbMinLogicCardValue>GetCardLogicValue(CanOutCard.cbCardData[Index][0]))
				{
					//针对大牌
					bool bNoLargeCard = true ;

					//当地主手上牌数大于4，而且地主出的是小于K的牌而且不是地主手上的最大牌时，不能出2去打
					if(m_cbUserCardCount[m_wBankerUser]>=4 && cbHandCardCount>=5  && CanOutCard.cbEachHandCardCount[Index]>=2 && 
						GetCardLogicValue(CanOutCard.cbCardData[Index][0])>=15 &&
						GetCardLogicValue(cbTurnCardData[0])<13 &&
						GetCardLogicValue(cbTurnCardData[0])<GetCardLogicValue(m_cbAllCardData[m_wBankerUser][0]) && 
						CanOutCard.cbEachHandCardCount[Index]!=cbHandCardCount)
						bNoLargeCard=false ;

					//搜索有没有大牌（针对飞机带翅膀后面的带牌）
					for(BYTE k=3; k<CanOutCard.cbEachHandCardCount[Index]; ++k)
					{
						if(GetCardLogicValue(CanOutCard.cbCardData[Index][k])>=15 && 
							CanOutCard.cbEachHandCardCount[Index]!=cbHandCardCount)
							bNoLargeCard = false ;
					}
					if(bNoLargeCard)
					{
						bFindCard = true ;
						cbCanOutIndex = Index ; 
						cbMinLogicCardValue = GetCardLogicValue(CanOutCard.cbCardData[Index][0]) ;
					}
				}
			}

			if(bFindCard)
			{
				//地主的最大牌
				BYTE cbLargestLogicCard = GetCardLogicValue(m_cbAllCardData[m_wBankerUser][0]) ;
				bool bCanOut=true ;

				//王只压2
				if(GetCardLogicValue(cbTurnCardData[0])<cbLargestLogicCard)
				{
					if(GetCardColor(CanOutCard.cbCardData[cbCanOutIndex][0])==0x40 && GetCardLogicValue(cbTurnCardData[0])<=14 && cbHandCardCount>5) 								
					{
						bCanOut = false ;
					}
				}
				 //双王判断
				if(GetCardLogicValue(CanOutCard.cbCardData[cbCanOutIndex][0])>=16 && cbHandCardCount>=2 && cbHandCardData[0]==0x4F && cbHandCardData[1]==0x4E)
				{
					bool bOutMissileCard = false ;
					//一手牌和一个炸弹
					BYTE cbRemainCardData[20], cbRemainCardCount=cbHandCardCount ;
					CopyMemory(cbRemainCardData, cbHandCardData, cbHandCardCount) ;
					RemoveCard(cbRemainCardData, 2, cbRemainCardData, cbRemainCardCount) ;
					cbRemainCardCount -= 2 ;
					if(CT_ERROR!=GetCardType(cbRemainCardData, cbRemainCardCount)) bOutMissileCard = true;

					//只剩少量牌
					BYTE cbRemainLargestCard = GetCardLogicValue(cbRemainCardData[0]) ;
					if(cbRemainCardCount<5 && cbRemainCardCount>0 && GetCardLogicValue(cbRemainCardData[0])>=14) bOutMissileCard = true;

					//炸后单牌数
					BYTE cbSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, CanOutCard.cbCardData[cbCanOutIndex],
						CanOutCard.cbEachHandCardCount[cbCanOutIndex]) ;
					if(cbSingleCardCount<=1 && GetCardLogicValue(cbRemainCardData[0])>=11) bOutMissileCard = true;

					//还有小牌
					if (GetCardLogicValue(cbRemainCardData[0]) <= 10 && CT_ERROR == GetCardType(cbRemainCardData, cbRemainCardCount) && 
						GetCardLogicValue(m_cbAllCardData[m_wBankerUser][0]) > 10)
						bOutMissileCard = false ;

					//火箭扑克
					if(bOutMissileCard)
					{
						//优先其他炸弹
						BYTE cbIndex = CardTypeResult[CT_BOMB_CARD].cbCardTypeCount - 1 ;

						OutCardResult.cbCardCount = CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[cbIndex] ;
						CopyMemory(OutCardResult.cbResultCard, CardTypeResult[CT_BOMB_CARD].cbCardData[cbIndex], OutCardResult.cbCardCount) ;
						return ;
					}
				}

				if(bCanOut)
				{
					//设置变量
					OutCardResult.cbCardCount=CanOutCard.cbEachHandCardCount[cbCanOutIndex];
					CopyMemory(OutCardResult.cbResultCard,CanOutCard.cbCardData[cbCanOutIndex],CanOutCard.cbEachHandCardCount[cbCanOutIndex]*sizeof(BYTE));

					return ;
				}
			}

			if(cbOutCardType==CT_SINGLE)
			{
				//地主的最大牌
				BYTE cbLargestLogicCard = GetCardLogicValue(m_cbAllCardData[m_wBankerUser][0]) ;

				if(GetCardLogicValue(cbTurnCardData[0])==14 || 
					GetCardLogicValue(cbTurnCardData[0])>=cbLargestLogicCard || 
					(GetCardLogicValue(cbTurnCardData[0])<cbLargestLogicCard-1) ||
					m_cbUserCardCount[m_wBankerUser]<=5)
				{
					//取一张大于等于2而且要比地主出的牌大的牌，
					BYTE cbIndex=MAX_COUNT ;
					for(BYTE i=0; i<cbHandCardCount; ++i)
						if(GetCardLogicValue(cbHandCardData[i])>GetCardLogicValue(cbTurnCardData[0]) &&	GetCardLogicValue(cbHandCardData[i])>=15)
						{
							cbIndex = i ;
						}
						if(cbIndex!=MAX_COUNT)
						{
							//设置变量
							OutCardResult.cbCardCount=1;
							OutCardResult.cbResultCard[0] = cbHandCardData[cbIndex] ;

							return ;
						}
				}
			}
		}

		//还要考虑炸弹
		if(CardTypeResult[CT_BOMB_CARD].cbCardTypeCount>0 || 
			(NORMAL_COUNT  == cbHandCardCount&& NORMAL_COUNT == m_cbUserCardCount[(m_wBankerUser+1)%GAME_PLAYER]))
		{
			tagOutCardTypeResult const &BomCard = CardTypeResult[CT_BOMB_CARD] ;
			BYTE cbMinLogicValue = GetCardLogicValue(BomCard.cbCardData[0][0]) ;
			BYTE Index = 0 ;
			for(BYTE i=0; i<BomCard.cbCardTypeCount; ++i)
			{
				if(cbMinLogicValue>GetCardLogicValue(BomCard.cbCardData[i][0]))
				{
					cbMinLogicValue = GetCardLogicValue(BomCard.cbCardData[i][0]) ;
					Index = i ;
				}
			}

			bool bOutBomb = false ;

			//春天判断
			if (NORMAL_COUNT  == cbHandCardCount && NORMAL_COUNT == m_cbUserCardCount[(m_wBankerUser+1)%GAME_PLAYER] && CT_ERROR != GetCardType(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser]))
				bOutBomb = true ;

			//一手牌和一个炸弹
			BYTE cbRemainCardData[20], cbRemainCardCount=cbHandCardCount ;
			CopyMemory(cbRemainCardData, cbHandCardData, cbHandCardCount) ;
			RemoveCard(BomCard.cbCardData[Index], BomCard.cbEachHandCardCount[Index], cbRemainCardData, cbRemainCardCount) ;
			cbRemainCardCount -= BomCard.cbEachHandCardCount[Index] ;
			if(CT_ERROR!=GetCardType(cbRemainCardData, cbRemainCardCount)) bOutBomb = true ;

			//炸后单牌数
			BYTE cbSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, BomCard.cbCardData[Index],BomCard.cbEachHandCardCount[Index]) ;
			if(cbSingleCardCount==0 && GetCardLogicValue(cbRemainCardData[0]) > GetCardLogicValue(m_cbAllCardData[m_wBankerUser][0])) 
				bOutBomb = true ;

			//只剩一手
			BYTE cbRemainCardType = GetCardType(m_cbAllCardData[wOutCardUser], m_cbUserCardCount[wOutCardUser]) ;
			if(cbRemainCardType>CT_ERROR && cbRemainCardType<CT_FOUR_LINE_TAKE_ONE && GetCardLogicValue(m_cbAllCardData[wOutCardUser][0])<15 && 
				cbSingleCardCount < 2 && (GetCardLogicValue(cbRemainCardData[0]) >= GetCardLogicValue(m_cbAllCardData[m_wBankerUser][0])))
				bOutBomb = true ;

			//只剩少量牌
			BYTE cbRemainLargestCard = GetCardLogicValue(cbRemainCardData[0]) ;
			if(cbRemainCardCount<5 && cbRemainCardCount>0 && (cbRemainLargestCard!=GetCardLogicValue(BomCard.cbCardData[Index][0])) && 
				cbRemainLargestCard>GetCardLogicValue(m_cbAllCardData[wOutCardUser][0]) && cbRemainLargestCard > 14) bOutBomb = true ;

			//分析扑克
			tagAnalyseResult AnalyseResult ;
			AnalysebCardData(cbRemainCardData, cbRemainCardCount, AnalyseResult) ;

			if (m_cbUserCardCount[m_wBankerUser] ==1 &&  (AnalyseResult.cbDoubleCount * 2 + AnalyseResult.cbThreeCount * 3 + AnalyseResult.cbFourCount * 4 + 1 >= cbRemainCardCount)) bOutBomb = true ;

			//设置变量
			if(bOutBomb)
			{
				OutCardResult.cbCardCount=BomCard.cbEachHandCardCount[Index];
				CopyMemory(OutCardResult.cbResultCard,BomCard.cbCardData[Index],BomCard.cbEachHandCardCount[Index]*sizeof(BYTE));
			}

			return ;
		}

		return ;
	}
	return ;
}

//地主下家（先出牌）
void CGameLogic::UndersideOfBankerOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wMeChairID,tagOutCardResult & OutCardResult) 
{
	//零下标没用
	tagOutCardTypeResult CardTypeResult[12+1] ;
	ZeroMemory(CardTypeResult, sizeof(CardTypeResult)) ;

	//初始变量
	ZeroMemory(&OutCardResult, sizeof(OutCardResult)) ;

	BYTE cbLineCard[MAX_COUNT] ;
	BYTE cbThreeLineCard[MAX_COUNT] ;
	BYTE cbDoubleLineCard[MAX_COUNT] ;
	BYTE cbLineCardCount;
	BYTE cbThreeLineCardCount ;
	BYTE cbDoubleLineCount ;
	GetAllLineCard(cbHandCardData, cbHandCardCount, cbLineCard, cbLineCardCount) ;
	GetAllThreeCard(cbHandCardData, cbHandCardCount, cbThreeLineCard, cbThreeLineCardCount) ;
	GetAllDoubleCard(cbHandCardData, cbHandCardCount, cbDoubleLineCard, cbDoubleLineCount) ;

	//判断可否出完
	BYTE cbSingleCardCount = MAX_COUNT+CT_MISSILE_CARD ;
	bool bFindBestCard = false ;
	AnalyseOutCardType(cbHandCardData, cbHandCardCount, CardTypeResult) ;
	for(BYTE cbCardType=CT_SINGLE; cbCardType<=CT_MISSILE_CARD; ++cbCardType)
		if(CardTypeResult[cbCardType].cbCardTypeCount>0)
		{
			for(LONG lIndex=0; lIndex<CardTypeResult[cbCardType].cbCardTypeCount; ++lIndex)
			{
				WORD wMeChairID = (m_wBankerUser+1)%GAME_PLAYER ;

				if(TestOutAllCard(wMeChairID, CardTypeResult[cbCardType].cbCardData[lIndex], CardTypeResult[cbCardType].cbEachHandCardCount[lIndex], true))
				{
					//计算单牌
					BYTE cbTmpSingleCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, CardTypeResult[cbCardType].cbCardData[lIndex], 
						CardTypeResult[cbCardType].cbEachHandCardCount[lIndex]) ;

					//结果判断
					if (cbTmpSingleCount >= MAX_COUNT) continue ;

					//炸弹优先级排后
					BYTE cbBombCardType = GetCardType(CardTypeResult[cbCardType].cbCardData[lIndex], CardTypeResult[cbCardType].cbEachHandCardCount[lIndex]) ;
					if (cbBombCardType == CT_BOMB_CARD) cbTmpSingleCount += 4 ;
					else if (cbBombCardType == CT_MISSILE_CARD) cbTmpSingleCount += 5 ;
					else if ( 15 == GetCardLogicValue( CardTypeResult[ cbCardType ].cbCardData[ lIndex ][ 0 ] ) ) cbTmpSingleCount += 2;
					else if ( 15 < GetCardLogicValue( CardTypeResult[ cbCardType ].cbCardData[ lIndex ][ 0 ] ) ) cbTmpSingleCount += 3;


					////改变权值
					//if (cbBombCardType != CT_ERROR) cbTmpSingleCount += cbBombCardType ;
					
					if(cbTmpSingleCount <= cbSingleCardCount)
					{
						//设置变量
						OutCardResult.cbCardCount=CardTypeResult[cbCardType].cbEachHandCardCount[lIndex];
						CopyMemory(OutCardResult.cbResultCard, CardTypeResult[cbCardType].cbCardData[lIndex], 
						CardTypeResult[cbCardType].cbEachHandCardCount[lIndex]*sizeof(BYTE));
						cbSingleCardCount = cbTmpSingleCount ;

                        bFindBestCard = true ;
					}
				}
			}
		}
	//直接返回
	if (bFindBestCard) return ;

	//对王和两单
	if ( cbHandCardCount == 4 && GetCardLogicValue(cbHandCardData[1]) == 16 && m_cbUserCardCount[m_wBankerUser] == 1 && 
		GetCardLogicValue(cbHandCardData[2]) < GetCardLogicValue(m_cbAllCardData[m_wBankerUser][0]))
	{
		OutCardResult.cbCardCount = 1 ;
		OutCardResult.cbResultCard[0] = cbHandCardData[2] ;
		return ;
	}

	//四带牌型判断
	if ( AnalyseFourCardType(cbHandCardData, cbHandCardCount, m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser], OutCardResult ) )
	{
		return ;
	}


	//如果有顺牌和单只或一对，而且单只或对比地主的小，则先出顺
	{
		if(cbLineCardCount+1==cbHandCardCount && CT_SINGLE==GetCardType(cbLineCard, cbLineCardCount))
		{
			OutCardResult.cbCardCount = cbLineCardCount ;
			CopyMemory(OutCardResult.cbResultCard, cbLineCard, cbLineCardCount) ;
		}
		else if(cbThreeLineCardCount+1==cbHandCardCount && CT_THREE_LINE==GetCardType(cbThreeLineCard, cbThreeLineCardCount))
		{
			OutCardResult.cbCardCount = cbThreeLineCardCount ;
			CopyMemory(OutCardResult.cbResultCard, cbThreeLineCard, cbThreeLineCardCount) ;
		}
		else if(cbDoubleLineCount+1==cbHandCardCount && CT_DOUBLE_LINE==GetCardType(cbDoubleLineCard, cbDoubleLineCount))
		{
			OutCardResult.cbCardCount = cbDoubleLineCount ;
			CopyMemory(OutCardResult.cbResultCard, cbDoubleLineCard, cbDoubleLineCount) ;
		}
		//双王炸弹和一手
		else if(cbHandCardCount>2 && cbHandCardData[0]==0x4f && cbHandCardData[1]==0x4e && CT_ERROR!=GetCardType(cbHandCardData+2, cbHandCardCount-2) &&
			CT_FOUR_LINE_TAKE_ONE != GetCardType(cbHandCardData+2, cbHandCardCount-2) && CT_FOUR_LINE_TAKE_TWO != GetCardType(cbHandCardData+2, cbHandCardCount-2))
		{
			OutCardResult.cbCardCount = 2 ;
			OutCardResult.cbResultCard[0] = 0x4f ;
			OutCardResult.cbResultCard[1] = 0x4e ;
		}

		if(OutCardResult.cbCardCount>0)
		{
			return ;
		}
	}
	//对王加一只
	if(cbHandCardCount==3 && GetCardColor(cbHandCardData[0])==0x40 && GetCardColor(cbHandCardData[1])==0x40)
	{
		OutCardResult.cbCardCount = 2 ;
		OutCardResult.cbResultCard[0] = 0x4f ;
		OutCardResult.cbResultCard[1] = 0x4e ;
		return ;
	}
	//对王
	else if(cbHandCardCount==2 && GetCardColor(cbHandCardData[0])==0x40 && GetCardColor(cbHandCardData[1])==0x40)
	{
		OutCardResult.cbCardCount = 2 ;
		OutCardResult.cbResultCard[0] = 0x4f ;
		OutCardResult.cbResultCard[1] = 0x4e ;
		return ;
	}
	//只剩一手牌
	else if(CT_ERROR!=GetCardType(cbHandCardData, cbHandCardCount) && CT_FOUR_LINE_TAKE_ONE!=GetCardType(cbHandCardData, cbHandCardCount) && CT_FOUR_LINE_TAKE_TWO!=GetCardType(cbHandCardData, cbHandCardCount))
	{
		OutCardResult.cbCardCount = cbHandCardCount ;
		CopyMemory(OutCardResult.cbResultCard, cbHandCardData, cbHandCardCount) ;
		return ;
	}

	//只剩一张和一手
	if(cbHandCardCount>=2)
	{
		//地主扑克
		tagOutCardTypeResult BankerCanOutCardType1[13] ;
		ZeroMemory(BankerCanOutCardType1, sizeof(BankerCanOutCardType1)) ;
		tagOutCardTypeResult BankerCanOutCardType2[13] ;
		ZeroMemory(BankerCanOutCardType2, sizeof(BankerCanOutCardType2)) ;

		BYTE cbFirstHandCardType = GetCardType(cbHandCardData, cbHandCardCount-1) ;
		BYTE cbSecondHandCardType = GetCardType(cbHandCardData+1, cbHandCardCount-1) ;

		//是否有炸
		BYTE cbAllBombCardData[MAX_COUNT], cbAllBombCount=0 ;
		GetAllBomCard(cbHandCardData, cbHandCardCount, cbAllBombCardData, cbAllBombCount) ;

		//没有炸弹
		if (cbAllBombCount <= 0 && cbFirstHandCardType!=CT_THREE_LINE_TAKE_ONE)
		{
		//地主可以出的牌
		if(cbFirstHandCardType!=CT_ERROR)
			AnalyseOutCardType(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser], cbHandCardData, cbHandCardCount-1, BankerCanOutCardType1) ;
		if(cbSecondHandCardType!=CT_ERROR)
			AnalyseOutCardType(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser], cbHandCardData+1, cbHandCardCount-1, BankerCanOutCardType2) ;

		if(cbSecondHandCardType!=CT_ERROR && cbSecondHandCardType!=CT_FOUR_LINE_TAKE_ONE && cbSecondHandCardType!= CT_FOUR_LINE_TAKE_TWO && 
			BankerCanOutCardType2[cbSecondHandCardType].cbCardTypeCount==0 && BankerCanOutCardType2[CT_BOMB_CARD].cbCardTypeCount==0)
		{
			OutCardResult.cbCardCount = cbHandCardCount-1 ;
			CopyMemory(OutCardResult.cbResultCard, cbHandCardData+1, cbHandCardCount-1) ;
			return ;
		}

		if(cbFirstHandCardType!=CT_ERROR && cbFirstHandCardType!=CT_FOUR_LINE_TAKE_ONE && cbFirstHandCardType!= CT_FOUR_LINE_TAKE_TWO &&
			BankerCanOutCardType1[cbFirstHandCardType].cbCardTypeCount==0 && BankerCanOutCardType2[CT_BOMB_CARD].cbCardTypeCount==0)
		{
			OutCardResult.cbCardCount = cbHandCardCount-1 ;
			CopyMemory(OutCardResult.cbResultCard, cbHandCardData, cbHandCardCount-1) ;
			return ;
		}

		if(GetCardLogicValue(cbHandCardData[0])>=GetCardLogicValue(m_cbAllCardData[m_wBankerUser][0]) &&
			CT_ERROR!=cbSecondHandCardType && cbSecondHandCardType!=CT_FOUR_LINE_TAKE_ONE && cbSecondHandCardType!= CT_FOUR_LINE_TAKE_TWO &&
			BankerCanOutCardType2[CT_BOMB_CARD].cbCardTypeCount==0)
		{
			OutCardResult.cbCardCount = 1 ;
			OutCardResult.cbResultCard[0] = cbHandCardData[0] ;
			return ;
		}

		if(CT_ERROR!=cbSecondHandCardType && cbSecondHandCardType!=CT_FOUR_LINE_TAKE_ONE && cbSecondHandCardType!= CT_FOUR_LINE_TAKE_TWO &&
			BankerCanOutCardType2[CT_BOMB_CARD].cbCardTypeCount==0)
		{
			OutCardResult.cbCardCount = cbHandCardCount-1 ;
			CopyMemory(OutCardResult.cbResultCard, cbHandCardData+1, cbHandCardCount-1) ;
			return ;
		}
		}
		//还有炸弹
		else
		{
			//除去炸后的牌
			BYTE cbRemainCard[MAX_COUNT], cbRemainCount=0 ;
			CopyMemory(cbRemainCard, cbHandCardData, cbHandCardCount) ;
			cbRemainCount = cbHandCardCount ;
			RemoveCard(cbAllBombCardData, cbAllBombCount, cbRemainCard, cbRemainCount) ;
			cbRemainCount -= cbAllBombCount ;
			if (GetCardType(cbRemainCard, cbRemainCount) != CT_ERROR)
			{
				OutCardResult.cbCardCount = cbRemainCount ;
				CopyMemory(OutCardResult.cbResultCard, cbRemainCard, cbRemainCount) ;
				return ;
			}
		}
	}

	//放走队友
	WORD wFriendID = (m_wBankerUser+2)%GAME_PLAYER ;
	BYTE cbFriendCardType = GetCardType(m_cbAllCardData[wFriendID], m_cbUserCardCount[wFriendID]) ;
	
	//单张扑克
	if(CT_SINGLE==cbFriendCardType)
	{
		//合法判断
		ASSERT(m_cbUserCardCount[wFriendID]==1) ;
		if(m_cbUserCardCount[wFriendID]==1 && GetCardLogicValue(cbHandCardData[cbHandCardCount-1]) < GetCardLogicValue(m_cbAllCardData[wFriendID][0]))
		{
			OutCardResult.cbCardCount = 1 ;
			OutCardResult.cbResultCard[0] = cbHandCardData[cbHandCardCount-1] ;
			return ;
		}
	}
	//一对扑克
	else if(CT_DOUBLE==cbFriendCardType && cbDoubleLineCount>=2)
	{
		if(GetCardLogicValue(cbDoubleLineCard[cbDoubleLineCount-1]) < GetCardLogicValue(m_cbAllCardData[wFriendID][0]))
		{
			OutCardResult.cbCardCount = 2 ;
			OutCardResult.cbResultCard[0] = cbDoubleLineCard[cbDoubleLineCount-2] ;
			OutCardResult.cbResultCard[1] = cbDoubleLineCard[cbDoubleLineCount-1] ;

			return ;
		}
	}

	//对牌接牌判断
	if (1 == m_cbUserCardCount[m_wBankerUser] && cbHandCardCount >= 2 && m_cbUserCardCount[wFriendID] >= 2)
	{
		tagAnalyseResult MeAnalyseResult ;
		ZeroMemory(&MeAnalyseResult, sizeof(MeAnalyseResult)) ;

		//分析扑克
		AnalysebCardData(cbHandCardData, cbHandCardCount, MeAnalyseResult) ;

		tagAnalyseResult FriendAnalyseResult ;
		ZeroMemory(&FriendAnalyseResult, sizeof(FriendAnalyseResult)) ;

		//分析扑克
		AnalysebCardData(m_cbAllCardData[wFriendID], m_cbUserCardCount[wFriendID], FriendAnalyseResult) ;

		//对牌判断
		if ((m_cbUserCardCount[wFriendID] == (FriendAnalyseResult.cbDoubleCount * 2 + FriendAnalyseResult.cbThreeCount * 3 + FriendAnalyseResult.cbFourCount * 4) || 
			m_cbUserCardCount[wFriendID] == (FriendAnalyseResult.cbDoubleCount * 2 + FriendAnalyseResult.cbThreeCount * 3 + FriendAnalyseResult.cbFourCount * 4 + 1)) &&
			MeAnalyseResult.cbDoubleCount > 0 && FriendAnalyseResult.cbDoubleCount > 0)
		{
			//最小对子
			BYTE cbMeLeastDoubleCardLogic = GetCardLogicValue(MeAnalyseResult.cbDoubleCardData[MeAnalyseResult.cbDoubleCount*2-2]) ;
			//最大对子
			BYTE cbFriendLargestDoublecardLogic = GetCardLogicValue(FriendAnalyseResult.cbDoubleCardData[0]) ;

			//出对判断
			if (cbMeLeastDoubleCardLogic < 14 && cbMeLeastDoubleCardLogic < cbFriendLargestDoublecardLogic)
			{
				OutCardResult.cbCardCount = 2 ;
				OutCardResult.cbResultCard[0] = MeAnalyseResult.cbDoubleCardData[MeAnalyseResult.cbDoubleCount*2-2] ;
				OutCardResult.cbResultCard[1] = MeAnalyseResult.cbDoubleCardData[MeAnalyseResult.cbDoubleCount*2-1] ;
				return ;
			}
          			
		}
	}

	//下家为地主，而且地主扑克少于5张
	//	if(m_cbUserCardCount[m_wBankerUser]<=5)
	{
		//分析扑克
		tagOutCardTypeResult MeOutCardTypeResult[13] ;
		ZeroMemory(MeOutCardTypeResult, sizeof(MeOutCardTypeResult)) ;
		AnalyseOutCardType(cbHandCardData, cbHandCardCount, MeOutCardTypeResult) ;

		//对家扑克
		WORD wFriendID ;
		for(WORD wChairID=0; wChairID<GAME_PLAYER; ++wChairID) 
			if(wChairID!=m_wBankerUser && wMeChairID!=wChairID) wFriendID = wChairID ;

		//计算单牌
		BYTE cbMinSingleCardCount[4] ;
		cbMinSingleCardCount[0]=MAX_COUNT ;
		cbMinSingleCardCount[1]=MAX_COUNT ;
		cbMinSingleCardCount[2]=MAX_COUNT ;
		cbMinSingleCardCount[3]=MAX_COUNT ;
		BYTE cbIndex[4]={0} ;
		BYTE cbOutcardType[4]={CT_ERROR} ;
		BYTE cbMinValue=MAX_COUNT ; 
		BYTE cbMinSingleCountInFour=MAX_COUNT ;
		BYTE cbMinCardType=CT_ERROR ;
		BYTE cbMinIndex=0 ;

		//除炸弹外的牌
		for(BYTE cbCardType=CT_DOUBLE; cbCardType<CT_BOMB_CARD; ++cbCardType)
		{
			tagOutCardTypeResult const &tmpCardResult = MeOutCardTypeResult[cbCardType] ;

			//相同牌型，相同长度，单连，对连等相同牌型可能长度不一样
			BYTE cbThisHandCardCount = MAX_COUNT ;

			//地主扑克
			tagOutCardTypeResult BankerCanOutCard[13] ;
			ZeroMemory(BankerCanOutCard, sizeof(BankerCanOutCard)) ;

			tagOutCardTypeResult FriendOutCardTypeResult[13] ;
			ZeroMemory(FriendOutCardTypeResult, sizeof(FriendOutCardTypeResult)) ;

			for(BYTE i=0; i<tmpCardResult.cbCardTypeCount; ++i)
			{
				//拆三条判断
				if ( cbCardType == CT_DOUBLE )
				{
					tagAnalyseResult AnalyseResult ;
					ZeroMemory( &AnalyseResult,  sizeof( AnalyseResult ) ) ;
					AnalysebCardData( cbHandCardData, cbHandCardCount, AnalyseResult ) ;
					if ( AnalyseResult.cbSignedCount + AnalyseResult.cbThreeCount * 3 == cbHandCardCount )
					{
						bool bContinue = false ;
						for ( BYTE cbThreeIndex = 0; cbThreeIndex < AnalyseResult.cbThreeCount; ++cbThreeIndex )
							if ( GetCardValue(  tmpCardResult.cbCardData[i][0] ) == GetCardValue( AnalyseResult.cbThreeCardData[3 * cbThreeIndex] ) )
							{
								bContinue = true ;
								break ;
							}
						if ( bContinue ) continue ;
					}
				}

				BYTE cbTmpCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, tmpCardResult.cbCardData[i], tmpCardResult.cbEachHandCardCount[i]) ;

				//重新分析
				if(tmpCardResult.cbEachHandCardCount[i]!=cbThisHandCardCount)
				{
					cbThisHandCardCount = tmpCardResult.cbEachHandCardCount[i] ;
					AnalyseOutCardType(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser], 
						tmpCardResult.cbCardData[i], tmpCardResult.cbEachHandCardCount[i] ,BankerCanOutCard) ;
					AnalyseOutCardType(m_cbAllCardData[wFriendID], m_cbUserCardCount[wFriendID],
						tmpCardResult.cbCardData[i], tmpCardResult.cbEachHandCardCount[i] ,FriendOutCardTypeResult) ;
				}

				BYTE cbMaxValue=0 ; 
				BYTE Index = 0 ;

				
				//针对顺子，三条的大牌
				BYTE cbCurrentCardType = GetCardType(tmpCardResult.cbCardData[i], cbThisHandCardCount) ;
				if (cbThisHandCardCount != cbHandCardCount && cbCurrentCardType >= CT_SINGLE_LINE && cbCurrentCardType <= CT_THREE_LINE_TAKE_ONE &&
					( GetCardLogicValue(tmpCardResult.cbCardData[i][cbThisHandCardCount-1]) > GetCardLogicValue(cbHandCardData[cbHandCardCount-2]) || 
						GetCardLogicValue(tmpCardResult.cbCardData[i][0]) >= 11 ))
				{
					BYTE cbRemainCardData[MAX_COUNT], cbRemainCardCount ;
					CopyMemory(cbRemainCardData, cbHandCardData, cbHandCardCount) ;
					cbRemainCardCount = cbHandCardCount ;

					//移除扑克
					RemoveCard(tmpCardResult.cbCardData[i], cbThisHandCardCount, cbRemainCardData, cbRemainCardCount) ;
					cbRemainCardCount -= cbThisHandCardCount ;

					//最大扑克
					BYTE cbCurrentLargestLogicCard = GetCardLogicValue(tmpCardResult.cbCardData[i][0]) ;

					if (GetCardType(cbRemainCardData, cbRemainCardCount) == CT_ERROR && (cbCurrentCardType >= CT_THREE_LINE_TAKE_ONE && 
						cbCurrentCardType <= CT_THREE_LINE_TAKE_ONE && cbCurrentLargestLogicCard >= 11 && cbThisHandCardCount <=5 || 
						cbCurrentCardType == CT_SINGLE_LINE && cbThisHandCardCount <= 6 && cbCurrentLargestLogicCard >= 12 || 
						cbCurrentCardType >= CT_DOUBLE_LINE && cbCurrentCardType <= CT_THREE_LINE && cbCurrentLargestLogicCard >= 12 && cbThisHandCardCount <= 8))
					{
						//暂时不出
						if ( cbCurrentCardType >= CT_SINGLE_LINE && cbCurrentCardType <= CT_THREE_LINE &&
							GetCardLogicValue(tmpCardResult.cbCardData[i][cbThisHandCardCount - 1]) > GetCardLogicValue(cbHandCardData[cbHandCardCount-3]) )
							continue ;

						if ( cbCurrentCardType >= CT_THREE_LINE_TAKE_ONE && cbCurrentCardType <= CT_THREE_LINE_TAKE_ONE &&
							GetCardLogicValue(tmpCardResult.cbCardData[i][0]) > GetCardLogicValue(cbHandCardData[cbHandCardCount-3]) )
							continue ;
					}
				}

				//针对大对（不可先出）
				if (cbCardType == CT_DOUBLE && GetCardLogicValue(tmpCardResult.cbCardData[i][0]) >= 11)
				{
					BYTE cbAllSingleCardData[MAX_COUNT], cbAllSingleCount ;
					cbAllSingleCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, NULL, 0, cbAllSingleCardData) ;
					if (cbAllSingleCount >= 2 && GetCardLogicValue(cbAllSingleCardData[cbAllSingleCount-2]) < 10) continue ;
				}

				//地主可以压牌，而且队友不可以压地主
				if((BankerCanOutCard[cbCardType].cbCardTypeCount>0&&FriendOutCardTypeResult[cbCardType].cbCardTypeCount==0) || (BankerCanOutCard[cbCardType].cbCardTypeCount>0 && FriendOutCardTypeResult[cbCardType].cbCardTypeCount>0 &&
					GetCardLogicValue(FriendOutCardTypeResult[cbCardType].cbCardData[0][0])<=GetCardLogicValue(BankerCanOutCard[cbCardType].cbCardData[0][0])))
				{
					//地主跑掉
					if( BankerCanOutCard[cbCardType].cbEachHandCardCount[0] > 0 && m_cbUserCardCount[m_wBankerUser]<=BankerCanOutCard[cbCardType].cbEachHandCardCount[0]+1)
						continue ;

					//自己不可以再拿回牌权
					//if(GetCardLogicValue(tmpCardResult.cbCardData[0][0]) < GetCardLogicValue(BankerCanOutCard[cbCardType].cbCardData[0][0]) && 
					//	BankerCanOutCard[cbCardType].cbCardTypeCount > 0) 
					//	continue ;
				}
				//是否有大牌
				if(tmpCardResult.cbEachHandCardCount[i] != cbHandCardCount)
				{
					bool bHaveLargeCard=false ;
					for(BYTE j=0; j<tmpCardResult.cbEachHandCardCount[i]; ++j)
						if(GetCardLogicValue(tmpCardResult.cbCardData[i][j])>=15) bHaveLargeCard=true ;

					if(cbCardType!=CT_SINGLE_LINE && cbCardType!=CT_DOUBLE_LINE && GetCardLogicValue(tmpCardResult.cbCardData[i][0])==14) bHaveLargeCard=true ; 

					if(bHaveLargeCard) continue ;
				}

				//地主是否可以走掉，这里都没有考虑炸弹
				if(tmpCardResult.cbEachHandCardCount[i]==m_cbUserCardCount[m_wBankerUser] && cbCardType==GetCardType(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser]) &&
					GetCardLogicValue(m_cbAllCardData[m_wBankerUser][0])>GetCardLogicValue(tmpCardResult.cbCardData[i][0])) continue ;

				//搜索cbMinSingleCardCount[4]的最大值
				for(BYTE j=0; j<4; ++j)
				{
					if(cbMinSingleCardCount[j]>=cbTmpCount)
					{
						cbMinSingleCardCount[j] = cbTmpCount ;
						cbIndex[j] = i ;
						cbOutcardType[j] = cbCardType ;
						break ;
					}
				}

				//保存最小值
				if(cbMinSingleCountInFour>=cbTmpCount)
				{
					//最小牌型
					cbMinCardType = cbCardType ;
					//最小牌型中的最小单牌
					cbMinSingleCountInFour=cbTmpCount ;						
					//最小牌型中的最小牌
					cbMinIndex=i ;
				}
			}
		}

		if(cbMinSingleCountInFour>=AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, NULL, 0)+3 && 
			m_cbUserCardCount[m_wBankerUser]>4)
			cbMinSingleCountInFour=MAX_COUNT ;

		if(cbMinSingleCountInFour!=MAX_COUNT)
		{
			BYTE Index = cbMinIndex ;

			//选择最小牌
			for(BYTE i=0; i<4; ++i)
			{
				if(cbOutcardType[i]==cbMinCardType && cbMinSingleCardCount[i]<=cbMinSingleCountInFour &&
					GetCardLogicValue(MeOutCardTypeResult[cbMinCardType].cbCardData[cbIndex[i]][0])<GetCardLogicValue(MeOutCardTypeResult[cbMinCardType].cbCardData[Index][0]))
					Index = cbIndex[i] ;
			}

			//对王加一只
			if(cbHandCardCount==3 && GetCardColor(cbHandCardData[0])==0x40 && GetCardColor(cbHandCardData[1])==0x40)
			{
				OutCardResult.cbCardCount = 2 ;
				OutCardResult.cbResultCard[0] = 0x4f ;
				OutCardResult.cbResultCard[1] = 0x4e ;
				return ;
			}
			//对王
			else if(cbHandCardCount==2 && GetCardColor(cbHandCardData[0])==0x40 && GetCardColor(cbHandCardData[1])==0x40)
			{
				OutCardResult.cbCardCount = 2 ;
				OutCardResult.cbResultCard[0] = 0x4f ;
				OutCardResult.cbResultCard[1] = 0x4e ;
				return ;
			}
			else
			{
				//设置变量
				OutCardResult.cbCardCount=MeOutCardTypeResult[cbMinCardType].cbEachHandCardCount[Index];
				CopyMemory(OutCardResult.cbResultCard,MeOutCardTypeResult[cbMinCardType].cbCardData[Index],MeOutCardTypeResult[cbMinCardType].cbEachHandCardCount[Index]*sizeof(BYTE));
				return ;
			}

			ASSERT(OutCardResult.cbCardCount>0) ;

			return ;
		}

		//如果地主扑克少于5，还没有找到适合的牌则从大出到小
		if(OutCardResult.cbCardCount<=0 && m_cbUserCardCount[m_wBankerUser]<=5)
		{
			//只有一张牌时不能放地主走
			if(m_cbUserCardCount[m_wBankerUser]==1 && MeOutCardTypeResult[CT_SINGLE].cbCardTypeCount>0)
			{
				//最小一张
				BYTE Index=MAX_COUNT ;
				for(BYTE i=0; i<MeOutCardTypeResult[CT_SINGLE].cbCardTypeCount; ++i)
				{
					if(GetCardLogicValue(MeOutCardTypeResult[CT_SINGLE].cbCardData[i][0])>=GetCardLogicValue(m_cbAllCardData[m_wBankerUser][0]))
					{
						Index=i ;
					}
					else break ;
				}

				if(MAX_COUNT!=Index)
				{
					OutCardResult.cbCardCount = MeOutCardTypeResult[CT_SINGLE].cbEachHandCardCount[Index] ;
					CopyMemory(OutCardResult.cbResultCard, MeOutCardTypeResult[CT_SINGLE].cbCardData[Index], OutCardResult.cbCardCount) ;
					return ;
				}
			}
		}
	}

	BYTE cbFirstCard=0 ;
	//过滤王和2
	for(BYTE i=0; i<cbHandCardCount; ++i) 
		if(GetCardLogicValue(cbHandCardData[i])<15)
		{
			cbFirstCard = i ;
			break ;
		}

		if(cbFirstCard<cbHandCardCount-1)
			AnalyseOutCardType(cbHandCardData+cbFirstCard, cbHandCardCount-cbFirstCard, CardTypeResult) ;
		else
			AnalyseOutCardType(cbHandCardData, cbHandCardCount, CardTypeResult) ;

		//计算单牌
		BYTE cbMinSingleCardCount[4] ;
		cbMinSingleCardCount[0]=MAX_COUNT ;
		cbMinSingleCardCount[1]=MAX_COUNT ;
		cbMinSingleCardCount[2]=MAX_COUNT ;
		cbMinSingleCardCount[3]=MAX_COUNT ;
		BYTE cbIndex[4]={0} ;
		BYTE cbOutcardType[4]={CT_ERROR} ;
		BYTE cbMinValue=MAX_COUNT ; 
		BYTE cbMinSingleCountInFour=MAX_COUNT ;
		BYTE cbMinCardType=CT_ERROR ;
		BYTE cbMinIndex=0 ;

		//除炸弹外的牌
		for(BYTE cbCardType=CT_SINGLE; cbCardType<CT_BOMB_CARD; ++cbCardType)
		{
			tagOutCardTypeResult const &tmpCardResult = CardTypeResult[cbCardType] ;
			for(BYTE i=0; i<tmpCardResult.cbCardTypeCount; ++i)
			{
				//庄家可以走掉
				if ( CompareCard(tmpCardResult.cbCardData[i], m_cbAllCardData[m_wBankerUser], tmpCardResult.cbEachHandCardCount[i], m_cbUserCardCount[m_wBankerUser]) )
					continue ;

				//针对顺子，三条的大牌
				if ( tmpCardResult.cbEachHandCardCount[i] != cbHandCardCount && cbCardType >= CT_SINGLE_LINE && cbCardType <= CT_THREE_LINE_TAKE_ONE &&
					( GetCardLogicValue(tmpCardResult.cbCardData[i][tmpCardResult.cbEachHandCardCount[i]-1]) > GetCardLogicValue(cbHandCardData[cbHandCardCount-2]) || 
						GetCardLogicValue(tmpCardResult.cbCardData[i][0]) >= 11 ))
				{
					BYTE cbRemainCardData[MAX_COUNT], cbRemainCardCount ;
					CopyMemory(cbRemainCardData, cbHandCardData, cbHandCardCount) ;
					cbRemainCardCount = cbHandCardCount ;

					//移除扑克
					RemoveCard(tmpCardResult.cbCardData[i], tmpCardResult.cbEachHandCardCount[i], cbRemainCardData, cbRemainCardCount) ;
					cbRemainCardCount -= tmpCardResult.cbEachHandCardCount[i] ;

					//最大扑克
					BYTE cbCurrentLargestLogicCard = GetCardLogicValue(tmpCardResult.cbCardData[i][0]) ;

					if (GetCardType(cbRemainCardData, cbRemainCardCount) == CT_ERROR && (cbCardType >= CT_THREE_LINE_TAKE_ONE && 
						cbCardType <= CT_THREE_LINE_TAKE_ONE && cbCurrentLargestLogicCard >= 11 && tmpCardResult.cbEachHandCardCount[i] <=5 || 
						cbCardType == CT_SINGLE_LINE && tmpCardResult.cbEachHandCardCount[i] <= 6 && cbCurrentLargestLogicCard >= 12 || 
						cbCardType >= CT_DOUBLE_LINE && cbCardType <= CT_THREE_LINE && cbCurrentLargestLogicCard >= 12 && tmpCardResult.cbEachHandCardCount[i] <= 8))
					{
						//暂时不出
						if ( cbCardType >= CT_SINGLE_LINE && cbCardType <= CT_THREE_LINE &&
							GetCardLogicValue(tmpCardResult.cbCardData[i][tmpCardResult.cbEachHandCardCount[i] - 1]) > GetCardLogicValue(cbHandCardData[cbHandCardCount-3]) )
							continue ;

						if ( cbCardType >= CT_THREE_LINE_TAKE_ONE && cbCardType <= CT_THREE_LINE_TAKE_ONE &&
							GetCardLogicValue(tmpCardResult.cbCardData[i][0]) > GetCardLogicValue(cbHandCardData[cbHandCardCount-3]) )
							continue ;
					}
				}

				BYTE cbTmpCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, tmpCardResult.cbCardData[i], tmpCardResult.cbEachHandCardCount[i]) ;

				BYTE cbMaxValue=0 ; 
				BYTE Index = 0 ;

				//搜索cbMinSingleCardCount[4]的最大值
				for(BYTE j=0; j<4; ++j)
				{
					if(cbMinSingleCardCount[j]>=cbTmpCount)
					{
						cbMinSingleCardCount[j] = cbTmpCount ;
						cbIndex[j] = i ;
						cbOutcardType[j] = cbCardType ;
						break ;
					}
				}

				//保存最小值
				if(cbMinSingleCountInFour>=cbTmpCount)
				{
					//最小牌型
					cbMinCardType = cbCardType ;
					//最小牌型中的最小单牌
					cbMinSingleCountInFour=cbTmpCount ;						
					//最小牌型中的最小牌
					cbMinIndex=i ;
				}
			}
		}

		if(cbMinSingleCountInFour!=MAX_COUNT)
		{
			BYTE Index = cbMinIndex ;

			//选择最小牌
			for(BYTE i=0; i<4; ++i)
			{
				if(cbOutcardType[i]==cbMinCardType && cbMinSingleCardCount[i]<=cbMinSingleCountInFour &&
					GetCardLogicValue(CardTypeResult[cbMinCardType].cbCardData[cbIndex[i]][0])<GetCardLogicValue(CardTypeResult[cbMinCardType].cbCardData[Index][0]))
					Index = cbIndex[i] ;
			}

			//对王加一只
			if(cbHandCardCount==3 && GetCardColor(cbHandCardData[0])==0x40 && GetCardColor(cbHandCardData[1])==0x40)
			{
				OutCardResult.cbCardCount = 2 ;
				OutCardResult.cbResultCard[0] = 0x4f ;
				OutCardResult.cbResultCard[1] = 0x4e ;
				return ;
			}
			//对王
			else if(cbHandCardCount==2 && GetCardColor(cbHandCardData[0])==0x40 && GetCardColor(cbHandCardData[1])==0x40)
			{
				OutCardResult.cbCardCount = 2 ;
				OutCardResult.cbResultCard[0] = 0x4f ;
				OutCardResult.cbResultCard[1] = 0x4e ;
				return ;
			}
			else
			{
				//设置变量
				OutCardResult.cbCardCount=CardTypeResult[cbMinCardType].cbEachHandCardCount[Index];
				CopyMemory(OutCardResult.cbResultCard,CardTypeResult[cbMinCardType].cbCardData[Index],CardTypeResult[cbMinCardType].cbEachHandCardCount[Index]*sizeof(BYTE));
				return ;
			}

			ASSERT(OutCardResult.cbCardCount>0) ;

			return ;
		}
		//如果只剩炸弹
		else if (CardTypeResult[CT_BOMB_CARD].cbCardTypeCount > 0) 
		{	
			//BYTE Index=0 ;
			//BYTE cbLogicCardValue = GetCardLogicValue(0x4F)+1 ;
			////最小炸弹
			//for(BYTE i=0; i<CardTypeResult[CT_BOMB_CARD].cbCardTypeCount; ++i)
			//	if(cbLogicCardValue>GetCardLogicValue(CardTypeResult[CT_BOMB_CARD].cbCardData[i][0]))
			//	{
			//		cbLogicCardValue = GetCardLogicValue(CardTypeResult[CT_BOMB_CARD].cbCardData[i][0]) ;
			//		Index = i ;
			//	}

			//	//设置变量
			//	OutCardResult.cbCardCount=CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[Index];
			//	CopyMemory(OutCardResult.cbResultCard,CardTypeResult[CT_BOMB_CARD].cbCardData[Index],CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[Index]*sizeof(BYTE));

			//	return ;
		}

		BYTE cbAllSingleCardData[MAX_COUNT], cbAllSingleCardCount ;
		cbAllSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, NULL, 0, cbAllSingleCardData) ;

		if ( cbAllSingleCardCount > 0 )
		{
			//如果都没有搜索到就出最小的一张
			OutCardResult.cbCardCount = 1 ;
			OutCardResult.cbResultCard[0] = cbAllSingleCardData[cbAllSingleCardCount-1] ;

			return ;
		}

		//如果都没有搜索到就出最小的一张
		OutCardResult.cbCardCount = 1 ;
		OutCardResult.cbResultCard[0] = cbHandCardData[cbHandCardCount-1] ;
		return ;
}
//地主下家（后出牌）
void CGameLogic::UndersideOfBankerOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wOutCardUser, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResult & OutCardResult)
{
	//初始变量
	ZeroMemory(&OutCardResult, sizeof(OutCardResult)) ;

	//零下标没用
	tagOutCardTypeResult CardTypeResult[12+1] ;
	ZeroMemory(CardTypeResult, sizeof(CardTypeResult)) ;

	//出牌类型
	BYTE cbOutCardType = GetCardType(cbTurnCardData,cbTurnCardCount) ;

	//搜索可出牌
	tagOutCardTypeResult BankerOutCardTypeResult[13] ;
	ZeroMemory(BankerOutCardTypeResult, sizeof(BankerOutCardTypeResult)) ;

	AnalyseOutCardType(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser], BankerOutCardTypeResult) ;
	AnalyseOutCardType(cbHandCardData,cbHandCardCount,cbTurnCardData,cbTurnCardCount, CardTypeResult) ;

	//只剩炸弹
	if(cbHandCardCount==CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[0] && (cbOutCardType<CT_BOMB_CARD || 
		GetCardLogicValue(CardTypeResult[CT_BOMB_CARD].cbCardData[0][0])>GetCardLogicValue(cbTurnCardData[0])))
	{
		OutCardResult.cbCardCount = CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[0] ;
		CopyMemory(OutCardResult.cbResultCard,  CardTypeResult[CT_BOMB_CARD].cbCardData, OutCardResult.cbCardCount) ;

		return ;
	}
	//双王炸弹和一手
	else if(cbHandCardCount>2 && cbHandCardData[0]==0x4f && cbHandCardData[1]==0x4e && CT_ERROR!=GetCardType(cbHandCardData+2, cbHandCardCount-2) &&
		CT_FOUR_LINE_TAKE_ONE != GetCardType(cbHandCardData+2, cbHandCardCount-2) && CT_FOUR_LINE_TAKE_TWO != GetCardType(cbHandCardData+2, cbHandCardCount-2))
	{
		OutCardResult.cbCardCount = 2 ;
		OutCardResult.cbResultCard[0] = 0x4f ;
		OutCardResult.cbResultCard[1] = 0x4e ;
		return ;
	}

	//炸弹和一手
	BYTE cbRemainCard[MAX_COUNT], cbRemainCount=0;
	BYTE cbAllBombCard[MAX_COUNT], cbAllBombCount=0 ;
	GetAllBomCard(cbHandCardData, cbHandCardCount, cbAllBombCard, cbAllBombCount) ;

	//出炸判断
	if(cbAllBombCount>0 && wOutCardUser==m_wBankerUser)
	{
		//剩余扑克
		CopyMemory(cbRemainCard, cbHandCardData, cbHandCardCount) ;
		cbRemainCount = cbHandCardCount ;
		RemoveCard(cbAllBombCard, cbAllBombCount, cbRemainCard, cbRemainCount) ;
		cbRemainCount -= cbAllBombCount ;
		
		if(CT_ERROR != GetCardType(cbRemainCard, cbRemainCount) || 
			(2==cbRemainCount && GetCardLogicValue(cbRemainCard[0])>GetCardLogicValue(m_cbAllCardData[m_wBankerUser][0])))
		{
			if((cbOutCardType<CT_BOMB_CARD || GetCardLogicValue(cbAllBombCard[0])>GetCardLogicValue(cbTurnCardData[0])) &&
				( CardTypeResult[cbOutCardType].cbCardTypeCount <= 0 || CT_ERROR != GetCardType(cbRemainCard, cbRemainCount)) )
			{
				//双王炸弹
				if(GetCardColor(cbAllBombCard[0])==0x40)
				{
					OutCardResult.cbCardCount = 2 ;
					OutCardResult.cbResultCard[0] = 0x4f ;
					OutCardResult.cbResultCard[1] = 0x4e ;
					return ;
				}
				else
				{
					//分析地主牌
					BYTE cbBankerAllBombCard[MAX_COUNT], cbBankerAllBombCardCount ;
					GetAllBomCard( m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser], cbBankerAllBombCard, cbBankerAllBombCardCount) ;

					if ( !CompareCard( cbTurnCardData, cbRemainCard, cbTurnCardCount, cbRemainCount) ||  cbBankerAllBombCardCount <= 0 || 
						GetCardLogicValue( cbAllBombCard[0] ) > GetCardLogicValue( cbBankerAllBombCard[0] ) )
					{
						OutCardResult.cbCardCount = 4 ;
						CopyMemory(OutCardResult.cbResultCard, cbAllBombCard, 4) ;
						return ;
					}
				}
			}
		}
	}

	//只剩一手出炸
	if ( CardTypeResult[CT_BOMB_CARD].cbCardTypeCount > 0 && GetCardType(m_cbAllCardData[(m_wBankerUser + 2) % GAME_PLAYER], m_cbUserCardCount[(m_wBankerUser + 2) % GAME_PLAYER]) != CT_ERROR &&
		GetCardType(m_cbAllCardData[(m_wBankerUser + 2) % GAME_PLAYER], m_cbUserCardCount[(m_wBankerUser + 2) % GAME_PLAYER]) <= CT_DOUBLE )
	{
		//只剩单牌
		if ( GetCardType(m_cbAllCardData[(m_wBankerUser + 2) % GAME_PLAYER], m_cbUserCardCount[(m_wBankerUser + 2) % GAME_PLAYER]) == CT_SINGLE )
		{
			BYTE cbIndex = CardTypeResult[CT_BOMB_CARD].cbCardTypeCount - 1 ;
			BYTE cbCardCount = CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[cbIndex] ;
			CopyMemory(OutCardResult.cbResultCard, CardTypeResult[CT_BOMB_CARD].cbCardData[cbIndex], cbCardCount) ;
			OutCardResult.cbCardCount = cbCardCount ;

			return ;
		}
		else if ( GetCardType(m_cbAllCardData[(m_wBankerUser + 2) % GAME_PLAYER], m_cbUserCardCount[(m_wBankerUser + 2) % GAME_PLAYER]) == CT_DOUBLE )
		{
			BYTE cbAllDoubleCard[MAX_COUNT], cbAllDoubleCount ;
			GetAllDoubleCard(cbHandCardData, cbHandCardCount, cbAllDoubleCard, cbAllDoubleCount) ;

			if ( cbAllDoubleCount > 0 && GetCardLogicValue(cbAllDoubleCard[cbAllDoubleCount - 1]) <= 10)
			{
				BYTE cbIndex = CardTypeResult[CT_BOMB_CARD].cbCardTypeCount - 1 ;
				BYTE cbCardCount = CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[cbIndex] ;
				CopyMemory(OutCardResult.cbResultCard, CardTypeResult[CT_BOMB_CARD].cbCardData[cbIndex], cbCardCount) ;
				OutCardResult.cbCardCount = cbCardCount ;

				return ;
			}			
		}

	}

	//只有一手牌
	if ( GetCardType(cbHandCardData, cbHandCardCount) != CT_FOUR_LINE_TAKE_ONE && GetCardType(cbHandCardData, cbHandCardCount) != CT_FOUR_LINE_TAKE_TWO &&
		CompareCard(cbTurnCardData, cbHandCardData, cbTurnCardCount, cbHandCardCount) )
	{
		OutCardResult.cbCardCount = cbHandCardCount ;
		CopyMemory(OutCardResult.cbResultCard, cbHandCardData, cbHandCardCount) ;

		return ;
	}

	//对牌接牌判断
	if (1 == m_cbUserCardCount[m_wBankerUser] && cbHandCardCount >= 2 && cbOutCardType == CT_DOUBLE)
	{
		tagAnalyseResult AnalyseResult ;
		ZeroMemory(&AnalyseResult, sizeof(AnalyseResult)) ;

		//分析扑克
		AnalysebCardData(cbHandCardData, cbHandCardCount, AnalyseResult) ;

		//对牌判断
		if (cbHandCardCount == (AnalyseResult.cbDoubleCount * 2 + AnalyseResult.cbThreeCount * 3 + AnalyseResult.cbFourCount * 4) || 
			cbHandCardCount == (AnalyseResult.cbDoubleCount * 2 + AnalyseResult.cbThreeCount * 3 + AnalyseResult.cbFourCount * 4 + 1))
		{
			//出对判断
			for (int nIndex = AnalyseResult.cbDoubleCount-1; nIndex>=0 ; --nIndex)
			{
				if (GetCardLogicValue(AnalyseResult.cbDoubleCardData[nIndex*2]) > GetCardLogicValue(cbTurnCardData[0]))
				{
					OutCardResult.cbCardCount = 2 ;
					OutCardResult.cbResultCard[0] = AnalyseResult.cbDoubleCardData[nIndex*2] ;
					OutCardResult.cbResultCard[1] = AnalyseResult.cbDoubleCardData[nIndex*2+1] ;
					return ;
				}
			}

			//出炸判断
			if (AnalyseResult.cbFourCount > 0) 
			{
				//最小炸弹
				BYTE cbLestBombIndex = AnalyseResult.cbFourCount-1 ;

				OutCardResult.cbCardCount = 4 ;
				OutCardResult.cbResultCard[0] = AnalyseResult.cbFourCardData[cbLestBombIndex*4] ;
				OutCardResult.cbResultCard[1] = AnalyseResult.cbFourCardData[cbLestBombIndex*4+1] ;
				OutCardResult.cbResultCard[2] = AnalyseResult.cbFourCardData[cbLestBombIndex*4+2] ;
				OutCardResult.cbResultCard[3] = AnalyseResult.cbFourCardData[cbLestBombIndex*4+3] ;

				return ;
			}
            			
		}

		//分析对家
		if ( wOutCardUser != m_wBankerUser )
		{
			tagAnalyseResult FriendAnalyseResult ;
			ZeroMemory( &FriendAnalyseResult, sizeof( FriendAnalyseResult ) ) ;

			//分析扑克
			AnalysebCardData( m_cbAllCardData[wOutCardUser], m_cbUserCardCount[wOutCardUser], FriendAnalyseResult ) ;

			//对牌判断
			if ( m_cbUserCardCount[wOutCardUser] == (FriendAnalyseResult.cbDoubleCount * 2 + FriendAnalyseResult.cbThreeCount * 3 + FriendAnalyseResult.cbFourCount * 4) || 
				 m_cbUserCardCount[wOutCardUser] == (FriendAnalyseResult.cbDoubleCount * 2 + FriendAnalyseResult.cbThreeCount * 3 + FriendAnalyseResult.cbFourCount * 4 + 1))
			{
				return ;
			}

			//零下标没用
			tagOutCardTypeResult FriendCardTypeResult[12+1] ;
			ZeroMemory( FriendCardTypeResult, sizeof( FriendCardTypeResult ) ) ;

			AnalyseOutCardType( m_cbAllCardData[wOutCardUser], m_cbUserCardCount[wOutCardUser], FriendCardTypeResult ) ;

			for ( BYTE cbLineCardIdx = 0; cbLineCardIdx < FriendCardTypeResult[CT_SINGLE_LINE].cbCardTypeCount; ++cbLineCardIdx )
			{
				//剩余扑克
				BYTE cbRemainCardData[MAX_COUNT], cbRemainCardCount ;

				cbRemainCardCount = m_cbUserCardCount[wOutCardUser] ;
				CopyMemory( cbRemainCardData, m_cbAllCardData[wOutCardUser], cbRemainCardCount ) ;
				RemoveCard( FriendCardTypeResult[CT_SINGLE_LINE].cbCardData[cbLineCardIdx],  FriendCardTypeResult[CT_SINGLE_LINE].cbEachHandCardCount[cbLineCardIdx], 
					cbRemainCardData, cbRemainCardCount ) ;

				cbRemainCardCount -= FriendCardTypeResult[CT_SINGLE_LINE].cbEachHandCardCount[cbLineCardIdx] ;

				//分析扑克
				AnalysebCardData( cbRemainCardData, cbRemainCardCount, FriendAnalyseResult ) ;

				//对牌判断
				if (cbRemainCardCount == (FriendAnalyseResult.cbDoubleCount * 2 + FriendAnalyseResult.cbThreeCount * 3 + FriendAnalyseResult.cbFourCount * 4) || 
					cbRemainCardCount == (FriendAnalyseResult.cbDoubleCount * 2 + FriendAnalyseResult.cbThreeCount * 3 + FriendAnalyseResult.cbFourCount * 4 + 1))
				{
					return ;
				}
			}

			for ( BYTE cbLineCardIdx = 0; cbLineCardIdx < FriendCardTypeResult[CT_DOUBLE_LINE].cbCardTypeCount; ++cbLineCardIdx )
			{
				//剩余扑克
				BYTE cbRemainCardData[MAX_COUNT], cbRemainCardCount ;

				cbRemainCardCount = m_cbUserCardCount[wOutCardUser] ;
				CopyMemory( cbRemainCardData, m_cbAllCardData[wOutCardUser], cbRemainCardCount ) ;
				RemoveCard( FriendCardTypeResult[CT_SINGLE_LINE].cbCardData[cbLineCardIdx],  FriendCardTypeResult[CT_SINGLE_LINE].cbEachHandCardCount[cbLineCardIdx], 
					cbRemainCardData, cbRemainCardCount ) ;

				cbRemainCardCount -= FriendCardTypeResult[CT_SINGLE_LINE].cbEachHandCardCount[cbLineCardIdx] ;

				//分析扑克
				AnalysebCardData( cbRemainCardData, cbRemainCardCount, FriendAnalyseResult ) ;

				//对牌判断
				if (cbRemainCardCount == (FriendAnalyseResult.cbDoubleCount * 2 + FriendAnalyseResult.cbThreeCount * 3 + FriendAnalyseResult.cbFourCount * 4) || 
					cbRemainCardCount == (FriendAnalyseResult.cbDoubleCount * 2 + FriendAnalyseResult.cbThreeCount * 3 + FriendAnalyseResult.cbFourCount * 4 + 1))
				{
					return ;
				}
			}


		}
	}


	//对家可否出完
	if( m_wBankerUser != wOutCardUser && ! CompareCard( cbTurnCardData, m_cbAllCardData[ m_wBankerUser ], cbTurnCardCount, m_cbUserCardCount[ m_wBankerUser ] ) )
	{
		//庄家扑克
		bool bBankerCanOut = false ;
		tagOutCardTypeResult BankerOutCardResult[12+1] ;
		ZeroMemory(BankerOutCardResult, sizeof(BankerOutCardResult)) ;

        //分析扑克
		AnalyseOutCardType(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser], cbTurnCardData, cbTurnCardCount, BankerOutCardResult) ;
		for(BYTE cbCardType=CT_SINGLE; cbCardType<=CT_MISSILE_CARD; ++cbCardType) if(BankerOutCardResult[cbCardType].cbCardTypeCount>0) bBankerCanOut = true ; 

		if(!bBankerCanOut)
		{
		//对家ID
		WORD wFriendChairID = (m_wBankerUser+2)%GAME_PLAYER ;

		//分析扑克
		tagOutCardTypeResult FriendCardTypeResult[12+1] ;
		ZeroMemory(FriendCardTypeResult, sizeof(FriendCardTypeResult)) ;
		AnalyseOutCardType(m_cbAllCardData[wFriendChairID], m_cbUserCardCount[wFriendChairID], FriendCardTypeResult) ;

		for(BYTE cbCardType=CT_SINGLE; cbCardType<=CT_MISSILE_CARD; ++cbCardType)
			if(FriendCardTypeResult[cbCardType].cbCardTypeCount>0)
			{
				for(LONG lIndex=0; lIndex<FriendCardTypeResult[cbCardType].cbCardTypeCount; ++lIndex)
				{

					if(TestOutAllCard(wFriendChairID, FriendCardTypeResult[cbCardType].cbCardData[lIndex], FriendCardTypeResult[cbCardType].cbEachHandCardCount[lIndex], true))
					{
						//不压对家
						return ;
					}
				}
			}
		}
	}

	//放走对家
	if (GetCardType(m_cbAllCardData[(m_wBankerUser + 2) % GAME_PLAYER], m_cbUserCardCount[(m_wBankerUser + 2) % GAME_PLAYER]) == GetCardType(cbTurnCardData, cbTurnCardCount) &&
		CompareCard(cbTurnCardData, m_cbAllCardData[(m_wBankerUser + 2) % GAME_PLAYER], cbTurnCardCount, m_cbUserCardCount[(m_wBankerUser + 2) % GAME_PLAYER]))
		return ;

	if (CompareCard(cbTurnCardData, m_cbAllCardData[(m_wBankerUser + 2) % GAME_PLAYER], cbTurnCardCount, m_cbUserCardCount[(m_wBankerUser + 2) % GAME_PLAYER])) return ;

	//判断可否出完
	BYTE cbSingleCardCount = MAX_COUNT+CT_MISSILE_CARD ;
	bool bFindBestCard = false ;
	for(BYTE cbCardType=CT_SINGLE; cbCardType<=CT_MISSILE_CARD; ++cbCardType)
		if(CardTypeResult[cbCardType].cbCardTypeCount>0)
		{
			for(LONG lIndex=0; lIndex<CardTypeResult[cbCardType].cbCardTypeCount; ++lIndex)
			{
				WORD wMeChairID = (m_wBankerUser+1)%GAME_PLAYER ;

				if(TestOutAllCard(wMeChairID, CardTypeResult[cbCardType].cbCardData[lIndex], CardTypeResult[cbCardType].cbEachHandCardCount[lIndex], false))
				{
					//计算单牌
					BYTE cbTmpSingleCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, CardTypeResult[cbCardType].cbCardData[lIndex], 
						CardTypeResult[cbCardType].cbEachHandCardCount[lIndex]) ;

					//结果判断
					if (cbTmpSingleCount >= MAX_COUNT) continue ;

					//炸弹优先级排后
					BYTE cbBombCardType = GetCardType(CardTypeResult[cbCardType].cbCardData[lIndex], CardTypeResult[cbCardType].cbEachHandCardCount[lIndex]) ;
					if (cbBombCardType == CT_BOMB_CARD) cbTmpSingleCount += 4 ;
					else if (cbBombCardType == CT_MISSILE_CARD) cbTmpSingleCount += 5 ;

					////改变权值
					//if (cbBombCardType != CT_ERROR) cbTmpSingleCount += cbBombCardType ;

					//不出炸弹
					//BYTE cbWantOutCardType = GetCardType(CardTypeResult[cbCardType].cbCardData[lIndex], CardTypeResult[cbCardType].cbEachHandCardCount[lIndex]) ;
					//if (CardTypeResult[cbOutCardType].cbCardTypeCount > 0 && cbOutCardType < CT_BOMB_CARD && cbWantOutCardType >= CT_BOMB_CARD) continue ;

					if(cbTmpSingleCount <= cbSingleCardCount)
					{
						//设置变量
						OutCardResult.cbCardCount=CardTypeResult[cbCardType].cbEachHandCardCount[lIndex];
						CopyMemory(OutCardResult.cbResultCard, CardTypeResult[cbCardType].cbCardData[lIndex], 
						CardTypeResult[cbCardType].cbEachHandCardCount[lIndex]*sizeof(BYTE));
						cbSingleCardCount = cbTmpSingleCount ;
						bFindBestCard = true ;
					}
				}
			}
		}
		//直接返回
		if (bFindBestCard) return ;

	//取出四个最小单牌
	BYTE cbMinSingleCardCount[4] ;
	cbMinSingleCardCount[0]=MAX_COUNT ;
	cbMinSingleCardCount[1]=MAX_COUNT ;
	cbMinSingleCardCount[2]=MAX_COUNT ;
	cbMinSingleCardCount[3]=MAX_COUNT ;
	BYTE cbIndex[4]={0} ;	
	BYTE cbMinSingleCountInFour=MAX_COUNT ;

	//可出扑克（这里已经过滤掉炸弹了）
	tagOutCardTypeResult const &CanOutCard = CardTypeResult[cbOutCardType] ;

	for(BYTE i=0; i<CanOutCard.cbCardTypeCount; ++i)
	{
		//最小单牌
		BYTE cbTmpCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount,CanOutCard.cbCardData[i], CanOutCard.cbEachHandCardCount[i]) ; 
		BYTE cbMaxValue=0 ; 
		BYTE Index = 0 ;

		//搜索cbMinSingleCardCount[4]的最大值
		for(BYTE j=0; j<4; ++j)
		{
			if(cbMinSingleCardCount[j]>=cbTmpCount)
			{
				cbMinSingleCardCount[j] = cbTmpCount ;
				cbIndex[j] = i ;
				break ;
			}
		}

	}

	for(BYTE i=0; i<4; ++i)
		if(cbMinSingleCountInFour>cbMinSingleCardCount[i]) cbMinSingleCountInFour = cbMinSingleCardCount[i] ;


	//原始单牌数
	BYTE cbOriginSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount,NULL,0) ;

	//朋友出牌
	bool bFriendOut = m_wBankerUser!=wOutCardUser ;
	if(bFriendOut)
	{
		//不拦截朋友最后一手牌
		if (GetCardType(m_cbAllCardData[(2 + m_wBankerUser) % GAME_PLAYER], m_cbUserCardCount[(2 + m_wBankerUser) % GAME_PLAYER]) != CT_ERROR) return ;

		//在上面的TestOutAllCard中已对可出炸弹情况分析过
		if(CanOutCard.cbCardTypeCount > 0 && CanOutCard.cbCardType < CT_BOMB_CARD &&  cbMinSingleCountInFour < MAX_COUNT)
		{
			BYTE cbMinLogicCardValue = GetCardLogicValue(0x4F)+1 ;
			bool bFindCard = false ;
			BYTE cbCanOutIndex=0 ;
			for(BYTE i=0; i<4; ++i)
			{
				BYTE Index = cbIndex[i] ;
				
				//三带，和连牌不接对家牌
				if ( CanOutCard.cbCardType >= CT_THREE &&  CanOutCard.cbCardType <= CT_MISSILE_CARD && GetCardLogicValue(CanOutCard.cbCardData[Index][0]) >= 7 && 
					CanOutCard.cbEachHandCardCount[Index] <=5 )
					continue ;
				
				//小于J的牌，或者小于K而且是散牌
				if((cbMinSingleCardCount[i]<cbOriginSingleCardCount+3 &&  (cbMinSingleCardCount[i]<=cbMinSingleCountInFour || cbMinSingleCardCount[i]<=cbMinSingleCountInFour+1 && 
				CanOutCard.cbCardType >= CT_THREE_LINE_TAKE_ONE && CanOutCard.cbCardType <= CT_THREE_LINE_TAKE_ONE ) && 
					(GetCardLogicValue(CanOutCard.cbCardData[Index][0])<=11 || (cbMinSingleCardCount[i]<cbOriginSingleCardCount)&&GetCardLogicValue(CanOutCard.cbCardData[Index][0])<=13)) &&
					cbMinLogicCardValue>GetCardLogicValue(CanOutCard.cbCardData[Index][0]) && cbHandCardCount>5)
				{
					//搜索有没有大牌（针对飞机带翅膀后面的带牌）
					bool bNoLargeCard = true ;
					for(BYTE k=3; k<CanOutCard.cbEachHandCardCount[Index]; ++k)
					{
						//有大牌而且不能一次出完
						if(GetCardLogicValue(CanOutCard.cbCardData[Index][k])>=15 && 
							CanOutCard.cbEachHandCardCount[Index]!=cbHandCardCount) bNoLargeCard = false ;
					}
					if(bNoLargeCard)
					{
						bFindCard = true ;
						cbCanOutIndex = Index ; 
						cbMinLogicCardValue = GetCardLogicValue(CanOutCard.cbCardData[Index][0]) ;
					}
				}
				else if(cbHandCardCount<5 && cbMinSingleCardCount[i]<cbOriginSingleCardCount+4 && cbMinSingleCardCount[i]<=cbMinSingleCountInFour &&
					cbMinLogicCardValue>GetCardLogicValue(CanOutCard.cbCardData[Index][0]))
				{
					//能出王打自家的2
					if ( GetCardLogicValue( CanOutCard.cbCardData[Index][0] ) >= 16 && GetCardLogicValue( cbTurnCardData[0] ) >= 15 )
						continue ;

					bFindCard = true ;
					cbCanOutIndex = Index ; 
					cbMinLogicCardValue = GetCardLogicValue(CanOutCard.cbCardData[Index][0]) ;
				}
			}

			if(bFindCard)
			{

				//设置变量
				OutCardResult.cbCardCount=CanOutCard.cbEachHandCardCount[cbCanOutIndex];
				CopyMemory(OutCardResult.cbResultCard,CanOutCard.cbCardData[cbCanOutIndex],CanOutCard.cbEachHandCardCount[cbCanOutIndex]*sizeof(BYTE));

				return ;
			}
			//手上少于五张牌
			else if(cbHandCardCount<=5)
			{
				BYTE cbMinLogicCard = GetCardLogicValue(0x4f)+1 ;
				BYTE cbCanOutIndex = 0 ;
				for(BYTE i=0; i<4; ++i)
					if(cbMinSingleCardCount[i]<MAX_COUNT && cbMinSingleCardCount[i]<=cbMinSingleCountInFour && cbMinLogicCard>GetCardLogicValue(CanOutCard.cbCardData[cbIndex[i]][0]) && 
						GetCardLogicValue(CanOutCard.cbCardData[cbIndex[i]][0])<=14)
					{
						cbMinLogicCard = GetCardLogicValue(CanOutCard.cbCardData[cbIndex[i]][0]) ;
						cbCanOutIndex = cbIndex[i] ;
					}

					if(cbMinLogicCard != (GetCardLogicValue(0x4f)+1))
					{
						//设置变量
						OutCardResult.cbCardCount=CanOutCard.cbEachHandCardCount[cbCanOutIndex];
						CopyMemory(OutCardResult.cbResultCard,CanOutCard.cbCardData[cbCanOutIndex],CanOutCard.cbEachHandCardCount[cbCanOutIndex]*sizeof(BYTE));

						return ;
					}
			}

			return ;
		}
		else
		{
			return ;
		}

	}
	//地主出牌
	else
	{
		if(CanOutCard.cbCardTypeCount>0 && cbMinSingleCountInFour < MAX_COUNT)
		{
			BYTE cbMinLogicCardValue = GetCardLogicValue(0x4F)+1 ;
			bool bFindCard = false ;
			BYTE cbCanOutIndex=0 ;
			for(BYTE i=0; i<4; ++i)
			{
				BYTE Index = cbIndex[i] ;

				if((cbMinSingleCardCount[i]<cbOriginSingleCardCount+4)  &&  (cbMinSingleCardCount[i]<=cbMinSingleCountInFour || cbMinSingleCardCount[i]<=cbMinSingleCountInFour+1 && 
				CanOutCard.cbCardType >= CT_THREE_LINE_TAKE_ONE && CanOutCard.cbCardType <= CT_THREE_LINE_TAKE_ONE ) &&
					cbMinLogicCardValue>GetCardLogicValue(CanOutCard.cbCardData[Index][0]))
				{
					//针对大牌
					bool bNoLargeCard = true ;

					//当地主手上牌数大于4，而且地主出的是小于K的牌而且不是地主手上的最大牌时，不能出2去打
					if(m_cbUserCardCount[m_wBankerUser]>=4 && cbHandCardCount>=5 && CanOutCard.cbEachHandCardCount[Index]>=2 && 
						GetCardLogicValue(CanOutCard.cbCardData[Index][0])>=15 &&
						GetCardLogicValue(cbTurnCardData[0])<13 &&
						GetCardLogicValue(cbTurnCardData[0])<GetCardLogicValue(m_cbAllCardData[m_wBankerUser][0]) && 
						CanOutCard.cbEachHandCardCount[Index]!=cbHandCardCount)
						bNoLargeCard=false ;

					//搜索有没有大牌（针对飞机带翅膀后面的带牌）
					for(BYTE k=3; k<CanOutCard.cbEachHandCardCount[Index]; ++k)
					{
						if(GetCardLogicValue(CanOutCard.cbCardData[Index][k])>=15 && 
							CanOutCard.cbEachHandCardCount[Index]!=cbHandCardCount)
							bNoLargeCard = false ;
					}
					if(bNoLargeCard)
					{
						bFindCard = true ;
						cbCanOutIndex = Index ; 
						cbMinLogicCardValue = GetCardLogicValue(CanOutCard.cbCardData[Index][0]) ;
					}
				}
			}

			if(bFindCard)
			{
				//地主的最大牌
				BYTE cbLargestLogicCard = GetCardLogicValue(m_cbAllCardData[m_wBankerUser][0]) ;
				bool bCanOut=true ;

				//王只压2
				if(GetCardLogicValue(cbTurnCardData[0])<cbLargestLogicCard)
				{
					if(GetCardColor(CanOutCard.cbCardData[cbCanOutIndex][0])==0x40 && GetCardLogicValue(cbTurnCardData[0])<=14 && cbHandCardCount>5) 								
					{
						bCanOut = false ;
					}
				}

                //双王判断
				if(GetCardLogicValue(CanOutCard.cbCardData[cbCanOutIndex][0])>=16 && cbHandCardCount>=2 && cbHandCardData[0]==0x4F && cbHandCardData[1]==0x4E)
				{
					bool bOutMissileCard = false ;
					//一手牌和一个炸弹
					BYTE cbRemainCardData[20], cbRemainCardCount=cbHandCardCount ;
					CopyMemory(cbRemainCardData, cbHandCardData, cbHandCardCount) ;
					RemoveCard(cbRemainCardData, 2, cbRemainCardData, cbRemainCardCount) ;
					cbRemainCardCount -= 2 ;
					if(CT_ERROR!=GetCardType(cbRemainCardData, cbRemainCardCount)) bOutMissileCard = true;

					//只剩少量牌
					BYTE cbRemainLargestCard = GetCardLogicValue(cbRemainCardData[0]) ;
					if(cbRemainCardCount<5 && cbRemainCardCount>0 && GetCardLogicValue(cbRemainCardData[0])>=14) bOutMissileCard = true;

					//炸后单牌数
					BYTE cbSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, CanOutCard.cbCardData[cbCanOutIndex],
						CanOutCard.cbEachHandCardCount[cbCanOutIndex]) ;
					if(cbSingleCardCount<=1 && GetCardLogicValue(cbRemainCardData[0])>=11) bOutMissileCard = true;

					//还有小牌
					if (GetCardLogicValue(cbRemainCardData[0]) <= 10 && CT_ERROR == GetCardType(cbRemainCardData, cbRemainCardCount) && 
						GetCardLogicValue(m_cbAllCardData[m_wBankerUser][0]) > 10)
						bOutMissileCard = false ;

					//火箭扑克
					if(bOutMissileCard)
					{
						//优先其他炸弹
						BYTE cbIndex = CardTypeResult[CT_BOMB_CARD].cbCardTypeCount - 1 ;

						OutCardResult.cbCardCount = CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[cbIndex] ;
						CopyMemory(OutCardResult.cbResultCard, CardTypeResult[CT_BOMB_CARD].cbCardData[cbIndex], OutCardResult.cbCardCount) ;
						return ;
					}
				}
				if(bCanOut)
				{
					//设置变量
					OutCardResult.cbCardCount=CanOutCard.cbEachHandCardCount[cbCanOutIndex];
					CopyMemory(OutCardResult.cbResultCard,CanOutCard.cbCardData[cbCanOutIndex],CanOutCard.cbEachHandCardCount[cbCanOutIndex]*sizeof(BYTE));

					return ;
				}
			}

			if(cbOutCardType==CT_SINGLE)
			{
				//地主的最大牌
				BYTE cbLargestLogicCard = GetCardLogicValue(m_cbAllCardData[m_wBankerUser][0]) ;

				if(GetCardLogicValue(cbTurnCardData[0])==14 || 
					GetCardLogicValue(cbTurnCardData[0])>=cbLargestLogicCard || 
					(GetCardLogicValue(cbTurnCardData[0])<cbLargestLogicCard-1) ||
					m_cbUserCardCount[m_wBankerUser]<=5)
				{
					//取一张大于等于2而且要比地主出的牌大的牌，
					BYTE cbIndex=MAX_COUNT ;
					for(BYTE i=0; i<cbHandCardCount; ++i)
						if(GetCardLogicValue(cbHandCardData[i])>GetCardLogicValue(cbTurnCardData[0]) &&
							GetCardLogicValue(cbHandCardData[i])>=15)
						{
							cbIndex = i ;
						}
						if(cbIndex!=MAX_COUNT)
						{
							//设置变量
							OutCardResult.cbCardCount=1;
							OutCardResult.cbResultCard[0] = cbHandCardData[cbIndex] ;

							return ;
						}
				}
			}

			//当朋友不能拦截地主时
			WORD wMeChairID = (m_wBankerUser+1)%GAME_PLAYER ;
			WORD wFriendID = (wMeChairID+1)%GAME_PLAYER ;

			tagOutCardTypeResult FriendCardTypeResult[13] ;
			ZeroMemory(FriendCardTypeResult, sizeof(FriendCardTypeResult)) ;
			AnalyseOutCardType(m_cbAllCardData[wFriendID], m_cbUserCardCount[wFriendID], cbTurnCardData, cbTurnCardCount, FriendCardTypeResult) ;

			//当朋友不能拦截地主时
			if(m_cbUserCardCount[m_wBankerUser]<=4 && FriendCardTypeResult[cbOutCardType].cbCardTypeCount==0 && CardTypeResult[cbOutCardType].cbCardTypeCount>0)
			{
				BYTE cbMinSingleCount=MAX_COUNT ;
				BYTE Index=0 ;
				for(BYTE i=0; i<CardTypeResult[cbOutCardType].cbCardTypeCount; ++i)
				{
					BYTE cbTmpCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, CardTypeResult[cbOutCardType].cbCardData[i], CardTypeResult[cbOutCardType].cbEachHandCardCount[i]) ;
					if(cbMinSingleCount>=cbTmpCount)
					{
						cbMinSingleCount = cbTmpCount ;
						Index = i ;
					}
				}
				//设置变量
				OutCardResult.cbCardCount=CardTypeResult[cbOutCardType].cbEachHandCardCount[Index];
				CopyMemory(OutCardResult.cbResultCard, CardTypeResult[cbOutCardType].cbCardData[Index], OutCardResult.cbCardCount) ;

				return ;
			}
		}

		//还要考虑炸弹
		if(CardTypeResult[CT_BOMB_CARD].cbCardTypeCount>0 || 
			(NORMAL_COUNT  == cbHandCardCount && NORMAL_COUNT == m_cbUserCardCount[(m_wBankerUser+2)%GAME_PLAYER]))
		{
			tagOutCardTypeResult const &BomCard = CardTypeResult[CT_BOMB_CARD] ;
			BYTE cbMinLogicValue = GetCardLogicValue(BomCard.cbCardData[0][0]) ;
			BYTE Index = 0 ;
			for(BYTE i=0; i<BomCard.cbCardTypeCount; ++i)
			{
				if(cbMinLogicValue>GetCardLogicValue(BomCard.cbCardData[i][0]))
				{
					cbMinLogicValue = GetCardLogicValue(BomCard.cbCardData[i][0]) ;
					Index = i ;
				}
			}

			bool bOutBomb = false ;

			//春天判断
			if (NORMAL_COUNT  == cbHandCardCount && NORMAL_COUNT == m_cbUserCardCount[(m_wBankerUser+2)%GAME_PLAYER] && CT_ERROR != GetCardType(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser]))
				bOutBomb = true ;

			//一手牌和一个炸弹
			BYTE cbRemainCardData[20], cbRemainCardCount=cbHandCardCount ;
			CopyMemory(cbRemainCardData, cbHandCardData, cbHandCardCount) ;
			RemoveCard(BomCard.cbCardData[Index], BomCard.cbEachHandCardCount[Index], cbRemainCardData, cbRemainCardCount) ;
			cbRemainCardCount -= BomCard.cbEachHandCardCount[Index] ;
			if(CT_ERROR!=GetCardType(cbRemainCardData, cbRemainCardCount)) bOutBomb = true ;

			//炸后单牌数
			BYTE cbSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, BomCard.cbCardData[Index],BomCard.cbEachHandCardCount[Index]) ;
			if(cbSingleCardCount==0 && GetCardLogicValue(cbRemainCardData[0]) > GetCardLogicValue(m_cbAllCardData[m_wBankerUser][0])) 
				bOutBomb = true ;

			//只剩一手
			BYTE cbRemainCardType = GetCardType(m_cbAllCardData[wOutCardUser], m_cbUserCardCount[wOutCardUser]) ;
			if(cbRemainCardType>CT_ERROR && cbRemainCardType<CT_FOUR_LINE_TAKE_ONE && GetCardLogicValue(m_cbAllCardData[wOutCardUser][0])<15 && 
				cbSingleCardCount < 2 && (GetCardLogicValue(cbRemainCardData[0]) >= GetCardLogicValue(m_cbAllCardData[m_wBankerUser][0])))
				bOutBomb = true ;

			//只剩少量牌
			BYTE cbRemainLargestCard = GetCardLogicValue(cbRemainCardData[0]) ;
			if(cbRemainCardCount<5 && cbRemainCardCount>0 && (cbRemainLargestCard!=GetCardLogicValue(BomCard.cbCardData[Index][0])) && 
				cbRemainLargestCard>GetCardLogicValue(m_cbAllCardData[wOutCardUser][0]) && cbRemainLargestCard > 14) bOutBomb = true ;

			//分析扑克
			tagAnalyseResult AnalyseResult ;
			AnalysebCardData(cbRemainCardData, cbRemainCardCount, AnalyseResult) ;

			if (m_cbUserCardCount[m_wBankerUser] == 1 && (AnalyseResult.cbDoubleCount * 2 + AnalyseResult.cbThreeCount * 3 + AnalyseResult.cbFourCount * 4 + 1 >= cbRemainCardCount)) bOutBomb = true ;


			//设置变量
			if(bOutBomb)
			{
				OutCardResult.cbCardCount=BomCard.cbEachHandCardCount[Index];
				CopyMemory(OutCardResult.cbResultCard,BomCard.cbCardData[Index],BomCard.cbEachHandCardCount[Index]*sizeof(BYTE));
			}

			return ;
		}

		return ;
	}
	return ;
}

//叫分判断
BYTE CGameLogic::LandScore(WORD wMeChairID, BYTE cbCurrentLandScore)
{
	//大牌数目
	BYTE cbLargeCardCount = 0 ;
	BYTE Index=0 ;
	while(GetCardLogicValue(m_cbLandScoreCardData[Index++])>=15) ++cbLargeCardCount ;

	//单牌个数
	BYTE cbSingleCardCount=AnalyseSinleCardCount(m_cbLandScoreCardData, sizeof(m_cbLandScoreCardData), NULL, 0) ;

	bool bCallBanker = false;
	//大王+2  小王+2+2  2+2+2  双王  大王+炸弹  俩炸以上。

	BYTE cbBombCardData[25];
	BYTE cbBomCardCount=0;
	GetAllBomCard(m_cbLandScoreCardData, sizeof(m_cbLandScoreCardData), cbBombCardData, cbBomCardCount);

	if ( m_cbLandScoreCardData[0] == 0x4F && GetCardValue(m_cbLandScoreCardData[1]) == 2 ) bCallBanker = true;
	else if ( m_cbLandScoreCardData[0] == 0x4E &&  GetCardValue(m_cbLandScoreCardData[1]) == 2  &&  GetCardValue(m_cbLandScoreCardData[2]) == 2 ) bCallBanker = true;
	else if ( GetCardValue(m_cbLandScoreCardData[0]) == 2 &&  GetCardValue(m_cbLandScoreCardData[1]) == 2  &&  GetCardValue(m_cbLandScoreCardData[2]) == 2 ) bCallBanker = true;
	else if ( m_cbLandScoreCardData[0] == 0x4F && m_cbLandScoreCardData[1] == 0x4E ) bCallBanker = true;
	else if ( m_cbLandScoreCardData[0] == 0x4F && cbBomCardCount > 0 ) bCallBanker = true;
	else if ( cbBomCardCount >= 2 ) bCallBanker = true;

	if(cbLargeCardCount>=4 && cbSingleCardCount<=4) bCallBanker = true;

	//叫牌判断
	switch ( m_bCurrentState )
	{
	case CALL_BANKER:
		{
			return bCallBanker ? MING_CATCH : 0;
		}
	case CALL_SCORE:
		{
			if ( m_wBankerUser != wMeChairID )
			{
				WORD wFriendID = INVALID_CHAIR;
				for ( WORD wIndex = 0; wIndex < GAME_PLAYER; ++wIndex )
					if ( wIndex != m_wBankerUser && wIndex != wMeChairID ) wFriendID = wIndex;
				if ( m_bCallScore[wFriendID] ) return DOUBLE_SCORE;
			}
			return bCallBanker ? DOUBLE_SCORE : 0;
		}
	case CALL_TWO_SCORE:
		{
			if ( m_wBankerUser != wMeChairID )
			{
				WORD wFriendID = INVALID_CHAIR;
				for ( WORD wIndex = 0; wIndex < GAME_PLAYER; ++wIndex )
					if ( wIndex != m_wBankerUser && wIndex != wMeChairID ) wFriendID = wIndex;
				if ( m_bCallScore[wFriendID] ) return DOUBLE_SCORE;
			}
			return bCallBanker ? DOUBLE_SCORE : 0;
		}
	}

	//放弃叫分
	return 0 ;
}

//出牌测试
bool CGameLogic::_TestOutAllCard(WORD wTestUser, BYTE cbWantOutCardData[], BYTE cbWantOutCardCount, BYTE	cbAllCardData[GAME_PLAYER][MAX_COUNT], BYTE cbUserCardCount[GAME_PLAYER], bool bFirstOutCard)
{
	//此递归暂不可用
	//return false ;

	//朋友ID
	WORD wFriendID = GAME_PLAYER ;
	if(wTestUser != m_wBankerUser) wFriendID = (wTestUser==((m_wBankerUser+1)%GAME_PLAYER)) ? (m_wBankerUser+2)%GAME_PLAYER : (m_wBankerUser+1)%GAME_PLAYER ;

	//暂时只处理少数牌
//	for(WORD wUser=0; wUser<GAME_PLAYER; ++wUser) if(wFriendID!=wUser && cbUserCardCount[wUser]>10) return false ;

//	m_lRecCount++ ;
	//防止深层递归
//	if(m_lRecCount>50) return false ;
	
	BYTE cbWantOutCardType = GetCardType(cbWantOutCardData, cbWantOutCardCount) ;

	//出完判断
	if ( cbUserCardCount[wTestUser] == cbWantOutCardCount ) return true ;

    //出牌判断
	try
	{
	for(WORD wUser=0; wUser<GAME_PLAYER; ++wUser)
	{
		if(wUser!=wTestUser && wFriendID!=wUser)
		{
			tagOutCardTypeResult CardTypeResult[12+1] ;
			ZeroMemory(CardTypeResult, sizeof(CardTypeResult)) ;

			//出牌分析
			try
			{
			AnalyseOutCardType(cbAllCardData[wUser], cbUserCardCount[wUser], cbWantOutCardData, cbWantOutCardCount, CardTypeResult) ;
			}
			catch(...)
			{
				return false ;
			}
			for(BYTE cbCardType=CT_SINGLE; cbCardType<=CT_MISSILE_CARD; ++cbCardType)
				if(CardTypeResult[cbCardType].cbCardTypeCount>0) return false ;
		}
	}
	}
	catch(...)
	{
		return false ;
	}

	//拆炸判断
	BYTE cbAllBombCard[MAX_COUNT], cbAllBombCount=0 ;
	GetAllBomCard(cbAllCardData[wTestUser], cbUserCardCount[wTestUser], cbAllBombCard, cbAllBombCount) ;
	if (cbAllBombCount > 0)
	{
		//剩余扑克
		BYTE cbRemainCard[MAX_COUNT], cbRemainCardCount ;

		CopyMemory(cbRemainCard, cbAllCardData[wTestUser], cbUserCardCount[wTestUser]) ;
		cbRemainCardCount = cbUserCardCount[wTestUser] ;

		RemoveCard(cbWantOutCardData, cbWantOutCardCount, cbRemainCard, cbRemainCardCount) ;
		cbRemainCardCount -= cbWantOutCardCount ;

		//愚蠢牌型
		if (CT_FOUR_LINE_TAKE_ONE == cbWantOutCardType || CT_FOUR_LINE_TAKE_TWO==cbWantOutCardType) return false ;

		//只剩炸弹
		if (cbWantOutCardType >= CT_BOMB_CARD && cbWantOutCardCount == cbUserCardCount[wTestUser]) return true ; 

		//只剩炸弹
		if (cbAllBombCount == cbUserCardCount[wTestUser] && cbWantOutCardType >= CT_BOMB_CARD) return true ;

		//炸弹和一手
		if ( cbWantOutCardType >= CT_BOMB_CARD && GetCardType(cbRemainCard, cbRemainCardCount) != CT_ERROR && 
			CT_FOUR_LINE_TAKE_TWO != GetCardType(cbRemainCard, cbRemainCardCount) && CT_FOUR_LINE_TAKE_ONE != GetCardType(cbRemainCard, cbRemainCardCount) )
			return true ;

		//首出牌时不出炸弹
		if (cbWantOutCardType >= CT_BOMB_CARD && bFirstOutCard)
		{
			//地主只剩一手牌
			if (wTestUser != m_wBankerUser && GetCardType(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser]) == CT_ERROR)
				return false ;

				WORD wUndersideOfBanker = (m_wBankerUser+1)%GAME_PLAYER ;	//地主下家
				WORD wUpsideOfBanker = (wUndersideOfBanker+1)%GAME_PLAYER ;	//地主上家

			//闲家只剩一手牌
			if (wTestUser == m_wBankerUser && GetCardType(m_cbAllCardData[wUndersideOfBanker], m_cbUserCardCount[wUndersideOfBanker]) == CT_ERROR &&
				GetCardType(m_cbAllCardData[wUpsideOfBanker], m_cbUserCardCount[wUpsideOfBanker]) == CT_ERROR)
				return false ;
		}

		//拆炸判断
		if (cbWantOutCardType < CT_FOUR_LINE_TAKE_ONE && cbWantOutCardType != CT_SINGLE_LINE)
		{
			for (BYTE i=0; i < cbWantOutCardCount; ++i)
				for (BYTE j=0; j < cbAllBombCount; ++j)
				{
					if (GetCardValue(cbAllBombCard[j]) == GetCardValue(cbWantOutCardData[i])) return false ;
				}
		}
	}

	if(cbUserCardCount[wTestUser]==0) return true ;

	//递归处理
	try
	{
	ASSERT(cbUserCardCount[wTestUser] >= cbWantOutCardCount) ;
	if (cbUserCardCount[wTestUser] < cbWantOutCardCount ) return false ;
	RemoveCard(cbWantOutCardData, cbWantOutCardCount, cbAllCardData[wTestUser], cbUserCardCount[wTestUser]) ;
	cbUserCardCount[wTestUser] -= cbWantOutCardCount ;

	//不带大牌
	if (cbWantOutCardType >= CT_THREE_LINE_TAKE_ONE && cbWantOutCardType <= CT_FOUR_LINE_TAKE_TWO && !bFirstOutCard)
	{
		bool bHaveLargeCard = false ;
		for (BYTE i = 3; i < cbWantOutCardCount; ++i) 
			if (GetCardLogicValue(cbWantOutCardData[i]) >= 15)
			{
				bHaveLargeCard = true ;
				break ;
			}
		if (bHaveLargeCard)
		{
			for (BYTE i = 0; i < cbUserCardCount[wTestUser]; ++i)
				if (GetCardLogicValue(cbAllCardData[wTestUser][i]) < 15) return false ;
		}
	}
	}
	catch(...)
	{
		return false ;
	}
	if(cbUserCardCount[wTestUser]!=0)
	{
		tagOutCardTypeResult CardTypeResult[12+1] ;
		ZeroMemory(CardTypeResult, sizeof(CardTypeResult)) ;

		//出牌分析
		try
		{
		AnalyseOutCardType(cbAllCardData[wTestUser], cbUserCardCount[wTestUser], CardTypeResult) ;
		}
		catch(...)
		{
			return false ;
		}
		for(BYTE cbCardType=CT_SINGLE; cbCardType<=CT_MISSILE_CARD; ++cbCardType)
			if(CardTypeResult[cbCardType].cbCardTypeCount>0) 
			{
				for(LONG lIndex=0; lIndex<CardTypeResult[cbCardType].cbCardTypeCount; ++lIndex)
				{
					//保存数据
					BYTE cbTmpCardData[GAME_PLAYER][MAX_COUNT], cbTmpCardCount[GAME_PLAYER] ;
					CopyMemory(cbTmpCardData, cbAllCardData, sizeof(cbTmpCardData)) ;
					CopyMemory(cbTmpCardCount, cbUserCardCount, sizeof(cbTmpCardCount)) ;

					//递归分析
					try
					{
					if(_TestOutAllCard(wTestUser, CardTypeResult[cbCardType].cbCardData[lIndex], CardTypeResult[cbCardType].cbEachHandCardCount[lIndex], cbTmpCardData, cbTmpCardCount, bFirstOutCard))
						return true ;
					}
					catch(...)
					{
						return false ;
					}
				}
			}
	}
	else
		return true ;

	return false ;
}


//四带牌型
bool CGameLogic::AnalyseFourCardType( BYTE const cbHandCardData[MAX_COUNT], BYTE cbHandCardCount, BYTE cbEnemyCardData[MAX_COUNT], BYTE cbEnemyCardCount, tagOutCardResult &CardResult )
{
	//初始变量
	ZeroMemory(&CardResult, sizeof(CardResult)) ;

	//牌数判断
	if ( cbHandCardCount < 5 ) return false ;

	//对方牌型分析
	if ( GetCardType(cbEnemyCardData, cbEnemyCardCount) == CT_SINGLE )
	{
		//对王判断
		if ( GetCardLogicValue(cbHandCardData[1] ) < 16 )
		{
			//分析扑克
			tagAnalyseResult AnalyseResult ;
			AnalysebCardData(cbHandCardData, cbHandCardCount, AnalyseResult) ;

			//分析牌型
			if ( cbHandCardCount != ( AnalyseResult.cbFourCount * 4 + AnalyseResult.cbDoubleCount * 2 + AnalyseResult.cbSignedCount ) || 
				AnalyseResult.cbFourCount <= 0 ) return false ;

			//对方单牌
			if ( GetCardType(cbEnemyCardData, cbEnemyCardCount) == CT_SINGLE )
			{
				//个数判断
				if ( AnalyseResult.cbSignedCount < 2 ) return false ;

				//大小判断
				if ( GetCardLogicValue(AnalyseResult.cbSignedCardData[0]) >= GetCardLogicValue(cbEnemyCardData[0]) ) return false ;

				//四张带两单
				if ( AnalyseResult.cbFourCount >= 1 && AnalyseResult.cbSignedCount == 2 )
				{
					//炸弹索引
					BYTE cbFourCardIndex = ( AnalyseResult.cbFourCount - 1 ) * 4 ;

					//返回结果
					CardResult.cbCardCount = 4 + 2 ;
					CopyMemory(CardResult.cbResultCard, AnalyseResult.cbFourCardData + cbFourCardIndex, 4 ) ;
					CardResult.cbResultCard[4] = AnalyseResult.cbSignedCardData[0] ;
					CardResult.cbResultCard[5] = AnalyseResult.cbSignedCardData[1] ;

					return true ;
				}
				//四张带三单
				else if ( AnalyseResult.cbFourCount >= 1 && AnalyseResult.cbSignedCount == 3 )
				{
					//炸弹索引
					BYTE cbFourCardIndex = ( AnalyseResult.cbFourCount - 1 ) * 4 ;

					//返回结果
					CardResult.cbCardCount = 4 + 2 ;
					CopyMemory(CardResult.cbResultCard, AnalyseResult.cbFourCardData + cbFourCardIndex, 4 ) ;
					CardResult.cbResultCard[4] = AnalyseResult.cbSignedCardData[1] ;
					CardResult.cbResultCard[5] = AnalyseResult.cbSignedCardData[2] ;

					return true ;
				}
			}
			else if ( GetCardType(cbEnemyCardData, cbEnemyCardCount) == CT_DOUBLE )
			{
			}

		}
		//对王判断
		else if ( GetCardLogicValue(cbHandCardData[1] ) == 16 )
		{
			//分析扑克
			tagAnalyseResult AnalyseResult ;
			AnalysebCardData(cbHandCardData, cbHandCardCount, AnalyseResult) ;

			//分析牌型
			if ( cbHandCardCount != ( AnalyseResult.cbFourCount * 4 + AnalyseResult.cbDoubleCount * 2 + AnalyseResult.cbSignedCount ) || 
				AnalyseResult.cbFourCount <= 0 ) return false ;

			//对方单牌
			if ( GetCardType(cbEnemyCardData, cbEnemyCardCount) == CT_SINGLE )
			{
				//个数判断
				if ( AnalyseResult.cbSignedCount < 4 ) return false ;

				//大小判断
				if ( GetCardLogicValue(AnalyseResult.cbSignedCardData[2]) >= GetCardLogicValue(cbEnemyCardData[0]) ) return false ;

				//四张带两单
				if ( AnalyseResult.cbFourCount >= 1 && AnalyseResult.cbSignedCount == 4 )
				{
					//炸弹索引
					BYTE cbFourCardIndex = ( AnalyseResult.cbFourCount - 1 ) * 4 ;

					//返回结果
					CardResult.cbCardCount = 4 + 2 ;
					CopyMemory(CardResult.cbResultCard, AnalyseResult.cbFourCardData + cbFourCardIndex, 4 ) ;
					CardResult.cbResultCard[4] = AnalyseResult.cbSignedCardData[2] ;
					CardResult.cbResultCard[5] = AnalyseResult.cbSignedCardData[3] ;

					return true ;
				}
				//四张带三单
				else if ( AnalyseResult.cbFourCount >= 1 && AnalyseResult.cbSignedCount == 5 )
				{
					//炸弹索引
					BYTE cbFourCardIndex = ( AnalyseResult.cbFourCount - 1 ) * 4 ;

					//返回结果
					CardResult.cbCardCount = 4 + 2 ;
					CopyMemory(CardResult.cbResultCard, AnalyseResult.cbFourCardData + cbFourCardIndex, 4 ) ;
					CardResult.cbResultCard[4] = AnalyseResult.cbSignedCardData[3] ;
					CardResult.cbResultCard[5] = AnalyseResult.cbSignedCardData[4] ;

					return true ;
				}
			}
		}
	}
	////一张大牌带一张
	//else if (GetCardType(cbEnemyCardData, cbEnemyCardCount) == CT_ERROR && cbEnemyCardCount == 2 )
	//{
	//	//对王判断
	//	if ( GetCardLogicValue(cbHandCardData[2] ) < 16 )
	//	{
	//		//分析扑克
	//		tagAnalyseResult AnalyseResult ;
	//		AnalysebCardData(cbHandCardData, cbHandCardCount, AnalyseResult) ;

	//		//分析牌型
	//		if ( cbHandCardCount != ( AnalyseResult.cbFourCount * 4 + AnalyseResult.cbDoubleCount * 2 + AnalyseResult.cbSignedCount ) || 
	//			AnalyseResult.cbFourCount <= 0 ) return false ;

	//		//个数判断
	//		if ( AnalyseResult.cbSignedCount < 2 ) return false ;

	//		//四张带三单
	//		if ( AnalyseResult.cbFourCount >= 1 && AnalyseResult.cbSignedCount == 3 )
	//		{
	//			//大小判断
	//			if ( GetCardLogicValue(AnalyseResult.cbSignedCardData[0]) > GetCardLogicValue(cbEnemyCardData[0]) ) return false ;

	//			//炸弹索引
	//			BYTE cbFourCardIndex = ( AnalyseResult.cbFourCount - 1 ) * 4 ;

	//			//返回结果
	//			CardResult.cbCardCount = 4 + 2 ;
	//			CopyMemory(CardResult.cbResultCard, AnalyseResult.cbFourCardData + cbFourCardIndex, 4 ) ;
	//			CardResult.cbResultCard[4] = AnalyseResult.cbSignedCardData[1] ;
	//			CardResult.cbResultCard[5] = AnalyseResult.cbSignedCardData[2] ;

	//			return true ;
	//		}
	//	}
	//}
	return false ;
}


//出牌测试
bool CGameLogic::TestOutAllCard( WORD wTestUser, BYTE cbWantOutCardData[], BYTE cbWantOutCardCount, bool bFirstOutCard ) {

	try {
		if ( ! VerifyOutCard( wTestUser, cbWantOutCardData, cbWantOutCardCount, m_cbAllCardData[ wTestUser ], m_cbUserCardCount[ wTestUser ], bFirstOutCard ) ) {
			return false;
		}

		//模拟递归处理
		if ( cbWantOutCardCount != m_cbUserCardCount[ wTestUser ] ) {

			//初始栈
			m_StackHandCardInfo.InitStack();

			//第一个元素
			tagHandCardInfo OriginHandCardInfo ;

			//手上扑克
			CopyMemory( OriginHandCardInfo.cbHandCardData, m_cbAllCardData[ wTestUser ], m_cbUserCardCount[ wTestUser ] );
			OriginHandCardInfo.cbHandCardCount = m_cbUserCardCount[ wTestUser ];

			//移除第一手牌
			RemoveCard( cbWantOutCardData, cbWantOutCardCount, OriginHandCardInfo.cbHandCardData, OriginHandCardInfo.cbHandCardCount );
			OriginHandCardInfo.cbHandCardCount -= cbWantOutCardCount;

			//分析扑克
			try {
				AnalyseOutCardType( OriginHandCardInfo.cbHandCardData, OriginHandCardInfo.cbHandCardCount, OriginHandCardInfo.CardTypeResult ) ;
			}
			catch( ... ) {
				return false ;
			}

			//元素压栈
			m_StackHandCardInfo.Push( &OriginHandCardInfo );

			//次数控制
			LONG lJudgeCount = 0;

			while ( ! m_StackHandCardInfo.IsEmpty() ) {

				//防止死循环
				if ( ++lJudgeCount == 100 ) {

					return false;
				}

				//栈顶元素
				tagHandCardInfo *pTopHandCardInfo = NULL;
				m_StackHandCardInfo.GetTop( pTopHandCardInfo );

				//合法判断
				ASSERT( pTopHandCardInfo != NULL ) ;
				if ( pTopHandCardInfo == NULL ) {
					return false;
				}

				//牌型数据
				tagOutCardTypeResult *pOutCardTypeResult = ( tagOutCardTypeResult * )pTopHandCardInfo->CardTypeResult;

				//禁止的牌型
				pOutCardTypeResult[ CT_FOUR_LINE_TAKE_ONE ].cbCardTypeCount = 0;
				pOutCardTypeResult[ CT_FOUR_LINE_TAKE_TWO ].cbCardTypeCount = 0;

				//所有牌型
				bool bBreakJudge = false;
				bool bFindLargestCard = false;
				for ( BYTE cbOutCardTypeIndx = CT_SINGLE; cbOutCardTypeIndx <= CT_MISSILE_CARD && ! bBreakJudge; ++cbOutCardTypeIndx ) {
					for ( BYTE cbHandCardIndx = 0; cbHandCardIndx < pOutCardTypeResult[ cbOutCardTypeIndx ].cbCardTypeCount && ! bBreakJudge ; ++cbHandCardIndx ) {

						//是否判断过
						if ( pOutCardTypeResult[ cbOutCardTypeIndx ].cbEachHandCardCount[ cbHandCardIndx ] == 0 ) {
							continue;
						}

						//最大判断
						if ( IsLargestCard( wTestUser, pOutCardTypeResult[ cbOutCardTypeIndx ].cbCardData[ cbHandCardIndx ], pOutCardTypeResult[ cbOutCardTypeIndx ].cbEachHandCardCount[ cbHandCardIndx ] ) ) {

							//最后一手
							bool bIsLastHandCard = pOutCardTypeResult[ cbOutCardTypeIndx ].cbEachHandCardCount[ cbHandCardIndx ] == pTopHandCardInfo->cbHandCardCount ;
							if ( bIsLastHandCard ) {
								return true ;
							}

							//是否能出
							if ( ! VerifyOutCard( wTestUser, pOutCardTypeResult[ cbOutCardTypeIndx ].cbCardData[ cbHandCardIndx ], 
								pOutCardTypeResult[ cbOutCardTypeIndx ].cbEachHandCardCount[ cbHandCardIndx ], pTopHandCardInfo->cbHandCardData, pTopHandCardInfo->cbHandCardCount, bFirstOutCard ) ) {

									//表明此牌已经判断过
									pOutCardTypeResult[ cbOutCardTypeIndx ].cbEachHandCardCount[ cbHandCardIndx ] = 0;
									continue;
								}

								//新建栈元素
								tagHandCardInfo NewHandCardInfo ;

								//手上扑克
								CopyMemory( NewHandCardInfo.cbHandCardData, pTopHandCardInfo->cbHandCardData, pTopHandCardInfo->cbHandCardCount );
								NewHandCardInfo.cbHandCardCount = pTopHandCardInfo->cbHandCardCount;

								//移除当前一手
								RemoveCard( pOutCardTypeResult[ cbOutCardTypeIndx ].cbCardData[ cbHandCardIndx ], pOutCardTypeResult[ cbOutCardTypeIndx ].cbEachHandCardCount[ cbHandCardIndx ],
									NewHandCardInfo.cbHandCardData, NewHandCardInfo.cbHandCardCount );
								NewHandCardInfo.cbHandCardCount -= pOutCardTypeResult[ cbOutCardTypeIndx ].cbEachHandCardCount[ cbHandCardIndx ];

								//表明此牌已经判断过
								pOutCardTypeResult[ cbOutCardTypeIndx ].cbEachHandCardCount[ cbHandCardIndx ] = 0;

								//分析扑克
								try {
									AnalyseOutCardType( NewHandCardInfo.cbHandCardData, NewHandCardInfo.cbHandCardCount, NewHandCardInfo.CardTypeResult ) ;
								}
								catch( ... ) {
									return false ;
								}

								//元素压栈
								m_StackHandCardInfo.Push( & NewHandCardInfo );

								//中断循环
								bBreakJudge = true;

								//设置标识
								bFindLargestCard = true;
						}
						//当前一手的数目等于手上扑克数目
						else if (  pOutCardTypeResult[ cbOutCardTypeIndx ].cbEachHandCardCount[ cbHandCardIndx ] == pTopHandCardInfo->cbHandCardCount ) {
							return true ;
						}
					}
				}

				//出栈判断
				if ( ! bFindLargestCard ) {
					m_StackHandCardInfo.Pop();
				}
			}
		}
		else {
			return true ;
		}
	}
	catch ( ... ) {
		ASSERT( false );
		return false;
	}

	return false ;
}

//最大判断
bool CGameLogic::IsLargestCard( WORD wTestUser, BYTE const cbWantOutCardData[], BYTE const cbWantOutCardCount ) {

	//朋友ID
	WORD wFriendID = GAME_PLAYER ;
	if ( wTestUser != m_wBankerUser ) {
		wFriendID = ( wTestUser == ( ( m_wBankerUser + 1 ) % GAME_PLAYER ) ) ? ( m_wBankerUser + 2 ) % GAME_PLAYER : 
			( m_wBankerUser + 1 ) % GAME_PLAYER ;
	}

	//出牌判断
	try{
		for ( WORD wUser = 0; wUser < GAME_PLAYER; ++wUser ) {
			if ( wUser != wTestUser && wFriendID != wUser ) {
				tagOutCardTypeResult CardTypeResult[ 12 + 1 ] ;
				ZeroMemory( CardTypeResult, sizeof( CardTypeResult ) ) ;

				//出牌分析
				try {
					AnalyseOutCardType( m_cbAllCardData[ wUser ], m_cbUserCardCount[ wUser ], cbWantOutCardData, cbWantOutCardCount, CardTypeResult ) ;
				}
				catch(...) {
					return false ;
				}

				for ( BYTE cbCardType = CT_SINGLE; cbCardType <= CT_MISSILE_CARD; ++cbCardType ) {
					if ( 0 < CardTypeResult[ cbCardType ].cbCardTypeCount ) {
						return false ;
					}
				}
			}
		}
	}
	catch( ... ) {
		return false ;
	}

	return true;
}

//是否能出
bool CGameLogic::VerifyOutCard( WORD wTestUser, BYTE const cbWantOutCardData[], BYTE const cbWantOutCardCount, BYTE const cbCurHandCardData[ MAX_COUNT ], BYTE const cbCurHandCardCount, bool bFirstOutCard ) {

	//朋友ID
	WORD wFriendID = GAME_PLAYER ;
	if ( wTestUser != m_wBankerUser ) {
		wFriendID = ( wTestUser == ( ( m_wBankerUser + 1 ) % GAME_PLAYER ) ) ? ( m_wBankerUser + 2 ) % GAME_PLAYER : 
			( m_wBankerUser + 1 ) % GAME_PLAYER ;
	}

	BYTE cbWantOutCardType = GetCardType( cbWantOutCardData, cbWantOutCardCount ) ;

	//首出判断
	if ( bFirstOutCard && cbWantOutCardCount == cbCurHandCardCount ) {
		return true;
	}

	//是否最大
	if ( ! IsLargestCard( wTestUser, cbWantOutCardData, cbWantOutCardCount ) ) {
		return false;
	}

	//拆炸判断
	BYTE cbAllBombCard[ MAX_COUNT ], cbAllBombCount = 0 ;
	GetAllBomCard( cbCurHandCardData, cbCurHandCardCount, cbAllBombCard, cbAllBombCount ) ;

	if ( 0 < cbAllBombCount ) {
		//剩余扑克
		BYTE cbRemainCard [MAX_COUNT ], cbRemainCardCount ;

		CopyMemory( cbRemainCard, cbCurHandCardData, cbCurHandCardCount ) ;
		cbRemainCardCount = cbCurHandCardCount ;

		RemoveCard( cbWantOutCardData, cbWantOutCardCount, cbRemainCard, cbRemainCardCount ) ;
		cbRemainCardCount -= cbWantOutCardCount ;

		//愚蠢牌型
		if ( CT_FOUR_LINE_TAKE_ONE == cbWantOutCardType || CT_FOUR_LINE_TAKE_TWO==cbWantOutCardType ) return false ;

		//只剩炸弹
		if ( CT_BOMB_CARD <= cbWantOutCardType && cbWantOutCardCount == cbCurHandCardCount ) return true ; 

		//只剩炸弹
		if ( cbAllBombCount == m_cbUserCardCount[ wTestUser ] && CT_BOMB_CARD <= cbWantOutCardType ) return true ;

		//炸弹和一手
		if ( CT_BOMB_CARD <= cbWantOutCardType && GetCardType(cbRemainCard, cbRemainCardCount) != CT_ERROR && 
			CT_FOUR_LINE_TAKE_TWO != GetCardType( cbRemainCard, cbRemainCardCount ) && CT_FOUR_LINE_TAKE_ONE != GetCardType( cbRemainCard, cbRemainCardCount ) ) {
			return true ;
		}

		//首出牌时不出炸弹
		if (CT_BOMB_CARD <= cbWantOutCardType && bFirstOutCard ) {
			//地主只剩一手牌
			if ( wTestUser != m_wBankerUser && GetCardType( m_cbAllCardData[ m_wBankerUser ], m_cbUserCardCount[ m_wBankerUser ] ) == CT_ERROR ) {
				return false ;
			}

			WORD wUndersideOfBanker = (m_wBankerUser+1)%GAME_PLAYER ;	//地主下家
			WORD wUpsideOfBanker = (wUndersideOfBanker+1)%GAME_PLAYER ;	//地主上家

			//闲家只剩一手牌
			if ( wTestUser == m_wBankerUser && GetCardType( m_cbAllCardData[ wUndersideOfBanker ], m_cbUserCardCount[ wUndersideOfBanker ] ) == CT_ERROR &&
				GetCardType( m_cbAllCardData[ wUpsideOfBanker ], m_cbUserCardCount[ wUpsideOfBanker ] ) == CT_ERROR ) {
				return false ;
			}
		}

		//拆炸判断
		if ( cbWantOutCardType < CT_FOUR_LINE_TAKE_ONE && cbWantOutCardType != CT_SINGLE_LINE ) {
			for ( BYTE i = 0; i < cbWantOutCardCount; ++i )
				for ( BYTE j = 0; j < cbAllBombCount; ++j ) {
					if ( GetCardValue( cbAllBombCard[ j ] ) == GetCardValue( cbWantOutCardData[ i ] ) ) return false ;
				}
		}
	}

	//出完判断
	if ( cbCurHandCardCount == cbWantOutCardCount ) {
		return true ;
	}


	if ( cbCurHandCardCount == 0 ) {
		return true ;
	}

	try {
		ASSERT( cbWantOutCardCount <= cbCurHandCardCount ) ;
		if ( cbCurHandCardCount < cbWantOutCardCount ) {
			return false ;
		}

		//剩余扑克
		BYTE cbRemainCard [MAX_COUNT ], cbRemainCardCount ;

		CopyMemory( cbRemainCard, cbCurHandCardData, cbCurHandCardCount ) ;
		cbRemainCardCount = cbCurHandCardCount ;

		RemoveCard( cbWantOutCardData, cbWantOutCardCount, cbRemainCard, cbRemainCardCount ) ;
		cbRemainCardCount -= cbWantOutCardCount ;

		if ( cbRemainCardCount == 0 ) {
			return false;
		}

		//不带大牌
		if ( CT_THREE_LINE_TAKE_ONE <= cbWantOutCardType && cbWantOutCardType <= CT_FOUR_LINE_TAKE_TWO && ! bFirstOutCard ) {
			bool bHaveLargeCard = false ;
			for ( BYTE i = 3; i < cbWantOutCardCount; ++i ) 
				if ( 15 <= GetCardLogicValue( cbWantOutCardData[ i ]) ) {
					bHaveLargeCard = true ;
					break ;
				}
				if ( bHaveLargeCard ) {
					for ( BYTE i = 0; i < cbRemainCardCount; ++i ) {
						if ( GetCardLogicValue( cbRemainCard[ i ]) < 15 ) {
							return false ;
						}
					}
				}
		}
	}
	catch(...) {
		return false ;
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
