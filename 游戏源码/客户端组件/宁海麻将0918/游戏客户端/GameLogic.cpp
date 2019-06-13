#include "StdAfx.h"
#include "GameLogic.h"
#include "AndroidAI.h"

//////////////////////////////////////////////////////////////////////////
//静态变量

//扑克数据
const BYTE CGameLogic::m_cbCardDataArray[MAX_REPERTORY]=
{
		0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//万子
		0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//万子
		0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//万子
		0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//万子
		0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//索子
		0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//索子
		0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//索子
		0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//索子
		0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//同子
		0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//同子
		0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//同子
		0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//同子
		0x31,0x32,0x33,0x34,0x35,0x36,0x37,									//番子
		0x31,0x32,0x33,0x34,0x35,0x36,0x37,									//番子
		0x31,0x32,0x33,0x34,0x35,0x36,0x37,									//番子
		0x31,0x32,0x33,0x34,0x35,0x36,0x37,									//番子
		0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F 							//花子
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

//混乱扑克
void CGameLogic::RandCardData(BYTE cbCardData[], BYTE cbMaxCount)
{
	//混乱准备
	BYTE cbCardDataTemp[CountArray(m_cbCardDataArray)];
	CopyMemory(cbCardDataTemp,m_cbCardDataArray,sizeof(m_cbCardDataArray));

	//混乱扑克
	BYTE cbRandCount=0,cbPosition=0;
	do
	{
		cbPosition=rand()%(cbMaxCount-cbRandCount);
		cbCardData[cbRandCount++]=cbCardDataTemp[cbPosition];
		cbCardDataTemp[cbPosition]=cbCardDataTemp[cbMaxCount-cbRandCount];
	} while (cbRandCount<cbMaxCount);

	return;
}

//删除扑克
bool CGameLogic::RemoveCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbRemoveCard)
{
	//效验扑克
	ASSERT(IsValidCard(cbRemoveCard));
	ASSERT(cbCardIndex[SwitchToCardIndex(cbRemoveCard)]>0);

	//删除扑克
	BYTE cbRemoveIndex=SwitchToCardIndex(cbRemoveCard);
	if (cbCardIndex[cbRemoveIndex]>0)
	{
		cbCardIndex[cbRemoveIndex]--;
		return true;
	}

	//失败效验
	ASSERT(FALSE);

	return false;
}

//删除扑克
bool CGameLogic::RemoveCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbRemoveCard[], BYTE cbRemoveCount)
{
	//删除扑克
	for (BYTE i=0;i<cbRemoveCount;i++)
	{
		//效验扑克
		ASSERT(IsValidCard(cbRemoveCard[i]));
		ASSERT(cbCardIndex[SwitchToCardIndex(cbRemoveCard[i])]>0);

		//删除扑克
		BYTE cbRemoveIndex=SwitchToCardIndex(cbRemoveCard[i]);
		if (cbCardIndex[cbRemoveIndex]==0)
		{
			//错误断言
			ASSERT(FALSE);

			//还原删除
			for (BYTE j=0;j<i;j++) 
			{
				ASSERT(IsValidCard(cbRemoveCard[j]));
				cbCardIndex[SwitchToCardIndex(cbRemoveCard[j])]++;
			}

			return false;
		}
		else 
		{
			//删除扑克
			--cbCardIndex[cbRemoveIndex];
		}
	}

	return true;
}

//删除扑克
bool CGameLogic::RemoveCard(BYTE cbCardData[], BYTE cbCardCount, BYTE cbRemoveCard[], BYTE cbRemoveCount)
{
	//检验数据
	ASSERT(cbCardCount<=MAX_COUNT);
	ASSERT(cbRemoveCount<=cbCardCount);

	//定义变量
	BYTE cbDeleteCount=0,cbTempCardData[MAX_COUNT];
	if (cbCardCount>CountArray(cbTempCardData))
		return false;
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

	//成功判断
	if (cbDeleteCount!=cbRemoveCount) 
	{
		ASSERT(FALSE);
		return false;
	}

	//清理扑克
	BYTE cbCardPos=0;
	for (BYTE i=0;i<cbCardCount;i++)
	{
		if (cbTempCardData[i]!=0) 
			cbCardData[cbCardPos++]=cbTempCardData[i];
	}

	return true;
}

//有效判断
bool CGameLogic::IsValidCard(BYTE cbCardData)
{
	BYTE cbValue=(cbCardData&MASK_VALUE);
	BYTE cbColor=(cbCardData&MASK_COLOR)>>4;
	return (((cbValue>0)&&(cbValue<=9)&&(cbColor<=2))||((cbValue>=1)&&(cbValue<=15)&&(cbColor==3)));
}

//扑克数目
BYTE CGameLogic::GetCardCount(BYTE cbCardIndex[MAX_INDEX])
{
	//数目统计
	BYTE cbCardCount=0;
	for (BYTE i=0;i<MAX_INDEX;i++) 
		cbCardCount+=cbCardIndex[i];

	return cbCardCount;
}

//获取组合
BYTE CGameLogic::GetWeaveCard(BYTE cbWeaveKind, BYTE cbCenterCard, BYTE cbCardBuffer[4])
{
	//组合扑克
	switch (cbWeaveKind)
	{
	case WIK_LEFT:		//上牌操作
		{
			//设置变量
			cbCardBuffer[0]=cbCenterCard;
			cbCardBuffer[1]=cbCenterCard+1;
			cbCardBuffer[2]=cbCenterCard+2;

			return 3;
		}
	case WIK_RIGHT:		//上牌操作
		{
			//设置变量
			cbCardBuffer[0]=cbCenterCard;
			cbCardBuffer[1]=cbCenterCard-1;
			cbCardBuffer[2]=cbCenterCard-2;

			return 3;
		}
	case WIK_CENTER:	//上牌操作
		{
			//设置变量
			cbCardBuffer[0]=cbCenterCard;
			cbCardBuffer[1]=cbCenterCard+1;
			cbCardBuffer[2]=cbCenterCard-1;

			return 3;
		}
	case WIK_PENG:		//碰牌操作
		{
			//设置变量
			cbCardBuffer[0]=cbCenterCard;
			cbCardBuffer[1]=cbCenterCard;
			cbCardBuffer[2]=cbCenterCard;

			return 3;
		}
	case WIK_GANG:		//杠牌操作
		{
			//设置变量
			cbCardBuffer[0]=cbCenterCard;
			cbCardBuffer[1]=cbCenterCard;
			cbCardBuffer[2]=cbCenterCard;
			cbCardBuffer[3]=cbCenterCard;

			return 4;
		}
	default:
		{
			ASSERT(FALSE);
		}
	}

	return 0;
}

//动作等级
BYTE CGameLogic::GetUserActionRank(BYTE cbUserAction)
{
	//胡牌等级
	if (cbUserAction&WIK_CHI_HU) { return 4; }

	//杠牌等级
	if (cbUserAction&WIK_GANG) { return 3; }

	//碰牌等级
	if (cbUserAction&WIK_PENG) { return 2; }

	//上牌等级
	if (cbUserAction&(WIK_RIGHT|WIK_CENTER|WIK_LEFT)) { return 1; }

	return 0;
}

//胡牌等级
BYTE CGameLogic::GetChiHuActionRank(tagChiHuResult & ChiHuResult)
{
	//变量定义
	BYTE cbChiHuOrder=0;
	WORD wChiHuRight=ChiHuResult.wChiHuRight;
	WORD wChiHuKind=(ChiHuResult.wChiHuKind&0xFF00)>>4;

	//大胡升级
	for (BYTE i=0;i<8;i++)
	{
		wChiHuKind>>=1;
		if ((wChiHuKind&0x0001)!=0) 
			cbChiHuOrder++;
	}

	//权位升级
	for (BYTE i=0;i<16;i++)
	{
		wChiHuRight>>=1;
		if ((wChiHuRight&0x0001)!=0) 
			cbChiHuOrder++;
	}

	return cbChiHuOrder;
}

//吃牌判断
BYTE CGameLogic::EstimateEatCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard)
{
	//参数效验
	ASSERT(IsValidCard(cbCurrentCard));

	//过滤判断
	//番子无连
	if (cbCurrentCard>=0x31) 
		return WIK_NULL;

	//变量定义
	BYTE cbExcursion[3]={0,1,2};
	BYTE cbItemKind[3]={WIK_LEFT,WIK_CENTER,WIK_RIGHT};

	//吃牌判断
	BYTE cbEatKind=0,cbFirstIndex=0;
	BYTE cbCurrentIndex=SwitchToCardIndex(cbCurrentCard);
	for (BYTE i=0;i<CountArray(cbItemKind);i++)
	{
		BYTE cbValueIndex=cbCurrentIndex%9;
		if ((cbValueIndex>=cbExcursion[i])&&((cbValueIndex-cbExcursion[i])<=6))
		{
			//吃牌判断
			cbFirstIndex=cbCurrentIndex-cbExcursion[i];
			if ((cbCurrentIndex!=cbFirstIndex)&&(cbCardIndex[cbFirstIndex]==0))
				continue;
			if ((cbCurrentIndex!=(cbFirstIndex+1))&&(cbCardIndex[cbFirstIndex+1]==0))
				continue;
			if ((cbCurrentIndex!=(cbFirstIndex+2))&&(cbCardIndex[cbFirstIndex+2]==0))
				continue;

			//设置类型
			cbEatKind|=cbItemKind[i];
		}
	}

	return cbEatKind;
}

//碰牌判断
BYTE CGameLogic::EstimatePengCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard)
{
	//参数效验
	ASSERT(IsValidCard(cbCurrentCard));

	//碰牌判断
	return (cbCardIndex[SwitchToCardIndex(cbCurrentCard)]>=2)?WIK_PENG:WIK_NULL;
}

//杠牌判断
BYTE CGameLogic::EstimateGangCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard)
{
	//参数效验
	ASSERT(IsValidCard(cbCurrentCard));

	//杠牌判断
	return (cbCardIndex[SwitchToCardIndex(cbCurrentCard)]==3)?WIK_GANG:WIK_NULL;
}

//杠牌分析
BYTE CGameLogic::AnalyseGangCard(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbWeaveCount, tagGangCardResult & GangCardResult)
{
	//设置变量
	BYTE cbActionMask=WIK_NULL;
	ZeroMemory(&GangCardResult,sizeof(GangCardResult));

	//手上杠牌
	for (BYTE i=0;i<MAX_INDEX;i++)
	{
		if (cbCardIndex[i]==4)
		{
			cbActionMask|=WIK_GANG;
			GangCardResult.cbCardData[GangCardResult.cbCardCount]=WIK_GANG;
			GangCardResult.cbCardData[GangCardResult.cbCardCount++]=SwitchToCardData(i);
		}
	}

	//组合杠牌
	for (BYTE i=0;i<cbWeaveCount;i++)
	{
		if (WeaveItem[i].cbWeaveKind==WIK_PENG)
		{
			if (cbCardIndex[SwitchToCardIndex(WeaveItem[i].cbCenterCard)]==1)
			{
				cbActionMask|=WIK_GANG;
				GangCardResult.cbCardData[GangCardResult.cbCardCount]=WIK_GANG;
				GangCardResult.cbCardData[GangCardResult.cbCardCount++]=WeaveItem[i].cbCenterCard;
			}
		}
	}

	return cbActionMask;
}

//吃胡分析
BYTE CGameLogic::AnalyseChiHuCard(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard, WORD wChiHuRight, tagChiHuResult & ChiHuResult,WORD wBankerUser,bool bJugeHuOnly,bool bZimo)
{
	//变量定义
	WORD wChiHuKind=CHK_NULL;
	BYTE cbHuCount = 0;
	BYTE cbHuTimes=0;
	static CAnalyseItemArray AnalyseItemArray;

	//设置变量
	AnalyseItemArray.RemoveAll();
	ZeroMemory(&ChiHuResult,sizeof(ChiHuResult));

	//构造扑克
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));

	//插入扑克
	if (cbCurrentCard!=0)
		cbCardIndexTemp[SwitchToCardIndex(cbCurrentCard)]++;

	if (IsQingYiSe(cbCardIndexTemp,WeaveItem,cbWeaveCount)==true) 
		wChiHuRight|=CHR_QING_YI_SE;
	if (IsHunYiSe(cbCardIndexTemp,WeaveItem,cbWeaveCount)==true)
		wChiHuRight|=CHR_HUN_YI_SE;

	if(true == bZimo)
	{
		wChiHuRight |=CHR_ZI_MO;
		cbHuCount  += 2;
	}

	//分析扑克
	AnalyseCard(cbCardIndexTemp,WeaveItem,cbWeaveCount,AnalyseItemArray);

	//胡牌分析
	if (AnalyseItemArray.GetCount()>0)
	{
		//牌型分析
		for (INT_PTR i=0;i<AnalyseItemArray.GetCount();i++)
		{
			//变量定义
			bool bLianCard=false,bPengCard=false;
			tagAnalyseItem * pAnalyseItem=&AnalyseItemArray[i];

			//牌型分析
			for (BYTE j=0;j<CountArray(pAnalyseItem->cbWeaveKind);j++)
			{
				BYTE cbWeaveKind=pAnalyseItem->cbWeaveKind[j];
				bPengCard=((cbWeaveKind&(WIK_GANG|WIK_PENG))!=0)?true:bPengCard;
				bLianCard=((cbWeaveKind&(WIK_LEFT|WIK_CENTER|WIK_RIGHT))!=0)?true:bLianCard;
			}

			//牌型判断
			ASSERT((bLianCard==true)||(bPengCard==true));

			//碰碰牌型
			if ((bLianCard==false)&&(bPengCard==true)) 
			{
				wChiHuRight|=CHR_PENG_PENG;
			}
			if ((bLianCard==true)||(bPengCard==true))
				wChiHuKind|=CHK_JI_HU;

			//字一色
			if (IsZiYiSe(*pAnalyseItem)==true) 
				wChiHuRight|=CHR_ZI_YI_SE;				
		}
	}
	if(wChiHuKind !=0 && bJugeHuOnly==false)
	{
		BYTE cbHuTempCount[2]={0,0};
		BYTE cbHuTempTimes[2]={0,0};
		
		//判断嵌
		if ((AnalyseItemArray.GetCount() >0)&&(cbCurrentCard !=0)) 
		{
			for (INT_PTR i=0;i<AnalyseItemArray.GetCount();i++)
			{
				tagAnalyseItem * pAnalyseItem=&AnalyseItemArray[i];

				if(pAnalyseItem->cbCardEye>=0x35 && pAnalyseItem->cbCardEye<=0x37)
				{
					cbHuTempCount[1] += 2;
				}

				if((pAnalyseItem->cbCardEye&MASK_COLOR) == 0x30)
				{
					if((pAnalyseItem->cbCardEye&MASK_VALUE)-1 == wBankerUser)
					{
						cbHuTempCount[1] += 2;
					}
				}

				//牌型分析
				for (BYTE j=0;j<cbWeaveCount;j++)
				{
					BYTE cbWeaveKind=WeaveItem[j].cbWeaveKind;
					if(cbWeaveKind==WIK_PENG)
					{
						if((WeaveItem[j].cbCenterCard&MASK_COLOR)==0x30)
						{
							cbHuTempCount[1] +=4;
						}
						else
						{
							if((WeaveItem[j].cbCenterCard&MASK_VALUE)==0x01 ||(WeaveItem[j].cbCenterCard&MASK_VALUE)==0x09)
							{
								cbHuTempCount[1] +=4;
							}
							else
								cbHuTempCount[1] +=2;
						}
					}
					if(cbWeaveKind==WIK_GANG)
					{
						if(WeaveItem[j].cbPublicCard==FALSE)
						{
							if((WeaveItem[j].cbCenterCard&MASK_COLOR)==0x30)
							{
								cbHuTempCount[1] +=32;
							}
							else
							{
								if((WeaveItem[j].cbCenterCard&MASK_VALUE)==0x01 ||(WeaveItem[j].cbCenterCard&MASK_VALUE)==0x09)
								{
									cbHuTempCount[1] +=32;
								}
								else
									cbHuTempCount[1] +=16;
							}
						}
						if(WeaveItem[j].cbPublicCard==TRUE)
						{
							if((WeaveItem[j].cbCenterCard&MASK_COLOR)==0x30)
							{
								cbHuTempCount[1] +=16;
							}
							else
							{
								if((WeaveItem[j].cbCenterCard&MASK_VALUE)==0x01 ||(WeaveItem[j].cbCenterCard&MASK_VALUE)==0x09)
								{
									cbHuTempCount[1] +=16;
								}
								else
									cbHuTempCount[1] +=8;
							}
						}
					}
				}

				for (BYTE j=cbWeaveCount;j<CountArray(pAnalyseItem->cbWeaveKind);j++)
				{
					if(pAnalyseItem->cbWeaveKind[j]==WIK_PENG)
					{
						if((pAnalyseItem->cbCenterCard[j]&MASK_COLOR)==0x30)
						{
							cbHuTempCount[1] += 8;
						}
						else
						{
							if((pAnalyseItem->cbCenterCard[j]&MASK_VALUE)==0x01 ||(pAnalyseItem->cbCenterCard[j]&MASK_VALUE)==0x09)
							{
								cbHuTempCount[1] += 8;
							}
							else
							{
								cbHuTempCount[1] +=4;
							}
						}
					}
	
				}

				for (BYTE j=0;j<CountArray(pAnalyseItem->cbWeaveKind);j++)
				{
					if(pAnalyseItem->cbCenterCard[j]>=0x35 && pAnalyseItem->cbCenterCard[j]<=0x37)
						cbHuTempTimes[1] += 1; 
					if(((pAnalyseItem->cbCenterCard[j]&MASK_VALUE)-1 == wBankerUser) && ((pAnalyseItem->cbCenterCard[j]&MASK_COLOR)==0x30))
						cbHuTempTimes[1] += 1;


				}
				for (BYTE j=cbWeaveCount;j<CountArray(pAnalyseItem->cbWeaveKind);j++)
				{
					if ((pAnalyseItem->cbCardData[j][1] == cbCurrentCard)&&(pAnalyseItem->cbWeaveKind[j]== WIK_LEFT))
					{
						cbHuTempCount[1] +=2;
						break;
					}
				}
				if(bZimo==false)
				{
					for (BYTE j=cbWeaveCount;j<CountArray(pAnalyseItem->cbWeaveKind);j++)
					{
						if(pAnalyseItem->cbCenterCard[j]!=cbCurrentCard) continue;
						if(pAnalyseItem->cbWeaveKind[j]==WIK_PENG)
						{
							if((pAnalyseItem->cbCenterCard[j]&MASK_COLOR)==0x30 && cbCardIndex[SwitchToCardIndex(cbCurrentCard)<3])
							{
								cbHuTempCount[1] -= 4;
							}
							else
							{
								if(((pAnalyseItem->cbCenterCard[j]&MASK_VALUE)==0x01 ||(pAnalyseItem->cbCenterCard[j]&MASK_VALUE)==0x09)&&( cbCardIndex[SwitchToCardIndex(cbCurrentCard)<3]))
								{
									cbHuTempCount[1] -= 4;
								}
								else
								{
									cbHuTempCount[1] -=2;
								}
							}
						}
					}

				}



				if(cbHuTempCount[0]<cbHuTempCount[1])
				{
					cbHuTempCount[0]=cbHuTempCount[1];
				}
				if(cbHuTempTimes[0]<cbHuTempTimes[1])
				{
					cbHuTempTimes[0]=cbHuTempTimes[1];
					cbHuTimes=cbHuTempTimes[0];
				}
	
			}


		}
		cbHuCount +=cbHuTempCount[0];

	}

	//结果判断
	if (wChiHuKind!=0)
	{
		//设置结果
		ChiHuResult.wChiHuKind=wChiHuKind;
		ChiHuResult.wChiHuRight=wChiHuRight;
		if(bJugeHuOnly)
		{
			ChiHuResult.cbHuCount=0;
			ChiHuResult.cbHuTimes=0;
		}
		else
		{
			ChiHuResult.cbHuCount=cbHuCount;
			ChiHuResult.cbHuTimes=cbHuTimes;
		}

		return WIK_CHI_HU;
	}

	return WIK_NULL;
}

//清一色牌
bool CGameLogic::IsQingYiSe(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbItemCount)
{
	//胡牌判断
	BYTE cbCardColor=0xFF;

	for (BYTE i=0;i<MAX_INDEX;i++)
	{
		if (cbCardIndex[i]!=0)
		{
			//花色判断
			if (cbCardColor!=0xFF)
				return false;

			//设置花色
			cbCardColor=(SwitchToCardData(i)&MASK_COLOR);

			//设置索引
			i=(i/9+1)*9-1;
		}
	}

	//过滤字牌
	if(cbCardColor == 0x30) return false;

	//组合判断
	for (BYTE i=0;i<cbItemCount;i++)
	{
		BYTE cbCenterCard=WeaveItem[i].cbCenterCard;
		if ((cbCenterCard&MASK_COLOR)!=cbCardColor)
			return false;
	}

	return true;
}

//混一色牌
bool CGameLogic::IsHunYiSe(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbItemCount)
{
	//变量定义
	bool bColorFlags[5];
	ZeroMemory(&bColorFlags,sizeof(bColorFlags));

	//扑克扑克
	for (BYTE i=0;i<MAX_INDEX;i++)
	{
		if (cbCardIndex[i]!=0) 
			bColorFlags[i/9]=true;
	}

	//组合判断
	for (BYTE i=0;i<cbItemCount;i++)
	{
		BYTE cbCenterCard=WeaveItem[i].cbCenterCard;
		bColorFlags[(cbCenterCard&MASK_COLOR)>>4]=true;
	}


	//花色统计
	BYTE cbColorCount=0;
	for (BYTE i=0;i<3;i++)
	{
		if (bColorFlags[i]==true)
			cbColorCount++;
	}
	if ((cbColorCount!=1)||(bColorFlags[3]==false)) 
		return false;

	return true;
}

//字一色牌
bool CGameLogic::IsZiYiSe(tagAnalyseItem & AnalyseItem)
{
	//扑克判断
	for (BYTE i=0;i<CountArray(AnalyseItem.cbWeaveKind);i++)
	{
		if ((AnalyseItem.cbCenterCard[i]&MASK_COLOR)!=0x30)
			return false;
	}

	return true;
}

//扑克转换
BYTE CGameLogic::SwitchToCardData(BYTE cbCardIndex)
{
	ASSERT(cbCardIndex<MAX_INDEX);
	if(cbCardIndex<34)
		return ((cbCardIndex/9)<<4)|(cbCardIndex%9+1);
	else
		return (48|((cbCardIndex-34)%8+8));
}

//扑克转换
BYTE CGameLogic::SwitchToCardIndex(BYTE cbCardData)
{
	ASSERT(IsValidCard(cbCardData));
	//计算位置
	BYTE cbValue=cbCardData&MASK_VALUE;
	BYTE cbColor=(cbCardData&MASK_COLOR)>>4;

	return (cbColor>=0x03)?(cbValue+27-1):(cbColor*9+cbValue-1);
}

//扑克转换
BYTE CGameLogic::SwitchToCardData(BYTE cbCardIndex[MAX_INDEX], BYTE cbCardData[MAX_COUNT])
{
	//转换扑克
	BYTE cbPosition=0;
	for (BYTE i=0;i<MAX_INDEX;i++)
	{
		if (cbCardIndex[i]!=0)
		{
			for (BYTE j=0;j<cbCardIndex[i];j++)
			{
				ASSERT(cbPosition<MAX_COUNT);
				cbCardData[cbPosition++]=SwitchToCardData(i);
			}
		}
	}

	return cbPosition;
}
//扑克转换
BYTE CGameLogic::SwitchToCardData(BYTE cbCardIndex[MAX_INDEX], BYTE cbCardData[MAX_COUNT],BYTE bMaxCount)
{
	//转换扑克 王牌放到最前面
	BYTE bPosition=0;

	for (BYTE i=0;i<MAX_INDEX;i++) 
	{
		if (cbCardIndex[i]!=0)
		{
			for (BYTE j=0;j<cbCardIndex[i];j++)
			{
				ASSERT(bPosition<bMaxCount);
				cbCardData[bPosition++]=SwitchToCardData(i);
			}
		}
	}

	return bPosition;
}


//扑克转换
BYTE CGameLogic::SwitchToCardIndex(BYTE cbCardData[], BYTE cbCardCount, BYTE cbCardIndex[MAX_INDEX])
{
	//设置变量
	ZeroMemory(cbCardIndex,sizeof(BYTE)*MAX_INDEX);

	//转换扑克
	for (BYTE i=0;i<cbCardCount;i++)
	{
		ASSERT(IsValidCard(cbCardData[i]));
		cbCardIndex[SwitchToCardIndex(cbCardData[i])]++;
	}

	return cbCardCount;
}

//算抬函数
LONG CGameLogic::CalScore(tagChiHuResult & ChiHuResult,LONG &lTimes)
{
	WORD wChihuKind = ChiHuResult.wChiHuKind;

	WORD wChihuRight = ChiHuResult.wChiHuRight;
	if (wChihuRight&CHR_HUN_YI_SE)
	{
		lTimes += RIGHT_HUN_YI_SE;
	}
	if(wChihuRight&CHR_PENG_PENG)
	{
		lTimes +=RIGHT_PENG_PENG;
	}
	if(wChihuRight&CHR_QING_YI_SE)
	{
		lTimes += RIGHT_QING_YI_SE;
	}
	if(wChihuRight&CHR_ZI_YI_SE)
	{
		lTimes += RIGHT_ZI_YI_SE;
	}

	return lTimes;
}

//分析扑克
bool CGameLogic::AnalyseCard(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbWeaveCount, CAnalyseItemArray & AnalyseItemArray)
{
	//计算数目
	BYTE cbCardCount=0;
	for (BYTE i=0;i<MAX_INDEX;i++) 
		cbCardCount+=cbCardIndex[i];

	//效验数目
	ASSERT((cbCardCount>=2)&&(cbCardCount<=MAX_COUNT)&&((cbCardCount-2)%3==0));
	if ((cbCardCount<2)||(cbCardCount>MAX_COUNT)||((cbCardCount-2)%3!=0))
		return false;

	//变量定义
	BYTE cbKindItemCount=0;
	tagKindItem KindItem[MAX_COUNT-2];
	ZeroMemory(KindItem,sizeof(KindItem));

	//需求判断
	BYTE cbLessKindItem=(cbCardCount-2)/3;
	ASSERT((cbLessKindItem+cbWeaveCount)==4);

	//单吊判断 
	if (cbLessKindItem==0)
	{
		//效验参数
		ASSERT((cbCardCount==2)&&(cbWeaveCount==4));

		//牌眼判断
		for (BYTE i=0;i<MAX_INDEX-8;i++)
		{
			if (cbCardIndex[i]==2)
			{
				//变量定义
				tagAnalyseItem AnalyseItem;
				ZeroMemory(&AnalyseItem,sizeof(AnalyseItem));

				//设置结果
				for (BYTE j=0;j<cbWeaveCount;j++)
				{
					AnalyseItem.cbWeaveKind[j]=WeaveItem[j].cbWeaveKind;
					AnalyseItem.cbCenterCard[j]=WeaveItem[j].cbCenterCard;
				}
				AnalyseItem.cbCardEye=SwitchToCardData(i);

				//插入结果
				AnalyseItemArray.Add(AnalyseItem);

				return true;
			}
		}

		return false;
	}

	//拆分分析
	if (cbCardCount>=3)
	{
		for (BYTE i=0;i<MAX_INDEX-8;i++)
		{
			//同牌判断
			if (cbCardIndex[i]>=3)
			{
				KindItem[cbKindItemCount].cbCenterCard=i;
				KindItem[cbKindItemCount].cbCardIndex[0]=i;
				KindItem[cbKindItemCount].cbCardIndex[1]=i;
				KindItem[cbKindItemCount].cbCardIndex[2]=i;
				KindItem[cbKindItemCount++].cbWeaveKind=WIK_PENG;
			}
			
			//连牌判断
			if ((i<(MAX_INDEX-2-15))&&(cbCardIndex[i]>0)&&((i%9)<7))
			{
				for (BYTE j=1;j<=cbCardIndex[i];j++)
				{
					if ((cbCardIndex[i+1]>=j)&&(cbCardIndex[i+2]>=j))
					{
						KindItem[cbKindItemCount].cbCenterCard=i;
						KindItem[cbKindItemCount].cbCardIndex[0]=i;
						KindItem[cbKindItemCount].cbCardIndex[1]=i+1;
						KindItem[cbKindItemCount].cbCardIndex[2]=i+2;
						KindItem[cbKindItemCount++].cbWeaveKind=WIK_LEFT;
					}
				}
			}
		}
	}

	//组合分析
	if (cbKindItemCount>=cbLessKindItem)
	{
		//变量定义
		BYTE cbCardIndexTemp[MAX_INDEX];
		ZeroMemory(cbCardIndexTemp,sizeof(cbCardIndexTemp));

		//变量定义
		BYTE cbIndex[4]={0,1,2,3};
		tagKindItem * pKindItem[4];
		ZeroMemory(&pKindItem,sizeof(pKindItem));

		//开始组合
		do
		{
			//设置变量
			CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));
			for (BYTE i=0;i<cbLessKindItem;i++)
				pKindItem[i]=&KindItem[cbIndex[i]];

			//数量判断
			bool bEnoughCard=true;
			for (BYTE i=0;i<cbLessKindItem*3;i++)
			{
				//存在判断
				BYTE cbCardIndex=pKindItem[i/3]->cbCardIndex[i%3]; 
				if (cbCardIndexTemp[cbCardIndex]==0)
				{
					bEnoughCard=false;
					break;
				}
				else 
					cbCardIndexTemp[cbCardIndex]--;
			}

			//胡牌判断
			if (bEnoughCard==true)
			{
				//牌眼判断
				BYTE cbCardEye=0;
				for (BYTE i=0;i<MAX_INDEX-8;i++)
				{
					if (cbCardIndexTemp[i]==2)
					{
						cbCardEye=SwitchToCardData(i);
						break;
					}
				}

				//组合类型
				if (cbCardEye!=0)
				{
					//变量定义
					tagAnalyseItem AnalyseItem;
					ZeroMemory(&AnalyseItem,sizeof(AnalyseItem));


					//设置组合
					for (BYTE i=0;i<cbWeaveCount;i++)
					{
						AnalyseItem.cbWeaveKind[i]=WeaveItem[i].cbWeaveKind;
						AnalyseItem.cbCenterCard[i]=WeaveItem[i].cbCenterCard;
						GetWeaveCard(WeaveItem[i].cbWeaveKind,WeaveItem[i].cbCenterCard,AnalyseItem.cbCardData[i]);

					}

					//设置牌型
					for (BYTE i=0;i<cbLessKindItem;i++) 
					{
						AnalyseItem.cbWeaveKind[i+cbWeaveCount]=pKindItem[i]->cbWeaveKind;
						BYTE cbCenterCard = SwitchToCardData(pKindItem[i]->cbCenterCard);
						AnalyseItem.cbCenterCard[i+cbWeaveCount]=cbCenterCard;
						GetWeaveCard(pKindItem[i]->cbWeaveKind,cbCenterCard,AnalyseItem.cbCardData[i+cbWeaveCount]);

					}

					//设置牌眼
					AnalyseItem.cbCardEye=cbCardEye;

					//插入结果
					AnalyseItemArray.Add(AnalyseItem);
				}
			}

			//设置索引
			if (cbIndex[cbLessKindItem-1]==(cbKindItemCount-1))
			{
				for (BYTE i=cbLessKindItem-1;i>0;i--)
				{
					if ((cbIndex[i-1]+1)!=cbIndex[i])
					{
						BYTE cbNewIndex=cbIndex[i-1];
						for (BYTE j=(i-1);j<cbLessKindItem;j++) 
							cbIndex[j]=cbNewIndex+j-i+2;
						break;
					}
				}
				if (i==0)
					break;
			}
			else
				cbIndex[cbLessKindItem-1]++;
			
		} while (true);

	}

	return (AnalyseItemArray.GetCount()>0);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//机器人相关


//听牌分析
bool CGameLogic::AnalyseTingCard(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbItemCount, WORD wChiHuRight)
{
	//变量定义
	tagChiHuResult ChiHuResult;
	ZeroMemory(&ChiHuResult,sizeof(ChiHuResult));

	//构造扑克
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));

	//听牌分析
	BYTE cbHandTai=0,cbHandFeng=0,cbBankerUser=0;
	for (BYTE i=0;i<MAX_INDEX;i++)
	{

		//听牌判断
		for (BYTE j=0;j<MAX_INDEX;j++)
		{
			//胡牌分析
			BYTE cbCurrentCard=SwitchToCardData(j);
			BYTE cbHuCardKind=AnalyseChiHuCard(cbCardIndexTemp,WeaveItem,cbItemCount,cbCurrentCard,wChiHuRight,ChiHuResult,
				cbBankerUser,true,true);

			//结果判断
			if (cbHuCardKind!=CHK_NULL) 
				return true;
		}

	}

	return false;
}

//分析单牌数
BYTE CGameLogic::AnalyseSingleCount( BYTE cbCardIndex[MAX_INDEX] )
{
	//计算数目
	BYTE cbCardCount=0;
	for (BYTE i=0;i<MAX_INDEX;i++) 
	{
		ASSERT( cbCardIndex[i]>=0 && cbCardIndex[i]<=4 );
		cbCardCount+=cbCardIndex[i];
	}

	//效验数目
	ASSERT((cbCardCount>=1)&&(cbCardCount<=MAX_COUNT));
	if ((cbCardCount<1)||(cbCardCount>MAX_COUNT)) return 0;

	BYTE bCardIndex[MAX_INDEX];
	CopyMemory(bCardIndex,cbCardIndex,sizeof(bCardIndex));

	//拆分分析
	if (cbCardCount>=1)
	{
		for (BYTE i=0;i<MAX_INDEX;i++)
		{	
			//连牌判断
			if ((i<25)&&(bCardIndex[i]>0)&&(i<25)&&((i%9)<7))
			{
				BYTE bCount = 0;
				while( bCardIndex[i]>bCount && bCardIndex[i+1]>bCount && bCardIndex[i+2]>bCount )
					bCount++;
				if( bCount > 0 )
				{
					bCardIndex[i] -= bCount;
					bCardIndex[i+1] -= bCount;
					bCardIndex[i+2] -= bCount;
					cbCardCount -= 3*bCount;
				}
			}
			//同牌判断
			if (bCardIndex[i]>=2)
			{
				cbCardCount -= bCardIndex[i];
				bCardIndex[i] = 0;
			}
		}
	}
	return cbCardCount;
}

//分析废弃牌
bool CGameLogic::AnalyseTrushCard( BYTE cbCardIndex[MAX_INDEX],BYTE bTrushCardIndex[MAX_INDEX] )
{
	//复制
	CopyMemory(bTrushCardIndex,cbCardIndex,sizeof(BYTE)*MAX_INDEX);

	//计算数目
	BYTE cbCardCount=0;
	for (BYTE i=0;i<MAX_INDEX;i++) 
	{
		ASSERT( cbCardIndex[i]>=0 && cbCardIndex[i]<=4 );
		cbCardCount+=cbCardIndex[i];
	}

	//效验数目
	ASSERT((cbCardCount>=1)&&(cbCardCount<=MAX_COUNT));
	if ((cbCardCount<1)||(cbCardCount>MAX_COUNT)) return 0;

	//变量定义
	BYTE cbKindItemCount=0;
	tagKindItem KindItem[MAX_COUNT*4];
	ZeroMemory(KindItem,sizeof(KindItem));

	//拆分分析
	if (cbCardCount>=2)
	{
		for (BYTE i=0;i<MAX_INDEX;i++)
		{
			//同牌判断
			BYTE cbLessSameCount = 2;			//最小相同牌数
			if (cbCardIndex[i]>=cbLessSameCount)
			{
				ASSERT( cbKindItemCount<MAX_COUNT*4 );
				KindItem[cbKindItemCount].cbCardIndex[0]=i;
				KindItem[cbKindItemCount].cbCardIndex[1]=i;
				KindItem[cbKindItemCount].cbCardIndex[2]=i;
				KindItem[cbKindItemCount].cbWeaveKind=WIK_PENG;
				KindItem[cbKindItemCount++].cbCenterCard=SwitchToCardData(i);
			}
			
			//连牌判断
			if ((i<25)&&(cbCardIndex[i]>0)&&(i<25))
			{
				if( i%9 < 7 )
				{
					for (BYTE j=1;j<=cbCardIndex[i];j++)
					{
						if ((cbCardIndex[i+1]>=j)&&(cbCardIndex[i+2]>=j) ||
							(cbCardIndex[i+1]==j-1&&cbCardIndex[i+2]>=j) ||
							(cbCardIndex[i+1]>=j&&cbCardIndex[i+2]==j-1) 
							)
						{
							ASSERT( cbKindItemCount<MAX_COUNT*4 );
							KindItem[cbKindItemCount].cbCardIndex[0]=i;
							KindItem[cbKindItemCount].cbCardIndex[1]=i+1;
							KindItem[cbKindItemCount].cbCardIndex[2]=i+2;
							KindItem[cbKindItemCount].cbWeaveKind=WIK_LEFT;
							KindItem[cbKindItemCount++].cbCenterCard=SwitchToCardData(i);
						}
					}
				}
				else if( (i%9)==7 && cbCardIndex[i] > 0 && cbCardIndex[i+1] > 0 && cbCardIndex[i-1]==0 )
				{
					KindItem[cbKindItemCount].cbCardIndex[0]=i-1;
					KindItem[cbKindItemCount].cbCardIndex[1]=i;
					KindItem[cbKindItemCount].cbCardIndex[2]=i+1;
					KindItem[cbKindItemCount].cbWeaveKind=WIK_LEFT;
					KindItem[cbKindItemCount++].cbCenterCard=SwitchToCardData(i);
				}
			}
		}
	}
	//减去可组合的牌
	for( BYTE i = 0; i < cbKindItemCount; i++ )
	{
		if( bTrushCardIndex[KindItem[i].cbCardIndex[0]]>0 )
		{
			bTrushCardIndex[KindItem[i].cbCardIndex[0]]--;
			cbCardCount--;
		}
		if( bTrushCardIndex[KindItem[i].cbCardIndex[1]]>0 )
		{
			bTrushCardIndex[KindItem[i].cbCardIndex[1]]--;
			cbCardCount--;
		}
		if( bTrushCardIndex[KindItem[i].cbCardIndex[2]]>0 )
		{
			bTrushCardIndex[KindItem[i].cbCardIndex[2]]--;
			cbCardCount--;
		}
	}
	ASSERT( cbCardCount>=0 && cbCardCount<=14 );
	return cbCardCount>0;
}

//排序,根据牌值排序
bool CGameLogic::SortCardList( BYTE cbCardData[MAX_COUNT], BYTE cbCardCount )
{
	//数目过虑
	if (cbCardCount==0||cbCardCount>MAX_COUNT) return false;

	//排序操作
	bool bSorted=true;
	BYTE cbSwitchData=0,cbLast=cbCardCount-1;
	do
	{
		bSorted=true;
		for (BYTE i=0;i<cbLast;i++)
		{
			if (cbCardData[i]>cbCardData[i+1])
			{
				//设置标志
				bSorted=false;

				//扑克数据
				cbSwitchData=cbCardData[i];
				cbCardData[i]=cbCardData[i+1];
				cbCardData[i+1]=cbSwitchData;
			}	
		}
		cbLast--;
	} while(bSorted==false);

	return true;
}

//搜索听牌
bool CGameLogic::SearchTingCard(WORD wMeChairId,BYTE (&cbCardIndex)[GAME_PLAYER][MAX_INDEX],tagWeaveItem (&cWeaveItem)[GAME_PLAYER][4],BYTE cbItemCount[4],
								BYTE (&cbDiscardCard)[GAME_PLAYER][55],BYTE cbDiscardCount[GAME_PLAYER],tagTingCardResult &TingCardResult)
{
	//变量定义
	tagChiHuResult ChiHuResult;
	ZeroMemory(&ChiHuResult,sizeof(ChiHuResult));
	ZeroMemory(&TingCardResult,sizeof(TingCardResult));
	BYTE bAbandonCardCount=0;

	//构造扑克
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp,cbCardIndex[wMeChairId],sizeof(cbCardIndexTemp));

	//听牌分析

	for (BYTE i=0;i<MAX_INDEX;i++)
	{
		//空牌过滤
		if (cbCardIndexTemp[i]==0) continue;

		//听牌处理
		cbCardIndexTemp[i]--;

		//听牌判断
		bool bHuCard = false;
		BYTE cbHandTai=0,cbHandFeng=0,cbBankerUser=0;
		bAbandonCardCount = TingCardResult.bAbandonCount;
		for (BYTE j=0;j<MAX_INDEX;j++)
		{
			//胡牌分析
			BYTE cbCurrentCard=SwitchToCardData(j);
			BYTE cbHuCardKind=AnalyseChiHuCard(cbCardIndexTemp,cWeaveItem[wMeChairId],cbItemCount[wMeChairId],cbCurrentCard,0,ChiHuResult,
				cbBankerUser,true);

			//结果判断
			if (cbHuCardKind!=CHK_NULL)
			{
				bHuCard = true;
				TingCardResult.bTingCard[bAbandonCardCount][TingCardResult.bTingCardCount[bAbandonCardCount]++] = cbCurrentCard;
			}
		}
		if( bHuCard == true )
		{
			TingCardResult.bAbandonCard[TingCardResult.bAbandonCount++] = SwitchToCardData(i);
		}
		//还原处理
		cbCardIndexTemp[i]++;
	}

	//计算剩余牌
	for( BYTE n = 0; n < TingCardResult.bAbandonCount; n++ )
	{
		TingCardResult.bRemainCount[n] = TingCardResult.bTingCardCount[n]*4;

		for( BYTE i = 0; i < TingCardResult.bTingCardCount[n]; i++ )
		{
			BYTE bCardData = TingCardResult.bTingCard[n][i];
			//减自己牌
			if( cbCardIndex[wMeChairId][SwitchToCardIndex(bCardData)] > 0 )
			{
				TingCardResult.bRemainCount[n] -= cbCardIndex[wMeChairId][SwitchToCardIndex(bCardData)];
				ASSERT( TingCardResult.bRemainCount[n]>=0 );
			}

			for( BYTE j = 0; j < GAME_PLAYER; j++ )
			{
				//减组合牌
				for( BYTE k = 0; k < cbItemCount[j]; k++ )
				{
					if( cWeaveItem[j][k].cbCenterCard == bCardData )
					{
						TingCardResult.bRemainCount[n] -= cWeaveItem[j][k].cbWeaveKind==WIK_GANG?4:3;
						ASSERT( TingCardResult.bRemainCount[n]>=0 );
					}
				}
				//减丢弃牌
				for( k = 0; k < cbDiscardCount[j]; k++ )
				{
					if( bCardData == cbDiscardCard[j][k] )
					{
						TingCardResult.bRemainCount[n]--;
						ASSERT( TingCardResult.bRemainCount[n]>=0 );
					}
				}
			}
		}
	}
	return true;
}

//搜索出牌
bool CGameLogic::SearchOutCard( WORD wMeChairId,WORD wCurrentUser,BYTE (&cbCardIndex)[GAME_PLAYER][MAX_INDEX],tagWeaveItem (&cWeaveItem)[GAME_PLAYER][4],
							   BYTE cbItemCount[4],BYTE (&cbDiscardCard)[GAME_PLAYER][55],BYTE cbDiscardCount[GAME_PLAYER],BYTE cbActionMask,BYTE cbActionCard,
							   const BYTE cbEnjoinCard[MAX_COUNT],BYTE cbEnjoinCount,tagOutCardResult &OutCardResult )
{
	//初始化
	ZeroMemory(&OutCardResult,sizeof(OutCardResult));

	ASSERT( wMeChairId != INVALID_CHAIR );
	if( wMeChairId == INVALID_CHAIR ) return false;

	//判断胡
	if( (cbActionMask&WIK_CHI_HU) != WIK_NULL )
	{
		OutCardResult.cbOperateCode = WIK_CHI_HU;
		OutCardResult.cbOperateCard = cbActionCard;
		return true;
	}

	//转换牌数据
	BYTE byCard[MAX_COUNT],byCardCount = 0;
	for( BYTE i = 0; i < MAX_INDEX; i++ )
	{
		for( BYTE j = 0; j < cbCardIndex[wMeChairId][i]; j++ )
		{
			byCard[byCardCount++] = i;
		}
	}
	BYTE byEnjoinCardIndex[MAX_COUNT];
	ZeroMemory(byEnjoinCardIndex,sizeof(byEnjoinCardIndex));
	for( i = 0; i < cbEnjoinCount; i++ )
		byEnjoinCardIndex[i] = SwitchToCardIndex(cbEnjoinCard[i]);

	//判断杠
	if( (cbActionMask&WIK_GANG) != WIK_NULL )
	{
		if( cbActionCard == 0 || wMeChairId == wCurrentUser)
		{
			ASSERT( wMeChairId == wCurrentUser );
			tagGangCardResult GangCardResult;
			AnalyseGangCard(cbCardIndex[wMeChairId],cWeaveItem[wMeChairId],cbItemCount[wMeChairId],GangCardResult);
			ASSERT( GangCardResult.cbCardCount > 0 );
			cbActionCard = GangCardResult.cbCardData[0];
		}
		//计算原始分
		int nOrgScore;
		CAndroidAI AndroidAi;
		AndroidAi.SetCardData(byCard,byCardCount);
		AndroidAi.Think();
		nOrgScore = AndroidAi.GetMaxScore();
		//计算杠后得分
		int nScore;
		AndroidAi.SetCardData(byCard,byCardCount);
		AndroidAi.SetAction(WIK_GANG,SwitchToCardIndex(cbActionCard));
		AndroidAi.Think();
		nScore = AndroidAi.GetMaxScore();
		//如果杠后得分>=原始分,则杠
		if( nScore >= nOrgScore ||
			wMeChairId==wCurrentUser&&nScore>=nOrgScore-100)
		{
			OutCardResult.cbOperateCard = cbActionCard;
			OutCardResult.cbOperateCode = WIK_GANG;
			return true;
		}
		if( wCurrentUser == INVALID_CHAIR )
			return false;
	}

	//判断听
	if( wMeChairId==wCurrentUser )
	{
		if( AnalyseTingCard(cbCardIndex[wMeChairId],cWeaveItem[wMeChairId],cbItemCount[wMeChairId],0) )
		{
			tagTingCardResult TingCardResult;
			//搜索听牌
			if( SearchTingCard(wMeChairId,cbCardIndex,cWeaveItem,
				cbItemCount,cbDiscardCard,cbDiscardCount,TingCardResult) )
			{
				ASSERT( TingCardResult.bAbandonCount > 0 );
				BYTE bMostIndex = 0;
				BYTE bMostCount = TingCardResult.bRemainCount[0];
				//筛选听牌数最多的
				for( BYTE i = 1; i < TingCardResult.bAbandonCount; i++ )
				{
					if( TingCardResult.bRemainCount[i] > bMostCount )
					{
						bMostIndex = i;
						bMostCount = TingCardResult.bRemainCount[i];
					}
				}
				//有牌可听
				if( bMostCount > 0 )
				{
					OutCardResult.cbOperateCode = WIK_NULL;
					OutCardResult.cbOperateCard = TingCardResult.bAbandonCard[bMostIndex];
					ASSERT( cbCardIndex[wMeChairId][SwitchToCardIndex(OutCardResult.cbOperateCard)] > 0 );
					return true;
				}
				//听死牌
				else
				{
					//机器AI
					CAndroidAI AndroidAi;
					AndroidAi.SetCardData(byCard,byCardCount);
					AndroidAi.SetEnjoinOutCard(byEnjoinCardIndex,cbEnjoinCount);
					AndroidAi.Think();
					//从组合中拆牌
					BYTE byBadlyCard = 0xff;
					if( byCardCount <= 2 )
						byBadlyCard = AndroidAi.GetBadlyCard();
					else
						byBadlyCard = AndroidAi.GetBadlyIn2Card();
					if( 0xff != byBadlyCard )
					{
						OutCardResult.cbOperateCode = WIK_NULL;
						ASSERT( cbCardIndex[wMeChairId][byBadlyCard] > 0 );
						OutCardResult.cbOperateCard = SwitchToCardData(byBadlyCard);
						return true;
					}
					else
					{
						//从最佳三只组合中拆牌
						byBadlyCard = AndroidAi.GetBadlyIn3Card();
						if( 0xff != byBadlyCard )
						{
							OutCardResult.cbOperateCode = WIK_NULL;
							ASSERT( cbCardIndex[wMeChairId][byBadlyCard] > 0 );
							OutCardResult.cbOperateCard = SwitchToCardData(byBadlyCard);
							return true;
						}
					}
				}
			}
			ASSERT( FALSE );
			return false;
		}
	}

	//判断碰,吃
	if( (cbActionMask&(WIK_PENG|WIK_LEFT|WIK_CENTER|WIK_RIGHT)) != WIK_NULL )
	{
		ASSERT( cbActionCard != 0 );

		int nScore[5] = { 0,0,0,0,0 };
		BYTE cbCode[5] = { WIK_NULL,WIK_PENG,WIK_LEFT,WIK_CENTER,WIK_RIGHT };
		//分析最高分
		CAndroidAI ai;
		ai.SetCardData(byCard,byCardCount);
		ai.Think();
		nScore[0] = ai.GetMaxScore();
		if( cbActionMask & WIK_PENG )
		{
			ai.SetCardData(byCard,byCardCount);
			ai.SetAction(WIK_PENG,SwitchToCardIndex(cbActionCard));
			ai.Think();
			nScore[1] = ai.GetMaxScore();
		}
		if( cbActionMask & WIK_LEFT )
		{
			ai.SetCardData(byCard,byCardCount);
			ai.SetAction(WIK_LEFT,SwitchToCardIndex(cbActionCard));
			ai.Think();
			nScore[2] = ai.GetMaxScore();
		}
		if( cbActionMask & WIK_CENTER )
		{
			ai.SetCardData(byCard,byCardCount);
			ai.SetAction(WIK_CENTER,SwitchToCardIndex(cbActionCard));
			ai.Think();
			nScore[3] = ai.GetMaxScore();
		}
		if( cbActionMask & WIK_RIGHT )
		{
			ai.SetCardData(byCard,byCardCount);
			ai.SetAction(WIK_RIGHT,SwitchToCardIndex(cbActionCard));
			ai.Think();
			nScore[4] = ai.GetMaxScore();
		}
		//搜索最高分
		BYTE byIndex = 0;
		int nMaxScore = nScore[0];
		for( i = 1; i < 5; i++ )
		{
			if( nScore[i] > nMaxScore )
			{
				byIndex = i;
				nMaxScore = nScore[i];
			}
		}
		//如果操作得分>原始分
		if( byIndex > 0 )
		{
			OutCardResult.cbOperateCode = cbCode[byIndex];
			OutCardResult.cbOperateCard = cbActionCard;
			return true;
		}
		return false;
	}

	//效验当前出牌者为自己
	ASSERT( wMeChairId == wCurrentUser );
	if( wMeChairId != wCurrentUser )
		return false;
	//效验数目
#ifdef _DEBUG
	BYTE bCardCount = 0;
	for( BYTE i = 0; i < MAX_INDEX; i++ )
		bCardCount += cbCardIndex[wMeChairId][i];
	ASSERT( (bCardCount+1)%3 == 0 );
#endif

	//设置变量
	OutCardResult.cbOperateCode = WIK_NULL;
	//搜索废牌
	CAndroidAI AndroidAi;
	AndroidAi.SetCardData(byCard,byCardCount);
	AndroidAi.SetEnjoinOutCard(byEnjoinCardIndex,cbEnjoinCount);
	AndroidAi.Think();
	BYTE byBadlyIndex = AndroidAi.GetBadlyCard();
	if( byBadlyIndex != 0xff )
	{
		OutCardResult.cbOperateCard = SwitchToCardData(byBadlyIndex);
		return true;
	}
	//防止碰到禁止出的牌
	else
	{
		byBadlyIndex = AndroidAi.GetBadlyIn2Card();
		if( byBadlyIndex != 0xff )
		{
			OutCardResult.cbOperateCard = SwitchToCardData(byBadlyIndex);
			return true;
		}else
		{
			byBadlyIndex = AndroidAi.GetBadlyIn3Card();
			if( 0xff != byBadlyIndex )
			{
				OutCardResult.cbOperateCard = SwitchToCardData(byBadlyIndex);
				return true;
			}
		}
	}

	ASSERT( FALSE );
	return false;
}



//////////////////////////////////////////////////////////////////////////
