#include "Stdafx.h"
#include "GamePlaza.h"
#include "GlobalUnits.h"
#include "RoomViewItem.h"
#include "PlazaViewItem.h"
#include "Zip.h"


//////////////////////////////////////////////////////////////////////////

#define		IDC_WEB_BBS			100
#define		IDC_WEB_BEFORE		101
#define		IDC_WEB_BRUSH		102
#define		IDC_WEB_HOME		103
#define		IDC_WEB_NEXT		104
#define		IDC_WEB_STOP		105

BEGIN_MESSAGE_MAP(CPlazaViewItem, CDialog)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_WEB_BBS, OnBnClickedBBS)
	ON_BN_CLICKED(IDC_WEB_HOME, OnBnClickedHOME)
	ON_BN_CLICKED(IDC_WEB_NEXT, OnBnClickedNEXT)
	ON_BN_CLICKED(IDC_WEB_STOP, OnBnClickedSTOP)
	ON_BN_CLICKED(IDC_WEB_BRUSH, OnBnClickedMBRUSH)
	ON_BN_CLICKED(IDC_WEB_BEFORE, OnBnClickedBEFORE)
	ON_COMMAND(IDM_LOGON_PLAZA,		OnCommandLogon)
	ON_COMMAND(IDM_CONNECT_SERVER,	OnCommandConnect)
	ON_COMMAND(IDM_CANCEL_CONNECT,	OnCommandCancelConnect)
	ON_MESSAGE(WM_DOWN_LOAD_FACE, OnDownloadFace)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////


CPlazaViewItem::CPlazaViewItem() : CDialog(IDD_GAME_PLAZA)
{
	//设置变量
	m_bInitDialog	= false;
	m_bLogonPlaza	= false;
	m_bDownloadConnect=false;
	m_bStartDownloadFace=false;
	m_pHtmlBrower	= NULL;

	return ;
}

CPlazaViewItem::~CPlazaViewItem()
{
}

//控件绑定
void CPlazaViewItem::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
}

BOOL CPlazaViewItem::PreTranslateMessage(MSG* pMsg)
{
	//提示消息
	if (m_ToolTipCtrl.m_hWnd!=NULL) 
	{
		m_ToolTipCtrl.RelayEvent(pMsg);
	}

	return __super::PreTranslateMessage(pMsg);
}

//初始化函数
BOOL CPlazaViewItem::OnInitDialog()
{
	__super::OnInitDialog();

	//创建控件
	m_pHtmlBrower=new CHtmlBrower;
	m_pHtmlBrower->Create(NULL,NULL,WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),this,20,NULL);

	//浏览野蛮
	m_pHtmlBrower->Navigate(szStationPage);

	//创建按钮
	m_WEB_BEFORE.Create(NULL,WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,IDC_WEB_BEFORE);
	m_WEB_BRUSH.Create(NULL,WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,IDC_WEB_BRUSH);
	m_WEB_HOME.Create(NULL,WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,IDC_WEB_HOME);
	m_WEB_NEXT.Create(NULL,WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,IDC_WEB_NEXT);
	m_WEB_STOP.Create(NULL,WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,IDC_WEB_STOP);

	//加载资源
	UpdateSkinResource();

	//提示控件
	m_ToolTipCtrl.Create(this);
	m_ToolTipCtrl.Activate(TRUE);
	m_ToolTipCtrl.AddTool(&m_WEB_BEFORE,TEXT("后退"));
	m_ToolTipCtrl.AddTool(&m_WEB_NEXT,TEXT("前进"));
	m_ToolTipCtrl.AddTool(&m_WEB_STOP,TEXT("停止"));
	m_ToolTipCtrl.AddTool(&m_WEB_HOME,TEXT("主页"));
	m_ToolTipCtrl.AddTool(&m_WEB_BRUSH,TEXT("刷新"));

	//登录广场
	SendLogonMessage();

	//设置变量
	m_bInitDialog=true;

	return TRUE;
}

//接口查询
void * __cdecl CPlazaViewItem::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IViewItem,Guid,dwQueryVer);
	QUERYINTERFACE(ITCPSocketSink,Guid,dwQueryVer);
	QUERYINTERFACE(IPlazaViewItem,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IViewItem,Guid,dwQueryVer);
	return NULL;
}

//网络连接消息
bool __cdecl CPlazaViewItem::OnEventTCPSocketLink(WORD wSocketID, INT nErrorCode)
{
	//错误处理
	if (nErrorCode!=0)
	{
		g_GlobalAttemper.DestroyStatusWnd(this);
		ShowMessageBox(TEXT("登录服务器连接失败，请稍后再试或留意网站公告！"),MB_ICONINFORMATION);
		SendLogonMessage();
		return true;
	}

	//发送登录包
	if (m_bLogonPlaza==false)
	{
		m_DlgLogon.SendLogonPacket(m_ClientSocket.GetInterface());
		g_GlobalAttemper.ShowStatusMessage(TEXT("正在验证用户登录信息..."),this);
	}

	//自定义头像
	CGameFrame *pGameFrame = (CGameFrame *)AfxGetMainWnd() ;
	CDlgCustomFace &DlgCustomFace = pGameFrame->m_DlgCustomFace;
	if ( DlgCustomFace.m_hWnd != NULL && DlgCustomFace.IsWindowVisible() )
	{
		DlgCustomFace.SendData();
	}

	//下载判断
	if ( m_bDownloadConnect )
	{
		//设置变量
		m_bDownloadConnect = false;

		//投递请求
		tagDownloadInfo &DownloadInfo = m_DownloadInfoArrary[0];

		CMD_GP_DownloadFace DownloadFace;
		DownloadFace.dwUserID = DownloadInfo.dwUserID;
		m_ClientSocket->SendData(MDM_GP_USER, SUB_GP_USER_DOWNLOAD_FACE, &DownloadFace, sizeof(DownloadFace));
	}

	return true;
}

//网络读取消息
bool __cdecl CPlazaViewItem::OnEventTCPSocketRead(WORD wSocketID, CMD_Command Command, VOID * pData, WORD wDataSize)
{
	switch (Command.wMainCmdID)
	{
	case MDM_GP_LOGON:				//登录消息
		{
			return OnSocketMainLogon(Command,pData,wDataSize);
		}
	case MDM_GP_SERVER_LIST:		//列表消息
		{
			return OnSocketMainServerList(Command,pData,wDataSize);
		}
	case MDM_GP_SYSTEM:				//系统消息
		{
			return OnSocketMainSystem(Command,pData,wDataSize);
		}
	case MDM_GP_USER:				//用户消息
		{
			return OnSocketMainUser(Command,pData,wDataSize);
		}
	}

	return true;
}

//网络关闭消息
bool __cdecl CPlazaViewItem::OnEventTCPSocketShut(WORD wSocketID, BYTE cbShutReason)
{
	if (m_bLogonPlaza==false)
	{
		if (cbShutReason!=SHUT_REASON_NORMAL)
		{
			g_GlobalAttemper.DestroyStatusWnd(this);
			ShowMessageBox(TEXT("登录服务器连接失败，请稍后再试或留意网站公告！"),MB_ICONINFORMATION);
			SendLogonMessage();
		}
	}

	//自定义头像
	CGameFrame *pGameFrame = (CGameFrame *)AfxGetMainWnd() ;
	CDlgCustomFace &DlgCustomFace = pGameFrame->m_DlgCustomFace;
	if ( DlgCustomFace.m_hWnd != NULL ) DlgCustomFace.SetStatus(enOperateStatus_NULL);

	//释放内存
	if ( m_CustomFace.pFaceData != NULL ) m_CustomFace.Clear();

	return true;
}

