#ifndef DOWN_LOAD_HEAD_FILE
#define DOWN_LOAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//平台头文件
#include "Template.h"

//组件头文件
#include "ComService.h"
#include "SkinControls.h"

//////////////////////////////////////////////////////////////////////////
//公共宏定义

//导出定义
#ifndef IDOWN_LOAD_CLASS
	#ifdef  IDOWN_LOAD_DLL
		#define IDOWN_LOAD_CLASS _declspec(dllexport)
	#else
		#define IDOWN_LOAD_CLASS _declspec(dllimport)
	#endif
#endif

//模块定义
#ifdef _DEBUG
	#define IDOWN_LOAD_DLL_NAME	TEXT("DownLoadD.dll")					//组件 DLL 名字
#else
	#define IDOWN_LOAD_DLL_NAME	TEXT("DownLoad.dll")					//组件 DLL 名字
#endif

//////////////////////////////////////////////////////////////////////////
//结构体定义

//下载请求
struct tagDownLoadRequest
{
	bool							bDisplay;							//是否显示
	TCHAR							szDescribe[128];					//描述信息
	TCHAR							szFileName[MAX_PATH];				//本地文件
	TCHAR							szLocalPath[MAX_PATH];				//本地路径
	TCHAR							szDownLoadUrl[MAX_PATH];			//下载路径
};

//////////////////////////////////////////////////////////////////////////

#define VER_IDownLoadService INTERFACE_VERSION(3,1)
static const GUID IID_IDownLoadService={0xe0b37184,0x6fbe,0x44ac,0xa4,0x83,0xab,0xa6,0x89,0x9b,0x33,0xd4};

//下载服务接口
interface IDownLoadService : public IUnknownEx
{
	//获取数目
	virtual INT_PTR __cdecl GetDownLoadMissionCount()=NULL;
	//设置接口
	virtual bool __cdecl SetDownLoadServiceSink(IUnknownEx * pIUnknownEx)=NULL;
	//下载请求
	virtual DWORD __cdecl AddDownLoadRequest(DWORD dwDownLoadType, tagDownLoadRequest * pDownLoadRequest)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_IDownLoadServiceSink INTERFACE_VERSION(1,1)
static const GUID IID_IDownLoadServiceSink={0xb339b588,0xd328,0x4482,0x96,0xde,0xc4,0x12,0xdd,0x78,0x81,0xec};

//下载服务回调接口
interface IDownLoadServiceSink : public IUnknownEx
{
};

//////////////////////////////////////////////////////////////////////////

//下载服务辅助类
class CDownLoadServiceHelper : public CTempldateHelper<IDownLoadService>
{
	//函数定义
public:
	//构造函数
	CDownLoadServiceHelper(void) : CTempldateHelper<IDownLoadService>(IID_IDownLoadService,
		VER_IDownLoadService,IDOWN_LOAD_DLL_NAME,TEXT("CreateDownLoadService"))
	{
	}
};

//////////////////////////////////////////////////////////////////////////

#endif