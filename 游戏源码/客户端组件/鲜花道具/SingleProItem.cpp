#include "Stdafx.h"
#include "resource.h"
#include "SingleProItem.h"
#include ".\singleproitem.h"

//////////////////////////////////////////////////////////////////////

//定时器 ID
#define IDT_STATUS_TEXT					1					//状态标题

//////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CBugleDlg, CSkinDialogEx)
	ON_BN_CLICKED(IDC_EXPRESSION, OnBnClickedExpression)
	ON_BN_CLICKED(IDC_COLOR_SET, OnBnClickedColorSet)
	ON_MESSAGE(WM_HIT_EXPMESSTION,OnHitExpression)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_WM_PAINT()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////

//构造函数
CBugleDlg::CBugleDlg(CWnd* pParent): CSkinDialogEx(IDD_BUGLE,pParent)
{
	//设置变量
	m_pMeUserData=NULL;
	m_crChatTX = RGB(0,0,0);
	m_pIClientKernel=NULL;
	m_pClientSocket=NULL;
}

//析构函数
CBugleDlg::~CBugleDlg()
{
}

//控件绑定
void CBugleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDOK, m_btOK);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Control(pDX, IDC_EXPRESSION, m_btExpression);
	DDX_Control(pDX, IDC_COLOR_SET, m_btColorSet);
	DDX_Control(pDX, IDC_InputChat, m_InputChat);
	DDX_Text(pDX, IDC_InputChat, m_strInuptChat);
	DDV_MaxChars(pDX, m_strInuptChat, BUGLE_MAX_CHAR);
	DDX_Text(pDX, IDC_PROPERTY_INFO, m_strPropertyInfo);

	return;
}

//初始框架
BOOL CBugleDlg::OnInitDialog()
{
	CSkinDialogEx::OnInitDialog();

	//加载资源
	UpdateSkinResource();

	m_strPropertyInfo.Format(TEXT("小喇叭%ld游戏币一条。每条消息不得超过%d个字符，消息将在房间及房间内的所有游戏桌子上以醒目的方式显示。"),
		CPropertyBar::m_PropertyInfo[PROP_BUGLE].lPrice1, BUGLE_MAX_CHAR);

	UpdateData(FALSE);

	CString strTitle ;
	strTitle.Format(TEXT("您现在剩余的小喇叭数目为[ %d ]"), m_pMeUserData->dwPropResidualTime[PROP_BUGLE]);
	
	//设置标题
	SetWindowText(strTitle);
	SetTimer(IDT_STATUS_TEXT,400,NULL);

	return TRUE;
}

//表情消息
LRESULT CBugleDlg::OnHitExpression(WPARAM wParam, LPARAM lParam)
{
	CExpressionItem * pExpressionItem=m_Expression.GetExpressItem((WORD)wParam);
	if (pExpressionItem!=NULL)
	{
		CString strChatMsg;
		m_InputChat.GetWindowText(strChatMsg);
		strChatMsg+=pExpressionItem->m_szTrancelate;
		m_InputChat.SetWindowText(strChatMsg);
		m_InputChat.SetFocus();
		m_InputChat.SetSel(strChatMsg.GetLength(),strChatMsg.GetLength());
	}

	return 0;
}

//颜色按钮
void CBugleDlg::OnBnClickedColorSet()
{
	//设置颜色
	CColorDialog ColorDialog(m_crChatTX,CC_FULLOPEN);
	
	if (ColorDialog.DoModal()==IDOK)
	{
		m_crChatTX=ColorDialog.GetColor();

		//设置界面
		m_InputChat.SetFocus();
	}

	return;
}

//表情按钮
void CBugleDlg::OnBnClickedExpression()
{
	//建立表情窗口
	if (m_Expression.GetSafeHwnd()==NULL) m_Expression.CreateExpression(this);

	//移动窗口
	CRect rcButton;
	CSize ExpressSize;
	m_Expression.GetFixSize(ExpressSize);
	m_btExpression.GetWindowRect(&rcButton);
	m_Expression.MoveWindow(rcButton.right-ExpressSize.cx,rcButton.top-ExpressSize.cy,ExpressSize.cx,ExpressSize.cy);
	m_Expression.ShowWindow(SW_SHOW);
	m_Expression.SetFocus();

	return;
}

//确认消息
void CBugleDlg::OnBnClickedOk()
{
	//获取信息
	CString strMsg;
	m_InputChat.GetWindowText(strMsg);
	if ( strMsg.IsEmpty() )
	{
		ShowInformationEx(TEXT("输入内容不能为空！"), 0,MB_ICONINFORMATION,TEXT("系统提示")); 
		return ;
	}
	if ( BUGLE_MAX_CHAR <= strMsg.GetLength() )
	{ 
		CString strMsg;
		strMsg.Format(TEXT("输入内容最多为%d个字符！"), BUGLE_MAX_CHAR);
		ShowInformationEx(strMsg, 0,MB_ICONINFORMATION,TEXT("系统提示")); 
		return ;
	}

	//获取玩家
	tagUserData const *pMeUserData = m_pMeUserData;

	//设置数据
	CMD_GF_BugleProperty BugleProperty ;
	ZeroMemory(&BugleProperty, sizeof(BugleProperty));
	BugleProperty.cbSendLocation=(m_pIClientKernel!=NULL)?LOCATION_GAME_ROOM:LOCATION_PLAZA_ROOM;
	lstrcpyn(BugleProperty.szUserName, pMeUserData->szName, CountArray(BugleProperty.szUserName));
	BugleProperty.crText = m_crChatTX;
	lstrcpyn(BugleProperty.szContext, strMsg, CountArray(BugleProperty.szContext));

	//发送消息
	SendData(MDM_GF_PRESENT, SUB_GF_PROP_BUGLE, &BugleProperty,sizeof(BugleProperty));

	OnOK();
	return;
}

//控件颜色
HBRUSH CBugleDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CSkinDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	if ( nCtlColor == CTLCOLOR_EDIT )
	{
		pDC->SetTextColor(m_crChatTX);
	}
	return hbr;
}

//时间消息
void CBugleDlg::OnTimer(UINT nIDEvent)
{
	switch(nIDEvent)
	{
	case IDT_STATUS_TEXT:			//更新题目
		{
			//获取玩家
			tagUserData const *pMeUserData = m_pMeUserData;

			//标题信息
			CString strTitle ;
			strTitle.Format(TEXT("您现在剩余的小喇叭数目为[ %d ]"),(WORD)m_pMeUserData->dwPropResidualTime[PROP_BUGLE]);
			SetWindowText(strTitle);

			return;
		}
	}

	CSkinDialogEx::OnTimer(nIDEvent);
}

//绘画函数
void CBugleDlg::OnPaint()
{
	CPaintDC dc(this); 

	//绘画标题
	DrawCaption(&dc);

	CRect ClientRect;
	GetClientRect(&ClientRect);
	int nXExcursion=GetXExcursionPos();
	int nYExcursion=GetYExcursionPos();

	//绘画背景
	COLORREF Color = RGB(161,212,226);
	dc.FillSolidRect(nXExcursion,nYExcursion,ClientRect.Width()-2*nXExcursion,4,Color);
	dc.FillSolidRect(nXExcursion,ClientRect.Height()-8,ClientRect.Width()-2*nXExcursion,7,Color);
	dc.FillSolidRect(nXExcursion,nYExcursion+4,12,ClientRect.Height()-nYExcursion-9,Color);
	dc.FillSolidRect(ClientRect.Width()-nXExcursion-12,nYExcursion+4,12,ClientRect.Height()-nYExcursion-9,Color);

	//绘画边框
	DrawBorder(&dc);

	return;
}

//更新资源
bool CBugleDlg::UpdateSkinResource()
{
	HINSTANCE hInstance= CPropertyBar::m_pPropertyBar->m_ReshInstance;
	m_btExpression.SetButtonImage(CPropertyBar::m_pPropertyBar->m_PropertyViewImage.uBtBugleExpression,hInstance,false);
	m_btColorSet.SetButtonImage(CPropertyBar::m_pPropertyBar->m_PropertyViewImage.uBtBugleColorSet,hInstance,false);

	return true;
}

//设置信息
void CBugleDlg::SetSendInfo(IClientKernel *pClientKernel,ITCPSocket *pClientSocket,tagUserData *pMeUserData)
{
	ASSERT((pClientKernel==NULL && pClientSocket!=NULL)||(pClientKernel!=NULL && pClientSocket==NULL));

	//设置信息
	m_pIClientKernel = pClientKernel;
	m_pClientSocket= pClientSocket;
	m_pMeUserData=pMeUserData;

	return;
}

//发送信息
void CBugleDlg::SendData(WORD wMainCmdID, WORD wSubCmdID, void * pBuffer, WORD wDataSize)
{
	ASSERT((m_pIClientKernel==NULL && m_pClientSocket!=NULL)||(m_pIClientKernel!=NULL && m_pClientSocket==NULL));

	//发送信息
	if(m_pClientSocket!=NULL)m_pClientSocket->SendData(wMainCmdID, wSubCmdID, pBuffer, wDataSize);
	else m_pIClientKernel->SendSocketData(wMainCmdID, wSubCmdID, pBuffer, wDataSize);

	return ;
}

//////////////////////////////////////////////////////////////////////

//啦八道具
extern "C" PROPERTY_MODULE_CLASS VOID __cdecl ShowBugleDlg(IClientKernel *pIClientKernel,ITCPSocket *pClientSocket,tagUserData *pMeUserData)
{
	//模态对话框
	CBugleDlg BugleDlg;
	BugleDlg.SetSendInfo(pIClientKernel,pClientSocket,pMeUserData);
	BugleDlg.DoModal();

	return ;
}

//////////////////////////////////////////////////////////////////////

