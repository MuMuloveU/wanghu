#ifndef FACTORY_HEAD_FILE
#define FACTORY_HEAD_FILE

#pragma once

#include "Array.h"

//////////////////////////////////////////////////////////////////////////

//宏定义
#define INFINITY_COUNT				-1									//无限数目

//类对象工厂
template <class TYPE> class CFactoryTemplate
{
	//类说明
	typedef CArrayTemplate<TYPE *> CItemPtrArray;

	//变量定义
protected:
	INT_PTR							m_nMaxCount;						//最大数目
	CItemPtrArray					m_FreeItem;							//空闲对象
	CItemPtrArray					m_ActiveItem;						//活动对象
	CItemPtrArray					m_StorageItem;						//存储对象

	//函数定义
public:
	//构造函数
	CFactoryTemplate(void);
	//析构函数
	virtual ~CFactoryTemplate(void);

	//信息函数
public:
	//最大数目
	INT_PTR GetMaxCount();
	//空闲数目
	INT_PTR GetFreeCount();
	//活动数目
	INT_PTR GetActiveCount();
	//存储数目
	INT_PTR GetStorageCount();
	//枚举对象
	TYPE * EnumActiveObject(INT_PTR nIndex);
	//枚举对象
	TYPE * EnumStorageObject(INT_PTR nIndex);

	//复制函数
public:
	//复制空闲
	void CopyFreeItem(CItemPtrArray & ItemPtrArray);
	//复制活动
	void CopyActiveItem(CItemPtrArray & ItemPtrArray);
	//复制存储
	void CopyStorageItem(CItemPtrArray & ItemPtrArray);

	//管理函数
public:
	//创建对象
	TYPE * ActiveItem();
	//删除对象
	bool FreeItem(TYPE * pObject);
	//删除所有
	void FreeAllItem();
	//释放内存
	void FreeExtra();
	//设置数目
	void SetMaxCount(INT_PTR nMaxCount=INFINITY_COUNT);
};

//////////////////////////////////////////////////////////////////////////
// CFactoryTemplate<TYPE> 内联函数

//最大数目
template<class TYPE> AFX_INLINE INT_PTR CFactoryTemplate<TYPE>::GetMaxCount()
{
	return m_nMaxCount;
}

//空闲数目
template<class TYPE> AFX_INLINE INT_PTR CFactoryTemplate<TYPE>::GetFreeCount()
{
	return m_FreeItem.GetCount();
}

//活动数目
template<class TYPE> AFX_INLINE INT_PTR CFactoryTemplate<TYPE>::GetActiveCount()
{
	return m_ActiveItem.GetCount();
}

//存储数目
template<class TYPE> AFX_INLINE INT_PTR CFactoryTemplate<TYPE>::GetStorageCount()
{
	return m_StorageItem.GetCount();
}

//设置数目
template<class TYPE> AFX_INLINE void CFactoryTemplate<TYPE>::SetMaxCount(INT_PTR nMaxCount)
{
	m_nMaxCount=nMaxCount;
}

//复制空闲
template<class TYPE> AFX_INLINE void CFactoryTemplate<TYPE>::CopyFreeItem(CItemPtrArray & ItemPtrArray)
{
	ItemPtrArray.Copy(m_FreeItem);
	return;
}

//复制活动
template<class TYPE> AFX_INLINE void CFactoryTemplate<TYPE>::CopyActiveItem(CItemPtrArray & ItemPtrArray)
{
	ItemPtrArray.Copy(m_ActiveItem);
	return;
}

//复制存储
template<class TYPE> AFX_INLINE void CFactoryTemplate<TYPE>::CopyStorageItem(CItemPtrArray & ItemPtrArray)
{
	ItemPtrArray.Copy(m_StorageItem);
	return;
}

//////////////////////////////////////////////////////////////////////////
// CFactoryTemplate<TYPE> 外联函数

//构造函数
template<class TYPE> CFactoryTemplate<TYPE>::CFactoryTemplate()
{
	m_nMaxCount=INFINITY_COUNT;
	return;
}

//构造函数
template<class TYPE> CFactoryTemplate<TYPE>::~CFactoryTemplate()
{
	//清理对象
	TYPE * pObject=NULL;
	for (INT_PTR i=0;i<m_StorageItem.GetCount();i++)
	{
		pObject=m_StorageItem[i];
		ASSERT(pObject!=NULL);
		SafeDelete(pObject);
	}
	m_FreeItem.RemoveAll();
	m_ActiveItem.RemoveAll();
	m_StorageItem.RemoveAll();

	return;
}

//创建对象
template<class TYPE> TYPE * CFactoryTemplate<TYPE>::ActiveItem()
{
	//获取空闲对象
	TYPE * pObject=NULL;
	if (m_FreeItem.GetCount()>0)
	{
		INT_PTR nItemPostion=m_FreeItem.GetCount()-1;
		pObject=m_FreeItem[nItemPostion];
		ASSERT(pObject!=NULL);
		m_FreeItem.RemoveAt(nItemPostion);
		m_ActiveItem.Add(pObject);
	}

	//创建新对象
	if (pObject==NULL)
	{
		if ((m_nMaxCount==INFINITY_COUNT)||(m_StorageItem.GetCount()<m_nMaxCount))
		{
			try
			{
				pObject=new TYPE;
				if (pObject==NULL) return NULL;
				m_StorageItem.Add(pObject);
				m_ActiveItem.Add(pObject);
			}
			catch (...) { return NULL; }
		}
	}

	return pObject;
}

//删除所有
template<class TYPE> void CFactoryTemplate<TYPE>::FreeAllItem()
{
	m_FreeItem.Append(m_ActiveItem);
	m_ActiveItem.RemoveAll();
	return;
}

//释放内存
template<class TYPE> void CFactoryTemplate<TYPE>::FreeExtra()
{
	TYPE * pObject=NULL;
	for (INT_PTR i=0;i<m_StorageItem.GetCount();i++)
	{
		pObject=m_StorageItem[i];
		ASSERT(pObject!=NULL);
		delete pObject;
	}
	m_StorageItem.RemoveAll();

	return;
}

//删除对象
template<class TYPE> bool CFactoryTemplate<TYPE>::FreeItem(TYPE * pObject)
{
	//效验参数
	ASSERT(pObject!=NULL);

	//释放对象
	INT_PTR nActiveCount=m_ActiveItem.GetCount();
	for (INT_PTR i=0;i<nActiveCount;i++)
	{
		if (pObject==m_ActiveItem[i])
		{
			m_ActiveItem.RemoveAt(i);
			m_FreeItem.Add(pObject);
			return true;
		}
	}

	//删除失败
	ASSERT(FALSE);
	return false;
}

//枚举对象
template<class TYPE> TYPE * CFactoryTemplate<TYPE>::EnumActiveObject(INT_PTR nIndex)
{
	//效验参数
	ASSERT(nIndex>=0);
	if (nIndex<0) return NULL;

	//获取对象
	if (nIndex<m_ActiveItem.GetCount())
	{
		TYPE * pObject=m_ActiveItem[nIndex];
		ASSERT(pObject!=NULL);
		return pObject;
	}

	return NULL;
}

//枚举对象
template<class TYPE> TYPE * CFactoryTemplate<TYPE>::EnumStorageObject(INT_PTR nIndex)
{
	//效验参数
	ASSERT(nIndex>=0);
	if (nIndex<0) return NULL;

	//获取对象
	if (nIndex<m_StorageItem.GetCount())
	{
		TYPE * pObject=m_StorageItem[nIndex];
		ASSERT(pObject!=NULL);
		return pObject;
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////

#endif