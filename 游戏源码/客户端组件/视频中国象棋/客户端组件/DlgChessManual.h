#ifndef DLG_CHESS_MANUAL_HEAD_FILE
#define DLG_CHESS_MANUAL_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "ChessBorad.h"
#include "ManualList.h"

//////////////////////////////////////////////////////////////////////////

#define VER_MANUAL_FILE				1									//棋谱版本

//文件头信息
struct tagChessManualHead
{
	//棋谱信息
	WORD							wVersion;							//版本号码
	WORD							wItemCount;							//棋谱数目
	SYSTEMTIME						ManualDateTime;						//棋谱时间

	//游戏信息
	WORD							wBankerUser;						//庄家用户
	TCHAR							szUserAccounts[2][LEN_ACCOUNTS];	//用户帐号
};

//////////////////////////////////////////////////////////////////////////

//棋谱文件
class CChessManualFile
{
	//棋谱信息
protected:
	SYSTEMTIME						m_ManualDateTime;					//棋谱时间

	//游戏信息
protected:
	WORD							m_wBankerUser;						//庄家用户
	TCHAR							m_szUserAccounts[2][LEN_ACCOUNTS];	//用户帐号

	//函数定义
public:
	//构造函数
	CChessManualFile();
	//析构函数
	virtual ~CChessManualFile();

	//棋谱时间
public:
	//棋谱时间
	SYSTEMTIME GetManualDateTime() { return m_ManualDateTime; }
	//设置时间
	VOID SetManualDateTime(SYSTEMTIME & ManualDateTime) { m_ManualDateTime=ManualDateTime; }

	//庄家信息
public:
	//庄家用户
	WORD GetBankerUser() { return m_wBankerUser; }
	//设置庄家
	VOID SetBankerUser(WORD wBankerUser) { m_wBankerUser=wBankerUser; }

	//用户信息
public:
	//获取用户
	TCHAR * GetUserAccounts(WORD wChairID);
	//设置用户
	bool SetUserAccounts(WORD wChairID, TCHAR * pszAccounts);

	//功能函数
public:
	//加载棋谱
	bool LoadChessManual(LPCTSTR pszFileName, CChessManualArray & ChessManualArray);
	//保存棋谱
	bool SaveChessManual(LPCTSTR pszFileName, CChessManualArray & ChessManualArray);
};

//////////////////////////////////////////////////////////////////////////

//棋谱视图
class CDlgChessManual : public CDialog
{
	//棋盘变量
protected:
	WORD							m_wStepCount;						//走棋步数
	tagChessItem					m_ChessItemView[2][COUNT_CHESS];	//棋子信息
	tagChessItem *					m_ChessBoradView[COUNT_X][COUNT_Y];	//棋盘信息

	//组件变量
protected:
	CGameLogic						m_GameLogic;						//游戏逻辑
	CChessBorad						m_ChessBorad;						//棋盘控件
	CManualList						m_ManualList;						//棋谱列表
	CToolTipCtrl					m_ToolTipCtrl;						//提示控件
	CChessManualFile				m_ChessManualFile;					//棋谱文件

	//控制按钮
protected:
	CSkinButton						m_btHide;							//隐藏按钮
	CSkinButton						m_btCancel;							//关闭按钮
	CSkinButton						m_btManualLoad;						//加载按钮
	CSkinButton						m_btManualSave;						//保存按钮
	CSkinButton						m_btManualNote;						//解说按钮

	//控制按钮
protected:
	CSkinButton						m_btManualHead;						//棋谱按钮
	CSkinButton						m_btManualLast;						//棋谱按钮
	CSkinButton						m_btManualPlay;						//棋谱按钮
	CSkinButton						m_btManualNext;						//棋谱按钮
	CSkinButton						m_btManualTail;						//棋谱按钮

	//资源变量
protected:
	CSkinImage						m_ImageViewBack;					//背景资源

	//函数定义
public:
	//构造函数
	CDlgChessManual();
	//析构函数
	virtual ~CDlgChessManual();

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定消息
	virtual VOID OnOK();
	//取消消息
	virtual VOID OnCancel();

	//功能函数
public:
	//设置棋谱
	bool SetChessManual(CChessManualArray & ChessManualArray, CChessManualFile & ChessManualFile);

	//内部函数
private:
	//更新控制
	VOID UpdateManualControl();
	//棋谱定位
	VOID OrientationManual(WORD wStepCount);
	//调整控件
	VOID RectifyGameView(INT nWidth, INT nHeight);

	//棋谱按钮
protected:
	//棋谱按钮
	VOID OnBnClickedHead();
	//棋谱按钮
	VOID OnBnClickedLast();
	//棋谱按钮
	VOID OnBnClickedPlay();
	//棋谱按钮
	VOID OnBnClickedNext();
	//棋谱按钮
	VOID OnBnClickedTail();

	//控制按钮
protected:
	//隐藏按钮
	VOID OnBnClickedHide();
	//打开按钮
	VOID OnBnClickedLoad();
	//保存按钮
	VOID OnBnClickedSave();
	//解说按钮
	VOID OnBnClickedNote();

	//消息映射
protected:
	//重画函数
	VOID OnPaint();
	//时间消息
	VOID OnTimer(UINT nIDEvent);
	//绘画背景
	BOOL OnEraseBkgnd(CDC * pDC);
	//选择改变
	VOID OnLbnSelchangeManualList();
	//鼠标消息
	VOID OnLButtonDown(UINT nFlags, CPoint Point);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif
