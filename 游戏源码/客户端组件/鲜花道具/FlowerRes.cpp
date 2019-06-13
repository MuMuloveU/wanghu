#include "StdAfx.h"
#include ".\flowerres.h"
#include "resource.h"

//构造函数
CFlowerRes::CFlowerRes(void)
{
	//装载资源
	UINT uResourceID[] = {FLOWER1_3, FLOWER2_3, FLOWER3_3, FLOWER4_3, FLOWER5_3,FLOWER6_3, FLOWER7_3, FLOWER8_3, FLOWER9_3, FLOWER10_3, FLOWER11_3};
	HINSTANCE hInstance = GetModuleHandle(PROPERTY_MODULE_DLL_NAME);

	for ( int n = 0; n < CountArray(m_ImageFlower); ++n )
		m_ImageFlower[n].SetLoadInfo(uResourceID[n], hInstance);
}

//析构函数
CFlowerRes::~CFlowerRes(void)
{
}

//绘画鲜花
void __cdecl CFlowerRes::DrawFlower(CDC * pDC, int nXPos, int nYPos, UINT uFlowerID)
{
	CImageHandle ImageHandleFlower(&m_ImageFlower[uFlowerID]);
	m_ImageFlower[uFlowerID].AlphaDrawImage(pDC, nXPos, nYPos, RGB(255,255,255));
	return ;
}

//绘画鲜花
void __cdecl CFlowerRes::DrawGift(CDC * pDC, int nXPos, int nYPos, UINT uFlowerID)
{
	//加载图片
	CPngImage PngGift;
	CString strInfo=TEXT("MOVE_GIFT"),strImage;
	strImage.Format(TEXT("%s%ld"),strInfo,uFlowerID+1);
	PngGift.LoadImage(GetModuleHandle(PROPERTY_MODULE_DLL_NAME),strImage);

	//绘画图片
	PngGift.DrawImage(pDC, nXPos, nYPos);
	return ;
}


//接口查询
void * __cdecl CFlowerRes::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IFlowerRes,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IFlowerRes,Guid,dwQueryVer);
	return NULL;
}

//////////////////////////////////////////////////////////////////////////

//建立对象函数
extern "C" __declspec(dllexport) void * __cdecl CreateFlowerRes(const GUID & Guid, DWORD dwInterfaceVer)
{
	//建立对象
	CFlowerRes * pFlowerRes=NULL;
	try
	{
		pFlowerRes=new CFlowerRes();
		if (pFlowerRes==NULL) throw TEXT("创建失败");
		void * pObject=pFlowerRes->QueryInterface(Guid,dwInterfaceVer);
		if (pObject==NULL) throw TEXT("接口查询失败");
		return pObject;
	}
	catch (...) {}

	//清理对象
	SafeDelete(pFlowerRes);
	return NULL;
}

//////////////////////////////////////////////////////////////////////////