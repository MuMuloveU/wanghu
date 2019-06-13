#ifndef CLIENT_KERNEL_SINK_HEAD_FILE
#define CLIENT_KERNEL_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"

class CGameFrameDlg;

//////////////////////////////////////////////////////////////////////////

//内核钩子类
class CClientKernelSink : public IClientKernelSink
{
	//变量定义
protected:
	CGameFrameDlg				& m_GameFrameDlg;						//游戏框架

	//函数定义
public:
	//构造函数
	CClientKernelSink(CGameFrameDlg & GameFrameDlg);
	//析构函数
	virtual ~CClientKernelSink();

	//基础接口
public:
	//释放对象
	virtual void __cdecl Release() { delete this; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//驱动接口
private:
	//获取窗口
	virtual HWND __cdecl GetFrameWnd();
	//重置状态
	virtual void __cdecl ResetGameFrame();
	//关闭窗口
	virtual void __cdecl CloseGameFrame(bool bForceClose);

	//内核事件
private:
	//时间消息
	virtual bool __cdecl OnEventTimer(WORD wChairID, UINT nElapse, UINT nTimerID);
	//网络消息
	virtual bool __cdecl OnEventSocket(const CMD_Command & Command, const void * pBuffer, WORD wDataSize);
	//进程消息
	virtual bool __cdecl OnEventProcess(const IPC_Head & Head, const void * pBuffer, WORD wDataSize, HWND hWndSend);

	//游戏事件
private:
	//游戏状态
	virtual bool __cdecl OnEventGameScene(BYTE cbGameStation, bool bLookonOther, void * pBuffer, WORD wDataSize);
	//聊天信息
	virtual bool __cdecl OnUserChatMessage(tagUserData * pSendUserData, tagUserData * pRecvUserData, LPCTSTR pszChatMessage, COLORREF crColor);
	//时间更新
	virtual void __cdecl OnEventTimerKilled(WORD wChairID, UINT nTimerID);
	//旁观状态
	virtual void __cdecl OnEventLookonChanged(bool bLookonUser, void * pBuffer, WORD wDataSize);

	//用户事件
public:
	//用户进入
	virtual void __cdecl OnEventUserEnter(tagUserData * pUserData, WORD wChairID, bool bLookonUser);
	//用户离开
	virtual void __cdecl OnEventUserLeave(tagUserData * pUserData, WORD wChairID, bool bLookonUser);
	//用户积分
	virtual void __cdecl OnEventUserScore(tagUserData * pUserData, WORD wChairID, bool bLookonUser);
	//用户状态
	virtual void __cdecl OnEventUserStatus(tagUserData * pUserData, WORD wChairID, bool bLookonUser);
	//会员等级
	virtual void __cdecl OnEventUserMemberOrder(tagUserData * pUserData, WORD wChairID, bool bLookonUser);
	
	//礼物事件
public:
	//初始道具
	virtual void __cdecl OnEventInitProperty(tagPropertyInfo *pPropertyInfo, int nInfoCount);
	//初始鲜花
	virtual void __cdecl OnEventInitFlower(tagFlowerInfo *pFlowerInfo, int nInfoCount);
	//鲜花消息
	virtual void __cdecl OnEventFlower(const tagUserData * pSendUserData, const tagUserData * pRecvUserData, UINT uFlowerID, UINT uFlowerEffectID);

};

//////////////////////////////////////////////////////////////////////////

#endif