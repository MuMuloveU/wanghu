#include "StdAfx.h"
#include "GameLogic.h"

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
};

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameLogic::CGameLogic()
{
	FillMemory(m_cbKingCardIndex,sizeof(m_cbKingCardIndex),0xFF);
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
	if (cbCardCount>CountArray(cbTempCardData))	return false;

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
//设置精牌
void CGameLogic::SetKingCardIndex(BYTE cbKingCardIndex[2])
{
	//循环设置
	for (BYTE i=0;i<2;i++)
	{
		m_cbKingCardIndex[i]=cbKingCardIndex[i];
	}
}
//获取精牌
BYTE* CGameLogic::GetKingCardIndex(BYTE cbKingCardIndex[2])
{
	//初始赋值
	memset(cbKingCardIndex,0xFF,sizeof(BYTE)*2);

	//循环获取
	for (BYTE i=0;i<2;i++)
	{
		cbKingCardIndex[i]=m_cbKingCardIndex[i];
	}
	return cbKingCardIndex;
}
//判断正精
bool CGameLogic::IsKingCardData(BYTE cbCardData)
{
	return m_cbKingCardIndex[0]==SwitchToCardIndex(cbCardData);
}
//判断正精
bool CGameLogic::IsKingCardIndex(BYTE cbCardIndex)
{
	return m_cbKingCardIndex[0]==cbCardIndex;
}
//判断副精
bool CGameLogic::IsViceKingCardData(BYTE cbCardData)
{
	return m_cbKingCardIndex[1]==SwitchToCardIndex(cbCardData);
}
//判断副精
bool CGameLogic::IsViceKingCardIndex(BYTE cbCardIndex)
{
	return m_cbKingCardIndex[1]==cbCardIndex;
}
//有效判断
bool CGameLogic::IsValidCard(BYTE cbCardData)
{
	BYTE cbValue=(cbCardData&MASK_VALUE);
	BYTE cbColor=(cbCardData&MASK_COLOR)>>4;
	return (((cbValue>=1)&&(cbValue<=9)&&(cbColor<=2))||((cbValue>=1)&&(cbValue<=7)&&(cbColor==3)));
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
BYTE CGameLogic::GetWeaveCard(WORD wWeaveKind, BYTE cbCenterCard, BYTE cbCardBuffer[4])
{
	//组合扑克
	switch (wWeaveKind)
	{
	case WIK_LEFT:		//上牌操作
		{
			//设置变量
			cbCardBuffer[1]=cbCenterCard+1;
			cbCardBuffer[2]=cbCenterCard+2;
			cbCardBuffer[0]=cbCenterCard;

			return 3;
		}
	case WIK_RIGHT:		//上牌操作
		{
			//设置变量
			cbCardBuffer[1]=cbCenterCard-2;
			cbCardBuffer[2]=cbCenterCard-1;
			cbCardBuffer[0]=cbCenterCard;

			return 3;
		}
	case WIK_CENTER:	//上牌操作
		{
			//设置变量
			cbCardBuffer[1]=cbCenterCard-1;
			cbCardBuffer[0]=cbCenterCard;
			cbCardBuffer[2]=cbCenterCard+1;

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
	case WIK_DNBL:		//东南北左
		{
			//设置变量
			cbCardBuffer[1]=cbCenterCard+1;
			cbCardBuffer[2]=cbCenterCard+3;
			cbCardBuffer[0]=cbCenterCard;

			return 3;
		}
	case WIK_DNBC:		//东南北中
		{
			//设置变量
			cbCardBuffer[1]=cbCenterCard-1;
			cbCardBuffer[0]=cbCenterCard;
			cbCardBuffer[2]=cbCenterCard+2;

			return 3;
		}
	case WIK_DNBR:		//东南北右
		{
			//设置变量
			cbCardBuffer[1]=cbCenterCard-3;
			cbCardBuffer[2]=cbCenterCard-2;
			cbCardBuffer[0]=cbCenterCard;

			return 3;
		}
	case WIK_DXBL:		//东西北左
		{
			//设置变量
			cbCardBuffer[1]=cbCenterCard+2;
			cbCardBuffer[2]=cbCenterCard+3;
			cbCardBuffer[0]=cbCenterCard;

			return 3;
		}
	case WIK_DXBC:		//东西北中
		{
			//设置变量
			cbCardBuffer[1]=cbCenterCard-2;
			cbCardBuffer[0]=cbCenterCard;
			cbCardBuffer[2]=cbCenterCard+1;

			return 3;
		}
	case WIK_DXBR:		//东西北右
		{
			//设置变量
			cbCardBuffer[1]=cbCenterCard-3;
			cbCardBuffer[2]=cbCenterCard-1;
			cbCardBuffer[0]=cbCenterCard;

			return 3;
		}
	default:
		{
			ASSERT(FALSE);
		}
	}

	return 0;
}

//动作等级
BYTE CGameLogic::GetUserActionRank(WORD wUserAction)
{
	//胡牌等级
	if (wUserAction&WIK_CHI_HU) { return 4; }

	//杠牌等级
	if (wUserAction&WIK_GANG) { return 3; }

	//碰牌等级
	if (wUserAction&WIK_PENG) { return 2; }

	//上牌等级
	if (wUserAction&(WIK_RIGHT|WIK_CENTER|WIK_LEFT|WIK_DNBL|WIK_DNBC|WIK_DNBR|WIK_DXBL|WIK_DXBC|WIK_DXBR)) { return 1; }

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
WORD CGameLogic::EstimateEatCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard)
{

	//参数效验
	ASSERT(IsValidCard(cbCurrentCard));


	//变量定义
	BYTE cbExcursion[3]={0,1,2};
	WORD wItemKind[3]={WIK_LEFT,WIK_CENTER,WIK_RIGHT};

	//吃牌判断
	WORD wEatKind=0;
	BYTE cbFirstIndex=0;
	BYTE cbCurrentIndex=SwitchToCardIndex(cbCurrentCard);
	//中发白
	if(cbCurrentIndex>=31)
	{
		for (BYTE i=0;i<CountArray(wItemKind);i++)
		{
			BYTE cbValueIndex=cbCurrentIndex%9;
			if ((cbValueIndex>=cbExcursion[i])&&((cbValueIndex-cbExcursion[i])<=4))
			{
				//吃牌判断
				cbFirstIndex=cbCurrentIndex-cbExcursion[i];
				//过滤
				if(cbFirstIndex<31) continue;
				if ((cbCurrentIndex!=cbFirstIndex)&&(cbCardIndex[cbFirstIndex]==0))
					continue;
				if ((cbCurrentIndex!=(cbFirstIndex+1))&&(cbCardIndex[cbFirstIndex+1]==0))
					continue;
				if ((cbCurrentIndex!=(cbFirstIndex+2))&&(cbCardIndex[cbFirstIndex+2]==0))
					continue;

				//设置类型
				wEatKind|=wItemKind[i];
			}
		}

	}
	else if(cbCurrentIndex>=27)
	{
		for (BYTE i=0;i<CountArray(wItemKind);i++)
		{
			BYTE cbValueIndex=cbCurrentIndex%9;
			if ((cbValueIndex>=cbExcursion[i])&&((cbValueIndex-cbExcursion[i])<=1))
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
				wEatKind|=wItemKind[i];
			}
		}

	}
	else
	{
		for (BYTE i=0;i<CountArray(wItemKind);i++)
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
				wEatKind|=wItemKind[i];
			}
		}
	}
	//东南西北中发白的组合
	if(cbCurrentIndex>=27)
	{
		if(cbCurrentIndex==27)
		{
			if(cbCardIndex[28]>0&&cbCardIndex[30]>0)
				wEatKind |=WIK_DNBL;
			if(cbCardIndex[29]>0&&cbCardIndex[30]>0)
				wEatKind |=WIK_DXBL;
		}
		if(cbCurrentIndex==28)
		{
			if(cbCardIndex[27]>0&&cbCardIndex[30]>0)
				wEatKind |=WIK_DNBC;
		}
		if(cbCurrentIndex==29)
		{
			if(cbCardIndex[27]>0&&cbCardIndex[30]>0)
				wEatKind |=WIK_DXBC;
		}
		if(cbCurrentIndex==30)
		{
			if(cbCardIndex[27]>0&&cbCardIndex[28]>0)
				wEatKind |=WIK_DNBR;
			if(cbCardIndex[27]>0&&cbCardIndex[29]>0)
				wEatKind |=WIK_DXBR;
		}
	}

	return wEatKind;
}

//碰牌判断
WORD CGameLogic::EstimatePengCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard)
{

	//参数效验
	ASSERT(IsValidCard(cbCurrentCard));

	//碰牌判断
	return (cbCardIndex[SwitchToCardIndex(cbCurrentCard)]>=2)?WIK_PENG:WIK_NULL;
}

//杠牌判断
WORD CGameLogic::EstimateGangCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard)
{

	//参数效验
	ASSERT(IsValidCard(cbCurrentCard));

	//杠牌判断
	return (cbCardIndex[SwitchToCardIndex(cbCurrentCard)]==3)?WIK_GANG:WIK_NULL;
}

//杠牌分析
WORD CGameLogic::AnalyseGangCard(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbWeaveCount, tagGangCardResult & GangCardResult)
{
	//设置变量
	WORD wActionMask=WIK_NULL;
	ZeroMemory(&GangCardResult,sizeof(GangCardResult));

	//手上杠牌 
	for (BYTE i=0;i<MAX_INDEX-1;i++)
	{
		if (cbCardIndex[i]==4)
		{
			wActionMask|=WIK_GANG;
			GangCardResult.cbCardData[GangCardResult.cbCardCount++]=SwitchToCardData(i);
		}
	}

	//组合杠牌
	for (BYTE i=0;i<cbWeaveCount;i++)
	{
		if (WeaveItem[i].wWeaveKind==WIK_PENG)
		{
			if (cbCardIndex[SwitchToCardIndex(WeaveItem[i].cbCenterCard)]==1)
			{
				wActionMask|=WIK_GANG;
				GangCardResult.cbCardData[GangCardResult.cbCardCount++]=WeaveItem[i].cbCenterCard;
			}
		}
	}

	return wActionMask;
}
//分析扑克
bool CGameLogic::AnalyseCard(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbWeaveCount, CAnalyseItemArray & AnalyseItemArray,BYTE cbCurrentCard,bool bZimo)
{
	//计算数目
	BYTE cbCardCount=0;
	for (BYTE i=0;i<MAX_INDEX;i++) 
		cbCardCount+=cbCardIndex[i];

	//拷贝数据
	BYTE cbTempCardIndex[MAX_INDEX];
	CopyMemory(cbTempCardIndex,cbCardIndex,sizeof(cbTempCardIndex));

	//效验数目
	ASSERT((cbCardCount>=2)&&(cbCardCount<=MAX_COUNT)&&((cbCardCount-2)%3==0));
	if ((cbCardCount<2)||(cbCardCount>MAX_COUNT)||((cbCardCount-2)%3!=0))	return false;

	//变量定义
	BYTE cbKindItemCount=0;
	tagKindItem KindItem[MAX_INDEX-2];
	ZeroMemory(KindItem,sizeof(KindItem));

	//需求判断
	BYTE cbLessKindItem=(cbCardCount-2)/3;
	ASSERT((cbLessKindItem+cbWeaveCount)==4);

	//单吊判断
	if (cbLessKindItem==0)
	{
		//效验参数
		ASSERT((cbCardCount==2)&&(cbWeaveCount==4));

		//正精
		if(cbCardIndex[m_cbKingCardIndex[0]] == 2 )
		{
			//变量定义
			tagAnalyseItem AnalyseItem;
			ZeroMemory(&AnalyseItem,sizeof(AnalyseItem));

			//精牌替换
			AnalyseItem.cbEyeKingReplace=0;

			//设置结果
			for (BYTE j=0;j<cbWeaveCount;j++)
			{
				AnalyseItem.wWeaveKind[j]=WeaveItem[j].wWeaveKind;
				AnalyseItem.cbCenterCard[j]=WeaveItem[j].cbCenterCard;
			}
			//精牌替换 
			AnalyseItem.cbWeaveKingReplace=0;

			AnalyseItem.cbCardEye=SwitchToCardData(m_cbKingCardIndex[0]);

			//插入结果
			AnalyseItemArray.Add(AnalyseItem);
			return true;

		}

		//副精
		if( cbCardIndex[m_cbKingCardIndex[1]]==2)
		{
			//变量定义
			tagAnalyseItem AnalyseItem;
			ZeroMemory(&AnalyseItem,sizeof(AnalyseItem));

			//精牌替换
			AnalyseItem.cbEyeKingReplace=0;

			//设置结果
			for (BYTE j=0;j<cbWeaveCount;j++)
			{
				AnalyseItem.wWeaveKind[j]=WeaveItem[j].wWeaveKind;
				AnalyseItem.cbCenterCard[j]=WeaveItem[j].cbCenterCard;
			}
			//精牌替换 
			AnalyseItem.cbWeaveKingReplace=0;

			AnalyseItem.cbCardEye=SwitchToCardData(m_cbKingCardIndex[1]);

			//插入结果
			AnalyseItemArray.Add(AnalyseItem);
			return true;

		}
		//正精+副精
		if (cbCardIndex[m_cbKingCardIndex[0]]==1&& cbCardIndex[m_cbKingCardIndex[1]]==1) 
		{
			//变量定义
			tagAnalyseItem AnalyseItem;
			ZeroMemory(&AnalyseItem,sizeof(AnalyseItem));

			//精牌替换
			AnalyseItem.cbEyeKingReplace=1;

			//设置结果
			for (BYTE j=0;j<cbWeaveCount;j++)
			{
				AnalyseItem.wWeaveKind[j]=WeaveItem[j].wWeaveKind;
				AnalyseItem.cbCenterCard[j]=WeaveItem[j].cbCenterCard;
			}
			//精牌替换
			AnalyseItem.cbWeaveKingReplace=0;
			
			AnalyseItem.cbCardEye=SwitchToCardData(m_cbKingCardIndex[0]);

			//插入结果
			AnalyseItemArray.Add(AnalyseItem);
			return true;
		}

		
		//自摸处理
		//精牌+单张
		if ((cbCardIndex[m_cbKingCardIndex[0]]  + cbCardIndex[m_cbKingCardIndex[1]])==1 && bZimo==true)
		{
			//变量定义
			tagAnalyseItem AnalyseItem;
			ZeroMemory(&AnalyseItem,sizeof(AnalyseItem));

			//精牌替换
			AnalyseItem.cbEyeKingReplace=1;

			//设置结果
			for (BYTE j=0;j<cbWeaveCount;j++)
			{
				AnalyseItem.wWeaveKind[j]=WeaveItem[j].wWeaveKind;
				AnalyseItem.cbCenterCard[j]=WeaveItem[j].cbCenterCard;
			}
			//精牌替换
			AnalyseItem.cbWeaveKingReplace=0;

			for (BYTE i=0;i<MAX_INDEX;i++)
			{
				// 正精过滤
				if(IsKingCardIndex(i)) continue;

				//副精过滤
				if(IsViceKingCardIndex(i)) continue;

				//找到独张
				if (cbCardIndex[i]==1)
				{
					AnalyseItem.cbCardEye=SwitchToCardData(i);
				}
			}
			//插入结果
			AnalyseItemArray.Add(AnalyseItem);
			return true;
		}

		//正精单吊 放炮胡
		if ((cbCardIndex[m_cbKingCardIndex[0]])==1 && bZimo==false &&SwitchToCardIndex(cbCurrentCard)!=m_cbKingCardIndex[0])
		{
			//变量定义
			tagAnalyseItem AnalyseItem;
			ZeroMemory(&AnalyseItem,sizeof(AnalyseItem));

			//精牌替换
			AnalyseItem.cbEyeKingReplace=1;

			//设置结果
			for (BYTE j=0;j<cbWeaveCount;j++)
			{
				AnalyseItem.wWeaveKind[j]=WeaveItem[j].wWeaveKind;
				AnalyseItem.cbCenterCard[j]=WeaveItem[j].cbCenterCard;
			}
			//精牌替换
			AnalyseItem.cbWeaveKingReplace=false;

			for (BYTE i=0;i<MAX_INDEX;i++)
			{
				// 正精过滤
				if(IsKingCardIndex(i)) continue;


				//找到独张
				if (cbCardIndex[i]==1)
				{
					AnalyseItem.cbCardEye=SwitchToCardData(i);
				}
			}
			//插入结果
			AnalyseItemArray.Add(AnalyseItem);
			return true;
		}

		
		//副精单吊 放炮胡
		if ((cbCardIndex[m_cbKingCardIndex[1]])==1 && bZimo==false &&SwitchToCardIndex(cbCurrentCard)!=m_cbKingCardIndex[1])
		{
			//变量定义
			tagAnalyseItem AnalyseItem;
			ZeroMemory(&AnalyseItem,sizeof(AnalyseItem));

			//精牌替换
			AnalyseItem.cbEyeKingReplace=1;

			//设置结果
			for (BYTE j=0;j<cbWeaveCount;j++)
			{
				AnalyseItem.wWeaveKind[j]=WeaveItem[j].wWeaveKind;
				AnalyseItem.cbCenterCard[j]=WeaveItem[j].cbCenterCard;
			}
			//精牌替换
			AnalyseItem.cbWeaveKingReplace=false;

			for (BYTE i=0;i<MAX_INDEX;i++)
			{
				// 正精过滤
				if(IsViceKingCardIndex(i)) continue;


				//找到独张
				if (cbCardIndex[i]==1)
				{
					AnalyseItem.cbCardEye=SwitchToCardData(i);
				}
			}
			//插入结果
			AnalyseItemArray.Add(AnalyseItem);
			return true;
		}


		//牌眼判断
		for (BYTE i=0;i<MAX_INDEX;i++)
		{

			//这里过不过滤都无所谓了
			// 正精过滤
			if(IsKingCardIndex(i)) continue;

			//副精过滤
			if(IsViceKingCardIndex(i)) continue;

			if (cbCardIndex[i]==2)
			{
				//变量定义
				tagAnalyseItem AnalyseItem;
				ZeroMemory(&AnalyseItem,sizeof(AnalyseItem));

				//设置结果
				for (BYTE j=0;j<cbWeaveCount;j++)
				{
					AnalyseItem.wWeaveKind[j]=WeaveItem[j].wWeaveKind;
					AnalyseItem.cbCenterCard[j]=WeaveItem[j].cbCenterCard;
				}
				AnalyseItem.cbWeaveKingReplace=0;
				AnalyseItem.cbEyeKingReplace=0;

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
		for (BYTE i=0;i<MAX_INDEX;i++)
		{
			//同牌判断
			if (cbCardIndex[i]>=3)
			{
				ASSERT( cbKindItemCount < CountArray(KindItem) );
				KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
				KindItem[cbKindItemCount].cbCardIndex[0]=i;
				KindItem[cbKindItemCount].cbCardIndex[1]=i;
				KindItem[cbKindItemCount].cbCardIndex[2]=i;
				KindItem[cbKindItemCount++].wWeaveKind=WIK_PENG;
			}

			//连牌判断
			if ((i<(MAX_INDEX-2-7))&&(cbCardIndex[i]>0)&&((i%9)<7))
			{
				for (BYTE j=1;j<=cbCardIndex[i];j++)
				{
					if ((cbCardIndex[i+1]>=j)&&(cbCardIndex[i+2]>=j))
					{
						ASSERT( cbKindItemCount < CountArray(KindItem) );
						KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
						KindItem[cbKindItemCount].cbCardIndex[0]=i;
						KindItem[cbKindItemCount].cbCardIndex[1]=i+1;
						KindItem[cbKindItemCount].cbCardIndex[2]=i+2;
						KindItem[cbKindItemCount++].wWeaveKind=WIK_LEFT;
					}
				}
			}


		}
		//东南西北
		for (BYTE i=MAX_INDEX-7;i<MAX_INDEX-5;i++)
		{
			//变量定义
			BYTE cbCardIndexTemp[MAX_INDEX];

			//设置变量
			CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));

			//东南西 南西北
			for (BYTE j=1;j<=cbCardIndex[i]&&cbCardIndex[i]>0;j++)
			{
				if ((cbCardIndex[i+1]>=j)&&(cbCardIndex[i+2]>=j))
				{
					ASSERT( cbKindItemCount < CountArray(KindItem) );
					KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
					KindItem[cbKindItemCount].cbCardIndex[0]=i;
					KindItem[cbKindItemCount].cbCardIndex[1]=i+1;
					KindItem[cbKindItemCount].cbCardIndex[2]=i+2;
					KindItem[cbKindItemCount++].wWeaveKind=WIK_LEFT;
				}
			}

			//东南北
			if(i==MAX_INDEX-7)
			{
				for (BYTE j=1;j<=cbCardIndex[i]&&cbCardIndex[i]>0;j++)
				{
					if ((cbCardIndex[i+1]>=j)&&(cbCardIndex[i+3]>=j))
					{
						ASSERT( cbKindItemCount < CountArray(KindItem) );
						KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
						KindItem[cbKindItemCount].cbCardIndex[0]=i;
						KindItem[cbKindItemCount].cbCardIndex[1]=i+1;
						KindItem[cbKindItemCount].cbCardIndex[2]=i+3;
						KindItem[cbKindItemCount++].wWeaveKind=WIK_DNBL;
					}
				}

			}
			//东西北
			if(i==MAX_INDEX-7)
			{
				for (BYTE j=1;j<=cbCardIndex[i]&&cbCardIndex[i]>0;j++)
				{
					if ((cbCardIndex[i+2]>=j)&&(cbCardIndex[i+3]>=j))
					{
						ASSERT( cbKindItemCount < CountArray(KindItem) );
						KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
						KindItem[cbKindItemCount].cbCardIndex[0]=i;
						KindItem[cbKindItemCount].cbCardIndex[1]=i+2;
						KindItem[cbKindItemCount].cbCardIndex[2]=i+3;
						KindItem[cbKindItemCount++].wWeaveKind=WIK_DXBL;
					}
				}
			}


		}	
		//中发白
		for (BYTE i=MAX_INDEX-3;i<MAX_INDEX;i++)
		{

			//连牌判断
			if ((i<(MAX_INDEX-2))&&(cbCardIndex[i]>0)&&((i%9)<7))
			{
				for (BYTE j=1;j<=cbCardIndex[i];j++)
				{
					if ((cbCardIndex[i+1]>=j)&&(cbCardIndex[i+2]>=j))
					{
						KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
						KindItem[cbKindItemCount].cbCardIndex[0]=i;
						KindItem[cbKindItemCount].cbCardIndex[1]=i+1;
						KindItem[cbKindItemCount].cbCardIndex[2]=i+2;
						KindItem[cbKindItemCount++].wWeaveKind=WIK_LEFT;
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
		BYTE cbIndex[MAX_WEAVE]={0,1,2,3};
		tagKindItem * pKindItem[MAX_WEAVE];
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

			//德国判断
			if (bEnoughCard==true)
			{
				//牌眼判断
				BYTE cbCardEye=0;

				for (BYTE i=0;i<MAX_INDEX;i++)
				{
					if (cbCardIndexTemp[i]==2)
					{
						//眼牌数据
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
						AnalyseItem.wWeaveKind[i]=WeaveItem[i].wWeaveKind;
						AnalyseItem.cbCenterCard[i]=WeaveItem[i].cbCenterCard;
					}

					//设置牌型
					for (BYTE i=0;i<cbLessKindItem;i++) 
					{
						AnalyseItem.wWeaveKind[i+cbWeaveCount]=pKindItem[i]->wWeaveKind;
						AnalyseItem.cbCenterCard[i+cbWeaveCount]=pKindItem[i]->cbCenterCard;
					}
					//精牌替换
					AnalyseItem.cbWeaveKingReplace=0;

					//设置牌眼
					AnalyseItem.cbCardEye=cbCardEye;

					//精牌替换
					AnalyseItem.cbEyeKingReplace=0;

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

	
	//硬胡分析
	if (AnalyseItemArray.GetCount()>0)
		return true;
	
	//硬胡清空
	AnalyseItemArray.RemoveAll();

	//变量定义
	cbKindItemCount=0;
	ZeroMemory(KindItem,sizeof(KindItem));
	
	if(bZimo)
	{

		//正副精数目
		BYTE cbKingCount=cbCardIndex[m_cbKingCardIndex[0]];
		BYTE cbViceKingCount=cbCardIndex[m_cbKingCardIndex[1]];

		//精牌清空
		cbTempCardIndex[m_cbKingCardIndex[0]]=0;
		cbTempCardIndex[m_cbKingCardIndex[1]]=0;

		//精牌总数
		BYTE cbTotalKingCount=cbKingCount+cbViceKingCount;
		if(cbTotalKingCount==0) return false;
		//拆分分析
		//拆分分析
		if (cbCardCount>=3)
		{
			//3个财神先凑刻
			if(cbTotalKingCount>=3)
			{
				if(cbKingCount>0&&cbViceKingCount>0)
				{
					ASSERT( cbKindItemCount < CountArray(KindItem) );
					KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(m_cbKingCardIndex[0]);
					KindItem[cbKindItemCount].cbCardIndex[0]=m_cbKingCardIndex[0];
					KindItem[cbKindItemCount].cbCardIndex[1]=m_cbKingCardIndex[0];
					KindItem[cbKindItemCount].cbCardIndex[2]=m_cbKingCardIndex[0];
					KindItem[cbKindItemCount++].wWeaveKind=WIK_PENG;
				}
			}
			for (BYTE i=0;i<MAX_INDEX;i++)
			{
				//同牌判断
				if (cbTempCardIndex[i]>=3)
				{
					ASSERT( cbKindItemCount < CountArray(KindItem) );
					KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
					KindItem[cbKindItemCount].cbCardIndex[0]=i;
					KindItem[cbKindItemCount].cbCardIndex[1]=i;
					KindItem[cbKindItemCount].cbCardIndex[2]=i;
					KindItem[cbKindItemCount++].wWeaveKind=WIK_PENG;
				}
				else if(cbTempCardIndex[i]==1)
				{
					//过滤精牌
					if(IsViceKingCardIndex(i)==false &&IsViceKingCardIndex(i)==false)
					{
						if(cbTotalKingCount>=2)
						{
							ASSERT( cbKindItemCount < CountArray(KindItem) );
							KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
							KindItem[cbKindItemCount].cbCardIndex[0]=i;
							KindItem[cbKindItemCount].cbCardIndex[1]=i;
							KindItem[cbKindItemCount].cbCardIndex[2]=i;
							KindItem[cbKindItemCount++].wWeaveKind=WIK_PENG;
						}
					}

				}
				else if(cbTempCardIndex[i]==2)
				{
					//过滤精牌
					if(IsViceKingCardIndex(i)==false &&IsViceKingCardIndex(i)==false)
					{
						if(cbTotalKingCount>=1)
						{
							ASSERT( cbKindItemCount < CountArray(KindItem) );
							KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
							KindItem[cbKindItemCount].cbCardIndex[0]=i;
							KindItem[cbKindItemCount].cbCardIndex[1]=i;
							KindItem[cbKindItemCount].cbCardIndex[2]=i;
							KindItem[cbKindItemCount++].wWeaveKind=WIK_PENG;
						}
					}

				}
				else
				{
					//什么也不执行
				}


				//连牌判断
				if ((i<(MAX_INDEX-2-7))&&((i%9)<7))
				{

					if (cbTempCardIndex[i]+cbTempCardIndex[i+1]+cbTempCardIndex[i+2]+cbTotalKingCount>=3)
					{
						BYTE cbIndex[3]={cbTempCardIndex[i],cbTempCardIndex[i+1],cbTempCardIndex[i+2]};
						INT cbTempTotalKingCount=cbTotalKingCount;

						while (cbIndex[0]+cbIndex[1]+cbIndex[2]+cbTotalKingCount>=3)
						{
							//过滤3精
							if(cbIndex[0]+cbIndex[1]+cbIndex[2]==0) break;

							BYTE cbWeaveKingReplace=0;

							//精牌组合
							for( BYTE j = 0; j < CountArray(cbIndex); j++ )
							{
								if( cbIndex[j] > 0 ) 
								{
									cbIndex[j]--;
								}
								else 
								{
									//精配
									cbWeaveKingReplace=1;
									cbTempTotalKingCount--;
								}
							}
							if( cbTempTotalKingCount >= 0 )
							{
								KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
								KindItem[cbKindItemCount].cbCardIndex[0]=i;
								KindItem[cbKindItemCount].cbCardIndex[1]=i+1;
								KindItem[cbKindItemCount].cbCardIndex[2]=i+2;
								KindItem[cbKindItemCount++].wWeaveKind=WIK_LEFT;
							}
							else
								break;

						}
					}

				}
			}
			//东南西北
			for (BYTE i=MAX_INDEX-7;i<MAX_INDEX-5;i++)
			{
				//变量定义
				BYTE cbCardIndexTemp[MAX_INDEX];

				//设置变量
				CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));

				//东南西 南西北
				if (cbTempCardIndex[i]+cbTempCardIndex[i+1]+cbTempCardIndex[i+2]+cbTotalKingCount>=3)
				{
					BYTE cbIndex[3]={cbTempCardIndex[i],cbTempCardIndex[i+1],cbTempCardIndex[i+2]};
					INT cbTempTotalKingCount=cbTotalKingCount;

					while (cbIndex[0]+cbIndex[1]+cbIndex[2]+cbTotalKingCount>=3)
					{
						//过滤3精
						if(cbIndex[0]+cbIndex[1]+cbIndex[2]==0) break;

						BYTE cbWeaveKingReplace=0;

						//精牌组合
						for( BYTE j = 0; j < CountArray(cbIndex); j++ )
						{
							if( cbIndex[j] > 0 ) 
							{
								cbIndex[j]--;
							}
							else 
							{
								//精配
								cbWeaveKingReplace=1;
								cbTempTotalKingCount--;
							}
						}
						if( cbTempTotalKingCount >= 0 )
						{
							KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
							KindItem[cbKindItemCount].cbCardIndex[0]=i;
							KindItem[cbKindItemCount].cbCardIndex[1]=i+1;
							KindItem[cbKindItemCount].cbCardIndex[2]=i+2;
							KindItem[cbKindItemCount++].wWeaveKind=WIK_LEFT;
						}
						else
							break;

					}
				}

				//东南北
				if(i==MAX_INDEX-7)
				{
					if (cbTempCardIndex[i]+cbTempCardIndex[i+1]+cbTempCardIndex[i+3]+cbTotalKingCount>=3)
					{
						BYTE cbIndex[3]={cbCardIndex[i],cbCardIndex[i+1],cbCardIndex[i+3]};
						INT cbTempTotalKingCount=cbTotalKingCount;

						while (cbIndex[0]+cbIndex[1]+cbIndex[2]+cbTotalKingCount>=3)
						{
							//过滤3精
							if(cbIndex[0]+cbIndex[1]+cbIndex[2]==0) break;

							BYTE cbWeaveKingReplace=0;

							//精牌组合
							for( BYTE j = 0; j < CountArray(cbIndex); j++ )
							{
								if( cbIndex[j] > 0 ) 
								{
									cbIndex[j]--;
								}
								else 
								{
									//精配
									cbWeaveKingReplace=1;
									cbTempTotalKingCount--;
								}
							}
							if( cbTempTotalKingCount >= 0 )
							{
								KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
								KindItem[cbKindItemCount].cbCardIndex[0]=i;
								KindItem[cbKindItemCount].cbCardIndex[1]=i+1;
								KindItem[cbKindItemCount].cbCardIndex[2]=i+3;
								KindItem[cbKindItemCount++].wWeaveKind=WIK_DNBL;
							}
							else
								break;

						}
					}

				}
				//东西北
				if(i==MAX_INDEX-7)
				{
					if (cbTempCardIndex[i]+cbTempCardIndex[i+2]+cbTempCardIndex[i+3]+cbTotalKingCount>=3)
					{
						BYTE cbIndex[3]={cbTempCardIndex[i],cbTempCardIndex[i+2],cbTempCardIndex[i+3]};
						INT cbTempTotalKingCount=cbTotalKingCount;

						while (cbIndex[0]+cbIndex[1]+cbIndex[2]+cbTotalKingCount>=3)
						{

							//过滤3精
							if(cbIndex[0]+cbIndex[1]+cbIndex[2]==0) break;

							BYTE cbWeaveKingReplace=0;

							//精牌组合
							for( BYTE j = 0; j < CountArray(cbIndex); j++ )
							{
								if( cbIndex[j] > 0 ) 
								{
									cbIndex[j]--;
								}
								else 
								{
									cbWeaveKingReplace=1;
									cbTempTotalKingCount--;

								}
							}
							if( cbTempTotalKingCount >= 0 )
							{
								KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
								KindItem[cbKindItemCount].cbCardIndex[0]=i;
								KindItem[cbKindItemCount].cbCardIndex[1]=i+2;
								KindItem[cbKindItemCount].cbCardIndex[2]=i+3;
								KindItem[cbKindItemCount++].wWeaveKind=WIK_DXBL;
							}
							else
								break;

						}
					}

				}


			}	



			for (BYTE i=MAX_INDEX-3;i<MAX_INDEX;i++)		//中发白
			{
				if ( i==MAX_INDEX-3)
				{
					if (cbTempCardIndex[i]+cbTempCardIndex[i+1]+cbTempCardIndex[i+2]+cbTotalKingCount>=3)
					{
						BYTE cbIndex[3]={cbTempCardIndex[i],cbTempCardIndex[i+1],cbTempCardIndex[i+2]};
						INT cbTempTotalKingCount=cbTotalKingCount;

						while (cbIndex[0]+cbIndex[1]+cbIndex[2]+cbTotalKingCount>=3)
						{
							//过滤3精
							if(cbIndex[0]+cbIndex[1]+cbIndex[2]==0) break;
							BYTE cbWeaveKingReplace=0;

							//精牌组合
							for( BYTE j = 0; j < CountArray(cbIndex); j++ )
							{
								if( cbIndex[j] > 0 ) 
								{
									cbIndex[j]--;
								}
								else 
								{
									//精吊
									cbWeaveKingReplace=1;
									cbTempTotalKingCount--;
								}
							}
							if( cbTempTotalKingCount >= 0 )
							{
								KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
								KindItem[cbKindItemCount].cbCardIndex[0]=i;
								KindItem[cbKindItemCount].cbCardIndex[1]=i+1;
								KindItem[cbKindItemCount].cbCardIndex[2]=i+2;
								KindItem[cbKindItemCount++].wWeaveKind=WIK_LEFT;
							}
							else
								break;

						}
					}
				}

			}

		}


		{
			//3个财神先凑刻
			if(cbTotalKingCount>=3)
			{
				if(cbKingCount>0&&cbViceKingCount>0)
				{
					ASSERT( cbKindItemCount < CountArray(KindItem) );
					KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(m_cbKingCardIndex[0]);
					KindItem[cbKindItemCount].cbCardIndex[0]=m_cbKingCardIndex[0];
					KindItem[cbKindItemCount].cbCardIndex[1]=m_cbKingCardIndex[0];
					KindItem[cbKindItemCount].cbCardIndex[2]=m_cbKingCardIndex[0];
					KindItem[cbKindItemCount++].wWeaveKind=WIK_PENG;
				}
			}
			for (BYTE i=0;i<MAX_INDEX;i++)
			{
				//同牌判断
				if (cbCardIndex[i]>=3)
				{
					ASSERT( cbKindItemCount < CountArray(KindItem) );
					KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
					KindItem[cbKindItemCount].cbCardIndex[0]=i;
					KindItem[cbKindItemCount].cbCardIndex[1]=i;
					KindItem[cbKindItemCount].cbCardIndex[2]=i;
					KindItem[cbKindItemCount++].wWeaveKind=WIK_PENG;
				}
				else if(cbCardIndex[i]==1)
				{
					//过滤精牌
					if(IsViceKingCardIndex(i)==false &&IsViceKingCardIndex(i)==false)
					{
						if(cbTotalKingCount>=2)
						{
							ASSERT( cbKindItemCount < CountArray(KindItem) );
							KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
							KindItem[cbKindItemCount].cbCardIndex[0]=i;
							KindItem[cbKindItemCount].cbCardIndex[1]=i;
							KindItem[cbKindItemCount].cbCardIndex[2]=i;
							KindItem[cbKindItemCount++].wWeaveKind=WIK_PENG;
						}
					}

				}
				else if(cbCardIndex[i]==2)
				{
					//过滤精牌
					if(IsViceKingCardIndex(i)==false &&IsViceKingCardIndex(i)==false)
					{
						if(cbTotalKingCount>=1)
						{
							ASSERT( cbKindItemCount < CountArray(KindItem) );
							KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
							KindItem[cbKindItemCount].cbCardIndex[0]=i;
							KindItem[cbKindItemCount].cbCardIndex[1]=i;
							KindItem[cbKindItemCount].cbCardIndex[2]=i;
							KindItem[cbKindItemCount++].wWeaveKind=WIK_PENG;
						}
					}

				}
				else
				{
					//什么也不执行
				}


				//连牌判断
				if ((i<(MAX_INDEX-2-7))&&((i%9)<7))
				{

					if (cbCardIndex[i]+cbCardIndex[i+1]+cbCardIndex[i+2]+cbTotalKingCount>=3)
					{
						BYTE cbIndex[3]={cbCardIndex[i],cbCardIndex[i+1],cbCardIndex[i+2]};
						INT cbTempTotalKingCount=cbTotalKingCount;

						while (cbIndex[0]+cbIndex[1]+cbIndex[2]+cbTotalKingCount>=3)
						{
							//过滤3精
							if(cbIndex[0]+cbIndex[1]+cbIndex[2]==0) break;

							//精牌组合
							for( BYTE j = 0; j < CountArray(cbIndex); j++ )
							{
								if( cbIndex[j] > 0 ) 
								{
									cbIndex[j]--;
								}
								else 
								{
									cbTempTotalKingCount--;
								}
							}
							if( cbTempTotalKingCount >= 0 )
							{
								KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
								KindItem[cbKindItemCount].cbCardIndex[0]=i;
								KindItem[cbKindItemCount].cbCardIndex[1]=i+1;
								KindItem[cbKindItemCount].cbCardIndex[2]=i+2;
								KindItem[cbKindItemCount++].wWeaveKind=WIK_LEFT;
							}
							else
								break;

						}
					}

				}
			}
			//东南西北
			for (BYTE i=MAX_INDEX-7;i<MAX_INDEX-5;i++)
			{
				//变量定义
				BYTE cbCardIndexTemp[MAX_INDEX];

				//设置变量
				CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));

				//东南西 南西北
				if (cbCardIndex[i]+cbCardIndex[i+1]+cbCardIndex[i+2]+cbTotalKingCount>=3)
				{
					BYTE cbIndex[3]={cbCardIndex[i],cbCardIndex[i+1],cbCardIndex[i+2]};
					INT cbTempTotalKingCount=cbTotalKingCount;

					while (cbIndex[0]+cbIndex[1]+cbIndex[2]+cbTotalKingCount>=3)
					{
						//过滤3精
						if(cbIndex[0]+cbIndex[1]+cbIndex[2]==0) break;

						//精牌组合
						for( BYTE j = 0; j < CountArray(cbIndex); j++ )
						{
							if( cbIndex[j] > 0 ) 
							{
								cbIndex[j]--;
							}
							else 
							{
								//精配
								cbTempTotalKingCount--;
							}
						}
						if( cbTempTotalKingCount >= 0 )
						{
							KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
							KindItem[cbKindItemCount].cbCardIndex[0]=i;
							KindItem[cbKindItemCount].cbCardIndex[1]=i+1;
							KindItem[cbKindItemCount].cbCardIndex[2]=i+2;
							KindItem[cbKindItemCount++].wWeaveKind=WIK_LEFT;
						}
						else
							break;

					}
				}

				//东南北
				if(i==MAX_INDEX-7)
				{
					if (cbCardIndex[i]+cbCardIndex[i+1]+cbCardIndex[i+3]+cbTotalKingCount>=3)
					{
						BYTE cbIndex[3]={cbCardIndex[i],cbCardIndex[i+1],cbCardIndex[i+3]};
						INT cbTempTotalKingCount=cbTotalKingCount;

						while (cbIndex[0]+cbIndex[1]+cbIndex[2]+cbTotalKingCount>=3)
						{
							//过滤3精
							if(cbIndex[0]+cbIndex[1]+cbIndex[2]==0) break;


							//精牌组合
							for( BYTE j = 0; j < CountArray(cbIndex); j++ )
							{
								if( cbIndex[j] > 0 ) 
								{
									cbIndex[j]--;
								}
								else 
								{
									//精配
									cbTempTotalKingCount--;
								}
							}
							if( cbTempTotalKingCount >= 0 )
							{
								KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
								KindItem[cbKindItemCount].cbCardIndex[0]=i;
								KindItem[cbKindItemCount].cbCardIndex[1]=i+1;
								KindItem[cbKindItemCount].cbCardIndex[2]=i+3;
								KindItem[cbKindItemCount++].wWeaveKind=WIK_DNBL;
							}
							else
								break;

						}
					}

				}
				//东西北
				if(i==MAX_INDEX-7)
				{
					if (cbCardIndex[i]+cbCardIndex[i+2]+cbCardIndex[i+3]+cbTotalKingCount>=3)
					{
						BYTE cbIndex[3]={cbCardIndex[i],cbCardIndex[i+2],cbCardIndex[i+3]};
						INT cbTempTotalKingCount=cbTotalKingCount;

						while (cbIndex[0]+cbIndex[1]+cbIndex[2]+cbTotalKingCount>=3)
						{

							//过滤3精
							if(cbIndex[0]+cbIndex[1]+cbIndex[2]==0) break;


							//精牌组合
							for( BYTE j = 0; j < CountArray(cbIndex); j++ )
							{
								if( cbIndex[j] > 0 ) 
								{
									cbIndex[j]--;
								}
								else 
								{
									//精配
									cbTempTotalKingCount--;

								}
							}
							if( cbTempTotalKingCount >= 0 )
							{
								KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
								KindItem[cbKindItemCount].cbCardIndex[0]=i;
								KindItem[cbKindItemCount].cbCardIndex[1]=i+2;
								KindItem[cbKindItemCount].cbCardIndex[2]=i+3;
								KindItem[cbKindItemCount++].wWeaveKind=WIK_DXBL;
							}
							else
								break;

						}
					}

				}
			}	


			for (BYTE i=MAX_INDEX-3;i<MAX_INDEX;i++)		//中发白
			{
				if ( i==MAX_INDEX-3)
				{
					if (cbCardIndex[i]+cbCardIndex[i+1]+cbCardIndex[i+2]+cbTotalKingCount>=3)
					{
						BYTE cbIndex[3]={cbCardIndex[i],cbCardIndex[i+1],cbCardIndex[i+2]};
						INT cbTempTotalKingCount=cbTotalKingCount;

						while (cbIndex[0]+cbIndex[1]+cbIndex[2]+cbTotalKingCount>=3)
						{
							//过滤3精
							if(cbIndex[0]+cbIndex[1]+cbIndex[2]==0) break;

							//精牌组合
							for( BYTE j = 0; j < CountArray(cbIndex); j++ )
							{
								if( cbIndex[j] > 0 ) 
								{
									cbIndex[j]--;
								}
								else 
								{
									cbTempTotalKingCount--;
								}
							}
							if( cbTempTotalKingCount >= 0 )
							{
								KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
								KindItem[cbKindItemCount].cbCardIndex[0]=i;
								KindItem[cbKindItemCount].cbCardIndex[1]=i+1;
								KindItem[cbKindItemCount].cbCardIndex[2]=i+2;
								KindItem[cbKindItemCount++].wWeaveKind=WIK_LEFT;
							}
							else
								break;

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
			BYTE cbIndex[MAX_WEAVE]={0,1,2,3};
			tagKindItem * pKindItem[MAX_WEAVE];
			ZeroMemory(&pKindItem,sizeof(pKindItem));

			//开始组合
			do
			{
				//设置变量
				CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));
				for (BYTE i=0;i<cbLessKindItem;i++)
					pKindItem[i]=&KindItem[cbIndex[i]];

				BYTE cbWeaveKingReplace=0;
				//数量判断
				bool bEnoughCard=true;
				for (BYTE i=0;i<cbLessKindItem*3;i++)
				{
					//存在判断
					BYTE cbCardIndex=pKindItem[i/3]->cbCardIndex[i%3]; 
					if (cbCardIndexTemp[cbCardIndex]==0)
					{
						cbWeaveKingReplace=1;
						if(cbCardIndexTemp[m_cbKingCardIndex[0]]==0)
						{
							if(cbCardIndexTemp[m_cbKingCardIndex[1]]==0)
							{
								bEnoughCard=false;
								break;

							}
							else
							
								cbCardIndexTemp[m_cbKingCardIndex[1]]--;

						}
						else
							cbCardIndexTemp[m_cbKingCardIndex[0]]--;
					}
					else 
						cbCardIndexTemp[cbCardIndex]--;
				}

				//胡牌判断
				if (bEnoughCard==true)
				{
					//牌眼判断
					BYTE cbCardEye=0;

					//眼牌替换
					BYTE cbEyeKingReplace=0;

					//是否继续
					bool bContinue=true;

					if ((cbCardIndexTemp[m_cbKingCardIndex[0]]==1&& cbCardIndexTemp[m_cbKingCardIndex[1]])==1) //正精+副精
					{
						//精牌替换
						cbEyeKingReplace=1;	

						//眼牌数据
						cbCardEye=SwitchToCardData(m_cbKingCardIndex[0]);

						//是否继续
						bContinue=false;
					}
					if(bContinue==true)
					{
						if ((cbCardIndexTemp[m_cbKingCardIndex[0]]  + cbCardIndexTemp[m_cbKingCardIndex[1]])==1) //精牌+单张
						{

							//精牌替换
							cbEyeKingReplace=1;

							for (BYTE i=0;i<MAX_INDEX;i++)
							{
								// 正精过滤
								if(IsKingCardIndex(i)) continue;

								//副精过滤
								if(IsViceKingCardIndex(i)) continue;

								//找到独张
								if (cbCardIndex[i]==1)
								{
									cbCardEye=SwitchToCardData(i);
								}
							}
							//是否继续
							bContinue=false;
						}


					}
					if(bContinue==true)
					{
						for (BYTE i=0;i<MAX_INDEX;i++)
						{
							if (cbCardIndexTemp[i]==2)
							{
								//眼牌数据
								cbCardEye=SwitchToCardData(i);

								//精牌替换
								cbEyeKingReplace=0;

								//是否继续
								bContinue=false;
								break;
							}
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
							AnalyseItem.wWeaveKind[i]=WeaveItem[i].wWeaveKind;
							AnalyseItem.cbCenterCard[i]=WeaveItem[i].cbCenterCard;
						}
						//精牌替换
						AnalyseItem.cbWeaveKingReplace=cbWeaveKingReplace;

						//设置牌型
						for (BYTE i=0;i<cbLessKindItem;i++) 
						{
							AnalyseItem.wWeaveKind[i+cbWeaveCount]=pKindItem[i]->wWeaveKind;
							AnalyseItem.cbCenterCard[i+cbWeaveCount]=pKindItem[i]->cbCenterCard;
						}

						//设置牌眼
						AnalyseItem.cbCardEye=cbCardEye;

						//精牌替换
						AnalyseItem.cbEyeKingReplace=cbEyeKingReplace;

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
	}
	else
	{

		//正副精数目
		BYTE cbKingCount=cbTempCardIndex[m_cbKingCardIndex[0]];
		BYTE cbViceKingCount=cbTempCardIndex[m_cbKingCardIndex[1]];

		//精牌清空
		cbTempCardIndex[m_cbKingCardIndex[0]]=0;
		cbTempCardIndex[m_cbKingCardIndex[1]]=0;


		//插入数据
		BYTE cbCurrentIndex = SwitchToCardIndex(cbCurrentCard);
		//放炮正副精
		if(bZimo==false &&(IsKingCardIndex(cbCurrentIndex)))
		{
			cbTempCardIndex[m_cbKingCardIndex[0]]=1;
			ASSERT(cbKingCount>=1);
			cbKingCount -=1;

		}
		if(bZimo==false &&(IsViceKingCardIndex(cbCurrentIndex)))
		{
			cbTempCardIndex[m_cbKingCardIndex[1]]=1;
			ASSERT(cbViceKingCount>=1);
			cbViceKingCount -=1;

		}
		//精牌总数
		BYTE cbTotalKingCount=cbKingCount+cbViceKingCount;
		if(cbTotalKingCount==0) return false;
	
		//拆分分析
		if (cbCardCount>=3)
		{
			//3个财神先凑刻
			if(cbTotalKingCount>=3)
			{
				if(cbKingCount>0&&cbViceKingCount>0)
				{
					ASSERT( cbKindItemCount < CountArray(KindItem) );
					KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(m_cbKingCardIndex[0]);
					KindItem[cbKindItemCount].cbCardIndex[0]=m_cbKingCardIndex[0];
					KindItem[cbKindItemCount].cbCardIndex[1]=m_cbKingCardIndex[0];
					KindItem[cbKindItemCount].cbCardIndex[2]=m_cbKingCardIndex[0];
					KindItem[cbKindItemCount++].wWeaveKind=WIK_PENG;
				}
			}
			for (BYTE i=0;i<MAX_INDEX;i++)
			{
				//同牌判断
				if (cbTempCardIndex[i]>=3)
				{
					ASSERT( cbKindItemCount < CountArray(KindItem) );
					KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
					KindItem[cbKindItemCount].cbCardIndex[0]=i;
					KindItem[cbKindItemCount].cbCardIndex[1]=i;
					KindItem[cbKindItemCount].cbCardIndex[2]=i;
					KindItem[cbKindItemCount++].wWeaveKind=WIK_PENG;
				}
				else if(cbTempCardIndex[i]==1)
				{
					//过滤精牌
					if(IsViceKingCardIndex(i)==false &&IsViceKingCardIndex(i)==false)
					{
						if(cbTotalKingCount>=2)
						{
							ASSERT( cbKindItemCount < CountArray(KindItem) );
							KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
							KindItem[cbKindItemCount].cbCardIndex[0]=i;
							KindItem[cbKindItemCount].cbCardIndex[1]=i;
							KindItem[cbKindItemCount].cbCardIndex[2]=i;
							KindItem[cbKindItemCount++].wWeaveKind=WIK_PENG;
						}
					}

				}
				else if(cbTempCardIndex[i]==2)
				{
					//过滤精牌
					if(IsViceKingCardIndex(i)==false &&IsViceKingCardIndex(i)==false)
					{
						if(cbTotalKingCount>=1)
						{
							ASSERT( cbKindItemCount < CountArray(KindItem) );
							KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
							KindItem[cbKindItemCount].cbCardIndex[0]=i;
							KindItem[cbKindItemCount].cbCardIndex[1]=i;
							KindItem[cbKindItemCount].cbCardIndex[2]=i;
							KindItem[cbKindItemCount++].wWeaveKind=WIK_PENG;
						}
					}

				}
				else
				{
					//什么也不执行
				}


				//连牌判断
				if ((i<(MAX_INDEX-2-7))&&((i%9)<7))
				{

					if (cbTempCardIndex[i]+cbTempCardIndex[i+1]+cbTempCardIndex[i+2]+cbTotalKingCount>=3)
					{
						BYTE cbIndex[3]={cbTempCardIndex[i],cbTempCardIndex[i+1],cbTempCardIndex[i+2]};
						INT cbTempTotalKingCount=cbTotalKingCount;

						while (cbIndex[0]+cbIndex[1]+cbIndex[2]+cbTotalKingCount>=3)
						{
							//过滤3精
							if(cbIndex[0]+cbIndex[1]+cbIndex[2]==0) break;

							BYTE cbWeaveKingReplace=0;

							//精牌组合
							for( BYTE j = 0; j < CountArray(cbIndex); j++ )
							{
								if( cbIndex[j] > 0 ) 
								{
									cbIndex[j]--;
								}
								else 
								{
									//精配
									cbWeaveKingReplace=1;
									cbTempTotalKingCount--;
								}
							}
							if( cbTempTotalKingCount >= 0 )
							{
								KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
								KindItem[cbKindItemCount].cbCardIndex[0]=i;
								KindItem[cbKindItemCount].cbCardIndex[1]=i+1;
								KindItem[cbKindItemCount].cbCardIndex[2]=i+2;
								KindItem[cbKindItemCount++].wWeaveKind=WIK_LEFT;
							}
							else
								break;

						}
					}

				}
			}
			//东南西北
			for (BYTE i=MAX_INDEX-7;i<MAX_INDEX-5;i++)
			{
				//变量定义
				BYTE cbCardIndexTemp[MAX_INDEX];

				//设置变量
				CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));

				//东南西 南西北
				if (cbTempCardIndex[i]+cbTempCardIndex[i+1]+cbTempCardIndex[i+2]+cbTotalKingCount>=3)
				{
					BYTE cbIndex[3]={cbTempCardIndex[i],cbTempCardIndex[i+1],cbTempCardIndex[i+2]};
					INT cbTempTotalKingCount=cbTotalKingCount;

					while (cbIndex[0]+cbIndex[1]+cbIndex[2]+cbTotalKingCount>=3)
					{
						//过滤3精
						if(cbIndex[0]+cbIndex[1]+cbIndex[2]==0) break;

						BYTE cbWeaveKingReplace=0;

						//精牌组合
						for( BYTE j = 0; j < CountArray(cbIndex); j++ )
						{
							if( cbIndex[j] > 0 ) 
							{
								cbIndex[j]--;
							}
							else 
							{
								//精配
								cbWeaveKingReplace=1;
								cbTempTotalKingCount--;
							}
						}
						if( cbTempTotalKingCount >= 0 )
						{
							KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
							KindItem[cbKindItemCount].cbCardIndex[0]=i;
							KindItem[cbKindItemCount].cbCardIndex[1]=i+1;
							KindItem[cbKindItemCount].cbCardIndex[2]=i+2;
							KindItem[cbKindItemCount++].wWeaveKind=WIK_LEFT;
						}
						else
							break;

					}
				}

				//东南北
				if(i==MAX_INDEX-7)
				{
					if (cbTempCardIndex[i]+cbTempCardIndex[i+1]+cbTempCardIndex[i+3]+cbTotalKingCount>=3)
					{
						BYTE cbIndex[3]={cbCardIndex[i],cbCardIndex[i+1],cbCardIndex[i+3]};
						INT cbTempTotalKingCount=cbTotalKingCount;

						while (cbIndex[0]+cbIndex[1]+cbIndex[2]+cbTotalKingCount>=3)
						{
							//过滤3精
							if(cbIndex[0]+cbIndex[1]+cbIndex[2]==0) break;
							
							BYTE cbWeaveKingReplace=0;

							//精牌组合
							for( BYTE j = 0; j < CountArray(cbIndex); j++ )
							{
								if( cbIndex[j] > 0 ) 
								{
									cbIndex[j]--;
								}
								else 
								{
									//精配
									cbWeaveKingReplace=1;
									cbTempTotalKingCount--;
								}
							}
							if( cbTempTotalKingCount >= 0 )
							{
								KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
								KindItem[cbKindItemCount].cbCardIndex[0]=i;
								KindItem[cbKindItemCount].cbCardIndex[1]=i+1;
								KindItem[cbKindItemCount].cbCardIndex[2]=i+3;
								KindItem[cbKindItemCount++].wWeaveKind=WIK_DNBL;
							}
							else
								break;

						}
					}

				}
				//东西北
				if(i==MAX_INDEX-7)
				{
					if (cbTempCardIndex[i]+cbTempCardIndex[i+2]+cbTempCardIndex[i+3]+cbTotalKingCount>=3)
					{
						BYTE cbIndex[3]={cbTempCardIndex[i],cbTempCardIndex[i+2],cbTempCardIndex[i+3]};
						INT cbTempTotalKingCount=cbTotalKingCount;

						while (cbIndex[0]+cbIndex[1]+cbIndex[2]+cbTotalKingCount>=3)
						{

							//过滤3精
							if(cbIndex[0]+cbIndex[1]+cbIndex[2]==0) break;

							BYTE cbWeaveKingReplace=0;

							//精牌组合
							for( BYTE j = 0; j < CountArray(cbIndex); j++ )
							{
								if( cbIndex[j] > 0 ) 
								{
									cbIndex[j]--;
								}
								else 
								{
									cbWeaveKingReplace=1;
									cbTempTotalKingCount--;

								}
							}
							if( cbTempTotalKingCount >= 0 )
							{
								KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
								KindItem[cbKindItemCount].cbCardIndex[0]=i;
								KindItem[cbKindItemCount].cbCardIndex[1]=i+2;
								KindItem[cbKindItemCount].cbCardIndex[2]=i+3;
								KindItem[cbKindItemCount++].wWeaveKind=WIK_DXBL;
							}
							else
								break;

						}
					}

				}


			}	



			for (BYTE i=MAX_INDEX-3;i<MAX_INDEX;i++)		//中发白
			{
				if ( i==MAX_INDEX-3)
				{
					if (cbTempCardIndex[i]+cbTempCardIndex[i+1]+cbTempCardIndex[i+2]+cbTotalKingCount>=3)
					{
						BYTE cbIndex[3]={cbTempCardIndex[i],cbTempCardIndex[i+1],cbTempCardIndex[i+2]};
						INT cbTempTotalKingCount=cbTotalKingCount;

						while (cbIndex[0]+cbIndex[1]+cbIndex[2]+cbTotalKingCount>=3)
						{
							//过滤3精
							if(cbIndex[0]+cbIndex[1]+cbIndex[2]==0) break;
							BYTE cbWeaveKingReplace=0;

							//精牌组合
							for( BYTE j = 0; j < CountArray(cbIndex); j++ )
							{
								if( cbIndex[j] > 0 ) 
								{
									cbIndex[j]--;
								}
								else 
								{
									//精吊
									cbWeaveKingReplace=1;
									cbTempTotalKingCount--;
								}
							}
							if( cbTempTotalKingCount >= 0 )
							{
								KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
								KindItem[cbKindItemCount].cbCardIndex[0]=i;
								KindItem[cbKindItemCount].cbCardIndex[1]=i+1;
								KindItem[cbKindItemCount].cbCardIndex[2]=i+2;
								KindItem[cbKindItemCount++].wWeaveKind=WIK_LEFT;
							}
							else
								break;

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
			BYTE cbIndex[MAX_WEAVE]={0,1,2,3};
			tagKindItem * pKindItem[MAX_WEAVE];
			ZeroMemory(&pKindItem,sizeof(pKindItem));

			//开始组合
			do
			{
				//设置变量
				CopyMemory(cbCardIndexTemp,cbTempCardIndex,sizeof(cbCardIndexTemp));
				for (BYTE i=0;i<cbLessKindItem;i++)
					pKindItem[i]=&KindItem[cbIndex[i]];

				BYTE cbTempKingCount=cbKingCount;
				BYTE cbTempViceKingCount=cbViceKingCount;

				//数量判断
				BYTE cbWeaveKingReplace=0;
				bool bEnoughCard=true;
				for (BYTE i=0;i<cbLessKindItem*3;i++)
				{
					//存在判断
					BYTE cbCardIndex=pKindItem[i/3]->cbCardIndex[i%3]; 
					if (cbCardIndexTemp[cbCardIndex]==0)
					{
						cbWeaveKingReplace=1;
						if(cbTempKingCount==0)
						{
							if(cbTempViceKingCount==0)
							{
								bEnoughCard=false;
								break;

							}
							else
								cbTempViceKingCount--;

						}
						else
							cbTempKingCount--;
					}
					else 
						cbCardIndexTemp[cbCardIndex]--;
				}

				//胡牌判断
				if (bEnoughCard==true)
				{
					//牌眼判断
					BYTE cbCardEye=0;

					//眼牌替换
					BYTE cbEyeKingReplace=0;

					//是否继续
					bool bContinue=true;

					if (cbTempViceKingCount==1&& cbTempKingCount==1) //正精+副精
					{
						//精牌替换
						cbEyeKingReplace=1;	

						//眼牌数据
						cbCardEye=SwitchToCardData(m_cbKingCardIndex[0]);

						//是否继续
						bContinue=false;
					}
					if(bContinue==true)
					{
						if (cbTempViceKingCount==1&&SwitchToCardIndex(cbCurrentCard)!=m_cbKingCardIndex[1]) //精牌+单张
						{

							//精牌替换
							cbEyeKingReplace=1;

							for (BYTE i=0;i<MAX_INDEX;i++)
							{
								if(i==m_cbKingCardIndex[1]) continue;

								//找到独张
								if (cbCardIndex[i]==1)
								{
									cbCardEye=SwitchToCardData(i);
								}
							}
							//是否继续
							bContinue=false;
						}


						if (cbTempKingCount==1&&SwitchToCardIndex(cbCurrentCard)!=m_cbKingCardIndex[0]) //精牌+单张
						{

							//精牌替换
							cbEyeKingReplace=1;

							for (BYTE i=0;i<MAX_INDEX;i++)
							{
								if(i==m_cbKingCardIndex[0]) continue;

								//找到独张
								if (cbCardIndex[i]==1)
								{
									cbCardEye=SwitchToCardData(i);
								}
							}
							//是否继续
							bContinue=false;
						}

					}
					if(bContinue==true)
					{
						for (BYTE i=0;i<MAX_INDEX;i++)
						{
							if (cbCardIndexTemp[i]==2)
							{
								//眼牌数据
								cbCardEye=SwitchToCardData(i);

								//精牌替换
								cbEyeKingReplace=0;

								//是否继续
								bContinue=false;
								break;
							}
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
							AnalyseItem.wWeaveKind[i]=WeaveItem[i].wWeaveKind;
							AnalyseItem.cbCenterCard[i]=WeaveItem[i].cbCenterCard;
						}

						//精牌替换
						AnalyseItem.cbWeaveKingReplace=cbWeaveKingReplace;

						//设置牌型
						for (BYTE i=0;i<cbLessKindItem;i++) 
						{
							AnalyseItem.wWeaveKind[i+cbWeaveCount]=pKindItem[i]->wWeaveKind;
							AnalyseItem.cbCenterCard[i+cbWeaveCount]=pKindItem[i]->cbCenterCard;
						}

						//设置牌眼
						AnalyseItem.cbCardEye=cbCardEye;

						//精牌替换
						AnalyseItem.cbEyeKingReplace=cbEyeKingReplace;

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
	}
	return false;


}


//吃胡分析
WORD CGameLogic::AnalyseChiHuCard(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard, WORD wChiHuRight, tagChiHuResult & ChiHuResult,bool bZimo)
{
	//变量定义
	WORD wChiHuKind=CHK_NULL;

	//设置变量
	ZeroMemory(&ChiHuResult,sizeof(ChiHuResult));

	//构造扑克
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));

	//插入扑克
	if (cbCurrentCard!=0)
		cbCardIndexTemp[SwitchToCardIndex(cbCurrentCard)]++;
	
	//自模权位
	if(bZimo)
	{
		wChiHuRight|= CHR_ZI_MO;
	}

	//精吊权位
	if((bZimo) && (cbWeaveCount ==4) && ((cbCardIndex[m_cbKingCardIndex[0]]+cbCardIndex[m_cbKingCardIndex[1]])==1))
	{
		wChiHuRight |=CHR_KING_WAIT;
	}

	
	//权位调整 杠胡+自摸 = 杠上花
	if((wChiHuRight&CHR_QIANG_GANG)&&(wChiHuRight&CHR_ZI_MO))
	{
		wChiHuRight &= ~CHR_QIANG_GANG;
		wChiHuRight &= ~CHR_ZI_MO;
		wChiHuRight |= CHR_GANG_FLOWER;
	}

	//德国七星
	BYTE bGerman=0;

	//7星
	if(ServenStar(cbCardIndexTemp,bGerman,cbCurrentCard,bZimo)==true)
	{
		if(bGerman==1)
			wChiHuRight |=CHR_GERMAN;
		ChiHuResult.wChiHuKind = CHK_SERVEN;
		ChiHuResult.wChiHuRight |= wChiHuRight;

		return WIK_CHI_HU;

	}
	bGerman=0;

	//13离
	if(cbWeaveCount==0 &&IsNeatAlone(cbCardIndexTemp,bGerman,cbCurrentCard,bZimo)==true)
	{
		if(bGerman==1)
			wChiHuRight |=CHR_GERMAN;

		ChiHuResult.wChiHuKind=CHK_THIRTEEN;
		ChiHuResult.wChiHuRight |=wChiHuRight;
		return WIK_CHI_HU;
	}

	//精吊
	if((bZimo||wChiHuRight&CHR_DI) &&(cbCardIndex[m_cbKingCardIndex[0]]>0))
	{
		BYTE cbCardIndexTemp2[MAX_INDEX];
		CopyMemory(cbCardIndexTemp2,cbCardIndexTemp,sizeof(cbCardIndexTemp2));
		RemoveCard(cbCardIndexTemp2,cbCurrentCard);
		RemoveCard(cbCardIndexTemp2,SwitchToCardData(m_cbKingCardIndex[0]));

		bool bCurrentKing=IsKingCardData(cbCurrentCard)||IsViceKingCardData(cbCurrentCard);
		BYTE cbGerman=0;

		if(IsNeat2(cbCardIndexTemp2,WeaveItem,cbWeaveCount)==true)
		{
			ChiHuResult.wChiHuKind = CHK_QI_DUI;
			ChiHuResult.wChiHuRight =( CHR_GERMAN|CHR_KING_WAIT|wChiHuRight);
			return WIK_CHI_HU;
		}
		if(IsQiXiaoDui(cbCardIndexTemp2,WeaveItem,cbWeaveCount,cbCurrentCard,cbGerman,bZimo)==true)
		{
			if(cbGerman==1&&(bCurrentKing==true &&( SwitchToCardIndex((cbCurrentCard))==m_cbKingCardIndex[0]||(SwitchToCardIndex((cbCurrentCard))==m_cbKingCardIndex[1]&&cbCardIndex[m_cbKingCardIndex[1]]>0))))
				wChiHuRight |= CHR_GERMAN;
			ChiHuResult.wChiHuKind = CHK_QI_DUI;
			ChiHuResult.wChiHuRight =(CHR_KING_WAIT|wChiHuRight);
			return WIK_CHI_HU;
		}
		cbGerman=0;
		if(IsNeat3(cbCardIndexTemp2,cbGerman)==true)
		{
			if(cbGerman==1&&(bCurrentKing==true &&( SwitchToCardIndex((cbCurrentCard))==m_cbKingCardIndex[0]||(SwitchToCardIndex((cbCurrentCard))==m_cbKingCardIndex[1]&&cbCardIndex[m_cbKingCardIndex[1]]>0))))
				wChiHuRight |= CHR_GERMAN;
			wChiHuRight =(CHR_KING_WAIT|wChiHuRight);
		}
	}

	//精吊
	if((bZimo||wChiHuRight&CHR_DI) &&(cbCardIndex[m_cbKingCardIndex[1]]>0))
	{
		BYTE cbCardIndexTemp2[MAX_INDEX];
		CopyMemory(cbCardIndexTemp2,cbCardIndexTemp,sizeof(cbCardIndexTemp2));
		RemoveCard(cbCardIndexTemp2,cbCurrentCard);
		RemoveCard(cbCardIndexTemp2,SwitchToCardData(m_cbKingCardIndex[1]));

		bool bCurrentKing=IsKingCardData(cbCurrentCard)||IsViceKingCardData(cbCurrentCard);
		BYTE cbGerman=0;

		if(IsNeat2(cbCardIndexTemp2,WeaveItem,cbWeaveCount)==true)
		{
			ChiHuResult.wChiHuKind = CHK_QI_DUI;
			ChiHuResult.wChiHuRight =( CHR_GERMAN|CHR_KING_WAIT|wChiHuRight);
			return WIK_CHI_HU;
		}
		if(IsQiXiaoDui(cbCardIndexTemp2,WeaveItem,cbWeaveCount,cbCurrentCard,cbGerman,bZimo)==true)
		{
			if(cbGerman==1&&(bCurrentKing==true &&( SwitchToCardIndex((cbCurrentCard))==m_cbKingCardIndex[1]||(SwitchToCardIndex((cbCurrentCard))==m_cbKingCardIndex[0]&&cbCardIndex[m_cbKingCardIndex[0]]>0))))
				wChiHuRight |= CHR_GERMAN;
			ChiHuResult.wChiHuKind = CHK_QI_DUI;
			ChiHuResult.wChiHuRight =(CHR_KING_WAIT|wChiHuRight);
			return WIK_CHI_HU;
		}
		cbGerman=0;
		if(IsNeat3(cbCardIndexTemp2,cbGerman)==true)
		{
			if(cbGerman==1&&(bCurrentKing==true &&( SwitchToCardIndex((cbCurrentCard))==m_cbKingCardIndex[1]||(SwitchToCardIndex((cbCurrentCard))==m_cbKingCardIndex[0]&&cbCardIndex[m_cbKingCardIndex[0]]>0))))
				wChiHuRight |= CHR_GERMAN;			
			wChiHuRight =(CHR_KING_WAIT|wChiHuRight);
		}

	}
	
	bGerman=0;
	//7小对分析
	if(IsQiXiaoDui(cbCardIndexTemp,WeaveItem,cbWeaveCount,cbCurrentCard,bGerman,bZimo))
	{
		if(bGerman>0)
			wChiHuKind=CHK_GERMAN_SERVEN;
		else
			wChiHuKind=CHK_QI_DUI;

		ChiHuResult.wChiHuKind = wChiHuKind;
		ChiHuResult.wChiHuRight |= wChiHuRight;

		return WIK_CHI_HU;
	}

    //牌型数组
	static CAnalyseItemArray AnalyseItemArray;

	//设置变量
	AnalyseItemArray.RemoveAll();
	ZeroMemory(&ChiHuResult,sizeof(ChiHuResult));

	//分析扑克
	AnalyseCard(cbCardIndexTemp,WeaveItem,cbWeaveCount,AnalyseItemArray,cbCurrentCard,bZimo);

	//胡牌分析
	if (AnalyseItemArray.GetCount()>0)
	{
		//牌型分析
		for (INT_PTR i=0;i<AnalyseItemArray.GetCount();i++)
		{
			//变量定义
			bool bLianCard=false,bPengCard=false;
			bool bGerman=true;
			tagAnalyseItem * pAnalyseItem=&AnalyseItemArray[i];

			//牌型分析
			for (BYTE j=0;j<CountArray(pAnalyseItem->wWeaveKind);j++)
			{
				WORD wWeaveKind=pAnalyseItem->wWeaveKind[j];
				bPengCard=((wWeaveKind&(WIK_GANG|WIK_PENG))!=0)?true:bPengCard;
				bLianCard=((wWeaveKind&(WIK_LEFT|WIK_CENTER|WIK_RIGHT))!=0)?true:bLianCard;

			}
			bGerman=(pAnalyseItem->cbWeaveKingReplace||pAnalyseItem->cbEyeKingReplace)?false:bGerman;
			//牌型判断
			ASSERT((bLianCard==true)||(bPengCard==true));	
			//基胡
			if ((bLianCard==true)&&(bPengCard==true)) 
				wChiHuKind|=CHK_JI_HU;
			if ((bLianCard==true)&&(bPengCard==false)) 
				wChiHuKind|=CHK_JI_HU;
			if((bLianCard==false)&&(bPengCard==true))
				wChiHuKind|=CHK_PENG_PENG;

			if(bGerman==true)
				wChiHuRight |=CHR_GERMAN;
		}
	}

	//结果判断
	if (wChiHuKind!=CHK_NULL)
	{
		//设置结果
		ChiHuResult.wChiHuKind=wChiHuKind;
		ChiHuResult.wChiHuRight=wChiHuRight;
		return WIK_CHI_HU;
	}

	return WIK_NULL;
}

//是否精吊
bool CGameLogic::IsKingWait(BYTE cbCardIndex[MAX_INDEX],tagWeaveItem WeaveItem[], BYTE cbWeaveCount,BYTE cbCurrentCard,bool bZimo)
{
	//构造扑克
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));

	//插入扑克
	if (cbCurrentCard!=0)
		cbCardIndexTemp[SwitchToCardIndex(cbCurrentCard)]++;

	//精吊
	if(cbCardIndex[m_cbKingCardIndex[0]]>0)
	{
		BYTE cbCardIndexTemp2[MAX_INDEX];
		CopyMemory(cbCardIndexTemp2,cbCardIndexTemp,sizeof(cbCardIndexTemp2));
		RemoveCard(cbCardIndexTemp2,cbCurrentCard);
		RemoveCard(cbCardIndexTemp2,SwitchToCardData(m_cbKingCardIndex[0]));

		bool bCurrentKing=IsKingCardData(cbCurrentCard)||IsViceKingCardData(cbCurrentCard);
		BYTE cbGerman=0;

		if(IsNeat2(cbCardIndexTemp2,WeaveItem,cbWeaveCount)==true)
		{
			return true;
		}
		if(IsQiXiaoDui(cbCardIndexTemp2,WeaveItem,cbWeaveCount,cbCurrentCard,cbGerman,bZimo)==true)
		{
			return true;
		}
		cbGerman=0;
		if(IsNeat3(cbCardIndexTemp2,cbGerman)==true)
		{
			return true;
		}
	}

	//精吊
	if(cbCardIndex[m_cbKingCardIndex[1]]>0)
	{
		BYTE cbCardIndexTemp2[MAX_INDEX];
		CopyMemory(cbCardIndexTemp2,cbCardIndexTemp,sizeof(cbCardIndexTemp2));
		RemoveCard(cbCardIndexTemp2,cbCurrentCard);
		RemoveCard(cbCardIndexTemp2,SwitchToCardData(m_cbKingCardIndex[1]));

		bool bCurrentKing=IsKingCardData(cbCurrentCard)||IsViceKingCardData(cbCurrentCard);
		BYTE cbGerman=0;

		if(IsNeat2(cbCardIndexTemp2,WeaveItem,cbWeaveCount)==true)
		{
			return true;
		}
		if(IsQiXiaoDui(cbCardIndexTemp2,WeaveItem,cbWeaveCount,cbCurrentCard,cbGerman,bZimo)==true)
		{
			return true;
		}
		cbGerman=0;
		if(IsNeat3(cbCardIndexTemp2,cbGerman)==true)
		{
			return true;
		}
	}


	return false;
}

//扑克转换
BYTE CGameLogic::SwitchToCardData(BYTE cbCardIndex)
{
	ASSERT(cbCardIndex<MAX_INDEX);

	ASSERT(cbCardIndex<34);
	return ((cbCardIndex/9)<<4)|(cbCardIndex%9+1);
}

//扑克转换
BYTE CGameLogic::SwitchToCardIndex(BYTE cbCardData)
{

	ASSERT(IsValidCard(cbCardData));
	////计算位置
	//BYTE cbValue=cbCardData&MASK_VALUE;
	//BYTE cbColor=(cbCardData&MASK_COLOR)>>4;
	return ((cbCardData&MASK_COLOR)>>4)*9+(cbCardData&MASK_VALUE)-1;
}

//扑克转换
BYTE CGameLogic::SwitchToCardData(BYTE cbCardIndex[MAX_INDEX], BYTE cbCardData[MAX_COUNT],BYTE bMaxCount)
{
	//转换扑克 精牌放到原来位置
	BYTE bPosition=0;
	if(cbCardIndex[m_cbKingCardIndex[0]] > 0)
	{
		for (BYTE j=0;j<cbCardIndex[m_cbKingCardIndex[0]];j++)
		{
			ASSERT(bPosition<bMaxCount);
			cbCardData[bPosition++]=SwitchToCardData(m_cbKingCardIndex[0]);
		}
	}
	if(cbCardIndex[m_cbKingCardIndex[1]] > 0)
	{
		for (BYTE j=0;j<cbCardIndex[m_cbKingCardIndex[1]];j++)
		{
			ASSERT(bPosition<bMaxCount);
			cbCardData[bPosition++]=SwitchToCardData(m_cbKingCardIndex[1]);
		}
	}
	for (BYTE i=0;i<MAX_INDEX;i++) 
	{
	
		if (cbCardIndex[i]!=0)
		{
			//过滤正副精
			if(i==m_cbKingCardIndex[0]) continue;
			if(i==m_cbKingCardIndex[1]) continue;

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
		cbCardIndex[SwitchToCardIndex(cbCardData[i])]++;
	}

	return cbCardCount;
}


//计算积分
LONG CGameLogic::CalScore(tagChiHuResult & ChihuResult,LONG &lScore,LONG &lTimes)
{
	//胡牌校验
	ASSERT(ChihuResult.wChiHuKind!=CHK_NULL);
	if (ChihuResult.wChiHuKind== CHK_NULL) return 0;

	//赋值初始化
	lTimes=1L;

	//自摸
	if(ChihuResult.wChiHuRight&CHR_ZI_MO)
	{
		lTimes *= 2;
	}
	//抢杠
	if(ChihuResult.wChiHuRight&CHR_QIANG_GANG)
	{
		lTimes *= 2;
	}
	//杠开
	if(ChihuResult.wChiHuRight&CHR_GANG_FLOWER)
	{
		lTimes *= 4;
	}
	//天胡
	if(ChihuResult.wChiHuRight&CHR_TIAN)
	{
		lTimes *= 20;
	}
	//地胡
	if(ChihuResult.wChiHuRight&CHR_DI)
	{
		lTimes *= 20;
	}
	//精吊
	if(ChihuResult.wChiHuRight&CHR_KING_WAIT)
	{
		lTimes *= 2;
	}
	//德国
	if(ChihuResult.wChiHuRight&CHR_GERMAN)
	{
		lTimes *= 2;
	}
	//德中德
	if(ChihuResult.wChiHuRight&CHR_GERMAN_GERMAN)
	{
		lTimes *= 2;
	}
	//7对
	if(ChihuResult.wChiHuKind&CHK_QI_DUI)
	{
		lTimes *= 2;
	}
	//大七对（碰碰胡）
	if(ChihuResult.wChiHuKind&CHK_PENG_PENG)
	{
		lTimes *= 2;
	}
	//13乱
	if(ChihuResult.wChiHuKind&CHK_THIRTEEN)
	{
		lTimes *= 2;
	}
	//七星十三烂
	if(ChihuResult.wChiHuKind&CHK_SERVEN)
	{
		lTimes *= 4;
	}
	//德国7对 和7对和德国
	if(ChihuResult.wChiHuKind&CHK_GERMAN_SERVEN)
	{
		lTimes *= 4;
	}

	return lTimes;
}

//13乱
bool CGameLogic::IsNeatAlone(BYTE cbCardIndex[MAX_INDEX],BYTE &cbGerman,BYTE cbCurrentCard,bool bZimo)
{
	BYTE cbTempCardIndex[MAX_INDEX];
	CopyMemory(cbTempCardIndex,cbCardIndex,sizeof(BYTE)*MAX_INDEX);

	//计算数目
	BYTE cbCardCount=0;
	for (BYTE i=0;i<MAX_INDEX;i++) 
		cbCardCount+=cbTempCardIndex[i];
	
	//满牌才能全不靠
	if(cbCardCount<14) return false;


	//返回值判断
	bool bRet=true;


	//变量处理
	const BYTE COMMON_TYPE_SUM = 9;
	for (BYTE i=0;i<34;i++) 
	{
		//重复字牌
		if(cbTempCardIndex[i] > 1)
		{
			bRet=false;
			break;
		}
	}
	if(bRet)
	{
		BYTE* pKingIndex = 0;
		for(i=0; i<3; i++)
		{
			pKingIndex = &cbTempCardIndex[i*COMMON_TYPE_SUM];
			for(BYTE j=0; j<COMMON_TYPE_SUM; j++)
			{
				if(pKingIndex[j] > 0)
				{
					for(BYTE k=0; k<2; k++)
					{
						j ++;

						if(j<COMMON_TYPE_SUM)
						{
							if(pKingIndex[j] > 0)
							{
								bRet= false;
								break;
							}
						}
					}

				}
			}
		}
	}
	if(bRet)
	{
		//德国判断
		cbGerman=1;

		return true;
	}

	//德国判断
	cbGerman=0;

	for (BYTE i=0;i<34;i++) 
	{
		//过滤精牌
		if((i==m_cbKingCardIndex[0]) &&((bZimo )||(bZimo==false&&SwitchToCardIndex(cbCurrentCard)!=m_cbKingCardIndex[0]))) continue;
			
		//过滤精牌
		if((i==m_cbKingCardIndex[1]) &&((bZimo )||(bZimo==false&&SwitchToCardIndex(cbCurrentCard)!=m_cbKingCardIndex[1]))) continue;

		//重复字牌
		if(cbTempCardIndex[i] > 1)
		{
			return false;
		}
	}

	BYTE* pKingIndex = 0;
	for(i=0; i<3; i++)
	{

		pKingIndex = &cbTempCardIndex[i*COMMON_TYPE_SUM];
		for(BYTE j=0; j<COMMON_TYPE_SUM; j++)
		{
			//过滤精牌
			if((i*COMMON_TYPE_SUM+j==m_cbKingCardIndex[0])&&((bZimo)||(bZimo==false&&SwitchToCardIndex(cbCurrentCard)!=m_cbKingCardIndex[0]))) continue;
			if((i*COMMON_TYPE_SUM+j==m_cbKingCardIndex[1])&&((bZimo)||(bZimo==false&&SwitchToCardIndex(cbCurrentCard)!=m_cbKingCardIndex[1]))) continue;

			if(pKingIndex[j] > 0)
			{
				for(BYTE k=0; k<2; k++)
				{
					j ++;
					//过滤精牌
					if((i*COMMON_TYPE_SUM+j==m_cbKingCardIndex[0])&&((bZimo)||(bZimo==false&&SwitchToCardIndex(cbCurrentCard)!=m_cbKingCardIndex[0]))) continue;
					if((i*COMMON_TYPE_SUM+j==m_cbKingCardIndex[1])&&((bZimo)||(bZimo==false&&SwitchToCardIndex(cbCurrentCard)!=m_cbKingCardIndex[1]))) continue;

					if(j<COMMON_TYPE_SUM)
					{
						if(pKingIndex[j] > 0)
						{
							return false;
						}
					}
				}

			}
		}
	}

	return true;
}

//3顺 3刻
bool CGameLogic::IsNeat3(const BYTE cbCardIndex[MAX_INDEX],BYTE &cbGerman)
{
	//德国
	cbGerman=1;

	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));

	//变量定义
	BYTE cbIndex[4]={0,1,2,3};
	tagKindItem * pKindItem[4];
	ZeroMemory(&pKindItem,sizeof(pKindItem));


	//需求判断
	BYTE cbCardCount = 0;
	for(BYTE i=0; i<MAX_INDEX; i++)
	{
		cbCardCount += cbCardIndexTemp[i];
	}
	//效验数目
	ASSERT(cbCardCount<=14);
	if((cbCardCount%3) != 0)
	{
		return false;
	}

	BYTE cbLessKindItem = cbCardCount/3;
	if (cbLessKindItem == 0)
	{
		return true;
	}

	//变量定义
	BYTE cbKindItemCount=0;
	tagKindItem KindItem[MAX_INDEX-2];
	ZeroMemory(KindItem,sizeof(KindItem));

	for (BYTE i=0;i<MAX_INDEX;i++)
	{
		//同牌判断
		if (cbCardIndex[i]>=3)
		{
			KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
			KindItem[cbKindItemCount].cbCardIndex[0]=i;
			KindItem[cbKindItemCount].cbCardIndex[1]=i;
			KindItem[cbKindItemCount].cbCardIndex[2]=i;
			KindItem[cbKindItemCount++].wWeaveKind=WIK_PENG;
		}

		//连牌判断
		if ((i<(MAX_INDEX-2-7))&&(cbCardIndex[i]>0)&&((i%9)<7))
		{
			for (BYTE j=1;j<=cbCardIndex[i];j++)
			{
				if ((cbCardIndex[i+1]>=j)&&(cbCardIndex[i+2]>=j))
				{
					KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
					KindItem[cbKindItemCount].cbCardIndex[0]=i;
					KindItem[cbKindItemCount].cbCardIndex[1]=i+1;
					KindItem[cbKindItemCount].cbCardIndex[2]=i+2;
					KindItem[cbKindItemCount++].wWeaveKind=WIK_LEFT;
				}
			}
		}

	}
	//东南西北
	for (BYTE i=MAX_INDEX-7;i<MAX_INDEX-5;i++)
	{
		//变量定义
		BYTE cbCardIndexTemp[MAX_INDEX];

		//设置变量
		CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));

		//东南西 南西北
		for (BYTE j=1;j<=cbCardIndex[i]&&cbCardIndex[i]>0;j++)
		{
			if ((cbCardIndex[i+1]>=j)&&(cbCardIndex[i+2]>=j))
			{
				KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
				KindItem[cbKindItemCount].cbCardIndex[0]=i;
				KindItem[cbKindItemCount].cbCardIndex[1]=i+1;
				KindItem[cbKindItemCount].cbCardIndex[2]=i+2;
				KindItem[cbKindItemCount++].wWeaveKind=WIK_LEFT;
			}
		}

		//东南北
		if(i==MAX_INDEX-7)
		{
			for (BYTE j=1;j<=cbCardIndex[i]&&cbCardIndex[i]>0;j++)
			{
				if ((cbCardIndex[i+1]>=j)&&(cbCardIndex[i+3]>=j))
				{
					KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
					KindItem[cbKindItemCount].cbCardIndex[0]=i;
					KindItem[cbKindItemCount].cbCardIndex[1]=i+1;
					KindItem[cbKindItemCount].cbCardIndex[2]=i+3;
					KindItem[cbKindItemCount++].wWeaveKind=WIK_DNBL;
				}
			}

		}
		//东西北
		if(i==MAX_INDEX-7)
		{
			for (BYTE j=1;j<=cbCardIndex[i]&&cbCardIndex[i]>0;j++)
			{
				if ((cbCardIndex[i+2]>=j)&&(cbCardIndex[i+3]>=j))
				{
					KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
					KindItem[cbKindItemCount].cbCardIndex[0]=i;
					KindItem[cbKindItemCount].cbCardIndex[1]=i+2;
					KindItem[cbKindItemCount].cbCardIndex[2]=i+3;
					KindItem[cbKindItemCount++].wWeaveKind=WIK_DXBL;
				}
			}
		}


	}	
	//中发白
	for (BYTE i=MAX_INDEX-3;i<MAX_INDEX;i++)
	{

		//连牌判断
		if ((i<(MAX_INDEX-2))&&(cbCardIndex[i]>0)&&((i%9)<7))
		{
			for (BYTE j=1;j<=cbCardIndex[i];j++)
			{
				if ((cbCardIndex[i+1]>=j)&&(cbCardIndex[i+2]>=j))
				{
					KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
					KindItem[cbKindItemCount].cbCardIndex[0]=i;
					KindItem[cbKindItemCount].cbCardIndex[1]=i+1;
					KindItem[cbKindItemCount].cbCardIndex[2]=i+2;
					KindItem[cbKindItemCount++].wWeaveKind=WIK_LEFT;
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
			{
				pKindItem[i]=&KindItem[cbIndex[i]];
			}

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
				{
					cbCardIndexTemp[cbCardIndex]--;
				}
			}
			if(bEnoughCard)
			{
				return true;
			}

			//设置索引
			if (cbIndex[cbLessKindItem-1] == (cbKindItemCount-1))
			{
				for (BYTE i=cbLessKindItem-1;i>0;i--)
				{
					if ((cbIndex[i-1]+1) != cbIndex[i])
					{
						BYTE cbNewIndex = cbIndex[i-1];
						for (BYTE j=(i-1); j<cbLessKindItem; j++) 
						{
							cbIndex[j]=cbNewIndex+j-i+2;
						}
						break;
					}
				}
				if (i==0)
				{
					break;
				}
			}
			else 
			{
				cbIndex[cbLessKindItem-1]++;
			}

		} while (true);

	}

	//非德国
	cbGerman=0;

	//正副精数目
	BYTE cbKingCount=cbCardIndex[m_cbKingCardIndex[0]];
	BYTE cbViceKingCount=cbCardIndex[m_cbKingCardIndex[1]];

	//精牌总数
	BYTE cbTotalKingCount=cbKingCount+cbViceKingCount;

	if(cbTotalKingCount==0) return false;
	//变量定义
	cbKindItemCount=0;
	ZeroMemory(KindItem,sizeof(KindItem));

	//拆分分析
	if (cbCardCount>=3)
	{
		if(cbTotalKingCount>=3)
		{
			if(cbKingCount>0&&cbViceKingCount>0)
			{
				ASSERT( cbKindItemCount < CountArray(KindItem) );
				KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(m_cbKingCardIndex[0]);
				KindItem[cbKindItemCount].cbCardIndex[0]=m_cbKingCardIndex[0];
				KindItem[cbKindItemCount].cbCardIndex[1]=m_cbKingCardIndex[0];
				KindItem[cbKindItemCount].cbCardIndex[2]=m_cbKingCardIndex[0];
				KindItem[cbKindItemCount++].wWeaveKind=WIK_PENG;
			}
		}
		for (BYTE i=0;i<MAX_INDEX;i++)
		{
			//同牌判断
			if (cbCardIndex[i]>=3)
			{
				ASSERT( cbKindItemCount < CountArray(KindItem) );
				KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
				KindItem[cbKindItemCount].cbCardIndex[0]=i;
				KindItem[cbKindItemCount].cbCardIndex[1]=i;
				KindItem[cbKindItemCount].cbCardIndex[2]=i;
				KindItem[cbKindItemCount++].wWeaveKind=WIK_PENG;
			}
			else if(cbCardIndex[i]==1)
			{
				//过滤精牌
				if(IsViceKingCardIndex(i)==false &&IsViceKingCardIndex(i)==false)
				{
					if(cbTotalKingCount>=2)
					{
						ASSERT( cbKindItemCount < CountArray(KindItem) );
						KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
						KindItem[cbKindItemCount].cbCardIndex[0]=i;
						KindItem[cbKindItemCount].cbCardIndex[1]=i;
						KindItem[cbKindItemCount].cbCardIndex[2]=i;
						KindItem[cbKindItemCount++].wWeaveKind=WIK_PENG;
					}
				}

			}
			else if(cbCardIndex[i]==2)
			{
				//过滤精牌
				if(IsViceKingCardIndex(i)==false &&IsViceKingCardIndex(i)==false)
				{
					if(cbTotalKingCount>=1)
					{
						ASSERT( cbKindItemCount < CountArray(KindItem) );
						KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
						KindItem[cbKindItemCount].cbCardIndex[0]=i;
						KindItem[cbKindItemCount].cbCardIndex[1]=i;
						KindItem[cbKindItemCount].cbCardIndex[2]=i;
						KindItem[cbKindItemCount++].wWeaveKind=WIK_PENG;
					}
				}

			}
			else
			{
				//什么也不执行
			}


			//连牌判断
			if ((i<(MAX_INDEX-2-7))&&((i%9)<7))
			{

				if (cbCardIndex[i]+cbCardIndex[i+1]+cbCardIndex[i+2]+cbTotalKingCount>=3)
				{
					BYTE cbIndex[3]={cbCardIndex[i],cbCardIndex[i+1],cbCardIndex[i+2]};
					INT cbTempTotalKingCount=cbTotalKingCount;

					while (cbIndex[0]+cbIndex[1]+cbIndex[2]+cbTotalKingCount>=3)
					{
						//过滤3精
						if(cbIndex[0]+cbIndex[1]+cbIndex[2]==0) break;

						//精牌组合
						for( BYTE j = 0; j < CountArray(cbIndex); j++ )
						{
							if( cbIndex[j] > 0 ) 
							{
								cbIndex[j]--;
							}
							else 
							{
								cbTempTotalKingCount--;
							}
						}
						if( cbTempTotalKingCount >= 0 )
						{
							KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
							KindItem[cbKindItemCount].cbCardIndex[0]=i;
							KindItem[cbKindItemCount].cbCardIndex[1]=i+1;
							KindItem[cbKindItemCount].cbCardIndex[2]=i+2;
							KindItem[cbKindItemCount++].wWeaveKind=WIK_LEFT;
						}
						else
							break;

					}
				}

			}

		}
		//东南西北
		for (BYTE i=MAX_INDEX-7;i<MAX_INDEX-5;i++)
		{
			//变量定义
			BYTE cbCardIndexTemp[MAX_INDEX];

			//设置变量
			CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));

			//东南西 南西北
			for (BYTE j=1;j<=cbCardIndex[i];j++)
			{
				if (cbCardIndex[i]+cbCardIndex[i+1]+cbCardIndex[i+2]+cbTotalKingCount>=3)
				{
					BYTE cbIndex[3]={cbCardIndex[i],cbCardIndex[i+1],cbCardIndex[i+2]};
					INT cbTempTotalKingCount=cbTotalKingCount;

					while (cbIndex[0]+cbIndex[1]+cbIndex[2]+cbTotalKingCount>=3)
					{
						//过滤3精
						if(cbIndex[0]+cbIndex[1]+cbIndex[2]==0) break;

						//精牌组合
						for( BYTE j = 0; j < CountArray(cbIndex); j++ )
						{
							if( cbIndex[j] > 0 ) 
							{
								cbIndex[j]--;
							}
							else 
							{
								cbTempTotalKingCount--;
							}
						}
						if( cbTempTotalKingCount >= 0 )
						{
							KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
							KindItem[cbKindItemCount].cbCardIndex[0]=i;
							KindItem[cbKindItemCount].cbCardIndex[1]=i+1;
							KindItem[cbKindItemCount].cbCardIndex[2]=i+2;
							KindItem[cbKindItemCount++].wWeaveKind=WIK_LEFT;
						}
						else
							break;

					}
				}
			}

			//东南北
			if(i==MAX_INDEX-7)
			{
				if (cbCardIndex[i]+cbCardIndex[i+1]+cbCardIndex[i+3]+cbTotalKingCount>=3)
				{
					BYTE cbIndex[3]={cbCardIndex[i],cbCardIndex[i+1],cbCardIndex[i+3]};
					BYTE cbTempTotalKingCount=cbTotalKingCount;

					while (cbIndex[0]+cbIndex[1]+cbIndex[2]+cbTotalKingCount>=3)
					{
						//过滤3精
						if(cbIndex[0]+cbIndex[1]+cbIndex[2]==0) break;

						//精牌组合
						for( BYTE j = 0; j < CountArray(cbIndex); j++ )
						{
							if( cbIndex[j] > 0 ) 
							{
								cbIndex[j]--;
							}
							else 
							{
								cbTempTotalKingCount--;
							}
						}
						if( cbTempTotalKingCount >= 0 )
						{
							KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
							KindItem[cbKindItemCount].cbCardIndex[0]=i;
							KindItem[cbKindItemCount].cbCardIndex[1]=i+1;
							KindItem[cbKindItemCount].cbCardIndex[2]=i+3;
							KindItem[cbKindItemCount++].wWeaveKind=WIK_DNBL;
						}

					}
				}

			}
			//东西北
			if(i==MAX_INDEX-7)
			{
				if (cbCardIndex[i]+cbCardIndex[i+2]+cbCardIndex[i+3]+cbTotalKingCount>=3)
				{
					BYTE cbIndex[3]={cbCardIndex[i],cbCardIndex[i+2],cbCardIndex[i+3]};
					INT cbTempTotalKingCount=cbTotalKingCount;

					while (cbIndex[0]+cbIndex[1]+cbIndex[2]+cbTotalKingCount>=3)
					{
						//过滤3精
						if(cbIndex[0]+cbIndex[1]+cbIndex[2]==0) break;

						//精牌组合
						for( BYTE j = 0; j < CountArray(cbIndex); j++ )
						{
							if( cbIndex[j] > 0 ) 
							{
								cbIndex[j]--;
							}
							else 
							{
								cbTempTotalKingCount--;
							}
						}
						if( cbTempTotalKingCount >= 0 )
						{
							KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
							KindItem[cbKindItemCount].cbCardIndex[0]=i;
							KindItem[cbKindItemCount].cbCardIndex[1]=i+2;
							KindItem[cbKindItemCount].cbCardIndex[2]=i+3;
							KindItem[cbKindItemCount++].wWeaveKind=WIK_DXBL;
						}
						else
							break;

					}
				}

			}


		}	
		//中发白
		for (BYTE i=MAX_INDEX-3;i<MAX_INDEX;i++)
		{
			if(i==MAX_INDEX-3)			
			{
				if (cbCardIndex[i]+cbCardIndex[i+1]+cbCardIndex[i+2]+cbTotalKingCount>=3)
				{
					BYTE cbIndex[3]={cbCardIndex[i],cbCardIndex[i+1],cbCardIndex[i+2]};
					INT cbTempTotalKingCount=cbTotalKingCount;

					while (cbIndex[0]+cbIndex[1]+cbIndex[2]+cbTotalKingCount>=3)
					{
						//过滤3精
						if(cbIndex[0]+cbIndex[1]+cbIndex[2]==0) break;

						//精牌组合
						for( BYTE j = 0; j < CountArray(cbIndex); j++ )
						{
							if( cbIndex[j] > 0 ) 
							{
								cbIndex[j]--;
							}
							else 
							{
								cbTempTotalKingCount--;
							}
						}
						if( cbTempTotalKingCount >= 0 )
						{
							KindItem[cbKindItemCount].cbCenterCard=SwitchToCardData(i);
							KindItem[cbKindItemCount].cbCardIndex[0]=i;
							KindItem[cbKindItemCount].cbCardIndex[1]=i+1;
							KindItem[cbKindItemCount].cbCardIndex[2]=i+2;
							KindItem[cbKindItemCount++].wWeaveKind=WIK_LEFT;
						}
						else
							break;

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
		BYTE cbIndex[MAX_WEAVE]={0,1,2,3};
		tagKindItem * pKindItem[MAX_WEAVE];
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
					if(cbCardIndexTemp[m_cbKingCardIndex[0]]==0)
					{
						if(cbCardIndexTemp[m_cbKingCardIndex[1]]==0)
						{
							bEnoughCard=false;
							break;

						}
						else
							cbCardIndexTemp[m_cbKingCardIndex[1]]--;

					}
					else
						cbCardIndexTemp[m_cbKingCardIndex[0]]--;
				}
				else 
					cbCardIndexTemp[cbCardIndex]--;
			}
			if(bEnoughCard)
			{
				return true;
			}

			//设置索引
			if (cbIndex[cbLessKindItem-1] == (cbKindItemCount-1))
			{
				for (BYTE i=cbLessKindItem-1;i>0;i--)
				{
					if ((cbIndex[i-1]+1) != cbIndex[i])
					{
						BYTE cbNewIndex = cbIndex[i-1];
						for (BYTE j=(i-1); j<cbLessKindItem; j++) 
						{
							cbIndex[j]=cbNewIndex+j-i+2;
						}
						break;
					}
				}
				if (i==0)
				{
					break;
				}
			}
			else 
			{
				cbIndex[cbLessKindItem-1]++;
			}



		} while (true);

	}


	return false;
}

// 对子 豪华对子
//是否全2
bool CGameLogic::IsNeat2(BYTE cbCardIndex[MAX_INDEX],tagWeaveItem WeaveItem[], BYTE cbWeaveCount)
{
	//倒牌过滤
	if(cbWeaveCount>0) return false;

	//德国判断
	for(BYTE i=0; i<MAX_INDEX; i++)
	{
		if(cbCardIndex[i] != 0 && cbCardIndex[i] != 2 &&cbCardIndex[i] != 4)
		{
			return false;
		}
	}

	return true;
}
//七小对牌
bool CGameLogic::IsQiXiaoDui(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount,const BYTE cbCurrentCard,BYTE &cbGerman,bool bZimo)
{
	//组合判断
	if (cbWeaveCount!=0) return false;

	//单牌数目
	BYTE cbReplaceCount = 0;

	//临时数据
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));
	
	//数目验证
	ASSERT(GetCardCount(cbCardIndexTemp)%2==0);
	if(GetCardCount(cbCardIndexTemp)%2 !=0) return false;
	
	//当前索引
	BYTE cbCurrentIndex = SwitchToCardIndex(cbCurrentCard);

	//计算单牌
	for (BYTE i=0;i<MAX_INDEX;i++)
	{
		BYTE cbCardCount=cbCardIndexTemp[i];

		//精牌过滤
		if( i == m_cbKingCardIndex[0] ) continue;
		if( i == m_cbKingCardIndex[1] ) continue;

		//单牌统计
		if( cbCardCount == 1 || cbCardCount == 3 ) 	cbReplaceCount++;
	}
    
	//自摸判断
	if(bZimo==true)
	{

		//精牌不够
		if( cbReplaceCount > (cbCardIndexTemp[m_cbKingCardIndex[0]]+cbCardIndexTemp[m_cbKingCardIndex[1]])) return false;

		//德国7星
		if ((cbReplaceCount==0)&&(cbCardIndexTemp[m_cbKingCardIndex[0]]%2==0)&&(cbCardIndexTemp[m_cbKingCardIndex[1]]%2==0 ))
		{
			cbGerman=1;
		}
		return true;
	}

	//放炮判断  财神
	if(bZimo==false &&(IsKingCardIndex(cbCurrentIndex)||IsViceKingCardIndex(cbCurrentIndex)))
	{
		//精牌不够
		if( (cbReplaceCount+2) > (cbCardIndexTemp[m_cbKingCardIndex[0]]+cbCardIndexTemp[m_cbKingCardIndex[1]])) return false;

		//德国7星
		if ((cbReplaceCount==0)&&(cbCardIndexTemp[m_cbKingCardIndex[0]]%2==0)&&(cbCardIndexTemp[m_cbKingCardIndex[1]]%2==0 ))
		{
			cbGerman=1;
		}

	}

	//放炮判断  财神
	if(bZimo==false &&((!IsKingCardIndex(cbCurrentIndex))&&(!IsViceKingCardIndex(cbCurrentIndex))))
	{
		//精牌不够
		if( (cbReplaceCount) > (cbCardIndexTemp[m_cbKingCardIndex[0]]+cbCardIndexTemp[m_cbKingCardIndex[1]])) return false;

		//德国7星
		if ((cbReplaceCount==0)&&(cbCardIndexTemp[m_cbKingCardIndex[0]]%2==0)&&(cbCardIndexTemp[m_cbKingCardIndex[1]]%2==0 ))
		{
			cbGerman=1;
		}

	}

	return true;

}

//7星
bool CGameLogic::ServenStar(BYTE cbCardIndex[MAX_INDEX],BYTE &cbGerman,BYTE cbCurrentCard,bool bZimo)
{

	cbGerman=0;
	//临时数据
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));

	//全不靠
	bool bRet = false;
	bool bAlone=IsNeatAlone(cbCardIndexTemp,cbGerman,cbCurrentCard,bZimo);
	if (bAlone==false)
	{
		return false;
	}

	//7风记录
	BYTE cbHasFeng[7];
	ZeroMemory(cbHasFeng,sizeof(cbHasFeng));
	for(int i=27; i<34; i++)
	{
		cbHasFeng[i-27]=cbCardIndexTemp[i];

		//重复风牌
		if(cbHasFeng[i-27]>1) return false;
	}

	//7星全
	int j=0;
	for (int i=0;i<7;i++)
	{
		if(cbHasFeng[i]>0)
			j++;
	}
	if(j==7)
	{
		return true;
	}
	return false;

}
//计算副精
BYTE CGameLogic::GetKingFromBrother(BYTE cbBrotherData)
{
	if(IsValidCard(cbBrotherData) == false)
	{
		return 0xFF;
	}
	BYTE cbColor = cbBrotherData & MASK_COLOR;
	BYTE cbValue = (cbBrotherData & MASK_VALUE) + 1;

	//万 索 筒
	if(cbColor != 0x30)
	{
		if(cbValue > 0x09)
		{
			cbValue = 0x01;
		}
	}
	else if(cbValue < 0x06)
	{
		if(cbValue > 0x04)
		{
			cbValue = 0x01;
		}
	}
	else
	{
		if(cbValue > 0x07)
		{
			cbValue = 0x05;
		}
	}

	return (cbColor | cbValue);
}
//////////////////////////////////////////////////////////////////////////
