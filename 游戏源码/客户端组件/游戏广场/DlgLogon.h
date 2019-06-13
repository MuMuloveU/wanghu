#ifndef DLG_LOGON_HEAD_FILE
#define DLG_LOGON_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "Resource.h"

//////////////////////////////////////////////////////////////////////////

//登录模式
enum enLogonMode
{
	LogonMode_Accounts,			//帐号登录
	LogonMode_UserID,			//I D 登录
};

//注册表项名
#define REG_OPTION_LOGON		TEXT("OptionLogon")
#define REG_USER_INFO			TEXT("Software\\WHSCDCQPGame\\GamePlaza\\UserInfo")
#define REG_LOGON_SERVER		TEXT("Software\\WHSCDCQPGame\\GamePlaza\\LogonServer")

//////////////////////////////////////////////////////////////////////////

//登录 TAB 控件
class CTabCtrlLogon : public CTabCtrl
{
	//函数定义
public:
	//构造函数
	CTabCtrlLogon();
	//析够函数
	virtual ~CTabCtrlLogon();

	//消息函数
protected:
	//重画函数	
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//用户注册
class CDlgRegister : public CSkinDialogEx
{
	friend class CDlgLogon;

	//登录信息
public:
	WORD								m_wFaceID;						//头像标识
	BYTE								m_cbGender;						//用户性别
	TCHAR								m_szSpreader[NAME_LEN];			//电子邮箱
	TCHAR								m_szAccounts[NAME_LEN];			//游戏帐号
	TCHAR								m_szPassword[PASS_LEN];			//游戏密码

	//控件变量
public:
	CImageList							m_ImageList;
	CComboBoxEx							m_FaceSelect;
	CSkinButton							m_btLogon;						//登录按钮
	CSkinButton							m_btCancel;						//取消按钮
	CSkinHyperLink						m_LineRegWeb;					//网站注册
	CSkinHyperLink						m_LineMainPage;					//游戏主页
	CSkinHyperLink						m_LinePassWord;					//密码保护

	//函数定义
public:
	//构造函数
	CDlgRegister();
	//析构函数
	virtual ~CDlgRegister();

	//重载函数
protected:
	//控件绑定
	virtual void DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定函数
	virtual void OnOK();
	//取消消息
	virtual void OnCancel();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//登录对话框
class CDlgLogon : public CSkinDialogEx
{
	friend class CRoomViewItem;
	friend class CPlazaViewItem;

	//登录信息
protected:
	WORD								m_wFaceID;						//头像标识
	BYTE								m_cbGender;						//用户性别
	DWORD								m_dwUserID;						//用户 I D
	TCHAR								m_szSpreader[NAME_LEN];			//推广人名
	TCHAR								m_szAccounts[NAME_LEN];			//游戏帐号
	TCHAR								m_szPassword[PASS_LEN];			//游戏密码

	//位置变量
protected:
	INT									m_nFullWidth;					//全部宽度
	INT									m_nFullHeight;					//全部高度
	bool								m_bNetOption;					//网络设置
	CRect								m_rcNormalFrame;				//框架位置

	//登录模式
protected:
	enLogonMode							m_LogonMode;					//登录模式

	//辅助信息
protected:
	bool								m_bRegister;					//注册标志
	bool								m_bChangePassWord;				//改变标志

	//连接信息
protected:
	CString								m_strLogonServer;				//服务器地址

	//按钮变量
public:
	CSkinButton							m_btLogon;						//登录按钮
	CSkinButton							m_btCancel;						//取消按钮
	CSkinButton							m_btDelete;						//删除按钮
	CSkinButton							m_btRegister;					//注册按钮
	CSkinButton							m_btNetOption;					//网络按钮
	CSkinButton							m_btProxyTest;					//测试按钮

	//控件变量
public:
	CWebBrowser							m_BrowerAD;						//浏览窗口
	CTabCtrlLogon						m_TabLogonMode;					//登录选择
	CSkinHyperLink						m_LineMainPage;					//游戏主页
	CSkinHyperLink						m_LinePassWord;					//密码保护
	CSkinHyperLink						m_LineGetPassWord;				//取回密码

	//编辑控件
protected:
	CSkinEdit							m_edProxyPort;					//代理端口
	CSkinEdit							m_edProxyServer;				//代理地址
	CSkinEdit							m_edProxyUserName;				//代理用户
	CSkinEdit							m_edProxyPassword;				//代理密码
	CComboBox							m_cmProxyServerType;			//代理类型

	//函数定义
public:
	//构造函数
	CDlgLogon();
	//析构函数
	virtual ~CDlgLogon();

	//重载函数
protected:
	//控件绑定
	virtual void DoDataExchange(CDataExchange * pDX);
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定函数
	virtual void OnOK();
	//取消消息
	virtual void OnCancel();

	//功能函数
public:
	//获取服务器
	LPCTSTR GetLogonServer() { return m_strLogonServer; }
	//发送登录包
	bool SendLogonPacket(ITCPSocket * pIClientSocke);
	//登录成功处理
	bool OnLogonSuccess();

	//内部函数
private:
	//加载服务器
	void LoadLogonServer();
	//读取帐号
	void LoadAccountsInfo();
	//代理信息
	void LoadProxyServerInfo();
	//效验输入
	bool CheckLogonInput(bool bShowError);
	//设置模式
	void SetLogonMode(enLogonMode LogonMode);

	//辅助函数
private:
	//更新密码
	void UpdateUserPassWord(DWORD dwUserDBID);
	//代理模式
	void SwitchNetOption(bool bNetOption);
	//更新选择
	void UpdateUserComboBox(UINT uComboBoxID);
	//查找字符
	int ComboBoxFindString(CComboBox * pComboBox, LPCTSTR pszFindString);
	//获取代理
	void GetProxyInfo(enProxyServerType &ProxyServerType, tagProxyServerInfo &ProxyServerInfo);
	//代理判断
	bool EnableProxy() { return m_bNetOption;}

	//消息函数
public:
	//注册帐号
	afx_msg void OnRegisterAccounts();
	//删除用户
	afx_msg void OnDeleteAccounts();
	//网络设置
	afx_msg void OnBnClickedNetOption();
	//代理测试
	afx_msg void OnBnClickedProxyTest();
	//密码改变
	afx_msg void OnEnChangePassword();
	//选择改变
	afx_msg void OnSelchangeAccounts();
	//选择改变
	afx_msg void OnSelchangeUserID();
	//类型改变
	afx_msg void OnTcnSelchangeLogonType(NMHDR * pNMHDR, LRESULT * pResult);
	//重画消息
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif
