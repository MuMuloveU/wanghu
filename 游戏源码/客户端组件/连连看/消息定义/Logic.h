#pragma once

#include "CMD_LLShow.h"

#define		WAY_UP			0
#define		WAY_RIGHT		1
#define		WAY_DOWN		2
#define		WAY_LEFT		3

class CLogic
{
public:
	CLogic(void);
	~CLogic(void);
	
	// 判断矩阵是否相交
	bool IsLink(BYTE  chess[CHESSCOUNT_H][CHESSCOUNT_W], CPoint first, CPoint second,CArrayTemplate<CPoint> *v,CPoint *ipoint1,CPoint *ipoint2);
	// 搜索
	bool FindMap(CArrayTemplate<CPoint> *v,BYTE  chess[11][19],CPoint point,  int way, bool isFirst);
	// 位置是否合法
	bool InChess(BYTE  chess[CHESSCOUNT_H][CHESSCOUNT_W],int w, int h);
	// 找出焦点
	bool AssertHaveSame(CArrayTemplate<CPoint> * f, CArrayTemplate<CPoint> * s, CPoint *same);
	// 重新排列
	void ShortPath(BYTE  chess[CHESSCOUNT_H][CHESSCOUNT_W],CArrayTemplate<CPoint> * r, CPoint one , CPoint two, CPoint* ipoint);
	// 连接提示
	bool LinkPrompt(BYTE  chess[CHESSCOUNT_H][CHESSCOUNT_W], CPoint * first, CPoint * end);
	// 重新排列
	void Realign(BYTE  chess[CHESSCOUNT_H][CHESSCOUNT_W]);


	//计算等级
	LPCTSTR AssertUserClass(LONG score);
	//
	float AssertUserPer(LONG score);
	//打开网页
	void OpenUrl(CString url);
	// 棋盘是否为空
	bool ChessEmpty(BYTE  chess[CHESSCOUNT_H][CHESSCOUNT_W]);
};
