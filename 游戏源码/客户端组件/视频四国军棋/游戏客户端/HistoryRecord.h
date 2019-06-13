#pragma once
#ifndef HISTORYRECORD_HEAD_FILE
#define HISTORYRECORD_HEAD_FILE

//历史记录
#include "afxtempl.h"

struct SaveChessStruct
{
	BYTE									bSourceX ;							//起始位置
	BYTE									bSourceY ;							//起始位置
	BYTE									bTargetX ;							//目标位置
	BYTE									bTargetY ;							//目标位置
};



/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

 注意：m_Chess和SaveChessStruct里的坐标，保存时是按照服务器坐标保存的，当读取时会相对客户端转换成客户端的坐标

 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
class CHistoryRecord
{
public:
	CHistoryRecord(void);
	~CHistoryRecord(void);

public:
	CList<SaveChessStruct,SaveChessStruct>	m_list;								//起点终点
public:

	tagChessItem							m_Chess[17][17];					//布局阵法
	BYTE									m_Player;							//玩家人数
	POSITION								m_pos;								//位置标志

public:
	//保存布局
	void SavePlacement(const tagChessItem Chess[17][17]);
	//添加棋子
	void AddToList(SaveChessStruct);
	//删除棋子
	void RemoveAllFromList();
	//获取棋子
	bool GetFirstFromList(SaveChessStruct &ChessStruct);
	//获取棋子
	bool GetNextFromList(SaveChessStruct &ChessStruct);
	//获取棋子
	bool GetPrevFromList(SaveChessStruct &ChessStruct);
	//获取棋子
	bool GetOneFromList(short num , SaveChessStruct &ChessStruct);
	//获取步数
	LONG GetStepCount() ;
	//初始链表
	void Init();
	//坐标转换
	void ClientToServer(const BYTE bChessColor , const BYTE bRealClientX , const BYTE bRealClientY , BYTE &bServerX , BYTE &bServerY)  ;
};
#endif
