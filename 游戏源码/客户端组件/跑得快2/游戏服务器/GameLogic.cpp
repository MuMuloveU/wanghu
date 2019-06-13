
#include "stdafx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameLogic::CGameLogic()
{
	m_LastPisa.nCount=0;
	m_LastPisa.nChairId=0;
	m_bFirst=FALSE;
}

//析构函数
CGameLogic::~CGameLogic()
{
}

int CGameLogic::StartANewGame()
{
	for(int i=0;i<GAME_PLAYER;i++)
		m_PisaArray[i].RemoveAll();
	//开始发牌
	struct tagPisa tmp;
	srand(GetTickCount());
	int Index=0;
	int tmpindex;
#ifdef _TWICE
	int time;
#endif
	for(int row=1;row<=13;row++)
	{
		for(int line=1;line<=4;line++)
		{
#ifdef _TWICE
			for(time=0;time<2;time++)
			{
#endif
			while(1)
			{
				tmpindex=rand()%GAME_PLAYER;
				if (m_PisaArray[tmpindex].GetCount()<PISACOUNT)
					break;
			}
			tmp.row=uncompare[row];
			tmp.line=line;
			if (uncompare[row]==3 && line==4)//黑桃3
				Index=tmpindex;
			m_PisaArray[tmpindex].Add(tmp);
#ifdef _TWICE
			}
#endif
		}
	}
#ifdef _TWICE
	for(time=0;time<2;time++)
	{
		//小王
		while(1)
		{
			tmpindex=rand()%GAME_PLAYER;
			if (m_PisaArray[tmpindex].GetCount()<PISACOUNT)
				break;
		}
		tmp.line=1;
		tmp.row=14;
		m_PisaArray[tmpindex].Add(tmp);
	}
	for(time=0;time<2;time++)
	{
		//大王
		while(1)
		{
			tmpindex=rand()%GAME_PLAYER;
			if (m_PisaArray[tmpindex].GetCount()<PISACOUNT)
				break;
		}
		tmp.line=2;
		tmp.row=14;
		m_PisaArray[tmpindex].Add(tmp);
	}
#endif
	m_LastPisa.nCount=0;
	m_LastPisa.nChairId=0;
	m_bFirst=TRUE;
	return Index;
}

void CGameLogic::GetInitPisa(int nIndex,struct tagSetInitPisa *pPisa)
{
	memset(pPisa,0,sizeof(tagSetInitPisa));
	pPisa->nOwnerIndex=nIndex;
	ASSERT(m_PisaArray[nIndex].GetCount()>=PISACOUNT);
	for(int i=0;i<PISACOUNT;i++)
		pPisa->tPisa[i]=m_PisaArray[nIndex].GetAt(i);
	return;
}

void CGameLogic::EmptyLastCardInfo()
{
	m_LastPisa.nCount=0;
}

BOOL CGameLogic::IsUserBegin(int index)
{
	//如果有最后一次出牌记录而且
	if (m_LastPisa.nCount!=0 && m_LastPisa.nChairId==index)
		return TRUE;
	return FALSE;
}

BOOL CGameLogic::IsRegular(const struct tagSetOutPisa *pisa)
{
	if (pisa->nCount==1) return TRUE;
	else if (pisa->nCount==2)
	{
		//是否为对
		if (pisa->tOutPisa[0].row == pisa->tOutPisa[1].row)
			return TRUE;
		else
			_tcscpy(m_chErrorMsg,_T("您出的牌不是对！"));
	}
	else if (pisa->nCount==3)
	{
		//是否为三同张
		if (pisa->tOutPisa[0].row == pisa->tOutPisa[1].row && pisa->tOutPisa[0].row == pisa->tOutPisa[2].row)
			return TRUE;
		else
			_tcscpy(m_chErrorMsg,_T("您出的牌不是三同张！"));
	}
	else
	{
		int Count;
		struct tagPisa PisaPoint;
		BOOL bSameColor;
		//如果出牌是炸弹
		if (IsBomb(pisa,&Count,&PisaPoint))
		{
			TRACE("符合炸弹规则！\n");
			return TRUE;
		}
		//如果出牌是同三张
		if (IsContinue3(pisa,&PisaPoint))
		{
			TRACE("符合同三张规则！\n");
			return TRUE;
		}
		//如果出牌是连对
		if (IsContinue2(pisa,&PisaPoint))
		{
			TRACE("符合连对规则！\n");
			return TRUE;
		}
		//如果出牌是顺子
		if (IsStraight(pisa,&PisaPoint,&bSameColor))
		{
			TRACE("符合顺子规则！\n");
			return TRUE;
		}
		//如果出牌是三连二
		if (Is3And2(pisa,&PisaPoint))
		{
			TRACE("符合三连二规则！\n");
			return TRUE;
		}
		//如果出牌是蝴蝶
		if (IsButterfly(pisa,&PisaPoint))
		{
			TRACE("符合蝴蝶规则！\n");
			return TRUE;
		}
	}
	_tcscpy(m_chErrorMsg,_T("您出的牌不合符规则！"));
	return FALSE;
}

BOOL CGameLogic::IsBiggerThanLastCard(const struct tagSetOutPisa *pisa)
{
	//判断出牌是否合符要求
	if (!IsRegular(pisa))
		return FALSE;
	_tcscpy(m_chErrorMsg,_T("您出的牌不够大！"));
	if (m_bFirst)
	{
		BOOL m_bFound3=FALSE;
		//如果是第一次出牌，需要检查是不是含有黑桃3
		for(int i=0;i<pisa->nCount;i++)
			if (pisa->tOutPisa[i].row==3 && pisa->tOutPisa[i].line==4)//黑桃3
			{
				m_bFound3=TRUE;
				break;
			}
		if (!m_bFound3)
		{
			_tcscpy(m_chErrorMsg,_T("第一次出牌必须含有黑桃3！"));
			return FALSE;
		}
	}
	//存在最后一次出牌记录
	if (m_LastPisa.nCount!=0)
	{
		int Count,Count1;
		struct tagPisa PisaPoint;
		struct tagPisa PisaPoint1;
		BOOL bIsBomb=IsBomb(pisa,&Count,&PisaPoint);
		BOOL bIsBomb1=IsBomb(&m_LastPisa,&Count1,&PisaPoint1);
		//如果玩家出的是炸弹，而且上家出的不是炸弹，直接返回TRUE
		if (bIsBomb && !bIsBomb1)
			return TRUE;
		//如果玩家出的不是炸弹，而且上家出的是炸弹，直接返回TRUE
		if (!bIsBomb && bIsBomb1)
			return FALSE;
		//如果两个玩家都是炸弹
		if (bIsBomb && bIsBomb1)
		{
			TRACE("炸弹比较：Count:%d,%d, Pisa:%d,%d:%d,%d！\n",Count,Count1,PisaPoint.row,PisaPoint.line,PisaPoint1.row,PisaPoint1.line);
			//玩家出的是天王炸弹
			if (PisaPoint.row==14) return TRUE;
			//上家出的是天王炸弹
			if (PisaPoint1.row==14) return FALSE;
			//张数较多
			if (Count>Count1)
				return TRUE;
			//点数较大
			else if (Count==Count1 && compare[PisaPoint.row]>compare[PisaPoint1.row])
				return TRUE;
			//花式较大
			else if (Count==Count1 && PisaPoint.row==PisaPoint1.row && PisaPoint.line>PisaPoint1.line)
				return TRUE;
			return FALSE;
		}
		//判断出牌数是否相同
		if (pisa->nCount==m_LastPisa.nCount && pisa->nCount<=PISACOUNT)
		{
			if(pisa->nCount==1)
			{
				//点数较大
				if (compare[pisa->tOutPisa[0].row] > compare[m_LastPisa.tOutPisa[0].row])
					return TRUE;
				//花色较大
				else if (pisa->tOutPisa[0].row == m_LastPisa.tOutPisa[0].row &&
					pisa->tOutPisa[0].line > m_LastPisa.tOutPisa[0].line)
					return TRUE;
				TRACE("单张牌比较完毕\n");
			}
			else if (pisa->nCount==2)
			{
				//点数较大
				if (compare[pisa->tOutPisa[0].row] > compare[m_LastPisa.tOutPisa[0].row])
					return TRUE;
				//花色较大
				else if (pisa->tOutPisa[0].row == m_LastPisa.tOutPisa[0].row &&
					max(pisa->tOutPisa[0].line,pisa->tOutPisa[1].line) > max(m_LastPisa.tOutPisa[0].line,m_LastPisa.tOutPisa[1].line))
					return TRUE;
				TRACE("2张牌比较完毕\n");
			}
			else if (pisa->nCount==3)
			{
				//点数较大
				if (compare[pisa->tOutPisa[0].row] > compare[m_LastPisa.tOutPisa[0].row])
					return TRUE;
				//花色较大
				else if (pisa->tOutPisa[0].row == m_LastPisa.tOutPisa[0].row &&
					(max(pisa->tOutPisa[0].line,max(pisa->tOutPisa[1].line,pisa->tOutPisa[2].line)) >
					max(m_LastPisa.tOutPisa[0].line,max(m_LastPisa.tOutPisa[1].line,m_LastPisa.tOutPisa[2].line))))
					return TRUE;
				TRACE("3张牌比较完毕\n");
			}
			else
			{
				//不是以上类型
/*				if (IsBomb(pisa,&Count,&PisaPoint))
				{
					TRACE("玩家出的是炸弹！\n");
					//如果是炸弹
					if (IsBomb(&m_LastPisa,&Count1,&PisaPoint1))
					{
						TRACE("炸弹比较：Count:%d,%d, Pisa:%d,%d:%d,%d！\n",Count,Count1,PisaPoint.row,PisaPoint.line,PisaPoint1.row,PisaPoint1.line);
						//点数较大
						if (compare[PisaPoint.row]>compare[PisaPoint1.row])
							return TRUE;
						//花式较大
						else if (PisaPoint.row==PisaPoint1.row && PisaPoint.line>PisaPoint1.line)
							return TRUE;
					}
					else
						_tcscpy(m_chErrorMsg,_T("您出的牌是炸弹，但是上家出的不是炸弹！"));
				}*/
				//连三同张，连对，顺子都必须张数相同，所以只需要判断点数就可以了
				if (IsContinue3(pisa,&PisaPoint))
				{
					TRACE("玩家出的是三同张！\n");
					//如果是连三同张
					if (IsContinue3(&m_LastPisa,&PisaPoint1))
					{
						TRACE("玩家三同张最大牌：%d,%d,上家最大牌：%d,%d！\n",PisaPoint.line,PisaPoint.row,PisaPoint1.line,PisaPoint1.row);
						//点数较大
						if (compare[PisaPoint.row] > compare[PisaPoint1.row])
							return TRUE;
						//花色较大
						else if (PisaPoint.row == PisaPoint1.row && PisaPoint.line > PisaPoint1.line)
							return TRUE;
					}
					else
						_tcscpy(m_chErrorMsg,_T("您出的牌是三同张，但是上家出的不是三同张！"));
				}
				if (IsContinue2(pisa,&PisaPoint))
				{
					TRACE("玩家出的是连对！\n");
					//如果是连对
					if (IsContinue2(&m_LastPisa,&PisaPoint1))
					{
						TRACE("玩家连对最大牌：%d,%d,上家最大牌：%d,%d！\n",PisaPoint.line,PisaPoint.row,PisaPoint1.line,PisaPoint1.row);
						//点数较大
						if (compare[PisaPoint.row] > compare[PisaPoint1.row])
							return TRUE;
						//花色较大
						else if (PisaPoint.row == PisaPoint1.row && PisaPoint.line > PisaPoint1.line)
							return TRUE;
					}
					else
						_tcscpy(m_chErrorMsg,_T("您出的牌是连对，但是上家出的不是连对！"));
				}
				BOOL bSameColor;
				BOOL bSameColor1;
				if (IsStraight(pisa,&PisaPoint,&bSameColor))
				{
					TRACE("玩家出的是顺子！\n");
					//如果是顺子
					if (IsStraight(&m_LastPisa,&PisaPoint1,&bSameColor1))
					{
						TRACE("玩家顺子最大牌：(!%d) %d,%d,上家最大牌：(!%d) %d,%d！\n",bSameColor,PisaPoint.line,PisaPoint.row,bSameColor1,PisaPoint1.line,PisaPoint1.row);
						//如果是同花顺，而且之前的不是同花顺
						if (bSameColor>bSameColor1)
							return TRUE;
						//点数较大
						if (compare[PisaPoint.row] > compare[PisaPoint1.row])
							return TRUE;
						//花色较大
						else if (PisaPoint.row == PisaPoint1.row && PisaPoint.line > PisaPoint1.line)
							return TRUE;
					}
					else
						_tcscpy(m_chErrorMsg,_T("您出的牌是顺子，但是上家出的不是顺子！"));
				}
				//如果出牌是三连二
				if (Is3And2(pisa,&PisaPoint))
				{
					TRACE("玩家出的是三连二！\n");
					//如果是三连二
					if (Is3And2(&m_LastPisa,&PisaPoint1))
					{
						TRACE("玩家三连二最大牌：%d,%d,上家最大牌：%d,%d！\n",PisaPoint.line,PisaPoint.row,PisaPoint1.line,PisaPoint1.row);
						//点数较大
						if (compare[PisaPoint.row] > compare[PisaPoint1.row])
							return TRUE;
						//花色较大
						else if (PisaPoint.row == PisaPoint1.row && PisaPoint.line > PisaPoint1.line)
							return TRUE;
					}
					else
						_tcscpy(m_chErrorMsg,_T("您出的牌是三连二，但是上家出的不是三连二！"));
				}
				//如果出牌是蝴蝶
				if (IsButterfly(pisa,&PisaPoint))
				{
					TRACE("玩家出的是蝴蝶！\n");
					//如果是三连二
					if (Is3And2(&m_LastPisa,&PisaPoint1))
					{
						TRACE("玩家蝴蝶最大牌：%d,%d,上家最大牌：%d,%d！\n",PisaPoint.line,PisaPoint.row,PisaPoint1.line,PisaPoint1.row);
						//点数较大
						if (compare[PisaPoint.row] > compare[PisaPoint1.row])
							return TRUE;
						//花色较大
						else if (PisaPoint.row == PisaPoint1.row && PisaPoint.line > PisaPoint1.line)
							return TRUE;
					}
					else
						_tcscpy(m_chErrorMsg,_T("您出的牌是蝴蝶，但是上家出的不是蝴蝶！"));
				}
			}
		}
		else
		{
/*			//如果出牌数不同，需要比较是不是炸弹
			if (IsBomb(pisa,&Count,&PisaPoint) && IsBomb(&m_LastPisa,&Count1,&PisaPoint1))
			{
			}
			else*/
			{
				_tcscpy(m_chErrorMsg,_T("您出的牌数量与上家不同，而且不是炸弹！"));
			}
		}
	}
	else//没有出牌记录，当前出牌通过
		return TRUE;
	return FALSE;
}

