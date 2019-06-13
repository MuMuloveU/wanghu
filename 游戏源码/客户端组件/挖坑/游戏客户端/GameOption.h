#pragma once

#include "Stdafx.h"
#include "Resource.h"

//////////////////////////////////////////////////////////////////////////

//象素定义
#define MAX_PELS					25									//最小象素
#define LESS_PELS					10									//最小象素
#define DEFAULT_PELS				18									//默认象素

//声音模式
enum enSoundMode
{
	enNoSound,						//没有声音
	enNormalSound,					//传统声音
	enMandarinSound,				//普通话声音
	enShanXiSound,					//陕西话声音
};

//////////////////////////////////////////////////////////////////////////

//游戏配置
class CGameOption : public CSkinDialogEx
{
	//变量定义
public:
	bool							m_bDeasilOrder;						//出牌顺序
	DWORD							m_dwCardHSpace;						//扑克象素
	enSoundMode						m_GameSoundMode;					//声音模式

	//控件变量
public:
	CSkinButton						m_btOK;								//确定按钮
	CSkinButton						m_btCancel;							//取消按钮
	CSkinButton						m_btDefault;						//默认按钮

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

	//消息映射
protected:
	//默认参数
	afx_msg void OnDefault();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
