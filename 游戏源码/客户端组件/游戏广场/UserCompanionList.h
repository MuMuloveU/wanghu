#ifndef USER_COMPANION_LIST_HEAD_FILE
#define USER_COMPANION_LIST_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//关系列表
class CUserCompanionList : public CTreeCtrl
{
	//变量定义
protected:
	HTREEITEM						m_hItemRoot;						//最顶树项
	HTREEITEM						m_hItemDetest;						//厌恶树项
	HTREEITEM						m_hItemFriend;						//好友树项
	HTREEITEM						m_hItemNormal;						//陌生树项

	//资源变量
protected:
	CImageList						m_ImageCompanionList;				//图片资源

	//函数定义
public:
	//构造函数
	CUserCompanionList();
	//析构函数
	virtual ~CUserCompanionList();

	//关系接口
public:
	//插入通知
	virtual bool __cdecl OnCompanionInsert(tagCompanionItem * pCompanionInfo);
	//更新通知
	virtual bool __cdecl OnCompanionUpdate(tagCompanionItem * pCompanionInfo);
	//删除通知
	virtual bool __cdecl OnCompanionDelete(tagCompanionItem * pCompanionInfo);

	//管理函数
private:
	//插入信息
	bool InsertCompanionInfo(const tagCompanionItem * pCompanionInfo);
	//删除关系
	bool DeleteCompanionInfo(tagCompanionItem * pCompanionInfo);
	//更新信息
	bool UpdateCompanionInfo(HTREEITEM hTreeItem, tagCompanionItem * pCompanionInfo);

	//内部函数
private:
	//获取关系
	BYTE GetTreeItemCompanion(HTREEITEM hTreeItem);
	//寻找树项
	HTREEITEM SearchCompanionInfo(HTREEITEM hRootTreeItem, tagCompanionItem * pCompanionInfo);

	//消息映射
protected:
	//创建消息
	INT OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif