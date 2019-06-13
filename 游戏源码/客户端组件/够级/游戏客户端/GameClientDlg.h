#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "GameClientView.h"

//////////////////////////////////////////////////////////////////////////

//游戏对话框
class CGameClientDlg : public CGameFrameDlg
{
	//游戏变量
protected:
	BYTE							m_bCardCount[GAME_PLAYER];					//扑克数目
	BYTE							m_bHandCardCount;							//扑克数目
	BYTE							m_bHandCardData[PLAYER_CARDS];				//手上扑克
	BYTE							m_EndOrder;

	//出牌变量
protected:
	BYTE							m_bTurnOutType;						//出牌类型
	BYTE							m_bTurnCardCount;					//出牌数目
	BYTE							m_bTurnCardData[PLAYER_CARDS];				//出牌列表
	BYTE							m_bHintCardData[PLAYER_CARDS];				//提示出牌列表
	BYTE							m_bHintCardCount;					//提示出牌数目

	//配置变量
protected:
	bool							m_bDeasilOrder;						//出牌顺序
	DWORD							m_dwCardHSpace;						//扑克象素

	//辅助变量
protected:
	WORD							m_wMostUser;						//最大玩家
	WORD							m_wTimeOutCount;					//超时次数

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
	//发送扑克
	bool OnSubSendCard(const void * pBuffer, WORD wDataSize);
	//游戏开始
	bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
	//用户出牌
	bool OnSubOutCard(const void * pBuffer, WORD wDataSize);
	//放弃出牌
	bool OnSubPassCard(const void * pBuffer, WORD wDataSize);
	//游戏结束
	bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);
	//有玩家出完牌
	bool OnHaveEnd(const void * pBuffer, WORD wDataSize);

	//辅助函数
protected:
	//出牌判断
	bool VerdictOutCard();
	//自动出牌
	bool AutomatismOutCard();
	//提示出牌
	void AutoOutCard(BYTE m_WhichOnsKindCard);
	void AutoOutCard111(BYTE m_WhichOnsKindCard);
	//消息映射
protected:
	//定时器消息
	afx_msg void OnTimer(UINT nIDEvent);
	//开始消息
	LRESULT OnStart(WPARAM wParam, LPARAM lParam);
	//出牌消息
	LRESULT OnOutCard(WPARAM wParam, LPARAM lParam);
	//放弃出牌
	LRESULT OnPassCard(WPARAM wParam, LPARAM lParam);
	//出牌提示
	LRESULT OnAutoOutCard(WPARAM wParam, LPARAM lParam);
	//右键扑克
	LRESULT OnLeftHitCard(WPARAM wParam, LPARAM lParam);
	//左键扑克
	LRESULT OnRightHitCard(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
