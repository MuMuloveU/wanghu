#include "Stdafx.h"
#include "NtList.h"

#include "Resource.h"
#include ".\ntlist.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



BEGIN_MESSAGE_MAP(CNtList, CDialog)
	ON_BN_CLICKED(IDC_1, On1)
	ON_BN_CLICKED(IDC_2, On2)
	ON_BN_CLICKED(IDC_3, On3)
	ON_BN_CLICKED(IDC_4, On4)
	ON_BN_CLICKED(IDC_5, OnBnClicked5)
	ON_BN_CLICKED(IDC_7, OnBnClicked7)
END_MESSAGE_MAP()

//构造函数
CNtList::CNtList(CWnd* pParent /*=NULL*/): CDialog(CNtList::IDD, pParent)
{
}

//DDX 支持
void CNtList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_4, m_Bm4);
	DDX_Control(pDX, IDC_3, m_Bm3);
	DDX_Control(pDX, IDC_2, m_Bm2);
	DDX_Control(pDX, IDC_1, m_Bm1);
	DDX_Control(pDX, IDC_5, m_Bm5);
	DDX_Control(pDX, IDC_7, m_bt20);
}

//初始化函数
BOOL CNtList::OnInitDialog() 
{
	CDialog::OnInitDialog();

	//加载位图;
	m_Bm1.LoadBitmaps(TEXT("FL1U"),TEXT("FL1D"),TEXT("FL1U"),TEXT("FL1X")); //黑桃
	m_Bm2.LoadBitmaps(TEXT("FL3U"),TEXT("FL3D"),TEXT("FL3U"),TEXT("FL3X")); //红桃
	m_Bm3.LoadBitmaps(TEXT("FL2U"),TEXT("FL2D"),TEXT("FL2U"),TEXT("FL2X")); //梅花
	m_Bm4.LoadBitmaps(TEXT("FL4U"),TEXT("FL4D"),TEXT("FL4U"),TEXT("FL4X")); //方块

	m_Bm5.LoadBitmaps(TEXT("FLSKU"),TEXT("FLSKD"),TEXT("FLSKU"),TEXT("FLSKX"));//猫
	m_bt20.LoadBitmaps(TEXT("FLSKU"),TEXT("FLSKD"),TEXT("FLSKU"),TEXT("FLSKX"));//大猫

	m_Bm1.SizeToContent();
	m_Bm2.SizeToContent();
	m_Bm3.SizeToContent();
	m_Bm4.SizeToContent();
	m_Bm5.SizeToContent();
	m_bt20.SizeToContent();


	int iHide=0,iWide=0;
	CRect rect;
	m_Bm1.GetWindowRect(&rect);
	iWide=rect.Width()*6;
	iHide=rect.Height();

	m_bt20.MoveWindow(0,0,rect.Width(),rect.Height());
	m_Bm5.MoveWindow(rect.Width(),0,rect.Width(),rect.Height());
	m_Bm1.MoveWindow(rect.Width()*2,0,rect.Width(),rect.Height());
	m_Bm2.MoveWindow(rect.Width()*3,0,rect.Width(),rect.Height());
	m_Bm3.MoveWindow(rect.Width()*4,0,rect.Width(),rect.Height());
	m_Bm4.MoveWindow(rect.Width()*5,0,rect.Width(),rect.Height());

	GetWindowRect(&rect);
	MoveWindow(0,0,iWide,iHide);
	return TRUE;
}

//更新按钮状态
void CNtList::UpdateNtList(BOOL b[])
{
	m_bt20.EnableWindow(b[5]);
	m_Bm5.EnableWindow(b[4]);
	m_Bm1.EnableWindow(b[3]);
	m_Bm2.EnableWindow(b[2]);
	m_Bm3.EnableWindow(b[1]);
	m_Bm4.EnableWindow(b[0]);
	return;
}

//按钮消息
void CNtList::On1()   
{	
	AfxGetMainWnd()->SendMessage(IDM_PUBLIC_CARD,UG_HEI_TAO,UG_HEI_TAO); 
}

void CNtList::On2()   
{	
	AfxGetMainWnd()->SendMessage(IDM_PUBLIC_CARD,UG_HONG_TAO,UG_HONG_TAO); 
}

void CNtList::On3()   
{	
	AfxGetMainWnd()->SendMessage(IDM_PUBLIC_CARD,UG_MEI_HUA,UG_MEI_HUA); 
}

void CNtList::On4()  
{	
	AfxGetMainWnd()->SendMessage(IDM_PUBLIC_CARD,UG_FANG_KUAI,UG_FANG_KUAI); 
}

void CNtList::OnBnClicked5()
{
	AfxGetMainWnd()->SendMessage(IDM_PUBLIC_CARD,UG_NT_CARD_KING,0); 
}

void CNtList::OnBnClicked7()
{
	// TODO: 在此添加控件通知处理程序代码
	AfxGetMainWnd()->SendMessage(IDM_PUBLIC_CARD,UG_NT_CARD_KING,1); 
}
