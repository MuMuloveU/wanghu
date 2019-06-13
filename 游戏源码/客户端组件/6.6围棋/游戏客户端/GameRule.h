#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////
//消息定义

#define IDM_AGREE_GAME_RULE			(WM_USER+300)						//同意消息

//////////////////////////////////////////////////////////////////////////

//游戏规则
class CGameRule : public CDialog
{
	//游戏规则
public:
	tagGameRuleInfo					m_GameRuleInfo;						//游戏规则

	//资源变量
public:
	CBrush							m_Brush;							//背景画刷
	CSkinImage						m_ImageBack;						//背景资源

	//控件变量
public:
	CSkinButton						m_btOk;								//按钮控件
	CSkinButton						m_btCancel;							//按钮控件

	//函数定义
public:
	//构造函数
	CGameRule();
	//析构函数
	virtual ~CGameRule();

	//重载函数
protected:
	//控件绑定
	virtual void DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定消息
	virtual void OnOK();
	//取消消息
	virtual void OnCancel();

	//功能函数
public:
	//更新状态
	void UpdateControlStatus();
	//允许控制
	void ControlEnable(bool bEnable);
	
	//内部函数
private:
	//设置数值
	bool SetComboBoxValue(UINT uComboBoxID, WORD wValue);
	//获取数值
	bool GetComboBoxValue(UINT uComboBoxID, WORD & wValue);

	//消息映射
protected:
	//重画函数
	afx_msg void OnPaint();
	//销毁消息
	afx_msg void OnDestroy();
	//绘画背景
	afx_msg BOOL OnEraseBkgnd(CDC * pDC);
	//鼠标消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//控件颜色
	afx_msg HBRUSH OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor);

	//控件消息
protected:
	//限定步时
	afx_msg void OnBnClickedEnableStep();
	//限定步时
	afx_msg void OnBnClickedEnableLimit();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
