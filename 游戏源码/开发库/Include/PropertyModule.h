#ifndef PROPERTY_MODULE_HEAD_FILE1
#define PROPERTY_MODULE_HEAD_FILE1
//////////////////////////////////////////////////////////////////////////

//全局头文件
#include "GlobalProperty.h"

//模板库
#include "Template.h"
#include "Module.h"

//组件头文件
#include "ClientShare.h"
#include "NetworkServiceHead.h"

//接口头文件
#include "ComService.h"
#include "SkinControls.h"
#include "SkinResourceModule.h"

//////////////////////////////////////////////////////////////////////////

//导出定义
#ifndef PROPERTY_MODULE_CLASS
	#ifdef PROPERTY_MODULE_DLL
		#define PROPERTY_MODULE_CLASS _declspec(dllexport)
	#else
		#define PROPERTY_MODULE_CLASS _declspec(dllimport)
	#endif
#endif

//模块定义
#ifdef _DEBUG
	#define PROPERTY_MODULE_DLL_NAME TEXT("PropertyModuleD.dll")
#else
	#define PROPERTY_MODULE_DLL_NAME TEXT("PropertyModule.dll")
#endif

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IPurchaseInfo INTERFACE_VERSION(1,1)
	static const GUID IID_IPurchaseInfo={0xaa45290b,0xb9f1,0x4469,0x00a1,0x7b,0x04,0xcc,0xb1,0x77,0x61,0x7d};
#else
	#define VER_IPurchaseInfo INTERFACE_VERSION(1,1)
	static const GUID IID_IPurchaseInfo={0xdc5ae116,0x48a5,0x48f7,0x008e,0x18,0xfd,0xb5,0x46,0x2f,0xbc,0xc4};
#endif

//////////////////////////////////////////////////////////////////////////

//鲜花接口
interface IPurchaseInfo: public IUnknownEx
{
	//函数定义
public:
	//购买场所
	virtual BYTE __cdecl GetLocation()=NULL;
	//房间类型
	virtual WORD __cdecl GetGameGenre()=NULL;
	//枚举玩家
	virtual tagUserData * __cdecl EnumLookonUser(WORD wIndex)=NULL;
	//查找用户
	virtual tagUserData * __cdecl SearchUserItem(DWORD dwUserID)=NULL;
	//查找用户
	virtual tagUserData * __cdecl SearchUserItem(LPCTSTR pszUserName)=NULL;
	//获取玩家
	virtual const tagUserData * __cdecl GetUserInfo(WORD wChairID)=NULL;
	//获取自己
	virtual const tagUserData * __cdecl GetMeUserInfo()=NULL;
	//发送信息
	virtual void __cdecl SendData(WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////

//导出类头文件 
#include "SingleProItem.h"
#include "CharmValueExchange.h"
#include "BankStorage.h"

//////////////////////////////////////////////////////////////////////////
#endif