#pragma once

#include "Stdafx.h"
#include "GameClientView.h"

//////////////////////////////////////////////////////////////////////////

//游戏对话框
class CGameClientDlg : public CGameFrameDlg
{
	//限制信息
protected:
	LONG							m_lMeMaxScore;						//最大下注
	LONG							m_lAreaLimitScore;					//区域限制

	//下注信息
protected:
	LONG							m_lMeTieScore;						//买平总注
	LONG							m_lMeBankerScore;					//买庄总注
	LONG							m_lMePlayerScore;					//买闲总注
	LONG							m_lMeTieSamePointScore;				//同点平注
	LONG							m_lMePlayerKingScore;				//闲天王注
	LONG							m_lMeBankerKingScore;				//庄天王注

	//状态变量
protected:
	bool							m_bPlaying;							//游戏状态
	int								m_nShowResultTime;					//显示时间

	//扑克信息
protected:
	BYTE							m_cbCardCount[2];					//扑克数目
	BYTE							m_cbSendCount[2];					//扑克数目
    BYTE							m_cbTableCardArray[2][3];			//桌面扑克
	CString							m_strDispatchCardTips;				//发牌提示

	//历史信息
protected:
	WORD							m_wDrawCount;						//游戏局数
	LONG							m_lMeResultCount;					//游戏成绩

	//庄家信息
protected:
	LONG							m_lApplyBankerCondition;			//申请条件
	LONG							m_lBankerScore;						//庄家积分
	WORD							m_wCurrentBanker;					//当前庄家
	bool							m_bMeApplyBanker;					//申请标识

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
	//取消消息
	virtual void OnCancel();

	//消息处理
protected:
	//游戏开始
	bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
	//用户加注
	bool OnSubPlaceJetton(const void * pBuffer, WORD wDataSize);
	//游戏结束
	bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);
	//申请做庄
	bool OnUserApplyBanker(const void * pBuffer, WORD wDataSize);
	//切换庄家
	bool OnChangeBanker(const void * pBuffer, WORD wDataSize);
	//更新积分
	bool OnChangeUserScore(const void * pBuffer, WORD wDataSize);
	//游戏记录
	bool OnSubGameRecord(const void * pBuffer, WORD wDataSize);
	//游戏积分
	bool OnSubGameScore(const void * pBuffer, WORD wDataSize);

	//功能函数
protected:
	//更新控制
	void UpdateButtonContron();
	//派发扑克
	bool DispatchUserCard(BYTE cbPlayerCard[], BYTE cbPlayerCount, BYTE cbBankerCard[], BYTE cbBankerCount);
	//发牌提示
	void SetDispatchCardTips();
	//推断赢家
	void DeduceWinner(BYTE &cbWinner, BYTE &cbKingWinner);

	//消息映射
protected:
	//定时器消息
	afx_msg void OnTimer(UINT nIDEvent);
	//加注消息
	LRESULT OnPlaceJetton(WPARAM wParam, LPARAM lParam);
	//申请消息
	LRESULT OnApplyBanker(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
