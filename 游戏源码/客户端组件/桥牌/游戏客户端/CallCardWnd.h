#ifndef CALL_CARD_WND_HEAD_FILE
#define CALL_CARD_WND_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "Resource.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

//花色定义
#define CALL_NONE					0x00								//叫牌定义
#define CALL_NT						0x10								//叫牌定义
#define CALL_HEI_TAO				0x01								//叫牌定义
#define CALL_HONG_TAO				0x02								//叫牌定义
#define CALL_MEI_HUA				0x04								//叫牌定义
#define CALL_FANG_KUAI				0x08								//叫牌定义

//消息定义
#define IDM_CALL_CARD				(WM_USER+200)						//叫牌消息

//////////////////////////////////////////////////////////////////////////

//叫牌窗口
class CCallCardWnd : public CWnd
{
	//按钮控件
protected:
	TransButton						m_btCallNT;							//叫牌按钮
	TransButton						m_btCallHeiTao;						//叫牌按钮
	TransButton						m_btCallHongTao;					//叫牌按钮
	TransButton						m_btCallMeiHua;						//叫牌按钮
	TransButton						m_btCallFangKuai;					//叫牌按钮

	//函数定义
public:
	//构造函数
	CCallCardWnd();
	//析构函数
	virtual ~CCallCardWnd();

	//重载函数
public:
	//命令函数
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	//功能函数
public:
	//更新控制
	void UpdateCallControl(BYTE cbCallColor);
	//更新控制
	void UpdateCallControl(bool bCallNT, bool bCallHeiTao, bool bCallHongTao, bool CallMeiHua, bool bCallFangKuai);

	//消息函数
protected:
	//建立消息
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif