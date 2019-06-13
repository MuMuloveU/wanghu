#ifndef SKIN_CONTROLS_HEAD_FILE
#define SKIN_CONTROLS_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//系统头文件
#include <GdiPlus.h>
#include <RichOle.h>
#include <AtlImage.h>
using namespace Gdiplus;

//平台头文件
#include "..\..\模板库\Template.h"

//QQ控件
#import "ImageOle.dll" named_guids
//////////////////////////////////////////////////////////////////////////
//公共宏定义

//导出定义
#ifndef SKIN_CONTROL_CLASS
	#ifdef  SKIN_CONTROL_DLL
		#define SKIN_CONTROL_CLASS _declspec(dllexport)
	#else
		#define SKIN_CONTROL_CLASS _declspec(dllimport)
	#endif
#endif

//模块定义
#ifdef _DEBUG
	#define SKIN_CONTROL_DLL_NAME	TEXT("SkinControlsD.dll")			//组件 DLL 名字
#else
	#define SKIN_CONTROL_DLL_NAME	TEXT("SkinControls.dll")			//组件 DLL 名字
#endif
//////////////////////////////////////////////////////////////////////////

//加载位图结构
struct tagImageLoadInfo
{
	//变量定义
	UINT								uResourceID;						//资源 ID
	CString								strFileName;						//文件名字
	HINSTANCE							hResourceDLL;						//资源句柄
	//LPCTSTR								pszResource;						//资源标识

	//函数定义
	tagImageLoadInfo()
	{
		uResourceID=0;
		hResourceDLL=NULL;
	}
};

//////////////////////////////////////////////////////////////////////////
//枚举定义

//属性类型枚举
enum enAttributeType
{
	Type_EMPTY,
	Type_VOID,
	Type_INT,
	Type_BYTE,
	Type_WORD,
	Type_LONG,
	Type_DWORD,
	Type_DOUBLE,
	Type_STRING,
};

//渲染信息
struct tagImageRender
{
	INT								nXImage;										//图片位置
	INT								nYImage;										//图片位置
	INT								nXScreen;										//渲染位置
	INT								nYScreen;										//渲染位置
	INT								cxRender;										//渲染尺寸
	INT								cyRender;										//渲染尺寸
};

//////////////////////////////////////////////////////////////////////////

#define VER_IMouseEvent INTERFACE_VERSION(1,1)
static const GUID IID_IMouseEvent={0x4bbf8f62,0xa706,0x45c2,0x97,0x65,0xb8,0x47,0xaa,0x7,0xe7,0x8e};

//鼠标事件回调接口
interface IMouseEvent : public IUnknownEx
{
	//鼠标离开
	virtual HRESULT __cdecl OnEventMouseLeft(UINT uControlID, WPARAM wParam, LPARAM lParam)=NULL;
	//鼠标移动
	virtual HRESULT __cdecl OnEventMouseMove(UINT uControlID, WPARAM wParam, LPARAM lParam)=NULL;
	//鼠标按钮
	virtual HRESULT __cdecl OnEventLButtonUp(UINT uControlID, WPARAM wParam, LPARAM lParam)=NULL;
	//鼠标按钮
	virtual HRESULT __cdecl OnEventLButtonDown(UINT uControlID, WPARAM wParam, LPARAM lParam)=NULL;
	//鼠标按钮
	virtual HRESULT __cdecl OnEventRButtonUp(UINT uControlID, WPARAM wParam, LPARAM lParam)=NULL;
	//鼠标按钮
	virtual HRESULT __cdecl OnEventRButtonDown(UINT uControlID, WPARAM wParam, LPARAM lParam)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_IAttribute INTERFACE_VERSION(1,1)
static const GUID IID_IAttribute={0xf75b8e91,0x5e0d,0x4e3a,{0xb7,0xae,0xa7,0x72,0x3d,0x4,0x41,0x8a}};

//属性项接口
interface IAttribute : public IUnknownEx
{
	//获取哈希值
	virtual DWORD __cdecl GetHashValue()=NULL;
	//是否只读
	virtual bool __cdecl IsReadOnly()=NULL;
	//获取名字
	virtual LPCTSTR __cdecl GetAttributeName()=NULL;
	//获取类型
	virtual enAttributeType __cdecl GetAttributeType()=NULL;
	//获取 PTR
	virtual bool __cdecl GetValue(VOID * * ppvValue)=NULL;
	//获取 INT
	virtual bool __cdecl GetValue(INT & iValue)=NULL;
	//获取 BYTE
	virtual bool __cdecl GetValue(BYTE & cbValue)=NULL;
	//获取 WORD
	virtual bool __cdecl GetValue(WORD & wValue)=NULL;
	//获取 LONG
	virtual bool __cdecl GetValue(LONG & lValue)=NULL;
	//获取 DWORD
	virtual bool __cdecl GetValue(DWORD & dwValue)=NULL;
	//获取 DOUBLE
	virtual bool __cdecl GetValue(DOUBLE & dValue)=NULL;
	//获取字符串
	virtual bool __cdecl GetValue(CString & strValue)=NULL;
	//获取变量
	virtual bool __cdecl GetVarValue(CString & strValue)=NULL;
	//设置 INT
	virtual bool __cdecl SetValue(void * pValue)=NULL;
	//设置 INT
	virtual bool __cdecl SetValue(INT iValue)=NULL;
	//设置 BYTE
	virtual bool __cdecl SetValue(BYTE cbValue)=NULL;
	//设置 WORD
	virtual bool __cdecl SetValue(WORD wValue)=NULL;
	//设置 LONG
	virtual bool __cdecl SetValue(LONG lValue)=NULL;
	//设置 DWORD
	virtual bool __cdecl SetValue(DWORD dwValue)=NULL;
	//设置 DOUBLE
	virtual bool __cdecl SetValue(DOUBLE dValue)=NULL;
	//设置 STRING
	virtual bool __cdecl SetValue(LPCTSTR pszValue)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_IAttributeManager INTERFACE_VERSION(1,1)
static const GUID IID_IAttributeManager={0x5935b008,0xeaa,0x432b,{0xa4,0x89,0x3c,0x5a,0xd7,0xda,0x17,0xb1}};

//属性管理接口
interface IAttributeManager : public IUnknownEx
{
	//注销属性
	virtual bool __cdecl UnRegisterAttribute(LPCTSTR pszName)=NULL;
	//注册属性
	virtual bool __cdecl RegisterAttribute(LPCTSTR pszName, VOID * pPtr, bool bReadOnly)=NULL;
	//注册属性
	virtual bool __cdecl RegisterAttribute(LPCTSTR pszName, INT * pInt, bool bReadOnly)=NULL;
	//注册属性
	virtual bool __cdecl RegisterAttribute(LPCTSTR pszName, BYTE * pByte, bool bReadOnly)=NULL;
	//注册属性
	virtual bool __cdecl RegisterAttribute(LPCTSTR pszName, WORD * pWord, bool bReadOnly)=NULL;
	//注册属性
	virtual bool __cdecl RegisterAttribute(LPCTSTR pszName, LONG * pLong, bool bReadOnly)=NULL;
	//注册属性
	virtual bool __cdecl RegisterAttribute(LPCTSTR pszName, DWORD * pDword, bool bReadOnly)=NULL;
	//注册属性
	virtual bool __cdecl RegisterAttribute(LPCTSTR pszName, DOUBLE * pDouble, bool bReadOnly)=NULL;
	//注册属性
	virtual bool __cdecl RegisterAttribute(LPCTSTR pszName, CString * pString, bool bReadOnly)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_IAttributeService INTERFACE_VERSION(1,1)
static const GUID IID_IAttributeService={0x51860649,0xcff4,0x44e7,0xaa,0x63,0x6,0xb5,0x2d,0x95,0x5e,0x9f};

//属性服务接口
interface IAttributeService : public IUnknownEx
{
	//查询属性
	virtual IAttribute * __cdecl QueryAttribute(WORD wIndex)=NULL;
	//查询属性
	virtual IAttribute * __cdecl QueryAttribute(LPCTSTR pszName)=NULL;
	//获取属性
	virtual bool __cdecl GetAttributeValue(LPCTSTR pszName, VOID * * ppvValue)=NULL;
	//获取属性
	virtual bool __cdecl GetAttributeValue(LPCTSTR pszName, INT & iValue)=NULL;
	//获取属性
	virtual bool __cdecl GetAttributeValue(LPCTSTR pszName, BYTE & cbValue)=NULL;
	//获取属性
	virtual bool __cdecl GetAttributeValue(LPCTSTR pszName, WORD & wValue)=NULL;
	//获取属性
	virtual bool __cdecl GetAttributeValue(LPCTSTR pszName, LONG & lValue)=NULL;
	//获取属性
	virtual bool __cdecl GetAttributeValue(LPCTSTR pszName, DWORD & dwValue)=NULL;
	//获取属性
	virtual bool __cdecl GetAttributeValue(LPCTSTR pszName, DOUBLE & dValue)=NULL;
	//获取属性
	virtual bool __cdecl GetAttributeValue(LPCTSTR pszName, CString & strValue)=NULL;
	//获取属性
	virtual bool __cdecl GetAttributeVarValue(LPCTSTR pszName, CString & strValue)=NULL;
	//设置属性
	virtual bool __cdecl SetAttributeValue(LPCTSTR pszName, void * pValue)=NULL;
	//设置属性
	virtual bool __cdecl SetAttributeValue(LPCTSTR pszName, INT iValue)=NULL;
	//设置属性
	virtual bool __cdecl SetAttributeValue(LPCTSTR pszName, BYTE cbValue)=NULL;
	//设置属性
	virtual bool __cdecl SetAttributeValue(LPCTSTR pszName, WORD wValue)=NULL;
	//设置属性
	virtual bool __cdecl SetAttributeValue(LPCTSTR pszName, LONG lValue)=NULL;
	//设置属性
	virtual bool __cdecl SetAttributeValue(LPCTSTR pszName, DWORD dwValue)=NULL;
	//设置属性
	virtual bool __cdecl SetAttributeValue(LPCTSTR pszName, DOUBLE dValue)=NULL;
	//设置属性
	virtual bool __cdecl SetAttributeValue(LPCTSTR pszName, LPCTSTR pszValue)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_ISkinRecord INTERFACE_VERSION(1,1)
static const GUID IID_ISkinRecord={0x3b41889b,0xc8c2,0x4b07,0xa1,0x7e,0xad,0x98,0x41,0x57,0x1d,0x60};

//界面配置接口
interface ISkinRecord : public IUnknownEx
{
	//获取目录
	virtual LPCTSTR __cdecl GetResourceDirectory()=NULL;
	//读取参数
	virtual int __cdecl ReadInt(LPCTSTR pszClassName, LPCTSTR pszItem, int iDefValue)=NULL;
	//读取参数
	virtual COLORREF __cdecl ReadColor(LPCTSTR pszClassName, LPCTSTR pszItem, COLORREF crDefValue)=NULL;
	//读取参数
	virtual RECT __cdecl ReadRect(LPCTSTR pszClassName, LPCTSTR pszItem, const RECT & rcDefValue)=NULL;
	//读取参数
	virtual POINT __cdecl ReadPoint(LPCTSTR pszClassName, LPCTSTR pszItem, const POINT & ptDefValue)=NULL;
	//读取参数
	virtual CString __cdecl ReadString(LPCTSTR pszClassName, LPCTSTR pszItem, LPCTSTR pszDefValue)=NULL;
	//写入参数
	virtual bool __cdecl WriteInt(LPCTSTR pszClassName, LPCTSTR pszItem, int iValue)=NULL;
	//写入参数
	virtual bool __cdecl WriteColor(LPCTSTR pszClassName, LPCTSTR pszItem, COLORREF crValue)=NULL;
	//写入参数
	virtual bool __cdecl WriteRect(LPCTSTR pszClassName, LPCTSTR pszItem, const RECT & rcValue)=NULL;
	//写入参数
	virtual bool __cdecl WritePoint(LPCTSTR pszClassName, LPCTSTR pszItem, const POINT & ptValue)=NULL;
	//写入参数
	virtual bool __cdecl WriteString(LPCTSTR pszClassName, LPCTSTR pszItem, LPCTSTR pszValue)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_ISkinAttribute INTERFACE_VERSION(1,1)
static const GUID IID_ISkinAttribute={0xbd02b27d,0x7274,0x46a5,0x82,0x7b,0xc4,0x49,0x83,0xfe,0x8b,0x66};

//界面属性接口
interface ISkinAttribute : public IUnknownEx
{
	//获取名字
	virtual LPCTSTR __cdecl GetClassName()=NULL;
	//获取字体
	virtual HFONT __cdecl GetDefaultFont()=NULL;
	//保存配置
	//virtual bool __cdecl SaveSkinOption()=NULL;
	//加载配置
	virtual bool __cdecl UpdateSkinResource(/*ISkinResource * pISkinResource*/)=NULL;
};

//////////////////////////////////////////////////////////////////////////

//编辑资源
struct tagSkinEditResource
{
	COLORREF						crFocusTX;							//字体颜色
	COLORREF						crFocusBK;							//背景颜色
	COLORREF						crNoFocusTX;						//字体颜色
	COLORREF						crNoFocusBK;						//背景颜色
	COLORREF						crDisFocusTX;						//字体颜色
	COLORREF						crDisFocusBK;						//背景颜色
	//COLORREF						crBoradNormal;						//边框颜色
};

//按钮资源
struct tagSkinButtonResource
{
	COLORREF						crButtonText;						//字体颜色
	tagImageLoadInfo				LoadInfoButton;						//加载信息
};

//窗口资源
struct tagSkinDialogResource
{
	//配色信息
	COLORREF						crTitleText;						//标题颜色
	COLORREF						crBackGround;						//背景颜色
	COLORREF						crControlText;						//控制颜色
	COLORREF						crInsideBorder;						//边框颜色
	COLORREF						crBackFrame;						//背框颜色

	//加载信息
	tagImageLoadInfo				LoadInfoImageTL;					//加载信息
	tagImageLoadInfo				LoadInfoImageTM;					//加载信息
	tagImageLoadInfo				LoadInfoImageTR;					//加载信息

	//加载信息
	tagImageLoadInfo				LoadInfoButtonMin;					//加载信息
	tagImageLoadInfo				LoadInfoButtonMax;					//加载信息
	tagImageLoadInfo				LoadInfoButtonRes;					//加载信息
	tagImageLoadInfo				LoadInfoButtonClose;				//加载信息
};

//列表资源
struct tagSkinListCtrlResource
{
	//颜色信息
	COLORREF						crListTX;							//列表文字

	//资源信息
	tagImageLoadInfo				LoadInfoImageBack;					//加载信息
};

//拆分资源
struct tagSkinSplitterResource
{
	tagImageLoadInfo				LoadInfoVor;						//加载信息
	tagImageLoadInfo				LoadInfoHor;						//加载信息
};

//热点资源
struct tagSkinHyperLinkResource
{
	COLORREF						crBackColor;						//背景颜色
	COLORREF						crHoverText;						//字体颜色
	COLORREF						crNormalText;						//字体颜色
	COLORREF						crVisitedText;						//字体颜色
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_ISkinResource INTERFACE_VERSION(1,1)
	static const GUID IID_ISkinResource={0x843d0310,0x44d5,0x4ee1,0x00a1,0xc3,0x90,0x4b,0x40,0x28,0x67,0x3a};
#else
	#define VER_ISkinResource INTERFACE_VERSION(1,1)
	static const GUID IID_ISkinResource={0x9f36f76d,0xa27c,0x4402,0x00a8,0xdd,0x01,0x49,0x9c,0xe5,0xe5,0xd1};
#endif

//界面资源
interface ISkinResource : public IUnknownEx
{
	//资源接口
public:
	//创建字体
	virtual bool __cdecl CreateDefaultFont(CFont & Font)=NULL;

	//资源接口
public:
	//获取信息
	virtual bool __cdecl GetResourceInfo(tagSkinEditResource & SkinEditResource)=NULL;
	//获取信息
	virtual bool __cdecl GetResourceInfo(tagSkinDialogResource & SkinDialogResource)=NULL;
	//获取信息
	virtual bool __cdecl GetResourceInfo(tagSkinButtonResource & SkinButtonResource)=NULL;
	//获取信息
	virtual bool __cdecl GetResourceInfo(tagSkinSplitterResource & SkinSplitterResource)=NULL;
	//获取信息
	virtual bool __cdecl GetResourceInfo(tagSkinListCtrlResource & SkinListCtrlResource)=NULL;
	//获取信息
	virtual bool __cdecl GetResourceInfo(tagSkinHyperLinkResource & SkinHyperLinkResource)=NULL;
};

//////////////////////////////////////////////////////////////////////////

//导出类头文件
#ifndef SKIN_CONTROL_DLL
	#include "SkinEdit.h"
	#include "PngImage.h"
	#include "SkinImage.h"
	#include "SkinRichEdit.h"
	#include "SkinDialog.h"
	#include "SkinFrameWnd.h"
	#include "SkinButton.h"
	#include "SkinListCtrl.h"
	#include "SkinSplitter.h"
	#include "SkinHyperLink.h"
	#include "SkinAide.h"
	#include "SkinResourceManager.h"
#endif

//////////////////////////////////////////////////////////////////////////

//透明绘画
extern "C" SKIN_CONTROL_CLASS void AlphaDrawImage(CDC * pDesDC, int nXDes, int nYDes, int nDesWidth, int nDesHeight, 
	CDC * pScrDC, int nXScr, int nYScr, int nScrWidth, int nScrHeight, COLORREF crTransColor);

//////////////////////////////////////////////////////////////////////////

//辅助函数
//#ifndef SKIN_CONTROL_DLL
//
//	//保存界面
//	//inline bool SaveSkinResource() 
//	//{ 
//		//return CSkinAttribute::SaveSkinAttribute(); 
//	//}
//	
//	//加载界面
//	inline bool LoadSkinResource(IUnknownEx * pIUnknownEx) 
//	{ 
//		return CSkinAttribute::LoadSkinAttribute(pIUnknownEx); 
//	}
//
//#endif

//////////////////////////////////////////////////////////////////////////

#endif