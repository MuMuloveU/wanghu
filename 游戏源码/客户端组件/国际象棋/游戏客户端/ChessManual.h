#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "ChessBorad.h"

//////////////////////////////////////////////////////////////////////////

#define VER_MANUAL					1									//棋谱版本

//文件头信息
struct tagChessManualHead
{
	WORD							wVersion;							//版本号码
	BYTE							cbChessColor;						//棋子颜色
	DWORD							dwManualDate;						//棋谱时间
	DWORD							dwManualCount;						//棋谱数目
};

//////////////////////////////////////////////////////////////////////////

//棋谱窗口
class CChessManual : public CDialog
{
	//棋谱变量
public:
	WORD							m_wChessStep;						//走棋步数
	CArrayTemplate<tagChessManual>	m_ChessManualInfo;					//棋谱信息

	//控件变量
public:
	CGameLogic						m_GameLogic;						//游戏逻辑
	CGameLogic *					m_pDlgGameLogic;					//游戏逻辑

	//控件变量
public:
	CSkinButton						m_btOpen;							//按钮控件
	CSkinButton						m_btPreserve;						//按钮控件
	CSkinButton						m_btReLoad;							//按钮控件
	CSkinButton						m_btFirst;							//按钮控件
	CSkinButton						m_btBefore;							//按钮控件
	CSkinButton						m_btNext;							//按钮控件
	CSkinButton						m_btLast;							//按钮控件
	CSkinButton						m_btCancel;							//按钮控件
	CListBox						m_ManualList;						//棋谱列表
	CChessBorad						m_ChessBorad;						//游戏棋盘

	//资源变量
protected:
	CSkinImage						m_ImageBack;						//底图资源

	//函数定义
public:
	//构造函数
	CChessManual();
	//析构函数
	virtual ~CChessManual();

	//重载函数
protected:
	//控件绑定
	virtual void DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定消息
	virtual void OnOK() { return; }
	//取消消息
	virtual void OnCancel() { DestroyWindow(); }

	//辅助函数
protected:
	//更新控制
	void UpdateControl();
	//更新棋谱
	void UpdateManualList();

	//消息函数
protected:
	//重画函数
	afx_msg void OnPaint();
	//绘画背景
	afx_msg BOOL OnEraseBkgnd(CDC * pDC);
	//选择改变
	afx_msg void OnLbnSelchangeManualList();
	//鼠标消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	//按钮消息
protected:
	//打开按钮
	afx_msg void OnOpen();
	//保存按钮
	afx_msg void OnPreserve();
	//刷新按钮
	afx_msg void OnReLoad();
	//最前按钮
	afx_msg void OnFirst();
	//上手按钮
	afx_msg void OnBefore();
	//下手按钮
	afx_msg void OnNext();
	//最后按钮
	afx_msg void OnLast();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
