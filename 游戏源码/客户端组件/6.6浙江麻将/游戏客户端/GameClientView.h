#pragma once

#include "Stdafx.h"
#include "ControlWnd.h"
#include "CardControl.h"
#include "GameScoreWnd.h"
#include "IDataCtrl.h"

//////////////////////////////////////////////////////////////////////////
//消息定义

#define IDM_START					(WM_USER+100)						//开始消息
#define IDM_OUT_CARD				(WM_USER+101)						//出牌消息
#define IDM_CONSIGN_CHANGED			(WM_USER+102)						//托管消息

//////////////////////////////////////////////////////////////////////////

//游戏视图
class CGameClientView : public CGameFrameView
{
	//友元定义
	friend class CGameClientDlg;

	//游戏属性
protected:
	IDataCtrl*						m_pDataCtrl;						//数据控制
	bool							m_bOutCard;							//出牌标志
	bool							m_bWaitOther;						//等待标志
	WORD							m_wBankerUser;						//庄家用户

	//用户状态
protected:
	BYTE							m_cbCardData;						//出牌扑克
	WORD							m_wOutCardUser;						//出牌用户
	BYTE							m_bUserAction[4];					//用户动作

	//位置变量
protected:
	CPoint							m_UserFlagPos[4];					//标志位置

	//位图变量
protected:
	CSkinImage						m_ImageBack;						//背景图案
	CSkinImage						m_ImageWait;						//等待提示
	CSkinImage						m_ImageOutCard;						//出牌提示
	CSkinImage						m_ImageUserFlag;					//用户标志
	CSkinImage						m_ImageUserAction;					//用户动作
	CSkinImage						m_ImageActionBack;					//动作背景
	CSkinImage						m_ImageMoneyLeft;					//剩余财富
	CSkinImage						m_ImageTipInfo;						//提示信息

	//扑克控件
public:
	CHeapCard						m_HeapCard[4];						//堆立扑克
	CUserCard						m_UserCard[3];						//用户扑克
	CTableCard						m_TableCard[4];						//桌面扑克
	CWeaveCard						m_WeaveCard[4][4];					//组合扑克
	CDiscardCard					m_DiscardCard[4];					//丢弃扑克
	CCardControl					m_HandCardControl;					//手上扑克

	//控件变量
public:
	CSkinButton						m_btStart;							//开始按钮
	CSkinButton						m_btConsignEnter;					//托管按钮
	CSkinButton						m_btConsignCancel;					//取消托管
	CControlWnd						m_ControlWnd;						//控制窗口
	CGameScoreWnd					m_GameScoreWnd;						//结束窗口

	//函数定义
public:
	//构造函数
	CGameClientView(IDataCtrl* pDataCtrl);
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

	//功能函数
public:
	//庄家用户
	void SetBankerUser(WORD wBankerUser);
	//状态标志
	void SetStatusFlag(bool bOutCard, bool bWaitOther);
	//出牌信息
	void SetOutCardInfo(WORD wViewChairID, BYTE cbCardData);
	//动作信息
	void SetUserAction(WORD wViewChairID, BYTE bUserAction);
	//托管信息
	void SetConsign(bool bEnter);

	//消息映射
protected:
	//开始按钮
	afx_msg void OnStart();
	//托管按钮
	afx_msg void OnConsignEnter();
	//取消托管
	afx_msg void OnConsignCancel();
	//建立函数
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint Point);
	//光标消息
	afx_msg BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);

	DECLARE_MESSAGE_MAP()

private:
	//画剩余财富
	void	_DrawScoreLeft(CDC * pDC, int nWidth, int nHeight);
	//画提示信息
	void    _DrawTipInfo(CDC * pDC, int nWidth, int nHeight);
	//画选装文字
	CRect   _DrawTitleInRect(CDC *pDC, CString szString, LPRECT lpRect, long lMode = 1, long lHori = 1, long lVert = 1);
};

//////////////////////////////////////////////////////////////////////////