char *CGameLogic::GetCompareError()
{
	return m_chErrorMsg;
}

BOOL CGameLogic::SaveLastCard(const struct tagSetOutPisa *pisa)
{
	//在对应玩家的初始牌里面移除对应多的牌
	m_PisaArray[pisa->nChairId].RemoveAt(0,pisa->nCount);
	memcpy(&m_LastPisa,pisa,sizeof(tagSetOutPisa));
	m_bFirst=FALSE;
	return (m_PisaArray[pisa->nChairId].GetCount()!=0);
}

int CGameLogic::GetPlayerLeftCard(int nIndex)
{
	int count=(int)m_PisaArray[nIndex].GetCount();
	//一张没出基数乘4倍；出牌不到1/4基数乘3倍；出牌不到1/2基数乘 2倍
	if (count>=PISACOUNT) return count*4;
	else if (count>=PISACOUNT-PISACOUNT/4) return count*3;
	else if (count>=PISACOUNT-PISACOUNT/2) return count*2;
	return count;
}

//判断牌中是否为连对，返回的PisaCount是最大牌值
BOOL CGameLogic::IsContinue2(const struct tagSetOutPisa *pisa,struct tagPisa *PisaPoint)
{
	if (!_IsContinue(pisa,PisaPoint,2)) return FALSE;
	//必须要3对以上
	if (pisa->nCount<6) return FALSE;
	for(int i=0;i<pisa->nCount;i++)
	{
		//2不能出现在连对上
		if (pisa->tOutPisa[i].row==2) return FALSE;
	}
	return TRUE;
}

//判断牌中是否为连三同张，返回的PisaCount是最大牌值
BOOL CGameLogic::IsContinue3(const struct tagSetOutPisa *pisa,struct tagPisa *PisaPoint)
{
	if (!_IsContinue(pisa,PisaPoint,3)) return FALSE;
	//必须要3个连三同张以上
	if (pisa->nCount<9) return FALSE;
	for(int i=0;i<pisa->nCount;i++)
	{
		//2不能出现在连三同张上
		if (pisa->tOutPisa[i].row==2) return FALSE;
	}
	return TRUE;
}

//判断牌中是否为顺子，返回的PisaCount是最大牌值，bSameColor表示是否为同花顺
BOOL CGameLogic::IsStraight(const struct tagSetOutPisa *pisa,struct tagPisa *PisaPoint,BOOL *bSameColor)
{
	if (!_IsContinue(pisa,PisaPoint,1)) return FALSE;
	//顺子必须5张以上
	if (pisa->nCount<5) return FALSE;
	for(int i=0;i<pisa->nCount;i++)
	{
		//2不能出现在顺子上
		if (pisa->tOutPisa[i].row==2) return FALSE;
	}
	int line=pisa->tOutPisa[0].line;
	*bSameColor=TRUE;
	for(int i=1;i<pisa->nCount;i++)
	{
		if (pisa->tOutPisa[i].line!=line)
		{
			*bSameColor=FALSE;
			break;
		}
	}
	return TRUE;
}

//判断牌中是否为三连二，返回的PisaCount是最大牌值
BOOL CGameLogic::Is3And2(const struct tagSetOutPisa *pisa,struct tagPisa *PisaPoint)
{
	//三连二必须是5张的
	if (pisa->nCount!=5) return FALSE;
	//开始两张和结尾两张必须一样
	if (pisa->tOutPisa[0].row!=pisa->tOutPisa[1].row ||
		pisa->tOutPisa[3].row!=pisa->tOutPisa[4].row)
		return FALSE;
	if (pisa->tOutPisa[2].row==pisa->tOutPisa[3].row)
	{
		PisaPoint->row=pisa->tOutPisa[2].row;
		PisaPoint->line=max(pisa->tOutPisa[2].line,max(pisa->tOutPisa[3].line,pisa->tOutPisa[4].line));
		return TRUE;
	}
	else if (pisa->tOutPisa[2].row==pisa->tOutPisa[1].row)
	{
		PisaPoint->row=pisa->tOutPisa[2].row;
		PisaPoint->line=max(pisa->tOutPisa[2].line,max(pisa->tOutPisa[1].line,pisa->tOutPisa[0].line));
		return TRUE;
	}
	return FALSE;
}

//判断牌中是否为蝴蝶牌型，返回的PisaCount是最大牌值
BOOL CGameLogic::IsButterfly(const struct tagSetOutPisa *pisa,struct tagPisa *PisaPoint)
{
	//蝴蝶必须是15张
	if (pisa->nCount!=15) return FALSE;
	struct tagSetOutPisa p1,p2;
	struct tagPisa Pisa,Pisa1;
	//假设是三张的在前
	memset(&p1,0,sizeof(p1));
	memset(&p2,0,sizeof(p2));
	p1.nCount=9;
	p2.nCount=6;
	for(int i=0;i<15;i++)
	{
		if (i<9)
			p1.tOutPisa[i]=pisa->tOutPisa[i];
		else
			p2.tOutPisa[i-9]=pisa->tOutPisa[i];
	}
	if (_IsContinue(&p1,&Pisa,3) && _IsContinue(&p2,&Pisa1,2))
	{
		//是蝴蝶牌型
		memcpy(PisaPoint,&Pisa,sizeof(Pisa));
		return TRUE;
	}
	//假设是三张的在后
	memset(&p1,0,sizeof(p1));
	memset(&p2,0,sizeof(p2));
	p1.nCount=6;
	p2.nCount=9;
	for(int i=0;i<15;i++)
	{
		if (i<6)
			p1.tOutPisa[i]=pisa->tOutPisa[i];
		else
			p2.tOutPisa[i-6]=pisa->tOutPisa[i];
	}
	if (_IsContinue(&p1,&Pisa,2) && _IsContinue(&p2,&Pisa1,3))
	{
		//是蝴蝶牌型
		memcpy(PisaPoint,&Pisa1,sizeof(Pisa));
		return TRUE;
	}
	return FALSE;
}

//判断牌中是否为炸弹，返回的Count是张数，返回的PisaCount是最大牌值
BOOL CGameLogic::IsBomb(const struct tagSetOutPisa *pisa,int *Count,struct tagPisa *PisaPoint)
{
	int row=pisa->tOutPisa[0].row;
	int maxline=pisa->tOutPisa[0].line;
	for(int i=1;i<pisa->nCount;i++)
	{
		if (pisa->tOutPisa[i].row!=row)
			return FALSE;
		if (pisa->tOutPisa[i].line>maxline)
			maxline=pisa->tOutPisa[i].line;
	}
	if (pisa->nCount<4) return FALSE;
	*Count=pisa->nCount;
	PisaPoint->line=maxline;
	PisaPoint->row=row;
	return TRUE;
}

BOOL CGameLogic::_IsContinue(const struct tagSetOutPisa *pisa,struct tagPisa *PisaPoint,int offest)
{
	//牌数不能整除offest
	if (pisa->nCount%offest!=0)
	{
		TRACE("连续牌比较函数：牌数不能整除offest!\n");
		return FALSE;
	}
	//检查其中的值是否合法
	int NotNeighborCount=0;
	for(int i=0;i<pisa->nCount;i+=offest)
	{
		//如果是对或者三连张的，检查相邻的牌是否同样
		if (offest>1)
		{
			for(int j=i+1;j<i+offest;j++)
				if (pisa->tOutPisa[i].row != pisa->tOutPisa[j].row)
				{
					TRACE("连续牌比较函数：相邻牌不同样[%d]:%d,[%d]:%d！\n",i,pisa->tOutPisa[i].row,j,pisa->tOutPisa[j].row);
					return FALSE;
				}
		}
		//检查相间的牌是否是连续的
		if (i!=0)
			if (!IsNeighbor(pisa->tOutPisa[i].row,pisa->tOutPisa[i-1].row))
			{
				NotNeighborCount++;
				TRACE("连续牌比较函数：发现%d次不是相邻：[%d]:%d,[%d]:%d！\n",NotNeighborCount,i-1,pisa->tOutPisa[i-1].row,i,pisa->tOutPisa[i].row);
				//如果断层是由于连续牌造成的，应该退出
				if (pisa->tOutPisa[i].row==pisa->tOutPisa[i-1].row)
					return FALSE;
			}
	}
	BOOL bEnd=FALSE;
	//比较最后一张牌和第一张牌是否连续,只出现在2和3之间的情况
	if (!IsNeighbor(pisa->tOutPisa[0].row,pisa->tOutPisa[pisa->nCount-1].row))
	{
		TRACE("连续牌比较函数：发现最后的牌和第一张牌不相邻：[%d]:%d,[%d]:%d！\n",0,pisa->tOutPisa[0].row,pisa->nCount-1,pisa->tOutPisa[pisa->nCount-1].row);
		NotNeighborCount++;
	}
	else
		bEnd=TRUE;
	//由于连续是可以允许回环的,所以这里判断如果不连续的情况出现两次，就说明出错牌了
	if (NotNeighborCount>=2)
	{
		TRACE("连续牌比较函数：牌面不连续，出现%d次不相邻！\n",NotNeighborCount);
		return FALSE;
	}
	if (bEnd)
	{
		//说明是有头尾连接的，因为需要判断是否含有K和3
		if (IsBothHave3AndK(pisa))
		{
			TRACE("连续牌比较函数：牌面有头尾连接，而且含有K和3！\n");
			return FALSE;
		}
	}
	GetMaxPoint(pisa,PisaPoint);
	return TRUE;
}

BOOL CGameLogic::IsBothHave3AndK(const struct tagSetOutPisa *pisa)
{
	BOOL bFound3=FALSE;
	BOOL bFoundK=FALSE;
	for(int i=0;i<pisa->nCount;i++)
	{
		if (pisa->tOutPisa[i].row==3)
			bFound3=TRUE;
		else if (pisa->tOutPisa[i].row==13)
			bFoundK=TRUE;
	}
	return (bFound3 && bFoundK);
}

void CGameLogic::GetMaxPoint(const struct tagSetOutPisa *pisa,struct tagPisa *PisaPoint)
{
	struct tagPisa Max;//最大值
	struct tagPisa No12Max;//没有A、2的最大值
	BOOL bFound3=FALSE;
	Max.line=0;
	Max.row=0;
	No12Max.line=0;
	No12Max.row=0;
	for(int i=0;i<pisa->nCount;i++)
	{
		int value=compare[pisa->tOutPisa[i].row];
		if (pisa->tOutPisa[i].row==3)
			bFound3=TRUE;
		if (value<12 && compare[No12Max.row]<value)
		{
			No12Max.row=pisa->tOutPisa[i].row;
			No12Max.line=pisa->tOutPisa[i].line;
		}
		if (compare[Max.row]<value)
		{
			Max.row=pisa->tOutPisa[i].row;
			Max.line=pisa->tOutPisa[i].line;
		}
	}
	if (bFound3)
		memcpy(PisaPoint,&No12Max,sizeof(No12Max));
	else
		memcpy(PisaPoint,&Max,sizeof(Max));
}

BOOL CGameLogic::IsNeighbor(int index,int index2)
{
	if (index>index2 && index-index2==1) return TRUE;
	if (index<index2 && index2-index==1) return TRUE;
	if ((index==13 && index2==1) ||
		(index==1 && index2==13)) return TRUE;
	return FALSE;
}
