#ifndef GAME_VIEW_HEAD_FILE
#define GAME_VIEW_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GlobalOption.h"
#include "MyListView.h"
#include "D3DWnd.h"
#include "GameOption.h"

//////////////////////////////////////////////////////////////////////////

//游戏视图
class CGameClientView : public CGameFrameView
{
	//友元定义
	friend class CGameClientDlg;
	friend class CGameThreadDraw;

	//界面变量
protected:
	CD3DWnd			m_wndD3D;								//direct9.0窗口
	CDC				m_MemDC;								//背景图片缓冲


	HCURSOR m_hButtonCursor;

public:
	//user info myself
	tagUserData *    m_UserID[4];

	//控件变量
public:
	CMyListView     m_MyListView;							//控件视图
	CLogic			m_Logic;							   //游戏逻辑类
	CGameOption		m_GameOption;

	//函数定义
public:
	//构造函数
	CGameClientView();
	//析构函数
	virtual ~CGameClientView();

	//继承函数
private:
	//重置界面
	virtual void ResetGameView();
	//调整控件
	virtual void RectifyGameView(int nWidth, int nHeight);
	//绘画界面
	virtual void DrawGameView(CDC * pDC, int nWidth, int nHeight);
	//消息映射
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()


public:

	// 连接消息到达
	LRESULT OnLink(WPARAM wParam, LPARAM lParam);
	// 连接处理
	void LinkPro(CPoint first, CPoint end, WORD userid);
	// 其他玩家连接处理
	void OtherPlayLinkPro(CPoint first, CPoint end, WORD userid);

	//消息
	LRESULT OnStart(WPARAM wParam, LPARAM lParam);						// 开始
	LRESULT OnExecise(WPARAM wParam, LPARAM lParam);					// 练习
	LRESULT OnQuit(WPARAM wParam, LPARAM lParam);						// 结束
	//道具消息
	LRESULT OnGuide(WPARAM wParam, LPARAM lParam);						// 指南针
	LRESULT OnRealign(WPARAM wParam, LPARAM lParam);					// 重列
	LRESULT OnBomb(WPARAM wParam, LPARAM lParam);						// 炸弹
	LRESULT OnStopIt(WPARAM wParam, LPARAM lParam);						// 禁手
	LRESULT OnClog(WPARAM wParam, LPARAM lParam);						// 障碍

	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnTimer(UINT nIDEvent);

	//
	LRESULT OnChatColor(WPARAM wParam, LPARAM lParam);
	
	// 设置用户信息
	void MySetUserInfo(WORD wChairID, tagUserData * pUserData);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};
#endif
//////////////////////////////////////////////////////////////////////////
