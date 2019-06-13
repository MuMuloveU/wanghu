#ifndef PLAZA_USER_INFO_VIEW_HEAD_FILE
#define PLAZA_USER_INFO_VIEW_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "Resource.h"

//////////////////////////////////////////////////////////////////////////

//用户信息
class CUserInfoView : public CDialog
{
	//状态变量
protected:
	bool							m_bShowInfo;						//显示标志
	bool							m_bCreateFlag;						//创建标志
	UINT							m_nTitleHeight;						//标题高度

	//资源变量
protected:
	CSkinImage						m_ImageTitleL;						//位图对象
	CSkinImage						m_ImageTitleM;						//位图对象
	CSkinImage						m_ImageTitleR;						//位图对象

	//函数定义
public:
	//构造函数
	CUserInfoView();
	//析构函数
	virtual ~CUserInfoView();

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定函数
	virtual VOID OnOK() { return; };
	//取消函数
	virtual VOID OnCancel() { return; };

	//功能函数
public:
	//标题高度
	UINT GetTitleHeight();
	//更新信息
	VOID UpdateUserInfo();
	//显示信息
	VOID ShowUserInfo(bool bShowInfo);
	//更新资源
	bool UpdateSkinResource();

	//消息函数
protected:
	//绘画背景
	BOOL OnEraseBkgnd(CDC * pDC);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif