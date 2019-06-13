#ifndef USER_LIST_VIEW_HEAD_FILE
#define USER_LIST_VIEW_HEAD_FILE

#pragma once

#include "GameFrame.h"

//宏定义
#define	MAX_COLUMN						32									//最大列数
#define	IMGAE_SPACE						27									//单元图宽度

//////////////////////////////////////////////////////////////////////////

//用户列表视图
class GAME_FRAME_CLASS CUserListView : public CSkinListCtrl
{
public:
	WORD								m_wGameGenre;						//游戏类型
	WORD								m_wKindID;							//类型标识
	CGameRankManagerHelper				m_GameRankManagerHelper;			//游戏等级

	//资源变量
protected:
	static CImageList					m_StatusImage;						//状态位图

	//列表信息
protected:
	BYTE								m_cbAscendSort;						//升序标志
	WORD								m_wColumnCount;						//列表数目
	WORD								m_wDataDescribe[MAX_COLUMN];		//数据描述

	//函数定义
public:
	//构造函数
	CUserListView();
	//析构函数
	virtual ~CUserListView();

	//功能函数
public:
	//初始化列表
	bool InitUserListView();
	//加入用户
	bool InsertUserItem(tagUserData * pUserData);
	//更新用户
	bool UpdateUserItem(tagUserData * pUserData);
	//删除用户
	bool DeleteUserItem(tagUserData * pUserData);

	//重载函数
public:
	//绘画函数
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	//内部函数
protected:
	//获取位图
	int GetImageStation(tagUserData * pUserData);
	//获取插入
	int GetInsertStation(tagUserData * pUserData);
	//填写资料
	void FillUserListItem(int iItem, tagUserData * pUserData);
	//获取字符
	LPCTSTR GetDisplayText(tagUserData * pUserData, WORD wColumnPos);
	//排列函数
	static int CALLBACK SortFun(LPARAM lParam1, LPARAM lParam2, LPARAM lParamList);

	//消息映射
protected:
	//点击列表
	afx_msg void OnColumnclick(NMHDR * pNMHDR, LRESULT * pResult);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif