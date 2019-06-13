#include "stdafx.h"
#include "resource.h"
#include "PropertyBar.h"
#include "PropertyItem.h"

///////////////////////////////////////////////////////////////////////////////////
//宏定义
#define CR_BACK_GROUND					RGB(57,162,214)					//背景颜色

#define BTM_BUTTON_Y					41									//按钮位置
#define TOP_BUTTON_Y					6									//按钮位置
#define BUTTON_EXCUSION_X				8									//偏移位置
#define BUTTON_DISTANCE_X				5									//按钮间距

//变量定义
tagPropertyInfo	CPropertyBar::m_PropertyInfo[PROPERTY_COUNT];				//道具信息
tagFlowerInfo	CPropertyBar::m_FlowerInfo[FLOWER_COUNT];					//鲜花信息

//////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CPropertyBar, CDialog)

	//控制消息
	ON_BN_CLICKED(IDC_BOTTOM_MOVE_TO_L, OnBnClickedBtmMoveToL)
	ON_BN_CLICKED(IDC_BOTTOM_MOVE_TO_R, OnBnClickedBtmMoveToR)
	ON_BN_CLICKED(IDC_TOP_MOVE_TO_L, OnBnClickedTopMoveToL)
	ON_BN_CLICKED(IDC_TOP_MOVE_TO_R, OnBnClickedTopMoveToR)

	//普通消息
	ON_WM_PAINT()
	ON_WM_SIZE()

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////

//静态变量
CPropertyBar *CPropertyBar::m_pPropertyBar = NULL;					//控制对象


//////////////////////////////////////////////////////////////////////
//构造函数
CPropertyBar::CPropertyBar(CWnd* pParent):CDialog(IDD_PROPERTY_BAR, pParent)
{
	//资源变量
	m_ReshInstance = NULL;
	ZeroMemory(&m_PropertyViewImage, sizeof(m_PropertyViewImage));	

	//组件变量
	m_pIPurchaseInfo = NULL;

	//控制位置
	m_nXoriginal=0;
	m_nButtonCount=0;
	m_nTopButtonHead=0;	
	m_nBtmButtonHead=0;	
	//初始变量
	m_bInitDialog = false;
	m_pPropertyItem= new CPropertyItem;
	m_pFlowerSetting = new CFlowerSetting;

	//静态变量
	//ASSERT(m_pPropertyBar==NULL);
	m_pPropertyBar = this;

	//道具变量
	m_nPropertyCount=0;	
	m_nFlowerCount=0;
	ZeroMemory(m_PropertyInfo, CountArray(m_PropertyInfo));	
	ZeroMemory(m_FlowerInfo, CountArray(m_FlowerInfo));	
	for ( int nIndex = 0; nIndex < CountArray(m_PropertyInfo); ++nIndex ) m_PropertyInfo[nIndex].bNullity = true;
	for ( int nIndex = 0; nIndex < CountArray(m_FlowerInfo); ++nIndex ) m_FlowerInfo[nIndex].bNullity = true;
}

//析构函数
CPropertyBar::~CPropertyBar()
{
	delete m_pPropertyItem;
	delete m_pFlowerSetting;
	m_pPropertyItem=NULL;
	m_pFlowerSetting=NULL;
}

//控件绑定
void CPropertyBar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//道具映射
	UINT uProperyID[] = {IDC_BT_PROPERTY1,IDC_BT_PROPERTY2,IDC_BT_PROPERTY3,IDC_BT_PROPERTY4,IDC_BT_PROPERTY5,IDC_BT_PROPERTY6,
		IDC_BT_PROPERTY7,IDC_BT_PROPERTY8,IDC_BT_PROPERTY9,IDC_BT_PROPERTY10,IDC_BT_PROPERTY11,IDC_BT_PROPERTY12,IDC_BT_PROPERTY13,
		IDC_BT_PROPERTY14,IDC_BT_PROPERTY15};
	for ( int nIndex = 0; nIndex < CountArray(m_BtProperty); ++nIndex )
		DDX_Control(pDX, uProperyID[nIndex], m_BtProperty[nIndex]);

	//鲜花映射
	UINT uFlowerID[] = {IDC_BT_FLOWER1,IDC_BT_FLOWER2,IDC_BT_FLOWER3,IDC_BT_FLOWER4,IDC_BT_FLOWER5,IDC_BT_FLOWER6,IDC_BT_FLOWER7,
	IDC_BT_FLOWER8,IDC_BT_FLOWER9,IDC_BT_FLOWER10,IDC_BT_FLOWER11};
	for ( int nIndex = 0; nIndex < CountArray(m_BtFlower); ++nIndex ) 
		DDX_Control(pDX, uFlowerID[nIndex], m_BtFlower[nIndex]);

	//控制映射
	DDX_Control(pDX, IDC_TOP_MOVE_TO_L, m_btTopMoveToL);
	DDX_Control(pDX, IDC_TOP_MOVE_TO_R, m_btTopMoveToR);
	DDX_Control(pDX, IDC_BOTTOM_MOVE_TO_L, m_btBtmMoveToL);
	DDX_Control(pDX, IDC_BOTTOM_MOVE_TO_R, m_btBtmMoveToR);

	//初始变量
	m_bInitDialog = true;

	return;
}

//绘画函数
void CPropertyBar::OnPaint()
{
	CPaintDC dc(this); 

	//获取大小
	CRect rcClient;
	GetWindowRect(rcClient);

	//填充背景
	dc.FillSolidRect(0, 0, rcClient.Width(), rcClient.Height(), CR_BACK_GROUND);

	//图片句柄
	CImageHandle ImagePropertyLHandle(&m_ImageL);
	CImageHandle ImagePropertyMHandle(&m_ImageM);
	CImageHandle ImagePropertyRHandle(&m_ImageR);

	//开始绘画
	m_ImageL.BitBlt(dc.GetSafeHdc(), 0, 0);
	int nXDest = rcClient.Width() - m_ImageR.GetWidth();
	for ( int nXPos = m_ImageL.GetWidth(); nXPos <= nXDest; nXPos += m_ImageM.GetWidth() )
	{
		int nWidth = __min(nXDest-nXPos,m_ImageM.GetWidth());
		m_ImageM.BitBlt(dc.GetSafeHdc(), 0+nXPos, 0, nWidth, m_ImageM.GetHeight(),0,0);
	}
	m_ImageR.BitBlt(dc.GetSafeHdc(), nXDest , 0);

	return;
}

//初始函数
BOOL CPropertyBar::OnInitDialog()
{
	CDialog::OnInitDialog();

	//资源效验
	ASSERT(m_ReshInstance!=NULL);
	if(m_ReshInstance==NULL)return FALSE;

	//加载资源
	UpdateSkinResource();

	//置能按钮
	m_btTopMoveToL.EnableWindow(FALSE);
	m_btTopMoveToR.EnableWindow(FALSE);
	m_btBtmMoveToL.EnableWindow(FALSE);
	m_btBtmMoveToR.EnableWindow(FALSE);

	//道具提示
	m_ToolTipCtrl.Create(this);
	m_ToolTipCtrl.Activate(TRUE);
	for ( int nIndex = 0; nIndex < CountArray(m_BtProperty); ++nIndex )
		m_ToolTipCtrl.AddTool(&m_BtProperty[nIndex],g_PropertyDescribe[nIndex].szName);

	//鲜花提示
	for ( int nIndex = 0; nIndex < CountArray(m_BtFlower); ++nIndex )
		m_ToolTipCtrl.AddTool(&m_BtFlower[nIndex],g_FlowerDescribe[nIndex].szName);

	return TRUE; 
}

//调整控件
void CPropertyBar::RectifyControl(int nWidth, int nHeight)
{
	//初始判断
	if ( m_bInitDialog == false ) return ;

	//获取大小
	CRect rcClient;
	GetWindowRect(rcClient);

	//设置图片
	CImageHandle ImageHandleAreaL(&m_ImageL);
	CImageHandle ImageHandleAreaLM(&m_ImageM);
	CImageHandle ImageHandleAreaLR(&m_ImageR);

	//获取大小
	const int nAreaLWidth = m_ImageL.GetWidth();
	const int nAreaMWidth = m_ImageM.GetWidth();
	const int nAreaRWidth = m_ImageR.GetWidth();
	const int nAreaMGift = m_ImageM.GetWidth();

	//按钮大小
	CRect rcButtonClient,rcMoveButClient;
	m_BtProperty[0].GetWindowRect(rcButtonClient);
	m_btTopMoveToL.GetWindowRect(rcMoveButClient);

	//按钮个数
	INT nAllLong = rcClient.Width() - (nAreaLWidth-5) - (14+rcMoveButClient.Width()*2+8);
	m_nButtonCount = nAllLong / (rcButtonClient.Width()+BUTTON_DISTANCE_X);
	const int nButtonCount=m_nButtonCount;

	//起始位置
	m_nXoriginal = nAreaLWidth-5+(nAllLong-m_nButtonCount*(rcButtonClient.Width()+BUTTON_DISTANCE_X))/2;

	//设置变量
	m_nBtmButtonHead = 0;
	m_nTopButtonHead = 0;

	//使能判断
	m_btBtmMoveToR.EnableWindow(FALSE);
	m_btBtmMoveToL.EnableWindow((m_nFlowerCount <= nButtonCount)?FALSE:TRUE);
	m_btTopMoveToR.EnableWindow(FALSE);
	m_btTopMoveToL.EnableWindow((m_nPropertyCount <= nButtonCount)?FALSE:TRUE);

	//移动按钮
	HDWP hDwp = BeginDeferWindowPos(20);
	const UINT uFlags = SWP_NOSIZE | SWP_NOCOPYBITS | SWP_NOACTIVATE | SWP_NOZORDER;

	//左右按钮
	DeferWindowPos(hDwp, m_btTopMoveToR, NULL, rcClient.Width() - 18-rcMoveButClient.Width()*2, 10, 0, 0, uFlags);
	DeferWindowPos(hDwp, m_btTopMoveToL, NULL, rcClient.Width() - rcMoveButClient.Width()-10, 10, 0, 0, uFlags);
	DeferWindowPos(hDwp, m_btBtmMoveToL, NULL, rcClient.Width() - rcMoveButClient.Width()-10, 45, 0, 0, uFlags);
	DeferWindowPos(hDwp, m_btBtmMoveToR, NULL, rcClient.Width() - 18-rcMoveButClient.Width()*2, 45, 0, 0, uFlags);

	EndDeferWindowPos(hDwp);

	//更新控件
	UpdateTopButton(m_nTopButtonHead);
	UpdateBtmButton(m_nBtmButtonHead);

	return;
}

//窗口更改
void CPropertyBar::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	//调整控件
	RectifyControl(cx, cy);
	InvalidateRect(NULL);

	return;
}

//更新控件
void CPropertyBar::UpdateTopButton(INT nTopButtonHead)
{
	//获取大小
	CRect rcClient;
	GetWindowRect(rcClient);

	//设置图片
	CImageHandle ImageHandleAreaL(&m_ImageL);
	CImageHandle ImageHandleAreaLM(&m_ImageM);
	CImageHandle ImageHandleAreaLR(&m_ImageR);

	//获取大小
	const int nAreaLWidth = m_ImageL.GetWidth();
	const int nAreaMWidth = m_ImageM.GetWidth();
	const int nAreaRWidth = m_ImageR.GetWidth();

	//按钮个数
	const int nButtonCount = m_nButtonCount;

	//移动按钮
	HDWP hDwp = BeginDeferWindowPos(20);
	const UINT uFlags = SWP_NOSIZE | SWP_NOCOPYBITS | SWP_NOACTIVATE | SWP_NOZORDER;

	//按钮大小
	CRect rcButtonClient;
	m_BtProperty[0].GetWindowRect(rcButtonClient);

	//上边按钮
	for ( int nIndex =0, nButtonIndex = nTopButtonHead; nIndex < nButtonCount && nIndex < m_nPropertyCount && 
		nButtonIndex < CountArray(m_BtProperty) ; nButtonIndex++ )
	{
		if ( m_PropertyInfo[nButtonIndex].bNullity == true ) continue;

		DeferWindowPos(hDwp, m_BtProperty[nButtonIndex].GetSafeHwnd(), NULL,m_nXoriginal+ nIndex * (rcButtonClient.Width()+BUTTON_DISTANCE_X) + BUTTON_DISTANCE_X,
			TOP_BUTTON_Y, 0, 0, uFlags);
		++nIndex;
	}

	EndDeferWindowPos(hDwp);

	//隐藏按钮
	for ( INT nIndex = 0; nIndex < CountArray(m_BtProperty); ++nIndex ) 
		m_BtProperty[nIndex].ShowWindow(SW_HIDE);

	//可见设置
	for ( INT nIndex = nTopButtonHead, nShowButtonCount=0; nShowButtonCount < nButtonCount && nShowButtonCount < m_nPropertyCount && 
		nIndex < CountArray(m_BtProperty) ; ++nIndex )
	{
		if ( m_PropertyInfo[nIndex].bNullity == false )
		{
			m_BtProperty[nIndex].ShowWindow(SW_SHOW);
			nShowButtonCount++;
		}
	}

	return;
}

//更新控件
void CPropertyBar::UpdateBtmButton(INT nBtmButtonHead)
{
	//获取大小
	CRect rcClient;
	GetWindowRect(rcClient);

	//设置图片
	CImageHandle ImageHandleAreaL(&m_ImageL);
	CImageHandle ImageHandleAreaLM(&m_ImageM);
	CImageHandle ImageHandleAreaLR(&m_ImageR);

	//获取大小
	const int nAreaLWidth = m_ImageL.GetWidth();
	const int nAreaMWidth = m_ImageM.GetWidth();
	const int nAreaRWidth = m_ImageR.GetWidth();

	//按钮个数
	const int nButtonCount = m_nButtonCount;

	//移动按钮
	HDWP hDwp = BeginDeferWindowPos(20);
	const UINT uFlags = SWP_NOSIZE | SWP_NOCOPYBITS | SWP_NOACTIVATE | SWP_NOZORDER;

	//按钮大小
	CRect rcButtonClient;
	m_BtProperty[0].GetWindowRect(rcButtonClient);

	//下边按钮
	for ( int nIndex =0, nButtonIndex = nBtmButtonHead; nIndex < nButtonCount && nIndex < m_nFlowerCount && 
		nButtonIndex < CountArray(m_BtFlower) ; nButtonIndex++ )
	{		
		if ( m_FlowerInfo[nButtonIndex].bNullity == true ) continue;

		DeferWindowPos(hDwp, m_BtFlower[nButtonIndex].GetSafeHwnd(), NULL, m_nXoriginal+ nIndex * (rcButtonClient.Width()+BUTTON_DISTANCE_X) + BUTTON_DISTANCE_X,
			BTM_BUTTON_Y, 0, 0, uFlags);
		++nIndex;
	}

	EndDeferWindowPos(hDwp);

	//隐藏按钮
	for ( INT nIndex = 0; nIndex < CountArray(m_BtFlower); ++nIndex ) 
		m_BtFlower[nIndex].ShowWindow(SW_HIDE);

	//显示按钮
	for ( INT nIndex = nBtmButtonHead, nShowButtonCount=0; nShowButtonCount < nButtonCount && nShowButtonCount < m_nFlowerCount && 
		nIndex < CountArray(m_BtFlower) ; ++nIndex )
	{
		if ( m_FlowerInfo[nIndex].bNullity == false )
		{
			m_BtFlower[nIndex].ShowWindow(SW_SHOW);
			nShowButtonCount++;
		}
	}

	return;
}

//获取高度
int CPropertyBar::GetHeight()
{
	CImageHandle ImagePropertyLHandle(&m_ImageL);

	return m_ImageL.GetHeight();
}

//设置资源
void CPropertyBar::SetSkinResource(HINSTANCE ReshInstance,tagPropertyViewImage *pPropertyViewImage)
{
	//设置资源
	m_ReshInstance=ReshInstance;
	CopyMemory(&m_PropertyViewImage,pPropertyViewImage,sizeof(tagPropertyViewImage));

	return ;
}

//更新资源
bool CPropertyBar::UpdateSkinResource()
{
	//加载资源
	HINSTANCE hInstance = m_ReshInstance;
	m_ImageL.SetLoadInfo(m_PropertyViewImage.uPropertyFrameL, hInstance, false);
	m_ImageM.SetLoadInfo(m_PropertyViewImage.uPropertyFrameM, hInstance, false);
	m_ImageR.SetLoadInfo(m_PropertyViewImage.uPropertyFrameR, hInstance, false);

	//道具按钮
	for ( int nIndex = 0; nIndex < CountArray(m_BtProperty); ++nIndex ) 
		m_BtProperty[nIndex].SetButtonImage(m_PropertyViewImage.uBtProperty[nIndex], hInstance, false);

	//鲜花按钮
	for ( int nIndex = 0; nIndex < CountArray(m_BtFlower); ++nIndex )
		m_BtFlower[nIndex].SetButtonImage(m_PropertyViewImage.uBtGift[nIndex], hInstance, false);

	//控制按钮
	m_btTopMoveToL.SetButtonImage(m_PropertyViewImage.uBtPropertyToLeft, hInstance, false);
	m_btTopMoveToR.SetButtonImage(m_PropertyViewImage.uBtPropertyToRight, hInstance, false);
	m_btBtmMoveToL.SetButtonImage(m_PropertyViewImage.uBtPropertyToLeft, hInstance, false);
	m_btBtmMoveToR.SetButtonImage(m_PropertyViewImage.uBtPropertyToRight, hInstance, false);

	return true;
}

//设置组件
void CPropertyBar::SetPurchaseInfoSink(IPurchaseInfo *pIPurchaseInfo)
{
	m_pIPurchaseInfo = pIPurchaseInfo;

	return;
}

//消息过虑
BOOL CPropertyBar::PreTranslateMessage(MSG * pMsg)
{
	m_ToolTipCtrl.RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

//移动消息
void CPropertyBar::OnBnClickedTopMoveToR()
{
	if ( m_bInitDialog == false ) return ;

	//设置变量
	m_nTopButtonHead--;

	//设置按钮
	if ( m_nTopButtonHead == 0 ) m_btTopMoveToR.EnableWindow(FALSE);
	m_btTopMoveToL.EnableWindow(TRUE);

	//更新控件
	UpdateTopButton(m_nTopButtonHead);

	return;
}

//移动消息
void CPropertyBar::OnBnClickedTopMoveToL()
{
	if ( m_bInitDialog == false ) return ;

	//设置变量
	m_nTopButtonHead++;

	//设置按钮
	m_btTopMoveToR.EnableWindow(TRUE);
	int nLeftButtonCount = 0;
	for ( int nIndex = 0; nIndex < m_nBtmButtonHead; ++ nIndex ) if ( m_FlowerInfo[nIndex].bNullity == false ) nLeftButtonCount++;
	if ( m_nPropertyCount - m_nTopButtonHead <= m_nButtonCount ) m_btTopMoveToL.EnableWindow(FALSE);
	else m_btTopMoveToL.EnableWindow(TRUE);

	//更新控件
	UpdateTopButton(m_nTopButtonHead);

	return;
}


//移动消息
void CPropertyBar::OnBnClickedBtmMoveToL()
{
	if ( m_bInitDialog == false ) return ;

	//设置变量
	m_nBtmButtonHead++;
	
	//设置按钮
	m_btBtmMoveToR.EnableWindow(TRUE);
	int nLeftButtonCount = 0;
	for ( int nIndex = 0; nIndex < m_nBtmButtonHead; ++ nIndex ) if ( m_FlowerInfo[nIndex].bNullity == false ) nLeftButtonCount++;
	if ( m_nFlowerCount - nLeftButtonCount <= m_nButtonCount ) m_btBtmMoveToL.EnableWindow(FALSE);
	else m_btBtmMoveToL.EnableWindow(TRUE);

	//更新控件
	UpdateBtmButton(m_nBtmButtonHead);

	return;
}

//移动消息
void CPropertyBar::OnBnClickedBtmMoveToR()
{
	if ( m_bInitDialog == false ) return ;

	//设置变量
	m_nBtmButtonHead--;

	//设置按钮
	if ( m_nBtmButtonHead == 0 ) m_btBtmMoveToR.EnableWindow(FALSE);
	m_btBtmMoveToL.EnableWindow(TRUE);

	//更新控件
	UpdateBtmButton(m_nBtmButtonHead);

	return;
}

//道具事件
void CPropertyBar::PropertyEvent(int nPropertyID)
{
	//创建判断
	if ( m_pPropertyItem->m_hWnd == NULL )
	{
		ASSERT(m_pIPurchaseInfo != NULL);
		m_pPropertyItem->SetPurchaseInfoSink(m_pIPurchaseInfo);
		m_pPropertyItem->Create(IDD_PROPERTY_ITEM, this);
	}

	//设置子项
	m_pPropertyItem->SetPropertyID(nPropertyID);
	m_pPropertyItem->CenterWindow();
	m_pPropertyItem->ShowWindow(SW_SHOW);
	m_pPropertyItem->BringWindowToTop();

	return;
}

//鲜花道具
void CPropertyBar::FlowerEvent(UINT nFlowerID)
{
	//创建判断
	if ( m_pFlowerSetting->m_hWnd == NULL )
	{
		ASSERT(m_pIPurchaseInfo != NULL);
		m_pFlowerSetting->SetPurchaseInfoSink(m_pIPurchaseInfo);
		m_pFlowerSetting->Create(IDD_FLOWER_SETTING, this);
		
	}

	//设置子项
	m_pFlowerSetting->SetFlowerID(nFlowerID);
	m_pFlowerSetting->CenterWindow();
	m_pFlowerSetting->ShowWindow(SW_SHOW);
	m_pFlowerSetting->BringWindowToTop();

	return;
}

//创建函数
void CPropertyBar::CreateBar(CWnd *pParent)
{
	if ( m_hWnd == NULL )
	{
		Create(IDD_PROPERTY_BAR, pParent);
	}
	return;
}

//销毁函数
void CPropertyBar::DestroyBar()
{
	if ( m_hWnd != NULL ) DestroyWindow();
	return;
}

//初始函数
void CPropertyBar::SetPropertyInfo(int nPropID, tagPropertyInfo const & PropertyInfo)
{
	//参数验证
	ASSERT(nPropID < PROPERTY_COUNT);
	if ( nPropID >= PROPERTY_COUNT ) return;

    //设置元素
	m_PropertyInfo[nPropID] = PropertyInfo;
	m_PropertyInfo[nPropID].bNullity = false;
	m_nPropertyCount++;

	//调整位置
	CRect rcClient;
	GetClientRect(rcClient);
	RectifyControl(rcClient.Width(), rcClient.Height());

	return;
}

//初始函数
void CPropertyBar::SetFlowerInfo (int nFlowerID, tagFlowerInfo const & FlowerInfo)
{
	//参数验证
	ASSERT(nFlowerID < FLOWER_COUNT);
	if ( nFlowerID >= FLOWER_COUNT ) return;

	//设置元素
	m_FlowerInfo[nFlowerID] = FlowerInfo;
	m_FlowerInfo[nFlowerID].bNullity = false;
	m_nFlowerCount++;

	//调整位置
	CRect rcClient;
	GetClientRect(rcClient);
	RectifyControl(rcClient.Width(), rcClient.Height());

	return;
}

//选择用户
void CPropertyBar::SetUserDataInfo(tagUserData *pUserData)
{
	if(m_pFlowerSetting->m_hWnd != NULL && m_pFlowerSetting->IsWindowVisible()==TRUE)
	{
		m_pFlowerSetting->SetUserDataInfo(pUserData);
	}

	if(m_pPropertyItem->m_hWnd != NULL && m_pPropertyItem->IsWindowVisible()==TRUE)
	{
		m_pPropertyItem->SetUserDataInfo(pUserData);
	}

	return;
}

//命令信息
BOOL CPropertyBar::OnCommand(WPARAM wParam, LPARAM lParam)
{
	//辅助处理
	struct DealWithList
	{
		WORD wCommandID;
		UINT uFunctionPar;
	}CommandList[]=
	{
		IDC_BT_FLOWER1,		FLOWER_1,
		IDC_BT_FLOWER2,		FLOWER_2,
		IDC_BT_FLOWER3,		FLOWER_3,
		IDC_BT_FLOWER4,		FLOWER_4,
		IDC_BT_FLOWER5,		FLOWER_5,
		IDC_BT_FLOWER6,		FLOWER_6,
		IDC_BT_FLOWER7,		FLOWER_7,
		IDC_BT_FLOWER8,		FLOWER_8,
		IDC_BT_FLOWER9,		FLOWER_9,
		IDC_BT_FLOWER10,	FLOWER_10,
		IDC_BT_FLOWER11,	FLOWER_11,

		IDC_BT_PROPERTY1,	PROP_DOUBLE,
		IDC_BT_PROPERTY2,	PROP_FOURDOLD,
		IDC_BT_PROPERTY3,	PROP_NEGAGIVE,
		IDC_BT_PROPERTY4,	PROP_FLEE,
		IDC_BT_PROPERTY5,	PROP_BUGLE,
		IDC_BT_PROPERTY6,	PROP_KICK,
		IDC_BT_PROPERTY7,	PROP_SHIELD,
		IDC_BT_PROPERTY8,	PROP_MEMBER_1,
		IDC_BT_PROPERTY9,	PROP_MEMBER_2,
		IDC_BT_PROPERTY10,	PROP_MEMBER_3,
		IDC_BT_PROPERTY11,	PROP_MEMBER_4,
		IDC_BT_PROPERTY12,	PROP_MEMBER_5,
		IDC_BT_PROPERTY13,	PROP_MEMBER_6,
		IDC_BT_PROPERTY14,	PROP_MEMBER_7,
		IDC_BT_PROPERTY15,	PROP_MEMBER_7,
	};

	//查找信息
	WORD wContorlID = LOWORD(wParam);
	for(WORD i=0;i<CountArray(CommandList);i++)
	{
		if(wContorlID!=CommandList[i].wCommandID)continue;

		//处理信息
		if(i<FLOWER_COUNT)FlowerEvent(CommandList[i].uFunctionPar);
		else PropertyEvent(CommandList[i].uFunctionPar);

		return TRUE;
	}

	return CDialog::OnCommand(wParam, lParam);
}

//////////////////////////////////////////////////////////////////////