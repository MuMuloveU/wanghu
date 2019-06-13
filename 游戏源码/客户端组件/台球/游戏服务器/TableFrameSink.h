#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "..\消息定义\CMD_Billiards.h"

#define GFF_NORNAL			10				//正常结束

//////////////////////////////////////////////////////////////////////////

////游戏逻辑类
//class CGameLogic
//{
//	//变量定义
//private:
//
//	//函数定义
//public:
//	//构造函数
//	CGameLogic();
//	//析构函数
//	virtual ~CGameLogic();
//
//	//内部函数
//private:
//
//};

//////////////////////////////////////////////////////////////////////////

//游戏桌子类
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//组件变量
protected:
//	CGameLogic						m_GameLogic;							//游戏逻辑
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
	virtual bool __cdecl OnActionUserReqReady(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	//请求断线
	virtual bool __cdecl OnActionUserReqOffLine(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	//请求重入
	virtual bool __cdecl OnActionUserReqReConnect(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	//请求坐下
	virtual bool __cdecl OnActionUserReqSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bReqLookon) { return true; }
	//请求起来
	virtual bool __cdecl OnActionUserReqStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bReqLookon) { return true; }

	//动作事件
public:
	//用户同意
	virtual bool __cdecl OnActionUserReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize) { return true; }
	//用户断线
	virtual bool __cdecl OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	//用户重入
	virtual bool __cdecl OnActionUserReConnect(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	//用户坐下
	virtual bool __cdecl OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser) { return true; }
	//用户起来
	virtual bool __cdecl OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);


	//框架消息处理
protected:
	BOOL OnHitFinish(WORD bDeskStation, tagGS_HitFinishStruct * pHitFinishInfo);
	//放置白球
	BOOL OnPlaceWhite(WORD bDeskStation, tagCS_PlaceBallStruct * pPlaceWhite);
	//球桌上是否还有红球了
	BOOL IsHaveRedBall();
	BOOL GameFinish(WORD bDeskStation, int iCloseFlag);
	void InitData();
private:
//	IGoldPresent                    * m_pIGoldPresent;                  //送金币借口
	BYTE                    m_GameLayer;
	WORD                    m_GameSetCount;                  //设置规则的玩家
	int						m_iTimeDelay;					//单步延时
	long int				m_dwMoney;						//游戏金币
	int						m_GsqPoint;						//误使彩球进洞，临时记录要扣的分，等彩球放好后，再把要扣的分发给玩家
	BOOL					m_AgreeGoOn[2];					

	//运行数据
	int						m_iUserPoint[2];				//用户分数
	BOOL					m_bHitColor;					
	BYTE					bHitColor;			
	BYTE					m_bTargetBall;					//目标球
	BYTE					m_bFinish[2];					//信息同步
	BYTE					m_bNowHitUser;					//现在击球者
	//BYTE					m_bNextHitUser;					//下一个击球者
	BYTE					m_bInBallIndex[BALL_NUM];				//进袋的球索引
	BOOL					m_bBallAccess[BALL_NUM];				//是否可以访问
	double					m_iXBallStation[BALL_NUM];			//球的 X 位置
	double					m_iYBallStation[BALL_NUM];			//球的 Y 位置
	small					m_bNextColor;					//下一个该打哪一个彩球了
	BOOL					m_bIsPlayColor;					//是否打彩球阶段
	BOOL					m_bWhiteBallOver;				//是否白球进袋
	int						m_PlaerNum;
	BOOL                    bIsSetRule;
	BYTE                    m_NowSetRule;                   //设置规则的玩家
	BYTE                    m_StarPos;
	BYTE                    m_StarPoint;
	BOOL                    bIsCanAddStar;
	BOOL                    bIsSendTakeWhite;
	tagGS_HitBallStruct     pNowHitInfo;
	BOOL                    bIsHitFinish;
public:
	static const int		m_bPointIndex[BALL_NUM];				//分数索引
	static const BYTE		m_bColorIndex[BALL_NUM];				//颜色索引
};

//////////////////////////////////////////////////////////////////////////

#endif