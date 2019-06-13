#include "StdAfx.h"
#include "resource.h"
#include "GlobalOption.h"
#include "ClientKernel.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CClientKernel, CWnd)
	ON_WM_TIMER()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CClientKernel::CClientKernel()
{
	//房间信息
	m_dwUserID=0L;
	m_wTableID=INVALID_TABLE;
	m_wChairID=INVALID_CHAIR;
	memset(&m_ServerAttribute,0,sizeof(m_ServerAttribute));

	//游戏信息
	m_bInquire=true;
	m_bLookonMode=false;
	m_pMeUserItem=NULL;
	m_bGameStatus=GS_FREE;
	memset(m_pUserItem,0,sizeof(m_pUserItem));

	//配置信息
	m_bAllowLookon=false;
	m_bAllowUserLookon=false;

	//定时器信息
	m_nTimerID=0;
	m_nElapseCount=0;
	m_hWndGameFrame=NULL;
	m_wTimerChairID=INVALID_CHAIR;

	//通讯变量
	m_hShareMemory=NULL;
	m_pShareMemory=NULL;

	//接口指针
	m_pIMessageProxy=NULL;
	m_pIClientKernelSink=NULL;

	return;
}

//析构函数
CClientKernel::~CClientKernel(void)
{
	DestroyClientKernel();

	return;
}

//接口查询
void * __cdecl CClientKernel::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IClientKernel,Guid,dwQueryVer);
	QUERYINTERFACE(IChannelMessageSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IClientKernel,Guid,dwQueryVer);
	return NULL;
}

//设置接口
bool __cdecl CClientKernel::SetMessageProxy(IUnknownEx * pIUnknownEx)
{
	ASSERT(pIUnknownEx!=NULL);
	m_pIMessageProxy=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IMessageProxy);
	ASSERT(m_pIMessageProxy!=NULL);

	//视频设置
	CVideoServiceManager * pVideoServiceManager=CVideoServiceManager::GetInstance();
	if (pVideoServiceManager!=NULL) pVideoServiceManager->SetMessageProxy(QUERY_OBJECT_PTR_INTERFACE(m_pIMessageProxy,IUnknownEx));

	return (m_pIMessageProxy!=NULL);
}

//设置接口
bool __cdecl CClientKernel::DestroyClientKernel()
{
	//设置接口
	m_pIMessageProxy=NULL;
	m_pIClientKernelSink=NULL;

	//关闭窗口
	if (m_hWnd!=NULL)
	{
		DestroyWindow();
	}

	//共享内存
	if (m_pShareMemory!=NULL)
	{
		UnmapViewOfFile(m_pShareMemory);
		m_pShareMemory=NULL;
	}
	if (m_hShareMemory!=NULL)
	{
		CloseHandle(m_hShareMemory);
		m_hShareMemory=NULL;
	}

	//关闭组件
	if (m_ChannelServiceHelper.GetInterface()!=NULL)
	{
		m_ChannelServiceHelper->CloseChannel(true,false);
	}

	return true;
}

//内核配置
bool __cdecl CClientKernel::InitClientKernel(LPCTSTR lpszComLine, IUnknownEx * pIUnknownEx)
{
	//效验参数
	ASSERT(lpszComLine!=NULL);
	if (lpszComLine==NULL) return false;

	//创建窗口
	if (m_hWnd==NULL) 
	{
		CRect rcCreate(0,0,0,0);
		Create(NULL,NULL,WS_CHILD,rcCreate,GetDesktopWindow(),100);
	}

	//获取框架
	m_pIClientKernelSink=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IClientKernelSink);
	if (m_pIClientKernelSink==NULL) return false;
	m_hWndGameFrame=m_pIClientKernelSink->GetFrameWnd();

	//读取配置
	m_bAllowUserLookon=AfxGetApp()->GetProfileInt(TEXT("GameOption"),TEXT("AllowLookon"),FALSE)?true:false;

	//视频设置
	CVideoServiceManager * pVideoServiceManager=CVideoServiceManager::GetInstance();
	if (pVideoServiceManager!=NULL) pVideoServiceManager->SetClientKernel(QUERY_OBJECT_PTR_INTERFACE(this,IUnknownEx));

	//命令行处理
	if (lpszComLine[0]!=0)
	{
		//提出 TOKEN
		int nStringLength=0;
		CString strRoomToken;
		LPCTSTR pszRoomToken=TEXT("/RoomToken:");
		LPCTSTR lpszBeginString=lpszComLine;
		while (true)
		{
			LPCTSTR lpszEndString=_tcschr(lpszBeginString,TEXT(' '));
			nStringLength=(lpszEndString==NULL)?lstrlen(lpszBeginString):(int)(lpszEndString-lpszBeginString);

			//判断标识
			const int nTokenLength=lstrlen(pszRoomToken);
			if ((nStringLength>=nTokenLength)&&(memcmp(lpszBeginString,pszRoomToken,nTokenLength*sizeof(TCHAR))==0))
			{
				CopyMemory(strRoomToken.GetBufferSetLength(nStringLength-nTokenLength),lpszBeginString+nTokenLength,
					(nStringLength-nTokenLength)*sizeof(TCHAR));
				strRoomToken.ReleaseBuffer();
				break;
			}

			//设置变量
			if (lpszEndString==NULL) break;
			lpszBeginString=(lpszEndString+1);
		}

		//共享内存
		if (strRoomToken.GetLength()>0)
		{
			m_hShareMemory=OpenFileMapping(FILE_MAP_ALL_ACCESS,FALSE,strRoomToken);
			if (m_hShareMemory==NULL) return false;
			m_pShareMemory=(tagShareMemory *)MapViewOfFile(m_hShareMemory,FILE_MAP_ALL_ACCESS,0,0,0);
			if (m_pShareMemory==NULL) return false;
			if (m_pShareMemory->wDataSize<sizeof(tagShareMemory)) return false;
			m_pShareMemory->hWndGameFrame=m_hWndGameFrame;
		}

		//信道模块
		if (m_hShareMemory!=NULL)
		{
			ASSERT(m_pShareMemory->hWndGameServer!=NULL);
			IUnknownEx * pIUnknownEx=QUERY_ME_INTERFACE(IUnknownEx);
			if (m_ChannelServiceHelper.CreateInstance()==false) return false;
			if (m_ChannelServiceHelper->SetChannelMessageSink(pIUnknownEx)==false) return false;
			if (m_ChannelServiceHelper->CreateChannel(m_pShareMemory->hWndGameServer)==false) return false;
		}
	}

	//更新标题
	UpdateGameTitle();

	return true;
}

//离开状态
bool __cdecl CClientKernel::GetExitFlags()
{
	if (m_bLookonMode==true) return true;
	if (m_pMeUserItem==NULL) return true;
	if (m_pMeUserItem->cbUserStatus!=US_PLAY) return true;
	return (m_bInquire==true)?false:true;
}

//旁观控制
bool __cdecl CClientKernel::AllowUserLookon(DWORD dwUserID, bool bAllowLookon)
{
	//设置变量
	if (dwUserID==0L)
	{
		m_bAllowUserLookon=bAllowLookon;
		AfxGetApp()->WriteProfileInt(TEXT("GameOption"),TEXT("AllowLookon"),(m_bAllowUserLookon==true)?TRUE:FALSE);
	}

	//发送消息
	if (IsLookonMode()==false)
	{
		//构造数据
		CMD_GF_LookonControl LookonControl;
		ZeroMemory(&LookonControl,sizeof(LookonControl));

		//设置变量
		LookonControl.dwUserID=dwUserID;
		LookonControl.bAllowLookon=(bAllowLookon==true)?TRUE:FALSE;

		//发送消息
		SendSocketData(MDM_GF_FRAME,SUB_GF_LOOKON_CONTROL,&LookonControl,sizeof(LookonControl));
	}

	return true;
}

//获取玩家
const tagUserData * __cdecl CClientKernel::GetUserInfo(WORD wChairID)
{
	if (wChairID>=MAX_CHAIR) return NULL;
	if (wChairID>=m_ServerAttribute.wChairCount) return NULL;
	return m_pUserItem[wChairID];
}

//显示消息
int __cdecl CClientKernel::ShowMessageBox(LPCTSTR pszMessage, UINT nType)
{
	LPCTSTR pszTitle=m_ServerAttribute.szServerName;
	return	ShowInformationEx(pszMessage,30,nType,pszTitle);
}


//发送函数
bool __cdecl CClientKernel::SendSocketData(WORD wMainCmdID, WORD wSubCmdID)
{
	//效验状态
	if (m_ChannelServiceHelper.GetInterface()==NULL) return false;

	//构造数据
	IPC_SocketPackage SocketPackage;
	memset(&SocketPackage,0,sizeof(SocketPackage));
	SocketPackage.Command.wMainCmdID=wMainCmdID;
	SocketPackage.Command.wSubCmdID=wSubCmdID;

	//发送数据
	WORD wSendSize=sizeof(SocketPackage.Command);
	return m_ChannelServiceHelper->SendData(IPC_MAIN_SOCKET,IPC_SUB_SOCKET_SEND,&SocketPackage,wSendSize);
}

//发送函数
bool __cdecl CClientKernel::SendSocketData(WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize)
{
	//效验状态
	if (m_ChannelServiceHelper.GetInterface()==NULL) return false;

	//效验数据
	ASSERT(wDataSize<=SOCKET_PACKET);
	if (wDataSize>SOCKET_PACKET) return false;

	//构造数据
	IPC_SocketPackage SocketPackage;
	memset(&SocketPackage,0,sizeof(SocketPackage));
	SocketPackage.Command.wMainCmdID=wMainCmdID;
	SocketPackage.Command.wSubCmdID=wSubCmdID;
	if (wDataSize>0)
	{
		ASSERT(pData!=NULL);
		CopyMemory(SocketPackage.cbBuffer,pData,wDataSize);
	}

	//发送数据
	WORD wSendSize=sizeof(SocketPackage.Command)+wDataSize;
	return m_ChannelServiceHelper->SendData(IPC_MAIN_SOCKET,IPC_SUB_SOCKET_SEND,&SocketPackage,wSendSize);
}

//发送数据
bool __cdecl CClientKernel::SendProcessData(WORD wMainCmdID, WORD wSubCmdID)
{
	if (m_ChannelServiceHelper.GetInterface()==NULL) return false;
	return m_ChannelServiceHelper->SendData(wMainCmdID,wSubCmdID);	
}

//发送数据
bool __cdecl CClientKernel::SendProcessData(WORD wMainCmdID, WORD wSubCmdID, void * pBuffer, WORD wDataSize)
{
	if (m_ChannelServiceHelper.GetInterface()==NULL) return false;
	return m_ChannelServiceHelper->SendData(wMainCmdID,wSubCmdID,pBuffer,wDataSize); 
}

//发送准备
bool __cdecl CClientKernel::SendUserReady(void * pBuffer, WORD wDataSize)
{
	return SendSocketData(MDM_GF_FRAME,SUB_GF_USER_READY,pBuffer,wDataSize);
}

//发送聊天
bool __cdecl CClientKernel::SendChatMessage(DWORD dwTargetUserID, LPCTSTR pszChatMessage, COLORREF crFontColor)
{
	//效验状态
	if (m_pMeUserItem==NULL) return false;

	//构造数据
	CMD_GF_UserChat UserChat;
	UserChat.crFontColor=crFontColor;
	UserChat.dwTargetUserID=dwTargetUserID;
	UserChat.dwSendUserID=m_pMeUserItem->dwUserID;
	lstrcpyn(UserChat.szChatMessage,pszChatMessage,CountArray(UserChat.szChatMessage));
	UserChat.wChatLength=CountStringBuffer(UserChat.szChatMessage);

	//发送命令
	WORD wSendSize=sizeof(UserChat)-sizeof(UserChat.szChatMessage)+UserChat.wChatLength;
	SendSocketData(MDM_GF_FRAME,SUB_GF_USER_CHAT,&UserChat,wSendSize);

	return true;
}

//设置定时器
bool __cdecl CClientKernel::SetGameTimer(WORD wChairID, UINT nTimerID, UINT nElapse)
{
	//效验参数
	ASSERT(m_hWnd!=NULL);
	ASSERT(m_pIClientKernelSink!=NULL);
	if (m_pIClientKernelSink==NULL) return false;

	//逻辑处理
	if (m_nTimerID!=0L) KillGameTimer(m_nTimerID);
	if ((wChairID<m_ServerAttribute.wChairCount)&&(nElapse>0L))
	{
		//设置变量
		m_nTimerID=nTimerID;
		m_nElapseCount=nElapse;
		m_wTimerChairID=wChairID;

		//设置界面
		SetTimer(nTimerID,1000,NULL);
		m_pIClientKernelSink->OnEventTimer(m_wTimerChairID,nElapse,nTimerID);

		return true;
	}

	return false;
}

//删除定时器
bool __cdecl CClientKernel::KillGameTimer(UINT nTimerID)
{
	//效验参数
	ASSERT(m_hWnd!=NULL);
	ASSERT(m_pIClientKernelSink!=NULL);
	if (m_pIClientKernelSink==NULL) return false;

	//逻辑处理
	if ((m_nTimerID!=0)&&((m_nTimerID==nTimerID)||(nTimerID==0)))
	{
		//设置界面
		KillTimer(m_nTimerID);
		if (m_wTimerChairID!=INVALID_CHAIR)
		{
			ASSERT(m_wTimerChairID<m_ServerAttribute.wChairCount);
			ASSERT(m_pUserItem[m_wTimerChairID]!=NULL);
			m_pIClientKernelSink->OnEventTimerKilled(m_wTimerChairID,m_nTimerID);
		}

		//设置变量
		m_nTimerID=0L;
		m_nElapseCount=0L;
		m_wTimerChairID=INVALID_CHAIR;

		return true;
	}

	return false;
}

//信道数据处理
bool __cdecl CClientKernel::OnChannelMessage(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend)
{
	//特殊处理
	ASSERT(m_pIClientKernelSink!=NULL);
	if (m_pIClientKernelSink==NULL) return true;
	if (m_pIClientKernelSink->OnEventProcess(*pHead,pIPCBuffer,wDataSize,hWndSend)) return true;

	//默认处理
	switch (pHead->wMainCmdID)
	{
	case IPC_MAIN_SOCKET:	//网络数据
		{
			bool bSuccess=OnIPCSocket(pHead,pIPCBuffer,wDataSize,hWndSend);
			if (bSuccess==false)
			{
				m_bInquire=false;
				m_pIClientKernelSink->CloseGameFrame(true);
			}
			return true;
		}
	case IPC_MAIN_CONFIG:	//配置信息
		{
			return OnIPCConfig(pHead,pIPCBuffer,wDataSize,hWndSend);
		}
	case IPC_MAIN_USER:		//用户信息
		{
			return OnIPCUser(pHead,pIPCBuffer,wDataSize,hWndSend);
		}
	case IPC_MAIN_CONCTROL:	//控制消息
		{
			return OnIPCControl(pHead,pIPCBuffer,wDataSize,hWndSend);
		}
	}

	return false;
}

//网络消息
bool CClientKernel::OnIPCSocket(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend)
{
	ASSERT(pHead->wMainCmdID==IPC_MAIN_SOCKET);
	switch (pHead->wSubCmdID)
	{
	case IPC_SUB_SOCKET_RECV:	//数据接收
		{
			//效验数据
			ASSERT(wDataSize>=sizeof(CMD_Command));
			if (wDataSize<sizeof(CMD_Command)) return false;

			//提取数据
			WORD wPacketSize=wDataSize-sizeof(CMD_Command);
			IPC_SocketPackage * pSocketPackage=(IPC_SocketPackage *)pIPCBuffer;

			//特殊处理
			bool bHandle=false;
			ASSERT(m_pIClientKernelSink!=NULL);
			if (m_pIClientKernelSink==NULL) return true;
			if (wPacketSize==0)	bHandle=m_pIClientKernelSink->OnEventSocket(pSocketPackage->Command,NULL,0);
			else bHandle=m_pIClientKernelSink->OnEventSocket(pSocketPackage->Command,pSocketPackage->cbBuffer,wPacketSize);
			
			//框架消息
			if ((bHandle==false)&&(pSocketPackage->Command.wMainCmdID==MDM_GF_FRAME))
			{
				switch (pSocketPackage->Command.wSubCmdID)
				{
				case SUB_GF_OPTION:			//游戏配置
					{
						//效验参数
						ASSERT(wPacketSize==sizeof(CMD_GF_Option));
						if (wPacketSize!=sizeof(CMD_GF_Option)) return false;

						//消息处理
						CMD_GF_Option * pOption=(CMD_GF_Option *)pSocketPackage->cbBuffer;
						m_bGameStatus=pOption->bGameStatus;
						m_bAllowLookon=pOption->bAllowLookon?true:false;

						return true;
					}
				case SUB_GF_SCENE:			//游戏场景
					{
						return m_pIClientKernelSink->OnEventGameScene(m_bGameStatus,m_bLookonMode,pSocketPackage->cbBuffer,wPacketSize);
					}
				case SUB_GF_LOOKON_CONTROL:	//旁观控制
					{
						//效验参数
						ASSERT(wPacketSize==sizeof(CMD_GF_LookonControl));
						if (wPacketSize!=sizeof(CMD_GF_LookonControl)) return false;

						//消息处理
						if (IsLookonMode()==true)
						{
							//变量定义
							CMD_GF_LookonControl * pLookonControl=(CMD_GF_LookonControl *)pSocketPackage->cbBuffer;

							//事件处理
							m_bAllowLookon=(pLookonControl->bAllowLookon==TRUE)?true:false;

							//旁观处理
							m_pIClientKernelSink->OnEventLookonChanged(true,NULL,0);

							//提示消息
							if (m_bAllowLookon==true) m_pIMessageProxy->InsertSystemString(TEXT("您被允许观看玩家游戏"),0);
							else m_pIMessageProxy->InsertSystemString(TEXT("您被禁止观看玩家游戏"),0);
						}

						return true;
					}
				case SUB_GF_USER_CHAT:		//聊天信息
					{
						//效验参数
						CMD_GF_UserChat * pUserChat=(CMD_GF_UserChat *)pSocketPackage->cbBuffer;
						ASSERT(wPacketSize>=(sizeof(CMD_GF_UserChat)-sizeof(pUserChat->szChatMessage)));
						ASSERT(wPacketSize==(sizeof(CMD_GF_UserChat)-sizeof(pUserChat->szChatMessage)+pUserChat->wChatLength));
						if (wPacketSize<(sizeof(CMD_GF_UserChat)-sizeof(pUserChat->szChatMessage))) return false;
						if (wPacketSize!=(sizeof(CMD_GF_UserChat)-sizeof(pUserChat->szChatMessage)+pUserChat->wChatLength)) return false;

						//寻找用户
						tagUserData * pRecvUserData=NULL;
						tagUserData * pSendUserData=SearchUserItem(pUserChat->dwSendUserID);
						if (pSendUserData==NULL) return true;
						if (pUserChat->dwTargetUserID!=0L) pRecvUserData=SearchUserItem(pUserChat->dwTargetUserID);

						//消息过滤
						if ((pUserChat->dwSendUserID!=m_dwUserID)&&(pSendUserData->cbCompanion==enCompanion_Detest)) return true;

						//显示消息
						m_pIClientKernelSink->OnUserChatMessage(pSendUserData,pRecvUserData,pUserChat->szChatMessage,pUserChat->crFontColor);

						return true;
					}
				case SUB_GF_MESSAGE:		//系统消息
					{
						//效验参数
						CMD_GF_Message * pMessage=(CMD_GF_Message *)pSocketPackage->cbBuffer;
						ASSERT(wDataSize>(sizeof(CMD_GF_Message)-sizeof(pMessage->szContent)));
						if (wDataSize<=(sizeof(CMD_GF_Message)-sizeof(pMessage->szContent))) return false;

						//消息处理
						WORD wHeadSize=sizeof(CMD_GF_Message)-sizeof(pMessage->szContent);
						ASSERT(wPacketSize==(wHeadSize+pMessage->wMessageLength*sizeof(TCHAR)));
						if (wPacketSize!=(wHeadSize+pMessage->wMessageLength*sizeof(TCHAR))) return false;
						pMessage->szContent[pMessage->wMessageLength-1]=0;

						//中断判断
						if (pMessage->wMessageType&SMT_CLOSE_GAME) 
						{
							KillGameTimer(m_nTimerID);
							m_ChannelServiceHelper->CloseChannel(true,true);
						}

						//显示消息
						if (pMessage->wMessageType&SMT_INFO) m_pIMessageProxy->InsertSystemString(pMessage->szContent,MS_NORMAL);
						if (pMessage->wMessageType&SMT_EJECT) ShowMessageBox(pMessage->szContent,MB_ICONINFORMATION);

						//关闭房间
						if (pMessage->wMessageType&SMT_CLOSE_GAME)
						{
							m_bInquire=false;
							m_pIClientKernelSink->CloseGameFrame(true);
						}

						return true;
					}
				}
			}

			//礼物消息
			if ((bHandle==false)&&(pSocketPackage->Command.wMainCmdID==MDM_GF_PRESENT))
			{
				switch (pSocketPackage->Command.wSubCmdID)
				{
				case SUB_GF_FLOWER:			//赠送鲜花
					{
						//效验参数
						ASSERT(wPacketSize==sizeof(CMD_GF_GiftNotify));
						if (wPacketSize!=sizeof(CMD_GF_GiftNotify)) return false;

						//变量定义
						CMD_GF_GiftNotify * pGiftNotify=(CMD_GF_GiftNotify *)pSocketPackage->cbBuffer;
						if(pGiftNotify->cbSendLocation!=LOCATION_GAME_ROOM)return true;

						//获取玩家
						tagUserData const *pSendUserData = NULL;
						tagUserData const *pRcvUserData = NULL;
						for ( WORD wChairID = 0; wChairID < MAX_CHAIR; ++wChairID )
						{
							tagUserData const *pUserData = GetUserInfo( wChairID );
							if ( pUserData == NULL ) continue;

							if (pUserData->dwUserID == pGiftNotify->dwSendUserID )
							{
								pSendUserData = pUserData ;

								//自己判断
								if ( pGiftNotify->dwSendUserID == pGiftNotify->dwRcvUserID ) pRcvUserData = pSendUserData;
							}
							else if ( pUserData->dwUserID == pGiftNotify->dwRcvUserID ) pRcvUserData = pUserData;

							//中断判断
							if ( pSendUserData != NULL && pRcvUserData != NULL ) break;
						}

						//旁观搜索
						if ( pSendUserData == NULL || pRcvUserData == NULL )
						{
							for (INT_PTR i=0;i<m_UserItemLookon.GetCount();i++)
							{
								tagUserData *pLookonUserData = m_UserItemLookon[i];
								if ( pLookonUserData->dwUserID == pGiftNotify->dwSendUserID )
								{
									pSendUserData = pLookonUserData;

									//自己判断
									if ( pGiftNotify->dwSendUserID == pGiftNotify->dwRcvUserID ) pRcvUserData = pSendUserData;
								}
								else if ( pLookonUserData->dwUserID == pGiftNotify->dwRcvUserID ) pRcvUserData = pLookonUserData;

								//中断判断
								if ( pSendUserData != NULL && pRcvUserData != NULL ) break;
							}
						}

						//结果判断
						ASSERT( pSendUserData != NULL );
						ASSERT( pRcvUserData != NULL );
						if ( pSendUserData == NULL || pRcvUserData == NULL ) return true;
						ASSERT( pGiftNotify->wGiftID < FLOWER_COUNT ) ;
						if ( pGiftNotify->wGiftID >= FLOWER_COUNT ) return true;

						//构造消息
						CString strGiftMsg;
						int nFlowerID = pGiftNotify->wGiftID;

						//获取指针
						CSkinRichEdit *pSkinRichEdit = m_pIMessageProxy->GetRichEdit();

						pSkinRichEdit->InsertHyperLink(pSendUserData->szName, RGB(230,249,255));

						strGiftMsg = g_FlowerDescribe[nFlowerID].szAction;
						m_pIMessageProxy->InsertShtickString(strGiftMsg,RGB(255,0,255), strGiftMsg.GetLength(), false);

						pSkinRichEdit->InsertHyperLink(pRcvUserData->szName, RGB(230,249,255));

						strGiftMsg = g_FlowerDescribe[nFlowerID].szQuantifier1;
						m_pIMessageProxy->InsertShtickString(strGiftMsg,RGB(255,0,255), strGiftMsg.GetLength(), false);

						//插入图片
						CBitmap Bitmap;
						AfxSetResourceHandle(g_GlobalOption.m_PlatformResourceModule->GetResInstance());
						if ( Bitmap.LoadBitmap(g_GlobalOption.m_ChatExpViewImage.uChatGift[nFlowerID]) )
						{
							m_pIMessageProxy->InsertImage(&Bitmap);
							Bitmap.DeleteObject();
						}
						AfxSetResourceHandle(GetModuleHandle(NULL));

						WORD wFlowerCount = pGiftNotify->wFlowerCount;
						strGiftMsg.Format(TEXT("，共%d%s%s[ %s ]"), pGiftNotify->wFlowerCount, g_FlowerDescribe[nFlowerID].szQuantifier2,
							g_FlowerDescribe[nFlowerID].szResult, pRcvUserData->szName);
						m_pIMessageProxy->InsertShtickString(strGiftMsg,RGB(255,0,255), strGiftMsg.GetLength(), false);	

						strGiftMsg.Format(g_FlowerDescribe[nFlowerID].szCharmResult, wFlowerCount * abs(CPropertyBar::m_FlowerInfo[nFlowerID].lRcvUserCharm));
						m_pIMessageProxy->InsertShtickString(strGiftMsg,RGB(255,0,255), strGiftMsg.GetLength(), true);	

						//鲜花效果
						m_pIClientKernelSink->OnEventFlower(pSendUserData, pRcvUserData, nFlowerID, nFlowerID*2);

						return true;
					}
				case SUB_GF_RESIDUAL_PROPERTY:	//剩余道具
					{
						//效验参数
						ASSERT(wPacketSize==sizeof(CMD_GF_ResidualProperty));
						if (wPacketSize!=sizeof(CMD_GF_ResidualProperty)) return false;

						//变量定义
						CMD_GF_ResidualProperty * ResidualProperty=(CMD_GF_ResidualProperty *)pSocketPackage->cbBuffer;

						//设置道具
						for ( WORD wPropID = 0; wPropID < PROPERTY_COUNT; ++wPropID )
						{
							m_pMeUserItem->dwPropResidualTime[wPropID] = ResidualProperty->dwResidualTime[wPropID];
						}

						return true;
					}
				case SUB_GF_PROP_ATTRIBUTE:		//道具属性
					{
						//校验参数
						ASSERT( wPacketSize % sizeof(tagPropertyInfo) == 0 );
						if ( wPacketSize % sizeof(tagPropertyInfo) != 0 ) return true;

						//类型转换
						int nInfoCount = wPacketSize / sizeof(tagPropertyInfo);
						tagPropertyInfo *pPropertyInfo = (tagPropertyInfo*)pSocketPackage->cbBuffer;

						//设置属性
						m_pIClientKernelSink->OnEventInitProperty(pPropertyInfo, nInfoCount);

						return true;
					}
				case SUB_GF_FLOWER_ATTRIBUTE:	//鲜花属性
					{
						//校验参数
						ASSERT( wPacketSize % sizeof(tagFlowerInfo) == 0 );
						if ( wPacketSize % sizeof(tagFlowerInfo) != 0 ) return true;

						//类型转换
						int nInfoCount = wPacketSize / sizeof(tagFlowerInfo);
						tagFlowerInfo *pFlowerInfo= (tagFlowerInfo*)pSocketPackage->cbBuffer;

						//设置属性
						m_pIClientKernelSink->OnEventInitFlower(pFlowerInfo, nInfoCount);

						return true;
					}
				case SUB_GF_PROP_BUGLE:			//喇叭消息
					{
						ASSERT( sizeof(CMD_GF_BugleProperty) == wPacketSize );
						if ( sizeof(CMD_GF_BugleProperty) != wPacketSize ) return false;

						//类型转换
						CMD_GF_BugleProperty *pBugleProperty = (CMD_GF_BugleProperty*)pSocketPackage->cbBuffer;

						//喇叭图片
						CBitmap Bitmap;
						AfxSetResourceHandle(g_GlobalOption.m_PlatformResourceModule->GetResInstance());
						if ( Bitmap.LoadBitmap(g_GlobalOption.m_ChatExpViewImage.uChatBugle) )
						{
							m_pIMessageProxy->InsertImage(&Bitmap);
							Bitmap.DeleteObject();
						}
						AfxSetResourceHandle(GetModuleHandle(NULL));

						//喇叭内容
						m_pIMessageProxy->InsertUserChat(pBugleProperty->szUserName,pBugleProperty->szContext,pBugleProperty->crText,MS_NORMAL);

						return true;
					}
				}
			}

			return bHandle;
		}
	}

	return false;
}

//配置信息
bool CClientKernel::OnIPCConfig(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend)
{
	ASSERT(pHead->wMainCmdID==IPC_MAIN_CONFIG);
	switch (pHead->wSubCmdID)
	{
	case IPC_SUB_SERVER_INFO:	//房间信息
		{
			//效验参数
			ASSERT(wDataSize>=sizeof(IPC_GF_ServerInfo));
			if (wDataSize<sizeof(IPC_GF_ServerInfo)) return false;

			//处理数据
			IPC_GF_ServerInfo * pServerInfo=(IPC_GF_ServerInfo *)pIPCBuffer;
			m_wTableID=pServerInfo->wTableID;
			m_wChairID=pServerInfo->wChairID;
			m_dwUserID=pServerInfo->dwUserID;
			m_ServerAttribute.wKindID=pServerInfo->wKindID;
			m_ServerAttribute.wServerID=pServerInfo->wServerID;
			m_ServerAttribute.wGameGenre=pServerInfo->wGameGenre;
			m_ServerAttribute.wChairCount=pServerInfo->wChairCount;
			m_ServerAttribute.dwVideoAddr=pServerInfo->dwVideoAddr;
			m_ServerAttribute.cbHideUserInfo=pServerInfo->cbHideUserInfo;
			lstrcpyn(m_ServerAttribute.szKindName,pServerInfo->szKindName,CountArray(m_ServerAttribute.szKindName));
			lstrcpyn(m_ServerAttribute.szServerName,pServerInfo->szServerName,CountArray(m_ServerAttribute.szServerName));

			//设置视频
			CVideoServiceManager * pVideoServiceManager=CVideoServiceManager::GetInstance();
			if ( pVideoServiceManager!=NULL) pVideoServiceManager->SetVideoServiceInfo(pServerInfo->dwUserID,htonl(pServerInfo->dwVideoAddr),PORT_VIDEO_SERVER);

			//更新标题
			UpdateGameTitle();

			return true;
		}
	}
	return false;
}

//用户通知
bool CClientKernel::OnIPCUser(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend)
{
	ASSERT(pHead->wMainCmdID==IPC_MAIN_USER);
	switch (pHead->wSubCmdID)
	{
	case IPC_SUB_USER_COME:		//用户消息
		{
			//效验参数
			ASSERT(wDataSize>=sizeof(tagUserInfoHead));
			if (wDataSize<sizeof(tagUserInfoHead)) return false;

			//读取基本信息
			tagUserData UserData;
			memset(&UserData,0,sizeof(UserData));
			tagUserInfoHead * pUserInfoHead=(tagUserInfoHead *)pIPCBuffer;
			UserData.wFaceID=pUserInfoHead->wFaceID;
			UserData.dwCustomFaceVer=pUserInfoHead->dwCustomFaceVer;
			UserData.wTableID=pUserInfoHead->wTableID;
			UserData.wChairID=pUserInfoHead->wChairID;
			UserData.cbGender=pUserInfoHead->cbGender;
			UserData.cbUserStatus=pUserInfoHead->cbUserStatus;
			UserData.cbMemberOrder=pUserInfoHead->cbMemberOrder;
			UserData.cbMasterOrder=pUserInfoHead->cbMasterOrder;
			UserData.dwUserID=pUserInfoHead->dwUserID;
			UserData.dwGameID=pUserInfoHead->dwGameID;
			UserData.dwGroupID=pUserInfoHead->dwGroupID;
			UserData.dwUserRight=pUserInfoHead->dwUserRight;
			UserData.lLoveliness=pUserInfoHead->lLoveliness;
			UserData.dwMasterRight=pUserInfoHead->dwMasterRight;
			UserData.lScore=pUserInfoHead->UserScoreInfo.lScore;
			UserData.lGameGold=pUserInfoHead->UserScoreInfo.lGameGold;
			UserData.lInsureScore=pUserInfoHead->UserScoreInfo.lInsureScore;
			UserData.lWinCount=pUserInfoHead->UserScoreInfo.lWinCount;
			UserData.lLostCount=pUserInfoHead->UserScoreInfo.lLostCount;
			UserData.lDrawCount=pUserInfoHead->UserScoreInfo.lDrawCount;
			UserData.lFleeCount=pUserInfoHead->UserScoreInfo.lFleeCount;
			UserData.lExperience=pUserInfoHead->UserScoreInfo.lExperience;
			//UserData.lInsureScore=pUserInfoHead->lInsureScore;
			for ( WORD wPropID = 0; wPropID < PROPERTY_COUNT; ++wPropID )
			{
				UserData.dwPropResidualTime[wPropID] = pUserInfoHead->dwPropResidualTime[wPropID];
			}

			//读取扩展信息
			void * pDataBuffer=NULL;
			tagDataDescribe DataDescribe;
			CRecvPacketHelper RecvPacket(pUserInfoHead+1,wDataSize-sizeof(tagUserInfoHead));
			while (true)
			{
				pDataBuffer=RecvPacket.GetData(DataDescribe);
				if (DataDescribe.wDataDescribe==DTP_NULL) break;
				switch (DataDescribe.wDataDescribe)
				{
				case DTP_USER_ACCOUNTS:		//用户帐户
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize<=sizeof(UserData.szName));
						if (DataDescribe.wDataSize<=sizeof(UserData.szName))
						{
							CopyMemory(&UserData.szName,pDataBuffer,DataDescribe.wDataSize);
							UserData.szName[CountArray(UserData.szName)-1]=0;
						}
						break;
					}
				case DTP_UNDER_WRITE:		//个性签名
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize<=sizeof(UserData.szUnderWrite));
						if (DataDescribe.wDataSize<=sizeof(UserData.szUnderWrite))
						{
							CopyMemory(&UserData.szUnderWrite,pDataBuffer,DataDescribe.wDataSize);
							UserData.szUnderWrite[CountArray(UserData.szUnderWrite)-1]=0;
						}
						break;
					}
				case DTP_USER_GROUP_NAME:	//用户社团
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize<=sizeof(UserData.szGroupName));
						if (DataDescribe.wDataSize<=sizeof(UserData.szGroupName))
						{
							CopyMemory(&UserData.szGroupName,pDataBuffer,DataDescribe.wDataSize);
							UserData.szGroupName[CountArray(UserData.szGroupName)-1]=0;
						}
						break;
					}
				case DTP_USER_COMPANION:	//用户关系
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize<=sizeof(UserData.cbCompanion));
						if (DataDescribe.wDataSize<=sizeof(UserData.cbCompanion))
						{
							CopyMemory(&UserData.cbCompanion,pDataBuffer,DataDescribe.wDataSize);
						}
						break;
					}
				}
			}

			//增加用户
			ActiveUserItem(&UserData);

			return true;
		}
	case IPC_SUB_USER_STATUS:	//用户状态
		{
			//效验参数
			ASSERT(wDataSize>=sizeof(IPC_UserStatus));
			if (wDataSize<sizeof(IPC_UserStatus)) return false;

			//消息处理
			IPC_UserStatus * pUserStatus=(IPC_UserStatus *)pIPCBuffer;
			if (pUserStatus->cbUserStatus<US_SIT) 
			{
				if (pUserStatus->dwUserID==m_dwUserID)
				{
					ResetClientKernel();
					return true;
				}
				else DeleteUserItem(pUserStatus->dwUserID);
			}
			else UpdateUserItem(pUserStatus->dwUserID,pUserStatus->cbUserStatus,pUserStatus->wNetDelay);

			return true;
		}
	case IPC_SUB_USER_SCORE:	//用户积分
		{
			//效验参数
			ASSERT(wDataSize>=sizeof(IPC_UserScore));
			if (wDataSize<sizeof(IPC_UserScore)) return false;

			//消息处理
			IPC_UserScore * pUserScore=(IPC_UserScore *)pIPCBuffer;
			tagUserData *pUserData = SearchUserItem(pUserScore->dwUserID);
			pUserData->lLoveliness = pUserScore->lLoveliness;
			//pUserData->lInsureScore = pUserScore->lInsureScore;
			UpdateUserItem(pUserScore->dwUserID,&pUserScore->UserScore);

			return true;
		}
	case IPC_SUB_MEMBERORDER:	//会员等级
		{
			//效验参数
			ASSERT(wDataSize>=sizeof(IPC_MemberOrder));
			if (wDataSize<sizeof(IPC_MemberOrder)) return false;

			//消息处理
			IPC_MemberOrder * pMemberOrder=(IPC_MemberOrder *)pIPCBuffer;
			tagUserData *pUserData = SearchUserItem(pMemberOrder->dwUserID);
			pUserData->cbMemberOrder = pMemberOrder->cbMemberOrder;

			//更新界面
			m_pIClientKernelSink->OnEventUserMemberOrder(pUserData,pUserData->wChairID,false);

			return true;
		}
	case IPC_SUB_GAME_START:	//游戏开始
		{
			//设置用户
			for (WORD i=0;i<m_ServerAttribute.wChairCount;i++)
			{
				if (m_pUserItem[i]!=NULL) 
				{
					m_pUserItem[i]->cbUserStatus=US_PLAY;
					m_pIClientKernelSink->OnEventUserStatus(m_pUserItem[i],m_pUserItem[i]->wChairID,false);
				}
			}

			return true;
		}
	case IPC_SUB_GAME_FINISH:	//游戏结束
		{
			//设置用户
			for (WORD i=0;i<m_ServerAttribute.wChairCount;i++)
			{
				if (m_pUserItem[i]!=NULL)
				{
					m_pUserItem[i]->cbUserStatus=US_SIT;
					m_pIClientKernelSink->OnEventUserStatus(m_pUserItem[i],m_pUserItem[i]->wChairID,false);
				}
			}

			return true;
		}
	case IPC_SUB_UPDATE_FACE:	//更新头像
		{
			//效验参数
			ASSERT(wDataSize>=sizeof(IPC_UpdateFace));
			if (wDataSize<sizeof(IPC_UpdateFace)) return false;

			//消息处理
			IPC_UpdateFace * pUpdateFace=(IPC_UpdateFace *)pIPCBuffer;
			tagUserData *pUserData = SearchUserItem(m_dwUserID);
			pUserData->dwCustomFaceVer = pUpdateFace->dwCustomFace;

			return true;
		}
	}

	return false;
}

