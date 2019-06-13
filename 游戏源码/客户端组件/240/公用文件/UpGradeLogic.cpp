#include "StdAfx.h"
#include "UpGradeLogic.h"

//构造函数
CUpGradeGameLogic::CUpGradeGameLogic(void)
{
	//始终打7
	m_iNTNum=10;
	m_iNTHuaKind=UG_ERROR_HUA;
	m_iStation[4]=500;
	for (int i=0;i<4;i++) m_iStation[i]=100*i;
	//从小到大放入容器中
	m_MustNTList.push_back(0x02);
}

//设置主牌花色
void CUpGradeGameLogic::SetNTHuaKind(BYTE iNTHuaKind)
{
	if(iNTHuaKind==UG_NO_NT)
	{
		m_iNTHuaKind=UG_ERROR_HUA;
		return;
	}
	//设置数据
	m_iNTHuaKind=iNTHuaKind & UG_HUA_MASK;
	return;
}
BOOL CUpGradeGameLogic::IsMustNT(BYTE bCard)
{
	BYTE bNum =GetCardNum(bCard);
	if(bNum==m_iNTNum ||bCard>0x40)
	{
		return TRUE;
	}
	if(find(m_MustNTList.begin(),m_MustNTList.end(),bNum) !=m_MustNTList.end())
	{
		return TRUE;
	}
	return FALSE;
}
//获取扑克花色
BYTE CUpGradeGameLogic::GetCardHuaKind(BYTE iCard, BOOL bTrueHua)
{
	int iHuaKind=(iCard&UG_HUA_MASK);
	if (!bTrueHua)
	{
		BYTE bNum =GetCardNum(iCard);
		if ((iHuaKind==m_iNTHuaKind)||(bNum==m_iNTNum)||(find(m_MustNTList.begin(),m_MustNTList.end(),bNum) !=m_MustNTList.end()))
			return iHuaKind=UG_NT_CARD;
	}
	return iHuaKind; 
}

//获取扑克大小 （2 - 18 ， 15 以上是主牌 ： 2 - 21 ， 15 以上是主）
//true用做排序
//false用判断是否是拖拉机
int CUpGradeGameLogic::GetCardBulk(BYTE iCard, BOOL bExtVol)
{
	//如果当前打的级别包含在常主中
	bool bIsInclude=true;
	if(find(m_MustNTList.begin(),m_MustNTList.end(),m_iNTNum)==m_MustNTList.end())
	{
		bIsInclude =false;		
	}
	int  iMustNTSize =bIsInclude ?(int)(m_MustNTList.size()-1):(int)m_MustNTList.size();

	BYTE bFromNum= 0x0F-(1+iMustNTSize);
	int iCardNum=GetCardNum(iCard);

	if (iCardNum==m_iNTNum)
	{
		int iHuaKind=GetCardHuaKind(iCard,TRUE);
		BYTE bAddNum =bExtVol?iMustNTSize*4:iMustNTSize*2;
		bFromNum += bAddNum;
		if (bExtVol)
		{
			if (iHuaKind==m_iNTHuaKind) return bFromNum+3;
			//副常主退位
			if(iHuaKind>m_iNTHuaKind) return (iHuaKind>>4)+bFromNum-1;
			return (iHuaKind>>4)+bFromNum;

		}
		return (iHuaKind==m_iNTHuaKind)?bFromNum+1:bFromNum;
	}
	//如果是常主
	vector<BYTE>::iterator posIte=find(m_MustNTList.begin(),m_MustNTList.end(),iCardNum);
	if(posIte !=m_MustNTList.end())
	{
		int bPosSize = static_cast<int>(posIte -m_MustNTList.begin());
		//如果含主，退一位
		if(*posIte >m_iNTNum &&bIsInclude)
		{
			bPosSize--;
		}
		BYTE bAddNum =bExtVol?bPosSize*4:bPosSize*2;
		bFromNum += bAddNum;
		int iHuaKind=GetCardHuaKind(iCard,TRUE);
		if(bExtVol)
		{
			if (iHuaKind==m_iNTHuaKind) return bFromNum+3;
			//副常主退位
			if(iHuaKind>m_iNTHuaKind) return (iHuaKind>>4)+bFromNum-1;
			return (iHuaKind>>4)+bFromNum;
		}
		return (iHuaKind==m_iNTHuaKind)?bFromNum+1:bFromNum;
	}



	if ((iCard==0x4E)||(iCard==0x4F))
	{
		BYTE bAddNum =bExtVol?(1+iMustNTSize)*4:(1+iMustNTSize)*2;
		bFromNum += bAddNum;
		return iCard==0x4E?bFromNum:bFromNum+1;
	}
	//其他平常的牌要缩小
	vector<BYTE>::iterator temp =lower_bound(m_MustNTList.begin(),m_MustNTList.end(),m_iNTNum);
	if(bIsInclude==false)
	{
		m_MustNTList.insert(temp,m_iNTNum);
	}
	posIte =lower_bound(m_MustNTList.begin(),m_MustNTList.end(),iCardNum);

	int iReduceNum =static_cast<int>( posIte-m_MustNTList.begin());
	iCardNum -= iReduceNum;
	if(bIsInclude==false)
	{
		temp=find(m_MustNTList.begin(),m_MustNTList.end(),m_iNTNum);
		m_MustNTList.erase(temp);
	}

	return iCardNum;
}

//排列扑克
BOOL CUpGradeGameLogic::SortCard(BYTE iCardList[], BYTE bUp[], int iCardCount)
{
	BOOL bSorted=TRUE,bTempUp;
	int iTemp,iLast=iCardCount-1,iStationVol[45];

	//获取位置数值
	for (int i=0;i<iCardCount;i++)
	{
		iStationVol[i]=GetCardBulk(iCardList[i],TRUE);
		iStationVol[i]+=m_iStation[GetCardHuaKind(iCardList[i],FALSE)>>4];
	}

	//排序操作
	do
	{
		bSorted=TRUE;
		for (int i=0;i<iLast;i++)
		{
			if (iStationVol[i]<iStationVol[i+1])
			{	
				//交换位置
				iTemp=iCardList[i];
				iCardList[i]=iCardList[i+1];
				iCardList[i+1]=iTemp;
				iTemp=iStationVol[i];
				iStationVol[i]=iStationVol[i+1];
				iStationVol[i+1]=iTemp;
				if (bUp!=NULL)
				{
					bTempUp=bUp[i];
					bUp[i]=bUp[i+1];
					bUp[i+1]=bTempUp;
				}
				bSorted=FALSE;
			}	
		}
		iLast--;
	} while(!bSorted);

	return TRUE;
}

//获取扑克
BYTE CUpGradeGameLogic::GetCardFromHua(int iHuaKind, int iNum)
{
	if (iHuaKind!=UG_ERROR_HUA) return (iHuaKind+iNum-1);
	return 0;
}

//是否对牌
BOOL CUpGradeGameLogic::IsDouble(BYTE iCardList[], int iCardCount)
{
	if (iCardCount!=2) return FALSE;
	return (iCardList[0]==iCardList[1]);
}

//是否三条
BOOL CUpGradeGameLogic::IsThree(BYTE iCardList[], int iCardCount)
{
	if (iCardCount!=3) return FALSE;
	return ((iCardList[0]==iCardList[1])&&(iCardList[0]==iCardList[2]));
}

//是否拖拉机逻辑
BOOL CUpGradeGameLogic::IsTrackorRule(BYTE iFirstCard, BYTE iNextCard)
{
	int iCardBulk=GetCardBulk(iFirstCard,FALSE);

	if (GetCardBulk(iNextCard,FALSE)!=(iCardBulk-1)) 
		return FALSE;
	return TRUE;
}

//是否拖拉机（两条）
BOOL CUpGradeGameLogic::IsDoubleTrackor(BYTE iCardList[], int iCardCount)
{
	if ((iCardCount<4)||((iCardCount%2)!=0)) return FALSE;

	BYTE iCard=iCardList[0];
	if (!IsDouble(iCardList,2)) return FALSE;
	for (int i=2;i<iCardCount;i+=2)
	{
		if (!IsDouble(&iCardList[i],2)||!IsTrackorRule(iCard,iCardList[i])) return FALSE;
		iCard=iCardList[i];
	}
	return TRUE;
}

//是否拖拉机（三条）
BOOL CUpGradeGameLogic::IsThreeTrackor(BYTE iCardList[], int iCardCount)
{
	if ((iCardCount<6)||((iCardCount%3)!=0)) return FALSE;

	BYTE iCard=iCardList[0];
	if (!IsThree(iCardList,3)) return FALSE;
	for (int i=3;i<iCardCount;i+=3)
	{
		if (!IsThree(&iCardList[i],3)||!IsTrackorRule(iCard,iCardList[i])) return FALSE;
		iCard=iCardList[i];
	}
	return TRUE;
}

//是否甩牌
BOOL CUpGradeGameLogic::IsShowCard(BYTE iCardList[], int iCardCount)
{
	return (GetCardListHua(iCardList,iCardCount)==UG_NT_CARD);
}

//得到牌列花色
BYTE CUpGradeGameLogic::GetCardListHua(BYTE iCardList[], int iCardCount)
{
	int iHuaKind=GetCardHuaKind(iCardList[0],FALSE);
	if (GetCardHuaKind(iCardList[iCardCount-1],FALSE)!=iHuaKind) return UG_ERROR_HUA;
	return iHuaKind;
}

//获取牌型
BYTE CUpGradeGameLogic::GetCardShape(BYTE iCardList[], int iCardCount)
{
	if(iCardCount ==0)
	{
		return UG_ERROR_KIND;
	}
	if (GetCardListHua(iCardList,iCardCount)!=UG_ERROR_HUA)
	{
		if (IsOnlyOne(iCardList,iCardCount)) return UG_ONLY_ONE;
		if (IsDouble(iCardList,iCardCount)) return UG_DOUBLE;
		if (IsThree(iCardList,iCardCount)) return UG_THREE;
		if (IsDoubleTrackor(iCardList,iCardCount)) return UG_DOUBLE_TRACKOR;
		if (IsThreeTrackor(iCardList,iCardCount)) return UG_THREE_TARCKOR;
		if (IsShowCard(iCardList,iCardCount)) return UG_SHOW_CARD;
	}
	return UG_ERROR_KIND;
}

//对比单牌
BOOL CUpGradeGameLogic::CompareOnlyOne(BYTE iFirstCard, BYTE iNextCard)
{
	int iFristHua=GetCardHuaKind(iFirstCard,FALSE);
	int iNextHua=GetCardHuaKind(iNextCard,FALSE);
	if (iFristHua!=iNextHua)
	{
		//不同花色对比
		if (iFristHua==UG_NT_CARD) return TRUE;
		return (iNextHua!=UG_NT_CARD);
	}

	//同花色对比
	return GetCardBulk(iFirstCard,FALSE)>=GetCardBulk(iNextCard,FALSE);
}

BOOL CUpGradeGameLogic::IsHaveScore(BYTE bCard[],BYTE bCardCnt)
{
	for(int i=0;i<bCardCnt;i++)
	{
		if(GetScore(bCard[i]))
		{
			return TRUE;
		}
	}
	return FALSE;
}
BOOL CUpGradeGameLogic::AutoOutCardEx(BYTE iHandCard[], int iHandCardCount, BYTE iBaseCard[], int iBaseCardCount, BYTE iResultCard[], int & iResultCardCount,BYTE iMaxCard[],int iMaxCardCount)
{
	if (iBaseCardCount !=0)	//跟随出牌者
	{
		int iSameHuaCount=0,iTempCount=0;
		BYTE iSameHuaCard[39],iTempCard[54];
		BYTE iBaseHuaKind=GetCardHuaKind(iBaseCard[0],FALSE);
		BYTE iBaseCardKind=GetCardShape(iBaseCard,iBaseCardCount);
		BYTE iMoreCard[PLAYER_CARD_CNT]={0};
		int  iMoreCardCnt=0;

		BYTE iScoreCard[PLAYER_CARD_CNT]={0};
		BYTE  iScoreCardCount=0;

		//提取扑克
		iSameHuaCount=TackOutCardByHua(iHandCard,iHandCardCount,iBaseHuaKind,iSameHuaCard,FALSE);
		if(iSameHuaCount==0)
		{
			FindMoreCard(iHandCard,iHandCardCount,iMaxCard,iMaxCardCount,iMoreCard,iMoreCardCnt);
			if(iMoreCardCnt)
			{
				iResultCardCount=iBaseCardCount;
				memcpy(iResultCard,iMoreCard,iMoreCardCnt);
				return TRUE;
			}
		}

		if (iSameHuaCount<=iBaseCardCount)	//没有了同色扑克
		{
			int iCardPos=iHandCardCount-1;
			iResultCardCount=iSameHuaCount;
			::CopyMemory(iResultCard,iSameHuaCard,sizeof(BYTE)*iSameHuaCount);
			if(iSameHuaCount==iBaseCardCount)
			{
				return TRUE;
			}

			BYTE iTempCard[PLAYER_CARD_CNT]={0};
			int  iTempCardCount=iHandCardCount;
			memcpy(iTempCard,iHandCard,iHandCardCount);

			//清除掉花色
			iTempCardCount-=RemoveCard(iSameHuaCard,iSameHuaCount,iTempCard,iTempCardCount);
			GetScore(iTempCard,iTempCardCount,iScoreCard,iScoreCardCount);
			if(iScoreCardCount)
			{
				//清除掉分牌
				iTempCardCount-=RemoveCard(iScoreCard,iScoreCardCount,iTempCard,iTempCardCount);
				//非分牌够出的话
				if(iSameHuaCount+iTempCardCount>=iBaseCardCount)
				{
					memcpy(&iResultCard[iResultCardCount],&iTempCard[iTempCardCount-(iBaseCardCount-iResultCardCount)],iBaseCardCount-iResultCardCount);
					iResultCardCount=iBaseCardCount;
					goto loop;
				}
				memcpy(&iResultCard[iResultCardCount],iTempCard,iTempCardCount);
				iResultCardCount +=iTempCardCount;
				//分数按值排序
				SortCardsValue(iScoreCard,iScoreCardCount);
				memcpy(&iResultCard[iResultCardCount],&iScoreCard[iScoreCardCount-(iBaseCardCount-iResultCardCount)],iBaseCardCount-iResultCardCount);
				iResultCardCount=iBaseCardCount;
				goto loop;
			}

			while (iResultCardCount<iBaseCardCount)
			{
				if (GetCardHuaKind(iHandCard[iCardPos],FALSE)!=iBaseHuaKind)
					iResultCard[iResultCardCount++]=iHandCard[iCardPos];
				iCardPos--;
			}
			goto loop;
		}
		else	//分析扑克
		{
			//首先查找是否有比最大牌大的牌

			FindMoreCard(iSameHuaCard,iSameHuaCount,iMaxCard,iMaxCardCount,iMoreCard,iMoreCardCnt);
			if(iMoreCardCnt)
			{
				iResultCardCount=iBaseCardCount;
				memcpy(iResultCard,iMoreCard,iMoreCardCnt);
				goto loop;
			}
			iResultCardCount=0;
			int iStationPos=0,iTakeCount=0;
			CardAnalyseStruct AnalyseBase;
			AnalyseCard(iBaseCard,iBaseCardCount,AnalyseBase);

			//三条拖拉机分析
			if (AnalyseBase.iThreeTractorCount>0)
			{
				//提出三条拖拉机
				iTakeCount=0,iStationPos=0;
				iTempCount=TackOutThreeTrackor(iSameHuaCard,iSameHuaCount,iTempCard,TRUE);
				iSameHuaCount-=iTempCount;
				while ((iTakeCount<iTempCount)&&(iTakeCount<iBaseCardCount))
				{
					::CopyMemory(&iResultCard[iResultCardCount],&iTempCard[iStationPos+2],sizeof(BYTE)*iTempCard[iStationPos+1]);
					iResultCardCount+=iTempCard[iStationPos+1];
					iTakeCount+=iTempCard[iStationPos+1];
					iStationPos+=iTempCard[iStationPos+1]+2;
				}
				iResultCardCount=__min(iBaseCardCount,iResultCardCount+iTakeCount);
				if (iResultCardCount==iBaseCardCount) goto loop;
				if (AnalyseBase.iThreeTractorCount>iTempCount)
				{	
					AnalyseBase.iThreeCount+=(AnalyseBase.iThreeTractorCount-iTempCount);
					AnalyseBase.iThreeTractorCount=iTempCount;
				}
			}

			//三条分析
			if (AnalyseBase.iThreeCount>0)
			{
				//提出三条
				iTakeCount=0,iStationPos=0;
				iTempCount=TackOutThree(iSameHuaCard,iSameHuaCount,iTempCard,TRUE);
				iSameHuaCount-=iTempCount;
				::CopyMemory(&iResultCard[iResultCardCount],iTempCard,sizeof(BYTE)*iTempCount);
				iResultCardCount=__min(iBaseCardCount,iResultCardCount+iTempCount);
				if (iResultCardCount==iBaseCardCount) goto loop;
				if (AnalyseBase.iThreeCount>iTempCount)
				{	
					AnalyseBase.iDoubleCount+=(AnalyseBase.iThreeCount-iTempCount)/3*2;
					AnalyseBase.iSignedCount+=(AnalyseBase.iThreeCount-iTempCount)/3;
					AnalyseBase.iThreeTractorCount=iTempCount;
				}
			}

			//两条拖拉机分析
			if (AnalyseBase.iDoubleTractorCount>0)
			{
				//提出两条拖拉机
				iTakeCount=0,iStationPos=0;
				iTempCount=TackOutDoubleTrackor(iSameHuaCard,iSameHuaCount,iTempCard,TRUE);
				iSameHuaCount-=iTempCount;
				//当玩家有几个拖拉机，但是却没有一个是比首先出牌的人大的，就可以送分
				while ((iTakeCount<iTempCount)&&(iTakeCount<iBaseCardCount))
				{
					::CopyMemory(&iResultCard[iResultCardCount],&iTempCard[iStationPos+2],sizeof(BYTE)*iTempCard[iStationPos+1]);
					iResultCardCount+=iTempCard[iStationPos+1];
					iTakeCount+=iTempCard[iStationPos+1];
					iStationPos+=iTempCard[iStationPos+1]+2;
				}
				iResultCardCount=__min(iBaseCardCount,iResultCardCount+iTakeCount);
				if (iResultCardCount==iBaseCardCount) goto loop;
				if (AnalyseBase.iDoubleTractorCount>iTempCount)
				{	
					AnalyseBase.iDoubleCount+=(AnalyseBase.iDoubleTractorCount-iTempCount);
					AnalyseBase.iDoubleTractorCount=iTempCount;
				}
			}

			//对牌分析
			if (AnalyseBase.iDoubleCount>0)
			{
				//提出对牌
				iTakeCount=0,iStationPos=0;
				iTempCount=TackOutDouble(iSameHuaCard,iSameHuaCount,iTempCard,TRUE);
				//对子很多，只提取非分牌或小的分牌
				if(iTempCount>AnalyseBase.iDoubleCount)
				{
					GetScore(iTempCard,iTempCount,iScoreCard,iScoreCardCount);
					if(iScoreCardCount)
					{
						//把分牌移除掉
						iTempCount-=RemoveCard(iScoreCard,iScoreCardCount,iTempCard,iTempCount);
						//分牌重新加入
						memcpy(&iSameHuaCard[iSameHuaCount],iScoreCard,iScoreCardCount);
						iSameHuaCount+=iScoreCardCount;
						SortCard(iSameHuaCard,NULL,iSameHuaCount);
						if(iTempCount<AnalyseBase.iDoubleCount)
						{
							//再次把小分牌清除掉
							iSameHuaCount-=RemoveCard(&iScoreCard[iScoreCardCount-(AnalyseBase.iDoubleCount-iTempCount)],\
								AnalyseBase.iDoubleCount-iTempCount,iSameHuaCard,iSameHuaCount);
							//只取小分牌
							memcpy(&iTempCard[iTempCount],&iScoreCard[iScoreCardCount-(AnalyseBase.iDoubleCount-iTempCount)],\
								AnalyseBase.iDoubleCount-iTempCount);
							iTempCount=AnalyseBase.iDoubleCount;
							SortCard(iTempCard,NULL,iTempCount);
						}
					}
				}
				iSameHuaCount-=iTempCount;
				::CopyMemory(&iResultCard[iResultCardCount],iTempCard,sizeof(BYTE)*iTempCount);
				iResultCardCount=__min(iBaseCardCount,iResultCardCount+iTempCount);
				if (iResultCardCount==iBaseCardCount) goto loop;
				if (AnalyseBase.iDoubleCount>iTempCount)
				{	
					AnalyseBase.iSignedCount+=AnalyseBase.iDoubleCount-iTempCount;
					AnalyseBase.iDoubleCount=iTempCount;
				}
			}
			GetScore(iSameHuaCard,iSameHuaCount,iScoreCard,iScoreCardCount);
			if(!iScoreCardCount)
			{
				//提出单牌
				::CopyMemory(&iResultCard[iResultCardCount],&iSameHuaCard[iSameHuaCount+iResultCardCount-iBaseCardCount],
					sizeof(BYTE)*(iBaseCardCount-iResultCardCount));
				iResultCardCount=iBaseCardCount;
				goto loop;
			}
			iSameHuaCount-=RemoveCard(iScoreCard,iScoreCardCount,iSameHuaCard,iSameHuaCount);
			memcpy(&iResultCard[iResultCardCount],iSameHuaCard,iSameHuaCount);
			iResultCardCount=__min(iBaseCardCount,iResultCardCount+iSameHuaCount);
			if(iResultCardCount==iBaseCardCount) goto loop;//return TRUE;
			//取分牌中的小牌
			memcpy(&iResultCard[iResultCardCount],&iScoreCard[iScoreCardCount-(iBaseCardCount-iResultCardCount)],\
				iBaseCardCount-iResultCardCount);
			iResultCardCount=iBaseCardCount;
			goto loop;
		}
	}
	else //第一个出牌者
	{
		iResultCardCount=1;
		iResultCard[0]=iHandCard[iHandCardCount-1];
		return true;
	}
loop:
	SortCard(iResultCard,NULL,iResultCardCount);
	return TRUE;
}
//自动出牌函数
BOOL CUpGradeGameLogic::AutoOutCard(BYTE iHandCard[], int iHandCardCount, BYTE iBaseCard[], int iBaseCardCount,
									BYTE iResultCard[], int & iResultCardCount)
{
	if (iBaseCardCount !=0)	//跟随出牌者
	{
		int iSameHuaCount=0,iTempCount=0;
		BYTE iSameHuaCard[39],iTempCard[54];
		BYTE iBaseHuaKind=GetCardHuaKind(iBaseCard[0],FALSE);
		BYTE iBaseCardKind=GetCardShape(iBaseCard,iBaseCardCount);

		//提取扑克
		iSameHuaCount=TackOutCardByHua(iHandCard,iHandCardCount,iBaseHuaKind,iSameHuaCard,FALSE);

		if (iSameHuaCount<=iBaseCardCount)	//没有了同色扑克
		{
			int iCardPos=iHandCardCount-1;
			iResultCardCount=iSameHuaCount;
			::CopyMemory(iResultCard,iSameHuaCard,sizeof(BYTE)*iSameHuaCount);
			while (iResultCardCount<iBaseCardCount)
			{
				if (GetCardHuaKind(iHandCard[iCardPos],FALSE)!=iBaseHuaKind)
					iResultCard[iResultCardCount++]=iHandCard[iCardPos];
				iCardPos--;
			}
			return TRUE;
		}
		else	//分析扑克
		{
			iResultCardCount=0;
			int iStationPos=0,iTakeCount=0;
			CardAnalyseStruct AnalyseBase;
			AnalyseCard(iBaseCard,iBaseCardCount,AnalyseBase);

			//三条拖拉机分析
			if (AnalyseBase.iThreeTractorCount>0)
			{
				//提出三条拖拉机
				iTakeCount=0,iStationPos=0;
				iTempCount=TackOutThreeTrackor(iSameHuaCard,iSameHuaCount,iTempCard,TRUE);
				iSameHuaCount-=iTempCount;
				while ((iTakeCount<iTempCount)&&(iTakeCount<iBaseCardCount))
				{
					::CopyMemory(&iResultCard[iResultCardCount],&iTempCard[iStationPos+2],sizeof(BYTE)*iTempCard[iStationPos+1]);
					iResultCardCount+=iTempCard[iStationPos+1];
					iTakeCount+=iTempCard[iStationPos+1];
					iStationPos+=iTempCard[iStationPos+1]+2;
				}
				iResultCardCount=__min(iBaseCardCount,iResultCardCount+iTakeCount);
				if (iResultCardCount==iBaseCardCount) return TRUE;
				if (AnalyseBase.iThreeTractorCount>iTempCount)
				{	
					AnalyseBase.iThreeCount+=(AnalyseBase.iThreeTractorCount-iTempCount);
					AnalyseBase.iThreeTractorCount=iTempCount;
				}
			}

			//三条分析
			if (AnalyseBase.iThreeCount>0)
			{
				//提出三条
				iTakeCount=0,iStationPos=0;
				iTempCount=TackOutThree(iSameHuaCard,iSameHuaCount,iTempCard,TRUE);
				iSameHuaCount-=iTempCount;
				::CopyMemory(&iResultCard[iResultCardCount],iTempCard,sizeof(BYTE)*iTempCount);
				iResultCardCount=__min(iBaseCardCount,iResultCardCount+iTempCount);
				if (iResultCardCount==iBaseCardCount) return TRUE;
				if (AnalyseBase.iThreeCount>iTempCount)
				{	
					AnalyseBase.iDoubleCount+=(AnalyseBase.iThreeCount-iTempCount)/3*2;
					AnalyseBase.iSignedCount+=(AnalyseBase.iThreeCount-iTempCount)/3;
					AnalyseBase.iThreeTractorCount=iTempCount;
				}
			}

			//两条拖拉机分析
			if (AnalyseBase.iDoubleTractorCount>0)
			{
				//提出两条拖拉机
				iTakeCount=0,iStationPos=0;
				iTempCount=TackOutDoubleTrackor(iSameHuaCard,iSameHuaCount,iTempCard,TRUE);
				iSameHuaCount-=iTempCount;
				while ((iTakeCount<iTempCount)&&(iTakeCount<iBaseCardCount))
				{
					::CopyMemory(&iResultCard[iResultCardCount],&iTempCard[iStationPos+2],sizeof(BYTE)*iTempCard[iStationPos+1]);
					iResultCardCount+=iTempCard[iStationPos+1];
					iTakeCount+=iTempCard[iStationPos+1];
					iStationPos+=iTempCard[iStationPos+1]+2;
				}
				iResultCardCount=__min(iBaseCardCount,iResultCardCount+iTakeCount);
				if (iResultCardCount==iBaseCardCount) return TRUE;
				if (AnalyseBase.iDoubleTractorCount>iTempCount)
				{	
					AnalyseBase.iDoubleCount+=(AnalyseBase.iDoubleTractorCount-iTempCount);
					AnalyseBase.iDoubleTractorCount=iTempCount;
				}
			}

			//对牌分析
			if (AnalyseBase.iDoubleCount>0)
			{
				//提出对牌
				iTakeCount=0,iStationPos=0;
				iTempCount=TackOutDouble(iSameHuaCard,iSameHuaCount,iTempCard,TRUE);
				iSameHuaCount-=iTempCount;
				::CopyMemory(&iResultCard[iResultCardCount],iTempCard,sizeof(BYTE)*iTempCount);
				iResultCardCount=__min(iBaseCardCount,iResultCardCount+iTempCount);
				if (iResultCardCount==iBaseCardCount) return TRUE;
				if (AnalyseBase.iDoubleCount>iTempCount)
				{	
					AnalyseBase.iSignedCount+=AnalyseBase.iDoubleCount-iTempCount;
					AnalyseBase.iDoubleCount=iTempCount;
				}
			}
			//提出单牌
			::CopyMemory(&iResultCard[iResultCardCount],&iSameHuaCard[iSameHuaCount+iResultCardCount-iBaseCardCount],
				sizeof(BYTE)*(iBaseCardCount-iResultCardCount));
			iResultCardCount=iBaseCardCount;

			return TRUE;
		}
	}
	else //第一个出牌者
	{
		iResultCardCount=1;
		iResultCard[0]=iHandCard[iHandCardCount-1];
	}

	return TRUE;
}

//是否可以甩牌
BOOL CUpGradeGameLogic::CanShowCard(BYTE iOutCard[], int iOutCardCount, int iOutStation, 
									BYTE * iCardArray[], int iCardArrayCount, 
									BYTE iResultCard[], int & iResultCardCount)
{
	CardAnalyseStruct AnalyseOut;
	BYTE iTempCard[54],iSameHuaCard[39];
	int iTempCount=0,iOtherSameCount=0;

	AnalyseCard(iOutCard,iOutCardCount,AnalyseOut);
	for (int i=0;i<4;i++)
	{
		if (iOutStation==i) continue;

		//提取扑克
		iOtherSameCount=TackOutCardByHua(iCardArray[i],iCardArrayCount,AnalyseOut.iCardHuaKind,iSameHuaCard,FALSE);

		//分析三条拖拉机
		if (AnalyseOut.iThreeTractorCount!=0)
		{
			int iOutStation=0,iOtherStation=0,iCompOutCount=0,iCompOtherCount=0;
			iTempCount=TackOutThreeTrackor(iSameHuaCard,iOtherSameCount,iTempCard,FALSE);
			if (iTempCount!=0)
			{
				do
				{
					iOtherStation=0;
					iCompOtherCount=0;
					while (iCompOtherCount<iTempCount)
					{
						if ((AnalyseOut.iThreeTractorArray[iOutStation+1]<=iTempCard[iOtherStation+1])
							&&(CompareOnlyOne(AnalyseOut.iThreeTractorArray[iOutStation+2],iTempCard[0])==FALSE))
						{
							//甩牌失败
							iResultCardCount=AnalyseOut.iThreeTractorArray[iOutStation+1];
							::CopyMemory(iResultCard,&AnalyseOut.iThreeTractorArray[iOutStation+2],sizeof(BYTE)*iResultCardCount);
							return FALSE;
						}
						iCompOtherCount+=iTempCard[iOtherStation+1];
						iOtherStation+=iTempCard[iOtherStation+1]+2;
					}
					iCompOutCount+=AnalyseOut.iThreeTractorArray[iOutStation+1];
					iOutStation+=AnalyseOut.iThreeTractorArray[iOutStation+1]+2;
				} while (iCompOutCount<AnalyseOut.iThreeTractorCount);
			}
		}

		//分析两条拖拉机
		if (AnalyseOut.iDoubleTractorCount!=0)
		{
			int iOutStation=0,iOtherStation=0,iCompOutCount=0,iCompOtherCount=0;
			iTempCount=TackOutDoubleTrackor(iSameHuaCard,iOtherSameCount,iTempCard,FALSE);
			if (iTempCount!=0)
			{
				do
				{
					iOtherStation=0;
					iCompOtherCount=0;
					while (iCompOtherCount<iTempCount)
					{
						if ((AnalyseOut.iDoubleTractorArray[iOutStation+2]<=iTempCard[iOtherStation+2])
							&&(CompareOnlyOne(AnalyseOut.iDoubleTractorArray[iOutStation+2],iTempCard[2])==FALSE))
						{
							//甩牌失败
							iResultCardCount=AnalyseOut.iDoubleTractorArray[iOutStation+1];
							::CopyMemory(iResultCard,&AnalyseOut.iDoubleTractorArray[iOutStation+2],sizeof(BYTE)*iResultCardCount);
							return FALSE;
						}
						iCompOtherCount+=iTempCard[iOtherStation+1];
						iOtherStation+=iTempCard[iOtherStation+1]+2;
					}
					iCompOutCount+=AnalyseOut.iDoubleTractorArray[iOutStation+1];
					iOutStation+=AnalyseOut.iDoubleTractorArray[iOutStation+1]+2;
				} while (iCompOutCount<AnalyseOut.iDoubleTractorCount);
			}
		}

		//分析三条
		if (AnalyseOut.iThreeCount!=0)
		{
			iTempCount=TackOutThree(iSameHuaCard,iOtherSameCount,iTempCard,FALSE);
			if ((iTempCount!=0)&&(CompareOnlyOne(AnalyseOut.iThreeArray[AnalyseOut.iThreeCount-3],iTempCard[0])==FALSE))
			{
				//甩牌失败
				iResultCardCount=3;
				::CopyMemory(iResultCard,&AnalyseOut.iThreeArray[AnalyseOut.iThreeCount-3],sizeof(BYTE)*iResultCardCount);
				return FALSE;
			}
		}

		//分析对牌
		if (AnalyseOut.iDoubleCount!=0)
		{
			iTempCount=TackOutDouble(iSameHuaCard,iOtherSameCount,iTempCard,FALSE);
			if ((iTempCount!=0)&&(CompareOnlyOne(AnalyseOut.iDoubleArray[AnalyseOut.iDoubleCount-2],iTempCard[0])==FALSE))
			{
				//甩牌失败
				iResultCardCount=2;
				::CopyMemory(iResultCard,&AnalyseOut.iDoubleArray[AnalyseOut.iDoubleCount-2],sizeof(BYTE)*iResultCardCount);
				return FALSE;
			}
		}

		//分析单牌
		if ((AnalyseOut.iSignedCount!=0)&&(iOtherSameCount>0)&&
			(CompareOnlyOne(AnalyseOut.iLessSignedCard,iSameHuaCard[0])==FALSE))
		{
			//甩牌失败
			iResultCardCount=1;
			iResultCard[0]=AnalyseOut.iLessSignedCard;
			return FALSE;
		}
	}

	//甩牌成功
	iResultCardCount=iOutCardCount;
	::CopyMemory(iResultCard,iOutCard,sizeof(BYTE)*iResultCardCount);

	return TRUE;
}

//对比函数
int CUpGradeGameLogic::CompareCard(BYTE * iCardListArray[], int iCardCount)
{
	//排列扑克
	for (int i=0;i<4;i++) SortCard(iCardListArray[i],NULL,iCardCount);

	//对比处理
	int iWinner=0;
	int iCardShape=GetCardShape(iCardListArray[0],iCardCount);
	switch (iCardShape)
	{
	case UG_ONLY_ONE:
	case UG_DOUBLE:
	case UG_THREE: 
	case UG_DOUBLE_TRACKOR:
	case UG_THREE_TARCKOR:
		{
			for (int i=1;i<4;i++)
			{
				if ((GetCardShape(iCardListArray[i],iCardCount)==iCardShape)&&
					(CompareOnlyOne(iCardListArray[iWinner][0],iCardListArray[i][0])==FALSE))
					iWinner=i;
			}
			return iWinner;
		}
	case UG_SHOW_CARD: return CompareShowCard(iCardListArray,iCardCount);
	}

	return -1;
}

//对比甩牌
int CUpGradeGameLogic::CompareShowCard(BYTE * iCardListArray[], int iCardCount)
{
	CardAnalyseStruct AnalyseResul[4];

	//分析扑克
	AnalyseCard(iCardListArray[0],iCardCount,AnalyseResul[0]);
	//如果甩的是主肯定是首家大
	//	if (AnalyseResul[0].iCardHuaKind==UG_NT_CARD) return 0;
	//其他玩家出的是错误牌
	BYTE bErrorCardCnt=0;
	for(int i=1;i<4;i++)
	{
		if(GetCardShape(iCardListArray[i],iCardCount) ==UG_ERROR_KIND)
		{
			bErrorCardCnt++;
		}
	}
	if(bErrorCardCnt ==3)
	{
		return 0;
	}
	//甩牌比较逻辑
	//需要记录每个牌型的最大牌，和这个牌型的个数（拖拉机个数不一样的时候也算不同的牌型）
	//首先比较相同牌型的最大牌，然后看这个牌型的个数是否等于（或大于）上个玩家该牌型的个数

	//最多只可能出现7种牌型
	CmpShowCardStruct cmpCardBuf[7];

	memset(cmpCardBuf,0,sizeof(cmpCardBuf));

	//标记是否已经被排除
	BYTE pFlag[50] ={0};
	//当前记录位置
	int iCurRecordPos=0;
	for(int cnt=iCardCount;cnt>0;cnt--)
	{
		bool bRecord =false;
		for(int pos=0;pos<iCardCount-cnt+1;pos++)
		{
			if(!pFlag[pos])
			{
				int temp=GetCardShape(&iCardListArray[0][pos],cnt);
				if(temp !=UG_SHOW_CARD && temp !=UG_ERROR_KIND)
				{
					bRecord =true;
					//同类型的牌只取最大的那个,并且记录其个数
					//记录最大牌
					if(cmpCardBuf[iCurRecordPos].bCardKindCnt==0)
					{
						memcpy(cmpCardBuf[iCurRecordPos].bCards,&iCardListArray[0][pos],cnt);
					}
					cmpCardBuf[iCurRecordPos].bCardKindCnt++;
					cmpCardBuf[iCurRecordPos].bCardKind=temp;
					cmpCardBuf[iCurRecordPos].bCardCnt=cnt;
					memset(&pFlag[pos],TRUE,cnt);
					//单张只记录一次
					if(cnt ==1)
					{
						break;
					}
				}
			}
		}
		if(bRecord)
		{
			iCurRecordPos++;
		}
	}

	int iWinner =0;
	//记录最大牌（如果牌型个数相同(或大于)的话，则替换原来的最大牌）
	BYTE bTempCard[7][30]={0};
	BYTE bTempCardKindCnt=0;
	BYTE bTempCardCnt=0;

	int iCurCmpPos=0;
	int size=0;
	for(int i=1;i<4;i++)
	{
		if(GetCardShape(iCardListArray[i],iCardCount) ==UG_ERROR_KIND) continue;

		memset(pFlag,FALSE,iCardCount);
		//是否全部类型都大于
		bool bAllBig=true;	
		for(iCurCmpPos=0;iCurCmpPos<iCurRecordPos;iCurCmpPos++)
		{
			size=cmpCardBuf[iCurCmpPos].bCardCnt;
			bTempCardCnt=0;
			bTempCardKindCnt=0;
			for(int pos=0;pos<iCardCount-size+1;pos++)
			{
				if(!pFlag[pos])
				{
					int iTempShape=GetCardShape(&iCardListArray[i][pos],size);
					if(iTempShape == cmpCardBuf[iCurCmpPos].bCardKind)	
					{
						//标记
						memset(&pFlag[pos],TRUE,size);
						if(bTempCardCnt ==0 &&iCurCmpPos==0)
						{
							//只记录最大的
							if(CompareOnlyOne(cmpCardBuf[iCurCmpPos].bCards[0],iCardListArray[i][pos])==FALSE)
							{
								memcpy(bTempCard[iCurCmpPos],&iCardListArray[i][pos],size);
								bTempCardCnt =size;
							}
							//没有对应类型的最大的大，则不用往后比较了,继续下一个玩家的比较
							else
							{
								bAllBig=false;
								break;
							}
						}
						bTempCardKindCnt++;
						//如果牌型个数相同
						if(bTempCardKindCnt == cmpCardBuf[iCurCmpPos].bCardKindCnt)
						{
							break;
						}
					}
				}
			}
			//搜索完之后仍然不等
			if(bTempCardKindCnt !=cmpCardBuf[iCurCmpPos].bCardKindCnt)
			{
				bAllBig=false;
			}
			if(!bAllBig)
			{
				break;
			}
		}
		//替换掉最大的
		if(bAllBig)
		{
			iWinner=i;
			for(int i=0;i<1;i++)
			{
				memcpy(cmpCardBuf[i].bCards,bTempCard[i],cmpCardBuf[i].bCardCnt);
			}
		}
	}

	return iWinner;
}

//分析函数
BOOL CUpGradeGameLogic::AnalyseCard(BYTE iCardList[], int iCardCount, CardAnalyseStruct & Analyse)
{
	//初始化数据
	::memset(&Analyse,0,sizeof(Analyse));

	//判断是否同花色
	Analyse.iCardHuaKind=GetCardListHua(iCardList,iCardCount);
	if (Analyse.iCardHuaKind==UG_ERROR_HUA) return FALSE;

	//分析临时数据
	BYTE iTempCard[39];
	::CopyMemory(iTempCard,iCardList,sizeof(BYTE)*iCardCount);

	//去除一张扑克的分析过程
	if (iCardCount==1) 
	{
		Analyse.iSignedCount=1;
		Analyse.iLessSignedCard=iCardList[0];
		return TRUE;
	}

	//提取三条
	Analyse.iThreeCount=TackOutThree(iTempCard,iCardCount,Analyse.iThreeArray,TRUE);
	iCardCount-=Analyse.iThreeCount;

	//提取三条拖拉机
	Analyse.iThreeTractorCount=TackOutThreeTrackor(Analyse.iThreeArray,Analyse.iThreeCount,
		Analyse.iThreeTractorArray,TRUE);
	Analyse.iThreeCount-=Analyse.iThreeTractorCount;

	//提取对牌
	Analyse.iDoubleCount=TackOutDouble(iTempCard,iCardCount,Analyse.iDoubleArray,TRUE);
	iCardCount-=Analyse.iDoubleCount;

	//提取两条拖拉机
	Analyse.iDoubleTractorCount=TackOutDoubleTrackor(Analyse.iDoubleArray,Analyse.iDoubleCount,
		Analyse.iDoubleTractorArray,TRUE);
	Analyse.iDoubleCount-=Analyse.iDoubleTractorCount;

	//提取单牌
	if (iCardCount>0)
	{
		Analyse.iSignedCount=iCardCount;
		memcpy(Analyse.iSingleCardArray,iTempCard,iCardCount);
		Analyse.iBigSignedCard=iTempCard[0];
		Analyse.iLessSignedCard=iTempCard[iCardCount-1];
	}

	return TRUE;
}

