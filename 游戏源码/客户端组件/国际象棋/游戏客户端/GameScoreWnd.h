#include "Stdafx.h"
#include "Resource.h"

#pragma once

//////////////////////////////////////////////////////////////////////////
//结构定义

//积分信息
struct tagGameScoreInfo
{
	LONG							lScore[GAME_PLAYER];				//用户积分
	TCHAR							szUserName[GAME_PLAYER][NAME_LEN];	//用户帐号
};

//////////////////////////////////////////////////////////////////////////

//积分报告
class CGameScoreWnd : public CWnd
{
	//积分信息
protected:
	tagGameScoreInfo				m_GameScoreInfo;					//积分信息

	//辅助信息
protected:
	CSize							m_ScoreSize;						//窗口大小
	CSkinImage						m_ImageBack;						//背景资源

	//函数定义
public:
	//构造函数
	CGameScoreWnd();
	//析构函数
	virtual ~CGameScoreWnd();

	//功能函数
public:
	//设置积分
	void SetScoreInfo(tagGameScoreInfo & GameScoreInfo);

	//信息函数
public:
	//获取大小
	const CSize & GetGameScoreSize() { return m_ScoreSize; }

	//消息映射
protected:
	//重画函数
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
