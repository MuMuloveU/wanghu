#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////

//结点信息
CNode::CNode()
{
	bXPos		 = 0 ;
	bYPos		 = 0 ;
	bToDesDis    = 0 ;	
	bToStartDis  = 0 ;
	bTotalDisDis = 0 ;
	pParent		 = NULL ;
}

CNode::CNode(int x,int y)
{
	bXPos	= x ;
	bYPos	= y ;	
	pParent = NULL;
}

CNode::~CNode()
{

}

//计算距离
void CNode::ComputeDistance()
{
	BYTE bXDis = 0 ,
		 bYDis = 0 ;

	bXDis = abs(bXPos - pParent->bXPos);
	bYDis = abs(bYPos - pParent->bYPos);	

	//对角距离
	if (1==bXDis && 1==bYDis)
	{
		bToStartDis = 2 + pParent->bToStartDis ;
	}
	//一步距离
	else
		bToStartDis = 1 + pParent->bToStartDis;

	//总共距离
	bTotalDisDis=bToStartDis+bToDesDis;

	return;
}

///////////////////////////////////////////////////////////////////////

//构造函数
CGameLogic::CGameLogic()
{
	//辅助变量

	//棋盘变量										
	memset(m_ChessItem,0,sizeof(m_ChessItem));
	memset(m_ChessBorad,0,sizeof(m_ChessBorad));

	return;
}

//析构函数
CGameLogic::~CGameLogic()
{
}

//能否移动
bool CGameLogic::AbleMove(const BYTE bChessColor , const tagChessItem ChessItem[17][17] , const BYTE bXPos , const BYTE bYPos)
{
	//非自己棋
	if(bChessColor!=ChessItem[bXPos][bYPos].bColor)		return false ;

	//错误位置
	if(CHESS_NULL==ChessItem[bXPos][bYPos].bChessID)	return false ;

	//在大本营
	if(true==IsHeadquarters(bXPos , bYPos))				return false ;

	//地雷棋子
	if(CHESS_DI_LEI==ChessItem[bXPos][bYPos].bChessID)	return false ;

	return true ;
}

//移动棋子
bool CGameLogic::MoveChess(BYTE  const bPlayerCount , tagChessItem ChessItem[17][17] , const BYTE bSourceXPos , const BYTE bSourceYPos , const BYTE bDesXPos , const BYTE bDesYPos , bool bMove)
{
	//自己棋子
	if(ChessItem[bSourceXPos][bSourceYPos].bColor==ChessItem[bDesXPos][bDesYPos].bColor) return false ;

	//错误棋子
	if(CHESS_NULL==ChessItem[bSourceXPos][bSourceYPos].bChessID) return false ;

	//不是敌方
	if(CHESS_NULL!=ChessItem[bDesXPos][bDesYPos].bChessID)
        if(false==IsEnemy(bPlayerCount , ChessItem , bSourceXPos , bSourceYPos , bDesXPos , bDesYPos)) return false ;

	//非结点处
	if(true==NotOnNode(bDesXPos , bDesYPos)) return false ;

	//不能入营
	if(true==IsBarback(bDesXPos , bDesYPos) && CHESS_NULL!=ChessItem[bDesXPos][bDesYPos].bChessID) return false ;

	//不在棋盘
	if(false==IsOnBoard(bSourceXPos,bSourceYPos) || false==IsOnBoard(bDesXPos , bDesYPos)) return false ;

	if(true==bMove) m_ptPathArray.RemoveAll() ;

	int	  sign;
	POINT point ;

	//不是工兵
	if (CHESS_GONG_BING!=ChessItem[bSourceXPos][bSourceYPos].bChessID)
	{
		//只走一步
		if ((abs(bSourceYPos-bDesYPos)==1 && abs(bSourceXPos-bDesXPos)==0) || (abs(bSourceXPos-bDesXPos)==1 && abs(bSourceYPos-bDesYPos)==0))
		{
			//保存位置
			if (true==bMove)
			{	
				point.x = bDesXPos ;
				point.y = bDesYPos ;
				m_ptPathArray.Add(point);

				point.x = bSourceXPos ;
				point.y = bSourceYPos ;
				m_ptPathArray.Add(point);

			}
			return true ;
		}
		//是否拐弯
		else if ( AbleTurn(bSourceXPos , bSourceYPos) && AbleTurn(bDesXPos , bDesYPos) && abs(bSourceYPos-bDesYPos)==1 && abs(bSourceXPos-bDesXPos)==1 )
		{
			//保存位置
			if (true==bMove)
			{
				point.x = bDesXPos ;
				point.y = bDesYPos ;
				m_ptPathArray.Add(point);

				point.x = bSourceXPos ;
				point.y = bSourceYPos ;
				m_ptPathArray.Add(point);
			}
			return true ;
		}
		//最后一行
		else if (0==bSourceXPos || 16==bSourceXPos || 0==bSourceYPos || 16 ==bSourceYPos)
		{
			if ( (bSourceXPos == bDesXPos && abs(bSourceYPos-bDesYPos) == 1 ) || (bSourceYPos == bDesYPos && abs(bSourceXPos-bDesXPos) == 1) )			 
			{
				//保存位置
				if (true==bMove)
				{
					point.x = bDesXPos ;
					point.y = bDesYPos ;
					m_ptPathArray.Add(point);

					point.x = bSourceXPos ;
					point.y = bSourceYPos ;
					m_ptPathArray.Add(point);
				}
				return true ;
			}
			return false ;
		}
		//最后一行
		else if (0 == bDesXPos || 16 == bDesXPos)
		{
			if (abs(bSourceXPos-bDesXPos)==1 && bSourceYPos == bDesYPos)
			{
				//保存位置
				if (true==bMove)
				{
					point.x = bDesXPos ;
					point.y = bDesYPos ;
					m_ptPathArray.Add(point);

					point.x = bSourceXPos ;
					point.y = bSourceYPos ;
					m_ptPathArray.Add(point);
				}
				return true;
			}
			
			return false;
		}
		else if (0==bDesYPos || 16==bDesYPos)
		{
			if (abs(bSourceYPos - bDesYPos)==1 && bSourceXPos==bDesXPos)
			{
				//保存位置
				if (true==bMove)
				{
					point.x = bDesXPos ;
					point.y = bDesYPos ;
					m_ptPathArray.Add(point);

					point.x = bSourceXPos ;
					point.y = bSourceYPos ;
					m_ptPathArray.Add(point);
				}
				return true ;
			}
			return false ;
		}
		//不在铁路
		else if ( NotOnRail(bSourceXPos , bSourceYPos) )
		{
			if ( abs(bSourceYPos - bDesYPos) > 1 || abs(bSourceXPos-bDesXPos) > 1 )
				return false;
			else if (IsBarback(bSourceXPos , bSourceYPos))
			{
				//保存位置
				if (true==bMove)
				{
					point.x = bDesXPos ;
					point.y = bDesYPos ;
					m_ptPathArray.Add(point);

					point.x = bSourceXPos ;
					point.y = bSourceYPos ;
					m_ptPathArray.Add(point);
				}
				return true ;
			}
			//只走一步
			else if (((bSourceYPos == bDesYPos) && abs(bSourceXPos-bDesXPos)==1) || ((bSourceXPos==bDesXPos) && abs(bSourceYPos - bDesYPos)==1) )
			{
				//保存位置
				if (true==bMove)
				{
					point.x = bDesXPos ;
					point.y = bDesYPos ;
					m_ptPathArray.Add(point);

					point.x = bSourceXPos ;
					point.y = bSourceYPos ;
					m_ptPathArray.Add(point);
				}
				return true ;
			}
			return false ;
		} 
		//不在铁路
		else if ( NotOnRail(bDesXPos , bDesYPos) )
		{
			if ( abs(bSourceYPos - bDesYPos) > 1 || abs(bSourceXPos-bDesXPos) > 1 )
				return false;
			else if (IsBarback(bDesXPos , bDesYPos))				
			{
				//保存位置
				if (true==bMove)
				{
					point.x = bDesXPos ;
					point.y = bDesYPos ;
					m_ptPathArray.Add(point);

					point.x = bSourceXPos ;
					point.y = bSourceYPos ;
					m_ptPathArray.Add(point);
				}
				return true ;
			}
			else if (((bSourceYPos == bDesYPos) && abs(bSourceXPos-bDesXPos)==1) || ((bSourceXPos==bDesXPos) && abs(bSourceYPos - bDesYPos)==1) )
			{
				//保存位置
				if (true==bMove)
				{
					point.x = bDesXPos ;
					point.y = bDesYPos ;
					m_ptPathArray.Add(point);

					point.x = bSourceXPos ;
					point.y = bSourceYPos ;
					m_ptPathArray.Add(point);
				}
				return true ;
			}
			return false ;
		} 
		else if ( (5==bSourceXPos || 11==bSourceXPos ) && (7==bSourceYPos || 9==bSourceYPos) )
		{	
			if (bSourceXPos==bDesXPos)
			{
				if(bDesYPos<bSourceYPos)
				{
					//中间结点
					for(int y=bDesYPos ; y<=bSourceYPos ; ++y)
					{
						//保存位置
						if (true==bMove)
						{
							point.x = bSourceXPos ;
							point.y = y ;
							m_ptPathArray.Add(point);
						}

						if(y!=bDesYPos && y!=bSourceYPos)
                            if(CHESS_NULL!=ChessItem[bSourceXPos][y].bChessID)
							{
								//清除结点
								m_ptPathArray.RemoveAll() ;
								return false ;
							}
					}
	
				}
				else if(bDesYPos>bSourceYPos)
				{
					for(int y=bDesYPos ; y>=bSourceYPos ; --y)
					{
						//保存位置
						if(true==bMove)
						{
							point.x = bSourceXPos ;
							point.y = y ;
							m_ptPathArray.Add(point);
						}
						if(y!=bDesYPos && y!=bSourceYPos)
							if(CHESS_NULL!=ChessItem[bSourceXPos][y].bChessID)
							{
								//清除结点
								m_ptPathArray.RemoveAll() ;
								return false ;
							}
					}
				}
				return true ;
			}
			return false ;
		}
		else if ( (11==bSourceYPos || 5==bSourceYPos ) && (7==bSourceXPos || 9==bSourceXPos)  )
		{
			if (bSourceYPos == bDesYPos)
			{
				//保存位置
				if (bDesXPos<bSourceXPos)
				{

					for (int x=bDesXPos ; x<=bSourceXPos ; x++)
					{
						//保存位置
						if(true==bMove)
						{
							point.y=bSourceYPos;
							point.x=x;
							m_ptPathArray.Add(point);
						}
						if(x!=bDesXPos && x!=bSourceXPos)
							if(CHESS_NULL!=ChessItem[x][bSourceYPos].bChessID)
							{
								//清除结点
								m_ptPathArray.RemoveAll() ;
								return false ;
							}
					}					
				}
				else if (bDesXPos>bSourceXPos)
				{					
					for (int x=bDesXPos ; x>=bSourceXPos && 255!=x ; x--)
					{
						//保存位置
						if(true==bMove)
						{
							point.y=bSourceYPos;
							point.x=x;
							m_ptPathArray.Add(point);
						}
						if(x!=bDesXPos && x!=bSourceXPos)
							if(CHESS_NULL!=ChessItem[x][bSourceYPos].bChessID)
							{
								//清除结点
								m_ptPathArray.RemoveAll() ;
								return false ;
							}
					}					
				}

				return true ;
			}
			
			return false ;
		}
		else if (bSourceXPos==bDesXPos && bDesYPos>0 && bDesYPos<16 )
		{
			if ( bDesYPos > bSourceYPos )
			{				
				for (int y=bDesYPos ; y>=bSourceYPos ; y--)
				{
					if (true==bMove)
					{
						point.x=bSourceXPos;
						point.y=y;
						m_ptPathArray.Add(point);
					}

					if(y!=bDesYPos && y!=bSourceYPos)
                        if (CHESS_NULL!=ChessItem[bSourceXPos][y].bChessID)
						{
							//清除结点
							m_ptPathArray.RemoveAll() ;
							return false ;
						}
				}
				return true ;
			}
			else if (bDesYPos < bSourceYPos)
			{
				for (int y=bDesYPos ; y<=bSourceYPos ; y++)
				{
					if (true==bMove)
					{
						point.x=bSourceXPos;
						point.y=y;
						m_ptPathArray.Add(point);
					}
					if(y!=bDesYPos && y!=bSourceYPos)
						if (CHESS_NULL!=ChessItem[bSourceXPos][y].bChessID)
						{
							//清除结点
							m_ptPathArray.RemoveAll() ;
							return false ;
						}
				}
				return true ;
			}
		}
		else if (bSourceYPos==bDesYPos && bDesXPos>0 && bDesXPos<16 )
		{
			if ( bDesXPos > bSourceXPos )
			{
				for (int x=bDesXPos ; x>=bSourceXPos ; x--)
				{
					if (true==bMove)
					{
						point.y=bSourceYPos;
						point.x=x;
						m_ptPathArray.Add(point);
					}
					if(x!=bDesXPos && x!=bSourceXPos)
                        if (CHESS_NULL!=ChessItem[x][bSourceYPos].bChessID)
						{
							m_ptPathArray.RemoveAll() ;
							return false ;
						}						
				}
				return true;
			}
			else if (bDesXPos < bSourceXPos)
			{
				for (int x=bDesXPos ; x<=bSourceXPos ; x++)
				{
					if (true==bMove)
					{
						point.y=bSourceYPos;
						point.x=x;
						m_ptPathArray.Add(point);
					}
					if(x!=bDesXPos && x!=bSourceXPos)
						if (CHESS_NULL!=ChessItem[x][bSourceYPos].bChessID)
						{
							m_ptPathArray.RemoveAll() ;
							return false ;
						}					
				}				
				return true ;
			}
		}
		//棋子拐弯
		else if ( (bSourceXPos==6  && bSourceYPos>=bDesYPos && bDesXPos<=bSourceXPos && bDesYPos==10 ) ||	//下转左上
				  (bSourceXPos==6  && bSourceYPos<=bDesYPos && bDesXPos<=bSourceXPos && bDesYPos==6  ) ||	//上转左下
				  (bSourceXPos==10 && bSourceYPos>=bDesYPos && bDesXPos>=bSourceXPos && bDesYPos==10 ) ||	//下转右上
				  (bSourceXPos==10 && bSourceYPos<=bDesYPos && bDesXPos>=bSourceXPos && bDesYPos==6  ) )	//上转右下
		{
			int i,j,k;
			
			//转到中心
			if ( bSourceYPos!=bDesYPos && ((bDesXPos>6 && bDesXPos<10 )|| ((bDesXPos==6 || bDesXPos==10) && bSourceXPos!=bDesXPos))) 
				return false ;

			//横向距离
			if (bDesXPos<6)
			{	
				i=5;
				k=abs(bDesXPos-i);
			}
			//横向距离
			if (bDesXPos>10 )
			{
				i=11;
				k=abs(bDesXPos-i);
			}
			if (k==0)
				sign=0;
			else
				sign=(bDesXPos-i)/abs(bDesXPos-i);
			if (true==bMove)
			{
				point.x = bDesXPos ;
				point.y = bDesYPos ;
				m_ptPathArray.Add(point);
			}
			i=bDesXPos;
			//横向坐标
			for (j=0;j<k;j++)
			{				
				i=i-sign;
				if (true==bMove)
				{
					point.x=i;
					point.y=bDesYPos;
					m_ptPathArray.Add(point);
				}
				if (i>=0 && i<17 && CHESS_NULL!=ChessItem[i][bDesYPos].bChessID)
				{
					m_ptPathArray.RemoveAll() ;
					return false ;
				}

			}			

			if (bSourceYPos<6)
			{
				i=5;
				k=abs(bSourceYPos-i);
			}
			if (bSourceYPos>10)
			{
				i=11;			
				k=abs(bSourceYPos-i);
			}
			if (k==0)
				sign=0;
			else
				sign=abs(bSourceYPos-i)/(bSourceYPos-i);

			//纵向坐标 
			for (j=0;j<k;j++)
			{
				if (true==bMove)
				{
					point.x=bSourceXPos;
					point.y=i;
					m_ptPathArray.Add(point);
				}
				if (i>=0 && i<17 && CHESS_NULL!=ChessItem[bSourceXPos][i].bChessID)
				{
					m_ptPathArray.RemoveAll() ;
					return false ;
				}
				i=i+sign;

			}
			if (true==bMove)
			{
				point.x = bSourceXPos ;
				point.y = bSourceYPos ;
				m_ptPathArray.Add(point);
			}
			return true ;
		}
		//棋子拐弯
		else if ( (bSourceYPos==6  && bSourceYPos>=bDesYPos && bDesXPos<=bSourceXPos && bDesXPos==10 ) ||	//右转左上
				  (bSourceYPos==6  && bSourceYPos>=bDesYPos	&& bDesXPos>=bSourceXPos && bDesXPos==6 )  ||	//左转右上
				  (bSourceYPos==10 && bSourceYPos<=bDesYPos	&& bDesXPos<=bSourceXPos && bDesXPos==10 ) ||	//右转左下
				  (bSourceYPos==10 && bSourceYPos<=bDesYPos	&& bDesXPos>=bSourceXPos && bDesXPos==6  ) )	//左转右下
		{
			int i,j,k;

			//转到中心 
			if (bSourceXPos!=bDesXPos && ((bDesYPos>=6 && bDesYPos<=10) ||((bDesYPos==6 || bDesYPos==10) && bDesYPos!=bSourceYPos)))
				return false ;
			if (bDesYPos<6)
			{
				i=5;
				k=abs(bDesYPos-i);
			}
			if (bDesYPos>10)
			{
				i=11;		
				k=abs(bDesYPos-i);
			}
			if (k==0)
				sign=0;
			else
				sign=(bDesYPos-i)/abs(bDesYPos-i);
			if (true==bMove)
			{
				point.x = bDesXPos ;
				point.y = bDesYPos ;
				m_ptPathArray.Add(point);
			}
			i=bDesYPos;
			//纵向坐标
			for (j=0;j<k;j++)
			{	
				i=i-sign;
				if (true==bMove)
				{
					point.y=i;
					point.x=bDesXPos ;
					m_ptPathArray.Add(point);
				}
				if (i>=0 && i<17 && CHESS_NULL!=ChessItem[bDesXPos][i].bChessID)
				{
					m_ptPathArray.RemoveAll() ;
					return false;
				}

			}			
			if (bSourceXPos<6)
			{
				i=5;
				k=abs(bSourceXPos-i);
			}

			if (bSourceXPos>10)
			{
				i=11;			
				k=abs(bSourceXPos-i);
			}

			//横向坐标
			for (j=0;j<k;j++)
			{	
				if (true==bMove)
				{
					point.y=bSourceYPos;
					point.x=i;
					m_ptPathArray.Add(point);
				}
				if (i>=0 && i<17 && CHESS_NULL!=ChessItem[i][bSourceYPos].bChessID)
				{
					m_ptPathArray.RemoveAll() ;
					return false ;
				}

				i=i+abs(bSourceXPos-i)/(bSourceXPos-i);

			}
			if (true==bMove)
			{
				point.x = bSourceXPos ;
				point.y = bSourceYPos ;
				m_ptPathArray.Add(point);
			}
			return true;

		}
	}
	//工兵棋子
	else
	{	
		//最后一行
		if (bSourceXPos == 0 || bSourceXPos == 16 || bSourceYPos == 0 || bSourceYPos == 16)
		{			
				//只走一步
			if ( (bSourceXPos == bDesXPos && abs(bSourceYPos-bDesYPos) == 1 ) || (bSourceYPos == bDesYPos && abs(bSourceXPos-bDesXPos) == 1) )			
			{
				//保存坐标
				if (true==bMove)
				{
					point.x = bDesXPos ;
					point.y = bDesYPos ;
					m_ptPathArray.Add(point);

					point.x = bSourceXPos ;
					point.y = bSourceYPos ;
					m_ptPathArray.Add(point);
				}
				return true ;
			}
			return false ;
		}
		else if (bDesXPos==0 || bDesXPos==16)
		{			
			
			if (abs(bSourceXPos-bDesXPos)==1 && bSourceYPos==bDesYPos)
			{
				if (true==bMove)
				{
					point.x = bDesXPos ;
					point.y = bDesYPos ;
					m_ptPathArray.Add(point);

					point.x = bSourceXPos ;
					point.y = bSourceYPos ;
					m_ptPathArray.Add(point);
				}
				return true ;
			}
			return false ;
		}
		else if (bDesYPos==0 || bDesYPos==16)
		{			
			if (abs(bSourceYPos-bDesYPos)==1 && bSourceXPos==bDesXPos)
			{
				if (true==bMove)
				{
					point.x = bDesXPos ;
					point.y = bDesYPos ;
					m_ptPathArray.Add(point);

					point.x = bSourceXPos ;
					point.y = bSourceYPos ;
					m_ptPathArray.Add(point);
				}
				return true;
			}
			return false;
		}	
		//移动一步
		else if (( abs(bSourceXPos-bDesXPos)==1 && abs(bSourceYPos-bDesYPos)==0 ) || (abs(bSourceYPos-bDesYPos)==1 && abs(bSourceXPos-bDesXPos)==0) )
		{			
			if (true==bMove)
			{
				point.x = bDesXPos ;
				point.y = bDesYPos ;
				m_ptPathArray.Add(point);

				point.x = bSourceXPos ;
				point.y = bSourceYPos ;
				m_ptPathArray.Add(point);
			}
			return true ;

		}
		//不在铁路
		else if ( NotOnRail(bSourceXPos , bSourceYPos) )
		{			
			if (true==bMove)
			{
				point.x = bDesXPos ;
				point.y = bDesYPos ;
				m_ptPathArray.Add(point);

				point.x = bSourceXPos ;
				point.y = bSourceYPos ;
				m_ptPathArray.Add(point);
			}
			if ( abs(bSourceYPos-bDesYPos) > 1 || abs(bSourceXPos-bDesXPos) > 1 )
			{
				m_ptPathArray.RemoveAll() ;
				return false ;
			}
			else if (IsBarback(bSourceXPos,bSourceYPos))				
			{
				return true ;
			}
			else if (((bSourceYPos==bDesYPos) && abs(bSourceXPos-bDesXPos)==1) || ((bSourceXPos==bDesXPos) && abs(bSourceYPos-bDesYPos)==1) )
			{
				return true ;
			}
			else
			{
				m_ptPathArray.RemoveAll() ;
				return false ;
			}
		} 
		//不在铁路
		else if ( NotOnRail(bDesXPos , bDesYPos) )
		{			
			if (true==bMove)
			{
				point.x = bDesXPos ;
				point.y = bDesYPos ;
				m_ptPathArray.Add(point);

				point.x = bSourceXPos ;
				point.y = bSourceYPos ;
				m_ptPathArray.Add(point);
			}
			if ( abs(bSourceYPos-bDesYPos) > 1 || abs(bSourceXPos-bDesXPos) > 1 )
			{
				m_ptPathArray.RemoveAll() ;
				return false ;
			}
			else if (IsBarback(bDesXPos , bDesYPos))				
			{
				return true ;
			}
			else if (((bSourceYPos==bDesYPos) && abs(bSourceXPos-bDesXPos)==1) || ((bSourceXPos==bDesXPos) && abs(bSourceYPos-bDesYPos)==1) )
			{
				return true ;
			}
			else
			{
				m_ptPathArray.RemoveAll() ;
				return false ;
			}
		} 
		POINT ptDest , ptStart ;
		ptDest.x  = bDesXPos ;
		ptDest.y  = bDesYPos ;
		ptStart.x = bSourceXPos ;
		ptStart.y = bSourceYPos ;

		//设置目标
		tagChessItem TargetChessItem=ChessItem[bDesXPos][bDesYPos] ;
		ChessItem[bDesXPos][bDesYPos].bChessID = CHESS_NULL ;

		bool bFind = FindPath(ptDest , ptStart , ChessItem , bMove) ;
		//恢复目标
		ChessItem[bDesXPos][bDesYPos] = TargetChessItem ;

		return bFind ;
	}

	return false ;
}

//交换棋子
bool CGameLogic::SwapChess(tagChessItem ChessItem[17][17] , BYTE bSourceX , BYTE bSourceY , BYTE bDesX , BYTE bDesY , bool bSwapChess) 
{
	//军棋棋子
	if (CHESS_NULL!=ChessItem[bSourceX][bSourceY].bChessID &&
		CHESS_NULL!=ChessItem[bDesX][bDesY].bChessID &&
		ChessItem[bSourceX][bSourceY].bColor==ChessItem[bDesX][bDesY].bColor)
	{			
		if ((CHESS_JUN_QI==ChessItem[bSourceX][bSourceY].bChessID ||
			 CHESS_JUN_QI==ChessItem[bDesX][bDesY].bChessID)      &&
			 bSourceY!=bDesY)
		{
			return false;
		}

		//不在军营
		if ((CHESS_JUN_QI==ChessItem[bSourceX][bSourceY].bChessID ||
			CHESS_JUN_QI==ChessItem[bDesX][bDesY].bChessID)       &&
			(abs(bSourceX-bDesX)!=2))
		{
			return false;
		}

		//地雷位置
		if (CHESS_DI_LEI==ChessItem[bSourceX][bSourceY].bChessID &&	bDesY<15)
		{
			return false;			
		}

		//地雷位置
		if (CHESS_DI_LEI==ChessItem[bDesX][bDesY].bChessID && bSourceY<15 )
		{			
			return false;
		}

		//炸弹位置
		if (CHESS_ZHA_DAN==ChessItem[bSourceX][bSourceY].bChessID && bDesY<12 )
		{
			return	false ;
		}

		//炸弹位置
		if (CHESS_ZHA_DAN==ChessItem[bDesX][bDesY].bChessID	&& bSourceY<12 )
		{
			return false;
		}

		if(true==bSwapChess)
		{
			//交换棋子
			tagChessItem tmpChess ;

			tmpChess = ChessItem[bSourceX][bSourceY] ;
			ChessItem[bSourceX][bSourceY] = ChessItem[bDesX][bDesY] ;
			ChessItem[bDesX][bDesY] = tmpChess ;
		}
		return true;

	}
	else
		return false ;

	return false ;
}

