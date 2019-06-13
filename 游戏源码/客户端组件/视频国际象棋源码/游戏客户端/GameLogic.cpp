#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameLogic::CGameLogic()
{
	//辅助变量
	memset(m_pItemLately,0,sizeof(m_pItemLately));

	//棋盘变量
	memset(m_ChessItem,0,sizeof(m_ChessItem));
	memset(m_wStepCount,0,sizeof(m_wStepCount));
	memset(m_ChessBorad,0,sizeof(m_ChessBorad));

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
	return m_wStepCount[GetColorIndex(cbColor)];
}

//获取棋谱
const tagChessManual * CGameLogic::GetLastChessManual()
{
	INT_PTR nManualCount=m_ChessManualInfo.GetCount();
	return (nManualCount>0)?&m_ChessManualInfo[nManualCount-1]:NULL;
}

//获取棋子
const tagChessItem * CGameLogic::GetChessItem(BYTE cbXPos, BYTE cbYPos)
{
	//效验参数
	ASSERT((cbXPos<8)&&(cbYPos<8));
	return m_ChessBorad[cbXPos][cbYPos];
}

//重置棋盘
void CGameLogic::ResetChessBorad()
{
	//变量定义
	BYTE cbChessInfo[8]={CHESS_ROOK,CHESS_HORSE,CHESS_ELEPHANT,CHESS_KING,CHESS_QUEEN,
		CHESS_ELEPHANT,CHESS_HORSE,CHESS_ROOK};

	//设置变量
	m_ChessManualInfo.RemoveAll();
	memset(m_ChessItem,0,sizeof(m_ChessItem));
	memset(m_wStepCount,0,sizeof(m_wStepCount));
	memset(m_ChessBorad,0,sizeof(m_ChessBorad));

	//设置数据
	BYTE cbYPos=255;
	for (BYTE cbUser=0;cbUser<2;cbUser++)
	{
		for (BYTE nIndex=0;nIndex<8;nIndex++)
		{
			//设置棋子
			cbYPos=(cbUser==0)?7:0;
			m_ChessItem[cbUser][nIndex].cbYPos=cbYPos;
			m_ChessItem[cbUser][nIndex].cbXPos=nIndex;
			m_ChessItem[cbUser][nIndex].cbChessID=nIndex;
			m_ChessItem[cbUser][nIndex].cbChess=cbChessInfo[nIndex];
			m_ChessItem[cbUser][nIndex].cbColor=BLACK_CHESS+cbUser;
			m_ChessBorad[nIndex][cbYPos]=&m_ChessItem[cbUser][nIndex];

			//设置棋子
			cbYPos=(cbUser==0)?6:1;
			m_ChessItem[cbUser][nIndex+8].cbYPos=cbYPos;
			m_ChessItem[cbUser][nIndex+8].cbXPos=nIndex;
			m_ChessItem[cbUser][nIndex+8].cbChessID=nIndex+8;
			m_ChessItem[cbUser][nIndex+8].cbChess=CHESS_SOLDIER;
			m_ChessItem[cbUser][nIndex+8].cbColor=BLACK_CHESS+cbUser;
			m_ChessBorad[nIndex][cbYPos]=&m_ChessItem[cbUser][nIndex+8];
		}
	}

	return;
}

//更新棋盘
void CGameLogic::UpdateChessBorad()
{
	//重置棋盘
	memset(m_ChessBorad,0,sizeof(m_ChessBorad));
	memset(m_pItemLately,0,sizeof(m_pItemLately));

	//更新棋盘
	for (BYTE i=0;i<2;i++)
	{
		for (BYTE j=0;j<16;j++)
		{
			if ((m_ChessItem[i][j].cbXPos!=255)&&(m_ChessItem[i][j].cbYPos!=255))
			{
				ASSERT(m_ChessItem[i][j].cbXPos<8);
				ASSERT(m_ChessItem[i][j].cbYPos<8);
				m_ChessBorad[m_ChessItem[i][j].cbXPos][m_ChessItem[i][j].cbYPos]=&m_ChessItem[i][j];
			}
		}
	}

	//最近走棋
	tagChessManual * pChessManual=NULL;
	INT_PTR nManualCount=m_ChessManualInfo.GetCount(),nIndex=0;
	if (nManualCount>=1)
	{
		nIndex=(nManualCount-1)%2;
		pChessManual=&m_ChessManualInfo[nManualCount-1];
		m_pItemLately[nIndex]=&m_ChessItem[nIndex][pChessManual->cbSourceID];
	}
	if (nManualCount>=2)
	{
		nIndex=nManualCount%2;
		pChessManual=&m_ChessManualInfo[nManualCount-2];
		m_pItemLately[nIndex]=&m_ChessItem[nIndex][pChessManual->cbSourceID];
	}

	return;
}

//设置棋谱
void CGameLogic::InsertChessManual(tagChessManual ChessManual[], WORD wManualCount)
{
	INT_PTR nScrCount=m_ChessManualInfo.GetCount();
	m_ChessManualInfo.SetSize(nScrCount+wManualCount);
	for (WORD i=0;i<wManualCount;i++) m_ChessManualInfo[nScrCount+i]=ChessManual[i];

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

		//获取棋子
		tagChessItem * pSourceChessItem=&m_ChessItem[GetColorIndex(pChessManual->cbSourceColor)][pChessManual->cbSourceID];
		tagChessItem * pTargetChessItem=(pChessManual->cbUniteChess!=NO_CHESS)?pTargetChessItem=&m_ChessItem[GetColorIndex(pChessManual->cbUniteColor)][pChessManual->cbUniteID]:NULL;

		//效验坐标
		ASSERT(pSourceChessItem->cbXPos!=255);
		ASSERT(pSourceChessItem->cbYPos!=255);

		//设置步数
		m_wStepCount[GetColorIndex(pChessManual->cbSourceColor)]--;

		//设置源棋
		pSourceChessItem->wWalkCount--;
		pSourceChessItem->cbChess=pChessManual->cbSourceChess;
		pSourceChessItem->cbXPos=pChessManual->cbXSourceChessPos;
		pSourceChessItem->cbYPos=pChessManual->cbYSourceChessPos;

		//关联棋子
		if (pTargetChessItem!=NULL)
		{
			pTargetChessItem->cbXPos=pChessManual->cbXUniteChessPos;
			pTargetChessItem->cbYPos=pChessManual->cbYUniteChessPos;
			if (pSourceChessItem->cbColor==pTargetChessItem->cbColor) pTargetChessItem->wWalkCount--;
		}
	}

	//删除棋谱
	m_ChessManualInfo.RemoveAt(nManualCount-wStepCount,wStepCount);

	//更新棋盘
	UpdateChessBorad();

	return true;
}

//移动棋子
enMoveType CGameLogic::MoveChess(BYTE cbXSourcePos, BYTE cbYSourcePos, BYTE cbXTargetPos, BYTE cbYTargetPos, BYTE cbSwitchChess)
{
	//效验坐标
	ASSERT((cbXSourcePos<8)&&(cbYSourcePos<8));
	ASSERT((cbXTargetPos<8)&&(cbYTargetPos<8));

	//获取棋子
	tagChessItem * pSourceChessItem=m_ChessBorad[cbXSourcePos][cbYSourcePos];
	tagChessItem * pTargetChessItem=m_ChessBorad[cbXTargetPos][cbYTargetPos];

	//棋谱变量
	tagChessManual ChessManual;
	memset(&ChessManual,0,sizeof(ChessManual));
	ChessManual.cbXSourceChessPos=cbXSourcePos;
	ChessManual.cbYSourceChessPos=cbYSourcePos;
	ChessManual.cbXTargetChessPos=cbXTargetPos;
	ChessManual.cbYTargetChessPos=cbYTargetPos;
	ChessManual.cbSourceID=pSourceChessItem->cbChessID;
	ChessManual.cbSourceColor=pSourceChessItem->cbColor;
	ChessManual.cbSourceChess=pSourceChessItem->cbChess;

	//兵的升变
	enMoveType MoveChessType=enMoveType_Normal;
	if ((pSourceChessItem->cbChess==CHESS_SOLDIER)&&((cbYTargetPos==0)||(cbYTargetPos==7)))
	{
		if ((cbSwitchChess<CHESS_QUEEN)||(cbSwitchChess>CHESS_HORSE)) return enMoveType_Error;
		pSourceChessItem->cbChess=cbSwitchChess;
		ChessManual.cbSwitchChess=cbSwitchChess;
		MoveChessType=enMoveType_SoldierUp;
	}
	else 
	{
		cbSwitchChess=NO_CHESS;
		ChessManual.cbSwitchChess=cbSwitchChess;
	}

	//王车易位
	if ((pTargetChessItem!=NULL)&&(pTargetChessItem->cbColor==pSourceChessItem->cbColor))
	{
		//获取棋子
		tagChessItem * pKingChessItem=(pTargetChessItem->cbChess==CHESS_KING)?pTargetChessItem:pSourceChessItem;
		tagChessItem * pRookChessItem=(pTargetChessItem->cbChess==CHESS_ROOK)?pTargetChessItem:pSourceChessItem;

		//位置定义
		BYTE cbXKingPos=pKingChessItem->cbXPos;
		BYTE cbXRookPos=pRookChessItem->cbXPos;
		BYTE cbXKingExcursion=(cbXKingPos>cbXRookPos)?-2:2;
		BYTE cbXRookExcursion=(cbXKingPos>cbXRookPos)?1:-1;

		//记录棋谱
		ChessManual.cbUniteID=pTargetChessItem->cbChessID;
		ChessManual.cbUniteColor=pTargetChessItem->cbColor;
		ChessManual.cbUniteChess=pTargetChessItem->cbChess;
		ChessManual.cbXUniteChessPos=pTargetChessItem->cbXPos;
		ChessManual.cbYUniteChessPos=pTargetChessItem->cbYPos;
		m_ChessManualInfo.Add(ChessManual);

		//设置棋子
		pKingChessItem->wWalkCount++;
		pRookChessItem->wWalkCount++;
		pKingChessItem->cbXPos+=cbXKingExcursion;
		pRookChessItem->cbXPos=pKingChessItem->cbXPos+cbXRookExcursion;

		//设置棋盘
		m_ChessBorad[cbXKingPos][pKingChessItem->cbYPos]=NULL;
		m_ChessBorad[cbXRookPos][pRookChessItem->cbYPos]=NULL;
		m_ChessBorad[pKingChessItem->cbXPos][pKingChessItem->cbYPos]=pKingChessItem;
		m_ChessBorad[pRookChessItem->cbXPos][pRookChessItem->cbYPos]=pRookChessItem;

		return enMoveType_KingRook;
	}

	//吃子判断
	if (pTargetChessItem!=NULL)
	{
		//记录棋谱
		ChessManual.cbUniteID=pTargetChessItem->cbChessID;
		ChessManual.cbUniteColor=pTargetChessItem->cbColor;
		ChessManual.cbUniteChess=pTargetChessItem->cbChess;
		ChessManual.cbXUniteChessPos=pTargetChessItem->cbXPos;
		ChessManual.cbYUniteChessPos=pTargetChessItem->cbYPos;

		//设置棋子
		pTargetChessItem->cbXPos=255;
		pTargetChessItem->cbYPos=255;
		m_ChessBorad[cbXTargetPos][cbYTargetPos]=NULL;
	}

	//过路兵判断
	if ((pTargetChessItem==NULL)&&(pSourceChessItem->cbChess==CHESS_SOLDIER)&&(cbXSourcePos!=cbXTargetPos))
	{
		//获取棋子
		tagChessItem * pEnemyChessItem=m_ChessBorad[cbXTargetPos][cbYSourcePos];
		ASSERT(pEnemyChessItem!=NULL);

		//记录棋谱
		ChessManual.cbUniteID=pEnemyChessItem->cbChessID;
		ChessManual.cbUniteColor=pEnemyChessItem->cbColor;
		ChessManual.cbUniteChess=pEnemyChessItem->cbChess;
		ChessManual.cbXUniteChessPos=pEnemyChessItem->cbXPos;
		ChessManual.cbYUniteChessPos=pEnemyChessItem->cbYPos;

		//设置棋子
		pEnemyChessItem->cbXPos=255;
		pEnemyChessItem->cbYPos=255;
		m_ChessBorad[cbXTargetPos][cbYSourcePos]=NULL;
	}

	//记录棋谱
	m_ChessManualInfo.Add(ChessManual);
	m_wStepCount[pSourceChessItem->cbColor-1]++;
	m_pItemLately[pSourceChessItem->cbColor-1]=pSourceChessItem;

	//移动棋子
	pSourceChessItem->wWalkCount++;
	pSourceChessItem->cbXPos=cbXTargetPos;
	pSourceChessItem->cbYPos=cbYTargetPos;
	m_ChessBorad[cbXSourcePos][cbYSourcePos]=NULL;
	m_ChessBorad[cbXTargetPos][cbYTargetPos]=pSourceChessItem;

	return MoveChessType;
}

//结束判断
bool CGameLogic::IsGameFinish(BYTE cbColor)
{
	//获取王棋
	tagChessItem * pKindChessItem=&m_ChessItem[(cbColor==BLACK_CHESS)?0:1][3];

	//死亡判断
	return ((pKindChessItem->cbXPos==255)&&(pKindChessItem->cbYPos==255));
}

//杀王判断
bool CGameLogic::IsKingDanger(BYTE cbColor)
{
	//获取王棋
	tagChessItem * pKindChessItem=&m_ChessItem[(cbColor==BLACK_CHESS)?0:1][3];

	//获取坐标
	ASSERT(pKindChessItem->cbXPos!=255);
	ASSERT(pKindChessItem->cbYPos!=255);

	//危险判断
	return IsChessDanger(pKindChessItem->cbXPos,pKindChessItem->cbYPos);
}

//危险判断
bool CGameLogic::IsChessDanger(BYTE cbXTargetPos, BYTE cbYTargetPos)
{
	//效验变量
	ASSERT((cbXTargetPos<8)&&(cbYTargetPos<8));

	//获取棋子
	tagChessItem * pTargetChessItem=m_ChessBorad[cbXTargetPos][cbYTargetPos];
	if (pTargetChessItem==NULL) return false;

	//索引定义
	BYTE cbEnemyIndex=(pTargetChessItem->cbColor==BLACK_CHESS)?1:0;

	//棋子搜索
	BYTE cbXSourcePos=255,cbYSourcePos=255;
	for (BYTE i=0;i<CountArray(m_ChessItem[cbEnemyIndex]);i++)
	{
		//获取棋子
		tagChessItem * pSourceChessItem=&m_ChessItem[cbEnemyIndex][i];
		if ((pSourceChessItem->cbXPos==255)||(pSourceChessItem->cbYPos==255)) continue;

		//获取坐标
		cbXSourcePos=pSourceChessItem->cbXPos;
		cbYSourcePos=pSourceChessItem->cbYPos;

		//路径搜索
		switch (pSourceChessItem->cbChess)
		{
		case CHESS_KING:	//王
			{
				//偏移变量
				BYTE cbXExcursion=GetExcursion(cbXSourcePos,cbXTargetPos);
				BYTE cbYExcursion=GetExcursion(cbYSourcePos,cbYTargetPos);

				//步法判断
				if ((cbXExcursion<=1)&&(cbYExcursion<=1)) return true;

				break;
			}
		case CHESS_QUEEN:	//后
			{
				//偏移变量
				BYTE cbXExcursion=GetExcursion(cbXSourcePos,cbXTargetPos);
				BYTE cbYExcursion=GetExcursion(cbYSourcePos,cbYTargetPos);

				//步法判断
				if ((cbXSourcePos!=cbXTargetPos)&&(cbYSourcePos!=cbYTargetPos)&&(cbXExcursion!=cbYExcursion)) break;

				//阻塞分析
				if (ObstructAnalyse(cbXSourcePos,cbYSourcePos,cbXTargetPos,cbYTargetPos)==false) return true;

				break;
			}
		case CHESS_ROOK:	//车
			{
				//步法判断
				if ((cbXSourcePos!=cbXTargetPos)&&(cbYSourcePos!=cbYTargetPos)) break;

				//阻塞分析
				if (ObstructAnalyse(cbXSourcePos,cbYSourcePos,cbXTargetPos,cbYTargetPos)==false) return true;

				break;
			}
		case CHESS_ELEPHANT://象
			{
				//偏移变量
				BYTE cbXExcursion=GetExcursion(cbXSourcePos,cbXTargetPos);
				BYTE cbYExcursion=GetExcursion(cbYSourcePos,cbYTargetPos);

				//步法判断
				if ((cbXExcursion==0)||(cbYExcursion==0)||(cbXExcursion!=cbYExcursion)) break;

				//阻塞分析
				if (ObstructAnalyse(cbXSourcePos,cbYSourcePos,cbXTargetPos,cbYTargetPos)==false) return true;

				break;
			}
		case CHESS_HORSE:	//马
			{
				//偏移变量
				BYTE cbXExcursion=GetExcursion(cbXSourcePos,cbXTargetPos);
				BYTE cbYExcursion=GetExcursion(cbYSourcePos,cbYTargetPos);

				//步法判断
				if ((cbXExcursion==1)&&(cbYExcursion==2)) return true;
				if ((cbXExcursion==2)&&(cbYExcursion==1)) return true;

				break;
			}
		case CHESS_SOLDIER:	//兵
			{
				//变量定义
				int nYStep=(pSourceChessItem->cbColor==BLACK_CHESS)?-1:1;
				BYTE cbXExcursion=GetExcursion(cbXSourcePos,cbXTargetPos);

				//步法判断
				if (cbXExcursion==1)
				{
					if ((cbYSourcePos+nYStep)==cbYTargetPos) return true;
					if ((cbYSourcePos==cbYTargetPos)&&(pTargetChessItem->cbChess==CHESS_SOLDIER)&&(pTargetChessItem->wWalkCount==1)) return true;
				}

				break;
			}
		}
	}

	return false;
}

//效验步法
bool CGameLogic::IsWalkLegality(BYTE cbXSourcePos, BYTE cbYSourcePos, BYTE cbXTargetPos, BYTE cbYTargetPos)
{
	//效验坐标
	ASSERT((cbXSourcePos<8)&&(cbYSourcePos<8));
	ASSERT((cbXTargetPos<8)&&(cbYTargetPos<8));

	//获取棋子
	tagChessItem * pSourceChessItem=m_ChessBorad[cbXSourcePos][cbYSourcePos];
	tagChessItem * pTargetChessItem=m_ChessBorad[cbXTargetPos][cbYTargetPos];

	//效验起点
	ASSERT(pSourceChessItem!=NULL);
	if (pSourceChessItem==NULL) return false;

	//王车易位
	if ((pTargetChessItem!=NULL)&&(pTargetChessItem->cbColor==pSourceChessItem->cbColor))
	{
		//步数判断
		if ((pTargetChessItem->wWalkCount!=0)&&(pSourceChessItem->wWalkCount!=0)) return false;

		//王棋判断
		tagChessItem * pKingChessItem=NULL;
		if (pTargetChessItem->cbChess==CHESS_KING) pKingChessItem=pTargetChessItem;
		else if (pSourceChessItem->cbChess==CHESS_KING) pKingChessItem=pTargetChessItem;
		else return false;

		//车棋判断
		tagChessItem * pRookChessItem=NULL;
		if (pTargetChessItem->cbChess==CHESS_ROOK) pKingChessItem=pTargetChessItem;
		else if (pSourceChessItem->cbChess==CHESS_ROOK) pKingChessItem=pTargetChessItem;
		else return false;

		//阻挡判断
		return (ObstructAnalyse(cbXSourcePos,cbYSourcePos,cbXTargetPos,cbYTargetPos)==false);
	}

    //走棋判断
	switch (pSourceChessItem->cbChess)
	{
	case CHESS_KING:	//王
		{
			//偏移变量
			BYTE cbXExcursion=GetExcursion(cbXSourcePos,cbXTargetPos);
			BYTE cbYExcursion=GetExcursion(cbYSourcePos,cbYTargetPos);

			//步法判断
			if ((cbXExcursion>1)||(cbYExcursion>1)) return false;

			return true;
		}
	case CHESS_QUEEN:	//后
		{
			//偏移变量
			BYTE cbXExcursion=GetExcursion(cbXSourcePos,cbXTargetPos);
			BYTE cbYExcursion=GetExcursion(cbYSourcePos,cbYTargetPos);

			//步法判断
			if ((cbXSourcePos!=cbXTargetPos)&&(cbYSourcePos!=cbYTargetPos)&&(cbXExcursion!=cbYExcursion)) return false;

			//阻塞分析
			return (ObstructAnalyse(cbXSourcePos,cbYSourcePos,cbXTargetPos,cbYTargetPos)==false);
		}
	case CHESS_ROOK:	//车
		{
			//步法判断
			if ((cbXSourcePos!=cbXTargetPos)&&(cbYSourcePos!=cbYTargetPos)) return false;

			//阻塞分析
			return (ObstructAnalyse(cbXSourcePos,cbYSourcePos,cbXTargetPos,cbYTargetPos)==false);
		}
	case CHESS_ELEPHANT://象
		{
			//偏移变量
			BYTE cbXExcursion=GetExcursion(cbXSourcePos,cbXTargetPos);
			BYTE cbYExcursion=GetExcursion(cbYSourcePos,cbYTargetPos);

			//步法判断
			if ((cbXExcursion==0)||(cbYExcursion==0)||(cbXExcursion!=cbYExcursion)) return false;

			//阻塞分析
			return (ObstructAnalyse(cbXSourcePos,cbYSourcePos,cbXTargetPos,cbYTargetPos)==false);
		}
	case CHESS_HORSE:	//马
		{
			//偏移变量
			BYTE cbXExcursion=GetExcursion(cbXSourcePos,cbXTargetPos);
			BYTE cbYExcursion=GetExcursion(cbYSourcePos,cbYTargetPos);

			//步法判断
			if ((cbXExcursion==1)&&(cbYExcursion==2)) return true;
			if ((cbXExcursion==2)&&(cbYExcursion==1)) return true;

			return false;
		}
	case CHESS_SOLDIER:	//兵
		{
			//变量定义
			int nYStep=(pSourceChessItem->cbColor==BLACK_CHESS)?-1:1;

			//直行判断
			if (cbXSourcePos==cbXTargetPos)
			{
				if (pTargetChessItem!=NULL) return false;
				if ((cbYSourcePos+nYStep)==cbYTargetPos) return true;
				if ((pSourceChessItem->wWalkCount==0)&&((cbYSourcePos+nYStep*2)==cbYTargetPos)
					&&(m_ChessBorad[cbXSourcePos][cbYSourcePos+nYStep]==NULL)) return true;
				return false;
			}

			//斜行判断
			if ((cbYSourcePos+nYStep)!=cbYTargetPos) return false;
			if ((GetExcursion(cbXSourcePos,cbXTargetPos))!=1) return false;

			//吃子判断
			if ((pTargetChessItem!=NULL)&&(pTargetChessItem->cbColor!=pSourceChessItem->cbColor)) return true;

			//过路兵判断
			BYTE cbYTempPos=(pSourceChessItem->cbColor==BLACK_CHESS)?3:4;
			if ((pTargetChessItem==NULL)&&(cbYSourcePos==cbYTempPos))
			{
				tagChessItem * pTempChessItem=m_pItemLately[(pSourceChessItem->cbColor==BLACK_CHESS)?1:0];
				if (pTempChessItem==NULL) return false;
				if (pTempChessItem->wWalkCount!=1) return false;
				if (pTempChessItem->cbXPos!=cbXTargetPos) return false;
				if (pTempChessItem->cbChess!=CHESS_SOLDIER) return false;
				return true;
			}

			return false;
		}
	}

	ASSERT(FALSE);

	return false;
}

//获取索引
BYTE CGameLogic::GetColorIndex(BYTE cbChessColor)
{
	return (cbChessColor==BLACK_CHESS)?0:1;
}

//获取偏移
BYTE CGameLogic::GetExcursion(BYTE cbSourcePos, BYTE cbTargetPos)
{
	return (cbSourcePos>cbTargetPos)?(cbSourcePos-cbTargetPos):(cbTargetPos-cbSourcePos);
}

//阻塞分析
bool CGameLogic::ObstructAnalyse(BYTE cbXSourcePos, BYTE cbYSourcePos, BYTE cbXTargetPos, BYTE cbYTargetPos)
{
	//效验坐标
	ASSERT((cbXSourcePos!=cbXTargetPos)||(cbYSourcePos!=cbYTargetPos));

	//变量定义
	int nXStep=0,nYStep=0;
	if (cbXTargetPos!=cbXSourcePos) nXStep=(cbXTargetPos>cbXSourcePos)?1:-1;
	if (cbYTargetPos!=cbYSourcePos) nYStep=(cbYTargetPos>cbYSourcePos)?1:-1;

	//阻塞分析
	do
	{
		cbXSourcePos+=nXStep;
		cbYSourcePos+=nYStep;
		if ((cbXSourcePos==cbXTargetPos)&&(cbYSourcePos==cbYTargetPos))	return false;
	} while (m_ChessBorad[cbXSourcePos][cbYSourcePos]==NULL);

	return true;
}

//////////////////////////////////////////////////////////////////////////
