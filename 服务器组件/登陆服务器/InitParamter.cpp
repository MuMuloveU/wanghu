#include "StdAfx.h"
#include "AtlBase.h"
#include "InitParamter.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CInitParamter::CInitParamter() 
{ 
	InitParamter();
}

//析构函数
CInitParamter::~CInitParamter()
{
}

//初始化
void CInitParamter::InitParamter()
{
	//设置变量
	m_szMainPage[0]=0;
	m_wMaxConnect=512;
	m_szCenterServerAddr[0]=0;
	m_wListenPort=PORT_LOGON_SERVER;

	//登录数据库
	m_wUserDataBasePort=1433;
	m_szUserDataBaseAddr[0]=0;
	m_szUserDataBaseUser[0]=0;
	m_szUserDataBasePass[0]=0;
	lstrcpyn(m_szUserDataBaseName,szGameUserDB,CountArray(m_szUserDataBaseName));

	return;
}

//加载配置
void CInitParamter::LoadInitParamter()
{
	//初始化参数
	InitParamter();

	//打开注册表
	CRegKey RegParamter;
	TCHAR szRegName[MAX_PATH];
	_snprintf(szRegName,sizeof(szRegName),TEXT("Software\\%s\\LogonServer"),szSystemRegKey);
	if (RegParamter.Open(HKEY_CURRENT_USER,szRegName,KEY_READ)!=ERROR_SUCCESS) return;

	//变量定义
	TCHAR szReadData[1024]=TEXT("");
	DWORD dwDataType,dwReadData,dwDataSize;

	//游戏主站
	szReadData[0]=0;
	dwDataSize=sizeof(szReadData);
	LONG lErrorCode=RegParamter.QueryValue(TEXT("MainPage"),&dwDataType,szReadData,&dwDataSize);
	if (lErrorCode==ERROR_SUCCESS) CXOREncrypt::CrevasseData(szReadData,m_szMainPage,sizeof(m_szMainPage));

	//中心服务器
	szReadData[0]=0;
	dwDataSize=sizeof(szReadData);
	lErrorCode=RegParamter.QueryValue(TEXT("CenterServerAddr"),&dwDataType,szReadData,&dwDataSize);
	if (lErrorCode==ERROR_SUCCESS) CXOREncrypt::CrevasseData(szReadData,m_szCenterServerAddr,sizeof(m_szCenterServerAddr));

	//最大连接
	dwDataSize=sizeof(dwReadData);
	lErrorCode=RegParamter.QueryValue(TEXT("MaxConnect"),&dwDataType,&dwReadData,&dwDataSize);
	if (lErrorCode==ERROR_SUCCESS) m_wMaxConnect=(WORD)dwReadData;

	//监听端口
	dwDataSize=sizeof(dwReadData);
	lErrorCode=RegParamter.QueryValue(TEXT("ListenPort"),&dwDataType,&dwReadData,&dwDataSize);
	if (lErrorCode==ERROR_SUCCESS) m_wListenPort=(WORD)dwReadData;

	//数据库端口
	dwDataSize=sizeof(dwReadData);
	lErrorCode=RegParamter.QueryValue(TEXT("UserDataBasePort"),&dwDataType,&dwReadData,&dwDataSize);
	if (lErrorCode==ERROR_SUCCESS) m_wUserDataBasePort=(WORD)dwReadData;

	//数据库地址
	szReadData[0]=0;
	dwDataSize=sizeof(szReadData);
	lErrorCode=RegParamter.QueryValue(TEXT("UserDataBaseAddr"),&dwDataType,szReadData,&dwDataSize);
	if (lErrorCode==ERROR_SUCCESS) CXOREncrypt::CrevasseData(szReadData,m_szUserDataBaseAddr,sizeof(m_szUserDataBaseAddr));

	//数据库用户名
	szReadData[0]=0;
	dwDataSize=sizeof(szReadData);
	lErrorCode=RegParamter.QueryValue(TEXT("UserDataBaseUser"),&dwDataType,szReadData,&dwDataSize);
	if (lErrorCode==ERROR_SUCCESS) CXOREncrypt::CrevasseData(szReadData,m_szUserDataBaseUser,sizeof(m_szUserDataBaseUser));

	//数据库密码
	szReadData[0]=0;
	dwDataSize=sizeof(szReadData);
	lErrorCode=RegParamter.QueryValue(TEXT("UserDataBasePass"),&dwDataType,szReadData,&dwDataSize);
	if (lErrorCode==ERROR_SUCCESS) CXOREncrypt::CrevasseData(szReadData,m_szUserDataBasePass,sizeof(m_szUserDataBasePass));

	//数据库名字
	szReadData[0]=0;
	dwDataSize=sizeof(szReadData);
	lErrorCode=RegParamter.QueryValue(TEXT("UserDataBaseName"),&dwDataType,szReadData,&dwDataSize);
	if (lErrorCode==ERROR_SUCCESS) CXOREncrypt::CrevasseData(szReadData,m_szUserDataBaseName,sizeof(m_szUserDataBaseName));

	return;
}

//保存配置
void CInitParamter::SaveInitParamter(bool bSaveAll)
{
	//打开注册表
	CRegKey RegParamter;
	TCHAR szRegName[MAX_PATH];
	_snprintf(szRegName,sizeof(szRegName),TEXT("Software\\%s\\LogonServer"),szSystemRegKey);
	if (RegParamter.Create(HKEY_CURRENT_USER,szRegName)!=ERROR_SUCCESS) return;

	//变量定义
	TCHAR szWriteBuffer[1024]=TEXT("");

	//登录数据库
	RegParamter.SetDWORDValue(TEXT("UserDataBasePort"),m_wUserDataBasePort);
	CXOREncrypt::EncryptData(m_szUserDataBaseAddr,szWriteBuffer,sizeof(szWriteBuffer));
	RegParamter.SetKeyValue(TEXT(""),szWriteBuffer,TEXT("UserDataBaseAddr"));
	CXOREncrypt::EncryptData(m_szUserDataBaseUser,szWriteBuffer,sizeof(szWriteBuffer));
	RegParamter.SetKeyValue(TEXT(""),szWriteBuffer,TEXT("UserDataBaseUser"));
	CXOREncrypt::EncryptData(m_szUserDataBasePass,szWriteBuffer,sizeof(szWriteBuffer));
	RegParamter.SetKeyValue(TEXT(""),szWriteBuffer,TEXT("UserDataBasePass"));

	//中心服务器
	CXOREncrypt::EncryptData(m_szCenterServerAddr,szWriteBuffer,sizeof(szWriteBuffer));
	RegParamter.SetKeyValue(TEXT(""),szWriteBuffer,TEXT("CenterServerAddr"));

	//游戏主站
	CXOREncrypt::EncryptData(m_szMainPage,szWriteBuffer,sizeof(szWriteBuffer));
	RegParamter.SetKeyValue(TEXT(""),szWriteBuffer,TEXT("MainPage"));

	if (bSaveAll==true)
	{
		//网络配置
		RegParamter.SetDWORDValue(TEXT("MaxConnect"),m_wMaxConnect);
		RegParamter.SetDWORDValue(TEXT("ListenPort"),m_wListenPort);

		//登录数据库
		CXOREncrypt::EncryptData(m_szUserDataBaseName,szWriteBuffer,sizeof(szWriteBuffer));
		RegParamter.SetKeyValue(TEXT(""),szWriteBuffer,TEXT("UserDataBaseName"));
	}

	return;
}

//////////////////////////////////////////////////////////////////////////
