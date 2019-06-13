#include "Stdafx.h"
#include "Resource.h"
#include "GlobalUnits.h"
#include "DlgIndividualInfo.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgIndividualInfo, CSkinDialogEx)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////
//构造函数
CDlgIndividualInfo::CDlgIndividualInfo() : CSkinDialogEx(IDD_MODIFY_INDIVIDUAL)
{

	return;
}

//析构函数
CDlgIndividualInfo::~CDlgIndividualInfo()
{
}

//控件绑定
void CDlgIndividualInfo::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
}

//初始化函数
BOOL CDlgIndividualInfo::OnInitDialog()
{
	__super::OnInitDialog();

	//设置标题
	SetWindowText(TEXT("用户基本信息"));

	//广告控件
	m_BrowerAD.Create(NULL,NULL,WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),this,100,NULL);
	m_BrowerAD.Navigate(TEXT("http://www.dc173.com/personalsettings.asp"),NULL,NULL,NULL,NULL);

	//广告位置
	int nXExcursion=GetXExcursionPos();
	int nYExcursion=GetYExcursionPos();
	m_BrowerAD.MoveWindow(nXExcursion+4,nYExcursion+5,327,248);

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////

