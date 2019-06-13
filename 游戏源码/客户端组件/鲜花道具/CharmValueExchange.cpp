#include "Stdafx.h"
#include "resource.h"
#include "CharmValueExchange.h"
#include ".\charmvalueexchange.h"

//////////////////////////////////////////////////////////////////////////

//定时器I D
#define IDI_CHARMVALUE_UPDATE_VIEW		1								//更新界面
#define TIME_CHARMVALUE_UPDATE_VIEW		500								//更新界面

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CCharmValueExchange, CSkinDialogEx)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()	
	ON_EN_CHANGE(IDC_EXCHANGE_CHARM_VALUE, OnEnChangeExchangeCharmValue)
	ON_BN_CLICKED(IDC_BT_EXCHANGE, OnBnClickedBtExchange)
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CCharmValueExchange::CCharmValueExchange(CWnd* pParent):CSkinDialogEx(IDD_CHARM_VALUE_EXCHANGE, pParent)
{
	m_lExchangeCharmValue=0;
	m_lLoveliness=0;
	m_lBankGoldValue=0;
	m_pIClientKernel=NULL;
	m_pClientSocket=NULL;
	m_pMeUserData = NULL;
}

//析构函数
CCharmValueExchange::~CCharmValueExchange()
{
}

//控件绑定
void CCharmValueExchange::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BT_EXCHANGE, m_btOK);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Text(pDX, IDC_EXCHANGE_VALUE, m_strExChangeValue);
	DDX_Text(pDX, IDC_CHARM_VALUE, m_lLoveliness);
	DDX_Text(pDX, IDC_GOLD_VALUE, m_lBankGoldValue);
}

//确定信息
void CCharmValueExchange::OnBnClickedBtExchange()
{
	//参数验证
	if(m_pMeUserData->lLoveliness==0)
	{
		CString strMessage;
		strMessage.Format(TEXT("您的魅力值为0,不能进行兑换操作!"));
		ShowInformationEx(strMessage,0 ,MB_ICONINFORMATION,TEXT("系统消息")) ;

		__super::OnOK();
		return;
	}
	else if ( m_lExchangeCharmValue <= 0 || m_lExchangeCharmValue > DWORD(m_pMeUserData->lLoveliness))
	{
		CString strMessage;
		strMessage.Format(TEXT("你输入的魅力值必须在1和%ld之间"), m_pMeUserData->lLoveliness);
		ShowInformationEx(strMessage,0 ,MB_ICONINFORMATION,TEXT("系统消息")) ;

		GetDlgItem(IDC_EXCHANGE_CHARM_VALUE)->SetFocus();
		((CEdit*)GetDlgItem(IDC_EXCHANGE_CHARM_VALUE))->SetSel(0,-1);
		return;
	}

	//发送信息
	CMD_GF_ExchangeCharm ExchangeCharmValue;
	ZeroMemory(&ExchangeCharmValue, sizeof(ExchangeCharmValue));
	ExchangeCharmValue.cbSendLocation=(m_pIClientKernel!=NULL)?LOCATION_GAME_ROOM:LOCATION_PLAZA_ROOM;
	ExchangeCharmValue.lLoveliness = m_lExchangeCharmValue ;
	ExchangeCharmValue.lGoldValue = m_lExchangeCharmValue * CHARM_EXCHANGE_RATE;

	SendData(MDM_GF_PRESENT, SUB_GF_EXCHANGE_CHARM, &ExchangeCharmValue, sizeof(ExchangeCharmValue));

	return;
}

//初始化函数
BOOL CCharmValueExchange::OnInitDialog()
{
	CSkinDialogEx::OnInitDialog();

	UpdateData(FALSE);

	SetWindowText(TEXT("魅力兑换"));

	//交换数值
	CString strValue;
	strValue.Format(TEXT("%ld"),m_lExchangeCharmValue);
	GetDlgItem(IDC_EXCHANGE_CHARM_VALUE)->SetWindowText(strValue);
	GetDlgItem(IDC_EXCHANGE_CHARM_VALUE)->SetFocus();

	//按钮颜色
	m_btOK.SetTextColor(RGB(0,0,0));
	m_btCancel.SetTextColor(RGB(30,30,30));

	//定时更新
	SetTimer(IDI_CHARMVALUE_UPDATE_VIEW,TIME_CHARMVALUE_UPDATE_VIEW,NULL);

	return TRUE;
}

//更新界面
void CCharmValueExchange::UpdateView()
{
	UpdateData(TRUE);

	//兑换信息
	if(m_pMeUserData!=NULL)
	{
		m_lBankGoldValue = m_pMeUserData->lInsureScore;
		m_lLoveliness = ((m_pMeUserData->lLoveliness<0)?0:m_pMeUserData->lLoveliness);
	}

	//设置信息
	m_strExChangeValue.Format(TEXT("转换汇率：1魅力 = %d游戏币"), CHARM_EXCHANGE_RATE);

	//InvalidateRect(NULL);
	UpdateData(FALSE);
}

//绘画函数
void CCharmValueExchange::OnPaint()
{
	CPaintDC dc(this); 

	//绘画标题
	DrawCaption(&dc);

	//绘画背景
	DrawBackGround(&dc);

	//绘画边框
	DrawBorder(&dc);

	//绘画图片
	CPngImage PngImage;
	HINSTANCE hInstance = CPropertyBar::m_pPropertyBar->m_ReshInstance;
	PngImage.LoadImage(hInstance,CPropertyBar::m_pPropertyBar->m_PropertyViewImage.pszExChagneArrowhead);
	PngImage.DrawImage(&dc,168,46);

	return;
}

//鼠标信息
void CCharmValueExchange::OnLButtonDown(UINT nFlags, CPoint point)
{
	CSkinDialogEx::OnLButtonDown(nFlags, point);
}

//鼠标信息
void CCharmValueExchange::OnMouseMove(UINT nFlags, CPoint point)
{
	CSkinDialogEx::OnMouseMove(nFlags, point);
}

//命令信息
BOOL CCharmValueExchange::PreTranslateMessage(MSG* pMsg)
{
	//过滤字母
	if(GetFocus()==GetDlgItem(IDC_EXCHANGE_CHARM_VALUE) && pMsg->message==WM_CHAR)
	{
		BYTE bMesValue = (BYTE)(pMsg->wParam);
		BYTE bTemp = bMesValue-'0';
		if((bTemp<0 || bTemp>9) && bMesValue!=VK_BACK) return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//时间消息
void CCharmValueExchange::OnTimer(UINT nIDEvent)
{
	//时间消息
	switch(nIDEvent)
	{
	case IDI_CHARMVALUE_UPDATE_VIEW:		//更新界面
		{
			//更新界面
			UpdateView();

			return;
		}
	}

	__super::OnTimer(nIDEvent);
}

//输入信息
void CCharmValueExchange::OnEnChangeExchangeCharmValue()
{
	if(m_pMeUserData==NULL)return ;

	CString strCount;
	GetDlgItem(IDC_EXCHANGE_CHARM_VALUE)->GetWindowText(strCount);
	m_lExchangeCharmValue=_tstol(strCount);

	return ;
}

//设置颜色
HBRUSH CCharmValueExchange::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch (nCtlColor)
	{
	case CTLCOLOR_DLG:
	case CTLCOLOR_STATIC:
		{
			if(pWnd->m_hWnd==GetDlgItem(IDC_EXCHANGE_VALUE)->m_hWnd)
			{
				pDC->SetTextColor(RGB(255,0,0));
			}
			//else if(pWnd->m_hWnd==GetDlgItem(IDC_GOLD_VALUE)->m_hWnd || pWnd->m_hWnd==GetDlgItem(IDC_STATIC_1)->m_hWnd)
			//{
			//	pDC->SetTextColor(RGB(30,30,30));
			//}
			else
			{
				pDC->SetTextColor(RGB(0,0,0));
			}
			pDC->SetBkMode(TRANSPARENT);

			return m_SkinAttribute.m_brBackGround;
		}
	}

	return __super::OnCtlColor(pDC,pWnd,nCtlColor);
}

//设置信息
void CCharmValueExchange::SetSendInfo(IClientKernel *pClientKernel,ITCPSocket *pClientSocket,tagUserData *pUserData)
{
	ASSERT(pUserData!=NULL);
	ASSERT((pClientKernel==NULL && pClientSocket!=NULL)||(pClientKernel!=NULL && pClientSocket==NULL));

	//设置信息
	m_pIClientKernel = pClientKernel;
	m_pClientSocket= pClientSocket;
	m_pMeUserData = pUserData;
	m_lExchangeCharmValue = m_pMeUserData->lLoveliness;

	return;
}

//发送信息
void CCharmValueExchange::SendData(WORD wMainCmdID, WORD wSubCmdID, void * pBuffer, WORD wDataSize)
{
	ASSERT((m_pIClientKernel==NULL && m_pClientSocket!=NULL)||(m_pIClientKernel!=NULL && m_pClientSocket==NULL));

	//发送信息
	if(m_pClientSocket!=NULL)m_pClientSocket->SendData(wMainCmdID, wSubCmdID, pBuffer, wDataSize);
	else m_pIClientKernel->SendSocketData(wMainCmdID, wSubCmdID, pBuffer, wDataSize);

	return ;
}

//////////////////////////////////////////////////////////////////////////

//兑换魅力
extern "C" PROPERTY_MODULE_CLASS VOID __cdecl ShowCharmValueExchangeDlg(IClientKernel *pIClientKernel,ITCPSocket *pClientSocket,tagUserData *pUserData)
{
	//兑换窗口
	CCharmValueExchange CharmValueExchangeDlg;

	//设置信息
	CharmValueExchangeDlg.SetSendInfo(pIClientKernel,pClientSocket,pUserData);
	CharmValueExchangeDlg.DoModal();

	return ;
}

//////////////////////////////////////////////////////////////////////////
