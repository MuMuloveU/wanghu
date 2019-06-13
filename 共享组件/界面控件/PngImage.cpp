#include "StdAfx.h"
#include "PngImage.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CPngImage::CPngImage()
{
	//设置变量
	m_pImage=NULL;

	return;
}

//析构函数
CPngImage::~CPngImage()
{
	//销毁图片
	DestroyImage(); 

	return;
}

//是否加载
bool CPngImage::IsNull()
{
	//加载判断
	if (m_pImage==NULL) return true;
	if (m_pImage->GetLastStatus()!=Ok) return true;

	return false;
}

//获取宽度
INT CPngImage::GetWidth()
{
	//加载判断
	ASSERT(IsNull()==false);
	if (IsNull()==true) return 0;

	//获取宽度
	return m_pImage->GetWidth();
}

//获取高度
INT CPngImage::GetHeight()
{
	//加载判断
	ASSERT(IsNull()==false);
	if (IsNull()==true) return 0;

	//获取高度
	return m_pImage->GetHeight();
}

//销毁图片
bool CPngImage::DestroyImage()
{
	//删除对象
	if (m_pImage!=NULL) SafeDelete(m_pImage);

	return true;
}

//加载图片
bool CPngImage::LoadImage(LPCTSTR pszFileName)
{
	//加载判断
	ASSERT(m_pImage==NULL);
	if (m_pImage!=NULL) return false;

	//加载文件
	CT2CW strFileName(pszFileName);
	m_pImage=Image::FromFile((LPCWSTR)strFileName);

	//错误判断
	if ((m_pImage==NULL)||(m_pImage->GetLastStatus()!=Ok)) 
	{
		DestroyImage();
		return false;
	}

	return true;
}

//加载图片
bool CPngImage::LoadImage(HINSTANCE hInstance, LPCTSTR pszResourceName)
{
	//加载判断
	ASSERT(m_pImage==NULL);
	if (m_pImage!=NULL) return false;

	//变量定义
	HRSRC hResource=NULL;
	HGLOBAL hGlobal=NULL;
	IStream * pIStream=NULL;

	//加载位图
	try
	{
		//查找资源
		hResource=FindResource(hInstance,pszResourceName,TEXT("PNG"));
		if (hResource==NULL) throw 0;

		//加载资源
		hGlobal=LoadResource(hInstance,hResource);
		if (hGlobal==NULL) throw 0;

		//读取资源
		LPVOID pImageBuffer=LockResource(hGlobal);
		DWORD dwImageSize=SizeofResource(hInstance,hResource);

		//创建数据
		CreateStreamOnHGlobal(NULL,TRUE,&pIStream);
		if (pIStream==NULL) throw 0;

		//写入数据
		pIStream->Write(pImageBuffer,dwImageSize,NULL);

		//创建位图
		m_pImage=Image::FromStream(pIStream);
		if ((m_pImage==NULL)||(m_pImage->GetLastStatus()!=Ok)) throw 0;

		//释放资源
		if (pIStream!=NULL) pIStream->Release();

		//是否资源
		if (hGlobal!=NULL)
		{
			UnlockResource(hGlobal);
			FreeResource(hGlobal);
		}

		return true;
	}
	catch (...)
	{
		//释放资源
		if (pIStream!=NULL) pIStream->Release();

		//是否资源
		if (hGlobal!=NULL)
		{
			UnlockResource(hGlobal);
			FreeResource(hGlobal);
		}

		return false;
	}

	return true;
}

//绘画图像
bool CPngImage::DrawImage(CDC * pDC, INT nXPos, INT nYPos)
{
	//加载判断
	ASSERT(m_pImage!=NULL);
	if (m_pImage==NULL) return false;

	//创建屏幕
	ASSERT(pDC!=NULL);
	Graphics graphics(pDC->GetSafeHdc());

	//获取属性
	INT nImageWidth=m_pImage->GetWidth();
	INT nImageHeight=m_pImage->GetHeight();

	//构造位置
	RectF rcDrawRect;
	rcDrawRect.X=(REAL)nXPos;
	rcDrawRect.Y=(REAL)nYPos;
	rcDrawRect.Width=(REAL)nImageWidth;
	rcDrawRect.Height=(REAL)nImageHeight;

	//绘画图像
	graphics.DrawImage(m_pImage,rcDrawRect,0,0,(REAL)nImageWidth,(REAL)nImageHeight,UnitPixel);

	return true;
}

//绘画图像
bool CPngImage::DrawImage(CDC * pDC, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXScr, INT YSrc)
{
	//加载判断
	ASSERT(m_pImage!=NULL);
	if (m_pImage==NULL) return false;

	//创建屏幕
	ASSERT(pDC!=NULL);
	Graphics graphics(pDC->GetSafeHdc());

	//构造位置
	RectF rcDrawRect;
	rcDrawRect.X=(REAL)nXDest;
	rcDrawRect.Y=(REAL)nYDest;
	rcDrawRect.Width=(REAL)nDestWidth;
	rcDrawRect.Height=(REAL)nDestHeight;

	//绘画图像
	graphics.DrawImage(m_pImage,rcDrawRect,(REAL)nXScr,(REAL)YSrc,(REAL)nDestWidth,(REAL)nDestHeight,UnitPixel);

	return true;
}

//绘画图像
bool CPngImage::DrawImage(CDC * pDC, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXScr, INT YSrc, INT nSrcWidth, INT nSrcHeight)
{
	//加载判断
	ASSERT(m_pImage!=NULL);
	if (m_pImage==NULL) return false;

	//创建屏幕
	ASSERT(pDC!=NULL);
	Graphics graphics(pDC->GetSafeHdc());

	//构造位置
	RectF rcDrawRect;
	rcDrawRect.X=(REAL)nXDest;
	rcDrawRect.Y=(REAL)nYDest;
	rcDrawRect.Width=(REAL)nDestWidth;
	rcDrawRect.Height=(REAL)nDestHeight;

	//绘画图像
	graphics.DrawImage(m_pImage,rcDrawRect,(REAL)nXScr,(REAL)YSrc,(REAL)nSrcWidth,(REAL)nSrcHeight,UnitPixel);

	return true;
}

//////////////////////////////////////////////////////////////////////////
