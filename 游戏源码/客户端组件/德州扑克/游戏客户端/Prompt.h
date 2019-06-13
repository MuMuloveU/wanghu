#ifndef PROMPT_VIEW_HEAD_FILE
#define PROMPT_VIEW_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

#pragma once

#include "Stdafx.h"
#include "Resource.h"

//提示框
class CPrompt : public CDialog
{
	//数据变量
protected:
	TCHAR							m_tStr[1024];				//对白

public:
	//构造函数
	CPrompt();   
	//析构函数
	virtual ~CPrompt();

	//重载函数
protected:
	//控件绑定
	virtual void DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();

	//消息函数
protected:
	//重画函数
	afx_msg void OnPaint();
	//
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//命令信息
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	//
public:
	//设置对白
	void SetString(TCHAR szString[]);

	DECLARE_MESSAGE_MAP()

};

//////////////////////////////////////////////////////////////////////////

#endif