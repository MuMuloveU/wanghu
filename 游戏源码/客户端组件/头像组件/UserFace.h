#ifndef USER_FACE_HEAD_FILE
#define USER_FACE_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//平台头文件
#include "..\..\模板库\Template.h"

//////////////////////////////////////////////////////////////////////////
//公共宏定义

//宏定义
#define LIT_FACE_WIDTH						16							//头像宽度
#define LIT_FACE_HEIGHT						16							//头像高度
#define NOR_FACE_WIDTH						32							//头像宽度
#define NOR_FACE_HEIGHT						32							//头像高度
#define BIG_FACE_WIDTH						100							//头像宽度
#define BIG_FACE_HEIGHT						100							//头像高度

//模块定义
#ifdef _DEBUG
	#define USER_FACE_DLL_NAME	TEXT("UserFaceD.dll")					//组件 DLL 名字
#else
	#define USER_FACE_DLL_NAME	TEXT("UserFace.dll")					//组件 DLL 名字
#endif

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IUserFaceRes INTERFACE_VERSION(1,1)
	static const GUID IID_IUserFaceRes={0x5ac6a7f0,0x1e38,0x49ef,0x91,0x2c,0x4f,0x7a,0x5b,0x03,0xdf,0xca};
#else
	#define VER_IUserFaceRes INTERFACE_VERSION(1,1)
	static const GUID IID_IUserFaceRes={0x567fb4df,0xd5b6,0x488b,0x90,0x40,0x90,0x17,0x15,0x10,0xf7,0x41};
#endif

//头像组件接口
interface IUserFaceRes : public IUnknownEx
{
	//获取数目
	virtual WORD __cdecl GetFaceCount()=NULL;
	//获取大小
	virtual const CSize & __cdecl GetNormalFaceSize()=NULL;
	//填充列表
	virtual WORD __cdecl FillImageList(CImageList & m_ImageList)=NULL;
	//绘画头像
	virtual void __cdecl DrawNormalFace(CDC * pDC, int nXPos, int nYPos, WORD wFaceID, DWORD dwUserID,DWORD dwCustomFace)=NULL;
	//绘画头像
	virtual void __cdecl DrawCustomBigFace(CDC * pDC, int nXPos, int nYPos, DWORD dwUserID,DWORD dwCustomFace,UINT nWidth=BIG_FACE_WIDTH,UINT nHeight=BIG_FACE_HEIGHT)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IFaceControl INTERFACE_VERSION(1,1)
	static const GUID IID_IFaceControl={0xa728d07d,0xe453,0x46bd,0xb4,0x27,0x25,0x7e,0x2a,0xa0,0xc4,0xc4};
#else
	#define VER_IFaceControl INTERFACE_VERSION(1,1)
	static const GUID IID_IFaceControl={0x6b0e8af2,0x0e5b,0x4024,0xb8,0x2c,0x1e,0x8e,0x17,0xac,0x6b,0x2e};
#endif

//头像控件接口
interface IFaceControl : public IUnknownEx
{
	//信息接口
public:
	//头像宽度
	virtual int __cdecl GetFaceWidth()=NULL;
	//头像高度
	virtual int __cdecl GetFaceHeight()=NULL;
	//头像数目
	virtual WORD __cdecl GetFaceItemCount()=NULL;

	//绘画接口
public:
	//绘画头像
	virtual void __cdecl DrawFaceItem(CDC * pDC, int nXPos, int nYPos, WORD wFaceID)=NULL;
};

//////////////////////////////////////////////////////////////////////////

//头像组件辅助类
class CUserFaceResHelper : public CTempldateHelper<IUserFaceRes>
{
	//函数定义
public:
	//构造函数
	CUserFaceResHelper(void) : CTempldateHelper<IUserFaceRes>(IID_IUserFaceRes,
		VER_IUserFaceRes,USER_FACE_DLL_NAME,TEXT("CreateUserFaceRes"))
	{
	}
};

//////////////////////////////////////////////////////////////////////////

//头像控件辅助类
class CFaceControlHelper : public CTempldateHelper<IFaceControl>
{
	//函数定义
public:
	//构造函数
	CFaceControlHelper(void) : CTempldateHelper<IFaceControl>(IID_IFaceControl,
		VER_IFaceControl,USER_FACE_DLL_NAME,TEXT("CreateFaceControl"))
	{
	}
};

//////////////////////////////////////////////////////////////////////////

#endif