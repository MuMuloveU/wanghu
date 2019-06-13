#include "StdAfx.h"
#include ".\androidai.h"


/////////////////////////////////////////////////////////////////////////////////////
//CAndroidAIBase

CAndroidAIBase::CAndroidAIBase()
{
	memset(m_byCardData,0xff,sizeof(m_byCardData));
}

CAndroidAIBase::~CAndroidAIBase()
{
}

//设置牌
void CAndroidAIBase::SetCardData( const BYTE cbCardData[],BYTE byCardCount )
{
	//复制牌
	CopyMemory(m_byCardData,cbCardData,sizeof(BYTE)*byCardCount);
	m_byCardCount = byCardCount;

	//初始化
	ZeroMemory(m_bSelect,sizeof(m_bSelect));

	//
	ZeroMemory(m_byThreeCard,sizeof(m_byThreeCard));
	m_byThreeCount = 0;
	ZeroMemory(m_byGoodThreeCard,sizeof(m_byGoodThreeCard));
	m_byGoodThreeCount = 0;

	//
	ZeroMemory(m_byTwoCard,sizeof(m_byTwoCard));
	m_byTwoCount = 0;
	ZeroMemory(m_byGoodTwoCard,sizeof(m_byGoodTwoCard));
	m_byGoodTwoCount = 0;

	//
	ZeroMemory(m_byRemainThree,sizeof(m_byRemainThree));
	m_byRemainThreeCount = 0;
	ZeroMemory(m_byRemainTwo,sizeof(m_byRemainTwo));
	m_byRemainTwoCount = 0;

	//
	m_nMaxScoreThree = m_nMaxScoreTwo = 0;
	m_nActionScore = 0;
	m_nScoreThree = m_nScoreTwo = 0;

	//
	m_bHaveJiang = false;

	//
	m_byBadlyCard = 0xff;
}

//模拟操作
void CAndroidAIBase::SetAction( BYTE byActionMask,BYTE byActionCard )
{
	//验证
	ASSERT( byActionCard >=0 && byActionCard < 34 );
	//枚举
	BYTE byIndex = 0xff;
	switch( byActionMask )
	{
	case WIK_LEFT:
		{
			m_nActionScore = 300;
			byIndex = FindIndex(byActionCard+1);
			ASSERT( byIndex != 0xff );
			VERIFY( RemoveCard(byIndex) );
			byIndex = FindIndex(byActionCard+2);
			ASSERT( byIndex != 0xff );
			VERIFY( RemoveCard(byIndex) );
			break;
		}
	case WIK_CENTER:
		{
			m_nActionScore = 300;
			byIndex = FindIndex(byActionCard-1);
			ASSERT( byIndex != 0xff );
			VERIFY( RemoveCard(byIndex) );
			byIndex = FindIndex(byActionCard+1);
			ASSERT( byIndex != 0xff );
			VERIFY( RemoveCard(byIndex) );
			break;
		}
	case WIK_RIGHT:
		{
			m_nActionScore = 300;
			byIndex = FindIndex(byActionCard-2);
			ASSERT( byIndex != 0xff );
			VERIFY( RemoveCard(byIndex) );
			byIndex = FindIndex(byActionCard-1);
			ASSERT( byIndex != 0xff );
			VERIFY( RemoveCard(byIndex) );
			break;
		}
	case WIK_PENG:
		{
			m_nActionScore = 300;
			byIndex = FindIndex(byActionCard);
			ASSERT( byIndex != 0xff );
			VERIFY( RemoveCard(byIndex) );
			byIndex = FindIndex(byActionCard);
			ASSERT( byIndex != 0xff );
			VERIFY( RemoveCard(byIndex) );
			break;
		}
	case WIK_GANG:
		{
			m_nActionScore = 300;
			byIndex = FindIndex(byActionCard);
			ASSERT( byIndex != 0xff );
			VERIFY( RemoveCard(byIndex) );
			byIndex = FindIndex(byActionCard);
			if( byIndex != 0xff )
			{
				VERIFY( RemoveCard(byIndex) );
				byIndex = FindIndex(byActionCard);
				ASSERT( byIndex != 0xff );
				VERIFY( RemoveCard(byIndex) );
				byIndex = FindIndex(byActionCard);
				if( byIndex != 0xff )
					VERIFY( RemoveCard(byIndex) );
			}
			break;
		}
	default:
		ASSERT( FALSE );
	}
}

//获取最差牌
BYTE CAndroidAIBase::GetBadlyCard()
{
	return m_byBadlyCard;
}

//获取最大分
int CAndroidAIBase::GetMaxScore()
{
	return m_nActionScore+m_nMaxScoreThree+m_nMaxScoreTwo;
}

//两只牌是否是边的
bool CAndroidAIBase::IsEdge( BYTE byCard1,BYTE byCard2 )
{
	if( 0 == byCard1%9 || 8 == byCard2%9 )
		return true;
	return false;
}

//搜索相同牌
bool CAndroidAIBase::SearchSameCard( BYTE byCardData,BYTE &byIndex1,BYTE &byIndex2 )
{
	//
	byIndex1 = FindIndex(byCardData);
	if( byIndex1 == 0xff ) return false;
	byIndex2 = FindIndex(byCardData,byIndex1+1);
	if( byIndex2 == 0xff ) return false;
	return true;
}

//搜索连牌
bool CAndroidAIBase::SearchLinkCard( BYTE byCardData,BYTE &byIndex1,BYTE &byIndex2 )
{
	//效验
	if( byCardData >= 27 ) return false;
	//第二,三只
	BYTE byCard1 = byCardData+1;
	BYTE byCard2 = byCardData+2;
	if( byCard1 >= 27 || byCard2 >= 27 || byCardData/9 != byCard1/9 || byCardData/9 != byCard2/9 )
		return false;
	//寻找
	byIndex1 = FindIndex(byCard1);
	if( byIndex1 == 0xff ) return false;
	byIndex2 = FindIndex(byCard2);
	if( byIndex2 == 0xff ) return false;

	return true;
}

//搜索两只同牌
bool CAndroidAIBase::SearchSameCardRemain( BYTE byCardData,BYTE &byIndex,BYTE byStart )
{
	byIndex = FindIndexRemain(byCardData,byStart);
	return 0xff==byIndex?false:true;
}

//搜索有卡连牌
bool CAndroidAIBase::SearchLinkCardRemain( BYTE byCardData,BYTE byLinkType,BYTE &byIndex,BYTE byStart )
{
	//验证
	if( byCardData >= 27 ) return false;
	//判断类型
	BYTE byCard1 = 0xff;
	if( 0 == byLinkType )			//紧连
		byCard1 = byCardData+1;
	else if( 1 == byLinkType )		//有卡
		byCard1 = byCardData+2;
	//过滤
	if( byCard1 >= 27 || byCardData/9 != byCard1/9 ) return false;
	byIndex = FindIndexRemain(byCard1,byStart);
	return 0xff==byIndex?false:true;
}

//搜索牌
BYTE CAndroidAIBase::FindIndex( BYTE byCardData,BYTE byStart )
{
	for( BYTE i = byStart; i < m_byCardCount; i++ )
	{
		if( byCardData == m_byCardData[i] && !m_bSelect[i] )
			return i;
	}
	return 0xff;
}

//在移除最佳三只后搜索牌
BYTE CAndroidAIBase::FindIndexRemain( BYTE byCardData,BYTE byStart )
{
	for( BYTE i = byStart; i < m_byRemainThreeCount; i++ )
	{
		if( byCardData == m_byRemainThree[i] && !m_bSelect[i] )
			return i;
	}
	return 0xff;
}

//移除牌
bool CAndroidAIBase::RemoveCard( BYTE byCardIndex )
{
	//效验
	ASSERT( m_byCardCount > 0 );
	if( m_byCardCount == 0 ) return false;
	ASSERT( byCardIndex >= 0 && byCardIndex < MAX_COUNT );
	if( byCardIndex < 0 || byCardIndex >= MAX_COUNT ) return false;

	//删除
	BYTE byCount = m_byCardCount;
	m_byCardCount = 0;
	for( BYTE i = 0; i < byCount; i++ )
	{
		if( i == byCardIndex ) continue;
		m_byCardData[m_byCardCount++] = m_byCardData[i];
	}
	m_byCardData[byCount-1] = 0xff;
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////
//CAndroidAI

CAndroidAI::CAndroidAI(void)
{
	ZeroMemory(m_byEnjoinOutCard,sizeof(m_byEnjoinOutCard));
	m_byEnjoinOutCount = 0;
}

CAndroidAI::~CAndroidAI(void)
{
}

//思考
void CAndroidAI::Think()
{
	//分析三只
	AnalyseThree();
	//如果没三只
	BYTE i;
	if( m_nMaxScoreThree == 0 || m_byRemainThreeCount == 0 )
	{
		m_byRemainThreeCount = m_byCardCount;
		for( i = 0; i < m_byRemainThreeCount; i++ )
			m_byRemainThree[i] = m_byCardData[i];
	}
	//分析两只
	AnalyseTwo();
	if( m_nMaxScoreTwo == 0 )
	{
		m_byRemainTwoCount = m_byRemainThreeCount;
		for( i = 0; i < m_byRemainTwoCount; i++ )
			m_byRemainTwo[i] = m_byRemainThree[i];
	}
	//如果全部是两只
	if( m_byRemainTwoCount == 0 )
	{
		SearchTwo();
		return;
	}
	//分析一只
	AnalyseOne();
}

////
//BYTE CAndroidAI::GetThreeCard( BYTE byCard[],BYTE byMaxCount )
//{
//	//
//	BYTE byCopyCount = byMaxCount>m_byGoodThreeCount?m_byGoodThreeCount:byMaxCount;
//	CopyMemory(byCard,m_byGoodThreeCard,sizeof(BYTE)*byCopyCount);
//	return byCopyCount;
//}
//
////
//BYTE CAndroidAI::GetTwoCard( BYTE byCard[],BYTE byMaxCount )
//{
//	//
//	BYTE byCopyCount = byMaxCount>m_byGoodTwoCount?m_byGoodThreeCount:byMaxCount;
//	CopyMemory(byCard,m_byGoodTwoCard,sizeof(BYTE)*byCopyCount);
//	return byCopyCount;
//}

//从最佳两只牌组合中搜索最差牌
BYTE CAndroidAI::GetBadlyIn2Card()
{
	BYTE byBadly = 0xff;
	int nMin = 33;
	int nScore;
	BYTE byCard;
	for( BYTE i = 0; i < m_byGoodTwoCount*2; i++ )
	{
		byCard = m_byGoodTwoCard[i];
		if( byCard >= 27 )						//如果是字
		{
			nScore = 2;
		}
		else if( byCard%9 == 0 || byCard%9 == 8 )	//如果是一或者九
		{
			nScore = 6;
		}
		else 
		{
			nScore = 10;
		}
		nScore += AddScore(byCard);
		if( nScore < nMin )
		{
			nMin = nScore;
			byBadly = byCard;
		}
	}
	return byBadly;
}

//从最佳三只牌组合中搜索最差牌
BYTE CAndroidAI::GetBadlyIn3Card()
{
	BYTE byBadly = 0xff;
	int nMin = 33;
	int nScore;
	BYTE byCard;
	for( BYTE i = 0; i < m_byGoodThreeCount*3; i++ )
	{
		byCard = m_byGoodThreeCard[i];
		if( byCard >= 27 )						//如果是字
		{
			nScore = 2;
		}
		else if( byCard%9 == 0 || byCard%9 == 8 )	//如果是一或者九
		{
			nScore = 6;
		}
		else 
		{
			nScore = 10;
		}
		nScore += AddScore(byCard);
		if( nScore < nMin )
		{
			nMin = nScore;
			byBadly = byCard;
		}
	}
	return byBadly;
}

//设置禁止出的牌
void CAndroidAI::SetEnjoinOutCard( const BYTE cbEnjoinOutCard[],BYTE cbEnjoinOutCount )
{
	m_byEnjoinOutCount = cbEnjoinOutCount;
	if( m_byEnjoinOutCount > 0 )
	{
		CopyMemory(m_byEnjoinOutCard,cbEnjoinOutCard,sizeof(BYTE)*cbEnjoinOutCount);
	}
}

//加权
int CAndroidAI::AddScore( BYTE byCardData )
{
	int nScore = 0;
	if( byCardData >= 27 )
	{
		return 0;
	}
	if( byCardData%9 != 0 && FindIndex(byCardData-1) != 0xff )	//如果剩余的牌中有比其少一的牌
	{
		if( byCardData%9 != 1 )						//如果当前牌不是二类即加3
		{
			nScore += 3;
		}
		else
		{
			nScore += 1;
		}
	}
	if( byCardData%9 != 8 && FindIndex(byCardData+1) != 0xff )	//如果剩余的牌中有比起多一个的牌
	{
		if( byCardData%9 != 7 )
		{
			nScore += 3;
		}
		else
		{
			nScore += 1;
		}
	}
	if( byCardData%9 > 1 && FindIndex(byCardData-2) != 0xff )		//如果剩余的牌中有比其少二的牌（如3—5,1_3等）
	{
		nScore += 2;
	}
	
	if( byCardData%9 < 7 && FindIndex(byCardData+2) != 0xff )		//如果剩余的牌中有比其多二的牌
	{
		nScore += 2;
	}
	//
	if( IsEnjoinOutCard(byCardData) ) nScore = 33;
	return nScore;
}

//分析三只
void CAndroidAI::AnalyseThree()
{
	BYTE byIndex1,byIndex2;
	for( BYTE i = 0; i < m_byCardCount; i++ )
	{
		if( !m_bSelect[i] )
		{
			m_bSelect[i] = true;
			//搜索三只
			if( SearchSameCard(m_byCardData[i],byIndex1,byIndex2) )
			{
				//临时记录
				m_byThreeCard[m_byThreeCount*3] = m_byCardData[i];
				m_byThreeCard[m_byThreeCount*3+1] = m_byCardData[i];
				m_byThreeCard[m_byThreeCount*3+2] = m_byCardData[i];
				
				//递归
				m_byThreeCount++;
				m_nScoreThree += 300;
				m_bSelect[byIndex1] = true;
				m_bSelect[byIndex2] = true;
				AnalyseThree();
				m_bSelect[byIndex1] = false;
				m_bSelect[byIndex2] = false;
				m_nScoreThree -= 300;
				m_byThreeCount--;
			}
			//搜索连牌
			if( SearchLinkCard(m_byCardData[i],byIndex1,byIndex2) )
			{
				//临时记录
				m_byThreeCard[m_byThreeCount*3] = m_byCardData[i];
				m_byThreeCard[m_byThreeCount*3+1] = m_byCardData[i]+1;
				m_byThreeCard[m_byThreeCount*3+2] = m_byCardData[i]+2;

				//递归
				m_byThreeCount++;
				m_nScoreThree += 300;
				m_bSelect[byIndex1] = true;
				m_bSelect[byIndex2] = true;
				AnalyseThree();
				m_bSelect[byIndex1] = false;
				m_bSelect[byIndex2] = false;
				m_nScoreThree -= 300;
				m_byThreeCount--;
			}
			m_bSelect[i] = false;
		}
	}
	//如果搜索到分数更高的
	if( m_nScoreThree > m_nMaxScoreThree )
	{
		//记录剩下的
		m_byRemainThreeCount = 0;
		m_nMaxScoreThree = m_nScoreThree;
		for( i = 0; i < m_byCardCount; i++ )
		{
			if( !m_bSelect[i] )
				m_byRemainThree[m_byRemainThreeCount++] = m_byCardData[i];
		}
		//记录最佳三只组合
		m_byGoodThreeCount = m_byThreeCount;
		CopyMemory(m_byGoodThreeCard,m_byThreeCard,sizeof(m_byThreeCard));
	}
}

//分析两只
void CAndroidAI::AnalyseTwo()
{
	BYTE byIndex;
	for( BYTE i = 0; i < m_byRemainThreeCount; i++ )
	{
		if( !m_bSelect[i] )
		{
			m_bSelect[i] = true;
			//搜索两只相同
			if( SearchSameCardRemain(m_byRemainThree[i],byIndex,i+1) )
			{
				//临时记录
				m_byTwoCard[m_byTwoCount*2] = m_byRemainThree[i];
				m_byTwoCard[m_byTwoCount*2+1] = m_byRemainThree[byIndex];

				//判断将
				m_byTwoCount++;
				int nGoodSame = 90;
				if( !m_bHaveJiang )
				{
					m_bHaveJiang = true;
					nGoodSame = 120;
				}
				//递归
				m_nScoreTwo += nGoodSame;
				m_bSelect[byIndex] = true;
				AnalyseTwo();
				m_bSelect[byIndex] = false;
				if( 120 == nGoodSame )
					m_bHaveJiang = false;
				m_nScoreTwo -= nGoodSame;
				m_byTwoCount--;
			}
			//搜索紧连牌
			if( SearchLinkCardRemain(m_byRemainThree[i],0,byIndex,i+1) )
			{
				//临时记录
				m_byTwoCard[m_byTwoCount*2] = m_byRemainThree[i];
				m_byTwoCard[m_byTwoCount*2+1] = m_byRemainThree[byIndex];

				//判断边
				m_byTwoCount++;
				int nGoodLink;
				if( IsEdge(m_byRemainThree[i],m_byRemainThree[byIndex]) )
					nGoodLink = 80;
				else nGoodLink = 100;
				//递归
				m_nScoreTwo += nGoodLink;
				m_bSelect[byIndex] = true;
				AnalyseTwo();
				m_bSelect[byIndex] = false;
				m_nScoreTwo -= nGoodLink;
				m_byTwoCount--;
			}
			//搜索有卡的连牌
			if( SearchLinkCardRemain(m_byRemainThree[i],1,byIndex,i+1) )
			{				
				//临时记录
				m_byTwoCard[m_byTwoCount*2] = m_byRemainThree[i];
				m_byTwoCard[m_byTwoCount*2+1] = m_byRemainThree[byIndex];

				//判断边
				m_byTwoCount++;
				int nGoodLink;
				if( IsEdge(m_byRemainThree[i],m_byRemainThree[byIndex]) )
					nGoodLink = 70;
				else nGoodLink = 90;
				//递归
				m_nScoreTwo += nGoodLink;
				m_bSelect[byIndex] = true;
				AnalyseTwo();
				m_bSelect[byIndex] = false;
				m_nScoreTwo -= nGoodLink;
				m_byTwoCount--;
			}
			m_bSelect[i] = false;
		}
	}
	//如果有分数更高的
	if( m_nScoreTwo > m_nMaxScoreTwo )
	{
		//记录剩下的
		m_nMaxScoreTwo = m_nScoreTwo;
		m_byRemainTwoCount = 0;
		for( i = 0; i < m_byRemainThreeCount; i++ )
		{
			if( !m_bSelect[i] )
				m_byRemainTwo[m_byRemainTwoCount++] = m_byRemainThree[i];
		}
		//记录最佳两只组合
		m_byGoodTwoCount = m_byTwoCount;
		CopyMemory(m_byGoodTwoCard,m_byTwoCard,sizeof(m_byTwoCard));
	}
}

//分析单只
void CAndroidAI::AnalyseOne()
{
	BYTE byCard;
	int nScore;
	int nMin = 33;
	for(int i = 0;i < m_byRemainTwoCount;i++ )	//找出最差的一张牌
	{
		byCard = m_byRemainTwo[i];

		if( byCard >= 27 )						//如果是字
		{
			nScore = 2;
		}
		else if( byCard%9 == 0 || byCard%9 == 8 )	//如果是一或者九
		{
			nScore = 6;
		}
		else 
		{
			nScore = 10;
		}

		nScore += AddScore(byCard);
		if( nScore < nMin )
		{
			nMin = nScore;
			m_byBadlyCard = byCard;
		}
	}
}

//从两只组合中分析
void CAndroidAI::SearchTwo()
{
	//定义变量
	BYTE byCardTwo[14];
	BYTE byCardTwoCount = 0;
	bool bTeamHave = false;
	//设置变量
	for( int i = 0;i < m_byRemainThreeCount;i++ )
		byCardTwo[byCardTwoCount++] = m_byRemainThree[i];
	//仅有一对
	if( byCardTwoCount == 2 )												//如果只有两张牌
	{
		if( byCardTwo[0] == byCardTwo[1] )								//胡牌
		{
			m_byBadlyCard = byCardTwo[0];								//象山麻将胡牌必须有台
			return;
		}
		for( int j = 0;j < m_byCardCount-1;j++ )						//凑一对将
		{
			if( m_byCardData[j] == m_byCardData[j+1] 
				&& m_byCardData[j] == m_byCardData[j+2] )			//看是否有三个的
			{
				if( !IsEnjoinOutCard(m_byCardData[j]) )
				{
					bTeamHave = true;
					m_byBadlyCard = m_byCardData[j];
					break;
				}
			}
		}
		if( bTeamHave == false )
		{
			m_byRemainTwoCount = 2;
			m_byRemainTwo[0] = byCardTwo[0];
			m_byRemainTwo[1] = byCardTwo[1];
			AnalyseOne();
		}
	}
	else
	{
		bool bSameHave = false;
		int nMinScore = 33;
		int nScore[8];
		memset(nScore,33,sizeof(nScore));
		for( BYTE j = 0;j < byCardTwoCount/2;j++ )					//循环纪录每张牌的分数
		{
			if( IsEnjoinOutCard(byCardTwo[j*2]) || IsEnjoinOutCard(byCardTwo[j*2+1]) ) continue;
			if( byCardTwo[j*2] == byCardTwo[j*2 + 1] )				//对子
			{
				if( bSameHave )
				{
					nScore[j] = 6;
				}
				else 
				{
					nScore[j] = 8;
					bSameHave = true;
				}
			}
			else if( byCardTwo[j*2] == byCardTwo[j*2+1] - 1 )		//紧连门子
			{
				if( byCardTwo[j*2]%9 == 0 || byCardTwo[j*2+1]%9 == 8 )
				{
					nScore[j] = 4;
				}
				else
				{
					nScore[j] = 7;
				}
			}
			else													//漏一个门子
			{
				if( byCardTwo[j*2]%9 == 0 || byCardTwo[j*2+1]%9 == 8 )
					nScore[j] = 3;
				else
					nScore[j] = 5;
			}
			
		}
		for( BYTE k = 0;k < byCardTwoCount/2;k++)						//找出分数最小的一张牌就是最差的一张
		{
			if( nScore[k] < nMinScore )
			{
				nMinScore = nScore[k];
				if( byCardTwo[k*2] % 9 == 0 )
				{
					m_byBadlyCard = byCardTwo[k*2];
				}
				else if( byCardTwo[k*2+1] % 9 == 8 )
				{
					m_byBadlyCard = byCardTwo[k*2+1];
				}
				else 
				{
					m_byBadlyCard = byCardTwo[k*2 + rand()%2];
				}
			}
		}
	}
}

//判断牌是否禁止出
bool CAndroidAI::IsEnjoinOutCard( BYTE byCard )
{
	for( BYTE i = 0; i < m_byEnjoinOutCount; i++ )
	{
		if( byCard == m_byEnjoinOutCard[i] )
			return true;
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////
