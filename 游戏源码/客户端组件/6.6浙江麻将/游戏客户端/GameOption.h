#pragma once

#include "Stdafx.h"
#include "Resource.h"

//////////////////////////////////////////////////////////////////////////

//游戏配置
class CGameOption : public CSkinDialogEx
{
	//变量定义
public:
	bool							m_bEnableSound;						//启用声音
	bool							m_bAllowLookon;						//允许旁观

	//控件变量
public:
	CSkinButton						m_btOK;								//确定按钮
	CSkinButton						m_btCancel;							//取消按钮

	//函数定义
public:
	//构造函数
	CGameOption();
	//析构函数
	virtual ~CGameOption();

	//重载函数
protected:
	//控件绑定
	virtual void DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定消息
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
