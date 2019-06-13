#ifndef COMPANION_MANAGER_HEAD_FILE
#define COMPANION_MANAGER_HEAD_FILE

#pragma once

#include "Companion.h"
#include "DlgCompanionManager.h"

//////////////////////////////////////////////////////////////////////////

//类说明
typedef CArrayTemplate<tagCompanionItem *> CCompanionItemArray;			//用户关系

//////////////////////////////////////////////////////////////////////////

//关系管理
class CCompanionManager : public ICompanionManager
{
	//变量定义
protected:
	bool							m_bNeedUpdate;						//更新标志

	//组件变量
protected:
	CCompanionItemArray				m_CompanionItemActive;				//激活用户
	CCompanionItemArray				m_CompanionItemRelease;				//释放用户
	CDlgCompanionManager			m_DlgCompanionManager;				//管理窗口

	//函数定义
public:
	//构造函数
	CCompanionManager(void);
	//析构函数
	virtual ~CCompanionManager(void);

	//基础接口
public:
	//释放对象
	virtual void __cdecl Release() { delete this; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//管理接口
public:
	//加载信息
	virtual bool __cdecl LoadCompanion();
	//保存关系
	virtual bool __cdecl SaveCompanion();
	//显示窗口
	virtual bool __cdecl ShowCompanionManager();
	//关系数目
	virtual INT_PTR __cdecl GetCompanionItemCount() { return m_CompanionItemActive.GetCount(); }

	//功能接口
public:
	//加入关系
	virtual const tagCompanionItem * __cdecl InsertCompanionItem(tagCompanionItem & CompanionItem);
	//更新关系
	virtual const tagCompanionItem * __cdecl UpdateCompanionItem(tagCompanionItem & CompanionItem);
	//更新关系
	virtual const tagCompanionItem * __cdecl UpdateCompanionItem(DWORD dwUserID, LPCTSTR pszNote);
	//更新关系
	virtual const tagCompanionItem * __cdecl UpdateCompanionItem(DWORD dwUserID, LPCTSTR pszAccounts, WORD wFaceID);
	//删除关系
	virtual bool __cdecl DeleteCompanionItem(DWORD dwUserID);

	//查找接口
public:
	//枚举关系
	virtual const tagCompanionItem * __cdecl EmunCompanionItem(WORD wIndex);
	//寻找关系
	virtual const tagCompanionItem * __cdecl SearchCompanionItem(DWORD dwUserID);
};

//////////////////////////////////////////////////////////////////////////

#endif