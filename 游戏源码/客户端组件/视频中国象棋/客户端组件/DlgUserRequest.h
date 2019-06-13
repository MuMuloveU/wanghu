#ifndef DLG_USER_REQUEST_HEAD_FILE
#define DLG_USER_REQUEST_HEAD_FILE

#pragma once

#include "Stdafx.h"
//#include "GameClient.h"

//////////////////////////////////////////////////////////////////////////

//内容定义
#define REQUEST_PEACE				1									//和棋请求
#define REQUEST_REGRET				2									//悔棋请求

//消息定义
#define IDM_USER_REQUEST_RESPOND	(WM_USER+400)						//请求回应

//////////////////////////////////////////////////////////////////////////

//用户请求
class CDlgUserRequest : public CDialog
{
	//变量定义
public:
	BYTE							m_cbRequestType;					//请求类型
	LONG							m_nResidualTime;					//剩余时间
	CSkinImage						m_ImageViewBack;					//背景资源

	//控件变量
protected:
	CSkinButton						m_btOk;								//按钮控件
	CSkinButton						m_btCancel;							//按钮控件

	//函数定义
public:
	//构造函数
	CDlgUserRequest();
	//析构函数
	virtual ~CDlgUserRequest();

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定消息
	virtual VOID OnOK();
	//取消消息
	virtual VOID OnCancel();

	//消息映射
protected:
	//重画函数
	VOID OnPaint();
	//时间消息
	VOID OnTimer(UINT nIDEvent);
	//绘画背景
	BOOL OnEraseBkgnd(CDC * pDC);
	//鼠标消息
	VOID OnLButtonDown(UINT nFlags, CPoint Point);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif
