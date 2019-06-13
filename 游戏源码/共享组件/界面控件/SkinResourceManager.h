#ifndef SKIN_RESOURCE_MANAGER_HEAD_FILE
#define SKIN_RESOURCE_MANAGER_HEAD_FILE

#pragma once

#include "SkinControls.h"

//////////////////////////////////////////////////////////////////////////

//资源管理
class SKIN_CONTROL_CLASS CSkinResourceManager
{
	//资源变量
protected:
	static CFont					m_DefaultFont;						//默认字体
	static ISkinResource *			m_pISkinResource;					//资源接口

	//函数定义
protected:
	//构造函数
	CSkinResourceManager();
	//析构函数
	virtual ~CSkinResourceManager();

	//资源函数
public:
	//默认字体
	static HFONT GetDefaultFont() { return m_DefaultFont; }//?

	//管理函数
public:
	//加载资源
	static bool LoadSkinResource(IUnknownEx * pIUnknownEx);
	//获取资源
	static VOID * GetSkinResource(REFGUID Guid, DWORD dwQueryVer);
};

//////////////////////////////////////////////////////////////////////////

#endif
