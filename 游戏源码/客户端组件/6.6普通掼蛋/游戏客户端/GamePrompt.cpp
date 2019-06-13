#include "StdAfx.h"
#include ".\gameprompt.h"



//构造函数
CGamePrompt::CGamePrompt(void)
{
    //辅助变量
	m_bTurnCardType=CT_ERROR;
	m_bHongTaoCount=0;
	m_bCardCount=0;
	ZeroMemory(m_bHongTaoLocation,sizeof(m_bHongTaoLocation));
	ZeroMemory(m_bCardData,sizeof(m_bCardData));
	ZeroMemory(m_bCardPosition,sizeof(m_bCardPosition));
	ZeroMemory(desiCardData,sizeof(desiCardData));
	ZeroMemory(CardData,sizeof(CardData));
}


//析构函数
CGamePrompt::~CGamePrompt(void)
{
}

//分析提示
void   CGamePrompt::Prompt()
{
	m_bHongTaoCount=0;
	
	//计算红桃主牌数
    for(BYTE i=0,j=0;i<m_bCardCount;i++)
	{ 
		
		if(m_bCardData[i]==m_bSeries)
		{
            m_bHongTaoLocation[m_bHongTaoCount]=i;
			m_bHongTaoCount++;
		}else
		{
			m_bCardPosition[j]=i;
			j++;
		}
	}
	
	//删除红桃
	BYTE   card2[]={m_bSeries,m_bSeries};
	RemoveCard(card2,m_bHongTaoCount,m_bCardData,m_bCardCount);
	m_bCardCount=m_bCardCount-m_bHongTaoCount;

	BYTE cbLogicValue[MAX_COUNT];
	for (BYTE i=0;i<m_bCardCount;i++)
	{
		cbLogicValue[i]		=	GetCardLogicValue(m_bCardData[i]);
	}

	//排序操作
	bool bSorted;
	BYTE cbTempData,bLast=m_bCardCount-1;
	do
	{
		bSorted=false;
		for (BYTE i=0;i<bLast;i++)
		{
			if (cbLogicValue[i]<cbLogicValue[i+1])
			{
				//交换位置
				cbTempData=m_bCardData[i];
				m_bCardData[i]=m_bCardData[i+1];
				m_bCardData[i+1]=cbTempData;
				
				cbTempData=cbLogicValue[i];
				cbLogicValue[i]=cbLogicValue[i+1];
				cbLogicValue[i+1]=cbTempData;
                
				cbTempData			= m_bCardPosition[i];
				m_bCardPosition[i]	= m_bCardPosition[i+1];
				m_bCardPosition[i+1]= cbTempData;

				bSorted=true;
			}	
		}
		bLast--;
	} while(bSorted);
  
  //排值相同排数
  BYTE bStart=0,bRound=0,bNext=0;
  while(bNext<m_bCardCount)
  {
	  BYTE i=1;
	  bStart=bNext;
	  while(bNext<(m_bCardCount-1)&&(cbLogicValue[bNext]==cbLogicValue[bNext+1])) 
	  {
		  i++;
		  bNext++;
	  }

	  CardData[bRound].bCardCount		=i;
	  CardData[bRound].bCardLocation	=bStart;
	  CardData[bRound].bCardLogicValue	=GetCardLogicValue(m_bCardData[bStart]);
	  CardData[bRound].bCardValue		=m_bCardData[bStart];
	  bRound++;
	  bNext++;
  }
  m_bRound=bRound;
  CopyMemory(desiCardData,CardData,sizeof(CardData));
  
  //排序
  bSorted=true;
  bLast=m_bRound-1;
  CardNumber bCardTemp;
  do
  {
	  bSorted=false;
	  for(BYTE i=0;i<bLast;i++)
	  {
		  if(CardData[i].bCardCount<CardData[i+1].bCardCount)
		  {
			  bCardTemp		=	CardData[i];
			  CardData[i]	=	CardData[i+1];
			  CardData[i+1]	=	bCardTemp;

			  bSorted=true;
		  }
	  }
	  bLast--;
  }while(bSorted);
  //开始分析
	//分析单条
	GetSingle();
	//分析对子
	GetDouble();
	//分析santiao
	GetSanTiao();
	//分析葫芦
	GetHulu();
	//分析两联对
	GetLiangLianDui();
	//分析顺子
	GetShunZhi();
	//分析钢板
	GetGangBan();
	//分析四炸弹
	GetSiZhangBomb();
	//分析五炸弹
	GetWuZhangBomb();
	//分析同花顺
	GetTongHuaShun();
	//分析同花顺
	GetTongHuaShun(13);
	//分析六炸弹
	GetLiuZhangBomb();
	//分析七炸弹
	GetQiZhangBomb();
	//分析八炸弹
	 GetBaZhangBomb();
	//分析九炸弹
	 GetJiuZhangBomb();
	//分析十炸弹
	GetShiZhangBomb();
	//分析四王
	GetSiWang();

}
    
//分析单条
void  CGamePrompt::GetSingle()
{
	//类型判断
	if(((BYTE)m_bTurnCardType)!=CT_SINGLE)
		return;
	
	//分析处理
	BYTE  bFirstValue=(m_bTurnCardType>>8);
    for(int  i=m_bRound-1; i>=0; i--)
	{
		if(CardData[i].bCardLogicValue>bFirstValue)
		{
            
			CardType   card	;
			card.bCardCount			=1;
			card.bCardType			=CT_SINGLE;
			card.bCardLocation[0]	=m_bCardPosition[CardData[i].bCardLocation];
            
			OutCardList.Add(card);
		}
	}
}


//分析对子
void  CGamePrompt::GetDouble()
{
	//分析判断
	if(((BYTE)m_bTurnCardType)!=CT_DOUBLE)
		return;
	
	//分析处理
	BYTE bFirstValue=(m_bTurnCardType>>8);
    for(int  i=m_bRound-1;i>=0 ; i--)
	{
		if(CardData[i].bCardCount>3) break;
		if(CardData[i].bCardCount>=2&&CardData[i].bCardCount<=3&&CardData[i].bCardLogicValue>bFirstValue)
		{
			  
			  CardType  card;		
			  card.bCardCount			=	2;
			  card.bCardType			=	CT_DOUBLE;
			  card.bCardLocation[0]		=	m_bCardPosition[CardData[i].bCardLocation];
			  card.bCardLocation[1]		=	m_bCardPosition[CardData[i].bCardLocation+1];

			  OutCardList.Add(card);
		}

	}
}
	

//分析santiao
void   CGamePrompt::GetSanTiao()
{
    //类型判断
	if(((BYTE)m_bTurnCardType)!=CT_THREE_TIAO)
		return;
	
	//分析处理
	BYTE bFirstValue=(m_bTurnCardType>>8);
    for(int  i=m_bRound-1;i>=0 ; i--)
	{
		if(CardData[i].bCardCount>3) break;
		if(CardData[i].bCardCount==3&&CardData[i].bCardLogicValue>bFirstValue)
		{
			           
				CardType  card			;
				card.bCardCount			=	3;
				card.bCardType			=	CT_THREE_TIAO;
				card.bCardLocation[0]		=	m_bCardPosition[CardData[i].bCardLocation];
				card.bCardLocation[1]		=	m_bCardPosition[CardData[i].bCardLocation+1];
				card.bCardLocation[2]		=   m_bCardPosition[CardData[i].bCardLocation+2];

				 OutCardList.Add(card);
		}

	}

}
//分析葫芦
void   CGamePrompt::GetHulu()
{
	//类型判断
	if(((BYTE)m_bTurnCardType)!=CT_HU_LU)
		return ;
	
	//分析处理
	BYTE bFirstValue=(m_bTurnCardType>>8);
	for(int  i=m_bRound-1;i>=0;i--)
	{
		if(CardData[i].bCardCount>3) break;
		if(CardData[i].bCardCount==3&&CardData[i].bCardLogicValue>bFirstValue)
		{
			for(int j=m_bRound-1;j>=0;j--)
			{
				if(CardData[j].bCardCount>3) break;
				if(j!=i&&(CardData[j].bCardCount==2||CardData[j].bCardCount==3))
				{
					CardType  card;
					card.bCardCount			=	5;
					card.bCardType			=	CT_HU_LU;
					card.bCardLocation[0]	=	m_bCardPosition[CardData[i].bCardLocation];
					card.bCardLocation[1]	=	m_bCardPosition[CardData[i].bCardLocation+1];
					card.bCardLocation[2]	=   m_bCardPosition[CardData[i].bCardLocation+2];
					card.bCardLocation[3]	=   m_bCardPosition[CardData[j].bCardLocation];
					card.bCardLocation[4]	=	m_bCardPosition[CardData[j].bCardLocation+1];

					OutCardList.Add(card);
                    
				}

			}
	   }
	}
}

//分析两连对
void  CGamePrompt::GetLiangLianDui()
{
    //类型判断
	if(((BYTE)m_bTurnCardType)!=CT_LIANG_LIAN_DUI)
		return ;
	if(m_bRound<3) return ;
	
	//分析处理
	BYTE bFirstValue=(m_bTurnCardType>>8);
	for(int i=m_bRound-3;i>=0;i--)
    {
		if(CardData[i].bCardLogicValue<=bFirstValue)
			continue;
		if(CardData[i].bCardLogicValue>=15) continue;  
		if(CardData[i].bCardCount>=3) break;
		if(CardData[i].bCardCount==2&&CardData[i+1].bCardCount==2&&CardData[i+2].bCardCount==2)
			if(CardData[i].bCardLogicValue==(CardData[i+1].bCardLogicValue+1))
				if(CardData[i+1].bCardLogicValue==(CardData[i+2].bCardLogicValue+1))
				{
				         CardType card;
						 card.bCardCount		=	6;
						 card.bCardType			=	CT_LIANG_LIAN_DUI;
						 card.bCardLocation[0]	=	m_bCardPosition[CardData[i].bCardLocation];
						 card.bCardLocation[1]	=	m_bCardPosition[CardData[i].bCardLocation+1];
						 card.bCardLocation[2]	=	m_bCardPosition[CardData[i+1].bCardLocation];
						 card.bCardLocation[3]	=	m_bCardPosition[CardData[i+1].bCardLocation+1];
						 card.bCardLocation[4]	=	m_bCardPosition[CardData[i+2].bCardLocation];
						 card.bCardLocation[5]	=	m_bCardPosition[CardData[i+2].bCardLocation+1];

						 OutCardList.Add(card);

				} 

	} 
}

//分析顺子
void  CGamePrompt::GetShunZhi()
{
    //类型判断
	if((BYTE(m_bTurnCardType))!=CT_SHUN_ZI)	return;
	if(m_bRound<5)							return;

	//开始分析
	BYTE		bFirstValue=(m_bTurnCardType>>8);
	for(int i=m_bRound-1;i>=0;i--)
	{
		if(desiCardData[i].bCardLogicValue>=16) break;
		if(desiCardData[i].bCardLogicValue<=bFirstValue)
		   continue;
		if(	desiCardData[i].bCardCount>2)
		{ 
			i=i-4;
			continue;
		}
		
		if((i+4)<m_bRound)
		{   
			BYTE l=1;
            while(desiCardData[i].bCardLogicValue==(desiCardData[i+l].bCardLogicValue+l))
			{
				l++;
				if(l==5) break;
			}
			if(l==5)
			{
                 
					ASSERT(l==5);	
					CardType  card;
					card.bCardCount			=	5;
					card.bCardType			=	CT_SHUN_ZI;
					card.bCardLocation[0]	=	m_bCardPosition[desiCardData[i].bCardLocation];
					card.bCardLocation[1]	=	m_bCardPosition[desiCardData[i+1].bCardLocation];
					card.bCardLocation[2]	=   m_bCardPosition[desiCardData[i+2].bCardLocation];
					card.bCardLocation[3]	=   m_bCardPosition[desiCardData[i+3].bCardLocation];
					card.bCardLocation[4]	=	m_bCardPosition[desiCardData[i+4].bCardLocation];

					OutCardList.Add(card);
			}
		}
	}
}
//分析钢板
void  CGamePrompt::GetGangBan()
{
       //类型判断
	if(((BYTE)m_bTurnCardType)!=CT_GANG_BAN)
		return ;
	
	//分析处理
	BYTE bFirstValue=(m_bTurnCardType>>8);
	for(int  i=m_bRound-1;i>=0;i--)
	{
		if(CardData[i].bCardLogicValue>=16) continue;    
		if(CardData[i].bCardCount==3&&CardData[i].bCardLogicValue>bFirstValue)
		{
			for(int j=m_bRound-1;j>=0;j--)
			{
				if(j!=i&&CardData[j].bCardCount==3&&(CardData[i].bCardLogicValue==(CardData[j].bCardLogicValue+1)))
				{
					CardType  card;
					card.bCardCount			=	6;
					card.bCardType			=	CT_GANG_BAN;
					card.bCardLocation[0]	=	m_bCardPosition[CardData[i].bCardLocation];
					card.bCardLocation[1]	=	m_bCardPosition[CardData[i].bCardLocation+1];
					card.bCardLocation[2]	=   m_bCardPosition[CardData[i].bCardLocation+2];
					card.bCardLocation[3]	=   m_bCardPosition[CardData[j].bCardLocation];
					card.bCardLocation[4]	=	m_bCardPosition[CardData[j].bCardLocation+1];
					card.bCardLocation[5]	=	m_bCardPosition[CardData[j].bCardLocation+2];

					OutCardList.Add(card);
                    
				}
           }
	   }
	}
}
//分析同花顺
void  CGamePrompt::GetTongHuaShun(BYTE mark)
{
  if((BYTE(m_bTurnCardType))>CT_TONG_HUA_SHUN)
	  return ;
  if(m_bRound<5) return ;

  //开始分析
  BYTE  bFirstValue;
  if(CT_TONG_HUA_SHUN>(BYTE(m_bTurnCardType)))
	  bFirstValue=0;
  else 
	  bFirstValue=(m_bTurnCardType>>8);
  
//转换数值
	BYTE cbCardPosition[28],cbCardData[28];
	CopyMemory(cbCardPosition,	m_bCardPosition,sizeof(BYTE)*27);
	CopyMemory(cbCardData,		m_bCardData,	sizeof(BYTE)*27);	
    BYTE     bHongTaoCount=m_bHongTaoCount;
	
	//按花色排序操作
	bool bSorted;
	BYTE cbTempData,bLast=m_bCardCount-1;
	do
	{
		bSorted=false;
		for (BYTE i=0;i<bLast;i++)
		{
			if ((((cbCardData[i]&0x0f)!=1)&&cbCardData[i]<cbCardData[i+1])||(
				((cbCardData[i]&0x0f)==1)&&(cbCardData[i]+mark)<cbCardData[i+1]))
				
			{
				//交换位置
				cbTempData=cbCardData[i];
				cbCardData[i]=cbCardData[i+1];
				cbCardData[i+1]=cbTempData;
				
				cbTempData=cbCardPosition[i];
				cbCardPosition[i]=cbCardPosition[i+1];
				cbCardPosition[i+1]=cbTempData;
				bSorted=true;
			}	
		}
		bLast--;
	}while(bSorted);
    
	//临时位置
	BYTE    CardPosition[10];
	for(BYTE i=0;i<m_bCardCount-4+m_bHongTaoCount;i++)
	{
		if(GetCardLogicValue(cbCardData[i])>=16)
			continue;
		bHongTaoCount=m_bHongTaoCount;
		CardPosition[0]=cbCardPosition[i];
		BYTE j=1,k=1;
		if(cbCardData[i+1]==cbCardData[i])
			k++;
		bool bCanGo=true;
		while(bCanGo)
		{
			while(((i+k)<m_bCardCount)&&cbCardData[i]!=(cbCardData[i+k]+j))
			{
				if(bHongTaoCount>0)
				{	
					CardPosition[j]=m_bHongTaoLocation[m_bHongTaoCount-bHongTaoCount];
					bHongTaoCount--;
					j++;
				}else
				{
					bCanGo=false;
					break;
				}
				if(j==5)
				{
					bCanGo=false;
					break;
				}
			}
			if(j==5) break;
			while(((i+k)<m_bCardCount)&&cbCardData[i]==(cbCardData[i+k]+j))
			{
				CardPosition[j]=cbCardPosition[i+k];
				j++;
				if((i+k+1)<m_bCardCount&&cbCardData[i+k+1]==cbCardData[i+k])
					k++;
				k++;
				if(j>=5||(i+k)>=m_bCardCount)
				{
					bCanGo=false;
					break;
				}
			}
			if((i+k)>=m_bCardCount)
				break;
		}
	
		if(j==5)
		{
			CardType card;
			card.bCardCount		=	5;
			card.bCardType		=	CT_TONG_HUA_SHUN;
			CopyMemory(card.bCardLocation,CardPosition,sizeof(BYTE)* 5);

			OutCardList.Add(card);
		}
		if(j==3&&bHongTaoCount==2)
		{
			CardPosition[3]=m_bHongTaoLocation[0];
			CardPosition[4]=m_bHongTaoLocation[1];
			CardType card;
			card.bCardCount		=	5;
			card.bCardType		=	CT_TONG_HUA_SHUN;
			CopyMemory(card.bCardLocation,CardPosition,sizeof(BYTE)* 5);

			OutCardList.Add(card);
		}
		if(j==4&&bHongTaoCount==1)
		{
			
			CardPosition[4]=m_bHongTaoLocation[m_bHongTaoCount-bHongTaoCount];
			CardType card;
			card.bCardCount		=	5;
			card.bCardType		=	CT_TONG_HUA_SHUN;
			CopyMemory(card.bCardLocation,CardPosition,sizeof(BYTE)* 5);

			OutCardList.Add(card);
		}

	}

}

//分析四炸弹
void  CGamePrompt::GetSiZhangBomb()
{
	 //类型判断
	if(((BYTE)m_bTurnCardType)>CT_SI_ZHANG_BOMB)
		return ;
	
	//分析处理
	BYTE  bFirstValue;
	if(CT_SI_ZHANG_BOMB>(BYTE(m_bTurnCardType)))
		bFirstValue=0;
	else 
		bFirstValue=(m_bTurnCardType>>8);
	for(int  i=m_bRound-1;i>=0;i--)
	{
		if(CardData[i].bCardLogicValue<=bFirstValue||CardData[i].bCardLogicValue>=16)
			continue;
		
		if(CardData[i].bCardCount==2&&m_bHongTaoCount==2)
		{
			CardType  card;
			card.bCardCount			=	4;
			card.bCardType			=	CT_SI_ZHANG_BOMB;
			card.bCardLocation[0]	=	m_bCardPosition[CardData[i].bCardLocation];
			card.bCardLocation[1]	=	m_bCardPosition[CardData[i].bCardLocation+1];
			card.bCardLocation[2]	=   m_bHongTaoLocation[0];
			card.bCardLocation[3]	=   m_bHongTaoLocation[1];

			OutCardList.Add(card);
		}
		else if(CardData[i].bCardCount==3&&m_bHongTaoCount==1)
		{
			CardType  card	;
			card.bCardCount			=	4;
			card.bCardType			=	CT_SI_ZHANG_BOMB;
			card.bCardLocation[0]	=	m_bCardPosition[CardData[i].bCardLocation];
			card.bCardLocation[1]	=	m_bCardPosition[CardData[i].bCardLocation+1];
			card.bCardLocation[2]	=   m_bCardPosition[CardData[i].bCardLocation+2];
			card.bCardLocation[3]	=   m_bHongTaoLocation[0];

			OutCardList.Add(card);
		}
		else if(CardData[i].bCardCount==4)
		{
	        CardType  card;
			card.bCardCount			=	4;
			card.bCardType			=	CT_SI_ZHANG_BOMB;
			card.bCardLocation[0]	=	m_bCardPosition[CardData[i].bCardLocation];
			card.bCardLocation[1]	=	m_bCardPosition[CardData[i].bCardLocation+1];
			card.bCardLocation[2]	=   m_bCardPosition[CardData[i].bCardLocation+2];
			card.bCardLocation[3]	=   m_bCardPosition[CardData[i].bCardLocation+3];

			OutCardList.Add(card);
		}
	}

}

//分析五炸弹
void  CGamePrompt::GetWuZhangBomb()
{
	//类型判断
	if(((BYTE)m_bTurnCardType)>CT_WU_ZHANG_BOMB)
		return ;
	
	//分析处理
	BYTE  bFirstValue;
	if(CT_WU_ZHANG_BOMB>(BYTE(m_bTurnCardType)))
		bFirstValue=0;
	else 
		bFirstValue=(m_bTurnCardType>>8);
    for(int  i=m_bRound-1;i>=0;i--)
	{
		if(CardData[i].bCardLogicValue<=bFirstValue||CardData[i].bCardLogicValue>=16)
			continue;
		
		if(CardData[i].bCardCount==3&&m_bHongTaoCount==2)
		{
			CardType  card;
			card.bCardCount				=	5;
			card.bCardType				=	CT_WU_ZHANG_BOMB;
			CopyMemory(card.bCardLocation,&m_bCardPosition[CardData[i].bCardLocation],sizeof(BYTE)*3);
			card.bCardLocation[3]		=   m_bHongTaoLocation[0];
			card.bCardLocation[4]		=   m_bHongTaoLocation[1];

			OutCardList.Add(card);
		}
		else if(CardData[i].bCardCount==4&&m_bHongTaoCount==1)
		{
			CardType  card;
			card.bCardCount				=	5;
			card.bCardType				=	CT_WU_ZHANG_BOMB;
			CopyMemory(card.bCardLocation,&m_bCardPosition[CardData[i].bCardLocation],sizeof(BYTE)*4);
			card.bCardLocation[4]		=   m_bHongTaoLocation[0];

			OutCardList.Add(card);
		}
		else if(CardData[i].bCardCount==5)
		{
	        CardType  card	;
			card.bCardCount			=	5;
			card.bCardType			=	CT_WU_ZHANG_BOMB;
			CopyMemory(card.bCardLocation,&m_bCardPosition[CardData[i].bCardLocation],sizeof(BYTE)*5);

			OutCardList.Add(card);
		}
	}

}

//分析六炸弹
void  CGamePrompt::GetLiuZhangBomb()
{
    //类型判断
	if(((BYTE)m_bTurnCardType)>CT_LIU_ZHANG_BOMB)
		return ;
	
	//分析处理
	BYTE  bFirstValue;
	if(CT_LIU_ZHANG_BOMB>(BYTE(m_bTurnCardType)))
		bFirstValue=0;
	else 
		bFirstValue=(m_bTurnCardType>>8);
    for(int  i=m_bRound-1;i>=0;i--)
	{
		if(CardData[i].bCardLogicValue<=bFirstValue||CardData[i].bCardLogicValue>=16)
			continue;
		
		if(CardData[i].bCardCount==4&&m_bHongTaoCount==2)
		{
			CardType  card;
			card.bCardCount			=	6;
			card.bCardType			=	CT_LIU_ZHANG_BOMB;
			CopyMemory(card.bCardLocation,&m_bCardPosition[CardData[i].bCardLocation],sizeof(BYTE)*4);
			card.bCardLocation[4]	=   m_bHongTaoLocation[0];
			card.bCardLocation[5]	=   m_bHongTaoLocation[1];
			
			OutCardList.Add(card);
		}
		else if(CardData[i].bCardCount==5&&m_bHongTaoCount==1)
		{
			CardType  card;
			card.bCardCount			=	6;
			card.bCardType			=	CT_LIU_ZHANG_BOMB;
			CopyMemory(card.bCardLocation,&m_bCardPosition[CardData[i].bCardLocation],sizeof(BYTE)*5);
			card.bCardLocation[5]	=   m_bHongTaoLocation[0];

			OutCardList.Add(card);
		}
		else if(CardData[i].bCardCount==6)
		{
	        CardType  card;
			card.bCardCount			=	6;
			card.bCardType			=	CT_LIU_ZHANG_BOMB;
			CopyMemory(card.bCardLocation,&m_bCardPosition[CardData[i].bCardLocation],sizeof(BYTE)*6);

			OutCardList.Add(card);
		}
	}
}

//分析七炸弹
void  CGamePrompt::GetQiZhangBomb()
{
	//类型判断
	if(((BYTE)m_bTurnCardType)>CT_QI_ZHANG_BOMB)
		return ;
	
	//分析处理
	BYTE  bFirstValue;
	if(CT_QI_ZHANG_BOMB>(BYTE(m_bTurnCardType)))
		bFirstValue=0;
	else 
		bFirstValue=(m_bTurnCardType>>8);
    for(int  i=m_bRound-1;i>=0;i--)
	{
		if(CardData[i].bCardLogicValue<=bFirstValue||CardData[i].bCardLogicValue>=16)
			continue;
		
		if(CardData[i].bCardCount==5&&m_bHongTaoCount==2)
		{
			CardType  card	;
			card.bCardCount			=	7;
			card.bCardType			=	CT_QI_ZHANG_BOMB;
			CopyMemory(card.bCardLocation,&m_bCardPosition[CardData[i].bCardLocation],sizeof(BYTE)*5);
			card.bCardLocation[5]	=   m_bHongTaoLocation[0];
			card.bCardLocation[6]	=   m_bHongTaoLocation[1];
			
			OutCardList.Add(card);
		}
		else if(CardData[i].bCardCount==6&&m_bHongTaoCount==1)
		{
			CardType  card;
			card.bCardCount			=	7;
			card.bCardType			=	CT_QI_ZHANG_BOMB;
			CopyMemory(card.bCardLocation,&m_bCardPosition[CardData[i].bCardLocation],sizeof(BYTE)*6);
			card.bCardLocation[6]	=   m_bHongTaoLocation[0];

			OutCardList.Add(card);
		}
		else if(CardData[i].bCardCount==7)
		{
	        CardType  card;
			card.bCardCount			=	7;
			card.bCardType			=	CT_QI_ZHANG_BOMB;
			CopyMemory(card.bCardLocation,&m_bCardPosition[CardData[i].bCardLocation],sizeof(BYTE)*7);

			OutCardList.Add(card);
		}
	}
}

//分析八炸弹
void  CGamePrompt::GetBaZhangBomb()
{
     
	//类型判断
	if(((BYTE)m_bTurnCardType)>CT_BA_ZHANG_BOMB)
		return ;
	
	//分析处理
	BYTE  bFirstValue;
	if(CT_BA_ZHANG_BOMB>(BYTE(m_bTurnCardType)))
		bFirstValue=0;
	else 
		bFirstValue=(m_bTurnCardType>>8);
    for(int  i=m_bRound-1;i>=0;i--)
	{
		if(CardData[i].bCardLogicValue<=bFirstValue||CardData[i].bCardLogicValue>=16)
			continue;
		
		if(CardData[i].bCardCount==6&&m_bHongTaoCount==2)
		{
			CardType  card	;
			card.bCardCount			=	8;
			card.bCardType			=	CT_BA_ZHANG_BOMB;
			CopyMemory(card.bCardLocation,&m_bCardPosition[CardData[i].bCardLocation],sizeof(BYTE)*6);
			card.bCardLocation[6]	=   m_bHongTaoLocation[0];
			card.bCardLocation[7]	=   m_bHongTaoLocation[1];
			
			OutCardList.Add(card);
		}
		else if(CardData[i].bCardCount==7&&m_bHongTaoCount==1)
		{
			CardType  card	;
			card.bCardCount			=	8;
			card.bCardType			=	CT_QI_ZHANG_BOMB;
			CopyMemory(card.bCardLocation,&m_bCardPosition[CardData[i].bCardLocation],sizeof(BYTE)*7);
			card.bCardLocation[7]	=   m_bHongTaoLocation[0];

			OutCardList.Add(card);
		}
		else if(CardData[i].bCardCount==8)
		{
	        CardType  card	;
			card.bCardCount			=	8;
			card.bCardType			=	CT_BA_ZHANG_BOMB;
			CopyMemory(card.bCardLocation,&m_bCardPosition[CardData[i].bCardLocation],sizeof(BYTE)*8);

			OutCardList.Add(card);
		}
	}
}

