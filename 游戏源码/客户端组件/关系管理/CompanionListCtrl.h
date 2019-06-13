#ifndef COMPANION_LIST_CTRL_HEAD_FILE
#define COMPANION_LIST_CTRL_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "Companion.h"

//////////////////////////////////////////////////////////////////////////

//关系列表视图
class CCompanionListCtrl : public CSkinListCtrl
{
	//控件变量
protected:
	static CImageList					m_ImageList;					//图片资源

	//函数定义
public:
	//构造函数
	CCompanionListCtrl();
	//析构函数
	virtual ~CCompanionListCtrl();

	//重载函数
protected:
	//绑定消息
	virtual void PreSubclassWindow();

	//功能函数
public:
	//加入关系
	bool InsertCompanionItem(const tagCompanionItem * pCompanionItem);
	//更新关系
	bool UpdateCompanionItem(const tagCompanionItem * pCompanionItem);
	//删除关系
	bool DeleteCompanionItem(const tagCompanionItem * pCompanionItem);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif
