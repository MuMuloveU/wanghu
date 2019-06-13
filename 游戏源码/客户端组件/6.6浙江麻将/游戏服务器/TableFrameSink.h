#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"

#define IDI_DECLARE_OVERTIME		10				//抉择超时定时器
#define TIMER_BIND_WAITING			101				//等待定时器bind参数标识
#define TIME_SCORE_STAKE_TIMES		100				//赌注倍数
//////////////////////////////////////////////////////////////////////////

//游戏桌子类
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//游戏变量
protected:
	WORD							m_wSiceCount;						//骰子点数
	WORD							m_wBankerUser;						//庄家用户
	WORD							m_wCurrentUser;						//当前用户
	BYTE							m_cbCardIndex[4][MAX_INDEX];		//用户扑克

	//出牌信息
protected:
	BYTE							m_bOutCardCount;					//出牌数目
	WORD							m_wOutCardUser[136];				//出牌用户
	BYTE							m_cbOutCardData[136];				//出牌扑克

	//发牌信息
protected:
	BYTE							m_cbSendCardData;					//发牌扑克
	BYTE							m_bSendCardCount;					//发牌数目
	BYTE							m_bLeftCardCount;					//剩余数目
	BYTE							m_cbRepertoryCard[136];				//库存扑克
	BYTE							m_cbKingBrothterPos;				//财神弟弟所在位置

	//用户状态
protected:
	bool							m_bReady[4];						//ready信号
	bool							m_bChiHu[4];						//吃胡标志
	bool							m_bResponse[4];						//响应标志
	BYTE							m_bUserAction[4];					//用户动作
	BYTE							m_bOperateCard[4];					//操作扑克
	BYTE							m_bPerformAction[4];				//执行动作

	//组合扑克
protected:
	BYTE							m_bWeaveItemCount[4];				//组合数目
	tagWeaveItem					m_WeaveItemArray[4][4];				//组合扑克

	//杠牌信息
protected:
	tagGangInfo						m_LastGangInfo;						//杠牌信息

	//分数信息
protected:
	LONG							m_lCellScore;						//单位积分
	bool							m_bCellScoreChange;					//是否改变

	//结束信息
protected:
	tagHuCourt						m_HuCourt;							//胡牌详情	//慈溪麻将只允许一个人胡
	BYTE							m_cbBankerContinuCount[4];			//连庄数目
	LONG							m_lScoreLeft[4];					//剩余分数	

	//组件变量
protected:
	CGameLogic						m_GameLogic;						//游戏逻辑
	ITableFrame	*					m_pITableFrame;						//框架接口
	const tagGameServiceOption *	m_pGameServiceOption;				//配置参数

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
	virtual bool __cdecl SendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);

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
	virtual bool __cdecl OnActionUserReqReady(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	//请求断线
	virtual bool __cdecl OnActionUserReqOffLine(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	//请求重入
	virtual bool __cdecl OnActionUserReqReConnect(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	//请求坐下
	virtual bool __cdecl OnActionUserReqSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bReqLookon) { return true; }
	//请求起来
	virtual bool __cdecl OnActionUserReqStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bReqLookon) { return true; }

	//动作事件
public:
	//用户同意
	virtual bool __cdecl OnActionUserReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize);
	//用户断线
	virtual bool __cdecl OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	//用户重入
	virtual bool __cdecl OnActionUserReConnect(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	//用户坐下
	virtual bool __cdecl OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户起来
	virtual bool __cdecl OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);

	//游戏事件
protected:
	//用户出牌
	bool OnUserOutCard(WORD wChairID, BYTE cbCardData);
	//用户操作
	bool OnUserOperateCard(WORD wChairID, BYTE cbOperateCode, BYTE cbOperateCard);
	//ready信号
	bool OnUserReady(WORD wChairID);

	//辅助函数
protected:
	//结束事件
	bool	_OnGameEndNormal(WORD wChairID, IServerUserItem * pIServerUserItem);
	bool	_OnGameEndAbnormal(WORD wChairID, IServerUserItem * pIServerUserItem);
	bool	_OnGameEndDismiss();
	//检查包牌
	BYTE 	_CheckAfford(tagHuCourt& HuCourt);
	//虚拟结束
	void	_OnFakeOver();
	//发一张牌
	BYTE    _DealOneCard(bool bHead = true);
};

//////////////////////////////////////////////////////////////////////////

#endif