//控制通知
bool CClientKernel::OnIPCControl(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend)
{
	ASSERT(pHead->wMainCmdID==IPC_MAIN_CONCTROL);
	switch (pHead->wSubCmdID)
	{
	case IPC_SUB_START_FINISH:		//启动完成
		{
			//获取场景
			CMD_GF_Info Info;
			ZeroMemory(&Info,sizeof(Info));
			Info.bAllowLookon=(m_bAllowUserLookon==true)?TRUE:FALSE;

			//发送消息
			SendSocketData(MDM_GF_FRAME,SUB_GF_INFO,&Info,sizeof(Info));

			return true;
		}
	case IPC_SUB_CLOSE_FRAME:		//关闭框架
		{
			m_bInquire=false;
			m_ChannelServiceHelper->CloseChannel(false,false);
			if (m_pIClientKernelSink!=NULL) m_pIClientKernelSink->CloseGameFrame(true);
			return true;
		}
	}

	return false;
}

//查找用户
tagUserData * CClientKernel::SearchUserItem(DWORD dwUserID)
{
	//变量定义
	WORD wIndex=0;
	tagUserData * pUserData=NULL;

	//寻找用户
	do
	{
		pUserData=m_UserItemFactory.EnumActiveObject(wIndex++);
		if (pUserData==NULL) break;
		if (pUserData->dwUserID==dwUserID) return pUserData;
	} while (true);

	return NULL;
}

//枚举玩家
tagUserData * CClientKernel::EnumLookonUser(WORD wIndex)
{
	if ( wIndex >= m_UserItemLookon.GetCount() ) return NULL;

	return m_UserItemLookon[wIndex];
}

//增加用户
bool CClientKernel::ActiveUserItem(const tagUserData * pUserData)
{
	//效验参数
	ASSERT(pUserData!=NULL);
	ASSERT(pUserData->wTableID!=INVALID_TABLE);
	ASSERT(pUserData->wChairID<m_ServerAttribute.wChairCount);
	if (pUserData->wTableID==INVALID_TABLE) return false;
	if (pUserData->wChairID>=m_ServerAttribute.wChairCount) return false;

	//插入用户
	tagUserData * pActiveUserData=NULL;
	pActiveUserData=m_UserItemFactory.ActiveItem();
	ASSERT(pActiveUserData!=NULL);
	if (pActiveUserData==NULL) return false;
	CopyMemory(pActiveUserData,pUserData,sizeof(tagUserData));

	//设置变量
	bool bLookonMode=(pActiveUserData->cbUserStatus==US_LOOKON);
	if (bLookonMode==false)
	{
		ASSERT(m_pUserItem[pUserData->wChairID]==NULL);
		m_pUserItem[pUserData->wChairID]=pActiveUserData;
	}
	else m_UserItemLookon.Add(pActiveUserData);

	//判断自己
	if (m_dwUserID==pActiveUserData->dwUserID)
	{
		m_bLookonMode=bLookonMode;
		m_pMeUserItem=pActiveUserData;
		m_wTableID=m_pMeUserItem->wTableID;
		m_wChairID=m_pMeUserItem->wChairID;
	}	

	//通知改变
	ASSERT(m_pIClientKernelSink!=NULL);
	if (m_pIClientKernelSink!=NULL) m_pIClientKernelSink->OnEventUserEnter(pActiveUserData,pActiveUserData->wChairID,bLookonMode);

	//判断自己,此调用需在OnEventUserEnter之后设定好视频的m_dwLocalUserID，否则判断失败
	if (m_dwUserID==pActiveUserData->dwUserID)
	{
		//激活视频
		CVideoServiceManager * pVideoServiceManager=CVideoServiceManager::GetInstance();
		if (pVideoServiceManager!=NULL) pVideoServiceManager->OnShowLocalVD(IsLookonMode()==false);
	}

	return true;
}

//删除用户
bool CClientKernel::DeleteUserItem(DWORD dwUserID)
{
	//游戏用户
	tagUserData * pUserData=NULL;
	for (WORD i=0;i<m_ServerAttribute.wChairCount;i++)
	{
		pUserData=m_pUserItem[i];
		if ((pUserData!=NULL)&&(pUserData->dwUserID==dwUserID))
		{
			//设置变量
			m_pUserItem[i]=NULL;
			m_UserItemFactory.FreeItem(pUserData);

			//通知改变
			ASSERT(m_pIClientKernelSink!=NULL);
			if (m_pIClientKernelSink!=NULL) m_pIClientKernelSink->OnEventUserLeave(pUserData,pUserData->wChairID,false);

			return true;
		}
	}

	//旁观用户
	for (INT_PTR i=0;i<m_UserItemLookon.GetCount();i++)
	{
		pUserData=m_UserItemLookon[i];
		if (pUserData->dwUserID==dwUserID)
		{
			//设置变量
			m_UserItemLookon.RemoveAt(i);
			m_UserItemFactory.FreeItem(pUserData);

			//判断自己
			if (m_dwUserID==dwUserID)
			{
				m_pMeUserItem=NULL;
				m_wTableID=INVALID_TABLE;
				m_wChairID=INVALID_CHAIR;
			}

			//通知改变
			ASSERT(m_pIClientKernelSink!=NULL);
			if (m_pIClientKernelSink!=NULL) m_pIClientKernelSink->OnEventUserLeave(pUserData,pUserData->wChairID,true);

			return true;
		}
	}

	return false;
}

//更新用户
bool CClientKernel::UpdateUserItem(DWORD dwUserID, const tagUserScore * pUserScore)
{
	//寻找用户
	tagUserData * pUserData=SearchUserItem(dwUserID);
	if (pUserData==NULL) return false;

	//设置数据
	pUserData->lScore=pUserScore->lScore;
	pUserData->lGameGold=pUserScore->lGameGold;
	pUserData->lInsureScore=pUserScore->lInsureScore;
	pUserData->lWinCount=pUserScore->lWinCount;
	pUserData->lLostCount=pUserScore->lLostCount;
	pUserData->lDrawCount=pUserScore->lDrawCount;
	pUserData->lFleeCount=pUserScore->lFleeCount;
	pUserData->lExperience=pUserScore->lExperience;

	//通知改变
	ASSERT(m_pIClientKernelSink!=NULL);
	bool bLookonUser=(pUserData->cbUserStatus==US_LOOKON);
	if (m_pIClientKernelSink!=NULL) m_pIClientKernelSink->OnEventUserScore(pUserData,pUserData->wChairID,bLookonUser);

	return true;
}

//更新用户
bool CClientKernel::UpdateUserItem(DWORD dwUserID, BYTE cbUserStatus, WORD wNetDelay)
{
	//寻找用户
	tagUserData * pUserData=SearchUserItem(dwUserID);
	if (pUserData==NULL) return false;

	//设置数据
	pUserData->cbUserStatus=cbUserStatus;

	//通知改变
	ASSERT(m_pIClientKernelSink!=NULL);
	bool bLookonUser=(cbUserStatus==US_LOOKON);
	if (m_pIClientKernelSink!=NULL) m_pIClientKernelSink->OnEventUserStatus(pUserData,pUserData->wChairID,bLookonUser);

	return false;
}

//更新标题
void CClientKernel::UpdateGameTitle()
{
	//变量定义
	TCHAR szTitle[128]=TEXT("");

	//获取文件
	TCHAR szProcessName[MAX_PATH]=TEXT("");
	GetModuleFileName(AfxGetInstanceHandle(),szProcessName,CountArray(szProcessName));

	//获取信息
	CWinFileInfo WinFileInfo;
	WinFileInfo.OpenWinFile(szProcessName);

	//构造标题
	if ((m_ServerAttribute.szServerName[0]!=0)&&(m_wTableID!=INVALID_TABLE)&&(m_ServerAttribute.cbHideUserInfo==FALSE))
	{
		_sntprintf(szTitle,CountArray(szTitle),TEXT("%s ［ %s - 第 %d 桌 ］"),WinFileInfo.GetProductName(),m_ServerAttribute.szServerName,m_wTableID+1);
	}
	else
	{
		_sntprintf(szTitle,CountArray(szTitle),TEXT("%s － [ Ver：%s ]"),WinFileInfo.GetProductName(),WinFileInfo.GetProductVersion());
	}

	//设置标题
	AfxGetMainWnd()->SetWindowText(szTitle);

	return;
}

//内核重置
void CClientKernel::ResetClientKernel()
{
	//删除定时器
	if (m_nTimerID!=0)
	{
		KillTimer(m_nTimerID);
		m_nTimerID=0L;
		m_nElapseCount=0L;
		m_wTimerChairID=INVALID_CHAIR;
	}

	//旁观用户
	for (INT_PTR i=0;i<m_UserItemLookon.GetCount();i++)
	{
		m_UserItemFactory.FreeItem(m_UserItemLookon[i]);
	}
	m_UserItemLookon.RemoveAll();

	//游戏用户
	for (WORD i=0;i<m_ServerAttribute.wChairCount;i++)
	{
		if (m_pUserItem[i]!=NULL)
		{
			m_UserItemFactory.FreeItem(m_pUserItem[i]);
			m_pUserItem[i]=NULL;
		}
	}

	//设置变量
	m_bLookonMode=true;
	m_pMeUserItem=NULL;
	m_bGameStatus=US_FREE;
	m_wTableID=INVALID_TABLE;
	m_wChairID=INVALID_CHAIR;

	//清理界面
	if (m_pIClientKernelSink!=NULL) m_pIClientKernelSink->ResetGameFrame();

	return;
}

//时间消息
void CClientKernel::OnTimer(UINT nIDEvent)
{
	//时间处理
	if ((m_nTimerID==nIDEvent)&&(m_wTimerChairID!=INVALID_CHAIR))
	{
		//消息处理
		if (m_nElapseCount>0) m_nElapseCount--;
		bool bSuccess=m_pIClientKernelSink->OnEventTimer(m_wTimerChairID,m_nElapseCount,m_nTimerID);
		if ((m_nElapseCount==0)||(bSuccess==false)) KillGameTimer(m_nTimerID);

		return;
	}

	__super::OnTimer(nIDEvent);
}

//////////////////////////////////////////////////////////////////////////

//建立对象函数
extern "C" __declspec(dllexport) void * __cdecl CreateClientKernel(const GUID & Guid, DWORD dwInterfaceVer)
{
	//建立对象
	CClientKernel * pClientKernel=NULL;
	try
	{
		pClientKernel=new CClientKernel();
		if (pClientKernel==NULL) throw TEXT("创建失败");
		void * pObject=pClientKernel->QueryInterface(Guid,dwInterfaceVer);
		if (pObject==NULL) throw TEXT("接口查询失败");
		return pObject;
	}
	catch (...) {}

	//清理对象
	SafeDelete(pClientKernel);
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
