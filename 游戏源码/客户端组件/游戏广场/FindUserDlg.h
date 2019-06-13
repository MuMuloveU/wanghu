#pragma once

#include "StdAfx.h"
#include "Resource.h"
#include "Afxcmn.h"
#include "UserListView.h"

//用户查找对话框 
class CFindUserDlg : public CSkinDialogEx
{
	//变量定义
protected:
	UINT							m_uGameType;						//游戏类型
	CWnd							* m_pParentWnd;						//父窗口

	//控件定义
public:
	CSkinEdit						m_UserName;							//用户名字
	CSkinEdit						m_UserAcc;							//用户 ACC
	CSkinButton						m_BtFind;							//查找按钮
	CSkinButton						m_BtCancel;							//关闭按钮
	CUserListView					m_UserList;							//用户列表

	//函数定义
public:
	//构造函数
	CFindUserDlg(CWnd * pParentWnd, UINT uGameType);
	//析够函数
	virtual ~CFindUserDlg();
	//更新用户信息
	void UpdateUserItem(IUserItem * pUserItem);
	//删除用户信息
	void DeleteUserItem(IUserItem * pUserItem);

	//重载函数
public:
	//数据绑定
	virtual void DoDataExchange(CDataExchange * pDX);    
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定
	virtual void OnOK();
	//取消
	virtual void OnCancel() { DestroyWindow(); } 

	//消息函数
protected:
	//单击鼠标右键
	afx_msg void OnNMRclickList(NMHDR * pNMHDR, LRESULT * pResult);

	DECLARE_MESSAGE_MAP()
};
