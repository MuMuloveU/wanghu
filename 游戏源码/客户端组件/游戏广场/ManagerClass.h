#ifndef MANAGER_CLASS_HEAD_FILE
#define MANAGER_CLASS_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "Resource.h"

//////////////////////////////////////////////////////////////////////////

//发送警告
class CManagerSendWarning : public CSkinDialogEx
{
	//变量定义
protected:
	DWORD							m_dwUserID;							//用户标识
	IUserItem *						m_pIUserItem;						//用户接口

	//接口定义
protected:
	ITCPSocket *					m_pIClientSocket;					//网络接口
	IClientUserManager	*			m_pIClientUserManager;				//用户接口

	//控件变量
protected:
	CSkinButton						m_btSend;							//发送按钮
	CSkinButton						m_btClose;							//关闭按钮
	CSkinButton						m_btPreView;						//预览按钮
	CSkinRichEdit					m_WarningMessage;					//警告消息

	//函数定义
public:
	//构造函数
	CManagerSendWarning();
	//析构函数
	virtual ~CManagerSendWarning();

	//功能函数
public:
	//设置用户
	bool SetUserItem(IUnknownEx * pIUnknownEx);
	//设置接口
	bool SetClientSocket(IUnknownEx * pIUnknownEx);
	//设置接口
	bool SetClientUserManager(IUnknownEx * pIUnknownEx);

	//重载函数
protected:
	//控件绑定
	virtual void DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定函数
	virtual void OnOK();
	//取消消息
	virtual void OnCancel();

	//消息函数
public:
	//预览消息
	afx_msg void OnPreViewMessage();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//权限管理
class CManagerUserRight : public CSkinDialogEx
{
	//变量定义
protected:
	DWORD							m_dwUserID;							//用户标识
	IUserItem *						m_pIUserItem;						//用户接口

	//权限变量
protected:
	DWORD							m_dwUserRight;						//用户权限
	//DWORD							m_dwLastUserRight;					//用户权限
	DWORD							m_dwMasterRight;					//管理权限

	//接口定义
protected:
	ITCPSocket *					m_pIClientSocket;					//网络接口
	IClientUserManager	*			m_pIClientUserManager;				//用户接口

	//控件变量
protected:
	CSkinButton						m_btOk;								//确定按钮
	CSkinButton						m_btClose;							//关闭按钮

	//函数定义
public:
	//构造函数
	CManagerUserRight();
	//析构函数
	virtual ~CManagerUserRight();

	//功能函数
public:
	//设置权限
	bool SetMasterRight(DWORD dwMasterRight);
	//设置用户
	bool SetUserItem(IUnknownEx * pIUnknownEx);
	//设置接口
	bool SetClientSocket(IUnknownEx * pIUnknownEx);
	//设置接口
	bool SetClientUserManager(IUnknownEx * pIUnknownEx);

	//内部函数
protected:
	//更新数据
	bool UpdateRightData();
	//更新控制
	bool UpdateMasterControl();

	//重载函数
protected:
	//控件绑定
	virtual void DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定函数
	virtual void OnOK();
	//取消消息
	virtual void OnCancel();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//系统消息
class CManagerMessage : public CSkinDialogEx
{
	//接口定义
protected:
	ITCPSocket *					m_pIClientSocket;					//网络接口

	//控件变量
protected:
	CSkinButton						m_btSend;							//发送按钮
	CSkinButton						m_btClose;							//关闭按钮
	CSkinButton						m_btPreView;						//预览按钮
	CSkinRichEdit					m_SystemMessage;					//系统消息

	//函数定义
public:
	//构造函数
	CManagerMessage();
	//析构函数
	virtual ~CManagerMessage();

	//功能函数
public:
	//设置接口
	bool SetClientSocket(IUnknownEx * pIUnknownEx);

	//重载函数
protected:
	//控件绑定
	virtual void DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定函数
	virtual void OnOK();
	//取消消息
	virtual void OnCancel();

	//消息函数
public:
	//预览消息
	afx_msg void OnPreViewMessage();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//房间管理
class CManagerServer : public CSkinDialogEx
{
	//接口定义
protected:
	ITCPSocket *					m_pIClientSocket;					//网络接口

	//控件变量
protected:
	CSkinButton						m_btLimitGameChat;					//控制按钮
	CSkinButton						m_btAllowGameChat;					//控制按钮
	CSkinButton						m_btLimitRoomChat;					//控制按钮
	CSkinButton						m_btAllowRoomChat;					//控制按钮
	CSkinButton						m_btLimitEnterRoom;					//控制按钮
	CSkinButton						m_btAllowEnterRoom;					//控制按钮
	CSkinButton						m_btLimitEnterGame;					//控制按钮
	CSkinButton						m_btAllowEnterGame;					//控制按钮
	CSkinButton						m_btSetShallClose;					//控制按钮
	CSkinButton						m_btReSetShallClose;				//控制按钮
	CSkinButton						m_btLimitRoomWisper;				//控制按钮
	CSkinButton						m_btAllowRoomWisper;				//控制按钮

	//函数定义
public:
	//构造函数
	CManagerServer();
	//析构函数
	virtual ~CManagerServer();

	//重载函数 
protected:
	//菜单命令
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	//功能函数
public:
	//设置接口
	bool SetClientSocket(IUnknownEx * pIUnknownEx);

	//重载函数
protected:
	//控件绑定
	virtual void DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定函数
	virtual void OnOK();
	//取消消息
	virtual void OnCancel();

	//消息函数
public:

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif