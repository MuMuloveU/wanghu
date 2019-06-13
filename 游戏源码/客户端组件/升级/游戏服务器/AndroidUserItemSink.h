#ifndef ANDROID_USER_ITEM_SINK_HEAD_FILE
#define ANDROID_USER_ITEM_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////

//请求离开用户
typedef CArrayTemplate<WORD>	RequestLeaveUserAry;

//机器人类
class CAndroidUserItemSink : public IAndroidUserItemSink
{
	//逻辑变量
protected:
	BYTE							m_cbPackCount;						//牌副数目
	BYTE							m_cbMainColor;						//主牌花色
	BYTE							m_cbMainValue;						//主牌数值

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
	BYTE							m_cbScoreCardData[12*PACK_COUNT];	//得分扑克

	//出牌变量
protected:
	WORD							m_wFirstOutUser;					//出牌用户
	BYTE							m_cbOutCardCount[4];				//出牌数目
	BYTE							m_cbOutCardData[4][MAX_COUNT];		//出牌列表

	//底牌扑克
protected:
	BYTE							m_cbConcealCount;					//暗藏数目
	BYTE							m_cbConcealCard[8];					//暗藏扑克

	//请求离开用户
	RequestLeaveUserAry				m_RequestUserAry;					

	//用户扑克
protected:
	BYTE							m_cbHandCardCount;					//扑克数目
	BYTE							m_cbHandCardData[MAX_COUNT];		//手上扑克	

	//控件变量
protected:
	CGameLogic						m_GameLogic;						//游戏逻辑
	IAndroidUserItem *				m_pIAndroidUserItem;				//用户接口

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
	//游戏开始
	bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
	//发送扑克
	bool OnSubSendCard(const void * pBuffer, WORD wDataSize);
	//用户叫牌
	bool OnSubCallCard(const void * pBuffer, WORD wDataSize);
	//发送底牌
	bool OnSubSendConceal(const void * pBuffer, WORD wDataSize);
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
	//请求离开
	bool OnSubRequestLeave(const void * pBuffer, WORD wDataSize);
	//批量出牌
	bool OnSubBatchCard(const void * pBuffer, WORD wDataSize);

	//辅助函数
protected:
	//叫牌
	bool CallCard();
	//下底牌
	bool SendConceal();
	//出牌
	bool OutCard();
};

//////////////////////////////////////////////////////////////////////////

#endif