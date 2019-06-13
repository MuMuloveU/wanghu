#pragma once

#include "Stdafx.h"
#include "..\\游戏服务器\\GameLogic.h"
#include "GameClientView.h"
#include "IDataCtrl.h"

//////////////////////////////////////////////////////////////////////////

//游戏对话框
class CGameClientDlg : public CGameFrameDlg, public IDataCtrl
{
	//游戏变量
protected:
	bool							m_bChiHu;							//吃胡标志
	WORD							m_wHeapHand;						//堆立头部
	WORD							m_wHeapTail;						//堆立尾部
	WORD							m_wBankerUser;						//庄家用户
	WORD							m_wCurrentUser;						//当前用户
	BYTE							m_bLeftCardCount;					//剩余数目
	LONG							m_lScoreLeft[4];					//剩余财富
	LONG							m_lCellScore;						//单位注额
	WORD							m_wSiceCount;						//骰子数目

	//出牌信息
protected:
	BYTE							m_bOutCardCount;					//出牌数目
	WORD							m_wOutCardUser[108];				//出牌用户
	BYTE							m_cbOutCardData[108];				//出牌扑克

	//组合扑克
protected:
	BYTE							m_bWeaveCount[4];					//组合数目
	tagWeaveItem					m_WeaveItemArray[4][4];				//组合扑克

	//扑克变量
protected:
	BYTE							m_bHeapCardInfo[4][2];				//堆牌信息
	BYTE							m_cbCardIndex[MAX_INDEX];			//手中扑克

	//财神弟弟相关变量
	WORD							m_wKingBrotherHeapPos;
	WORD							m_wKingBrotherCount;

	//控件变量
protected:
	CGameLogic						m_GameLogic;						//游戏逻辑
	CGameClientView					m_GameClientView;					//游戏视图

	//属性变量
protected:
	bool							m_bSelfReady;						//是否自定义ready
	bool							m_bReady[4];						//软ready
	BYTE							m_cbOverTimeCount;					//超时次数
	bool							m_bConsign;							//是否托管
	
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

	//IDataCtrl
public:
	//获取游戏状态
	virtual BYTE	GetGameStatus(){ return GetGameStatus(); }	
	//获取庄家
	virtual WORD	GetBankerUser(){ return m_wBankerUser; }
	//获取王牌
	virtual BYTE	GetKingCardData(){ return m_GameLogic.GetKingCardData(); };	
	//获取财神的弟弟
	virtual BYTE	GetKingBrotherData(){ return m_GameLogic.GetBrotherFromKing(m_GameLogic.GetKingCardData());	}
	//是否准备
	virtual bool	IsUserReady(WORD wChairID);		
	//是否托管
	virtual bool	IsConsign(){ return m_bConsign;	}
	//获取剩余财富
	virtual LONG    GetScoreLeft(WORD wChairID){ return m_lScoreLeft[wChairID]; }
	//获取底注
	virtual LONG	GetCellScore(){ return m_lCellScore; }
	//是否旁观者
	virtual bool	IsLookOn(){ return IsLookonMode();	}
	//是否允许旁观
	virtual bool	IsAllowLookOn(){ return CGameFrameDlg::IsAllowUserLookon(); }
	//是否可以旁观
	virtual bool	IsEnableLookOn(){ return CGameFrameDlg::IsAllowLookon(); }
	//是否允许声效
	virtual bool	IsEnableSound(){ return CGameFrameDlg::IsEnableSound(); }

	//消息处理
protected:
	//游戏开始
	bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
	//用户出牌
	bool OnSubOutCard(const void * pBuffer, WORD wDataSize);
	//操作扑克
	bool OnSubOperateCard(const void * pBuffer, WORD wDataSize);
	//发送扑克
	bool OnSubSendCard(const void * pBuffer, WORD wDataSize);
	//游戏结束
	bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);
	//系统消息
	bool OnSubSysMsg(const void * pBuffer, WORD wDataSize);
	//玩家准备
	bool OnSubUserReady(const void * pBuffer, WORD wDataSize);
	//注额改变
	bool OnSubStake(const void * pBuffer, WORD wDataSize);

	//辅助函数
protected:
	//扣除扑克
	void DeductionTableCard(bool bHeadCard);
	//手牌信息
	void SetHandCardData(BYTE cbCardData[], WORD wCardCount, BYTE cbCurrentCard);
	//手牌之当前牌
	void SetHandCurrentCard(BYTE cbCurrentCard);
	//智能打牌
	BYTE GetIsolatedCard();
	//设置托管
	void SetConsign(bool bEnter);
	//设置定时器
	bool SetGameTimer(WORD wChairID, UINT nTimerID, UINT nElapse);
	//播放出牌声音
	void PlayOutCardSound(WORD wChairID, BYTE cbCardData);
	//播放其他声音
	void PlayChairSound(WORD wChairID, LPCSTR lpstrName);
	//DEBUG
	void _DebugAssert();

	//消息映射
protected:
	//开始消息
	LRESULT OnStart(WPARAM wParam, LPARAM lParam);
	//出牌操作
	LRESULT OnOutCard(WPARAM wParam, LPARAM lParam);
	//扑克操作
	LRESULT OnCardOperate(WPARAM wParam, LPARAM lParam);
	//托管操作
	LRESULT OnConsignChanged(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
