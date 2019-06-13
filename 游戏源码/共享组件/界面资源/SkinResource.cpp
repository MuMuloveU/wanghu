#include "StdAfx.h"
#include "Resource.h"
#include "SkinResource.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CSkinResource::CSkinResource()
{
	m_cbResourceType=0;
}

//析构函数
CSkinResource::~CSkinResource()
{
}

//接口查询
VOID * __cdecl CSkinResource::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ISkinResource,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ISkinResource,Guid,dwQueryVer);
	return NULL;
}

//创建字体
bool __cdecl CSkinResource::CreateDefaultFont(CFont & Font)
{
	//释放对象
	ASSERT(Font.GetSafeHandle()==NULL);
	if (Font.GetSafeHandle()!=NULL) Font.DeleteObject();

	//创建字体
	Font.CreateFont(-12,0,0,0,400,0,0,0,134,3,2,1,2,TEXT("宋体"));
	ASSERT(Font.GetSafeHandle()!=NULL);

	return (Font.GetSafeHandle()!=NULL);
}

//获取信息
bool __cdecl CSkinResource::GetResourceInfo(tagSkinEditResource & SkinEditResource)
{
	//变量定义
	HINSTANCE hInstance=GetModuleHandle(SKIN_RESOURCE_DLL_NAME);

	//颜色变量
	SkinEditResource.crFocusTX=RGB(255,255,255);
	SkinEditResource.crFocusBK=RGB(0,128,250);
	SkinEditResource.crNoFocusTX=RGB(10,10,10);
	SkinEditResource.crNoFocusBK=RGB(255,255,255);
	SkinEditResource.crDisFocusTX=RGB(50,50,50);
	SkinEditResource.crDisFocusBK=RGB(125,125,125);

	return true;
}

//获取信息
bool __cdecl CSkinResource::GetResourceInfo(tagSkinDialogResource & SkinDialogResource)
{
	//变量定义
	HINSTANCE hInstance=GetModuleHandle(SKIN_RESOURCE_DLL_NAME);

	switch(m_cbResourceType)
	{
	case RESOURCE_DEFAULT:
		{
			break;
		}
	case RESOURCE_BLACK:
		{
			break;
		}
	case RESOURCE_RED:
		{
			break;
		}
	}
	//边框信息
	SkinDialogResource.crTitleText=RGB(200,200,200);
	SkinDialogResource.crControlText=RGB(0,0,0);
	SkinDialogResource.crBackGround=RGB(229,242,255);
	SkinDialogResource.crInsideBorder=RGB(85,132,174);
	SkinDialogResource.crBackFrame=RGB(229,242,255);

	//标题信息
	SkinDialogResource.LoadInfoImageTL.hResourceDLL=hInstance;
	SkinDialogResource.LoadInfoImageTM.hResourceDLL=hInstance;
	SkinDialogResource.LoadInfoImageTR.hResourceDLL=hInstance;
	SkinDialogResource.LoadInfoImageTL.uResourceID=DEF_IDB_SKIN_WND_TL;
	SkinDialogResource.LoadInfoImageTM.uResourceID=DEF_IDB_SKIN_WND_TM;
	SkinDialogResource.LoadInfoImageTR.uResourceID=DEF_IDB_SKIN_WND_TR;

	//按钮信息
	SkinDialogResource.LoadInfoButtonMin.hResourceDLL=hInstance;
	SkinDialogResource.LoadInfoButtonMax.hResourceDLL=hInstance;
	SkinDialogResource.LoadInfoButtonRes.hResourceDLL=hInstance;
	SkinDialogResource.LoadInfoButtonClose.hResourceDLL=hInstance;
	SkinDialogResource.LoadInfoButtonMin.uResourceID=DEF_IDB_SKIN_WND_MIN;
	SkinDialogResource.LoadInfoButtonMax.uResourceID=DEF_IDB_SKIN_WND_MAX;
	SkinDialogResource.LoadInfoButtonRes.uResourceID=DEF_IDB_SKIN_WND_RESORE;
	SkinDialogResource.LoadInfoButtonClose.uResourceID=DEF_IDB_SKIN_WND_CLOSE;

	return true;
}

//获取信息
bool __cdecl CSkinResource::GetResourceInfo(tagSkinButtonResource & SkinButtonResource)
{
	//变量定义
	HINSTANCE hInstance=GetModuleHandle(SKIN_RESOURCE_DLL_NAME);

	switch(m_cbResourceType)
	{
	case RESOURCE_DEFAULT:
		{
			break;
		}
	case RESOURCE_BLACK:
		{
			break;
		}
	case RESOURCE_RED:
		{
			break;
		}
	}
	//设置变量
	SkinButtonResource.crButtonText=RGB(10,10,10);
	SkinButtonResource.LoadInfoButton.hResourceDLL=hInstance;
	SkinButtonResource.LoadInfoButton.uResourceID=DEF_IDB_SKIN_BUTTON_BACK;

	return true;
}

//获取信息
bool __cdecl CSkinResource::GetResourceInfo(tagSkinSplitterResource & SkinSplitterResource)
{
	//变量定义
	HINSTANCE hInstance=GetModuleHandle(SKIN_RESOURCE_DLL_NAME);

	switch(m_cbResourceType)
	{
	case RESOURCE_DEFAULT:
		{
			break;
		}
	case RESOURCE_BLACK:
		{
			break;
		}
	case RESOURCE_RED:
		{
			break;
		}
	}
	//标题信息
	SkinSplitterResource.LoadInfoVor.hResourceDLL=hInstance;
	SkinSplitterResource.LoadInfoHor.hResourceDLL=hInstance;
	SkinSplitterResource.LoadInfoVor.uResourceID=DEF_IDB_SKIN_SPLITTER_VOR;
	SkinSplitterResource.LoadInfoHor.uResourceID=DEF_IDB_SKIN_SPLITTER_HOR;

	return true;
}

//获取信息
bool __cdecl CSkinResource::GetResourceInfo(tagSkinListCtrlResource & SkinListCtrlResource)
{
	//变量定义
	HINSTANCE hInstance=GetModuleHandle(SKIN_RESOURCE_DLL_NAME);

	switch(m_cbResourceType)
	{
	case RESOURCE_DEFAULT:
		{
			break;
		}
	case RESOURCE_BLACK:
		{
			break;
		}
	case RESOURCE_RED:
		{
			break;
		}
	}
	//颜色变量
	SkinListCtrlResource.crListTX=RGB(0,18,87);

	//资源信息
	SkinListCtrlResource.LoadInfoImageBack.hResourceDLL=hInstance;
	SkinListCtrlResource.LoadInfoImageBack.uResourceID=DEF_IDB_SKIN_HEAD_TITLE;

	return true;
}

//获取信息
bool __cdecl CSkinResource::GetResourceInfo(tagSkinHyperLinkResource & SkinHyperLinkResource)
{
	switch(m_cbResourceType)
	{
	case RESOURCE_DEFAULT:
		{
			break;
		}
	case RESOURCE_BLACK:
		{
			break;
		}
	case RESOURCE_RED:
		{
			break;
		}
	}
	//设置变量
	SkinHyperLinkResource.crHoverText=RGB(220,0,0);
	SkinHyperLinkResource.crNormalText=RGB(0,0,230);
	SkinHyperLinkResource.crVisitedText=RGB(0,0,100);
	SkinHyperLinkResource.crBackColor=RGB(240,240,240);

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////

//组件创建函数
DECLARE_CREATE_MODULE(SkinResource);

//SkinButtonResource.LoadInfoButton.pszResource=MAKEINTRESOURCE(DEF_IDB_SKIN_BUTTON_BACK);
//////////////////////////////////////////////////////////////////////////////////////
