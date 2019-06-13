#pragma once
#ifndef RULESET_HEAD_FILE
#define RULESET_HEAD_FILE

#endif
#include "StdAfx.h"
#include "Resource.h"
#include "afxwin.h"

//规则设置
class CGameRuleSet : public CSkinDialogEx
{
	DECLARE_DYNAMIC(CGameRuleSet)
	//变量定义
public:

	enum						{ IDD = IDD_INFO };
	CSkinButton					m_Play;							//开始游戏按钮

	//函数定义
public:
	//构造函数
	CGameRuleSet(CWnd* pParent = NULL);   // 标准构造函数
	//析构函数
	virtual ~CGameRuleSet();
	//初始化函数
	virtual BOOL OnInitDialog();

protected:
	//DDX/DDV 支持
	virtual void DoDataExchange(CDataExchange* pDX);   

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedStratPlay();
	afx_msg void OnClose();
};
