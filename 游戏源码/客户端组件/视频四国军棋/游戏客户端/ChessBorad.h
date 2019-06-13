#ifndef CHESS_BORAD_HEAD_FILE
#define CHESS_BORAD_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include <AFXTEMPL.H>


//////////////////////////////////////////////////////////////////////////
//宏定义

//位置定义
#define CHESS_WIDTH					30									//棋子宽度
#define CHESS_HEIGTH				30									//棋子高度
#define NODE_WIDTH					36									//结点宽度
#define NODE_HEIGTH					36									//结点高度

#define ARROW_WIDTH					30									//箭头宽度
#define ARROW_HEIGTH				30									//箭头高度

//////////////////////////////////////////////////////////////////////////
//标志信息
struct tagFlagInfo
{
	bool							bSet;								//设置标志
	BYTE							bXPos ,								//标志坐标
									bYPos ;								//标志坐标
};

class CShowFlagDlg ;
class CGameClientView ;

//游戏棋盘
class CChessBorad
{
	//配置变量
protected:
	BYTE							m_cbUserColor;						//用户颜色
	bool							m_bPositively;						//响应标志

public:
	static LONG						m_bExsursionX;						//棋盘偏移
	static LONG						m_bExsursionY;						//棋盘偏移
	CPoint							m_StartPoint;						//标志位置
	BYTE							m_bSetFlagX ,						//标志位置
									m_bSetFlagY ;						//标志位置

	//辅助变量
protected:
	CGameClientView					*m_pParent;							//父类指针
	CArray <POINT,POINT>			m_ptPathArray;						//走棋路线
	tagFlagInfo						m_FlagInfo[17][17] ;				//设置标志

	//状态变量
protected:
	tagChessItem					m_ChessBorad[17][17];				//棋盘信息

	//资源变量
protected:
	CSize							m_BoradSize;						//棋盘大小
	CSkinImage						m_ImageChess;						//棋子资源
	CSkinImage						m_ImageBorad;						//棋盘资源
	CSkinImage						m_ImageBackGround;					//背景资源
	CSkinImage						m_ImageArrow;						//箭头资源
	CSkinImage						m_ImageFlag;						//标志图案

	//控件变量
public:
	CShowFlagDlg					*m_pFlagImageDlg;					//标志窗体


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
	//设置棋子
	void SetChess(const tagChessItem tagChessItems[17][17]) ;

	//状态控制
public:
	//设置父类
	void SetParent(CGameClientView *pWnd) ;
	//重画函数
	void DrawBoard(CDC *pDC);
	//棋盘偏移
	void SetExcursionX(const LONG ExcursionX) ;
	//棋盘偏移
	void SetExcursionY(const LONG ExcursionY) ;

	//状态查询
public:
	//获取大小
	const CSize & GetChessBoradSize() { return m_BoradSize; }

	//辅助函数
public:
	//获取坐标
	int GetChessXPos(BYTE cbXPos);
	//获取坐标
	int GetChessYPos(BYTE cbYPos);
	//转换坐标
	void SwitchMousePos(const CPoint & MousePoint, BYTE & cbXPos, BYTE & cbYPos);
	//走棋路线
	void SetPath(const CArray <POINT,POINT> *pPtPath) ;
	//绘画箭头
	void DrawArrow(CDC *pDC) ;
	//设置标志
	void SetFlagImage(const BYTE bFlagXPos , const BYTE bFlagYPos , bool bSet=true) ;
	//显示标志
	void ShowFlagImage(CPoint point , bool bShow) ;
	//标志棋子
	void SetFlaChessPos(BYTE bXPos , BYTE bYPos) ;
	//清除标志
	void ClearFlagImage(BYTE bXPos , BYTE bYPos) ;
	//逆转标志
	void ReverseFlagImage() ;
	//移动标志
	void MoveFlagImage(BYTE bSourceX , BYTE bSourceY , BYTE bTargetX , BYTE bTargetY , bool bSet) ;

	//消息映射
protected:

};

//////////////////////////////////////////////////////////////////////////

#endif