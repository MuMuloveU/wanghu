#ifndef DLG_OPTION_HEAD_FILE
#define DLG_OPTION_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "Resource.h"
#include "SystemOption.h"

//////////////////////////////////////////////////////////////////////////

//类说明
class CDlgOptionItem;
typedef CArrayTemplate<CDlgOptionItem *> COptionsItemPtrArray;

//////////////////////////////////////////////////////////////////////////

//按钮工具栏
class CButtonBar : public CWnd
{
	//变量定义
protected:
	INT_PTR								m_nHotItem;						//激活项
	INT_PTR								m_nActiveItem;					//激活项
	COptionsItemPtrArray				m_OptionsItemPtr;				//按钮数组

	//其他信息
protected:
	CWnd								* m_pItemParent;				//项父窗口
	CRect								m_rcItemRect;					//设置框位置
	
	//位图资源
protected:
	CSkinImage							m_ImageHot;						//资源变量
	CSkinImage							m_ImageActive;					//资源变量
	CSkinImage							m_ImageNormal;					//资源变量
	CSkinImage							m_ImageBackGround;				//资源变量
	
	//函数定义
public:
	//构造函数
	CButtonBar();
	//析构函数
	virtual ~CButtonBar();
	
	//重载函数
public:
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	
	//功能函数
public:
	//设置信息
	bool InitButtonBar(const CRect & rcItemRect, CWnd * pItemParent);
	//激活设置
	bool SetActiveOptionItem(CDlgOptionItem * pDlgOptionItem);
	//加入设置
	bool InsertOptionItem(CDlgOptionItem * pDlgOptionItem);
	//更新资源
	bool UpdateSkinResource();
	
	//内部函数
private:
	//按键测试
	INT_PTR ItemHitTest(CPoint MousePoint);
	
	//消息函数
protected:
	//重画消息
	afx_msg void OnPaint();
	//鼠标消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//光标消息
	afx_msg BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT message);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//设置子项
class CDlgOptionItem : public CDialog
{
	//变量定义
private:
	bool							m_bCreate;							//是否建立
	const UINT						m_uIDTemplate;						//模板标示

	//函数定义
public:
	//构造函数
	CDlgOptionItem(UINT uIDTemplate);
	//析构函数
	virtual ~CDlgOptionItem();

	//重载函数
protected:
	//确定消息
	virtual void OnOK() { GetParent()->PostMessage(WM_COMMAND,IDOK,0); }
	//取消消息
	virtual void OnCancel() { GetParent()->PostMessage(WM_COMMAND,IDCANCEL,0); }

	//重载函数
public:
	//保存设置
	virtual bool SaveOptions()=NULL;
	//获取标题
	virtual LPCTSTR GetTitleText()=NULL;
	//获取标题
	virtual LPCTSTR GetButtonText()=NULL;

	//功能函数
public:
	//创建函数
	bool CreateOptionsItem(CWnd * pParentWmd);
	//是否选中
	bool IsButtonSelected(UINT uButtonID);
	//选中按钮
	void SetButtonSelected(UINT uButtonID, bool bSelected);
	
	//消息函数
protected:
	//重画函数
	afx_msg void OnPaint();
	//背景消息
	afx_msg BOOL OnEraseBkgnd(CDC * pDC);
	//颜色消息
	afx_msg HBRUSH OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//设置参数
struct tagSystemParameter
{
	enInviteMode						enInviteMode;					//邀请模式
	enAcountsRule						enAcountsRule;					//帐户规则
	bool								bShowInOutMessage;				//进出信息
	WORD								wBossHotKey;					//老板热键
};

//系统设置
class CDlgOptionSystem : public CDlgOptionItem
{
	//配置变量
public:
	tagSystemParameter					m_SystemParameter;				//配置参数

	//函数定义
public:
	//构造函数
	CDlgOptionSystem();
	//析构函数
	virtual ~CDlgOptionSystem();

	//重载函数
public:
	//初始化函数
	virtual BOOL OnInitDialog();

	//重载函数
public:
	//保存设置
	virtual bool SaveOptions();
	//获取标题
	virtual LPCTSTR GetTitleText();
	//获取标题
	virtual LPCTSTR GetButtonText();
};

//////////////////////////////////////////////////////////////////////////

//设置参数
struct tagTableRuleParameter
{
	bool								bPassword;							//设置密码
	bool								bLimitWin;							//限制胜率
	bool								bLimitFlee;							//限制断线
	bool								bLimitScore;						//限制分数
	bool								bLimitDetest;						//限制玩家
	bool								bCheckSameIP;						//效验地址
	WORD								wWinRate;							//胜利率
	WORD								wFleeRate;							//逃跑率
	LONG								lMaxScore;							//最高分数 
	LONG								lLessScore;							//最低分数
	TCHAR								szPassword[PASS_LEN];				//桌子密码
};

//同桌设置
class CDlgOptionTableRule : public CDlgOptionItem
{
	//配置变量
public:
	tagTableRuleParameter				m_TableRuleParameter;				//配置参数

	//函数定义
public:
	//构造函数
	CDlgOptionTableRule();
	//析构函数
	virtual ~CDlgOptionTableRule();

	//重载函数
public:
	//初始化函数
	virtual BOOL OnInitDialog();

	//重载函数
public:
	//保存设置
	virtual bool SaveOptions();
	//获取标题
	virtual LPCTSTR GetTitleText();
	//获取标题
	virtual LPCTSTR GetButtonText();
};

//////////////////////////////////////////////////////////////////////////

//设置对话框类
class CDlgOption : public CSkinDialogEx
{
	//配置变量
protected:
	CGameOption							* m_pGameOption;				//游戏设置
	CServerOption						* m_pServerOption;				//房间设置

	//控件变量
public:
	CButtonBar							m_ButtonBar;					//选择按钮
	CSkinButton							m_btOk;							//确定按钮
	CSkinButton							m_btCancel;						//取消按钮

	//配置子项
public:
	CDlgOptionSystem					m_SystemItem;					//系统配置
	CDlgOptionTableRule					m_TableRuleItem;				//规则设置

	//函数定义
public:
	//构造函数
	CDlgOption(CGameOption * pGameOption, CServerOption * pServerOption);
	//析构函数
	virtual ~CDlgOption();

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

	//消息函数
public:
	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//设置密码框类
class CDlgEnterTablePass : public CSkinDialogEx
{
	//密码变量
private:
	LPTSTR								m_lpPassword;					//桌子密码
	BYTE								m_cbPasswordLen;				//密码长度

	//控件变量
public:
	CSkinButton							m_btOk;							//确定按钮
	CSkinButton							m_btCancel;						//取消按钮

	//函数定义
public:
	//构造函数
	CDlgEnterTablePass();
	//析构函数
	virtual ~CDlgEnterTablePass();

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
	//设置变量
	void SetPassword(LPTSTR lpPassword,BYTE cbPasswordLen);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif