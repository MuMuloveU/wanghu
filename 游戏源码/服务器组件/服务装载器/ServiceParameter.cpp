#include "StdAfx.h"
#include "Resource.h"
#include "ServiceParameter.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CServiceParameter, CDialog)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CServiceParameter::CServiceParameter() : CDialog(IDD_PARAMETER)
{
	m_bLoaded=false;
	m_hDllInstance=NULL;
	m_pIGameServiceManager=NULL;
	memset(&m_OptionParameter,0,sizeof(m_OptionParameter));
	memset(&m_GameServiceAttrib,0,sizeof(m_GameServiceAttrib));
}

//析构函数
CServiceParameter::~CServiceParameter()
{
	FreeGameServiceModule();
}

//控件子类化
void CServiceParameter::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
}

//初始化函数
BOOL CServiceParameter::OnInitDialog()
{
	__super::OnInitDialog();

	//效验参数
	ASSERT(m_hDllInstance!=NULL);
	ASSERT(m_pIGameServiceManager!=NULL);

	//设置控件
	((CEdit *)GetDlgItem(IDC_PORT))->LimitText(5);
	((CEdit *)GetDlgItem(IDC_SERVER_ID))->LimitText(5);
	((CEdit *)GetDlgItem(IDC_STATION_ID))->LimitText(5);
	((CEdit *)GetDlgItem(IDC_TABLE_COUNT))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_MAX_CONNECT))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_DATABASE_NAME))->LimitText(31);
	((CEdit *)GetDlgItem(IDC_SERVER_NAME))->LimitText(SERVER_LEN-1);

	//服务地址
	TCHAR szHostName[64],szServerAddr[16];
	gethostname(szHostName,sizeof(szHostName));
	HOSTENT * pHostent=gethostbyname(szHostName);
    LPIN_ADDR * lpAddrList=(LPIN_ADDR *)pHostent->h_addr_list;
	CComboBox * pServerAddr=(CComboBox *)GetDlgItem(IDC_SERVER_ADDR);
    while (*lpAddrList!=NULL)
	{
		lstrcpyn(szServerAddr,inet_ntoa(**lpAddrList),CountArray(szServerAddr));
		int nIndex=pServerAddr->AddString(szServerAddr);
		pServerAddr->SetItemData(nIndex,(*lpAddrList)->S_un.S_addr);
		lpAddrList++;
	}

	//更新信息
	UpdateOptionParameter();

	return TRUE;
}

//确定函数
void CServiceParameter::OnOK()
{
	//变量定义
	tagOptionParameter OptionParameter;
	memset(&OptionParameter,0,sizeof(OptionParameter));
	lstrcpyn(OptionParameter.szModuleName,m_GameServiceAttrib.szServerModuleName,CountArray(OptionParameter.szModuleName));

	//房间标识
	OptionParameter.GameServiceOption.wServerID=GetDlgItemInt(IDC_SERVER_ID,NULL,FALSE);
	if (OptionParameter.GameServiceOption.wServerID==0)
	{
		AfxMessageBox(TEXT("请输入正确的房间标识号码"),MB_ICONINFORMATION);
		GetDlgItem(IDC_SERVER_ID)->SetFocus();
		return;
	}

	//类型标识
	OptionParameter.GameServiceOption.wKindID=GetDlgItemInt(IDC_OPTION_KIND_ID,FALSE);
	if (OptionParameter.GameServiceOption.wKindID==0) OptionParameter.GameServiceOption.wKindID=m_GameServiceAttrib.wKindID;

	//排序标识
	OptionParameter.GameServiceOption.wSortID=GetDlgItemInt(IDC_OPTION_SORT_ID,FALSE);
	if (OptionParameter.GameServiceOption.wSortID==0) OptionParameter.GameServiceOption.wSortID=200;

	//站点标识
	OptionParameter.GameServiceOption.wNodeID=GetDlgItemInt(IDC_STATION_ID,NULL,FALSE);

	//桌子数目
	OptionParameter.GameServiceOption.wTableCount=GetDlgItemInt(IDC_TABLE_COUNT,NULL,FALSE);
	if (OptionParameter.GameServiceOption.wTableCount==0)
	{
		AfxMessageBox(TEXT("请输入正确的桌子数目"),MB_ICONINFORMATION);
		GetDlgItem(IDC_TABLE_COUNT)->SetFocus();
		return;
	}

	//单元积分
	OptionParameter.GameServiceOption.lCellScore=GetDlgItemInt(IDC_CELL_SCORE,NULL,FALSE);

	//最低积分
	OptionParameter.GameServiceOption.lLessScore=GetDlgItemInt(IDC_LESS_SCORE,NULL,FALSE);

	//最高积分
	OptionParameter.GameServiceOption.lMaxScore=GetDlgItemInt(IDC_MAX_SCORE,NULL,FALSE);

	//限制积分
	OptionParameter.GameServiceOption.lRestrictScore=GetDlgItemInt(IDC_RESTRICT_SCORE,NULL,FALSE);

	//游戏税收
	OptionParameter.GameServiceOption.wRevenue=GetDlgItemInt(IDC_REVENUE,NULL,FALSE);
	OptionParameter.GameServiceOption.wRevenue=__min(OptionParameter.GameServiceOption.wRevenue,1000);

	//隐藏信息
	OptionParameter.GameServiceOption.cbHideUserInfo=(((CButton *)GetDlgItem(IDC_HIDE_USER_INFO))->GetCheck()==BST_CHECKED)?TRUE:FALSE;

	//限制旁观
	OptionParameter.GameServiceOption.cbUnLookOnTag=(((CButton *)GetDlgItem(IDC_LIMIT_LOOK_ON))->GetCheck()==BST_CHECKED)?TRUE:FALSE;

	//限制同IP
	OptionParameter.GameServiceOption.cbUnSameIPPlay=(((CButton *)GetDlgItem(IDC_LIMIT_SAME_IP))->GetCheck()==BST_CHECKED)?TRUE:FALSE;

	//控制开始
	OptionParameter.GameServiceOption.cbControlStart=(((CButton *)GetDlgItem(IDC_CONTROL_START))->GetCheck()==BST_CHECKED)?TRUE:FALSE;

	//比赛局数
	OptionParameter.GameServiceOption.lMatchDraw=GetDlgItemInt(IDC_MATCH_DRAW,NULL,FALSE);;

	//最大连接
	OptionParameter.GameServiceOption.wMaxConnect=GetDlgItemInt(IDC_MAX_CONNECT,NULL,FALSE);
	WORD wMaxConnect=m_GameServiceAttrib.wChairCount*OptionParameter.GameServiceOption.wTableCount;
	OptionParameter.GameServiceOption.wMaxConnect=__max(wMaxConnect,OptionParameter.GameServiceOption.wMaxConnect);

	//游戏类型
	CComboBox * pGameGenre=(CComboBox *)GetDlgItem(IDC_GAME_GENRE);
	int nCurSel=pGameGenre->GetCurSel();
	if (nCurSel==CB_ERR)
	{
		AfxMessageBox(TEXT("请选择正确的游戏类型"),MB_ICONINFORMATION);
		GetDlgItem(IDC_GAME_GENRE)->SetFocus();
		return;
	}
	OptionParameter.GameServiceOption.wServerType=(WORD)pGameGenre->GetItemData(nCurSel);

	//房间名字
	GetDlgItemText(IDC_SERVER_NAME,OptionParameter.GameServiceOption.szGameRoomName,CountArray(OptionParameter.GameServiceOption.szGameRoomName));
	if (OptionParameter.GameServiceOption.szGameRoomName[0]==0)
	{
		AfxMessageBox(TEXT("请填写正确的游戏房间名字"),MB_ICONINFORMATION);
		GetDlgItem(IDC_SERVER_NAME)->SetFocus();
		return;
	}

	//监听端口
	OptionParameter.GameServiceOption.wServerPort=GetDlgItemInt(IDC_PORT,NULL,FALSE);
	if (OptionParameter.GameServiceOption.wServerPort==0)
	{
		AfxMessageBox(TEXT("请填写正确的房间监听端口"),MB_ICONINFORMATION);
		GetDlgItem(IDC_PORT)->SetFocus();
		return;
	}

	//视频地址
	((CIPAddressCtrl *)GetDlgItem(IDC_VIDEO_ADDR))->GetAddress(OptionParameter.GameServiceOption.dwVideoServer);

	//服务地址
	CComboBox * pServerAddr=(CComboBox *)GetDlgItem(IDC_SERVER_ADDR);
	nCurSel=pServerAddr->GetCurSel();
	if (nCurSel==CB_ERR)
	{
		TCHAR szServerAddr[256]=TEXT("");
		pServerAddr->GetWindowText(szServerAddr,CountArray(szServerAddr));
		if (szServerAddr[0]==0)
		{
			AfxMessageBox(TEXT("请选择正确的服务地址"),MB_ICONINFORMATION);
			GetDlgItem(IDC_SERVER_ADDR)->SetFocus();
			return;
		}
		OptionParameter.GameServiceOption.dwServerAddr=inet_addr(szServerAddr);
	}
	else OptionParameter.GameServiceOption.dwServerAddr=(DWORD)pServerAddr->GetItemData(nCurSel);

	//数据库名字
	GetDlgItemText(IDC_DATABASE_NAME,OptionParameter.szDataBaseName,CountArray(OptionParameter.szDataBaseName));
	if (OptionParameter.szDataBaseName[0]==0)
	{
		AfxMessageBox(TEXT("请填写正确的游戏数据库名字"),MB_ICONINFORMATION);
		GetDlgItem(IDC_DATABASE_NAME)->SetFocus();
		return;
	}

	//数据库地址
	DWORD dwDataBaseAddr=0L;
	((CIPAddressCtrl *)GetDlgItem(IDC_DATABASE_ADDR))->GetAddress(dwDataBaseAddr);
	if (dwDataBaseAddr==0L)
	{
		AfxMessageBox(TEXT("请填写正确的游戏数据库地址"),MB_ICONINFORMATION);
		GetDlgItem(IDC_DATABASE_ADDR)->SetFocus();
		return;
	}

	//保存地址
	BYTE * pcbDataBaseAddr=(BYTE *)&dwDataBaseAddr;
	_snprintf(OptionParameter.szDataBaseAddr,sizeof(OptionParameter.szDataBaseAddr),TEXT("%d.%d.%d.%d"),
		pcbDataBaseAddr[3],pcbDataBaseAddr[2],pcbDataBaseAddr[1],pcbDataBaseAddr[0]);

	//保存参数
	m_bLoaded=true;
	m_OptionParameter=OptionParameter;

	__super::OnOK();
}

//加载配置
bool CServiceParameter::LoadParameter(LPCTSTR pszFileName)
{
	//效验参数
	ASSERT(pszFileName!=NULL);

	//打开文件
	CFile ParameterFile;
	if (ParameterFile.Open(pszFileName,CFile::modeRead)==FALSE) return false;

	//读取大小
	LONGLONG lFileLength=ParameterFile.GetLength();
	ASSERT(lFileLength==sizeof(tagRecordParameter));
	if (lFileLength!=sizeof(tagRecordParameter)) return false;

	//读取信息
	tagRecordParameter RecordParameter;
	UINT uReadCount=ParameterFile.Read(&RecordParameter,sizeof(RecordParameter));
	ASSERT(uReadCount==sizeof(tagRecordParameter));
	if (uReadCount!=sizeof(tagRecordParameter)) return false;

	//效验信息
	ASSERT(RecordParameter.wVersion==VER_PARAMETER);
	ASSERT(RecordParameter.wFileSize==sizeof(tagRecordParameter));
	if (RecordParameter.wVersion!=VER_PARAMETER) return false;
	if (RecordParameter.wFileSize!=sizeof(tagRecordParameter)) return false;

	//保存信息
	m_bLoaded=true;
	m_OptionParameter=RecordParameter.OptionParameter;

	//关闭文件
	ParameterFile.Close();

	return true;
}

//保存配置
bool CServiceParameter::SaveParameter(LPCTSTR pszFileName)
{
	//效验参数
	ASSERT(m_bLoaded==true);
	ASSERT(pszFileName!=NULL);

	//打开文件
	CFile ParameterFile;
	if (ParameterFile.Open(pszFileName,CFile::modeWrite|CFile::modeCreate)==FALSE) return false;

	//构造信息
	tagRecordParameter RecordParameter;
	RecordParameter.wVersion=VER_PARAMETER;
	RecordParameter.wFileSize=sizeof(RecordParameter);
	RecordParameter.OptionParameter=m_OptionParameter;

	//写入文件
	ParameterFile.Write(&RecordParameter,sizeof(RecordParameter));
	ParameterFile.SetLength(RecordParameter.wFileSize);

	//关闭文件
	ParameterFile.Close();

	return true;
}

//重置参数
void CServiceParameter::ResetParameter()
{
	//释放组件
	FreeGameServiceModule();

	//设置变量
	m_bLoaded=false;
	memset(&m_OptionParameter,0,sizeof(m_OptionParameter));

	//游戏税收
	m_OptionParameter.GameServiceOption.wRevenue=5;

	return;
}

//修改参数
bool CServiceParameter::ConfigParameter()
{
	//效验状态
	ASSERT(m_bLoaded==true);
	ASSERT(m_hDllInstance==NULL);

	//加载信息
	if (LoadGameServiceModule(m_OptionParameter.szModuleName)==false)
	{
		AfxMessageBox(TEXT("“服务组件不存在”或“无法识别”，无法配置游戏房间！"),MB_ICONINFORMATION);
		return false;
	}

	//配置参数
	INT_PTR nResult=DoModal();

	//释放模块
	FreeGameServiceModule();

	return (nResult==IDOK);
}

//创建参数
bool CServiceParameter::CreateParameter(LPCTSTR pszModuleName)
{
	//重置参数
	ResetParameter();

	//加载模块
	if (LoadGameServiceModule(pszModuleName)==false)
	{
		AfxMessageBox(TEXT("“服务组件不存在”或“无法识别”，无法创建游戏房间！"),MB_ICONINFORMATION);
		return false;
	}

	//配置参数
	INT_PTR nResult=DoModal();

	//释放模块
	FreeGameServiceModule();

	return (nResult==IDOK);
}

//加载模块
bool CServiceParameter::LoadGameServiceModule(LPCTSTR pszModuleName)
{
	//效验状态
	ASSERT(m_hDllInstance==NULL);
	ASSERT(m_pIGameServiceManager==NULL);

	//加载模块
	m_hDllInstance=AfxLoadLibrary(pszModuleName);
	if (m_hDllInstance==NULL)
	{
		FreeGameServiceModule();
		return false;
	}

	//查找函数
	ModuleCreateProc * CreateFunc=(ModuleCreateProc *)GetProcAddress(m_hDllInstance,TEXT("CreateGameServiceManager"));
	if (CreateFunc==NULL)
	{
		FreeGameServiceModule();
		return false;
	}

	//获取接口
	m_pIGameServiceManager=(IGameServiceManager *)CreateFunc(IID_IGameServiceManager,VER_IGameServiceManager);
	if (m_pIGameServiceManager==NULL)
	{
		FreeGameServiceModule();
		return false;
	}

	//获取属性
	m_pIGameServiceManager->GetGameServiceAttrib(m_GameServiceAttrib);
	lstrcpyn(m_OptionParameter.szModuleName,m_GameServiceAttrib.szServerModuleName,CountArray(m_OptionParameter.szModuleName));

	return true;
}

//释放模块
void CServiceParameter::FreeGameServiceModule()
{
	//释放模块
	if (m_pIGameServiceManager!=NULL)
	{
		m_pIGameServiceManager->Release();
		m_pIGameServiceManager=NULL;
	}
	if (m_hDllInstance!=NULL)
	{
		AfxFreeLibrary(m_hDllInstance);
		m_hDllInstance=NULL;
	}

	//设置变量
	memset(&m_GameServiceAttrib,0,sizeof(m_GameServiceAttrib));

	return;
}

//更新配置
void CServiceParameter::UpdateOptionParameter()
{
	//变量定义
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(IDC_GAME_GENRE);
	WORD wGameGenre[4]={GAME_GENRE_EDUCATE,GAME_GENRE_MATCH,GAME_GENRE_GOLD,GAME_GENRE_SCORE};
	LPCTSTR pszGameGenre[4]={TEXT("练习类型"),TEXT("比赛类型"),TEXT("金币类型"),TEXT("点值类型")};

	//游戏类型
	pComboBox->Clear();
	for (int i=0;i<CountArray(wGameGenre);i++)
	{
		int nIndex=pComboBox->AddString(pszGameGenre[i]);
		pComboBox->SetItemData(nIndex,wGameGenre[i]);
		if (m_OptionParameter.GameServiceOption.wServerType==wGameGenre[i]) pComboBox->SetCurSel(nIndex);
	}

	//类型标识
	if (m_OptionParameter.GameServiceOption.wKindID==0) SetDlgItemText(IDC_OPTION_KIND_ID,TEXT(""));
	else SetDlgItemInt(IDC_OPTION_KIND_ID,m_OptionParameter.GameServiceOption.wKindID,FALSE);

	//排序标识
	if (m_OptionParameter.GameServiceOption.wSortID==0) SetDlgItemText(IDC_OPTION_SORT_ID,TEXT(""));
	else SetDlgItemInt(IDC_OPTION_SORT_ID,m_OptionParameter.GameServiceOption.wSortID,FALSE);

	//房间标识
	if (m_OptionParameter.GameServiceOption.wServerID==0) SetDlgItemText(IDC_SERVER_ID,TEXT(""));
	else SetDlgItemInt(IDC_SERVER_ID,m_OptionParameter.GameServiceOption.wServerID,FALSE);

	//站点标识
	if (m_OptionParameter.GameServiceOption.wNodeID==0) SetDlgItemText(IDC_STATION_ID,TEXT(""));
	else SetDlgItemInt(IDC_STATION_ID,m_OptionParameter.GameServiceOption.wNodeID,FALSE);

	//桌子数目
	if (m_OptionParameter.GameServiceOption.wTableCount==0) SetDlgItemText(IDC_TABLE_COUNT,TEXT(""));
	else SetDlgItemInt(IDC_TABLE_COUNT,m_OptionParameter.GameServiceOption.wTableCount,FALSE);

	//单元积分
	if (m_OptionParameter.GameServiceOption.lCellScore==0) SetDlgItemText(IDC_CELL_SCORE,TEXT(""));
	else SetDlgItemInt(IDC_CELL_SCORE,m_OptionParameter.GameServiceOption.lCellScore,FALSE);

	//最少积分
	if (m_OptionParameter.GameServiceOption.lLessScore==0) SetDlgItemText(IDC_LESS_SCORE,TEXT(""));
	else SetDlgItemInt(IDC_LESS_SCORE,m_OptionParameter.GameServiceOption.lLessScore,FALSE);

	//最多积分
	if (m_OptionParameter.GameServiceOption.lMaxScore==0) SetDlgItemText(IDC_MAX_SCORE,TEXT(""));
	else SetDlgItemInt(IDC_MAX_SCORE,m_OptionParameter.GameServiceOption.lMaxScore,FALSE);

	//输赢限制
	if (m_OptionParameter.GameServiceOption.lRestrictScore==0) SetDlgItemText(IDC_RESTRICT_SCORE,TEXT(""));
	else SetDlgItemInt(IDC_RESTRICT_SCORE,m_OptionParameter.GameServiceOption.lRestrictScore,FALSE);

	//游戏税收
	SetDlgItemInt(IDC_REVENUE,m_OptionParameter.GameServiceOption.wRevenue);

	//隐藏信息
	BYTE cbHideUserInfo=m_OptionParameter.GameServiceOption.cbHideUserInfo;
	((CButton *)GetDlgItem(IDC_HIDE_USER_INFO))->SetCheck((cbHideUserInfo==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//限制旁观
	BYTE cbUnLookOnTag=m_OptionParameter.GameServiceOption.cbUnLookOnTag;
	((CButton *)GetDlgItem(IDC_LIMIT_LOOK_ON))->SetCheck((cbUnLookOnTag==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//限制同IP
	BYTE cbUnSameIPPlay=m_OptionParameter.GameServiceOption.cbUnSameIPPlay;
	((CButton *)GetDlgItem(IDC_LIMIT_SAME_IP))->SetCheck((cbUnSameIPPlay==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//控制开始
	BYTE cbControlStart=m_OptionParameter.GameServiceOption.cbControlStart;
	((CButton *)GetDlgItem(IDC_CONTROL_START))->SetCheck((cbControlStart==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//比赛局数
	LONG lMatchDraw=m_OptionParameter.GameServiceOption.lMatchDraw;
	if (lMatchDraw>0) SetDlgItemInt(IDC_MATCH_DRAW,lMatchDraw,FALSE);

	//最大连接
	if (m_OptionParameter.GameServiceOption.wMaxConnect==0) SetDlgItemText(IDC_MAX_CONNECT,TEXT(""));
	else SetDlgItemInt(IDC_MAX_CONNECT,m_OptionParameter.GameServiceOption.wMaxConnect,FALSE);

	//房间名字
	if (m_OptionParameter.GameServiceOption.szGameRoomName[0]==0) 
	{
		TCHAR szServerName[SERVER_LEN]=TEXT("");
		_snprintf(szServerName,sizeof(szServerName),TEXT("%s房间"),m_GameServiceAttrib.szKindName);
		SetDlgItemText(IDC_SERVER_NAME,szServerName);
	}
	else SetDlgItemText(IDC_SERVER_NAME,m_OptionParameter.GameServiceOption.szGameRoomName);

	//视频地址
	((CIPAddressCtrl *)GetDlgItem(IDC_VIDEO_ADDR))->SetAddress(m_OptionParameter.GameServiceOption.dwVideoServer);

	//监听端口
	if (m_OptionParameter.GameServiceOption.wServerPort==0) SetDlgItemText(IDC_PORT,TEXT(""));
	else SetDlgItemInt(IDC_PORT,m_OptionParameter.GameServiceOption.wServerPort,FALSE);

	//服务地址
	CComboBox * pServerAddr=(CComboBox *)GetDlgItem(IDC_SERVER_ADDR);
	int nItemCount=pServerAddr->GetCount();
	for (int i=0;i<nItemCount;i++)
	{
		DWORD_PTR dwItemAddr=pServerAddr->GetItemData(i);
		if (m_OptionParameter.GameServiceOption.dwServerAddr==dwItemAddr) 
		{
			pServerAddr->SetCurSel(i);
			break;
		}
	}
	if (nItemCount==i)
	{
		TCHAR szServerAddr[256]=TEXT("");
		BYTE * cbServerAddr=(BYTE *)&m_OptionParameter.GameServiceOption.dwServerAddr;
		_snprintf(szServerAddr,CountArray(szServerAddr),TEXT("%ld.%ld.%ld.%ld"),cbServerAddr[0],cbServerAddr[1],cbServerAddr[2],cbServerAddr[3]);
		pServerAddr->SetWindowText(szServerAddr);
	}

	//数据库名字
	if (m_OptionParameter.szDataBaseName[0]!=0) SetDlgItemText(IDC_DATABASE_NAME,m_OptionParameter.szDataBaseName);
	else SetDlgItemText(IDC_DATABASE_NAME,m_GameServiceAttrib.szDataBaseName);

	//数据库地址
	DWORD dwDataBaseAddr=ntohl(inet_addr(m_OptionParameter.szDataBaseAddr));
	((CIPAddressCtrl *)GetDlgItem(IDC_DATABASE_ADDR))->SetAddress(dwDataBaseAddr);

	//组件属性
	SetDlgItemInt(IDC_KIND_ID,m_GameServiceAttrib.wKindID,FALSE);
	SetDlgItemText(IDC_KIND_NAME,m_GameServiceAttrib.szKindName);
	SetDlgItemInt(IDC_CHAIR_COUNT,m_GameServiceAttrib.wChairCount,FALSE);
	SetDlgItemText(IDC_DEF_DATABASE_NAME,m_GameServiceAttrib.szDataBaseName);
	SetDlgItemText(IDC_CLIENT_MODULE,m_GameServiceAttrib.szClientModuleName);
	SetDlgItemText(IDC_SERVICE_MODULE,m_GameServiceAttrib.szServerModuleName);
	SetDlgItemText(IDC_MODULE_DESCRIBE,m_GameServiceAttrib.szDescription);

	return;
}

//////////////////////////////////////////////////////////////////////////
