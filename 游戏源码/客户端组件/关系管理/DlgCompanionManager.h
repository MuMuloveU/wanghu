#ifndef DLG_COMPAION_MANAGER_HEAD_FILE
#define DLG_COMPAION_MANAGER_HEAD_FILE

#pragma once

#include "Companion.h"
#include "CompanionTreeCtrl.h"
#include "CompanionListCtrl.h"

//////////////////////////////////////////////////////////////////////////

//好友管理对话框类
class CDlgCompanionManager : public CSkinDialogEx
{
	//变量定义
protected:
	ICompanionManager				* m_pICompanionManager;				//管理接口

	//控件变量
public:
	CSkinButton						m_btClose;							//关闭按钮
	CCompanionListCtrl				m_FriendListCtrl;					//好友列表
	CCompanionListCtrl				m_DetestListCtrl;					//厌恶列表
	CCompanionTreeCtrl				m_CompanionTreeCtrl;				//树性控件

	//函数定义
public:
	//构造函数
	CDlgCompanionManager();
	//析构函数
	virtual ~CDlgCompanionManager();

	//重载函数
protected:
	//控件绑定
	virtual void DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定函数
	virtual void OnOK() { return; }
	//取消消息
	virtual void OnCancel();

	//功能函数
public:
	//设置接口
	bool SetCompanionManager(IUnknownEx * pIUnknownEx);
	//加入关系
	bool InsertCompanionItem(const tagCompanionItem * pCompanionItem);
	//更新关系
	bool UpdateCompanionItem(const tagCompanionItem * pCompanionItem, bool bChangeCompanion);
	//删除关系
	bool DeleteCompanionItem(const tagCompanionItem * pCompanionItem);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif