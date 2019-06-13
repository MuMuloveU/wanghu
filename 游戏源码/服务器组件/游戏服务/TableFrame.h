#ifndef TABLE_FRAME_HEAD_FILE
#define TABLE_FRAME_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameServiceExport.h"
#include "AttemperEngineSink.h"

//////////////////////////////////////////////////////////////////////////

//用户接口数组
typedef CArrayTemplate<IServerUserItem *> CUserItemPtrArray;

//游戏桌子框架
class CTableFrame : public ITableFrameManager, public ITableFrameControl
{
	//属性变量
protected:
	WORD							m_wTableID;							//桌子号码
	WORD							m_wChairCount;						//椅子数目

	//状态变量
protected:
	bool							m_bTableLocked;						//锁定标志
	bool							m_bGameStarted;						//开始标志

	//状态变量
protected:
	BYTE							m_bGameStatus;						//游戏状态
	DWORD							m_dwTimeStart;						//开始时间
	TCHAR							m_szPassword[PASS_LEN];				//桌子密码

	//分数信息
protected:
	LONG							m_lGameTaxScore;					//游戏税收
	DWORD							m_dwPlayerID[MAX_CHAIR];			//玩家 I D
	tagScoreInfo					m_ScoreInfo[MAX_CHAIR];				//游戏积分

	//用户信息
protected:
	bool							m_bAllowLookon[MAX_CHAIR];			//旁观标志
	WORD							m_wOffLineCount[MAX_CHAIR];			//断线次数
	IServerUserItem	*				m_pIUserItem[MAX_CHAIR];			//用户指针
	CUserItemPtrArray				m_LookonUserItemPtr;				//旁观用户

	//辅助变量
protected:
	CMap<DWORD,DWORD,DWORD,DWORD>	m_ClientReadyUser;					//完毕用户

	//配置信息
protected:
	const tagGameServiceAttrib		* m_pGameServiceAttrib;				//服务属性
	const tagGameServiceOption		* m_pGameServiceOption;				//服务配置

	//对象接口
public:
	ITableFrameSink					* m_pITableFrameSink;				//桌子接口
	ITableUserAction				* m_pITableUserAction;				//事件接口
	IGameServiceFrame				* m_pIGameServiceFrame;				//功能接口

	//函数定义
public:
	//构造函数
	CTableFrame();
	//析构函数
	virtual ~CTableFrame();

	//基础接口
public:
	//释放对象
	virtual void __cdecl Release() { delete this; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//配置接口
public:
	//初始化
	virtual bool __cdecl InitTableFrame(WORD wTableID, tagTableFrameParameter * pTableFrameParameter);

	//信息接口
public:
	//桌子号码
	virtual WORD __cdecl GetTableID() { return m_wTableID; }
	//游戏人数
	virtual WORD __cdecl GetChairCount() { return m_wChairCount; }
	//游戏时间
	virtual DWORD __cdecl GetPlayTimeCount() { return (DWORD)time(NULL)-m_dwTimeStart; }

	//用户接口
public:
	//枚举用户
	virtual IServerUserItem * __cdecl EnumLookonUserItem(WORD wIndex);
	//获取用户
	virtual IServerUserItem * __cdecl GetServerUserItem(WORD wChairID);

	//状态接口
public:
	//获取状态
	virtual BYTE __cdecl GetGameStatus() { return m_bGameStatus; }
	//设置状态
	virtual void __cdecl SetGameStatus(BYTE bGameStatus) { m_bGameStatus=bGameStatus; }
	//获取配置
	virtual const tagGameServiceOption * __cdecl GetGameServiceOption() { return m_pGameServiceOption; }

	//定时器接口
public:
	//设置定时器
	virtual bool __cdecl SetGameTimer(DWORD DwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM wBindParam);
	//删除定时器
	virtual bool __cdecl KillGameTimer(DWORD DwTimerID);

	//网络接口
public:
	//发送数据
	virtual bool __cdecl SendUserData(IServerUserItem * pIServerUserItem, WORD wSubCmdID);
	//发送数据
	virtual bool __cdecl SendUserData(IServerUserItem * pIServerUserItem, WORD wSubCmdID, void * pData, WORD wDataSize);
	//发送数据
	virtual bool __cdecl SendTableData(WORD wChairID, WORD wSubCmdID);
	//发送数据
	virtual bool __cdecl SendTableData(WORD wChairID, WORD wSubCmdID, void * pData, WORD wDataSize);
	//发送数据
	virtual bool __cdecl SendLookonData(WORD wChairID, WORD wSubCmdID);
	//发送数据
	virtual bool __cdecl SendLookonData(WORD wChairID, WORD wSubCmdID, void * pData, WORD wDataSize);

	//消息接口
public:
	//发送房间消息
	virtual bool __cdecl SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType);
	//发送游戏消息
	virtual bool __cdecl SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType);

	//游戏接口
public:
	//解散游戏
	virtual bool __cdecl DismissGame();
	//结束游戏
	virtual bool __cdecl ConcludeGame();
	//发送场景
	virtual bool __cdecl SendGameScene(IServerUserItem * pIServerUserItem, void * pData, WORD wDataSize);

	//写分接口
public:
	//写入积分
	virtual bool __cdecl WriteUserScore(WORD wChairID, LONG lScore, LONG lRevenue, enScoreKind ScoreKind, LONG lPlayTimeCount=-1);
	//写入积分
	virtual bool __cdecl WriteUserScore(IServerUserItem * pIServerUserItem, LONG lScore, LONG lRevenue, enScoreKind ScoreKind, LONG lPlayTimeCount=-1);

	//执行接口
public:
	//坐下动作
	virtual bool __cdecl PerformSitDownAction(WORD wChairID, IServerUserItem * pIServerUserItem,LPCTSTR szPassword);
	//旁观动作
	virtual bool __cdecl PerformLookonAction(WORD wChairID, IServerUserItem * pIServerUserItem,LPCTSTR szPassword);
	//离开动作
	virtual bool __cdecl PerformStandUpAction(IServerUserItem * pIServerUserItem);

	//用户事件
public:
	//断线事件
	virtual bool __cdecl OnUserOffLine(WORD wChairID);
	//重进事件
	virtual bool __cdecl OnUserReConnect(WORD wChairID);
	//请求断线
	virtual bool __cdecl OnUserReqOffLine(WORD wChairID);

	//系统事件
public:
	//定时器事件
	virtual bool __cdecl OnEventTimer(DWORD dwTimerID, WPARAM wBindParam);
	//游戏事件处理
	virtual bool __cdecl OnEventSocketGame(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//框架事件处理
	virtual bool __cdecl OnEventSocketFrame(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//视频事件处理
	virtual bool __cdecl OnEventSocketVideo(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//信息接口
public:
	//锁定状态
	virtual bool __cdecl IsTableLocked() { return m_bTableLocked; }
	//游戏状态
	virtual bool __cdecl IsGameStarted() { return m_bGameStarted; }
	//游戏状态
	virtual bool __cdecl IsUserPlaying(IServerUserItem * pIServerUserItem);

	//控制接口
private:
	//开始游戏
	virtual bool __cdecl StartGame();
	//开始判断
	virtual bool __cdecl StartVerdict();
	//调换位置
	virtual bool __cdecl SwitchUserChair(WORD wSourceID[], WORD wTargetID[], WORD wSwitchCount);

	//辅助函数
public:
	//获取空位
	WORD GetNullChairID();
	//是否完毕
	bool IsClientReady(IServerUserItem * pIServerUserItem);
	//发送坐下失败
	void SendSitFailedPacket(IServerUserItem * pIServerUserItem, LPCTSTR pszFailedDescribe);
};

//////////////////////////////////////////////////////////////////////////

#endif