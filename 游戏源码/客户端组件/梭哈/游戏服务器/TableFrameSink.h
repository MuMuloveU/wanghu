#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "AfxTempl.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////

//结束原因
#define GER_NO_PLAYER						0x10						//没有玩家

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
class CTableFrameSink : public ITableFrameSink
{
	//游戏变量
protected:
	WORD							m_wOperaCount;							//操作次数
	WORD							m_wCurrentUser;							//当前用户
	BYTE							m_bSendCardCount;						//发牌数目
	BYTE							m_cbCardCount[GAME_PLAYER];				//扑克数目
	BYTE							m_cbHandCardData[GAME_PLAYER][5];		//桌面扑克
	BYTE							m_cbGameEndReason;						//游戏结束理由

	//金币信息
protected:
	LONG							m_lMaxGold;								//最大下注
	LONG							m_lBasicGold;							//单元下注
	LONG							m_lTurnBasicGold;						//最少下注
	LONG							m_lTableGold[2*GAME_PLAYER];			//下注金币

	//辅助变量
protected:
	BYTE							m_cbPlayStatus[GAME_PLAYER];			//游戏状态

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

	//游戏事件
protected:
	//放弃事件
	bool OnUserGiveUp(WORD wChairID);
	//加注事件
	bool OnUserAddGold(WORD wChairID, LONG lGold, bool bCancel);
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