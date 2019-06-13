#ifndef CLIENT_SHARE_HEAD_FILE
#define CLIENT_SHARE_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//平台头文件
#include "Template.h"

//组件头文件
#include "SkinControls.h"

//////////////////////////////////////////////////////////////////////////
//公共宏定义

//导出定义
#ifndef CLIENT_SHARE_CLASS
	#ifdef  CLIENT_SHARE_DLL
		#define CLIENT_SHARE_CLASS _declspec(dllexport)
	#else
		#define CLIENT_SHARE_CLASS _declspec(dllimport)
	#endif
#endif

//模块定义
#ifdef _DEBUG
	#define CLIENT_SHARE_DLL_NAME	TEXT("ClientShareD.dll")			//组件 DLL 名字
#else
	#define CLIENT_SHARE_DLL_NAME	TEXT("ClientShare.dll")				//组件 DLL 名字
#endif

//////////////////////////////////////////////////////////////////////////
//结构体定义

//消息配置
struct tagMessageOption
{
	COLORREF							crName;							//名字颜色
	COLORREF							crSystemHead;					//系统颜色
	COLORREF							crSystemString;					//系统颜色
	TCHAR								szDefaultFont[32];				//默认字体
};

//消息大小
#define MS_NORMAL						0								//常规大小
#define MS_MIDDLING						1								//中等大小
#define MS_LARGE						2								//大号字体

//////////////////////////////////////////////////////////////////////////

#define VER_IMessageProxy INTERFACE_VERSION(1,1)
static const GUID IID_IMessageProxy={0xe8bd2387,0x9c2a,0x4346,0xb3,0x2c,0x17,0xb7,0xb3,0x51,0x9a,0x2f};

//信息代理接口
interface IMessageProxy : public IUnknownEx
{
	//配置接口
public:
	//设置句柄
	virtual bool __cdecl SetRichEditHwnd(CSkinRichEdit * pSkinRichEdit)=NULL;
	//设置颜色
	virtual bool __cdecl SetMessageOption(tagMessageOption & MessageOption)=NULL;

	//控制接口
public:
	//清除屏幕
	virtual bool __cdecl CleanScreen()=NULL;
	//设置滚动
	virtual bool __cdecl SetAutoScroll(bool bScroll)=NULL;
	//加载消息
	virtual bool __cdecl LoadMessage(LPCTSTR pszFileName)=NULL;
	//保存信息
	virtual bool __cdecl SaveMessage(LPCTSTR pszFileName)=NULL;
	//获取指针
	virtual CSkinRichEdit * __cdecl GetRichEdit()=NULL;

	//信息接口
public:
	//插入图片
	virtual bool __cdecl InsertImage(CBitmap * pBitmap)=NULL;
	//插入时间
	virtual bool __cdecl InsertSystemTime(COLORREF crColor)=NULL;
	//用户名字
	virtual bool __cdecl InsertUserName(LPCTSTR pszUserName)=NULL;
	//系统消息
	virtual bool __cdecl InsertSystemString(LPCTSTR pszString, UINT uSize)=NULL;
	//特色消息
	virtual bool __cdecl InsertShtickString(LPCTSTR pszString, COLORREF crColor, UINT uSize, bool bNewLine)=NULL;
	//普通消息
	virtual bool __cdecl InsertGeneralString(LPCTSTR pszString, COLORREF crColor, UINT uSize, bool bNewLine)=NULL;
	//用户聊天
	virtual bool __cdecl InsertUserChat(LPCTSTR pszSendName, LPCTSTR pszString, COLORREF crColor, UINT uSize)=NULL;
	//用户聊天
	virtual bool __cdecl InsertUserChat(LPCTSTR pszSendName, LPCTSTR pszRecvName, LPCTSTR pszString, COLORREF crColor, UINT uSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////

//消息代理辅助类
class CMessageProxyHelper : public CTempldateHelper<IMessageProxy>
{
	//函数定义
public:
	//构造函数
	CMessageProxyHelper(void) : CTempldateHelper<IMessageProxy>(IID_IMessageProxy,
		VER_IMessageProxy,CLIENT_SHARE_DLL_NAME,TEXT("CreateMessageProxy")) { }
};

//////////////////////////////////////////////////////////////////////////
//导出类头文件

#include "Expression.h"
#include "HintMsgBox.h"

//////////////////////////////////////////////////////////////////////////

#endif