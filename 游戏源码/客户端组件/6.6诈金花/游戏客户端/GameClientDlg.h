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
	//游戏变量
protected:
	WORD							m_wCurrentUser;							//当前用户
	WORD							m_wBankerUser;							//庄家用户

	//用户状态
protected:
	TCHAR							m_szAccounts[GAME_PLAYER][NAME_LEN];	//玩家名字
	BYTE							m_cbPlayStatus[GAME_PLAYER];			//游戏状态
	LONG							m_lTableScore[GAME_PLAYER];				//下注数目

	//扑克变量
protected:
	BYTE							m_cbHandCardData[GAME_PLAYER][MAX_COUNT];//桌面扑克

	//下注信息
protected:
	LONG							m_lMaxScore;							//封顶数目
	LONG							m_lMaxCellScore;						//单元上限
	LONG							m_lCellScore;							//单元下注
	LONG							m_lCurrentTimes;						//当前倍数
	LONG							m_lUserMaxScore;						//最大分数
	bool							m_bMingZhu[GAME_PLAYER];				//看牌动作

	//输牌用户
protected:
	WORD							m_wLostUser;							//比牌失败
	WORD							m_wWinnerUser;							//胜利用户

	//动画信息
protected:
	WORD							m_wLostUserID[GAME_PLAYER];				//比败用户

	//椅子信息
protected:
	WORD							m_wViewChairID[GAME_PLAYER];			//玩家椅子

	//配置变量
protected:
	DWORD							m_dwCardHSpace;							//扑克象素

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
	//游戏开始
	bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
	//用户放弃
	bool OnSubGiveUp(const void * pBuffer, WORD wDataSize);
	//用户加注
	bool OnSubAddScore(const void * pBuffer, WORD wDataSize);
	//用户看牌
	bool OnSubLookCard(const void * pBuffer, WORD wDataSize);
	//用户比牌
	bool OnSubCompareCard(const void * pBuffer, WORD wDataSize);
	//用户开牌
	bool OnSubOpenCard(const void * pBuffer, WORD wDataSize);
	//用户强退
	bool OnSubPlayerExit(const void * pBuffer, WORD wDataSize);
	//游戏结束
	bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);

	//辅助函数
protected:
	//处理控制
	void ScoreControl(BOOL bShow);
	//加注控制
	void AddScoreControl(BOOL bShow);
	//处理控制
	void UpdataControl();

	//消息映射
protected:
	//开始按钮
	LRESULT	OnStart(WPARAM wParam, LPARAM lParam);
	//加注按钮 
	LRESULT	OnAddScore(WPARAM wParam, LPARAM lParam);
	//最少加注
	LRESULT	OnMinScore(WPARAM wParam, LPARAM lParam);
	//最大加注
	LRESULT	OnMaxScore(WPARAM wParam, LPARAM lParam);
	//确定消息
	LRESULT OnConfirmScore(WPARAM wParam, LPARAM lParam);
	//取消消息
	LRESULT OnCancelAdd(WPARAM wParam, LPARAM lParam);
	//看牌消息
	LRESULT	OnLookCard(WPARAM wParam, LPARAM lParam);
	//比牌消息
	LRESULT	OnCompareCard(WPARAM wParam, LPARAM lParam);
	//被选比牌用户
	LRESULT OnChooseCUser(WPARAM wParam, LPARAM lParam);
	//开牌消息
	LRESULT OnOpenCard(WPARAM wParam, LPARAM lParam);
	//放弃消息
	LRESULT	OnGiveUp(WPARAM wParam, LPARAM lParam);
	//发牌完成
	LRESULT OnSendCardFinish(WPARAM wParam, LPARAM lParam);
	//闪牌完成
	LRESULT OnFlashCardFinish(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
