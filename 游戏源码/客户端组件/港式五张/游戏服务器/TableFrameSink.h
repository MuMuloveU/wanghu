#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "AfxTempl.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////

//用户信息
struct tagScoreHistory
{
	__int64							lScore;									//变化积分
	__int64							lWinScore;								//胜利积分
	__int64							lLoseScore;								//输出积分
	SYSTEMTIME						RecordTime;								//记录时间
};

//索引定义
typedef CMap<DWORD,DWORD,DWORD,DWORD> CBlacklistIDMap;
typedef CMap<DWORD,DWORD,tagScoreHistory *,tagScoreHistory * &>	CScoreHistoryMap;

//////////////////////////////////////////////////////////////////////////

//游戏桌子类
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//游戏变量
protected:
	bool							m_bShowHand;							//梭哈标志
	bool							m_bChaosCard;							//混乱标志
	WORD							m_wOperaCount;							//操作次数
	WORD							m_wCurrentUser;							//当前用户
	WORD							m_wLastMostUser;						//上次发牌最大用户

	//用户状态
protected:
	LONG							m_lServiceScore;						//服务费用
	BYTE							m_cbPlayStatus[GAME_PLAYER];			//游戏状态
	BYTE							m_cbOperaScore[GAME_PLAYER];			//操作标志
	LONG							m_lTableScore[GAME_PLAYER*2];			//下注数目
	LONG							m_lUserMaxScore[GAME_PLAYER];			//最大下注

	//扑克变量
protected:
	BYTE							m_cbSendCardCount;						//发牌数目
	BYTE							m_cbCardCount[GAME_PLAYER];				//扑克数目
	BYTE							m_cbHandCardData[GAME_PLAYER][5];		//桌面扑克

	//下注信息
protected:
	LONG							m_lMaxScore;							//最大下注
	LONG							m_lCellScore;							//单元下注
	LONG							m_lTurnMaxScore;						//最大下注
	LONG							m_lTurnLessScore;						//最小下注

	//组件变量
protected:
	CGameLogic						m_GameLogic;							//游戏逻辑
	ITableFrame						* m_pITableFrame;						//框架接口
	const tagGameServiceOption		* m_pGameServiceOption;					//配置参数

	//属性变量
protected:
	static const WORD				m_wPlayerCount;							//游戏人数
	static const enStartMode		m_GameStartMode;						//开始模式

	//历史记录
protected:
	static __int64					g_lMaxScore;							//最大变化
	static __int64					g_lMaxWinScore;							//最大赢分
	static __int64					g_lMaxLoseScore;						//最大输分
	static CBlacklistIDMap			g_BlacklistIDMap;						//限制用户
	static CScoreHistoryMap			g_ScoreHistoryMap;						//历史记录

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

	//游戏事件
public:
	//游戏开始
	virtual bool __cdecl OnEventGameStart();
	//游戏结束
	virtual bool __cdecl OnEventGameEnd(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//发送场景
	virtual bool __cdecl SendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE bGameStatus, bool bSendSecret);

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
	//用户断线
	virtual bool __cdecl OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem);
	//用户重入
	virtual bool __cdecl OnActionUserReConnect(WORD wChairID, IServerUserItem * pIServerUserItem);
	//用户坐下
	virtual bool __cdecl OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser){ return true; }
	//用户起来
	virtual bool __cdecl OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser){ return true; };
	//用户同意
	virtual bool __cdecl OnActionUserReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize){ return true; }

	//游戏事件
protected:
	//放弃事件
	bool OnUserGiveUp(WORD wChairID);
	//加注事件
	bool OnUserAddScore(WORD wChairID, LONG lScore, bool bGiveUp);
	//获取胜者
	bool OnUserGetWinner(IServerUserItem * pIServerUserItem);

	//游戏函数
protected:
	//推断输者
	WORD EstimateLoser(BYTE cbStartPos, BYTE cbConcludePos);
	//推断胜者
	WORD EstimateWinner(BYTE cbStartPos, BYTE cbConcludePos);
};

//////////////////////////////////////////////////////////////////////////

#endif