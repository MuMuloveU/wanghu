#ifndef CALL_SCORE_INFO_HEAD_FILE
#define CALL_SCORE_INFO_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "Resource.h"


//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////

//积分视图类
class CCallScoreInfo : public CWnd
{
	//积分信息
protected:
	LONG							m_lScore[GAME_PLAYER][60];			//游戏叫分
	LONG							m_lColor[GAME_PLAYER][60];			//游戏花色
	TCHAR							m_szUserName[GAME_PLAYER][NAME_LEN];//用户名字

	//资源变量
protected:
	CSkinImage						m_ImageBack;						//背景图案
	CSkinImage						m_ImageColorFlag;					//花色标志

	//函数定义
public:
	//构造函数
	CCallScoreInfo();
	//析构函数
	virtual ~CCallScoreInfo();
	
	//功能函数
public:
	
	//设置积分
	void SetCallGameInfo(WORD wChairID, BYTE bColor,BYTE bScore);

	void SetCallUserName(WORD wChairID,LPCTSTR pszUserName);

	//消息函数
protected:
	//重画函数
	afx_msg void OnPaint();
	//建立消息
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint Point);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif