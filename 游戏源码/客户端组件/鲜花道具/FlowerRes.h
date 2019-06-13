#ifndef FLOWER_RES_HEAD_FILE
#define FLOWER_RES_HEAD_FILE

#pragma once
#include "PropertyModule.h"
///////////////////////////////////////////////////////////////////////////////////

class PROPERTY_MODULE_CLASS CFlowerRes : public IFlowerRes
{
	//变量定义
protected:
	CSkinImage						m_ImageFlower[FLOWER_COUNT];							//鲜花图片

	//函数定义
public:
	//构造函数
	CFlowerRes(void);
	//析构函数
	~CFlowerRes(void);

	//基础接口
public:
	//释放对象
	virtual void __cdecl Release() { if (IsValid()) delete this; return ; }
	//是否有效
	virtual bool __cdecl IsValid() { return AfxIsValidAddress(this,sizeof(CFlowerRes))?true:false; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//接口函数
public:
	//绘画鲜花
	virtual void __cdecl DrawFlower(CDC * pDC, int nXPos, int nYPos, UINT uFlowerID);
	//绘画鲜花
	virtual void __cdecl DrawGift(CDC * pDC, int nXPos, int nYPos, UINT uFlowerID);
};
///////////////////////////////////////////////////////////////////////////////////

#endif
