#ifndef SYSTEM_OPTION_HEAD_FILE
#define SYSTEM_OPTION_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//全局配置类
class CGlobalOption
{
	//消息设置
public:
	bool							m_bShowInOutMessage;				//进出信息
	CPlatformResourceHelper			m_PlatformResourceModule;			//资源组件

	//资源变量
public:
	tagChatExpViewImage				m_ChatExpViewImage;					//表情资源
	tagGameFrameImage				m_GameFrameImage;					//框架资源
	tagPropertyViewImage			m_PropertyViewImage;				//道具资源
	tagVideoFrameImage				m_VideoFrameImage;					//视频资源
	tagPropertyMenuImage			m_PropertyMenuImage;				//菜单资源
	tagGiftFlashImage				m_GiftFlashImage;					//动画资源

	//颜色设置
public:
	COLORREF						m_crChatTX;							//聊天字体
	COLORREF						m_crMsgName;						//名字消息
	COLORREF						m_crMsgNormal;						//普通消息
	COLORREF						m_crMsgSysHead;						//系统消息
	COLORREF						m_crMsgSysString;					//系统消息

	//函数定义
public:
	//构造函数
	CGlobalOption();
	//析构函数
	virtual ~CGlobalOption();

	//功能函数
public:
	//加载资源
	bool LoadGameFrameRes();
	//加载设置
	void LoadOptionParameter();
	//保存设置
	void SaveOptionParameter();
};

//////////////////////////////////////////////////////////////////////////

//全局信息 
extern	CGlobalOption					g_GlobalOption;					//系统配置

//////////////////////////////////////////////////////////////////////////

#endif