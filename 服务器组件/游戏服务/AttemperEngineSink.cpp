#include "StdAfx.h"
#include "TableFrame.h"
#include "DataBaseSink.h"
#include "AttemperEngineSink.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CAttemperEngineSink::CAttemperEngineSink()
{
	//游戏变量
	m_wMaxUserItem=0;
	m_pTableFrame=NULL;
	m_pNormalParameter=NULL;
	m_pAndroidParameter=NULL;

	//发送场所
	m_cbSendLocation = 0;

	//消息变量
	m_cbRoomMsgIndex=0;
	m_cbGameMsgIndex=0;

	//配置变量
	m_dwCenterServer=0L;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	//控制变量
	m_bShallClose=false;
	m_bAllowWisper=true;
	m_bAllowRoomChat=true;
	m_bAllowGameChat=true;
	m_bAllowEnterRoom=true;
	m_bAllowEnterGame=true;

	//辅助变量
	m_wColumnInfoSize=0;
	m_wOnLineInfoSize=0;
	memset(&m_ColumnInfo,0,sizeof(m_ColumnInfo));
	memset(&m_OnLineCountInfo,0,sizeof(m_OnLineCountInfo));

	//游戏组件
	m_hDllInstance=NULL;
	m_pIGameServiceManager=NULL;

	//接口变量
	m_pITimerEngine=NULL;
	m_pIAttemperEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pITCPSocketCorrespond=NULL;

	return;
}

//析构函数
CAttemperEngineSink::~CAttemperEngineSink()
{
	//删除内存
	SafeDeleteArray(m_pTableFrame);
	SafeDeleteArray(m_pNormalParameter);
	SafeDeleteArray(m_pAndroidParameter);

	return;
}

//接口查询
void * __cdecl CAttemperEngineSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAttemperEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE(IGameServiceFrame,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAttemperEngineSink,Guid,dwQueryVer);
	return NULL;
}

//调度模块启动
bool __cdecl CAttemperEngineSink::OnAttemperEngineStart(IUnknownEx * pIUnknownEx)
{
	//效验参数
	ASSERT(pIUnknownEx!=NULL);

	//设置变量
	m_wColumnInfoSize=0;
	m_wOnLineInfoSize=0;
	memset(&m_ColumnInfo,0,sizeof(m_ColumnInfo));
	memset(&m_OnLineCountInfo,0,sizeof(m_OnLineCountInfo));

	//创建辅助变量
	m_wMaxUserItem=m_pGameServiceOption->wMaxConnect;
	m_pNormalParameter=new tagConnectItemInfo [m_wMaxUserItem];
	m_pAndroidParameter=new tagConnectItemInfo [MAX_ANDROID];
	memset(m_pNormalParameter,0,sizeof(tagConnectItemInfo)*m_wMaxUserItem);
	memset(m_pAndroidParameter,0,sizeof(tagConnectItemInfo)*MAX_ANDROID);

	//属性变量
	m_PropertyAttributeArrary.RemoveAll();
	m_FlowerAttributeArrary.RemoveAll();

	//加载游戏组件
	if (m_pIGameServiceManager->RectifyServiceOption(m_pGameServiceOption)==false) return false;

	//创建游戏桌子
	ASSERT(m_pGameServiceOption->wTableCount!=0);
	m_pTableFrame=new CTableFrame [m_pGameServiceOption->wTableCount];
	if (m_pTableFrame==NULL) return false;

	//初始化桌子
	bool bSuccess=false;
	tagTableFrameParameter TableFrameParameter;
	TableFrameParameter.pGameServiceAttrib=m_pGameServiceAttrib;
	TableFrameParameter.pGameServiceOption=m_pGameServiceOption;
	TableFrameParameter.pIGameServiceFrame=QUERY_ME_INTERFACE(IUnknownEx);
	TableFrameParameter.pIGameServiceManager=QUERY_OBJECT_PTR_INTERFACE(m_pIGameServiceManager,IUnknownEx);
	for (WORD i=0;i<m_pGameServiceOption->wTableCount;i++)
	{
		bSuccess=(m_pTableFrame+i)->InitTableFrame(i,&TableFrameParameter);
		if (bSuccess==false) return false;
	}

	//机器人类
	m_AndroidUserManager.m_pITimerEngine=m_pITimerEngine;
	m_AndroidUserManager.m_pGameServiceAttrib=m_pGameServiceAttrib;
	m_AndroidUserManager.m_pGameServiceOption=m_pGameServiceOption;
	m_AndroidUserManager.m_pIGameServiceManager=m_pIGameServiceManager;
	m_AndroidUserManager.m_pIServerUserManager=QUERY_OBJECT_INTERFACE(m_ServerUserManager,IServerUserManager);
	m_AndroidUserManager.m_pITCPNetworkEngineEvent=QUERY_OBJECT_PTR_INTERFACE(m_pIAttemperEngine,ITCPNetworkEngineEvent);

	//构造列表解释
	switch (m_pGameServiceOption->wServerType)
	{
	case GAME_GENRE_GOLD:	//金币类型
		{
			//变量定义
			tagColumnItem ColumnItem[]=
			{
				115,DTP_USER_ACCOUNTS,TEXT("用户名"),
					60,DTP_GAME_ID,TEXT("ID 号码"),
					60,DTP_USER_SCORE,TEXT("游戏币"),
					40,DTP_USER_TABLE,TEXT("桌号"),
					60,DTP_LOVE_LINESS,TEXT("魅力"),
					85,DTP_GAME_LEVEL,TEXT("游戏级别"),
					50,DTP_USER_EXPERIENCE,TEXT("经验值"),
					60,DTP_WIN_RATE,TEXT("胜率"),
					60,DTP_FLEE_RATE,TEXT("逃率"),
					60,DTP_USER_PLAY_COUNT,TEXT("总局"),
					60,DTP_USER_WIN,TEXT("赢局"),
					60,DTP_USER_LOST,TEXT("输局"),
					60,DTP_USER_DRAW,TEXT("和局"),
					60,DTP_USER_FLEE,TEXT("逃局"),
					200,DTP_UNDER_WRITE,TEXT("个性签名"),
			};

			//设置变量
			m_ColumnInfo.wColumnCount=CountArray(ColumnItem);
			CopyMemory(m_ColumnInfo.ColumnItem,ColumnItem,sizeof(ColumnItem));

			break;
		}
	case GAME_GENRE_MATCH:	//比赛类型
		{
			//变量定义
			tagColumnItem ColumnItem[]=
			{
				115,DTP_USER_ACCOUNTS,TEXT("用户名"),
					60,DTP_GAME_ID,TEXT("ID 号码"),
					60,DTP_USER_SCORE,TEXT("赛币"),
					40,DTP_USER_TABLE,TEXT("桌号"),
					60,DTP_LOVE_LINESS,TEXT("魅力"),
					85,DTP_GAME_LEVEL,TEXT("游戏级别"),
					50,DTP_USER_EXPERIENCE,TEXT("经验值"),
					60,DTP_WIN_RATE,TEXT("胜率"),
					60,DTP_FLEE_RATE,TEXT("逃率"),
					60,DTP_USER_PLAY_COUNT,TEXT("总局"),
					60,DTP_USER_WIN,TEXT("赢局"),
					60,DTP_USER_LOST,TEXT("输局"),
					60,DTP_USER_DRAW,TEXT("和局"),
					60,DTP_USER_FLEE,TEXT("逃局"),
					200,DTP_UNDER_WRITE,TEXT("个性签名"),
			};

			//设置变量
			m_ColumnInfo.wColumnCount=CountArray(ColumnItem);
			CopyMemory(m_ColumnInfo.ColumnItem,ColumnItem,sizeof(ColumnItem));

			break;
		}
	default:
		{
			//变量定义
			tagColumnItem ColumnItem[]=
			{
				115,DTP_USER_ACCOUNTS,TEXT("用户名"),
					60,DTP_GAME_ID,TEXT("ID 号码"),
					60,DTP_USER_SCORE,TEXT("积分"),
					40,DTP_USER_TABLE,TEXT("桌号"),
					60,DTP_LOVE_LINESS,TEXT("魅力"),
					85,DTP_GAME_LEVEL,TEXT("游戏级别"),
					50,DTP_USER_EXPERIENCE,TEXT("经验值"),
					60,DTP_WIN_RATE,TEXT("胜率"),
					60,DTP_FLEE_RATE,TEXT("逃率"),
					60,DTP_USER_PLAY_COUNT,TEXT("总局"),
					60,DTP_USER_WIN,TEXT("赢局"),
					60,DTP_USER_LOST,TEXT("输局"),
					60,DTP_USER_DRAW,TEXT("和局"),
					60,DTP_USER_FLEE,TEXT("逃局"),
					200,DTP_UNDER_WRITE,TEXT("个性签名"),
			};

			//设置变量
			m_ColumnInfo.wColumnCount=CountArray(ColumnItem);
			CopyMemory(m_ColumnInfo.ColumnItem,ColumnItem,sizeof(ColumnItem));

			break;
		}
	}

	//设置列表
	WORD wColumnInfoHandSize=sizeof(m_ColumnInfo)-sizeof(m_ColumnInfo.ColumnItem);
	m_wColumnInfoSize=wColumnInfoHandSize+m_ColumnInfo.wColumnCount*sizeof(m_ColumnInfo.ColumnItem[0]);

	//随机数字
	srand((DWORD)time(NULL));

	//连接中心服务器
	m_pITCPSocketCorrespond->Connect(ntohl(m_dwCenterServer),PORT_CENTER_SERVER);

	//系统消息
	m_pITimerEngine->SetTimer(IDI_SEND_SYSTEM_MESSAGE,300000L,(DWORD)(-1),0);

	//启动机器
	if (m_AndroidUserManager.StartService()==false) return false;

	//加载用户
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_ANDROID,0,NULL,0);

	//加载道具
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_PROPERTY, 0,NULL, 0);

	//分配用户
	m_pITimerEngine->SetTimer(IDI_DISTRIBUTE_ANDROID,TIME_DISTRIBUTE_ANDROID*1000L,(DWORD)(-1),NULL);

	return true;
}

//调度模块关闭
bool __cdecl CAttemperEngineSink::OnAttemperEngineStop(IUnknownEx * pIUnknownEx)
{
	//注销房间
	CMD_CS_UnRegGameServer UnRegGameServer;
	memset(&UnRegGameServer,0,sizeof(UnRegGameServer));
	UnRegGameServer.wKindID=m_pGameServiceAttrib->wKindID;
	UnRegGameServer.wServerID=m_pGameServiceOption->wServerID;
	m_pITCPSocketCorrespond->SendData(MDM_CS_SERVER_MANAGER,SUB_CS_UNREG_GAME_SERVER,&UnRegGameServer,sizeof(UnRegGameServer));

	//关闭组件
	m_AndroidUserManager.StopService();
	m_ServerUserManager.ResetUserManager();

	//删除数据
	m_wMaxUserItem=0;
	SafeDeleteArray(m_pTableFrame);
	SafeDeleteArray(m_pNormalParameter);
	SafeDeleteArray(m_pAndroidParameter);

	//控制变量
	m_bShallClose=false;
	m_bAllowWisper=true;
	m_bAllowRoomChat=true;
	m_bAllowGameChat=true;
	m_bAllowEnterRoom=true;
	m_bAllowEnterGame=true;

	//消息变量
	m_cbRoomMsgIndex=0;
	m_cbGameMsgIndex=0;

	//设置变量
	m_wColumnInfoSize=0;
	m_wOnLineInfoSize=0;
	memset(&m_ColumnInfo,0,sizeof(m_ColumnInfo));
	memset(&m_OnLineCountInfo,0,sizeof(m_OnLineCountInfo));

	return true;
}

//发送状态
bool __cdecl CAttemperEngineSink::SendTableStatus(WORD wTableID)
{
	//效验参数
	ASSERT(wTableID<m_pGameServiceOption->wTableCount);
	if (wTableID>=m_pGameServiceOption->wTableCount) return false;

	//构造变量
	CMD_GR_TableStatus TableStatus;
	memset(&TableStatus,0,sizeof(TableStatus));
	TableStatus.wTableID=wTableID;
	TableStatus.bTableLock=(m_pTableFrame+wTableID)->IsTableLocked()?TRUE:FALSE;
	TableStatus.bPlayStatus=(m_pTableFrame+wTableID)->IsGameStarted()?TRUE:FALSE;

	//发送数据
	m_pITCPNetworkEngine->SendDataBatch(MDM_GR_STATUS,SUB_GR_TABLE_STATUS,&TableStatus,sizeof(TableStatus));
	m_AndroidUserManager.SendDataToClient(MDM_GR_STATUS,SUB_GR_TABLE_STATUS,&TableStatus,sizeof(TableStatus));

	return true;
}

//发送分数
bool __cdecl CAttemperEngineSink::SendUserScore(IServerUserItem * pIServerUserItem)
{
	tagServerUserData * pUserData=pIServerUserItem->GetUserData();

	//发送数据
	CMD_GR_UserScore UserScore;
	UserScore.dwUserID=pUserData->dwUserID;
	UserScore.UserScore=pUserData->UserScoreInfo;
	UserScore.lLoveliness = pUserData->lLoveliness;
	UserScore.UserScore.lScore=pUserData->UserScoreInfo.lScore+pUserData->lStorageScore;

	m_pITCPNetworkEngine->SendDataBatch(MDM_GR_USER,SUB_GR_USER_SCORE,&UserScore,sizeof(UserScore));
	m_AndroidUserManager.SendDataToClient(MDM_GR_USER,SUB_GR_USER_SCORE,&UserScore,sizeof(UserScore));

	return true;
}

//发送状态
bool __cdecl CAttemperEngineSink::SendUserStatus(IServerUserItem * pIServerUserItem)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	ASSERT(pIServerUserItem->IsActive()==true);
	if (pIServerUserItem->IsActive()==false) return false;

	//变量定义
	CMD_GR_UserStatus UserStatus;
	memset(&UserStatus,0,sizeof(UserStatus));
	tagServerUserData * pUserData=pIServerUserItem->GetUserData();

	//构造数据
	UserStatus.dwUserID=pUserData->dwUserID;
	UserStatus.wTableID=pUserData->wTableID;
	UserStatus.wChairID=pUserData->wChairID;
	UserStatus.cbUserStatus=pUserData->cbUserStatus;

	//发送数据
	m_pITCPNetworkEngine->SendDataBatch(MDM_GR_USER,SUB_GR_USER_STATUS,&UserStatus,sizeof(UserStatus));
	m_AndroidUserManager.SendDataToClient(MDM_GR_USER,SUB_GR_USER_STATUS,&UserStatus,sizeof(UserStatus));

	return true;
}

//发送道具
bool __cdecl CAttemperEngineSink::SendResidualProperty(IServerUserItem * pIServerUserItem)
{
	//变量定义
	CMD_GF_ResidualProperty ResidualProperty;
	ZeroMemory(&ResidualProperty, sizeof(ResidualProperty));
	tagServerUserData *pServerUserData = pIServerUserItem->GetUserData();

	//合法判断
	ASSERT(PROPERTY_COUNT <= CountArray(ResidualProperty.dwResidualTime));

	//计算道具
	for ( WORD wPropertyID = 0; wPropertyID < PROPERTY_COUNT; ++wPropertyID )
	{
		DWORD dwResidualTime = 0;
		if ( pServerUserData->PropertyInfo[wPropertyID].dwPropCount < pServerUserData->PropertyInfo[wPropertyID].dwConsumedCount)
			dwResidualTime = 0;
		else
            dwResidualTime = pServerUserData->PropertyInfo[wPropertyID].dwPropCount - pServerUserData->PropertyInfo[wPropertyID].dwConsumedCount;

		ResidualProperty.dwResidualTime[wPropertyID] = dwResidualTime;
	}

	//发送消息
	SendData(pIServerUserItem, MDM_GF_PRESENT, SUB_GF_RESIDUAL_PROPERTY, &ResidualProperty, sizeof(ResidualProperty));

	return true;
}

//删除用户
bool __cdecl CAttemperEngineSink::DeleteUserItem(IServerUserItem * pIServerUserItem)
{
	//效验参数
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->IsActive()==true));
	if ((pIServerUserItem==NULL)||(pIServerUserItem->IsActive()==false)) return false;

	//效验状态
	ASSERT(pIServerUserItem->GetTableID()==INVALID_TABLE);
	ASSERT(pIServerUserItem->GetChairID()==INVALID_CHAIR);

	//获取用户
	tagServerUserData *pServerUserData = pIServerUserItem->GetUserData();

	//写入道具
	DBR_GR_WriteProperty WriteProperty;
	ZeroMemory(&WriteProperty, sizeof(WriteProperty));
	WriteProperty.dwUserID = pIServerUserItem->GetUserID();
	WriteProperty.dwClientIP = pIServerUserItem->GetClientIP();
	for ( int nIdex = 0; nIdex < PROPERTY_COUNT; ++nIdex )
	{
		//会员卡
		if ( PROP_MEMBER_1 <= nIdex && nIdex <= PROP_MEMBER_8 ) continue;

		if ( 0 < pServerUserData->PropertyInfo[nIdex].dwPropCount )
		{
			LONG lLeftTime = pServerUserData->PropertyInfo[nIdex].dwPropCount - pServerUserData->PropertyInfo[nIdex].dwConsumedCount;
			if ( lLeftTime < 0 ) lLeftTime = 0;
			WriteProperty.lUseableTime = lLeftTime;
			WriteProperty.nPropertyID = nIdex;

			//投递请求
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_WRITE_PROPERTY, 0, &WriteProperty, sizeof(WriteProperty));
		}
	}

	//写入积分
	DBR_GR_LeaveGameServer LeaveGameServer;
	ZeroMemory(&LeaveGameServer,sizeof(LeaveGameServer));

	//设置变量
	LeaveGameServer.dwUserID=pIServerUserItem->GetUserID();
	LeaveGameServer.dwClientIP=pIServerUserItem->GetClientIP();
	pIServerUserItem->GetUserScoreInfo(LeaveGameServer.ScoreModifyInfo);//

	//附加信息
	LeaveGameServer.lLoveliness = pIServerUserItem->GetUserData()->lLoveliness;
	LeaveGameServer.lRevenue=pIServerUserItem->GetUserRevenue();
	LeaveGameServer.dwPlayTimeCount=pIServerUserItem->GetPlayTimeCount();
	LeaveGameServer.dwOnlineTimeCount=pIServerUserItem->GetOnlineTimeCount();

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LEAVE_GAME_SERVER,0,&LeaveGameServer,sizeof(LeaveGameServer));

	//发送状态
	CMD_GR_UserStatus UserStatus;
	ZeroMemory(&UserStatus,sizeof(UserStatus));
	tagServerUserData * pUserData=pIServerUserItem->GetUserData();
	UserStatus.dwUserID=pIServerUserItem->GetUserID();
	UserStatus.wTableID=INVALID_TABLE;
	UserStatus.wChairID=INVALID_CHAIR;
	UserStatus.cbUserStatus=US_NULL;
	m_pITCPNetworkEngine->SendDataBatch(MDM_GR_USER,SUB_GR_USER_STATUS,&UserStatus,sizeof(UserStatus));
	m_AndroidUserManager.SendDataToClient(MDM_GR_USER,SUB_GR_USER_STATUS,&UserStatus,sizeof(UserStatus));

	//常规连接
	WORD wBindIndex=pIServerUserItem->GetUserIndex();
	if (wBindIndex<m_pGameServiceOption->wMaxConnect)
	{
		(m_pNormalParameter+wBindIndex)->pIServerUserItem=NULL;
	}
	else if ((wBindIndex>=INDEX_ANDROID)&&(wBindIndex<(INDEX_ANDROID+MAX_ANDROID)))
	{
		(m_pAndroidParameter+(wBindIndex-INDEX_ANDROID))->pIServerUserItem=NULL;
	}

	//删除用户
	m_ServerUserManager.DeleteUserItem(pIServerUserItem);

	return true;
}

//发送数据
bool __cdecl CAttemperEngineSink::SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	ASSERT(pIServerUserItem->IsActive()==true);
	if (pIServerUserItem->IsActive()==false) return false;

	//发送数据
	WORD wIndex=pIServerUserItem->GetUserIndex();
	if (wIndex!=INVALID_WORD)
	{
		tagConnectItemInfo * pConnectItemInfo=GetBindParameter(wIndex);
		if (wIndex>=INDEX_ANDROID)
		{
			DWORD dwID=pConnectItemInfo->dwSocketID;
			m_AndroidUserManager.SendDataToClient(dwID,wMainCmdID,wSubCmdID,NULL,0);
		}
		else
		{
			m_pITCPNetworkEngine->SendData(pConnectItemInfo->dwSocketID,wMainCmdID,wSubCmdID);
		}
	}

	return true;
}

//发送数据
bool __cdecl CAttemperEngineSink::SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	ASSERT(pIServerUserItem->IsActive()==true);
	if (pIServerUserItem->IsActive()==false) return false;

	//发送数据
	WORD wIndex=pIServerUserItem->GetUserIndex();
	if (wIndex!=INVALID_WORD)
	{
		tagConnectItemInfo * pConnectItemInfo=GetBindParameter(wIndex);
		if (wIndex>=INDEX_ANDROID)
		{
			DWORD dwID=pConnectItemInfo->dwSocketID;
			m_AndroidUserManager.SendDataToClient(dwID,wMainCmdID,wSubCmdID,pData,wDataSize);
		}
		else
		{
			m_pITCPNetworkEngine->SendData(pConnectItemInfo->dwSocketID,wMainCmdID,wSubCmdID,pData,wDataSize);
		}
	}

	return true;
}

//设置定时器
bool __cdecl CAttemperEngineSink::SetTableTimer(WORD wTableID, DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM wBindParam)
{
	//效验参数
	ASSERT(dwTimerID<RANGE_TABLE_TIME);
	ASSERT(wTableID<m_pGameServiceOption->wTableCount);

	//设置定时器
	if (wTableID>=m_pGameServiceOption->wTableCount) return false;
	DWORD dwTableTimeID=IDI_TABLE_FRAME_START+wTableID*RANGE_TABLE_TIME+dwTimerID;
	return m_pITimerEngine->SetTimer(dwTableTimeID,dwElapse,dwRepeat,wBindParam);
}

//删除定时器
bool __cdecl CAttemperEngineSink::KillTableTimer(WORD wTableID, DWORD dwTimerID)
{
	//效验参数
	ASSERT(dwTimerID<RANGE_TABLE_TIME);
	ASSERT(wTableID<m_pGameServiceOption->wTableCount);

	//删除定时器
	if (wTableID>=m_pGameServiceOption->wTableCount) return false;
	DWORD dwTableTimeID=IDI_TABLE_FRAME_START+wTableID*RANGE_TABLE_TIME+dwTimerID;
	return m_pITimerEngine->KillTimer(dwTableTimeID);
}

//输出信息
void __cdecl CAttemperEngineSink::ExportInformation(LPCTSTR pszString, enTraceLevel TraceLevel)
{
	CTraceService::TraceString(pszString,TraceLevel);
	return;
}

//发送登录失败
bool CAttemperEngineSink::SendLogonFailed(DWORD dwSocketID, LPCTSTR pszMessage, LONG lErrorCode)
{
	//构造数据
	CMD_GR_LogonError LogonError;
	LogonError.lErrorCode=lErrorCode;
	lstrcpyn(LogonError.szErrorDescribe,pszMessage,sizeof(LogonError.szErrorDescribe));
	WORD wDescribeSize=CountStringBuffer(LogonError.szErrorDescribe);

	//发送数据
	WORD wSendSize=sizeof(LogonError)-sizeof(LogonError.szErrorDescribe)+wDescribeSize;
	if (LOWORD(dwSocketID)>=INDEX_ANDROID)
	{
		m_AndroidUserManager.SendDataToClient(dwSocketID,MDM_GR_LOGON,SUB_GR_LOGON_ERROR,&LogonError,wSendSize);
	}
	else m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GR_LOGON,SUB_GR_LOGON_ERROR,&LogonError,wSendSize);

	return true;
}

//发送道具消息
bool CAttemperEngineSink::SendProMessage(DWORD dwSocketID, LPCTSTR lpszMessage, WORD wMessageType)
{
	//tagConnectItemInfo * pConnectItemInfo=GetBindParameter(LOWORD(dwSocketID));
	//IServerUserItem *pIServerUserItem = pConnectItemInfo->pIServerUserItem;
	//if(pIServerUserItem->GetTableID()!=INVALID_TABLE)
	//	SendGameMessage(dwSocketID, lpszMessage, wMessageType);
	//else SendRoomMessage(dwSocketID, lpszMessage, wMessageType);

	//判断场所
	if(m_cbSendLocation==LOCATION_GAME_ROOM)
		SendGameMessage(dwSocketID, lpszMessage, wMessageType);
	else SendRoomMessage(dwSocketID, lpszMessage, wMessageType);

	return true;
}

//发送房间消息
bool CAttemperEngineSink::SendRoomMessage(DWORD dwSocketID, LPCTSTR lpszMessage, WORD wMessageType)
{
	//构造数据包
	CMD_GR_Message Message;
	Message.wMessageType=wMessageType;
	lstrcpyn(Message.szContent,lpszMessage,CountArray(Message.szContent));
	Message.wMessageLength=CountStringBuffer(Message.szContent);

	//发送数据
	WORD wSendSize=sizeof(Message)-sizeof(Message.szContent)+Message.wMessageLength*sizeof(TCHAR);
	if (LOWORD(dwSocketID)==INVALID_WORD)
	{
		m_pITCPNetworkEngine->SendDataBatch(MDM_GR_SYSTEM,SUB_GR_MESSAGE,&Message,wSendSize);
		m_AndroidUserManager.SendDataToClient(MDM_GR_SYSTEM,SUB_GR_MESSAGE,&Message,wSendSize);
	}
	else
	{
		if (LOWORD(dwSocketID)>=INDEX_ANDROID)
		{
			m_AndroidUserManager.SendDataToClient(dwSocketID,MDM_GR_SYSTEM,SUB_GR_MESSAGE,&Message,wSendSize);
		}
		else m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GR_SYSTEM,SUB_GR_MESSAGE,&Message,wSendSize);
	}

	return true;
}

//发送游戏消息
bool CAttemperEngineSink::SendGameMessage(DWORD dwSocketID, LPCTSTR lpszMessage, WORD wMessageType)
{
	//构造数据包
	CMD_GF_Message Message;
	Message.wMessageType=wMessageType;
	lstrcpyn(Message.szContent,lpszMessage,CountArray(Message.szContent));
	Message.wMessageLength=CountStringBuffer(Message.szContent);

	//发送数据
	WORD wSendSize=sizeof(Message)-sizeof(Message.szContent)+Message.wMessageLength*sizeof(TCHAR);
	if (LOWORD(dwSocketID)==INVALID_WORD)
	{
		m_pITCPNetworkEngine->SendDataBatch(MDM_GF_FRAME,SUB_GF_MESSAGE,&Message,wSendSize);
		m_AndroidUserManager.SendDataToClient(MDM_GF_FRAME,SUB_GF_MESSAGE,&Message,wSendSize);
	}
	else
	{
		if (LOWORD(dwSocketID)>=INDEX_ANDROID)
		{
			m_AndroidUserManager.SendDataToClient(dwSocketID,MDM_GF_FRAME,SUB_GF_MESSAGE,&Message,wSendSize);
		}
		else m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GF_FRAME,SUB_GF_MESSAGE,&Message,wSendSize);
	}

	return true;
}

//发送道具消息
bool CAttemperEngineSink::SendProMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType)
{
	//效验用户
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//发送数据
	WORD wIndex=pIServerUserItem->GetUserIndex();
	if (wIndex!=INVALID_WORD)
	{
		tagConnectItemInfo * pConnectItemInfo=GetBindParameter(wIndex);
		SendProMessage(pConnectItemInfo->dwSocketID,lpszMessage,wMessageType);
		return true;
	}

	return true;
}

//发送房间消息
bool CAttemperEngineSink::SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType)
{
	//效验用户
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//发送数据
	WORD wIndex=pIServerUserItem->GetUserIndex();
	if (wIndex!=INVALID_WORD)
	{
		tagConnectItemInfo * pConnectItemInfo=GetBindParameter(wIndex);
		SendRoomMessage(pConnectItemInfo->dwSocketID,lpszMessage,wMessageType);
		return true;
	}

	return false;
}

//发送游戏消息
bool CAttemperEngineSink::SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType)
{
	//效验用户
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//用户过滤
	if ((pIServerUserItem->GetTableID()==INVALID_TABLE)||(pIServerUserItem->GetChairID()==INVALID_CHAIR)) return false;

	//发送数据
	WORD wIndex=pIServerUserItem->GetUserIndex();
	if (wIndex!=INVALID_WORD)
	{
		tagConnectItemInfo * pConnectItemInfo=GetBindParameter(wIndex);
		SendGameMessage(pConnectItemInfo->dwSocketID,lpszMessage,wMessageType);

		return true;
	}

	return false;
}

//定时器事件
bool __cdecl CAttemperEngineSink::OnEventTimer(DWORD dwTimerID, WPARAM wBindParam)
{
	//内核定时器
	if ((dwTimerID>=IDI_ATTEMPER_START)&&(dwTimerID<=IDI_ATTEMPER_CONCLUDE))
	{
		switch (dwTimerID)
		{
		case IDI_CONNECT_CENTER_SERVER:		//连接服务器
			{
				//发起连接
				m_pITCPSocketCorrespond->Connect(ntohl(m_dwCenterServer),PORT_CENTER_SERVER);

				//错误提示
				CTraceService::TraceString(TEXT("正在尝试重新连接中心服务器...."),TraceLevel_Normal);

				return true;
			}
		case IDI_UPDATE_SERVER_LIST:		//更新列表
			{
				m_pITCPSocketCorrespond->SendData(MDM_CS_SERVER_LIST,SUB_CS_GET_SERVER_LIST,NULL,0);
				return true;
			}
		case IDI_UPDATE_ONLINE_COUNT:		//更新人数
			{
				//更新人数
				CMD_CS_ServerOnLineCount ServerOnLineCount;
				ServerOnLineCount.wKindID=m_pGameServiceOption->wKindID;
				ServerOnLineCount.wServerID=m_pGameServiceOption->wServerID;
				ServerOnLineCount.dwOnLineCount=(m_ServerUserManager.GetOnLineCount()+m_ServerUserManager.GetOffLineCount());
				m_pITCPSocketCorrespond->SendData(MDM_CS_SERVER_MANAGER,SUB_CS_SERVER_ONLINE_COUNT,&ServerOnLineCount,sizeof(ServerOnLineCount));

				return true;
			}
		case IDI_SEND_SYSTEM_MESSAGE:		//系统消息
			{
				//获取目录
				TCHAR szPath[MAX_PATH]=TEXT("");
				GetCurrentDirectory(sizeof(szPath),szPath);

				//读取配置
				TCHAR szFileName[MAX_PATH],szMessage[1024];
				_snprintf(szFileName,sizeof(szFileName),TEXT("%s\\GameService.ini"),szPath);
				
				//循环消息
				TCHAR szRoomMsgName[64],szGameMsgName[64];
				_snprintf(szRoomMsgName,sizeof(szRoomMsgName),TEXT("RoomMessage%d"),m_cbRoomMsgIndex++);
				_snprintf(szGameMsgName,sizeof(szGameMsgName),TEXT("GameMessage%d"),m_cbGameMsgIndex++);

				//系统消息
				GetPrivateProfileString(TEXT("Message"),szRoomMsgName,TEXT(""),szMessage,sizeof(szMessage),szFileName);
				if (szMessage[0]!=0) SendRoomMessage(INVALID_WORD,szMessage,SMT_INFO);
				else m_cbRoomMsgIndex=0;

				//游戏消息
				GetPrivateProfileString(TEXT("Message"),szGameMsgName,TEXT(""),szMessage,sizeof(szMessage),szFileName);
				if (szMessage[0]!=0) SendGameMessage(INVALID_WORD,szMessage,SMT_INFO);
				else m_cbGameMsgIndex=0;

				return true;
			}
		case IDI_LOAD_ANDROID_INFO:		//加载用户
			{
				//投递请求
				m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_ANDROID,0,NULL,0);

				return true;
			}
		case IDI_DISTRIBUTE_ANDROID:	//分配用户
			{
				//用户统计
				tagAndroidUserStatus AndroidUserStatus;
				WORD wAndroidCount=GetAndroidUserStatus(AndroidUserStatus);
				WORD wAndroidBusyCount=AndroidUserStatus.wPlayUserCount+AndroidUserStatus.wSitdownUserCount;

				//动作处理
				if (wAndroidCount>0)
				{
					//分析变量
					tagTableUserStatus TableUserStatus;
					ZeroMemory(&TableUserStatus,sizeof(TableUserStatus));

					//陪打处理
					if (AndroidUserStatus.wFreeUserCount>0)
					{
						for (INT_PTR i=0;i<m_pGameServiceOption->wTableCount;i++)
						{
							//获取桌子
							CTableFrame * pTableFrame=(m_pTableFrame+i);
							if (pTableFrame->IsGameStarted()==true) continue;

							//桌子状况
							WORD wUserSitCount=GetTableUserStatus(pTableFrame,TableUserStatus);

							//数目过虑
							if (wUserSitCount==0) continue;
							if (TableUserStatus.wTableUserCount==0) continue;

							if (wUserSitCount>=TableUserStatus.wLessUserCount) 
								continue;

							//坐下处理
							WORD wChairID=pTableFrame->GetNullChairID();
							if(wChairID==INVALID_CHAIR) continue ;

							IServerUserItem * pIServerUserItem=AndroidUserStatus.pServerUserFree[0];
							if (pTableFrame->PerformSitDownAction(wChairID,pIServerUserItem,NULL)==true) break ;
						}

						//全部机器人
						if(wAndroidBusyCount < (wAndroidCount*9)/10 && m_pGameServiceAttrib->wChairCount <= AndroidUserStatus.wFreeUserCount )
						{
							bool bSitSuccess = false ;
							for (INT_PTR i=0;i<m_pGameServiceOption->wTableCount;i++)
							{
								//获取桌子
								CTableFrame * pTableFrame=(m_pTableFrame+i);
								if (pTableFrame->IsGameStarted()==true) continue;

								//桌子状况
								WORD wUserSitCount=GetTableUserStatus(pTableFrame,TableUserStatus);

								//数目过虑
								if (TableUserStatus.wTableUserCount>0) continue;
								if (wUserSitCount>=TableUserStatus.wLessUserCount) continue;

								//坐下处理
								for(BYTE wChairIndex=0; wChairIndex<pTableFrame->GetChairCount(); wChairIndex++)
								{
									//获取空位
									WORD wChairID=pTableFrame->GetNullChairID();
									if(wChairID==INVALID_CHAIR) break ;

									for(WORD wAndroidUser=0; wAndroidUser<AndroidUserStatus.wFreeUserCount; ++wAndroidUser)
									{
										IServerUserItem * pIServerUserItem=AndroidUserStatus.pServerUserFree[wAndroidUser];

										//状态判断
										if(!pIServerUserItem) continue ;
										if(pIServerUserItem->GetUserStatus()>=US_SIT) continue ;
                                        
										//坐下处理
										if (pTableFrame->PerformSitDownAction(wChairID,pIServerUserItem,NULL)==true) break ;
									}
								}

								//查看情况
								GetTableUserStatus(pTableFrame,TableUserStatus);
								if(TableUserStatus.wTableAndroidCount>=TableUserStatus.wLessUserCount) return true;
							}
						}
					}

					//起立处理
					if (AndroidUserStatus.wSitdownUserCount>0)
					{
						for (INT_PTR i=0;i<m_pGameServiceOption->wTableCount;i++)
						{
							//获取桌子
							CTableFrame * pTableFrame=(m_pTableFrame+i);
							if (pTableFrame->IsGameStarted()==true) continue;

							//桌子状况
							GetTableUserStatus(pTableFrame,TableUserStatus);

							//用户过虑
							if (TableUserStatus.wTableUserCount>0) continue;
							if (TableUserStatus.wTableAndroidCount==0) continue;

							//起立处理
							for (WORD j=0;j<pTableFrame->GetChairCount();j++)
							{
								IServerUserItem * pIServerUserItem=pTableFrame->GetServerUserItem(j);
								if (pIServerUserItem==NULL) continue;
								if(pIServerUserItem->IsAndroidUser()==false) continue ;
								if (pTableFrame->PerformStandUpAction(pIServerUserItem)==true) return true;
							}
						}
					}
				}

				return true;
			}
		}

		return false;
	}

	//机器定时器
	if ((dwTimerID>=IDI_ANDROID_START)&&(dwTimerID<=IDI_ANDROID_CONCLUDE))
	{
		//时间处理
		m_AndroidUserManager.OnEventTimerPulse(dwTimerID,wBindParam);

		return false;
	}

	//桌子定时器
	if (dwTimerID>=IDI_TABLE_FRAME_START)
	{
		//变量定义
		WORD wTableID=(WORD)((dwTimerID-IDI_TABLE_FRAME_START)/RANGE_TABLE_TIME);
		DWORD dwTableTimeID=(DWORD)((dwTimerID-IDI_TABLE_FRAME_START)%RANGE_TABLE_TIME);

		//定时器处理
		ASSERT(wTableID<m_pGameServiceOption->wTableCount);
		return (m_pTableFrame+wTableID)->OnEventTimer(dwTableTimeID,wBindParam);
	}

	return false;
}

//数据库事件
bool __cdecl CAttemperEngineSink::OnEventDataBase(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	switch (wRequestID)
	{
	case DBR_GR_LOGON_SUCCESS:		//登录成功
		{
			return OnDBLogonSuccess(dwContextID,pData,wDataSize);
		}
	case DBR_GR_LOGON_ERROR:		//登录失败
		{
			return OnDBLogonError(dwContextID,pData,wDataSize);
		}
	case DBR_GR_ANDROID_USER:		//机器信息
		{
			return OnDBAndroidUser(dwContextID,pData,wDataSize);
		}
	case DBR_GR_USER_RIGHT_RESULT:	//用户权限
		{
			return OnDBUserRightResult(dwContextID,pData,wDataSize);
		}
	case DBR_GR_PROPERTY_ATTRIBUTE:	//加载道具
		{
			return OnDBLoadProperty(dwContextID,pData,wDataSize);
		}
	case DBR_GR_FLOWER_ATTRIBUTE:	//加载鲜花
		{
			return OnDBLoadFlower(dwContextID,pData,wDataSize);
		}
	case DBR_GR_USER_PROPERTY:		//玩家道具
		{
			return OnDBUserProperty(dwContextID,pData,wDataSize);
		}
	//case DBR_GR_EXCHANGE_RESULT:	//兑换结果
	//	{
	//		return OnDBExchangeCharmResult(dwContextID,pData,wDataSize);
	//	}
	case DBR_GR_LOAD_PROP_FINISHI:	//完成加载
		{
			return OnDBLadPropFinish(dwContextID,pData,wDataSize);
		}
	}

	return false;
}

//用户登录成功
bool CAttemperEngineSink::OnDBLogonSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_LogonSuccess));
	if (wDataSize!=sizeof(DBR_GR_LogonSuccess)) return false;

	//判断在线
	tagConnectItemInfo * pConnectItemInfo=GetBindParameter(LOWORD(dwContextID));
	if (pConnectItemInfo==NULL) return true;
	if (pConnectItemInfo->dwSocketID!=dwContextID) return true;
	ASSERT(pConnectItemInfo->pIServerUserItem==NULL);

	//消息处理
	DBR_GR_LogonSuccess * pLogonSuccess=(DBR_GR_LogonSuccess *)pData;

	//在线判断
	IServerUserItem * pIServerUserItem=m_ServerUserManager.SearchOnLineUser(pLogonSuccess->dwUserID);
	if (pIServerUserItem!=NULL) return false;

	//变量定义
	WORD wIndex=LOWORD(dwContextID);

	//状态判断
	if ((m_bAllowEnterRoom==false)&&(pLogonSuccess->dwMasterRight==0L))
	{
		//发送消息
		LPCTSTR pszMessage=TEXT("抱歉，由于系统维护的原因，本游戏房间不允许任何玩家登录进入！");
		SendRoomMessage(dwContextID,pszMessage,SMT_EJECT|SMT_INFO|SMT_INTERMIT_LINE|SMT_CLOSE_ROOM);

		//解除锁定
		DBR_GR_LeaveGameServer LeaveGameServer;
		ZeroMemory(&LeaveGameServer,sizeof(LeaveGameServer));
		LeaveGameServer.dwUserID=pLogonSuccess->dwUserID;
		LeaveGameServer.dwClientIP=pConnectItemInfo->dwClientIP;
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LEAVE_GAME_SERVER,0,&LeaveGameServer,sizeof(LeaveGameServer));

		return true;
	}

	//判断数目
	DWORD dwUserCount=m_ServerUserManager.GetOnLineCount()+m_ServerUserManager.GetOffLineCount();
	if ((pLogonSuccess->cbMemberOrder==0)&&(pLogonSuccess->cbMasterOrder==0)&&((dwUserCount+LESS_CONNECT_COUNT)>=m_pGameServiceOption->wMaxConnect))
	{
		//发送消息
		LPCTSTR pszMessage=TEXT("抱歉，本游戏房间已经满人，禁止非会员玩家登录进入！");
		SendRoomMessage(dwContextID,pszMessage,SMT_EJECT|SMT_INFO|SMT_INTERMIT_LINE|SMT_CLOSE_ROOM);

		//解除锁定
		DBR_GR_LeaveGameServer LeaveGameServer;
		ZeroMemory(&LeaveGameServer,sizeof(LeaveGameServer));
		LeaveGameServer.dwUserID=pLogonSuccess->dwUserID;
		LeaveGameServer.dwClientIP=pConnectItemInfo->dwClientIP;
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LEAVE_GAME_SERVER,0,&LeaveGameServer,sizeof(LeaveGameServer));

		return true;
	}

	//构造用户信息
	tagServerUserData ServerUserData;
	memset(&ServerUserData,0,sizeof(ServerUserData));
	ServerUserData.wFaceID=pLogonSuccess->wFaceID;
	ServerUserData.dwCustomFaceVer=pLogonSuccess->dwCustomFaceVer;
	ServerUserData.cbGender=pLogonSuccess->cbGender;
	ServerUserData.dwUserID=pLogonSuccess->dwUserID;
	ServerUserData.dwGameID=pLogonSuccess->dwGameID;
	ServerUserData.dwGroupID=pLogonSuccess->dwGroupID;
	ServerUserData.dwUserRight=pLogonSuccess->dwUserRight;
	ServerUserData.lLoveliness=pLogonSuccess->lLoveliness;
	ServerUserData.dwMasterRight=pLogonSuccess->dwMasterRight;
	ServerUserData.cbMemberOrder=pLogonSuccess->cbMemberOrder;
	ServerUserData.cbMasterOrder=pLogonSuccess->cbMasterOrder;
	ServerUserData.UserScoreInfo.lWinCount=pLogonSuccess->lWinCount;
	ServerUserData.UserScoreInfo.lLostCount=pLogonSuccess->lLostCount;
	ServerUserData.UserScoreInfo.lDrawCount=pLogonSuccess->lDrawCount;
	ServerUserData.UserScoreInfo.lFleeCount=pLogonSuccess->lFleeCount;
	ServerUserData.UserScoreInfo.lExperience=pLogonSuccess->lExperience;
	lstrcpyn(ServerUserData.szAccounts,pLogonSuccess->szAccounts,CountArray(ServerUserData.szAccounts));
	lstrcpyn(ServerUserData.szGroupName,pLogonSuccess->szGroupName,CountArray(ServerUserData.szGroupName));
	lstrcpyn(ServerUserData.szUnderWrite,pLogonSuccess->szUnderWrite,CountArray(ServerUserData.szUnderWrite));

	//提取积分
	ServerUserData.lStorageScore=0L;
	ServerUserData.UserScoreInfo.lScore=pLogonSuccess->lScore;

	//存储积分
	if ((m_pGameServiceOption->wServerType==GAME_GENRE_GOLD)&&(m_pGameServiceOption->lRestrictScore>0L))
	{
		if (pLogonSuccess->lScore>m_pGameServiceOption->lRestrictScore)
		{
			ServerUserData.UserScoreInfo.lScore=m_pGameServiceOption->lRestrictScore;
			ServerUserData.lStorageScore=pLogonSuccess->lScore-m_pGameServiceOption->lRestrictScore;
		}
	}

	//消费金币
	ServerUserData.UserScoreInfo.lInsureScore=pLogonSuccess->lInsureScore;
	ServerUserData.UserScoreInfo.lGameGold=pLogonSuccess->lGameGold;

	//状态信息
	ServerUserData.cbUserStatus=US_FREE;
	ServerUserData.wTableID=INVALID_TABLE;
	ServerUserData.wChairID=INVALID_CHAIR;

	//激活用户
	pConnectItemInfo->pIServerUserItem=m_ServerUserManager.ActiveUserItem(ServerUserData,pConnectItemInfo->dwClientIP,wIndex,pLogonSuccess->szPassWord,m_pGameServiceOption->wServerType);

	//发送房间信息
	SendGameServerInfo(pConnectItemInfo->pIServerUserItem,dwContextID);

	//广播用户信息
	SendUserItem(pConnectItemInfo->pIServerUserItem,INVALID_WORD);

	//发送消息
	SendRoomMessage(dwContextID,TEXT("亲爱的用户，欢迎你多提宝贵建议！"),SMT_INFO);

	//练习判断
	if (m_pGameServiceOption->wServerType&GAME_GENRE_EDUCATE)
	{
		LPCTSTR pszMessage=TEXT("请注意，这是游戏练习房间，你将得到一定数目的模拟游戏币，游戏成绩将不会记录到数据库！");
		SendRoomMessage(dwContextID,pszMessage,SMT_INFO);
	}

	//比赛判断
	if (m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)
	{
		LPCTSTR pszMessage=TEXT("请注意，这是游戏比赛房间，非比赛选手将不能在此房间进行游戏！");
		SendRoomMessage(dwContextID,pszMessage,SMT_INFO);
	}

	//防作弊房间
	if (m_pGameServiceOption->cbHideUserInfo==TRUE)
	{
		LPCTSTR pszMessage=TEXT("请注意，这是防作弊游戏房间，请点击 [ 1 ] 号桌子进行进入游戏！");
		SendRoomMessage(dwContextID,pszMessage,SMT_INFO);
	}

	//发送消息
	if (m_bShallClose==true)
	{
		LPCTSTR pszMessage=TEXT("请注意，由于需要进行游戏房间维护更新工作，此游戏房间即将暂停服务！");
		SendRoomMessage(dwContextID,pszMessage,SMT_INFO);
	}

	//设置群发
	if (wIndex<INDEX_ANDROID) m_pITCPNetworkEngine->AllowBatchSend(dwContextID,true);

	return true;
}

//用户登录失败
bool CAttemperEngineSink::OnDBLogonError(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_LogonError));
	if (wDataSize!=sizeof(DBR_GR_LogonError)) return false;

	//判断在线
	tagConnectItemInfo * pConnectItemInfo=GetBindParameter(LOWORD(dwContextID));
	if (pConnectItemInfo==NULL) return true;
	if (pConnectItemInfo->dwSocketID!=dwContextID) return true;

	//发送消息
	DBR_GR_LogonError * pLogonError=(DBR_GR_LogonError *)pData;
	pLogonError->szErrorDescribe[CountStringBuffer(pLogonError->szErrorDescribe)-1]=0;
	SendLogonFailed(dwContextID,pLogonError->szErrorDescribe,pLogonError->lErrorCode);

	//关闭网络
	if (LOWORD(dwContextID)>=INDEX_ANDROID)
	{
		m_AndroidUserManager.DeleteAndroidUserItem(dwContextID);
	}
	else
	{
		m_pITCPNetworkEngine->ShutDownSocket(dwContextID);
	}

	return true;
}

//用户信息
bool CAttemperEngineSink::OnDBAndroidUser(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//变量定义
	DBO_GR_AndroidUser * pAndroidUser=(DBO_GR_AndroidUser *)pData;
	WORD wHeadSize=sizeof(DBO_GR_AndroidUser)-sizeof(pAndroidUser->dwAndroidID);

	//效验参数
	ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pAndroidUser->wAndroidCount*sizeof(DWORD))));
	if ((wDataSize<wHeadSize)||(wDataSize!=(wHeadSize+pAndroidUser->wAndroidCount*sizeof(DWORD)))) return false;

	//设置用户
	if (pAndroidUser->lResultCode==0)
	{
		m_AndroidUserManager.SetAndroidStock(pAndroidUser->dwAndroidID,pAndroidUser->wAndroidCount);
	}

	//设置时间
	m_pITimerEngine->SetTimer(IDI_LOAD_ANDROID_INFO,TIME_LOAD_ANDROID_INFO*1000L,1,NULL);

	return true;
}

//用户权限
bool CAttemperEngineSink::OnDBUserRightResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_UserRightResult));
	if (wDataSize!=sizeof(DBR_GR_UserRightResult)) return false;

	//消息处理
	DBR_GR_UserRightResult * pUserRightResult=(DBR_GR_UserRightResult *)pData;

	//获取玩家
	IServerUserItem * pSendIServerUserItem=m_ServerUserManager.SearchOnLineUser(pUserRightResult->dwMasterUserID);
	IServerUserItem * pRcvIServerUserItem=m_ServerUserManager.SearchOnLineUser(pUserRightResult->dwUserID);
	if(pSendIServerUserItem==NULL || pRcvIServerUserItem==NULL)return false;
	tagServerUserData * pSendUserData=pSendIServerUserItem->GetUserData();

	//成功判断
	if (pUserRightResult->bGameSuccess || pUserRightResult->bAccountsSuccess)
	{
		//发送管理员
		WORD wEnumIndex=0;
		IServerUserItem * pIServerUserItemSend=NULL;
		while (true)
		{
			pIServerUserItemSend=m_ServerUserManager.EnumOnLineUser(wEnumIndex++);
			if (pIServerUserItemSend==NULL) break;
			if (pIServerUserItemSend->GetUserData()->cbMasterOrder==FALSE) continue;

			//构造数据
			CMD_GR_UserRight UserRight;
			memset(&UserRight,0,sizeof(UserRight));
			UserRight.dwUserRight = pUserRightResult->dwUserRight;
			UserRight.dwUserID = pRcvIServerUserItem->GetUserData()->dwUserID;

			//发送数据
			SendData(pIServerUserItemSend,MDM_GR_USER,SUB_GR_USER_RIGHT,&UserRight,sizeof(UserRight));
		}
	}
	else
	{
		//发送消息
		TCHAR szDescribe[128]=TEXT("");
		_snprintf(szDescribe,sizeof(szDescribe),TEXT("设置用户权限失败！"));
		SendGameMessage(pSendIServerUserItem,szDescribe,SMT_EJECT|SMT_INFO);		
		return true;
	}

	return true;
}

//加载道具
bool CAttemperEngineSink::OnDBLoadProperty(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_PropertyAttribute));
	if (wDataSize!=sizeof(DBR_GR_PropertyAttribute)) return false;

	//消息处理
	DBR_GR_PropertyAttribute * pPropertyAttribute=(DBR_GR_PropertyAttribute *)pData;

	tagPropertyInfo PropertyAttribute;
	ZeroMemory(&PropertyAttribute, sizeof(PropertyAttribute));
	PropertyAttribute.bNullity = false;
	PropertyAttribute.nPropertyID = pPropertyAttribute->wPropertyID;
	PropertyAttribute.cbDiscount = pPropertyAttribute->cbDiscount;
	PropertyAttribute.dwPropCount1 = pPropertyAttribute->dwPropCount1;
	PropertyAttribute.dwPropCount2 = pPropertyAttribute->dwPropCount2;
	PropertyAttribute.dwPropCount3 = pPropertyAttribute->dwPropCount3;
	PropertyAttribute.dwPropCount4 = pPropertyAttribute->dwPropCount4;
	PropertyAttribute.dwPropCount5 = pPropertyAttribute->dwPropCount5;
	PropertyAttribute.dwPropCount6 = pPropertyAttribute->dwPropCount6;
	PropertyAttribute.dwPropCount7 = pPropertyAttribute->dwPropCount7;
	PropertyAttribute.dwPropCount8 = pPropertyAttribute->dwPropCount8;
	PropertyAttribute.dwPropCount9 = pPropertyAttribute->dwPropCount9;
	PropertyAttribute.dwPropCount10 = pPropertyAttribute->dwPropCount10;

	PropertyAttribute.lPrice1 = pPropertyAttribute->lPrice1;
	PropertyAttribute.lPrice2 = pPropertyAttribute->lPrice2;
	PropertyAttribute.lPrice3 = pPropertyAttribute->lPrice3;
	PropertyAttribute.lPrice4 = pPropertyAttribute->lPrice4;
	PropertyAttribute.lPrice5 = pPropertyAttribute->lPrice5;
	PropertyAttribute.lPrice6 = pPropertyAttribute->lPrice6;
	PropertyAttribute.lPrice7 = pPropertyAttribute->lPrice7;
	PropertyAttribute.lPrice8 = pPropertyAttribute->lPrice8;
	PropertyAttribute.lPrice9 = pPropertyAttribute->lPrice9;
	PropertyAttribute.lPrice10 = pPropertyAttribute->lPrice10;

	//保存数据
	m_PropertyAttributeArrary.Add(PropertyAttribute);
	if(m_PropertyAttributeArrary.GetCount()==11)
	{
		UINT nCOunt = 0;
	}

	return true;
}

//加载鲜花
bool CAttemperEngineSink::OnDBLoadFlower(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_FlowerAttribute));
	if (wDataSize!=sizeof(DBR_GR_FlowerAttribute)) return false;

	//消息处理
	DBR_GR_FlowerAttribute * pFlowerAttribute=(DBR_GR_FlowerAttribute *)pData;

	tagFlowerInfo FlowerInfo;
	ZeroMemory(&FlowerInfo, sizeof(FlowerInfo));
	FlowerInfo.bNullity = false;
	FlowerInfo.cbDiscount = pFlowerAttribute->cbDiscount;
	FlowerInfo.lPrice = pFlowerAttribute->lPrice;
	FlowerInfo.lRcvUserCharm = pFlowerAttribute->lRcvUserCharm;
	FlowerInfo.lSendUserCharm = pFlowerAttribute->lSendUserCharm;
	FlowerInfo.nFlowerID = pFlowerAttribute->nFlowerID;

	m_FlowerAttributeArrary.Add(FlowerInfo);

	return true;
}

//玩家道具
bool CAttemperEngineSink::OnDBUserProperty(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_UserProperty));
	if (wDataSize!=sizeof(DBR_GR_UserProperty)) return false;

	//消息处理
	DBR_GR_UserProperty * pUserProperty=(DBR_GR_UserProperty *)pData;

	int nPropertyID = pUserProperty->nPropertyID;
	ASSERT(nPropertyID < PROPERTY_COUNT );
	if ( nPropertyID >= PROPERTY_COUNT ) return true;

	//搜索玩家
	IServerUserItem *pServerUserItem = m_ServerUserManager.SearchOnLineUser(pUserProperty->dwUserID);
	if ( pServerUserItem == NULL ) pServerUserItem = m_ServerUserManager.SearchOffLineUser(pUserProperty->dwUserID);
	if ( pServerUserItem == NULL ) return true;

	tagServerUserData *pServerUserData = pServerUserItem->GetUserData();

	pServerUserData->PropertyInfo[nPropertyID].dwConsumedCount = 0;
	pServerUserData->PropertyInfo[nPropertyID].nPropertyID = pUserProperty->nPropertyID;
	pServerUserData->PropertyInfo[nPropertyID].dwPropCount= pUserProperty->dwCurCount;

	return true;
}

////兑换结果
//bool CAttemperEngineSink::OnDBExchangeCharmResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
//{
//	//效验参数
//	ASSERT(wDataSize==sizeof(DBR_GR_ExchangeCharmResult));
//	if (wDataSize!=sizeof(DBR_GR_ExchangeCharmResult)) return false;
//
//	//消息处理
//	DBR_GR_ExchangeCharmResult * pExchangeCharmResult=(DBR_GR_ExchangeCharmResult *)pData;
//
//	IServerUserItem *pServerUserItem = m_ServerUserManager.SearchOnLineUser(pExchangeCharmResult->dwUserID);
//	
//	//更新通知
//	if ( pExchangeCharmResult->cbResultCode == 0 )
//	{
//		if ( pServerUserItem == NULL ) pServerUserItem = m_ServerUserManager.SearchOffLineUser(pExchangeCharmResult->dwUserID);
//
//		if ( pServerUserItem != NULL )
//		{
//			tagServerUserData *pServerUserData = pServerUserItem->GetUserData();
//
//			ASSERT(pExchangeCharmResult->lLoveliness<= pServerUserData->lLoveliness);
//			if ( pExchangeCharmResult->lLoveliness > pServerUserData->lLoveliness) return true;
//
//			//修改魅力
//			pServerUserData->lLoveliness -= pExchangeCharmResult->lLoveliness;
//
//			//修改帐款
//			ModifyBankStorageGold(pServerUserItem, pExchangeCharmResult->lLoveliness * CHARM_EXCHANGE_RATE);
//
//			//系统消息
//			TCHAR szMessage[] = TEXT("恭喜你，兑换成功");
//			SendGameMessage(pServerUserItem, szMessage, SMT_EJECT|SMT_INFO);
//		}
//	}
//	else
//	{
//		//系统消息
//		TCHAR szMessage[] = TEXT("兑换失败");
//		SendGameMessage(pServerUserItem, szMessage, SMT_EJECT|SMT_INFO);
//	}
//
//	return true;
//}

//完成加载
bool CAttemperEngineSink::OnDBLadPropFinish(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	tagConnectItemInfo * pConnectItemInfo=GetBindParameter(LOWORD(dwContextID));
	if (pConnectItemInfo==NULL) return true;

	//效验参数
	ASSERT(pConnectItemInfo->pIServerUserItem!=NULL);
	if ( pConnectItemInfo->pIServerUserItem==NULL) return true;

	//发送道具
	SendResidualProperty(pConnectItemInfo->pIServerUserItem);

	return true;
}

//网络应答事件
bool __cdecl CAttemperEngineSink::OnEventTCPNetworkBind(DWORD dwClientIP, DWORD dwSocketID)
{
	//变量定义
	WORD wBindIndex=LOWORD(dwSocketID);
	tagConnectItemInfo * pConnectItemInfo=GetBindParameter(wBindIndex);

	//设置变量
	pConnectItemInfo->dwSocketID=dwSocketID;
	pConnectItemInfo->dwClientIP=dwClientIP;

	return true;
}

//网络事件
bool CAttemperEngineSink::OnEventTCPNetworkRead(CMD_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (Command.wMainCmdID)
	{
	case MDM_GR_LOGON:	//登录消息
		{
			return OnSocketMainLogon(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GR_USER:	//用户消息
		{
			return OnSocketMainUser(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GR_MANAGER://管理消息
		{
			return OnSocketMainManager(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GF_FRAME:	//框架消息
		{
			return OnSocketMainFrame(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GF_GAME:	//游戏消息
		{
			return OnSocketMainGame(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GF_VIDEO:	//视频消息
		{
			return OnSocketMainVideo(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GF_PRESENT://礼物消息
		{
			return OnSocketPresent(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GF_BANK:	//银行消息
		{
			return OnSocketBank(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	}

	return false;
}

//网络关闭事件
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientIP, DWORD dwActiveTime, DWORD dwSocketID)
{
	//用户处理
	tagConnectItemInfo * pConnectItemInfo=GetBindParameter(LOWORD(dwSocketID));
	if (pConnectItemInfo==NULL) return true;
	IServerUserItem * pIServerUserItem=pConnectItemInfo->pIServerUserItem;
	if (pIServerUserItem!=NULL)
	{
		//判断游戏
		bool bOffLine=false;
		if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
		{
			//变量定义
			CTableFrame * pTableFrame=(m_pTableFrame+pIServerUserItem->GetTableID());

			//断线注册
			WORD wChairID=pIServerUserItem->GetChairID();
			if ((pTableFrame->IsUserPlaying(pIServerUserItem)==true)&&(pTableFrame->OnUserReqOffLine(wChairID)==true))
			{
				bOffLine=m_ServerUserManager.RegOffLineUserItem(pIServerUserItem);
			}

			//断线处理
			if (bOffLine==true) pTableFrame->OnUserOffLine(wChairID);
			else pTableFrame->PerformStandUpAction(pIServerUserItem);
		}

		//删除用户
		if (bOffLine==false) DeleteUserItem(pIServerUserItem);
	}

	//设置数据
	ZeroMemory(pConnectItemInfo,sizeof(tagConnectItemInfo));

	return true;
}

//中心连接事件
bool __cdecl CAttemperEngineSink::OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode)
{
	//错误判断
	if (nErrorCode!=0)
	{
		//设置时间
		m_pITimerEngine->SetTimer(IDI_CONNECT_CENTER_SERVER,TIME_RECONNECT,1,NULL);

		//错误提示
		CTraceService::TraceString(TEXT("中心服务器连接失败，稍后将会再次尝试...."),TraceLevel_Warning);

		return true;
	}
	else
	{
		//连上提示
		CTraceService::TraceString(TEXT("中心服务器连接成功"),TraceLevel_Normal);
	}

	//注册房间
	CMD_CS_RegGameServer RegGameServer;
	memset(&RegGameServer,0,sizeof(RegGameServer));
	RegGameServer.GameServer.wSortID=m_pGameServiceOption->wSortID;
	RegGameServer.GameServer.wKindID=m_pGameServiceOption->wKindID;
	RegGameServer.GameServer.wServerID=m_pGameServiceOption->wServerID;
	RegGameServer.GameServer.wStationID=m_pGameServiceOption->wNodeID;
	RegGameServer.GameServer.wServerPort=m_pGameServiceOption->wServerPort;
	RegGameServer.GameServer.dwServerAddr=m_pGameServiceOption->dwServerAddr;
	RegGameServer.GameServer.dwOnLineCount=(m_ServerUserManager.GetOnLineCount()+m_ServerUserManager.GetOffLineCount());
	lstrcpyn(RegGameServer.GameServer.szServerName,m_pGameServiceOption->szGameRoomName,CountArray(RegGameServer.GameServer.szServerName));
	m_pITCPSocketCorrespond->SendData(MDM_CS_SERVER_MANAGER,SUB_CS_REG_GAME_SERVER,&RegGameServer,sizeof(RegGameServer));

	//获取列表
	m_pITCPSocketCorrespond->SendData(MDM_CS_SERVER_LIST,SUB_CS_GET_SERVER_LIST,NULL,0);

	//设置定时器
	m_pITimerEngine->SetTimer(IDI_UPDATE_SERVER_LIST,TIME_UPDATE_LIST,TIMES_INFINITY,NULL);
	m_pITimerEngine->SetTimer(IDI_UPDATE_ONLINE_COUNT,TIME_UPDATE_ONLINE,TIMES_INFINITY,NULL);

	return true;
}

//中心读取事件
bool __cdecl CAttemperEngineSink::OnEventTCPSocketRead(WORD wServiceID, CMD_Command Command, VOID * pData, WORD wDataSize)
{
	switch (Command.wMainCmdID)
	{
	case MDM_CS_SERVER_LIST:	//列表消息
		{
			return OnCenterMainServerList(Command,pData,wDataSize);
		}
	}
	return true;
}

//中心关闭事件
bool __cdecl CAttemperEngineSink::OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason)
{
	//重新连接
	m_pITimerEngine->KillTimer(IDI_UPDATE_SERVER_LIST);
	m_pITimerEngine->KillTimer(IDI_UPDATE_ONLINE_COUNT);
	m_pITimerEngine->SetTimer(IDI_CONNECT_CENTER_SERVER,TIME_RECONNECT,1,NULL);

	//错误提示
	CTraceService::TraceString(TEXT("中心服务器连接中断，稍后将会再次尝试...."),TraceLevel_Warning);

	return true;
}

//登录消息处理
bool CAttemperEngineSink::OnSocketMainLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GR_LOGON_USERID:		//ID 登录
		{
			//效验参数
			ASSERT(wDataSize>=sizeof(CMD_GR_LogonByUserID));
			if (wDataSize<sizeof(CMD_GR_LogonByUserID)) return false;

			//处理消息
			CMD_GR_LogonByUserID * pLogonByUserID=(CMD_GR_LogonByUserID *)pData;
			pLogonByUserID->szPassWord[CountArray(pLogonByUserID->szPassWord)-1]=0;

			//重复判断
			tagConnectItemInfo * pConnectItemInfo=GetBindParameter(LOWORD(dwSocketID));
			if (pConnectItemInfo==NULL) return false;
			IServerUserItem * pIServerUserItem=pConnectItemInfo->pIServerUserItem;
			if (pIServerUserItem!=NULL) return false;

			//在线判断
			pIServerUserItem=m_ServerUserManager.SearchOnLineUser(pLogonByUserID->dwUserID);
			if (pIServerUserItem!=NULL)
			{
				//效验密码
				if (lstrcmp(pIServerUserItem->GetPassword(),pLogonByUserID->szPassWord)!=0)
				{
					SendLogonFailed(dwSocketID,TEXT("帐户密码有误，请查证后再次尝试登录！"),0);

					if (LOWORD(dwSocketID)>=INDEX_ANDROID)
					{
						m_AndroidUserManager.DeleteAndroidUserItem(dwSocketID);
					}
					else
					{
						m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);
					}

					return true;
				}

				//变量定义//修改
				WORD wSourceIndex=pIServerUserItem->GetUserIndex();
				tagConnectItemInfo * pConnectItemSource=GetBindParameter(wSourceIndex);
				tagConnectItemInfo * pConnectItemTarger=GetBindParameter(LOWORD(dwSocketID));

				//断开用户
				if (pConnectItemSource!=NULL)
				{
					//设置变量
					pConnectItemSource->pIServerUserItem=NULL;

					//构造消息
					LPCTSTR pszMessage=TEXT("请注意，你的帐号在另一地方进入此游戏房间了，你被迫离开！");
					SendRoomMessage(pConnectItemSource->dwSocketID,pszMessage,SMT_INFO|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_ROOM);

					//发生消息
					if (wSourceIndex>=INDEX_ANDROID)
					{
						m_AndroidUserManager.DeleteAndroidUserItem(pConnectItemSource->dwSocketID);
					}
					else
					{
						m_pITCPNetworkEngine->ShutDownSocket(pConnectItemSource->dwSocketID);
					}
				}

				//激活用户
				pConnectItemTarger->pIServerUserItem=pIServerUserItem;
				m_ServerUserManager.SwitchOnLineUserItem(pIServerUserItem,pConnectItemTarger->dwClientIP,LOWORD(dwSocketID));

				//发送房间信息
				SendGameServerInfo(pIServerUserItem,dwSocketID);

				//设置群发
				if (LOWORD(dwSocketID)<INDEX_ANDROID)
				{
					m_pITCPNetworkEngine->AllowBatchSend(dwSocketID,true);
				}

				return true;
			}

			//断线判断
			pIServerUserItem=m_ServerUserManager.SearchOffLineUser(pLogonByUserID->dwUserID);
			if (pIServerUserItem!=NULL)
			{
				//效验密码
				if (lstrcmp(pIServerUserItem->GetPassword(),pLogonByUserID->szPassWord)!=0)
				{
					SendLogonFailed(dwSocketID,TEXT("帐户密码有误，请查证后再次尝试登录！"),0);

					if (LOWORD(dwSocketID)>=INDEX_ANDROID)
					{
						m_AndroidUserManager.DeleteAndroidUserItem(dwSocketID);
					}
					else
					{
						m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);
					}

					return true;
				}

				//设置用户
				pConnectItemInfo->pIServerUserItem=pIServerUserItem;
				m_ServerUserManager.ActiveOffLineUserItem(pIServerUserItem,pConnectItemInfo->dwClientIP,LOWORD(dwSocketID));

				//断线重入
				WORD wTableID=pIServerUserItem->GetTableID();
				WORD wChairID=pIServerUserItem->GetChairID();
				(m_pTableFrame+wTableID)->OnUserReConnect(wChairID);

				//发送房间信息
				SendGameServerInfo(pIServerUserItem,dwSocketID);

				//设置群发
				if (LOWORD(dwSocketID)<INDEX_ANDROID)
				{
					m_pITCPNetworkEngine->AllowBatchSend(dwSocketID,true);
				}

				return true;
			}

			//构造数据
			DBR_GR_LogonByUserID LogonByUserID;
			memset(&LogonByUserID,0,sizeof(LogonByUserID));
			LogonByUserID.dwUserID=pLogonByUserID->dwUserID;
			LogonByUserID.dwClientIP=pConnectItemInfo->dwClientIP;
			lstrcpyn(LogonByUserID.szPassWord,pLogonByUserID->szPassWord,sizeof(LogonByUserID.szPassWord));

			//变量定义
			VOID * pData=NULL;
			tagDataDescribe DataDescribe;
			CRecvPacketHelper RecvPacket(pLogonByUserID+1,wDataSize-sizeof(CMD_GR_LogonByUserID));

			//扩展信息
			while (true)
			{
				//提取数据
				pData=RecvPacket.GetData(DataDescribe);
				if (DataDescribe.wDataDescribe==DTP_NULL) break;

				//数据分析
				switch (DataDescribe.wDataDescribe)
				{
				case DTP_COMPUTER_ID:		//机器标识
					{
						ASSERT(pData!=NULL);
						ASSERT(DataDescribe.wDataSize==sizeof(tagClientSerial));
						if (DataDescribe.wDataSize==sizeof(tagClientSerial))
						{
							BYTE * pcbByte=(BYTE *)pData;
							for (INT i=0;i<sizeof(tagClientSerial)/sizeof(BYTE);i++)
							{
								ASSERT(CountArray(LogonByUserID.szComputerID)>((i*2)+1));
								_stprintf(&LogonByUserID.szComputerID[i*2],TEXT("%02X"),*(pcbByte+i));
							}
						}

						break;
					}
				}
			}

			//投递数据库
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOGON_BY_USERID,dwSocketID,&LogonByUserID,sizeof(LogonByUserID));

			return true;
		}
	}

	return false;
}

//用户消息处理
bool CAttemperEngineSink::OnSocketMainUser(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GR_USER_SIT_REQ:		//坐下桌子
	case SUB_GR_USER_LOOKON_REQ:	//旁观桌子
		{
			//变量定义
			CMD_GR_UserSitReq * pUserSitReq=(CMD_GR_UserSitReq *)pData;
			WORD wHeadSize=sizeof(CMD_GR_UserSitReq)-sizeof(pUserSitReq->szTablePass);

			//效验数据
			ASSERT((wDataSize>=wHeadSize)&&(wDataSize<=sizeof(CMD_GR_UserSitReq)));
			if ((wDataSize<wHeadSize)||(wDataSize>sizeof(CMD_GR_UserSitReq))) return false;
			ASSERT((wHeadSize+pUserSitReq->cbPassLen)==wDataSize);
			if ((wHeadSize+pUserSitReq->cbPassLen)!=wDataSize) return false;

			//判断位置
			if ((pUserSitReq->wChairID>=m_pGameServiceAttrib->wChairCount)&&(pUserSitReq->wChairID!=INVALID_CHAIR)) return false;
			if ((pUserSitReq->wTableID>=m_pGameServiceOption->wTableCount)&&(pUserSitReq->wTableID!=INVALID_TABLE)) return false;

			//获取用户
			IServerUserItem * pIServerUserItem=GetServerUserItem(LOWORD(dwSocketID));
			if (pIServerUserItem==NULL) return false;
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();

			//随机位置
			if ((wSubCmdID==SUB_GR_USER_SIT_REQ)&&(m_pGameServiceOption->cbHideUserInfo==TRUE))
			{
				//离开位置
				if (pUserData->wTableID!=INVALID_TABLE)
				{
					CTableFrame * pTableFrame=(m_pTableFrame+pUserData->wTableID);
					if (pTableFrame->IsUserPlaying(pIServerUserItem)==true)
					{
						//构造数据
						CMD_GR_SitFailed SitFailed;
						memset(&SitFailed,0,sizeof(SitFailed));
						_snprintf(SitFailed.szFailedDescribe,sizeof(SitFailed.szFailedDescribe),TEXT("你正在的 [ %d ] 号游戏桌游戏，暂时不能离开位置！"),pUserData->wTableID+1);

						//发送数据
						WORD wSendSize=sizeof(SitFailed)-sizeof(SitFailed.szFailedDescribe)+CountStringBuffer(SitFailed.szFailedDescribe);
						SendData(pIServerUserItem,MDM_GR_USER,SUB_GR_SIT_FAILED,&SitFailed,wSendSize);

						return true;
					}
					pTableFrame->PerformStandUpAction(pIServerUserItem);
				}

				//坐下位置
				for (INT_PTR i=1;i<m_pGameServiceOption->wTableCount;i++)
				{
					//获取桌子
					CTableFrame * pTableFrame=(m_pTableFrame+i);
					if (pTableFrame->IsGameStarted()==true) continue;

					//坐下处理
					WORD wChairID=pTableFrame->GetNullChairID();
					if (wChairID!=INVALID_CHAIR)
					{
						if (pTableFrame->PerformSitDownAction(wChairID,pIServerUserItem,NULL)==true) return true;
						return true;
					}
				}

				//构造数据
				CMD_GR_SitFailed SitFailed;
				ZeroMemory(&SitFailed,sizeof(SitFailed));
				_snprintf(SitFailed.szFailedDescribe,sizeof(SitFailed.szFailedDescribe),TEXT("暂时没有能够加入的游戏桌，请稍后！"));

				//发送数据
				WORD wSendSize=sizeof(SitFailed)-sizeof(SitFailed.szFailedDescribe)+CountStringBuffer(SitFailed.szFailedDescribe);
				SendData(pIServerUserItem,MDM_GR_USER,SUB_GR_SIT_FAILED,&SitFailed,wSendSize);

				return true;
			}

			//旁观设置
			if ((wSubCmdID==SUB_GR_USER_LOOKON_REQ)&&(m_pGameServiceOption->cbUnLookOnTag==TRUE))
			{
				//构造数据
				CMD_GR_SitFailed SitFailed;
				memset(&SitFailed,0,sizeof(SitFailed));
				_snprintf(SitFailed.szFailedDescribe,sizeof(SitFailed.szFailedDescribe),TEXT("本房间限制了旁观游戏!"),pUserData->wTableID+1);

				//发送数据
				WORD wSendSize=sizeof(SitFailed)-sizeof(SitFailed.szFailedDescribe)+CountStringBuffer(SitFailed.szFailedDescribe);
				SendData(pIServerUserItem,MDM_GR_USER,SUB_GR_SIT_FAILED,&SitFailed,wSendSize);

				return true;
			}

			//处理过虑
			if ((pUserData->wTableID==pUserSitReq->wTableID)&&(pUserData->wChairID==pUserSitReq->wChairID))
			{
				if ((pUserData->cbUserStatus!=US_LOOKON)&&(wSubCmdID==SUB_GR_USER_SIT_REQ)) return true;
				if ((pUserData->cbUserStatus==US_LOOKON)&&(wSubCmdID==SUB_GR_USER_LOOKON_REQ)) return true;
			}

			//离开位置
			if (pUserData->wTableID!=INVALID_TABLE)
			{
				CTableFrame * pTableFrame=(m_pTableFrame+pUserData->wTableID);
				if (pTableFrame->IsUserPlaying(pIServerUserItem)==true)
				{
					//构造数据
					CMD_GR_SitFailed SitFailed;
					memset(&SitFailed,0,sizeof(SitFailed));
					_snprintf(SitFailed.szFailedDescribe,sizeof(SitFailed.szFailedDescribe),TEXT("你正在的 [ %d ] 号游戏桌游戏，暂时不能离开位置！"),pUserData->wTableID+1);

					//发送数据
					WORD wSendSize=sizeof(SitFailed)-sizeof(SitFailed.szFailedDescribe)+CountStringBuffer(SitFailed.szFailedDescribe);
					SendData(pIServerUserItem,MDM_GR_USER,SUB_GR_SIT_FAILED,&SitFailed,wSendSize);

					return true;
				}
				pTableFrame->PerformStandUpAction(pIServerUserItem);
			}

			//坐下位置
			CTableFrame * pTableFrame=(m_pTableFrame+pUserSitReq->wTableID);
			if (wSubCmdID==SUB_GR_USER_SIT_REQ)	
			{
				pTableFrame->PerformSitDownAction(pUserSitReq->wChairID,pIServerUserItem,pUserSitReq->szTablePass);
			}
			else
			{
				pTableFrame->PerformLookonAction(pUserSitReq->wChairID,pIServerUserItem,pUserSitReq->szTablePass);
			}

			return true;
		}
	case SUB_GR_USER_STANDUP_REQ:	//起立请求
	case SUB_GR_USER_LEFT_GAME_REQ:	//强退请求
		{
			//获取用户
			IServerUserItem * pIServerUserItem=GetServerUserItem(LOWORD(dwSocketID));
			if (pIServerUserItem==NULL) return false;

			//处理过虑
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if ((pUserData->wTableID==INVALID_TABLE)||(pUserData->wChairID==INVALID_CHAIR)) return true;

			//起立位置
			CTableFrame * pTableFrame=(m_pTableFrame+pUserData->wTableID);
			if ((wSubCmdID==SUB_GR_USER_STANDUP_REQ)&&(pTableFrame->IsUserPlaying(pIServerUserItem)==true))
			{
				TCHAR szMessage[256]=TEXT("");
				_snprintf(szMessage,sizeof(szMessage),TEXT("您正在的 [ %d ] 号游戏桌游戏，暂时不能离开位置！"),pUserData->wTableID+1);
				SendRoomMessage(dwSocketID,szMessage,SMT_EJECT);
				return true;
			}

			//离开位置
			pTableFrame->PerformStandUpAction(pIServerUserItem);

			return true;
		}
	case SUB_GR_USER_CHAT:			//聊天消息
		{
			//效验参数
			CMD_GR_UserChat * pUserChat=(CMD_GR_UserChat *)pData;
			ASSERT(wDataSize>=(sizeof(CMD_GR_UserChat)-sizeof(pUserChat->szChatMessage)));
			ASSERT(wDataSize==(sizeof(CMD_GR_UserChat)-sizeof(pUserChat->szChatMessage)+pUserChat->wChatLength));
			if (wDataSize<(sizeof(CMD_GR_UserChat)-sizeof(pUserChat->szChatMessage))) return false;
			if (wDataSize!=(sizeof(CMD_GR_UserChat)-sizeof(pUserChat->szChatMessage)+pUserChat->wChatLength)) return false;

			//获取用户
			IServerUserItem * pIServerUserItem=GetServerUserItem(LOWORD(dwSocketID));
			if (pIServerUserItem==NULL) return false;

			//寻找用户
			IServerUserItem * pIServerUserItemRecv=NULL;
			if (pUserChat->dwTargetUserID!=0) 
			{
				pIServerUserItemRecv=m_ServerUserManager.SearchOnLineUser(pUserChat->dwTargetUserID);
				if (pIServerUserItemRecv==NULL) return true;
			}

			//状态判断
			if (m_bAllowRoomChat==false)
			{
				LPCTSTR pszMessage=TEXT("抱歉，本游戏房间不允许进行大厅公聊！");
				SendRoomMessage(pIServerUserItem,pszMessage,SMT_EJECT|SMT_INFO);
				return true;
			}

			//权限判断
			if (CUserRight::CanRoomChat(pIServerUserItem->GetUserData()->dwUserRight)==false)
			{
				LPCTSTR pszMessage=TEXT("抱歉，你没有大厅公聊的权限，若需要帮助，请联系游戏客服咨询！");
				SendRoomMessage(pIServerUserItem,pszMessage,SMT_EJECT|SMT_INFO);
				return true;
			}

			//转发消息
			m_pITCPNetworkEngine->SendDataBatch(MDM_GR_USER,SUB_GR_USER_CHAT,pUserChat,wDataSize);
			m_AndroidUserManager.SendDataToClient(MDM_GR_USER,SUB_GR_USER_CHAT,pUserChat,wDataSize);

			return true;
		}
	case SUB_GR_USER_WISPER:		//私语消息
		{
			//效验参数
			CMD_GR_Wisper * pWisper=(CMD_GR_Wisper *)pData;
			ASSERT(wDataSize>=(sizeof(CMD_GR_Wisper)-sizeof(pWisper->szChatMessage)));
			ASSERT(wDataSize==(sizeof(CMD_GR_Wisper)-sizeof(pWisper->szChatMessage)+pWisper->wChatLength));
			if (wDataSize<(sizeof(CMD_GR_Wisper)-sizeof(pWisper->szChatMessage))) return false;
			if (wDataSize!=(sizeof(CMD_GR_Wisper)-sizeof(pWisper->szChatMessage)+pWisper->wChatLength)) return false;

			//获取用户
			IServerUserItem * pIServerUserItemSend=GetServerUserItem(LOWORD(dwSocketID));
			if (pIServerUserItemSend==NULL) return false;
			IServerUserItem * pIServerUserItemRecv=m_ServerUserManager.SearchOnLineUser(pWisper->dwTargetUserID);
			if (pIServerUserItemRecv==NULL) return true;

			//状态判断
			DWORD dwMasterRightSend=pIServerUserItemSend->GetUserData()->dwMasterRight;
			DWORD dwMasterRightRecv=pIServerUserItemRecv->GetUserData()->dwMasterRight;
			if ((m_bAllowWisper==false)&&(dwMasterRightSend==0L)&&(dwMasterRightRecv==0L))
			{
				LPCTSTR pszMessage=TEXT("抱歉，本游戏房间不能进行玩家私聊！");
				SendRoomMessage(pIServerUserItemSend,pszMessage,SMT_EJECT|SMT_INFO);
				return true;
			}

			//权限判断
			DWORD dwUserRightSend=pIServerUserItemSend->GetUserData()->dwUserRight;
			if ((dwMasterRightRecv==0L)&&(CUserRight::CanWisper(dwUserRightSend)==false))
			{
				TCHAR szMessage[256]=TEXT("你暂时没有向玩家发送私聊消息的权限，只能与管理员私聊！");
				SendRoomMessage(dwSocketID,szMessage,SMT_EJECT);
				return true;
			}

			//转发消息
			SendData(pIServerUserItemSend,MDM_GR_USER,SUB_GR_USER_WISPER,pWisper,wDataSize);
			SendData(pIServerUserItemRecv,MDM_GR_USER,SUB_GR_USER_WISPER,pWisper,wDataSize);

			return true;
		}
	case SUB_GR_USER_RULE:		//用户规则
		{
			//效验参数
			ASSERT(wDataSize==sizeof(CMD_GR_UserRule));
			if (wDataSize!=sizeof(CMD_GR_UserRule)) return false;

			//消息处理
			CMD_GR_UserRule * pUserRule=(CMD_GR_UserRule *)pData;
			pUserRule->szPassword[CountArray(pUserRule->szPassword)-1]=0;

			//获取用户
			IServerUserItem * pIServerUserItem=GetServerUserItem(LOWORD(dwSocketID));
			if (pIServerUserItem==NULL) return false;

			//构造规则
			tagUserRule UserRule;
			ZeroMemory(&UserRule,sizeof(UserRule));
			UserRule.bLimitWin=pUserRule->bLimitWin;
			UserRule.bLimitFlee=pUserRule->bLimitFlee;
			UserRule.bLimitScore=pUserRule->bLimitScore;
			UserRule.bCheckSameIP=pUserRule->bCheckSameIP;
			UserRule.wWinRate=pUserRule->wWinRate;
			UserRule.wFleeRate=pUserRule->wFleeRate;
			UserRule.lMaxScore=pUserRule->lMaxScore;
			UserRule.lLessScore=pUserRule->lLessScore;
			if(m_pGameServiceAttrib->wChairCount<=MAX_CHAIR_NORMAL)
			{
				UserRule.bPassword=pUserRule->bPassword;
				lstrcpyn(UserRule.szPassword,pUserRule->szPassword,CountArray(UserRule.szPassword));
			}

			//设置规则
			pIServerUserItem->SetUserRule(UserRule);

			return true;
		}

	case SUB_GR_USER_INVITE_REQ:		//邀请用户
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_GR_UserInviteReq));
			if (wDataSize!=sizeof(CMD_GR_UserInviteReq)) return false;

			//消息处理
			CMD_GR_UserInviteReq * pUserInviteReq=(CMD_GR_UserInviteReq *)pData;

			//获取用户
			IServerUserItem * pIServerUserItem=GetServerUserItem(LOWORD(dwSocketID));
			if (pIServerUserItem==NULL) return false;

			//效验状态
			if (pUserInviteReq->wTableID==INVALID_TABLE) return true;
			if (pIServerUserItem->GetTableID()!=pUserInviteReq->wTableID) return true;
			if (pIServerUserItem->GetUserStatus()==US_PLAY) return true;
			if (pIServerUserItem->GetUserStatus()==US_OFFLINE) return true;

			//目标用户
			IServerUserItem * pITargetUserItem=m_ServerUserManager.SearchOnLineUser(pUserInviteReq->dwUserID);
			if (pITargetUserItem==NULL) return true;
			if (pITargetUserItem->GetUserStatus()==US_PLAY) return true;

			//发送消息
			CMD_GR_UserInvite UserInvite;
			memset(&UserInvite,0,sizeof(UserInvite));
			UserInvite.wTableID=pUserInviteReq->wTableID;
			UserInvite.dwUserID=pIServerUserItem->GetUserID();
			SendData(pITargetUserItem,MDM_GR_USER,SUB_GR_USER_INVITE,&UserInvite,sizeof(UserInvite));

			return true;
		}
	}

	return true;
}

//管理消息处理
bool CAttemperEngineSink::OnSocketMainManager(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//消息处理
	switch (wSubCmdID)
	{
	case SUB_GR_SEND_WARNING:	//警告消息
		{
			//效验参数
			CMD_GR_SendWarning * pSendWarning=(CMD_GR_SendWarning *)pData;
			ASSERT(wDataSize>=(sizeof(CMD_GR_SendWarning)-sizeof(pSendWarning->szWarningMessage)));
			ASSERT(wDataSize==(sizeof(CMD_GR_SendWarning)-sizeof(pSendWarning->szWarningMessage)+pSendWarning->wChatLength));
			if (wDataSize<(sizeof(CMD_GR_SendWarning)-sizeof(pSendWarning->szWarningMessage))) return false;
			if (wDataSize!=(sizeof(CMD_GR_SendWarning)-sizeof(pSendWarning->szWarningMessage)+pSendWarning->wChatLength)) return false;

			//管理用户
			IServerUserItem * pIServerUserItemSend=GetServerUserItem(LOWORD(dwSocketID));
			if (pIServerUserItemSend==NULL) return false;

			//权限判断
			DWORD dwMasterRight=pIServerUserItemSend->GetUserData()->dwMasterRight;
			if (CUserRight::CanSendWarning(dwMasterRight)==false) return true;

			//目标用户
			IServerUserItem * pIServerUserItemRecv=m_ServerUserManager.SearchOnLineUser(pSendWarning->dwTargetUserID);
			if (pIServerUserItemRecv==NULL) return true;

			//发送消息
			SendGameMessage(pIServerUserItemRecv,pSendWarning->szWarningMessage,SMT_INFO);
			SendRoomMessage(pIServerUserItemRecv,pSendWarning->szWarningMessage,SMT_INFO|SMT_EJECT);

			return true;
		}
	case SUB_GR_LOOK_USER_IP:	//查看地址
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_GR_LookUserIP));
			if (wDataSize!=sizeof(CMD_GR_LookUserIP)) return false;

			//消息处理
			CMD_GR_LookUserIP * pLookUserIP=(CMD_GR_LookUserIP *)pData;

			//管理用户
			IServerUserItem * pIServerUserItemSend=GetServerUserItem(LOWORD(dwSocketID));
			if (pIServerUserItemSend==NULL) return false;

			//权限判断
			DWORD dwMasterRight=pIServerUserItemSend->GetUserData()->dwMasterRight;
			if (CUserRight::CanSeeUserIP(dwMasterRight)==false) return true;

			//目标用户
			IServerUserItem * pIServerUserItemRecv=m_ServerUserManager.SearchOnLineUser(pLookUserIP->dwTargetUserID);
			if (pIServerUserItemRecv==NULL) return true;

			//变量定义
			DWORD dwUserIP=pIServerUserItemRecv->GetClientIP();

			//构造信息
			TCHAR szMessage[128]=TEXT("");
			BYTE * pcbUserIP=(BYTE *)(&dwUserIP);
			_snprintf(szMessage,CountArray(szMessage),TEXT("[ %s ]：%d.%d.%d.%d"),pIServerUserItemRecv->GetAccounts(),
				pcbUserIP[0],pcbUserIP[1],pcbUserIP[2],pcbUserIP[3]);

			//发送消息
			SendRoomMessage(pIServerUserItemSend,szMessage,SMT_INFO);

			return true;
		}
	case SUB_GR_KILL_USER:		//踢出用户
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_GR_KillUser));
			if (wDataSize!=sizeof(CMD_GR_KillUser)) return false;

			//消息处理
			CMD_GR_KillUser * pKillUser=(CMD_GR_KillUser *)pData;

			//管理用户
			IServerUserItem * pIServerUserItemSend=GetServerUserItem(LOWORD(dwSocketID));
			if (pIServerUserItemSend==NULL) return false;

			//权限判断
			DWORD dwMasterRight=pIServerUserItemSend->GetUserData()->dwMasterRight;
			if (CUserRight::CanCutUser(dwMasterRight)==false) return true;

			//目标用户
			IServerUserItem * pIServerUserItemRecv=m_ServerUserManager.SearchOnLineUser(pKillUser->dwTargetUserID);
			if (pIServerUserItemRecv==NULL) return true;

			//发送消息
			LPCTSTR szMessage=TEXT("抱歉，您被管理员踢出房间，若有任何疑问，请联系游戏客服！");
			SendRoomMessage(pIServerUserItemRecv,szMessage,SMT_INFO|SMT_EJECT|SMT_INTERMIT_LINE);

			return true;
		}
	case SUB_GR_LIMIT_ACCOUNS:	//禁用帐户
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_GR_LimitAccounts));
			if (wDataSize!=sizeof(CMD_GR_LimitAccounts)) return false;

			//消息处理
			CMD_GR_LimitAccounts * pLimitAccounts=(CMD_GR_LimitAccounts *)pData;

			//管理用户
			IServerUserItem * pIServerUserItemSend=GetServerUserItem(LOWORD(dwSocketID));
			if (pIServerUserItemSend==NULL) return false;

			//权限判断
			DWORD dwMasterRight=pIServerUserItemSend->GetUserData()->dwMasterRight;
			if (CUserRight::CanForbidAccounts(dwMasterRight)==false) return true;

			//目标用户
			IServerUserItem * pIServerUserItemRecv=m_ServerUserManager.SearchOnLineUser(pLimitAccounts->dwTargetUserID);
			if (pIServerUserItemRecv==NULL) return true;

			//投递数据库
			DBR_GR_LimitAccounts LimitAccounts;
			LimitAccounts.dwUserID=pLimitAccounts->dwTargetUserID;
			LimitAccounts.dwMasterUserID=pIServerUserItemSend->GetUserID();
			LimitAccounts.dwMasterClientIP=pIServerUserItemSend->GetClientIP();
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LIMIT_ACCOUNTS,0,&LimitAccounts,sizeof(LimitAccounts));

			//发送消息
			LPCTSTR szMessage=TEXT("抱歉，您的帐号被冻结了，若有任何疑问，请联系游戏客服！");
			SendRoomMessage(pIServerUserItemRecv,szMessage,SMT_INFO|SMT_EJECT|SMT_INTERMIT_LINE);

			return true;
		}
	case SUB_GR_SET_USER_RIGHT:	//用户权限
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_GR_SetUserRight));
			if (wDataSize!=sizeof(CMD_GR_SetUserRight)) return false;

			//消息处理
			CMD_GR_SetUserRight * pSetUserRight=(CMD_GR_SetUserRight *)pData;

			//管理用户
			IServerUserItem * pIServerUserItemSend=GetServerUserItem(LOWORD(dwSocketID));
			if (pIServerUserItemSend==NULL) return false;

			//目标用户
			IServerUserItem * pIServerUserItemRecv=m_ServerUserManager.SearchOnLineUser(pSetUserRight->dwTargetUserID);
			if (pIServerUserItemRecv==NULL) return true;

			//权限判断
			DWORD dwMasterRight=pIServerUserItemSend->GetUserData()->dwMasterRight;
			if (dwMasterRight==0L) return true;

			//获取权限
			tagServerUserData * pUserData=pIServerUserItemRecv->GetUserData();

			//权限设置
			if (CUserRight::CanLimitPlay(dwMasterRight)==true)
			{
				if (pSetUserRight->cbLimitPlayGame==TRUE) pUserData->dwUserRight|=UR_CANNOT_PLAY;
				else pUserData->dwUserRight&=~UR_CANNOT_PLAY;
			}
			if (CUserRight::CanLimitLookon(dwMasterRight)==true)
			{
				if (pSetUserRight->cbLimitLookonGame==TRUE) pUserData->dwUserRight|=UR_CANNOT_LOOKON;
				else pUserData->dwUserRight&=~UR_CANNOT_LOOKON;
			}
			if (CUserRight::CanLimitWisper(dwMasterRight)==true)
			{
				if (pSetUserRight->cbLimitSendWisper==TRUE) pUserData->dwUserRight|=UR_CANNOT_WISPER;
				else pUserData->dwUserRight&=~UR_CANNOT_WISPER;
			}
			if (CUserRight::CanLimitRoomChat(dwMasterRight)==true)
			{
				if (pSetUserRight->cbLimitRoomChat==TRUE) pUserData->dwUserRight|=UR_CANNOT_ROOM_CHAT;
				else pUserData->dwUserRight&=~UR_CANNOT_ROOM_CHAT;
			}
			if (CUserRight::CanLimitGameChat(dwMasterRight)==true)
			{
				if (pSetUserRight->cbLimitGameChat==TRUE) pUserData->dwUserRight|=UR_CANNOT_GAME_CHAT;
				else pUserData->dwUserRight&=~UR_CANNOT_GAME_CHAT;
			}

			//设置权限
			DBR_GR_SetUserRight SetUserRight;
			SetUserRight.dwUserRight=pUserData->dwUserRight;
			SetUserRight.dwUserID=pSetUserRight->dwTargetUserID;
			SetUserRight.dwMasterUserID=pIServerUserItemSend->GetUserID();
			SetUserRight.dwMasterClientIP=pIServerUserItemSend->GetClientIP();
			SetUserRight.cbGame=(CUserRight::CanBindGame(dwMasterRight)==true)?TRUE:FALSE;
			SetUserRight.cbAccounts=(CUserRight::CanBindGlobal(dwMasterRight)==true)?TRUE:FALSE;

			//投递数据
			if ((SetUserRight.cbAccounts==TRUE)||(SetUserRight.cbGame==TRUE))
			{
				m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_SET_USER_RIGHT,0,&SetUserRight,sizeof(SetUserRight));
			}

			return true;
		}
	case SUB_GR_SEND_MESSAGE:	//发送消息
		{
			//效验参数
			CMD_GR_SendMessage * pSendMessage=(CMD_GR_SendMessage *)pData;
			ASSERT(wDataSize>=(sizeof(CMD_GR_SendMessage)-sizeof(pSendMessage->szSystemMessage)));
			ASSERT(wDataSize==(sizeof(CMD_GR_SendMessage)-sizeof(pSendMessage->szSystemMessage)+pSendMessage->wChatLength));
			if (wDataSize<(sizeof(CMD_GR_SendMessage)-sizeof(pSendMessage->szSystemMessage))) return false;
			if (wDataSize!=(sizeof(CMD_GR_SendMessage)-sizeof(pSendMessage->szSystemMessage)+pSendMessage->wChatLength)) return false;

			//管理用户
			IServerUserItem * pIServerUserItemSend=GetServerUserItem(LOWORD(dwSocketID));
			if (pIServerUserItemSend==NULL) return false;

			//权限判断
			DWORD dwMasterRight=pIServerUserItemSend->GetUserData()->dwMasterRight;
			if (CUserRight::CanIssueMessage(dwMasterRight)==false) return true;

			//发送消息
			if (pSendMessage->cbGame==TRUE) SendGameMessage(INVALID_WORD,pSendMessage->szSystemMessage,SMT_INFO);
			if (pSendMessage->cbRoom==TRUE) SendRoomMessage(INVALID_WORD,pSendMessage->szSystemMessage,SMT_INFO);

			return true;
		}
	case SUB_GR_OPTION_SERVER:	//房间设置
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_GR_OptionServer));
			if (wDataSize!=sizeof(CMD_GR_OptionServer)) return false;

			//消息处理
			CMD_GR_OptionServer * pOptionServer=(CMD_GR_OptionServer *)pData;

			//管理用户
			IServerUserItem * pIServerUserItemSend=GetServerUserItem(LOWORD(dwSocketID));
			if (pIServerUserItemSend==NULL) return false;

			//权限判断
			DWORD dwMasterRight=pIServerUserItemSend->GetUserData()->dwMasterRight;
			if (CUserRight::CanServerOption(dwMasterRight)==false) return true;

			//大厅聊天
			if (pOptionServer->cbOptionFlags==OSF_ROOM_CHAT)
			{
				m_bAllowRoomChat=(pOptionServer->cbOptionValue==TRUE)?true:false;
			}

			//游戏聊天
			if (pOptionServer->cbOptionFlags==OSF_GAME_CHAT)
			{
				m_bAllowGameChat=(pOptionServer->cbOptionValue==TRUE)?true:false;
			}

			//大厅私聊
			if (pOptionServer->cbOptionFlags==OSF_ROOM_WISPER)
			{
				m_bAllowWisper=(pOptionServer->cbOptionValue==TRUE)?true:false;
			}

			//进入游戏
			if (pOptionServer->cbOptionFlags==OSF_ENTER_GAME)
			{
				m_bAllowEnterGame=(pOptionServer->cbOptionValue==TRUE)?true:false;
			}

			//进入房间
			if (pOptionServer->cbOptionFlags==OSF_ENTER_ROOM)
			{
				m_bAllowEnterRoom=(pOptionServer->cbOptionValue==TRUE)?true:false;
			}

			//即将关闭
			if (pOptionServer->cbOptionFlags==OSF_SHALL_CLOSE)
			{
				m_bShallClose=(pOptionServer->cbOptionValue==TRUE)?true:false;
			}

			//发送消息
			LPCTSTR pszMessage=TEXT("服务器房间配置参数选项设置成功！");
			SendRoomMessage(pIServerUserItemSend,pszMessage,SMT_INFO);

			return true;
		}
	}

	return false;
}

