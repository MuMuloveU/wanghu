#ifndef DLG_RULE_OPTION_HEAD_FILE
#define DLG_RULE_OPTION_HEAD_FILE

#pragma once

#include "Stdafx.h"
//#include "GameClient.h"

//////////////////////////////////////////////////////////////////////////
//消息定义

#define IDM_GAME_RULE_AGREE			(WM_USER+300)						//同意消息
#define IDM_GAME_RULE_REJECT		(WM_USER+301)						//拒绝消息

//////////////////////////////////////////////////////////////////////////

//游戏规则
class CDlgRuleOption : public CDialog
{
	//状态变量
protected:
	LONG							m_nResidualTime;					//剩余时间

	//规则变量
protected:
	bool							m_bControl;							//控制标志
	bool							m_bSelectMode;						//模式标志
	tagGameRuleInfo					m_GameRuleInfo;						//游戏规则

	//资源变量
protected:
	CBrush							m_Brush;							//背景画刷
	CSkinImage						m_ImageViewBack;					//背景资源

	//控件变量
protected:
	CSkinButton						m_btOk;								//按钮控件
	CSkinButton						m_btCancel;							//按钮控件

	//函数定义
public:
	//构造函数
	CDlgRuleOption();
	//析构函数
	virtual ~CDlgRuleOption();

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定消息
	virtual VOID OnOK();
	//取消消息
	virtual VOID OnCancel();

	//功能函数
public:
	//设置控制
	VOID EnableControl(bool bControl);
	//获取规则
	VOID GetGameRuleInfo(tagGameRuleInfo & GameRuleInfo);
	//规则设置
	VOID SetGameRuleInfo(const tagGameRuleInfo & GameRuleInfo, bool bControl, bool bSelectMode);

	//内部函数
private:
	//更新状态
	VOID UpdateControlStatus();
	//设置数值
	bool SetComboBoxValue(UINT uComboBoxID, WORD wValue);
	//获取数值
	bool GetComboBoxValue(UINT uComboBoxID, WORD & wValue);

	//按钮消息
protected:
	//快棋模式
	VOID OnBnClickedFastMode();
	//慢棋模式
	VOID OnBnClickedSlowMode();
	//自定模式
	VOID OnBnClickedCustomMode();

	//消息映射
protected:
	//重画函数
	VOID OnPaint();
	//销毁消息
	VOID OnDestroy();
	//时间消息
	VOID OnTimer(UINT nIDEvent);
	//绘画背景
	BOOL OnEraseBkgnd(CDC * pDC);
	//鼠标消息
	VOID OnLButtonDown(UINT nFlags, CPoint Point);
	//控件颜色
	HBRUSH OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif
