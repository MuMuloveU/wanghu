#include "Stdafx.h"
#include "Resource.h"
#include "GameRule.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameRule, CDialog)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_ENABLE_STEP_TIME, OnBnClickedEnableStep)
	ON_BN_CLICKED(IDC_ENABLE_LIMIT_TIME, OnBnClickedEnableLimit)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameRule::CGameRule() : CDialog(IDD_GAME_RULE)
{
	//规则变量
	memset(&m_GameRuleInfo,0,sizeof(m_GameRuleInfo));

	//加载资源
	m_ImageBack.SetLoadInfo(IDB_GAME_RULE,AfxGetInstanceHandle());

	return;
}

//析构函数
CGameRule::~CGameRule()
{
}

//控件绑定
void CGameRule::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btOk);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
}

//初始化函数
BOOL CGameRule::OnInitDialog()
{
	__super::OnInitDialog();

	//设置标题
	SetWindowText(TEXT("规则设置："));

	//创建画刷
	m_Brush.CreateSolidBrush(RGB(245,238,221));

	//移动窗口
	CImageHandle BackImageHandle(&m_ImageBack);
	SetWindowPos(NULL,0,0,m_ImageBack.GetWidth(),m_ImageBack.GetHeight(),SWP_NOZORDER|SWP_NOMOVE);

	//设置按钮
	m_btOk.SetButtonImage(IDB_RULE_AGREE,AfxGetInstanceHandle(),false);
	m_btCancel.SetButtonImage(IDB_RULE_QUIT,AfxGetInstanceHandle(),false);

	//变量定义
	int nItem=0;
	TCHAR szBuffer[64];
	WORD wStepTime[]={10,30,60};
	BYTE wTimeCountCount[]={1,3,6};
	WORD wLimitTime[]={300,600,1200,1800,3600};

	//超时步时
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(IDC_STEP_TIME);
	for (int i=0;i<CountArray(wStepTime);i++)
	{
		_snprintf(szBuffer,sizeof(szBuffer),TEXT("%d"),wStepTime[i]);
		nItem=pComboBox->InsertString(i,szBuffer);
		pComboBox->SetItemData(nItem,wStepTime[i]);
	}

	//超时次数
	pComboBox=(CComboBox *)GetDlgItem(IDC_TIME_OUT_COUNT);
	for (int i=0;i<CountArray(wTimeCountCount);i++)
	{
		_snprintf(szBuffer,sizeof(szBuffer),TEXT("%d"),wTimeCountCount[i]);
		nItem=pComboBox->InsertString(i,szBuffer);
		pComboBox->SetItemData(nItem,wTimeCountCount[i]);
	}

	//限定时间
	pComboBox=(CComboBox *)GetDlgItem(IDC_LIMIT_TIME);
	for (int i=0;i<CountArray(wLimitTime);i++)
	{
		_snprintf(szBuffer,sizeof(szBuffer),TEXT("%d"),wLimitTime[i]/60);
		nItem=pComboBox->InsertString(i,szBuffer);
		pComboBox->SetItemData(nItem,wLimitTime[i]/60);
	}

	//更新状态
	UpdateControlStatus();

	return TRUE;
}

//确定消息
void CGameRule::OnOK()
{
	//限定步时
	CButton * pButton=(CButton *)(GetDlgItem(IDC_ENABLE_STEP_TIME));
	if (pButton->GetCheck()==BST_CHECKED)
	{
		//超时步时
		if (GetComboBoxValue(IDC_STEP_TIME,m_GameRuleInfo.wRuleStepTime)==false)
		{
			AfxMessageBox(TEXT("读秒时间错误，请重新输入正确的读秒时间！"),MB_ICONQUESTION);
			GetDlgItem(IDC_STEP_TIME)->SetFocus();
			return;
		}

		//超时次数
		if (GetComboBoxValue(IDC_TIME_OUT_COUNT,m_GameRuleInfo.wRuleTimeOutCount)==false)
		{
			AfxMessageBox(TEXT("读秒次数错误，请重新输入正确的读秒次数！"),MB_ICONQUESTION);
			GetDlgItem(IDC_TIME_OUT_COUNT)->SetFocus();
			return;
		}
	}
	else 
	{
		m_GameRuleInfo.wRuleStepTime=0;
		m_GameRuleInfo.wRuleTimeOutCount=0;
	}

	//限定时间
	pButton=(CButton *)(GetDlgItem(IDC_ENABLE_LIMIT_TIME));
	if (pButton->GetCheck()==BST_CHECKED)
	{
		//限定时间
		if (GetComboBoxValue(IDC_LIMIT_TIME,m_GameRuleInfo.wRuleLimitTime)==false)
		{
			AfxMessageBox(TEXT("限定时间错误，请重新输入正确的限定时间！"),MB_ICONQUESTION);
			GetDlgItem(IDC_LIMIT_TIME)->SetFocus();
			return;
		}

		//调整变量
		m_GameRuleInfo.wRuleLimitTime*=60;
	}
	else m_GameRuleInfo.wRuleLimitTime=0;

	//比赛模式
	pButton=(CButton *)GetDlgItem(IDC_MATCH_MODE);
	m_GameRuleInfo.cbGameMode=(pButton->GetCheck()==BST_CHECKED)?GM_MATCH:GM_FRIEND;

	//指点标志
	m_GameRuleInfo.cbDirections=(((CButton *)GetDlgItem(IDC_DIRECTIONS))->GetCheck()==BST_CHECKED)?TRUE:FALSE;

	//发送消息
	AfxGetMainWnd()->PostMessage(IDM_AGREE_GAME_RULE,0,0);

	//关闭窗口
	DestroyWindow();

	return;
}