//框架消息处理
bool CAttemperEngineSink::OnSocketMainFrame(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//获取用户
	IServerUserItem * pIServerUserItem=GetServerUserItem(LOWORD(dwSocketID));
	if (pIServerUserItem==NULL) return false;

	//处理过虑
	tagServerUserData * pUserData=pIServerUserItem->GetUserData();
	if ((pUserData->wTableID==INVALID_TABLE)||(pUserData->wChairID==INVALID_CHAIR)) return true;

	//消息处理 
	CTableFrame * pTableFrame=(m_pTableFrame+pUserData->wTableID);
	return pTableFrame->OnEventSocketFrame(wSubCmdID,pData,wDataSize,pIServerUserItem);
}

//游戏消息处理
bool CAttemperEngineSink::OnSocketMainGame(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//获取用户
	IServerUserItem * pIServerUserItem=GetServerUserItem(LOWORD(dwSocketID));
	if (pIServerUserItem==NULL) return false;

	//处理过虑
	tagServerUserData * pUserData=pIServerUserItem->GetUserData();
	if ((pUserData->wTableID==INVALID_TABLE)||(pUserData->wChairID==INVALID_CHAIR)) return true;

	//消息处理 
	CTableFrame * pTableFrame=(m_pTableFrame+pUserData->wTableID);
	return pTableFrame->OnEventSocketGame(wSubCmdID,pData,wDataSize,pIServerUserItem);
}

