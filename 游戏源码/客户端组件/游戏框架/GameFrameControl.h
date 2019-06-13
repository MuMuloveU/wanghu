#ifndef GAME_FRAME_CONTROL_HEAD_FILE
#define GAME_FRAME_CONTROL_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameFrame.h"
#include "UserInfoView.h"
#include "UserListView.h"
#include "GameFrameDlg.h"

//类说明
class CClientKernelSink;

//////////////////////////////////////////////////////////////////////////

//框架控制视图
class GAME_FRAME_CLASS CGameFrameControl : public CDialog, public IPurchaseInfo
{
	//变量定义
protected:
	bool								m_bRectify;						//调整标志
	bool								m_bInitDialog;					//创建标志
	bool								m_bShowAd;						//广告标志
	DWORD								m_dwLimitChatTimes;				//限制聊时
	tagUserData *						m_pUserData;					//用户指针

	//组件变量
protected:
	CGameFrameDlg						* m_pGameFrameDlg;				//对话框
	IClientKernel						* m_pIClientKernel;				//内核接口

	//环绕信息
protected:
	tagEncircleInfo						m_ImageInfoList;				//列表信息
	tagEncircleInfo						m_ImageInfoChat;				//聊框信息

	//资源变量
protected:
	tagEncircleImage					m_EncircleList;					//列表资源
	tagEncircleImage					m_EncircleChat;					//聊框资源

	//聊天控件
public:
	CComboBox							m_ChatInput;					//聊天输入
	CComboBox							m_ChatObject;					//聊天对象
	CWebBrowser							m_BrowerAD;						//浏览窗口
	//CSkinButton							m_btPhrase;						//短语按钮
	CSkinButton							m_btColorSet;					//颜色设置
	CSkinButton							m_btCleanScreen;				//清屏按钮
	CSkinButton							m_btBugle;						//啦八按钮
	CSkinButton							m_btSendChat;					//发送按钮
	CSkinButton							m_btExpression;					//表情按钮
	CExpression							m_Expression;					//表情窗口
	CSkinRichEdit						m_ChatMessage;					//聊天组件

	//其他变量
public:
	CSkinButton							m_btSound;						//游戏声音
	CSkinButton							m_btLookOn;						//旁观设置
	CSkinButton							m_btGameRule;					//游戏规则
	CSkinButton							m_btGameOption;					//游戏设置
	CSkinButton							m_btGameQuit;					//游戏设置

	//控件变量
public:
	CToolTipCtrl						m_ToolTipCtrl;					//提示控件
	CUserListView						m_UserListView;					//用户列表
	CUserInfoView						m_UserInfoView;					//玩家信息

	//道具控件
protected:
	CPropertyBar						m_PropertyBar;					//道具控件

	//函数定义
public:
	//构造函数
	CGameFrameControl(CGameFrameDlg * pGameFrameDlg); 
	//析构函数
	virtual ~CGameFrameControl();

	//基础接口
public:
	//释放对象
	virtual void __cdecl Release() { delete this; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//道具接口 
public:
	//购买场所
	virtual BYTE __cdecl GetLocation(){return LOCATION_GAME_ROOM;}
	//房间类型
	virtual WORD __cdecl GetGameGenre();
	//枚举玩家
	virtual tagUserData * __cdecl EnumLookonUser(WORD wIndex);
	//查找用户
	virtual tagUserData * __cdecl SearchUserItem(DWORD dwUserID);
	//查找用户
	virtual tagUserData * __cdecl SearchUserItem(LPCTSTR pszUserName){return NULL;}
	//获取玩家
	virtual const tagUserData * __cdecl GetUserInfo(WORD wChairID);
	//获取自己
	virtual const tagUserData * __cdecl GetMeUserInfo();
	//发送信息
	virtual void __cdecl SendData(WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize);

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

	//配置函数
public:
	//设置内核
	void SetClientKernel(IClientKernel * pIClientKernel);
	//设置接口
	bool SetUserFaceRes(IUnknownEx * pIUnknownEx);

	//功能函数
public:
	//更新资源
	bool UpdateSkinResource();
	//重置控制
	void ResetGameFrameControl();
	//加入用户
	bool InsertUserItem(tagUserData * pUserData);
	//更新用户
	bool UpdateUserItem(tagUserData * pUserData);
	//删除用户
	bool DeleteUserItem(tagUserData * pUserData);

	//内部函数
protected:
	//左视图区
	void DrawLeftViewFrame(CDC * pDC);
	//调整控件
	void RectifyControl(int nWidth, int nHeight);

	//消息函数
protected:
	//绘画消息
	afx_msg void OnPaint();
	//游戏规则
	afx_msg void OnBnClickedRule();
	//游戏退出
	afx_msg void OnBnClickedQuit();
	//游戏声音
	afx_msg void OnBnClickedSound();
	//游戏旁观
	afx_msg void OnBnClickedLookon();
	//游戏配置
	afx_msg void OnBnClickedOption();
	//绘画背景
	afx_msg BOOL OnEraseBkgnd(CDC * pDC);
	//位置消息
	afx_msg void OnSize(UINT nType, int cx, int cy);
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
	//表情消息
	LRESULT OnHitExpression(WPARAM wParam, LPARAM lParam);
	//右键列表
	afx_msg void OnNMRclickUserList(NMHDR * pNMHDR, LRESULT * pResult);
	//点中列表
	afx_msg void OnNMclickUserList(NMHDR *pNMHDR, LRESULT *pResult);
	//时间消息
	afx_msg void OnTimer(UINT nIDEvent);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif