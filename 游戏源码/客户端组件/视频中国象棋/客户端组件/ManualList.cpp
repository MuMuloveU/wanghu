#include "Stdafx.h"
#include "ManualList.h"

//////////////////////////////////////////////////////////////////////////

#define COLOR_LIST_BACK				RGB(210,193,158)					//背景颜色

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CManualList, CListBox)
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CManualList::CManualList()
{
	//创建画刷
	m_BackBrush.CreateSolidBrush(COLOR_LIST_BACK);

	return;
}

//析构函数
CManualList::~CManualList()
{
}

//控件颜色
HBRUSH CManualList::CtlColor(CDC * pDC, UINT nCtlColor)
{
	//设置颜色
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetBkColor(COLOR_LIST_BACK);
	//pDC->SetTextColor(CSkinDialog::m_SkinAttribute.m_crControlText);
	
	return m_BackBrush;
}

//////////////////////////////////////////////////////////////////////////