//视频消息处理
bool CAttemperEngineSink::OnSocketMainVideo(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//获取用户
	IServerUserItem * pIServerUserItem=GetServerUserItem(LOWORD(dwSocketID));
	if (pIServerUserItem==NULL) return false;

	//处理过虑
	tagServerUserData * pUserData=pIServerUserItem->GetUserData();
	if ((pUserData->wTableID==INVALID_TABLE)||(pUserData->wChairID==INVALID_CHAIR)) return true;

	//消息处理 
	CTableFrame * pTableFrame=(m_pTableFrame+pUserData->wTableID);
	return pTableFrame->OnEventSocketVideo(wSubCmdID,pData,wDataSize,pIServerUserItem);
}

//礼物消息处理
bool CAttemperEngineSink::OnSocketPresent(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//获取用户
	IServerUserItem * pIServerUserItem=GetServerUserItem(LOWORD(dwSocketID));
	if (pIServerUserItem==NULL) return false;

	//消息处理
	bool bReturnValue = false;
	switch ( wSubCmdID )
	{
	case SUB_GF_FLOWER:					//鲜花事件
		{
			CMD_GF_Gift * pTemp=(CMD_GF_Gift *)pData;
			m_cbSendLocation = pTemp->cbSendLocation;
			bReturnValue = OnEventFlower(pData,wDataSize,pIServerUserItem);
			break;
		}
	case SUB_GF_PROPERTY:				//道具事件
		{
			CMD_GF_Property *pTemp = (CMD_GF_Property*)pData;
			m_cbSendLocation = pTemp->cbSendLocation;
			bReturnValue = OnEventProperty(pData,wDataSize,pIServerUserItem);
			break;
		}
	case SUB_GF_EXCHANGE_CHARM:			//兑换事件
		{
			CMD_GF_ExchangeCharm *pTemp = (CMD_GF_ExchangeCharm*)pData;
			m_cbSendLocation = pTemp->cbSendLocation;
			bReturnValue = OnEventExchangeCharm(pData,wDataSize,pIServerUserItem);
			break;
		}
	case SUB_GF_PROP_BUGLE:				//喇叭事件
		{
			CMD_GF_BugleProperty *pTemp = (CMD_GF_BugleProperty*)pData;
			m_cbSendLocation = pTemp->cbSendLocation;
			bReturnValue = OnEventBugle(pData,wDataSize,pIServerUserItem);
			break;
		}
	}

	//初始场所
	m_cbSendLocation = 0;

	return bReturnValue;
}

//银行消息处理
bool CAttemperEngineSink::OnSocketBank(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//获取用户
	IServerUserItem * pIServerUserItem=GetServerUserItem(LOWORD(dwSocketID));
	if (pIServerUserItem==NULL) return false;

	//消息处理
	switch ( wSubCmdID )
	{
	case SUB_GF_BANK_GET:					//提取事件
		{
			return OnEventBankDrawoutGold(pData,wDataSize,dwSocketID);
		}
	case SUB_GF_BANK_STORAGE:				//存储事件
		{
			return OnEventBankStorage(pData,wDataSize,dwSocketID);
		}
	}

	return false;
}

//发送用户
bool CAttemperEngineSink::SendUserItem(IServerUserItem * pIServerUserItem, DWORD dwSocketID)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);

	//构造数据
	BYTE cbBuffer[SOCKET_PACKET];
	tagUserInfoHead * pUserInfoHead=(tagUserInfoHead *)cbBuffer;
	tagServerUserData * pUserData=pIServerUserItem->GetUserData();

	//填写数据
	memset(pUserInfoHead,0,sizeof(tagUserInfoHead));
	pUserInfoHead->wFaceID=pUserData->wFaceID;
	pUserInfoHead->dwCustomFaceVer=pUserData->dwCustomFaceVer;
	pUserInfoHead->wTableID=pUserData->wTableID;
	pUserInfoHead->wChairID=pUserData->wChairID;
	pUserInfoHead->cbGender=pUserData->cbGender;
	pUserInfoHead->cbUserStatus=pUserData->cbUserStatus;
	pUserInfoHead->dwUserID=pUserData->dwUserID;
	pUserInfoHead->dwGameID=pUserData->dwGameID;
	pUserInfoHead->dwGroupID=pUserData->dwGroupID;
	pUserInfoHead->dwUserRight=pUserData->dwUserRight;
	pUserInfoHead->lLoveliness=pUserData->lLoveliness;
	pUserInfoHead->dwMasterRight=pUserData->dwMasterRight;
	pUserInfoHead->cbGender=pUserData->cbGender;
	pUserInfoHead->cbMemberOrder=pUserData->cbMemberOrder;
	pUserInfoHead->cbMasterOrder=pUserData->cbMasterOrder;
	pUserInfoHead->UserScoreInfo=pUserData->UserScoreInfo;
	pUserInfoHead->UserScoreInfo.lScore=pUserData->UserScoreInfo.lScore+pUserData->lStorageScore;

	//叠加数据
	CSendPacketHelper SendPacket(cbBuffer+sizeof(tagUserInfoHead),sizeof(cbBuffer)-sizeof(tagUserInfoHead));
	SendPacket.AddPacket(pUserData->szAccounts,CountStringBuffer(pUserData->szAccounts),DTP_USER_ACCOUNTS);
	if (pUserData->szUnderWrite[0]!=0) SendPacket.AddPacket(pUserData->szUnderWrite,CountStringBuffer(pUserData->szUnderWrite),DTP_UNDER_WRITE);
	if (pUserData->szGroupName[0]!=0) SendPacket.AddPacket(pUserData->szGroupName,CountStringBuffer(pUserData->szGroupName),DTP_USER_GROUP_NAME);

	//发送数据
	WORD wSendLength=sizeof(tagUserInfoHead)+SendPacket.GetDataSize();
	if (LOWORD(dwSocketID)!=INVALID_WORD) 
	{
		if (LOWORD(dwSocketID)>=INDEX_ANDROID)
		{
			m_AndroidUserManager.SendDataToClient(dwSocketID,MDM_GR_USER,SUB_GR_USER_COME,cbBuffer,wSendLength);
		}
		else m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GR_USER,SUB_GR_USER_COME,cbBuffer,wSendLength);
	}
	else m_pITCPNetworkEngine->SendDataBatch(MDM_GR_USER,SUB_GR_USER_COME,cbBuffer,wSendLength);

	return true;
}

//发送信息
bool CAttemperEngineSink::SendGameServerInfo(IServerUserItem * pIServerUserItem, DWORD dwSocketID)
{
	//发送登录成功
	CMD_GR_LogonSuccess LogonSuccess;
	memset(&LogonSuccess,0,sizeof(LogonSuccess));
	LogonSuccess.dwUserID=pIServerUserItem->GetUserID();

	if (LOWORD(dwSocketID)>=INDEX_ANDROID)
	{
		m_AndroidUserManager.SendDataToClient(dwSocketID,MDM_GR_LOGON,SUB_GR_LOGON_SUCCESS,&LogonSuccess,sizeof(LogonSuccess));
	}
	else m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GR_LOGON,SUB_GR_LOGON_SUCCESS,&LogonSuccess,sizeof(LogonSuccess));

	//发送配置信息
	BYTE cbBuffer[SOCKET_PACKET];
	CMD_GR_ServerInfo * pServerInfo=(CMD_GR_ServerInfo *)cbBuffer;
	memset(pServerInfo,0,sizeof(CMD_GR_ServerInfo));
	pServerInfo->wKindID=m_pGameServiceAttrib->wKindID;
	pServerInfo->wGameGenre=m_pGameServiceOption->wServerType;
	pServerInfo->wTableCount=m_pGameServiceOption->wTableCount;
	pServerInfo->wChairCount=m_pGameServiceAttrib->wChairCount;
	pServerInfo->dwVideoAddr=m_pGameServiceOption->dwVideoServer;
	pServerInfo->cbHideUserInfo=m_pGameServiceOption->cbHideUserInfo;

	if (LOWORD(dwSocketID)>=INDEX_ANDROID)
	{
		m_AndroidUserManager.SendDataToClient(dwSocketID,MDM_GR_INFO,SUB_GR_SERVER_INFO,cbBuffer,sizeof(CMD_GR_ServerInfo));
	}
	else m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GR_INFO,SUB_GR_SERVER_INFO,cbBuffer,sizeof(CMD_GR_ServerInfo));

	//发送列表解释
	if (LOWORD(dwSocketID)>=INDEX_ANDROID)
	{
		m_AndroidUserManager.SendDataToClient(dwSocketID,MDM_GR_INFO,SUB_GR_COLUMN_INFO,&m_ColumnInfo,m_wColumnInfoSize);
	}
	else m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GR_INFO,SUB_GR_COLUMN_INFO,&m_ColumnInfo,m_wColumnInfoSize);

	//发送配置完成
	if (LOWORD(dwSocketID)>=INDEX_ANDROID)
	{
		m_AndroidUserManager.SendDataToClient(dwSocketID,MDM_GR_INFO,SUB_GR_CONFIG_FINISH,NULL,0);
	}
	else m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GR_INFO,SUB_GR_CONFIG_FINISH);

	//发送自己信息
	SendUserItem(pIServerUserItem,dwSocketID);

	//发送在线用户
	WORD wEnumIndex=0;
	IServerUserItem * pIServerUserItemSend=NULL;
	while (true)
	{
		pIServerUserItemSend=m_ServerUserManager.EnumOnLineUser(wEnumIndex++);
		if (pIServerUserItemSend==NULL) break;
		if (pIServerUserItemSend==pIServerUserItem) continue;
		SendUserItem(pIServerUserItemSend,dwSocketID);
	}

	//发送断线用户
	wEnumIndex=0;
	while (true)
	{
		pIServerUserItemSend=m_ServerUserManager.EnumOffLineUser(wEnumIndex++);
		if (pIServerUserItemSend==NULL) break;
		if (pIServerUserItemSend==pIServerUserItem) continue;
		SendUserItem(pIServerUserItemSend,dwSocketID);
	}

	//发送桌子信息
	CMD_GR_TableInfo TableInfo;
	TableInfo.wTableCount=m_pGameServiceOption->wTableCount;
	ASSERT(TableInfo.wTableCount<CountArray(TableInfo.TableStatus));
	for (WORD i=0;i<TableInfo.wTableCount;i++)
	{
		CTableFrame * pTableFrame=(m_pTableFrame+i);
		TableInfo.TableStatus[i].bTableLock=pTableFrame->IsTableLocked()?TRUE:FALSE;
		TableInfo.TableStatus[i].bPlayStatus=pTableFrame->IsGameStarted()?TRUE:FALSE;
	}
	WORD wSendSize=sizeof(TableInfo)-sizeof(TableInfo.TableStatus)+TableInfo.wTableCount*sizeof(TableInfo.TableStatus[0]);

	if (LOWORD(dwSocketID)>=INDEX_ANDROID)
	{
		m_AndroidUserManager.SendDataToClient(dwSocketID,MDM_GR_STATUS,SUB_GR_TABLE_INFO,&TableInfo,wSendSize);
	}
	else m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GR_STATUS,SUB_GR_TABLE_INFO,&TableInfo,wSendSize);

	//道具鲜花
	SendPresentAttribute(pIServerUserItem);

	//发送完成
	if (LOWORD(dwSocketID)>=INDEX_ANDROID)
	{
		m_AndroidUserManager.SendDataToClient(dwSocketID,MDM_GR_LOGON,SUB_GR_LOGON_FINISH,NULL,0);
	}
	else m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GR_LOGON,SUB_GR_LOGON_FINISH);

	return true;
}

//发送属性
void CAttemperEngineSink::SendPresentAttribute(IServerUserItem *pIServerUserItem)
{
	//内存变量
	WORD wSendSize=0L;
	BYTE cbBuffer[SOCKET_PACKET];

	//道具属性
	for (INT_PTR nIndex =0; nIndex < m_PropertyAttributeArrary.GetCount(); ++nIndex)
	{
		if ((wSendSize+sizeof(tagPropertyInfo))>sizeof(cbBuffer))
		{
			SendData(pIServerUserItem,MDM_GF_PRESENT,SUB_GF_PROP_ATTRIBUTE,cbBuffer,wSendSize);
			wSendSize=0;
		}
		tagPropertyInfo &PropertyInfo = m_PropertyAttributeArrary[nIndex];
		CopyMemory(cbBuffer+wSendSize,&PropertyInfo,sizeof(PropertyInfo));
		wSendSize+=sizeof(PropertyInfo);
	}
	if (wSendSize>0) SendData(pIServerUserItem,MDM_GF_PRESENT,SUB_GF_PROP_ATTRIBUTE,cbBuffer,wSendSize);

	//鲜花属性
	wSendSize=0L;
	for (INT_PTR nIndex =0; nIndex < m_FlowerAttributeArrary.GetCount(); ++nIndex)
	{
		if ((wSendSize+sizeof(tagFlowerInfo))>sizeof(cbBuffer))
		{
			SendData(pIServerUserItem,MDM_GF_PRESENT,SUB_GF_FLOWER_ATTRIBUTE,cbBuffer,wSendSize);
			wSendSize=0;
		}
		tagFlowerInfo &FlowerInfo = m_FlowerAttributeArrary[nIndex];
		CopyMemory(cbBuffer+wSendSize,&FlowerInfo,sizeof(FlowerInfo));
		wSendSize+=sizeof(FlowerInfo);
	}
	if (wSendSize>0) SendData(pIServerUserItem,MDM_GF_PRESENT,SUB_GF_FLOWER_ATTRIBUTE,cbBuffer,wSendSize);

}

//列表消息处理
bool CAttemperEngineSink::OnCenterMainServerList(CMD_Command Command, VOID * pData, WORD wDataSize)
{
	switch (Command.wSubCmdID)
	{
	case SUB_CS_LIST_INFO:		//列表信息
		{
			//效验参数
			ASSERT(wDataSize==sizeof(CMD_CS_ListInfo));
			if (wDataSize!=sizeof(CMD_CS_ListInfo)) return false;

			//消息处理
			m_ServerList.ResetServerListBuffer();

			return true;
		}
	case SUB_CS_LIST_TYPE:		//种类信息
		{
			//效验参数
			ASSERT(wDataSize>=sizeof(tagGameType));
			ASSERT(wDataSize%sizeof(tagGameType)==0);

			//消息处理
			DWORD dwCount=wDataSize/sizeof(tagGameType);
			tagGameType * pGameType=(tagGameType *)pData;
			m_ServerList.AppendGameTypeBuffer(pGameType,dwCount);

			return true;
		}
	case SUB_CS_LIST_KIND:		//类型信息
		{
			//效验参数
			ASSERT(wDataSize>=sizeof(tagGameKind));
			ASSERT(wDataSize%sizeof(tagGameKind)==0);

			//消息处理
			DWORD dwCount=wDataSize/sizeof(tagGameKind);
			tagGameKind * pGameKind=(tagGameKind *)pData;
			m_ServerList.AppendGameKindBuffer(pGameKind,dwCount);

			return true;
		}
	case SUB_CS_LIST_STATION:	//站点信息
		{
			//效验参数
			ASSERT(wDataSize>=sizeof(tagGameStation));
			ASSERT(wDataSize%sizeof(tagGameStation)==0);

			//消息处理
			DWORD dwCount=wDataSize/sizeof(tagGameStation);
			tagGameStation * pGameStation=(tagGameStation *)pData;
			m_ServerList.AppendGameStationBuffer(pGameStation,dwCount);

			return true;
		}
	case SUB_CS_LIST_SERVER:	//房间信息
		{
			//效验参数
			ASSERT(wDataSize>=sizeof(tagGameServer));
			ASSERT(wDataSize%sizeof(tagGameServer)==0);

			//消息处理
			DWORD dwCount=wDataSize/sizeof(tagGameServer);
			tagGameServer * pGameServer=(tagGameServer *)pData;
			m_ServerList.AppendGameServerBuffer(pGameServer,dwCount);

			return true;
		}
	case SUB_CS_LIST_FINISH:	//更新完成
		{
			//激活缓冲
			m_ServerList.ActiveServerListBuffer();

			//更新人数
			WORD wInfoCount=m_ServerList.GetOnLineCountInfo(m_OnLineCountInfo,CountArray(m_OnLineCountInfo));
			m_wOnLineInfoSize=wInfoCount*sizeof(tagOnLineCountInfo);

			//发送信息
			if ((m_wOnLineInfoSize>0)&&(m_ServerUserManager.GetOnLineCount()>0))
			{
				m_pITCPNetworkEngine->SendDataBatch(MDM_GR_SERVER_INFO,SUB_GR_ONLINE_COUNT_INFO,&m_OnLineCountInfo,m_wOnLineInfoSize);
				m_AndroidUserManager.SendDataToClient(MDM_GR_SERVER_INFO,SUB_GR_ONLINE_COUNT_INFO,&m_OnLineCountInfo,m_wOnLineInfoSize);
			}

			return true;
		}
	}

	return true;
}

//绑定参数
tagConnectItemInfo * CAttemperEngineSink::GetBindParameter(WORD wBindIndex)
{
	//无效连接
	if (wBindIndex==INVALID_WORD) return NULL;

	//常规连接
	if (wBindIndex<m_pGameServiceOption->wMaxConnect)
	{
		return m_pNormalParameter+wBindIndex;
	}

	//机器连接
	if ((wBindIndex>=INDEX_ANDROID)&&(wBindIndex<(INDEX_ANDROID+MAX_ANDROID)))
	{
		return m_pAndroidParameter+(wBindIndex-INDEX_ANDROID);
	}

	//错误断言
	ASSERT(FALSE);

	return NULL;
}

//绑定用户
IServerUserItem * CAttemperEngineSink::GetServerUserItem(WORD wBindIndex)
{
	//获取参数
	tagConnectItemInfo * pBindParameter=GetBindParameter(wBindIndex);

	//获取用户
	if (pBindParameter!=NULL)
	{
		return pBindParameter->pIServerUserItem;
	}

	//错误断言
	ASSERT(FALSE);

	return NULL;
}

//用户状况
WORD CAttemperEngineSink::GetAndroidUserStatus(tagAndroidUserStatus & AndroidUserStatus)
{
	//变量定义
	WORD wEnumIndex=0;
	IServerUserItem * pIServerUserItem=NULL;

	//设置变量
	ZeroMemory(&AndroidUserStatus,sizeof(AndroidUserStatus));

	//用户信息
	do
	{
		//获取用户
		pIServerUserItem=m_ServerUserManager.EnumOnLineUser(wEnumIndex++);

		//统计判断
		if ((pIServerUserItem!=NULL)&&(pIServerUserItem->IsAndroidUser()==true))
		{
			//获取属性
			BYTE cbUserStatus=pIServerUserItem->GetUserStatus();

			//用户统计
			switch (pIServerUserItem->GetUserStatus())
			{
			case US_FREE:
				{
					ASSERT(AndroidUserStatus.wFreeUserCount<CountArray(AndroidUserStatus.pServerUserFree));
					AndroidUserStatus.pServerUserFree[AndroidUserStatus.wFreeUserCount++]=pIServerUserItem;
					break;
				}
			case US_PLAY:
				{
					ASSERT(AndroidUserStatus.wPlayUserCount<CountArray(AndroidUserStatus.pServerUserPlay));
					AndroidUserStatus.pServerUserPlay[AndroidUserStatus.wPlayUserCount++]=pIServerUserItem;
					break;
				}
			case US_SIT:
			case US_READY:
				{
					ASSERT(AndroidUserStatus.wSitdownUserCount<CountArray(AndroidUserStatus.pServerUserSitdown));
					AndroidUserStatus.pServerUserSitdown[AndroidUserStatus.wSitdownUserCount++]=pIServerUserItem;
					break;
				}
			}
		}

	} while (pIServerUserItem!=NULL);

	return AndroidUserStatus.wFreeUserCount+AndroidUserStatus.wPlayUserCount+AndroidUserStatus.wSitdownUserCount;
}

//桌子状况
WORD CAttemperEngineSink::GetTableUserStatus(CTableFrame * pTableFrame, tagTableUserStatus & TableUserStatus)
{
	//设置变量
	ZeroMemory(&TableUserStatus,sizeof(TableUserStatus));

	//用户分析
	for (WORD i=0;i<m_pGameServiceAttrib->wChairCount;i++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem=pTableFrame->GetServerUserItem(i);
		if (pIServerUserItem==NULL) continue;

		//用户类型
		if (pIServerUserItem->IsAndroidUser()==false)
		{
			TableUserStatus.wTableUserCount++;
		}
		else
		{
			TableUserStatus.wTableAndroidCount++;
		}
	}

	//最少数目
	switch (pTableFrame->m_pITableFrameSink->GetGameStartMode())
	{
	case enStartMode_AllReady:	//所有准备
		{
			TableUserStatus.wLessUserCount=2;
			break;
		}
	default:					//默认模式
		{
			TableUserStatus.wLessUserCount=m_pGameServiceAttrib->wChairCount;
			break;
		}
	}

	return TableUserStatus.wTableAndroidCount+TableUserStatus.wTableUserCount;
}

//赠送鲜花
bool CAttemperEngineSink::OnEventFlower(const void * pData, WORD wDataSize, IServerUserItem * pSendIServerUserItem)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GF_Gift));
	if (wDataSize!=sizeof(CMD_GF_Gift)) return false;                

	//变量定义
	CMD_GF_Gift * pGift=(CMD_GF_Gift *)pData;

	//目标玩家
	IServerUserItem *pIRcvUserItem = NULL;
	pIRcvUserItem = m_ServerUserManager.SearchOnLineUser(pGift->dwRcvUserID);
	if ( pIRcvUserItem == NULL ) pIRcvUserItem = m_ServerUserManager.SearchOffLineUser(pGift->dwRcvUserID);
	tagServerUserData *pSendUserData = pSendIServerUserItem->GetUserData();

	if ( pIRcvUserItem != NULL )
	{
		//获取鲜花
		ASSERT(pGift->wGiftID < FLOWER_COUNT );
		if ( pGift->wGiftID >= FLOWER_COUNT ) return true;

		//查找鲜花
		tagFlowerInfo *pFlowerInfo = NULL;
		for ( INT_PTR nIndex = 0; nIndex < m_FlowerAttributeArrary.GetCount(); ++nIndex )
		{
			tagFlowerInfo &FlowerInfo = m_FlowerAttributeArrary[nIndex];
			if ( FlowerInfo.bNullity == false && FlowerInfo.nFlowerID == pGift->wGiftID )
			{
				pFlowerInfo = &FlowerInfo;
				break;
			}
		}

		//合法判断
		ASSERT( pFlowerInfo != NULL );
		if ( pFlowerInfo == NULL )
		{
			SendProMessage(pSendIServerUserItem, TEXT("购买失败，鲜花加载错误！"), SMT_EJECT|SMT_INFO);
			return true;
		}

		//获取玩家
		tagServerUserData *pRcvUserData = pIRcvUserItem->GetUserData();

		LONG lFlowerPrice = pFlowerInfo->lPrice * pGift->wFlowerCount * (pSendUserData->cbMemberOrder > 0 ? pFlowerInfo->cbDiscount : 100) / 100;

		//获取金币
		LONG lInsureScore = GetBankStorageGold(pSendIServerUserItem);

		//积分判断
		if ( lFlowerPrice <= lInsureScore )
		{
			//帐款支付
			ModifyBankStorageGold(pSendIServerUserItem, -lFlowerPrice);
		}
		else
		{
			//发送消息
			SendProMessage(pSendIServerUserItem,TEXT("购买失败，您购物金币不够！"),SMT_EJECT|SMT_INFO);
			return true;
		}

		//修改魅力
		LONG lSendUserCharm = pFlowerInfo->lSendUserCharm * pGift->wFlowerCount;
		LONG lRcvUserCharm = pFlowerInfo->lRcvUserCharm * pGift->wFlowerCount;
		if ( 0 < lSendUserCharm ) 
		{
			pSendUserData->lLoveliness += lSendUserCharm;
		}
		pRcvUserData->lLoveliness += lRcvUserCharm;

		//发送积分
		SendUserScore(pIRcvUserItem);
		SendUserScore(pSendIServerUserItem);

		//消息通知
		CMD_GF_GiftNotify GiftNotify;
		ZeroMemory( &GiftNotify, sizeof( GiftNotify ) );
		GiftNotify.cbSendLocation = pGift->cbSendLocation;
		GiftNotify.dwSendUserID = pSendUserData->dwUserID;
		GiftNotify.dwRcvUserID = pRcvUserData->dwUserID;
		GiftNotify.wGiftID = pGift->wGiftID;
		GiftNotify.wFlowerCount = pGift->wFlowerCount;

		//游戏房间
		if(pGift->cbSendLocation == LOCATION_GAME_ROOM)
		{
			ASSERT(pSendUserData->wTableID!=INVALID_TABLE);

			//玩家通知
			CTableFrame * pTableFrame=(m_pTableFrame+pSendUserData->wTableID);
			for ( WORD wChairID = 0; wChairID < m_pGameServiceAttrib->wChairCount; ++wChairID )
			{
				//获取玩家
				IServerUserItem *pPlayServerUserItem = pTableFrame->GetServerUserItem( wChairID );
				if ( pPlayServerUserItem == NULL ) continue;

				//发送消息
				SendData( pPlayServerUserItem, MDM_GF_PRESENT, SUB_GF_FLOWER, &GiftNotify, sizeof( GiftNotify ) );
			}

			//旁观通知
			IServerUserItem *pLookonServerUserItem = NULL;
			WORD wIndex = 0;
			do
			{
				//获取旁观
				pLookonServerUserItem = pTableFrame->EnumLookonUserItem( wIndex++ );

				//发送消息
				if ( pLookonServerUserItem != NULL ) SendData( pLookonServerUserItem, MDM_GF_PRESENT, SUB_GF_FLOWER, &GiftNotify, sizeof( GiftNotify ) );

			}while( pLookonServerUserItem != NULL );
		}
		//大厅房间
		else
		{
			//群发信息
			WORD wEnumIndex=0;
			IServerUserItem * pIServerUserItemSend=NULL;
			while (true)
			{
				pIServerUserItemSend=m_ServerUserManager.EnumOnLineUser(wEnumIndex++);
				if (pIServerUserItemSend==NULL) break;

				//发送数据
				SendData( pIServerUserItemSend, MDM_GF_PRESENT, SUB_GF_FLOWER, &GiftNotify, sizeof( GiftNotify ) );
			}
		}

		//数据库通知
		DBR_GR_SendGift SendGift;
		SendGift.dwSendUserID = pGift->dwSendUserID;
		SendGift.dwRcvUserID = pGift->dwRcvUserID;
		SendGift.dwClientIP = pSendIServerUserItem->GetClientIP();
		SendGift.wGiftID = pGift->wGiftID;
		SendGift.wFlowerCount = pGift->wFlowerCount;
		SendGift.lFlowerPrice = lFlowerPrice;
		SendGift.lRcvUserCharm = lSendUserCharm;
		SendGift.lSendUserCharm = lRcvUserCharm;

		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_SEND_GIFT,0,&SendGift, sizeof( SendGift ) );
	}
	else
	{
		//发送消息
		SendProMessage(pSendIServerUserItem,TEXT("对方已经离开，赠送失败！"),SMT_EJECT|SMT_INFO);
		return true;
	}

	return true;
}

//道具处理
bool CAttemperEngineSink::OnEventProperty(const void * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GF_Property));
	if (wDataSize!=sizeof(CMD_GF_Property)) return false;     
	CMD_GF_Property *pPropertyHeadInfo = (CMD_GF_Property*)pData;

	//道具I D
	int nPropertyID = pPropertyHeadInfo->nPropertyID;

	//合法判断
	ASSERT(nPropertyID < PROPERTY_COUNT );
	if ( nPropertyID >= PROPERTY_COUNT ) return true;

	//目标玩家
	IServerUserItem *pTargetUserItem = NULL;
	pTargetUserItem = m_ServerUserManager.SearchOnLineUser(pPropertyHeadInfo->dwTargetUserID);
	if ( pTargetUserItem == NULL ) pTargetUserItem = m_ServerUserManager.SearchOffLineUser(pPropertyHeadInfo->dwTargetUserID);

	//使用条件
	if(!IsPropertyUseRight(nPropertyID,pIServerUserItem,pTargetUserItem))return true;

	//获取信息
	tagPropertyInfo *pPropertyAttribute = NULL;
	for ( WORD wIndex = 0; wIndex < m_PropertyAttributeArrary.GetCount(); ++wIndex )
	{
		tagPropertyInfo *pTmpPropertyAttribute = &m_PropertyAttributeArrary[wIndex];
		if ( pTmpPropertyAttribute != NULL && pTmpPropertyAttribute->bNullity == false && pTmpPropertyAttribute->nPropertyID == nPropertyID )
		{
			pPropertyAttribute = pTmpPropertyAttribute;
			break;
		}
	}

	//合法判断
	ASSERT(pPropertyAttribute != NULL);
	if ( pPropertyAttribute == NULL )
	{
		SendProMessage(pIServerUserItem, TEXT("购买失败，道具加载错误！"), SMT_EJECT|SMT_INFO);
		return true;
	}

	//玩家信息
	tagServerUserData *pConsumeUserData = pIServerUserItem->GetUserData();
	tagServerUserData *pTargetUserData = pTargetUserItem->GetUserData();

	//打折判断
	int nDiscount = pConsumeUserData->cbMemberOrder > 0 ? pPropertyAttribute->cbDiscount : 100;

	//变量定义
	DWORD dwPropertyCount[] = {pPropertyAttribute->dwPropCount1, pPropertyAttribute->dwPropCount2, pPropertyAttribute->dwPropCount3,
		pPropertyAttribute->dwPropCount4, pPropertyAttribute->dwPropCount5, pPropertyAttribute->dwPropCount6,
		pPropertyAttribute->dwPropCount7, pPropertyAttribute->dwPropCount8, pPropertyAttribute->dwPropCount9, 
		pPropertyAttribute->dwPropCount10};
	LONG lPropertyPrice[] = {pPropertyAttribute->lPrice1, pPropertyAttribute->lPrice2, pPropertyAttribute->lPrice3,
		pPropertyAttribute->lPrice4, pPropertyAttribute->lPrice5, pPropertyAttribute->lPrice6, pPropertyAttribute->lPrice7,
		pPropertyAttribute->lPrice8, pPropertyAttribute->lPrice9, pPropertyAttribute->lPrice10};

	//价格判断
	LONG lPrice = 0;
	for ( int nIndex = 0; nIndex < CountArray(dwPropertyCount); ++ nIndex )
	{
		if ( dwPropertyCount[nIndex] == pPropertyHeadInfo->dwOnceCount )
		{
			lPrice = lPropertyPrice[nIndex] * pPropertyHeadInfo->dwPachurseCount * nDiscount / 100;
			break;
		}

		//错误判断
		ASSERT(nIndex < CountArray(dwPropertyCount) - 1);
		if ( nIndex >= CountArray(dwPropertyCount) - 1 )
		{
			SendProMessage(pIServerUserItem, TEXT("购买失败，购买数量有误！"), SMT_EJECT|SMT_INFO);

			return true;
		}
	}

	//获取金币
	LONG lInsureScore = GetBankStorageGold(pIServerUserItem);

	//积分判断
	if ( lPrice <= lInsureScore )
	{
		//帐款支付
		ModifyBankStorageGold(pIServerUserItem, -lPrice);
	}
	else
	{
		//发送消息
		SendProMessage(pIServerUserItem,TEXT("购买失败，您购物金币不够！"),SMT_EJECT|SMT_INFO);		
		return true;
	}

	//递增数目
	DWORD dwCurPropCount = 0;
	if ( nPropertyID != PROP_NEGAGIVE && nPropertyID != PROP_FLEE )
	{
		DWORD dwCurPropCount = pTargetUserData->PropertyInfo[nPropertyID].dwPropCount - pTargetUserData->PropertyInfo[nPropertyID].dwConsumedCount;
		if ( pTargetUserData->PropertyInfo[nPropertyID].dwPropCount < pTargetUserData->PropertyInfo[nPropertyID].dwConsumedCount) 
			dwCurPropCount = 0;
		dwCurPropCount += ( pPropertyHeadInfo->dwPachurseCount * pPropertyHeadInfo->dwOnceCount );
		pTargetUserData->PropertyInfo[nPropertyID].dwPropCount = dwCurPropCount;
		pTargetUserData->PropertyInfo[nPropertyID].dwConsumedCount = 0;

		//发送道具
		SendResidualProperty( pTargetUserItem );
	}

	//负分清零
	if ( nPropertyID == PROP_NEGAGIVE )
	{
		LONG lNowScore = pTargetUserData->UserScoreInfo.lScore;
		if ( lNowScore < 0)
		{
			//变量定义
			tagScoreInfo ScoreInfo;
			ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));

			//设置变量
			ScoreInfo.lScore=-lNowScore;
			ScoreInfo.ScoreKind=enScoreKind_Present;

			//修改积分
			pTargetUserItem->WriteScore(ScoreInfo,0);

			//发送积分
			SendUserScore(pTargetUserItem);
		}
	}
	//逃跑清零
	else if ( nPropertyID == PROP_FLEE )
	{
		LONG lNowFleeCount = pTargetUserData->UserScoreInfo.lFleeCount;
		if ( 0 < lNowFleeCount )
		{
			tagUserScore *pScoreModify = pTargetUserItem->GetUserScoreModifyInfo();
			pScoreModify->lFleeCount = -lNowFleeCount;
			pTargetUserData->UserScoreInfo.lFleeCount = 0;

			//发送积分
			SendUserScore(pTargetUserItem);
		}
	}
	//会员权限
	else if( nPropertyID >= PROP_MEMBER_1 )
	{
		//更新等级
		BYTE cbMemberOrder = pTargetUserData->cbMemberOrder ;
		if(cbMemberOrder<(nPropertyID-PROP_MEMBER_1+1))
		{
			//设置等级
			pTargetUserData->cbMemberOrder = nPropertyID-PROP_MEMBER_1+1;

			//发送数据
			CMD_GR_MemberOrder MemberOrder;
			MemberOrder.dwUserID = pTargetUserData->dwUserID;
			MemberOrder.cbMemberOrder = nPropertyID-PROP_MEMBER_1+1;
			m_pITCPNetworkEngine->SendDataBatch(MDM_GR_USER,SUB_GR_MEMBER_ORDER,&MemberOrder,sizeof(MemberOrder));
			m_AndroidUserManager.SendDataToClient(MDM_GR_USER,SUB_GR_MEMBER_ORDER,&MemberOrder,sizeof(MemberOrder));
		}
	}

	//设置信息
	DBR_GR_Property Property;
	ZeroMemory(&Property, sizeof(Property));

	Property.dwUserID = pIServerUserItem->GetUserID();
	Property.nPropertyID = nPropertyID;
	Property.dwCurCount = dwCurPropCount;
	Property.dwClientIP = pIServerUserItem->GetClientIP();
	Property.lPrice = lPrice;
	Property.dwOnceCount = pPropertyHeadInfo->dwOnceCount;
	Property.dwPachurseCount = pPropertyHeadInfo->dwPachurseCount;
	Property.dwTargetUserID = pTargetUserData->dwUserID;

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_BUY_PROPERTY,0,&Property, sizeof(Property));

	//发送消息
	if ( pConsumeUserData->dwUserID != pTargetUserData->dwUserID )
	{
		CString strRoomMsg;
		strRoomMsg.Format(TEXT("[ %s ]赠送[ %s ](%s)"), pConsumeUserData->szAccounts, pTargetUserData->szAccounts, 
			g_PropertyDescribe[nPropertyID].szName);
		SendProMessage(INVALID_WORD, LPCTSTR(strRoomMsg), SMT_INFO);

		CString strGameMsg;
		strGameMsg.Format(TEXT("[ %s ]赠送您(%s)"), pConsumeUserData->szAccounts, g_PropertyDescribe[nPropertyID].szName);
		SendProMessage(pTargetUserItem, strGameMsg, SMT_INFO);
	}
	SendProMessage(pIServerUserItem, TEXT("恭喜您，购买成功！"),SMT_EJECT|SMT_INFO);

	////赠送判断
	//if ( PROP_MEMBER_1 <= nPropertyID && nPropertyID <= PROP_MEMBER_8 )
	//{
	//	//发送消息
	//	CString strMsg;
	//	LONG lGold[] = {10000, 30000, 60000, 100000, 600000, 1200000};
	//	int nIndex = nPropertyID - PROP_MEMBER_1;
	//	strMsg.Format(TEXT("恭喜你，获赠(%ld)购物金币"), lGold[nIndex]);
	//	SendProMessage(pTargetUserItem, strMsg,SMT_INFO); 

	//	//修改帐款
	//	ModifyBankStorageGold(pTargetUserItem, lGold[nIndex]);
	//}
 
	return true;
}

//魅力兑换
bool CAttemperEngineSink::OnEventExchangeCharm(const void * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//参数验证
	ASSERT( sizeof(CMD_GF_ExchangeCharm) == wDataSize );
	if ( sizeof(CMD_GF_ExchangeCharm) != wDataSize ) return false;

	CMD_GF_ExchangeCharm *pExchangeLoveliness = (CMD_GF_ExchangeCharm*)pData;

	//合法验证
	tagServerUserData *pServerUserData = pIServerUserItem->GetUserData();
	if ( pServerUserData->lLoveliness< pExchangeLoveliness->lLoveliness ) return false;
	if ( pExchangeLoveliness->lGoldValue != pExchangeLoveliness->lLoveliness * CHARM_EXCHANGE_RATE ) return false;

	//修改魅力
	pServerUserData->lLoveliness -= pExchangeLoveliness->lLoveliness;

	//修改帐款
	ModifyBankStorageGold(pIServerUserItem, pExchangeLoveliness->lLoveliness * CHARM_EXCHANGE_RATE);

	//系统消息
	TCHAR szMessage[] = TEXT("恭喜你，兑换成功");
	SendProMessage(pIServerUserItem, szMessage, SMT_EJECT|SMT_INFO);

	//数据库通知
	DBR_GR_ExchangeLoveliness ExchangeLoveliness;
	ZeroMemory(&ExchangeLoveliness, sizeof(ExchangeLoveliness));
	ExchangeLoveliness.dwUserID = pServerUserData->dwUserID;
	ExchangeLoveliness.lLoveliness = pExchangeLoveliness->lLoveliness;
	ExchangeLoveliness.lGoldValue = pExchangeLoveliness->lGoldValue;
	ExchangeLoveliness.dwClientIP = pIServerUserItem->GetClientIP();

	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_EXCHANGE_CHARM,0,&ExchangeLoveliness, sizeof(ExchangeLoveliness));

	return true;
}

//喇叭事件
bool CAttemperEngineSink::OnEventBugle(const void * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//参数验证
	ASSERT( sizeof(CMD_GF_BugleProperty) == wDataSize );
	if ( sizeof(CMD_GF_BugleProperty) != wDataSize ) return false;

	//类型转换
	CMD_GF_BugleProperty *pBugleProperty = (CMD_GF_BugleProperty*)pData;

	//获取玩家
	tagServerUserData *pServerUserData = pIServerUserItem->GetUserData();

	//数目判断
	if ( pServerUserData->PropertyInfo[PROP_BUGLE].dwPropCount <= pServerUserData->PropertyInfo[PROP_BUGLE].dwConsumedCount )
	{
		//获取信息
		tagPropertyInfo *pPropertyAttribute = NULL;
		for ( WORD wIndex = 0; wIndex < m_PropertyAttributeArrary.GetCount(); ++wIndex )
		{
			tagPropertyInfo *pTmpPropertyAttribute = &m_PropertyAttributeArrary[wIndex];
			if ( pTmpPropertyAttribute != NULL && pTmpPropertyAttribute->bNullity == false && pTmpPropertyAttribute->nPropertyID == PROP_BUGLE )
			{
				pPropertyAttribute = pTmpPropertyAttribute;
				break;
			}
		}

		//合法判断
		ASSERT(pPropertyAttribute != NULL);
		if ( pPropertyAttribute == NULL )
		{
			SendProMessage(pIServerUserItem, TEXT("购买失败，道具加载错误！"), SMT_EJECT|SMT_INFO);
			return true;
		}

		//设置数据
		CMD_GF_Property PropertyHeadInfo;
		ZeroMemory(&PropertyHeadInfo, sizeof(PropertyHeadInfo));
		PropertyHeadInfo.cbSendLocation = pBugleProperty->cbSendLocation;
		PropertyHeadInfo.nPropertyID = PROP_BUGLE;
		PropertyHeadInfo.dwSourceUserID = pServerUserData->dwUserID;
		PropertyHeadInfo.dwTargetUserID = pServerUserData->dwUserID;
		PropertyHeadInfo.dwPachurseCount = 1;
		PropertyHeadInfo.dwOnceCount = pPropertyAttribute->dwPropCount1;

		//购买喇叭
		OnEventProperty(&PropertyHeadInfo,sizeof(PropertyHeadInfo),pIServerUserItem);

		//购买成功
		tagServerUserData *pTargetUserData = pIServerUserItem->GetUserData();
		if(pTargetUserData->PropertyInfo[PROP_BUGLE].dwConsumedCount == 0
			&& pTargetUserData->PropertyInfo[PROP_BUGLE].dwPropCount==pPropertyAttribute->dwPropCount1)
		{
			//递增数目
			pServerUserData->PropertyInfo[PROP_BUGLE].dwConsumedCount++;

			//发送数目
			SendResidualProperty(pIServerUserItem);
		}
		else return true;
	}
	else
	{
		//递增数目
		pServerUserData->PropertyInfo[PROP_BUGLE].dwConsumedCount++;

		//发送数目
		SendResidualProperty(pIServerUserItem);
	}

	//发送数据
	m_pITCPNetworkEngine->SendDataBatch(MDM_GF_PRESENT,SUB_GF_PROP_BUGLE,pBugleProperty,wDataSize);
	m_AndroidUserManager.SendDataToClient(MDM_GF_PRESENT,SUB_GF_PROP_BUGLE,pBugleProperty,wDataSize);

	return true;
}

//提取事件
bool CAttemperEngineSink::OnEventBankDrawoutGold(const void * pData, WORD wDataSize, DWORD dwSocketID)
{
	//参数验证
	ASSERT( sizeof(CMD_GF_BankGet) == wDataSize );
	if ( sizeof(CMD_GF_BankGet) != wDataSize ) return false;

	//获取玩家
	IServerUserItem * pIServerUserItem=GetServerUserItem(LOWORD(dwSocketID));
	ASSERT(pIServerUserItem!=NULL);
	if(pIServerUserItem==NULL) return false;
	tagServerUserData *pServerUserData = pIServerUserItem->GetUserData();

	//判断状态
	if(m_pGameServiceOption->wServerType==GAME_GENRE_GOLD)
	{
		if(pServerUserData->wTableID!=INVALID_TABLE)
		{
			SendRoomMessage(dwSocketID,TEXT("请退出游戏后，再进行提取操作!"),SMT_EJECT);
			return true;
		}
	}

	//类型转换
	CMD_GF_BankGet *pBankGet= (CMD_GF_BankGet*)pData;
	LONG lStorageGold = pServerUserData->UserScoreInfo.lInsureScore;

	//效验数据
	ASSERT(pBankGet->lGetValue>0 && pBankGet->lGetValue<=lStorageGold);
	if(!(pBankGet->lGetValue>0 && pBankGet->lGetValue<=lStorageGold))return false;

	//密码效验
	if (lstrcmp(pIServerUserItem->GetPassword(),pBankGet->szPassword)!=0)
	{
		SendRoomMessage(dwSocketID,TEXT("密码有误，请查证后再次尝试操作！"),SMT_EJECT);
		return true;
	}

	//修改金币
	ModifyGameGold(pIServerUserItem,pBankGet->lGetValue);

	//修改帐款
	ModifyBankStorageGold(pIServerUserItem,-pBankGet->lGetValue);
	SendRoomMessage(dwSocketID,TEXT("提取成功!"),SMT_EJECT);

	//变量定义
	DBR_GR_BankDrawoutGold BankDrawoutGold;
	ZeroMemory(&BankDrawoutGold, sizeof(BankDrawoutGold));
	BankDrawoutGold.DrawoutGoldCount = pBankGet->lGetValue;
	BankDrawoutGold.dwUserID = pServerUserData->dwUserID;
	BankDrawoutGold.dwClientIP = pIServerUserItem->GetClientIP();

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_BANK_GET_GOLD,0,&BankDrawoutGold, sizeof(BankDrawoutGold));

	return true;
}

//存储事件
bool CAttemperEngineSink::OnEventBankStorage(const void * pData, WORD wDataSize, DWORD dwSocketID)
{
	//参数验证
	ASSERT( sizeof(CMD_GF_BankStorage) == wDataSize );
	if ( sizeof(CMD_GF_BankStorage) != wDataSize ) return false;

	//获取玩家
	IServerUserItem * pIServerUserItem=GetServerUserItem(LOWORD(dwSocketID));
	ASSERT(pIServerUserItem!=NULL);
	if(pIServerUserItem==NULL) return false;
	tagServerUserData *pServerUserData = pIServerUserItem->GetUserData();

	//判断状态
	if(m_pGameServiceOption->wServerType==GAME_GENRE_GOLD)
	{
		if(pServerUserData->wTableID!=INVALID_TABLE)
		{
			SendRoomMessage(dwSocketID,TEXT("请退出游戏后，再进行存储操作!"),SMT_EJECT);
			return true;
		}
	}

	//类型转换
	CMD_GF_BankStorage *pBankStorage= (CMD_GF_BankStorage*)pData;
	LONG lGameGold = pServerUserData->UserScoreInfo.lGameGold;

	//效验数据
	ASSERT(pBankStorage->lStorageValue>0 && pBankStorage->lStorageValue<=lGameGold);
	if(!(pBankStorage->lStorageValue>0 && pBankStorage->lStorageValue<=lGameGold))return false;

	//密码效验
	if (lstrcmp(pIServerUserItem->GetPassword(),pBankStorage->szPassword)!=0)
	{
		SendRoomMessage(dwSocketID,TEXT("密码有误，请查证后再次尝试操作！"),SMT_EJECT);
		return true;
	}

	//修改金币
	ModifyGameGold(pIServerUserItem,-pBankStorage->lStorageValue);

	//修改帐款
	ModifyBankStorageGold(pIServerUserItem,pBankStorage->lStorageValue);
	SendRoomMessage(dwSocketID,TEXT("存储成功!"),SMT_EJECT);

	//变量定义
	DBR_GR_BankStorage BankStorage;
	ZeroMemory(&BankStorage, sizeof(BankStorage));
	BankStorage.lStorageCount = pBankStorage->lStorageValue;
	BankStorage.dwUserID = pServerUserData->dwUserID;
	BankStorage.dwClientIP = pIServerUserItem->GetClientIP();

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_BANK_STORAGE_GOLD,0,&BankStorage, sizeof(BankStorage));

	return true;
}

//使用权限
bool CAttemperEngineSink::IsPropertyUseRight(INT nPropertyID,IServerUserItem *pIServerUserItem,IServerUserItem *pTargetUserItem)
{
	//目标玩家
	if ( pTargetUserItem == NULL )
	{
		SendProMessage(pIServerUserItem, TEXT("赠送失败，您要赠送的玩家已经离开！"), SMT_EJECT|SMT_INFO);
		return false;
	}

	//获取玩家
	ASSERT(pIServerUserItem!=NULL);
	tagServerUserData *pConsumeUserData = pIServerUserItem->GetUserData();
	tagServerUserData *pTargetUserData = pTargetUserItem->GetUserData();

	//房间判断
	if ( (m_pGameServiceOption->wServerType == GAME_GENRE_GOLD || m_pGameServiceOption->wServerType == GAME_GENRE_MATCH )
		&& ( nPropertyID == PROP_NEGAGIVE || nPropertyID == PROP_DOUBLE || nPropertyID == PROP_FOURDOLD || nPropertyID == PROP_SHIELD) )
	{
		TCHAR szMessage[] = TEXT("此房间不可以使用此道具！");
		SendProMessage(pIServerUserItem, szMessage, SMT_INFO|SMT_EJECT);
		return false;
	}

	//使用判断
	switch(nPropertyID)
	{
	case PROP_NEGAGIVE :			//负分清零
		{
			LONG lNowScore = pTargetUserData->UserScoreInfo.lScore;
			if( 0 <= lNowScore)
			{
				CString strMessage;
				if ( pIServerUserItem == pTargetUserItem ) 
					strMessage.Format(TEXT("您现在的积分已经是非负数，不需要使用负分清零道具！"));
				else
					strMessage.Format(TEXT("[ %s ]现在的积分已经是非负数，不需要使用负分清零道具！"), pTargetUserData->szAccounts);

				SendProMessage(pIServerUserItem, strMessage, SMT_INFO|SMT_EJECT);
				return false;
			}
			break;
		}
	case PROP_FLEE :				//逃跑清零
		{
			LONG lNowFleeCount = pTargetUserData->UserScoreInfo.lFleeCount;

			if ( lNowFleeCount == 0 )
			{
				CString strMessage;
				if ( pIServerUserItem == pTargetUserItem ) 
					strMessage.Format(TEXT("您现在的逃跑率已经为0，不需要使用逃跑清零道具！"));
				else
					strMessage.Format(TEXT("[ %s ]现在的逃跑率已经为0，不需要使用逃跑清零道具！"), pTargetUserData->szAccounts);

				SendProMessage(pIServerUserItem, strMessage, SMT_INFO|SMT_EJECT);
				return false;
			}
			break;
		}
	case PROP_BUGLE :				//喇叭判断
		{
			//变量定义
			bool bMatchServer=((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0);
			bool bControlStart=((bMatchServer==true)&&m_pGameServiceOption->cbControlStart==TRUE);

			//比赛旁观
			if (pConsumeUserData->dwMasterRight==0L)
			{
				if ((bMatchServer==true)&&(pIServerUserItem->GetUserStatus()==US_LOOKON))
				{
					LPCTSTR pszMessage=TEXT("为了不影响比赛选手比赛，旁观的用户禁止发言！");
					SendProMessage(pIServerUserItem,pszMessage,SMT_EJECT|SMT_INFO);
					return false;
				}
			}

			//权限判断
			if (CUserRight::CanGameChat(pConsumeUserData->dwUserRight)==false)
			{
				LPCTSTR pszMessage=TEXT("你暂时没有发送房间消息的权限，只能与管理员私聊！");
				SendProMessage(pIServerUserItem,pszMessage,SMT_EJECT|SMT_INFO);
				return false;
			}

			//状态查询
			if ((pConsumeUserData->dwMasterRight==0L)&&(IsAllowGameChat()==false))
			{
				LPCTSTR pszMessage=TEXT("抱歉，本游戏房间不允许发送聊天信息！");
				SendProMessage(pIServerUserItem,pszMessage,SMT_EJECT|SMT_INFO);
				return false;
			}
			break;
		}
	}

	return true;
}

//修改金币
void CAttemperEngineSink::ModifyGameGold(IServerUserItem * pIServerUserItem,LONG lGameGold)
{
	//用户信息
	ASSERT(pIServerUserItem!=NULL);
	tagServerUserData *pServerUserData = pIServerUserItem->GetUserData();
	tagUserScore *pScoreModify = pIServerUserItem->GetUserScoreModifyInfo();

	//修改金币
	pServerUserData->UserScoreInfo.lGameGold += lGameGold;
	pScoreModify->lGameGold +=lGameGold;

	//金币房间
	if(m_pGameServiceOption->wServerType == GAME_GENRE_GOLD)
	{
		pServerUserData->UserScoreInfo.lScore += lGameGold;
		pScoreModify->lScore +=lGameGold;

		//重新设置
		pServerUserData->UserScoreInfo.lScore += pServerUserData->lStorageScore;
		pServerUserData->lStorageScore=0L;
		ASSERT(pServerUserData->UserScoreInfo.lGameGold==pServerUserData->UserScoreInfo.lScore);

		//存储积分
		if ( m_pGameServiceOption->lRestrictScore>0L && pServerUserData->UserScoreInfo.lScore>m_pGameServiceOption->lRestrictScore )
		{
			LONG lUserScore=pServerUserData->UserScoreInfo.lScore;
			pServerUserData->UserScoreInfo.lScore=m_pGameServiceOption->lRestrictScore;
			pServerUserData->lStorageScore=lUserScore-m_pGameServiceOption->lRestrictScore;
		}

		//最少额度
		LONG lLessScore=m_pGameServiceOption->lLessScore;
		if((pIServerUserItem->GetTableID()!=INVALID_TABLE) && (lLessScore>0) && (lLessScore>pServerUserData->UserScoreInfo.lScore))
		{
			CTableFrame* pTableFrame=m_pTableFrame+pIServerUserItem->GetTableID();

			if (!(pTableFrame->IsUserPlaying(pIServerUserItem)))
			{
				//发送信息
				TCHAR szDescribe[256]=TEXT("");
				_snprintf(szDescribe,sizeof(szDescribe),TEXT("加入游戏至少需要 %ld 的金币，您的金币不够，不能加入！"),lLessScore);
				SendRoomMessage(pIServerUserItem,szDescribe,SMT_INFO);

				//弹起用户
				pTableFrame->PerformStandUpAction(pIServerUserItem);
			}
		}

		//最大额度
		LONG lMaxScore=m_pGameServiceOption->lMaxScore;
		if((pIServerUserItem->GetTableID()!=INVALID_TABLE) && (lMaxScore>m_pGameServiceOption->lLessScore) && (lMaxScore<pServerUserData->UserScoreInfo.lScore))
		{
			CTableFrame* pTableFrame=m_pTableFrame+pIServerUserItem->GetTableID();

			if (!(pTableFrame->IsUserPlaying(pIServerUserItem)))
			{
				//发送信息
				TCHAR szDescribe[256]=TEXT("");
				_snprintf(szDescribe,sizeof(szDescribe),TEXT("您的金币超过本房间的最大限制额度%ld,请更换游戏房间!"),lMaxScore);
				SendRoomMessage(pIServerUserItem,szDescribe,SMT_INFO);

				//弹起用户
				pTableFrame->PerformStandUpAction(pIServerUserItem);
			}
		}
	}

	return;
}

//获取帐款
LONG CAttemperEngineSink::GetBankStorageGold(IServerUserItem * pIServerUserItem)
{
	//获取结构
	tagServerUserData *pServerUserData = pIServerUserItem->GetUserData();

	return pServerUserData->UserScoreInfo.lInsureScore;
}

//修改帐款
void CAttemperEngineSink::ModifyBankStorageGold(IServerUserItem * pIServerUserItem, LONG lStorageCount)
{
	//修改金币
	pIServerUserItem->ModifyBankStorageGold(lStorageCount);

	//积分通知
	SendUserScore( pIServerUserItem );

	return ;
}


//////////////////////////////////////////////////////////////////////////
