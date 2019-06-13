#include "Stdafx.h"
#include "WinFileInfo.h"

//导入库说明
#pragma comment(lib,"Version")

//////////////////////////////////////////////////////////////////////////

//构造函数
CWinFileInfo::CWinFileInfo() 
{
	m_dwDataSize=0;
	m_dwLangCharset=0;
	m_pFixedFileInfo=NULL;
	memset(m_cbInfoBuffer,0,sizeof(m_cbInfoBuffer));
}

//析构函数
CWinFileInfo::~CWinFileInfo() 
{ 
} 

//打开文件
bool CWinFileInfo::OpenWinFile(LPCTSTR lpszFileName)
{
	//设置变量
	m_dwDataSize=0;
	m_dwLangCharset=0;
	m_pFixedFileInfo=NULL;
	memset(m_cbInfoBuffer,0,sizeof(m_cbInfoBuffer));

	//打开文件
    DWORD dwFileHandle=NULL;
	DWORD dwBufferSize=::GetFileVersionInfoSize((LPTSTR)lpszFileName,&dwFileHandle);
    if ((dwBufferSize==0)||(dwBufferSize>sizeof(m_cbInfoBuffer))) return false;

	//文件信息
	m_dwDataSize=dwBufferSize;
    if (::GetFileVersionInfo(lpszFileName,dwFileHandle,sizeof(m_cbInfoBuffer),m_cbInfoBuffer)==FALSE) return false;

	//语言信息
	UINT uQuerySize=0;
    DWORD * pTransTable=NULL;
	BOOL bSuccess=::VerQueryValue(m_cbInfoBuffer,TEXT("\\VarFileInfo\\Translation"),(LPVOID *)&pTransTable,&uQuerySize);
    if (bSuccess) m_dwLangCharset=MAKELONG(HIWORD(pTransTable[0]),LOWORD(pTransTable[0]));

	//版本信息
    if (::VerQueryValue(m_cbInfoBuffer,TEXT("\\"),(void * *)&m_pFixedFileInfo,&uQuerySize)==FALSE) return false;
	if ((uQuerySize!=sizeof(VS_FIXEDFILEINFO))||(m_pFixedFileInfo==NULL)) return false;

    return true;
}

//查询信息
LPCTSTR CWinFileInfo::QueryValue(LPCTSTR lpszValueName)
{
    UINT nQuerySize=0;
    LPTSTR pszValue=NULL;
	TCHAR szBlockName[128]=TEXT("");
	_snprintf(szBlockName,sizeof(szBlockName),TEXT("\\StringFileInfo\\%08lx\\%s"),m_dwLangCharset,lpszValueName);
    ::VerQueryValue(m_cbInfoBuffer,szBlockName,(LPVOID *)&pszValue,&nQuerySize);
	return pszValue;
}

//获取版本
bool CWinFileInfo::GetFileVersion(DWORD & dwFileVersionMS, DWORD & dwFileVersionLS)
{
	//效验参数
	ASSERT(m_pFixedFileInfo!=NULL);
	if (m_pFixedFileInfo==NULL) 
	{
		dwFileVersionMS=0L;
		dwFileVersionLS=0L;
		return false;
	}

	//设置变量
	dwFileVersionMS=m_pFixedFileInfo->dwFileVersionMS;
	dwFileVersionLS=m_pFixedFileInfo->dwFileVersionLS;

	return true;
}

//获取版本
bool CWinFileInfo::GetProductVersion(DWORD & dwProductVersionMS, DWORD & dwProductVersionLS)
{
	//效验参数
	ASSERT(m_pFixedFileInfo!=NULL);
	if (m_pFixedFileInfo==NULL)
	{
		dwProductVersionMS=0L;
		dwProductVersionLS=0L;
		return false;
	}

	//设置变量
	dwProductVersionMS=m_pFixedFileInfo->dwProductVersionMS;
	dwProductVersionLS=m_pFixedFileInfo->dwProductVersionLS;

    return true;
}

//////////////////////////////////////////////////////////////////////////
