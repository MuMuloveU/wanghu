#ifndef USER_INFO_VIEW_HEAD_FILE
#define USER_INFO_VIEW_HEAD_FILE

#pragma once

#include "StdAfx.h"
#include "GameFrame.h"

//////////////////////////////////////////////////////////////////////////

//用户信息
class CUserInfoView : public CWnd
{
	//接口变量
protected:
	IClientKernel *					m_pIClientKernel;					//内核接口
	tagUserData const*				m_pCurrentUserData;					//当前用户

	//组件指针
private:
	IUserFaceRes					* m_pIUserFaceRes;					//头像接口

	//资源变量
protected:
	CSkinImage						m_ImageMaleFace;					//头像图片
	CSkinImage						m_ImageFemaleFace;					//头像图片

	//控件变量
protected:
	CSkinButton						m_btExchange;						//兑换按钮

	//函数定义
public:
	//构造函数
	CUserInfoView();
	//析构函数
	virtual ~CUserInfoView();

	//配置函数
public:
	//设置接口
	bool SetUserFaceRes(IUnknownEx * pIUnknownEx);

	//功能函数
public:
	//更新资源
	bool UpdateSkinResource();
	//设置用户
	VOID SetUserViewInfo(tagUserData const* pCurrentUserData);
	//设置内核
	void SetClientKernel(IClientKernel *pIClientKernel);
	//更新函数
	void UpdateUserInfo();
	//获取函数
	tagUserData const *GetCurrentUser();

	//内部函数
protected:
	//调整函数
	void RectifyControl(int nWidth, int nHeight);
	//更新控件
	void UpdateControls();

	//消息映射
protected:
	//绘画函数
	VOID OnPaint();
	//创建消息
	INT OnCreate(LPCREATESTRUCT lpCreateStruct);
	//兑换消息
	afx_msg void OnBtnClickedExchange();
	//窗口大小
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif