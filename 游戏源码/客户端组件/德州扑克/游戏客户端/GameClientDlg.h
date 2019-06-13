#pragma once

#include "Stdafx.h"
#include "GameClientView.h"
#include "GameLogic.h"
#include "ShowHand.h"

//////////////////////////////////////////////////////////////////////////

//游戏对话框
class CGameClientDlg : public CGameFrameDlg
{
	//游戏变量
protected:
	WORD							m_wMeChairID;						//本身位置
	WORD							m_wDUser;							//D玩家
	WORD							m_wCurrentUser;						//当前玩家
	bool							m_bOpenCard;						//开牌
	bool							m_bExitTag;							//强退标志
	bool							m_bReset;							//复位标志
	TCHAR							m_bUserName[GAME_PLAYER][32];		//用户名字			

	//加注信息
protected:
	LONG							m_lCellScore;						//单元下注
	LONG							m_lTurnLessScore;					//最小下注
	LONG							m_lTurnMaxScore;					//最大下注
	LONG							m_lAddLessScore;					//加最小注
	LONG							m_lTableScore[GAME_PLAYER];			//下注数目
	LONG							m_lTotalScore[GAME_PLAYER];			//累计下注
	LONG							m_lCenterScore;						//中心筹码
	LONG							m_lBalanceScore;					//平衡筹码

	//状态变量
protected:
	BYTE							m_cbPlayStatus[GAME_PLAYER];			//游戏状态
	BOOL							m_bAutoStart;							//自动开始
	LONG							m_dEndScore[GAME_PLAYER];				//结束分数

	//扑克信息
protected:
	BYTE							m_cbCenterCardData[MAX_CENTERCOUNT];	//中心扑克
	BYTE							m_cbHandCardData[GAME_PLAYER][MAX_COUNT];//手上扑克
	BYTE							m_cbOverCardData[GAME_PLAYER][MAX_CENTERCOUNT];//结束扑克

	//控件变量
public:
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
	//框架消息
	virtual bool OnFrameMessage(WORD wSubCmdID, const void * pBuffer, WORD wDataSize);

	//消息处理
protected:
	//游戏开始
	bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
	//用户加注
	bool OnSubAddScore(const void * pBuffer, WORD wDataSize);
	//用户放弃
	bool OnSubGiveUp(const void * pBuffer, WORD wDataSize);
	//发牌消息
	bool OnSubSendCard(const void * pBuffer, WORD wDataSize);
	//游戏结束
	bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);
	//开牌消息
	bool OnSubOpenCard(const void * pBuffer, WORD wDataSize);

	//辅助函数
protected:
	//隐藏控制
	void HideScoreControl();
	//更新控制
	void UpdateScoreControl();

	//消息映射
protected:
	//开始按钮
	LRESULT	OnStart(WPARAM wParam, LPARAM lParam);
	//离开按纽
	LRESULT OnExit(WPARAM wParam,LPARAM lParam);
	//放弃按纽
	LRESULT OnGiveUp(WPARAM wParam,LPARAM lParam);
	//加注按钮 
	LRESULT	OnAddScore(WPARAM wParam, LPARAM lParam);
	//最下按钮
	LRESULT	OnMinScore(WPARAM wParam, LPARAM lParam);
	//最大按钮
	LRESULT	OnMaxScore(WPARAM wParam, LPARAM lParam);
	//确定按钮
	LRESULT OnOKScore(WPARAM wParam,LPARAM lParam);
	//取消按钮
	LRESULT OnCancelScore(WPARAM wParam,LPARAM lParam);
	//让牌按钮
	LRESULT OnPassCard(WPARAM wParam,LPARAM lParam);
	//跟注按钮
	LRESULT OnFollow(WPARAM wParam,LPARAM lParam);
	//梭哈按钮
	LRESULT OnShowHand(WPARAM wParam,LPARAM lParam);
	//开始倒计
	LRESULT OnStartTimes(WPARAM wParam,LPARAM lParam);
	//自动开始
	LRESULT OnAutoStart(WPARAM wParam,LPARAM lParam);
	//坐下按钮
	LRESULT OnSitDown(WPARAM wParam,LPARAM lParam);
	//游戏结束
	LRESULT OnGameOver(WPARAM wParam,LPARAM lParam);
	//游戏结束
	LRESULT OnSendFinish(WPARAM wParam,LPARAM lParam);
	//开牌信息
	LRESULT OnOpenCard(WPARAM wParam,LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
