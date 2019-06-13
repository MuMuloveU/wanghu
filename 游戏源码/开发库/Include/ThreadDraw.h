#ifndef THREAD_DRAW_HEAD_FILE
#define THREAD_DRAW_HEAD_FILE

#pragma once

#include "GameFrame.h"

//宏定义
#define THREAD_TIME_ID					1								//定时器 ID

//////////////////////////////////////////////////////////////////////////

//绘画线程模块
class GAME_FRAME_CLASS CThreadDraw
{
	//线程变量
private:
	bool								m_bExit;						//退出标志
	UINT								m_uSpeed;						//显示帧数
	UINT								m_uTimeSpace;					//间隔时间
	UINT								m_uDrawSpace;					//间隔时间
	HANDLE								m_hDrawThreadHandle;			//线程句柄
	HANDLE								m_hTimeThreadHandle;			//线程句柄

	//状态信息
protected:
	int									m_nClientWidth;					//窗口宽度
	int									m_nClientHeight;				//窗口高度
	CWnd								* const m_pGameView;			//游戏视图

	//函数定义
public:
	//构造函数
	CThreadDraw(CWnd * pGameView);
	//析构函数
	virtual ~CThreadDraw();

	//重载函数
public:
	//初始化绘画
	virtual bool InitDraw(CDC * pDrawDC);
	//销毁绘画
	virtual bool UnInitDraw();
	//绘画函数
	virtual void ThreadDrawView(CDC * pDC, int nClinetWith, int nClinetHeight)=NULL;

	//功能函数
public:
	//获取帧数
	UINT GetSpeed() { return m_uSpeed; }
	//设置范围
	void SetClientRange(int nClientWidth, int nClientHeight);
	//设置定时器间隔
	void SetTimeSpace(UINT uTimeSpace) { m_uTimeSpace=uTimeSpace; };
	//设置 GDI 线程间隔
	void SetDrawSpace(UINT uDrawSpace) { m_uDrawSpace=uDrawSpace; };

	//线程函数
public:
	//开始线程
	bool BeginThread();
	//结束线程
	bool EndThread();
	//停止线程
	void SuspendDrawThread();
	//启动线程
	void ResumeDrawThread();
	//画图线程
	static unsigned __stdcall ThreadDraw(LPVOID pThreadData);
	//定时器线程
	static unsigned __stdcall ThreadTimer(LPVOID pThreadData);
};

//////////////////////////////////////////////////////////////////////////

#endif