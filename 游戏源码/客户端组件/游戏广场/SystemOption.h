#ifndef SYSTEM_OPTION_HEAD_FILE
#define SYSTEM_OPTION_HEAD_FILE

#pragma once

#include "Stdafx.h"

//类说明
class CListKind;
class CListServer;

//////////////////////////////////////////////////////////////////////////
//枚举定义

//邀请模式
enum enInviteMode
{
	enInviteMode_All					=1,								//全部显示
	enInviteMode_Friend					=2,								//朋友显示
	enInviteMode_None					=3,								//全不显示
};

//帐户规则
enum enAcountsRule
{
	enAcountsRule_None					=1,								//不记录信息
	enAcountsRule_Accounts				=2,								//记住帐户
	enAcountsRule_AccountsAndPass		=3,								//帐户和密码
};

//////////////////////////////////////////////////////////////////////////

//房间设置信息
class CServerOption
{
	//属性变量
public:
	const WORD						m_wServerID;						//房间标示
	const CListServer 				* m_pListServer;					//游戏房间

	//携带密码
public:
	bool							m_bPassword;						//设置密码
	TCHAR							m_szPassword[PASS_LEN];				//桌子密码
	TCHAR							m_szEnterPassword[PASS_LEN];		//进入密码

	//函数定义
public:
	//构造函数
	CServerOption(WORD wServerID, CListServer * pListServer);
	//析构函数
	virtual ~CServerOption();

	//功能函数
public:
	//加载设置
	void LoadOptionParameter();
	//保存设置
	void SaveOptionParameter();
};

//////////////////////////////////////////////////////////////////////////

//游戏设置信息
class CGameOption
{
	//属性变量
public:
	const WORD						m_wKindID;							//游戏标示
	CListKind 						* m_pListKind;						//游戏类型

	//积分限制
public:
	LONG							m_lMaxScore;						//最高分数 
	LONG							m_lLessScore;						//最低分数
	bool							m_bLimitScore;						//限制分数

	//逃跑限制
public:
	WORD							m_wFleeRate;						//逃跑率
	bool							m_bLimitFlee;						//限制断线

	//胜利限制
public:
	WORD							m_wWinRate;							//胜利率
	bool							m_bLimitWin;						//限制胜率

	//函数定义
public:
	//构造函数
	CGameOption(WORD wKindID, CListKind * pListKind);
	//析构函数
	virtual ~CGameOption();

	//功能函数
public:
	//加载设置
	void LoadOptionParameter();
	//保存设置
	void SaveOptionParameter();
};

//////////////////////////////////////////////////////////////////////////

//类说明
typedef CArrayTemplate<CGameOption *> CGameOptionArray;
typedef CArrayTemplate<CServerOption *> CServerOptionArray;

//全局配置类
class CGlobalOption
{
	//全局配置
public:
	bool							m_bExpandedList;					//展开列表
	enAcountsRule					m_enAcountsRule;					//帐户规则

	//房间设置
public:
	bool							m_bLimitDetest;						//限制玩家
	bool							m_bCheckSameIP;						//效验地址

	//消息设置
public:
	bool							m_bSaveWisper;						//保存私聊
	bool							m_bShowInOutMessage;				//进出信息
	bool							m_bOnlyFriendMessage;				//朋友消息
	bool							m_bHideDetestMessage;				//厌恶消息

	//其他配置
public:
	bool							m_bAutoSit;							//自动坐下
	WORD							m_wBossHotKey;						//老板热键
	enInviteMode					m_InviteMode;						//邀请模式

	//代理配置
public:
	enProxyServerType				m_ProxyServerType;					//代理类型
	tagProxyServerInfo				m_ProxyServerInfo;					//代理信息

	//颜色设置
public:
	COLORREF						m_crChatTX;							//聊天字体
	COLORREF						m_crMsgName;						//名字消息
	COLORREF						m_crMsgNormal;						//普通消息
	COLORREF						m_crMsgSysHead;						//系统消息
	COLORREF						m_crMsgSysString;					//系统消息
	COLORREF						m_crServerListTX;					//列表字色
	COLORREF						m_crServerListBK;					//列表底色

	//内部变量
private:
	CGameOptionArray				m_GameOptionArray;					//游戏配置
	CServerOptionArray				m_ServerOptionArray;				//房间配置

	//函数定义
public:
	//构造函数
	CGlobalOption();
	//析构函数
	virtual ~CGlobalOption();

	//功能函数
public:
	//加载设置
	void LoadOptionParameter();
	//保存设置
	void SaveOptionParameter();
	//游戏配置
	CGameOption * GetGameOption(CListKind * pListKind);
	//房间设置
	CServerOption * GetServerOption(CListServer * pListServer);
};

//////////////////////////////////////////////////////////////////////////

//全局信息 
extern	CGlobalOption					g_GlobalOption;					//系统配置

//////////////////////////////////////////////////////////////////////////

#endif