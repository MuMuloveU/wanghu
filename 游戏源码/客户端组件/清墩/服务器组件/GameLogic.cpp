#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////
//静态变量

//索引变量
const BYTE cbIndexCount=5;

//扑克数据
const BYTE	CGameLogic::m_cbCardData[FULL_COUNT]=
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//方块 A - K
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//梅花 A - K
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//红桃 A - K
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,	//黑桃 A - K
	0x4E,0x4F,
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//方块 A - K
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//梅花 A - K
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//红桃 A - K
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,	//黑桃 A - K
	0x4E,0x4F,
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
	//逻辑变量
	m_cbMainValue=2;
	m_bFirst2=false;
	return;
}

//析构函数
CGameLogic::~CGameLogic()
{
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
	case 3:	//三牌
		{
			if(cbCardData[0]==cbCardData[1]&&cbCardData[0]==cbCardData[2]) return CT_QING_DUN;
			if (GetCardValue(cbCardData[0])==GetCardValue(cbCardData[2]))  return CT_THREE;
			return CT_ERROR;
		}
	}

	//分析扑克
	tagAnalyseResult AnalyseResult;
	AnalysebCardData(cbCardData,cbCardCount,AnalyseResult);

	//炸弹类型
	if ((cbCardCount==4)&&(AnalyseResult.cbBlockCount[cbCardCount-1]==1)) return CT_BOMB_4;
	if ((cbCardCount==5)&&(AnalyseResult.cbBlockCount[cbCardCount-1]==1)) return CT_BOMB_5;
	if ((cbCardCount>=6)&&(AnalyseResult.cbBlockCount[cbCardCount-1]==1))
	{
		if(cbCardCount==8)//八无墩
		{
			bool bHaveDun=false;
			for(BYTE i=0;i<6;i++)
			{
				if(cbCardData[i]==cbCardData[i+2]) bHaveDun=true;
			}
			if(bHaveDun==false) return CT_BOMB_8;
		}
		if(cbCardCount%3==0)//兄弟墩
		{
			bool bAllDun=true;
			for(BYTE i=0;i<=cbCardCount-3;i+=3)
			{
				if(cbCardData[i]!=cbCardData[i+2]) bAllDun=false;
			}
			if(bAllDun==true) return CT_XIONGDI_DUN;
		}
		return CT_BOMB_6;
	}

	//顺子类型
	if ((cbCardCount==5)&&(AnalyseResult.cbBlockCount[0]==cbCardCount))
	{
		//扑克属性
		BYTE cbSignedCount=AnalyseResult.cbBlockCount[0];
		BYTE cbCardColor=GetCardColor(AnalyseResult.cbCardData[0],cbSignedCount);
		bool bStructureLink=IsStructureLink(AnalyseResult.cbCardData[0],cbSignedCount,1);

		//类型判断
		if ((bStructureLink==true)&&(cbCardColor==0xF0)) return CT_SINGLE_LINK;
		if ((bStructureLink==true)&&(cbCardColor!=0xF0)) return CT_TONG_HUA_SHUN;
	}

	//对连类型
	if ((cbCardCount>=6)&&((AnalyseResult.cbBlockCount[1]*2)==cbCardCount))
	{
		BYTE cbDoubleCount=AnalyseResult.cbBlockCount[1]*2;
		if (IsStructureLink(AnalyseResult.cbCardData[1],cbDoubleCount,2)==true) return CT_DOUBLE_LINK;
	}

	//三带二型
	if ((cbCardCount==5)&&(AnalyseResult.cbBlockCount[1]==1)&&(AnalyseResult.cbBlockCount[2]==1)) 
	{
		BYTE bDoubleValue=GetCardValue(AnalyseResult.cbCardData[1][0]);
		BYTE bThreeValue=GetCardValue(AnalyseResult.cbCardData[2][0]);
		if(bThreeValue==bDoubleValue+1||bThreeValue==bDoubleValue-1|| (bDoubleValue==1&&bThreeValue==13) || (bDoubleValue==13&&bThreeValue==1)) 
			return CT_THREE_DOUBLE;
	}

	//蝴蝶
	if ((cbCardCount>=15)&&(AnalyseResult.cbBlockCount[1]==AnalyseResult.cbBlockCount[2])&&cbCardCount%5==0)
	{
		//检测是否连2连3
		bool lian3=false,lian2=false;
		BYTE cbThreeCount=AnalyseResult.cbBlockCount[2]*3;
		if (IsStructureLink(AnalyseResult.cbCardData[2],cbThreeCount,3)==true) lian3=true;
		BYTE cbDoubleCount=AnalyseResult.cbBlockCount[1]*2;
		if (IsStructureLink(AnalyseResult.cbCardData[1],cbDoubleCount,2)==true) lian2=true;
		
		if(cbThreeCount==0||cbDoubleCount==0) lian3=false;

		//若分别为连2连3
		if(lian3&&lian2)
		{
			BYTE bLinkCount=0;
			BYTE bLinkData[40];
			ZeroMemory(bLinkData,sizeof(bLinkData));

			for(BYTE i=0;i<cbThreeCount;i+=3)
			{
				bLinkData[bLinkCount++]=AnalyseResult.cbCardData[2][i];
			}
			for(BYTE i=0;i<cbDoubleCount;i+=2)
			{
				bLinkData[bLinkCount++]=AnalyseResult.cbCardData[1][i];
			}

			//全连才是蝴蝶
			SortCardList(bLinkData,bLinkCount,ST_ORDER);
			if(IsStructureLink(bLinkData,bLinkCount,1)==true) return CT_HU_DIE;
		}
	}

	//连墩
	if ((cbCardCount>=6)&&((AnalyseResult.cbBlockCount[2]*3)==cbCardCount))
	{
		//大小王连墩
		if( cbCardCount==6 && (AnalyseResult.cbCardData[2][0]&0x0F)==0x0F && (AnalyseResult.cbCardData[2][3]&0x0F)==0x0E )
			return CT_LIAN_DUN;
		BYTE cbThreeCount=AnalyseResult.cbBlockCount[2]*3;
		if (IsStructureLink(AnalyseResult.cbCardData[2],cbThreeCount,3)==true)
		{
			bool bSameColor=true;
			for(BYTE i=0;i<cbCardCount;i++)
			{
				if(GetCardColor(cbCardData[i])!=GetCardColor(cbCardData[0])) bSameColor=false;
			}
			if(bSameColor==true) return CT_LIAN_DUN;
		}
	}

	//三连类型
	if ((cbCardCount>=9)&&((AnalyseResult.cbBlockCount[2]*3)==cbCardCount))
	{
		BYTE cbThreeCount=AnalyseResult.cbBlockCount[2]*3;
		if (IsStructureLink(AnalyseResult.cbCardData[2],cbThreeCount,3)==true) return CT_THREE_LINK;
	}

	//排炮类型
	if (cbCardCount>=8)
	{
		BYTE byCellCount = 1;
		BYTE byLogicValue = (cbCardData[0]&0x0F);
		for( BYTE i = 1; i < cbCardCount; i++ )
			if( byLogicValue == (cbCardData[i]&0x0F) ) byCellCount++;
			else break;
		if(byCellCount<4) return CT_ERROR;
		if( cbCardCount == AnalyseResult.cbBlockCount[byCellCount-1]*byCellCount )
		{
			if( IsStructureLink(AnalyseResult.cbCardData[byCellCount-1],cbCardCount,byCellCount) ) return CT_PAI_PAO;
		}	
	}

	return CT_ERROR;
}

//获取花色
BYTE CGameLogic::GetCardColor(const BYTE cbCardData[], BYTE cbCardCount)
{
	//效验参数
	ASSERT(cbCardCount>0);
	if (cbCardCount==0) return 0xF0;

	//首牌花色
	BYTE cbCardColor=GetCardColor(cbCardData[0]);

	//花色判断
	for (BYTE i=0;i<cbCardCount;i++)
	{
		if (GetCardColor(cbCardData[i])!=cbCardColor) return 0xF0;
	}

	return cbCardColor;
}

//排列扑克
VOID CGameLogic::SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType)
{
	//排序过虑
	if (cbCardCount==0) return;
	if (cbSortType==ST_CUSTOM) return;

	//转换数值
	BYTE cbSortValue[MAX_COUNT];
	for (BYTE i=0;i<cbCardCount;i++)
	{
		switch (cbSortType)
		{
		case ST_COUNT:	//数目排序
		case ST_ORDER:	//等级排序
			{
				cbSortValue[i]=GetCardLogicValue(cbCardData[i]);
				break;
			}
		case ST_VALUE:	//数值排序
			{
				cbSortValue[i]=GetCardValue(cbCardData[i]);
				break;
			}
		case ST_COLOR:	//花色排序
			{
				cbSortValue[i]=GetCardColor(cbCardData[i])+GetCardLogicValue(cbCardData[i]);
				break;
			}
		}
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

	//数目排序
	if (cbSortType==ST_COUNT)
	{
		//变量定义
		BYTE cbCardIndex=0;

		//分析扑克
		tagAnalyseResult AnalyseResult;
		AnalysebCardData(cbCardData,cbCardCount,AnalyseResult);

		//提取扑克
		for (BYTE i=0;i<CountArray(AnalyseResult.cbBlockCount);i++)
		{
			//拷贝扑克
			BYTE cbIndex=CountArray(AnalyseResult.cbBlockCount)-i-1;
			CopyMemory(&cbCardData[cbCardIndex],AnalyseResult.cbCardData[cbIndex],AnalyseResult.cbBlockCount[cbIndex]*(cbIndex+1)*sizeof(BYTE));

			//设置索引
			cbCardIndex+=AnalyseResult.cbBlockCount[cbIndex]*(cbIndex+1)*sizeof(BYTE);
		}
	}

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
	BYTE cbCardValue=GetCardValue(cbCardData);

	//主牌扑克
	if (cbCardValue==m_cbMainValue) return 15;

	//逻辑数值
	switch (cbCardValue)
	{
	case 0x01: { return 14; }
	case 0x0E: { return 17; }
	case 0x0F: { return 18; }
	}

	//转换数值
	return cbCardValue;
}
//对比扑克
bool CGameLogic::CompareCard( const BYTE cbFirstCard,const BYTE cbNextCard )
{
	if(m_bFirst2==true&&GetCardValue(cbFirstCard)==2) 
		return GetCardValue(cbNextCard)>GetCardValue(cbFirstCard);
	return GetCardLogicValue(cbNextCard)>GetCardLogicValue(cbFirstCard);
}
//按变幻牌分析扑克
VOID CGameLogic::AnalyseMagicData( const BYTE cbHandCardData[],BYTE cbHandCardCount,tagMagicResult &MagicResult )
{
	//清0
	ZeroMemory( &MagicResult,sizeof(MagicResult) );
	//验证
	if( cbHandCardCount <= 0 ) return ;

	for( BYTE i = 0; i < cbHandCardCount; i++ )
	{
		BYTE cbCardValue = GetCardValue(cbHandCardData[i]);
		BYTE cbCardColor = GetCardColor(cbHandCardData[i]);
		if( cbCardValue==m_cbMainValue && cbCardColor==0x20 )
			MagicResult.cbMagicCard[MagicResult.cbMagicCardCount++] = cbHandCardData[i];
		else
			MagicResult.cbNormalCard[MagicResult.cbNormalCardCount++] = cbHandCardData[i];
	}
}
void CGameLogic::SetFirst2(bool bFirst2)
{
	m_bFirst2=bFirst2;return;
}
//对比点数
bool CGameLogic::CompareCardMaxValue( const BYTE cbFirstCard[],const BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount )
{
	BYTE bFirstMaxValue=0;
	BYTE bNextMaxValue=0;

	//前一手牌的最大
	for(BYTE i=0;i<cbFirstCount;i++)
	{
		if(GetCardValue(cbFirstCard[i])>bFirstMaxValue) bFirstMaxValue=GetCardValue(cbFirstCard[i]);
	}
	if(GetCardValue(cbFirstCard[0])==1&&GetCardValue(cbFirstCard[1])==13) bFirstMaxValue=14;

	//后一手牌的最大
	for(BYTE i=0;i<cbNextCount;i++)
	{
		if(GetCardValue(cbNextCard[i])>bNextMaxValue) bNextMaxValue=GetCardValue(cbNextCard[i]);
	}
	if(GetCardValue(cbNextCard[0])==1&&GetCardValue(cbNextCard[1])==13) bNextMaxValue=14;

	return bNextMaxValue>bFirstMaxValue;
}
//对比扑克
bool CGameLogic::CompareCard(const BYTE cbFirstCard[], const BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount)
{
	//类型判断
	BYTE cbNextType=GetCardType(cbNextCard,cbNextCount);
	BYTE cbFirstType=GetCardType(cbFirstCard,cbFirstCount);

	//2先出的特殊处理
	if(m_bFirst2)
	{
		//检测是否全是2
		bool bPure2=true;
		for(BYTE i=0;i<cbFirstCount;i++) if(GetCardValue(cbFirstCard[i])!=2) bPure2=false;

		if(bPure2)
		{
			//炸弹判断
			if(cbNextType>=CT_BOMB_4&&cbFirstType<CT_BOMB_4) return true;
			if(cbNextType>=CT_BOMB_4&&cbFirstType>=CT_BOMB_4&&cbNextType!=cbFirstType) return cbNextType>cbFirstType;
			if(cbNextType>=CT_BOMB_4&&cbFirstType>=CT_BOMB_4&&cbNextType==cbFirstType&&cbNextCount!=cbFirstCount) return cbNextCount>cbFirstCount;
			//牌型判断
			if(cbFirstType!=cbNextType) return false;
			if(GetCardValue(cbNextCard[0])==1) return true;
			return GetCardValue(cbNextCard[0])>=GetCardValue(cbFirstCard[0]);
		}
	}

	//第二类型牌
	if(cbNextType>=CT_BOMB_4)
	{
		//不同类型
		if(cbFirstType!=cbNextType)
		{
			return cbNextType>cbFirstType;
		}
		//相同类型
		if(cbFirstType==cbNextType)
		{
			//牌张数比较
			if(cbFirstCount!=cbNextCount) return cbNextCount>cbFirstCount;
			//排炮比较
			if(cbNextType==CT_PAI_PAO||cbNextType==CT_LIAN_DUN)
			{
				BYTE bSameCount=1;
				//先出的牌
				tagAnalyseResult AnalyseResultFirst;
				AnalysebCardData(cbFirstCard,cbFirstCount,AnalyseResultFirst);
				BYTE bFirstCount=1;
				for(BYTE i=0;i<12;i++)	
				{
					if(AnalyseResultFirst.cbBlockCount[i]>bFirstCount) 
					{
						bFirstCount=AnalyseResultFirst.cbBlockCount[i];
						bSameCount=i;
						break;
					}
				}
				//后出的牌
				tagAnalyseResult AnalyseResultNext;
				AnalysebCardData(cbNextCard,cbNextCount,AnalyseResultNext);
				BYTE bNextCount=1;
				for(BYTE i=0;i<12;i++)
				{
					if(AnalyseResultNext.cbBlockCount[i]>bNextCount) 
						bNextCount=AnalyseResultNext.cbBlockCount[i];
				}

				//连数长的大
				if(bFirstCount!=bNextCount)
				{
					return bNextCount>bFirstCount;
				}
				
				BYTE bFirstLogicValue=GetCardLogicValue(AnalyseResultFirst.cbCardData[bSameCount][cbFirstCount-1]);
				BYTE bNextLogicValue=GetCardLogicValue(AnalyseResultNext.cbCardData[bSameCount][cbNextCount-1]);
				return  bNextLogicValue>bFirstLogicValue;

			}
			if(cbNextType==CT_TONG_HUA_SHUN)
			{
				return CompareCardMaxValue(cbFirstCard,cbNextCard,cbFirstCount,cbNextCount);
			}
			//否则比较点数大小
			return GetCardLogicValue(cbNextCard[0])>GetCardLogicValue(cbFirstCard[0]);			
		}
	}


	//不同类型
	if (cbFirstType!=cbNextType) return false;

	//相同类型
	switch (cbFirstType)
	{
	case CT_SINGLE:				//单牌类型
	case CT_DOUBLE:				//对子类型
	case CT_THREE:				//三条类型
		{
			//变量定义
			BYTE cbConsultNext=GetCardLogicValue(cbNextCard[0]);
			BYTE cbConsultFirst=GetCardLogicValue(cbFirstCard[0]);

			return cbConsultNext>cbConsultFirst;
		}
	case CT_SINGLE_LINK:		//单连类型
	case CT_DOUBLE_LINK:		//对连类型
	case CT_THREE_LINK:			//三连类型
		{
			if(cbFirstCount!=cbNextCount) return false;
			return CompareCardByValue(cbFirstCard,cbNextCard,cbFirstCount,cbNextCount);
		}
	case CT_THREE_DOUBLE:		//三带对型
	case CT_HU_DIE:				//蝴蝶类型
		{
			if(cbFirstCount!=cbNextCount) return false;
			//分析扑克
			tagAnalyseResult AnalyseResultNext;
			tagAnalyseResult AnalyseResultFirst;
			AnalysebCardData(cbNextCard,cbNextCount,AnalyseResultNext);
			AnalysebCardData(cbFirstCard,cbFirstCount,AnalyseResultFirst);

			//变量定义
			BYTE cbThreeNext=GetCardLogicValue(AnalyseResultNext.cbCardData[2][0]);
			BYTE cbThreeFirst=GetCardLogicValue(AnalyseResultFirst.cbCardData[2][0]);
			BYTE cbDoubleNext=GetCardLogicValue(AnalyseResultNext.cbCardData[1][0]);
			BYTE cbDoubleFirst=GetCardLogicValue(AnalyseResultFirst.cbCardData[1][0]);
			
			//2葫芦的特殊处理
			if(cbNextType==CT_THREE_DOUBLE&&cbThreeNext==15&&cbDoubleNext==3)	//22233的葫芦
			{
				return false;		//非2带的葫芦
			}
			if(cbFirstType==CT_THREE_DOUBLE&&cbThreeFirst==15&&cbDoubleFirst==3)	//22233的葫芦
			{
				if(cbThreeNext!=15) return true;		//非2带的葫芦
			}

			//3个不同时比较3个的大小
			if(cbThreeNext!=cbThreeFirst) return cbThreeNext>cbThreeFirst;
			//3个相同时比较对牌的大小
			if(cbThreeNext==cbThreeFirst) return (cbDoubleFirst==15&&cbDoubleNext!=15)?true:(cbDoubleNext>cbDoubleFirst);
		}
	}

	//错误断言
	ASSERT(FALSE);
	
	return false;
}

//变幻扑克
bool CGameLogic::MagicCardData(BYTE cbCardData[], BYTE cbCardCount, BYTE cbResultCard[MAX_COUNT], BYTE cbCardType)
{
	//不变换
	//设置扑克
	CopyMemory(cbResultCard,cbCardData,cbCardCount*sizeof(BYTE));

	return false;

	//变量定义
	BYTE cbNormalCard[MAX_COUNT];
	BYTE cbMagicCardCount=0,cbNormalCardCount=0;

	//变幻准备
	for (BYTE i=0;i<cbCardCount;i++)
	{
		//扑克属性
		BYTE cbCardValue=GetCardValue(cbCardData[i]);
		BYTE cbCardColor=GetCardColor(cbCardData[i]);

		//变幻统计
		if ((cbCardValue==m_cbMainValue)&&(cbCardColor==0x20)) cbMagicCardCount++;
		else cbNormalCard[cbNormalCardCount++]=cbCardData[i];
	}

	//失败判断
	if ((cbMagicCardCount==0)||(cbNormalCardCount==0))
	{
		CopyMemory(cbResultCard,cbCardData,cbCardCount*sizeof(BYTE));
		return false;
	}

	//获取数值
	BYTE cbLogicValue=GetCardLogicValue(cbNormalCard[0]);

	//同牌变幻
	if (cbLogicValue<=15)
	{
		for (BYTE i=1;i<cbNormalCardCount;i++)
		{
			if (GetCardLogicValue(cbNormalCard[i])!=cbLogicValue) break;
		}
		if (i==cbNormalCardCount)
		{
			//设置结果
			for (BYTE j=0;j<cbMagicCardCount;j++) cbResultCard[j]=cbNormalCard[0];
			for (BYTE j=0;j<cbNormalCardCount;j++) cbResultCard[j+cbMagicCardCount]=cbNormalCard[j];

			return true;
		}
	}

	//扑克分析
	BYTE cbMaxSameCount=1,cbSameCount=1;
	for (BYTE i=1;i<cbNormalCardCount;i++)
	{
		//获取扑克
		BYTE cbCardValueTemp=GetCardLogicValue(cbNormalCard[i]);
		if (cbCardValueTemp==cbLogicValue) cbSameCount++;

		//结果处理
		if ((cbCardValueTemp!=cbLogicValue)||(i==(cbNormalCardCount-1)))
		{
			cbMaxSameCount=__max(cbSameCount,cbMaxSameCount);
		}

		//恢复变量
		if ((cbCardValueTemp!=cbLogicValue)&&(i!=(cbNormalCardCount-1)))
		{
			cbSameCount=1;
			cbLogicValue=cbCardValueTemp;
		}
	}

	//三带二型
	if ((cbCardCount==5)&&(cbMaxSameCount<=3))
	{
		//变量定义
		BYTE cbHeadCount=1;
		BYTE cbTailCount=1;

		//变量定义
		BYTE cbHeadValue=GetCardValue(cbNormalCard[0]);
		BYTE cbTailValue=GetCardValue(cbNormalCard[cbNormalCardCount-1]);

		//变幻判断
		for (BYTE i=1;i<cbNormalCardCount-1;i++)
		{
			//获取数值
			BYTE cbCardValue=GetCardValue(cbNormalCard[i]);

			//扑克判断
			if (cbCardValue==cbHeadValue) 
			{
				cbHeadCount++;
				continue;
			}

			//扑克判断
			if (cbCardValue==cbTailValue) 
			{
				cbTailCount++;
				continue;
			}

			break;
		}

		//成功判断
		if (i==(cbNormalCardCount-1))
		{
			//变化数值
			BYTE cbMagicValue=cbHeadValue;
			if (cbHeadCount==3) cbMagicValue=cbTailValue;

			//构造扑克
			CopyMemory(cbResultCard,cbNormalCard,cbHeadCount*sizeof(BYTE));
			CopyMemory(&cbResultCard[cbCardCount-cbTailCount],&cbNormalCard[cbHeadCount],cbTailCount*sizeof(BYTE));

			//变幻扑克
			for (BYTE j=0;j<cbMagicCardCount;j++)
			{
				cbResultCard[cbHeadCount+j]=MakeCardData(cbMagicValue-1,0);
			}

			return true;
		}
	}

	//连牌判断
	bool bMagicLink=false;
	if ((cbMaxSameCount==1)&&(cbCardCount==5)) bMagicLink=true;
	if (((cbMaxSameCount==2)||(cbMaxSameCount==3))&&(cbCardCount==6)) bMagicLink=true;

	//连牌变幻
	if (bMagicLink==true)
	{
		//分析分布
		tagDistributing Distributing;
		AnalysebDistributing(cbNormalCard,cbNormalCardCount,Distributing);

		//填充分析
		for (BYTE cbTimes=0;cbTimes<2;cbTimes++)
		{
			//变量定义
			BYTE cbFillCount=0;
			BYTE cbLeaveCount=cbNormalCardCount;
			BYTE cbUseableCount=cbMagicCardCount;

			//填充状态
			BYTE cbFillStatus[13];
			ZeroMemory(cbFillStatus,sizeof(cbFillStatus));

			//填充扑克
			for (BYTE i=0;i<14;i++)
			{
				//变量定义
				BYTE cbIndex=((cbTimes==1)?(13-i)%13:i)%13;

				//填充判断
				if ((cbLeaveCount!=cbNormalCardCount)||(Distributing.cbDistributing[cbIndex][cbIndexCount]!=0))
				{
					//失败判断
					if ((Distributing.cbDistributing[cbIndex][cbIndexCount]+cbUseableCount)<cbMaxSameCount) break;

					//变量定义
					BYTE cbTurnFillCount=0;

					//常规扑克
					for (BYTE j=0;j<4;j++)
					{
						for (BYTE k=0;k<Distributing.cbDistributing[cbIndex][j];k++)
						{
							cbLeaveCount--;
							cbTurnFillCount++;
							cbResultCard[cbFillCount++]=MakeCardData(cbIndex,j);
						}
					}

					//填充变幻
					for (BYTE i=cbTurnFillCount;i<cbMaxSameCount;i++)
					{
						cbUseableCount--;
						cbResultCard[cbFillCount++]=MakeCardData(cbIndex,GetCardColor(cbNormalCard[0])>>4);
					}

					//设置变量
					cbFillStatus[cbIndex]=cbMaxSameCount;
				}
			}

			//成功判断
			if ((cbUseableCount==0)&&(cbLeaveCount==0)) 
			{
				SortCardList(cbResultCard,cbCardCount,ST_ORDER);
				return true;
			}
		}
	}

	//设置扑克
	CopyMemory(cbResultCard,cbCardData,cbCardCount*sizeof(BYTE));

	return false;
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
		BYTE cbIndex=AnalyseResult.cbBlockCount[cbSameCount-1]++;
		for (BYTE j=0;j<cbSameCount;j++) AnalyseResult.cbCardData[cbSameCount-1][cbIndex*cbSameCount+j]=cbCardData[i+j];

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
		BYTE cbCardValue=GetCardValue(cbCardData[i]);

		//分布信息
		Distributing.cbCardCount++;
		Distributing.cbDistributing[cbCardValue-1][cbIndexCount]++;
		Distributing.cbDistributing[cbCardValue-1][cbCardColor>>4]++;
	}

	return;
}

//搜索蝴蝶
bool CGameLogic::SearchHuDie( const BYTE cbHandCardData[],BYTE cbHandCardCount,BYTE cbReferCard,tagOutCardResult &OutCardResult ,BYTE TurnCardCount )
{
	//牌数不够返回
	if(cbHandCardCount<TurnCardCount) return false;

	//保存手牌
	BYTE tempHandData[MAX_COUNT];
	ZeroMemory(tempHandData,sizeof(tempHandData));
	CopyMemory(tempHandData,cbHandCardData,sizeof(BYTE)*cbHandCardCount);

	//计算连数
	ASSERT(TurnCardCount%5==0);
	BYTE bLinkCount=TurnCardCount/5;

	//循环查找连3
	for(BYTE targetValue=GetCardValue(cbReferCard);targetValue<13;targetValue++)
	{
		//搜索连3
		bool HaveLian3=SearchLinkCard(tempHandData,cbHandCardCount,targetValue, CT_THREE_LINK,OutCardResult,bLinkCount*3);

		//有连3才继续检测，否则增大目标继续寻找
		if(HaveLian3==true)
		{
			//移除连3后再检测连2
			RemoveCard(OutCardResult.cbResultCard,bLinkCount*3,tempHandData,cbHandCardCount);
				
			//构造数据
			tagOutCardResult OutCardResult2;
			ZeroMemory(&OutCardResult2,sizeof(OutCardResult2));
			OutCardResult2.cbCardCount=bLinkCount*2;

			//连3的头尾
			BYTE bThreeValueMin=GetCardValue(OutCardResult.cbResultCard[8]);
			BYTE bThreeValueMax=GetCardValue(OutCardResult.cbResultCard[0]);

			//连2的头尾
			BYTE bDoubleValueMin=bThreeValueMax+1;
			BYTE bDoubleValueMax=bThreeValueMin-1;

			//向前搜索
			bool HaveLian2=SearchLinkCard(tempHandData,cbHandCardCount-bLinkCount*3,bDoubleValueMax-bLinkCount+2, CT_DOUBLE_LINK,OutCardResult2,bLinkCount*2);
			if(HaveLian2==true)
			{
				OutCardResult.cbCardCount=bLinkCount*5;
				CopyMemory(&OutCardResult.cbResultCard[bLinkCount*3],OutCardResult2.cbResultCard,sizeof(BYTE)*bLinkCount*2);

				//分析相连
				tagAnalyseResult ResultAnalyse;
				AnalysebCardData(OutCardResult.cbResultCard,OutCardResult.cbCardCount,ResultAnalyse);

				BYTE cbThreeCount=ResultAnalyse.cbBlockCount[2]*3;
				BYTE cbDoubleCount=ResultAnalyse.cbBlockCount[1]*2;

				BYTE bLinkCount=0;
				BYTE bLinkData[40];
				ZeroMemory(bLinkData,sizeof(bLinkData));

				for(BYTE i=0;i<cbThreeCount;i+=3)
				{
					bLinkData[bLinkCount++]=ResultAnalyse.cbCardData[2][i];
				}
				for(BYTE i=0;i<cbDoubleCount;i+=2)
				{
					bLinkData[bLinkCount++]=ResultAnalyse.cbCardData[1][i];
				}

				//全连才是蝴蝶
				SortCardList(bLinkData,bLinkCount,ST_ORDER);
				if(IsStructureLink(bLinkData,bLinkCount,1)==true) return true;
			}

			//向后搜索
			HaveLian2=SearchLinkCard(tempHandData,cbHandCardCount-bLinkCount*3,bDoubleValueMin-bLinkCount, CT_DOUBLE_LINK,OutCardResult2,bLinkCount*2);
			if(HaveLian2==true)
			{
				OutCardResult.cbCardCount=bLinkCount*5;
				CopyMemory(&OutCardResult.cbResultCard[bLinkCount*3],OutCardResult2.cbResultCard,sizeof(BYTE)*bLinkCount*2);

				//分析相连
				tagAnalyseResult ResultAnalyse;
				AnalysebCardData(OutCardResult.cbResultCard,OutCardResult.cbCardCount,ResultAnalyse);

				BYTE cbThreeCount=ResultAnalyse.cbBlockCount[2]*3;
				BYTE cbDoubleCount=ResultAnalyse.cbBlockCount[1]*2;

				BYTE bLinkCount=0;
				BYTE bLinkData[40];
				ZeroMemory(bLinkData,sizeof(bLinkData));

				for(BYTE i=0;i<cbThreeCount;i+=3)
				{
					bLinkData[bLinkCount++]=ResultAnalyse.cbCardData[2][i];
				}
				for(BYTE i=0;i<cbDoubleCount;i+=2)
				{
					bLinkData[bLinkCount++]=ResultAnalyse.cbCardData[1][i];
				}

				//全连才是蝴蝶
				SortCardList(bLinkData,bLinkCount,ST_ORDER);
				if(IsStructureLink(bLinkData,bLinkCount,1)==true) return true;
			}
		}
	}

	return false;
}
//搜索排炮
bool CGameLogic::SearchPaiPao( const BYTE cbHandCardData[],BYTE cbHandCardCount,const BYTE cbTurnCardData[], BYTE cbTurnCardCount,tagOutCardResult &OutCardResult )
{
	//牌数不够返回
	if(cbHandCardCount<cbTurnCardCount) return false;

	//分析扑克
	tagDistributing Distribute;
	AnalysebDistributing( cbHandCardData,cbHandCardCount,Distribute );

	//A的数据复制
	CopyMemory(Distribute.cbDistributing[13],Distribute.cbDistributing[0],sizeof(BYTE)*6);

	BYTE bCellCount=4;	//最小单元数为4
	BYTE bLinkCoun=2;	//最小连数为2

	for(bCellCount=4;bCellCount<8;bCellCount++) //单元数4~7 8个就有8无墩或者清墩 9个或以上一定有清墩
	{
		for(bLinkCoun=2;bLinkCoun<11;bLinkCoun++)//连数2~10
		{
			for(BYTE i=0;i<14;i++)//点数1到13(A到KA)
			{
				for(BYTE j=0;j<bLinkCoun;j++)
				{
					if((i+j)<15&&Distribute.cbDistributing[i+j][5]<bCellCount) break;
				}
				if(j==bLinkCoun)
				{
					//构造结果
					for( BYTE k = i; k < i + j; k++ )
					{
						BYTE bCount = 0;
						for( BYTE n = 0; n < 4; n++ )
						{
							if( Distribute.cbDistributing[k][n] > 0 )
							{
								for( BYTE m = 0; m < Distribute.cbDistributing[k][n]; m++ )
								{
									if(k==13)
									{
										OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=((n<<4)|1);
										//(cbColorIndex<<4)|(cbValueIndex+1)
									}
									else 
										OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = MakeCardData( k,n );
									if( ++bCount == bCellCount ) break;
								}
							}
							if( bCount == bCellCount ) break;
						}
					}
					//排序结果
					SortCardList(OutCardResult.cbResultCard,OutCardResult.cbCardCount,ST_ORDER);
					//比较为真就返回 否则继续搜索
					if(CompareCard(cbTurnCardData, OutCardResult.cbResultCard,cbTurnCardCount,OutCardResult.cbCardCount)==true)	return true;
					else 
					{
						ZeroMemory(&OutCardResult,sizeof(OutCardResult));
						continue;
					}
				}
			}
		}
	}
	

	
	return false;
}
//搜索洞牌
bool CGameLogic::SearchDong( const BYTE cbHandCardData[],BYTE cbHandCardCount,BYTE cbReferCard,tagOutCardResult &OutCardResult )
{
	BYTE tempCardData[MAX_COUNT];ZeroMemory(tempCardData,sizeof(tempCardData));
	CopyMemory(tempCardData,cbHandCardData,sizeof(BYTE)*cbHandCardCount);

	SortCardList(tempCardData,cbHandCardCount,ST_ORDER);

	for(BYTE i=cbHandCardCount;i>=2;i--)
	{
		if(tempCardData[i]==tempCardData[i-2])
		{
			if(GetCardLogicValue(tempCardData[i])>GetCardLogicValue(cbReferCard))
			{
				OutCardResult.cbCardCount=3;
				OutCardResult.cbResultCard[0]=tempCardData[i-2];
				OutCardResult.cbResultCard[1]=tempCardData[i-1];
				OutCardResult.cbResultCard[2]=tempCardData[i-0];
				return true;
			}
		}
	}
	return false;
}

//搜索八无墩
bool CGameLogic::SearchBomb8( const BYTE cbHandCardData[],BYTE cbHandCardCount,const BYTE cbTurnCardData[], BYTE cbTurnCardCount,tagOutCardResult &OutCardResult )
{
	//牌数不够返回
	if(cbHandCardCount<8) return false;

	//分析扑克
	tagDistributing Distribute;
	AnalysebDistributing( cbHandCardData,cbHandCardCount,Distribute );

	//循环搜索
	for(BYTE i=0;i<13;i++)
	{
		if(Distribute.cbDistributing[i][5]>=8)//有8张以上
		{
			if( Distribute.cbDistributing[i][0]>=2 && Distribute.cbDistributing[i][1]>=2 && Distribute.cbDistributing[i][2]>=2 && Distribute.cbDistributing[i][3]>=2)
			{
				OutCardResult.cbCardCount=8;
				OutCardResult.cbResultCard[0]=MakeCardData(i,0);
				OutCardResult.cbResultCard[1]=MakeCardData(i,0);
				OutCardResult.cbResultCard[2]=MakeCardData(i,1);
				OutCardResult.cbResultCard[3]=MakeCardData(i,1);
				OutCardResult.cbResultCard[4]=MakeCardData(i,2);
				OutCardResult.cbResultCard[5]=MakeCardData(i,2);
				OutCardResult.cbResultCard[6]=MakeCardData(i,3);
				OutCardResult.cbResultCard[7]=MakeCardData(i,3);
				if(CompareCard(cbTurnCardData,OutCardResult.cbResultCard,cbTurnCardCount,OutCardResult.cbCardCount)==true) return true;
			}
		}
	}

	return false;
}

//搜索连墩
bool CGameLogic::SearchLianDun( const BYTE cbHandCardData[],BYTE cbHandCardCount,const BYTE cbTurnCardData[], BYTE cbTurnCardCount,tagOutCardResult &OutCardResult )
{
	//牌数不够返回
	if(cbHandCardCount<cbTurnCardCount) return false;

	//分析扑克
	tagDistributing Distribute;
	AnalysebDistributing( cbHandCardData,cbHandCardCount,Distribute );

	//A的数据复制
	CopyMemory(Distribute.cbDistributing[13],Distribute.cbDistributing[0],sizeof(BYTE)*6);

	for(BYTE bLinkCount=2;bLinkCount<13;bLinkCount++)//连数2~13
	{
		for(BYTE i=0;i<14;i++)
		{
			for(BYTE bColor=0;bColor<4;bColor++)
			{
				for(BYTE j=0;j<bLinkCount;j++)
				{
					if((i+j)<15&&Distribute.cbDistributing[i+j][bColor]<3) break;
				}
				if(j==bLinkCount)
				{
					//构造结果		
					for( BYTE k = i; k < i + j; k++ )
					{
						//各取3张
						if(k==13)
						{
							OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=((bColor<<4)|1);
							OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=((bColor<<4)|1);
							OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=((bColor<<4)|1);
						}
						else 
						{
							OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = MakeCardData( k,bColor );	
							OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = MakeCardData( k,bColor );	
							OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = MakeCardData( k,bColor );	
						}
					}
					//排序结果
					SortCardList(OutCardResult.cbResultCard,OutCardResult.cbCardCount,ST_ORDER);
					//比较为真就返回 否则继续搜索
					if(CompareCard(cbTurnCardData, OutCardResult.cbResultCard,cbTurnCardCount,OutCardResult.cbCardCount)==true)	return true;
					else 
					{
						ZeroMemory(&OutCardResult,sizeof(OutCardResult));
						continue;
					}
				}
			}		
		}
	}
		
	return false;
}
//搜索兄弟墩
bool CGameLogic::SearchXiongdiDun( const BYTE cbHandCardData[],BYTE cbHandCardCount,const BYTE cbTurnCardData[], BYTE cbTurnCardCount,tagOutCardResult &OutCardResult )
{
	//牌数不够返回
	if(cbHandCardCount<cbTurnCardCount) return false;

	//分析扑克
	tagDistributing Distribute;
	AnalysebDistributing( cbHandCardData,cbHandCardCount,Distribute );

	//循环搜索
	for(BYTE i=0;i<13;i++)
	{
		//6张或以上才有兄弟墩
		if(Distribute.cbDistributing[i][5]>=6)
		{
			for(BYTE bColor=0;bColor<4;bColor++)
			{
				if(Distribute.cbDistributing[i][bColor]==3)
				{
					OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = MakeCardData( i,bColor );	
					OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = MakeCardData( i,bColor );	
					OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = MakeCardData( i,bColor );	
				}
			}
			//排序结果
			SortCardList(OutCardResult.cbResultCard,OutCardResult.cbCardCount,ST_ORDER);
			//比较为真就返回 否则继续搜索
			if(CompareCard(cbTurnCardData, OutCardResult.cbResultCard,cbTurnCardCount,OutCardResult.cbCardCount)==true)	return true;
			else 
			{
				ZeroMemory(&OutCardResult,sizeof(OutCardResult));
				continue;
			}
		}
	}
		
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
	SortCardList(cbCardData,cbCardCount,ST_ORDER);

	//获取类型
	BYTE cbTurnOutType=GetCardType(cbTurnCardData,cbTurnCardCount);

	tagAnalyseResult AnalyseResult;
	AnalysebCardData(cbCardData,cbCardCount,AnalyseResult);

	//出牌分析
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
		{
			//BYTE byTurnCardLogicValue = GetCardLogicValue(cbTurnCardData[0]);
			//for( BYTE i = 0; i < cbCardCount; i++ )
			//{
			//	if( GetCardLogicValue(cbCardData[cbCardCount-i-1]) > byTurnCardLogicValue )
			//	{
			//		OutCardResult.cbCardCount = 1;
			//		OutCardResult.cbResultCard[0] = cbCardData[cbCardCount-i-1];
			//		return true;
			//	}
			//}
			//从单牌搜索起
			//for( BYTE i = 0; i < CountArray(AnalyseResult.cbBlockCount); i++ )
			//{
			//	if( AnalyseResult.cbBlockCount[i] > 0 )
			//	{
			//		for( int j = AnalyseResult.cbBlockCount[i]-1; j >= 0 ; j-- )
			//		{
			//			if( CompareCard(cbTurnCardData[0],AnalyseResult.cbCardData[i][(i+1)*j]) )
			//			{
			//				OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = AnalyseResult.cbCardData[i][(i+1)*j];
			//				return true;
			//			}
			//		}
			//	}
			//}
			//return false;
		}
	case CT_DOUBLE:					//对牌
	case CT_THREE:					//三张牌
		{
			for( BYTE i = cbTurnCardCount-1; i < 3; i++ )
			{
				if( AnalyseResult.cbBlockCount[i] > 0 )
				{
					for( int j = AnalyseResult.cbBlockCount[i]-1; j >= 0; j-- )
					{
						//比较扑克
						if( CompareCard(cbTurnCardData,&AnalyseResult.cbCardData[i][(i+1)*j],cbTurnCardCount,cbTurnCardCount) )
						{
							//构造数据
							OutCardResult.cbCardCount = cbTurnCardCount;
							CopyMemory(OutCardResult.cbResultCard,&AnalyseResult.cbCardData[i][(i+1)*j],sizeof(BYTE)*(cbTurnCardCount));
							return true;
						}
					}
				}
			}
		}
	case CT_THREE_DOUBLE:			//葫芦
		{
			//只有出牌牌型对应,才搜索
			if( cbTurnOutType == CT_THREE_DOUBLE)
			{
				if( SearchHuLu(cbHandCardData,cbHandCardCount,cbTurnCardData[2],OutCardResult,cbTurnCardCount) )
				{
					ASSERT( CompareCard(cbTurnCardData,OutCardResult.cbResultCard,cbTurnCardCount,OutCardResult.cbCardCount) );
					return true;
				}
			}
		}
	case CT_HU_DIE:
		{
			//只有出牌牌型对应,才搜索
			if( cbTurnOutType == CT_HU_DIE)
			{
				tagAnalyseResult TurnAnalyseResult;
				AnalysebCardData(cbTurnCardData,cbTurnCardCount,TurnAnalyseResult);

				if( SearchHuDie(cbHandCardData,cbHandCardCount,TurnAnalyseResult.cbCardData[2][0],OutCardResult,cbTurnCardCount) )
				{
					ASSERT( CompareCard(cbTurnCardData,OutCardResult.cbResultCard,cbTurnCardCount,OutCardResult.cbCardCount) );
					return true;
				}
			}
		}
	case CT_SINGLE_LINK:			//单顺
	case CT_DOUBLE_LINK:			//双连
	case CT_THREE_LINK:				//三连
		{
			//只有出牌牌型对应,才搜索
			if( cbTurnOutType == CT_SINGLE_LINK || cbTurnOutType == CT_DOUBLE_LINK || cbTurnOutType == CT_THREE_LINK)
			{
				//寻找对比牌
				BYTE byReferCard = cbTurnCardData[0];
				if( GetCardValue(cbTurnCardData[0])==m_cbMainValue )
				{
					BYTE nCellCount = cbTurnOutType-4;
					if( GetCardValue(cbTurnCardData[nCellCount])<6 && GetCardValue(cbTurnCardData[cbTurnCardCount])<6 )
					{
						byReferCard = IsStructureLink(&cbTurnCardData[nCellCount],cbTurnCardCount-nCellCount,nCellCount)?
							MakeCardData(5,0):MakeCardData(4,0);
					}
					else if( IsStructureLink(&cbTurnCardData[nCellCount],cbTurnCardCount-nCellCount,nCellCount) )
					{
						byReferCard = GetCardValue(cbTurnCardData[nCellCount])==1?
							cbTurnCardData[nCellCount]:MakeCardData(GetCardValue(cbTurnCardData[nCellCount])%13,0);
					}
					else byReferCard = cbTurnCardData[nCellCount];
				}
				if( SearchLinkCard(cbCardData,cbCardCount,byReferCard,cbTurnOutType,OutCardResult,cbTurnCardCount) )
				{
					ASSERT( CompareCard(cbTurnCardData,OutCardResult.cbResultCard,cbTurnCardCount,OutCardResult.cbCardCount) );
					return true;
				}
			}
		}
	case CT_BOMB_4:					//炸弹
	case CT_BOMB_5:
		{
			//确定牌数
			BYTE bSameCount = 0;
			bSameCount = cbTurnOutType>=CT_BOMB_4?cbTurnCardCount:4;

			for( BYTE i = bSameCount-1; i < CountArray(AnalyseResult.cbBlockCount); i++ )
			{
				if( AnalyseResult.cbBlockCount[i] > 0 )
				{
					for( int j = AnalyseResult.cbBlockCount[i]-1; j >= 0; j-- )
					{
						if( CompareCard(cbTurnCardData,&AnalyseResult.cbCardData[i][j*(i+1)],cbTurnCardCount,i+1) )
						{
							OutCardResult.cbCardCount = i+1;
							CopyMemory(OutCardResult.cbResultCard,&AnalyseResult.cbCardData[i][j*(i+1)],sizeof(BYTE)*(i+1));
							ASSERT( CompareCard(cbTurnCardData,OutCardResult.cbResultCard,cbTurnCardCount,OutCardResult.cbCardCount) );
							return true;
						}
					}
				}
				//如果5张搜索不到,先搜索同花顺,再搜索6张
				if( i == 4 )
				{
					if( SearchTongHuaShun(cbCardData,cbCardCount,0,	OutCardResult) )
					{
						ASSERT( CompareCard(cbTurnCardData,OutCardResult.cbResultCard,cbTurnCardCount,OutCardResult.cbCardCount) );
						return true;
					}
				}
			}
		}
	case CT_TONG_HUA_SHUN:
		{
			//只有出牌牌型对应,才搜索
			if( cbTurnOutType==CT_TONG_HUA_SHUN )
			{
				//寻找对比牌
				BYTE byReferCard = cbTurnCardData[0];
				if( GetCardValue(cbTurnCardData[0])==m_cbMainValue )
				{
					BYTE nCellCount = 1;
					if( GetCardValue(cbTurnCardData[nCellCount])<6 && GetCardValue(cbTurnCardData[cbTurnCardCount])<6 )
					{
						byReferCard = IsStructureLink(&cbTurnCardData[nCellCount],cbTurnCardCount-nCellCount,nCellCount)?
							MakeCardData(5,0):MakeCardData(4,0);
					}
					else if( IsStructureLink(&cbTurnCardData[nCellCount],cbTurnCardCount-nCellCount,nCellCount) )
					{
						byReferCard = GetCardValue(cbTurnCardData[nCellCount])==1?
							cbTurnCardData[nCellCount]:MakeCardData(GetCardValue(cbTurnCardData[nCellCount])%13,0);
					}
					else byReferCard = cbTurnCardData[nCellCount];
				}
				if( SearchTongHuaShun(cbCardData,cbCardCount,byReferCard,OutCardResult) )
				{
					ASSERT( CompareCard(cbTurnCardData,OutCardResult.cbResultCard,cbTurnCardCount,OutCardResult.cbCardCount) );
					return true;
				}
			}
		}
	case CT_BOMB_6:
		{
			if(cbTurnOutType<=CT_BOMB_6)
			{
				for(BYTE i=5;i<8;i++)
				{
					if(AnalyseResult.cbCardData[i][0]!=0)
					{
						if(CompareCard(cbTurnCardData,AnalyseResult.cbCardData[i],cbTurnCardCount,i)==true)
						{
							OutCardResult.cbCardCount=i;
							CopyMemory(OutCardResult.cbResultCard,AnalyseResult.cbCardData[i],sizeof(BYTE)*i);
							return true;
						}
					}
				}
			}
		}
	case CT_PAI_PAO:
		{
			//只有出牌牌型对应,才搜索
			if( cbTurnOutType <= CT_PAI_PAO )
			{
				if(SearchPaiPao(cbCardData,cbCardCount,cbTurnCardData,cbTurnCardCount,OutCardResult))
				{
					ASSERT( CompareCard(cbTurnCardData,OutCardResult.cbResultCard,cbTurnCardCount,OutCardResult.cbCardCount) );
					return true;
				}
			}
		}
	case CT_QING_DUN:
		{
			//寻找墩牌
			BYTE byReferCard = cbTurnCardData[0];
			if( cbTurnOutType < CT_QING_DUN ) byReferCard=0;
			if( SearchDong(cbCardData,cbCardCount,byReferCard,OutCardResult) )
			{
				ASSERT( CompareCard(cbTurnCardData,OutCardResult.cbResultCard,cbTurnCardCount,OutCardResult.cbCardCount) );
				return true;
			}	
		}
	case CT_BOMB_8:
		{
			//寻找墩牌
			BYTE byReferCard = cbTurnCardData[0];
			if( cbTurnOutType < CT_BOMB_8 ) byReferCard=0;		
			if( SearchBomb8(cbCardData,cbCardCount,cbTurnCardData,cbTurnCardCount,OutCardResult) )
			{
				ASSERT( CompareCard(cbTurnCardData,OutCardResult.cbResultCard,cbTurnCardCount,OutCardResult.cbCardCount) );
				return true;
			}	
		}
	case CT_LIAN_DUN:
		{
			//搜索连墩
			if( cbTurnOutType <= CT_LIAN_DUN )
			{
				if( SearchLianDun(cbCardData,cbCardCount,cbTurnCardData,cbTurnCardCount,OutCardResult) )
				{
					ASSERT( CompareCard(cbTurnCardData,OutCardResult.cbResultCard,cbTurnCardCount,OutCardResult.cbCardCount) );
					return true;
				}	
			}
		}
	case CT_XIONGDI_DUN:
		{
			//搜索兄弟墩
			if( cbTurnOutType <= CT_XIONGDI_DUN )
			{
				if( SearchXiongdiDun(cbCardData,cbCardCount,cbTurnCardData,cbTurnCardCount,OutCardResult) )
				{
					ASSERT( CompareCard(cbTurnCardData,OutCardResult.cbResultCard,cbTurnCardCount,OutCardResult.cbCardCount) );
					return true;
				}	
			}
		}
	}

//	if( cbTurnOutType < CT_BOMB_TW )
//	{
//		//变幻搜索
//		if( SearchMagicOutCard(cbCardData,cbCardCount,cbTurnCardData,cbTurnCardCount,OutCardResult) )
//		{
//#ifdef _DEBUG
//			BYTE byCardData[MAX_COUNT];
//			ASSERT( MagicCardData(OutCardResult.cbResultCard,OutCardResult.cbCardCount,byCardData,0) );
//			ASSERT( CompareCard(cbTurnCardData,byCardData,cbTurnCardCount,OutCardResult.cbCardCount) );
//#endif
//			return true;
//		}
//		//搜索天王牌
//		if( cbTurnOutType < CT_BOMB_TW && SearchTianWang(cbCardData,cbCardCount,OutCardResult) )
//		{
//			ASSERT( CompareCard(cbTurnCardData,OutCardResult.cbResultCard,cbTurnCardCount,OutCardResult.cbCardCount) );
//			return true;
//		}
//	}
	return false;
}

//构造扑克
BYTE CGameLogic::MakeCardData(BYTE cbValueIndex, BYTE cbColorIndex)
{
	return (cbColorIndex<<4)|(cbValueIndex+1);
}

//是否连牌
bool CGameLogic::IsStructureLink(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbCellCount)
{
	//数目判断
	ASSERT((cbCardCount%cbCellCount)==0);
	if ((cbCardCount%cbCellCount)!=0) return false;
	
	//构造扑克
	BYTE cbCardDataTemp[MAX_COUNT];
	CopyMemory(cbCardDataTemp,cbCardData,cbCardCount*sizeof(BYTE));

	//SortCardList(cbCardDataTemp,cbCardCount,ST_ORDER);
	//if(GetCardValue(cbCardDataTemp[0])==2) return false;

	//扑克排序
	SortCardList(cbCardDataTemp,cbCardCount,ST_VALUE);


	//变量定义
	BYTE cbBlockCount=cbCardCount/cbCellCount;
	BYTE cbFirstValue=GetCardValue(cbCardDataTemp[0]);

	//无效过虑
	if (cbFirstValue>=14) return false;

	//扑克搜索
	for (BYTE i=1;i<cbBlockCount;i++)
	{
		//扑克数值
		BYTE cbCardValue=GetCardValue(cbCardDataTemp[i*cbCellCount]);

		//特殊过虑
		if ((cbCardValue==1)&&(cbFirstValue==13)) continue;

		//连牌判断
		if (cbFirstValue!=(cbCardValue+i)) return false;
	}

	//结果判断
	return true;
}

//对比扑克
bool CGameLogic::CompareCardByValue(const BYTE cbFirstCard[], const BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount)
{
	//变量定义
	bool bHaveTwoNext=false;
	BYTE cbConsultNext[2]={0x00,0x00};

	//参照扑克
	for (BYTE i=0;i<cbNextCount;i++)
	{
		//获取数值
		BYTE cbConsultValue=GetCardValue(cbNextCard[i]);

		//设置变量
		if ((bHaveTwoNext==false)&&(cbConsultValue==0x02)) bHaveTwoNext=true;

		//设置扑克
		if (cbConsultValue==0x01)
		{
			if (14>cbConsultNext[0]) cbConsultNext[0]=14;
			if (cbConsultValue>cbConsultNext[1]) cbConsultNext[1]=cbConsultValue;
		}
		else
		{
			if (cbConsultValue>cbConsultNext[0]) cbConsultNext[0]=cbConsultValue;
			if (cbConsultValue>cbConsultNext[1]) cbConsultNext[1]=cbConsultValue;
		}
	}

	//变量定义
	bool bHaveTwoFirst=false;
	BYTE cbConsultFirst[2]={0x00,0x00};

	//参照扑克
	for (BYTE i=0;i<cbFirstCount;i++)
	{
		//获取数值
		BYTE cbConsultValue=GetCardValue(cbFirstCard[i]);

		//设置变量
		if ((bHaveTwoFirst==false)&&(cbConsultValue==0x02)) bHaveTwoFirst=true;

		//设置扑克
		if (cbConsultValue==0x01)
		{
			if (14>cbConsultFirst[0]) cbConsultFirst[0]=14;
			if (cbConsultValue>cbConsultFirst[1]) cbConsultFirst[1]=cbConsultValue;
		}
		else
		{
			if (cbConsultValue>cbConsultFirst[0]) cbConsultFirst[0]=cbConsultValue;
			if (cbConsultValue>cbConsultFirst[1]) cbConsultFirst[1]=cbConsultValue;
		}
	}

	//对比扑克
	BYTE cbResultNext=(bHaveTwoNext==false)?cbConsultNext[0]:cbConsultNext[1];
	BYTE cbResultFirst=(bHaveTwoFirst==false)?cbConsultFirst[0]:cbConsultFirst[1];

	return cbResultNext>cbResultFirst;
}
//搜索同花顺
bool CGameLogic::SearchTongHuaShun( const BYTE cbHandCardData[],BYTE cbHandCardCount,BYTE cbReferCard,tagOutCardResult &OutCardResult )
{
	//A封顶,直接返回
	if( GetCardValue(cbReferCard) == 1 ) return false;
	if( cbHandCardCount < 5 ) return false;

	//分析扑克
	tagDistributing Distribute;
	AnalysebDistributing( cbHandCardData,cbHandCardCount,Distribute );

	//确定搜索开始位置
	BYTE byReferIndex = 0;
	if( cbReferCard != 0 ) 
		byReferIndex = GetCardValue(cbReferCard)-4;

	BYTE byColorCount[4] = {0,0,0,0};
	//搜索常规同花顺
	for( BYTE i = byReferIndex; i < 13; i++ )
	{
		if( Distribute.cbDistributing[i][cbIndexCount] == 0 ) 
		{
			ZeroMemory(byColorCount,sizeof(byColorCount));
			continue;
		}
		
		for( BYTE j = 0; j < 4; j++ )
		{
			if( Distribute.cbDistributing[i][j] == 0 )
			{
				byColorCount[j] = 0;
				continue;
			}
			//判断是否搜索足够
			if( ++byColorCount[j] == 5 )
			{
				OutCardResult.cbCardCount = 0;
				for( int k = i; k >= i-4; k-- )
					OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = MakeCardData(k,j);
				SortCardList(OutCardResult.cbResultCard,OutCardResult.cbCardCount,ST_ORDER);
				return true;
			}
		}
	}
	//搜索10到A的同花顺
	for( i = 0; i < 4; i++ )
	{
		//如果有顺
		if( byColorCount[i] == 4 &&
			Distribute.cbDistributing[0][i] > 0 )
		{
			OutCardResult.cbCardCount = 1;
			OutCardResult.cbResultCard[0] = MakeCardData(0,i);
			for( BYTE j = 12; j >= 9; j-- )
				OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = MakeCardData(j,i);
			return true;
		}
	}
	return false;
}

//搜索连牌
bool CGameLogic::SearchLinkCard( const BYTE cbHandCardData[],BYTE cbHandCardCount,BYTE cbReferCard,BYTE cbCardType,tagOutCardResult &OutCardResult  ,BYTE TurnCardCount)
{
	//A封顶,直接返回
	if( GetCardValue(cbReferCard) == 1 ) return false;


	//确定单元数据,连牌数目
	BYTE byCellCount,byLinkCount;
	switch( cbCardType )
	{
	case CT_SINGLE_LINK:
		{
			byCellCount = 1;
			byLinkCount = TurnCardCount/byCellCount;
			break;
		}
	case CT_DOUBLE_LINK:
		{
			byCellCount = 2;
			byLinkCount = TurnCardCount/byCellCount;
			break;
		}
	case CT_THREE_LINK:
		{
			byCellCount = 3;
			byLinkCount = TurnCardCount/byCellCount;
			break;
		}
	case CT_PAI_PAO:
		{
			byCellCount = 4;
			byLinkCount = TurnCardCount/byCellCount;
			break;	
		}
	default:
		ASSERT( FALSE );
		return false;
	}
	if( cbHandCardCount < byCellCount*byLinkCount ) return false;

	//确定搜索开始位置
	BYTE byReferIndex = 0;
	if( cbReferCard != 0 ) 
		byReferIndex = GetCardValue(cbReferCard)-byLinkCount+1;

	//分析扑克
	tagDistributing Distribute;
	AnalysebDistributing( cbHandCardData,cbHandCardCount,Distribute );

	//搜索常规顺子
	BYTE byTempLinkCount = 0;
	for( BYTE i = byReferIndex; i < 13; i++ )
	{
		if( Distribute.cbDistributing[i][cbIndexCount] < byCellCount )
		{
			byTempLinkCount = 0;
			continue;
		}
		//搜索到
		if( ++byTempLinkCount == byLinkCount )
		{
			//构造数据
			OutCardResult.cbCardCount = 0;
			for( int j = i; j >= i-byLinkCount+1; j-- )
			{
				BYTE byCount = 0;
				for( BYTE k = 3; ; k = (k+1)%4 )
				{
					if( Distribute.cbDistributing[j][k] > 0 )
					{
						for( BYTE n = 0; n < Distribute.cbDistributing[j][k]; n++ )
						{
							OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = MakeCardData(j,k);
							if( ++byCount == byCellCount ) break;
						}
						if( byCount == byCellCount ) break;
					}
				}
			}
			return true;
		}
	}
	//搜索到A的顺子
	if( byTempLinkCount == byLinkCount-1 && Distribute.cbDistributing[0][cbIndexCount] >= byCellCount )
	{
		//构造数据
		OutCardResult.cbCardCount = 0;
		BYTE byCount = 0;
		//放入A
		for( BYTE k = 3; ; k = (k+1)%4 )
		{
			if( Distribute.cbDistributing[0][k] > 0 )
			{
				for( BYTE n = 0; n < Distribute.cbDistributing[0][k]; n++ )
				{
					OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = MakeCardData(0,k);
					if( ++byCount == byCellCount ) break;
				}
				if( byCount == byCellCount ) break;
			}
		}
		//放入其他牌
		for( i = 12; i >= 14-byLinkCount; i-- )
		{
			byCount = 0;
			for( k = 3; ; k = (k+1)%4 )
			{
				if( Distribute.cbDistributing[i][k] > 0 )
				{
					for( BYTE n = 0; n < Distribute.cbDistributing[i][k]; n++ )
					{
						OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = MakeCardData(i,k);
						if( ++byCount == byCellCount ) break;
					}
					if( byCount == byCellCount ) break;
				}
			}
		}
		return true;
	}
	return false;
}

//搜索葫芦
bool CGameLogic::SearchHuLu( const BYTE cbHandCardData[],BYTE cbHandCardCount,BYTE cbReferCard,tagOutCardResult &OutCardResult  ,BYTE TurnCardCount)
{
	//验证
	if( cbHandCardCount < TurnCardCount ) return false;

	//分析扑克
	tagDistributing Distributing;
	AnalysebDistributing(cbHandCardData,cbHandCardCount,Distributing);

	//变量定义
	BYTE cbThreeIndex=0xFF;
	BYTE cbDoubleIndex=0xFF;

	//搜索葫芦,从2到K,除主牌外
	for (BYTE cbValueIndex=0;cbValueIndex<13;cbValueIndex++)
	{
		//排除主牌
		if( cbValueIndex+1 == m_cbMainValue ) continue;
		//搜索三条
		if ((cbThreeIndex==0xFF)&&(Distributing.cbDistributing[cbValueIndex][cbIndexCount]==3))		//注:不是大于3,因为大于3张,可以搜索出炸弹
		{
			//大小判断
			BYTE cbHeadCard=MakeCardData(cbValueIndex,0);
			if ((cbReferCard==0x00)||(CompareCard(cbReferCard,cbHeadCard)==true))
			{
				cbThreeIndex=cbValueIndex;
				//cbValueIndex = 0;
				//continue;
			}
		}

		if( cbThreeIndex <= 11&&( Distributing.cbDistributing[cbThreeIndex+1][5]>=2)&&( Distributing.cbDistributing[cbThreeIndex+1][5]<=3))
		{
			cbDoubleIndex = cbThreeIndex+1;
			break;
		}
		else if( cbThreeIndex >= 1&&Distributing.cbDistributing[cbThreeIndex-1][5]>=2&&Distributing.cbDistributing[cbThreeIndex-1][5]<=3 )
		{
			cbDoubleIndex = cbThreeIndex-1;
			break;
		}
		else if( cbThreeIndex != 0xFF ) cbThreeIndex = 0xFF;
		////搜索对牌
		//if ((cbDoubleIndex==0xFF)&&cbValueIndex!=cbThreeIndex&&(Distributing.cbDistributing[cbValueIndex][5]>=2)&&
		//	Distributing.cbDistributing[cbValueIndex][5]<=3)
		//{
		//	cbDoubleIndex=cbValueIndex;
		//	continue;
		//}

		//完成判断
		//if ((cbThreeIndex!=0xFF)&&(cbDoubleIndex!=0xFF))
		//{
		//	if(cbThreeIndex==(cbDoubleIndex+1) || cbThreeIndex==(cbDoubleIndex-1))
		//		break;
		//	else cbDoubleIndex = 0xFF;
		//}
	}
	//搜索A和主牌
	//cbValueIndex = 0xFF;
	//if( cbThreeIndex == 0xFF )
	//{
	//	if( Distributing.cbDistributing[0][cbIndexCount] == 3 )
	//		cbValueIndex = 0;
	//	else if( Distributing.cbDistributing[m_cbMainValue-1][cbIndexCount] == 3 )
	//		cbValueIndex = m_cbMainValue-1;
	//	if( cbValueIndex != 0xFF )
	//	{
	//		BYTE cbHeadCard = MakeCardData(0,0);
	//		if( (cbReferCard==0x00) || CompareCard(cbReferCard,cbHeadCard) )
	//			cbThreeIndex = cbValueIndex;
	//	}
	//}
	//cbValueIndex = 0xFF;
	//if( cbDoubleIndex == 0xFF )
	//{
	//	if( Distributing.cbDistributing[0][cbIndexCount] >= 2 && Distributing.cbDistributing[0][cbIndexCount] <= 3 
	//		&& 0 != cbThreeIndex )
	//		cbValueIndex = 0;
	//	else if( Distributing.cbDistributing[m_cbMainValue-1][cbIndexCount] >= 2 && Distributing.cbDistributing[m_cbMainValue-1][cbIndexCount] <= 3
	//		&& cbThreeIndex != m_cbMainValue-1)
	//		cbValueIndex = m_cbMainValue-1;
	//	if( cbValueIndex != 0xFF )
	//	{
	//		cbDoubleIndex = cbValueIndex;
	//	}
	//}

	//构造葫芦
	if ((cbThreeIndex!=0xFF)&&(cbDoubleIndex!=0xFF)&&cbThreeIndex==(cbDoubleIndex+1) || cbThreeIndex==(cbDoubleIndex-1))
	{
		//变量定义
		BYTE cbThreeCount=0;
		BYTE cbDoubleCount=0;

		//构造三条
		for (BYTE cbColorIndex=3;;cbColorIndex=(cbColorIndex+1)%4)
		{
			//扑克判断
			if (Distributing.cbDistributing[cbThreeIndex][cbColorIndex]==0) continue;
			
			//构造扑克
			//BYTE cbIndex=(cbThreeIndex>cbDoubleIndex)?0:2;
			BYTE cbIndex= 0;
			for( BYTE i = 0; i < Distributing.cbDistributing[cbThreeIndex][cbColorIndex]; i++ )
			{
				OutCardResult.cbResultCard[cbIndex+cbThreeCount++]=MakeCardData(cbThreeIndex,cbColorIndex);
				if( cbThreeCount == 3 ) break;
			}

			//完成判断
			if (cbThreeCount==3) break;
		}

		//构造对子
		for (BYTE cbColorIndex=3;;cbColorIndex=(cbColorIndex+1)%4)
		{
			//扑克判断
			if (Distributing.cbDistributing[cbDoubleIndex][cbColorIndex]==0) continue;
			
			//构造扑克
			//BYTE cbIndex=(cbThreeIndex>cbDoubleIndex)?3:0;
			BYTE cbIndex= 3;
			for( BYTE i = 0; i < Distributing.cbDistributing[cbDoubleIndex][cbColorIndex]; i++ )
			{
				OutCardResult.cbResultCard[cbIndex+cbDoubleCount++]=MakeCardData(cbDoubleIndex,cbColorIndex);
				if( cbDoubleCount == 2 ) break;
			}

			//完成判断
			if (cbDoubleCount==2) break;
		}
		OutCardResult.cbCardCount = 5;
		SortCardList(OutCardResult.cbResultCard,OutCardResult.cbCardCount,ST_ORDER);
		return true;
	}
	return false;
}

