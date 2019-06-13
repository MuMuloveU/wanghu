#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "GameClientView.h"

//////////////////////////////////////////////////////////////////////////

//数组说明
typedef CArrayTemplate<CMD_Buffer,CMD_Buffer &> CSocketPacketArray;

//////////////////////////////////////////////////////////////////////////

//游戏对话框
class CGameClientDlg : public CGameFrameDlg
{
	//配置变量
protected:
	DWORD							m_dwCardHSpace;							//扑克象素

	//加注信息
protected:
	LONG							m_lMaxScore;							//最大下注
	LONG							m_lCellScore;							//单元下注
	LONG							m_lTurnMaxScore;						//最大下注
	LONG							m_lTurnLessScore;						//最小下注
	LONG							m_lTableScore[GAME_PLAYER*2];			//下注数目

	//状态变量
protected:
	bool							m_bAddScore;							//加注标志
	bool							m_bShowHand;							//梭哈标志
	WORD							m_wCurrentUser;							//当前用户
	BYTE							m_cbPlayStatus[GAME_PLAYER];			//游戏状态
	TCHAR							m_szAccounts[GAME_PLAYER][NAME_LEN];	//玩家名字

	//控件变量
public:
	CGameLogic						m_GameLogic;							//游戏逻辑
	CGameClientView					m_GameClientView;						//游戏视图
	
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
	//用户加注
	bool OnSubAddScore(const void * pBuffer, WORD wDataSize);
	//用户放弃
	bool OnSubGiveUp(const void * pBuffer, WORD wDataSize);
	//发牌消息
	bool OnSubSendCard(const void * pBuffer, WORD wDataSize);
	//游戏结束
	bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);

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
	//放弃按钮
	LRESULT OnGiveUp(WPARAM wParam, LPARAM lParam);
	//跟注按钮
	LRESULT OnFollow(WPARAM wParam, LPARAM lParam);
	//加注按钮 
	LRESULT	OnAddScore(WPARAM wParam, LPARAM lParam);
	//梭哈按钮
	LRESULT OnShowHand(WPARAM wParam, LPARAM lParam);
	//发牌完成
	LRESULT OnSendCardFinish(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
