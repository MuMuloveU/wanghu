#include "StdAfx.h"
#include "Commonality.h"
#include "DataBaseSink.h"
#include "LogonServer.h"


//声明变量
extern CLogonServerApp theApp;
//////////////////////////////////////////////////////////////////////////

//构造函数
CDataBaseSink::CDataBaseSink()
{
	//设置变量
	m_pInitParamter=NULL;
	m_pIDataBaseEngineEvent=NULL;

	return;
}

//析构函数
CDataBaseSink::~CDataBaseSink()
{
}

//接口查询
VOID * __cdecl CDataBaseSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDataBaseEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDataBaseEngineSink,Guid,dwQueryVer);
	return NULL;
}

//模块启动
bool __cdecl CDataBaseSink::OnDataBaseEngineStart(IUnknownEx * pIUnknownEx)
{
	//创建对象
	if ((m_AccountsDBModule.GetInterface()==NULL)&&(m_AccountsDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}

	try
	{
		//连接用户数据库
		m_AccountsDBModule->SetConnectionInfo(m_pInitParamter->m_szUserDataBaseAddr,m_pInitParamter->m_wUserDataBasePort,
			m_pInitParamter->m_szUserDataBaseName,m_pInitParamter->m_szUserDataBaseUser,m_pInitParamter->m_szUserDataBasePass);
		m_AccountsDBModule->OpenConnection();

		//发起连接
		m_AccountsDBModule->OpenConnection();
		m_AccountsDBAide.SetDataBase(m_AccountsDBModule.GetInterface());

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		return false;
	}

	return true;
}

//模块关闭
bool __cdecl CDataBaseSink::OnDataBaseEngineStop(IUnknownEx * pIUnknownEx)
{
	//设置对象
	m_AccountsDBAide.SetDataBase(NULL);

	//关闭连接
	if (m_AccountsDBModule.GetInterface()!=NULL)
	{
		m_AccountsDBModule->CloseConnection();
		m_AccountsDBModule.CloseInstance();
	}

	//组件变量
	m_pInitParamter=NULL;
	m_pIDataBaseEngineEvent=NULL;

	return true;
}

//数据操作处理
bool __cdecl CDataBaseSink::OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	switch (wRequestID)
	{
	case DBR_GP_LOGON_BY_ACCOUNTS:		//帐号登录
		{
			return OnRequestLogon(wRequestID,dwContextID,pData,wDataSize);
		}
	case DBR_GP_LOGON_BY_USERID:		//I D 登录
		{
			return OnRequestLogon(wRequestID,dwContextID,pData,wDataSize);
		}
	case DBR_GP_REGISTER_ACCOUNTS:		//注册帐号
		{
			return OnRequestLogon(wRequestID,dwContextID,pData,wDataSize);
		}
	case DBR_GP_USER_UPLOAD_FACE:		//上传头像
		{
			return OnRequestUploadFace(wRequestID,dwContextID,pData,wDataSize);
		}
	case DBR_GP_DOWNLOAD_FACE:			//下载头像
		{
			return OnRequestDownloadFace(wRequestID,dwContextID,pData,wDataSize);
		}
	case DBR_GP_DELETE_FACE:			//删除消息
		{
			return OnRequestCustomFaceDelete(wRequestID,dwContextID,pData,wDataSize);
		}
	case DBR_GP_MODIFY_INDIVIDUAL:		//修改资料
		{
			return OnRequestModifyIndividual(wRequestID,dwContextID,pData,wDataSize);
		}
	}

	return false;
}

//登录请求处理
bool CDataBaseSink::OnRequestLogon(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//变量定义
		LONG lReturnValue=-1;
		LPCTSTR pszPassword=NULL;

		//登录处理
		switch (wRequestID)
		{
		case DBR_GP_LOGON_BY_ACCOUNTS:		//帐号登录
			{
				//效验参数
				ASSERT(wDataSize==sizeof(DBR_GP_LogonByAccounts));
				if (wDataSize!=sizeof(DBR_GP_LogonByAccounts)) return false;

				//执行查询
				DBR_GP_LogonByAccounts * pLogonByAccounts=(DBR_GP_LogonByAccounts *)pData;
				lReturnValue=SPLogonByAccounts(pLogonByAccounts->szAccounts,pLogonByAccounts->szPassWord,pLogonByAccounts->dwClientIP,pLogonByAccounts->szComputerID);
				pszPassword=pLogonByAccounts->szPassWord;

				break;
			}
		case DBR_GP_LOGON_BY_USERID:		//I D 登录
			{
				//效验参数
				ASSERT(wDataSize==sizeof(DBR_GP_LogonByUserID));
				if (wDataSize!=sizeof(DBR_GP_LogonByUserID)) return false;

				//执行查询
				DBR_GP_LogonByUserID * pLogonByUserID=(DBR_GP_LogonByUserID *)pData;
				lReturnValue=SPLogonByUserID(pLogonByUserID->dwUserID,pLogonByUserID->szPassWord,pLogonByUserID->dwClientIP,pLogonByUserID->szComputerID);
				pszPassword=pLogonByUserID->szPassWord;

				break;
			}
		case DBR_GP_REGISTER_ACCOUNTS:		//注册帐号
			{
				//效验参数
				ASSERT(wDataSize==sizeof(DBR_GP_RegisterAccounts));
				if (wDataSize!=sizeof(DBR_GP_RegisterAccounts)) return false;

				//执行查询
				DBR_GP_RegisterAccounts * pRegisterAccounts=(DBR_GP_RegisterAccounts *)pData;
				lReturnValue=SPRegisterAccounts(pRegisterAccounts->szAccounts,pRegisterAccounts->szPassWord,pRegisterAccounts->szSpreader,
					pRegisterAccounts->wFaceID,pRegisterAccounts->cbGender,pRegisterAccounts->dwClientIP,pRegisterAccounts->szComputerID);
				pszPassword=pRegisterAccounts->szPassWord;

				break;
			}
		default :	//默认处理
			{
				ASSERT(FALSE); 
				return false;
			}
		}

		//结果判断
		if (lReturnValue==0L)
		{
			//登录成功
			DBR_GP_LogonSuccess LogonSuccess;
			ZeroMemory(&LogonSuccess,sizeof(LogonSuccess));

			//读取变量
			LogonSuccess.wFaceID=m_AccountsDBAide.GetValue_WORD(TEXT("FaceID"));
			LogonSuccess.dwUserID=m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
			LogonSuccess.dwGameID=m_AccountsDBAide.GetValue_DWORD(TEXT("GameID"));
			LogonSuccess.cbGender=m_AccountsDBAide.GetValue_BYTE(TEXT("Gender"));
			LogonSuccess.cbMember=m_AccountsDBAide.GetValue_BYTE(TEXT("MemberOrder"));
			LogonSuccess.dwExperience=m_AccountsDBAide.GetValue_DWORD(TEXT("Experience"));
			LogonSuccess.dwCustomFaceVer=m_AccountsDBAide.GetValue_DWORD(TEXT("CustomFaceVer"));
			m_AccountsDBAide.GetValue_String(TEXT("Accounts"),LogonSuccess.szAccounts,CountArray(LogonSuccess.szAccounts));
			m_AccountsDBAide.GetValue_String(TEXT("UnderWrite"),LogonSuccess.szUnderWrite,CountArray(LogonSuccess.szUnderWrite));
			m_AccountsDBAide.GetValue_String(TEXT("ErrorDescribe"),LogonSuccess.szDescribeString,CountArray(LogonSuccess.szDescribeString));

			//设置密码
			CopyMemory(LogonSuccess.szPassword,pszPassword,CountArray(LogonSuccess.szPassword));

			//投递结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_LOGON_SUCCESS,dwContextID,&LogonSuccess,sizeof(LogonSuccess));
		}
		else
		{
			//登录失败
			DBR_GP_LogonError LogonError;
			ZeroMemory(&LogonError,sizeof(LogonError));

			//设置变量
			LogonError.lErrorCode=lReturnValue;
			m_AccountsDBAide.GetValue_String(TEXT("ErrorDescribe"),LogonError.szErrorDescribe,CountArray(LogonError.szErrorDescribe));

			//投递结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_LOGON_ERROR,dwContextID,&LogonError,sizeof(LogonError));
		}
	}
	catch (IDataBaseException * pIException)
	{
		//登录失败
		DBR_GP_LogonError LogonError;
		ZeroMemory(&LogonError,sizeof(LogonError));

		//输出错误
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//操作失败
		LogonError.lErrorCode=-1;
		lstrcpyn(LogonError.szErrorDescribe,TEXT("由于数据库操作异常，请您稍后重试或选择另一服务器登录！"),CountArray(LogonError.szErrorDescribe));

		//投递结果
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_LOGON_ERROR,dwContextID,&LogonError,sizeof(LogonError));
	}

	//关闭记录集
	m_AccountsDBModule->CloseRecordset();

	return true;
}

//上传头像
bool CDataBaseSink::OnRequestUploadFace(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//数据验证
	ASSERT(wDataSize==sizeof(DBR_GP_UploadCustomFace));
	if (wDataSize!=sizeof(DBR_GP_UploadCustomFace)) return false;

	//数据转换
	DBR_GP_UploadCustomFace *pUploadCustomFace = (DBR_GP_UploadCustomFace*)pData;

	//文件定义
	CFile fileCustomFace;
	TCHAR szFileName[128];
	_snprintf(szFileName, CountArray(szFileName), TEXT("%s\\UploadFile_%ld.zip"), theApp.m_szDirWork, pUploadCustomFace->dwUserID);

	try
	{
		if ( fileCustomFace.Open( szFileName, CFile::modeRead|CFile::typeBinary) )
		{
			BYTE cbCustomFaceData[1024*30];

			//读取文件
			UINT uReadSize = fileCustomFace.Read(cbCustomFaceData, CountArray(cbCustomFaceData));

			//关闭文件
			fileCustomFace.Close();

			LONG lRet=-1;
			try
			{
				lRet = SPUploadFace(pUploadCustomFace->dwUserID, cbCustomFaceData, uReadSize);
			}
			catch (IDataBaseException * pIException)
			{
				//错误信息
				LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
				CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
			}

			//成功判断
			if ( lRet != -1 )
			{
				DBR_GP_UploadFaceResult UploadFaceResult;
				ZeroMemory(&UploadFaceResult, sizeof(UploadFaceResult));
				lstrcpyn(UploadFaceResult.szDescribeMsg, TEXT("头像上传成功！"), sizeof(UploadFaceResult.szDescribeMsg));
				UploadFaceResult.dwFaceVer=lRet;
				UploadFaceResult.bOperateSuccess=true;

				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_UPLOAD_FACE_RESULT,dwContextID,
					&UploadFaceResult,sizeof(UploadFaceResult));
			}
			else
			{
				//操作失败
				DBR_GP_UploadFaceResult UploadFaceResult;
				ZeroMemory(&UploadFaceResult, sizeof(UploadFaceResult));
				lstrcpyn(UploadFaceResult.szDescribeMsg, TEXT("由于数据库操作异常，请重新上传！"), sizeof(UploadFaceResult.szDescribeMsg));
				UploadFaceResult.bOperateSuccess=false;

				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_UPLOAD_FACE_RESULT,dwContextID,
					&UploadFaceResult,sizeof(UploadFaceResult));
			}

			//删除文件
			CFile::Remove(szFileName);
		}
	}catch(...){}	

	return true;
}

//下载头像
bool CDataBaseSink::OnRequestDownloadFace(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//校验参数
	ASSERT(wDataSize == sizeof(DBR_GP_DownloadCustomFace));
	if ( wDataSize != sizeof(DBR_GP_DownloadCustomFace)) return false;

	//类型转换
	DBR_GP_DownloadCustomFace *pDownloadCustomFace = (DBR_GP_DownloadCustomFace*)pData;

	LONG lReturnValue=-1;
	long lSize=0;
	bool bRet = false;
	try
	{
		lReturnValue=SPDownloadFace(pDownloadCustomFace->dwUserID);

		//读取数据
		if ( lReturnValue==0 )
		{	
			DWORD dwImageSize=0;
			dwImageSize=m_AccountsDBAide.GetValue_DWORD(TEXT("ImgSize"));
			
			//读取数据
			IDataBase * pIDataBase=m_AccountsDBModule.GetInterface();
			_variant_t vtFld;
			pIDataBase->GetRecordsetValue(TEXT("CustomFaceImage"), vtFld);
			BYTE * pcbBuf=NULL;
			SafeArrayAccessData(vtFld.parray,(LPVOID *)&pcbBuf);

			//投递请求
			DBR_GP_DownloadFaceSuccess DownloadFaceSuccess;
			ZeroMemory(&DownloadFaceSuccess, sizeof(DownloadFaceSuccess));
			DownloadFaceSuccess.dwToltalSize = dwImageSize;
			DownloadFaceSuccess.dwUserID = pDownloadCustomFace->dwUserID;
			DWORD dwSendSize = 0;
			while( dwSendSize <dwImageSize )
			{
				//设置数据
				if ( 1024 < dwImageSize - dwSendSize ) DownloadFaceSuccess.dwCurrentSize = 1024;
				else DownloadFaceSuccess.dwCurrentSize = dwImageSize - dwSendSize ;
				CopyMemory(DownloadFaceSuccess.bFaceData, pcbBuf+dwSendSize, DownloadFaceSuccess.dwCurrentSize);

				//投递消息
				WORD wPostSize = WORD(sizeof(DownloadFaceSuccess)-sizeof(DownloadFaceSuccess.bFaceData)+DownloadFaceSuccess.dwCurrentSize);
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_DOWNLOADFACE_SUCCESS, dwContextID, &DownloadFaceSuccess, wPostSize);

				//递增变量
				dwSendSize += DownloadFaceSuccess.dwCurrentSize;
			}

			//释放资源
			SafeArrayUnaccessData(vtFld.parray);
		}
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}


	return true;
}

//删除消息
bool CDataBaseSink::OnRequestCustomFaceDelete(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数验证
	ASSERT(wDataSize == sizeof(DBR_GP_CustomFaceDelete));
	if ( wDataSize != sizeof(DBR_GP_CustomFaceDelete)) return false;

	//类型转换
	DBR_GP_CustomFaceDelete *pCustomFaceDelete = (DBR_GP_CustomFaceDelete*)pData;

	//执行存储过程
	LONG lReturnValue = -1;
	try
	{
		lReturnValue = SPDeleteCustomFace(pCustomFaceDelete->dwUserID);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	//成功判断
	if ( lReturnValue == 0 )
	{
		DBR_GP_DeleteFaceResult DeleteFaceResult;
		ZeroMemory(&DeleteFaceResult, sizeof(DeleteFaceResult));
		lstrcpyn(DeleteFaceResult.szDescribeMsg, TEXT("头像删除成功！"), sizeof(DeleteFaceResult.szDescribeMsg));
		DeleteFaceResult.dwFaceVer=pCustomFaceDelete->dwFaceVer;
		DeleteFaceResult.bOperateSuccess = true;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_DELETE_FACE_RESULT,dwContextID,
			&DeleteFaceResult,sizeof(DeleteFaceResult));
	}
	else
	{
		DBR_GP_DeleteFaceResult DeleteFaceResult;
		ZeroMemory(&DeleteFaceResult, sizeof(DeleteFaceResult));
		lstrcpyn(DeleteFaceResult.szDescribeMsg, TEXT("由于数据库操作异常，删除失败！"), sizeof(DeleteFaceResult.szDescribeMsg));
		DeleteFaceResult.dwFaceVer=pCustomFaceDelete->dwFaceVer;
		DeleteFaceResult.bOperateSuccess = false;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_DELETE_FACE_RESULT,dwContextID,
			&DeleteFaceResult,sizeof(DeleteFaceResult));
	}

	return true;
}

//修改资料
bool CDataBaseSink::OnRequestModifyIndividual(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数验证
	ASSERT(wDataSize == sizeof(DBR_GP_ModifyIndividual));
	if ( wDataSize != sizeof(DBR_GP_ModifyIndividual)) return false;

	//类型转换
	DBR_GP_ModifyIndividual *pModifyIndividual = (DBR_GP_ModifyIndividual*)pData;

	//执行存储过程
	LONG lReturnValue = -1;
	try
	{
		lReturnValue = SPModifyIndividual(pModifyIndividual->dwUserID, pModifyIndividual->szNickname, pModifyIndividual->nGender,
			pModifyIndividual->nAge, pModifyIndividual->szAddress, pModifyIndividual->szUnderWrite, pModifyIndividual->szPassword);
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);
	}

	//成功判断
	if ( lReturnValue == 0 )
	{
		DBR_GP_ModifyIndividualResult ModifyIndividualResult;
		ZeroMemory(&ModifyIndividualResult, sizeof(ModifyIndividualResult));
		lstrcpyn(ModifyIndividualResult.szDescribeMsg, TEXT("资料修改成功！"), sizeof(ModifyIndividualResult.szDescribeMsg));
		ModifyIndividualResult.bOperateSuccess = true;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_MODIFY_RESULT,dwContextID,
			&ModifyIndividualResult,sizeof(ModifyIndividualResult));
	}
	else
	{
		DBR_GP_ModifyIndividualResult ModifyIndividualResult;
		ZeroMemory(&ModifyIndividualResult, sizeof(ModifyIndividualResult));
		lstrcpyn(ModifyIndividualResult.szDescribeMsg, TEXT("由于数据库操作异常，修改失败！"), sizeof(ModifyIndividualResult.szDescribeMsg));
		ModifyIndividualResult.bOperateSuccess = false;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GP_MODIFY_RESULT,dwContextID,
			&ModifyIndividualResult,sizeof(ModifyIndividualResult));
	}
	return true;
}

//帐号存储过程
LONG CDataBaseSink::SPLogonByAccounts(LPCTSTR pszAccounts, LPCTSTR pszPassword, DWORD dwClientIP, LPCTSTR pszComputerID)
{
	//效验参数
	ASSERT(pszAccounts!=NULL);
	ASSERT(pszPassword!=NULL);

	//转化地址
	TCHAR szClientIP[16]=TEXT("");
	BYTE * pClientIP=(BYTE *)&dwClientIP;
	_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@strAccounts"),pszAccounts);
	m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pszPassword);
	m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);
	m_AccountsDBAide.AddParameter(TEXT("@strMachineSerial"),pszComputerID);

	return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_EfficacyAccounts"),true);
}

//更新存储过程
LONG CDataBaseSink::SPRegisterAccounts(LPCTSTR pszAccounts, LPCTSTR pszPassword, LPCTSTR pszSpreader, WORD wFaceID, BYTE cbGender, DWORD dwClientIP, LPCTSTR pszComputerID)
{
	//效验参数
	ASSERT(pszAccounts!=NULL);
	ASSERT(pszPassword!=NULL);

	//转化地址
	TCHAR szClientIP[16]=TEXT("");
	BYTE * pClientIP=(BYTE *)&dwClientIP;
	_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@strAccounts"),pszAccounts);
	m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pszPassword);
	m_AccountsDBAide.AddParameter(TEXT("@strSpreader"),pszSpreader);
	m_AccountsDBAide.AddParameter(TEXT("@nFaceID"),wFaceID);
	m_AccountsDBAide.AddParameter(TEXT("@cbGender"),cbGender);
	m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);
	m_AccountsDBAide.AddParameter(TEXT("@strMachineSerial"),pszComputerID);

	return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_RegisterAccounts"),true);
}

//I D 存储过程
LONG CDataBaseSink::SPLogonByUserID(DWORD dwGameID, LPCTSTR pszPassword, DWORD dwClientIP, LPCTSTR pszComputerID)
{
	//效验参数
	ASSERT(dwGameID!=0L);
	ASSERT(pszPassword!=NULL);

	//转化地址
	TCHAR szClientIP[16]=TEXT("");
	BYTE * pClientIP=(BYTE *)&dwClientIP;
	_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@dwGameID"),dwGameID);
	m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pszPassword);
	m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);
	m_AccountsDBAide.AddParameter(TEXT("@strMachineSerial"),pszComputerID);

	return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_EfficacyGameID"),true);
}

//上传存储过程
LONG CDataBaseSink::SPUploadFace(DWORD dwUserID, BYTE *bFaceData, DWORD dwFaceDataSize)
{
	//效验参数
	ASSERT(dwUserID!=0L);
	ASSERT(dwFaceDataSize!=0L);
	ASSERT(bFaceData!=NULL);

	//效验参数
	ASSERT(dwUserID!=0L);
	ASSERT(dwFaceDataSize!=0L);
	ASSERT(bFaceData!=NULL);

	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);

	VARIANT varChunk;
	SAFEARRAY *psa=NULL;
	SAFEARRAYBOUND rgsabound[1];
	rgsabound[0].cElements = dwFaceDataSize;
    rgsabound[0].lLbound = 0;
    psa = SafeArrayCreate(VT_UI1,1,rgsabound);
    for (long index =0; index < long(dwFaceDataSize); index++ )
	{
		SafeArrayPutElement(psa,&index,&bFaceData[index]);
	}
    varChunk.vt = VT_ARRAY|VT_UI1;
    varChunk.parray = psa;

	IDataBase * pIDataBase=m_AccountsDBModule.GetInterface();
	pIDataBase->AddParameter(TEXT("@imgCustomFaceImage"),adLongVarBinary,adParamInput,dwFaceDataSize,_variant_t(varChunk));

	LONG lReturnValue=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CF_InsertCustomFace"),false);

	//释放资源
	SafeArrayDestroy(psa);

	return lReturnValue;
}

//加载存储过程
LONG CDataBaseSink::SPDownloadFace(DWORD dwUserID)
{
	//效验参数
	ASSERT(dwUserID!=0L);

	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);


	return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CF_LoadCustomFace"),true);
}

//删除存储过程
LONG CDataBaseSink::SPDeleteCustomFace(DWORD dwUserID)
{
	//效验参数
	ASSERT(dwUserID!=0L);

	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);

	return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CF_DeleteCustomFace"),true);
}

//修改存储过程
LONG CDataBaseSink::SPModifyIndividual(DWORD dwUserID, TCHAR szNickname[NAME_LEN], int nGender, int nAge, TCHAR szAddress[64], TCHAR szUnderWrite[32], TCHAR szPassword[32])
{
	//执行存储过程
	m_AccountsDBAide.ResetParameter();
	m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),dwUserID);
	m_AccountsDBAide.AddParameter(TEXT("@strLogonPass"),szPassword);
	m_AccountsDBAide.AddParameter(TEXT("@strNickName"),szNickname);
	m_AccountsDBAide.AddParameter(TEXT("@nGender"),nGender);
	m_AccountsDBAide.AddParameter(TEXT("@nAge"),nAge);
	m_AccountsDBAide.AddParameter(TEXT("@strAddress"),szAddress);
	m_AccountsDBAide.AddParameter(TEXT("@strUnderWrite"),szUnderWrite);

	return m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CF_ModifyIndividual"),false);
}
//////////////////////////////////////////////////////////////////////////
