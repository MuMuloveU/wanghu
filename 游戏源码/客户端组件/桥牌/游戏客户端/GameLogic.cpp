#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

//索引定义
#define INDEX_OUT					0									//出牌索引
#define INDEX_HAND					1									//手牌索引
#define INDEX_FIRST					2									//首出索引

//////////////////////////////////////////////////////////////////////////
//静态变量

//扑克数据
const BYTE	CGameLogic::m_cbCardData[CELL_PACK]=
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//梅花 A - K
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//方块 A - K
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//红桃 A - K
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,	//黑桃 A - K
	0x4E,0x4F,
};

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameLogic::CGameLogic()
{
	//属性变量
	m_cbPackCount=0;
	m_cbDispatchCount=0;

	//逻辑变量
	m_cbMainColor=COLOR_ERROR;

	//辅助变量
	for (BYTE i=0;i<CountArray(m_cbSortRight);i++) m_cbSortRight[i]=i*COLOR_RIGHT;

	return;
}

//析构函数
CGameLogic::~CGameLogic()
{
}

//设置副数
bool CGameLogic::SetPackCount(BYTE cbPackCount)
{
	//效验参数
	ASSERT((cbPackCount>0)&&(cbPackCount<=2));
	if ((cbPackCount==0)||(cbPackCount>2)) return false;

	//副数数目
	m_cbPackCount=cbPackCount;

	//派发数目
	BYTE cbConcealCount=((m_cbPackCount%2)==0)?8:6;
	m_cbDispatchCount=((m_cbPackCount*CELL_PACK)-cbConcealCount)/4;

	return false;
}

//主牌花色
bool CGameLogic::SetMainColor(BYTE cbMainColor)
{
	//效验参数
	ASSERT((cbMainColor==COLOR_ERROR)||(cbMainColor<=COLOR_NT));
	if ((cbMainColor!=COLOR_ERROR)&&(cbMainColor>COLOR_NT)) return false;

	//设置变量
	m_cbMainColor=cbMainColor;

	//更新权位
	UpdateSortRight();

	return true;
}

//主牌数值
bool CGameLogic::SetMainValue(BYTE cbMainValue)
{
	//效验参数
	ASSERT((cbMainValue==VALUE_ERROR)||(cbMainValue<=0x0D));
	if ((cbMainValue!=VALUE_ERROR)&&(cbMainValue>0x0D)) return false;

	//设置变量

	//更新权位
	//UpdateSortRight();

	return true;
}

//获取类型
BYTE CGameLogic::GetCardType(const BYTE cbCardData[], BYTE cbCardCount)
{

	//状态效验
	ASSERT(m_cbMainColor!=COLOR_ERROR);

	//数目判断
	ASSERT((cbCardCount>0)&&(cbCardCount<=MAX_COUNT));
	if ((cbCardCount==0)||(cbCardCount>MAX_COUNT)) return CT_ERROR;

	//同花判断
	if (GetCardLogicColor(cbCardData,cbCardCount)==COLOR_ERROR) return CT_ERROR;

	//同牌判断
	BYTE cbSameCount=1;
	for (BYTE i=1;i<cbCardCount;i++) 
	{
		if (cbCardData[i]==cbCardData[0]) cbSameCount++;
		else break;
	}
	if (cbSameCount==cbCardCount) return CT_SINGLE+cbSameCount-1;

	//拖牌判断
	if ((cbSameCount>=2)&&(cbCardCount%cbSameCount)==0)
	{
		//变量定义
		BYTE cbBlockCount=cbCardCount/cbSameCount;

		//单元判断
		BYTE i;
		for (i=1;i<cbBlockCount;i++)
		{
			//变量定义
			BYTE cbIndex=i*cbSameCount;
			BYTE cbFirstValue=GetCardValue(cbCardData[0]);

			//相同分析
			BYTE j;
			for (j=0;j<cbSameCount;j++)
			{
				if (cbCardData[cbIndex+j]!=cbCardData[cbIndex]) break;
			}
			if (j!=cbSameCount) break;

			//相连分析
			if (IsLineValue(cbCardData[cbIndex-cbSameCount],cbCardData[cbIndex])==false) break;
		}

		//成功判断
		if (i==cbBlockCount) return CT_TRACKOR_2+cbSameCount-2;
	}

	//return CT_THROW_CARD;
	return CT_SINGLE;
}

//排列扑克
void CGameLogic::SortCardList(BYTE cbCardData[], BYTE cbCardCount)
{
	//转换数值
	BYTE bLogicVolue[20];
	for (BYTE i=0;i<cbCardCount;i++)	bLogicVolue[i]=GetCardLogicValue(cbCardData[i]);

	//排序操作
	bool bSorted=true;
	BYTE bTempData,bLast=cbCardCount-1;
	do
	{
		bSorted=true;
		for (BYTE i=0;i<bLast;i++)
		{
			if ((bLogicVolue[i]<bLogicVolue[i+1])||
			   ((bLogicVolue[i]==bLogicVolue[i+1])&&(cbCardData[i]<cbCardData[i+1])))
			{
				//交换位置
				bTempData=cbCardData[i];
				cbCardData[i]=cbCardData[i+1];
				cbCardData[i+1]=bTempData;
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
void CGameLogic::RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//混乱准备
	BYTE cbCardData[CELL_PACK*2];
	for (BYTE i=0;i<2;i++) CopyMemory(&cbCardData[CELL_PACK*i],m_cbCardData,sizeof(m_cbCardData));

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

//获取积分
WORD CGameLogic::GetCardScore(const BYTE cbCardData[], BYTE cbCardCount)
{
	//变量定义
	WORD wCardScore=0;

	//扑克累计
	for (BYTE i=0;i<cbCardCount;i++)
	{
		//获取数值
		BYTE cbCardValue=GetCardValue(cbCardData[i]);

		//累计积分
		if (cbCardValue==5) 
		{
			wCardScore+=5;
			continue;
		}

		//累计积分
		if ((cbCardValue==10)||(cbCardValue==13)) 
		{
			wCardScore+=10;
			continue;
		}
	}

	return wCardScore;
}

//获取积分
BYTE CGameLogic::GetScoreCard(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbScoreCard[], BYTE cbMaxCount)
{
	//变量定义
	BYTE cbPostion=0;

	//扑克累计
	for (BYTE i=0;i<cbCardCount;i++)
	{
		//获取数值
		BYTE cbCardValue=GetCardValue(cbCardData[i]);

		//累计扑克
		if ((cbCardValue==5)||(cbCardValue==10)||(cbCardValue==13))
		{
			ASSERT(cbPostion<cbMaxCount);
			cbScoreCard[cbPostion++]=cbCardData[i];
		}
	}

	return cbPostion;
}

//效验甩牌
bool CGameLogic::EfficacyThrowCard(const BYTE cbOutCardData[], BYTE cbOutCardCount, WORD wOutCardIndex, const BYTE cbHandCardData[GAME_PLAYER][MAX_COUNT], BYTE cbHandCardCount, tagOutCardResult & OutCardResult)
{
	//分析扑克
	tagAnalyseResult AnalyseResult;
	AnalyseCardData(cbOutCardData,cbOutCardCount,AnalyseResult);

	//循环比较
	for (WORD i=0;i<GAME_PLAYER-1;i++)
	{
		//获取数据
		WORD wUserIndex=(wOutCardIndex+i+1)%GAME_PLAYER;

		//提取扑克
		BYTE cbDistillCardHand[MAX_COUNT];
		BYTE cbDistillCountHand=DistillCardByColor(cbHandCardData[wUserIndex],cbHandCardCount,AnalyseResult.cbCardColor,cbDistillCardHand);

		//判断过滤
		if (cbDistillCountHand==0) continue;

		//牌型判断
		for (BYTE j=2;j>=2;j--)
		{
			//变量定义
			tagSameDataInfo * pSameDataInfo=&AnalyseResult.SameDataInfo[j-1];
			tagTractorDataInfo * pTractorDataInfo=&AnalyseResult.TractorDataInfo[j-2];

			//拖牌判断
			if (pTractorDataInfo->cbTractorCount>0)
			{
				//变量定义
				BYTE cbOutTractorIndex=0;
				BYTE cbUserTractorIndex=0;

				//提取拖牌
				tagTractorDataInfo TractorDataInfo;
				DistillTrackorByCount(cbDistillCardHand,cbDistillCountHand,j,TractorDataInfo);

				//循环对比
				for (BYTE cbUserRound=0;cbUserRound<TractorDataInfo.cbTractorCount;cbUserRound++)
				{
					//获取属性
					BYTE cbLineCountUser=TractorDataInfo.cbTractorData[cbUserTractorIndex];
					BYTE cbSameCountUser=TractorDataInfo.cbTractorData[cbUserTractorIndex+1];

					//对比拖牌
					for (BYTE cbOutRound=0;cbOutRound<pTractorDataInfo->cbTractorCount;cbOutRound++)
					{
						//获取属性
						BYTE cbLineCountOut=pTractorDataInfo->cbTractorData[cbOutTractorIndex];
						BYTE cbSameCountOut=pTractorDataInfo->cbTractorData[cbOutTractorIndex+1];

						//拖牌对比
						if (cbLineCountUser>=cbLineCountOut)
						{
							//扑克定义
							BYTE cbNextCardData=TractorDataInfo.cbTractorData[cbUserTractorIndex+2];
							BYTE cbFisrtCardData=pTractorDataInfo->cbTractorData[cbOutTractorIndex+2];

							//对比大小
							if (CompareCardData(cbFisrtCardData,cbNextCardData)==true)
							{
								OutCardResult.cbCardCount=cbLineCountOut*cbSameCountOut;
								CopyMemory(OutCardResult.cbResultCard,&pTractorDataInfo->cbTractorData[cbOutTractorIndex+2],sizeof(BYTE)*OutCardResult.cbCardCount);

								return false;
							}
						}

						//设置索引
						cbOutTractorIndex+=(2+cbLineCountOut*cbSameCountOut);
					}

					//设置索引
					cbUserTractorIndex+=(2+cbLineCountUser*cbSameCountUser);
				}
			}

			//同牌判断
			if (pSameDataInfo->cbBlockCount>0)
			{
				//提取同牌
				tagSameDataInfo SameDataInfo;
				DistillCardByCount(cbDistillCardHand,cbDistillCountHand,j,SameDataInfo);

				//同牌判断
				if (SameDataInfo.cbBlockCount>0)
				{
					//扑克定义
					BYTE cbLessSameIndex=pSameDataInfo->cbCardCount-j;
					BYTE cbFisrtCardData=pSameDataInfo->cbSameData[cbLessSameIndex];

					//对比大小
					if (CompareCardData(cbFisrtCardData,SameDataInfo.cbSameData[0])==true)
					{
						OutCardResult.cbCardCount=j;
						CopyMemory(OutCardResult.cbResultCard,&pSameDataInfo->cbSameData[cbLessSameIndex],sizeof(BYTE)*OutCardResult.cbCardCount);

						return false;
					}
				}
			}
		}

		//单牌判断
		if (AnalyseResult.SameDataInfo[0].cbCardCount!=0)
		{
			//变量定义
			BYTE cbCardCount=AnalyseResult.SameDataInfo[0].cbCardCount;
			BYTE cbLessCardData=AnalyseResult.SameDataInfo[0].cbSameData[cbCardCount-1];

			//对比扑克
			if (CompareCardData(cbLessCardData,cbDistillCardHand[0])==true)
			{
				OutCardResult.cbCardCount=1;
				OutCardResult.cbResultCard[0]=cbLessCardData;

				return false;
			}
		}
	}

	return true;
}

//对比扑克
bool CGameLogic::CompareCardData(BYTE cbFirstCardData, BYTE cbNextCardData)
{
	//获取花色
	BYTE cbLogicColorNext=GetCardLogicColor(cbNextCardData);
	BYTE cbLogicColorFirst=GetCardLogicColor(cbFirstCardData);

	//对比大小
	if ((cbLogicColorNext==COLOR_NT)||(cbLogicColorNext==cbLogicColorFirst))
	{
		//获取大小
		BYTE cbLogicValueNext=GetCardLogicValue(cbNextCardData);
		BYTE cbLogicValueFirst=GetCardLogicValue(cbFirstCardData);

		//大小判断
		if (cbLogicValueNext>cbLogicValueFirst) return true;
	}

	return false;
}

//对比扑克
bool CGameLogic::CompareCardResult(const tagAnalyseResult WinnerResult, const tagAnalyseResult UserResult)
{
	//对比拖牌
	for (BYTE i=2;i>=2;i--)
	{
		//变量定义
		const tagTractorDataInfo * pTractorDataInfoUser=&UserResult.TractorDataInfo[i-2];
		const tagTractorDataInfo * pTractorDataInfoWinner=&WinnerResult.TractorDataInfo[i-2];

		//对比拖牌
		if ((pTractorDataInfoWinner->cbTractorCount>0)&&(pTractorDataInfoUser->cbTractorCount>0))
		{
			//获取大小
			BYTE cbNextCardData=pTractorDataInfoUser->cbTractorData[2];
			BYTE cbFisrtCardData=pTractorDataInfoWinner->cbTractorData[2];

			//对比大小
			if (CompareCardData(cbFisrtCardData,cbNextCardData)==true) return true;
		}
	}

	//对比同牌
	for (BYTE i=2;i>=1;i--)
	{
		//变量定义
		const tagSameDataInfo * pSameDataInfoUser=&UserResult.SameDataInfo[i-1];
		const tagSameDataInfo * pSameDataInfoWinner=&WinnerResult.SameDataInfo[i-1];

		//对比同牌
		if ((pSameDataInfoWinner->cbCardCount>0)&&(pSameDataInfoUser->cbCardCount>0))
		{
			//获取大小
			BYTE cbNextCardData=pSameDataInfoUser->cbSameData[0];
			BYTE cbFisrtCardData=pSameDataInfoWinner->cbSameData[0];

			//对比大小
			if (CompareCardData(cbFisrtCardData,cbNextCardData)==true) return true;
		}
	}

	return false;
}

//对比扑克
WORD CGameLogic::CompareCardArray(const BYTE cbOutCardData[GAME_PLAYER][MAX_COUNT], BYTE cbCardCount, WORD wFirstIndex)
{
	//变量定义
	WORD wWinnerIndex=wFirstIndex;
	BYTE cbCardType=GetCardType(cbOutCardData[wFirstIndex],cbCardCount);

	//对比扑克
	switch (cbCardType)
	{
	case CT_SINGLE:		//单牌类型
	case CT_SAME_2:		//对牌类型
	case CT_SAME_3:		//三牌类型
	case CT_SAME_4:		//四牌类型
	case CT_TRACKOR_2:	//拖拉机型
	case CT_TRACKOR_3:	//拖拉机型
	case CT_TRACKOR_4:	//拖拉机型
		{
			//循环比较
			for (WORD i=0;i<GAME_PLAYER-1;i++)
			{
				//用户索引
				WORD wUserIndex=(wFirstIndex+i+1)%GAME_PLAYER;

				//牌型判断
				if (GetCardType(cbOutCardData[wUserIndex],cbCardCount)!=cbCardType) continue;

				//对比大小
				if (CompareCardData(cbOutCardData[wWinnerIndex][0],cbOutCardData[wUserIndex][0])==true) 
				{
					wWinnerIndex=wUserIndex;
					continue;
				}
			}

			return wWinnerIndex;
		}
	case CT_THROW_CARD:	//甩牌类型
		{
			//花色判断
			if (GetCardLogicColor(cbOutCardData[wWinnerIndex][0])==COLOR_NT) return wFirstIndex;

			//分析牌型
			tagAnalyseResult WinnerResult;
			AnalyseCardData(cbOutCardData[wFirstIndex],cbCardCount,WinnerResult);

			//循环比较
			for (WORD i=0;i<GAME_PLAYER-1;i++)
			{
				//规则判断
				WORD wUserIndex=(wFirstIndex+i+1)%GAME_PLAYER;
				if (GetCardLogicColor(cbOutCardData[wUserIndex],cbCardCount)!=COLOR_NT) continue;

				//构造牌型
				tagAnalyseResult UserResult;
				if (RectifyCardWeave(cbOutCardData[wUserIndex],cbCardCount,WinnerResult,UserResult)==false) continue;

				//对比结果
				if (CompareCardResult(WinnerResult,UserResult)==true)
				{
					wWinnerIndex=wUserIndex;
					CopyMemory(&WinnerResult,&UserResult,sizeof(WinnerResult));
				}
			}

			return wWinnerIndex;
		}
	}

	//错误断言
	ASSERT(FALSE);

	return wFirstIndex;
}

//分析扑克
bool CGameLogic::AnalyseCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResult & AnalyseResult)
{
	//设置结果
	ZeroMemory(&AnalyseResult,sizeof(AnalyseResult));

	//花色判断
	AnalyseResult.cbCardColor=GetCardLogicColor(cbCardData,cbCardCount);
	if (AnalyseResult.cbCardColor==COLOR_ERROR) return false;

	//变量定义
	BYTE cbAnalyseCard[MAX_COUNT];
	BYTE cbAnalyseCount=cbCardCount;
	CopyMemory(cbAnalyseCard,cbCardData,cbCardCount*sizeof(BYTE));

	//提取扑克
	for (BYTE i=2;i>=2;i--)
	{
		//提取拖牌
		tagTractorDataInfo * pTractorDataInfo=&AnalyseResult.TractorDataInfo[i-2];
		DistillTrackorByCount(cbAnalyseCard,cbAnalyseCount,i,AnalyseResult.TractorDataInfo[i-2]);

		//拖牌处理
		if (pTractorDataInfo->cbTractorCount>0)
		{
			BYTE cbIndex=0,cbRemoveCount=0;
			for (BYTE j=0;j<pTractorDataInfo->cbTractorCount;j++)
			{
				//变量计算
				cbRemoveCount=pTractorDataInfo->cbTractorData[cbIndex]*i;

				//删除扑克
				cbAnalyseCount-=cbRemoveCount;
				RemoveCard(&pTractorDataInfo->cbTractorData[cbIndex+2],cbRemoveCount,cbAnalyseCard,cbAnalyseCount+cbRemoveCount);

				//设置变量
				cbIndex+=cbRemoveCount+2;
			}
		}

		//提取同牌
		tagSameDataInfo * pSameDataInfo=&AnalyseResult.SameDataInfo[i-1];
		DistillCardByCount(cbAnalyseCard,cbAnalyseCount,i,AnalyseResult.SameDataInfo[i-1]);

		//删除扑克
		if (pSameDataInfo->cbBlockCount>0)
		{
			cbAnalyseCount-=pSameDataInfo->cbCardCount;
			RemoveCard(pSameDataInfo->cbSameData,pSameDataInfo->cbCardCount,cbAnalyseCard,cbAnalyseCount+pSameDataInfo->cbCardCount);
		}
	}

	//提取单牌
	AnalyseResult.SameDataInfo[0].cbCardCount=cbAnalyseCount;
	AnalyseResult.SameDataInfo[0].cbBlockCount=cbAnalyseCount;
	CopyMemory(AnalyseResult.SameDataInfo[0].cbSameData,cbAnalyseCard,cbAnalyseCount*sizeof(BYTE));

	return true;
}

//出牌判断
bool CGameLogic::SearchOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount,const  BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResult & OutCardResult)
{
	//设置变量
	ZeroMemory(&OutCardResult,sizeof(OutCardResult));

	//首出用户
	if (cbTurnCardCount==0)
	{
		for (BYTE i=2;i>=2;i--)
		{
			//提取拖牌
			tagTractorDataInfo TractorDataInfo;
			DistillTrackorByCount(cbHandCardData,cbHandCardCount,i,TractorDataInfo);

			//拖牌判断
			if (TractorDataInfo.cbTractorCount>0)
			{
				//获取属性
				BYTE cbLineCount=TractorDataInfo.cbTractorData[0];
				BYTE cbSameCount=TractorDataInfo.cbTractorData[1];
				
				//构造结果
				OutCardResult.cbCardCount=cbLineCount*cbSameCount;
				CopyMemory(OutCardResult.cbResultCard,&TractorDataInfo.cbTractorData[2],sizeof(BYTE)*OutCardResult.cbCardCount);

				return true;
			}

			//提取同牌
			tagSameDataInfo SameDataInfo;
			DistillCardByCount(cbHandCardData,cbHandCardCount,i,SameDataInfo);

			//同牌判断
			if (SameDataInfo.cbCardCount>0)
			{
				//构造结果
				OutCardResult.cbCardCount=i;
				CopyMemory(OutCardResult.cbResultCard,&SameDataInfo.cbSameData[0],sizeof(BYTE)*i);

				return true;
			}
		}

		//提取单牌
		OutCardResult.cbCardCount=1;
		OutCardResult.cbResultCard[0]=cbHandCardData[0];

		return true;
	}

	//分析扑克
	tagAnalyseResult AnalyseResult;
	AnalyseCardData(cbTurnCardData,cbTurnCardCount,AnalyseResult);

	//提取扑克
	BYTE cbAnalyseCard[MAX_COUNT];
	BYTE cbAnalyseCardCount=DistillCardByColor(cbHandCardData,cbHandCardCount,GetCardLogicColor(cbTurnCardData[0]),cbAnalyseCard);

	//数目判断
	if (cbTurnCardCount>=cbAnalyseCardCount)
	{
		//构造扑克
		OutCardResult.cbCardCount=cbAnalyseCardCount;
		CopyMemory(OutCardResult.cbResultCard,cbAnalyseCard,sizeof(BYTE)*cbAnalyseCardCount);

		//剩余扑克
		if (cbTurnCardCount>cbAnalyseCardCount)
		{
			//变量定义
			BYTE cbSourceCount=OutCardResult.cbCardCount;
			BYTE cbLeaveCount=(cbTurnCardCount-cbAnalyseCardCount);

			//构造扑克
			BYTE cbCardDataTemp[MAX_COUNT];
			BYTE cbCardCountTemp=cbHandCardCount;
			CopyMemory(cbCardDataTemp,cbHandCardData,sizeof(BYTE)*cbHandCardCount);

			//删除扑克
			cbCardCountTemp-=cbAnalyseCardCount;
			RemoveCard(cbAnalyseCard,cbAnalyseCardCount,cbCardDataTemp,cbCardCountTemp+cbAnalyseCardCount);

			//构造结果
			OutCardResult.cbCardCount+=cbLeaveCount;
			CopyMemory(&OutCardResult.cbResultCard[cbSourceCount],&cbCardDataTemp[cbCardCountTemp-cbLeaveCount],sizeof(BYTE)*cbLeaveCount);
		}

		return true;
	}

	//需求变量
	tagDemandInfo DemandInfo;
	ZeroMemory(&DemandInfo,sizeof(DemandInfo));

	//需求分析
	for (BYTE i=2;i>=2;i--)
	{
		//变量定义
		tagSameDataInfo * pSameDataInfo=&AnalyseResult.SameDataInfo[i-1];
		tagTractorDataInfo * pTractorDataInfo=&AnalyseResult.TractorDataInfo[i-2];

		//拖牌需求
		if (pTractorDataInfo->cbTractorCount>0)
		{
			DemandInfo.cbTractorCardCount[i-2]=pTractorDataInfo->cbCardCount;
			DemandInfo.cbMaxTractor[i-2]=pTractorDataInfo->cbTractorMaxLength;
		}

		//同牌需求
		if (pSameDataInfo->cbBlockCount>0) DemandInfo.cbSameCardCount[i-1]=pSameDataInfo->cbCardCount;
	}

	//单牌需求
	if (AnalyseResult.SameDataInfo[0].cbCardCount>0) DemandInfo.cbSameCardCount[0]=AnalyseResult.SameDataInfo[0].cbCardCount;

	//出牌搜索
	for (BYTE i=2;i>=2;i--)
	{
		//拖牌判断
		if (DemandInfo.cbTractorCardCount[i-2]>0)
		{
			//提取拖牌
			tagTractorDataInfo TractorDataInfo;
			DistillTrackorByCount(cbAnalyseCard,cbAnalyseCardCount,i,TractorDataInfo);

			//变量定义
			BYTE cbIndex=0;
			BYTE cbLeaveCount=DemandInfo.cbTractorCardCount[i-2];

			//提取扑克
			for (BYTE j=0;j<TractorDataInfo.cbTractorCount;j++)
			{
				//调整索引
				if (j==0) cbIndex=TractorDataInfo.cbTractorMaxIndex;
				if ((j==1)&&(TractorDataInfo.cbTractorMaxIndex!=0)) cbIndex=0;

				//调整索引
				if ((j!=0)&&(cbIndex==TractorDataInfo.cbTractorMaxIndex))
				{
					//获取属性
					BYTE cbLineCount=TractorDataInfo.cbTractorData[cbIndex];
					BYTE cbSameCount=TractorDataInfo.cbTractorData[cbIndex+1];

					//设置索引
					cbIndex+=cbLineCount*cbSameCount+2;
				}

				//获取属性
				BYTE cbLineCount=TractorDataInfo.cbTractorData[cbIndex];
				BYTE cbSameCount=TractorDataInfo.cbTractorData[cbIndex+1];

				//提取变量
				BYTE cbTractorCardCount=cbLineCount*cbSameCount;
				BYTE cbTakeCardCount=__min(cbTractorCardCount,cbLeaveCount);

				//提取扑克
				OutCardResult.cbCardCount+=cbTakeCardCount;
				CopyMemory(&OutCardResult.cbResultCard[OutCardResult.cbCardCount-cbTakeCardCount],
					&TractorDataInfo.cbTractorData[cbIndex+2+cbTractorCardCount-cbTakeCardCount],sizeof(BYTE)*cbTakeCardCount);

				//删除扑克
				cbLeaveCount-=cbTakeCardCount;
				cbAnalyseCardCount-=cbTakeCardCount;
				RemoveCard(&TractorDataInfo.cbTractorData[cbIndex+2+cbTractorCardCount-cbTakeCardCount],cbTakeCardCount,
					cbAnalyseCard,cbAnalyseCardCount+cbTakeCardCount);

				//设置索引
				cbIndex+=cbLineCount*cbSameCount+2;
			}

			//设置需求
			DemandInfo.cbSameCardCount[i-1]+=cbLeaveCount;
		}

		//同牌判断
		if (DemandInfo.cbSameCardCount[i-1]>0)
		{
			//提取同牌
			tagSameDataInfo SameDataInfo;
			DistillCardByCount(cbAnalyseCard,cbAnalyseCardCount,i,SameDataInfo);

			//提取分析
			BYTE cbTakeCardCount=__min(SameDataInfo.cbCardCount,DemandInfo.cbSameCardCount[i-1]);
			cbTakeCardCount=cbTakeCardCount-cbTakeCardCount%i;

			//构造结果
			OutCardResult.cbCardCount+=cbTakeCardCount;
			CopyMemory(&OutCardResult.cbResultCard[OutCardResult.cbCardCount-cbTakeCardCount],
				&SameDataInfo.cbSameData[SameDataInfo.cbCardCount-cbTakeCardCount],sizeof(BYTE)*cbTakeCardCount);

			//删除扑克
			cbAnalyseCardCount-=cbTakeCardCount;
			RemoveCard(&SameDataInfo.cbSameData[SameDataInfo.cbCardCount-cbTakeCardCount],cbTakeCardCount,cbAnalyseCard,cbAnalyseCardCount+cbTakeCardCount);

			//设置需求
			DemandInfo.cbSameCardCount[i-2]+=(DemandInfo.cbSameCardCount[i-1]-cbTakeCardCount);
		}
	}

	//提取单牌
	if (DemandInfo.cbSameCardCount[0]>0)
	{
		//变量定义
		BYTE cbSourceCount=OutCardResult.cbCardCount;
		BYTE cbLeaveCount=DemandInfo.cbSameCardCount[0];

		//构造结果
		OutCardResult.cbCardCount+=cbLeaveCount;
		CopyMemory(&OutCardResult.cbResultCard[cbSourceCount],&cbAnalyseCard[cbAnalyseCardCount-cbLeaveCount],sizeof(BYTE)*cbLeaveCount);
	}

	return true;
}

//效验出牌
bool CGameLogic::EfficacyOutCard(const BYTE cbOutCardData[], BYTE cbOutCardCount, const BYTE cbFirstCardData[], BYTE cbFirstCardCount, const BYTE cbHandCardData[], BYTE cbHandCardCount)
{
	//状态效验
	ASSERT(m_cbMainColor!=COLOR_ERROR);

	//数目判断
	if (cbOutCardCount!=cbFirstCardCount) return false;

	//扑克类型
	BYTE cbOutCardType=GetCardType(cbOutCardData,cbOutCardCount);
	BYTE cbFirstCardType=GetCardType(cbFirstCardData,cbFirstCardCount);

	//逻辑花色
	BYTE cbOutLogicColor=GetCardLogicColor(cbOutCardData,cbOutCardCount);
	BYTE cbFirstLogicColor=GetCardLogicColor(cbFirstCardData,cbFirstCardCount);

	//同型判断
	ASSERT(cbFirstCardType!=CT_ERROR);
	if ((cbFirstCardType!=CT_THROW_CARD)&&(cbOutLogicColor==cbFirstLogicColor)&&(cbOutCardType==cbFirstCardType)) return true;

	//提取扑克
	BYTE cbDistillCardOut[MAX_COUNT];
	BYTE cbDistillCardHand[MAX_COUNT];
	BYTE cbDistillCountOut=DistillCardByColor(cbOutCardData,cbOutCardCount,cbFirstLogicColor,cbDistillCardOut);
	BYTE cbDistillCountHand=DistillCardByColor(cbHandCardData,cbHandCardCount,cbFirstLogicColor,cbDistillCardHand);
		
	//花色判断
	if (cbFirstLogicColor!=cbOutLogicColor) return (cbDistillCountOut==cbDistillCountHand);
	
	//变量定义
	tagSameDataInfo SameDataInfo[3];
	tagTractorDataInfo TractorDataInfo[3];

	//扑克分析
	for (BYTE i=2;i>=2;i--)
	{
		//提取拖牌
		DistillTrackorByCount(cbFirstCardData,cbFirstCardCount,i,TractorDataInfo[INDEX_FIRST]);

		//拖牌判断
		if (TractorDataInfo[INDEX_FIRST].cbTractorCount>0)
		{
			//提取拖牌
			DistillTrackorByCount(cbDistillCardOut,cbDistillCountOut,i,TractorDataInfo[INDEX_OUT]);
			DistillTrackorByCount(cbDistillCardHand,cbDistillCountHand,i,TractorDataInfo[INDEX_HAND]);

			//出牌判断
			if (TractorDataInfo[INDEX_HAND].cbCardCount>TractorDataInfo[INDEX_OUT].cbCardCount)
			{
				//数目判断
				if (TractorDataInfo[INDEX_FIRST].cbCardCount==(TractorDataInfo[INDEX_OUT].cbCardCount+i))
				{
					//提取同牌
					tagSameDataInfo SameDataInfoOut;
					DistillCardByCount(cbDistillCardOut,cbDistillCountOut,i,SameDataInfoOut);

					//提取拖牌
					BYTE cbHandTractorData[MAX_COUNT];
					BYTE cbHandTractorCardCount=0,cbIndex=0;
					for (BYTE j=0;j<TractorDataInfo[INDEX_HAND].cbTractorCount;j++)
					{
						//获取属性
						BYTE cbLineCount=TractorDataInfo[INDEX_HAND].cbTractorData[cbIndex];
						BYTE cbSameCount=TractorDataInfo[INDEX_HAND].cbTractorData[cbIndex+1];

						//设置扑克
						BYTE cbSourceCount=cbHandTractorCardCount;
						cbHandTractorCardCount+=cbLineCount*cbSameCount;
						CopyMemory(&cbHandTractorData[cbSourceCount],&TractorDataInfo[INDEX_HAND].cbTractorData[cbIndex+2],cbLineCount*cbSameCount*sizeof(BYTE));

						//设置索引
						cbIndex+=(2+cbLineCount*cbSameCount);
					}

					//交集判断
					BYTE cbIntersectionCount=GetIntersectionCount(SameDataInfoOut.cbSameData,SameDataInfoOut.cbCardCount,cbHandTractorData,cbHandTractorCardCount);
					if (cbIntersectionCount<TractorDataInfo[INDEX_FIRST].cbCardCount) return false;
				}

				//数目判断
				if (TractorDataInfo[INDEX_FIRST].cbCardCount>(TractorDataInfo[INDEX_OUT].cbCardCount+i)) return false;
			}
		}

		//提取同牌
		DistillCardByCount(cbFirstCardData,cbFirstCardCount,i,SameDataInfo[INDEX_FIRST]);

		//同牌判断
		if (SameDataInfo[INDEX_FIRST].cbBlockCount>0)
		{
			//提取同牌
			DistillCardByCount(cbDistillCardOut,cbDistillCountOut,i,SameDataInfo[INDEX_OUT]);
			DistillCardByCount(cbDistillCardHand,cbDistillCountHand,i,SameDataInfo[INDEX_HAND]);

			//数目判断
			BYTE cbBlockCountOut=SameDataInfo[INDEX_OUT].cbBlockCount;
			BYTE cbBlockCountHand=SameDataInfo[INDEX_HAND].cbBlockCount;
			BYTE cbBlockCountFirst=SameDataInfo[INDEX_FIRST].cbBlockCount;
			if ((cbBlockCountFirst>cbBlockCountOut)&&(cbBlockCountHand>cbBlockCountOut)) return false;
		}
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

//排序等级
BYTE CGameLogic::GetCardSortOrder(BYTE cbCardData)
{
	//逻辑数值
	BYTE cbCardColor=GetCardColor(cbCardData);
	BYTE cbCardValue=GetCardValue(cbCardData);

	//花色变换
	BYTE cbSortColor=cbCardColor;
	if ((cbCardColor==m_cbMainColor)) cbSortColor=0x40;

	//数值变换
	BYTE cbSortValue=cbCardValue;
	if (cbCardValue==1) cbSortValue+=13;

	return cbSortValue+m_cbSortRight[cbSortColor>>4];

}

//逻辑数值
BYTE CGameLogic::GetCardLogicValue(BYTE cbCardData)
{
	//逻辑数值
	BYTE cbCardColor=GetCardColor(cbCardData);
	BYTE cbCardValue=GetCardValue(cbCardData);
	BYTE cbNTRight=0;

	//花色变换
	BYTE cbLogicColor=cbCardColor;
	//if ((cbCardColor==m_cbMainColor)) cbLogicColor=0x40;

	//转换变换
	BYTE cbLogicValue=cbCardValue;
	if (cbCardValue==1) cbLogicValue+=13;

	//紧凑处理
	if (cbCardValue!=1)
	{
		 cbLogicValue++;
	}
	return cbLogicValue+m_cbSortRight[cbLogicColor>>4];
}

//逻辑花色
BYTE CGameLogic::GetCardLogicColor(BYTE cbCardData)
{
	//逻辑数值
	BYTE cbCardColor=GetCardColor(cbCardData);
	BYTE cbCardValue=GetCardValue(cbCardData);

	//花色判断
	if (cbCardColor==COLOR_NT) return COLOR_NT;
	if (cbCardColor==m_cbMainColor) return COLOR_NT;
	
	return cbCardColor;
}

//牌列花色
BYTE CGameLogic::GetCardLogicColor(const BYTE cbCardData[], BYTE cbCardCount)
{
	//获取花色
	BYTE cbFirstColor=GetCardLogicColor(cbCardData[0]);
	BYTE cbSecondColor=GetCardLogicColor(cbCardData[cbCardCount-1]);

	return (cbFirstColor==cbSecondColor)?cbFirstColor:COLOR_ERROR;
}

//是否相连
bool CGameLogic::IsLineValue(BYTE cbFirstCard, BYTE cbSecondCard)
{
	//获取数值
	BYTE cbFirstLogicValue=GetCardLogicValue(cbFirstCard);
	BYTE cbSecondLogicValue=GetCardLogicValue(cbSecondCard);

	//相连判断
	return (cbFirstLogicValue==(cbSecondLogicValue+1));
}

//是否同花
bool CGameLogic::IsSameColor(BYTE cbFirstCard, BYTE cbSecondCard)
{
	//获取花色
	BYTE cbFirstLogicColor=GetCardLogicColor(cbFirstCard);
	BYTE cbSecondLogicColor=GetCardLogicColor(cbSecondCard);

	//同花判断
	return (cbFirstLogicColor==cbSecondLogicColor);
}

//提取扑克
BYTE CGameLogic::DistillCardByColor(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbCardColor, BYTE cbResultCard[])
{
	//变量定义
	BYTE cbResultCount=0;

	//提取扑克
	for (BYTE i=0;i<cbCardCount;i++)
	{
		//花色判断
		if (GetCardLogicColor(cbCardData[i])==cbCardColor) 
		{
			cbResultCard[cbResultCount++]=cbCardData[i];
			continue;
		}

		//终止判断
		if (cbResultCount>0) break;
	}

	return cbResultCount;
}

//提取扑克
BYTE CGameLogic::DistillCardByCount(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbSameCount, tagSameDataInfo & SameDataInfo)
{
	//参数效验
	ASSERT((cbSameCount>=2)&&(cbSameCount<=2));
	if ((cbSameCount<2)||(cbSameCount>2)) return 0;

	//设置变量
	ZeroMemory(&SameDataInfo,sizeof(SameDataInfo));

	//提取判断
	if (cbCardCount<cbSameCount) return 0;

	//提取扑克
	for (BYTE i=0;i<cbCardCount-cbSameCount+1;i++)
	{
		//数目判断
		BYTE j;
		for (j=1;j<cbSameCount;j++)
		{
			if (cbCardData[i]!=cbCardData[i+j]) break;
		}

		//成功处理
		if (j==cbSameCount)
		{
			//同牌属性
			SameDataInfo.cbBlockCount++;

			//扑克数据
			for (BYTE k=0;k<cbSameCount;k++)
			{
				SameDataInfo.cbSameData[SameDataInfo.cbCardCount++]=cbCardData[i+k];
			}
		}

		//递增处理
		i+=j-1;
	}

	return SameDataInfo.cbBlockCount;
}

//提取扑克
BYTE CGameLogic::DistillTrackorByCount(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbSameCount, tagTractorDataInfo & TractorDataInfo)
{
	//参数效验
	ASSERT((cbSameCount>=2)&&(cbSameCount<=2));
	if ((cbSameCount<2)||(cbSameCount>2)) return 0;

	//设置变量
	ZeroMemory(&TractorDataInfo,sizeof(TractorDataInfo));

	//提取同牌
	tagSameDataInfo SameDataInfo;
	DistillCardByCount(cbCardData,cbCardCount,cbSameCount,SameDataInfo);

	//牌段判断
	if (SameDataInfo.cbBlockCount<2) return 0;

	//分析处理
	BYTE cbResultIndex=0;
	for (BYTE i=0;i<SameDataInfo.cbBlockCount-1;i++)
	{
		//分析过滤
		if (SameDataInfo.cbSameData[i*cbSameCount]==0) continue;

		//变量定义
		BYTE cbLineCount=1;
		BYTE cbTrackorIndex[MAX_TRACKOR]={i*cbSameCount};

		//连牌判断
		for (BYTE j=i;j<SameDataInfo.cbBlockCount-1;j++)
		{
			//分析过滤
			if (SameDataInfo.cbSameData[(j+1)*cbSameCount]==0) continue;

			//变量定义
			BYTE cbSecondIndex=(j+1)*cbSameCount;
			BYTE cbFirstIndex=cbTrackorIndex[cbLineCount-1];
			BYTE cbFirstCardData=SameDataInfo.cbSameData[cbFirstIndex];
			BYTE cbSecondCardData=SameDataInfo.cbSameData[cbSecondIndex];

			//连牌判断
			if (IsLineValue(cbFirstCardData,cbSecondCardData)==true)
			{
				ASSERT(cbLineCount<MAX_TRACKOR);
				cbTrackorIndex[cbLineCount++]=cbSecondIndex;
				continue;
			}

			//副主间隔
			if (cbFirstCardData==cbSecondCardData) continue;
			
			break;
		}

		//成功判断
		if (cbLineCount>=2)
		{
			//最大长度
			if (cbLineCount>TractorDataInfo.cbTractorMaxLength)
			{
				TractorDataInfo.cbTractorMaxLength=cbLineCount;
				TractorDataInfo.cbTractorMaxIndex=cbResultIndex;
			}

			//设置结果
			TractorDataInfo.cbTractorCount++;
			TractorDataInfo.cbCardCount+=cbLineCount*cbSameCount;
			TractorDataInfo.cbTractorData[cbResultIndex++]=cbLineCount;
			TractorDataInfo.cbTractorData[cbResultIndex++]=cbSameCount;

			//设置数据
			for (BYTE k=0;k<cbLineCount;k++)
			{
				BYTE cbIndex=cbTrackorIndex[k];
				for (BYTE l=0;l<cbSameCount;l++)
				{
					TractorDataInfo.cbTractorData[cbResultIndex++]=SameDataInfo.cbSameData[cbIndex+l];
					SameDataInfo.cbSameData[cbIndex+l]=0;
				}
			}
		}
	}

	return TractorDataInfo.cbTractorCount;
}

//更新权位
void CGameLogic::UpdateSortRight()
{
	//设置权位
	for (BYTE i=0;i<CountArray(m_cbSortRight);i++) m_cbSortRight[i]=i*COLOR_RIGHT;
	/*0=0	    c
	1=40	d
	2=80    h
	3=120   s
	4=160   n
	*/
	//主牌权位
	if (m_cbMainColor!=COLOR_ERROR) m_cbSortRight[m_cbMainColor>>4]=4*COLOR_RIGHT;
	/*0=0		c
	1=40    d
	2=80    h
	3=120   s
	4=160   n
	*/
	

	//调整权位
	switch (m_cbMainColor)
	{
		case COLOR_FANG_KUAI:	 //方块
		{
			m_cbSortRight[COLOR_HEI_TAO>>4]=3*COLOR_RIGHT;
			break;
		}
		case COLOR_HONG_TAO:	//红桃
		{
			m_cbSortRight[COLOR_MEI_HUA>>4]=0*COLOR_RIGHT;
			break;
		}
		/*
		case COLOR_NT:         //无将
		{
			m_cbSortRight[COLOR_MEI_HUA>>4]=0*COLOR_RIGHT;
			m_cbSortRight[COLOR_FANG_KUAI>>4]=1*COLOR_RIGHT;
			m_cbSortRight[COLOR_HONG_TAO>>4]=2*COLOR_RIGHT;
			m_cbSortRight[COLOR_HEI_TAO>>4]=3*COLOR_RIGHT;
			break;
		}
		*/
	}
	

	return;
}

//调整扑克
bool CGameLogic::RectifyCardWeave(const BYTE cbCardData[], BYTE cbCardCount, const tagAnalyseResult & TargetResult, tagAnalyseResult & RectifyResult)
{
	//设置变量
	ZeroMemory(&RectifyResult,sizeof(RectifyResult));

	//花色判断
	RectifyResult.cbCardColor=GetCardLogicColor(cbCardData,cbCardCount);
	if (RectifyResult.cbCardColor==COLOR_ERROR) return false;

	//构造扑克
	BYTE cbTempCardData[MAX_COUNT];
	BYTE cbTempCardCount=cbCardCount;
	CopyMemory(cbTempCardData,cbCardData,sizeof(BYTE)*cbCardCount);

	//循环调整
	for (BYTE i=2;i>=2;i--)
	{
		//调整变量
		tagSameDataInfo * pSameDataInfoRectify=&RectifyResult.SameDataInfo[i-1];
		tagTractorDataInfo * pTractorDataInfoRectify=&RectifyResult.TractorDataInfo[i-2];

		//目标变量
		const tagSameDataInfo * pSameDataInfoTarget=&TargetResult.SameDataInfo[i-1];
		const tagTractorDataInfo * pTractorDataInfoTarget=&TargetResult.TractorDataInfo[i-2];

		//调整拖牌
		if (pTractorDataInfoTarget->cbTractorCount>0)
		{
			//提取拖牌
			tagTractorDataInfo TractorDataInfo;
			DistillTrackorByCount(cbTempCardData,cbTempCardCount,i,TractorDataInfo);

			//数据判断
			if (pTractorDataInfoTarget->cbCardCount>TractorDataInfo.cbCardCount) return false;
			if (pTractorDataInfoTarget->cbTractorMaxLength>TractorDataInfo.cbTractorMaxLength) return false;

			//变量定义
			BYTE cbCollectCount=0;
			BYTE cbCollectCardData[MAX_COUNT];

			//设置属性
			pTractorDataInfoRectify->cbTractorMaxIndex=255;
			pTractorDataInfoRectify->cbTractorMaxLength=pTractorDataInfoTarget->cbTractorMaxLength;

			//设置结果
			BYTE cbIndex=0;
			for (BYTE j=0;j<TractorDataInfo.cbTractorCount;j++)
			{
				//变量定义
				BYTE cbRectifyLineCount=TractorDataInfo.cbTractorData[cbIndex];
				BYTE cbResidualLineCount=(pTractorDataInfoTarget->cbCardCount-cbCollectCount)/i;

				//属性定义
				BYTE cbSameCount=i;
				BYTE cbLineCount=__min(cbRectifyLineCount,cbResidualLineCount);

				//设置属性
				pTractorDataInfoRectify->cbTractorCount++;
				pTractorDataInfoRectify->cbCardCount+=cbLineCount*cbSameCount;

				//设置属性
				pTractorDataInfoRectify->cbTractorData[cbIndex]=cbLineCount;
				pTractorDataInfoRectify->cbTractorData[cbIndex+1]=cbSameCount;

				//设置扑克
				BYTE cbInsertCount=cbLineCount*cbSameCount;
				CopyMemory(&cbCollectCardData[cbCollectCount],&TractorDataInfo.cbTractorData[cbIndex+2],cbInsertCount*sizeof(BYTE));
				CopyMemory(&pTractorDataInfoRectify->cbTractorData[cbIndex+2],&TractorDataInfo.cbTractorData[cbIndex+2],cbInsertCount*sizeof(BYTE));

				//设置变量
				cbIndex+=(cbInsertCount+2);
				cbCollectCount+=cbInsertCount;

				//完成判断
				if (cbCollectCount>=pTractorDataInfoTarget->cbCardCount) break;
			}

			//删除扑克
			cbTempCardCount-=cbCollectCount;
			RemoveCard(cbCollectCardData,cbCollectCount,cbTempCardData,cbTempCardCount+cbCollectCount);
		}

		//调整同牌
		if (pSameDataInfoTarget->cbCardCount>0)
		{
			//提取同牌
			tagSameDataInfo SameDataInfo;
			DistillCardByCount(cbTempCardData,cbTempCardCount,i,SameDataInfo);

			//数据判断
			if (pSameDataInfoTarget->cbCardCount>SameDataInfo.cbCardCount) return false;
			if (pSameDataInfoTarget->cbBlockCount>SameDataInfo.cbBlockCount) return false;

			//设置扑克
			pSameDataInfoRectify->cbCardCount=pSameDataInfoTarget->cbCardCount;
			pSameDataInfoRectify->cbBlockCount=pSameDataInfoTarget->cbBlockCount;
			CopyMemory(pSameDataInfoRectify->cbSameData,SameDataInfo.cbSameData,sizeof(BYTE)*pSameDataInfoRectify->cbCardCount);

			//变量定义
			BYTE cbRemoveCount=pSameDataInfoRectify->cbCardCount;
			LPBYTE pcbRemoveCard=&pSameDataInfoRectify->cbSameData[0];

			//删除扑克
			cbTempCardCount-=cbRemoveCount;
			RemoveCard(pcbRemoveCard,cbRemoveCount,cbTempCardData,cbTempCardCount+cbRemoveCount);
		}
	}

	//设置单牌
	if (cbTempCardCount>0)
	{
		RectifyResult.SameDataInfo[0].cbCardCount=cbTempCardCount;
		RectifyResult.SameDataInfo[0].cbBlockCount=cbTempCardCount;
		CopyMemory(RectifyResult.SameDataInfo[0].cbSameData,cbTempCardData,cbTempCardCount);
	}

	return true;
}

//交集数目
BYTE CGameLogic::GetIntersectionCount(const BYTE cbCardData1[], BYTE cbCardCount1, const BYTE cbCardData2[], BYTE cbCardCount2)
{
	//变量定义
	BYTE cbAnalyseCard[MAX_COUNT];
	CopyMemory(cbAnalyseCard,cbCardData2,sizeof(BYTE)*cbCardCount2);

	//分析判断
	BYTE cbIntersectionCount=0;
	for (BYTE i=0;i<cbCardCount1;i++)
	{
		for (BYTE j=0;j<cbCardCount2;j++)
		{
			if (cbCardData1[i]==cbAnalyseCard[j])
			{
				//设置变量
				cbAnalyseCard[j]=0;
				cbIntersectionCount++;

				break;
			}
		}
	}

	return cbIntersectionCount;
}

//////////////////////////////////////////////////////////////////////////
