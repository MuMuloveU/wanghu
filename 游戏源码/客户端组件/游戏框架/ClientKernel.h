#ifndef CLIENT_KERNEL_HEAD_FILE
#define CLIENT_KERNEL_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameFrame.h"

//////////////////////////////////////////////////////////////////////////

//类说明
typedef CFactoryTemplate<tagUserData>	CUserItemFactory;
typedef CArrayTemplate<tagUserData *>	CUserItemPtrArray;

//////////////////////////////////////////////////////////////////////////

//客户端内核
class CClientKernel : public CWnd, public IClientKernel, public IChannelMessageSink
{
	//房间信息
protected:
	WORD								m_wTableID;						//桌子号码
	WORD								m_wChairID;						//椅子号码

	//固有信息
protected:
	DWORD								m_dwUserID;						//用户 I D
	tagServerAttribute					m_ServerAttribute;				//房间属性

	//游戏信息
protected:
	bool								m_bInquire;						//关闭询问
	bool								m_bLookonMode;					//旁观模式
	BYTE								m_bGameStatus;					//游戏状态

	//配置信息
protected:
	bool								m_bAllowLookon;					//允许旁观
	bool								m_bAllowUserLookon;				//允许旁观

	//定时器信息
private:
	UINT								m_nTimerID;						//定时器 ID
	UINT								m_nElapseCount;					//定时器计数
	WORD								m_wTimerChairID;				//定期器位置
	HWND								m_hWndGameFrame;				//框架句柄

	//用户信息
protected:
	tagUserData							* m_pMeUserItem;				//自己信息
	tagUserData							* m_pUserItem[MAX_CHAIR];		//玩家信息
	CUserItemPtrArray					m_UserItemLookon;				//旁观数组
	CUserItemFactory					m_UserItemFactory;				//用户工厂

	//通讯变量
protected:
	HANDLE								m_hShareMemory;					//共享句柄
	tagShareMemory						* m_pShareMemory;				//共享内存
	CChannelServiceHelper				m_ChannelServiceHelper;			//信道服务

	//接口指针
protected:
	IMessageProxy						* m_pIMessageProxy;				//消息代理
	IClientKernelSink					* m_pIClientKernelSink;			//钩子接口

	//函数定义
public:
	//构造函数
	CClientKernel();
	//析构函数
	virtual ~CClientKernel();

	//基础接口
public:
	//释放对象
	virtual void __cdecl Release() { delete this; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//驱动接口
public:
	//设置接口
	virtual bool __cdecl SetMessageProxy(IUnknownEx * pIUnknownEx);
	//设置接口
	virtual bool __cdecl DestroyClientKernel();
	//内核配置
	virtual bool __cdecl InitClientKernel(LPCTSTR lpszComLine, IUnknownEx * pIUnknownEx);

	//信息接口
public:
	//离开状态
	virtual bool __cdecl GetExitFlags();
	//旁观状态
	virtual bool __cdecl IsLookonMode() { return m_bLookonMode; }
	//允许旁观
	virtual bool __cdecl IsAllowLookon() { return m_bAllowLookon; }
	//房间属性
	virtual const tagServerAttribute * __cdecl GetServerAttribute() { return &m_ServerAttribute; }

	//旁观控制
public:
	//旁观配置
	virtual bool __cdecl IsAllowUserLookon()  { return m_bAllowUserLookon; }
	//旁观控制
	virtual bool __cdecl AllowUserLookon(DWORD dwUserID, bool bAllowLookon);

	//状态接口
public:
	//游戏状态
	virtual BYTE __cdecl GetGameStatus() { return m_bGameStatus; }
	//游戏状态
	virtual void __cdecl SetGameStatus(BYTE bGameStatus) { m_bGameStatus=bGameStatus; }

	//用户接口
public:
	//自己位置
	virtual WORD __cdecl GetMeChairID() { return m_wChairID; }
	//获取自己
	virtual const tagUserData * __cdecl GetMeUserInfo() { return m_pMeUserItem; }
	//获取玩家
	virtual const tagUserData * __cdecl GetUserInfo(WORD wChairID);

	//发送接口
public:
	//显示消息
	virtual int __cdecl ShowMessageBox(LPCTSTR pszMessage, UINT nType);
	//发送函数
	virtual bool __cdecl SendSocketData(WORD wMainCmdID, WORD wSubCmdID);
	//发送函数
	virtual bool __cdecl SendSocketData(WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize);
	//发送数据
	virtual bool __cdecl SendProcessData(WORD wMainCmdID, WORD wSubCmdID);
	//发送数据
	virtual bool __cdecl SendProcessData(WORD wMainCmdID, WORD wSubCmdID, void * pBuffer, WORD wDataSize);

	//功能接口
public:
	//发送准备
	virtual bool __cdecl SendUserReady(void * pBuffer, WORD wDataSize);
	//发送聊天
	virtual bool __cdecl SendChatMessage(DWORD dwTargetUserID, LPCTSTR pszChatMessage, COLORREF crFontColor);

	//定时器接口
public:
	//定时器位置
	virtual WORD __cdecl GetTimeChairID() { return m_wTimerChairID; }
	//设置定时器
	virtual bool __cdecl SetGameTimer(WORD wChairID, UINT nTimerID, UINT nElapse);
	//删除定时器
	virtual bool __cdecl KillGameTimer(UINT nTimerID);

	//信道接口
public:
	//信道数据处理
	virtual bool __cdecl OnChannelMessage(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend);

	//IPC 子命令
private:
	//网络消息
	bool OnIPCSocket(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend);
	//配置信息
	bool OnIPCConfig(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend);
	//用户通知
	bool OnIPCUser(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend);
	//控制通知
	bool OnIPCControl(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend);

	//用户管理
private:
	//查找用户
	virtual tagUserData * __cdecl SearchUserItem(DWORD dwUserID);
	//枚举玩家
	virtual tagUserData * __cdecl EnumLookonUser(WORD wIndex);
	//增加用户
	bool ActiveUserItem(const tagUserData * pUserData);
	//删除用户
	bool DeleteUserItem(DWORD dwUserID);
	//更新用户
	bool UpdateUserItem(DWORD dwUerID, const tagUserScore * pUserScore);
	//更新用户
	bool UpdateUserItem(DWORD dwUserID, BYTE cbUserStatus, WORD wNetDelay);

	//内部函数
private:
	//更新标题
	void UpdateGameTitle();
	//内核重置
	void ResetClientKernel();

	//消息映射
protected:
	//时间消息
	afx_msg void OnTimer(UINT nIDEvent);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif