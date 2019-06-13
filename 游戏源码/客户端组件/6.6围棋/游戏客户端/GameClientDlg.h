#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "GameClientView.h"

//////////////////////////////////////////////////////////////////////////

//游戏对话框
class CGameClientDlg : public CGameFrameDlg
{
	//规则变量
protected:
	tagGameRuleInfo					m_GameRuleInfo;						//游戏规则

	//配置变量
protected:
	bool							m_bDoubleMouse;						//双击下棋
	bool							m_bShowChessID;						//显示手数
	bool							m_bAutoSaveManual;					//自动保存

	//游戏辅助
protected:
	bool							m_ShamView;							//预先显示
	bool							m_bAgreeRule;						//同意标志
	bool							m_bCustomize;						//定制标志
	bool							m_bGameStart;						//游戏标志
	bool							m_bLimitTime;						//限定时间
	bool							m_bReqChessCount;					//点目请求
	bool							m_bResponsionChessCount;			//响应点目

	//游戏变量
protected:
	WORD							m_wBlackUser;						//黑棋玩家
	WORD							m_wCurrentUser;						//当前用户
	tagStatusInfo					m_StatusInfo[2];					//游戏信息

	//控件变量
protected:
	CGameLogic						m_GameLogic;						//游戏逻辑
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

	//消息处理
protected:
	//游戏开始
	bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
	//用户放棋
	bool OnSubPlaceChess(const void * pBuffer, WORD wDataSize);
	//放弃下棋
	bool OnSubPassChess(const void * pBuffer, WORD wDataSize);
	//悔棋请求
	bool OnSubRegretReq(const void * pBuffer, WORD wDataSize);
	//悔棋失败
	bool OnSubRegretFaile(const void * pBuffer, WORD wDataSize);
	//悔棋结果
	bool OnSubRegretResult(const void * pBuffer, WORD wDataSize);
	//和棋请求
	bool OnSubPeaceReq(const void * pBuffer, WORD wDataSize);
	//和棋应答
	bool OnSubPeaceAnser(const void * pBuffer, WORD wDataSize);
	//游戏结束
	bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);
	//游戏棋谱
	bool OnSubChessManual(const void * pBuffer, WORD wDataSize);
	//请求设置
	bool OnSubSetRule(const void * pBuffer, WORD wDataSize);
	//请求同意
	bool OnSubAgreeRule(const void * pBuffer, WORD wDataSize);
	//请求点目
	bool OnSubReqChessCount(const void * pBuffer, WORD wDataSize);
	//点目应答
	bool OnSubChessCountResponsion(const void * pBuffer, WORD wDataSize);

	//辅助函数
protected:
	//保存棋谱
	bool SaveChessManual(LPCTSTR pszFileName);

	//消息映射
protected:
	//定时器消息
	afx_msg void OnTimer(UINT nIDEvent);
	//开始按钮
	LRESULT OnStart(WPARAM wParam, LPARAM lParam);
	//点目按钮
	LRESULT OnCount(WPARAM wParam, LPARAM lParam);
	//通过按钮
	LRESULT OnPass(WPARAM wParam, LPARAM lParam);
	//悔棋按钮
	LRESULT OnRegret(WPARAM wParam, LPARAM lParam);
	//求和按钮
	LRESULT OnPeace(WPARAM wParam, LPARAM lParam);
	//认输按钮
	LRESULT OnGiveUp(WPARAM wParam, LPARAM lParam);
	//保存按钮
	LRESULT OnPreserve(WPARAM wParam, LPARAM lParam);
	//研究按钮
	LRESULT OnStudy(WPARAM wParam, LPARAM lParam);
	//和棋应答
	LRESULT OnPeaceAnswer(WPARAM wParam, LPARAM lParam);
	//悔棋应答
	LRESULT OnRegretAnswer(WPARAM wParam, LPARAM lParam);
	//点击棋盘
	LRESULT OnHitChessBorad(WPARAM wParam, LPARAM lParam);
	//同意规则
	LRESULT OnAgreeGameRule(WPARAM wParam, LPARAM lParam);
	//点目应答
	LRESULT OnChessCountResponsion(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
