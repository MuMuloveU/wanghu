#ifndef CHESS_BORAD_HEAD_FILE
#define CHESS_BORAD_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

//位置定义
#define EXCURSION_X					18									//偏移坐标
#define EXCURSION_Y					17									//偏移坐标
#define SPACE_BORAD_X				25									//间隙大小
#define SPACE_BORAD_Y				25									//间隙大小

//选择模式
#define SM_NO_SELECT				0x00								//禁止选择
#define SM_NULL_CHESS				0x01								//选择空白
#define SM_BLACK_CHESS				0x02								//选择黑棋
#define SM_WHITE_CHESS				0x04								//选择白棋

//消息定义
#define WM_HIT_CHESS_BORAD			(WM_USER+200)						//点击棋盘

//////////////////////////////////////////////////////////////////////////
//结构定义

//棋子定义
struct tagChessInfo
{
	BYTE							cbFlag;								//棋子标志
	BYTE							cbColor;							//棋子颜色
	WORD							wIdentifier;						//棋子标识
};

//////////////////////////////////////////////////////////////////////////

//游戏棋盘
class CChessBorad : public CWnd
{
	//配置变量
protected:
	bool							m_bShowChessID;						//显示手数
	bool							m_bDoubleMouse;						//双击下棋
	BYTE							m_cbSelectMode;						//选择模式

	//辅助变量
protected:
	BYTE							m_cbXCursor;						//光标位置
	BYTE							m_cbYCursor;						//光标位置

	//点目变量
protected:
	bool							m_bCountInfo;						//显示点目
	BYTE							m_cbCountInfo[19][19];				//点目结果

	//状态变量
protected:
	BYTE							m_cbXPosFocus;						//焦点棋子
	BYTE							m_cbYPosFocus;						//焦点棋子
	tagChessInfo					m_ChessInfo[19][19];				//棋子信息

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
	//清除点目
	void CleanCountInfo();
	//清理棋子
	void CleanChess(WORD wDeadChess[], WORD wDeadChessCount);
	//设置点目
	void SetChessCountInfo(BYTE cbCountInfo[19][19]);
	//设置棋子
	void SetChess(BYTE cbXPos, BYTE cbYPos, BYTE cbColor, WORD wIdentifier);
	//设置棋盘
	void SetChessBorad(const BYTE cbChessBorad[19][19], const WORD wIdentifier[19][19]);

	//状态控制
public:
	//显示手数
	void ShowChessID(bool bShowChessID);
	//双击设置
	void SetDoubleMouse(bool bDoubleMouse);
	//选择模式
	void SetSelectMode(BYTE cbSelectMode);
	//设置焦点
	void SetChessFocus(BYTE cbXPos, BYTE cbYPos);
	//显示胜利
	void ShowWinChessLine(BYTE cbXFirstPos, BYTE cbYFirstPos, BYTE cbXEndPos, BYTE cbYEndPos);
	//设置棋盘
	void SetChessBorad(const WORD wBlackChess[], BYTE cbBlackCount, const WORD wWhiteChess[], BYTE cbWhiteCount);

	//状态查询
public:
	//是否点目
	bool IsChessCount() { return m_bCountInfo; }
	//显示手数
	bool IsShowChessID() { return m_bShowChessID; } 
	//双击下棋
	bool IsDoubleMouse() { return m_bDoubleMouse; }
	//选择模式
	BYTE GetSelectMode() { return m_cbSelectMode; }
	//获取大小
	const CSize & GetChessBoradSize() { return m_BoradSize; }

	//辅助函数
private:
	//更新棋子
	void UpdateBoradPos(BYTE cbXPos, BYTE cbYPos);
	//选择判断
	bool EfficacySelect(BYTE cbXPos, BYTE cbYPos);
	//转换坐标
	void SwitchMousePos(const CPoint & MousePoint, BYTE & cbXPos, BYTE & cbYPos);
	//获取坐标
	inline int GetChessXPos(BYTE cbXPos) { return EXCURSION_X+cbXPos*SPACE_BORAD_X; }
	//获取坐标
	inline int GetChessYPos(BYTE cbYPos) { return EXCURSION_Y+cbYPos*SPACE_BORAD_Y; }

	//消息映射
protected:
	//重画函数
	afx_msg void OnPaint();
	//建立消息
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint Point);
	//鼠标双击
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//光标消息
	afx_msg BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif