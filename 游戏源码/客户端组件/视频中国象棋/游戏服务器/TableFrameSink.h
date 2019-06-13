#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////

//游戏桌子类
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//规则变量
protected:
	bool							m_bGameRule;						//规则标志
	WORD							m_wRuleRoundID;						//规则标识
	tagGameRuleInfo					m_GameRuleInfo;						//游戏规则

	//用户状态
protected:
	WORD							m_wBankerUser;						//庄家玩家
	WORD							m_wCurrentUser;						//当前用户
	WORD							m_wLeaveDrawTime[GAME_PLAYER];		//剩余局时

	//请求状态
protected:
	BYTE							m_cbConcludeReason;					//结束原因
	BYTE							m_cbPeaceCount[GAME_PLAYER];		//求和次数
	BYTE							m_cbRegretCount[GAME_PLAYER];		//悔棋次数
	BYTE							m_cbRequestStatus[GAME_PLAYER];		//请求状态

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
	virtual bool __cdecl SendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);

	//事件接口
public:
	//定时器事件
	virtual bool __cdecl OnTimerMessage(WORD wTimerID, WPARAM wBindParam);
	//游戏消息
	virtual bool __cdecl OnGameMessage(WORD wSubCmdID,const VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//框架消息
	virtual bool __cdecl OnFrameMessage(WORD wSubCmdID,const VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//动作事件
public:
	//用户断线
	virtual bool __cdecl OnActionUserOffLine(WORD wChairID,IServerUserItem * pIServerUserItem) { return true; }
	//用户重入
	virtual bool __cdecl OnActionUserReConnect(WORD wChairID,IServerUserItem * pIServerUserItem) { return true; }
	//用户坐下
	virtual bool __cdecl OnActionUserSitDown(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户起来
	virtual bool __cdecl OnActionUserStandUp(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户同意
	virtual bool __cdecl OnActionUserReady(WORD wChairID,IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize);

	//游戏消息
protected:
	//移动棋子
	bool OnUserMoveChess(WORD wChairID, BYTE cbXSourcePos, BYTE cbYSourcePos, BYTE cbXTargetPos, BYTE cbYTargetPos, WORD wUserTimeCount);
	//和棋请求
	bool OnUserPeaceRequest(WORD wChairID);
	//和棋回应
	bool OnUserPeaceRespond(WORD wChairID, BYTE cbApprove);
	//悔棋请求
	bool OnUserRegretRequest(WORD wChairID);
	//悔棋回应
	bool OnUserRegretRespond(WORD wChairID, BYTE cbApprove);
	//结束请求
	bool OnUserConcludeRequest(WORD wChairID, BYTE cbReason);

	//辅助函数
protected:
	//积分计算
	bool CalculateGameScore(WORD wWinnerUser, LONGLONG lGameScore[GAME_PLAYER]);
};

//////////////////////////////////////////////////////////////////////////

#endif