//变化扑克格式
BOOL CUpGradeGameLogic::FixCardType(BYTE iCardList[], int iCardCount, CardAnalyseStruct & BaseAnalyse, CardAnalyseStruct & OutAnalyse)
{
	if (GetCardListHua(iCardList,iCardCount)==UG_ERROR_HUA) return FALSE;

	BYTE iSoureCard[39],iTempCard[39];
	int iSoureCardCount=iCardCount,iTempCount=0;
	::CopyMemory(iSoureCard,iCardList,sizeof(BYTE)*iCardCount);
	::memset(&OutAnalyse,0,sizeof(OutAnalyse));

	//调整三条拖拉机
	if (BaseAnalyse.iThreeTractorCount!=0)
	{
		iTempCount=TackOutThreeTrackor(iSoureCard,iSoureCardCount,iTempCard,TRUE);
		iSoureCardCount-=iTempCount;
		if (BaseAnalyse.iThreeTractorCount>iTempCount) return FALSE;
		int iOutPos=0,iTempPos=0,iNeedLength=0,iCopyLength=0;
		do
		{
			iNeedLength=BaseAnalyse.iThreeTractorCount-OutAnalyse.iThreeTractorCount;
			if (iNeedLength<6) return FALSE;
			iCopyLength=__min(iTempCard[iTempPos+1],iNeedLength);
			::CopyMemory(&OutAnalyse.iThreeTractorArray[iOutPos+2],&iTempCard[iTempPos+2],sizeof(BYTE)*iCopyLength);
			OutAnalyse.iThreeTractorArray[iOutPos]=0;
			OutAnalyse.iThreeTractorArray[iOutPos+1]=iCopyLength;
			OutAnalyse.iThreeTractorCount+=iCopyLength;
			if (iCopyLength==iTempCard[iTempPos+1])
			{
				iTempCount-=iTempCard[iTempPos+1];
				iTempPos+=iTempCard[iTempPos+1]+2;
				iOutPos+=OutAnalyse.iThreeTractorArray[iOutPos+1]+2;
			}
			else
			{
				::CopyMemory(&iSoureCard[iSoureCardCount],&iTempCard[iTempPos+2+iCopyLength],
					sizeof(BYTE)*(iTempCard[iTempPos+1]-iCopyLength));
				iTempCount-=iTempCard[iTempPos+1];
				iSoureCardCount+=(iTempCard[iTempPos+1]-iCopyLength);
				iTempPos+=iTempCard[iTempPos+1]+2;
				while ((iSoureCardCount+OutAnalyse.iThreeTractorCount)<iCardCount)
				{
					::CopyMemory(&iSoureCard[iSoureCardCount],&iTempCard[iTempPos+2],sizeof(BYTE)*iTempCard[iTempPos+1]);
					iTempCount-=iTempCard[iTempPos+1];
					iSoureCardCount+=iTempCard[iTempPos+1];
					iTempPos+=iTempCard[iTempPos+1]+2;
				}
				SortCard(iSoureCard,NULL,iSoureCardCount);
			}
		} while (BaseAnalyse.iThreeTractorCount>=OutAnalyse.iThreeTractorCount);
	}

	//调整三条
	if (BaseAnalyse.iThreeCount!=0)
	{
		iTempCount=TackOutThree(iSoureCard,iSoureCardCount,iTempCard,FALSE);
		if (BaseAnalyse.iThreeCount>iTempCount) return FALSE;
		OutAnalyse.iThreeCount=BaseAnalyse.iThreeCount;
		::CopyMemory(OutAnalyse.iThreeArray,iTempCard,sizeof(BYTE)*BaseAnalyse.iThreeCount);
		iSoureCardCount-=RemoveCard(iTempCard,BaseAnalyse.iThreeCount,iSoureCard,iSoureCardCount);
	}

	//调整两条拖拉机
	if (BaseAnalyse.iDoubleTractorCount!=0)
	{
		iTempCount=TackOutDoubleTrackor(iSoureCard,iSoureCardCount,iTempCard,TRUE);
		iSoureCardCount-=iTempCount;
		if (BaseAnalyse.iDoubleTractorCount>iTempCount) return FALSE;
		int iOutPos=0,iTempPos=0,iNeedLength=0,iCopyLength=0;
		do
		{
			iNeedLength=BaseAnalyse.iDoubleTractorCount-OutAnalyse.iDoubleTractorCount;
			if (iNeedLength<4) return FALSE;
			iCopyLength=__min(iTempCard[iTempPos+1],iNeedLength);
			::CopyMemory(&OutAnalyse.iDoubleTractorArray[iOutPos+2],&iTempCard[iTempPos+2],sizeof(BYTE)*iCopyLength);
			OutAnalyse.iDoubleTractorArray[iOutPos]=0;
			OutAnalyse.iDoubleTractorArray[iOutPos+1]=iCopyLength;
			OutAnalyse.iDoubleTractorCount+=iCopyLength;
			if (iCopyLength==iTempCard[iTempPos+1])
			{
				iTempCount-=iTempCard[iTempPos+1];
				iTempPos+=iTempCard[iTempPos+1]+2;
				iOutPos+=OutAnalyse.iDoubleTractorArray[iOutPos+1]+2;
			}
			else
			{
				::CopyMemory(&iSoureCard[iSoureCardCount],&iTempCard[iTempPos+2+iCopyLength],
					sizeof(BYTE)*(iTempCard[iTempPos+1]-iCopyLength));
				iTempCount-=iTempCard[iTempPos+1];
				iSoureCardCount+=(iTempCard[iTempPos+1]-iCopyLength);
				iTempPos+=iTempCard[iTempPos+1]+2;
				while ((iSoureCardCount+OutAnalyse.iDoubleTractorCount)<iCardCount)
				{
					::CopyMemory(&iSoureCard[iSoureCardCount],&iTempCard[iTempPos+2],sizeof(BYTE)*iTempCard[iTempPos+1]);
					iTempCount-=iTempCard[iTempPos+1];
					iSoureCardCount+=iTempCard[iTempPos+1];
					iTempPos+=iTempCard[iTempPos+1]+2;
				}
				SortCard(iSoureCard,NULL,iSoureCardCount);
			}
		} while (BaseAnalyse.iDoubleTractorCount>=OutAnalyse.iDoubleTractorCount);
	}

	//调整对牌
	if (BaseAnalyse.iDoubleCount!=0)
	{
		iTempCount=TackOutDouble(iSoureCard,iSoureCardCount,iTempCard,FALSE);
		if (BaseAnalyse.iDoubleCount>iTempCount) return FALSE;
		OutAnalyse.iDoubleCount=BaseAnalyse.iDoubleCount;
		::CopyMemory(OutAnalyse.iDoubleArray,iTempCard,sizeof(BYTE)*BaseAnalyse.iDoubleCount);
		iSoureCardCount-=RemoveCard(iTempCard,BaseAnalyse.iDoubleCount,iSoureCard,iSoureCardCount);
	}

	//调整单牌
	if (iSoureCardCount>0)
	{
		OutAnalyse.iSignedCount=iSoureCardCount;
		OutAnalyse.iBigSignedCard=iSoureCard[0];
		OutAnalyse.iLessSignedCard=iSoureCard[iSoureCardCount-1];
	}

	return TRUE;
}

//提取对牌函数
int CUpGradeGameLogic::TackOutDouble(BYTE iCardList[], int iCardCount, BYTE iDoubleBuffer[], BOOL bRemoveCard)
{
	int iCount=0;
	for (int i=0;i<iCardCount-1;i++)
	{
		if (iCardList[i]==iCardList[i+1])
		{
			iDoubleBuffer[iCount+1]=iDoubleBuffer[iCount]=iCardList[i];
			iCount+=2;
			if (bRemoveCard) iCardList[i]=iCardList[i+1]=0;
			i++;
		}
	}
	if (iCount && bRemoveCard) RemoveNummCard(iCardList,iCardCount);

	return iCount;
}

//提取两条拖拉机函数
int CUpGradeGameLogic::TackOutDoubleTrackor(BYTE iCardList[], int iCardCount, BYTE iDoubleBuffer[], BOOL bRemoveCard)
{
	BOOL bEndFind=FALSE;
	int iCount=0,iRecordPos=0,iDataPos=0,iTrackorLen=0;

	for (int i=0;i<iCardCount-3;i++)
	{
		if (IsDouble(&iCardList[i],2))
		{
			iDataPos=i;
			iTrackorLen=2;
			bEndFind=FALSE;
			do
			{
				if ((IsDouble(&iCardList[i+2],2))&&(IsTrackorRule(iCardList[i],iCardList[i+2])))
				{
					//是拖拉机
					i+=2;
					iTrackorLen+=2;
				}
				else bEndFind=TRUE;
			} while ((bEndFind==FALSE)&&(i<iCardCount-3));

			//记录数据
			if (iTrackorLen>=4)
			{
				iCount+=iTrackorLen;
				iDoubleBuffer[iRecordPos]=0;
				iDoubleBuffer[iRecordPos+1]=iTrackorLen;
				for (int j=0;j<iTrackorLen;j++)
				{
					iDoubleBuffer[iRecordPos+j+2]=iCardList[iDataPos+j];
					if (bRemoveCard) iCardList[iDataPos+j]=0;
				}
				iRecordPos+=(iTrackorLen+2);
			}
		}
	}
	if (iCount && bRemoveCard) RemoveNummCard(iCardList,iCardCount);

	return iCount;
}

//提取三条函数
int CUpGradeGameLogic::TackOutThree(BYTE iCardList[], int iCardCount, BYTE iThreeBuffer[], BOOL bRemoveCard)
{
	int iCount=0;
	for (int i=0;i<iCardCount-2;i++)
	{
		if ((iCardList[i]==iCardList[i+1])&&(iCardList[i]==iCardList[i+2]))
		{
			iThreeBuffer[iCount]=iThreeBuffer[iCount+1]=iThreeBuffer[iCount+2]=iCardList[i];
			iCount+=3;
			if (bRemoveCard) iCardList[i]=iCardList[i+1]=iCardList[i+2]=0;
			i+=2;
		}
	}
	if (iCount && bRemoveCard) RemoveNummCard(iCardList,iCardCount);

	return iCount;
}

//提取三条拖拉机函数
int CUpGradeGameLogic::TackOutThreeTrackor(BYTE iCardList[], int iCardCount, BYTE iThreeBuffer[], BOOL bRemoveCard)
{
	BOOL bEndFind=FALSE;
	int iCount=0,iRecordPos=0,iDataPos=0,iTrackorLen=0;

	for (int i=0;i<iCardCount-5;i++)
	{
		if (IsThree(&iCardList[i],3))
		{
			iDataPos=i;
			iTrackorLen=3;
			bEndFind=FALSE;
			do
			{
				if ((IsThree(&iCardList[i+3],3))&&(IsTrackorRule(iCardList[i],iCardList[i+3])))
				{
					//是拖拉机
					i+=3;
					iTrackorLen+=3;
				}
				else bEndFind=TRUE;
			} while ((bEndFind==FALSE)&&(i<iCardCount-5));

			//记录数据
			if (iTrackorLen>=6)
			{
				iCount+=iTrackorLen;
				iThreeBuffer[iRecordPos]=0;
				iThreeBuffer[iRecordPos+1]=iTrackorLen;
				for (int j=0;j<iTrackorLen;j++)
				{
					iThreeBuffer[iRecordPos+j+2]=iCardList[iDataPos+j];
					if (bRemoveCard) iCardList[iDataPos+j]=0;
				}
				iRecordPos+=(iTrackorLen+2);
			}
		}
	}
	if (iCount && bRemoveCard) RemoveNummCard(iCardList,iCardCount);

	return iCount;
}

//提取同一花色扑克
int CUpGradeGameLogic::TackOutCardByHua(BYTE iCardList[], int iCardCount, BYTE iHuaKind, BYTE iCardBuffer[], BOOL bRemoveCard)
{
	for (int iBeginPos=0;iBeginPos<iCardCount;iBeginPos++)
	{
		if (GetCardHuaKind(iCardList[iBeginPos],FALSE)==iHuaKind) break;
	}
	if (iBeginPos==iCardCount) return 0;
	for (int iEndPos=iCardCount-1;iEndPos>=0;iEndPos--)
	{
		if (GetCardHuaKind(iCardList[iEndPos],FALSE)==iHuaKind) break;
	}
	::CopyMemory(iCardBuffer,&iCardList[iBeginPos],sizeof(BYTE)*(iEndPos-iBeginPos+1));
	if (bRemoveCard) RemoveNummCard(iCardList,iCardCount);
	return (iEndPos-iBeginPos+1);
}

//清除 0 位扑克
int CUpGradeGameLogic::RemoveNummCard(BYTE iCardList[], int iCardCount)
{
	int iRemoveCount=0;
	for (int i=0;i<iCardCount;i++)
	{
		if (iCardList[i]!=0) iCardList[i-iRemoveCount]=iCardList[i];
		else iRemoveCount++;
	}
	return iRemoveCount;
}

//混乱扑克
BYTE CUpGradeGameLogic::RandCard(BYTE iCard[], int iCardCount)
{
	static const BYTE m_CardArray[46]={
		0x01, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,		//方块 2 - A
			0x11, 0x14 , 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,		//梅花 2 - A
			0x21, 0x24 , 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D,		//红桃 2 - A
			0x31, 0x34 , 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D,		//黑桃 2 - A
			0x4E, 0x4F};																		//小鬼，大鬼

		BYTE iSend=0,iStation=0,iCardList[162];
		srand((unsigned)time(NULL));

		for (int i=0;i<iCardCount;i+=46)
			::CopyMemory(&iCardList[i],m_CardArray,sizeof(m_CardArray));
		do
		{
			iStation=rand()%(iCardCount-iSend);
			iCard[iSend]=iCardList[iStation];
			iSend++;
			iCardList[iStation]=iCardList[iCardCount-iSend];
		} while (iSend<iCardCount);

		return iCardCount;
}

//删除扑克
int CUpGradeGameLogic::RemoveCard(BYTE iRemoveCard[], int iRemoveCount, BYTE iCardList[], int iCardCount)
{
	//检验数据
	if ((iRemoveCount>iCardCount)) return 0;

	//把要删除的牌置零
	int iDeleteCount=0;
	for (int i=0;i<iRemoveCount;i++)
	{
		for (int j=0;j<iCardCount;j++)
		{
			if (iRemoveCard[i]==iCardList[j])
			{
				iDeleteCount++;
				iCardList[j]=0;
				break;
			}
		}
	}
	RemoveNummCard(iCardList,iCardCount);
	if (iDeleteCount!=iRemoveCount) return 0;

	return iDeleteCount;
}

//是否可以出牌
BOOL CUpGradeGameLogic::CanOutCard(BYTE iOutCard[], int iOutCount, BYTE iBaseCard[], 
								   int iBaseCount, BYTE iHandCard[], int iHandCount)
{
	//首先出牌只需要判断是否出牌合理就可以了
	if(iBaseCount ==0)
	{
		return GetCardShape(iOutCard,iOutCount)!=UG_ERROR_KIND;
	}
	//判断数目是否相同
	if (iOutCount!=iBaseCount) return FALSE;

	//得取扑克的基本信息
	int iBaseHua=GetCardListHua(iBaseCard,iBaseCount);
	int iOutHua=GetCardListHua(iOutCard,iOutCount);
	BYTE iBaseKind=GetCardShape(iBaseCard,iBaseCount);
	BYTE iOutKind=GetCardShape(iOutCard,iOutCount);
	BYTE iSameHuaCardBuf[39],iTempCard[54];

	//判断是否与这轮的出牌参数敏合
	if ((iBaseKind==iOutKind)&&(iBaseHua==iOutHua)&&(iBaseKind!=UG_SHOW_CARD))	return TRUE;

	int iSameHuaCount=TackOutCardByHua(iHandCard,iHandCount,iBaseHua,iSameHuaCardBuf,FALSE);

	if (iBaseHua!=iOutHua)	//花色不相同
	{
		//花色不相同,判断是否没了这种花色的扑克
		return (TackOutCardByHua(iOutCard,iOutCount,iBaseHua,iTempCard,FALSE)==iSameHuaCount);
	}
	if ((iOutKind!=iBaseKind)&&(iBaseKind!=UG_SHOW_CARD))	//类型不相同
	{
		//类型不相同，判断是否游戏者没有此类型
		switch (iBaseKind)
		{
			//单牌类型的已经在花色判断中过滤掉了
		case UG_DOUBLE:
			{
				//判断是否没有了对牌
				return TackOutDouble(iSameHuaCardBuf,iSameHuaCount,iTempCard,FALSE)==0;
			}
		case UG_THREE:
			{
				//判断是否没有了三条
				if (TackOutThree(iSameHuaCardBuf,iSameHuaCount,iTempCard,FALSE)!=0) return FALSE;

				//判断是否已经尽力出对牌
				BYTE iMeOutLength=TackOutDouble(iOutCard,iOutCount,iTempCard,FALSE);
				BYTE iMeHandLength=TackOutDouble(iSameHuaCardBuf,iSameHuaCount,iTempCard,FALSE);
				if ((iMeHandLength>iMeOutLength)&&(iMeOutLength<(iBaseCount-1))) return FALSE;

				return TRUE;
			}
		case UG_DOUBLE_TRACKOR:
			{
				//判断游戏者是否尽力出拖拉机
				BYTE iMeOutLength=MaxDoubleTractorLong(iOutCard,iOutCount,NULL);
				BYTE iMeHandLength=MaxDoubleTractorLong(iSameHuaCardBuf,iSameHuaCount,NULL);
				if (iMeHandLength>iMeOutLength) return FALSE;
				//得到玩家出对应该尽力出的长度
				iBaseCount-=iMeOutLength;
				//拖拉机的长度
				BYTE  bTrackorLen = iMeOutLength;
				iMeOutLength = TackOutDouble(iOutCard,iOutCount,iTempCard,FALSE);
				iMeHandLength =TackOutDouble(iSameHuaCardBuf,iSameHuaCount,iTempCard,FALSE);
				//应该把拖拉机上的对去掉
				iMeOutLength -= bTrackorLen;
				iMeHandLength -= bTrackorLen;
				//判断是否已经尽力出对牌(手上有对，但是却并没有)
				if ((iMeOutLength<(iBaseCount))&&(iMeHandLength>iMeOutLength)) return FALSE;
				return TRUE;
			}
		case UG_THREE_TARCKOR:
			{
				//判断游戏者是否尽力出拖拉机
				BYTE iMeOutLength=MaxThreeTractorLong(iOutCard,iOutCount,NULL);
				BYTE iMeHandLength=MaxThreeTractorLong(iSameHuaCardBuf,iSameHuaCount,NULL);
				if (iMeHandLength>iMeOutLength) return FALSE;
				iBaseCount-=iMeOutLength;

				//判断是否已经尽力出三条
				iMeOutLength=TackOutThree(iOutCard,iOutCount,iTempCard,FALSE);
				iMeHandLength=TackOutThree(iSameHuaCardBuf,iSameHuaCount,iTempCard,FALSE);
				if ((iMeHandLength>iMeOutLength)&&(iMeOutLength<iBaseCount)) return FALSE;
				iBaseCount-=iMeOutLength;

				//判断游戏者是否尽力出拖拉机
				iMeOutLength=MaxDoubleTractorLong(iOutCard,iOutCount,NULL);
				iMeHandLength=MaxDoubleTractorLong(iSameHuaCardBuf,iSameHuaCount,NULL);
				if ((iMeHandLength>iMeOutLength)&&(iMeOutLength<(iBaseCount-1))) return FALSE;
				iBaseCount-=iMeOutLength;

				//判断是否已经尽力出对牌
				iMeOutLength=TackOutDouble(iOutCard,iOutCount,iTempCard,FALSE);
				iMeHandLength=TackOutDouble(iSameHuaCardBuf,iSameHuaCount,iTempCard,FALSE);
				if ((iMeHandLength>iMeOutLength)&&(iMeOutLength<(iBaseCount-1))) return FALSE;

				return TRUE;
			}
		}
	}
	else if (iBaseKind==UG_SHOW_CARD)	//甩牌
	{
		//分析扑克
		CardAnalyseStruct AnalyBase,AnalyOut,AnalyHand;
		AnalyseCard(iBaseCard,iBaseCount,AnalyBase);
		AnalyseCard(iOutCard,iOutCount,AnalyOut);
		AnalyseCard(iSameHuaCardBuf,iSameHuaCount,AnalyHand);

		BYTE iHandMaxLength,iOutMaxLength;
		//三条拖拉机分析
		if (AnalyOut.iThreeTractorCount<AnalyBase.iThreeTractorCount)
		{
			iHandMaxLength=MaxThreeTractorLong(iSameHuaCardBuf,iSameHuaCount,NULL);
			iOutMaxLength=MaxThreeTractorLong(iOutCard,iOutCount,NULL);
			if(iHandMaxLength>iOutMaxLength) return FALSE;
			//if (AnalyHand.iThreeTractorCount>AnalyOut.iThreeTractorCount) return FALSE;
			AnalyBase.iThreeCount+=AnalyBase.iThreeTractorCount-AnalyOut.iThreeTractorCount;
			AnalyBase.iThreeTractorCount-=AnalyOut.iThreeTractorCount;
		}
		else
		{
			AnalyHand.iThreeCount+=(AnalyHand.iThreeTractorCount-AnalyBase.iThreeTractorCount);
			AnalyHand.iThreeTractorCount-=AnalyBase.iThreeTractorCount;
			AnalyOut.iThreeCount+=(AnalyOut.iThreeTractorCount-AnalyBase.iThreeTractorCount);
			AnalyOut.iThreeTractorCount-=AnalyBase.iThreeTractorCount;
		}

		//三条分析
		if (AnalyOut.iThreeCount<AnalyBase.iThreeCount)		
		{
			
			if (AnalyHand.iThreeCount>AnalyOut.iThreeCount) return FALSE;
			AnalyBase.iDoubleCount+=(AnalyBase.iThreeCount-AnalyOut.iThreeCount)*2/3;
			AnalyBase.iSignedCount+=(AnalyBase.iThreeCount-AnalyOut.iThreeCount)/3;
			AnalyBase.iThreeCount-=AnalyOut.iThreeCount;
		}
		else
		{
			AnalyHand.iDoubleCount+=(AnalyHand.iThreeCount-AnalyBase.iThreeCount)/3*2;
			AnalyHand.iSignedCount+=(AnalyHand.iThreeCount-AnalyBase.iThreeCount)/3;
			AnalyHand.iThreeCount-=AnalyBase.iThreeCount;
			AnalyOut.iDoubleCount+=(AnalyOut.iThreeCount-AnalyBase.iThreeCount)/3*2;
			AnalyOut.iSignedCount+=(AnalyOut.iThreeCount-AnalyBase.iThreeCount)/3;
			AnalyOut.iThreeCount-=AnalyBase.iThreeCount;
		}

		//两对拖拉机分析
		if (AnalyOut.iDoubleTractorCount<AnalyBase.iDoubleTractorCount)
		{
			iHandMaxLength=MaxDoubleTractorLong(iSameHuaCardBuf,iSameHuaCount,NULL);
			iOutMaxLength=MaxDoubleTractorLong(iOutCard,iOutCount,NULL);
			if(iHandMaxLength>iOutMaxLength) return FALSE;
			AnalyBase.iDoubleCount+=AnalyBase.iDoubleTractorCount-AnalyOut.iDoubleTractorCount;
			AnalyBase.iDoubleTractorCount-=AnalyOut.iDoubleTractorCount;
		}
		else
		{
			AnalyHand.iDoubleCount+=(AnalyHand.iDoubleTractorCount-AnalyBase.iDoubleTractorCount);
			AnalyHand.iDoubleTractorCount-=AnalyBase.iDoubleTractorCount;
			AnalyOut.iDoubleCount+=(AnalyOut.iDoubleTractorCount-AnalyBase.iDoubleTractorCount);
			AnalyOut.iDoubleTractorCount-=AnalyBase.iDoubleTractorCount;
		}

		//对牌分析
		if ((AnalyOut.iDoubleCount<AnalyBase.iDoubleCount)&&(AnalyHand.iDoubleCount>AnalyOut.iDoubleCount))
			return FALSE;
	}
	return TRUE;
}

