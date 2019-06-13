#pragma once

#include "Stdafx.h"
#include "GameClientView.h"
#include "..\消息定义\CMD_QuickRun.h"

//////////////////////////////////////////////////////////////////////////

//游戏对话框
class CGameClientDlg : public CGameFrameDlg
{
	//控件变量
protected:
	CGameClientView					m_GameClientView;					//游戏视图
	
	//函数定义
public:
	//构造函数
	CGameClientDlg();
	//析构函数
	virtual ~CGameClientDlg();

	//常规继承
private:
	//初始函数
	virtual bool InitGameFrame();
	//重置框架
	virtual void ResetGameFrame();
	//游戏设置
	virtual void OnGameOptionSet();
	//时间消息
	virtual bool OnTimerMessage(WORD wChairID, UINT nElapse, UINT nTimerID);
	//旁观状态
	virtual void OnLookonChanged(bool bLookonUser, const void * pBuffer, WORD wDataSize);
	//网络消息
	virtual bool OnGameMessage(WORD wSubCmdID, const void * pBuffer, WORD wDataSize);
	//游戏场景
	virtual bool OnGameSceneMessage(BYTE cbGameStatus, bool bLookonOther, const void * pBuffer, WORD wDataSize);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg LRESULT OnCmdOutPisa(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnCmdUserReady(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnCmdTimerOut(WPARAM wParam,LPARAM lParam);
};

//////////////////////////////////////////////////////////////////////////
