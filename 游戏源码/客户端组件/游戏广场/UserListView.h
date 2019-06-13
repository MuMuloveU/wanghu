#pragma once

#include "Stdafx.h"
#include "UserManager.h"

//宏定义
#define	MAX_COLUMN						32									//最大列数
#define	IMGAE_SPACE						27									//单元图宽度

//////////////////////////////////////////////////////////////////////////

//用户列表视图
class CUserListView : public CSkinListCtrl
{
public:
	WORD								m_wGameGenre;						//游戏类型
	WORD								m_wKindID;							//类型标识

	//资源变量
protected:
	UINT								m_uImageSpace;						//位图间隔
	BYTE								m_cbAscendSort;						//升序标志
	static CImageList					m_StatusImage;						//状态位图

	//列表信息
protected:
	WORD								m_wColumnCount;						//列表数目
	WORD								m_wDataDescribe[MAX_COLUMN];		//数据描述

	//组件定义
public:
	CGameRankManagerHelper				m_GameRankManager;					//游戏等级

	//函数定义
public:
	//构造函数
	CUserListView();
	//析构函数
	virtual ~CUserListView();

	//配置函数
public:
	//初始化列表
	bool InitUserListView();
	//设置列信息
	bool SetColumnDescribe(tagColumnItem ColumnItem[], WORD wColumnCount);

	//功能函数
public:
	//加入用户
	bool InsertUserItem(IUserItem * pIUserItem);
	//更新用户
	bool UpdateUserItem(IUserItem * pIUserItem);
	//删除用户
	bool DeleteUserItem(IUserItem * pIUserItem);
	//好友置顶
	bool UpdataUserList();

	//内部函数
protected:
	//获取位图
	int GetImageStation(IUserItem * pIUserItem);
	//获取位图
	int GetImageStation(tagUserData * pUserData);
	//获取插入
	int GetInsertStation(IUserItem * pIUserItem);
	//获取字符
	LPCTSTR GetDisplayText(IUserItem * pIUserItem, WORD wColumnPos);
	//排列函数
	static int CALLBACK SortFun(LPARAM lParam1, LPARAM lParam2, LPARAM lParamList);

	//重载函数
public:
	//绘画函数
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	//消息映射
protected:
	//点击列表
	afx_msg void OnColumnclick(NMHDR * pNMHDR, LRESULT * pResult);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
