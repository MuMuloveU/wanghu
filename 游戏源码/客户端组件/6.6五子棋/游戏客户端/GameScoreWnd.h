#pragma once

#include "Stdafx.h"

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
class CGameScoreWnd : public CDialog
{
	//积分信息
protected:
	tagGameScoreInfo				m_GameScoreInfo;					//积分信息

	//辅助信息
protected:
	CSkinImage						m_ImageBack;						//背景资源
	CSkinButton						m_btClose;							//关闭按钮

	//函数定义
public:
	//构造函数
	CGameScoreWnd();
	//析构函数
	virtual ~CGameScoreWnd();

	//重载函数
public:
	//控件绑定
	virtual void DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定消息
	virtual void OnOK() { return; }
	//取消消息
	virtual void OnCancel() { return; }

	//功能函数
public:
	//设置积分
	void SetScoreInfo(tagGameScoreInfo & GameScoreInfo);

	//消息映射
protected:
	//关闭按钮
	afx_msg void OnClose();
	//重画函数
	afx_msg void OnPaint();
	//绘画背景
	afx_msg BOOL OnEraseBkgnd(CDC * pDC);
	//鼠标消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