BOOL CUpGradeGameLogic::CanOutCardEx(BYTE iOutCard[], int iOutCount, BYTE iBaseCard[], int iBaseCount, BYTE iHandCard[], int iHandCount,BYTE iMaxCard[],int iMaxCardCount,BOOL bIsHaveScore)
{
	//按照升级的规则可以出的话,再进行进一步的判断
	if(CanOutCard(iOutCard,iOutCount,iBaseCard,iBaseCount,iHandCard,iHandCount))
	{
		if(iBaseCount==0)
		{
			return TRUE;
		}
		//查找是否有比最大牌大的牌(根据手上是否有这种花色来确定查找范围)
		int iBaseHuaKind=GetCardHuaKind(iBaseCard[0],FALSE);
		BYTE iSameHuaCard[PLAYER_CARD_CNT]={0};
		int iSameHuaCount=TackOutCardByHua(iHandCard,iHandCount,iBaseHuaKind,iSameHuaCard,FALSE);
		bool bIsHaveHua=true;
		//没有这种花色了就在全部扑克里面找
		if(iSameHuaCount<iBaseCount)
		{
			bIsHaveHua=false;
			if(iSameHuaCount==0)
			{
				iSameHuaCount=iHandCount;
				memcpy(iSameHuaCard,iHandCard,iHandCount);
			}
			//花色不够的情况特殊处理
			else
			{
				BYTE temp[PLAYER_CARD_CNT]={0};
				int  temp_cnt=iSameHuaCount;
				memcpy(temp,iSameHuaCard,iSameHuaCount);

				iSameHuaCount=iHandCount;
				memcpy(iSameHuaCard,iHandCard,iHandCount);
				iSameHuaCount-=RemoveCard(temp,temp_cnt,iSameHuaCard,iSameHuaCount);

				int iBaseTemp=iBaseCount-iSameHuaCount;
				BYTE iOutTemp[PLAYER_CARD_CNT]={0};
				int  iOutCountTemp=iOutCount;
				//从出的牌中清除掉该该花色
				memcpy(iOutTemp,iOutCard,iOutCountTemp);
				iOutCountTemp-=RemoveCard(temp,temp_cnt,iOutTemp,iOutCountTemp);

				return IsGiveScore(iSameHuaCard,iSameHuaCount,iBaseCard,iBaseTemp,iOutTemp,iOutCountTemp,bIsHaveHua);
			}
		}
		//如果当前出的牌比最大牌大
		if(CompareCardEx(iOutCard,iOutCount,iMaxCard,iMaxCardCount)&&GetCardShape(iOutCard,iOutCount)!=UG_SHOW_CARD)
		{
			return TRUE;
		}
		if(!bIsHaveScore)
		{
			bIsHaveScore=IsHaveScore(iOutCard,iOutCount);
		}
		//如果有分的情况下
		if(bIsHaveScore)
		{	
			BYTE iMoreCard[PLAYER_CARD_CNT]={0};
			int iMoreCardCount=0;
			//查找是否有更大的牌
			FindMoreCard(iSameHuaCard,iSameHuaCount,iMaxCard,iMaxCardCount,iMoreCard,iMoreCardCount);
			//有更大的牌必须出更大的
			if(iMoreCardCount!=0)
			{
				return FALSE;
			}
			else
			{
				return IsGiveScore(iSameHuaCard,iSameHuaCount,iBaseCard,iBaseCount,iOutCard,iOutCount,bIsHaveHua);
			}
		}
		return IsGiveScore(iSameHuaCard,iSameHuaCount,iBaseCard,iBaseCount,iOutCard,iOutCount,bIsHaveHua);

	}
	return FALSE;
}

//是否是送分
BOOL CUpGradeGameLogic::IsGiveScore(BYTE iHandCard[], int iHandCardCount,BYTE iBaseCard[], int iBaseCount,BYTE iOutCard[], int iOutCount,bool bIsHaveHua)
{
	BYTE iScoreCard[PLAYER_CARD_CNT]={0};
	BYTE iScoreCardCount=0;
	GetScore(iOutCard,iOutCount,iScoreCard,iScoreCardCount);
	//如果出的牌中没分数
	if(iScoreCardCount==0)
	{
		return TRUE;
	}
	//检验是否是在送分
	BYTE iTempCard[PLAYER_CARD_CNT]={0};
	int iTempCardCount=iHandCardCount;
	memcpy(iTempCard,iHandCard,iTempCardCount);

	BYTE iAllScore[PLAYER_CARD_CNT]={0};
	BYTE  iAllScoreCount=0;
	GetScore(iTempCard,iTempCardCount,iAllScore,iAllScoreCount);
	if(iAllScoreCount<iScoreCardCount)
	{
		ASSERT(FALSE);
		return FALSE;
	}
	iTempCardCount -=RemoveCard(iAllScore,iAllScoreCount,iTempCard,iTempCardCount);
	//如果没有该花色了，必须要求分牌是必须稀的（只有分牌了），并且是分牌是所有分牌中最小的
	//出
	if(!bIsHaveHua||iOutCount==1)
	{
		//非分牌够跟或跟的分牌太多
		if(iTempCardCount>=iOutCount||iScoreCardCount!=iOutCount-iTempCardCount)
		{
			return FALSE;
		}
		return FollowScoreRule(iScoreCard,iScoreCardCount,iAllScore,iAllScoreCount);
	}
	BYTE kind=GetCardShape(iBaseCard,iBaseCount);
	BYTE kind1=GetCardShape(iOutCard,iOutCount);
	BYTE temp[PLAYER_CARD_CNT]={0};
	int  cnt=0;
	if(kind ==UG_DOUBLE)
	{
		if(kind1 ==kind)
		{
			//提取对牌
			cnt=TackOutDouble(iHandCard,iHandCardCount,temp,FALSE);
			if(cnt==2)
			{
				return TRUE;
			}
			else
			{
				if(cnt<2)
				{
					ASSERT(FALSE);
					return FALSE;
				}
				cnt-=RemoveCard(iScoreCard,iScoreCardCount,temp,cnt);

				int tempcnt=0;
				for(int i=0;i<cnt;i++)
				{
					if(GetScore(temp[i]))
					{
						if(CompareOnlyOne(temp[i],iScoreCard[0]))
						{
							tempcnt++;
						}
						else
						{
							return FALSE;	
						}
					}
				}
				return tempcnt!=0;
			}
		}
		else
		{
			//非分牌够跟或跟的分牌太多
			if(iTempCardCount>=iOutCount||iScoreCardCount!=iOutCount-iTempCardCount)
			{
				return FALSE;
			}
			return FollowScoreRule(iScoreCard,iScoreCardCount,iAllScore,iAllScoreCount);
		}
	}
	memset(temp,0,sizeof(temp));
	if(kind==UG_DOUBLE_TRACKOR)
	{

		if(kind1 ==kind)
		{
			//提取拖拉机
			cnt=TackOutDoubleTrackor(iHandCard,iHandCardCount,temp,FALSE);
			if(cnt==iOutCount)
			{
				return TRUE;
			}
			else
			{
				if(cnt<iOutCount)
				{
					ASSERT(FALSE);
					return FALSE;
				}
				//			cnt-=RemoveCard(iOutCard,iOutCount,temp,cnt);
				int pos=0;
				int size_double=0;
				for(pos=0;pos<PLAYER_CARD_CNT;)
				{
					if(temp[pos+1]==0)
					{
						break;
					}
					if(temp[pos+2]==iOutCard[0])
					{
						//清除掉该拖拉机
						for(int i=pos;i<PLAYER_CARD_CNT-(temp[pos+1]+2);i++)
						{
							temp[i]=temp[i+temp[pos+1]+2];
						}
						break;
					}
					pos += temp[pos+1]+2;
				}
				int tempcnt=0;
				for(pos=0;pos<PLAYER_CARD_CNT;)
				{
					if(temp[pos+1]==0)
					{
						break;
					}
					if(temp[pos+1]==iOutCount)
					{
						for(int i=pos+2;i<pos+2+temp[pos+1];i++)
						{
							if(GetScore(temp[i]))
							{
								if(CompareOnlyOne(temp[i],iScoreCard[0]))
								{
									tempcnt++;
								}
								else
								{
									return FALSE;	
								}
							}
						}
					}
					pos += temp[pos+1]+2;
				}
				return tempcnt!=0;
			}
		}
		else
		{
			cnt=TackOutDouble(iOutCard,iOutCount,temp,FALSE);
			//不是全对
			if(cnt!=iOutCount)
			{
				//查看分牌是否是必须出的
				BYTE iOutCard_temp[PLAYER_CARD_CNT]={0};
				int  iOutCount_temp=iOutCount;
				memcpy(iOutCard_temp,iOutCard,iOutCount);
				//清除对牌
				iOutCount_temp -=RemoveCard(temp,cnt,iOutCard_temp,iOutCount_temp);
				BYTE iScore_temp[PLAYER_CARD_CNT]={0};
				BYTE  iScore_cnt=0;
				//非对牌中查找
				GetScore(iOutCard_temp,iOutCount_temp,iScore_temp,iScore_cnt);
				if(!iScore_cnt)
				{
					return TRUE;
				}

				//非分牌 够跟或跟的分牌太多
				if(iTempCardCount>=iOutCount_temp||iScoreCardCount!=iOutCount_temp-iTempCardCount)
				{
					return FALSE;
				}
				//找到出牌中的对牌中的分数
				BYTE iScore_temp_[PLAYER_CARD_CNT]={0};
				BYTE  iScore_cnt_=0;
				GetScore(temp,cnt,iScore_temp_,iScore_cnt_);
				//清除对牌中的分数
				if(iScore_cnt_)
				{
					iScoreCardCount-=RemoveCard(iScore_temp_,iScore_cnt_,iScoreCard,iScoreCardCount);
					iAllScoreCount-=RemoveCard(iScore_temp_,iScore_cnt_,iAllScore,iAllScoreCount);
				}
				return FollowScoreRule(iScoreCard,iScoreCardCount,iAllScore,iAllScoreCount);
			}

			iTempCardCount=iHandCardCount;
			memcpy(iTempCard,iHandCard,iTempCardCount);
			iTempCardCount -=RemoveCard(iOutCard,iOutCount,iTempCard,iTempCardCount);

			BYTE temp1[PLAYER_CARD_CNT]={0};
			//查看是否还有其他的对
			int   cnt1=TackOutDouble(iTempCard,iTempCardCount,temp1,FALSE);
			if(cnt1==0)
			{
				return TRUE;
			}
			BYTE iScore_temp[PLAYER_CARD_CNT]={0};
			BYTE  iScore_cnt=0;
			GetScore(temp1,cnt1,iScore_temp,iScore_cnt);
			//没分牌
			if(iScore_cnt==0)
			{
				return FALSE;
			}
			//有分牌，查看出的是否是最小的
			for(int i=0;i<iScore_cnt;i++)
			{
				for(int j=0;j<iScoreCardCount;j++)
				{
					if(GetCardBulk(iScoreCard[j],FALSE)>GetCardBulk(iScore_temp[i],FALSE))
					{
						return FALSE;
					}
				}
			}
			return TRUE;
		}
	}

	memset(temp,0,sizeof(temp));
	if(kind==UG_SHOW_CARD)
	{
		//分析扑克
		CardAnalyseStruct AnalyBase,AnalyOut,AnalyHand;
		AnalyseCard(iBaseCard,iBaseCount,AnalyBase);
		AnalyseCard(iOutCard,iOutCount,AnalyOut);
		AnalyseCard(iHandCard,iHandCardCount,AnalyHand);
		//两对拖拉机分析
		//如果首先出牌的拖拉机长
		if (AnalyOut.iDoubleTractorCount<AnalyBase.iDoubleTractorCount)
		{
			//还有更长的未出
			if (AnalyHand.iDoubleTractorCount>AnalyOut.iDoubleTractorCount) return FALSE;
			//把多出的拖拉机对子仅仅当成对子
			AnalyBase.iDoubleCount+=AnalyBase.iDoubleTractorCount-AnalyOut.iDoubleTractorCount;
			AnalyBase.iDoubleTractorCount-=AnalyOut.iDoubleTractorCount;
		}
		else
		{
			//把手上多出的拖拉机对子当成普通对子
			AnalyHand.iDoubleCount+=(AnalyHand.iDoubleTractorCount-AnalyBase.iDoubleTractorCount);
			AnalyHand.iDoubleTractorCount-=AnalyBase.iDoubleTractorCount;
			//同上
			AnalyOut.iDoubleCount+=(AnalyOut.iDoubleTractorCount-AnalyBase.iDoubleTractorCount);
			AnalyOut.iDoubleTractorCount-=AnalyBase.iDoubleTractorCount;
		}
		BYTE score_temp[PLAYER_CARD_CNT]={0};
		BYTE  score_cnt=0;

		if(AnalyOut.iDoubleCount>AnalyBase.iDoubleCount)
		{
			//把分牌当单牌;
			GetScore(AnalyOut.iDoubleArray,AnalyOut.iDoubleCount,score_temp,score_cnt);
			if(score_cnt)
			{
				int i=score_cnt;
				//只提分牌中小的
				while(AnalyOut.iDoubleCount-i<AnalyBase.iDoubleCount)
				{
					i-=2;
				}
				//把分牌加入到单牌中
				if(i)
				{
					memcpy(&AnalyOut.iSingleCardArray[AnalyOut.iSignedCount],\
						score_temp,i);	
					AnalyOut.iSignedCount+=i;
				}
			}
		}
		GetScore(AnalyOut.iSingleCardArray,AnalyOut.iSignedCount,score_temp,score_cnt);
		if(!score_cnt)
		{
			return TRUE;
		}
		memcpy(iTempCard,iHandCard,iHandCardCount);
		iTempCardCount=iHandCardCount;
		iTempCardCount-=RemoveCard(iOutCard,iOutCount,iTempCard,iTempCardCount);
		if(iTempCardCount==0)
		{
			return TRUE;
		}
		BYTE score__temp[PLAYER_CARD_CNT]={0};
		BYTE score__cnt=0;
		GetScore(iTempCard,iTempCardCount,score__temp,score__cnt);
		//剩下的牌中要全部是分
		if(score__cnt!=iTempCardCount)
		{
			return FALSE;
		}
		//比较出的分牌是否是最小的
		for(int i=0;i<score_cnt;i++)
		{
			for(int j=0;j<score__cnt;j++)
			{
				if(GetCardBulk(score_temp[i],FALSE)>GetCardBulk(score__temp[j],FALSE))
				{
					return FALSE;
				}
			}
		}
		return TRUE;
	}
	return TRUE;
}

BOOL CUpGradeGameLogic::FollowScoreRule(BYTE iScoreCard[],int iScoreCardCount,BYTE iAllScore[],int iAllScoreCount)
{

	if(iScoreCardCount==iAllScoreCount)
	{
		return TRUE;
	}
	//分牌是否是最小的那几个
	SortCardsValue(iScoreCard,iScoreCardCount);
	SortCardsValue(iAllScore,iAllScoreCount);
	//如果出的分牌中最大的大于应该出的分牌中的最大牌
	if(GetScore(iScoreCard[0])>GetScore(iAllScore[iAllScoreCount-iScoreCardCount]))
	{
		return FALSE;
	}
	return TRUE;
}

void CUpGradeGameLogic::SortCardsValue(BYTE bCards[],BYTE bSize)
{
	BYTE temp=0;
	for(int i=0;i<bSize;i++)
	{
		bCards[i] = (bCards[i]>>4)|(bCards[i]<<4);
	}
	for(int i=1;i<bSize;i++)
	{
		for(int j=0;j<i;j++)
		{
			if(bCards[j]<bCards[i])
			{
				temp=bCards[i];
				for(int k=i;k>j;k--)
				{
					bCards[k]=bCards[k-1];
				}

				bCards[j]=temp;
				break;
			}
		}
	}
	for(int i=0;i<bSize;i++)
	{
		bCards[i] = (bCards[i]>>4)|(bCards[i]<<4);
	}
}

void CUpGradeGameLogic::FindMoreCard(BYTE iHandCard[], int iHandCardCount,BYTE iMaxCard[],int iMaxCardCount,BYTE iMoreCard[],int &iMoreCardCount)
{
	iMoreCardCount=0;
	for(int i=0;i<iHandCardCount-iMaxCardCount+1;i++)
	{
		if(CompareCardEx(&iHandCard[i],iMaxCardCount,iMaxCard,iMaxCardCount))
		{
			memcpy(iMoreCard,&iHandCard[i],iMaxCardCount);
			iMoreCardCount=iMaxCardCount;
			break;
		}
	}
}
//second先出，first后出
BOOL CUpGradeGameLogic::CompareCardEx(BYTE iFirstCard[],int iFirstCardCount,BYTE iSecondCard[],int iSecondCardCount)
{
	BYTE iCard[PLAYER_CARD_CNT]={0};
	//永远都是最小的那几个
	for(int j=0;j<PLAYER_CARD_CNT;j++)
				iCard[j]=GetCardHuaKind(iFirstCard[0],FALSE);
	BYTE *iTempCard[GAME_PLAYER]={0};
	for(int i=0;i<GAME_PLAYER;i++)
	{
		if(i==1)
		{
			iTempCard[i]=iFirstCard;
		}
		else if(i==0)
		{
			iTempCard[i]=iSecondCard;
		}
		else
		{
			iTempCard[i]=iCard;
		}
	}
	int iWiner=CompareCard(iTempCard,iFirstCardCount);
	if(iWiner==1)
	{
		return TRUE;
	}
	return FALSE;
}

//查找分数
int CUpGradeGameLogic::FindPoint(BYTE iCardList[], int iCardCount)
{
	int iPoint=0,iNum=0;
	for (int i=0;i<iCardCount;i++)
	{
		iNum=GetCardNum(iCardList[i]);
		if (iNum==5) iPoint+=5;
		else if ((iNum==10)||(iNum==13)) iPoint+=10;
	}
	return iPoint;
}

BYTE CUpGradeGameLogic::GetScore(BYTE bCard)
{
	BYTE bValue=GetCardNum(bCard);
	if(bValue ==0x05 ||bValue== 0x0a|| bValue==0x0d)
	{
		return bValue>0x0a?0x0a:bValue;
	}
	//大小王也是分数
	if(GetCardHuaKind(bCard,TRUE)==0x40)
	{
		return 10;
	}
	return 0;
}

BYTE CUpGradeGameLogic::GetBackScore(BYTE bCards[],BYTE bSize)
{
	BYTE bTempScore=0;
	BYTE bAllScore=0;
	for(int i=0;i<bSize;i++)
	{
		bTempScore=GetScore(bCards[i]);
		if(bTempScore)
		{
			bAllScore += bTempScore;
		}
	}
	return bAllScore;
}
//不需要转化为逻辑牌
BYTE CUpGradeGameLogic::GetScore(BYTE bCards[],BYTE bSize,BYTE bScoreCard[],BYTE &bScoreSize)
{
	BYTE bAllScore=0;
	BYTE bTempScore=0;
	bScoreSize=0;
	for(int i=0;i<bSize;i++)
	{
		bTempScore=GetScore(bCards[i]);
		if(bTempScore !=0)
		{
			bAllScore += bTempScore;
			bScoreCard[bScoreSize++]=bCards[i];
		}
	}
	return bAllScore;
}

//查找最长的拖拉机
int	CUpGradeGameLogic::MaxThreeTractorLong(BYTE iCardList[], int iCardCount, BYTE iTrackorBuffer[])
{
	int iMaxPos=0,iMaxCount=0,i=0;
	BYTE iTempThreeTrackor[48];
	int iCount=TackOutThreeTrackor(iCardList,iCardCount,iTempThreeTrackor,FALSE);
	while (i<iCount)
	{
		if (iTempThreeTrackor[i+1]>iMaxCount)
		{
			iMaxPos=i;
			iMaxCount=iTrackorBuffer[i+1];
		}
		i+=(iTempThreeTrackor[i+1]+2);
	}
	if ((iMaxCount!=0)&&(iTrackorBuffer!=NULL)) 
		::CopyMemory(iTrackorBuffer,&iTempThreeTrackor[iMaxPos],sizeof(BYTE)*(iMaxCount+2));

	return iMaxCount;
}

//查找最长的拖拉机
int	CUpGradeGameLogic::MaxDoubleTractorLong(BYTE iCardList[], int iCardCount, BYTE iTrackorBuffer[])
{
	int iMaxPos=0,iMaxCount=0,i=0;
	BYTE iTempDoubleTrackor[54];
	int iCount=TackOutDoubleTrackor(iCardList,iCardCount,iTempDoubleTrackor,FALSE);
	while (i<iCount)
	{
		if (iTempDoubleTrackor[i+1]>iMaxCount)
		{
			iMaxPos=i;
			iMaxCount=iTempDoubleTrackor[i+1];
		}
		i+=(iTempDoubleTrackor[i+1]+2);
	}
	if ((iMaxCount!=0)&&(iTrackorBuffer!=NULL)) 
		::CopyMemory(iTrackorBuffer,&iTempDoubleTrackor[iMaxPos],sizeof(BYTE)*(iMaxCount+2));

	return iMaxCount;
}
