#pragma once

#include "Stdafx.h"
#include "ControlWnd.h"
#include "CardControl.h"
#include "ScoreControl.h"
#include "DrawSiceWnd.h"
#include "DrawSiceWnd2.h"

//////////////////////////////////////////////////////////////////////////
//消息定义

#define IDM_START					(WM_USER+100)						//开始消息
#define IDM_OUT_CARD				(WM_USER+101)						//出牌消息
#define IDM_BATCH					(WM_USER+102)						//买底消息	
#define IDM_TRUSTEE_CONTROL			(WM_USER+103)						//托管控制
#define IDM_BATCH_END				(WM_USER+104)						//洗澡结束
#define IDI_DISC_EFFECT					103								//丢弃效果

//////////////////////////////////////////////////////////////////////////

//游戏视图
class CGameClientView : public CGameFrameView
{
	//标志变量
protected:
	bool							m_bOutCard;							//出牌标志
	bool							m_bWaitOther;						//等待标志
	bool							m_bHuangZhuang;						//荒庄标志
	bool							m_bTrustee[GAME_PLAYER];			//是否托管
	bool							m_bShowScore;						//积分状态

	//游戏属性
protected:
	LONG							m_lCellScore;						//基础积分
	WORD							m_wBankerUser;						//庄家用户
	WORD							m_wCurrentUser;						//当前用户
	BYTE							m_cbBatch[GAME_PLAYER];				//洗澡结果

	//历史积分
public:
	LONG                            m_lAllTurnScore[GAME_PLAYER];		//总局得分
	LONG                            m_lLastTurnScore[GAME_PLAYER];		//上局得分

	//上下番信息
	BYTE							m_cbUpDownKingCardData;				//上下番
	LONG							m_lUpDownScore[GAME_PLAYER];		//上下番
	BYTE							m_cbFloor;							//楼层		

	//动作动画
protected:
	bool							m_bBombEffect;						//动作效果
	BYTE							m_cbBombFrameIndex;					//帧数索引

	//洗澡效果
	bool							m_bBatchEffect;						//动作效果
	BYTE							m_cbBatchFrameIndex;				//帧数索引

	//丢弃效果
	WORD							m_wDiscUser;						//丢弃用户
	BYTE							m_cbDiscFrameIndex;					//帧数索引	

	//用户状态
protected:
	BYTE							m_cbCardData;						//出牌扑克
	WORD							m_wOutCardUser;						//出牌用户
	WORD							m_wUserAction[4];					//用户动作
	BYTE							m_cbKingCardIndex[2];				//王牌索引

	//位置变量
protected:
	CPoint							m_UserFlagPos[4];					//标志位置
	CPoint							m_PointTrustee[GAME_PLAYER];		//托管位置

	//位图变量
protected:
	CSkinImage						m_ImageBack;						//背景图案
	CSkinImage						m_ImageCenter;						//LOGO图
	CSkinImage						m_ImageLeft;						//倍数图片
	CSkinImage						m_ImageWait;						//等待提示
	CSkinImage						m_ImageOutCard;						//出牌提示
	CSkinImage						m_ImageUserFlag;					//用户标志
	CSkinImage						m_ImageUserAction;					//用户动作
	CSkinImage						m_ImageActionBack;					//动作背景
	CSkinImage						m_ImageHuangZhuang;					//荒庄标志
	CPngImage						m_ImageTrustee;						//托管标志	
	CPngImage						m_PngBatch[4];						//自己洗澡

	CPngImage						m_ImageActionAni;					//吃牌动画资源
	CPngImage						m_ImageDisc;						//丢弃效果
	CPngImage						m_ImageArrow;						//定时器箭头	
	CSkinImage						m_ImageHistoryScore;				//积分位图

	//图片大小
private:
	CSize							m_sizeHistory;						//积分大小

	//视频组件
private:
	//CVideoServiceControl 			m_DlgVedioService[4];				//视频窗口

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
	CControlWnd						m_ControlWnd;						//控制窗口
	CScoreControl					m_GameScoreWnd;						//结束窗口
	CDrawSiceWnd                    m_DrawSiceWnd;                      //筛子窗口
	CDrawSiceWnd2					m_DrawSiceWnd2;						//筛子窗口
	CSkinButton						m_btStusteeControl;					//拖管控制
	CSkinButton						m_btScore;							//看分按钮
	CSkinButton						m_btScoreClose;						//关闭成绩
	CSkinButton						m_btBatch;							//洗澡按钮


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

	//功能函数
public:
	//基础积分
	void SetCellScore(LONG lCellScore);
	//庄家用户
	void SetBankerUser(WORD wBankerUser);
	//荒庄设置
	void SetHuangZhuang(bool bHuangZhuang);
	//状态标志
	void SetStatusFlag(bool bOutCard, bool bWaitOther);
	//出牌信息
	void SetOutCardInfo(WORD wViewChairID, BYTE cbCardData);
	//动作信息
	void SetUserAction(WORD wViewChairID, WORD wUserAction);
	//设置王牌
	void SetKingCardIndex(BYTE cbKingCardIndex[2]);
	//设置动作
	bool SetBombEffect(bool bBombEffect);
	//设置洗澡
	bool SetBatchEffect(bool bBatchEffect);
	//丢弃用户
	void SetDiscUser(WORD wDiscUser);
	//定时玩家
	void SetCurrentUser(WORD wCurrentUser);
	//设置托管
	void SetTrustee(WORD wTrusteeUser,bool bTrustee);
	//洗澡结果
	void SetBatchResult(BYTE cbBatchResult[4]);

	//辅助函数
protected:
	//艺术字体
	void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos);

	//消息映射
protected:
	//开始按钮
	afx_msg void OnStart();
	//洗澡按钮
	afx_msg void OnBatch();
	 //拖管控制
	 afx_msg void OnStusteeControl();
	//建立函数
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//积分按钮
	afx_msg void OnBnClickedScore();
	//积分按钮
	afx_msg void OnBnClickedCloseScore();
	//鼠标消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint Point);
	//光标消息
	afx_msg BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);
	//定时器消息
	afx_msg void OnTimer(UINT nIDEvent);
	DECLARE_MESSAGE_MAP()

};

//////////////////////////////////////////////////////////////////////////
