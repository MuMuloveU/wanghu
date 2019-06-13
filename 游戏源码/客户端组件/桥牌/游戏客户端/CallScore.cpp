#include "StdAfx.h"
#include "CallScore.h"

//花色定义

#define COLOR_NT					0x40								//花色定义
#define COLOR_HEI_TAO				0x30								//花色定义
#define COLOR_HONG_TAO				0x20								//花色定义
#define COLOR_FANG_KUAI				0x10								//花色定义
#define COLOR_MEI_HUA				0x00								//花色定义


//游戏按钮
#define IDC_Call_C1						500
#define IDC_Call_C2						501
#define IDC_Call_C3						502
#define IDC_Call_C4						503
#define IDC_Call_C5						504
#define IDC_Call_C6						505
#define IDC_Call_C7						506

#define IDC_Call_D1						601
#define IDC_Call_D2						602
#define IDC_Call_D3						603
#define IDC_Call_D4						604
#define IDC_Call_D5						605
#define IDC_Call_D6						606
#define IDC_Call_D7						607

#define IDC_Call_H1						701
#define IDC_Call_H2						702
#define IDC_Call_H3						703
#define IDC_Call_H4						704
#define IDC_Call_H5						705
#define IDC_Call_H6						706
#define IDC_Call_H7						707

#define IDC_Call_S1						801
#define IDC_Call_S2						802
#define IDC_Call_S3						803
#define IDC_Call_S4						804
#define IDC_Call_S5						805
#define IDC_Call_S6						806
#define IDC_Call_S7						807

#define IDC_Call_N1						901
#define IDC_Call_N2						902
#define IDC_Call_N3						903
#define IDC_Call_N4						904
#define IDC_Call_N5						905
#define IDC_Call_N6						906
#define IDC_Call_N7						907

#define IDC_Call_B1						1001
#define IDC_Call_B2						1002

#define IDC_GIVE_UP						1003								//不叫
//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CCallScore, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_Call_C1,OnCallC1)
	ON_BN_CLICKED(IDC_Call_C2,OnCallC2)
	ON_BN_CLICKED(IDC_Call_C3,OnCallC3)
	ON_BN_CLICKED(IDC_Call_C4,OnCallC4)
	ON_BN_CLICKED(IDC_Call_C5,OnCallC5)
	ON_BN_CLICKED(IDC_Call_C6,OnCallC6)
	ON_BN_CLICKED(IDC_Call_C7,OnCallC7)

	ON_BN_CLICKED(IDC_Call_D1,OnCallD1)
	ON_BN_CLICKED(IDC_Call_D2,OnCallD2)
	ON_BN_CLICKED(IDC_Call_D3,OnCallD3)
	ON_BN_CLICKED(IDC_Call_D4,OnCallD4)
	ON_BN_CLICKED(IDC_Call_D5,OnCallD5)
	ON_BN_CLICKED(IDC_Call_D6,OnCallD6)
	ON_BN_CLICKED(IDC_Call_D7,OnCallD7)

	ON_BN_CLICKED(IDC_Call_H1,OnCallH1)
	ON_BN_CLICKED(IDC_Call_H2,OnCallH2)
	ON_BN_CLICKED(IDC_Call_H3,OnCallH3)
	ON_BN_CLICKED(IDC_Call_H4,OnCallH4)
	ON_BN_CLICKED(IDC_Call_H5,OnCallH5)
	ON_BN_CLICKED(IDC_Call_H6,OnCallH6)
	ON_BN_CLICKED(IDC_Call_H7,OnCallH7)

	ON_BN_CLICKED(IDC_Call_S1,OnCallS1)
	ON_BN_CLICKED(IDC_Call_S2,OnCallS2)
	ON_BN_CLICKED(IDC_Call_S3,OnCallS3)
	ON_BN_CLICKED(IDC_Call_S4,OnCallS4)
	ON_BN_CLICKED(IDC_Call_S5,OnCallS5)
	ON_BN_CLICKED(IDC_Call_S6,OnCallS6)
	ON_BN_CLICKED(IDC_Call_S7,OnCallS7)

	ON_BN_CLICKED(IDC_Call_N1,OnCallN1)
	ON_BN_CLICKED(IDC_Call_N2,OnCallN2)
	ON_BN_CLICKED(IDC_Call_N3,OnCallN3)
	ON_BN_CLICKED(IDC_Call_N4,OnCallN4)
	ON_BN_CLICKED(IDC_Call_N5,OnCallN5)
	ON_BN_CLICKED(IDC_Call_N6,OnCallN6)
	ON_BN_CLICKED(IDC_Call_N7,OnCallN7)

	ON_BN_CLICKED(IDC_Call_B1,OnCallB1)
	ON_BN_CLICKED(IDC_Call_B2,OnCallB2)

	ON_BN_CLICKED(IDC_GIVE_UP, OnGiveUpScore)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

//构造函数
CCallScore::CCallScore()
{
	return;
}

//析构函数
CCallScore::~CCallScore()
{
}

//建立消息
int CCallScore::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//创建控件
	CRect rcCreate(-999,0,0,0);

	//创建按钮
	m_CallC1.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_Call_C1);
	m_CallC2.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_Call_C2);
	m_CallC3.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_Call_C3);
	m_CallC4.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_Call_C4);
	m_CallC5.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_Call_C5);
	m_CallC6.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_Call_C6);
	m_CallC7.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_Call_C7);

	m_CallD1.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_Call_D1);
	m_CallD2.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_Call_D2);
	m_CallD3.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_Call_D3);
	m_CallD4.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_Call_D4);
	m_CallD5.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_Call_D5);
	m_CallD6.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_Call_D6);
	m_CallD7.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_Call_D7);

	m_CallH1.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_Call_H1);
	m_CallH2.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_Call_H2);
	m_CallH3.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_Call_H3);
	m_CallH4.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_Call_H4);
	m_CallH5.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_Call_H5);
	m_CallH6.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_Call_H6);
	m_CallH7.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_Call_H7);

	m_CallS1.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_Call_S1);
	m_CallS2.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_Call_S2);
	m_CallS3.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_Call_S3);
	m_CallS4.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_Call_S4);
	m_CallS5.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_Call_S5);
	m_CallS6.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_Call_S6);
	m_CallS7.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_Call_S7);

	m_CallN1.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_Call_N1);
	m_CallN2.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_Call_N2);
	m_CallN3.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_Call_N3);
	m_CallN4.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_Call_N4);
	m_CallN5.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_Call_N5);
	m_CallN6.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_Call_N6);
	m_CallN7.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_Call_N7);

	m_CallB1.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_Call_B1);
	m_CallB2.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_Call_B2);
	
	m_btGiveUpScore.Create(TEXT(""),WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_GIVE_UP);

	//设置按钮
	HINSTANCE hInstance=AfxGetInstanceHandle();

	m_CallC1.SetButtonImage(IDB_C1,hInstance,false);
	m_CallC2.SetButtonImage(IDB_C2,hInstance,false);
	m_CallC3.SetButtonImage(IDB_C3,hInstance,false);
	m_CallC4.SetButtonImage(IDB_C4,hInstance,false);
	m_CallC5.SetButtonImage(IDB_C5,hInstance,false);
	m_CallC6.SetButtonImage(IDB_C6,hInstance,false);
	m_CallC7.SetButtonImage(IDB_C7,hInstance,false);

	m_CallD1.SetButtonImage(IDB_D1,hInstance,false);
	m_CallD2.SetButtonImage(IDB_D2,hInstance,false);
	m_CallD3.SetButtonImage(IDB_D3,hInstance,false);
	m_CallD4.SetButtonImage(IDB_D4,hInstance,false);
	m_CallD5.SetButtonImage(IDB_D5,hInstance,false);
	m_CallD6.SetButtonImage(IDB_D6,hInstance,false);
	m_CallD7.SetButtonImage(IDB_D7,hInstance,false);

	m_CallH1.SetButtonImage(IDB_H1,hInstance,false);
	m_CallH2.SetButtonImage(IDB_H2,hInstance,false);
	m_CallH3.SetButtonImage(IDB_H3,hInstance,false);
	m_CallH4.SetButtonImage(IDB_H4,hInstance,false);
	m_CallH5.SetButtonImage(IDB_H5,hInstance,false);
	m_CallH6.SetButtonImage(IDB_H6,hInstance,false);
	m_CallH7.SetButtonImage(IDB_H7,hInstance,false);

	m_CallS1.SetButtonImage(IDB_S1,hInstance,false);
	m_CallS2.SetButtonImage(IDB_S2,hInstance,false);
	m_CallS3.SetButtonImage(IDB_S3,hInstance,false);
	m_CallS4.SetButtonImage(IDB_S4,hInstance,false);
	m_CallS5.SetButtonImage(IDB_S5,hInstance,false);
	m_CallS6.SetButtonImage(IDB_S6,hInstance,false);
	m_CallS7.SetButtonImage(IDB_S7,hInstance,false);

	m_CallN1.SetButtonImage(IDB_N1,hInstance,false);
	m_CallN2.SetButtonImage(IDB_N2,hInstance,false);
	m_CallN3.SetButtonImage(IDB_N3,hInstance,false);
	m_CallN4.SetButtonImage(IDB_N4,hInstance,false);
	m_CallN5.SetButtonImage(IDB_N5,hInstance,false);
	m_CallN6.SetButtonImage(IDB_N6,hInstance,false);
	m_CallN7.SetButtonImage(IDB_N7,hInstance,false);

	m_CallB1.SetButtonImage(IDB_B1,hInstance,false);
	m_CallB2.SetButtonImage(IDB_B2,hInstance,false);
	m_btGiveUpScore.SetButtonImage(IDB_PASS,hInstance,false);

	

	//设置背景
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);

	//加载资源
	m_ImageScoreBack.SetLoadInfo(IDB_CALL_SCORE,hInstance);     //背景
	//移动窗口
	CImageHandle ImageHandle(&m_ImageScoreBack);
	SetWindowPos(NULL,0,0,m_ImageScoreBack.GetWidth(),m_ImageScoreBack.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);

	return true;
}

//重画函数
void CCallScore::OnPaint() 
{
	CPaintDC dc(this);
	
	CImageHandle HandleBack(&m_ImageScoreBack);
	
	m_ImageScoreBack.BitBlt(dc,0,0);

	ReloadView();

	return;
}

//重新改变按钮状态
void CCallScore::ReloadView() 
{
	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	int nWidth=rcClient.Width();
	int nHeight=rcClient.Height();

	HDWP hDwp=BeginDeferWindowPos(32);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;

	DeferWindowPos(hDwp,m_CallC1,NULL,3,30,0,0,uFlags);
	DeferWindowPos(hDwp,m_CallC2,NULL,3,50,0,0,uFlags);
	DeferWindowPos(hDwp,m_CallC3,NULL,3,70,0,0,uFlags);
	DeferWindowPos(hDwp,m_CallC4,NULL,3,90,0,0,uFlags);
	DeferWindowPos(hDwp,m_CallC5,NULL,3,110,0,0,uFlags);
	DeferWindowPos(hDwp,m_CallC6,NULL,3,130,0,0,uFlags);
	DeferWindowPos(hDwp,m_CallC7,NULL,3,150,0,0,uFlags);

	DeferWindowPos(hDwp,m_CallD1,NULL,40,30,0,0,uFlags);
	DeferWindowPos(hDwp,m_CallD2,NULL,40,50,0,0,uFlags);
	DeferWindowPos(hDwp,m_CallD3,NULL,40,70,0,0,uFlags);
	DeferWindowPos(hDwp,m_CallD4,NULL,40,90,0,0,uFlags);
	DeferWindowPos(hDwp,m_CallD5,NULL,40,110,0,0,uFlags);
	DeferWindowPos(hDwp,m_CallD6,NULL,40,130,0,0,uFlags);
	DeferWindowPos(hDwp,m_CallD7,NULL,40,150,0,0,uFlags);

	DeferWindowPos(hDwp,m_CallH1,NULL,77,30,0,0,uFlags);
	DeferWindowPos(hDwp,m_CallH2,NULL,77,50,0,0,uFlags);
	DeferWindowPos(hDwp,m_CallH3,NULL,77,70,0,0,uFlags);
	DeferWindowPos(hDwp,m_CallH4,NULL,77,90,0,0,uFlags);
	DeferWindowPos(hDwp,m_CallH5,NULL,77,110,0,0,uFlags);
	DeferWindowPos(hDwp,m_CallH6,NULL,77,130,0,0,uFlags);
	DeferWindowPos(hDwp,m_CallH7,NULL,77,150,0,0,uFlags);

	DeferWindowPos(hDwp,m_CallS1,NULL,114,30,0,0,uFlags);
	DeferWindowPos(hDwp,m_CallS2,NULL,114,50,0,0,uFlags);
	DeferWindowPos(hDwp,m_CallS3,NULL,114,70,0,0,uFlags);
	DeferWindowPos(hDwp,m_CallS4,NULL,114,90,0,0,uFlags);
	DeferWindowPos(hDwp,m_CallS5,NULL,114,110,0,0,uFlags);
	DeferWindowPos(hDwp,m_CallS6,NULL,114,130,0,0,uFlags);
	DeferWindowPos(hDwp,m_CallS7,NULL,114,150,0,0,uFlags);

	DeferWindowPos(hDwp,m_CallN1,NULL,151,30,0,0,uFlags);
	DeferWindowPos(hDwp,m_CallN2,NULL,151,50,0,0,uFlags);
	DeferWindowPos(hDwp,m_CallN3,NULL,151,70,0,0,uFlags);
	DeferWindowPos(hDwp,m_CallN4,NULL,151,90,0,0,uFlags);
	DeferWindowPos(hDwp,m_CallN5,NULL,151,110,0,0,uFlags);
	DeferWindowPos(hDwp,m_CallN6,NULL,151,130,0,0,uFlags);
	DeferWindowPos(hDwp,m_CallN7,NULL,151,150,0,0,uFlags);
	
	DeferWindowPos(hDwp,m_CallB1,NULL,20,170,0,0,uFlags);
	DeferWindowPos(hDwp,m_CallB2,NULL,110,170,0,0,uFlags);
	
	DeferWindowPos(hDwp,m_btGiveUpScore,NULL,nWidth/2-35,190,0,0,uFlags);
	
	//移动控件
	EndDeferWindowPos(hDwp);
	
	m_CallC1.EnableWindow(CompareCallScore(COLOR_MEI_HUA,1)?true:false);
	m_CallC2.EnableWindow(CompareCallScore(COLOR_MEI_HUA,2)?true:false);
	m_CallC3.EnableWindow(CompareCallScore(COLOR_MEI_HUA,3)?true:false);
	m_CallC4.EnableWindow(CompareCallScore(COLOR_MEI_HUA,4)?true:false);
	m_CallC5.EnableWindow(CompareCallScore(COLOR_MEI_HUA,5)?true:false);
	m_CallC6.EnableWindow(CompareCallScore(COLOR_MEI_HUA,6)?true:false);
	m_CallC7.EnableWindow(CompareCallScore(COLOR_MEI_HUA,7)?true:false);

	m_CallD1.EnableWindow(CompareCallScore(COLOR_FANG_KUAI,1)?true:false);
	m_CallD2.EnableWindow(CompareCallScore(COLOR_FANG_KUAI,2)?true:false);
	m_CallD3.EnableWindow(CompareCallScore(COLOR_FANG_KUAI,3)?true:false);
	m_CallD4.EnableWindow(CompareCallScore(COLOR_FANG_KUAI,4)?true:false);
	m_CallD5.EnableWindow(CompareCallScore(COLOR_FANG_KUAI,5)?true:false);
	m_CallD6.EnableWindow(CompareCallScore(COLOR_FANG_KUAI,6)?true:false);
	m_CallD7.EnableWindow(CompareCallScore(COLOR_FANG_KUAI,7)?true:false);

	m_CallH1.EnableWindow(CompareCallScore(COLOR_HONG_TAO,1)?true:false);
	m_CallH2.EnableWindow(CompareCallScore(COLOR_HONG_TAO,2)?true:false);
	m_CallH3.EnableWindow(CompareCallScore(COLOR_HONG_TAO,3)?true:false);
	m_CallH4.EnableWindow(CompareCallScore(COLOR_HONG_TAO,4)?true:false);
	m_CallH5.EnableWindow(CompareCallScore(COLOR_HONG_TAO,5)?true:false);
	m_CallH6.EnableWindow(CompareCallScore(COLOR_HONG_TAO,6)?true:false);
	m_CallH7.EnableWindow(CompareCallScore(COLOR_HONG_TAO,7)?true:false);

	m_CallS1.EnableWindow(CompareCallScore(COLOR_HEI_TAO,1)?true:false);
	m_CallS2.EnableWindow(CompareCallScore(COLOR_HEI_TAO,2)?true:false);
	m_CallS3.EnableWindow(CompareCallScore(COLOR_HEI_TAO,3)?true:false);
	m_CallS4.EnableWindow(CompareCallScore(COLOR_HEI_TAO,4)?true:false);
	m_CallS5.EnableWindow(CompareCallScore(COLOR_HEI_TAO,5)?true:false);
	m_CallS6.EnableWindow(CompareCallScore(COLOR_HEI_TAO,6)?true:false);
	m_CallS7.EnableWindow(CompareCallScore(COLOR_HEI_TAO,7)?true:false);

	m_CallN1.EnableWindow(CompareCallScore(COLOR_NT,1)?true:false);
	m_CallN2.EnableWindow(CompareCallScore(COLOR_NT,2)?true:false);
	m_CallN3.EnableWindow(CompareCallScore(COLOR_NT,3)?true:false);
	m_CallN4.EnableWindow(CompareCallScore(COLOR_NT,4)?true:false);
	m_CallN5.EnableWindow(CompareCallScore(COLOR_NT,5)?true:false);
	m_CallN6.EnableWindow(CompareCallScore(COLOR_NT,6)?true:false);
	m_CallN7.EnableWindow(CompareCallScore(COLOR_NT,7)?true:false);

	m_CallB1.EnableWindow((m_bDbl==true)?true:false);
	m_CallB2.EnableWindow((m_bReDbl==true)?true:false);
	
	m_btGiveUpScore.EnableWindow(true);
	//m_btGiveUpScore.EnableWindow((m_cbMultiples==1)?true:false);
}

//------------------------------------------------------------------------------------------------
void CCallScore::OnCallC1()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,COLOR_MEI_HUA,1);
	return;
}
void CCallScore::OnCallC2()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,COLOR_MEI_HUA,2);
	return;
}
void CCallScore::OnCallC3()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,COLOR_MEI_HUA,3);
	return;
}
void CCallScore::OnCallC4()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,COLOR_MEI_HUA,4);
	return;
}
void CCallScore::OnCallC5()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,COLOR_MEI_HUA,5);
	return;
}
void CCallScore::OnCallC6()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,COLOR_MEI_HUA,6);
	return;
}
void CCallScore::OnCallC7()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,COLOR_MEI_HUA,7);
	return;
}

//------------------------------------------------------------------------------------------------
void CCallScore::OnCallD1()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,COLOR_FANG_KUAI,1);
	return;
}
void CCallScore::OnCallD2()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,COLOR_FANG_KUAI,2);
	return;
}
void CCallScore::OnCallD3()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,COLOR_FANG_KUAI,3);
	return;
}
void CCallScore::OnCallD4()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,COLOR_FANG_KUAI,4);
	return;
}
void CCallScore::OnCallD5()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,COLOR_FANG_KUAI,5);
	return;
}
void CCallScore::OnCallD6()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,COLOR_FANG_KUAI,6);
	return;
}
void CCallScore::OnCallD7()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,COLOR_FANG_KUAI,7);
	return;
}
//------------------------------------------------------------------------------------------------
void CCallScore::OnCallH1()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,COLOR_HONG_TAO,1);
	return;
}
void CCallScore::OnCallH2()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,COLOR_HONG_TAO,2);
	return;
}
void CCallScore::OnCallH3()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,COLOR_HONG_TAO,3);
	return;
}
void CCallScore::OnCallH4()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,COLOR_HONG_TAO,4);
	return;
}
void CCallScore::OnCallH5()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,COLOR_HONG_TAO,5);
	return;
}
void CCallScore::OnCallH6()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,COLOR_HONG_TAO,6);
	return;
}
void CCallScore::OnCallH7()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,COLOR_HONG_TAO,7);
	return;
}

//------------------------------------------------------------------------------------------------
void CCallScore::OnCallS1()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,COLOR_HEI_TAO,1);
	return;
}
void CCallScore::OnCallS2()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,COLOR_HEI_TAO,2);
	return;
}
void CCallScore::OnCallS3()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,COLOR_HEI_TAO,3);
	return;
}
void CCallScore::OnCallS4()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,COLOR_HEI_TAO,4);
	return;
}
void CCallScore::OnCallS5()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,COLOR_HEI_TAO,5);
	return;
}
void CCallScore::OnCallS6()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,COLOR_HEI_TAO,6);
	return;
}
void CCallScore::OnCallS7()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,COLOR_HEI_TAO,7);
	return;
}
//------------------------------------------------------------------------------------------------
void CCallScore::OnCallN1()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,COLOR_NT,1);
	return;
}
void CCallScore::OnCallN2()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,COLOR_NT,2);
	return;
}
void CCallScore::OnCallN3()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,COLOR_NT,3);
	return;
}
void CCallScore::OnCallN4()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,COLOR_NT,4);
	return;
}
void CCallScore::OnCallN5()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,COLOR_NT,5);
	return;
}
void CCallScore::OnCallN6()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,COLOR_NT,6);
	return;
}
void CCallScore::OnCallN7()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,COLOR_NT,7);
	return;
}
//------------------------------------------------------------------------------------------------
void CCallScore::OnCallB1()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_MULTIPLES,2,2);
	return;
}
void CCallScore::OnCallB2()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_MULTIPLES,4,4);
	return;
}
//------------------------------------------------------------------------------------------------
void CCallScore::OnGiveUpScore()
{
	AfxGetMainWnd()->SendMessage(IDM_LAND_SCORE,255,250);
	return;
}

//比较叫牌
bool CCallScore::CompareCallScore(BYTE bColor,BYTE bValue)
{
	if(m_cbLandScore==250||m_cbLandColor==0xFF) return true;
	if(bColor<m_cbLandColor)
	{
		if(bValue>m_cbLandScore) 
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		if(bColor==m_cbLandColor) 
		{
			if(bValue>m_cbLandScore) 
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			if(bValue>=m_cbLandScore) 
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
