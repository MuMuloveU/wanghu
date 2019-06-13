#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////

//游戏桌子
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//辅助变量
protected:
	DWORD							m_dwPlayTimeCount;					//游戏时间

	//离开请求
protected:
	BYTE							m_cbResponses[GAME_PLAYER];			//回应状态
	WORD							m_wRequestTimes[GAME_PLAYER];		//请求离开

	//游戏变量
protected:
	WORD							m_wHeadUser;						//首出用户
	WORD							m_wCurrentUser;						//当前玩家
	BYTE							m_cbContinue[GAME_PLAYER];			//继续标志
	WORD							m_wPersistInfo[GAME_PLAYER][2];		//游戏信息
	bool							m_bAutoPlayer[4];	
	int								m_TurnScore;
	int								m_PlayerScore[4];

	//游戏等级
protected:
	WORD							m_wOrderIndex;						//等级索引
	BYTE							m_cbMainValue;						//主牌数值
	BYTE							m_cbValueOrder[GAME_PLAYER];		//等级数值

	//胜利信息
protected:
	WORD							m_wWinCount;						//胜利人数
	WORD							m_wWinOrder[GAME_PLAYER];			//胜利列表

	//出牌信息
protected:
	WORD							m_wTurnWiner;						//胜利玩家
	BYTE							m_cbTurnCardType;					//扑克类型
	BYTE							m_cbTurnCardCount;					//出牌数目
	BYTE							m_cbTurnCardData[MAX_COUNT];		//出牌数据
	BYTE							m_cbMagicCardData[MAX_COUNT];		//变幻扑克

	//扑克信息
protected:
	BYTE							m_cbHandCardCount[GAME_PLAYER];		//扑克数目
	BYTE							m_cbHandCardData[GAME_PLAYER][MAX_COUNT];	//手上扑克

	//组件变量
protected:
	CGameLogic						m_GameLogic;						//游戏逻辑

	//组件接口
protected:
	ITableFrame	*					m_pITableFrame;						//框架接口
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
	//配置桌子
	virtual bool __cdecl InitTableFrameSink(IUnknownEx * pIUnknownEx);
	//复位桌子
	virtual VOID __cdecl RepositTableFrameSink();

	//游戏事件
public:
	//游戏开始
	virtual bool __cdecl OnEventGameStart();
	//游戏结束
	virtual bool __cdecl OnEventGameEnd(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//发送场景
	virtual bool __cdecl SendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);

	//信息接口
public:
	//开始模式
	virtual enStartMode __cdecl GetGameStartMode();
	//游戏状态
	virtual bool __cdecl IsUserPlaying(WORD wChairID) { return true; }

	//事件接口
public:
	//定时器事件
	virtual bool __cdecl OnTimerMessage(WORD wTimerID, WPARAM wBindParam);
	//游戏消息处理
	virtual bool __cdecl OnGameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//框架消息处理
	virtual bool __cdecl OnFrameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//用户事件
public:
	//用户断线
	virtual bool __cdecl OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	//用户重入
	virtual bool __cdecl OnActionUserReConnect(WORD wChairID,IServerUserItem * pIServerUserItem) { return true; }
	//用户坐下
	virtual bool __cdecl OnActionUserSitDown(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户起立
	virtual bool __cdecl OnActionUserStandUp(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户同意
	virtual bool __cdecl OnActionUserReady(WORD wChairID,IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize) { return true; }

	//游戏事件
protected:
	//用户出牌
	bool OnUserOutCard(WORD wChairID, BYTE cbCardData[], BYTE cbCardCount);
	//用户放弃
	bool OnUserPassCard(WORD wChairID);
	//继续游戏
	bool OnUserContinueGame(WORD wChairID);
};

//////////////////////////////////////////////////////////////////////////

#endif