//是否军营
bool CGameLogic::IsBarback(BYTE bXPos , BYTE bYPos)
{
	ASSERT(bXPos>=0 && bXPos<=16 && bYPos>=0 && bYPos<=16) ;
	if(bXPos<0 || bXPos>16 || bYPos<0 || bYPos>16) return false ;

	if(7==bXPos  && (2==bYPos  || 4==bYPos)  || 8==bXPos  && 3==bYPos  || 9==bXPos  && (2==bYPos  || 4==bYPos)  || //第0玩家
	   2==bXPos  && (7==bYPos  || 9==bYPos)  || 3==bXPos  && 8==bYPos  || 4==bXPos  && (7==bYPos  || 9==bYPos)  || //第1玩家
	   7==bXPos  && (12==bYPos || 14==bYPos) || 8==bXPos  && 13==bYPos || 9==bXPos  && (12==bYPos || 14==bYPos) || //第2玩家
	   12==bXPos && (7==bYPos  || 9==bYPos)  || 13==bXPos && 8==bYPos  || 14==bXPos && (7==bYPos  || 9==bYPos)     //第3玩家
	   )
	   return true ;

	return false ;
}

//是否敌方
bool CGameLogic::IsEnemy(BYTE bPlayerCount , const tagChessItem ChessItem[17][17] , const BYTE bSourceXPos , const BYTE bSourceYPos , const BYTE bDesXPos , const BYTE bDesYPos)
{
	if(ChessItem[bSourceXPos][bSourceYPos].bColor == ChessItem[bDesXPos][bDesYPos].bColor) return false ;

	if(4==bPlayerCount)
	{
		if((2+ChessItem[bSourceXPos][bSourceYPos].bColor)%4==ChessItem[bDesXPos][bDesYPos].bColor || (2+ChessItem[bSourceXPos][bSourceYPos].bColor)==ChessItem[bDesXPos][bDesYPos].bColor)
			return false ;
	}

	return true ;
}

//是否拐弯
bool CGameLogic::AbleTurn(BYTE const bXPos , BYTE const bYPos)
{
	if ( (bXPos==6   &&  bYPos==5  ) ||
		 (bXPos==5   &&  bYPos==6  ) ||
		 (bXPos==10  &&  bYPos==5  ) ||
		 (bXPos==11  &&  bYPos==6  ) ||
		 (bXPos==5   &&  bYPos==10 ) ||
		 (bXPos==6   &&  bYPos==11 ) ||
		 (bXPos==10  &&  bYPos==11 ) ||
		 (bXPos==11  &&  bYPos==10 ) )
	{
		return true;
	}
	return false;
}

//不在铁路
bool CGameLogic::NotOnRail(BYTE const bXPos , BYTE const bYPos)
{
	if ((bXPos>=2  && bXPos<=4  && bYPos>=7 && bYPos<=9) ||
		(bYPos>=2  && bYPos<=4  && bXPos>=7 && bXPos<=9) ||
		(bXPos>=12 && bXPos<=14 && bYPos>=7 && bYPos<=9) ||
		(bYPos>=12 && bYPos<=14 && bXPos>=7 && bXPos<=9) )
        return true ;

	return false ;
}

//比较棋子
enCompareResult CGameLogic::CompareChess(const tagChessItem ChessItem[17][17] , const BYTE bSourceXPos , const BYTE bSourceYPos , const BYTE bDesXPos , const BYTE bDesYPos)
{
	if(CHESS_NULL==ChessItem[bSourceXPos][bSourceYPos].bChessID) 
		return enError ;

	else if(CHESS_NULL==ChessItem[bDesXPos][bDesYPos].bChessID)
		return enLarge ;
	
	else if (CHESS_ZHA_DAN==ChessItem[bDesXPos][bDesYPos].bChessID)
		return enEqual;

	else if (CHESS_ZHA_DAN==ChessItem[bSourceXPos][bSourceYPos].bChessID)
		return enEqual;

	else if(CHESS_DI_LEI==ChessItem[bDesXPos][bDesYPos].bChessID && CHESS_GONG_BING!=ChessItem[bSourceXPos][bSourceYPos].bChessID)
		return enSmall ;

	else if (CHESS_GONG_BING==ChessItem[bSourceXPos][bSourceYPos].bChessID && CHESS_DI_LEI==ChessItem[bDesXPos][bDesYPos].bChessID)
		return enLarge;

	if ((ChessItem[bDesXPos][bDesYPos].bChessID	> ChessItem[bSourceXPos][bSourceYPos].bChessID))
		return enSmall;

	else if (ChessItem[bDesXPos][bDesYPos].bChessID	== ChessItem[bSourceXPos][bSourceYPos].bChessID)
		return enEqual;

	else if(ChessItem[bSourceXPos][bSourceYPos].bChessID > ChessItem[bDesXPos][bDesYPos].bChessID)
		return enLarge ;

	return enError ;
}

