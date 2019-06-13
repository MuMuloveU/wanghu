#ifndef USER_FACE_RES_HEAD_FILE
#define USER_FACE_RES_HEAD_FILE

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "AtlImage.h"
#include "UserFace.h"

//用户头像资源类
class CUserFaceRes : public IUserFaceRes
{
	//变量定义
protected:
	TCHAR								m_szDirWork[MAX_PATH];			//工作目录
	WORD								m_wCount;						//头像数目
	CSize								m_NormalSize;					//头像大小
	CImage								m_NormalFace;					//普通头像

	//函数定义
public:
	//构造函数
	CUserFaceRes(void);
	//析构函数
	virtual ~CUserFaceRes(void);

	//基础接口
public:
	//释放对象
	virtual void __cdecl Release() { delete this; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//接口函数
public:
	//头像数目
	virtual WORD __cdecl GetFaceCount() { return m_wCount; }
	//获取大小
	virtual const CSize & __cdecl GetNormalFaceSize() { return m_NormalSize; }

	//功能函数
public:
	//填充列表
	virtual WORD __cdecl FillImageList(CImageList & ImageList);
	//绘画头像
	virtual void __cdecl DrawNormalFace(CDC * pDC, int nXPos, int nYPos, WORD wFaceID, DWORD dwUserID,DWORD dwCustomFace);
	//绘画头像
	virtual void __cdecl DrawCustomBigFace(CDC * pDC, int nXPos, int nYPos, DWORD dwUserID,DWORD dwCustomFace,UINT nWidth=BIG_FACE_WIDTH,UINT nHeight=BIG_FACE_HEIGHT);

	//内部函数
protected:
	//透明绘画
	bool AlphaDrawImage(CDC * pDestDC, int xDest, int yDest, int cxDest, int cyDest, CDC * pScrDC, int xSrc, int ySrc);
};

//////////////////////////////////////////////////////////////////////////

#endif