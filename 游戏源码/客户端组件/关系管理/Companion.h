#ifndef COMPANION_HEAD_FILE
#define COMPANION_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//平台头文件
#include "..\..\模板库\Template.h"

//全局头文件
#include "..\..\公共文件\GlobalDef.h"

//组件头文件
#include "..\..\共享组件\界面控件\SkinControls.h"

//////////////////////////////////////////////////////////////////////////
//公共宏定义

//导出定义
#ifndef COMPANION_CLASS
	#ifdef  COMPANION_DLL
		#define COMPANION_CLASS _declspec(dllexport)
	#else
		#define COMPANION_CLASS _declspec(dllimport)
	#endif
#endif

//模块定义
#ifdef _DEBUG
	#define COMPANION_DLL_NAME	TEXT("CompanionD.dll")					//组件 DLL 名字
#else
	#define COMPANION_DLL_NAME	TEXT("Companion.dll")					//组件 DLL 名字
#endif

//////////////////////////////////////////////////////////////////////////
//枚举定义

//关系定义
enum enCompanion
{
	enCompanion_UnKnow,				//未知关系
	enCompanion_Friend,				//好友关系
	enCompanion_Detest,				//厌恶关系
};

//////////////////////////////////////////////////////////////////////////
//结构定义

//长度定义
#define CP_NOTE_LEN					128									//备注长度

//用户记录结构
struct tagCompanionItem
{
	DWORD							dwUserID;							//用户 I D
	DWORD							dwGameID;							//游戏 I D
	WORD							wFaceID;							//头像 I D
	enCompanion						Companion;							//用户关系
	TCHAR							szNote[CP_NOTE_LEN];				//用户备注
	TCHAR							szAccounts[NAME_LEN];				//用户账号
};

//////////////////////////////////////////////////////////////////////////

#define VER_ICompanionManager INTERFACE_VERSION(2,1)
static const GUID IID_ICompanionManager={0x7bef6deb,0x5915,0x4021,0xa1,0x4f,0xa9,0xa3,0xec,0xad,0x74,0xf3};

//关系管理接口
interface ICompanionManager : public IUnknownEx
{
	//管理接口
public:
	//加载信息
	virtual bool __cdecl LoadCompanion()=NULL;
	//保存关系
	virtual bool __cdecl SaveCompanion()=NULL;
	//显示窗口
	virtual bool __cdecl ShowCompanionManager()=NULL;
	//关系数目
	virtual INT_PTR __cdecl GetCompanionItemCount()=NULL;

	//功能接口
public:
	//加入关系
	virtual const tagCompanionItem * __cdecl InsertCompanionItem(tagCompanionItem & CompanionItem)=NULL;
	//更新关系
	virtual const tagCompanionItem * __cdecl UpdateCompanionItem(tagCompanionItem & CompanionItem)=NULL;
	//更新关系
	virtual const tagCompanionItem * __cdecl UpdateCompanionItem(DWORD dwUserID, LPCTSTR pszNote)=NULL;
	//更新关系
	virtual const tagCompanionItem * __cdecl UpdateCompanionItem(DWORD dwUserID, LPCTSTR pszAccounts, WORD wFaceID)=NULL;
	//删除关系
	virtual bool __cdecl DeleteCompanionItem(DWORD dwUserID)=NULL;

	//查找接口
public:
	//枚举关系
	virtual const tagCompanionItem * __cdecl EmunCompanionItem(WORD wIndex)=NULL;
	//寻找关系
	virtual const tagCompanionItem * __cdecl SearchCompanionItem(DWORD dwUserID)=NULL;
};

//////////////////////////////////////////////////////////////////////////

//关系管理辅助类
class CCompanionManagerHelper : public CTempldateHelper<ICompanionManager>
{
	//函数定义
public:
	//构造函数
	CCompanionManagerHelper(void) : CTempldateHelper<ICompanionManager>(IID_ICompanionManager,
		VER_ICompanionManager,COMPANION_DLL_NAME,TEXT("CreateCompanionManager")) { }
};

//////////////////////////////////////////////////////////////////////////

#endif