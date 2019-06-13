#ifndef GAME_CLIENT_ENGINE_HEAD_FILE
#define GAME_CLIENT_ENGINE_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "..\服务器组件\GameLogic.h"
#include "HistoryRecord.h"
#include "GameClientView.h"
#include "GameOption.h"


//////////////////////////////////////////////////////////////////////////

//游戏引擎
class CGameClientDlg : public CGameFrameDlg
{
	//游戏变量
protected:
	BYTE							m_cbMainValue;						//主牌数值
	WORD							m_wCurrentUser;						//当前用户
	BYTE							m_cbValueOrder[GAME_PLAYER];		//等级数值
	bool							m_bAutoStatus[GAME_PLAYER];			//托管状态
	int								m_TurnScore;						//本轮分数
	int								m_PlayerScore[GAME_PLAYER];			//用户分数

	BYTE							m_bTongHuaCount;
	BYTE							m_bTongHuaIndex;
	BYTE							m_bTongHuaCard[100][5];

	//胜利信息
protected:
	WORD							m_wWinCount;						//胜利人数
	WORD							m_wWinOrder[GAME_PLAYER];			//胜利列表

	//扑克变量
protected:
	BYTE							m_cbHandCardData[MAX_COUNT];		//手上扑克
	BYTE							m_cbHandCardCount[GAME_PLAYER];		//扑克数目

	//出牌变量
protected:
	BYTE							m_cbTurnCardType;					//扑克类型
	BYTE							m_cbTurnCardCount;					//出牌数目
	BYTE							m_cbTurnCardData[MAX_COUNT];		//出牌列表
	BYTE							m_cbMagicCardData[MAX_COUNT];		//变幻扑克

	//辅助变量
protected:
	bool							m_bStustee;							//托管标志
	bool							m_bLastTurn;						//上轮标志
	WORD							m_wMostUser;						//最大玩家
	BYTE							m_cbSortType;						//排序类型
	WORD							m_wTimeOutCount;					//超时次数

	//发牌变量
protected:
	BYTE							m_cbRemnantCardCount;				//剩余数目
	BYTE							m_cbDispatchCardCount;				//派发数目
	BYTE							m_cbDispatchCardData[MAX_COUNT];	//派发扑克

	//配置变量
protected:
	DWORD							m_dwCardHSpace;						//扑克象素

	//组件变量
protected:
	CHistoryCard					m_HistoryCard;						//历史扑克

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
	bool OnSubGameStart(const void * pData, WORD wDataSize);
	//用户出牌
	bool OnSubOutCard(const void * pData, WORD wDataSize);
	//用户放弃
	bool OnSubPassCard(const void * pData, WORD wDataSize);
	//扑克信息
	bool OnSubCardInfo(const void * pData, WORD wDataSize);
	//游戏结束
	bool OnSubGameEnd(const void * pData, WORD wDataSize);
	//请求看牌
	bool OnSubRequestLook(const void * pData, WORD wDataSize);
	//请求离开
	bool OnSubRequestLeave(const void * pData, WORD wDataSize);
	//继续游戏
	bool OnSubContinueGame(const void * pData, WORD wDataSize);

	bool OnSubUserAutomatism(const void * pBuffer, WORD wDataSize);
	//功能函数
protected:
	//出牌判断
	bool VerdictOutCard();
	//放弃判断
	bool VerdictPassCard();

	//功能函数
protected:
	//自动开始
	bool PerformAutoStart();
	//自动出牌
	bool AutomatismOutCard();
	//当前扑克
	bool SwitchToCurrentCard();
	//停止发牌
	bool DoncludeDispatchCard();
	//派发扑克
	bool DispatchUserCard(BYTE cbCardData[], BYTE cbCardCount);

	//消息映射
protected:
	//时间消息
	VOID OnTimer(UINT nIDEvent);
	//开始消息
	LRESULT OnMessageStart(WPARAM wParam, LPARAM lParam);
	//出牌消息
	LRESULT OnMessageOutCard(WPARAM wParam, LPARAM lParam);
	//拖管控制
	LRESULT OnMessageStustee(WPARAM wParam, LPARAM lParam);
	//排列扑克
	LRESULT OnMessageSortCard(WPARAM wParam, LPARAM lParam);
	//查看上轮
	LRESULT OnMessageLastTurn(WPARAM wParam, LPARAM lParam);
	//PASS消息
	LRESULT OnMessagePassCard(WPARAM wParam, LPARAM lParam);
	//提示消息
	LRESULT OnMessageOutPrompt(WPARAM wParam, LPARAM lParam);
	//进贡消息
	LRESULT OnMessageThrowCard(WPARAM wParam, LPARAM lParam);
	//回贡消息
	LRESULT OnMessageIncepCard(WPARAM wParam, LPARAM lParam);
	//左键扑克
	LRESULT OnMessageLeftHitCard(WPARAM wParam, LPARAM lParam);
	//启用上轮
	LRESULT OnMessageEnableHistory(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif