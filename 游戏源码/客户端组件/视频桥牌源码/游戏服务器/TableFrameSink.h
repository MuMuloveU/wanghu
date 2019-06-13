#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////

//游戏桌子类
class CTableFrameSink : public ITableFrameSink//, public ITableFrameEvent
{
	//逻辑变量
protected:
	BYTE							m_cbPackCount;						//牌副数目
	BYTE							m_cbMainColor;						//主牌花色
	int								m_cbJuKuang;						//局况
	BYTE							m_cbMultiples;						//倍数

	//连局信息
protected:
	BYTE							m_cbValueOrder[2];					//数值等级

	//游戏变量
protected:
	WORD							m_wBankerUser;						//庄家用户
	WORD							m_wCurrentUser;						//当前玩家
	BYTE							m_bLandScore;						//庄家分数
	WORD							m_wFirstUser;						//首叫用户
	BYTE							m_bPlayStatus[GAME_PLAYER];			//游戏状态
	bool							bHandOutFlag[4];					//明手控制

protected:
	//叫分信息
	BYTE							m_bScoreInfo[GAME_PLAYER][50];				//叫分信息
	BYTE							m_bColorInfo[GAME_PLAYER][50];				//花色信息

	//叫牌信息
protected:
	BYTE							m_cbCallCard;						//叫牌扑克
	BYTE							m_cbCallCount;						//叫牌数目
	WORD							m_wCallCardUser;					//叫牌用户

	//状态变量
protected:
	bool							m_bCallCard[4];						//叫牌标志
	BYTE							m_cbScoreCardCount;					//扑克数目
	BYTE							m_cbScoreCardData[12*2];			//得分扑克

	//出牌变量
protected:
	WORD							m_wTurnWinner;						//胜利玩家
	WORD							m_wFirstOutUser;					//出牌用户
	BYTE							m_cbOutCardCount[4];				//出牌数目
	BYTE							m_cbOutCardData[4][MAX_COUNT];		//出牌列表

	//底牌扑克
protected:
	BYTE							m_cbConcealCount;					//暗藏数目
	BYTE							m_cbConcealCard[8];					//暗藏扑克

	//用户扑克
protected:
	BYTE							m_cbHandCardCount[4];				//扑克数目
	BYTE							m_cbHandCardData[4][MAX_COUNT];		//手上扑克

	//贴点计算扑克
protected:
	BYTE							m_cbTieCardCount[4];				//贴点扑克数目
	BYTE							m_cbTieCardData[4][MAX_COUNT];		//贴点扑克数据

	//组件变量
protected:
	CGameLogic						m_GameLogic;						//游戏逻辑
	ITableFrame						* m_pITableFrame;					//框架接口
	const tagGameServiceOption		* m_pGameServiceOption;				//配置参数

	//属性变量
protected:
	static const WORD				m_wPlayerCount;						//游戏人数
	static const enStartMode		m_GameStartMode;					//开始模式

	//函数定义
public:
	//构造函数
	CTableFrameSink();
	//析构函数
	virtual ~CTableFrameSink();

	//基础接口
public:
	//释放对象
	virtual VOID __cdecl Release() {  }
	//是否有效
	virtual bool __cdecl IsValid() { return AfxIsValidAddress(this,sizeof(CTableFrameSink))?true:false; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//管理接口
public:
	//初始化
	virtual bool __cdecl InitTableFrameSink(IUnknownEx * pIUnknownEx);
	//复位桌子
	virtual void __cdecl RepositTableFrameSink();

	//信息接口
public:
	//开始模式
	virtual enStartMode __cdecl GetGameStartMode();
	//游戏状态
	virtual bool __cdecl IsUserPlaying(WORD wChairID);

	//游戏事件
public:
	//游戏开始
	virtual bool __cdecl OnEventGameStart();
	//游戏结束
	virtual bool __cdecl OnEventGameEnd(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//发送场景
	virtual bool __cdecl SendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);

	//事件接口
public:
	//定时器事件
	virtual bool __cdecl OnTimerMessage(WORD wTimerID, WPARAM wBindParam);
	//游戏消息处理
	virtual bool __cdecl OnGameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//框架消息处理
	virtual bool __cdecl OnFrameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//请求事件
public:
	//请求同意
	virtual bool __cdecl OnEventUserReqReady(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	//请求断线
	virtual bool __cdecl OnEventUserReqOffLine(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	//请求重入
	virtual bool __cdecl OnEventUserReqReConnect(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	//请求坐下
	virtual bool __cdecl OnEventUserReqSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bReqLookon) { return true; }
	//请求起来
	virtual bool __cdecl OnEventUserReqStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bReqLookon) { return true; }

	//动作事件
public:
	//用户同意
	virtual bool __cdecl OnEventUserReady(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	//用户断线
	virtual bool __cdecl OnEventUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	//用户重入
	virtual bool __cdecl OnEventUserReConnect(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	//用户坐下
	virtual bool __cdecl OnEventUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser) { return true; }
	//用户起来
	virtual bool __cdecl OnEventUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);

	//游戏事件
protected:
	//用户出牌
	bool OnUserOutCard(WORD wChairID, BYTE cbCardData[], BYTE cbCardCount, bool bIsHandOut[],bool bIsAutoOut);
	//叫分事件
	bool OnUserLandScore(WORD wChairID, BYTE bBossScore,BYTE bBossColor);
	//加倍事件
	bool OnUserMultiples(WORD wChairID,BYTE bMultiples);
	//所叫次数
	BYTE GetScoreNum(WORD wChariID);
	//辅助函数
protected:
	//派发扑克
	bool DispatchUserCard();
	//设置局况
	bool SetJuKuang(WORD wFirstUser);
	//得到大牌点数
	BYTE GetBigCardPoint(WORD wFirstUser,WORD wNextUser);
};

//////////////////////////////////////////////////////////////////////////

#endif