//取消消息
void CGameRule::OnCancel()
{
	//发送消息
	AfxGetMainWnd()->PostMessage(WM_CLOSE,0,0);

	//关闭窗口
	DestroyWindow();

	return;
}

//更新状态
void CGameRule::UpdateControlStatus()
{
	//窗口效验
	if (GetSafeHwnd()==NULL) return;

	//时间规则
	SetComboBoxValue(IDC_STEP_TIME,m_GameRuleInfo.wRuleStepTime);
	SetComboBoxValue(IDC_LIMIT_TIME,m_GameRuleInfo.wRuleLimitTime/60);
	SetComboBoxValue(IDC_TIME_OUT_COUNT,m_GameRuleInfo.wRuleTimeOutCount);

	//先定步时
	if (m_GameRuleInfo.wRuleStepTime!=0) ((CButton *)GetDlgItem(IDC_ENABLE_STEP_TIME))->SetCheck(BST_CHECKED);
	else ((CButton *)GetDlgItem(IDC_ENABLE_STEP_TIME))->SetCheck(BST_UNCHECKED);

	//限时判断
	if (m_GameRuleInfo.wRuleLimitTime!=0) ((CButton *)GetDlgItem(IDC_ENABLE_LIMIT_TIME))->SetCheck(BST_CHECKED);
	else ((CButton *)GetDlgItem(IDC_ENABLE_LIMIT_TIME))->SetCheck(BST_UNCHECKED);

	//比赛模式
	CButton * pButton=(CButton *)GetDlgItem(IDC_MATCH_MODE);
	pButton->SetCheck((m_GameRuleInfo.cbGameMode==GM_MATCH)?BST_CHECKED:BST_UNCHECKED);

	//友谊模式
	pButton=(CButton *)GetDlgItem(IDC_FRIEND_MODE);
	pButton->SetCheck((m_GameRuleInfo.cbGameMode==GM_FRIEND)?BST_CHECKED:BST_UNCHECKED);

	//指点设置
	pButton=(CButton *)GetDlgItem(IDC_DIRECTIONS);
	pButton->SetCheck(m_GameRuleInfo.cbDirections?BST_CHECKED:BST_UNCHECKED);

	return;
}

//允许控制
void CGameRule::ControlEnable(bool bEnable)
{
	//变量定义
	BOOL bEnableFlag=bEnable?TRUE:FALSE;

	//设置控件
	GetDlgItem(IDC_MATCH_MODE)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_FRIEND_MODE)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_DIRECTIONS)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_ENABLE_STEP_TIME)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_ENABLE_LIMIT_TIME)->EnableWindow(bEnableFlag);

	//特殊控制
	if (bEnable==true)
	{
		//限定步时
		if (((CButton *)GetDlgItem(IDC_ENABLE_STEP_TIME))->GetCheck()==BST_CHECKED)
		{
			GetDlgItem(IDC_STEP_TIME)->EnableWindow(true);
			GetDlgItem(IDC_TIME_OUT_COUNT)->EnableWindow(true);
		}
		else
		{
			GetDlgItem(IDC_STEP_TIME)->EnableWindow(false);
			GetDlgItem(IDC_TIME_OUT_COUNT)->EnableWindow(false);
		}

		//限定时间
		if (((CButton *)GetDlgItem(IDC_ENABLE_LIMIT_TIME))->GetCheck()==BST_CHECKED)
		{
			GetDlgItem(IDC_LIMIT_TIME)->EnableWindow(true);
		}
		else GetDlgItem(IDC_LIMIT_TIME)->EnableWindow(false);
	}
	else
	{
		GetDlgItem(IDC_STEP_TIME)->EnableWindow(false);
		GetDlgItem(IDC_LIMIT_TIME)->EnableWindow(false);
		GetDlgItem(IDC_TIME_OUT_COUNT)->EnableWindow(false);
	}

	return;
}

