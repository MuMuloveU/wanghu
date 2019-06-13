#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////

//游戏桌子类
class CTableFrameSink : public ITableFrameSink
{
	//游戏变量
protected:
	WORD							m_wBankerUser;							//庄家用户
	WORD							m_wCurrentUser;							//当前玩家
	BYTE                            m_wPassCount;							 //pass用户的人数
	BYTE                            m_bResidualCardCount;                  //明牌数
	bool                            m_bAnyFriendEnd;						//有玩家打完扑克
	BYTE							m_bTempCardData[54];					//用来发牌和补牌的扑克
	//用户分组
protected:
	CArray<WORD,WORD>               m_UserScoreCard[GAME_PLAYER];			 //玩家得分
	CArray<WORD,WORD>               m_bOneTurnScoreCard;					 //不可能成为独食的用户组
	CArray<WORD,WORD>               m_wEndUsers;							 //结束用户组
	//赢家标志
	int                            m_bWinKind;								 //输赢的情况 
	//历史积分
protected:
	LONG                            m_lAllTurnScore[GAME_PLAYER];		//总局得分
	LONG                            m_lLastTurnScore[GAME_PLAYER];		//上局得分

	//运行信息
protected:
	BYTE							m_bCount1[4];						//出一的数
	BYTE							m_bCount2[4];						//出二的数
	BYTE							m_bTurnCardCount;					//出牌数目
	BYTE							m_bTurnCardData[10];				//出牌列表


	//扑克信息
protected:
	BYTE							m_bCardCount[GAME_PLAYER];			//扑克数目
	BYTE							m_bHandCardData[GAME_PLAYER][10];	//手上扑克

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
	virtual void __cdecl Release() {  }
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

	//游戏事件
protected:
	//设置服务器时间
	void SetGameTimer(WORD wTimerID,UINT nElapse);
	//删出定时器
	void KillGameTimer(WORD wTimerID);
	//发牌
	void SendCard();
	//用户出牌
	bool OnUserOutCard(WORD wChairID, BYTE bCardData[], BYTE bCardCount);
	//用户放弃
	bool OnUserPassCard(WORD wChairID);
protected:
	//游戏是否结束
	bool IsGameOver();
	//如果是明牌让自己的对友接风
	WORD OnGetMeFriend(WORD wChairID);
	//判断哪个组用户获胜
	int GetSpecialWinKind();
	//补牌
	void RecruitCard(WORD wChairID);

};

//////////////////////////////////////////////////////////////////////////

#endif