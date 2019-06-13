#include "StdAfx.h"
#include "SkinEdit.h"
#include "SkinDialog.h"
#include "SkinButton.h"
#include "SkinListCtrl.h"
#include "SkinSplitter.h"
#include "SkinHyperLink.h"
#include "SkinResourceManager.h"

//////////////////////////////////////////////////////////////////////////
//静态变量

CFont				CSkinResourceManager::m_DefaultFont;				//默认字体
ISkinResource *		CSkinResourceManager::m_pISkinResource=NULL;		//资源接口
//////////////////////////////////////////////////////////////////////////

//构造函数
CSkinResourceManager::CSkinResourceManager()
{
	return;
}

//析构函数
CSkinResourceManager::~CSkinResourceManager()
{
	return;
}

//加载资源
bool CSkinResourceManager::LoadSkinResource(IUnknownEx * pIUnknownEx)
{
	//效验参数
	ASSERT(pIUnknownEx!=NULL);
	if (pIUnknownEx==NULL) return false;

	//查询接口
	ISkinResource * pISkinResource=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ISkinResource);
	ASSERT(pISkinResource!=NULL);
	if (pISkinResource==NULL) return false;

	//设置资源
	if (m_pISkinResource!=pISkinResource)
	{
		//释放资源
		m_DefaultFont.DeleteObject();
		if (m_pISkinResource!=NULL) m_pISkinResource->Release();

		//创建资源
		m_pISkinResource=pISkinResource;
		if (m_pISkinResource->CreateDefaultFont(m_DefaultFont)==false) return false;

		//更新通知
		if (CSkinEdit::m_SkinAttribute.UpdateSkinResource(pISkinResource)==false) return false;
		if (CSkinButton::m_SkinAttribute.UpdateSkinResource(pISkinResource)==false) return false;
		if (CSkinDialog::m_SkinAttribute.UpdateSkinResource(pISkinResource)==false) return false;
		if (CSkinSplitter::m_SkinAttribute.UpdateSkinResource(pISkinResource)==false) return false;
		if (CSkinHeaderCtrl::m_SkinAttribute.UpdateSkinResource(pISkinResource)==false) return false;
		if (CSkinHyperLink::m_SkinAttribute.UpdateSkinResource(pISkinResource)==false) return false;
	}

	return true;
}

//获取资源
VOID * CSkinResourceManager::GetSkinResource(REFGUID Guid, DWORD dwQueryVer)
{
	if (m_pISkinResource==NULL) return NULL;
	return m_pISkinResource->QueryInterface(Guid,dwQueryVer);
}

//////////////////////////////////////////////////////////////////////////
