#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "..\公用文件\UpGradeLogic.h"
#include <deque>
using namespace std;

//////////////////////////////////////////////////////////////////////////
#define TEST

#define		TIME_ID_SEND_CARD		30

//////////////////////////////////////////////////////////////////////////

//游戏桌子类
class CTableFrameSink : public ITableFrameSink
{
	//组件变量
protected:
	CUpGradeGameLogic				m_Logic;									//游戏逻辑
	ITableFrame						* m_pITableFrame;							//框架接口
	const tagGameServiceOption		* pGameServiceOption;						//配置参数
//////////////////
	BOOL							m_bIsHaveScore;								//是否有分
	BYTE							m_bMaxCardCnt;
	BYTE							m_bMaxCard[PLAYER_CARD_CNT];
////////////////
	BOOL							m_bIsCanLook[GAME_PLAYER];					//是否允许旁观
	BYTE							m_bFirstCard[PLAYER_CARD_CNT];				//首先出的牌
	BYTE							m_bFirstCardCnt;							
	BYTE							m_bCurPublicFlower;							//当前亮的花色
	BYTE							m_bTempCnt;									//缓冲记数用
	BYTE							m_bPublicCard[GAME_PLAYER][2];				//亮过的牌
	BYTE							m_bPublicCardCnt[GAME_PLAYER];				//亮牌个数

	WORD							m_wFirstOut;								//首先出牌者
	BYTE							m_bScoreCardCnt[GAME_PLAYER];				//分牌个数
	BYTE							m_bLastEndReason;							//上一轮游戏结束的原因
	BYTE							m_bLastOutCard[PLAYER_CARD_CNT][GAME_PLAYER][PLAYER_CARD_CNT];//出过的牌
	BYTE							m_bLastOutCardCnt[PLAYER_CARD_CNT][GAME_PLAYER];				//个数

	BYTE							m_bOutHandCnt;								//出了几手牌
	DWORD							m_dwUserID[GAME_PLAYER];					//记录刚开始时候用户的ID
////////////////


	BYTE							m_bUserCard[GAME_PLAYER][PLAYER_CARD_CNT];	//玩家手上的牌
	BYTE							m_bUserCardCnt[GAME_PLAYER];				//牌个数
	
	BYTE							m_bOutCard[GAME_PLAYER][PLAYER_CARD_CNT];	//出过的牌
	BYTE							m_bOutCardCnt[GAME_PLAYER];					//个数

	BYTE							m_bScoreCard[GAME_PLAYER][25];	//分牌

	BYTE							m_bCurPublicCard[2];						//当前亮牌
	BYTE							m_bCurPublicCardCnt;						//个数

	
	WORD							m_wBanker;									//庄家
	WORD							m_wPublic;									//当前亮（顶，粘）牌者

	WORD							m_wCurOutCard;								//当前出牌者

	int								m_nScoreValue[GAME_PLAYER];					//分值

//////////////////////////////////////////////////
private:
	//自定义函数
	void			InitVariable();											//初始化	
	void			ResendCard();											//重新发牌
	void			SendCardNoPublic();										//无人亮牌
	void			BeginOutCard();
	void			SendScoreCard(bool bIsLastRound);

	WORD			GetNextSeat(WORD wSeat);
	WORD			GetNextBanker(WORD wBanker);
	WORD			GetWiner(WORD wFirstOut);

	//网络消息
	bool			SubOutCard(WORD wOutSeat,BYTE bOutCards[],BYTE bOutCardsCnt);
	bool			SubNTCard(ClientNtStruct *pNT,WORD wSeat);
	bool			GameEnd(WORD wSeat,BYTE bEndReason);

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
};

//////////////////////////////////////////////////////////////////////////

#endif