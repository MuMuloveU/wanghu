#pragma once

#include "Stdafx.h"
#include <AFXTEMPL.H>

//////////////////////////////////////////////////////////////////////////

enum enCompareResult 
{
	enSmall , 
	enEqual , 
	enLarge ,
	enError
};

//结点信息
class CNode  
{
public:
	//计算距离
	void ComputeDistance();	
	CNode();
	CNode(int x,int y);
	virtual ~CNode();

public:
	BYTE							bXPos ,								//结点坐标
									bYPos;								//结点坐标
public:
	BYTE							bToDesDis,							//目标距离
									bToStartDis,						//起始距离
									bTotalDisDis;						//总共距离

	CNode							*pParent;							//上一结点
};


//游戏逻辑类
class CGameLogic
{
	//棋谱变量
public:


	//棋盘变量
public:
	tagChessItem					m_ChessItem[GAME_PLAYER][25];		//棋子信息
	tagChessItem *					m_ChessBorad[17][17];				//棋盘信息
	CArray <POINT,POINT>			m_ptPathArray;						//走棋路线

	//工兵变量
public:
	bool							m_bStop,							//停止标志
									m_bFind;							//找到标志

	BYTE							m_bYDest,							//目标结点 
									m_bXDest;							//目标结点 

	CPtrList						m_ListCanPassNode,					//可走结点				
									m_ListHaveGoneNode;					//走过结点

	CNode							*m_pCurNode,						//当前结点					
									m_StartNode,						//开始结点
									*m_pEndNode;						//结束结点


	//函数定义
public:
	//构造函数
	CGameLogic();
	//析构函数
	virtual ~CGameLogic();

	//信息函数
public:

	//管理函数
public:

	//控制函数
public:
	//交换棋子
	bool SwapChess(tagChessItem ChessItem[17][17] , BYTE bSourceX , BYTE bSourceY , BYTE bDesX , BYTE bDesY , bool bSwapChess=true)  ;


	//逻辑函数
public:
	//能否移动
	bool AbleMove(const BYTE bChessColor , const tagChessItem ChessItem[17][17] , const BYTE bXPos , const BYTE bYPos) ;
	//是否军营
	bool IsBarback(BYTE bXPos , BYTE bYPos) ;
	//移动棋子
	bool MoveChess(BYTE  const bPlayerCount , tagChessItem ChessItem[17][17] , const BYTE bSourceXPos , const BYTE bSourceYPos , const BYTE bDesXPos , const BYTE bDesYPos , bool bMove=true) ;
	//是否敌方
	bool IsEnemy(BYTE bPlayerCount , const tagChessItem ChessItem[17][17] , const BYTE bSourceXPos , const BYTE bSourceYPos , const BYTE bDesXPos , const BYTE bDesYPos) ;
	//是否拐弯
	bool AbleTurn(BYTE const bXPos , BYTE const bYPos) ;
	//不在铁路
	bool NotOnRail(BYTE const bXPos , BYTE const bYPos) ;
	//比较棋子
	enCompareResult CompareChess(const tagChessItem ChessItem[17][17] , const BYTE bSourceXPos , const BYTE bSourceYPos , const BYTE bDesXPos , const BYTE bDesYPos) ;
	//在大本营
	bool IsHeadquarters(BYTE bXPos , BYTE bYPos) ;
	//非结点处
	bool NotOnNode(BYTE bXPos , BYTE bYPos) ;
	//是否棋盘
	bool IsOnBoard(BYTE bXPos , BYTE bYPos) ;
	//可否扛旗
    bool CanCarry(BYTE const bChessID , BYTE const bChessColor , const tagChessItem ChessItem[17][17]) ;
	//没有棋子
	bool NoChess(BYTE const bChessColor , const tagChessItem ChessItem[17][17]) ;
	//棋子名字
	CString IDToName(BYTE bChessID) ;

	//工兵函数 
public:
	//寻找路径
	bool FindPath(POINT ptDest,POINT ptStart , tagChessItem ChessItem[17][17] , bool bMove=true);
	//结点距离
	BYTE ComputeToStartDis(CNode &cellA,CNode &cellB);
	//清除链表
	void ClearList();	
	//两点距离 
	int ComputeNodeDis(int xdest,int ydest,int xorgin,int yorgin);
	//最小结点
	void GetMinDisNode(CPtrList *list,POSITION &pos);
	//寻找位置
	void FinPos(CPtrList &list,int x,int y,POSITION &pos);
	//初始结点
	void Init(POINT ptDest,POINT ptStart , tagChessItem ChessItem[17][17]);

	//内部函数
private:
};

//////////////////////////////////////////////////////////////////////////
