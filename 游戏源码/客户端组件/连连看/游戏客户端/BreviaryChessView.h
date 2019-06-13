#ifndef BRECHESSVIEW_HEAD_FILE
#define BRECHESSVIEW_HEAD_FILE

#pragma once


// BreviaryChessView

class BreviaryChessView
{
public:
	BreviaryChessView();
	virtual ~BreviaryChessView();


	DBall										m_Chess[82];						//游戏棋盘，第0元素没有使用
	int											m_score;								//分数
	TCHAR										m_scorestring[32];							
	LONG										m_ChessFrameWidth;						//棋盘边框宽度
	int											m_ChessFrame_W;
	int											m_ChessFrame_H;

	BYTE										m_GroupID;								//组队ID

	TCHAR										m_class[32];
	TCHAR										m_per[32];

	bool										m_isLose;								//是否失败
protected:	

	CGameLogic      								m_Logic;								//游戏逻辑类
	
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	void InitChess(void);
};
#endif

