#include "Stdafx.h"
#include <AfxDLLx.h>
#include "SkinControls.h"

static AFX_EXTENSION_MODULE SkinControlsDLL={NULL,NULL};

//////////////////////////////////////////////////////////////////////////

//DLL 主函数
extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);
	if (dwReason==DLL_PROCESS_ATTACH)
	{
		if (!AfxInitExtensionModule(SkinControlsDLL, hInstance)) return 0;
		new CDynLinkLibrary(SkinControlsDLL);
	}
	else if (dwReason==DLL_PROCESS_DETACH)
	{
		AfxTermExtensionModule(SkinControlsDLL);
	}
	return 1;
}

//////////////////////////////////////////////////////////////////////////

//透明绘画
extern "C" SKIN_CONTROL_CLASS void AlphaDrawImage(CDC * pDesDC, int nXDes, int nYDes, int nDesWidth, int nDesHeight, 
			   CDC * pScrDC, int nXScr, int nYScr, int nScrWidth, int nScrHeight, COLORREF crTransColor)
{
	//保存变量
	COLORREF crTextColor=pDesDC->GetTextColor();

	//建立变量
	CDC ScreenDC,ImageDC,MaskDC;
	CBitmap BmpScreen,BmpImage,BmpMaskBmp;
	BmpMaskBmp.CreateBitmap(nDesWidth,nDesHeight,1,1,NULL);
	BmpImage.CreateCompatibleBitmap(pDesDC,nDesWidth,nDesHeight);
	BmpScreen.CreateCompatibleBitmap(pDesDC,nDesWidth,nDesHeight);
	MaskDC.CreateCompatibleDC(pDesDC);
	ImageDC.CreateCompatibleDC(pDesDC);
	ScreenDC.CreateCompatibleDC(pDesDC);
	CBitmap * pOldBmpImage=ImageDC.SelectObject(&BmpImage);
	CBitmap * pOldMaskBmp=MaskDC.SelectObject(&BmpMaskBmp);
	CBitmap * pOldScreenBmp=ScreenDC.SelectObject(&BmpScreen);

	//绘画图像
	if ((nDesWidth==nScrWidth)&&(nDesHeight==nScrHeight))
	{
		ScreenDC.BitBlt(0,0,nDesWidth,nDesHeight,pDesDC,nXDes,nYDes,SRCCOPY);
		ImageDC.BitBlt(0,0,nDesWidth,nDesHeight,pScrDC,nXScr,nYScr,SRCCOPY);
	}
	else 
	{
		ScreenDC.StretchBlt(0,0,nDesWidth,nDesHeight,pDesDC,nXDes,nYDes,nScrWidth,nScrHeight,SRCCOPY);
		ImageDC.StretchBlt(0,0,nDesWidth,nDesHeight,pScrDC,nXScr,nYScr,nScrWidth,nScrHeight,SRCCOPY);
	}
	ImageDC.SetBkColor(crTransColor);
	MaskDC.BitBlt(0,0,nDesWidth,nDesHeight,&ImageDC,0,0,SRCCOPY);

	//生成透明区域为黑色的位图
	ImageDC.SetBkColor(RGB(0,0,0));
	ImageDC.SetTextColor(RGB(255,255,255));
	ImageDC.BitBlt(0,0,nDesWidth,nDesHeight,&MaskDC,0,0,SRCAND);

	//其它部分变成黑色
	ScreenDC.SetBkColor(RGB(255,255,255));
	ScreenDC.SetTextColor(RGB(0,0,0));
	ScreenDC.BitBlt(0,0,nDesWidth,nDesHeight,&MaskDC,0,0,SRCAND);
	ScreenDC.BitBlt(0,0,nDesWidth,nDesHeight,&ImageDC,0,0,SRCPAINT);

	//更新屏幕
	pDesDC->BitBlt(nXDes,nYDes,nDesWidth,nDesHeight,&ScreenDC,0,0,SRCCOPY);

	//清理变量
	ImageDC.SelectObject(pOldBmpImage);
	MaskDC.SelectObject(pOldMaskBmp);
	ScreenDC.SelectObject(pOldScreenBmp);
	ImageDC.DeleteDC();
	MaskDC.DeleteDC();
	ScreenDC.DeleteDC();
	BmpImage.DeleteObject();
	BmpScreen.DeleteObject();
	BmpMaskBmp.DeleteObject();

	//恢复设置
	pDesDC->SetTextColor(crTextColor);

	return;
}

//////////////////////////////////////////////////////////////////////////
