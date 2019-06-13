#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "GameClientView.h"

//////////////////////////////////////////////////////////////////////////

//游戏对话框
class CGameClientDlg : public CGameFrameDlg
{
	//用户变量
protected:
	WORD							m_wBankerUser;						//庄家用户
	WORD							m_wCurrentUser;						//当前用户

	//堆立变量
protected:
	WORD							m_wHeapHand;						//堆立头部
	WORD							m_wHeapTail;						//堆立尾部
	BYTE							m_cbHeapCardInfo[4][2];				//堆牌信息


	//精牌相关变量
	WORD							m_wKingHeapPos;						//精牌堆位
	WORD							m_wKingPos;							//精牌位置
	BYTE							m_cbKingCardIndex[2];				//精牌所以
	WORD							m_wSiceCount3;						//精牌筛子

	//上下番
	LONG							m_lUpDownScore[GAME_PLAYER];		//上下翻
	BYTE							m_cbFloor;							//楼层
	BYTE							m_cbUpDownKingCardData;				//上下番财神

	//托管变量
	bool							m_bStustee;							//托管标志
	WORD							m_wTimeOutCount;					//超时次数

	//扑克变量
protected:
	BYTE							m_cbLeftCardCount;					//剩余数目
	BYTE							m_cbCardIndex[GAME_PLAYER][MAX_INDEX];//手中扑克
	BYTE							m_cbCardData[MAX_COUNT];			//自己手中的扑克

	//组合扑克
protected:
	BYTE							m_cbWeaveCount[GAME_PLAYER];		//组合数目
	tagWeaveItem					m_WeaveItemArray[GAME_PLAYER][4];	//组合扑克

	//出牌信息
protected:
	WORD							m_wOutCardUser;						//出牌用户
	BYTE							m_cbOutCardData;					//出牌扑克
	BYTE							m_cbDiscardCount[GAME_PLAYER];		//丢弃数目
	BYTE							m_cbDiscardCard[GAME_PLAYER][55];	//丢弃记录
	bool                            m_bStart;                           //筛子动作是不是结束
	WORD							m_wUserAction;						//响应掩码
	bool							m_bStart2;							//筛子动作是不是结束

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
	//用户出牌
	bool OnSubOutCard(const void * pBuffer, WORD wDataSize);
	//发牌消息
	bool OnSubSendCard(const void * pBuffer, WORD wDataSize);
	//操作提示
	bool OnSubOperateNotify(const void * pBuffer, WORD wDataSize);
	//操作结果
	bool OnSubOperateResult(const void * pBuffer, WORD wDataSize);
	//游戏结束
	bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);
	//游戏洗澡
	bool OnSubGameChip(const void *pBuffer,WORD wDataSize);
	//用户托管
	bool OnSubTrustee(const void * pBuffer,WORD wDataSize);
	//洗澡结果
	bool OnSubBatchResult(const void *pBuffer,WORD wDataSize);

	//辅助函数
protected:
	//手牌信息
	void SetHandCardData(BYTE cbCardData[], WORD wCardCount, BYTE cbCurrentCard);
	//手牌之当前牌
	void SetHandCurrentCard(BYTE cbCurrentCard);
	//播放声音
	void PlayCardSound(WORD wChairID,BYTE cbCardData);
	//播放声音
	void PlayActionSound(WORD wChairID,WORD wAction);
	//播放声音
	void PlayHuSound(WORD wChairID,DWORD dwChiHuKind,DWORD dwCHiHuRight);

	//扣除扑克
	void DeductionTableCard(bool bHeadCard);
	//显示控制
	bool ShowOperateControl(WORD wUserAction, BYTE cbActionCard);
	//出牌校验
	bool VerdictOutCard(BYTE cbPreCardData);
	//消息映射
protected:
	//开始消息
	LRESULT OnStart(WPARAM wParam, LPARAM lParam);
	//出牌操作
	LRESULT OnOutCard(WPARAM wParam, LPARAM lParam);
	//扑克操作
	LRESULT OnCardOperate(WPARAM wParam, LPARAM lParam);
	//筛子动作完毕
	LRESULT OnGameStart(WPARAM wParam, LPARAM lParam);
	//筛子动作完毕
	LRESULT OnGameStart2(WPARAM wParam,LPARAM lParam);
	//筛子动作完毕
	LRESULT OnSiceTwo(WPARAM wParam, LPARAM lParam);
	//买底分数
	LRESULT OnBatch(WPARAM wParam,LPARAM lParam);
	//拖管控制
	LRESULT OnStusteeControl(WPARAM wParam, LPARAM lParam);
	//洗澡完毕
	LRESULT OnBatchEnd(WPARAM wParam,LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
