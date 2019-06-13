#ifndef STATION_HEAD_FILE
#define STATION_HEAD_FILE

#pragma once

#include "StdAfx.h"
#include "Resource.h"

#define STATION_WND_HIGHT			80			//状态窗口高度 
#define PIC_BALL_RADII				21		//图球半径
#define ID_TIMER					10			//定时器 

class CGameClientView;

//绘画信息
//状态控制类
class CStation : public CWnd
{
public:
	//变量定义
	BYTE				m_iTargetBall[2];		//目标球
	bool				m_bPowerTimer;			//是否启动定时器
	bool				m_bAddMode;				//是否增加模式
	int					m_iPower;				//力量百分比
	int					m_iPoint[2];			//得分
	double				m_iXBallPos;			//X 基准点
	double				m_iYBallPos;			//Y 基准点
	long int			m_iBaseMoney;			//基础金币
	BOOL                m_IsTrain;
	double				m_iXAnglePos;			//X 基准点
	double				m_iYAnglePos;			//Y 基准点
	int					iPos;
	//控件变量
public:
	HCURSOR				m_Cursor;				//光标
	HCURSOR				m_NormalCursor;			//光标
	//CGameClientView		* m_pPlayView;			//视图指针

	//控件变量
public:
	
	//函数定义
public:
	//构造函数
	CStation();
	//CGameClientView * pPlayView
	//设置击球点
	void SetBallPos(double iXBallPos, double iYBallPos,bool bIsAngle=false);
	//获取击球点 X
	inline double GetBallXPos() { return m_iYBallPos/PIC_BALL_RADII; };
	//获取击球点 Y
	inline double GetBallYPos() { return m_iXBallPos/PIC_BALL_RADII; };
	//设置力量
	void SetPower(int iPower);
	//获取力量
	inline int GetPower() { return __max(0,__min(m_iPower,100)); };
	//开始力量
	void BeginPower();
	//停止力量
	void EndPower();
	//设置分数
	void SetUserPoint(int iViewStation, int Point, bool bAdd);
	//设置目标球
	void SetTargertBall(int iViewStation, BYTE bColor);
	//绘画图片
	void DrawBmp(CDC * pDC, int x, int y, UINT uBmpID, UINT uType, CRect * pRect, int xSrc=0, int ySrc=0, int iWidth=0, COLORREF rcTranColor=RGB(0,0,0));

	//消息函数
protected:
	//重画函数
	afx_msg void OnPaint();
	//光标消息
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//鼠标右键消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//定时器消息
	afx_msg void OnTimer(UINT nIDEvent);

	DECLARE_MESSAGE_MAP()

};

#endif