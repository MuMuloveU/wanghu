#include "StdAfx.h"
#include "AtlBase.h"
#include "InitParamter.h"

//////////////////////////////////////////////////////////////////////////

CInitParamter::CInitParamter() 
{ 
	InitParamter();
}

CInitParamter::~CInitParamter()
{
}

//初始化
void CInitParamter::InitParamter()
{
	//网络设置
	m_wMaxConnect=512;
	m_wListenPort=PORT_CENTER_SERVER;

	//信息数据库
	m_wServerDataBasePort=1433;
	m_szServerDataBaseAddr[0]=0;
	m_szServerDataBaseUser[0]=0;
	m_szServerDataBasePass[0]=0;
	lstrcpyn(m_szServerDataBaseName,
				szServerInfoDB,
				CountArray(m_szServerDataBaseName));

	return ;
}

//加载配置
void CInitParamter::LoadInitParamter()
{
	//初始化参数
	InitParamter();

	//打开注册表
	CRegKey RegParamter;
	TCHAR szRegName[MAX_PATH];
	_snprintf(szRegName,
				sizeof(szRegName),
				TEXT("Software\\%s\\CenterServer"),
				szSystemRegKey);
	if (RegParamter.Open(HKEY_CURRENT_USER,szRegName,KEY_READ)!=ERROR_SUCCESS) 
	{
		return ;
	}

	//变量定义
	TCHAR szReadData[1024]=TEXT("");
	DWORD dwDataType,dwReadData,dwDataSize;

	//最大连接
	dwDataSize=sizeof(dwReadData);
	LONG lErrorCode = RegParamter.QueryValue(TEXT("MaxConnect"),&dwDataType,&dwReadData,&dwDataSize);
	if (lErrorCode==ERROR_SUCCESS) 
	{
		m_wMaxConnect=(WORD)dwReadData;
	}

	//监听端口
	dwDataSize=sizeof(dwReadData);
	lErrorCode=RegParamter.QueryValue(TEXT("ListenPort"),&dwDataType,&dwReadData,&dwDataSize);
	if (lErrorCode==ERROR_SUCCESS) 
	{
		m_wListenPort=(WORD)dwReadData;
	}

	//数据库端口
	dwDataSize=sizeof(dwReadData);
	lErrorCode=RegParamter.QueryValue(TEXT("ServerDataBasePort"),&dwDataType,&dwReadData,&dwDataSize);
	if (lErrorCode==ERROR_SUCCESS) 
	{
		m_wServerDataBasePort=(WORD)dwReadData;
	}

	//数据库地址
	szReadData[0]=0;
	dwDataSize=sizeof(szReadData);
	lErrorCode=RegParamter.QueryValue(TEXT("ServerDataBaseAddr"),&dwDataType,szReadData,&dwDataSize);
	if (lErrorCode==ERROR_SUCCESS)
	{
		CXOREncrypt::CrevasseData(szReadData,m_szServerDataBaseAddr,sizeof(m_szServerDataBaseAddr));
	}

	//数据库用户名
	szReadData[0]=0;
	dwDataSize=sizeof(szReadData);
	lErrorCode=RegParamter.QueryValue(TEXT("ServerDataBaseUser"),&dwDataType,szReadData,&dwDataSize);
	if (lErrorCode==ERROR_SUCCESS) 
	{
		CXOREncrypt::CrevasseData(szReadData,m_szServerDataBaseUser,sizeof(m_szServerDataBaseUser));
	}

	//数据库密码
	szReadData[0]=0;
	dwDataSize=sizeof(szReadData);
	lErrorCode=RegParamter.QueryValue(TEXT("ServerDataBasePass"),&dwDataType,szReadData,&dwDataSize);
	if (lErrorCode==ERROR_SUCCESS) 
	{
		CXOREncrypt::CrevasseData(szReadData,m_szServerDataBasePass,sizeof(m_szServerDataBasePass));
	}

	//数据库名字
	szReadData[0]=0;
	dwDataSize=sizeof(szReadData);
	lErrorCode=RegParamter.QueryValue(TEXT("ServerDataBaseName"),&dwDataType,szReadData,&dwDataSize);
	if (lErrorCode==ERROR_SUCCESS) 
	{
		CXOREncrypt::CrevasseData(szReadData,m_szServerDataBaseName,sizeof(m_szServerDataBaseName));
	}

	return;
}

//保存配置
void CInitParamter::SaveInitParamter(bool bSaveAll)
{
	//打开注册表
	CRegKey RegParamter;
	TCHAR szRegName[MAX_PATH];
	_snprintf(szRegName,sizeof(szRegName),TEXT("Software\\%s\\CenterServer"),szSystemRegKey);
	if (RegParamter.Create(HKEY_CURRENT_USER,szRegName)!=ERROR_SUCCESS) return;

	//变量定义
	TCHAR szWriteBuffer[1024]=TEXT("");

	//信息数据库
	RegParamter.SetDWORDValue(TEXT("ServerDataBasePort"),m_wServerDataBasePort);
	CXOREncrypt::EncryptData(m_szServerDataBaseAddr,szWriteBuffer,sizeof(szWriteBuffer));
	RegParamter.SetKeyValue(TEXT(""),szWriteBuffer,TEXT("ServerDataBaseAddr"));
	CXOREncrypt::EncryptData(m_szServerDataBaseUser,szWriteBuffer,sizeof(szWriteBuffer));
	RegParamter.SetKeyValue(TEXT(""),szWriteBuffer,TEXT("ServerDataBaseUser"));
	CXOREncrypt::EncryptData(m_szServerDataBasePass,szWriteBuffer,sizeof(szWriteBuffer));
	RegParamter.SetKeyValue(TEXT(""),szWriteBuffer,TEXT("ServerDataBasePass"));

	if (bSaveAll==true)
	{
		//网络配置
		RegParamter.SetDWORDValue(TEXT("MaxConnect"),m_wMaxConnect);
		RegParamter.SetDWORDValue(TEXT("ListenPort"),m_wListenPort);

		//信息数据库
		CXOREncrypt::EncryptData(m_szServerDataBaseName,szWriteBuffer,sizeof(szWriteBuffer));
		RegParamter.SetKeyValue(TEXT(""),szWriteBuffer,TEXT("ServerDataBaseName"));
	}

	return;
}

//////////////////////////////////////////////////////////////////////////
