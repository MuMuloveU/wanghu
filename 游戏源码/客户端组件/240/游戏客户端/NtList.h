#pragma once

#include "StdAfx.h"
#include "Resource.h"


class CNtList : public CDialog
{
	//控件变量
public:
	CBitmapButton		m_Bm4;							//按钮
	CBitmapButton		m_Bm3;							//按钮
	CBitmapButton		m_Bm2;							//按钮
	CBitmapButton		m_Bm1;							//按钮
	CBitmapButton		m_Bm5;							//按钮
	// 大王
	CBitmapButton		m_bt20;

	enum				{ IDD = IDD_NT_LIST };			//ID 号码
	//函数定义
public:
	//构造函数
	CNtList(CWnd * pParent = NULL);
	//更新列表
	void UpdateNtList(BOOL b[]);

protected:
	//DDX 支持
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	//消息函数
protected:
	//初始化函数
	virtual BOOL OnInitDialog();
	//按钮消息
	afx_msg void On1();
	//按钮消息
	afx_msg void On2();
	//按钮消息
	afx_msg void On3();
	//按钮消息
	afx_msg void On4();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClicked5();
	
	afx_msg void OnBnClicked7();
};

