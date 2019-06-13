#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////

//游戏桌子类
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//玩家变量
protected:
	WORD							m_wDUser;								//D玩家
	WORD							m_wCurrentUser;							//当前玩家

	//玩家状态
protected:
	BYTE							m_cbPlayStatus[GAME_PLAYER];			//游戏状态

	//加注信息
protected:
	LONG							m_lCellScore;							//单元下注
	LONG							m_lTurnLessScore;						//最小下注
	LONG							m_lAddLessScore;						//加最小注
	LONG							m_lTurnMaxScore;						//最大下注
	LONG							m_lBalanceScore;						//平衡下注
	WORD							m_wOperaCount;							//操作次数
	BYTE							m_cbBalanceCount;						//平衡次数
	LONG							m_lTableScore[GAME_PLAYER];				//桌面下注
	LONG							m_lTotalScore[GAME_PLAYER];				//累计下注
	LONG							m_lUserMaxScore[GAME_PLAYER];			//最大下注
	BYTE							m_cbShowHand[GAME_PLAYER];				//梭哈用户

	//税收变量
protected:
	//LONG							m_bUserTax[GAME_PLAYER];				//用户税收,若不用处理界面,可不用保存
	//LONG							m_bLastTax[GAME_PLAYER];				//最后税收

	//扑克信息
protected:
	BYTE							m_cbSendCardCount;						//发牌数目
	BYTE							m_cbCenterCardData[MAX_CENTERCOUNT];	//中心扑克
	BYTE							m_cbHandCardData[GAME_PLAYER][MAX_COUNT];//手上扑克

	//组件变量
protected:
	CGameLogic						m_GameLogic;							//游戏逻辑
	ITableFrame						* m_pITableFrame;						//框架接口
	const tagGameServiceOption		* m_pGameServiceOption;					//配置参数

	//属性变量
protected:
	static const WORD				m_wPlayerCount;							//游戏人数
	static const enStartMode		m_GameStartMode;						//开始模式

	//函数定义
public:
	//构造函数
	CTableFrameSink();
	//析构函数
	virtual ~CTableFrameSink();

	//基础接口
public:
	//释放对象
	virtual VOID __cdecl Release() { }
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

	//动作事件 
public:
	//用户断线
	virtual bool __cdecl OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	//用户重入
	virtual bool __cdecl OnActionUserReConnect(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	//用户坐下
	virtual bool __cdecl OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户起来
	virtual bool __cdecl OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户同意
	virtual bool __cdecl OnActionUserReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize) { return true; }

	//游戏事件
public:
	//游戏开始
	virtual bool __cdecl OnEventGameStart();
	//游戏结束
	virtual bool __cdecl OnEventGameEnd(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//发送场景
	virtual bool __cdecl SendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);

	//消息事件
protected:
	//放弃事件
	bool OnUserGiveUp(WORD wChairID);
	//加注事件
	bool OnUserAddScore(WORD wChairID, LONG lScore, bool bGiveUp);

	//事件接口
public:
	//定时器事件
	virtual bool __cdecl OnTimerMessage(WORD wTimerID, WPARAM wBindParam);
	//游戏消息处理
	virtual bool __cdecl OnGameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//框架消息处理
	virtual bool __cdecl OnFrameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);
};

//////////////////////////////////////////////////////////////////////////

#endif