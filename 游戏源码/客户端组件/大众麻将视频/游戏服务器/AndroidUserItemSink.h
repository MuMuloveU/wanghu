#ifndef ANDROID_USER_ITEM_SINK_HEAD_FILE
#define ANDROID_USER_ITEM_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////

//机器人类
class CAndroidUserItemSink : public IAndroidUserItemSink
{
	//用户变量
protected:
	WORD							m_wBankerUser;							//庄家用户
	WORD							m_wCurrentUser;							//当前用户

	//状态变量
protected:
	bool							m_bHearStatus;							//听牌状态
	bool							m_bWillHearStatus;						//即将听牌
	BYTE							m_bActionMask;							//动作掩码
	BYTE							m_cbActionCard;							//动作扑克	

	//扑克变量
protected:
	BYTE							m_cbLeftCardCount;						//剩余数目
	BYTE							m_cbCardIndex[GAME_PLAYER][MAX_INDEX];	//手中扑克
	BYTE							m_cbRepertoryCard[MAX_REPERTORY];		//库存扑克

	//组合扑克
protected:
	BYTE							m_cbWeaveCount[GAME_PLAYER];			//组合数目
	tagWeaveItem					m_WeaveItemArray[GAME_PLAYER][4];		//组合扑克

	//出牌信息
protected:
	WORD							m_wOutCardUser;							//出牌用户
	BYTE							m_cbOutCardData;						//出牌扑克
	BYTE							m_cbDiscardCount[GAME_PLAYER];			//丢弃数目
	BYTE							m_cbDiscardCard[GAME_PLAYER][60];		//丢弃记录
	BYTE							m_cbSendCardData;						//发送扑克

	//控件变量
protected:
	CGameLogic						m_GameLogic;							//游戏逻辑
	IAndroidUserItem *				m_pIAndroidUserItem;					//用户接口


	//函数定义
public:
	//构造函数
	CAndroidUserItemSink();
	//析构函数
	virtual ~CAndroidUserItemSink();

	//基础接口
public:
	//释放对象
	virtual VOID __cdecl Release() { }
	//是否有效
	virtual bool __cdecl IsValid() { return AfxIsValidAddress(this,sizeof(CAndroidUserItemSink))?true:false; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//控制接口
public:
	//初始接口
	virtual bool __cdecl InitUserItemSink(IUnknownEx * pIUnknownEx);
	//重置接口
	virtual bool __cdecl RepositUserItemSink();

	//游戏事件
public:
	//时间消息
	virtual bool __cdecl OnEventTimer(UINT nTimerID);
	//游戏消息
	virtual bool __cdecl OnEventGameMessage(WORD wSubCmdID, void * pData, WORD wDataSize);
	//游戏消息
	virtual bool __cdecl OnEventFrameMessage(WORD wSubCmdID, void * pData, WORD wDataSize);
	//场景消息
	virtual bool __cdecl OnEventGameScene(BYTE cbGameStatus, bool bLookonOther, void * pData, WORD wDataSize);

	//用户事件
public:
	//用户进入
	virtual void __cdecl OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户离开
	virtual void __cdecl OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户积分
	virtual void __cdecl OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户状态
	virtual void __cdecl OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户段位
	virtual void __cdecl OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);

	//消息处理
protected:
	//庄家信息
	bool OnSubGameStart(void * pData, WORD wDataSize);
	//游戏结束
	bool OnSubGameEnd(void * pData, WORD wDataSize);
	//用户出牌
	bool OnSubOutCard(const void * pBuffer, WORD wDataSize);
	//发牌消息
	bool OnSubSendCard(const void * pBuffer, WORD wDataSize);
	//操作提示
	bool OnSubOperateNotify(const void * pBuffer, WORD wDataSize);
	//操作结果
	bool OnSubOperateResult(const void * pBuffer, WORD wDataSize);
	//用户听牌
	bool OnSubListenCard(const void * pBuffer, WORD wDataSize);
	//校验出牌
	bool VerdictOutCard(BYTE cbPreCardData);
	//处理函数
protected:
	//出牌操作
	void OnOutCard( BYTE cbOutCard );
	//扑克操作
	void OnOperateCard( BYTE cbOperateCode,BYTE cbOperateCard );
	//智能出牌
	BYTE GetIsolatedCard();
	//孤牌数目
	BYTE GetIsolatedCardCount(BYTE cbCardIndex[MAX_INDEX]);
	//最优操作
	BYTE GetTheBestOperate(BYTE cbOperateCode,BYTE cbOperateCard);

};

//////////////////////////////////////////////////////////////////////////

#endif