//设置选择
bool CGameRule::SetComboBoxValue(UINT uComboBoxID, WORD wValue)
{
	//获取控件
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(uComboBoxID);

	//寻找选项
	int nItemCount=pComboBox->GetCount();
	for (int i=0;i<nItemCount;i++)
	{
		if (wValue==(WORD)pComboBox->GetItemData(i))
		{
			pComboBox->SetCurSel(i);
			return true;
		}
	}

	//补充填写
	TCHAR szBuffer[16];
	_snprintf(szBuffer,sizeof(szBuffer),TEXT("%d"),wValue);
	pComboBox->SetWindowText(szBuffer);

	return true;
}

//获取数值
bool CGameRule::GetComboBoxValue(UINT uComboBoxID, WORD & wValue)
{
	//获取控件
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(uComboBoxID);

	//获取选择
	int nItem=pComboBox->GetCurSel();
	if (nItem!=LB_ERR)
	{
		wValue=(WORD)pComboBox->GetItemData(nItem);
		return true;
	}

	//获取字符
	TCHAR szBuffer[32];
	pComboBox->GetWindowText(szBuffer,CountArray(szBuffer));
	if (szBuffer[0]==0) return false;

	//输入效验
	int nIndex=0;
	do
	{
		if ((szBuffer[nIndex]<TEXT('0'))||(szBuffer[nIndex]>TEXT('9'))) return false;
	} while (szBuffer[++nIndex]!=0);

	//数值转换
	wValue=atoi(szBuffer);

	return true;
}

//重画函数
void CGameRule::OnPaint()
{
	CPaintDC dc(this);

	//绘画背景
	CImageHandle BackImageHandle(&m_ImageBack);
	m_ImageBack.BitBlt(dc,0,0);

	return;
}

//绘画背景
BOOL CGameRule::OnEraseBkgnd(CDC * pDC)
{
	Invalidate(FALSE);
	UpdateWindow();

	return TRUE;
}

//销毁消息
void CGameRule::OnDestroy()
{
	//清理资源
	m_Brush.DeleteObject();

	__super::OnDestroy();
}

//鼠标消息
void CGameRule::OnLButtonDown(UINT nFlags, CPoint point)
{
	__super::OnLButtonDown(nFlags,point);

	//消息模拟
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));

	return;
}

//控件颜色
HBRUSH CGameRule::OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor)
{
	switch (nCtlColor)
	{
	case CTLCOLOR_DLG:
	case CTLCOLOR_STATIC:
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(RGB(10,10,10));
			pDC->SetBkColor(RGB(245,238,221));
			return m_Brush;
		}
	}
	return __super::OnCtlColor(pDC,pWnd,nCtlColor);
}

//限定步时
void CGameRule::OnBnClickedEnableStep()
{
	//设置控件
	if (((CButton *)GetDlgItem(IDC_ENABLE_STEP_TIME))->GetCheck()==BST_CHECKED)
	{
		GetDlgItem(IDC_STEP_TIME)->EnableWindow(true);
		GetDlgItem(IDC_TIME_OUT_COUNT)->EnableWindow(true);
	}
	else
	{
		GetDlgItem(IDC_STEP_TIME)->EnableWindow(false);
		GetDlgItem(IDC_TIME_OUT_COUNT)->EnableWindow(false);
	}

	return;
}

//限定步时
void CGameRule::OnBnClickedEnableLimit()
{
	//设置控件
	if (((CButton *)GetDlgItem(IDC_ENABLE_LIMIT_TIME))->GetCheck()==BST_CHECKED)
	{
		GetDlgItem(IDC_LIMIT_TIME)->EnableWindow(true);
	}
	else GetDlgItem(IDC_LIMIT_TIME)->EnableWindow(false);

	return;
}

//////////////////////////////////////////////////////////////////////////
