#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////
//静态变量

//扑克数据
const BYTE	CGameLogic::m_bCardListData[108]=
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	    //方块 A - K
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	    //梅花 A - K
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	    //红桃 A - K
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,	    //黑桃 A - K
	0x41,0x42,                                                              //小王，大王
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	    //方块 A - K
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	    //梅花 A - K
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	    //红桃 A - K
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,	    //黑桃 A - K
	0x41,0x42                                                               //小王，大王
};

//初始级数
BYTE CGameLogic::m_bSeries=0x22;


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
short CGameLogic::GetCardType(BYTE cbCardData[], BYTE cbCardCount)
{
	//预先判断
	if(cbCardCount>5)
		if(GetCardColor(cbCardData[0])==0x40)	  return	CT_ERROR;
	
	//类型判断
	switch(cbCardCount)
	{ 
	case 1:
		 m_bValue=GetCardLogicValue(cbCardData[0]);
												  return	((short(m_bValue)<<8)|CT_SINGLE);
		 break;
	case 2:
	     if (IsDouble(cbCardData,cbCardCount))    return    ((short(m_bValue)<<8)|CT_DOUBLE);
		 break;
	case 3:
		if (IsSanTiao(cbCardData,cbCardCount))    return    ((short(m_bValue)<<8)|CT_THREE_TIAO);
	    break;
	case 4:
		if(IsFourKing(cbCardData,cbCardCount))    return    ((short(m_bValue)<<8)|CT_FOUR_KING);
		if(IsSiZhangBomb(cbCardData,cbCardCount)) return    ((short(m_bValue)<<8)|CT_SI_ZHANG_BOMB);
		break;
	case 5:
		if(IsWuZhangBomb(cbCardData,cbCardCount)) return    ((short(m_bValue)<<8)|CT_WU_ZHANG_BOMB);
		if(IsHuLu(cbCardData,cbCardCount))        return    ((short(m_bValue)<<8)|CT_HU_LU);    
		if (IsTongHuaShun(cbCardData,cbCardCount))return    ((short(m_bValue)<<8)|CT_TONG_HUA_SHUN);
	    if (IsShunZhi(cbCardData,cbCardCount))    return    ((short(m_bValue)<<8)|CT_SHUN_ZI);
		break;
	case 6:
		if(IsLiuZhangBomb(cbCardData,cbCardCount))return    ((short(m_bValue)<<8)|CT_LIU_ZHANG_BOMB);
		if(IsGangBan(cbCardData,cbCardCount))     return    ((short(m_bValue)<<8)|CT_GANG_BAN);
		if(IsLiangLianDui(cbCardData,cbCardCount))return    ((short(m_bValue)<<8)|CT_LIANG_LIAN_DUI);
		break;
	case 7:
		if(IsQiZhangBomb(cbCardData,cbCardCount)) return    ((short(m_bValue)<<8)|CT_QI_ZHANG_BOMB);
		break;
	case 8:
		if(IsBaZhangBomb(cbCardData,cbCardCount)) return    ((short(m_bValue)<<8)|CT_BA_ZHANG_BOMB);
		break;
	case 9:
		if(IsJiuZhangBomb(cbCardData,cbCardCount))return    ((short(m_bValue)<<8)|CT_JIU_ZHANG_BOMB);
		break;
	case 10:
		if(IsShiZhangBomb(cbCardData,cbCardCount))return    ((short(m_bValue)<<8)|CT_SHI_ZHANG_BOMB);
		break;
	}
	
	return CT_ERROR;
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
				(cbLogicValue[i]==cbLogicValue[i+1]&&cbCardData[i+1]==m_bSeries&&cbCardData[i]!=m_bSeries))
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
//按花色排
void CGameLogic::SortByColor(BYTE  cbCardData[], BYTE cbCardCount)
{    
	//先排序
	SortCardList(cbCardData,cbCardCount);
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
			if (((GetCardColor(cbCardData[i])<GetCardColor(cbCardData[i+1]))||
				(GetCardColor(cbCardData[i])==GetCardColor(cbCardData[i+1])&&cbLogicValue[i]<cbLogicValue[i+1]))
				&&cbCardData[i]!=m_bSeries)

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
  return ;
}

//按炸弹排
void CGameLogic::SortByBomb	(BYTE  cbCardData[], BYTE cbCardCount)
{
  
  //顺序排序
  SortCardList(cbCardData,cbCardCount);
  if(cbCardCount<3) return ;
  //排值相同排数
  BYTE   bTypeCount[15],bTypeStart[15];
  ZeroMemory(bTypeCount,sizeof(bTypeCount));

  BYTE bStart=0,bRound=0,bNext=0;
  
  while(bNext<cbCardCount)
  {
	  BYTE i=1;
	  bStart=bNext;
	  while(bNext<(cbCardCount-1)&&((
		  GetCardLogicValue(cbCardData[bNext])==GetCardLogicValue(cbCardData[bNext+1]))||
		  (GetCardColor(cbCardData[bNext])==0x40&&GetCardColor(cbCardData[bNext+1])==0x40)))
	  {
		  i++;
		  bNext++;
	  }
	  bTypeCount[bRound]=i;
	  bTypeStart[bRound]=bStart;
	  bRound++;
	  bNext++;
  }
  //开始排序
  bool bSorted;
  BYTE cbTempData,bLast=bRound-1;
  do{
      bSorted=false; 
	  for(BYTE i=0;i<bLast;i++)
	   {
		   if(bTypeCount[i]<bTypeCount[i+1])
		   {
			   cbTempData=bTypeCount[i];
			   bTypeCount[i]=bTypeCount[i+1];
			   bTypeCount[i+1]=cbTempData;

			   cbTempData=bTypeStart[i];
			   bTypeStart[i]=bTypeStart[i+1];
			   bTypeStart[i+1]=cbTempData;

			   bSorted=true;
		   }
	   }
	   bLast--;
  }while(bSorted);
  
  //创建数据
  BYTE  bTemp[27];
  CopyMemory(bTemp,cbCardData,cbCardCount);
  BYTE   bLocation=0;
  for(BYTE i=0;i<bRound;i++)
  {
	  for(BYTE j=0;j<bTypeCount[i];j++)
	  {
		  cbCardData[bLocation]=bTemp[bTypeStart[i]+j];
		  bLocation++;
	  }
  }

}

//混乱扑克
void CGameLogic::RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//混乱准备
	BYTE cbCardData[sizeof(m_bCardListData)];
	CopyMemory(cbCardData,m_bCardListData,sizeof(m_bCardListData));
	srand( (unsigned)time( NULL ));
	//混乱扑克
	BYTE bRandCount=0,bPosition=0;
	do
	{
		bPosition=rand()%(cbBufferCount-bRandCount);
		cbCardBuffer[bRandCount++]=cbCardData[bPosition];
		cbCardData[bPosition]=cbCardData[cbBufferCount-bRandCount];
	} while (bRandCount<cbBufferCount);

	return;
}

//删除扑克
bool CGameLogic::RemoveCard(const BYTE bRemoveCard[], BYTE bRemoveCount, BYTE cbCardData[], BYTE cbCardCount)
{
	//检验数据
	ASSERT(bRemoveCount<=cbCardCount);

	//定义变量
	BYTE bDeleteCount=0,bTempCardData[MAX_COUNT];
	if (cbCardCount>CountArray(bTempCardData)) return false;
	CopyMemory(bTempCardData,cbCardData,cbCardCount*sizeof(cbCardData[0]));
	//置零扑克
	for (BYTE i=0;i<bRemoveCount;i++)
	{
		for (BYTE j=0;j<cbCardCount;j++)
		{
			if (bRemoveCard[i]==bTempCardData[j])
			{
				bDeleteCount++;
				bTempCardData[j]=0;
				break;
			}
		}
	}
	if (bDeleteCount!=bRemoveCount) return false;

	//清理扑克
	BYTE bCardPos=0;
	for (BYTE i=0;i<cbCardCount;i++)
	{
		if (bTempCardData[i]!=0) cbCardData[bCardPos++]=bTempCardData[i];
	}

	return true;
}

//有效判断
bool CGameLogic::IsValidCard(BYTE cbCardData)
{
	//变量定义
	BYTE cbColor=GetCardColor(cbCardData);
	BYTE cbValue=GetCardValue(cbCardData);

	//有效判断
	if ((cbColor<=0x40)&&(cbValue>=0x01)&&(cbValue<=0x0D)) return true;

	return false;
}

//逻辑数值
BYTE CGameLogic::GetCardLogicValue(BYTE cbCardData)
{
	//扑克属性
	BYTE bCardColor=GetCardColor(cbCardData);
	BYTE bCardValue=GetCardValue(cbCardData);

	//转换数值
    
	//大王小王
    if(bCardColor==0x40)
		return (bCardValue+15);
	
	//是否为打
	if(bCardValue==GetCardValue(m_bSeries))  return 15;
	
	//是否A牌
	if(bCardValue==1)          return 14;
	
	//其它牌值
	return bCardValue;
}

//是否对子
bool CGameLogic::IsDouble(BYTE cbCardData[], BYTE cbCardCount)
{
	
	//数目判断
	m_bValue=GetCardLogicValue(cbCardData[1]);
	
	if(cbCardData[0]==m_bSeries)
		return true;
	
	//分析处理
	BYTE bNextValue=GetCardLogicValue(cbCardData[1]);
	BYTE bFirstValue=GetCardLogicValue(cbCardData[0]);
   

	return (bNextValue==bFirstValue);
}

//是否四张炸弹
bool CGameLogic::IsSiZhangBomb(BYTE cbCardData[], BYTE cbCardCount)
{
	
	m_bValue=GetCardLogicValue(cbCardData[cbCardCount-1]);
	return (BombCount(cbCardData,cbCardCount)==4);
}

//是否葫芦
bool CGameLogic::IsHuLu(BYTE cbCardData[], BYTE cbCardCount)
{
	//变量定义
	BYTE bSeriesCount=0,j=0;
	//计算红桃
	while(cbCardData[j]==m_bSeries)
	{
		bSeriesCount++;
		j++;
	} 
	
	//排数计算
	BYTE bFirstCount=1,bNextCount=1;
	
	//分析处理
	if(GetCardColor(cbCardData[0])==0x40)
	{
		if(cbCardData[1]!=cbCardData[0])
			return false;
		j++;
		bFirstCount++;
	}else{
		while(j<(cbCardCount-1)&&(GetCardValue(cbCardData[j])==GetCardValue(cbCardData[j+1])))
		{
			bFirstCount++;
			j++;
		}
	}
	if(bFirstCount>3) return false;
    if(j==4) return false;
	j++;
	while(j<(cbCardCount-1)&&(GetCardValue(cbCardData[j])==GetCardValue(cbCardData[j+1])))
	{
		bNextCount++;
		j++;
	}
	if(bNextCount>3) return false;
	if((bSeriesCount+bFirstCount+bNextCount)!=5) return false;
	if(bNextCount<3&&GetCardColor(cbCardData[0])!=0x40)
		m_bValue=GetCardLogicValue(cbCardData[2]);
	else
		m_bValue=GetCardLogicValue(cbCardData[4]);
          

	return true;
}

//是否五张炸弹
bool CGameLogic::IsWuZhangBomb(BYTE cbCardData[], BYTE cbCardCount)
{
	if(GetCardColor(cbCardData[0])==0x40)	  return	false;
    m_bValue=GetCardLogicValue(cbCardData[cbCardCount-1]);
	return (BombCount(cbCardData,cbCardCount)==5);
}

	
//是否六张炸弹
bool CGameLogic::IsLiuZhangBomb(BYTE cbCardData[], BYTE cbCardCount)
{
    m_bValue=GetCardLogicValue(cbCardData[cbCardCount-1]);
	return (BombCount(cbCardData,cbCardCount)==6);
} 
	
//是否七张炸弹
bool CGameLogic::IsQiZhangBomb(BYTE cbCardData[], BYTE cbCardCount)
{
    m_bValue=GetCardLogicValue(cbCardData[cbCardCount-1]);
	return (BombCount(cbCardData,cbCardCount)==7);
} 
	
//是否八张炸弹
bool CGameLogic::IsBaZhangBomb(BYTE cbCardData[], BYTE cbCardCount)
{
    m_bValue=GetCardLogicValue(cbCardData[cbCardCount-1]);
	return (BombCount(cbCardData,cbCardCount)==8);
} 

//是否九张炸弹
bool CGameLogic::IsJiuZhangBomb(BYTE cbCardData[], BYTE cbCardCount)
{
    m_bValue=GetCardLogicValue(cbCardData[cbCardCount-1]);
	return (BombCount(cbCardData,cbCardCount)==9);
}

//是否十张炸弹
bool CGameLogic::IsShiZhangBomb(BYTE cbCardData[], BYTE cbCardCount)
{
    m_bValue=GetCardLogicValue(cbCardData[cbCardCount-1]);
	return (BombCount(cbCardData,cbCardCount)==10);
} 

//是否四王
bool CGameLogic::IsFourKing(BYTE cbCardData[], BYTE cbCardCount)
{
	m_bValue=GetCardLogicValue(cbCardData[0]);
	//分析处理
	for(BYTE i=0;i<cbCardCount;i++)
		if(GetCardColor(cbCardData[i])!=0x40)
		   return false;
	
	return true;
}

//是否顺子
bool CGameLogic::IsShunZhi(BYTE cbCardData[], BYTE cbCardCount)
{
	
	if(GetCardColor(cbCardData[0])==0x40)	  return	false;
	//变量定义
	BYTE bSeriesCount=0,j=0;
	BYTE bCardTemp[5];
	memcpy(bCardTemp,cbCardData,cbCardCount);

	//计算红桃
	while(cbCardData[j]==m_bSeries&&j<cbCardCount)
	{
		bSeriesCount++;
		j++;
	} 

    BYTE bFirstValue=GetCardValue(cbCardData[j]);
	BYTE bLastValue;

	//顺序调整
	if((bFirstValue==GetCardValue(m_bSeries))&&(bFirstValue!=0x01)){
		for( BYTE i=j+1;i<cbCardCount;i++)
		{
			BYTE bNextValue=GetCardValue(cbCardData[i]);
			if(bFirstValue<bNextValue||bNextValue==0x01){
				bCardTemp[i-1]=bCardTemp[i];
		        bCardTemp[i]=bFirstValue;
			}
		}
	}
    
	//获取初值
    bFirstValue=GetCardValue(bCardTemp[j]);
    bLastValue=GetCardValue(bCardTemp[4]);
    
	//分析处理
	BYTE cbValue;
	BYTE bTemp=0;
	if (bFirstValue==1)
	{   
	   if(bLastValue>0 && bLastValue<5)
	   {
		   m_bValue=5;
		   bTemp=5;
	   }
	   else 
	   {   
		   m_bValue=14;
		   bTemp=13;
	   }
    }
		
    //开始判断
	for(BYTE i=j+1,m=1;i<cbCardCount;i++,m++)
	{
		cbValue=GetCardValue(bCardTemp[i]);
		while(cbValue!=(bFirstValue+bTemp-m))
		{
		    if(bSeriesCount<=0){
			   return false;
			 }
			 else{
					m=m+1;
					bSeriesCount--;
				 }
		}
     }

     //排型值计算
	 if(bFirstValue!=1)
	    m_bValue=((bFirstValue+bSeriesCount)>14)?  14:(bFirstValue+bSeriesCount);

	return true;
}

//是否三条
bool CGameLogic::IsSanTiao(BYTE cbCardData[], BYTE cbCardCount)
{
	//三王处理
	
	m_bValue=GetCardLogicValue(cbCardData[cbCardCount-1]);
	return (BombCount(cbCardData,cbCardCount)==3);
}

//是否同花顺
bool CGameLogic::IsTongHuaShun(BYTE cbCardData[], BYTE cbCardCount)
{
	if(GetCardColor(cbCardData[0])==0x40)	  return	false;
	//变量定义
	BYTE bSeriesCount=0,j=0;
	BYTE bCardTemp[5];
	memcpy(bCardTemp,cbCardData,cbCardCount);
	
	//计算红桃
	while(cbCardData[j]==m_bSeries&&j<cbCardCount)
	{
		bSeriesCount++;
		j++;
	} 
	
	//变量定义
	BYTE bFirstValue=GetCardValue(cbCardData[j]);
    BYTE bLastValue;
	BYTE bTempCard=cbCardData[j];

	//顺序调整
	if((bFirstValue==GetCardValue(m_bSeries))&&(bFirstValue!=0x01)){
		for( BYTE i=j+1;i<cbCardCount;i++)
		{
			BYTE bNextValue=GetCardValue(cbCardData[i]);
			if(bFirstValue<bNextValue||bNextValue==0x01){
				bCardTemp[i-1]=bCardTemp[i];
		        bCardTemp[i]=bTempCard;
			}
		}
	}
	
	//获取初值
	BYTE bFirstColor=GetCardColor(bCardTemp[j]);
	bFirstValue=GetCardValue(bCardTemp[j]);
    bLastValue=GetCardValue(bCardTemp[4]);
    
	//分析处理
	BYTE cbValue;
	BYTE bTemp=0;
	if (bFirstValue==1)
	{   
	   if(bLastValue>0 && bLastValue<5)
	   {
		   m_bValue=5;
		   bTemp=5;
	   }
	   else 
	   {   
		   m_bValue=14;
		   bTemp=13;
	   }
    }
	
	 //开始判断
	for(BYTE i=j+1,m=1;i<cbCardCount;i++,m++)
	{
		cbValue=GetCardValue(bCardTemp[i]);
		if (GetCardColor(bCardTemp[i])!=bFirstColor)
		{
				return false;
		}
		while (cbValue!=(bFirstValue+bTemp-m))
		{
		    if(bSeriesCount<=0){
			   return false;
			 }
			 else{
					m=m+1;
					bSeriesCount--;
				 }
		}
     }
	
	//排型值计算
    if(bFirstValue!=1)    
      m_bValue=((bFirstValue+bSeriesCount)>14)? 14:(bFirstValue+bSeriesCount);

	return true;
}
//是否钢板
bool CGameLogic::IsGangBan(BYTE cbCardData[], BYTE cbCardCount)
{
    //变量定义
	BYTE bSeriesCount=0,j=0;
	BYTE bFirstCount=1,bNextCount=1;
	
	//计算红桃
	while(j<cbCardCount&&cbCardData[j]==m_bSeries)
	{
		bSeriesCount++;
		j++;
	} 
    
	
	//分析处理
	while(j<(cbCardCount-1)&&(GetCardValue(cbCardData[j])==GetCardValue(cbCardData[j+1])))
	{
		bFirstCount++;
		j++;
	}
	//错误判断
	if(bFirstCount>3) return false;
	if(j==cbCardCount) return false;

	//取值
    BYTE bFirstValue = GetCardValue(cbCardData[j]);
	BYTE bNextValue  = GetCardValue(cbCardData[j+1]);
    
	//连续判断
    if((bFirstValue!=(bNextValue+1))&&(bFirstValue!=(bNextValue-1))
		&&(bFirstValue!=(bNextValue-12))&&(bFirstValue!=(bNextValue+12)))
	   return false;
	if(bFirstValue==(bNextValue+1))
		m_bValue=bFirstValue;
	if(bFirstValue==(bNextValue-1))
		m_bValue=bNextValue;
	if((bFirstValue==(bNextValue-12))||(bFirstValue==(bNextValue+12)))
		m_bValue=14;

	
	j++;
	while(j<(cbCardCount-1)&&(GetCardValue(cbCardData[j])==GetCardValue(cbCardData[j+1])))
	{
		bNextCount++;
		j++;
	}
	if(bNextCount>3) return false;
	
	if((bSeriesCount+bFirstCount+bNextCount)!=6) return false;
	
	return true;
	
}
//是否两连队
bool CGameLogic::IsLiangLianDui(BYTE cbCardData[], BYTE cbCardCount)
{ 
	//变量定义
	BYTE bSeriesCount=0,j=0;
	BYTE bCardTemp[6];
	memcpy(bCardTemp,cbCardData,cbCardCount);

	//计算红桃
	while(cbCardData[j]==m_bSeries&&j<cbCardCount)
	{
		bSeriesCount++;
		j++;
	} 

    BYTE bFirstValue=GetCardValue(cbCardData[j]);
	BYTE bMidValue,bLastValue;
    BYTE bFirstCount=1,bMidCount=0,bLastCount=0;
    
	//顺序调整
	if((bFirstValue==GetCardValue(m_bSeries))&&(bFirstValue!=0x01)){
		for( BYTE i=j+1;i<cbCardCount;i++)
			if(bFirstValue<=GetCardValue(cbCardData[i])){
				bCardTemp[i-1]=bCardTemp[i];
		        bCardTemp[i]=bFirstValue;
			}
	   for( BYTE i=j+1;i<cbCardCount;i++)
			if(bFirstValue<=GetCardValue(cbCardData[i])){
				bCardTemp[i-1]=bCardTemp[i];
		        bCardTemp[i]=bFirstValue;
			}
	}

	//开始分析
	bFirstValue=GetCardValue(bCardTemp[j]);
    while(j<(cbCardCount-1)&&GetCardValue(bCardTemp[j])==GetCardValue(bCardTemp[j+1]))
	{
		bFirstCount++;
		j++;
	}
	if(bFirstCount>2) return false;
	j++;
	bMidValue=GetCardValue(bCardTemp[j]);
	bMidCount++;
	while(j<(cbCardCount-1)&&GetCardValue(bCardTemp[j])==GetCardValue(bCardTemp[j+1]))
	{
		bMidCount++;
		j++;
	}
	if(bMidCount>2) return false;
	j++;
	if(j<cbCardCount)
	{
		bLastValue=GetCardValue(bCardTemp[j]);
		bLastCount++;
		while(j<(cbCardCount-1)&&(GetCardValue(bCardTemp[j])==GetCardValue(bCardTemp[j+1])))
		{
			bLastCount++;
			j++;
		}
		j++;
	}
    if(bLastCount>2) return false;
	if((bSeriesCount+bFirstCount+bMidCount+bLastCount)!=6) return false;
 
	if(bLastCount==0){
		if(bSeriesCount!=2)
			   return false;
        if(bFirstValue==1)
		{ 
				m_bValue=14;
				return   (bMidValue==13||bMidValue==12||bMidValue==3||bMidValue==2);
		} 
		else
		{   
		
			if((bFirstValue-bMidValue)==1)
			{
				m_bValue=bFirstValue+1;
				return true;
			}
		   else if((bFirstValue-bMidValue)==2)
		   { 
			   m_bValue=bFirstValue;
			   return true;
		   }
		   return false;
		}
	}
	m_bValue=bFirstValue;
    if(bFirstValue==1)
	{
		m_bValue=14;
		return  (bMidValue==13&&bLastValue==12)||(bMidValue==3&&bLastValue==2);
	}
	
	return (bFirstValue==(bMidValue+1))&&(bMidValue==(bLastValue+1));

}


//出牌判断
bool CGameLogic::CompareCard(BYTE bFirstList[], BYTE bNextList[], BYTE cbFirstCount, BYTE cbNextCount)
{
	//获取类型
	short bFirstType	=	GetCardType(bFirstList,cbFirstCount);
	short bNextType		=	GetCardType(bNextList,  cbNextCount);

	if((bFirstType&0x00ff)==(bNextType&0x00ff)){
		return  bFirstType>bNextType;
	}
	if((bFirstType&0x00ff)>(bNextType&0x00ff)){
		if((bFirstType&0x00ff)>=CT_SI_ZHANG_BOMB)
			return true;
	}
	return false;
}
//炸弹计算
int CGameLogic::BombCount(BYTE cbCardData[], BYTE cbCardCount)
{
	//变量定义
	BYTE bFirstValue,bNextValue;

	//特殊处理
	bFirstValue=GetCardColor(cbCardData[0]);
    if(bFirstValue==0x40) return 0;
	
	
	//计算红桃
	BYTE bBombCount=0;
	BYTE i=0;
    while(cbCardData[i]==m_bSeries)
	{
		i++;
		bBombCount++;
	}
	
	//分析处理
	bFirstValue=GetCardValue(cbCardData[i]);
	bBombCount++;
	i++;
	for(;i<cbCardCount;i++)
	{
		bNextValue=GetCardValue(cbCardData[i]);
		if(bFirstValue==bNextValue)
			bBombCount++;
		else
			return 0;
	}
	return  bBombCount;
}

//添加扑克
bool CGameLogic::Add(BYTE bAddCard, BYTE  cbCardData[],BYTE cbCardCount=27)
{
	cbCardData[cbCardCount]=bAddCard;
	SortCardList(cbCardData,cbCardCount+1);
	return true;
}



//
//////////////////////////////////////////////////////////////////////////
