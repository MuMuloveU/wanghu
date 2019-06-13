#include "StdAfx.h"
#include "GlobalUnits.h"
#include "ManagerClass.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CManagerServer, CSkinDialogEx)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CManagerMessage, CSkinDialogEx)
	ON_BN_CLICKED(IDC_PRIVIEW, OnPreViewMessage)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CManagerUserRight, CSkinDialogEx)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CManagerSendWarning, CSkinDialogEx)
	ON_BN_CLICKED(IDC_PRIVIEW, OnPreViewMessage)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CManagerSendWarning::CManagerSendWarning() : CSkinDialogEx(IDD_SEND_WARNING)
{
	//设置变量
	m_dwUserID=0L;
	m_pIUserItem=NULL;

	//设置接口
	m_pIClientSocket=NULL;
	m_pIClientUserManager=NULL;

}

//析构函数
CManagerSendWarning::~CManagerSendWarning()
{
}

//控件绑定
void CManagerSendWarning::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btSend);
	DDX_Control(pDX, IDCANCEL, m_btClose);
	DDX_Control(pDX, IDC_PRIVIEW, m_btPreView);
	DDX_Control(pDX, IDC_WARNING_MESSAGE, m_WarningMessage);
}

//初始化函数
BOOL CManagerSendWarning::OnInitDialog()
{
	__super::OnInitDialog();

	//设置标题
	SetWindowText(TEXT("发送警告："));

	return TRUE;
}

//确定函数
void CManagerSendWarning::OnOK()
{
	//调整用户
	if ((m_pIUserItem==NULL)||(m_pIUserItem->GetUserID()!=m_dwUserID))
	{
		//寻找用户
		ASSERT(m_pIClientUserManager!=NULL);
		m_pIUserItem=m_pIClientUserManager->SearchUserByUserID(m_dwUserID);

		//用户判断
		if (m_pIUserItem==NULL)
		{
			ShowInformation(TEXT("目标用户已经离开了游戏房间，无法向此用户发送警告通知！"),0,MB_ICONQUESTION);
			return;
		}
	}

	//构造数据
	CMD_GR_SendWarning SendWarning;
	SendWarning.dwTargetUserID=m_dwUserID;
	m_WarningMessage.GetWindowText(SendWarning.szWarningMessage,CountArray(SendWarning.szWarningMessage));
	SendWarning.wChatLength=CountStringBuffer(SendWarning.szWarningMessage);

	//消息判断
	if (SendWarning.szWarningMessage[0]==0)
	{
		
		ShowInformation(TEXT("警告消息不能为空，请写入警告消息内容！"),0,MB_ICONQUESTION);
		m_WarningMessage.SetFocus();
		return;
	}

	//安全控制
	SendWarning.szWarningMessage[CountArray(SendWarning.szWarningMessage)-1]=0;

	//发送消息
	ASSERT(m_pIClientSocket!=NULL);
	WORD wSendSize=sizeof(SendWarning)-sizeof(SendWarning.szWarningMessage)+SendWarning.wChatLength;
	m_pIClientSocket->SendData(MDM_GR_MANAGER,SUB_GR_SEND_WARNING,&SendWarning,wSendSize);

	//拷贝数据
	g_GlobalUnits.CopyToClipboard(SendWarning.szWarningMessage);

	//设置界面
	m_WarningMessage.SetFocus();
	m_WarningMessage.SetWindowText(TEXT(""));

	return;
}

//取消消息
void CManagerSendWarning::OnCancel()
{
	//销毁窗口
	DestroyWindow();

	return;
}

//设置用户
bool CManagerSendWarning::SetUserItem(IUnknownEx * pIUnknownEx)
{
	//查询接口
	m_pIUserItem=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IUserItem);
	if (m_pIUserItem==NULL) return false;

	//设置变量
	m_dwUserID=m_pIUserItem->GetUserID();

	//更新界面
	SetDlgItemInt(IDC_USER_ID,m_dwUserID,FALSE);
	SetDlgItemText(IDC_ACCOUNTS,m_pIUserItem->GetUserName());

	return true;
}

//设置接口
bool CManagerSendWarning::SetClientSocket(IUnknownEx * pIUnknownEx)
{
	//查询接口
	m_pIClientSocket=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITCPSocket);
	if (m_pIClientSocket==NULL) return false;

	return true;
}

//设置接口
bool CManagerSendWarning::SetClientUserManager(IUnknownEx * pIUnknownEx)
{
	//查询接口
	m_pIClientUserManager=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IClientUserManager);
	if (m_pIClientUserManager==NULL) return false;

	return true;
}

//预览消息
void CManagerSendWarning::OnPreViewMessage()
{
	//获取消息
	TCHAR szMessage[MAX_CHAT_LEN]=TEXT("");
	m_WarningMessage.GetWindowText(szMessage,CountArray(szMessage));

	//消息判断
	if (szMessage[0]==0)
	{
		
		ShowInformation(TEXT("警告消息不能为空，请写入警告消息内容！"),0,MB_ICONQUESTION);
		m_WarningMessage.SetFocus();
		return;
	}

	//预览消息
	ShowInformationEx(szMessage,0,MB_ICONQUESTION,TEXT("警告预览："));
	m_WarningMessage.SetFocus();

	return;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CManagerUserRight::CManagerUserRight() : CSkinDialogEx(IDD_MANAGER_RIGHT)
{
	//设置变量
	m_dwUserID=0L;
	m_pIUserItem=NULL;

	//权限变量
	m_dwUserRight=0L;
	//m_dwLastUserRight=0L;
	m_dwMasterRight=0L;

	//设置接口
	m_pIClientSocket=NULL;
	m_pIClientUserManager=NULL;

}

//析构函数
CManagerUserRight::~CManagerUserRight()
{
}

//控件绑定
void CManagerUserRight::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btOk);
	DDX_Control(pDX, IDCANCEL, m_btClose);
}

//初始化函数
BOOL CManagerUserRight::OnInitDialog()
{
	__super::OnInitDialog();

	//设置标题
	SetWindowText(TEXT("权限管理："));

	//更新控件
	UpdateRightData();
	UpdateMasterControl();

	return TRUE;
}

//确定函数
void CManagerUserRight::OnOK()
{
	//调整用户
	if ((m_pIUserItem==NULL)||(m_pIUserItem->GetUserID()!=m_dwUserID))
	{
		//寻找用户
		ASSERT(m_pIClientUserManager!=NULL);
		m_pIUserItem=m_pIClientUserManager->SearchUserByUserID(m_dwUserID);

		//用户判断
		if (m_pIUserItem==NULL)
		{
			
			ShowInformation(TEXT("目标用户已经离开了游戏房间，用户权限设置失败！"),0,MB_ICONQUESTION);
			return;
		}
	}

	//构造数据
	CMD_GR_SetUserRight SetUserRight;
	SetUserRight.dwTargetUserID=m_dwUserID;
	SetUserRight.cbGameRight=(((CButton *)GetDlgItem(IDC_BIND_GAME))->GetCheck()==BST_CHECKED)?TRUE:FALSE;
	SetUserRight.cbAccountsRight=(((CButton *)GetDlgItem(IDC_BIND_USER))->GetCheck()==BST_CHECKED)?TRUE:FALSE;
	SetUserRight.cbLimitRoomChat=(((CButton *)GetDlgItem(IDC_ROOM_CHAT))->GetCheck()==BST_CHECKED)?TRUE:FALSE;
	SetUserRight.cbLimitGameChat=(((CButton *)GetDlgItem(IDC_GAME_CHAT))->GetCheck()==BST_CHECKED)?TRUE:FALSE;
	SetUserRight.cbLimitPlayGame=(((CButton *)GetDlgItem(IDC_PLAY_GAME))->GetCheck()==BST_CHECKED)?TRUE:FALSE;
	SetUserRight.cbLimitSendWisper=(((CButton *)GetDlgItem(IDC_SEND_WISPER))->GetCheck()==BST_CHECKED)?TRUE:FALSE;
	SetUserRight.cbLimitLookonGame=(((CButton *)GetDlgItem(IDC_LOOKON_GAME))->GetCheck()==BST_CHECKED)?TRUE:FALSE;

	//发送消息
	ASSERT(m_pIClientSocket!=NULL);
	m_pIClientSocket->SendData(MDM_GR_MANAGER,SUB_GR_SET_USER_RIGHT,&SetUserRight,sizeof(SetUserRight));

	//关闭窗口
	DestroyWindow();

	return;
}

//取消消息
void CManagerUserRight::OnCancel()
{
	//销毁窗口
	DestroyWindow();

	return;
}

//设置权限
bool CManagerUserRight::SetMasterRight(DWORD dwMasterRight)
{
	//设置变量
	m_dwMasterRight=dwMasterRight;

	//更新控件
	UpdateMasterControl();

	return true;
}

//设置用户
bool CManagerUserRight::SetUserItem(IUnknownEx * pIUnknownEx)
{
	//查询接口
	m_pIUserItem=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IUserItem);
	if (m_pIUserItem==NULL) return false;

	//设置变量
	m_dwUserID=m_pIUserItem->GetUserID();
	//m_dwLastUserRight=m_dwUserRight;
	m_dwUserRight=m_pIUserItem->GetUserData()->dwUserRight;

	//更新界面
	SetDlgItemInt(IDC_USER_ID,m_dwUserID,FALSE);
	SetDlgItemText(IDC_ACCOUNTS,m_pIUserItem->GetUserName());

	//更新数据
	UpdateRightData();

	return true;
}

//设置接口
bool CManagerUserRight::SetClientSocket(IUnknownEx * pIUnknownEx)
{
	//查询接口
	m_pIClientSocket=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITCPSocket);
	if (m_pIClientSocket==NULL) return false;

	return true;
}

//设置接口
bool CManagerUserRight::SetClientUserManager(IUnknownEx * pIUnknownEx)
{
	//查询接口
	m_pIClientUserManager=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IClientUserManager);
	if (m_pIClientUserManager==NULL) return false;

	return true;
}

//更新数据
bool CManagerUserRight::UpdateRightData()
{
	//设置控件
	if (m_hWnd!=NULL)
	{
		((CButton *)GetDlgItem(IDC_PLAY_GAME))->SetCheck((CUserRight::CanPlay(m_dwUserRight)==true)?BST_UNCHECKED:BST_CHECKED);
		((CButton *)GetDlgItem(IDC_LOOKON_GAME))->SetCheck((CUserRight::CanLookon(m_dwUserRight)==true)?BST_UNCHECKED:BST_CHECKED);
		((CButton *)GetDlgItem(IDC_ROOM_CHAT))->SetCheck((CUserRight::CanRoomChat(m_dwUserRight)==true)?BST_UNCHECKED:BST_CHECKED);
		((CButton *)GetDlgItem(IDC_GAME_CHAT))->SetCheck((CUserRight::CanGameChat(m_dwUserRight)==true)?BST_UNCHECKED:BST_CHECKED);
		((CButton *)GetDlgItem(IDC_SEND_WISPER))->SetCheck((CUserRight::CanWisper(m_dwUserRight)==true)?BST_UNCHECKED:BST_CHECKED);
	}

	return true;
}

//更新控制
bool CManagerUserRight::UpdateMasterControl()
{
	//设置控件
	if (m_hWnd!=NULL)
	{
		//设置控制
		if (CUserRight::CanBindGame(m_dwMasterRight)==true) GetDlgItem(IDC_BIND_GAME)->EnableWindow(TRUE);
		if (CUserRight::CanLimitPlay(m_dwMasterRight)==true) GetDlgItem(IDC_PLAY_GAME)->EnableWindow(TRUE);
		if (CUserRight::CanBindGlobal(m_dwMasterRight)==true) GetDlgItem(IDC_BIND_USER)->EnableWindow(TRUE);
		if (CUserRight::CanLimitRoomChat(m_dwMasterRight)==true) GetDlgItem(IDC_ROOM_CHAT)->EnableWindow(TRUE);
		if (CUserRight::CanLimitGameChat(m_dwMasterRight)==true) GetDlgItem(IDC_GAME_CHAT)->EnableWindow(TRUE);
		if (CUserRight::CanLimitWisper(m_dwMasterRight)==true) GetDlgItem(IDC_SEND_WISPER)->EnableWindow(TRUE);
		if (CUserRight::CanLimitLookon(m_dwMasterRight)==true) GetDlgItem(IDC_LOOKON_GAME)->EnableWindow(TRUE);

		//启用控件
		if (m_dwMasterRight!=0) m_btOk.EnableWindow(TRUE);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CManagerMessage::CManagerMessage() : CSkinDialogEx(IDD_MANAGER_MESSAGE)
{
	//设置接口
	m_pIClientSocket=NULL;

	return;
}

//析构函数
CManagerMessage::~CManagerMessage()
{
}

//控件绑定
void CManagerMessage::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btSend);
	DDX_Control(pDX, IDCANCEL, m_btClose);
	DDX_Control(pDX, IDC_PRIVIEW, m_btPreView);
	DDX_Control(pDX, IDC_SYSTEM_MESSAGE, m_SystemMessage);
}

//初始化函数
BOOL CManagerMessage::OnInitDialog()
{
	__super::OnInitDialog();

	//设置标题
	SetWindowText(TEXT("系统消息："));

	return TRUE;
}

//确定函数
void CManagerMessage::OnOK()
{
	//构造数据
	CMD_GR_SendMessage SendMessage;
	SendMessage.cbGame=(((CButton *)GetDlgItem(IDC_GAME_MESSAGE))->GetCheck()==BST_CHECKED)?TRUE:FALSE;
	SendMessage.cbRoom=(((CButton *)GetDlgItem(IDC_ROOM_MESSAGE))->GetCheck()==BST_CHECKED)?TRUE:FALSE;
	m_SystemMessage.GetWindowText(SendMessage.szSystemMessage,CountArray(SendMessage.szSystemMessage));
	SendMessage.wChatLength=CountStringBuffer(SendMessage.szSystemMessage);

	//消息判断
	if (SendMessage.szSystemMessage[0]==0)
	{
		
		ShowInformation(TEXT("系统消息不能为空，请写入系统消息内容！"),0,MB_ICONQUESTION);
		m_SystemMessage.SetFocus();
		return;
	}

	//发送判断
	if ((SendMessage.cbGame==FALSE)&&(SendMessage.cbRoom==FALSE))
	{
		
		ShowInformation(TEXT("发送失败，请选择系统消息的类型！"),0,MB_ICONQUESTION);
		return;
	}

	//安全控制
	SendMessage.szSystemMessage[CountArray(SendMessage.szSystemMessage)-1]=0;

	//发送消息
	ASSERT(m_pIClientSocket!=NULL);
	WORD wSendSize=sizeof(SendMessage)-sizeof(SendMessage.szSystemMessage)+SendMessage.wChatLength;
	m_pIClientSocket->SendData(MDM_GR_MANAGER,SUB_GR_SEND_MESSAGE,&SendMessage,wSendSize);

	//拷贝数据
	g_GlobalUnits.CopyToClipboard(SendMessage.szSystemMessage);

	//设置界面
	m_SystemMessage.SetFocus();
	m_SystemMessage.SetWindowText(TEXT(""));

	return;
}

//取消消息
void CManagerMessage::OnCancel()
{
	//销毁窗口
	DestroyWindow();

	return;
}

//设置接口
bool CManagerMessage::SetClientSocket(IUnknownEx * pIUnknownEx)
{
	//查询接口
	m_pIClientSocket=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITCPSocket);
	if (m_pIClientSocket==NULL) return false;

	return true;
}

//预览消息
void CManagerMessage::OnPreViewMessage()
{
	//获取消息
	TCHAR szMessage[MAX_CHAT_LEN]=TEXT("");
	m_SystemMessage.GetWindowText(szMessage,CountArray(szMessage));

	//消息判断
	if (szMessage[0]==0)
	{
		
		ShowInformation(TEXT("系统消息不能为空，请写入系统消息内容！"),0,MB_ICONQUESTION);
		m_SystemMessage.SetFocus();
		return;
	}

	//预览消息
	ShowInformationEx(szMessage,0,MB_ICONQUESTION,TEXT("消息预览："));
	m_SystemMessage.SetFocus();

	return;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CManagerServer::CManagerServer() : CSkinDialogEx(IDD_MANAGER_SERVER)
{
	//设置接口
	m_pIClientSocket=NULL;

	return;
}

//析构函数
CManagerServer::~CManagerServer()
{
}

//控件绑定
void CManagerServer::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIMIT_GAME_CHAT, m_btLimitGameChat);
	DDX_Control(pDX, IDC_ALLOW_GAME_CHAT, m_btAllowGameChat);
	DDX_Control(pDX, IDC_LIMIT_ROOM_CHAT, m_btLimitRoomChat);
	DDX_Control(pDX, IDC_ALLOW_ROOM_CHAT, m_btAllowRoomChat);
	DDX_Control(pDX, IDC_LIMIT_ENTER_ROOM, m_btLimitEnterRoom);
	DDX_Control(pDX, IDC_ALLOW_ENTER_ROOM, m_btAllowEnterRoom);
	DDX_Control(pDX, IDC_LIMIT_ENTER_GAME, m_btLimitEnterGame);
	DDX_Control(pDX, IDC_ALLOW_ENTER_GAME, m_btAllowEnterGame);
	DDX_Control(pDX, IDC_SET_SHALL_CLOSE, m_btSetShallClose);
	DDX_Control(pDX, IDC_RESET_SHALL_CLOSE, m_btReSetShallClose);
	DDX_Control(pDX, IDC_LIMIT_WISPER, m_btLimitRoomWisper);
	DDX_Control(pDX, IDC_ALLOW_WISPER, m_btAllowRoomWisper);
}

//初始化函数
BOOL CManagerServer::OnInitDialog()
{
	__super::OnInitDialog();

	//设置标题
	SetWindowText(TEXT("房间管理："));

	return TRUE;
}

//确定函数
void CManagerServer::OnOK()
{
	return;
}

//取消消息
void CManagerServer::OnCancel()
{
	//销毁窗口
	DestroyWindow();

	return;
}

//菜单命令
BOOL CManagerServer::OnCommand(WPARAM wParam, LPARAM lParam)
{
	//变量定义
	CMD_GR_OptionServer OptionServer;
	ZeroMemory(&OptionServer,sizeof(OptionServer));

	switch (LOWORD(wParam))
	{
	case IDC_SET_SHALL_CLOSE:		//即将关闭
		{
			//设置变量
			OptionServer.cbOptionValue=TRUE;
			OptionServer.cbOptionFlags=OSF_SHALL_CLOSE;

			break;
		}
	case IDC_RESET_SHALL_CLOSE:		//即将关闭
		{
			//设置变量
			OptionServer.cbOptionValue=FALSE;
			OptionServer.cbOptionFlags=OSF_SHALL_CLOSE;

			break;
		}
	case IDC_LIMIT_WISPER:			//大厅私聊
		{
			//设置变量
			OptionServer.cbOptionValue=FALSE;
			OptionServer.cbOptionFlags=OSF_ROOM_WISPER;

			break;
		}
	case IDC_ALLOW_WISPER:			//大厅私聊
		{
			//设置变量
			OptionServer.cbOptionValue=TRUE;
			OptionServer.cbOptionFlags=OSF_ROOM_WISPER;

			break;
		}
	case IDC_LIMIT_ROOM_CHAT:		//大厅聊天
		{
			//设置变量
			OptionServer.cbOptionValue=FALSE;
			OptionServer.cbOptionFlags=OSF_ROOM_CHAT;

			break;
		}
	case IDC_ALLOW_ROOM_CHAT:		//大厅聊天
		{
			//设置变量
			OptionServer.cbOptionValue=TRUE;
			OptionServer.cbOptionFlags=OSF_ROOM_CHAT;

			break;
		}
	case IDC_LIMIT_GAME_CHAT:		//游戏聊天
		{
			//设置变量
			OptionServer.cbOptionValue=FALSE;
			OptionServer.cbOptionFlags=OSF_GAME_CHAT;

			break;
		}
	case IDC_ALLOW_GAME_CHAT:		//游戏聊天
		{
			//设置变量
			OptionServer.cbOptionValue=TRUE;
			OptionServer.cbOptionFlags=OSF_GAME_CHAT;

			break;
		}
	case IDC_LIMIT_ENTER_ROOM:		//进入房间
		{
			//设置变量
			OptionServer.cbOptionValue=FALSE;
			OptionServer.cbOptionFlags=OSF_ENTER_ROOM;

			break;
		}
	case IDC_ALLOW_ENTER_ROOM:		//进入房间
		{
			//设置变量
			OptionServer.cbOptionValue=TRUE;
			OptionServer.cbOptionFlags=OSF_ENTER_ROOM;

			break;
		}
	case IDC_LIMIT_ENTER_GAME:		//进入游戏
		{
			//设置变量
			OptionServer.cbOptionValue=FALSE;
			OptionServer.cbOptionFlags=OSF_ENTER_GAME;

			break;
		}
	case IDC_ALLOW_ENTER_GAME:		//进入游戏
		{
			//设置变量
			OptionServer.cbOptionValue=TRUE;
			OptionServer.cbOptionFlags=OSF_ENTER_GAME;

			break;
		}
	}

	//发送消息
	if (OptionServer.cbOptionFlags!=0)
	{
		ASSERT(m_pIClientSocket!=NULL);
		m_pIClientSocket->SendData(MDM_GR_MANAGER,SUB_GR_OPTION_SERVER,&OptionServer,sizeof(OptionServer));
		return TRUE;
	}

	return __super::OnCommand(wParam,lParam);
}

//设置接口
bool CManagerServer::SetClientSocket(IUnknownEx * pIUnknownEx)
{
	//查询接口
	m_pIClientSocket=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITCPSocket);
	if (m_pIClientSocket==NULL) return false;

	return true;
}

//////////////////////////////////////////////////////////////////////////
