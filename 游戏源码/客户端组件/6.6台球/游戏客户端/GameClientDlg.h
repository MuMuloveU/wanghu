// GameClientDlg.h : 头文件
//

#pragma once
#ifndef GAME_DLG_HEAD_FILE
#define GAME_DLG_HEAD_FILE

#endif
#include "StdAfx.h"
#include "GameClientView.h"
#include "Ball.h"

//消息定义
#define IDM_BEGIN			WM_USER+120						//开始按钮消息
#define IDM_HIT_BALL		WM_USER+121						//击球消息
#define IDM_PLACE_BALL		WM_USER+122						//放球消息
#define IDM_HITGIVEUP			WM_USER+123						//放球消息
#define IDM_HITREPLACE			WM_USER+124						//放球消息
#define IDM_HITCONCESSION		WM_USER+125						//放球消息



//游戏定时器 ID
#define BALL_TIMER						100					//台球定时器
#define ID_BEGIN_TIME					200					//开始定时器
#define ID_HIT_BALL						201					//击球定时器
#define PLACE_WHITE_BALL				202					//放白球定时器

//定时器时间	
#define BEGIN_COUNT						40					//开始时间

// CGameClientDlg 对话框
class CGameClientDlg : public CGameFrameDlg
{
// 构造
public:

	//辅助数据
	BOOL						m_bCanHitBall;				//是否可以
	HANDLE						m_hTimerThreadHandle;		//线程句柄

	//控件
public:
	CAmericaDesk				m_Desk;
	CGameClientView				m_GameClientView;					//游戏视图
	small						m_NextHitBall;
	BOOL                        bAgreeConession;
	CGameClientDlg();	// 标准构造函数
	virtual ~CGameClientDlg();
private:
	void	InitData();
	virtual bool InitGameFrame();
	virtual void ResetGameFrame();
	virtual void OnGameOptionSet();

	virtual bool OnTimerMessage(WORD wChairID, 
								UINT nElapse, 
								UINT nTimerID);
											//时间消息
	virtual void OnLookonChanged(bool bLookonUser, 
								const void * pBuffer, 
								WORD wDataSize);
											//旁观状态
	virtual bool OnGameMessage(WORD wSubCmdID, 
								const void * pBuffer, 
								WORD wDataSize);
											//网络消息
	virtual bool OnGameSceneMessage(BYTE cbGameStatus, 
									bool bLookonOther, 
									const void * pBuffer, 
									WORD wDataSize);

	//游戏场景
protected:
	DECLARE_MESSAGE_MAP()

public:
	//游戏退出
	void GameQuit();
	//游戏开始
	void GameStart();
	//开始击球消息
	LRESULT	OnHitBall(WPARAM wparam, LPARAM lparam);
	//放球消息
	LRESULT	OnPlaceBall(WPARAM wparam, LPARAM lparam);
	//放球消息
	LRESULT	OnHitGiveUp(WPARAM wparam, LPARAM lparam);
	//开始击球消息
	LRESULT	OnHitReplace(WPARAM wparam, LPARAM lparam);
	//放球消息
	LRESULT	OnHitConession(WPARAM wparam, LPARAM lparam);
	LRESULT OnAgreePeace(WPARAM wParam, LPARAM lParam);	
	//开始按钮函数
	LRESULT	OnHitBegin(WPARAM wparam, LPARAM lparam);
	afx_msg void OnTimer(UINT nIDEvent);

	//消息函数
protected:
	//定时器线程
	static unsigned __stdcall TimerThread(LPVOID pThreadData);                                         //连续击球次数
};
