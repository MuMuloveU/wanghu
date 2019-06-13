#ifndef INIT_PARAMTER_HEAD_FILE
#define INIT_PARAMTER_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//配置参数
class CInitParamter
{
	//其他配置
public:
	TCHAR							m_szAccreditID[39];					//授权号码
	TCHAR							m_szCenterServerAddr[16];			//中心服务器

	//用户数据库
public:
	WORD							m_wGameUserDBPort;					//数据库端口
	TCHAR							m_szGameUserDBAddr[16];				//数据库地址
	TCHAR							m_szGameUserDBUser[32];				//数据库用户
	TCHAR							m_szGameUserDBPass[32];				//数据库密码
	TCHAR							m_szGameUserDBName[32];				//数据库名字

	//游戏数据库
public:
	WORD							m_wServerInfoDBPort;				//数据库端口
	TCHAR							m_szServerInfoDBAddr[16];			//数据库地址
	TCHAR							m_szServerInfoDBUser[32];			//数据库用户
	TCHAR							m_szServerInfoDBPass[32];			//数据库密码
	TCHAR							m_szServerInfoDBName[32];			//数据库名字

	//函数定义
public:
	//构造函数
	CInitParamter();
	//析构函数
	virtual ~CInitParamter();

	//功能函数
public:
	//初始化
	void InitParamter();
	//加载配置
	void LoadInitParamter();
	//保存配置
	void SaveInitParamter(bool bSaveAll);
};

//////////////////////////////////////////////////////////////////////////

#endif