//搜索天王
bool CGameLogic::SearchTianWang( const BYTE cbHandCardData[],BYTE cbHandCardCount,tagOutCardResult &OutCardResult )
{
	//验证
	if( cbHandCardCount < 4 ) return false;

	//搜索
	if( cbHandCardData[0] == 0x4F && cbHandCardData[3] == 0x4E )
	{
		OutCardResult.cbCardCount = 4;
		CopyMemory(OutCardResult.cbResultCard,cbHandCardData,sizeof(BYTE)*4);
		return true;
	}
	return false;
}

//变幻搜索出牌
bool CGameLogic::SearchMagicOutCard( const BYTE cbHandCardData[],BYTE cbHandCardCount,const BYTE cbTurnCardData[],BYTE cbTurnCardCount,tagOutCardResult &OutCardResult )
{
	//获取牌型
	BYTE cbTurnCardType = GetCardType(cbTurnCardData,cbTurnCardCount);
	ASSERT( cbTurnCardType > CT_ERROR );
	//if( cbTurnCardType == CT_BOMB_TW ) return false;

	//根据牌型搜索出牌
	switch( cbTurnCardType )
	{
	case CT_SINGLE:				//单牌,跳到炸弹开始搜索
	case CT_DOUBLE:				//对牌
	case CT_THREE:				//三同张
		{
			BYTE cbSameCount = cbTurnCardType;
			//搜索同牌
			if(  cbTurnCardType!=CT_SINGLE && SearchMagicSameCard(cbHandCardData,cbHandCardCount,cbTurnCardData[0],cbSameCount,OutCardResult) )
				return true;
		}
	case CT_THREE_DOUBLE:		//葫芦
		{
			//只有出牌牌型为葫芦时,才进行搜索
			if( cbTurnCardType == CT_THREE_DOUBLE )
			{
				if( SearchMagicHuLu(cbHandCardData,cbHandCardCount,cbTurnCardData[0],OutCardResult) )
					return true;
			}
		}
	case CT_SINGLE_LINK:		//单连
	case CT_DOUBLE_LINK:		//双连
	case CT_THREE_LINK:			//三顺
		{
			//只有出牌牌型相对应,才搜索
			if( cbTurnCardType >= CT_SINGLE_LINK && cbTurnCardType <= CT_THREE_LINK )
			{
				//寻找对比牌
				BYTE byReferCard = cbTurnCardData[0];
				if( GetCardValue(cbTurnCardData[0])==m_cbMainValue )
				{				
					BYTE nCellCount = cbTurnCardType-3;
					if( GetCardValue(cbTurnCardData[nCellCount])<6 && GetCardValue(cbTurnCardData[cbTurnCardCount])<6 )
					{
						byReferCard = IsStructureLink(&cbTurnCardData[nCellCount],cbTurnCardCount-nCellCount,nCellCount)?
							MakeCardData(5,0):MakeCardData(4,0);
					}
					else if( IsStructureLink(&cbTurnCardData[nCellCount],cbTurnCardCount-nCellCount,nCellCount) )
					{
						byReferCard = GetCardValue(cbTurnCardData[nCellCount])==1?
							cbTurnCardData[nCellCount]:MakeCardData(GetCardValue(cbTurnCardData[nCellCount])%13,0);
					}
					else byReferCard = cbTurnCardData[nCellCount];
				}
				if( SearchMagicLink(cbHandCardData,cbHandCardCount,byReferCard,cbTurnCardType,OutCardResult) )
					return true;
			}
		}
	case CT_BOMB_4:				//炸弹
		{
			//确定炸弹数目
			BYTE cbSameCount = 0,cbReferCard = 0;
			cbSameCount = cbTurnCardType>=CT_BOMB_4?cbTurnCardCount:4;
			cbReferCard = cbTurnCardType>=CT_BOMB_4?cbTurnCardData[0]:0;

			//如果炸弹数小于等于5
			if( cbSameCount <= 5 )
			{
				if( SearchMagicSameCard(cbHandCardData,cbHandCardCount,cbReferCard,cbSameCount,OutCardResult) )
					return true;
				if( SearchMagicTongHuaShun(cbHandCardData,cbHandCardCount,0,OutCardResult) )
					return true;
			}
			//搜索同牌
			else if( SearchMagicSameCard(cbHandCardData,cbHandCardCount,cbReferCard,cbSameCount,OutCardResult) )
				return true;
			break;
		}
	}

	return false;
}

//变幻搜索葫芦
bool CGameLogic::SearchMagicHuLu( const BYTE cbHandCardData[],BYTE cbHandCardCount,BYTE cbReferCard,tagOutCardResult &OutCardResult )
{
	//验证
	if( cbHandCardCount < 5 ) return false;

	//变量定义
	tagMagicResult MagicResult;
	tagAnalyseResult AnalyseResult;
	
	//分析扑克
	AnalyseMagicData(cbHandCardData,cbHandCardCount,MagicResult);
	if( MagicResult.cbMagicCardCount == 0 ) return false;

	//分析扑克
	AnalysebCardData(MagicResult.cbNormalCard,MagicResult.cbNormalCardCount,AnalyseResult);

	//搜索
	BYTE byMagicCount = 0;
	if( AnalyseResult.cbBlockCount[1] > 0 )
	{
		BYTE byThreeIndex = 0xFF;
		for( int i = AnalyseResult.cbBlockCount[1]-1; i >= 0; i-- )
		{
			//排除大小王
			if( GetCardLogicValue(AnalyseResult.cbCardData[1][2*i]) > 14 ) continue;
			if( CompareCard(cbReferCard,AnalyseResult.cbCardData[1][2*i]) )
			{
				//构造数据
				byThreeIndex = i;
				OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = MagicResult.cbMagicCard[byMagicCount++];
				CopyMemory(&OutCardResult.cbResultCard[OutCardResult.cbCardCount],
					&AnalyseResult.cbCardData[1][2*i],sizeof(BYTE)*2);
				OutCardResult.cbCardCount += 2;
				break;
			}
		}
		//搜索2张失败时
		if( i < 0 )
		{
			if( MagicResult.cbMagicCardCount == 2 )
			{
				for( i = AnalyseResult.cbBlockCount[0]-1; i >= 0; i-- )
				{
					//排除大小王
					if( GetCardLogicValue(AnalyseResult.cbCardData[0][i]) > 14 ) continue;
					//构造数据
					if( CompareCard(cbReferCard,AnalyseResult.cbCardData[0][i]) )
					{
						CopyMemory(OutCardResult.cbResultCard,MagicResult.cbMagicCard,sizeof(BYTE)*2);
						byMagicCount = 2;
						OutCardResult.cbCardCount = 2;
						OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = AnalyseResult.cbCardData[0][i];
						break;
					}
				}
				if( i < 0 ) return false;
			}
			else return false;
		}
		//如果还有多余的两张或两张小于出牌者的三张
		if( AnalyseResult.cbBlockCount[1] > 1 || CompareCard(cbReferCard,AnalyseResult.cbCardData[1][0])==false )
		{
			for( int i = AnalyseResult.cbBlockCount[1]-1; i >= 0; i-- )
			{
				if( i != byThreeIndex )
				{
					CopyMemory(&OutCardResult.cbResultCard[OutCardResult.cbCardCount],
						&AnalyseResult.cbCardData[1][i*2],sizeof(BYTE)*2);
					OutCardResult.cbCardCount += 2;
					SortCardList(OutCardResult.cbResultCard,OutCardResult.cbCardCount,ST_ORDER);
					return true;
				}
			}
		}
		//填充两张部分
		else if( MagicResult.cbMagicCardCount > byMagicCount )
		{
			if( GetCardLogicValue(AnalyseResult.cbCardData[0][0]) <= 15 )
			{
				OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = MagicResult.cbMagicCard[byMagicCount++];
				OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = AnalyseResult.cbCardData[0][0];
				SortCardList(OutCardResult.cbResultCard,OutCardResult.cbCardCount,ST_ORDER);
				return true;
			}
		}
	}
	return false;
}

//变幻搜索连牌
bool CGameLogic::SearchMagicLink( const BYTE cbHandCardData[],BYTE cbHandCardCount,BYTE cbReferCard,BYTE cbCardType,tagOutCardResult &OutCardReslut )
{
	//A封顶,直接返回
	if( GetCardValue(cbReferCard) == 1 ) return false;

	//确定单元数,连牌数
	BYTE byCellCount,byLinkCount;
	switch(cbCardType)
	{
	case CT_SINGLE_LINK:
		{
			byCellCount = 1;
			byLinkCount = 5;
			break;
		}
	case CT_DOUBLE_LINK:
		{
			byCellCount = 2;
			byLinkCount = 3;
			break;
		}
	case CT_THREE_LINK:
		{
			byCellCount = 3;
			byLinkCount = 2;
			break;
		}
	default:
		ASSERT( FALSE );
		return false;
	}
	//手上牌少于必需数,直接返回
	if( byCellCount*byLinkCount > cbHandCardCount ) return false;

	//分析扑克
	tagMagicResult MagicResult;
	AnalyseMagicData(cbHandCardData,cbHandCardCount,MagicResult);
	if( MagicResult.cbMagicCardCount == 0 ) return false;

	//分析扑克
	tagDistributing Distribute;
	AnalysebDistributing( MagicResult.cbNormalCard,MagicResult.cbNormalCardCount,Distribute );

	//确定开始搜索位置
	BYTE byReferIndex = 0;
	if( cbReferCard != 0 ) 
		byReferIndex = GetCardValue(cbReferCard)-byLinkCount+1;

	//搜索
	for( BYTE byMagicCount = 1; byMagicCount <= MagicResult.cbMagicCardCount; byMagicCount++ )
	{
		//变量定义
		int nMagicCount = byMagicCount;
		BYTE byTmpLinkCount = 0;
		//搜索顺子
		BYTE i = byReferIndex;
		do
		{
			if( Distribute.cbDistributing[i][cbIndexCount] < byCellCount )
			{
				nMagicCount -= (byCellCount-Distribute.cbDistributing[i][cbIndexCount]);
				//判断是否足够
				if( nMagicCount < 0 )
				{
					byTmpLinkCount = 0;
					//可以填补时,向后追朔
					if( byMagicCount >= (byCellCount-Distribute.cbDistributing[i][cbIndexCount]) )
					{
						int nTmpMagicCount = byMagicCount;
						nTmpMagicCount -= (byCellCount-Distribute.cbDistributing[i][cbIndexCount]);
						byTmpLinkCount++;
						if( nTmpMagicCount > 0 )
						{
							BYTE byIndex = i-1;
							do
							{
								if( Distribute.cbDistributing[byIndex][cbIndexCount] < byCellCount )
								{
									nTmpMagicCount -= (byCellCount-Distribute.cbDistributing[i][cbIndexCount]);
									if( nTmpMagicCount < 0 ) break;
									byTmpLinkCount++;
								}
								else byTmpLinkCount++;
								byIndex--;					//注:byIndex始终大于byReferIndex
							}while(true);
						}
						nMagicCount = 0;
					}
					//不可填补时,向前继续搜索
					else nMagicCount = byMagicCount;

					i = (i+1)%13;
					if( i == 1 ) break;
					continue;
				}
				else byTmpLinkCount++;
			}
			else byTmpLinkCount++;
			//可变换
			if( byTmpLinkCount == byLinkCount )
			{
				//构造数据
				BYTE byIndex = (i==0?14-byLinkCount:i-byLinkCount+1);
				BYTE byCount = 0;
				nMagicCount = 0;
				do
				{
					BYTE byTmpCount = 0;
					if( Distribute.cbDistributing[byIndex][cbIndexCount] > 0 )
					{
						for( BYTE j = 0; j < 4; j++ )
						{
							if( Distribute.cbDistributing[byIndex][j] > 0 )
							{
								for( BYTE n = 0; n < Distribute.cbDistributing[byIndex][j]; n++ )
								{
									OutCardReslut.cbResultCard[OutCardReslut.cbCardCount++] = MakeCardData(byIndex,j);
									if( ++byTmpCount == byCellCount ) break;
								}
								if( byTmpCount == byCellCount ) break;
							}
						}
					}
					//填补
					if( byTmpCount < byCellCount )
					{
						for( BYTE k = 0; k < byCellCount-byTmpCount; k++ )
						{
							OutCardReslut.cbResultCard[OutCardReslut.cbCardCount++] = MagicResult.cbMagicCard[nMagicCount++];
							ASSERT( nMagicCount <= byMagicCount );
						}
					}
					byIndex = (byIndex+1)%13;
				}while( ++byCount < byLinkCount );
				//排序
				SortCardList(OutCardReslut.cbResultCard,OutCardReslut.cbCardCount,ST_ORDER);
				return true;
			}
			i = (i+1)%13;
		}while( i != 1 );		//搜索到A为止
	}
	return false;
}

//变幻搜索同张
bool CGameLogic::SearchMagicSameCard( const BYTE cbHandCardData[],BYTE cbHandCardCount,BYTE cbReferCard,BYTE cbLessSameCount,tagOutCardResult &OutCardResult )
{
	//验证
	if( cbHandCardCount < cbLessSameCount ) return false;

	//变量定义
	tagMagicResult MagicResult;
	tagAnalyseResult AnalyseResult;
	
	//分析扑克
	AnalyseMagicData(cbHandCardData,cbHandCardCount,MagicResult);
	if( MagicResult.cbMagicCardCount == 0 ) return false;

	//分析扑克
	AnalysebCardData(MagicResult.cbNormalCard,MagicResult.cbNormalCardCount,AnalyseResult);

	//搜索
	for( BYTE byMagicCount = 1; byMagicCount <= MagicResult.cbMagicCardCount; byMagicCount++ )
	{
		//确定搜索牌数位置
		int nIndex = cbLessSameCount-byMagicCount-1;
		if( nIndex < 0 ) break;
		if( AnalyseResult.cbBlockCount[nIndex] > 0 )
		{
			for( int i = AnalyseResult.cbBlockCount[nIndex]-1; i >= 0; i-- )
			{ 
				//大小王不能变幻
				if( GetCardLogicValue(AnalyseResult.cbCardData[nIndex][(nIndex+1)*i]) > 14 ) continue;
				//比较大小
				if( cbReferCard==0 || CompareCard(cbReferCard,AnalyseResult.cbCardData[nIndex][(nIndex+1)*i]) )
				{
					CopyMemory(OutCardResult.cbResultCard,MagicResult.cbMagicCard,sizeof(BYTE)*byMagicCount);
					OutCardResult.cbCardCount = byMagicCount;
					CopyMemory(&OutCardResult.cbResultCard[OutCardResult.cbCardCount],
						&AnalyseResult.cbCardData[nIndex][(nIndex+1)*i],sizeof(BYTE)*(nIndex+1));
					OutCardResult.cbCardCount += nIndex+1;
					SortCardList(OutCardResult.cbResultCard,OutCardResult.cbCardCount,ST_ORDER);
					return true;
				}
			}
		}
		//当有相同牌数,但小于cbReferCard时
		if( byMagicCount == 1 && nIndex != 0 /*确保不是在搜索两张牌*/  
			&& AnalyseResult.cbBlockCount[nIndex+1] > 0 )
		{
			ASSERT( CompareCard(AnalyseResult.cbCardData[nIndex+1][0],cbReferCard) );
			OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = MagicResult.cbMagicCard[0];
			CopyMemory(&OutCardResult.cbResultCard[OutCardResult.cbCardCount],
				&AnalyseResult.cbCardData[nIndex+1][(nIndex+2)*(AnalyseResult.cbBlockCount[nIndex+1]-1)],
				sizeof(BYTE)*(nIndex+2));
			OutCardResult.cbCardCount += nIndex+2;
			SortCardList(OutCardResult.cbResultCard,OutCardResult.cbCardCount,ST_ORDER);
			return true;
		}
		//当存在小于出牌者一张的相同牌,且有两个变幻牌时
		if( byMagicCount == 2 && AnalyseResult.cbBlockCount[nIndex+1] > 0 )
		{
			if( GetCardLogicValue(AnalyseResult.cbCardData[nIndex+1][(nIndex+2)*(AnalyseResult.cbBlockCount[nIndex+1]-1)]) < 15 )
			{
				CopyMemory(OutCardResult.cbResultCard,MagicResult.cbMagicCard,sizeof(BYTE)*byMagicCount);
				OutCardResult.cbCardCount = 2;
				CopyMemory(&OutCardResult.cbResultCard[OutCardResult.cbCardCount],
					&AnalyseResult.cbCardData[nIndex+1][(nIndex+2)*(AnalyseResult.cbBlockCount[nIndex+1]-1)],
					sizeof(BYTE)*(nIndex+2));
				SortCardList(OutCardResult.cbResultCard,OutCardResult.cbCardCount,ST_ORDER);
				return true;
			}
		}
	}

	return false;
}

//变幻搜索同花顺
bool CGameLogic::SearchMagicTongHuaShun( const BYTE cbHandCardData[],BYTE cbHandCardCount,BYTE cbReferCard,tagOutCardResult &OutCardResult )
{
	//A封顶,直接返回
	if( GetCardValue(cbReferCard) == 1 ) return false;
	if( cbHandCardCount < 5 ) return false;

	//分析扑克
	tagMagicResult MagicResult;
	AnalyseMagicData(cbHandCardData,cbHandCardCount,MagicResult);
	if( MagicResult.cbMagicCardCount == 0 ) return false;

	//分析扑克
	tagDistributing Distribute;
	AnalysebDistributing( MagicResult.cbNormalCard,MagicResult.cbNormalCardCount,Distribute );

	//确定开始搜索位置
	BYTE byReferIndex = 0;
	if( cbReferCard != 0 ) 
		byReferIndex = GetCardValue(cbReferCard)-4;

	//变量谜底
	BYTE byColorCount[4] = {0,0,0,0};
	BYTE byMagicIndex = 0xFF;
	BYTE byTmpMagicIndex = 0xFF;
	bool bTwice=byReferIndex==0?true:false;						//用于byReferIndex==0时
	for( BYTE byMagicCount = 1; byMagicCount <= MagicResult.cbMagicCardCount; byMagicCount++ )
	{
		//搜索常规同花顺
		BYTE i = byReferIndex;
		do
		{
			if( Distribute.cbDistributing[i][cbIndexCount] > 0 )
			{
				for( BYTE j = 0; j < 4; j++ )
				{
					if( Distribute.cbDistributing[i][j] == 0 ) continue;
					//可变换
					if( ++byColorCount[j] == 5 - byMagicCount )
					{
						//向后检查
						BYTE byInvalidCount = 0;	//缺位数
						BYTE byLianCount = 1;		//相连数
						BYTE byBeginIndex = i==0?12:i-1;
						BYTE byEndIndex = byBeginIndex-3;
						for( BYTE k = byBeginIndex; k >= byEndIndex; k-- )
						{
							if( Distribute.cbDistributing[k][cbIndexCount]==0 || Distribute.cbDistributing[k][j]==0 )
							{
								if( ++byInvalidCount > byMagicCount ) break;
								if( byLianCount != 0 )
								{
									byColorCount[j] = byLianCount;
									byLianCount = 0;
								}
							}
							else if( byLianCount != 0 ) byLianCount++;
						}
						//真正可变换
						if( byInvalidCount <= byMagicCount )
						{
							//定位变换位置
							k = byEndIndex;
							if( byColorCount[j] == 5-byMagicCount ) k += byMagicCount;
							//构造数据
							BYTE byCount = 0;
							BYTE byMagicIndex = 0;
							do
							{
								if( Distribute.cbDistributing[k][cbIndexCount]>0 && Distribute.cbDistributing[k][j]>0 )
									OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = MakeCardData(k,j);
								else OutCardResult.cbResultCard[OutCardResult.cbCardCount++] = MagicResult.cbMagicCard[byMagicIndex++];
								k = (k+1)%13;
							}while( ++byCount < 5 );
							//排序
							SortCardList(OutCardResult.cbResultCard,OutCardResult.cbCardCount,ST_ORDER);
							return true;
						}
					}
				}
			}
			if( i==1 && bTwice ) bTwice = false;
			i = (i+1)%13;
		}while( i!=1 || bTwice );
	}
	return false;
}


//////////////////////////////////////////////////////////////////////////
