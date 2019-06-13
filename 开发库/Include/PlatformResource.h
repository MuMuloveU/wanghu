#ifndef PLATFORM_RESOURCE_HEAD_FILE
#define PLATFORM_RESOURCE_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "SkinResource.h"
#include "SkinResourceModule.h"

//////////////////////////////////////////////////////////////////////////

//游戏资源
class CPlatformResource : public IPlatformResource
{
	//资源变量
protected:
	tagPlazaViewImage				*m_pPlazaViewImage ;
	tagServerViewImage				*m_pServerViewImage ;
	tagPlatformFrameImage			*m_pPlatformFrameImage;
	tagWhisperViewImage				*m_pWhisperViewImage ;
	tagControlViewImage				*m_pControlViewImage ;
	tagPropertyViewImage			*m_pPropertyViewImage ;
	tagChatExpViewImage				*m_pChatExpViewImage;
	tagGameFrameImage				*m_pGameFrameImage;
	tagVideoFrameImage				*m_pVideoFrameImage ;
	tagPropertyMenuImage			*m_pPropertyMenuImage ;
	tagGiftFlashImage				*m_pGiftFlashImage ;


	//皮肤类型
public:
	BYTE							m_cbResourceType;					//资源类型

	//内核变量
protected:
	HINSTANCE						m_hInstance;						//资源句柄
	CSkinResource					m_SkinResource;						//界面资源

	//函数定义
public:
	//构造函数
	CPlatformResource();
	//析构函数
	virtual ~CPlatformResource();

	//基础接口
public:
	//释放对象
	virtual VOID __cdecl Release() { delete this; }
	//接口查询
	virtual VOID * __cdecl QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//内核接口
public:
	//资源句柄
	virtual HINSTANCE __cdecl GetResInstance();
	//获取预览
	virtual bool __cdecl GetSkinPreview(CImage & ImagePreview);

	//资源类型
public:
	//资源类型
	virtual void __cdecl SetResourceType(BYTE cbResourceType);

	//界面资源
public:
	//界面资源
	virtual VOID * __cdecl GetSkinResource(REFGUID Guid, DWORD dwQueryVer);

	//大厅资源
public:
	//广场资源
	virtual bool __cdecl GetPlazaViewImage(tagPlazaViewImage * PlazaViewImage);
	//房间资源
	virtual bool __cdecl GetServerViewImage(tagServerViewImage * ServerViewImage);
	//框架资源
	virtual bool __cdecl GetPlatformFrameImage(tagPlatformFrameImage * PlatformFrameImage);

	//附加资源
public:
	//私聊资源
	virtual bool __cdecl GetWhisperViewImage(tagWhisperViewImage * WhisperViewImage);
	//控件资源
	virtual bool __cdecl GetControlViewImage(tagControlViewImage * ControlViewImage);
	//道具资源
	virtual bool __cdecl GetPropertyViewImage(tagPropertyViewImage * PropertyViewImage);
	//表情资源
	virtual bool __cdecl GetChatExpViewImage(tagChatExpViewImage * ChatExpViewImage);
	//游戏框架
	virtual bool __cdecl GetGameFrameImage(tagGameFrameImage * GameFrameImage);
	//视频资源
	virtual bool __cdecl GetVideoFrameImage(tagVideoFrameImage * VideoFrameImage);
	//道具菜单
	virtual bool __cdecl GetPropertyMenuImage(tagPropertyMenuImage * PropertyMenuImage);
	//动画资源
	virtual bool __cdecl GetGiftFlashImage(tagGiftFlashImage * GiftFlashImage);
};

//////////////////////////////////////////////////////////////////////////

#endif