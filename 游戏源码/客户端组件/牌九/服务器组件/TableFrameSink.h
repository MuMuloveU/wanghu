#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//扑克类型
#define CT_SINGLE								0						//单牌
#define CT_COLOR								1						//花牌
#define CT_DOUBLE								2						//对子

//数值掩码
#define	LOGIC_MASK_COLOR			0xF0								//花色掩码
#define	LOGIC_MASK_VALUE			0x0F								//数值掩码

//结束原因
#define GER_NO_PLAYER						0x10						//没有玩家

//////////////////////////////////////////////////////////////////////////

//游戏逻辑类
class CGameLogic
{
	//变量定义
private:
	static const BYTE				m_bCardArray[52];					//扑克定义

	//基础函数
private:
	//获取数值
	BYTE GetCardValue(BYTE bCardData) 
	{ 
		if ((bCardData&LOGIC_MASK_VALUE)==1) 
		{
			return 21;
		}
		return bCardData&LOGIC_MASK_VALUE; 
	}
	//获取花色
	BYTE GetCardColor(BYTE bCardData) { return bCardData&LOGIC_MASK_COLOR; }
	//逻辑数值
	BYTE GetCardLogicValue(BYTE bCardData)
	{
		//扑克属性
		BYTE bCardColor=GetCardColor(bCardData);
		BYTE bCardValue=GetCardValue(bCardData);

		//转换数值
		if (bCardValue==21) return 1;
		if (bCardValue>10) return 0;

		return bCardValue;
	}
	BYTE GetMaxColor(BYTE bCardList[]);
	//类型函数
public:
	//是否对子
	BYTE GetCardType(BYTE bCardList[], BYTE bCardCount);

	

	//功能函数
public:
	//混乱扑克
	void RandCard(BYTE bCardBuffer[], BYTE bBufferCount);
	//排列扑克
	void SortCard(BYTE bCardList[], BYTE bCardCount);
	//对比扑克
	bool CompareCard(BYTE bFirstList[], BYTE bNextList[], BYTE bCardCount);

	BYTE GetMaxValue(BYTE bCardList[]);
	BYTE GetMinValue(BYTE bCardList[]);
};

//////////////////////////////////////////////////////////////////////////

//游戏桌子类
class CTableFrameSink : public ITableFrameSink
{
	//游戏变量
protected:
	WORD							m_wOperaCount;							//操作次数
	WORD							m_wCurrentUser;							//当前用户
	BYTE							m_bSendCardCount;						//发牌数目
	BYTE							m_bTableCardCount[GAME_PLAYER];			//扑克数目
	BYTE							m_bTableCardArray[GAME_PLAYER][4];		//桌面扑克

	//金币信息
protected:
	LONG							m_lMaxGold;								//最大下注
	LONG							m_lTurnBasicGold;						//最少下注
	LONG							m_lTableGold[2*GAME_PLAYER];			//下注金币

	//辅助变量
protected:
	BYTE							m_bPlayStatus[GAME_PLAYER];				//游戏状态

	//组件变量
protected:
	CGameLogic						m_GameLogic;							//游戏逻辑
	ITableFrame						* m_pITableFrame;						//框架接口
	const tagGameServiceOption		* m_pGameServiceOption;					//配置参数

	//属性变量
protected:
	static const WORD				m_wPlayerCount;							//游戏人数
	static const enStartMode		m_GameStartMode;						//开始模式

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

	//游戏函数
protected:
	//推断胜者
	WORD DeduceWiner(BYTE bBeginPos, BYTE bEndPos);
};

//////////////////////////////////////////////////////////////////////////

#endif