//分析九炸弹
void  CGamePrompt::GetJiuZhangBomb()
{
     //类型判断
	if(((BYTE)m_bTurnCardType)>CT_JIU_ZHANG_BOMB)
		return ;
	
	//分析处理
	BYTE  bFirstValue;
	if(CT_JIU_ZHANG_BOMB>(BYTE(m_bTurnCardType)))
		bFirstValue=0;
	else 
		bFirstValue=(m_bTurnCardType>>8);
    for(int  i=m_bRound-1;i>=0;i--)
	{
		if(CardData[i].bCardLogicValue<=bFirstValue||CardData[i].bCardLogicValue>=16)
			continue;
		
		if(CardData[i].bCardCount==7&&m_bHongTaoCount==2)
		{
			CardType  card	;
			card.bCardCount			=	9;
			card.bCardType			=	CT_JIU_ZHANG_BOMB;
			CopyMemory(card.bCardLocation,&m_bCardPosition[CardData[i].bCardLocation],sizeof(BYTE)*7);
			card.bCardLocation[7]	=   m_bHongTaoLocation[0];
			card.bCardLocation[8]	=   m_bHongTaoLocation[1];
			
			OutCardList.Add(card);
		}
		else if(CardData[i].bCardCount==8&&m_bHongTaoCount==1)
		{
			CardType  card;
			card.bCardCount			=	9;
			card.bCardType			=	CT_JIU_ZHANG_BOMB;
			CopyMemory(card.bCardLocation,&m_bCardPosition[CardData[i].bCardLocation],sizeof(BYTE)*8);
			card.bCardLocation[8]	=   m_bHongTaoLocation[0];

			OutCardList.Add(card);
		}
	}

}
	
//分析十炸弹
void  CGamePrompt::GetShiZhangBomb()
{  
	
	//类型判断
	if(((BYTE)m_bTurnCardType)>CT_SHI_ZHANG_BOMB)
		return ;
	if(m_bHongTaoCount!=2) return;
	
	//分析处理
	BYTE  bFirstValue;
	if(CT_SHI_ZHANG_BOMB>(BYTE(m_bTurnCardType)))
		bFirstValue=0;
	else 
		bFirstValue=(m_bTurnCardType>>8);
    for(BYTE i=0;i<m_bRound;i++)
	{
		if(CardData[i].bCardLogicValue<=bFirstValue||CardData[i].bCardLogicValue>=16)
			continue;
		if(CardData[i].bCardCount==8)
		{
			CardType  card;
			card.bCardCount			=	10;
			card.bCardType			=	CT_SHI_ZHANG_BOMB;
			CopyMemory(card.bCardLocation,&m_bCardPosition[CardData[i].bCardLocation],sizeof(BYTE)*8);
			card.bCardLocation[8]	=   m_bHongTaoLocation[0];
			card.bCardLocation[9]	=   m_bHongTaoLocation[1];
			
			OutCardList.Add(card);
			
			break;
		}
	}
}

//分析四王
void  CGamePrompt::GetSiWang()
{
	//分析处理
	for(BYTE i=0;i<m_bRound-1;i++)
	{
		if(i<(m_bRound-1)&&CardData[i].bCardLogicValue==GetCardLogicValue(0x42)&&CardData[i].bCardCount==2
			&&CardData[i+1].bCardLogicValue==GetCardLogicValue(0x41)&&CardData[i+1].bCardCount==2)
		{
			CardType card;
			card.bCardCount		=	4;
			card.bCardType			=	CT_FOUR_KING;
			CopyMemory(card.bCardLocation,&m_bCardPosition[CardData[i].bCardLocation],sizeof(BYTE)*2);
			CopyMemory(&card.bCardLocation[2],&m_bCardPosition[CardData[i+1].bCardLocation],sizeof(BYTE)*2);
           
			OutCardList.Add(card);
			
			break;
		}
	}
}
