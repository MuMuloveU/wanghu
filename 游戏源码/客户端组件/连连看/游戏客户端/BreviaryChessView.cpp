// BreviaryChessView.cpp : 实现文件
//

#include "stdafx.h"
#include "Resource.h"
#include "BreviaryChessView.h"
#include ".\breviarychessview.h"


// BreviaryChessView
BreviaryChessView::BreviaryChessView()
{
	m_ChessFrameWidth = 10;
	m_GroupID = 0;
	InitChess();
}

BreviaryChessView::~BreviaryChessView()
{
}


void BreviaryChessView::InitChess(void)
{
	m_per[0]=0;
	m_score = 0;
	m_class[0]=0;
	
	m_isLose = false;

	_snprintf(m_scorestring,sizeof(m_scorestring),TEXT("得分:0"));
	for(int i=1; i< 82; i++)
	{
		memset(&m_Chess[i],0,sizeof(m_Chess[i]));
	}
}
