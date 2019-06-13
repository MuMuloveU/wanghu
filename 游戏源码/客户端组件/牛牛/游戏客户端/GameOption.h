#pragma once

#include "Stdafx.h"
#include "Resource.h"

//////////////////////////////////////////////////////////////////////////

//象素定义
#define MAX_PELS					25									//最小象素
#define LESS_PELS					10									//最小象素
#define DEFAULT_PELS				20									//默认象素

//////////////////////////////////////////////////////////////////////////

//游戏配置
class CGameOption : public CSkinDialogEx
{
	//变量定义
public:
	bool							m_bEnableSound;						//启用声音
	bool							m_bAllowLookon;						//允许旁观
	DWORD							m_dwCardHSpace;						//扑克象素

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
