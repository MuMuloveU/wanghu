#include "Stdafx.h"
#include "GameOption.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameOption, CSkinDialogEx)
	ON_BN_CLICKED(IDC_DEFAULT, OnDefault)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameOption::CGameOption() : CSkinDialogEx(IDD_OPTION)
{
	//设置变量
	m_bDeasilOrder=true;
	m_dwCardHSpace=DEFAULT_PELS;
	m_GameSoundMode=enMandarinSound;

	return;
}

//析构函数
CGameOption::~CGameOption()
{
}

//控件绑定
void CGameOption::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btOK);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Control(pDX, IDC_DEFAULT, m_btDefault);
}

//初始化函数
BOOL CGameOption::OnInitDialog()
{
	__super::OnInitDialog();

	//设置控件
	SetWindowText(TEXT("游戏配置"));

	//调整参数
	if ((m_dwCardHSpace>MAX_PELS)||(m_dwCardHSpace<LESS_PELS)) m_dwCardHSpace=DEFAULT_PELS;
	if ((m_GameSoundMode>enShanXiSound)||(m_GameSoundMode<enNoSound)) m_GameSoundMode=enMandarinSound;

	//出牌顺序
	if (m_bDeasilOrder==true) ((CButton *)GetDlgItem(IDC_DEASIL_ORDER))->SetCheck(BST_CHECKED);

	//扑克象素
	int nItem=0;
	TCHAR szBuffer[32]=TEXT("");
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(IDC_CARD_SPACE);
	for (DWORD i=LESS_PELS;i<=MAX_PELS;i++)
	{
		_snprintf(szBuffer,sizeof(szBuffer),TEXT("%2ld"),i);
		nItem=pComboBox->InsertString((i-LESS_PELS),szBuffer);
		pComboBox->SetItemData(nItem,i);
		if (m_dwCardHSpace==i) pComboBox->SetCurSel(nItem);
	}

	//声音效果
	if (m_GameSoundMode==enNoSound) ((CButton *)GetDlgItem(IDC_NO_SOUND))->SetCheck(BST_CHECKED);
	else if (m_GameSoundMode==enNormalSound) ((CButton *)GetDlgItem(IDC_NORMAL_SOUND))->SetCheck(BST_CHECKED);
	else if (m_GameSoundMode==enMandarinSound) ((CButton *)GetDlgItem(IDC_MANDARIN_SOUND))->SetCheck(BST_CHECKED);
	else if (m_GameSoundMode==enShanXiSound) ((CButton *)GetDlgItem(IDC_SHANXI_SOUND))->SetCheck(BST_CHECKED);

	return TRUE;
}

//确定消息
void CGameOption::OnOK()
{
	//出牌顺序
	m_bDeasilOrder=(((CButton *)GetDlgItem(IDC_DEASIL_ORDER))->GetCheck()==BST_CHECKED);

	//扑克象素
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(IDC_CARD_SPACE);
	m_dwCardHSpace=(DWORD)pComboBox->GetItemData(pComboBox->GetCurSel());

	//声音效果
	if (((CButton *)GetDlgItem(IDC_NO_SOUND))->GetCheck()==BST_CHECKED) m_GameSoundMode=enNoSound;
	else if (((CButton *)GetDlgItem(IDC_NORMAL_SOUND))->GetCheck()==BST_CHECKED) m_GameSoundMode=enNormalSound;
	else if (((CButton *)GetDlgItem(IDC_MANDARIN_SOUND))->GetCheck()==BST_CHECKED) m_GameSoundMode=enMandarinSound;
	else if (((CButton *)GetDlgItem(IDC_SHANXI_SOUND))->GetCheck()==BST_CHECKED) m_GameSoundMode=enShanXiSound;

	__super::OnOK();
}

//默认参数
void CGameOption::OnDefault()
{
	//设置控件
	((CButton *)GetDlgItem(IDC_NO_SOUND))->SetCheck(BST_UNCHECKED);
	((CButton *)GetDlgItem(IDC_NORMAL_SOUND))->SetCheck(BST_UNCHECKED);
	((CButton *)GetDlgItem(IDC_MANDARIN_SOUND))->SetCheck(BST_CHECKED);
	((CButton *)GetDlgItem(IDC_SHANXI_SOUND))->SetCheck(BST_UNCHECKED);
	((CComboBox *)GetDlgItem(IDC_CARD_SPACE))->SetCurSel(DEFAULT_PELS-LESS_PELS);

	return;
}

//////////////////////////////////////////////////////////////////////////