//在大本营
bool CGameLogic::IsHeadquarters(BYTE bXPos , BYTE bYPos)
{
	if((7==bXPos || 9==bXPos)  && (0==bYPos || 16==bYPos) ||
	   (0==bXPos || 16==bXPos) && (7==bYPos || 9==bYPos))
	   return true ;

	return false ;
}

//非结点处
bool CGameLogic::NotOnNode(BYTE bXPos , BYTE bYPos) 
{
	if ( (bXPos==6  &&  bYPos==9 ) ||
		 (bXPos==6  &&  bYPos==7 ) ||
		 (bXPos==7  &&  bYPos==6 ) ||
		 (bXPos==7  &&  bYPos==8 ) ||
		 (bXPos==7  &&  bYPos==10) ||
		 (bXPos==9  &&  bYPos==6 ) ||
		 (bXPos==9  &&  bYPos==8 ) ||
		 (bXPos==9  &&  bYPos==10) ||
		 (bXPos==8  &&  bYPos==7 ) ||
		 (bXPos==8  &&  bYPos==9 ) ||
		 (bXPos==10 &&  bYPos==7 ) ||
		 (bXPos==10 &&  bYPos==9 ) ||
		 (bXPos==7  &&  bYPos==7 ) || 
		 (bXPos==9  &&  bYPos==7 ) ||
		 (bXPos==7  &&  bYPos==9 ) ||
		 (bXPos==9  &&  bYPos==9))
	{
		return true;
	}
	return false;
}

//是否棋盘
bool CGameLogic::IsOnBoard(BYTE bXPos , BYTE bYPos)
{
	if( (bXPos>=6  && bXPos<=10 && bYPos>=0  && bYPos<=5)  ||
		(bXPos>=11 && bXPos<=16 && bYPos>=6  && bYPos<=10) ||
		(bXPos>=6  && bXPos<=10 && bYPos>=11 && bYPos<=16) ||
		(bXPos>=0  && bXPos<=5  && bYPos>=6  && bYPos<=10) ||
		(bXPos>=6  && bXPos<=10 && bYPos>=6  && bYPos<=10  && !NotOnNode(bXPos,bYPos))
		)
		return true ;
	return false ;
}

//可否扛旗
bool CGameLogic::CanCarry(BYTE const bChessID , BYTE const bChessColor , const tagChessItem ChessItem[17][17])
{
	BYTE bMinChessID=bChessID ;
	for(BYTE x=0 ; x<17 ; ++x)
		for(BYTE y=0 ; y<17 ; ++y)
		{
			if(bChessColor==ChessItem[x][y].bColor)
				if(CHESS_JUN_QI!=ChessItem[x][y].bChessID && CHESS_DI_LEI!=ChessItem[x][y].bChessID && CHESS_ZHA_DAN!=ChessItem[x][y].bChessID && CHESS_NULL!=ChessItem[x][y].bChessID && !IsHeadquarters(x,y))
					if(bMinChessID>ChessItem[x][y].bChessID)
						bMinChessID=ChessItem[x][y].bChessID ;
		}
	return bMinChessID==bChessID ;
}

//没有棋子
bool CGameLogic::NoChess(BYTE const bChessColor , const tagChessItem ChessItem[17][17]) 
{
	for(BYTE x=0 ; x<17 ; ++x)
		for(BYTE y=0 ; y<17 ; ++y)
		{
			if(bChessColor==ChessItem[x][y].bColor && CHESS_NULL!=ChessItem[x][y].bChessID && !IsHeadquarters(x,y) && CHESS_DI_LEI!=ChessItem[x][y].bChessID)
				return false ;
		}
	return true ;
}



//棋子名字
CString CGameLogic::IDToName(BYTE bChessID) 
{
	switch(bChessID)
	{
	case CHESS_SI_LING :
		return "司令" ;

	case CHESS_JUN_ZHANG :
		return "军长" ;

	case CHESS_SHI_ZHANG :
		return "师长" ;

	case CHESS_LV_ZHANG :
		return "旅长" ;

	case CHESS_TUAN_ZHANG :
		return "团长" ;

	case CHESS_YING_ZHANG :
		return "营长" ;

	case CHESS_LIAN_ZHANG :
		return "连长" ;

	case CHESS_PAI_ZHANG :
		return "排长" ;

	case CHESS_GONG_BING :
		return "工兵";

	case CHESS_ZHA_DAN :
		return "炸弹" ;

	case CHESS_DI_LEI :
		return "地雷" ;

	case CHESS_JUN_QI :
		return "军旗";

	case CHESS_NULL :
		return "没有";

	default:
		return "错误" ;
	}
}








////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//工兵函数


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//寻找路径
bool CGameLogic::FindPath(POINT ptDest,POINT ptStart , tagChessItem ChessItem[17][17] , bool bMove)
{
	//四个方向
	Init(ptDest,ptStart,ChessItem);

	int nXDirection[8]={1,1,0,-1,-1,-1,0,1};
	int nYDirection[8]={0,-1,-1,-1,0,1,1,1};
	int x,y;
	CString strOut,strBuffer;
	POSITION pTemp;	
	m_bStop=false;

	while (!m_bStop)
	{	
		pTemp=NULL;

		//最小结点
		GetMinDisNode(&m_ListCanPassNode,pTemp);							
		if (!pTemp)			
		{
			m_bStop=true;
			m_bFind=false;
			ClearList();
			return m_bFind;	
		}

		m_pCurNode=(CNode * )m_ListCanPassNode.GetAt(pTemp);

		m_ListCanPassNode.RemoveAt(pTemp);
		m_ListHaveGoneNode.AddTail((void *)m_pCurNode);

		int	 count=0;
		BOOL bEmpty;

		for (int i=0 ; i<8 ; i++)
		{

			x = m_pCurNode->bXPos + nXDirection[i] ;
			y = m_pCurNode->bYPos + nYDirection[i] ;


			if (x<0 || y<0 || x>=17 || y>=17)
				continue;
			//不在棋盘
			if(! ((x>=6  && x<=10 && y>=1  && y<=5)  ||
				  (x>=11 && x<=15 && y>=6  && y<=10) ||
				  (x>=6  && x<=10 && y>=11 && y<=15) ||
				  (x>=1  && x<=5  && y>=6  && y<=10) ||
				  (x>=6  && x<=10 && y>=6  && y<=10)
				))
				continue ;

			//不在铁路
			if(true==NotOnRail(x,y)) 
				continue ;

			//中心区域
			if((7==x && 7==y) || (7==x && 9==y) || (9==x && 7==y) || (9==x && 9==y))
				continue ;

			//军营位置
			if(IsBarback(x,y))
				continue ;

			//碰到棋子
			if (CHESS_NULL!=ChessItem[x][y].bChessID)
				continue;
			   
			//斜线，拐弯点
			if ( i%2!=0 && (!AbleTurn(x,y) || !AbleTurn(m_pCurNode->bXPos,m_pCurNode->bYPos) ) )
				continue;

			//上下左右
			if  ((6==x  && (7==y || 9==y)  && 0==i)	||	//向右走到非结点
				((7==x  || 9==x) && 10==y  && 2==i)	||	//向上走到非结点
				(10==x  && (7==y || 9==y)  && 4==i)	||	//向左走到非结点
				((7==x  || 9==x) && 6==y   && 6==i))	//向下走到非结点
				continue;

			if ((5==x  && (7==y  || 9==y ) && 4==i)||	//从非结点向左走
				((7==x || 9==x)  && 5==y   && 2==i)||	//从非结点向上走
				(11==x && (7==y  || 9==y)  && 0==i)||	//从非结点向右走
				((7==x || 9==x)  && 11==y  && 6==i))	//从非结点向下走
				continue;

			bEmpty=m_ListCanPassNode.IsEmpty();

			POSITION pInOpen,pInClose;
			pInOpen=pInClose=NULL;

			FinPos(m_ListCanPassNode,x,y,pInOpen);

			count=(int)(m_ListHaveGoneNode.GetCount());		
			if (0!=count)
			{
				FinPos(m_ListHaveGoneNode,x,y,pInClose);
			}

			//已经走过
			if (pInClose)
			{
				continue;
			}	

			//没有走过
			if ( pInOpen==NULL)
			{					
				CNode * pNewCell=new CNode(x,y);

				pNewCell->pParent=m_pCurNode;				
				pNewCell->bToDesDis=ComputeNodeDis(m_bXDest,m_bYDest,x,y);
				pNewCell->ComputeDistance();
				m_ListCanPassNode.AddHead((void *)pNewCell);				

			}		
			else 
			{
				CNode * pExist=(CNode *)m_ListCanPassNode.GetAt(pInOpen);				
				BYTE tempG=ComputeToStartDis(*pExist,*m_pCurNode);
				//更新距离 
				if (pExist->bToStartDis>tempG )
				{					
					pExist->pParent=m_pCurNode;
					pExist->bToStartDis=tempG;
					pExist->bTotalDisDis=pExist->bToStartDis+pExist->bToDesDis;	
				}
			}
			//找到目标
			if (x==m_bXDest && y==m_bYDest )					
			{
				m_bStop=true;
				m_bFind=true;
				m_pEndNode=(CNode *)m_ListCanPassNode.GetHead();

				if(true==bMove)
				{
					m_ptPathArray.RemoveAll() ;
					POINT point ;

					//保存路径
					for (CNode *pNode=m_pEndNode ; pNode!=NULL ; pNode=pNode->pParent)
					{
						point.x = pNode->bXPos ;
						point.y = pNode->bYPos ;
						m_ptPathArray.Add(point) ;
					}
				}

				//清除链表
				ClearList() ;

				return m_bFind;	
			}
		}
	}
	//清除链表
	ClearList() ;

	return m_bFind;
}

//结点距离
BYTE CGameLogic::ComputeToStartDis(CNode &cellA, CNode &cellB)
{
	BYTE bXPos , bYPos ;

	bXPos = abs(cellA.bXPos - cellB.bXPos);
	bYPos = abs(cellA.bYPos - cellB.bYPos);

	//对角距离
	if (1==bXPos && 1==bYPos)
	{
		return 2+cellB.bToStartDis;
	}
	//一步距离
	else	
		return 1+cellB.bToStartDis;
}

