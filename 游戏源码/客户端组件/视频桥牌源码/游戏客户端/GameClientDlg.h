#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "GameClientView.h"
#include "CallScore.h"


//////////////////////////////////////////////////////////////////////////

//游戏对话框
class CGameClientDlg : public CGameFrameDlg
{
	//逻辑变量
protected:
	BYTE							m_cbPackCount;						//牌副数目
	BYTE							m_cbMainColor;						//主牌花色
	BYTE							m_cbMultiples;						//游戏倍数
	BYTE							m_cbJuKuang;						//局况
	BYTE							m_cbMainValue;                      //主牌数值

protected:
	BYTE							m_cbValueOrder[2];					//数值等级

	//叫牌信息
protected:
	BYTE							m_cbCallCard;						//叫牌扑克
	BYTE							m_cbCallCount;						//叫牌数目
	WORD							m_wCallCardUser;					//叫牌用户

	//状态变量
protected:
	WORD							m_wBankerUser;						//庄家用户
	WORD							m_wCurrentUser;						//当前用户

	//得分信息
protected:
	BYTE							m_cbScoreCardCount;					//分牌数目
	BYTE							m_cbScoreCardData[12*2];		    //得分扑克

	//甩牌变量
protected:
	bool							m_bThrowCard;						//甩牌标志
	WORD							m_wThrowCardUser;					//甩牌用户
	BYTE							m_cbResultCardCount;				//还原扑克
	BYTE							m_cbResultCardData[MAX_COUNT];		//还原扑克

	//出牌变量
protected:
	bool							m_bLastTurn;						//上轮标志
	WORD							m_wFirstOutUser;					//出牌用户
	BYTE							m_cbOutCardCount[4][2];				//出牌数目
	BYTE							m_cbOutCardData[4][2][MAX_COUNT];	//出牌列表

	//底牌扑克
protected:
	BYTE							m_cbConcealCount;					//暗藏数目
	BYTE							m_cbConcealCard[8];					//暗藏扑克

	//用户扑克
protected:
	BYTE							m_cbHandCardCount;					//扑克数目
	BYTE							m_cbHandCardData[MAX_COUNT];		//手上扑克

	BYTE							m_HandOutCardCount[4];				//扑克数量
	BYTE							m_HandOutCardData[4][MAX_COUNT];	//明手扑克

	//配置变量
protected:
	bool							m_bStustee;							//托管标志
	bool							m_bDeasilOrder;						//出牌顺序
	DWORD							m_dwCardHSpace;						//扑克象素

	//辅助变量
protected:
	BYTE							m_cbRemnantCardCount;				//剩余数目
	BYTE							m_cbDispatchCardCount;				//派发数目
	BYTE							m_cbDispatchCardData[MAX_COUNT];	//派发扑克
	WORD							m_cbFocusUser;						//add
	bool							m_bHandOutControl;					//明手控制


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
	//发送扑克
	bool OnSubSendCard(const void * pBuffer, WORD wDataSize);
	//游戏开始
	bool OnSubGamePlay(const void * pBuffer, WORD wDataSize);
	//用户出牌
	bool OnSubOutCard(const void * pBuffer, WORD wDataSize);
	//甩牌结果
	bool OnSubThrowResult(const void * pBuffer, WORD wDataSize);
	//一轮结算
	bool OnSubTurnBalance(const void * pBuffer, WORD wDataSize);
	//游戏结束
	bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);
	//用户叫分
	bool OnSubCallScore(const void * pBuffer, WORD wDataSize);

	//功能函数
protected:
	//出牌判断
	bool VerdictOutCard();
	//明手出牌判断
	bool VerdictHandOutCard();
	//自动出牌
	bool AutomatismOutCard();
	//当前扑克
	bool SwitchToCurrentCard();
	//甩牌结果
	bool SwitchToThrowResult();
	//派发扑克
	bool DispatchUserCard(BYTE cbCardData[], BYTE cbCardCount);
	//设置扑克
	bool SetHandCardControl(BYTE cbCardData[], BYTE cbCardCount);
	//设置明手扑克
	bool SetHandOutCardControl(BYTE cbCardData[], BYTE cbCardCount, WORD wMeChairID, WORD wViewChairID);

	//界面函数
protected:

	//消息映射
protected:
	//定时器消息
	afx_msg void OnTimer(UINT nIDEvent);
	//开始消息
	LRESULT OnStart(WPARAM wParam, LPARAM lParam);
	//出牌消息
	LRESULT OnOutCard(WPARAM wParam, LPARAM lParam);
	//出牌提示
	LRESULT OnOutPrompt(WPARAM wParam, LPARAM lParam);
	//请求离开
	LRESULT OnRuquestLeave(WPARAM wParam, LPARAM lParam);
	//上轮扑克
	LRESULT OnLastTurnCard(WPARAM wParam, LPARAM lParam);
	//拖管控制
	LRESULT OnStusteeControl(WPARAM wParam, LPARAM lParam);
	//左键扑克
	LRESULT OnLeftHitCard(WPARAM wParam, LPARAM lParam);
	//右键扑克
	LRESULT OnRightHitCard(WPARAM wParam, LPARAM lParam);
	//叫分
	LRESULT OnLandScore(WPARAM wParam, LPARAM lParam);
	//加倍
	LRESULT OnMultiples(WPARAM wParam, LPARAM lParam);
	//放弃
	LRESULT OnGiveUpScore(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
