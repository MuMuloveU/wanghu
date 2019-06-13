#pragma once

#include "Stdafx.h"
#include "GameClientView.h"
#include "..\公用文件\UpGradeLogic.h"


//#define		TEST_
//AI反应时间

//最大超时次数
#define		MAX_OVER_TIME_CNT			3

#define		TIME_ID_WAIT_ADHIBIT		101
#define		TIME_ID_OUT_CARD			102
#define		TIME_ID_WAIT_START			103


#define		TIME_ID_WASH_DESK			1001
#define		TIME_CNT_WASH_DESK			2000
#define		TIME_ID_LAST_AROUND			1002
#define		TIME_CNT_LAST_AROUND		4000

#define		TIME_ID_LOOK_PUBLIC			1003
#define		TIME_CNT_LOOK_PUBLIC		2000

#define		TIME_ID_LOOK_BACK			1004
#define		TIME_CNT_LOOK_BACK			4000

//审查是否只有一个玩家没准备好
#define		TIME_ID_CHECK_READY			1005
#define		TIME_CNT_CHECK_READY		1000

#define		TIME_ID_SEND_CARD			1006


//甩牌定时器
#define		TIME_ID_SHOW_CARD			1007
#define		TIME_CNT_SHOW_CARD			1000


#ifdef _DEBUG
#define		START_AI_CNT				1
#define		TIME_CNT_WAIT_GIVE			20
#define		TIME_CNT_WAIT_ADHIBIT		20
#define		TIME_CNT_OUT_CARD			55
#define		TIME_CNT_WAIT_START			55
#define		TIME_CNT_SEND_CARD			300

#else
#define		START_AI_CNT				1
#define		TIME_CNT_SEND_CARD			500
#define		TIME_CNT_WAIT_GIVE			60
#define		TIME_CNT_WAIT_ADHIBIT		10
#define		TIME_CNT_OUT_CARD			20
#define		TIME_CNT_WAIT_START			30
#endif


//////////////////////////////////////////////////////////////////////////

//游戏对话框
class CGameClientDlg : public CGameFrameDlg
{
	//控件变量
protected:
	CGameClientView					m_GameClientView;								//游戏视图
	CUpGradeGameLogic				m_Logic;


/////////////////////////
	BOOL							m_bIsHaveScore;									//是否当轮有分
	BYTE							m_bMaxCardCnt;									//当轮最大牌
	BYTE							m_bMaxCard[PLAYER_CARD_CNT];					//
//////////////
	BOOL							m_bIsLookLast;									//是否在查看上一轮
	WORD							m_wThrowSeat;									//甩牌者
	BOOL							m_bIsCanLook;									//是否允许看


	WORD							m_wNoReadyer;									//未准备的玩家
	BYTE							m_bOverTimeCnt;									//超时记数
	BYTE							m_bTimeCnt;										//AI起作用记数
	BOOL							m_bIsAI;										//是否AI
	BYTE							m_bFirstOutCnt;									//首先出牌个数
	BYTE							m_bFirstOutCards[PLAYER_CARD_CNT];				//首先出的牌

	BYTE							m_bAllMeCardCnt;
	BYTE							m_bAllMeCard[PLAYER_CARD_CNT*2];				//所有的牌
	BYTE							m_bCurSendCardPos;								//当前发牌位置


	BYTE							m_bMeCardCnt;									//当前我的牌	
	BYTE							m_bMeCard[PLAYER_CARD_CNT+BACK_CARD_CNT];						//牌值
	BYTE							m_bOutCardCnt[GAME_PLAYER];						//出牌个数(为上一轮功能准备)
	BYTE							m_bOutCard[GAME_PLAYER][PLAYER_CARD_CNT];		//牌值
	//上一轮出的牌
	BYTE							m_bLastOutCardCnt[PLAYER_CARD_CNT][GAME_PLAYER];	//出牌个数
	BYTE							m_bLastOutCard[PLAYER_CARD_CNT][GAME_PLAYER][PLAYER_CARD_CNT];	//牌值
	BYTE							m_bCurRound;									//当前轮数(-1)

	BYTE							m_bCurLookRound;								//当前查看的轮数

	WORD							m_wCurSeat;										//当前出牌者
	BYTE							m_bPublicCardCnt;								//亮牌个数
	BYTE							m_bPublicCard[2];								//亮牌(最多只有2个)

private:
	void							InitVariable();									//初始化参数
	void							UpdateNTList();
	void							KillAllTimer();	
	void							AutoOut();										//自动出牌
	void							SetOutCardButton();								//设置出牌按钮
	void							RevertDeskCard();								//还原扑克(查看上一轮时候用)

	void							SendOutCard(BYTE bCards[],BYTE bCardsCnt);

	void							SetMeUpCard(BYTE bUpCards[],BYTE bUpCardsCnt);

	void							SetSpaceCard();
	void							HideNTList();

	//网络函数
	bool							SubBeginSend(ServerBeginSend *pCard);			//开始发送
	bool							SubNTCard(ServerNtStruct *pCards);				//亮牌
	bool							SubBeginOut(ServerBeginOut *pSeat);				//开始出牌

	bool							SubOutCard(ServerOutCard *p);					//出牌消息
	bool							SubBatchOut(ServerBatchOut *p);					//出牌消息
	bool							SubGamePoint(ServerGamePoint *pCard);			//牌分数
	bool							SubGameEnd(ServerGameEnd *pEnd);
	
	
	//函数定义
public:
	//构造函数
	CGameClientDlg();
	//析构函数
	virtual ~CGameClientDlg();

	//常规继承
private:
	//初始函数
	virtual bool InitGameFrame();
	//重置框架
	virtual void ResetGameFrame();
	//游戏设置
	virtual void OnGameOptionSet();
	//时间消息
	virtual bool OnTimerMessage(WORD wChairID, UINT nElapse, UINT nTimerID);
	//旁观状态
	virtual void OnLookonChanged(bool bLookonUser, const void * pBuffer, WORD wDataSize);
	//网络消息
	virtual bool OnGameMessage(WORD wSubCmdID, const void * pBuffer, WORD wDataSize);
	//游戏场景
	virtual bool OnGameSceneMessage(BYTE cbGameStatus, bool bLookonOther, const void * pBuffer, WORD wDataSize);
	//框架消息
	virtual bool OnFrameMessage(WORD wSubCmdID, const void * pBuffer, WORD wDataSize);
public:
	//还原扑克
	LRESULT	OnRevertCard(WPARAM wparam, LPARAM lparam);
	//开始按钮函数
	LRESULT	OnHitBegin(WPARAM wparam, LPARAM lparam);
	//NT 按钮函数.
	LRESULT	OnHitPublicCard(WPARAM wparam, LPARAM lparam);
	//按动扑克按钮函数
	LRESULT	OnHitCard(WPARAM wparam, LPARAM lparam);
	//提示出牌按钮
	LRESULT	OnHint(WPARAM wparam, LPARAM lparam);
	//按动出牌按钮函数
	LRESULT	OnHitOutCard(WPARAM wparam, LPARAM lparam);
	//按动上轮按钮函数
	LRESULT	OnHitLastTurn(WPARAM wparam, LPARAM lparam);
	//按动看底牌按钮函数
	LRESULT	OnHitLookBack(WPARAM wparam, LPARAM lparam);
	//右键用牌
	LRESULT	OnRightHitCard(WPARAM wparam, LPARAM lparam);
	//提前结束按钮
	LRESULT	OnAI(WPARAM wparam, LPARAM lparam);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnTimer(UINT nIDEvent);
};

//////////////////////////////////////////////////////////////////////////