//登录消息
bool CPlazaViewItem::OnSocketMainLogon(CMD_Command Command, void * pData, WORD wDataSize)
{
	switch (Command.wSubCmdID)
	{
	case SUB_GP_LOGON_SUCCESS:		//登录成功
		{
			//效验参数
			ASSERT(wDataSize>=sizeof(CMD_GP_LogonSuccess));
			if (wDataSize<sizeof(CMD_GP_LogonSuccess)) return false;

			//保存信息
			tagGlobalUserData & UserData=g_GlobalUnits.GetGolbalUserData();
			CMD_GP_LogonSuccess * pLogonSuccess=(CMD_GP_LogonSuccess *)pData;
			UserData.wFaceID=pLogonSuccess->wFaceID;
			UserData.cbGender=pLogonSuccess->cbGender;
			UserData.cbMember=pLogonSuccess->cbMember;
			UserData.dwUserID=pLogonSuccess->dwUserID;
			UserData.dwGameID=pLogonSuccess->dwGameID;
			UserData.dwExperience=pLogonSuccess->dwExperience;
			UserData.dwCustomFaceVer=pLogonSuccess->dwCustomFaceVer;

			//扩展信息
			void * pDataBuffer=NULL;
			tagDataDescribe DataDescribe;
			CRecvPacketHelper RecvPacket(pLogonSuccess+1,wDataSize-sizeof(CMD_GP_LogonSuccess));
			while (true)
			{
				pDataBuffer=RecvPacket.GetData(DataDescribe);
				if (DataDescribe.wDataDescribe==DTP_NULL) break;
				switch (DataDescribe.wDataDescribe)
				{
				case DTP_USER_ACCOUNTS:		//用户帐户
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize>0);
						ASSERT(DataDescribe.wDataSize<=sizeof(UserData.szAccounts));
						if (DataDescribe.wDataSize<=sizeof(UserData.szAccounts))
						{
							CopyMemory(UserData.szAccounts,pDataBuffer,DataDescribe.wDataSize);
							UserData.szAccounts[CountArray(UserData.szAccounts)-1]=0;
						}
						break;
					}
				case DTP_USER_PASS:			//用户密码
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize>0);
						ASSERT(DataDescribe.wDataSize<=sizeof(UserData.szPassWord));
						if (DataDescribe.wDataSize<=sizeof(UserData.szPassWord))
						{
							CopyMemory(UserData.szPassWord,pDataBuffer,DataDescribe.wDataSize);
							UserData.szPassWord[CountArray(UserData.szPassWord)-1]=0;
						}
						break;
					}
				case DTP_UNDER_WRITE:		//个性签名
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize>0);
						ASSERT(DataDescribe.wDataSize<=sizeof(UserData.szUnderWrite));
						if (DataDescribe.wDataSize<=sizeof(UserData.szUnderWrite))
						{
							CopyMemory(UserData.szUnderWrite,pDataBuffer,DataDescribe.wDataSize);
							UserData.szUnderWrite[CountArray(UserData.szUnderWrite)-1]=0;
						}
						break;
					}
				case DTP_USER_GROUP_NAME:	//社团名字
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize>0);
						ASSERT(DataDescribe.wDataSize<=sizeof(UserData.szGroupName));
						if (DataDescribe.wDataSize<=sizeof(UserData.szGroupName))
						{
							CopyMemory(UserData.szGroupName,pDataBuffer,DataDescribe.wDataSize);
							UserData.szGroupName[CountArray(UserData.szGroupName)-1]=0;
						}
						break;
					}
				case DTP_STATION_PAGE:		//游戏主站
					{
						ASSERT(pDataBuffer!=NULL);
						if (pDataBuffer!=NULL) 
						{
							g_GlobalUnits.SetStationPage((LPCTSTR)pDataBuffer);
							m_pHtmlBrower->Navigate(g_GlobalUnits.GetStationPage());
						}
						break;
					}
				default: { ASSERT(FALSE); }
				}
			}

			//设置提示
			g_GlobalAttemper.ShowStatusMessage(TEXT("正在读取服务器列表信息..."),this);

			return true;
		}
	case SUB_GP_LOGON_ERROR:		//登录失败
		{
			//效验参数
			CMD_GP_LogonError *pLogonError = (CMD_GP_LogonError *)pData;
			ASSERT(wDataSize>=(sizeof(CMD_GP_LogonError)-sizeof(pLogonError->szErrorDescribe)));
			if (wDataSize<(sizeof(CMD_GP_LogonError)-sizeof(pLogonError->szErrorDescribe))) return false;

			//关闭连接
			g_GlobalAttemper.DestroyStatusWnd(this);
			m_ClientSocket->CloseSocket();

			//显示消息
			WORD wDescribeSize=wDataSize-(sizeof(CMD_GP_LogonError)-sizeof(pLogonError->szErrorDescribe));
			if (wDescribeSize>0)
			{
				pLogonError->szErrorDescribe[wDescribeSize-1]=0;
				ShowMessageBox(pLogonError->szErrorDescribe,MB_ICONINFORMATION);
			}

			//发送登录
			SendLogonMessage();

			return true;
		}
	case SUB_GP_LOGON_FINISH:		//登录完成
		{
			//关闭提示
			g_GlobalAttemper.DestroyStatusWnd(this);

			//展开类型
			INT_PTR nIndex=0;
			CListType * pListType=NULL;
			do
			{
				pListType=g_GlobalUnits.m_ServerListManager.EnumTypeItem(nIndex++);
				if (pListType==NULL) break;
				g_GlobalUnits.m_ServerListManager.ExpandListItem(pListType);
			} while (true);

			//展开列表
			nIndex=0;
			CListInside * pListInside=NULL;
			do
			{
				pListInside=g_GlobalUnits.m_ServerListManager.EnumInsideItem(nIndex++);
				if (pListInside==NULL) break;
				g_GlobalUnits.m_ServerListManager.ExpandListItem(pListInside);
			} while (true);

			//记录信息
			m_bLogonPlaza=true;
			m_DlgLogon.OnLogonSuccess();
			m_pHtmlBrower->EnableBrowser(true);

			//记录信息
			g_GlobalUnits.WriteUserCookie();

			//显示头像
			((CGameFrame*)AfxGetMainWnd())->m_UserInfoView.ShowUserInfo(true);

			//自定义判断
			tagGlobalUserData &GlobalUserData = g_GlobalUnits.GetGolbalUserData();
			if ( GlobalUserData.dwCustomFaceVer!=0)
			{
				//头像名称
				CString strDirName = CString(g_GlobalUnits.GetWorkDirectory()) + TEXT("\\CustomFace");
				CString strFileName;
				strFileName.Format(TEXT("\\%ld_%d.bmp"), GlobalUserData.dwUserID, GlobalUserData.dwCustomFaceVer);

				//读取文件
				CImage FaceImage;
				HRESULT hResult = FaceImage.Load(strDirName + strFileName);
				if (SUCCEEDED(hResult))
				{
					//关闭连接
					m_ClientSocket->CloseSocket();

					FaceImage.Destroy();
				}
				//下载头像
				else
				{
					PostMessage(WM_DOWN_LOAD_FACE, LPARAM(GlobalUserData.dwCustomFaceVer), WPARAM(GlobalUserData.dwUserID));
				}
			}
			else
			{
				//关闭连接
				m_ClientSocket->CloseSocket();
			}

			return true;
		}
	}

	return true;
}

//游戏列表消息
bool CPlazaViewItem::OnSocketMainServerList(CMD_Command Command, void * pData, WORD wDataSize)
{
	ASSERT(Command.wMainCmdID==MDM_GP_SERVER_LIST);
	
	switch (Command.wSubCmdID)
	{
	case SUB_GP_LIST_TYPE:			//类型信息
		{
			//效验参数
			ASSERT(wDataSize%sizeof(tagGameType)==0);
			if (wDataSize%sizeof(tagGameType)!=0) return false;

			//处理消息
			tagGameType * pGameType=(tagGameType *)pData;
			WORD wItemCount=wDataSize/sizeof(tagGameType);
			g_GlobalUnits.m_ServerListManager.InsertTypeItem(pGameType,wItemCount);

			return true;
		}
	case SUB_GP_LIST_KIND:			//种类消息
		{
			//效验参数
			ASSERT(wDataSize%sizeof(tagGameKind)==0);
			if (wDataSize%sizeof(tagGameKind)!=0) return false;

			//处理消息
			tagGameKind * pGameKind=(tagGameKind *)pData;
			WORD wItemCount=wDataSize/sizeof(tagGameKind);
			g_GlobalUnits.m_ServerListManager.InsertKindItem(pGameKind,wItemCount);

			return true;
		}
	case SUB_GP_LIST_STATION:		//站点消息
		{
			//效验参数
			ASSERT(wDataSize%sizeof(tagGameStation)==0);
			if (wDataSize%sizeof(tagGameStation)!=0) return false;

			//处理消息
			tagGameStation * pGameStation=(tagGameStation *)pData;
			WORD wItemCount=wDataSize/sizeof(tagGameStation);
			g_GlobalUnits.m_ServerListManager.InsertStationItem(pGameStation,wItemCount);

			return true;
		}
	case SUB_GP_LIST_SERVER:		//服务器房间
		{
			//效验参数
			ASSERT(wDataSize%sizeof(tagGameServer)==0);
			if (wDataSize%sizeof(tagGameServer)!=0) return false;

			//处理消息
			tagGameServer * pGameServer=(tagGameServer *)pData;
			WORD wItemCount=wDataSize/sizeof(tagGameServer);
			g_GlobalUnits.m_ServerListManager.InsertServerItem(pGameServer,wItemCount);

			return true;
		}
	case SUB_GP_LIST_FINISH:		//列表发送完成
		{
			//更新人数
			INT_PTR nIndex=0;
			DWORD dwAllOnLineCount=0L;
			CListKind * pListKind=NULL;
			do
			{
				pListKind=g_GlobalUnits.m_ServerListManager.EnumKindItem(nIndex++);
				if (pListKind==NULL) break;
				dwAllOnLineCount+=pListKind->GetItemInfo()->dwOnLineCount;
			} while (true);
			g_GlobalUnits.m_ServerListManager.UpdateGameOnLineCount(dwAllOnLineCount);

			return true;
		}
	case SUB_GP_LIST_CONFIG:		//列表配置
		{
			//效验参数
			ASSERT(wDataSize%sizeof(CMD_GP_ListConfig)==0);
			if (wDataSize%sizeof(CMD_GP_ListConfig)!=0) return false;

			//处理消息
			CMD_GP_ListConfig * pListConfig=(CMD_GP_ListConfig *)pData;
			((CGameFrame*)AfxGetMainWnd())->m_ServerItemView.ShowOnLineCount(pListConfig->bShowOnLineCount?true:false);

			return true;
		}
	}

	return true;
}

//系统消息
bool CPlazaViewItem::OnSocketMainSystem(CMD_Command Command, void * pData, WORD wDataSize)
{
	ASSERT(Command.wMainCmdID==MDM_GP_SYSTEM);
	switch (Command.wSubCmdID)
	{
	case SUB_GP_VERSION:			//版本信息
		{
			//效验参数
			ASSERT(wDataSize>=sizeof(CMD_GP_Version));
			if (wDataSize<sizeof(CMD_GP_Version)) return false;

			//消息处理
			CMD_GP_Version * pVersion=(CMD_GP_Version *)pData;
			if (pVersion->bAllowConnect)
			{
				TCHAR szMessage[]=TEXT("游戏大厅版本已经升级，现在的版本还可以继续使用，现在是否马上下载新版本？");
				
				int iResult=ShowInformationEx(szMessage,0,MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON1,TEXT("游戏广场"));
				if (iResult==IDYES)
				{
					g_GlobalAttemper.DestroyStatusWnd(this);
					m_ClientSocket->CloseSocket();
					tagGlobalUserData & GlobalUserData=g_GlobalUnits.GetGolbalUserData();
					memset(&GlobalUserData,0,sizeof(GlobalUserData));
					g_GlobalAttemper.DownLoadClient(TEXT("游戏广场"),0,true);
				}
			}
			else
			{
				g_GlobalAttemper.DestroyStatusWnd(this);
				m_ClientSocket->CloseSocket();
				TCHAR szMessage[]=TEXT("游戏大厅版本已经升级，现在的版本不可以继续使用，现在是否马上下载新版本？");
				
				int iResult=ShowInformationEx(szMessage,0,MB_ICONSTOP|MB_YESNO|MB_DEFBUTTON1,TEXT("游戏广场"));
				if(iResult != IDYES)
				{
					tagGlobalUserData & GlobalUserData=g_GlobalUnits.GetGolbalUserData();
					memset(&GlobalUserData,0,sizeof(GlobalUserData));
					AfxGetMainWnd()->PostMessage(WM_CLOSE);
				}
				else 
				{
					g_GlobalAttemper.DownLoadClient(TEXT("游戏广场"),0,true);
				}
			}

			return true;
		}
	}
	return true;
}

//用户消息
bool CPlazaViewItem::OnSocketMainUser(CMD_Command Command, void * pBuffer, WORD wDataSize)
{
	ASSERT(Command.wMainCmdID == MDM_GP_USER);
	switch(Command.wSubCmdID)
	{
	case SUB_GP_USER_DOWNLOAD_FACE:			//下载头像
		{
			//类型转换
			CMD_GP_DownloadFaceSuccess *pDownloadFaceSuccess = (CMD_GP_DownloadFaceSuccess*)pBuffer;

			//参数验证
			WORD wSendSize = WORD(pDownloadFaceSuccess->dwCurrentSize + sizeof(CMD_GP_DownloadFaceSuccess) - sizeof(pDownloadFaceSuccess->bFaceData));
			ASSERT(wDataSize == wSendSize);
			if ( wDataSize != wSendSize ) return false;

			//第一次判断
			if ( m_CustomFace.pFaceData == NULL )
			{
				if ( m_CustomFace.pFaceData != NULL ) delete[] m_CustomFace.pFaceData;
				m_CustomFace.pFaceData = new BYTE[pDownloadFaceSuccess->dwToltalSize];
				if ( m_CustomFace.pFaceData == NULL ) return true;
				m_CustomFace.dwFaceSize = pDownloadFaceSuccess->dwToltalSize;
				m_CustomFace.dwUserID = pDownloadFaceSuccess->dwUserID;
			}

			//拷贝数据
			CopyMemory(m_CustomFace.pFaceData+m_CustomFace.dwCurrentSize, pDownloadFaceSuccess->bFaceData, pDownloadFaceSuccess->dwCurrentSize);
			m_CustomFace.dwCurrentSize += pDownloadFaceSuccess->dwCurrentSize;

			//下载完毕
			if ( m_CustomFace.dwFaceSize == m_CustomFace.dwCurrentSize )
			{
				tagDownloadInfo &DownloadInfo = m_DownloadInfoArrary[0];

				//创建目录
				CString strDirName = CString(g_GlobalUnits.GetWorkDirectory()) + TEXT("\\CustomFace");
				CreateDirectory(strDirName, NULL) ;

				//写入文件
				CFile fileFace;
				CString strZipFileName, strBmpFileName;
				strZipFileName.Format(TEXT("\\%ld_%d.zip"), DownloadInfo.dwUserID, DownloadInfo.bFaceVer);
				strBmpFileName.Format(TEXT("\\%ld_%d.bmp"), DownloadInfo.dwUserID, DownloadInfo.bFaceVer);
				if ( fileFace.Open(strDirName + strZipFileName, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary) )
				{
					fileFace.Write(m_CustomFace.pFaceData, m_CustomFace.dwFaceSize);
					fileFace.Close();

					//解压文件
					CUnzip oUnzip( strDirName + strZipFileName );
					oUnzip.SwapSize( strDirName + strBmpFileName );

					//删除文件
					CFile::Remove(strDirName + strZipFileName);
				}

				//自己判断
				tagGlobalUserData &GlobalUserData = g_GlobalUnits.GetGolbalUserData();
				if ( GlobalUserData.dwUserID ==  DownloadInfo.dwUserID )
				{
					//更新界面
					((CGameFrame*)AfxGetMainWnd())->m_UserInfoView.UpdateUserInfo();
				}

				//重置变量
				m_CustomFace.Clear();

				//删除元素
				m_DownloadInfoArrary.RemoveAt(0);

				//继续判断
				if ( 0 < m_DownloadInfoArrary.GetCount() )
				{
					//状态判断
					if ( m_ClientSocket->GetSocketStatus() != SOCKET_STATUS_CONNECT )
					{
						//链接网络
						if ( ! ConnectServer() ) return true;

						//设置标识
						m_bDownloadConnect = true;
					}

					//投递请求
					tagDownloadInfo &DownloadInfo = m_DownloadInfoArrary[0];

					CMD_GP_DownloadFace DownloadFace;
					DownloadFace.dwUserID = DownloadInfo.dwUserID;
					m_ClientSocket->SendData(MDM_GP_USER, SUB_GP_USER_DOWNLOAD_FACE, &DownloadFace, sizeof(DownloadFace));
				}
				else
				{
					//关闭链接
					m_ClientSocket->CloseSocket();

					//设置变量
					m_bStartDownloadFace=false;
				}
			}
			return true;
		}
	case SUB_GP_UPLOAD_FACE_RESULT:		//上传结果
		{
			ASSERT(sizeof(CMD_GP_UploadFaceResult) == wDataSize);
			if ( sizeof(CMD_GP_UploadFaceResult) != wDataSize) return true;

			//显示消息
			CMD_GP_UploadFaceResult *pUploadFaceResult = (CMD_GP_UploadFaceResult*)pBuffer;
			ShowMessageBox(pUploadFaceResult->szDescribeMsg, MB_ICONINFORMATION);

			//成功判断
			if ( pUploadFaceResult->bOperateSuccess )
			{
				//头像目录
				CString strDirName = CString(g_GlobalUnits.GetWorkDirectory()) + TEXT("\\CustomFace");
				CString strOldFileName;
				strOldFileName = TEXT("\\MyFace.bmp");

				//命名文件
				tagGlobalUserData &GlobalUserData = g_GlobalUnits.GetGolbalUserData();
				CString strNemFileName;
				strNemFileName.Format(TEXT("\\%ld_%d.bmp"), GlobalUserData.dwUserID, pUploadFaceResult->dwFaceVer);

				//删除文件
				try
				{
				CFile::Remove(strDirName + strNemFileName);
				}
				catch(...){}

				//删除文件
				CString strOldFile;
				strOldFile.Format(TEXT("\\%ld_%ld.bmp"), GlobalUserData.dwUserID, GlobalUserData.dwCustomFaceVer);
				try
				{
					CFile::Remove(strDirName + strOldFile);
				}
				catch(...){}

				try
				{
					CFile::Rename(strDirName + strOldFileName, strDirName + strNemFileName);

					//设置版本号
					GlobalUserData.dwCustomFaceVer = pUploadFaceResult->dwFaceVer;
					ASSERT(GlobalUserData.dwCustomFaceVer!=0);

					//投递消息
					for ( INT nIndex = 0; nIndex < MAX_SERVER; nIndex++ ) 
					{
						CRoomViewItem *pRoomViewItem = ((CGameFrame*)AfxGetMainWnd())->m_pRoomViewItem[nIndex];
						if ( pRoomViewItem == NULL ) continue;
						pRoomViewItem->SendMessage(WM_UPDATE_FACE, GlobalUserData.dwCustomFaceVer, GlobalUserData.dwUserID);
					}
				}
				catch(...)
				{
					ShowMessageBox(TEXT("头像文件命名失败，需要重新登录才可以显示新头像！"), MB_ICONINFORMATION);
				}

				//更新界面
				((CGameFrame*)AfxGetMainWnd())->m_UserInfoView.UpdateUserInfo();

				//设置界面
				CGameFrame *pGameFrame = (CGameFrame *)AfxGetMainWnd() ;
				CDlgCustomFace &DlgCustomFace = pGameFrame->m_DlgCustomFace;
				if ( DlgCustomFace.m_hWnd != NULL && DlgCustomFace.IsWindowVisible() )
				{
					DlgCustomFace.UpdateControls();
					DlgCustomFace.SetFocus();
				}
			}

			//关闭链接
			m_ClientSocket->CloseSocket();

			return true;
		}
	case SUB_GP_DELETE_FACE_RESULT:		//删除结果
		{
			ASSERT(sizeof(CMD_GP_DeleteFaceResult) == wDataSize);
			if ( sizeof(CMD_GP_DeleteFaceResult) != wDataSize) return true;

			//显示消息
			CMD_GP_DeleteFaceResult *pDeleteFaceResult = (CMD_GP_DeleteFaceResult*)pBuffer;
			ShowMessageBox(pDeleteFaceResult->szDescribeMsg, MB_ICONINFORMATION);

			//成功判断
			if ( pDeleteFaceResult->bOperateSuccess )
			{
				//获取信息
				tagGlobalUserData &GlobalUserData = g_GlobalUnits.GetGolbalUserData();

				//文件目录
				CString strDirName = CString(g_GlobalUnits.GetWorkDirectory()) + TEXT("\\CustomFace");

				//文件名字
				CString strFileName;
				strFileName.Format(TEXT("\\%ld_%d.bmp"), GlobalUserData.dwUserID, pDeleteFaceResult->dwFaceVer);

				//删除文件
				try
				{
				CFile::Remove(strDirName + strFileName);
				}
				catch(...){}
				
				//更新界面
				((CGameFrame*)AfxGetMainWnd())->m_UserInfoView.UpdateUserInfo();

				//设置界面
				CGameFrame *pGameFrame = (CGameFrame *)AfxGetMainWnd() ;
				CDlgCustomFace &DlgCustomFace = pGameFrame->m_DlgCustomFace;
				if ( DlgCustomFace.m_hWnd != NULL && DlgCustomFace.IsWindowVisible() )
				{
					DlgCustomFace.UpdateControls();
					DlgCustomFace.SetFocus();
				}
				
				//投递消息
				for ( INT nIndex = 0; nIndex < MAX_SERVER; nIndex++ ) 
				{
					CRoomViewItem *pRoomViewItem = ((CGameFrame*)AfxGetMainWnd())->m_pRoomViewItem[nIndex];
					if ( pRoomViewItem == NULL ) continue;
					pRoomViewItem->SendMessage(WM_UPDATE_FACE, GlobalUserData.dwCustomFaceVer, GlobalUserData.dwUserID);
				}
			}
			
			//关闭链接
			m_ClientSocket->CloseSocket();

			return true;
		}
	case SUB_GP_MODIFY_INDIVIDUAL_RESULT:	//修改结果
		{
			return true;
		}
	default:
		{
			ASSERT(FALSE);
			return false;
		}
	}
	return true;
}

//登录消息
bool __cdecl CPlazaViewItem::SendLogonMessage()
{
	m_bLogonPlaza=false;
	((CGameFrame*)AfxGetMainWnd())->m_UserInfoView.ShowUserInfo(false);
	PostMessage(WM_COMMAND,IDM_LOGON_PLAZA,0);
	return true;
}

//连接消息
bool __cdecl CPlazaViewItem::SendConnectMessage()
{
	PostMessage(WM_COMMAND,IDM_CONNECT_SERVER,0);
	return true;
}

//显示网页
void CPlazaViewItem::WebBrowse(LPCTSTR lpszUrl, bool bForceBrowse)
{
	if (m_pHtmlBrower->GetSafeHwnd())
	{
		m_pHtmlBrower->Navigate(lpszUrl);
		if (bForceBrowse==true) 
		{
			m_pHtmlBrower->EnableBrowser(true);
		}
	}
	return;
}

//显示消息
int CPlazaViewItem::ShowMessageBox(LPCTSTR pszMessage, UINT nType)
{				
	int nResult=ShowInformationEx(pszMessage,0,nType,TEXT("游戏大厅"));

	return nResult;
}

//网络链接
bool CPlazaViewItem::ConnectServer()
{
	//合法判断
	if ( m_ClientSocket.GetInterface() == NULL ) 
	{
		return false;
	}

	//地址解释
	CRegKey RegServer;
	LPCTSTR pszServerIP=m_DlgLogon.GetLogonServer();
	TCHAR szRegServer[256]=TEXT(""),szServerAddr[64]=TEXT("");
	_snprintf(szRegServer,sizeof(szRegServer),TEXT("%s\\%s"),REG_LOGON_SERVER,pszServerIP);

	if (RegServer.Open(HKEY_CURRENT_USER,szRegServer,KEY_READ)==ERROR_SUCCESS)
	{
		TCHAR szReadData[1024]=TEXT("");
		DWORD dwReadData=0L,dwDataType=0L,dwDataSize=sizeof(szReadData);
		LONG lErrorCode=RegServer.QueryValue(TEXT("ServerAddr"),&dwDataType,szReadData,&dwDataSize);
		if (lErrorCode==ERROR_SUCCESS) 
		{
			CXOREncrypt::CrevasseData(szReadData,szServerAddr,sizeof(szServerAddr));
			pszServerIP=szServerAddr;
		}
	}

	//连接服务器
	try
	{
		//连接服务器
		m_ClientSocket->CloseSocket();
		if (m_ClientSocket->Connect(pszServerIP,PORT_LOGON_SERVER)!=ERROR_SUCCESS)
		{
			throw TEXT("服务器连接错误，可能是你的系统还没有成功连接上网络！");
		}
	}
	catch (LPCTSTR pszError)
	{
		UNREFERENCED_PARAMETER(pszError);
		return false;
	}

	return true;
}

//更新资源
bool CPlazaViewItem::UpdateSkinResource()
{
	//加载资源
	tagPlazaViewImage & PlazaViewImage=g_GlobalUnits.m_PlazaViewImage;
	HINSTANCE hInstance=g_GlobalUnits.m_PlatformResourceModule->GetResInstance();

	//桌子区域
	m_EncircleWeb.ImageML.SetLoadInfo(PlazaViewImage.uBrowseML,hInstance);
	m_EncircleWeb.ImageMR.SetLoadInfo(PlazaViewImage.uBrowseMR,hInstance);
	m_EncircleWeb.ImageTL.SetLoadInfo(PlazaViewImage.uBrowseTL,hInstance);
	m_EncircleWeb.ImageTM.SetLoadInfo(PlazaViewImage.uBrowseTM,hInstance);
	m_EncircleWeb.ImageTR.SetLoadInfo(PlazaViewImage.uBrowseTR,hInstance);
	m_EncircleWeb.ImageBL.SetLoadInfo(PlazaViewImage.uBrowseBL,hInstance);
	m_EncircleWeb.ImageBM.SetLoadInfo(PlazaViewImage.uBrowseBM,hInstance);
	m_EncircleWeb.ImageBR.SetLoadInfo(PlazaViewImage.uBrowseBR,hInstance);

	//获取信息
	CSkinAide::GetEncircleInfo(m_ImageInfoWeb,m_EncircleWeb);

	//设置按钮
	m_WEB_BEFORE.SetButtonImage(PlazaViewImage.uBtWebLast,hInstance,false);
	m_WEB_BRUSH.SetButtonImage(PlazaViewImage.uBtWebReload,hInstance,false);
	m_WEB_HOME.SetButtonImage(PlazaViewImage.uBtWebHome,hInstance,false);
	m_WEB_NEXT.SetButtonImage(PlazaViewImage.uBtWebNext,hInstance,false);
	m_WEB_STOP.SetButtonImage(PlazaViewImage.uBtWebStop,hInstance,false);

	return true;
}

//位置消息
void CPlazaViewItem::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	//状态判断
	if (m_bInitDialog==false) return;
	if ((cx==0)||(cy==0)) return;

	//变量定义
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS;

	//移动控件
	HDWP hDwp=BeginDeferWindowPos(32);
	DeferWindowPos(hDwp,m_pHtmlBrower->GetSafeHwnd(),NULL,m_ImageInfoWeb.nLBorder,m_ImageInfoWeb.nTBorder,
		cx-m_ImageInfoWeb.nLBorder-m_ImageInfoWeb.nRBorder,cy-m_ImageInfoWeb.nTBorder-m_ImageInfoWeb.nBBorder,uFlags);

	//导航按钮
	CRect rcButton;
	m_WEB_BEFORE.GetWindowRect(&rcButton);
	DeferWindowPos(hDwp,m_WEB_BEFORE,NULL,10,3,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_WEB_NEXT,NULL,10+rcButton.Width(),3,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_WEB_BRUSH,NULL,10+rcButton.Width()*2,3,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_WEB_STOP,NULL,10+rcButton.Width()*3,3,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_WEB_HOME,NULL,10+rcButton.Width()*4,3,0,0,uFlags|SWP_NOSIZE);

	EndDeferWindowPos(hDwp);

	return;
}

//显示消息
void CPlazaViewItem::OnShowWindow(BOOL bShow, UINT nStatus)
{
	__super::OnShowWindow(bShow, nStatus);

	//设置标题
	if (bShow) 
	{
		UpdateTitleText();
	}

	return;
}

//下载消息
LRESULT CPlazaViewItem::OnDownloadFace(WPARAM wParam, LPARAM lParam)
{
	DWORD dwUserID = (DWORD)lParam;
	ASSERT(dwUserID != 0);
	BYTE bFaceVer = (BYTE)wParam;

	//查找存在
	bool bFind = false;
	for ( INT_PTR nIndex = 0; nIndex < m_DownloadInfoArrary.GetCount(); nIndex++ )
	{
		tagDownloadInfo &DownloadInfo = m_DownloadInfoArrary[nIndex];
		if ( DownloadInfo.dwUserID == dwUserID )DownloadInfo.bReference++;
	}

	if ( !bFind )
	{
		tagDownloadInfo DownloadInfo;
		ZeroMemory(&DownloadInfo, sizeof(DownloadInfo));
		DownloadInfo.bFaceVer = bFaceVer;
		DownloadInfo.dwUserID = dwUserID;
		DownloadInfo.bReference = 1;

		//加入队列
		INT_PTR nCount = m_DownloadInfoArrary.GetCount();
		m_DownloadInfoArrary.SetAtGrow(nCount, DownloadInfo);
	}

	//启动下载
	if (m_bStartDownloadFace==false)
	{
		m_bStartDownloadFace=true;

		//状态判断
		if ( m_ClientSocket->GetSocketStatus() != SOCKET_STATUS_CONNECT )
		{
			//链接网络
			if ( ! ConnectServer() ) return FALSE;

			//设置标识
			m_bDownloadConnect = true;
		}

		//投递请求
		CMD_GP_DownloadFace DownloadFace;
		DownloadFace.dwUserID = dwUserID;
		m_ClientSocket->SendData(MDM_GP_USER, SUB_GP_USER_DOWNLOAD_FACE, &DownloadFace, sizeof(DownloadFace));
	}

	return TRUE;
}

//绘画消息
void CPlazaViewItem::OnPaint()
{
	CPaintDC dc(this);

	//绘画界面
	DrawWebFrame(&dc);

	return;
}

//绘画背景
BOOL CPlazaViewItem::OnEraseBkgnd(CDC * pDC)
{
	Invalidate(FALSE);
	UpdateWindow();
	return TRUE;
}

//登录广场
void CPlazaViewItem::OnCommandLogon()
{
	//创建登录框
	if (m_DlgLogon.m_hWnd==NULL) 
	{
		m_DlgLogon.Create(IDD_LOGON,this);
	}

	//显示登录框
	m_DlgLogon.ShowWindow(SW_SHOW);
	m_DlgLogon.SetActiveWindow();
	m_DlgLogon.SetForegroundWindow();

	return;
}

//连接服务器
void CPlazaViewItem::OnCommandConnect()
{
	//创建组件
	if (m_ClientSocket.GetInterface()==NULL)
	{
		try
		{
			IUnknownEx * pIUnknownEx=(IUnknownEx *)QueryInterface(IID_IUnknownEx,VER_IUnknownEx);
			if (m_ClientSocket.CreateInstance()==false) 
			{
				throw TEXT("网络组件创建失败");
			}
			if (m_ClientSocket->SetTCPSocketSink(pIUnknownEx)==false) 
			{
				throw TEXT("网络组件回调接口设置失败");
			}
		}
		catch (...)
		{
			ShowMessageBox(TEXT("网络组件创建失败，请重新下载游戏大厅！"),MB_ICONSTOP);
			PostMessage(WM_COMMAND,IDM_LOGON_PLAZA,0);
			return;
		}
	}

	//地址解释
	CRegKey RegServer;
	LPCTSTR pszServerIP=m_DlgLogon.GetLogonServer();
	TCHAR szRegServer[256]=TEXT(""),szServerAddr[64]=TEXT("");
	_snprintf(szRegServer,sizeof(szRegServer),TEXT("%s\\%s"),REG_LOGON_SERVER,pszServerIP);

	if (RegServer.Open(HKEY_CURRENT_USER,szRegServer,KEY_READ)==ERROR_SUCCESS)
	{
		TCHAR szReadData[1024]=TEXT("");
		DWORD dwReadData=0L,dwDataType=0L,dwDataSize=sizeof(szReadData);
		LONG lErrorCode=RegServer.QueryValue(TEXT("ServerAddr"),&dwDataType,szReadData,&dwDataSize);
		if (lErrorCode==ERROR_SUCCESS) 
		{
			CXOREncrypt::CrevasseData(szReadData,szServerAddr,sizeof(szServerAddr));
			pszServerIP=szServerAddr;
		}
	}

	//连接服务器
	try
	{
		//连接服务器
		m_ClientSocket->CloseSocket();
		
		//代理判断
		if ( m_DlgLogon.EnableProxy() == true )
		{
			//获取代理
			enProxyServerType ProxyServerType;
			tagProxyServerInfo ProxyServerInfo;
			m_DlgLogon.GetProxyInfo(ProxyServerType, ProxyServerInfo);	

			//设置代理
			m_ClientSocket->SetProxyServerInfo(ProxyServerType,ProxyServerInfo);
		}

		if (m_ClientSocket->Connect(pszServerIP,PORT_LOGON_SERVER)!=CONNECT_SUCCESS)
		{
			throw TEXT("服务器连接错误，可能是你的系统还没有成功连接上网络！");
		}
	}
	catch (LPCTSTR pszError)
	{
		ShowMessageBox(pszError,MB_ICONINFORMATION);
		SendLogonMessage();
		return;
	}

	//显示提示框
	CString strMessage=TEXT("正在连接服务器，请稍候...");
	g_GlobalAttemper.ShowStatusMessage(strMessage,this);
	
	return;
}

//取消连接
void CPlazaViewItem::OnCommandCancelConnect()
{
	g_GlobalAttemper.DestroyStatusWnd(this);
	m_ClientSocket->CloseSocket();
	SendLogonMessage();
	return;
}

//更新标题
void CPlazaViewItem::UpdateTitleText()
{
	CString strTitle;
	strTitle.LoadString(AfxGetInstanceHandle(),IDS_MAIN_TITLE);
	AfxGetMainWnd()->SetWindowText(strTitle);

	return;
}

//绘画网页
void CPlazaViewItem::DrawWebFrame(CDC * pDC)
{
	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//绘画网页
	CSkinAide::DrawEncircleFrame(pDC,CRect(0,0,rcClient.Width(),rcClient.Height()),m_EncircleWeb);

	return;
}

afx_msg void CPlazaViewItem::OnBnClickedBEFORE()
{
	m_pHtmlBrower->GoBack();
}
afx_msg void CPlazaViewItem::OnBnClickedNEXT()
{
	m_pHtmlBrower->GoForward();
}
afx_msg void CPlazaViewItem::OnBnClickedMBRUSH()
{
	m_pHtmlBrower->Refresh();
}
afx_msg void CPlazaViewItem::OnBnClickedSTOP()
{
	m_pHtmlBrower->Stop();
}
afx_msg void CPlazaViewItem::OnBnClickedHOME()
{
	m_pHtmlBrower->Navigate(g_GlobalUnits.GetStationPage());
}
afx_msg void CPlazaViewItem::OnBnClickedBBS()
{
	m_pHtmlBrower->Navigate("http://www.dc173.com");
}

//-----------------------------------------------
//					the end
//-----------------------------------------------
