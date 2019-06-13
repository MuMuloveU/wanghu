#ifndef ANDROID_USER_TIEM_HEAD_FILE
#define ANDROID_USER_TIEM_HEAD_FILE

#pragma once

#include "GameServiceExport.h"

//////////////////////////////////////////////////////////////////////////

//时间子项
struct tagTimerItem
{
	UINT							nTimerID;							//时间标识
	UINT							nTimeLeave;							//剩余时间
};

//类说明
typedef CArrayTemplate<tagTimerItem *> CTimerItemArray;					//时间数组

//////////////////////////////////////////////////////////////////////////

//机器人信息
class CAndroidUserItem : public IAndroidUserItem
{
	//友元定义
	friend class CAndroidUserManager;

	//状态变量
protected:
	bool							m_bStartClient;						//游戏标志
	BYTE							m_cbGameStatus;						//游戏状态
	tagUserStatus					m_CurrentUserStatus;				//用户状态

	//绑定变量
protected:
	DWORD							m_dwUserID;							//用户标识
	IServerUserItem *				m_pIServerUserItem;					//用户接口

	//索引变量
protected:
	WORD							m_wRountID;							//循环索引
	WORD							m_wAndroidIndex;					//索引变量

	//时间组件
protected:
	CTimerItemArray					m_TimerItemActive;					//活动数组
	static CTimerItemArray			m_TimerItemStorage;					//库存数组

	//接口变量
protected:
	IServerUserManager *			m_pIServerUserManager;				//用户管理
	IAndroidUserManager *			m_pIAndroidUserManager;				//管理接口
	IAndroidUserItemSink *			m_pIAndroidUserItemSink;			//回调接口

	//函数定义
public:
	//构造函数
	CAndroidUserItem();
	//析构函数
	virtual ~CAndroidUserItem();

	//基础接口
public:
	//释放对象
	virtual VOID __cdecl Release() { delete this; }
	//接口查询
	virtual VOID * __cdecl QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//状态接口
public:
	//获取状态
	virtual BYTE __cdecl GetGameStatus() { return m_cbGameStatus; }
	//设置状态
	virtual VOID __cdecl SetGameStatus(BYTE cbGameStatus) { m_cbGameStatus=cbGameStatus; }

	//用户信息
public:
	//获取 I D
	virtual DWORD __cdecl GetUserID();
	//桌子号码
	virtual WORD __cdecl GetTableID();
	//椅子号码
	virtual WORD __cdecl GetChairID();

	//用户接口
public:
	//获取自己
	virtual IServerUserItem * __cdecl GetMeUserItem();
	//游戏用户
	virtual IServerUserItem * __cdecl GetTableUserItem(WORD wChariID);

	//网络接口
public:
	//发送函数
	virtual bool __cdecl SendSocketData(WORD wSubCmdID);
	//发送函数
	virtual bool __cdecl SendSocketData(WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//功能接口
public:
	//删除时间
	virtual bool __cdecl KillGameTimer(UINT nTimerID);
	//设置时间
	virtual bool __cdecl SetGameTimer(UINT nTimerID, UINT nElapse);
	//发送准备
	virtual bool __cdecl SendUserReady(VOID * pData, WORD wDataSize);
	//发送聊天
	virtual bool __cdecl SendChatMessage(DWORD dwTargetUserID, LPCTSTR pszChatMessage, COLORREF crFontColor);

	//事件通知
public:
	//时间事件
	bool OnTimerPulse(DWORD dwTimerID, WPARAM dwBindParameter);
	//网络消息
	bool OnSocketRead(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//网络事件
protected:
	//登录失败
	bool OnSocketSubLogonFailed(VOID * pData, WORD wDataSize);
	//登录完成
	bool OnSocketSubLogonFinish(VOID * pData, WORD wDataSize);
	//用户状态
	bool OnSocketSubUserStatus(VOID * pData, WORD wDataSize);
	//游戏消息
	bool OnSocketGameMessage(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//控制函数
protected:
	//启动游戏
	VOID StartGameClient();
	//关闭游戏
	VOID CloseGameClient();

	//辅助函数
public:
	//复位数据
	VOID RepositUserItem();
};

//////////////////////////////////////////////////////////////////////////

#endif