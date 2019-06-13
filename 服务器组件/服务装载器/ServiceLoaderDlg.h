#ifndef SERVIVE_LOADER_DLG_HEAD_FILE
#define SERVIVE_LOADER_DLG_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "Resource.h"
#include "InitParamter.h"
#include "ServiceParameter.h"

//////////////////////////////////////////////////////////////////////////

//数据库信息
struct tagDBConnectInfo
{
	WORD							wDataBasePort;						//数据库端口
	DWORD							dwDataBaseAddr;						//数据库地址
	TCHAR							szDataBaseUser[32];					//数据库用户
	TCHAR							szDataBasePass[32];					//数据库密码
};

//数组定义
typedef CArrayTemplate<tagDBConnectInfo> CDBConnectInfoArray;			//信息数组

//////////////////////////////////////////////////////////////////////////

//配置对话框
class CSystemOptionDlg : public CDialog
{
	//构造函数
public:
	//构造程序
	CSystemOptionDlg();
	//析构函数
	virtual ~CSystemOptionDlg();

	//重载函数
protected:
	//初始化函数
	virtual BOOL OnInitDialog();
	//控件子类化
	virtual void DoDataExchange(CDataExchange * pDX);
	//确定函数
	virtual void OnOK();

	//消息映射
	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//程序主窗口
class CServiceLoaderDlg : public CDialog
{
	//组件变量
public:
	CGameServiceHelper				m_GameService;						//游戏服务

	//控件变量
public:
	CRichEditTrace					m_RichEditCtrl;						//信息窗口
	CInitParamter					m_InitParamter;						//配置参数
	CServiceParameter				m_ServiceParameter;					//服务参数
	CDBConnectInfoArray				m_DBConnectInfoArray;				//信息数组

	//函数定义
public:
	//构造函数
	CServiceLoaderDlg();
	//析构函数
	virtual ~CServiceLoaderDlg();

	//重载函数
protected:
	//控件绑定
	virtual void DoDataExchange(CDataExchange * pDX);
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定消息
	virtual void OnOK();
	//取消函数
	virtual void OnCancel();

	//内部函数
protected:
	//获取密码
	const tagDBConnectInfo * GetDBConnectInfo(DWORD dwDataBaseAddr);
	//显示错误
	void ShowErrorMessasge(LPCTSTR pszString, enTraceLevel TraceLevel);

	//消息映射
protected:
	//启动服务
	afx_msg void OnBnClickedStart();
	//停止服务
	afx_msg void OnBnClickedStop();
	//创建房间
	afx_msg void OnBnClickedCreate();
	//打开房间
	afx_msg void OnBnClickedOpen();
	//配置房间
	afx_msg void OnBnClickedOption();
	//系统配置
	afx_msg void OnBnClickedSystem();
	//保存配置
	afx_msg void OnBnClickedSaveOption();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnQueryEndSession();
};

//////////////////////////////////////////////////////////////////////////

#endif