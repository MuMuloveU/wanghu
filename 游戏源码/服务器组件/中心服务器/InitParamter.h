#ifndef INIR_PARAMTER_HEAD_FILE
#define INIR_PARAMTER_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//配置参数
class CInitParamter
{
	//网络设置
public:
	WORD							m_wListenPort;						//监听端口
	WORD							m_wMaxConnect;						//最大连接

	//信息数据库
public:
	WORD							m_wServerDataBasePort;				//数据库端口
	TCHAR							m_szServerDataBaseAddr[16];			//数据库地址
	TCHAR							m_szServerDataBaseUser[32];			//数据库用户
	TCHAR							m_szServerDataBasePass[32];			//数据库密码
	TCHAR							m_szServerDataBaseName[32];			//数据库名字

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
