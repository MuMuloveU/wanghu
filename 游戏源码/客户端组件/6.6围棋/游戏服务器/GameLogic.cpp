#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameLogic::CGameLogic()
{
	//状态变量
	memset(m_wStepCount,0,sizeof(m_wStepCount));
	memset(m_wTakeChessCount,0,sizeof(m_wTakeChessCount));
	memset(m_cbLastChessBorad,0,sizeof(m_cbLastChessBorad));

	//棋盘变量
	memset(m_wIdentifier,0,sizeof(m_wIdentifier));
	memset(m_cbChessBorad,0,sizeof(m_cbChessBorad));
	
	return;
}

//析构函数
CGameLogic::~CGameLogic()
{
}

//获取步数
WORD CGameLogic::GetStepCount(BYTE cbColor)
{
	ASSERT((cbColor==BLACK_CHESS)||(cbColor==WHITE_CHESS));
	return m_wStepCount[(cbColor==BLACK_CHESS)?0:1];
}

//提子数目
WORD CGameLogic::GetTakeChessCount(BYTE cbColor)
{
	ASSERT((cbColor==BLACK_CHESS)||(cbColor==WHITE_CHESS));
	return m_wTakeChessCount[(cbColor==BLACK_CHESS)?0:1];
}

//获取棋谱
const tagChessManual * CGameLogic::GetLastChessManual()
{
	INT_PTR nManualCount=m_ChessManualInfo.GetCount();
	return (nManualCount>0)?&m_ChessManualInfo[nManualCount-1]:NULL;
}

//初始逻辑
void CGameLogic::InitGameLogic()
{
	//状态变量
	memset(m_wStepCount,0,sizeof(m_wStepCount));
	memset(m_wTakeChessCount,0,sizeof(m_wTakeChessCount));
	memset(m_cbLastChessBorad,0,sizeof(m_cbLastChessBorad));

	//棋盘变量
	memset(m_wIdentifier,0,sizeof(m_wIdentifier));
	memset(m_cbChessBorad,0,sizeof(m_cbChessBorad));

	//棋谱变量
	m_DeadChessInfo.RemoveAll();
	m_ChessManualInfo.RemoveAll();

	return;
}

//悔棋控制
bool CGameLogic::RegretChess(WORD wStepCount)
{
	//效验参数
	ASSERT(wStepCount<=(WORD)m_ChessManualInfo.GetCount());

	//变量定义
	tagChessManual * pChessManual=NULL;
	INT_PTR nManualCount=m_ChessManualInfo.GetCount();

	//悔棋操作
	for (WORD i=0;i<wStepCount;i++)
	{
		//变量定义
		pChessManual=&m_ChessManualInfo[nManualCount-i-1];

		//状态变量
		m_wStepCount[(pChessManual->cbColor==BLACK_CHESS)?0:1]--;
		m_wTakeChessCount[(pChessManual->cbColor==BLACK_CHESS)?0:1]-=pChessManual->wDeadChessCount;

		//棋盘变量
		m_wIdentifier[pChessManual->cbXPos][pChessManual->cbYPos]=0;
		m_cbChessBorad[pChessManual->cbXPos][pChessManual->cbYPos]=NO_CHESS;

		//恢复死棋
		if (pChessManual->wDeadChessCount>0)
		{
			INT_PTR wDeadChessInfoCount=m_DeadChessInfo.GetCount();
			BYTE cbDeadColor=(pChessManual->cbColor==BLACK_CHESS)?WHITE_CHESS:BLACK_CHESS;
			for (WORD wDeadChessCount=0;wDeadChessCount<pChessManual->wDeadChessCount;wDeadChessCount++)
			{
				tagChessItem * pChessItem=&m_DeadChessInfo[wDeadChessInfoCount-wDeadChessCount-1];
				m_cbChessBorad[pChessItem->cbXPos][pChessItem->cbYPos]=cbDeadColor;
				m_wIdentifier[pChessItem->cbXPos][pChessItem->cbYPos]=pChessItem->wIdentifier;
			}
			m_DeadChessInfo.RemoveAt(wDeadChessInfoCount-pChessManual->wDeadChessCount,pChessManual->wDeadChessCount);
		}
	}

	//删除棋谱
	m_ChessManualInfo.RemoveAt(nManualCount-wStepCount,wStepCount);

	return true;
}

//点目统计
bool CGameLogic::GetChessCountInfo(tagCountResult & CountResult)
{
	//点目过滤
	ASSERT((m_wStepCount[0]+m_wStepCount[1])>0);
	if ((m_wStepCount[0]+m_wStepCount[1])==0) return false;

	//设置变量
	tagCountAssistant CountAssistant;
	memset(&CountResult,0,sizeof(CountResult));
	memset(&CountAssistant,0,sizeof(CountAssistant));

	//点目统计
	int i=0;
	for (BYTE cbXPos=0;cbXPos<19;cbXPos++)
	{
		for (BYTE cbYPos=0;cbYPos<19;cbYPos++)
		{
			//空位过滤
			if (m_cbChessBorad[cbXPos][cbYPos]==NO_CHESS) continue;

			//设置变量
			CountAssistant.cbCurIndex=(m_cbChessBorad[cbXPos][cbYPos]==BLACK_CHESS)?0:1;
			CountAssistant.cbChessMask=(m_cbChessBorad[cbXPos][cbYPos]==BLACK_CHESS)?CT_BLACE_CHESS:CT_WHITE_CHESS;

			//区域设置
			CountResult.wChessCount[CountAssistant.cbCurIndex]++;
			CountResult.cbCountInfo[cbXPos][cbYPos]|=CountAssistant.cbChessMask;

			//扩张准备
			CountAssistant.wChessCount[CountAssistant.cbCurIndex]=1;
			CountAssistant.wChessList[CountAssistant.cbCurIndex][0]=MAKEWORD(cbXPos,cbYPos);

			//区域扩张
			for (WORD wExpandIndex=0;wExpandIndex<CountAssistant.wChessCount[CountAssistant.cbCurIndex];wExpandIndex++)
			{
				BYTE cbXChessPos=LOBYTE(CountAssistant.wChessList[CountAssistant.cbCurIndex][wExpandIndex]);
				BYTE cbYChessPos=HIBYTE(CountAssistant.wChessList[CountAssistant.cbCurIndex][wExpandIndex]);
				GetCrossExpand(cbXChessPos,cbYChessPos,CountResult,CountAssistant);
			}
		}
	}

	//公共数目
	ASSERT((CountResult.wChessCount[0]+CountResult.wChessCount[1])>=361);
	CountResult.wCommomCount=CountResult.wChessCount[0]+CountResult.wChessCount[1]-361;

	return true;
}

//放置棋子
WORD CGameLogic::PlaceChess(BYTE cbXPos, BYTE cbYPos, BYTE cbColor, WORD wDeadChess[361], WORD & wDeadChessCount)
{
	//效验参数
	ASSERT((cbXPos<19)&&(cbYPos<19));
	ASSERT(m_cbChessBorad[cbXPos][cbYPos]==NO_CHESS);

	//变量定义
	tagAreaResult AreaResult;
	memset(&AreaResult,0,sizeof(AreaResult));

	//设置棋盘
	m_cbChessBorad[cbXPos][cbYPos]=cbColor;
	m_wIdentifier[cbXPos][cbYPos]=(WORD)(m_ChessManualInfo.GetCount()+1);

	//下棋分析
	AreaResult.wChessCount=1;
	AreaResult.wChessList[0]=MAKEWORD(cbXPos,cbYPos);
	for (WORD wIndex=0;((wIndex<AreaResult.wChessCount)&&(AreaResult.wBurrowCount==0));wIndex++)
	{
		BYTE cbXChessPos=LOBYTE(AreaResult.wChessList[wIndex]);
		BYTE cbYChessPos=HIBYTE(AreaResult.wChessList[wIndex]);
		GetCrossArea(cbXChessPos,cbYChessPos,AreaResult);
	}

	//提取死棋
	wDeadChessCount=TakeOutChess(cbXPos,cbYPos,wDeadChess);

	//死穴判断
	if ((AreaResult.wBurrowCount==0)&&(wDeadChessCount==0))
	{
		m_cbChessBorad[cbXPos][cbYPos]=NO_CHESS;
		return 0;
	}

	//打劫判断
	if (memcmp(m_cbLastChessBorad[cbColor-1],m_cbChessBorad,sizeof(m_cbChessBorad))==0) 
	{
		m_cbChessBorad[cbXPos][cbYPos]=NO_CHESS;
		return 0xFFFF;
	}

	//记录状态
	CopyMemory(m_cbLastChessBorad[cbColor-1],m_cbChessBorad,sizeof(m_cbChessBorad));

	//状态变量
	m_wStepCount[(cbColor==BLACK_CHESS)?0:1]++;
	m_wTakeChessCount[(cbColor==BLACK_CHESS)?0:1]+=wDeadChessCount;

	//构造棋谱
	tagChessManual ChessManual;
	ChessManual.cbXPos=cbXPos;
	ChessManual.cbYPos=cbYPos;
	ChessManual.cbColor=cbColor;
	ChessManual.wDeadChessCount=wDeadChessCount;

	//记录棋谱
	m_ChessManualInfo.Add(ChessManual);
	if (wDeadChessCount>0)
	{
		//设置大小
		INT_PTR nChessItemCount=m_DeadChessInfo.GetCount();
		m_DeadChessInfo.SetSize(nChessItemCount+wDeadChessCount);

		//构造数据
		tagChessItem ChessItem;
		memset(&ChessItem,0,sizeof(ChessItem));

		//加入棋子
		for (WORD i=0;i<wDeadChessCount;i++)
		{
			ChessItem.cbXPos=LOBYTE(wDeadChess[i]);
			ChessItem.cbYPos=HIBYTE(wDeadChess[i]);
			ChessItem.wIdentifier=m_wIdentifier[ChessItem.cbXPos][ChessItem.cbYPos];
			CopyMemory(&m_DeadChessInfo[nChessItemCount+i],&ChessItem,sizeof(ChessItem));
		}
	}

	return m_wIdentifier[cbXPos][cbYPos];
}

//提子处理
WORD CGameLogic::TakeOutChess(BYTE cbXPos, BYTE cbYPos, WORD wDeadChess[361])
{
	//效验参数
	ASSERT((cbXPos<19)&&(cbYPos<19));
	ASSERT(m_cbChessBorad[cbXPos][cbYPos]!=NO_CHESS);

	//变量定义
	WORD wDeadChessCount=0;
	tagAreaResult AreaResult;
	memset(&AreaResult,0,sizeof(AreaResult));

	//吃子分析
	bool bAnalyse[4]={true,true,true,true};
	const int nExcursion[4][2]={{1,0},{0,1},{-1,0},{0,-1}};
	for (int i=0;i<CountArray(nExcursion);i++)
	{
		//分析排除
		if (bAnalyse[i]==false)	continue;

		//变量定义
		BYTE cbXStationPos=cbXPos+nExcursion[i][0];
		BYTE cbYStationPos=cbYPos+nExcursion[i][1];

		//棋子分析
		if ((cbXStationPos>=19)|(cbYStationPos>=19)) continue;
		if ((m_cbChessBorad[cbXStationPos][cbYStationPos]==NO_CHESS)||
			(m_cbChessBorad[cbXStationPos][cbYStationPos]==m_cbChessBorad[cbXPos][cbYPos])) continue;

		//气穴分析
		memset(&AreaResult,0,sizeof(AreaResult));
		GetChessArea(cbXStationPos,cbYStationPos,AreaResult);

		//同域判断
		for (int j=(i+1);j<CountArray(nExcursion);j++)
		{
			if (bAnalyse[j]==true)
			{
				BYTE cbXStationPos=cbXPos+nExcursion[j][0];
				BYTE cbYStationPos=cbYPos+nExcursion[j][1];
				if ((cbXStationPos>=19)|(cbYStationPos>=19)) bAnalyse[j]=false;
				else if (AreaResult.cbAreaResult[cbXStationPos][cbYStationPos]==AREA_CHESS) bAnalyse[j]=false;
			}
		}

		//合并死棋
		if (AreaResult.wBurrowCount==0)
		{
			ASSERT((wDeadChessCount+AreaResult.wChessCount)<=361);
			CopyMemory(wDeadChess+wDeadChessCount,AreaResult.wChessList,AreaResult.wChessCount*sizeof(AreaResult.wChessList[0]));
			wDeadChessCount+=AreaResult.wChessCount;
		}
	}

	//删除棋子
	for (WORD wIndex=0;wIndex<wDeadChessCount;wIndex++)
	{
		BYTE cbXChessPos=LOBYTE(wDeadChess[wIndex]);
		BYTE cbYChessPos=HIBYTE(wDeadChess[wIndex]);
		m_cbChessBorad[cbXChessPos][cbYChessPos]=NO_CHESS;
	}

	return wDeadChessCount;
}

//获取区域
void CGameLogic::GetChessArea(BYTE cbXPos, BYTE cbYPos, tagAreaResult & AreaResult)
{
	//效验参数
	ASSERT((cbXPos<19)&&(cbYPos<19));
	ASSERT(m_cbChessBorad[cbXPos][cbYPos]!=NO_CHESS);

	//设置变量
	AreaResult.wChessCount=1;
	AreaResult.wBurrowCount=0;
	AreaResult.wChessList[0]=MAKEWORD(cbXPos,cbYPos);
	AreaResult.cbAreaResult[cbXPos][cbYPos]=AREA_CHESS;
	
	//区域统计
	for (WORD wIndex=0;wIndex<AreaResult.wChessCount;wIndex++)
	{
		BYTE cbXChessPos=LOBYTE(AreaResult.wChessList[wIndex]);
		BYTE cbYChessPos=HIBYTE(AreaResult.wChessList[wIndex]);
		GetCrossArea(cbXChessPos,cbYChessPos,AreaResult);
	}

	return;
}

//十字区域
void CGameLogic::GetCrossArea(BYTE cbXPos, BYTE cbYPos, tagAreaResult & AreaResult)
{
	//效验参数
	ASSERT((cbXPos<19)&&(cbYPos<19));
	ASSERT(m_cbChessBorad[cbXPos][cbYPos]!=NO_CHESS);

	//变量定义
	BYTE cbChessColor=m_cbChessBorad[cbXPos][cbYPos];
	BYTE cbOtherChessColor=(cbChessColor==BLACK_CHESS)?WHITE_CHESS:BLACK_CHESS;

	//区域分析
	const int nExcursion[4][2]={{1,0},{0,1},{-1,0},{0,-1}};
	for (int i=0;i<CountArray(nExcursion);i++)
	{
		//变量定义
		BYTE cbXStationPos=cbXPos+nExcursion[i][0];
		BYTE cbYStationPos=cbYPos+nExcursion[i][1];

		//区域分析
		for (;((cbXStationPos<19)&&(cbYStationPos<19));cbXStationPos+=nExcursion[i][0],cbYStationPos+=nExcursion[i][1])
		{
			//区域过滤
			if (m_cbChessBorad[cbXStationPos][cbYStationPos]==cbOtherChessColor) break;
			if (AreaResult.cbAreaResult[cbXStationPos][cbYStationPos]!=AREA_UNKNOW) break;

			//自己棋子
			if (m_cbChessBorad[cbXStationPos][cbYStationPos]==cbChessColor)	
			{
				AreaResult.cbAreaResult[cbXStationPos][cbYStationPos]=AREA_CHESS;
				AreaResult.wChessList[AreaResult.wChessCount++]=MAKEWORD(cbXStationPos,cbYStationPos);
			}
			else //洞穴区域
			{
				AreaResult.wBurrowCount++;
				AreaResult.cbAreaResult[cbXStationPos][cbYStationPos]=AREA_BURROW;
				break;
			}
		}
	}

	return;
}

//十字扩张
void CGameLogic::GetCrossExpand(BYTE cbXPos, BYTE cbYPos, tagCountResult & CountResult, tagCountAssistant & CountAssistant)
{
	//效验参数
	ASSERT((cbXPos<19)&&(cbYPos<19));

	//扩张分析
	const int nExcursion[4][2]={{1,0},{0,1},{-1,0},{0,-1}};
	for (int i=0;i<CountArray(nExcursion);i++)
	{
		//变量定义
		BYTE cbXStationPos=cbXPos+nExcursion[i][0];
		BYTE cbYStationPos=cbYPos+nExcursion[i][1];

		//区域分析
		for (;((cbXStationPos<19)&&(cbYStationPos<19));cbXStationPos+=nExcursion[i][0],cbYStationPos+=nExcursion[i][1])
		{
			//扩张过滤
			if (m_cbChessBorad[cbXStationPos][cbYStationPos]!=NO_CHESS) break;
			if ((CountResult.cbCountInfo[cbXStationPos][cbYStationPos]&CountAssistant.cbChessMask)!=0) break;

			//扩张处理
			BYTE cbCurIndex=CountAssistant.cbCurIndex;
			CountResult.wChessCount[CountAssistant.cbCurIndex]++;
			CountResult.cbCountInfo[cbXStationPos][cbYStationPos]|=CountAssistant.cbChessMask;
			CountAssistant.wChessList[cbCurIndex][CountAssistant.wChessCount[cbCurIndex]++]=MAKEWORD(cbXStationPos,cbYStationPos);
		}
	}

	return;
}

//////////////////////////////////////////////////////////////////////////

