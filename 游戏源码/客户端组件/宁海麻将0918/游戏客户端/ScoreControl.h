#ifndef SCORE_CONTROL_HEAD_FILE
#define SCORE_CONTROL_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "CardControl.h"

//////////////////////////////////////////////////////////////////////////

//积分信息
struct tagScoreInfo
{
	//胡牌扑克
	BYTE							cbCardCount;							//扑克数目
	BYTE							cbCardData[MAX_COUNT];					//扑克数据

	//用户信息
	WORD							wBankerUser;							//庄家用户
	TCHAR							szUserName[GAME_PLAYER][NAME_LEN];		//用户名字

	//积分信息
	WORD							wProvideUser;							//供应用户
	BYTE							cbProvideCard;							//供应扑克
	LONG							lGameScore[GAME_PLAYER];				//游戏积分

	//胡牌结果
	DWORD							dwChiHuKind[GAME_PLAYER];				//胡牌类型
	DWORD							dwChiHuRight[GAME_PLAYER];				//胡牌类型
};

//组合信息
struct tagWeaveInfo
{
	BYTE							cbWeaveCount;							//组合数目
	BYTE							cbCardCount[4];							//扑克数目
	BYTE							cbPublicWeave[4];						//公共组合
	BYTE							cbCardData[4][4];						//组合扑克
};

//////////////////////////////////////////////////////////////////////////

//积分控件
class CScoreControl : public CWnd
{
	//变量定义
protected:
	tagScoreInfo					m_ScoreInfo;							//积分信息

	//控件变量
protected:
	BYTE							m_cbWeaveCount;							//组合数目
	CWeaveCard						m_WeaveCard[4];							//组合扑克
	CSkinButton						m_btCloseScore;							//关闭按钮

	//资源变量
protected:
	CSkinImage						m_ImageWinLose;							//胜利失败
	CImage							m_ImageGameScore;						//积分视图
	CSkinImage						m_ImageGameScoreFlag;					//积分视图

	//函数定义
public:
	//构造函数
	CScoreControl();
	//析构函数
	virtual ~CScoreControl();

	//功能函数
public:
	//复位数据
	void RestorationData();
	//设置积分
	void SetScoreInfo(const tagScoreInfo & ScoreInfo, const tagWeaveInfo & WeaveInfo);

	//按钮消息
protected:
	//关闭按钮
	afx_msg void OnBnClickedClose();

	//消息映射
protected:
	//重画函数
	afx_msg void OnPaint();
	//绘画背景
	afx_msg BOOL OnEraseBkgnd(CDC * pDC);
	//创建函数
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint Point);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMove(int x, int y);
};

//////////////////////////////////////////////////////////////////////////

#endif