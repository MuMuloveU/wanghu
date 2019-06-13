#include "StdAfx.h"
#include "historyrecord.h"

CHistoryRecord::CHistoryRecord(void)
{
    memset(m_Chess , 0 , sizeof(m_Chess)) ;
	RemoveAllFromList();
}

CHistoryRecord::~CHistoryRecord(void)
{
}

//添加棋子
void CHistoryRecord::AddToList(SaveChessStruct Chess)			
{
	m_list.AddTail(Chess);		
}

//初始链表
void CHistoryRecord::Init()
{
	memset(m_Chess , 0 , sizeof(m_Chess)) ;
	RemoveAllFromList();
}

//删除棋子
void CHistoryRecord::RemoveAllFromList()
{
	m_list.RemoveAll();
}

//获取棋子
bool CHistoryRecord::GetFirstFromList(SaveChessStruct &ChessStruct)
{
	m_pos=m_list.GetHeadPosition();
	if(!m_pos)
	{
		return false ;
	}
	ChessStruct = m_list.GetNext(m_pos);
	return true ;
}

//获取棋子
bool CHistoryRecord::GetNextFromList(SaveChessStruct &ChessStruct)
{
	if(!m_pos)
	{
		return false ;
	}
	SaveChessStruct save;
	save=m_list.GetNext(m_pos);

	ChessStruct= save ;
	return true ;
}

//获取棋子
bool  CHistoryRecord::GetPrevFromList(SaveChessStruct &ChessStruct)
{	
	if(!m_pos)
	{
		//出错处理
		return false ;
	}
	SaveChessStruct save;
	save=m_list.GetPrev(m_pos);

	ChessStruct = save ;
	return true ;
}

//获取棋子
bool CHistoryRecord::GetOneFromList(short num , SaveChessStruct &ChessStruct)
{
	SaveChessStruct save;
	POSITION pos;
	pos=m_list.GetHeadPosition();
	for(int i=0;i<num;i++)
	{
		if(!pos) return false ;

		save=m_list.GetNext(pos);
	}
	ChessStruct = save ;
	return true ;
}

//保存布局
void CHistoryRecord::SavePlacement(const tagChessItem Chess[17][17])
{
    CopyMemory(m_Chess , Chess , sizeof(m_Chess)) ;
}



//坐标转换
void CHistoryRecord::ClientToServer(const BYTE bChessColor , const BYTE bRealClientX , const BYTE bRealClientY , BYTE &bServerX , BYTE &bServerY) 
{
	switch(bChessColor) 
	{
	case CHESS_COLOR_GREEN:	//绿色棋子
		bServerX = 16-bRealClientX ;
		bServerY = 16-bRealClientY ;
		break;

	case CHESS_COLOR_BLACK:	//黑色棋子
		bServerX = bRealClientY ;
		bServerY = 16-bRealClientX ;
		break;

	case CHESS_COLOR_BLUE:	//蓝色棋子
		bServerX = bRealClientX ;
		bServerY = bRealClientY ;
		break;

	case CHESS_COLOR_RED:	//红色棋子
		bServerX = 16-bRealClientY ;
		bServerY = bRealClientX ;
		break;

	default:
#ifdef _DEBUG
		AfxMessageBox("CHistoryRecord::ClientToServer:错误棋子") ;
#endif
		bServerX = 255 ;
		bServerY = 255 ;
	}
}

//获取步数
LONG CHistoryRecord::GetStepCount() 
{
	return (LONG)(m_list.GetCount()) ;
}