//清除链表
void CGameLogic::ClearList()
{
	CNode * pTemp;
	POSITION posTemp=m_ListCanPassNode.GetHeadPosition();
	for (posTemp;posTemp!= NULL; )
	{
		pTemp=(CNode *)m_ListCanPassNode.GetNext(posTemp);		
		if (!pTemp)
			delete pTemp;
	}
	for (posTemp=m_ListHaveGoneNode.GetHeadPosition(); posTemp!= NULL; )
	{
		pTemp=(CNode *)m_ListHaveGoneNode.GetNext(posTemp);		
		if (!pTemp)
			delete pTemp;
	}
	m_ListCanPassNode.RemoveAll();
	m_ListHaveGoneNode.RemoveAll();
}

//两点距离 
int CGameLogic::ComputeNodeDis(int xdest, int ydest,int xorgin,int yorgin)
{
	BYTE bXPos , bYPos , bTotalDis ;

	bXPos = abs(xdest - xorgin);
	bYPos = abs(ydest - yorgin);

	//总共距离
	bTotalDis = (bXPos+bYPos);	

	return bTotalDis;
}

//最小结点
void CGameLogic::GetMinDisNode(CPtrList *plist, POSITION &pos)
{
	CNode *pMin,*pTemp;
	if (plist->IsEmpty())
	{
		pos=NULL;
		return;
	}

	pMin=(CNode * )plist->GetHead();
	POSITION posTemp=plist->GetTailPosition();

	if (pMin)
	{
		for (pTemp=pMin; posTemp!= NULL; pTemp=(CNode * )plist->GetPrev(posTemp))
		{				
			if (pTemp->bTotalDisDis <= pMin->bTotalDisDis)
			{
				pMin=pTemp;	
				continue;
			}		
		}
		pos=plist->Find((void *)pMin);
	}
	else
		pos=NULL;	

	pMin=NULL;
	pTemp=NULL;

}

//寻找位置
void CGameLogic::FinPos(CPtrList &list,int x,int y,POSITION &pos)
{
	CNode * pCell;
	POSITION pTemp;
	for (pTemp=list.GetHeadPosition();pTemp!= NULL;)
	{
		pCell=(CNode * )list.GetNext(pTemp);
		if (pCell->bXPos==x && pCell->bYPos==y)
		{
			pos=list.Find((void *)pCell);			
			break;
		}		
	}	
	return;
}

//初始结点
void CGameLogic::Init(POINT ptDest, POINT ptStart , tagChessItem ChessItem[17][17])
{
	int  tx[8]={1, 1, 0, -1,-1,-1, 0, 1};
	int  ty[8]={0,-1,-1, -1, 0, 1, 1, 1};
	BYTE x,y;
	CNode * pNew;
	ClearList();	

	m_bXDest=(BYTE)(ptDest.x);
	m_bYDest=(BYTE)(ptDest.y);

	m_StartNode=CNode(ptStart.x,ptStart.y);
	m_StartNode.bToStartDis=0;			
	m_StartNode.pParent=NULL;

	//两点步数
	m_StartNode.bTotalDisDis=ComputeNodeDis(m_bXDest,m_bYDest,m_StartNode.bXPos,m_StartNode.bYPos);

	////////////////////////////////////////////////////////

	m_bStop=false;
	m_bFind=false;	

	for (int i=0;i<8;i++)
	{
		x=m_StartNode.bXPos+tx[i];
		y=m_StartNode.bYPos+ty[i];		

		if (x<0 || y<0 || x>=17 || y>=17)
			continue;
		//不在棋盘
		if(! ((x>=6  && x<=10 && y>=1  && y<=5)  ||
			  (x>=11 && x<=15 && y>=6  && y<=10) ||
			  (x>=6  && x<=10 && y>=11 && y<=15) ||
			  (x>=1  && x<=5  && y>=6  && y<=10) ||
			  (x>=6  && x<=10 && y>=6  && y<=10)
			))
			continue ;

		//不在铁路
		if(true==NotOnRail(x,y)) 
			continue ;

		//中心区域
		if((7==x && 7==y) || (7==x && 9==y) || (9==x && 7==y) || (9==x && 9==y))
			continue ;

		//军营位置
		if(IsBarback(x,y))
			continue ;

		//碰到棋子
		if (CHESS_NULL!=ChessItem[x][y].bChessID)
			continue;

		//上下左右,或者特殊拐弯点
		if (i%2!=0 && (!AbleTurn(x,y) || !AbleTurn((BYTE)(ptStart.x) , (BYTE)(ptStart.y)) ))
			continue;

		if  ((6==x  && (7==y || 9==y)  && 0==i)	||	//向右走到非结点
			((7==x  || 9==x) && 10==y  && 2==i)	||	//向上走到非结点
			(10==x  && (7==y || 9==y)  && 4==i)	||	//向左走到非结点
			((7==x  || 9==x) && 6==y   && 6==i))	//向下走到非结点
			continue;

		if ((5==x  && (7==y || 9==y ) && 4==i) ||	//从非结点向左走
			((7==x || 9==x)  && 5==y   && 2==i)||	//从非结点向上走
			(11==x && (7==y  || 9==y)  && 0==i)||	//从非结点向右走
			((7==x || 9==x)  && 11==y  && 6==i))	//从非结点向下走
			continue;

		pNew=new CNode(x,y);						
		pNew->pParent=&m_StartNode;
		pNew->bToDesDis=ComputeNodeDis(m_bXDest,m_bYDest,x,y);
		pNew->ComputeDistance();			
		m_ListCanPassNode.AddTail((void *)pNew);
		pNew=NULL;		
	}
	m_ListHaveGoneNode.AddTail((void *)&m_StartNode);
}
//////////////////////////////////////////////////////////////////////////
