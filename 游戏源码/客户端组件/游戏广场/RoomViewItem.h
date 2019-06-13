#pragma once

#include "Stdafx.h"
#include "IGamePlaza.h"
#include "TableFrame.h"
#include "UserManager.h"
#include "FindUserDlg.h"
#include "UserListView.h"
#include "SystemOption.h"
#include "ShortMessage.h"
#include "ManagerClass.h"
#include "ServerItemView.h"
#include "GameFrame.h"

//////////////////////////////////////////////////////////////////////////

//房间服务状态
enum enServiceStatus
{
	ServiceStatus_Null,					//没有状态
	ServiceStatus_Connecting,			//连接状态
	ServiceStatus_EfficacyUser,			//效验用户
	ServiceStatus_RecvConfigInfo,		//接收配置
	ServiceStatus_RecvRoomInfo,			//接收信息
	ServiceStatus_Serviceing,			//服务状态
	ServiceStatus_NetShutDown,			//网络中断
};

//查找结构
struct tagFindTable
{
	bool								bOneNull;							//一个空位
	bool								bTwoNull;							//两个空位
	bool								bAllNull;							//全空位置
	bool								bNotFull;							//不全满位
	bool								bFilterPass;						//过滤密码
	WORD								wBeginTableID;						//开始索引
	WORD								wResultTableID;						//结果桌子
	WORD								wResultChairID;						//结果椅子
};

//类型定义
typedef CArrayTemplate<tagPropertyInfo> CPropertyAttributeArrary;
typedef CArrayTemplate<tagFlowerInfo> CFlowerAttributeArrary;
//////////////////////////////////////////////////////////////////////////

//消息定义
#define	WM_UPDATE_FACE					WM_USER+200							//更新头像

//////////////////////////////////////////////////////////////////////////

