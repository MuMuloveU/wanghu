#ifndef CALL_SCORE_HEAD_FILE
#define CALL_SCORE_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "Resource.h"

#define IDM_LAND_SCORE				(WM_USER+104)						//叫分消息
#define IDM_LAND_MULTIPLES			(WM_USER+105)						//加倍消息

//////////////////////////////////////////////////////////////////////////

//积分视图类
class CCallScore : public CWnd
{
	//梅花
	CSkinButton                    m_CallC1;             
	CSkinButton                    m_CallC2;            
	CSkinButton                    m_CallC3;             
	CSkinButton                    m_CallC4;            
	CSkinButton                    m_CallC5;            
	CSkinButton                    m_CallC6;            
	CSkinButton                    m_CallC7;           

	//方块
	CSkinButton                    m_CallD1;             
	CSkinButton                    m_CallD2;            
	CSkinButton                    m_CallD3;             
	CSkinButton                    m_CallD4;            
	CSkinButton                    m_CallD5;            
	CSkinButton                    m_CallD6;            
	CSkinButton                    m_CallD7;           

	//红桃
	CSkinButton                    m_CallH1;             
	CSkinButton                    m_CallH2;            
	CSkinButton                    m_CallH3;             
	CSkinButton                    m_CallH4;            
	CSkinButton                    m_CallH5;            
	CSkinButton                    m_CallH6;            
	CSkinButton                    m_CallH7;           

	//黑桃
	CSkinButton                    m_CallS1;             
	CSkinButton                    m_CallS2;            
	CSkinButton                    m_CallS3;             
	CSkinButton                    m_CallS4;            
	CSkinButton                    m_CallS5;            
	CSkinButton                    m_CallS6;            
	CSkinButton                    m_CallS7;           

	//NT
	CSkinButton                    m_CallN1;             
	CSkinButton                    m_CallN2;            
	CSkinButton                    m_CallN3;             
	CSkinButton                    m_CallN4;            
	CSkinButton                    m_CallN5;            
	CSkinButton                    m_CallN6;            
	CSkinButton                    m_CallN7;           

	//倍数
	CSkinButton                    m_CallB1;             
	CSkinButton                    m_CallB2;            
	
    //放弃
	CSkinButton					   m_btGiveUpScore;		 

	CSkinImage					   m_ImageScoreBack;			  //背景图案

	//叫分信息
public:
	BYTE						   m_cbLandScore;				//地主叫分
	BYTE						   m_cbLandColor;				//地主花色
	BYTE						   m_cbMultiples;				//游戏倍数
	bool						   m_bDbl;						//加倍
	bool						   m_bReDbl;					//再加倍
	//函数定义
public:
	//构造函数
	CCallScore();

	//重画界面
	void ReloadView();

	//叫牌比较
	bool CompareCallScore(BYTE bColor,BYTE bValue);

	//析构函数
	virtual ~CCallScore();

	
	//消息函数
protected:
	//重画函数
	afx_msg void OnPaint();
	//建立消息
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	//叫分OK按钮
	afx_msg void OnCallC1();
	afx_msg void OnCallC2();
	afx_msg void OnCallC3();
	afx_msg void OnCallC4();
	afx_msg void OnCallC5();
	afx_msg void OnCallC6();
	afx_msg void OnCallC7();

    afx_msg void OnCallD1();
	afx_msg void OnCallD2();
	afx_msg void OnCallD3();
	afx_msg void OnCallD4();
	afx_msg void OnCallD5();
	afx_msg void OnCallD6();
	afx_msg void OnCallD7();

	afx_msg void OnCallH1();
	afx_msg void OnCallH2();
	afx_msg void OnCallH3();
	afx_msg void OnCallH4();
	afx_msg void OnCallH5();
	afx_msg void OnCallH6();
	afx_msg void OnCallH7();

	afx_msg void OnCallS1();
	afx_msg void OnCallS2();
	afx_msg void OnCallS3();
	afx_msg void OnCallS4();
	afx_msg void OnCallS5();
	afx_msg void OnCallS6();
	afx_msg void OnCallS7();

	afx_msg void OnCallN1();
	afx_msg void OnCallN2();
	afx_msg void OnCallN3();
	afx_msg void OnCallN4();
	afx_msg void OnCallN5();
	afx_msg void OnCallN6();
	afx_msg void OnCallN7();

	afx_msg void OnCallB1();
	afx_msg void OnCallB2();
	
	//放弃按钮
	afx_msg void OnGiveUpScore();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif