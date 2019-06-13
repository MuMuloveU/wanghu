#ifndef GAMERANK_HEAD_FILE
#define GAMERANK_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//平台头文件
#include "..\..\模板库\Template.h"

//全局头文件
#include "..\..\公共文件\GlobalDef.h"

//////////////////////////////////////////////////////////////////////////
//公共宏定义

//导出定义
#ifndef GAMERANK_CLASS
	#ifdef  GAMERANK_DLL
		#define GAMERANK_CLASS _declspec(dllexport)
	#else
		#define GAMERANK_CLASS _declspec(dllimport)
	#endif
#endif

//模块定义
#ifdef _DEBUG
	#define GAMERANK_DLL_NAME	TEXT("GameRankD.dll")					//组件 DLL 名字
#else
	#define GAMERANK_DLL_NAME	TEXT("GameRank.dll")					//组件 DLL 名字
#endif

//////////////////////////////////////////////////////////////////////////

#define VER_IGameRankManager INTERFACE_VERSION(1,1)
static const GUID IID_IGameRankManager={0x850e5617,0x4eab,0x4f2e,0xae,0x11,0xef,0xa3,0x22,0x65,0xe0,0xc2};

//游戏等级接口
interface IGameRankManager : public IUnknownEx
{
	//信息接口
public:
	//加载判断
	virtual bool __cdecl IsLoadLevel()=NULL;
	//等级数目
	virtual WORD __cdecl GetGameLevelCount()=NULL;

	//配置接口
public:
	//加载等级
	virtual bool __cdecl LoadGameLevel(LPCTSTR pszDirectory, WORD wGameGenre=GAME_GENRE_SCORE)=NULL;
	//释放等级
	virtual bool __cdecl UnLoadGameLevel()=NULL;

	//描述接口
public:
	//等级描述
	virtual LPCTSTR __cdecl GetLevelDescribe(LONGLONG lScore)=NULL;

	//数据接口
public:
	//获取等级
	virtual WORD __cdecl GetGameLevelItem(tagLevelItem LevelItem[], WORD wMaxCount)=NULL;
	//设置等级
	virtual bool __cdecl SetGameLevelItem(tagLevelItem LevelItem[], WORD wItemCount)=NULL;
};

//////////////////////////////////////////////////////////////////////////

//关系管理辅助类
class CGameRankManagerHelper : public CTempldateHelper<IGameRankManager>
{
	//函数定义
public:
	//构造函数
	CGameRankManagerHelper(void) : CTempldateHelper<IGameRankManager>(IID_IGameRankManager,
		VER_IGameRankManager,GAMERANK_DLL_NAME,TEXT("CreateGameRankManager")) { }
};

//////////////////////////////////////////////////////////////////////////


#endif