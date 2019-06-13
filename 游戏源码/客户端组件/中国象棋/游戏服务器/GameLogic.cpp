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
	ASSERT((cbXPos<9)&&(cbYPos<10));
	return m_ChessBorad[cbXPos][cbYPos];
}

//重置棋盘
void CGameLogic::ResetChessBorad()
{
	//变量定义
	BYTE cbChessInfo[9]={CHESS_ROOK,CHESS_HORSE,CHESS_ELEPHANT,CHESS_KAVASS,CHESS_GENERAL,
		CHESS_KAVASS,CHESS_ELEPHANT,CHESS_HORSE,CHESS_ROOK};

	//设置变量
	m_ChessManualInfo.RemoveAll();
	memset(m_ChessItem,0,sizeof(m_ChessItem));
	memset(m_wStepCount,0,sizeof(m_wStepCount));
	memset(m_ChessBorad,0,sizeof(m_ChessBorad));

	//设置数据
	for (BYTE cbUserIndex=0;cbUserIndex<2;cbUserIndex++)
	{
		//主线棋子
		BYTE cbExcursion=0;
		BYTE cbYPos=(cbUserIndex==0)?9:0;
		for (BYTE nIndex=0;nIndex<9;nIndex++,cbExcursion++)
		{
			m_ChessItem[cbUserIndex][cbExcursion].cbYPos=cbYPos;
			m_ChessItem[cbUserIndex][cbExcursion].cbXPos=nIndex;
			m_ChessItem[cbUserIndex][cbExcursion].cbChessID=nIndex;
			m_ChessItem[cbUserIndex][cbExcursion].cbChess=cbChessInfo[nIndex];
			m_ChessItem[cbUserIndex][cbExcursion].cbColor=BLACK_CHESS+cbUserIndex;
			m_ChessBorad[nIndex][cbYPos]=&m_ChessItem[cbUserIndex][cbExcursion];
		}

		//兵的棋子
		cbYPos=(cbUserIndex==0)?6:3;
		for (BYTE nIndex=0;nIndex<5;nIndex++,cbExcursion++)
		{
			m_ChessItem[cbUserIndex][cbExcursion].cbYPos=cbYPos;
			m_ChessItem[cbUserIndex][cbExcursion].cbXPos=nIndex*2;
			m_ChessItem[cbUserIndex][cbExcursion].cbChess=CHESS_SOLDIER;
			m_ChessItem[cbUserIndex][cbExcursion].cbChessID=cbExcursion;
			m_ChessItem[cbUserIndex][cbExcursion].cbColor=BLACK_CHESS+cbUserIndex;
			m_ChessBorad[nIndex*2][cbYPos]=&m_ChessItem[cbUserIndex][cbExcursion];
		}

		//炮的棋子
		cbYPos=(cbUserIndex==0)?7:2;
		for (BYTE nIndex=0;nIndex<2;nIndex++,cbExcursion++)
		{
			m_ChessItem[cbUserIndex][cbExcursion].cbYPos=cbYPos;
			m_ChessItem[cbUserIndex][cbExcursion].cbXPos=1+nIndex*6;
			m_ChessItem[cbUserIndex][cbExcursion].cbChess=CHESS_CANNON;
			m_ChessItem[cbUserIndex][cbExcursion].cbChessID=cbExcursion;
			m_ChessItem[cbUserIndex][cbExcursion].cbColor=BLACK_CHESS+cbUserIndex;
			m_ChessBorad[1+nIndex*6][cbYPos]=&m_ChessItem[cbUserIndex][cbExcursion];
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
				ASSERT(m_ChessItem[i][j].cbXPos<9);
				ASSERT(m_ChessItem[i][j].cbYPos<10);
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
		BYTE cbMeIndex=GetColorIndex(pChessManual->cbSourceColor);
		tagChessItem * pSourceChessItem=&m_ChessItem[cbMeIndex][pChessManual->cbSourceID];
		tagChessItem * pTargetChessItem=(pChessManual->cbUniteChess!=NO_CHESS)?pTargetChessItem=&m_ChessItem[(cbMeIndex+1)%2][pChessManual->cbUniteID]:NULL;

		//效验坐标
		ASSERT(pSourceChessItem->cbXPos!=255);
		ASSERT(pSourceChessItem->cbYPos!=255);

		//设置步数
		m_wStepCount[GetColorIndex(pChessManual->cbSourceColor)]--;

		//设置源棋
		pSourceChessItem->cbChess=pChessManual->cbSourceChess;
		pSourceChessItem->cbXPos=pChessManual->cbXSourceChessPos;
		pSourceChessItem->cbYPos=pChessManual->cbYSourceChessPos;

		//关联棋子
		if (pTargetChessItem!=NULL)
		{
			pTargetChessItem->cbXPos=pChessManual->cbXTargetChessPos;
			pTargetChessItem->cbYPos=pChessManual->cbYTargetChessPos;
		}
	}

	//删除棋谱
	m_ChessManualInfo.RemoveAt(nManualCount-wStepCount,wStepCount);

	//更新棋盘
	UpdateChessBorad();

	return true;
}

//移动棋子
bool CGameLogic::MoveChess(BYTE cbXSourcePos, BYTE cbYSourcePos, BYTE cbXTargetPos, BYTE cbYTargetPos)
{
	//效验坐标
	ASSERT((cbXSourcePos<9)&&(cbYSourcePos<10));
	ASSERT((cbXTargetPos<9)&&(cbYTargetPos<10));

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

	//吃子判断
	if (pTargetChessItem!=NULL)
	{
		//设置棋子
		pTargetChessItem->cbXPos=255;
		pTargetChessItem->cbYPos=255;

		//设置棋谱
		ChessManual.cbUniteID=pTargetChessItem->cbChessID;
		ChessManual.cbUniteChess=pTargetChessItem->cbChess;
	}

	//记录棋谱
	m_ChessManualInfo.Add(ChessManual);
	m_wStepCount[pSourceChessItem->cbColor-1]++;
	m_pItemLately[pSourceChessItem->cbColor-1]=pSourceChessItem;

	//移动棋子
	pSourceChessItem->cbXPos=cbXTargetPos;
	pSourceChessItem->cbYPos=cbYTargetPos;
	m_ChessBorad[cbXSourcePos][cbYSourcePos]=NULL;
	m_ChessBorad[cbXTargetPos][cbYTargetPos]=pSourceChessItem;

	return true;
}

//结束判断
bool CGameLogic::IsGameFinish(BYTE cbColor)
{
	//获取将棋
	tagChessItem * pKindChessItem=&m_ChessItem[(cbColor==BLACK_CHESS)?0:1][4];

	//死亡判断
	return ((pKindChessItem->cbXPos==255)&&(pKindChessItem->cbYPos==255));
}

//杀王判断
bool CGameLogic::IsKingDanger(BYTE cbColor)
{
	//对方王棋
	tagChessItem * pKindChessItem=&m_ChessItem[(cbColor==BLACK_CHESS)?1:0][4];
	if ((pKindChessItem->cbXPos==255)||(pKindChessItem->cbYPos==255)) return false;

	//自己王棋
	pKindChessItem=&m_ChessItem[(cbColor==BLACK_CHESS)?0:1][4];
	if ((pKindChessItem->cbXPos==255)||(pKindChessItem->cbYPos==255)) return false;

	//危险判断
	return IsChessDanger(pKindChessItem->cbXPos,pKindChessItem->cbYPos);
}

//危险判断
bool CGameLogic::IsChessDanger(BYTE cbXTargetPos, BYTE cbYTargetPos)
{
	//效验变量
	ASSERT((cbXTargetPos<9)&&(cbYTargetPos<10));

	//获取棋子
	tagChessItem * pTargetChessItem=m_ChessBorad[cbXTargetPos][cbYTargetPos];
	if (pTargetChessItem==NULL) return false;

	//变量定义
	BYTE cbXSourcePos=255,cbYSourcePos=255;
	BYTE cbEnemyIndex=(pTargetChessItem->cbColor==BLACK_CHESS)?1:0;

	//棋子搜索
	for (BYTE i=0;i<CountArray(m_ChessItem[cbEnemyIndex]);i++)
	{
		//获取棋子
		const tagChessItem * pSourceChessItem=&m_ChessItem[cbEnemyIndex][i];
		if ((pSourceChessItem->cbXPos==255)||(pSourceChessItem->cbYPos==255)) continue;

		//威胁判断
		if (IsWalkLegality(pSourceChessItem,cbXTargetPos,cbYTargetPos)==true) return true;
	}

	return false;
}

//效验步法
bool CGameLogic::IsWalkLegality(const tagChessItem * pSourceChessItem, BYTE cbXTargetPos, BYTE cbYTargetPos)
{
	//效验坐标
	ASSERT(pSourceChessItem!=NULL);
	ASSERT((cbXTargetPos<9)&&(cbYTargetPos<10));
	ASSERT((pSourceChessItem->cbXPos<9)&&(pSourceChessItem->cbYPos<10));

	//获取棋子
	BYTE cbXSourcePos=pSourceChessItem->cbXPos;
	BYTE cbYSourcePos=pSourceChessItem->cbYPos;
	tagChessItem * pTargetChessItem=m_ChessBorad[cbXTargetPos][cbYTargetPos];

	//目标棋子
	if ((pTargetChessItem!=NULL)&&(pTargetChessItem->cbColor==pSourceChessItem->cbColor)) return false;

    //走棋判断
	switch (pSourceChessItem->cbChess)
	{
	case CHESS_GENERAL:	//将
		{
			//偏移变量
			BYTE cbXExcursion=GetExcursion(cbXSourcePos,cbXTargetPos);
			BYTE cbYExcursion=GetExcursion(cbYSourcePos,cbYTargetPos);

			//飞将判断
			if ((pTargetChessItem!=NULL)&&(pTargetChessItem->cbChess==CHESS_GENERAL))
			{
				if (cbXSourcePos!=cbXTargetPos) return false;
				if (ObstructAnalyse(cbXSourcePos,cbYSourcePos,cbXTargetPos,cbYTargetPos)==true) return false;
				return true;
			}

			//步法判断
			if ((cbXTargetPos<3)||(cbXTargetPos>5)) return false;
			if ((cbXExcursion>1)||(cbYExcursion>1)) return false;
			if ((cbXExcursion==1)&&(cbYExcursion==1)) return false;
			if ((pSourceChessItem->cbColor==BLACK_CHESS)&&(cbYTargetPos<7)) return false;
			if ((pSourceChessItem->cbColor==WHITE_CHESS)&&(cbYTargetPos>2)) return false;

			return true;
		}
	case CHESS_KAVASS:	//士
		{
			//偏移变量
			BYTE cbXExcursion=GetExcursion(cbXSourcePos,cbXTargetPos);
			BYTE cbYExcursion=GetExcursion(cbYSourcePos,cbYTargetPos);

			//步法判断
			if ((cbXExcursion!=1)||(cbYExcursion!=1)) return false;
			if ((cbXTargetPos<3)||(cbXTargetPos>5)) return false;
			if ((pSourceChessItem->cbColor==BLACK_CHESS)&&(cbYTargetPos<7)) return false;
			if ((pSourceChessItem->cbColor==WHITE_CHESS)&&(cbYTargetPos>2)) return false;

			return true;
		}
	case CHESS_ELEPHANT://象
		{
			//偏移变量
			BYTE cbXExcursion=GetExcursion(cbXSourcePos,cbXTargetPos);
			BYTE cbYExcursion=GetExcursion(cbYSourcePos,cbYTargetPos);

			//步法判断
			if ((cbXExcursion!=2)||(cbYExcursion!=2)) return false;
			if ((pSourceChessItem->cbColor==BLACK_CHESS)&&(cbYTargetPos<5)) return false;
			if ((pSourceChessItem->cbColor==WHITE_CHESS)&&(cbYTargetPos>4)) return false;
			if (m_ChessBorad[(cbXSourcePos+cbXTargetPos)/2][(cbYSourcePos+cbYTargetPos)/2]!=NULL) return false;

			return true;
		}
	case CHESS_HORSE:	//马
		{
			//偏移变量
			BYTE cbXExcursion=GetExcursion(cbXSourcePos,cbXTargetPos);
			BYTE cbYExcursion=GetExcursion(cbYSourcePos,cbYTargetPos);

			//步法判断
			if (((cbXExcursion!=1)||(cbYExcursion!=2))&&((cbXExcursion!=2)||(cbYExcursion!=1))) return false;

			//撇脚判断
			BYTE cbXVerdict=(cbXExcursion==1)?cbXSourcePos:(cbXSourcePos+cbXTargetPos)/2;
			BYTE cbYVerdict=(cbYExcursion==1)?cbYSourcePos:(cbYSourcePos+cbYTargetPos)/2;
			if (m_ChessBorad[cbXVerdict][cbYVerdict]!=NULL) return false;

			return true;
		}
	case CHESS_ROOK:	//车
		{
			//步法判断
			if ((cbXSourcePos!=cbXTargetPos)&&(cbYSourcePos!=cbYTargetPos)) return false;

			//阻塞分析
			if (ObstructAnalyse(cbXSourcePos,cbYSourcePos,cbXTargetPos,cbYTargetPos)==true) return false;

			return true;
		}
	case CHESS_CANNON:	//炮
		{
			//步法判断
			if ((cbXSourcePos!=cbXTargetPos)&&(cbYSourcePos!=cbYTargetPos)) return false;

			//吃子判断
			if (pTargetChessItem!=NULL)
			{
				//变量定义
				int nXStep=0,nYStep=0;
				if (cbXTargetPos!=cbXSourcePos) nXStep=(cbXTargetPos>cbXSourcePos)?1:-1;
				if (cbYTargetPos!=cbYSourcePos) nYStep=(cbYTargetPos>cbYSourcePos)?1:-1;

				//炮台分析
				bool bEmplacement=false;
				do
				{
					cbXSourcePos+=nXStep;
					cbYSourcePos+=nYStep;
					if ((cbXSourcePos==cbXTargetPos)&&(cbYSourcePos==cbYTargetPos))	return bEmplacement;
					if (m_ChessBorad[cbXSourcePos][cbYSourcePos]!=NULL)
					{
						if (bEmplacement==false) bEmplacement=true;
						else return false;
					}
				} while (true);

				return false;
			}

			//阻塞分析
			if (ObstructAnalyse(cbXSourcePos,cbYSourcePos,cbXTargetPos,cbYTargetPos)==true) return false;

			return true;
		}
	case CHESS_SOLDIER:	//兵
		{
			//偏移变量
			BYTE cbXExcursion=GetExcursion(cbXSourcePos,cbXTargetPos);
			BYTE cbYExcursion=GetExcursion(cbYSourcePos,cbYTargetPos);

			//步法判断
			if (pSourceChessItem->cbColor==BLACK_CHESS)
			{
				if (cbYSourcePos>=5)
				{
					if (cbXSourcePos!=cbXTargetPos) return false;
					if ((cbYSourcePos-1)!=cbYTargetPos) return false;
				}
				else
				{
					if (cbYTargetPos>cbYSourcePos) return false;
					if ((cbXExcursion>1)||(cbYExcursion>1)) return false;
					if ((cbXExcursion!=0)&&(cbYExcursion!=0)) return false;
				}
			}
			else
			{
				if (cbYSourcePos<=4)
				{
					if (cbXSourcePos!=cbXTargetPos) return false;
					if ((cbYSourcePos+1)!=cbYTargetPos) return false;
				}
				else
				{
					if (cbYTargetPos<cbYSourcePos) return false;
					if ((cbXExcursion>1)||(cbYExcursion>1)) return false;
					if ((cbXExcursion!=0)&&(cbYExcursion!=0)) return false;
				}
			}

			return true;
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
