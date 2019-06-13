#pragma once

#include "Stdafx.h"
#include "BJGameView.h"
#include "..\消息定义\CMD_BlackJack.h"

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//消息定义
#define IDM_GAME_START				WM_USER+120							//开始按钮消息
#define IDM_ADD_GOLD				WM_USER+121							//下注按钮消息
#define IDM_GIVE_UP				    WM_USER+122							//取消按钮消息
#define IDM_GET_CARD				WM_USER+123							//要牌按钮消息
#define IDM_STOP_CARD  				WM_USER+124							//停牌按钮消息
#define IDM_DOUBLE_CARD			    WM_USER+125							//双倍按钮消息
#define IDM_CUT_CARD			    WM_USER+126							//分牌按钮消息

//游戏对话框
class CBJGameDlg : public CGameFrameDlg
{
	
	//游戏变量
protected:
	LONG							m_lTurnMaxGold;						//最大下注
	LONG							m_lTurnBasicGold;					//跟注数目
	WORD							m_wCardSum;			                //扑克总值
	BOOL							m_bCutCard;			                //分牌标识

	//辅助变量
protected:
	bool							m_bMeAddGold;						//是否下注
	LONG							m_lGoldShow;						//加注筹码
	TCHAR							m_szName[GAME_PLAYER][NAME_LEN];	//玩家名字

	//控件变量
protected:
	CBJGameView					    m_GameView;							//游戏视图
	
	//函数定义
public:
	//构造函数
	CBJGameDlg();
	//析构函数
	virtual ~CBJGameDlg();

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
	//用户下注
	bool OnSubAddGold(const void * pBuffer, WORD wDataSize);
	//初始发牌
	bool OnSubInitCard(const void * pBuffer, WORD wDataSize);
	//发牌消息
	bool OnSubSendCard(const void * pBuffer, WORD wDataSize);
	//停牌消息
	bool OnSubStopCard(const void * pBuffer, WORD wDataSize);
	//分牌消息
	bool OnSubCutCard(const void * pBuffer, WORD wDataSize);
	//爆牌消息
	bool OnSubBrustCard(const void * pBuffer, WORD wDataSize);
	//庄家发牌
	bool OnSubBankerSendCard(const void * pBuffer, WORD wDataSize);
	//玩家强退
	bool OnSubUserLeft(const void * pBuffer, WORD wDataSize);
	//游戏结束
	bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);
	//改到右手
	bool OnCutCardMark(const void * pBuffer, WORD wDataSize);

	//消息映射
protected:
	//开始按钮	
	LRESULT	OnStart(WPARAM wParam, LPARAM lParam);
	//加注按钮 
	LRESULT	OnAddGold(WPARAM wParam, LPARAM lParam);
	//放弃按钮
	LRESULT OnGiveUp(WPARAM wParam, LPARAM lParam);
	//要牌按钮
	LRESULT OnGetCard(WPARAM wParam, LPARAM lParam);
	//停牌按钮
	LRESULT OnStopCard(WPARAM wParam, LPARAM lParam);
	//双倍按钮
	LRESULT OnDoubleCard(WPARAM wParam, LPARAM lParam);
	//分牌按钮
	LRESULT OnCutCard(WPARAM wParam, LPARAM lParam);
	
	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
