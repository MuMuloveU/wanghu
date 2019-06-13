#ifndef GAME_LEVEL_PARSER_HEAD_FILE
#define GAME_LEVEL_PARSER_HEAD_FILE

#pragma once

#include "GameRank.h"

//////////////////////////////////////////////////////////////////////////

//等级子项
struct tagLevelDescribe
{
	LONG							lLevelScore;						//等级积分
	TCHAR							szLevelName[16];					//等级描述
};

//等级数组
typedef CArrayTemplate<tagLevelDescribe *> CLevelDescribeArray;			//等级数组	

//////////////////////////////////////////////////////////////////////////

//等级解释
class CGameRankManager : public IGameRankManager
{
	//状态变量
protected:
	bool							m_bLoadLevel;						//加载标志

	//子项变量
protected:
	CLevelDescribeArray				m_LevelDescribeActive;				//活动数组
	static CLevelDescribeArray		m_LevelDescribeStorage;				//存储数组

	//函数定义
public:
	//构造函数
	CGameRankManager();
	//析构函数
	virtual ~CGameRankManager();

	//基础接口
public:
	//释放对象
	virtual VOID __cdecl Release() { delete this; }
	//接口查询
	virtual VOID * __cdecl QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//信息接口
public:
	//加载判断
	virtual bool __cdecl IsLoadLevel() { return m_bLoadLevel; }
	//等级数目
	virtual WORD __cdecl GetGameLevelCount() { return (WORD)m_LevelDescribeActive.GetCount(); }

	//配置接口
public:
	//加载等级
	virtual bool __cdecl LoadGameLevel(LPCTSTR pszDirectory, WORD wGameGenre=GAME_GENRE_SCORE);
	//释放等级
	virtual bool __cdecl UnLoadGameLevel();

	//描述接口
public:
	//等级描述
	virtual LPCTSTR __cdecl GetLevelDescribe(LONGLONG lScore);

	//数据接口
public:
	//获取等级
	virtual WORD __cdecl GetGameLevelItem(tagLevelItem LevelItem[], WORD wMaxCount);
	//设置等级
	virtual bool __cdecl SetGameLevelItem(tagLevelItem LevelItem[], WORD wItemCount);

	//内部函数
private:
	//默认等级
	LPCTSTR GetDefaultLevel(LONGLONG lScore);
	//获取子项
	tagLevelDescribe * CreateLevelDescribe();
};

//////////////////////////////////////////////////////////////////////////

#endif