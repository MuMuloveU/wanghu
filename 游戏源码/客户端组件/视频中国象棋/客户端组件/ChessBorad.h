#ifndef CHESS_BORAD_HEAD_FILE
#define CHESS_BORAD_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

//常量定义
#define INVALID_POS					0xFF								//无效索引

//消息定义
#define WM_HIT_CHESS_BORAD			(WM_USER+500)						//点击棋盘

//////////////////////////////////////////////////////////////////////////

//棋盘界面
class CChessBorad
{
	//状态变量
protected:
	BYTE							m_cbUserColor;						//用户颜色
	bool							m_bManualView;						//历史视图
	bool							m_bPositively;						//响应标志
	bool							m_bWaitGameRule;					//等待规则

	//辅助变量
protected:
	BYTE							m_cbXPosStart;						//位置坐标
	BYTE							m_cbYPosStart;						//位置坐标
	BYTE							m_cbXPosSelect;						//位置坐标
	BYTE							m_cbYPosSelect;						//位置坐标
	BYTE							m_cbXPosTerminal;					//位置坐标
	BYTE							m_cbYPosTerminal;					//位置坐标

	//棋子变量
protected:
	tagChessItem *					m_ChessItemPtr[COUNT_X][COUNT_Y];	//棋盘信息

	//位置变量
protected:
	INT								m_XExcursion;						//偏移变量
	INT								m_YExcursion;						//偏移变量
	INT								m_XBoradSpace;						//间隔变量
	INT								m_YBoradSpace;						//间隔变量

	//位置变量
protected:
	CSize							m_SizeChess;						//棋子大小
	CSize							m_SizeBorad;						//棋盘大小
	CPoint							m_BenchmarkPos;						//基准位置

	//资源变量
protected:
	CPngImage						m_ImageChessItem;					//棋子资源
	CSkinImage						m_ImageChessBorad;					//棋盘资源
	CSkinImage						m_ImageChessFrame;					//棋子框架
	CSkinImage						m_ImageWaitGameRule;				//等待资源

	//函数定义
public:
	//构造函数
	CChessBorad();
	//析构函数
	virtual ~CChessBorad();

	//功能函数
public:
	//绘画界面
	VOID DrawChessBorad(CDC * pDC);
	//设置模式
	VOID SetChessBoradMode(bool bSmallMode);

	//棋子控制
public:
	//清除棋子
	VOID CleanChessItem();
	//设置棋盘
	VOID SetChessBorad(tagChessItem * ChessItemPtr[COUNT_X][COUNT_Y]);

	//选择设置
public:
	//选择棋子
	VOID SelectChessItem(BYTE cbXPos, BYTE cbYPos);
	//走棋路线
	VOID SetMoveChessTrace(BYTE cbXPosStart, BYTE cbYPosStart, BYTE cbXPosTerminal, BYTE cbYPosTerminal);

	//状态控制
public:
	//设置颜色
	VOID SetChessColor(BYTE cbUserColor) { m_cbUserColor=cbUserColor; }
	//设置响应
	VOID SetPositively(bool bPositively) { m_bPositively=bPositively; }
	//棋谱视图
	VOID SetManualView(bool bManualView) { m_bManualView=bManualView; }
	//等待规则
	VOID SetWaitGameRule(bool bWaitGameRule) { m_bWaitGameRule=bWaitGameRule; }

	//位置函数
public:
	//获取大小
	VOID GetControlSize(CSize & ControlSize) { ControlSize=m_SizeBorad; }
	//获取位置
	VOID GetBenchmarkPos(CPoint & BenchmarkPos) { BenchmarkPos=m_BenchmarkPos; }
	//基准位置
	VOID SetBenchmarkPos(INT nXPos, INT nYPos) { m_BenchmarkPos.SetPoint(nXPos,nYPos); }

	//事件函数
public:
	//鼠标点击
	VOID OnEventLMouseDown(CPoint Point);

	//辅助函数
private:
	//获取坐标
	INT GetChessXPos(BYTE cbXPos);
	//获取坐标
	INT GetChessYPos(BYTE cbYPos);
	//转换坐标
	VOID SwitchMousePos(INT nXCursor, INT nYCursor, BYTE & cbXIndex, BYTE & cbYIndex);
};

//////////////////////////////////////////////////////////////////////////

#endif
