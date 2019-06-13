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
	WORD							m_wViewChairID[GAME_PLAYER];			//视图位置

	//加注信息
protected:
	LONG							m_lTurnMaxScore;						//最大下注
	LONG							m_lTableScore[GAME_PLAYER];				//下注数目

	//状态变量
protected:
	WORD							m_wBankerUser;							//庄家用户
	BYTE							m_cbPlayStatus[GAME_PLAYER];			//游戏状态
	TCHAR							m_szAccounts[GAME_PLAYER][NAME_LEN];	//玩家名字

	//用户扑克
protected:
	BYTE							m_cbHandCardData[GAME_PLAYER][MAX_COUNT];//用户数据
	BYTE							m_bUserOxCard[GAME_PLAYER];				//牛牛数据								

	//缓冲变量
protected:
	CSocketPacketArray				m_SocketPacketArray;					//数据缓存

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
	//用户叫庄
	bool OnSubCallBanker(const void * pBuffer, WORD wDataSize);
	//游戏开始
	bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
	//用户加注
	bool OnSubAddScore(const void * pBuffer, WORD wDataSize);
	//发牌消息
	bool OnSubSendCard(const void * pBuffer, WORD wDataSize);
	//用户摊牌
	bool OnSubOpenCard(const void * pBuffer, WORD wDataSize);
	//用户强退
	bool OnSubPlayerExit(const void * pBuffer, WORD wDataSize);
	//游戏结束
	bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);

	//辅助函数
protected:
	//更新控制
	void UpdateScoreControl(LONG lScore[],BOOL bShow);

	//消息映射
protected:
	//提示按钮
	LRESULT	OnHintOx(WPARAM wParam, LPARAM lParam);
	//开始按钮
	LRESULT	OnStart(WPARAM wParam, LPARAM lParam);
	//加注按钮 
	LRESULT	OnAddScore(WPARAM wParam, LPARAM lParam);
	//牛牛按钮
	LRESULT	OnOx(WPARAM wParam, LPARAM lParam);
	//快捷键按钮 
	LRESULT OnShortcut(WPARAM wParam, LPARAM lParam);
	//摊牌按钮
	LRESULT	OnOpenCard(WPARAM wParam, LPARAM lParam);
	//发牌完成
	LRESULT OnSendCardFinish(WPARAM wParam, LPARAM lParam);
	//显示点数
	LRESULT OnSetUserOxValue(WPARAM wParam, LPARAM lParam);
	//提示信息
	LRESULT OnShowInfo(WPARAM wParam, LPARAM lParam);
	//牛BT为真
	LRESULT OnOxEnable(WPARAM wParam, LPARAM lParam);
	//扑克分类
	LRESULT OnSortCard(WPARAM wParam, LPARAM lParam);
	//扑克分类
	LRESULT OnReSortCard(WPARAM wParam, LPARAM lParam);
	//做庄信息
	LRESULT OnBanker(WPARAM wParam, LPARAM lParam);

public:
	afx_msg void OnTimer(UINT nIDEvent);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
