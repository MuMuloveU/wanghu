#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameLogic::CGameLogic()
{
	//棋盘变量
	ZeroMemory(m_ChessItem,sizeof(m_ChessItem));
	ZeroMemory(m_ChessBorad,sizeof(m_ChessBorad));

	//状态变量
	m_wNullStep=0;
	ZeroMemory(m_CatchChessInfo,sizeof(m_CatchChessInfo));
	ZeroMemory(m_GeneralChessInfo,sizeof(m_GeneralChessInfo));

	return;
}

//析构函数
CGameLogic::~CGameLogic()
{
}

//获取步数
WORD CGameLogic::GetStepCount(BYTE cbColor)
{
	//效验参数
	ASSERT((cbColor==CHESS_BLACK)||(cbColor==CHESS_WHITE));
	if ((cbColor!=CHESS_BLACK)&&(cbColor!=CHESS_WHITE)) return 0;

	//获取步数
	WORD wStepCount=(WORD)m_ChessManualArray.GetCount();
	return (cbColor==CHESS_BLACK)?(wStepCount+1)/2:wStepCount/2;
}

//获取棋谱
tagChessManual * CGameLogic::GetLastChessManual()
{
	INT_PTR nManualCount=m_ChessManualArray.GetCount();
	return (nManualCount>0)?&m_ChessManualArray[nManualCount-1]:NULL;
}

//获取棋子
tagChessItem * CGameLogic::GetChessItem(BYTE cbXPos, BYTE cbYPos)
{
	//效验参数
	ASSERT((cbXPos<COUNT_X)&&(cbYPos<COUNT_Y));
	if ((cbXPos>=COUNT_X)||(cbYPos>=COUNT_Y)) return NULL;

	//获取棋子
	return m_ChessBorad[cbXPos][cbYPos];
}

//重置棋盘
VOID CGameLogic::ResetChessBorad()
{
	//清理棋谱
	m_ChessManualArray.RemoveAll();

	//棋盘变量
	ZeroMemory(m_ChessItem,sizeof(m_ChessItem));
	ZeroMemory(m_ChessBorad,sizeof(m_ChessBorad));

	//状态变量
	m_wNullStep=0;
	ZeroMemory(m_CatchChessInfo,sizeof(m_CatchChessInfo));
	ZeroMemory(m_GeneralChessInfo,sizeof(m_GeneralChessInfo));

	//设置棋盘
	InitChessBorad(m_ChessItem,m_ChessBorad);

	return;
}

//悔棋控制
bool CGameLogic::RegretChess(WORD wTargetStep)
{
	//效验参数
	ASSERT(wTargetStep<=(WORD)m_ChessManualArray.GetCount());
	if (wTargetStep>m_ChessManualArray.GetCount()) return false;

	//棋盘变量
	ZeroMemory(m_ChessItem,sizeof(m_ChessItem));
	ZeroMemory(m_ChessBorad,sizeof(m_ChessBorad));

	//状态变量
	m_wNullStep=0;
	ZeroMemory(m_CatchChessInfo,sizeof(m_CatchChessInfo));
	ZeroMemory(m_GeneralChessInfo,sizeof(m_GeneralChessInfo));

	//设置棋盘
	InitChessBorad(m_ChessItem,m_ChessBorad);

	//设置棋子
	for (INT_PTR i=0;i<wTargetStep;i++)
	{
		//变量定义
		tagChessManual * pChessManual=&m_ChessManualArray[i];
		BYTE cbXSourcePos=pChessManual->cbXSourceChessPos,cbYSourcePos=pChessManual->cbYSourceChessPos;
		BYTE cbXTargetPos=pChessManual->cbXTargetChessPos,cbYTargetPos=pChessManual->cbYTargetChessPos;

		//构造变量
		tagMoveChessResult MoveChessResult;
		ZeroMemory(&MoveChessResult,sizeof(MoveChessResult));

		//移动棋子
		if (PerformMoveChess(cbXSourcePos,cbYSourcePos,cbXTargetPos,cbYTargetPos,MoveChessResult)==false)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	//删除棋谱
	m_ChessManualArray.SetSize(wTargetStep);

	return true;
}

//移动棋子
bool CGameLogic::MoveChess(BYTE cbXSourcePos, BYTE cbYSourcePos, BYTE cbXTargetPos, BYTE cbYTargetPos, tagMoveChessResult & MoveChessResult, LPTSTR pszManualName)
{
	//效验坐标
	ASSERT((cbXSourcePos<COUNT_X)&&(cbYSourcePos<COUNT_Y));
	ASSERT((cbXTargetPos<COUNT_X)&&(cbYTargetPos<COUNT_Y));

	//效验坐标
	if ((cbXSourcePos>=COUNT_X)&&(cbYSourcePos>=COUNT_Y)) return false;//?
	if ((cbXTargetPos>=COUNT_X)&&(cbYTargetPos>=COUNT_Y)) return false;

	//获取棋谱
	if (pszManualName!=NULL)
	{
		GetManualItemName(cbXSourcePos,cbYSourcePos,cbXTargetPos,cbYTargetPos,pszManualName);
	}

	//移动棋子
	if (PerformMoveChess(cbXSourcePos,cbYSourcePos,cbXTargetPos,cbYTargetPos,MoveChessResult)==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//变量定义
	tagChessManual ChessManual;
	ZeroMemory(&ChessManual,sizeof(ChessManual));

	//构造变量
	ChessManual.cbXSourceChessPos=cbXSourcePos;
	ChessManual.cbYSourceChessPos=cbYSourcePos;
	ChessManual.cbXTargetChessPos=cbXTargetPos;
	ChessManual.cbYTargetChessPos=cbYTargetPos;

	//插入棋谱
	m_ChessManualArray.Add(ChessManual);

	return true;
}

//获取视图
VOID CGameLogic::GetHistoryStatus(tagChessItem ChessItem[2][COUNT_CHESS], tagChessItem * ChessBorad[COUNT_X][COUNT_Y], WORD wStepCount)
{
	//设置棋盘
	InitChessBorad(ChessItem,ChessBorad);

	//设置结果
	for (WORD i=0;i<wStepCount;i++)
	{
		//效验参数
		ASSERT(i<m_ChessManualArray.GetCount());
		if (i>=m_ChessManualArray.GetCount()) break;

		//获取棋谱
		tagChessManual * pChessManual=&m_ChessManualArray[i];

		//位置定义
		BYTE cbXSourcePos=pChessManual->cbXSourceChessPos;
		BYTE cbYSourcePos=pChessManual->cbYSourceChessPos;
		BYTE cbXTargetPos=pChessManual->cbXTargetChessPos;
		BYTE cbYTargetPos=pChessManual->cbYTargetChessPos;

		//获取棋子
		tagChessItem * pSourceChessItem=ChessBorad[cbXSourcePos][cbYSourcePos];
		tagChessItem * pTargetChessItem=ChessBorad[cbXTargetPos][cbYTargetPos];

		//吃子判断
		if (pTargetChessItem!=NULL)
		{
			pTargetChessItem->cbXPos=0xFF;
			pTargetChessItem->cbYPos=0xFF;
		}

		//移动棋子
		pSourceChessItem->cbXPos=cbXTargetPos;
		pSourceChessItem->cbYPos=cbYTargetPos;
		ChessBorad[cbXSourcePos][cbYSourcePos]=NULL;
		ChessBorad[cbXTargetPos][cbYTargetPos]=pSourceChessItem;
	}

	return;
}

//结束判断
bool CGameLogic::IsGameFinish(BYTE cbMoveColor, tagConcludeInfo & ConcludeInfo)
{
	//索引变量
	BYTE cbEnemyIndex=(cbMoveColor==CHESS_BLACK)?1:0;
	BYTE cbMySelfIndex=(cbMoveColor==CHESS_BLACK)?0:1;

	//捉子信息
	tagCatchChessInfo * pEnemyCatchInfo=&m_CatchChessInfo[cbEnemyIndex];
	tagCatchChessInfo * pMySelfCatchInfo=&m_CatchChessInfo[cbMySelfIndex];

	//将军信息
	tagGeneralChessInfo * pGeneralChessInfo=&m_GeneralChessInfo[cbMySelfIndex];

	////长将判断
	//if (pGeneralChessInfo->cbSourceCount>=6)
	//{
	//	//变量定义
	//	BYTE cbGeneralCount=0;
	//	tagChessItem * pSourceChessItem[3];

	//	//棋数计算
	//	for (BYTE i=0;i<pGeneralChessInfo->cbSourceCount;i++)
	//	{
	//		//棋子判断
	//		for (BYTE j=0;j<cbGeneralCount;j++)
	//		{
	//			if (pGeneralChessInfo->pSourceChessItem[i]==pSourceChessItem[j]) break;
	//		}

	//		//设置结果
	//		if (j==cbGeneralCount)
	//		{
	//			//设置结果
	//			pSourceChessItem[cbGeneralCount++]=pGeneralChessInfo->pSourceChessItem[i];

	//			//完成判断
	//			if (cbGeneralCount>=CountArray(pSourceChessItem)) break;
	//		}
	//	}

	//	//长将判断
	//	if (pGeneralChessInfo->cbSourceCount>=cbGeneralCount*6) 
	//	{
	//		//设置变量
	//		ConcludeInfo.cbReason = REASON_CATCH_GENERA;
	//		ConcludeInfo.bGameConclude=true;
	//		ConcludeInfo.cbWinnerColor=(cbMoveColor==CHESS_BLACK)?CHESS_WHITE:CHESS_BLACK;

	//		return true;
	//	}
	//}

	////长捉判断
	//if (pMySelfCatchInfo->cbSourceCount>=6)
	//{
	//	//变量定义
	//	BYTE cbCatchCount=0;
	//	tagChessItem * pSourceChessItem[3];

	//	//棋数计算
	//	for (BYTE i=0;i<pMySelfCatchInfo->cbSourceCount;i++)
	//	{
	//		//棋子判断
	//		for (BYTE j=0;j<cbCatchCount;j++)
	//		{
	//			if (pMySelfCatchInfo->pSourceChessItem[i]==pSourceChessItem[j]) break;
	//		}

	//		//设置结果
	//		if (j==cbCatchCount)
	//		{
	//			//设置结果
	//			pSourceChessItem[cbCatchCount++]=pMySelfCatchInfo->pSourceChessItem[i];

	//			//完成判断
	//			if (cbCatchCount>=CountArray(pSourceChessItem)) break;
	//		}
	//	}

	//	//长捉判断
	//	if (pMySelfCatchInfo->cbSourceCount>=cbCatchCount*6) 
	//	{
	//		//设置变量
	//		ConcludeInfo.cbReason = REASON_CATCH_CHESS;
	//		ConcludeInfo.bGameConclude=true;
	//		ConcludeInfo.cbWinnerColor=(cbMoveColor==CHESS_BLACK)?CHESS_WHITE:CHESS_BLACK;

	//		return true;
	//	}
	//}

	//获取将棋
	tagChessItem * pKindChessItem=&m_ChessItem[cbMySelfIndex][4];
	ASSERT((pKindChessItem->cbXPos!=0xFF)&&(pKindChessItem->cbYPos!=0xFF));

	//将死判断
	if (GetMinatoryChess(pKindChessItem->cbXPos,pKindChessItem->cbYPos)!=NULL)
	{
		//设置变量
		ConcludeInfo.bGameConclude=true;
		ConcludeInfo.cbWinnerColor=(cbMoveColor==CHESS_BLACK)?CHESS_WHITE:CHESS_BLACK;

		return true;
	}

	////空步判和
	//if ((m_wNullStep>=120)||(m_ChessManualArray.GetCount()>=400))
	//{
	//	//设置变量
	//	if(m_wNullStep>=120)ConcludeInfo.cbReason = REASON_LEISURE_STEP;
	//	else ConcludeInfo.cbReason = REASON_OVER_STEP;
	//	ConcludeInfo.bGameConclude = true;
	//	ConcludeInfo.cbWinnerColor = CHESS_NULL;

	//	return true;
	//}

	//困毙判断
	if(IsLoseChess((cbMoveColor==CHESS_BLACK)?CHESS_WHITE:CHESS_BLACK))
	{
		//获取将棋
		tagChessItem * pKindChessItem=&m_ChessItem[cbEnemyIndex][4];
		ASSERT((pKindChessItem->cbXPos!=0xFF)&&(pKindChessItem->cbYPos!=0xFF));

		//困毙判断
		if (GetMinatoryChess(pKindChessItem->cbXPos,pKindChessItem->cbYPos)==NULL)
		{
			ConcludeInfo.cbReason = REASON_CHESS_FEAZE;
		}
		else ConcludeInfo.cbReason = REASON_KILL_GENERA;

		//设置变量
		ConcludeInfo.bGameConclude=true;
		ConcludeInfo.cbWinnerColor=cbMoveColor;

		return true;
	}

	//设置结果
	ZeroMemory(&ConcludeInfo,sizeof(ConcludeInfo));

	return false;
}

//效验步法
bool CGameLogic::IsWalkLegality(const tagChessItem * pSourceChessItem, BYTE cbXTargetPos, BYTE cbYTargetPos)
{
	//效验坐标
	ASSERT(pSourceChessItem!=NULL);
	ASSERT((cbXTargetPos<COUNT_X)&&(cbYTargetPos<COUNT_Y));
	ASSERT((pSourceChessItem->cbXPos<COUNT_X)&&(pSourceChessItem->cbYPos<COUNT_Y));

	//获取棋子
	BYTE cbXSourcePos=pSourceChessItem->cbXPos;
	BYTE cbYSourcePos=pSourceChessItem->cbYPos;
	tagChessItem * pTargetChessItem=m_ChessBorad[cbXTargetPos][cbYTargetPos];

	//同子判断
	if ((pSourceChessItem->cbXPos==cbXTargetPos)&&(pSourceChessItem->cbYPos==cbYTargetPos)) return false;

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
			if ((pSourceChessItem->cbColor==CHESS_BLACK)&&(cbYTargetPos<7)) return false;
			if ((pSourceChessItem->cbColor==CHESS_WHITE)&&(cbYTargetPos>2)) return false;

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
			if ((pSourceChessItem->cbColor==CHESS_BLACK)&&(cbYTargetPos<7)) return false;
			if ((pSourceChessItem->cbColor==CHESS_WHITE)&&(cbYTargetPos>2)) return false;

			return true;
		}
	case CHESS_ELEPHANT://象
		{
			//偏移变量
			BYTE cbXExcursion=GetExcursion(cbXSourcePos,cbXTargetPos);
			BYTE cbYExcursion=GetExcursion(cbYSourcePos,cbYTargetPos);

			//步法判断
			if ((cbXExcursion!=2)||(cbYExcursion!=2)) return false;
			if ((pSourceChessItem->cbColor==CHESS_BLACK)&&(cbYTargetPos<5)) return false;
			if ((pSourceChessItem->cbColor==CHESS_WHITE)&&(cbYTargetPos>4)) return false;
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
				INT nXStep=0,nYStep=0;
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
			if (pSourceChessItem->cbColor==CHESS_BLACK)
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
	return (cbChessColor==CHESS_BLACK)?0:1;
}

//获取偏移
BYTE CGameLogic::GetExcursion(BYTE cbSourcePos, BYTE cbTargetPos)
{
	return (cbSourcePos>cbTargetPos)?(cbSourcePos-cbTargetPos):(cbTargetPos-cbSourcePos);
}

//执行走棋
bool CGameLogic::PerformMoveChess(BYTE cbXSourcePos, BYTE cbYSourcePos, BYTE cbXTargetPos, BYTE cbYTargetPos, tagMoveChessResult & MoveChessResult)
{
	//效验坐标
	ASSERT((cbXSourcePos<COUNT_X)&&(cbYSourcePos<COUNT_Y));
	ASSERT((cbXTargetPos<COUNT_X)&&(cbYTargetPos<COUNT_Y));

	//获取棋子
	tagChessItem * pSourceChessItem=m_ChessBorad[cbXSourcePos][cbYSourcePos];
	tagChessItem * pTargetChessItem=m_ChessBorad[cbXTargetPos][cbYTargetPos];

	//效验棋子
	ASSERT(pSourceChessItem!=NULL);
	if (pSourceChessItem==NULL) return false;

	//变量定义
	bool bRescueChess=false;
	tagChessItem * pCatchTargetChess=NULL;
	tagChessItem * pCatchSourceChess=NULL;

	//索引变量
	BYTE cbEnemyIndex=(pSourceChessItem->cbColor==CHESS_BLACK)?1:0;
	BYTE cbMySelfIndex=(pSourceChessItem->cbColor==CHESS_BLACK)?0:1;

	//捉子信息
	tagCatchChessInfo * pEnemyCatchInfo=&m_CatchChessInfo[cbEnemyIndex];
	tagCatchChessInfo * pMySelfCatchInfo=&m_CatchChessInfo[cbMySelfIndex];

	//被捉判断
	if ((pTargetChessItem==NULL)&&(pEnemyCatchInfo->pTargetChessItem!=pSourceChessItem))
	{
		//威胁判断
		tagChessItem * pMinatoryChess=GetMinatoryChess(pSourceChessItem->cbXPos,pSourceChessItem->cbYPos);

		//被捉判断
		if (pMinatoryChess!=NULL)
		{
			//有根判断
			tagChessItem * pProtectedChess=GetProtectedChess(pSourceChessItem->cbXPos,pSourceChessItem->cbYPos);

			//解救判断
			if (pProtectedChess!=NULL)
			{
				BYTE cbSourceChess=pMinatoryChess->cbChess;
				BYTE cbTargetChess=pSourceChessItem->cbChess;
				if ((cbTargetChess==CHESS_ROOK)&&((cbSourceChess==CHESS_HORSE)||(cbSourceChess==CHESS_CANNON))) bRescueChess=true;
			}
			else bRescueChess=true;
		}
	}

	//吃子处理
	if (pTargetChessItem!=NULL)
	{
		//设置状态
		m_wNullStep=0;

		//设置棋子
		pTargetChessItem->cbXPos=0xFF;
		pTargetChessItem->cbYPos=0xFF;
	}
	else
	{
		//设置状态
		m_wNullStep++;
	}

	//设置棋子
	pSourceChessItem->cbXPos=cbXTargetPos;
	pSourceChessItem->cbYPos=cbYTargetPos;
	m_ChessBorad[cbXSourcePos][cbYSourcePos]=NULL;
	m_ChessBorad[cbXTargetPos][cbYTargetPos]=pSourceChessItem;

	//设置结果
	ZeroMemory(&MoveChessResult,sizeof(MoveChessResult));

	//对方王棋
	BYTE cbChessColor=pSourceChessItem->cbColor;
	tagChessItem * pKindChessItem=&m_ChessItem[(cbChessColor==CHESS_BLACK)?1:0][4];

	//将军判断
	if ((pKindChessItem->cbXPos!=0xFF)&&(pKindChessItem->cbYPos!=0xFF))
	{
		//将军判断
		if (GetMinatoryChess(pKindChessItem->cbXPos,pKindChessItem->cbYPos)!=NULL)
		{
			//设置标志
			MoveChessResult.bMinatoryKing=true;

			//将军记录
			BYTE cbIndex=m_GeneralChessInfo[cbMySelfIndex].cbSourceCount++;
			m_GeneralChessInfo[cbMySelfIndex].pSourceChessItem[cbIndex]=pSourceChessItem;

			return true;
		}
		else
		{
			//设置记录
			m_GeneralChessInfo[cbMySelfIndex].cbSourceCount=0;
		}
	}

	//捉子判断
	if ((pTargetChessItem==NULL)&&(pSourceChessItem->cbChess!=CHESS_GENERAL)&&(pSourceChessItem->cbChess!=CHESS_SOLDIER))
	{
		//解救判断
		if ((bRescueChess==false)&&(pEnemyCatchInfo->cbSourceCount>0)&&(pEnemyCatchInfo->pTargetChessItem!=NULL))
		{
			//获取棋子
			tagChessItem * pMySelfChessItem=pEnemyCatchInfo->pTargetChessItem;
			tagChessItem * pEnemyChessItem=pEnemyCatchInfo->pSourceChessItem[pEnemyCatchInfo->cbSourceCount-1];

			//解救判断
			if (IsWalkLegality(pEnemyChessItem,pMySelfChessItem->cbXPos,pMySelfChessItem->cbYPos)==false) bRescueChess=true;
		}

		//还原状态
		if (bRescueChess==false)
		{
			pEnemyCatchInfo->cbSourceCount=0;
			pEnemyCatchInfo->pTargetChessItem=NULL;
		}

		//捉子判断
		if (bRescueChess==false)
		{
			//捉子搜索
			for (BYTE i=0;i<CountArray(m_ChessItem[cbMySelfIndex]);i++)
			{
				//获取棋子
				tagChessItem * pMySelfChessItem=&m_ChessItem[cbMySelfIndex][i];
				if ((pMySelfChessItem->cbXPos==0xFF)||(pMySelfChessItem->cbYPos==0xFF)) continue;

				//捉子判断
				for (BYTE j=0;j<CountArray(m_ChessItem[cbEnemyIndex]);j++)
				{
					//获取棋子
					tagChessItem * pEnemyChessItem=&m_ChessItem[cbEnemyIndex][j];
					if ((pEnemyChessItem->cbXPos==0xFF)||(pEnemyChessItem->cbYPos==0xFF)) continue;

					//威胁判断
					if (IsWalkLegality(pMySelfChessItem,pEnemyChessItem->cbXPos,pEnemyChessItem->cbYPos)==false) continue;

					//有根判断
					tagChessItem * pProtectedChess=GetProtectedChess(pEnemyChessItem->cbXPos,pEnemyChessItem->cbYPos);

					//解救判断
					bool bCatchChess=true;
					if (pProtectedChess!=NULL)
					{
						BYTE cbTargetChess=pEnemyChessItem->cbChess;
						BYTE cbSourceChess=pMySelfChessItem->cbChess;
						bCatchChess=((cbTargetChess==CHESS_ROOK)&&((cbSourceChess==CHESS_HORSE)||(cbSourceChess==CHESS_CANNON)));
					}

					//设置结果
					if (bCatchChess==true)
					{
						MoveChessResult.bCatchChess=true;
						pCatchTargetChess=pEnemyChessItem;
						pCatchSourceChess=pMySelfChessItem;

						break;
					}
				}

				//完成判断
				if ((pCatchSourceChess!=NULL)&&(pCatchTargetChess!=NULL)) break;
			}
		}

		//还原状态
		if ((pCatchTargetChess==NULL)||(pMySelfCatchInfo->pTargetChessItem!=pCatchTargetChess))
		{
			pMySelfCatchInfo->cbSourceCount=0;
			pMySelfCatchInfo->pTargetChessItem=pCatchTargetChess;
		}

		//设置列表
		if (pCatchTargetChess!=NULL)
		{
			ASSERT(pMySelfCatchInfo->cbSourceCount<CountArray(pMySelfCatchInfo->pSourceChessItem));
			pMySelfCatchInfo->pSourceChessItem[pMySelfCatchInfo->cbSourceCount++]=pCatchSourceChess;
		}
	}
	else
	{
		//变量定义
		BYTE cbMySelfIndex=(pSourceChessItem->cbColor==CHESS_BLACK)?0:1;
		tagCatchChessInfo * pMySelfCatchInfo=&m_CatchChessInfo[cbMySelfIndex];

		//还原状态
		pMySelfCatchInfo->cbSourceCount=0;
		pMySelfCatchInfo->pTargetChessItem=NULL;
	}

	return true;
}

//判断将军
bool CGameLogic::IsKillGeneral(BYTE cbXSourcePos, BYTE cbYSourcePos, BYTE cbXTargetPos, BYTE cbYTargetPos)
{
	//效验坐标
	ASSERT((cbXSourcePos<COUNT_X)&&(cbYSourcePos<COUNT_Y));
	ASSERT((cbXTargetPos<COUNT_X)&&(cbYTargetPos<COUNT_Y));

	//获取棋子
	tagChessItem * pSourceChessItem = m_ChessBorad[cbXSourcePos][cbYSourcePos];
	tagChessItem * pTargetChessItem = m_ChessBorad[cbXTargetPos][cbYTargetPos];

	//索引变量
	BYTE cbColor = pSourceChessItem->cbColor;
	BYTE cbMySelfIndex=(cbColor==CHESS_BLACK)?0:1;
	//BYTE cbEnemyIndex=(cbColor==CHESS_BLACK)?1:0;

	//吃子处理
	if (pTargetChessItem!=NULL)
	{
		//设置棋子
		pTargetChessItem->cbXPos=0xFF;
		pTargetChessItem->cbYPos=0xFF;
	}

	//设置棋子
	//tagChessItem * pSouTempChessItem = m_ChessBorad[cbXSourcePos][cbYSourcePos];
	pSourceChessItem->cbXPos=cbXTargetPos;
	pSourceChessItem->cbYPos=cbYTargetPos;
	m_ChessBorad[cbXSourcePos][cbYSourcePos]=NULL;
	m_ChessBorad[cbXTargetPos][cbYTargetPos]=pSourceChessItem;

	//我方王棋
	tagChessItem * pKindChessItem=&m_ChessItem[cbMySelfIndex][4];
	ASSERT((pKindChessItem->cbXPos!=0xFF)&&(pKindChessItem->cbYPos!=0xFF));
	ASSERT(pKindChessItem->cbChess==CHESS_GENERAL);

	//判断将军
	bool bKillGeneral = false;
	if (GetMinatoryChess(pKindChessItem->cbXPos,pKindChessItem->cbYPos)!=NULL)
	{
		bKillGeneral =  true;
	}

	//还原数据
	if (pTargetChessItem!=NULL)
	{
		pTargetChessItem->cbXPos = cbXTargetPos;
		pTargetChessItem->cbYPos = cbYTargetPos;
		m_ChessBorad[cbXTargetPos][cbYTargetPos] = pTargetChessItem;
	}
	else m_ChessBorad[cbXTargetPos][cbYTargetPos] = NULL;
	pSourceChessItem->cbXPos=cbXSourcePos;
	pSourceChessItem->cbYPos=cbYSourcePos;
	m_ChessBorad[cbXSourcePos][cbYSourcePos]=pSourceChessItem;

	return bKillGeneral;
}

//判断死棋
bool CGameLogic::IsLoseChess(BYTE cbColor)
{
	//索引变量
	BYTE cbEnemyIndex=(cbColor==CHESS_BLACK)?1:0;
	BYTE cbMySelfIndex=(cbColor==CHESS_BLACK)?0:1;

	//我方王棋
	tagChessItem * pKindChessItem=&m_ChessItem[cbMySelfIndex][4];
	ASSERT((pKindChessItem->cbXPos!=0xFF)&&(pKindChessItem->cbYPos!=0xFF));
	ASSERT(pKindChessItem->cbChess==CHESS_GENERAL);

	//解救搜索
	for (BYTE i=0;i<CountArray(m_ChessItem[cbMySelfIndex]);i++)
	{
		//获取棋子
		tagChessItem * pMySelfChessItem=&m_ChessItem[cbMySelfIndex][i];
		if ((pMySelfChessItem->cbXPos==0xFF)||(pMySelfChessItem->cbYPos==0xFF)) continue;

		//搜索走棋
		for(BYTE x=0;x<COUNT_X;x++)
		{
			for(BYTE y=0;y<COUNT_Y;y++)
			{
				//过滤同棋
				tagChessItem * pChessItem=m_ChessBorad[x][y];
				if ((pChessItem!=NULL)&&(pChessItem->cbColor==cbColor))continue;

				//判断走棋
				if (IsWalkLegality(pMySelfChessItem,x,y))
				{
					//获取棋子
					tagChessItem * pSourceChessItem=pMySelfChessItem;
					tagChessItem * pTargetChessItem=m_ChessBorad[x][y];

					//吃子处理
					if (pTargetChessItem!=NULL)
					{
						//设置棋子
						pTargetChessItem->cbXPos=0xFF;
						pTargetChessItem->cbYPos=0xFF;
					}

					//设置棋子
					BYTE cbXSourcePos = pSourceChessItem->cbXPos;
					BYTE cbYSourcePos = pSourceChessItem->cbYPos;
					//tagChessItem * pTempChessItem = m_ChessBorad[cbXSourcePos][cbYSourcePos];
					pSourceChessItem->cbXPos=x;
					pSourceChessItem->cbYPos=y;
					m_ChessBorad[cbXSourcePos][cbYSourcePos]=NULL;
					m_ChessBorad[x][y]=pSourceChessItem;

					//解救判断
					ASSERT(pKindChessItem->cbChess==CHESS_GENERAL);
					if (GetMinatoryChess(pKindChessItem->cbXPos,pKindChessItem->cbYPos)==NULL)
					{
						//还原数据
						if (pTargetChessItem!=NULL)
						{
							pTargetChessItem->cbXPos = x;
							pTargetChessItem->cbYPos = y;
							m_ChessBorad[x][y] = pTargetChessItem;
						}
						else m_ChessBorad[x][y] = NULL;
						pSourceChessItem->cbXPos=cbXSourcePos;
						pSourceChessItem->cbYPos=cbYSourcePos;
						m_ChessBorad[cbXSourcePos][cbYSourcePos]=pSourceChessItem;
						
						return false;
					}

					//还原数据
					if (pTargetChessItem!=NULL)
					{
						pTargetChessItem->cbXPos = x;
						pTargetChessItem->cbYPos = y;
						m_ChessBorad[x][y] = pTargetChessItem;
					}
					else m_ChessBorad[x][y] = NULL;
					pSourceChessItem->cbXPos=cbXSourcePos;
					pSourceChessItem->cbYPos=cbYSourcePos;
					m_ChessBorad[cbXSourcePos][cbYSourcePos]=pSourceChessItem;
				}
			}
		}
	}

	return true;
}

//复位棋盘
bool CGameLogic::InitChessBorad(tagChessItem ChessItem[2][COUNT_CHESS], tagChessItem * ChessBorad[COUNT_X][COUNT_Y])
{
	//变量定义
	BYTE cbChessInfo[9]={CHESS_ROOK,CHESS_HORSE,CHESS_ELEPHANT,CHESS_KAVASS,CHESS_GENERAL,
		CHESS_KAVASS,CHESS_ELEPHANT,CHESS_HORSE,CHESS_ROOK};

	//设置数据
	for (BYTE cbUserIndex=0;cbUserIndex<2;cbUserIndex++)
	{
		//主线棋子
		BYTE cbExcursion=0;
		BYTE cbYPos=(cbUserIndex==0)?9:0;
		for (BYTE nIndex=0;nIndex<9;nIndex++,cbExcursion++)
		{
			ChessItem[cbUserIndex][cbExcursion].cbYPos=cbYPos;
			ChessItem[cbUserIndex][cbExcursion].cbXPos=nIndex;
			ChessItem[cbUserIndex][cbExcursion].cbChessID=nIndex;
			ChessItem[cbUserIndex][cbExcursion].cbChess=cbChessInfo[nIndex];
			ChessItem[cbUserIndex][cbExcursion].cbColor=CHESS_BLACK+cbUserIndex;
			ChessBorad[nIndex][cbYPos]=&ChessItem[cbUserIndex][cbExcursion];
		}

		//兵的棋子
		cbYPos=(cbUserIndex==0)?6:3;
		for (BYTE nIndex=0;nIndex<5;nIndex++,cbExcursion++)
		{
			ChessItem[cbUserIndex][cbExcursion].cbYPos=cbYPos;
			ChessItem[cbUserIndex][cbExcursion].cbXPos=nIndex*2;
			ChessItem[cbUserIndex][cbExcursion].cbChess=CHESS_SOLDIER;
			ChessItem[cbUserIndex][cbExcursion].cbChessID=cbExcursion;
			ChessItem[cbUserIndex][cbExcursion].cbColor=CHESS_BLACK+cbUserIndex;
			ChessBorad[nIndex*2][cbYPos]=&ChessItem[cbUserIndex][cbExcursion];
		}

		//炮的棋子
		cbYPos=(cbUserIndex==0)?7:2;
		for (BYTE nIndex=0;nIndex<2;nIndex++,cbExcursion++)
		{
			ChessItem[cbUserIndex][cbExcursion].cbYPos=cbYPos;
			ChessItem[cbUserIndex][cbExcursion].cbXPos=1+nIndex*6;
			ChessItem[cbUserIndex][cbExcursion].cbChess=CHESS_CANNON;
			ChessItem[cbUserIndex][cbExcursion].cbChessID=cbExcursion;
			ChessItem[cbUserIndex][cbExcursion].cbColor=CHESS_BLACK+cbUserIndex;
			ChessBorad[1+nIndex*6][cbYPos]=&ChessItem[cbUserIndex][cbExcursion];
		}
	}

	return true;
}

//棋谱名字
bool CGameLogic::GetManualItemName(BYTE cbXSourcePos, BYTE cbYSourcePos, BYTE cbXTargetPos, BYTE cbYTargetPos, LPTSTR pszManualName)
{
	//棋子命名
	LPCTSTR pszChessName[2][7]=
	{
		TEXT("帅"),TEXT("仕"),TEXT("相"),TEXT("马"),TEXT("車"),TEXT("炮"),TEXT("兵"),
		TEXT("将"),TEXT("士"),TEXT("象"),TEXT("马"),TEXT("車"),TEXT("炮"),TEXT("卒"),
	};

	//线条命名
	LPCTSTR pszXLineName[2][COUNT_X]=
	{
		TEXT("九"),TEXT("八"),TEXT("七"),TEXT("六"),TEXT("五"),TEXT("四"),TEXT("三"),TEXT("二"),TEXT("一"),
		TEXT("１"),TEXT("２"),TEXT("３"),TEXT("４"),TEXT("５"),TEXT("６"),TEXT("７"),TEXT("８"),TEXT("９"),
	};

	//线条命名
	LPCTSTR pszYLineName[2][COUNT_Y]=
	{
		TEXT("一"),TEXT("二"),TEXT("三"),TEXT("四"),TEXT("五"),TEXT("六"),TEXT("七"),TEXT("八"),TEXT("九"),TEXT("十"),
		TEXT("１"),TEXT("２"),TEXT("３"),TEXT("４"),TEXT("５"),TEXT("６"),TEXT("７"),TEXT("８"),TEXT("９"),TEXT("１０"),
	};

	//变量定义
	bool bLocateDown=true;
	bool bExistSameChess=false;

	//棋子属性
	ASSERT(m_ChessBorad[cbXSourcePos][cbYSourcePos]!=NULL);
	BYTE cbSourceColor=m_ChessBorad[cbXSourcePos][cbYSourcePos]->cbColor;
	BYTE cbSourceChess=m_ChessBorad[cbXSourcePos][cbYSourcePos]->cbChess;

	//存在判断
	for (BYTE nYPos=0;nYPos<COUNT_Y;nYPos++)
	{
		if ((cbYSourcePos!=nYPos)&&(m_ChessBorad[cbXSourcePos][nYPos]!=NULL))
		{
			//获取属性
			BYTE cbTargetColor=m_ChessBorad[cbXSourcePos][nYPos]->cbColor;
			BYTE cbTargetChess=m_ChessBorad[cbXSourcePos][nYPos]->cbChess;
			
			//相同判断
			if ((cbTargetColor==cbSourceColor)&&(cbTargetChess==cbSourceChess))
			{
				bExistSameChess=true;
				bLocateDown=(nYPos<cbYSourcePos)?true:false;
				break;
			}
		}
	}

	//走棋用户
	lstrcpy(pszManualName,(cbSourceColor==CHESS_BLACK)?TEXT("红方："):TEXT("黑方："));

	//构造前叠
	if (bExistSameChess==true)
	{
		//前后字符
		if (cbSourceColor==CHESS_BLACK)
		{
			lstrcat(pszManualName,(bLocateDown==true)?TEXT("后"):TEXT("前"));
		}
		else
		{
			lstrcat(pszManualName,(bLocateDown==true)?TEXT("前"):TEXT("后"));
		}

		//棋子名字
		lstrcat(pszManualName,pszChessName[cbSourceColor-1][cbSourceChess-1]);
	}
	else
	{
		//前叠字符
		lstrcat(pszManualName,pszChessName[cbSourceColor-1][cbSourceChess-1]);
		lstrcat(pszManualName,pszXLineName[cbSourceColor-1][cbXSourcePos]);
	}

	//构造后叠
	if (cbYSourcePos==cbYTargetPos)
	{
		//平移字符
		lstrcat(pszManualName,TEXT("平"));
		lstrcat(pszManualName,pszXLineName[cbSourceColor-1][cbXTargetPos]);
	}
	else if (cbYSourcePos<cbYTargetPos)
	{
		//前进字符
		lstrcat(pszManualName,(cbSourceColor==CHESS_BLACK)?TEXT("退"):TEXT("进"));

		//移动数目
		if (cbXSourcePos==cbXTargetPos)
		{
			lstrcat(pszManualName,pszYLineName[cbSourceColor-1][cbYTargetPos-cbYSourcePos-1]);
		}
		else
		{
			lstrcat(pszManualName,pszXLineName[cbSourceColor-1][cbXTargetPos]);
		}
	}
	else
	{
		//后退字符
		lstrcat(pszManualName,(cbSourceColor==CHESS_BLACK)?TEXT("进"):TEXT("退"));

		//移动数目
		if (cbXSourcePos==cbXTargetPos)
		{
			lstrcat(pszManualName,pszYLineName[cbSourceColor-1][cbYSourcePos-cbYTargetPos-1]);
		}
		else
		{
			lstrcat(pszManualName,pszXLineName[cbSourceColor-1][cbXTargetPos]);
		}
	}

	return true;
}

//危险判断
tagChessItem * CGameLogic::GetMinatoryChess(BYTE cbXTargetPos, BYTE cbYTargetPos)
{
	//效验变量
	ASSERT((cbXTargetPos<COUNT_X)&&(cbYTargetPos<COUNT_Y));

	//获取棋子
	ASSERT(m_ChessBorad[cbXTargetPos][cbYTargetPos]!=NULL);
	tagChessItem * pTargetChessItem=m_ChessBorad[cbXTargetPos][cbYTargetPos];

	//变量定义
	BYTE cbEnemyIndex=(pTargetChessItem->cbColor==CHESS_BLACK)?1:0;

	//棋子搜索
	for (BYTE i=0;i<CountArray(m_ChessItem[cbEnemyIndex]);i++)
	{
		//获取棋子
		tagChessItem * pSourceChessItem=&m_ChessItem[cbEnemyIndex][i];
		if ((pSourceChessItem->cbXPos==0xFF)||(pSourceChessItem->cbYPos==0xFF)) continue;

		//威胁判断
		if (IsWalkLegality(pSourceChessItem,cbXTargetPos,cbYTargetPos)==true) return pSourceChessItem;
	}

	return NULL;
}

//有根判断
tagChessItem * CGameLogic::GetProtectedChess(BYTE cbXTargetPos, BYTE cbYTargetPos)
{
	//效验变量
	ASSERT((cbXTargetPos<COUNT_X)&&(cbYTargetPos<COUNT_Y));

	//获取棋子
	ASSERT(m_ChessBorad[cbXTargetPos][cbYTargetPos]!=NULL);
	tagChessItem * pTargetChessItem=m_ChessBorad[cbXTargetPos][cbYTargetPos];

	//变量定义
	BYTE cbMySelfIndex=(pTargetChessItem->cbColor==CHESS_BLACK)?0:1;

	//棋子搜索
	for (BYTE i=0;i<CountArray(m_ChessItem[cbMySelfIndex]);i++)
	{
		//获取棋子
		tagChessItem * pSourceChessItem=&m_ChessItem[cbMySelfIndex][i];
		if ((pSourceChessItem->cbXPos==0xFF)||(pSourceChessItem->cbYPos==0xFF)) continue;
		if ((pSourceChessItem->cbXPos==cbXTargetPos)&&(pSourceChessItem->cbYPos==cbYTargetPos)) continue;

		//威胁判断
		if (IsWalkLegality(pSourceChessItem,cbXTargetPos,cbYTargetPos)==true) return pSourceChessItem;
	}

	return NULL;
}

//阻塞分析
bool CGameLogic::ObstructAnalyse(BYTE cbXSourcePos, BYTE cbYSourcePos, BYTE cbXTargetPos, BYTE cbYTargetPos)
{
	//效验坐标
	ASSERT((cbXSourcePos!=cbXTargetPos)||(cbYSourcePos!=cbYTargetPos));

	//变量定义
	INT nXStep=0,nYStep=0;
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
