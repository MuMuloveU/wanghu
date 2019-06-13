#pragma once

#include "Stdafx.h"
#include "ScoreView.h"
#include "CardControl.h"


//////////////////////////////////////////////////////////////////////////
//消息定义

#define IDM_START					(WM_USER+100)						//开始消息
#define IDM_OUT_CARD				(WM_USER+101)						//出牌消息
#define IDM_PASS_CARD				(WM_USER+102)						//放弃出牌
#define IDM_LAND_SCORE				(WM_USER+103)						//叫分消息
#define IDM_AUTO_OUTCARD			(WM_USER+104)                       //提示消息

#define IDM_SORT_BY_NUM				(WM_USER+106)						//按张数排序
#define IDM_SORT_BY_SIZE			(WM_USER+107)						//按大小排序

//////////////////////////////////////////////////////////////////////////

//游戏视图
class CGameClientView : public CGameFrameView
{
	//游戏变量
protected:
	WORD							m_wLandUser;						//地主用户
	WORD							m_wBombTime;						//炸弹倍数
	LONG							m_lBaseScore;						//基础分数
	BYTE							m_cbLandScore;						//地主分数

	//配置变量
protected:
	bool							m_bDeasilOrder;						//出牌顺序

	//状态变量
public:
	bool							m_bLandTitle;						//地主标志
	bool							m_bPass[GAME_PLAYER];				//放弃数组
	BYTE							m_bScore[GAME_PLAYER];				//用户叫分
	BYTE							m_bCardCount[GAME_PLAYER];			//扑克数目
	BYTE							m_bMaxBombCount[GAME_PLAYER];		//最大出牌数
	BYTE							m_bOutBombCount[4];					//出的炸弹数
	bool							m_bMaxBombCountShow;				//可显示的出牌数

	//位置信息
public:
	CSize							m_LandSize;							//挖坑标志
	CPoint							m_ptScore[GAME_PLAYER];				//叫分位置
	CPoint							m_ptLand[GAME_PLAYER];				//反牌位置
	CPoint							m_ptBomb[GAME_PLAYER];				//炸弹位置

	//位图变量
protected:
	CSkinImage						m_ImageBomb;						//炸弹图片
	CSkinImage						m_ImageBack;						//背景资源
	CSkinImage						m_ImageFill;
	//按钮控件
public:
	CSkinButton						m_btStart;							//开始按钮
	CSkinButton						m_btOutCard;						//出牌按钮
	CSkinButton						m_btPassCard;						//放弃按钮
	CSkinButton						m_btOneScore;						//1 分按钮
	CSkinButton						m_btTwoScore;						//2 分按钮
	CSkinButton						m_btThreeScore;						//3 分按钮
	CSkinButton						m_btShowCard;						//明牌按钮
	CSkinButton						m_btGiveUpScore;					//放弃按钮

	CSkinButton						m_btSortByNum;						//按张数排列按钮
	CSkinButton						m_btSortBySize;						//按大小排列按钮

	//扑克控件
public:
	CCardControl					m_BackCardControl;					//底牌扑克
	CCardControl					m_UserCardControl[4];				//扑克视图
	CCardControl					m_HandCardControl[4];				//手上扑克

	//控件组件
public:
	CScoreView						m_ScoreView;						//积分视图

#ifdef VIDEO_GAME
	CVideoServiceControl			m_DlgVideoService[4];				//视频窗口
#endif

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

#ifdef VIDEO_GAME
	void RectifyVideoView(int nWidth, int nHeight);
#endif

	//配置函数
public:
	//用户顺序
	void SetUserOrder(bool bDeasilOrder);
	//获取顺序
	bool IsDeasilOrder() { return m_bDeasilOrder; }

	//功能函数
public:
	//炸弹倍数
	void SetBombTime(WORD wBombTime);
	//基础分数
	void SetBaseScore(LONG lBaseScore);
	//显示提示
	void ShowLandTitle(bool bLandTitle);
	//设置放弃
	void SetPassFlag(WORD wChairID, bool bPass);
	//扑克数目
	void SetCardCount(WORD wChairID, BYTE bCardCount);
	//挖坑分数
	void SetLandUser(WORD wChairID, BYTE bLandScore);
	//挖坑分数
	void SetLandScore(WORD wChairID, BYTE bLandScore);

	//消息映射
protected:
	//开始按钮
	afx_msg void OnStart();
	//出牌按钮
	afx_msg void OnOutCard();
	//放弃按钮
	afx_msg void OnPassCard();
	//叫分按钮
	afx_msg void OnOneScore();
	//叫分按钮
	afx_msg void OnTwoScore();
	//叫分按钮
	afx_msg void OnThreeScore();
	//叫分按钮
	afx_msg void OnShowCard();
	//放弃按钮
	afx_msg void OnGiveUpScore();
	//建立函数
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标消息
	afx_msg void OnRButtonUp(UINT nFlags, CPoint Point);
	//鼠标双击
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//按张数排序按钮
	afx_msg void OnSortByNum();
	//按大小排序按钮
	afx_msg void OnSortBySize();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
