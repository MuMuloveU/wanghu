#ifndef THREADDRAW_HEAD_FILE
#define THREADDRAW_HEAD_FILE

#pragma once

#include "StdAfx.h"
#include "Ball.h"
#include "Station.h"

struct DrawInfoStruct
{

	//公共访问
	double						iPower;					//力度
	bool						bOkSet;					//是否允许放置
	BYTE						bStation;				//状态
	POINT						CursorPos;				//光标位置
	RECT						ClientRect;				//客户端位置
	CBall						* pBallList;			//球数组
	POINT						OtherPolePos;			//其他玩家坐标

	//一次性初始化数据 
	double						dwTimes;				//图形拉伸系数
	CBallDesk					* pDesk;				//球桌指针
	CGameClientView				* pPlayView;			//视图指针

	//线程使用部分
	long int					dwSpeed;				//显示帧数
	CFont						BigFont;				//大字体
	CFont						LittleFont;				//小字体
	BITMAP						BackBmpInfo;			//底图信息
	BYTE                        m_StarPos;              //特别奖励（星星的位置）
	BYTE                        m_StarPoint;             //奖励分数
	int 					    Degree;					//角度
	double                      m_iPowerPos;
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
	void GDIModeDraw(CDC * pDC);
	void GDIModeInitThread();
private:
	CSkinImage    m_DeskImage;
	CSkinImage	  m_BallImage;
	CSkinImage	  m_BallAImage;
	CSkinImage	  m_BallInBagImage;
	CSkinImage	  m_PoleImage;
	CSkinImage    m_BagImage;
	CSkinImage    m_AppendImage;
	CSkinImage	  m_BackImage;
	BYTE    m_StarFlash;          //控制星星的闪烁
	double						m_iPower;					//力度
	BITMAP						BallBmpInfo;			//球图信息
	BITMAP						BallABmpInfo;			//球图信息
	BITMAP						BallInBagBmpInfo;			//球图信息
	BITMAP						BagBmpInfo;				//袋图信息
	BITMAP						PoleBmpInfo;			//棍图信息
};

#endif