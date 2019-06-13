#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameLogic::CGameLogic()
{
	//设置变量
	memset(m_cbStepCount,0,sizeof(m_cbStepCount));
	memset(m_cbIdentifier,0,sizeof(m_cbIdentifier));
	memset(m_cbChessBorad,0,sizeof(m_cbChessBorad));

	return;
}

//析构函数
CGameLogic::~CGameLogic()
{
}

//获取步数
BYTE CGameLogic::GetStepCount(BYTE cbColor)
{
	ASSERT((cbColor==BLACK_CHESS)||(cbColor==WHITE_CHESS));
	return m_cbStepCount[(cbColor==BLACK_CHESS)?0:1];
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
	//清理棋谱
	m_ChessManualInfo.RemoveAll();

	//棋子变量
	memset(m_cbStepCount,0,sizeof(m_cbStepCount));
	memset(m_cbIdentifier,0,sizeof(m_cbIdentifier));
	memset(m_cbChessBorad,0,sizeof(m_cbChessBorad));

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

		//棋盘变量
		m_cbStepCount[(pChessManual->cbColor==BLACK_CHESS)?0:1]--;
		m_cbChessBorad[pChessManual->cbXPos][pChessManual->cbYPos]=NO_CHESS;
	}

	//删除棋谱
	m_ChessManualInfo.RemoveAt(nManualCount-wStepCount,wStepCount);

	return true;
}

//放置棋子
BYTE CGameLogic::PlaceChess(BYTE cbXPos, BYTE cbYPos, BYTE cbColor)
{
	//效验参数
	ASSERT((cbXPos<15)&&(cbYPos<15));
	ASSERT(m_cbChessBorad[cbXPos][cbYPos]==NO_CHESS);
	if ((cbXPos>=15)||(cbYPos>=15)) return false;
	if (m_cbChessBorad[cbXPos][cbYPos]!=NO_CHESS) return false;

	//禁手判断
	if (cbColor==BLACK_CHESS)
	{
	}

	//设置棋盘
	m_cbChessBorad[cbXPos][cbYPos]=cbColor;
	m_cbStepCount[(cbColor==BLACK_CHESS)?0:1]++;
	m_cbIdentifier[cbXPos][cbYPos]=(BYTE)(m_ChessManualInfo.GetCount()+1);

	//记录棋谱
	tagChessManual ChessManual;
	ChessManual.cbXPos=cbXPos;
	ChessManual.cbYPos=cbYPos;
	ChessManual.cbColor=cbColor;
	m_ChessManualInfo.Add(ChessManual);

	return m_cbIdentifier[cbXPos][cbYPos];
}

//胜利判断
bool CGameLogic::WinEstimate(BYTE cbXPos, BYTE cbYPos)
{
	//变量定义
	int nMoveWay[4][2]={{1,0},{0,1},{1,-1},{1,1}};
	BYTE cbChess=m_cbChessBorad[cbXPos][cbYPos];
	BYTE cbXPosTemp=0,cbYPosTemp=0,cbChessCount=0;

	//胜利判断
	for (int i=0;i<sizeof(nMoveWay)/sizeof(nMoveWay[0]);i++)
	{
		//设置变量
		cbChessCount=0;

		//正向计数
		cbXPosTemp=cbXPos;
		cbYPosTemp=cbYPos;
		do
		{
			//移动棋点
			cbXPosTemp+=nMoveWay[i][0];
			cbYPosTemp+=nMoveWay[i][1];
			if ((cbXPosTemp>=15)||(cbYPosTemp>=15)) break;
			if (m_cbChessBorad[cbXPosTemp][cbYPosTemp]!=cbChess) break;

			//累计棋数
			cbChessCount++;
			if (cbChessCount==4) return true;

		} while (true);

		//反向计数
		cbXPosTemp=cbXPos;
		cbYPosTemp=cbYPos;
		do
		{
			//移动棋点
			cbXPosTemp-=nMoveWay[i][0];
			cbYPosTemp-=nMoveWay[i][1];
			if ((cbXPosTemp>=15)||(cbYPosTemp>=15)) break;
			if (m_cbChessBorad[cbXPosTemp][cbYPosTemp]!=cbChess) break;

			//累计棋数
			cbChessCount++;
			if (cbChessCount==4) return true;

		} while (true);
	};

	return false;
}

//////////////////////////////////////////////////////////////////////////
