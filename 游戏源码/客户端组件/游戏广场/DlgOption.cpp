#include "Stdafx.h"
#include "DlgOption.h"
#include "GameFrame.h"
#include "GlobalUnits.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

#define ITEM_WIDTH							117							//按钮宽度
#define ITEM_HEIGHT							23							//按钮高度
#define DLG_TITLE_HEIGHT					30							//标题高度
#define BUTTON_BAR_WIDTH					117							//按钮宽度
#define BUTTON_BAR_HEIGHT					290							//按钮高度
#define TITLE_GROUND_COLOR					RGB(79,151,199)				//背景颜色

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CButtonBar, CWnd)
	ON_WM_PAINT()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgOptionItem, CDialog)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgOption, CSkinDialogEx)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgEnterTablePass, CSkinDialogEx)
END_MESSAGE_MAP()
//////////////////////////////////////////////////////////////////////////

//构造函数
CButtonBar::CButtonBar()
{
	//设置变量
	m_nHotItem=-1;
	m_nActiveItem=-1;
	m_pItemParent=NULL;

	return;
}

//析构函数
CButtonBar::~CButtonBar()
{
}

//消息解释
BOOL CButtonBar::PreTranslateMessage(MSG * pMsg)
{
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
	{
        GetParent()->PostMessage(WM_CLOSE,0,0);
		return TRUE;
	}
	return __super::PreTranslateMessage(pMsg);
}

//设置信息
bool CButtonBar::InitButtonBar(const CRect & rcItemRect, CWnd * pItemParent)
{
	//设置变量
	m_rcItemRect=rcItemRect;
	m_pItemParent=pItemParent;

	//移动控件
	for (INT_PTR i=0;i<m_OptionsItemPtr.GetCount();i++)
	{
		CDlgOptionItem * pDlgOptionItem=m_OptionsItemPtr[i];
		if (pDlgOptionItem->m_hWnd!=NULL) pDlgOptionItem->MoveWindow(&rcItemRect);
	}

	//更新资源
	UpdateSkinResource();

	return true;
}

//激活设置
bool CButtonBar::SetActiveOptionItem(CDlgOptionItem * pDlgOptionItem)
{
	//判断子项
	CDlgOptionItem * pActiveItem=NULL;
	if (m_nActiveItem!=-1)
	{
		ASSERT(m_nActiveItem<m_OptionsItemPtr.GetCount());
		pActiveItem=m_OptionsItemPtr[m_nActiveItem];
		if (pActiveItem==pDlgOptionItem) return true;
		if (pActiveItem->SaveOptions()==false) return false;
	}

	//激活子项
	for (INT_PTR i=0;i<m_OptionsItemPtr.GetCount();i++)
	{
		if (m_OptionsItemPtr[i]==pDlgOptionItem)
		{
			//创建新项
			if (pDlgOptionItem->m_hWnd==NULL)
			{
				pDlgOptionItem->CreateOptionsItem(m_pItemParent);
				pDlgOptionItem->MoveWindow(&m_rcItemRect,FALSE);
			}

			//显示新项
			m_nActiveItem=i;
			pDlgOptionItem->ShowWindow(SW_SHOW);
			pDlgOptionItem->SetFocus();

			//隐藏旧项
			if ((pActiveItem!=NULL)&&(pActiveItem->m_hWnd!=NULL)) pActiveItem->ShowWindow(SW_HIDE);

			//刷新界面
			Invalidate(FALSE);

			return true;
		}
	}

	return false;
}

//加入设置
bool CButtonBar::InsertOptionItem(CDlgOptionItem * pDlgOptionItem)
{
	//插入子项
	INT_PTR nIndex=m_OptionsItemPtr.Add(pDlgOptionItem);
	if (m_nActiveItem==-1) SetActiveOptionItem(pDlgOptionItem);

	//更新界面
	Invalidate(FALSE);

	return true;
}

//更新资源
bool CButtonBar::UpdateSkinResource()
{
	//加载资源
	tagControlViewImage & ControlViewImage=g_GlobalUnits.m_ControlViewImage;
	HINSTANCE hResInstance=g_GlobalUnits.m_PlatformResourceModule->GetResInstance();

	m_ImageHot.SetLoadInfo(ControlViewImage.uCollocateBarHot,hResInstance);
	m_ImageActive.SetLoadInfo(ControlViewImage.uCollocateBarActive,hResInstance);
	m_ImageNormal.SetLoadInfo(ControlViewImage.uCollocateBarNormal,hResInstance);
	m_ImageBackGround.SetLoadInfo(ControlViewImage.uCollocateBarBackground,hResInstance);

	return true;
}

//按键测试
INT_PTR CButtonBar::ItemHitTest(CPoint MousePoint)
{
	//获取位置
	CRect rcRect;
	GetClientRect(&rcRect);

	//按键测试
	if ((MousePoint.x>2)&&(MousePoint.x<(rcRect.Width()-2))&&
		(MousePoint.y>10)&&((MousePoint.y-10)%(ITEM_HEIGHT+2)<ITEM_HEIGHT))
	{
		INT_PTR nIndex=(MousePoint.y-10)/(ITEM_HEIGHT+2);
		if (nIndex<m_OptionsItemPtr.GetCount()) return nIndex;
	}

	return -1;
}

//重画消息
void CButtonBar::OnPaint()
{
	CPaintDC dc(this);

	//获取位置
	CRect rcRect;
	GetClientRect(&rcRect);

	//加载资源
	CImageHandle ImageHotHandle(&m_ImageHot);
	CImageHandle ImageActiveHandle(&m_ImageActive);
	CImageHandle ImageNormalHandle(&m_ImageNormal);
	CImageHandle m_ImageBackGroundHandle(&m_ImageBackGround);

	//建立缓冲
	CDC BufferDC;
	CBitmap BufferBmp;
	BufferDC.CreateCompatibleDC(&dc);
	BufferBmp.CreateCompatibleBitmap(&dc,rcRect.Width(),rcRect.Height());
	BufferDC.SelectObject(&BufferBmp);

	//绘画背景
	CBrush BackBrush(TITLE_GROUND_COLOR);
	CPen BorderPen(PS_SOLID,1,CSkinWndObject::m_SkinAttribute.m_crInsideBorder);
	CPen * pOldPen=BufferDC.SelectObject(&BorderPen);
	CBrush * pOldBrush=BufferDC.SelectObject(&BackBrush);
	BufferDC.FillSolidRect(&rcRect,CSkinWndObject::m_SkinAttribute.m_crBackGround);
	BufferDC.RoundRect(&rcRect,CPoint(10,10));
	m_ImageBackGround.BitBlt(BufferDC,1,rcRect.Height()-m_ImageBackGround.GetHeight()-3);
	BufferDC.SelectObject(pOldPen);
	BufferDC.SelectObject(pOldBrush);
	BackBrush.DeleteObject();
	BorderPen.DeleteObject();

	//绘画按钮
	BufferDC.SetBkMode(TRANSPARENT);
	BufferDC.SelectObject(CSkinResourceManager::GetDefaultFont());

	//绘画按钮
	CRect rcButton;
	CSkinImage * pSkinImage=NULL;
	for (INT_PTR i=0;i<m_OptionsItemPtr.GetCount();i++)
	{
		LPCTSTR pszButtonText=m_OptionsItemPtr[i]->GetButtonText();
		rcButton.SetRect(1,(int)(ITEM_HEIGHT*i+10+2*i),rcRect.Width(),(int)(ITEM_HEIGHT*(i+1)+10+2*i));
		if (i==m_nActiveItem) 
		{
			pSkinImage=&m_ImageActive;
			BufferDC.SetTextColor(RGB(0,0,0));
		}
		else if (i==m_nHotItem) 
		{
			pSkinImage=&m_ImageHot;
			BufferDC.SetTextColor(RGB(0,0,0));
		}
		else 
		{
			pSkinImage=&m_ImageNormal;
			BufferDC.SetTextColor(RGB(0,0,0));
		}
		pSkinImage->BitBlt(BufferDC,rcButton.left,rcButton.top,ITEM_WIDTH,ITEM_HEIGHT,0,0,SRCCOPY);
		BufferDC.DrawText(pszButtonText,lstrlen(pszButtonText),rcButton,DT_END_ELLIPSIS|DT_VCENTER|DT_CENTER|DT_SINGLELINE);
	}

	//绘画界面
	dc.BitBlt(0,0,rcRect.Width(),rcRect.Height(),&BufferDC,0,0,SRCCOPY);

	//清理资源
	BufferDC.DeleteDC();
	BufferBmp.DeleteObject();

	return;
}

//鼠标消息
void CButtonBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	if ((m_nHotItem>=0)&&(m_nHotItem!=m_nActiveItem)) 
	{
		SetActiveOptionItem(m_OptionsItemPtr[m_nHotItem]);
	}
	__super::OnLButtonDown(nFlags,point);
}

//光标消息
BOOL CButtonBar::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT message)
{
	//获取位置
	CPoint MousePoint;
	GetCursorPos(&MousePoint);
	ScreenToClient(&MousePoint);

	//按键测试
	INT_PTR nOldHotItem=m_nHotItem;
	m_nHotItem=ItemHitTest(MousePoint);
	if (nOldHotItem!=m_nHotItem) Invalidate(FALSE);
	if (m_nHotItem>=0)
	{
		SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_HAND_CUR)));
		return TRUE;
	}

	return __super::OnSetCursor(pWnd,nHitTest,message);
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgOptionItem::CDlgOptionItem(UINT uIDTemplate) : CDialog(uIDTemplate), m_uIDTemplate(uIDTemplate)
{
	m_bCreate=false;
}

//析构函数
CDlgOptionItem::~CDlgOptionItem()
{
}

//创建函数
bool CDlgOptionItem::CreateOptionsItem(CWnd * pParentWmd)
{
	m_bCreate=true;
	Create(m_uIDTemplate,pParentWmd);
	return true;
}

//是否选中
bool CDlgOptionItem::IsButtonSelected(UINT uButtonID)
{
	return ((CButton *)GetDlgItem(uButtonID))->GetCheck()==BST_CHECKED;
}

//选中按钮
void CDlgOptionItem::SetButtonSelected(UINT uButtonID, bool bSelected)
{
	CButton * pButton=(CButton *)GetDlgItem(uButtonID);
	if (bSelected) pButton->SetCheck(BST_CHECKED);
	else pButton->SetCheck(BST_UNCHECKED);
	return;
}

//重画函数
void CDlgOptionItem::OnPaint()
{
	CPaintDC dc(this);

	//获取位置
	CRect rcRect;
	GetClientRect(&rcRect);

	//建立缓冲
	CDC BufferDC;
	CBitmap BufferBmp;
	BufferDC.CreateCompatibleDC(&dc);
	BufferBmp.CreateCompatibleBitmap(&dc,rcRect.Width(),rcRect.Height());
	BufferDC.SelectObject(&BufferBmp);

	//绘画背景
	COLORREF crBackGround=CSkinWndObject::m_SkinAttribute.m_crBackGround;
	BufferDC.FillSolidRect(0,0,rcRect.Width(),DLG_TITLE_HEIGHT,crBackGround);

	//绘画上框
	CBrush BackBrush(TITLE_GROUND_COLOR);
	CPen LinePen(PS_SOLID,1,CSkinWndObject::m_SkinAttribute.m_crInsideBorder);
	BufferDC.SelectObject(&LinePen);
	BufferDC.SelectObject(&BackBrush);
	BufferDC.RoundRect(0,0,rcRect.Width(),DLG_TITLE_HEIGHT+10,10,10);

	//绘画下框
	BufferDC.FillSolidRect(0,DLG_TITLE_HEIGHT,rcRect.Width(),rcRect.Height()-DLG_TITLE_HEIGHT,crBackGround);
	BufferDC.SelectObject(GetStockObject(NULL_BRUSH));
	BufferDC.RoundRect(0,0,rcRect.Width(),rcRect.Height(),10,10);

	//绘画标题
	LPCTSTR pszTitle=GetTitleText();
	CRect rcTitil(10,0,rcRect.Width()-20,DLG_TITLE_HEIGHT);
	BufferDC.SetBkMode(TRANSPARENT);
	BufferDC.SetTextColor(RGB(10,10,10));
	BufferDC.SelectObject(CSkinResourceManager::GetDefaultFont());
	BufferDC.DrawText(pszTitle,lstrlen(pszTitle),rcTitil,DT_END_ELLIPSIS|DT_VCENTER|DT_SINGLELINE);

	//绘画界面
	dc.BitBlt(0,0,rcRect.Width(),rcRect.Height(),&BufferDC,0,0,SRCCOPY);

	//清理资源
	BufferDC.DeleteDC();
	LinePen.DeleteObject();
	BackBrush.DeleteObject();
	BufferBmp.DeleteObject();

	return;
}

//背景消息
BOOL CDlgOptionItem::OnEraseBkgnd(CDC * pDC)
{
	Invalidate(FALSE);
	UpdateWindow();
	return TRUE;
}

//颜色消息
HBRUSH CDlgOptionItem::OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor)
{
	switch (nCtlColor)
	{
	case CTLCOLOR_DLG:
	case CTLCOLOR_STATIC:
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetBkColor(CSkinDialog::m_SkinAttribute.m_crBackGround);
			pDC->SetTextColor(CSkinDialog::m_SkinAttribute.m_crControlTXColor);
			return CSkinDialog::m_SkinAttribute.m_brBackGround;
		}
	}
	return __super::OnCtlColor(pDC,pWnd,nCtlColor);
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgOptionSystem::CDlgOptionSystem() : CDlgOptionItem(IDD_OPTION_SYSTEM)
{
	memset(&m_SystemParameter,0,sizeof(m_SystemParameter));
	return;
}

//析构函数
CDlgOptionSystem::~CDlgOptionSystem()
{
}

//初始化函数
BOOL CDlgOptionSystem::OnInitDialog()
{
	__super::OnInitDialog();

	//帐户规则
	if (m_SystemParameter.enAcountsRule==enAcountsRule_AccountsAndPass)
	{
		SetButtonSelected(IDC_SAVE_ACCOUNTS_AND_PASS,true);
	}
	else if (m_SystemParameter.enAcountsRule==enAcountsRule_Accounts)
	{
		SetButtonSelected(IDC_SAVE_ACCOUNTS,true);
	}
	else SetButtonSelected(IDC_SAVE_NONE,true);

	//邀请模式
	if (m_SystemParameter.enInviteMode==enInviteMode_All)
	{
		SetButtonSelected(IDC_RADIO1,true);
	}
	else if (m_SystemParameter.enInviteMode==enInviteMode_Friend)
	{
		SetButtonSelected(IDC_RADIO2,true);
	}
	else SetButtonSelected(IDC_RADIO3,true);

	//进出消息
	SetButtonSelected(IDC_SHOW_INOUT,m_SystemParameter.bShowInOutMessage);

	//老板键
	((CHotKeyCtrl *)GetDlgItem(IDC_BOSS_HOTKEY))->SetHotKey(LOBYTE(m_SystemParameter.wBossHotKey),
		HIBYTE(m_SystemParameter.wBossHotKey));

	return TRUE;
}

//保存设置
bool CDlgOptionSystem::SaveOptions()
{
	//效验状态
	if (m_hWnd==NULL) return true;

	//帐户规则
	if (IsButtonSelected(IDC_SAVE_ACCOUNTS_AND_PASS)) 
	{
		m_SystemParameter.enAcountsRule=enAcountsRule_AccountsAndPass;
	}
	else if (IsButtonSelected(IDC_SAVE_ACCOUNTS)) 
	{
		m_SystemParameter.enAcountsRule=enAcountsRule_Accounts;
	}
	else m_SystemParameter.enAcountsRule=enAcountsRule_None;

	//邀请模式
	if (IsButtonSelected(IDC_RADIO1)) 
	{
		m_SystemParameter.enInviteMode=enInviteMode_All;
	}
	else if (IsButtonSelected(IDC_RADIO2)) 
	{
		m_SystemParameter.enInviteMode=enInviteMode_Friend;
	}
	else m_SystemParameter.enInviteMode=enInviteMode_None;

	//进出消息
	m_SystemParameter.bShowInOutMessage=IsButtonSelected(IDC_SHOW_INOUT);

	//老板键
	m_SystemParameter.wBossHotKey=(WORD)((CHotKeyCtrl *)GetDlgItem(IDC_BOSS_HOTKEY))->GetHotKey();

	return true;
}

//获取标题
LPCTSTR CDlgOptionSystem::GetTitleText()
{
	return TEXT("系统配置选项");
}

//获取标题
LPCTSTR CDlgOptionSystem::GetButtonText()
{
	return TEXT("系统配置");
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgOptionTableRule::CDlgOptionTableRule() : CDlgOptionItem(IDD_OPTION_TABLE_RULE)
{
	memset(&m_TableRuleParameter,0,sizeof(m_TableRuleParameter));
	return;
}

//析构函数
CDlgOptionTableRule::~CDlgOptionTableRule()
{
}

//初始化函数
BOOL CDlgOptionTableRule::OnInitDialog()
{
	__super::OnInitDialog();

	//限制输入
	((CEdit *)GetDlgItem(IDC_WIN_RATE))->LimitText(5);
	((CEdit *)GetDlgItem(IDC_FLEE_RATE))->LimitText(5);
	((CEdit *)GetDlgItem(IDC_SCORE_LESS))->LimitText(11);
	((CEdit *)GetDlgItem(IDC_SCORE_MAX))->LimitText(10);
	((CEdit *)GetDlgItem(IDC_TABLE_PASSWORD))->LimitText(PASS_LEN-1);

	//设置控件
	TCHAR szBuffer[128]=TEXT("");
	SetButtonSelected(IDC_LIMIT_WIN_RATE,m_TableRuleParameter.bLimitWin);
	SetButtonSelected(IDC_LIMIT_FLEE_RATE,m_TableRuleParameter.bLimitFlee);
	SetButtonSelected(IDC_LIMIT_SCORE,m_TableRuleParameter.bLimitScore);
	SetButtonSelected(IDC_LIMIT_DETEST,m_TableRuleParameter.bLimitDetest);
	SetButtonSelected(IDC_LIMIT_IP,m_TableRuleParameter.bCheckSameIP);
	SetButtonSelected(IDC_BRING_TABLE_PASSWORD,m_TableRuleParameter.bPassword);

	//最低胜率
	_snprintf(szBuffer,sizeof(szBuffer),TEXT("%.2f"),((double)(m_TableRuleParameter.wWinRate))/100.0);
	SetDlgItemText(IDC_WIN_RATE,szBuffer);

	//最高逃率
	_snprintf(szBuffer,sizeof(szBuffer),TEXT("%.2f"),((double)(m_TableRuleParameter.wFleeRate))/100.0);
	SetDlgItemText(IDC_FLEE_RATE,szBuffer);

	//积分限制
	SetDlgItemInt(IDC_SCORE_MAX,m_TableRuleParameter.lMaxScore,TRUE);
	SetDlgItemInt(IDC_SCORE_LESS,m_TableRuleParameter.lLessScore,TRUE);

	//携带密码
	SetDlgItemText(IDC_TABLE_PASSWORD,m_TableRuleParameter.szPassword);

	return TRUE;
}

//保存设置
bool CDlgOptionTableRule::SaveOptions()
{
	//效验状态
	if (m_hWnd==NULL) return true;

	//获取参数
	m_TableRuleParameter.bLimitWin=IsButtonSelected(IDC_LIMIT_WIN_RATE);
	m_TableRuleParameter.bLimitFlee=IsButtonSelected(IDC_LIMIT_FLEE_RATE);
	m_TableRuleParameter.bLimitScore=IsButtonSelected(IDC_LIMIT_SCORE);
	m_TableRuleParameter.bLimitDetest=IsButtonSelected(IDC_LIMIT_DETEST);
	m_TableRuleParameter.bCheckSameIP=IsButtonSelected(IDC_LIMIT_IP);
	m_TableRuleParameter.bPassword=IsButtonSelected(IDC_BRING_TABLE_PASSWORD);

	//最低胜率
	TCHAR szBuffer[128]=TEXT("");
	GetDlgItemText(IDC_WIN_RATE,szBuffer,CountArray(szBuffer));
	m_TableRuleParameter.wWinRate=(WORD)(atof(szBuffer)*100.0)%10000;

	//最高逃率
	GetDlgItemText(IDC_FLEE_RATE,szBuffer,CountArray(szBuffer));
	m_TableRuleParameter.wFleeRate=(WORD)(atof(szBuffer)*100.0)%10000;

	//积分限制
	m_TableRuleParameter.lMaxScore=GetDlgItemInt(IDC_SCORE_MAX,NULL,TRUE);
	m_TableRuleParameter.lLessScore=GetDlgItemInt(IDC_SCORE_LESS,NULL,TRUE);
	if ((m_TableRuleParameter.bLimitScore)&&(m_TableRuleParameter.lLessScore>=m_TableRuleParameter.lMaxScore))
	{
		ShowInformation(TEXT("积分限制范围参数不正确，请正确设置积分限制范围！"),0,MB_ICONINFORMATION);
		GetDlgItem(IDC_SCORE_MAX)->SetFocus();
		return false;
	}

	//携带密码
	GetDlgItemText(IDC_TABLE_PASSWORD,m_TableRuleParameter.szPassword,CountArray(m_TableRuleParameter.szPassword));
	if ((m_TableRuleParameter.bPassword==true)&&(m_TableRuleParameter.szPassword[0]==0))
	{
		ShowInformation(TEXT("桌子携带密码没有设置，请输入设置桌子携带密码！"),0,MB_ICONINFORMATION);
		GetDlgItem(IDC_TABLE_PASSWORD)->SetFocus();
		return false;
	}

	return true;
}

//获取标题
LPCTSTR CDlgOptionTableRule::GetTitleText()
{
	return TEXT("桌子规则选项");
}

//获取标题
LPCTSTR CDlgOptionTableRule::GetButtonText()
{
	return TEXT("桌子规则");
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgOption::CDlgOption(CGameOption * pGameOption, CServerOption * pServerOption) : CSkinDialogEx(IDD_OPTION)
{
	//设置变量
	m_pGameOption=pGameOption;
	m_pServerOption=pServerOption;

	return;
}

//析构函数
CDlgOption::~CDlgOption()
{
}

//控件绑定
void CDlgOption::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btOk);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
}

//消息解释
BOOL CDlgOption::PreTranslateMessage(MSG * pMsg)
{
	return __super::PreTranslateMessage(pMsg);
}

//初始化函数
BOOL CDlgOption::OnInitDialog()
{
	__super::OnInitDialog();

	//获取位置
	CRect rcClient,rcFrame;
	GetClientRect(&rcClient);
	rcFrame.left=BUTTON_BAR_WIDTH+15;
	rcFrame.top=GetYExcursionPos()+8;
	rcFrame.right=rcClient.Width()-10;
	rcFrame.bottom=rcClient.Height()-35;

	//创建控件
	m_ButtonBar.Create(NULL,NULL,WS_CHILD,CRect(0,0,0,0),this,10);
	m_ButtonBar.MoveWindow(8,8+GetYExcursionPos(),BUTTON_BAR_WIDTH+2,BUTTON_BAR_HEIGHT);
	m_ButtonBar.ShowWindow(SW_SHOW);
	m_ButtonBar.InitButtonBar(rcFrame,this);

	//设置标题
	SetWindowText(TEXT("系统配置"));

	//系统配置
	tagSystemParameter * pSystemParameter=&m_SystemItem.m_SystemParameter;
	pSystemParameter->enAcountsRule=g_GlobalOption.m_enAcountsRule;
	pSystemParameter->enInviteMode=g_GlobalOption.m_InviteMode;
	pSystemParameter->bShowInOutMessage=g_GlobalOption.m_bShowInOutMessage;
	pSystemParameter->wBossHotKey=g_GlobalOption.m_wBossHotKey;
	m_ButtonBar.InsertOptionItem(&m_SystemItem);

	//同桌配置
	if ((m_pGameOption!=NULL)&&(m_pServerOption!=NULL))
	{
		tagTableRuleParameter * pTableRuleParameter=&m_TableRuleItem.m_TableRuleParameter;
		pTableRuleParameter->bPassword=m_pServerOption->m_bPassword;
		pTableRuleParameter->bLimitWin=m_pGameOption->m_bLimitWin;
		pTableRuleParameter->bLimitFlee=m_pGameOption->m_bLimitFlee;
		pTableRuleParameter->bLimitScore=m_pGameOption->m_bLimitScore;
		pTableRuleParameter->bLimitDetest=g_GlobalOption.m_bLimitDetest;
		pTableRuleParameter->bCheckSameIP=g_GlobalOption.m_bCheckSameIP;
		pTableRuleParameter->wWinRate=m_pGameOption->m_wWinRate;
		pTableRuleParameter->wFleeRate=m_pGameOption->m_wFleeRate;
		pTableRuleParameter->lMaxScore=m_pGameOption->m_lMaxScore;
		pTableRuleParameter->lLessScore=m_pGameOption->m_lLessScore;
		lstrcpyn(pTableRuleParameter->szPassword,m_pServerOption->m_szPassword,CountArray(pTableRuleParameter->szPassword));
		m_ButtonBar.InsertOptionItem(&m_TableRuleItem);
	}

	return TRUE;
}

//确定函数
void CDlgOption::OnOK()
{
	//保存参数
	if (m_SystemItem.SaveOptions()==false) return;
	if (m_TableRuleItem.SaveOptions()==false) return;

	//系统配置
	tagSystemParameter * pSystemParameter=&m_SystemItem.m_SystemParameter;
	g_GlobalOption.m_enAcountsRule=pSystemParameter->enAcountsRule;
	g_GlobalOption.m_InviteMode=pSystemParameter->enInviteMode;
	g_GlobalOption.m_bShowInOutMessage=pSystemParameter->bShowInOutMessage;
	g_GlobalOption.m_wBossHotKey=pSystemParameter->wBossHotKey;

	//老板热键
	UnregisterHotKey(AfxGetMainWnd()->GetSafeHwnd(),IDI_HOT_KEY_BOSS);
	g_GlobalUnits.RegisterHotKey(AfxGetMainWnd()->GetSafeHwnd(),IDI_HOT_KEY_BOSS,g_GlobalOption.m_wBossHotKey);

	//同桌配置
	if ((m_pGameOption!=NULL)&&(m_pServerOption!=NULL))
	{
		tagTableRuleParameter * pTableRuleParameter=&m_TableRuleItem.m_TableRuleParameter;
		m_pServerOption->m_bPassword=pTableRuleParameter->bPassword;
		m_pGameOption->m_bLimitWin=pTableRuleParameter->bLimitWin;
		m_pGameOption->m_bLimitFlee=pTableRuleParameter->bLimitFlee;
		m_pGameOption->m_bLimitScore=pTableRuleParameter->bLimitScore;
		g_GlobalOption.m_bLimitDetest=pTableRuleParameter->bLimitDetest;
		g_GlobalOption.m_bCheckSameIP=pTableRuleParameter->bCheckSameIP;
		m_pGameOption->m_wWinRate=pTableRuleParameter->wWinRate;
		m_pGameOption->m_wFleeRate=pTableRuleParameter->wFleeRate;
		m_pGameOption->m_lMaxScore=pTableRuleParameter->lMaxScore;
		m_pGameOption->m_lLessScore=pTableRuleParameter->lLessScore;
		lstrcpyn(m_pServerOption->m_szPassword,pTableRuleParameter->szPassword,CountArray(m_pServerOption->m_szPassword));
	}

	__super::OnOK();
}

//取消消息
void CDlgOption::OnCancel()
{
	__super::OnCancel();
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgEnterTablePass::CDlgEnterTablePass() : CSkinDialogEx(IDD_ENTER_TABLE_PASS)
{
	//设置变量
	m_lpPassword=NULL;
	m_cbPasswordLen=0;

	return;
}

//析构函数
CDlgEnterTablePass::~CDlgEnterTablePass()
{
}

//控件绑定
void CDlgEnterTablePass::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btOk);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
}

//消息解释
BOOL CDlgEnterTablePass::PreTranslateMessage(MSG * pMsg)
{
	return __super::PreTranslateMessage(pMsg);
}

//初始化函数
BOOL CDlgEnterTablePass::OnInitDialog()
{
	__super::OnInitDialog();

	((CEdit *)GetDlgItem(IDC_ENTER_TABLE_PASSWORD))->LimitText(PASS_LEN-1);

	//MoveWindow(0,0,100,100);

	//设置标题
	SetWindowText(TEXT("设置密码"));
	//ShowWindow(SW_SHOW);

	return TRUE;
}

//确定函数
void CDlgEnterTablePass::OnOK()
{
	ASSERT(m_lpPassword!=NULL && m_cbPasswordLen>0);

	//携带密码
	GetDlgItemText(IDC_ENTER_TABLE_PASSWORD,m_lpPassword,m_cbPasswordLen);
	if (m_lpPassword[0]==0)
	{
		ShowInformation(TEXT("请输入桌子密码！"),0,MB_ICONINFORMATION);
		GetDlgItem(IDC_ENTER_TABLE_PASSWORD)->SetFocus();
		return ;
	}

	__super::OnOK();
}

//取消消息
void CDlgEnterTablePass::OnCancel()
{
	__super::OnCancel();
}

//设置变量
void CDlgEnterTablePass::SetPassword(LPTSTR lpPassword,BYTE cbPasswordLen)
{
	m_lpPassword = lpPassword;
	m_cbPasswordLen=cbPasswordLen;

	return ;
}

//////////////////////////////////////////////////////////////////////////

