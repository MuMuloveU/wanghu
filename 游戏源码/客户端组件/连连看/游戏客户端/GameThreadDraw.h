#ifndef THREADDRAW_HEAD_FILE
#define THREADDRAW_HEAD_FILE

#pragma once

#include "StdAfx.h"
#include "ChessView.h"
#include "BreviaryChessView.h"
#define   MSCREENPOSY      80   

class CGameClientView;

struct DrawInfoStruct
{
	//标志数据
	bool						bChangeData;			//是否更改数据

	//公共访问
	RECT						ClientRect;				//客户端位置

	//一次性初始化数据 
	CChessView					*m_ChessView;			//棋盘视图
	CGameClientView				*pPlayView;				//游戏视图指针

	DBall						*m_Chess;			//主棋盘
	BreviaryChessView			*m_BreviaryView[3];	//缩略棋盘

	CDC							memDC;				//边框缓冲
	bool						ismemup;			//缓冲是否更新
	bool						isbomb;				//是否炸弹
	bool						isstart;			//显示游戏开始

	bool						start_do;			//闪耀动作

	//线程使用部分
	long int					dwSpeed;				//显示帧数

	int							m_XBoard;				//边框x
	int							m_YBoard;				//边框y
	int							m_ChessBoardX;			//棋盘边框
	int							m_ChessBoardY;
	int							m_sChessBoardX;			//缩略棋盘边框
	int							m_sChessBoardY;			//缩略棋盘边框
	int							m_ChessPos_X;
	int							m_ChessPos_Y;
	int							kx;						//横向空隙
	int 						m_ViewWidth;
	int 						m_ViewHeight;

	int							m_start;

	BYTE						m_Result;				//
	DBall						m_ResultColor;
	DBall						*m_nextballcolor;

	int							m_TimerCount;			//定时器

	CPoint						m_nextballpos;
	CPoint						m_PropPos;
	CPoint						m_BombPos;
	CPoint						m_SetStayPos;

	int							m_succ;
	bool						isHitToo;
	CPoint						m_TextScorePos;
	BYTE						m_TextScorePosKx;
    TCHAR						m_TextScore[40];
	TCHAR						m_TextHitToo[40];
	bool						m_isGetSelect;
	bool						m_isGetBomb;
	bool						m_isGetMagic;
	bool						m_isGetStay;
	bool						m_GameTimeOut;
	bool						m_isSetStay;
	TCHAR						m_TextGetSelect[40];
	TCHAR						m_TextGetBomb[40];
	TCHAR						m_TextGetMagic[40];
	TCHAR						m_TextGetStay[40];

	bool						m_TimeStay;
	TCHAR						m_TimeStayString[40];    //时间魔法

	//百分比
	bool						m_HundredDisplayed;

	//死亡图片过程
	int						m_succ_up;

	//练习模式
	bool						m_Exered;

	//是否游戏结束显示赢
	bool						m_isGameOver;
};
class CGameThreadDraw : public CThreadDraw
{
public:
	//构造函数
	CGameThreadDraw(CWnd * pGameView);
	virtual ~CGameThreadDraw();

	//重载函数
public:
	//初始化绘画
	virtual bool InitDraw(CDC * pDrawDC);
	//销毁绘画
	virtual bool UnInitDraw();
	//绘画函数
	virtual void ThreadDrawView(CDC * pDC, int nClinetWith, int nClinetHeight);
	 void DrawViewFrameAndBg(CDC *pDC);
	void GDIModeDraw(CDC * pDC, DrawInfoStruct * pDrawInfo);
	void GDIModeInitThread(CDC * pDrawDC,DrawInfoStruct * pDrawInfo);
private:
	CSkinImage										m_ImageBkg;								//背景
	CSkinImage										m_ImageChessboard;						//棋盘
	CSkinImage										m_ImageFrame;

	CSkinImage										m_ImageTalk;						//聊天	

	CSkinImage										m_ImageChessman;						//棋子
	CSkinImage										m_ImageChessman2;						//闪耀棋子

	CSkinImage										m_ImageMARK;							//标志
	CSkinImage										m_ImageProp;							//道具
	CSkinImage										m_ImageBomb;							//炸弹

	CSkinImage										m_ImageTimer1;							//定时器
	CSkinImage										m_ImageTimer2;							//

	CSkinImage										m_sImageChessboard;						//缩略棋盘
	CSkinImage										m_sImageChessman;						//缩略棋子

	CSkinImage										m_ImageMemA;							//团队信息
	CSkinImage										m_ImageMemB;							//

	CSkinImage										m_ImageBallImplement;
	CSkinImage										m_ImageBallImplementPick;

	CSkinImage										m_ImageLose;
	CSkinImage										m_ImageWin;

	CSkinImage										m_ImageRate;
	CSkinImage										m_ImageRate_B;

	CSkinImage										m_ImageDeadBall1;
	CSkinImage										m_ImageDeadBall2;

	CFont LittleFont;
	CFont BigFont;
	CFont MidFont;
	long int dwSpeed;
	long int dwBeginTime;
	long int dwEndTime;
public:
	void DrawBlog(CDC * pDC, int type, int x, int y);
};

#endif