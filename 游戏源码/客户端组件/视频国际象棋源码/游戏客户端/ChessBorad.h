#ifndef CHESS_BORAD_HEAD_FILE
#define CHESS_BORAD_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

//消息定义
#define WM_HIT_CHESS_BORAD			(WM_USER+200)						//点击棋盘

//////////////////////////////////////////////////////////////////////////

//游戏棋盘
class CChessBorad : public CWnd
{
	//配置变量
protected:
	BYTE							m_cbUserColor;						//用户颜色
	bool							m_bPositively;						//响应标志
	bool							m_bShowChessName;					//显示名字

	//辅助变量
protected:
	BYTE							m_cbXPosStart;						//位置坐标
	BYTE							m_cbYPosStart;						//位置坐标
	BYTE							m_cbXPosSelect;						//位置坐标
	BYTE							m_cbYPosSelect;						//位置坐标
	BYTE							m_cbXPosTerminal;					//位置坐标
	BYTE							m_cbYPosTerminal;					//位置坐标

	//状态变量
protected:
	tagChessItem					* m_ChessBorad[8][8];				//棋盘信息

	//资源变量
protected:
	CSize							m_BoradSize;						//棋盘大小
	CSkinImage						m_ImageChess;						//棋子资源
	CSkinImage						m_ImageBorad;						//棋盘资源
	CSkinImage						m_ImageFrame;						//棋子框架

	//函数定义
public:
	//构造函数
	CChessBorad();
	//析构函数
	virtual ~CChessBorad();

	//棋子控制
public:
	//清除棋子
	void CleanChess();
	//选择棋子
	void SetSelectChess(BYTE cbXPos, BYTE cbYPos);
	//设置棋盘
	void SetChessBorad(tagChessItem * ChessBorad[8][8]);

	//状态控制
public:
	//设置颜色
	void SetChessColor(BYTE cbUserColor);
	//设置响应
	void SetPositively(bool bPositively);
	//显示名字
	void ShowChessName(bool bShowChessName);
	//设置棋框
	void SetChessFrame(BYTE cbXPosStart, BYTE cbYPosStart, BYTE cbXPosTerminal, BYTE cbYPosTerminal);

	//状态查询
public:
	//获取大小
	const CSize & GetChessBoradSize() { return m_BoradSize; }

	//辅助函数
private:
	//获取坐标
	int GetChessXPos(BYTE cbXPos);
	//获取坐标
	int GetChessYPos(BYTE cbYPos);
	//更新棋子
	void UpdateBoradPos(BYTE cbXPos, BYTE cbYPos);
	//转换坐标
	void SwitchMousePos(const CPoint & MousePoint, BYTE & cbXPos, BYTE & cbYPos);

	//消息映射
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