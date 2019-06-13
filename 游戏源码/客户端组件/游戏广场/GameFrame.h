#ifndef PLAZA_GAME_FRAME_HEAD_FILE
#define PLAZA_GAME_FRAME_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "Resource.h"
#include "RoomViewItem.h"
#include "PlazaViewItem.h"
#include "UserInfoView.h"
#include "ServerItemView.h"
#include "UserCompanionList.h"
#include "DlgCustomFace.h"

/////////////////////////////////////////////////////////////////////////////

//列表状态
enum enServerListControl
{
	ServerListControl_Hide,			//隐藏列表
	ServerListControl_Show,			//显示列表
	ServerListControl_Turn,			//翻转列表
};

//宏定义
#define MAX_SERVER						3								//最大房间

//热键定义
#define IDI_HOT_KEY_BOSS			0x0100								//老板热键

/////////////////////////////////////////////////////////////////////////////

//主框架类
class CGameFrame : public CFrameWnd, public ISplitterSink, public ITreeCtrlSink
{
	//状态变量
public:
	bool								m_bRectify;						//调整标志
	bool								m_bHideGameList;				//隐藏标志
	WORD								m_wShowListType;				//列表标志
	CRect								m_rcViewItem;					//窗口位置
	CPlazaViewItem						m_DlgGamePlaza;					//游戏广场
	IViewItem							* m_pActiveViewItem;			//当前视图
	CRoomViewItem						* m_pRoomViewItemCreate;		//创建房间
	CRoomViewItem						* m_pRoomViewItem[MAX_SERVER];	//房间接口
	CDlgCustomFace						m_DlgCustomFace;				//自定头像

	//控件变量
public:
	CWebBrowser							m_BrowerAD;						//浏览窗口
	CToolTipCtrl						m_ToolTipCtrl;					//提示控件
	CUserInfoView						m_UserInfoView;					//游戏视图
	CSkinSplitter						m_Splitter;						//拆分窗口
	CServerItemView						m_ServerItemView;				//服务器视图
	CUserCompanionList					m_UserCompanionList;			//好友列表

	//界面状态
protected:
	bool								m_bMaxShow;						//最大标志
	CRect								m_rcNormalSize;					//正常位置

	//导航按钮
public:
	CSkinButton							m_btButton1;					//功能按钮
	CSkinButton							m_btButton2;					//功能按钮
	CSkinButton							m_btButton3;					//功能按钮
	CSkinButton							m_btButton4;					//功能按钮
	CSkinButton							m_btButton5;					//功能按钮

	//大厅按钮
public:
	CSkinButton							m_btMin;						//最小按钮
	CSkinButton							m_btMax;						//最大按钮
	CSkinButton							m_btClose;						//关闭按钮
	CSkinButton							m_btExChangeSkin;				//换肤按钮
	CSkinButton							m_btGamePlaza;					//游戏广场
	CSkinButton							m_btViewItem[MAX_SERVER];		//房间按钮
	//CSkinButton						m_btQuitGame;					//退出游戏
	CSkinButton							m_btSelfOption;					//个人配置
	CSkinButton							m_btSwitchUser;					//切换帐号
	CSkinButton							m_btGlobalOption;				//系统配置

	//列表按钮
public:
	CSkinButton							m_btListControl1;				//控制按钮
	CSkinButton							m_btListControl2;				//控制按钮

	//环绕信息
protected:
	tagEncircleInfo						m_ImageInfoFrame;				//框架信息
	tagEncircleInfo						m_ImageInfoServer;				//列表信息

	//资源变量
protected:
	tagEncircleImage					m_EncircleFrame;				//框架资源
	tagEncircleImage					m_EncircleServer;				//列表资源

	//函数定义
public:
	//构造函数
	CGameFrame();
	//析构函数
	virtual ~CGameFrame();

	//拆分条接口
public:
	//按钮消息
	virtual bool __cdecl OnSplitterButton(CSkinSplitter * pSkinSplitter, CSkinButton * pSplitterButton);
	//拆分条消息
	virtual bool __cdecl OnSplitterEvent(CSkinSplitter * pSkinSplitter, UINT uSplitID, int nXPos, int nYPos);

	//树表接口
public:
	//左键单击
	virtual void __cdecl OnTreeLeftClick(CListItem * pListItem, HTREEITEM hTreeItem, CTreeCtrl * pTreeCtrl);
	//右键单击
	virtual void __cdecl OnTreeRightClick(CListItem * pListItem, HTREEITEM hTreeItem, CTreeCtrl * pTreeCtrl);
	//左键双击
	virtual void __cdecl OnTreeLeftDBClick(CListItem * pListItem, HTREEITEM hTreeItem, CTreeCtrl * pTreeCtrl);
	//右键双击
	virtual void __cdecl OnTreeRightDBClick(CListItem * pListItem, HTREEITEM hTreeItem, CTreeCtrl * pTreeCtrl);
	//选择改变
	virtual void __cdecl OnTreeSelectChanged(CListItem * pListItem, HTREEITEM hTreeItem, CTreeCtrl * pTreeCtrl);
	//子项展开
	virtual void __cdecl OnTreeItemexpanded(CListItem * pListItem, HTREEITEM hTreeItem, CTreeCtrl * pTreeCtrl);

	//重载函数
public:
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//命令函数
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	//功能函数
public:
	//获取广场
	CPlazaViewItem * GetPlazaViewItem() { return &m_DlgGamePlaza; }
	//显示网页
	void WebBrowse(LPCTSTR lpszUrl, bool bForceBrowse);
	//系统设置
	void ShowSystemOption();
	//显示消息
    int ShowMessageBox(LPCTSTR pszMessage, UINT nType);
	//列表控制
	bool ControlServerList(enServerListControl ServerListControl);
	//资源句柄
	HINSTANCE GetResInstanceHandle();
	//获取资源
	tagPlatformFrameImage GetPlatformRes();
	//更新资源
	bool UpdateSkinResource();

	//房间函数
public:
	//激活广场
	void ActivePlazaViewItem();
	//激活房间
	void ActiveRoomViewItem(CRoomViewItem * pRoomViewItem);
	//查找房间
	CRoomViewItem * SearchRoomViewItem(WORD wKindID, WORD wServerID);
	//进入房间
	CRoomViewItem * CreateRoomViewItem(CListServer * pListServer);
	//关闭房间
	void CloseRoomViewItem(IRoomViewItem * pIRoomViewItem);
	//关闭房间
	void CloseAllRoomViewItem();
	//关闭当前
	void CloseCurrentViewItem();

	//内部函数
private:
	//更新控制
	void UpdateControlButton();
	//调整控件
	void RectifyControl(int nWidth, int nHeight);

	//界面函数
protected:
	//最大窗口
	bool MaxSizeWindow();
	//还原窗口
	bool RestoreWindow();

	//消息函数
protected:
	//绘画消息
	afx_msg void OnPaint();
	//绘画背景
	afx_msg BOOL OnEraseBkgnd(CDC * pDC);
	//创建消息
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//大小消息
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR * lpMMI);
	//位置消息
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//按键测试
	afx_msg UINT OnNcHitTest(CPoint Point);
	//鼠标消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint Point);
	//鼠标消息
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint Point);
	//光标信息
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	//激活事件
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	//改变消息
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	//关闭消息
	afx_msg void OnClose();

	//自定义消息
protected:
	//热键消息
	LRESULT OnHotKeyMessage(WPARAM wParam, LPARAM lParam);
	//安装完成
	LRESULT OnMessageSetupFinish(WPARAM wParam, LPARAM lParam);
	//关闭房间
	LRESULT OnCloseRoomViewItem(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

#endif
