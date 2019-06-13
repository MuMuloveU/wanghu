#ifndef COMPANION_TREE_CTRL_HEAD_FILE
#define COMPANION_TREE_CTRL_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "Companion.h"

//////////////////////////////////////////////////////////////////////////

//关系管理树控件
class CCompanionTreeCtrl : public CTreeCtrl
{
	//变量定义
protected:
	HTREEITEM							m_hItemDetest;					//厌恶树项
	HTREEITEM							m_hItemFriend;					//好友树项

	//控件变量
protected:
	CImageList							m_ImageList;					//图片资源

	//函数定义
public:
	//构造函数
	CCompanionTreeCtrl();
	//析构函数
	virtual ~CCompanionTreeCtrl();

	//重载函数
protected:
	//绑定消息
	virtual void PreSubclassWindow();

	//功能函数
public:
	//展开列表
	bool ExpandListItem();
	//加入关系
	bool InsertCompanionItem(const tagCompanionItem * pCompanionItem);
	//删除关系
	bool DeleteCompanionItem(const tagCompanionItem * pCompanionItem);

	//内部函数
private:
	//寻找项
	HTREEITEM SearchCompanionItem(HTREEITEM hRootTreeItem, DWORD_PTR dwParam);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif