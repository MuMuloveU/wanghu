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
	WORD							m_wBankerUser;						//庄家用户
	WORD							m_wCurrentUser;						//当前玩家
//////////////////////////////////////////////////////////////////////////////////
	WORD							m_wChaUser;							//够牌玩家
	bool							m_bgou;								//够牌
	bool							m_bdian;							//叉牌

	//////////////////////////////////////////添加的变量////////////////////////////////////////
	bool                            m_bLiOrJue[GAME_PLAYER];            //记录是否立棍或者撅棍
	BYTE                            m_cbLiSuperCount;                   //计数器，用来记录立棍超时的人数
	bool                            m_bLiRequest[GAME_PLAYER];          //用来记录玩家是不是已经发送了立棍请求
	bool                            m_bCurPlayer[GAME_PLAYER];          //当前游戏玩家（如果玩家同意了立棍或者撅棍，那么为false）
	bool                            m_bNotLiOrJue[GAME_PLAYER];         //当玩家点了不立棍或不撅棍按钮的时候，发送这个消息
	BYTE                            m_cbNotJue;                         //计数器，用来记录不撅棍的人数
	WORD                            m_cbMiCi[GAME_PLAYER];              //名次数组，用来记录第1-4名的座位号
	BYTE                            m_cbCiXu[GAME_PLAYER];              //次序数组，用来记录第0-3号玩家的名次
	BYTE                            m_cbMingCi;                         //名次
	BYTE                            m_cbEndType;                        //结束类型
	WORD                            m_wLiGun;
	WORD                            m_wJueGun;
//////////////////////////////////////////////////////////////////////////////////
	//运行信息
protected:
	WORD							m_wTurnWiner;						//胜利玩家
	BYTE							m_bTurnCardCount;					//出牌数目
	BYTE							m_bTurnCardData[15];				//出牌列表
	bool							m_bUserTrustee[GAME_PLAYER];		//玩家托管

	//扑克信息
protected:
	BYTE							m_bCardCount[GAME_PLAYER];			//扑克数目
	BYTE							m_bOutCardCount[GAME_PLAYER];		//出牌次数
	BYTE							m_bHandCardData[GAME_PLAYER][15];	//手上扑克

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
	//叫分事件
	bool OnUserLandScore(WORD wChairID, BYTE bLandScore);
	//用户出牌
	bool OnUserOutCard(WORD wChairID, BYTE bCardData[], BYTE bCardCount);
	//用户叉牌
	bool OnUserChaCard(WORD wChairID, BYTE bCardData[], BYTE bCardCount);
	//用户点牌
	bool OnUserDianCard(WORD wChairID, BYTE bCardData[], BYTE bCardCount);
	//用户放弃
	bool OnUserPassCard(WORD wChairID);
	//放弃叉牌
	bool OnUserPassChaCard(WORD wChairID);
	//放弃点牌
	bool OnUserPassDianCard(WORD wChairID);
	//判断游戏结束
	BYTE IsGameOver();
	//算分函数
	void CountScore(LONG lScore[]);
};

//////////////////////////////////////////////////////////////////////////

#endif