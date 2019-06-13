#ifndef SKIN_RESOURCE_HEAD_FILE
#define SKIN_RESOURCE_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "SkinResourceModule.h"

//////////////////////////////////////////////////////////////////////////

//资源管理类
class CSkinResource : public ISkinResource
{
	//皮肤类型
public:
	BYTE						m_cbResourceType;						//资源类型

	//函数定义
public:
	//构造函数
	CSkinResource();
	//析构函数
	virtual ~CSkinResource();

	//基础接口
public:
	//释放对象
	virtual VOID __cdecl Release() { return; }
	//接口查询
	virtual VOID * __cdecl QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//资源接口
public:
	//创建字体
	virtual bool __cdecl CreateDefaultFont(CFont & Font);

	//资源接口
public:
	//获取信息
	virtual bool __cdecl GetResourceInfo(tagSkinEditResource & SkinEditResource);
	//获取信息
	virtual bool __cdecl GetResourceInfo(tagSkinDialogResource & SkinDialogResource);
	//获取信息
	virtual bool __cdecl GetResourceInfo(tagSkinButtonResource & SkinButtonResource);
	//获取信息
	virtual bool __cdecl GetResourceInfo(tagSkinSplitterResource & SkinSplitterResource);
	//获取信息
	virtual bool __cdecl GetResourceInfo(tagSkinListCtrlResource & SkinListCtrlResource);
	//获取信息
	virtual bool __cdecl GetResourceInfo(tagSkinHyperLinkResource & SkinHyperLinkResource);
};

//////////////////////////////////////////////////////////////////////////

#endif