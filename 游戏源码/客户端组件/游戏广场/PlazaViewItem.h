#pragma once

#include "Stdafx.h"
#include "DlgLogon.h"
#include "GameFrame.h"
#include "UserInfoView.h"
#include "ServerItemView.h"

//////////////////////////////////////////////////////////////////////////

//消息定义
#define		WM_DOWN_LOAD_FACE		WM_USER+100				//下载消息

//自定义头像
struct tagCustomFace
{
	DWORD						dwUserID;				//玩家 ID
	BYTE						*pFaceData;				//头像数据
	DWORD						dwFaceSize;				//头像大小
	DWORD						dwCurrentSize;			//当前大小

	//构造函数
	tagCustomFace():dwUserID(0),pFaceData(NULL),dwFaceSize(0),dwCurrentSize(0){}

	//清除内存
	void Clear(void)
	{
		delete[]pFaceData;
		pFaceData=NULL;
		dwUserID=0;
		dwFaceSize=0;
		dwCurrentSize=0;
	}

	//析构函数
	~tagCustomFace()
	{
		Clear();
	}

};

//下载信息
struct tagDownloadInfo
{
	DWORD						dwUserID;				//玩家 ID
	BYTE						bFaceVer;				//头像版本
	BYTE						bReference;				//引用计数

	//初始化
	tagDownloadInfo():dwUserID(0),bFaceVer(0),bReference(1){}
};

//类型定义
typedef CArrayTemplate<tagDownloadInfo>			CDownloadInfoArrary;
//////////////////////////////////////////////////////////////////////////

//游戏广场
class CPlazaViewItem : public CDialog, public IPlazaViewItem,public ITCPSocketSink
{
	//状态变量
public:
	bool						m_bInitDialog;						//初始标志
	bool						m_bLogonPlaza;						//登录标志

	//控件变量
public:
	CDlgLogon					m_DlgLogon;							//登录对话框
	CTCPSocketHelper			m_ClientSocket;						//网络连接

	//头像变量
protected:
	bool						m_bStartDownloadFace			;	//下载标识
	bool						m_bDownloadConnect;					//下载标识
	tagCustomFace				m_CustomFace;						//自定义头像
	CDownloadInfoArrary			m_DownloadInfoArrary			;	//下载队列

	//导航按钮
public:
	CSkinButton					m_WEB_BEFORE;
	CSkinButton					m_WEB_BRUSH;
	CSkinButton					m_WEB_HOME;
	CSkinButton					m_WEB_NEXT;
	CSkinButton					m_WEB_STOP;

	//控件变量
public:
	CHtmlBrower *				m_pHtmlBrower;						//浏览窗口
	CToolTipCtrl				m_ToolTipCtrl;						//提示控件

	//环绕信息
protected:
	tagEncircleInfo				m_ImageInfoWeb;						//网页浏览

	//资源变量
protected:
	tagEncircleImage			m_EncircleWeb;						//网页浏览

	//函数定义
public:
	CPlazaViewItem();
	virtual ~CPlazaViewItem();

	//重载函数
protected:
	//控件绑定
	virtual void DoDataExchange(CDataExchange * pDX);
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定函数
	virtual void OnOK() { return; };
	//取消函数
	virtual void OnCancel() { return; };

	//基础接口
public:
	//释放对象
	virtual void __cdecl Release() { delete this; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//视图接口
public:
	//获取句柄
	virtual HWND __cdecl GetWindowHandle() { return GetSafeHwnd(); }

	//网络事件
public:
	//连接事件
	virtual bool __cdecl OnEventTCPSocketLink(WORD wSocketID, INT nErrorCode);
	//关闭事件
	virtual bool __cdecl OnEventTCPSocketShut(WORD wSocketID, BYTE cbShutReason);
	//读取事件
	virtual bool __cdecl OnEventTCPSocketRead(WORD wSocketID, CMD_Command Command, VOID * pData, WORD wDataSize);

	//消息处理
protected:
	//登录消息
	bool OnSocketMainLogon(CMD_Command Command, void * pBuffer, WORD wDataSize);
	//列表消息
	bool OnSocketMainServerList(CMD_Command Command, void * pBuffer, WORD wDataSize);
	//系统消息
	bool OnSocketMainSystem(CMD_Command Command, void * pBuffer, WORD wDataSize);
	//用户消息
	bool OnSocketMainUser(CMD_Command Command, void * pBuffer, WORD wDataSize);

	//广场接口
public:
	//登录消息
	virtual bool __cdecl SendLogonMessage();
	//连接消息
	virtual bool __cdecl SendConnectMessage();

	//功能函数
public:
	//显示网页
	void WebBrowse(LPCTSTR lpszUrl, bool bForceBrowse);
	//显示消息
    int ShowMessageBox(LPCTSTR pszMessage, UINT nType);
	//网络链接
	bool ConnectServer();
	//更新资源
	bool UpdateSkinResource();

	//内部函数
private:
	//更新标题
	void UpdateTitleText();
	//绘画网页
	void DrawWebFrame(CDC * pDC);

	//消息函数
protected:
	//绘画消息
	afx_msg void OnPaint();
	//绘画背景
	afx_msg BOOL OnEraseBkgnd(CDC * pDC);
	//登录广场
	afx_msg void OnCommandLogon();
	//连接服务器
	afx_msg void OnCommandConnect();
	//取消连接
	afx_msg void OnCommandCancelConnect();
	//位置消息
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//显示消息
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//下载消息
	LRESULT OnDownloadFace(WPARAM wParam, LPARAM lParam);

	//导航按钮
	afx_msg void OnBnClickedBEFORE();
	afx_msg void OnBnClickedNEXT();
	afx_msg void OnBnClickedMBRUSH();
	afx_msg void OnBnClickedSTOP();
	afx_msg void OnBnClickedHOME();
	afx_msg void OnBnClickedBBS();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
