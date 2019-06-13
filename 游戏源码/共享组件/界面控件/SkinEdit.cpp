#include "Stdafx.h"
#include "SkinEdit.h"

//////////////////////////////////////////////////////////////////////////

//静态变量
CSkinEditAttribute					CSkinEdit::m_SkinAttribute;				//界面属性

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CSkinEdit, CEdit)

BEGIN_MESSAGE_MAP(CSkinEdit, CEdit)
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CSkinEditAttribute::CSkinEditAttribute() 
{
	//初始化变量
	m_crFocusTX=RGB(0,0,0);
	m_crFocusBK=RGB(0,0,0);
	m_crNoFocusTX=RGB(0,0,0);
	m_crNoFocusBK=RGB(0,0,0);
	m_crDisFocusTX=RGB(0,0,0);
	m_crDisFocusBK=RGB(0,0,0);

	return;
}

//析构函数
CSkinEditAttribute::~CSkinEditAttribute() 
{
}

//加载配置
bool CSkinEditAttribute::UpdateSkinResource(ISkinResource * pISkinResource)
{
	//获取信息
	tagSkinEditResource SkinEditResource;
	if (pISkinResource!=NULL) pISkinResource->GetResourceInfo(SkinEditResource);

	//设置变量
	m_crFocusTX=SkinEditResource.crFocusTX;
	m_crFocusBK=SkinEditResource.crFocusBK;
	m_crNoFocusTX=SkinEditResource.crNoFocusTX;
	m_crNoFocusBK=SkinEditResource.crNoFocusBK;
	m_crDisFocusTX=SkinEditResource.crDisFocusTX;
	m_crDisFocusBK=SkinEditResource.crDisFocusBK;

	//创建画刷
	m_brFocus.DeleteObject();
	m_brNoFocus.DeleteObject();
	m_brDisbale.DeleteObject();
	m_brFocus.CreateSolidBrush(m_crFocusBK);
	m_brNoFocus.CreateSolidBrush(m_crNoFocusBK);

	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CSkinEdit::CSkinEdit()
{
	m_bFocus=false;
}

//析构函数
CSkinEdit::~CSkinEdit()
{
}

//绘画控件
HBRUSH CSkinEdit::CtlColor(CDC * pDC, UINT nCtlColor)
{
	pDC->SetBkMode(OPAQUE);
	if (IsWindowEnabled())
	{
		if (m_bFocus) 
		{
			pDC->SetTextColor(m_SkinAttribute.m_crFocusTX);
			pDC->SetBkColor(m_SkinAttribute.m_crFocusBK);
			return m_SkinAttribute.m_brFocus;
		}
		pDC->SetTextColor(m_SkinAttribute.m_crNoFocusTX);
		pDC->SetBkColor(m_SkinAttribute.m_crNoFocusBK);
		return m_SkinAttribute.m_brNoFocus;
	}
	pDC->SetTextColor(m_SkinAttribute.m_crDisFocusBK);
	pDC->SetBkColor(m_SkinAttribute.m_crDisFocusBK);
	return m_SkinAttribute.m_brDisbale;
}

//得到焦点
void CSkinEdit::OnSetFocus(CWnd * pOldWnd)
{
	m_bFocus=true;
	Invalidate(TRUE);
	__super::OnSetFocus(pOldWnd);
}

//失去焦点
void CSkinEdit::OnKillFocus(CWnd * pNewWnd)
{
	m_bFocus=false;
	Invalidate(TRUE);
	__super::OnKillFocus(pNewWnd);
}

//////////////////////////////////////////////////////////////////////////