//游戏房间
class CRoomViewItem : public CDialog, public IRoomViewItem, public ITCPSocketSink, public IPurchaseInfo,
	public ITableFrameSink, public IUserManagerSink, public ISplitterSink, public IChannelMessageSink
{
	friend class CFindUserDlg;
	friend class CShortMessage;

	//应用变量
protected:
	CListServer	*					m_pListServer;						//房间信息
	tagOptionBuffer					m_OptionBuffer;						//配置信息
	CMD_GR_ColumnInfo				m_ListColumnInfo;					//列表信息

	//房间配置
protected:
	WORD							m_wGameGenre;						//游戏类型
	DWORD							m_dwVideoAddr;						//视频地址
	BYTE							m_cbHideUserInfo;					//隐藏信息

	//辅助变量
protected:
	bool							m_bRectifyHor;						//调整标志
	bool							m_bRectifyVor;						//调整标志
	bool							m_bInitDialog;						//初始标志
	bool							m_bHideUserList;					//隐藏标志
	DWORD							m_dwLimitChatTimes;					//限制聊时
	WORD							m_wReqTableID;						//请求桌子
	WORD							m_wReqChairID;						//请求位置
	WORD							m_wFindTableID;						//查找位置
	DWORD							m_dwMenuUserID;						//菜单用户

	//配置变量
protected:
	CGameOption *					m_pGameOption;						//游戏设置
	CServerOption *					m_pServerOption;					//房间设置

	//管理变量
protected:
	IUserItem *						m_pMeUserItem;						//自己信息
	enServiceStatus					m_ServiceStatus;					//服务状态
	CClientUserManager				m_ClientUserManager;				//用户管理

	//进程信息
protected:
	HWND							m_hWndChannel;						//信道窗口
	HANDLE							m_hShareMemory;						//共享内存
	TCHAR							m_szShareName[64];					//共享名字
	tagShareMemory *				m_pShareMemory;						//共享内存
	PROCESS_INFORMATION				m_GameProcessInfo;					//进程信息

	//聊天变量
protected:
	DWORD							m_dwChatUserID;						//聊天用户
	TCHAR							m_szChatName[NAME_LEN];				//聊天用户

	//聊天控件
public:
	CComboBox						m_ChatInput;						//聊天输入
	CComboBox						m_ChatObject;						//聊天对象
	//CSkinButton						m_btPhrase;							//短语按钮
	CSkinButton						m_btColorSet;						//颜色设置
	CSkinButton						m_btCleanScreen;					//清屏按钮
	CSkinButton						m_btSendChat;						//发送按钮
	CSkinButton						m_btExpression;						//表情按钮
	CSkinButton						m_btBugle;							//喇叭按钮
	CExpression						m_Expression;						//表情窗口
	CSkinRichEdit					m_ChatMessage;						//聊天组件

	//功能控件
public:
	CSkinButton						m_btBank;							//银行按钮
	CSkinButton						m_btAutoSit;						//加入按钮
	CSkinButton						m_btFindUser;						//查找按钮
	CSkinButton						m_btQuitRoom;						//退出按钮

	//用户控件
public:
	CWebBrowser						m_BrowerAD;							//浏览窗口
	CTableFrame						m_TableFrame;						//桌子框架
	CUserListView					m_UserListView;						//用户列表

	//管理控件
public:
	CManagerServer					m_ManagerServer;					//房间管理
	CManagerMessage					m_ManagerMessage;					//系统消息
	CManagerUserRight				m_ManagerUserRight;					//权限管理
	CManagerSendWarning				m_ManagerSendWarning;				//警告管理

	//环绕信息
protected:
	tagEncircleInfo					m_ImageInfoTable;					//桌子信息
	tagEncircleInfo					m_ImageInfoList;					//列表信息
	tagEncircleInfo					m_ImageInfoChat;					//聊框信息

	//资源变量
protected:
	tagEncircleImage				m_EncircleTable;					//桌子资源
	tagEncircleImage				m_EncircleList;						//列表资源
	tagEncircleImage				m_EncircleChat;						//聊框资源

	//数据缓冲
protected:
	WORD							m_wPacketTableID;					//缓冲位置
	WORD							m_wPacketChairID;					//缓冲位置
	CDataQueue						m_PacketDataStorage;				//数据缓冲

	//其他控件
public:
	CToolTipCtrl					m_ToolTipCtrl;						//提示控件
	CSkinSplitter					m_HorSplitter;						//拆分窗口
	CSkinSplitter					m_VorSplitter;						//拆分窗口
	CIPCRecvCopyData				m_IPCRecvCopyData;					//信道组件
	CTCPSocketHelper				m_ClientSocket;						//网络连接
	CMessageProxyHelper				m_MessageProxyHelper;				//消息代理
	CPropertyBar					m_PropertyBar;						//道具控件

	//扩展变量
public:
	CPropertyAttributeArrary		m_PropertyAttributeArrary;			//道具信息
	CFlowerAttributeArrary			m_FlowerAttributeArrary;			//鲜花信息

	//函数定义
public:
	//构造函数
	CRoomViewItem();
	//析构函数
	virtual ~CRoomViewItem();

	//重载函数 
protected:
	//控件绑定
	virtual void DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定函数
	virtual void OnOK() { return; };
	//取消函数
	virtual void OnCancel() { return; };
	//消息过虑
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//菜单命令
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	//基础接口
public:
	//释放对象
	virtual void __cdecl Release() { delete this; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//道具接口 
public:
	//购买场所
	virtual BYTE __cdecl GetLocation(){return LOCATION_PLAZA_ROOM;}
	//房间类型
	virtual WORD __cdecl GetGameGenre();
	//枚举玩家
	virtual tagUserData * __cdecl EnumLookonUser(WORD wIndex);
	//查找用户
	virtual tagUserData * __cdecl SearchUserItem(DWORD dwUserID);
	//查找用户
	virtual tagUserData * __cdecl SearchUserItem(LPCTSTR pszUserName);
	//获取玩家
	virtual const tagUserData * __cdecl GetUserInfo(WORD wChairID);
	//获取自己
	virtual const tagUserData * __cdecl GetMeUserInfo();
	//发送信息
	virtual void __cdecl SendData(WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize);

	//视图接口
public:
	//老板事件
	void NotifyBossCome(bool bBossCome);
	//获取句柄
	virtual HWND __cdecl GetWindowHandle() { return GetSafeHwnd(); }

	//房间接口
public:
	//关闭询问
	virtual bool __cdecl QueryCloseRoom();
	//关闭房间
	virtual void __cdecl CloseRoomViewItem();
	//类型信息
	virtual tagGameKind * __cdecl GetKindInfo();
	//房间信息
	virtual tagGameServer * __cdecl GetServerInfo();

	//拆分条接口
public:
	//按钮消息
	virtual bool __cdecl OnSplitterButton(CSkinSplitter * pSkinSplitter, CSkinButton * pSplitterButton);
	//拆分条消息
	virtual bool __cdecl OnSplitterEvent(CSkinSplitter * pSkinSplitter, UINT uSplitID, int nXPos, int nYPos);

	//网络事件
public:
	//连接事件
	virtual bool __cdecl OnEventTCPSocketLink(WORD wSocketID, INT nErrorCode);
	//关闭事件
	virtual bool __cdecl OnEventTCPSocketShut(WORD wSocketID, BYTE cbShutReason);
	//读取事件
	virtual bool __cdecl OnEventTCPSocketRead(WORD wSocketID, CMD_Command Command, VOID * pData, WORD wDataSize);

	//信道接口
public:
	//信道数据处理
	virtual bool __cdecl OnChannelMessage(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend);

	//桌子接口
public:
	//鼠标左键按下
	virtual void __cdecl OnLButtonHitTable(WORD wTableID, WORD wChairID, bool bMirror);
	//鼠标右键按下
	virtual void __cdecl OnRButtonHitTable(WORD wTableID, WORD wChairID, bool bMirror);
	//鼠标双击
	virtual void __cdecl OnDoubleHitTable(WORD wTableID, bool bMirror);

	//用户接口
public:
	//用户激活
	virtual void __cdecl OnUserItemAcitve(IUserItem * pIUserItem);
	//用户更新
	virtual void __cdecl OnUserItemUpdate(IUserItem * pIUserItem);
	//用户删除
	virtual void __cdecl OnUserItemDelete(IUserItem * pIUserItem);

	//网络主命令
protected:
	//登录消息
	bool OnSocketMainLogon(CMD_Command Command, void * pBuffer, WORD wDataSize);
	//用户消息
	bool OnSocketMainUser(CMD_Command Command, void * pBuffer, WORD wDataSize);
	//配置消息
	bool OnSocketMainInfo(CMD_Command Command, void * pBuffer, WORD wDataSize);
	//状态消息
	bool OnSocketMainStatus(CMD_Command Command, void * pBuffer, WORD wDataSize);
	//系统消息
	bool OnSocketMainSystem(CMD_Command Command, void * pBuffer, WORD wDataSize);
	//房间消息
	bool OnSocketMainServerInfo(CMD_Command Command, void * pBuffer, WORD wDataSize);
	//游戏消息
	bool OnSocketMainGameFrame(CMD_Command Command, void * pBuffer, WORD wDataSize);

	//网络子命令
protected:
	//用户进入
	bool OnSocketSubUserCome(CMD_Command Command, void * pBuffer, WORD wDataSize);
	//用户状态
	bool OnSocketSubStatus(CMD_Command Command, void * pBuffer, WORD wDataSize);
	//用户分数
	bool OnSocketSubScore(CMD_Command Command, void * pBuffer, WORD wDataSize);
	//用户权限
	bool OnSocketSubRight(CMD_Command Command, void * pBuffer, WORD wDataSize);
	//会员等级
	bool OnSocketSubMemberOrder(CMD_Command Command, void * pBuffer, WORD wDataSize);
	//坐下失败
	bool OnSocketSubSitFailed(CMD_Command Command, void * pBuffer, WORD wDataSize);
	//用户聊天
	bool OnSocketSubChat(CMD_Command Command, void * pBuffer, WORD wDataSize);
	//用户私语
	bool OnSocketSubWisper(CMD_Command Command, void * pBuffer, WORD wDataSize);
	//邀请玩家
	bool OnSocketSubUserInvite(CMD_Command Command, void * pBuffer, WORD wDataSize);

	//IPC 事件处理
protected:
	//IPC 内核命令
	bool OnIPCKernel(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend);
	//IPC 发包命令
	bool OnIPCSocket(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend);

	//控制函数
public:
	//初始化房间
	bool InitRoomViewItem(CListServer * pListServer);
	//连接服务器
	bool ConnectGameServer();
	//发送规则
	bool SendUserRule();
	//私语信息
	bool SendUserWhisper(IUserItem * pIUserItem);
	//用户菜单
	bool ShowUserInfoMenu(IUserItem * pIUserItem, CPoint Point);

	//功能函数
public:
	//资源句柄
	HINSTANCE GetResInstanceHandle();
	//获取资源
	tagServerViewImage GetServerViewRes();
	//更新资源
	bool UpdateSkinResource();
	//游戏配置
	CGameOption * GetGameOption() { return m_pGameOption; }
	//房间配置
	CServerOption * GetServerOption() { return m_pServerOption; }

	//网络命令
protected:
	//发送登录命令
	bool SendLogonPacket();
	//发送设置命令
	bool SendOptionsPacket();
	//发送起来命令
	bool SendStandUpPacket();
	//发送强退命令
	bool SendLeftGamePacket();
	//发送房间规则
	bool SendUserRulePacket();
	//发送旁观命令
	bool SendLookonPacket(WORD wTableID, WORD wChairID, LPCTSTR pszTablePass);
	//发送坐下命令
	bool SendSitDownPacket(WORD wTableID, WORD wChairID, LPCTSTR pszTablePass);
	//发送聊天命令
	bool SendChatPacket(DWORD dwTargetUserID, LPCTSTR pszChatMessage, COLORREF crFontColor);

	//动作处理
protected:
	//旁观动作
	bool PerformLookonAction(WORD wTableID, WORD wChairID);
	//坐下动作
	bool PerformSitDownAction(WORD wTableID, WORD wChairID);

	//IPC 处理
protected:
	//发送游戏信息
	bool IPCSendGameInfo(CIPCSendCopyData * pSendCopyData);
	//发送用户信息
	bool IPCSendTableUsers(CIPCSendCopyData * pSendCopyData);

	//内部函数
protected:
	//启动游戏
	int StartGameClient();
	//关闭游戏
	void CloseGameClient();
	//发送用户
	bool SendTableUser(IUserItem * pIUserItem, CIPCSendCopyData * pSendCopyData);
	//发送消息
	bool SendProcessData(WORD wMainCmdID, WORD wSubCmdID, void * pBuffer, WORD wDataSize);

	//辅助函数
protected:
	//更新标题
	void UpdateTitleText();
	//绘画列表
	void DrawUserFrame(CDC * pDC);
	//绘画聊天
	void DrawChatFrame(CDC * pDC);
	//绘画桌子
	void DrawTableFrame(CDC * pDC);
	//左视图区
	void DrawLeftViewFrame(CDC * pDC);
	//聊天对象
	bool SetChatObject(IUserItem * pIUserItem);
	//查找桌子
	bool FindGameTable(tagFindTable & FindInfo);
	//调整控件
	void RectifyControl(int nWidth, int nHeight);
	//显示消息
    int ShowMessageBox(LPCTSTR pszMessage, UINT nType);
	//能否坐下
	bool CanSitDownTable(WORD wTableID, WORD wChairID, bool bLookon, bool bShowError);

	//消息映射
protected:
	//绘画消息
	afx_msg void OnPaint();
	//绘画背景
	afx_msg BOOL OnEraseBkgnd(CDC * pDC);
	//位置消息
	afx_msg void OnSize(UINT nType, int cx, int cy);

	//按钮消息
protected:
	//发送按钮
	afx_msg void OnBnClickedSendChat();
	//表情按钮
	afx_msg void OnBnClickedExpression();
	//颜色按钮
	afx_msg void OnBnClickedColorSet();
	//清屏按钮
	afx_msg void OnBnClickedCleanScreen();
	//喇叭按钮
	afx_msg void OnBnClickedBugle();
	//显示消息
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//双击户列
	afx_msg void OnNMDblclkUserList(NMHDR * pNMHDR, LRESULT * pResult);
	//右键列表
	afx_msg void OnNMRclickUserList(NMHDR * pNMHDR, LRESULT * pResult);
	//点键列表
	afx_msg void OnNMclickUserList(NMHDR * pNMHDR, LRESULT * pResult);
	//更新头像
	LRESULT OnUpdateFace(WPARAM wParam, LPARAM lParam);
	//IPC 消息
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT * pCopyDataStruct);
	//表情消息
	LRESULT OnHitExpression(WPARAM wParam, LPARAM lParam);

	//功能按钮
protected:
	//银行按钮
	afx_msg void OnBnClickedBank();
	//加入按钮
	afx_msg void OnBnClickedAutoSit();
	//查找按钮
	afx_msg void OnBnClickedFindUser();
	//退出按钮
	afx_msg void OnBnClickedQuitRoom();

	CPtrArray						m_ShortMessage;						//信息窗口
	CFindUserDlg					* m_pFindUserDlg;					//查

	//激活信息窗口
	CShortMessage * ActiveShortWnd(long int dwUserID, IUserItem * pUserItem, bool bCreate);


	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
