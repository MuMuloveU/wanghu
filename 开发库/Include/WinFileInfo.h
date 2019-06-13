#ifndef WIN_FILE_INFO_HEAD_FILE
#define WIN_FILE_INFO_HEAD_FILE

#pragma once

#include "ComService.h"

//////////////////////////////////////////////////////////////////////////

//文件版本对比类
class COM_SERVICE_CLASS CWinFileInfo
{ 
	//变量定义
protected:
	DWORD							m_dwLangCharset;					//语言标识
	DWORD							m_dwDataSize;						//数据大小
	BYTE							m_cbInfoBuffer[4096];				//文件信息
	VS_FIXEDFILEINFO				* m_pFixedFileInfo;					//文件信息

	//函数定义
public: 
	//构造函数
    CWinFileInfo();
	//析构函数
    virtual ~CWinFileInfo(); 

	//功能函数
public:
	//打开文件
	bool OpenWinFile(LPCTSTR lpszFileName);
	//查询信息
    LPCTSTR QueryValue(LPCTSTR lpszValueName);

	//版本函数
public:
	//获取版本
    bool GetFileVersion(DWORD & dwFileVersionMS, DWORD & dwFileVersionLS);
	//获取版本
    bool GetProductVersion(DWORD & dwProductVersionMS, DWORD & dwProductVersionLS);

	//扩展信息
public:
	//文件信息
	VS_FIXEDFILEINFO * const GetFixedFileInfo() { return m_pFixedFileInfo; }
	//文件版本
    LPCTSTR GetFileVersion() { return QueryValue(TEXT("FileVersion")); }
	//产品版本
    LPCTSTR GetProductVersion() { return QueryValue(TEXT("ProductVersion")); }
	//内部名字
    LPCTSTR GetInternalName() { return QueryValue(TEXT("InternalName")); }
	//公司名字
    LPCTSTR GetCompanyName() { return QueryValue(TEXT("CompanyName")); } 
	//版权说明
    LPCTSTR GetLegalCopyright() { return QueryValue(TEXT("LegalCopyright")); }
	//原始名字
    LPCTSTR GetOriginalFilename() { return QueryValue(TEXT("OriginalFilename")); }
	//产品名字
    LPCTSTR GetProductName() { return QueryValue(TEXT("ProductName")); };
	//文件描述
    LPCTSTR GetFileDescription()  { return QueryValue(TEXT("FileDescription")); }
}; 

//////////////////////////////////////////////////////////////////////////

#endif