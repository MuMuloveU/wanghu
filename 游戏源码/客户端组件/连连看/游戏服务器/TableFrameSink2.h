#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "..\消息定义\CMD_LLShow.h"
#include "..\消息定义\logic.h"

//结束原因
#define GER_NO_PLAYER						0x10						//没有玩家

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////

//游戏桌子类
class CTableFrameSink : public ITableFrameSink
{
	//辅助变量
	BYTE							m_bChess[GAME_PLAYER][CHESSCOUNT_H][CHESSCOUNT_W];//每个玩家的当前棋面状态
	BYTE							m_bPropCnt[GAME_PLAYER][PROP_MAX_CNT];				  //道具
protected:		
	BYTE							m_bPlayStatus[GAME_PLAYER];				//游戏状态
	BYTE							m_bTimerCnt[GAME_PLAYER];				//当前玩家的定时器时间
	BYTE							m_bSeriesLinkCnt[GAME_PLAYER];			//连击次数统计
tagScoreInfo						m_ScoreInfo[GAME_PLAYER];				//在游戏得到的分数
	
	//组件变量
protected:
	CLogic							m_GameLogic;							//游戏逻辑
	ITableFrame						* m_pITableFrame;						//框架接口

	bool							m_bIsGameEnd;




//////////////////////////////////////////////////////////////////////////////////////
	int								RandNumber(int nMin,int nMax);
	void							InitVariable();							//初始化参数

	bool							GameEnd(WORD wSeat,BYTE bEndReason);

	// 网络消息
	bool							SubGameLost(WORD wSeat);

	//函数定义
public:
	//构造函数
	CTableFrameSink();
	//析构函数
	virtual ~CTableFrameSink();

	//基础接口
public:
	//释放对象
	virtual bool __cdecl Release() { if (IsValid()) delete this; return true; }
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
};
//static void RetailOutput(TCHAR *szFormat, ...);
//////////////////////////////////////////////////////////////////////////

#endif