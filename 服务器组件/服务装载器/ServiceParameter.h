#ifndef SERVICE_PARAMETER_HEAD_FILE
#define SERVICE_PARAMETER_HEAD_FILE
#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

#define VER_PARAMETER				2									//文件版本

//////////////////////////////////////////////////////////////////////////
//结构定义

//配置信息
struct tagOptionParameter
{
	TCHAR							szDataBaseAddr[16];					//数据库地址
	TCHAR							szDataBaseName[32];					//数据库名字
	TCHAR							szModuleName[MODULE_LEN];			//模块名字
	tagGameServiceOption			GameServiceOption;					//服务配置
};

//记录参数
struct tagRecordParameter
{
	WORD							wVersion;							//配置版本
	WORD							wFileSize;							//文件大小
	tagOptionParameter				OptionParameter;					//配置参数
};

//////////////////////////////////////////////////////////////////////////

//服务参数类
class CServiceParameter : public CDialog
{
	//变量定义
protected:
	bool							m_bLoaded;							//加载标志
	tagOptionParameter				m_OptionParameter;					//配置参数

	//组件变量
protected:
	HINSTANCE						m_hDllInstance;						//游戏实例
	tagGameServiceAttrib			m_GameServiceAttrib;				//服务属性
	IGameServiceManager				* m_pIGameServiceManager;			//管理接口

	//函数定义
public:
	//构造函数
	CServiceParameter(void);
	//析构函数
	virtual ~CServiceParameter(void);

	//重载函数
protected:
	//初始化函数
	virtual BOOL OnInitDialog();
	//控件子类化
	virtual void DoDataExchange(CDataExchange * pDX);
	//确定函数
	virtual void OnOK();

	//功能函数
public:
	//是否加载
	bool IsLoaded() { return m_bLoaded; }
	//获取参数
	const tagOptionParameter & GetOptionParameter() { return m_OptionParameter; }

	//管理函数
public:
	//重置参数
	void ResetParameter();
	//修改参数
	bool ConfigParameter();
	//加载配置
	bool LoadParameter(LPCTSTR pszFileName);
	//保存配置
	bool SaveParameter(LPCTSTR pszFileName);
	//创建参数
	bool CreateParameter(LPCTSTR pszModuleName);

	//内部函数
private:
	//加载模块
	bool LoadGameServiceModule(LPCTSTR pszModuleName);
	//释放模块
	void FreeGameServiceModule();
	//更新配置
	void UpdateOptionParameter();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif