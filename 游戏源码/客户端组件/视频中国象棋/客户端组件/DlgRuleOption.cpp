#include "Stdafx.h"
#include "Resource.h"
#include "DlgRuleOption.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

//时间标识
#define IDI_GAME_RULE_TIME			10									//时间标识

//颜色定义
#define COLOR_RULE_BACK				RGB(221,197,164)					//背景颜色

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgRuleOption, CDialog)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_FAST_MODE, OnBnClickedFastMode)
	ON_BN_CLICKED(IDC_SLOW_MODE, OnBnClickedSlowMode)
	ON_BN_CLICKED(IDC_CUSTOM_MODE, OnBnClickedCustomMode)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgRuleOption::CDlgRuleOption() : CDialog(IDD_RULE_OPTION)
{
	//状态变量
	m_nResidualTime=0L;

	//设置变量
	m_bControl=false;
	m_bSelectMode=false;
	ZeroMemory(&m_GameRuleInfo,sizeof(m_GameRuleInfo));

	//加载资源
	m_ImageViewBack.SetLoadInfo(IDB_RULE_OPTION_VIEW,AfxGetInstanceHandle());

	return;
}

//析构函数
CDlgRuleOption::~CDlgRuleOption()
{
}

//控件绑定
VOID CDlgRuleOption::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btOk);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
}

//初始化函数
BOOL CDlgRuleOption::OnInitDialog()
{
	__super::OnInitDialog();

	//创建区域
	CRgn RgnImage;
	CImageHandle BackImageHandle(&m_ImageViewBack);
	m_ImageViewBack.CreateImageRegion(RgnImage,RGB(255,0,255));

	//设置窗口
	SetWindowRgn(RgnImage,TRUE);
	SetWindowPos(NULL,0,0,m_ImageViewBack.GetWidth(),m_ImageViewBack.GetHeight(),SWP_NOZORDER|SWP_NOMOVE);

	//设置窗口
	SetWindowText(TEXT("规则设置："));
	m_Brush.CreateSolidBrush(COLOR_RULE_BACK);

	//设置控件
	HINSTANCE hResInstance=AfxGetInstanceHandle();
	m_btOk.SetButtonImage(IDB_OPTION_BT_OK,hResInstance,false);
	m_btCancel.SetButtonImage(IDB_OPTION_BT_CANCEL,hResInstance,false);

	//构造提示
	TCHAR szFastDescribe[128]=TEXT(""),szSlowDescribe[128]=TEXT("");
	_sntprintf(szFastDescribe,CountChar(szFastDescribe),TEXT("局时 %d 分钟  步时 %d 分钟  读秒 %d 秒"),
		MODE_FAST_DRAW_TIME/60L,MODE_FAST_STEP_TIME/60L,MODE_FAST_SECOND_TIME);
	_sntprintf(szSlowDescribe,CountChar(szSlowDescribe),TEXT("局时 %d 分钟  步时 %d 分钟  无读秒"),
		MODE_SLOW_DRAW_TIME/60L,MODE_SLOW_STEP_TIME/60L);

	//设置提示
	SetDlgItemText(IDC_FAST_DESCRIBE,szFastDescribe);
	SetDlgItemText(IDC_SLOW_DESCRIBE,szSlowDescribe);

	//变量定义
	TCHAR szBuffer[32]=TEXT("");
	LONG lDrawTime[]={600L,1800L,3600L},lStepTime[]={60L,180L,300L};
	LONG lRuleSecondTime[]={30L,60L,180L,300L},lRuleSecondCount[]={1L,3L,5L};

	//游戏局时
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(IDC_DRAW_TIME);
	for (INT i=0;i<CountArray(lDrawTime);i++)
	{
		_sntprintf(szBuffer,CountChar(szBuffer),TEXT("%ld"),lDrawTime[i]/60L);
		pComboBox->SetItemData(pComboBox->InsertString(i,szBuffer),lDrawTime[i]/60L);
	}

	//游戏步时
	pComboBox=(CComboBox *)GetDlgItem(IDC_STEP_TIME);
	for (INT i=0;i<CountArray(lStepTime);i++)
	{
		_sntprintf(szBuffer,CountChar(szBuffer),TEXT("%ld"),lStepTime[i]/60L);
		pComboBox->SetItemData(pComboBox->InsertString(i,szBuffer),lStepTime[i]/60L);
	}

	//游戏读秒
	pComboBox=(CComboBox *)GetDlgItem(IDC_SECOND_TIME);
	for (INT i=0;i<CountArray(lRuleSecondTime);i++)
	{
		_sntprintf(szBuffer,CountChar(szBuffer),TEXT("%ld"),lRuleSecondTime[i]);
		pComboBox->SetItemData(pComboBox->InsertString(i,szBuffer),lRuleSecondTime[i]);
	}

	//更新控件
	UpdateControlStatus();

	return TRUE;
}

//确定消息
VOID CDlgRuleOption::OnOK()
{
	//获取数据
	if (m_bControl==true)
	{
		//限时设置
		WORD wRuleDrawTime=0;
		WORD wRuleStepTime=0;
		WORD wRuleSecondTime=0;
		GetComboBoxValue(IDC_DRAW_TIME,wRuleDrawTime);
		GetComboBoxValue(IDC_STEP_TIME,wRuleStepTime);
		GetComboBoxValue(IDC_SECOND_TIME,wRuleSecondTime);

		//设置变量
		m_GameRuleInfo.wRuleDrawTime=wRuleDrawTime*60L;
		m_GameRuleInfo.wRuleStepTime=wRuleStepTime*60L;
		m_GameRuleInfo.wRuleSecondTime=wRuleSecondTime;

		//限时模式
		if (IsDlgButtonChecked(IDC_FAST_MODE)==BST_CHECKED) m_GameRuleInfo.cbCurrentMode=MODE_FAST;
		if (IsDlgButtonChecked(IDC_SLOW_MODE)==BST_CHECKED) m_GameRuleInfo.cbCurrentMode=MODE_SLOW;
		if (IsDlgButtonChecked(IDC_CUSTOM_MODE)==BST_CHECKED) m_GameRuleInfo.cbCurrentMode=MODE_CUSTOM;

		//游戏模式
		m_GameRuleInfo.cbDirections=(IsDlgButtonChecked(IDC_DIRECTIONS)==BST_CHECKED)?TRUE:FALSE;
		m_GameRuleInfo.cbRegretFlag=(IsDlgButtonChecked(IDC_REGRET_FLAG)==BST_CHECKED)?TRUE:FALSE;
	}

	//查询接口
	//CGlobalUnits * pGlobalUnits=CGlobalUnits::GetGlobalUnits();
	//IGameFrameEngine * pIGameFrameEngine=(IGameFrameEngine *)pGlobalUnits->QueryGlobalModule(MODULE_GAME_FRAME_ENGINE,IID_IGameFrameEngine,VER_IGameFrameEngine);

	//发送消息
	//ASSERT(pIGameFrameEngine!=NULL);
	/*if (pIGameFrameEngine!=NULL)*/ AfxGetMainWnd()->SendMessage(IDM_GAME_RULE_AGREE,0,0);

	//关闭窗口
	DestroyWindow();

	return;
}

//取消消息
VOID CDlgRuleOption::OnCancel()
{
	//查询接口
	//CGlobalUnits * pGlobalUnits=CGlobalUnits::GetGlobalUnits();
	//IGameFrameEngine * pIGameFrameEngine=(IGameFrameEngine *)pGlobalUnits->QueryGlobalModule(MODULE_GAME_FRAME_ENGINE,IID_IGameFrameEngine,VER_IGameFrameEngine);

	//发送消息
	//ASSERT(pIGameFrameEngine!=NULL);
	/*if (pIGameFrameEngine!=NULL)*/ AfxGetMainWnd()->SendMessage(IDM_GAME_RULE_REJECT,0,0);

	//关闭窗口
	DestroyWindow();

	return;
}

//设置控制
VOID CDlgRuleOption::EnableControl(bool bControl)
{
	//设置变量
	m_bControl=bControl;

	//更新控制
	if (m_hWnd!=NULL) UpdateControlStatus();

	return;
}

//获取规则
VOID CDlgRuleOption::GetGameRuleInfo(tagGameRuleInfo & GameRuleInfo)
{
	//设置变量
	GameRuleInfo=m_GameRuleInfo;

	return;
}

//规则设置
VOID CDlgRuleOption::SetGameRuleInfo(const tagGameRuleInfo & GameRuleInfo, bool bControl, bool bSelectMode)
{
	//设置变量
	m_bControl=bControl;
	m_bSelectMode=bSelectMode;
	m_GameRuleInfo=GameRuleInfo;

	//更新控制
	if (m_hWnd!=NULL) UpdateControlStatus();

	return;
}

//更新状态
VOID CDlgRuleOption::UpdateControlStatus()
{
	//变量定义
	INT nShowCmd=(m_bControl==true)?SW_SHOW:SW_HIDE;

	//限时设置
	GetDlgItem(IDC_DRAW_TIME)->ShowWindow(nShowCmd);
	GetDlgItem(IDC_STEP_TIME)->ShowWindow(nShowCmd);
	GetDlgItem(IDC_SECOND_TIME)->ShowWindow(nShowCmd);

	//设置控制
	GetDlgItem(IDC_FAST_MODE)->ShowWindow(nShowCmd);
	GetDlgItem(IDC_SLOW_MODE)->ShowWindow(nShowCmd);
	GetDlgItem(IDC_CUSTOM_MODE)->ShowWindow(nShowCmd);

	//提示字符
	GetDlgItem(IDC_FAST_DESCRIBE)->ShowWindow(nShowCmd);
	GetDlgItem(IDC_SLOW_DESCRIBE)->ShowWindow(nShowCmd);
	GetDlgItem(IDC_CUSTOM_STRING_1)->ShowWindow(nShowCmd);
	GetDlgItem(IDC_CUSTOM_STRING_2)->ShowWindow(nShowCmd);
	GetDlgItem(IDC_CUSTOM_STRING_3)->ShowWindow(nShowCmd);
	GetDlgItem(IDC_CUSTOM_STRING_4)->ShowWindow(nShowCmd);
	GetDlgItem(IDC_CUSTOM_STRING_5)->ShowWindow(nShowCmd);
	GetDlgItem(IDC_CUSTOM_STRING_6)->ShowWindow(nShowCmd);

	//限时设置
	SetComboBoxValue(IDC_DRAW_TIME,m_GameRuleInfo.wRuleDrawTime/60L);
	SetComboBoxValue(IDC_STEP_TIME,m_GameRuleInfo.wRuleStepTime/60L);
	SetComboBoxValue(IDC_SECOND_TIME,m_GameRuleInfo.wRuleSecondTime);

	//设置控制
	GetDlgItem(IDC_FAST_MODE)->EnableWindow(((m_bControl==true)&&(m_bSelectMode==true))?TRUE:FALSE);
	GetDlgItem(IDC_SLOW_MODE)->EnableWindow(((m_bControl==true)&&(m_bSelectMode==true))?TRUE:FALSE);
	GetDlgItem(IDC_CUSTOM_MODE)->EnableWindow(((m_bControl==true)&&(m_bSelectMode==true))?TRUE:FALSE);

	//模式设置
	CheckDlgButton(IDC_FAST_MODE,(m_GameRuleInfo.cbCurrentMode==MODE_FAST)?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_SLOW_MODE,(m_GameRuleInfo.cbCurrentMode==MODE_SLOW)?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_CUSTOM_MODE,(m_GameRuleInfo.cbCurrentMode==MODE_CUSTOM)?BST_CHECKED:BST_UNCHECKED);

	//自定控件
	if ((m_bControl==true)&&(m_bSelectMode==true)&&(m_GameRuleInfo.cbCurrentMode==MODE_CUSTOM))
	{
		GetDlgItem(IDC_DRAW_TIME)->EnableWindow(TRUE);
		GetDlgItem(IDC_STEP_TIME)->EnableWindow(TRUE);
		GetDlgItem(IDC_SECOND_TIME)->EnableWindow(TRUE);
	}

	//游戏设置
	GetDlgItem(IDC_DIRECTIONS)->EnableWindow((m_bControl==true)?TRUE:FALSE);
	GetDlgItem(IDC_REGRET_FLAG)->EnableWindow((m_bControl==true)?TRUE:FALSE);
	CheckDlgButton(IDC_DIRECTIONS,(m_GameRuleInfo.cbDirections==TRUE)?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_REGRET_FLAG,(m_GameRuleInfo.cbRegretFlag==TRUE)?BST_CHECKED:BST_UNCHECKED);

	//设置时间
	m_nResidualTime=30L;
	SetTimer(IDI_GAME_RULE_TIME,1000,NULL);

	//更新界面
	Invalidate(FALSE);

	return;
}

//设置选择
bool CDlgRuleOption::SetComboBoxValue(UINT uComboBoxID, WORD wValue)
{
	//获取控件
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(uComboBoxID);

	//寻找选项
	INT nItemCount=pComboBox->GetCount();
	for (INT i=0;i<nItemCount;i++)
	{
		if (wValue==(WORD)pComboBox->GetItemData(i))
		{
			pComboBox->SetCurSel(i);
			return true;
		}
	}

	//构造字符
	TCHAR szBuffer[16];
	_sntprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),wValue);

	//设置控件
	pComboBox->SetWindowText(szBuffer);

	return true;
}

//获取数值
bool CDlgRuleOption::GetComboBoxValue(UINT uComboBoxID, WORD & wValue)
{
	//获取控件
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(uComboBoxID);

	//获取选择
	INT nItem=pComboBox->GetCurSel();
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
	INT nIndex=0;
	do
	{
		if ((szBuffer[nIndex]<TEXT('0'))||(szBuffer[nIndex]>TEXT('9'))) return false;
	} while (szBuffer[++nIndex]!=0);

	//数值转换
	wValue=_ttoi(szBuffer);

	return true;
}

//快棋模式
VOID CDlgRuleOption::OnBnClickedFastMode()
{
	//设置数值
	SetComboBoxValue(IDC_DRAW_TIME,MODE_FAST_DRAW_TIME/60L);
	SetComboBoxValue(IDC_STEP_TIME,MODE_FAST_STEP_TIME/60L);
	SetComboBoxValue(IDC_SECOND_TIME,MODE_FAST_SECOND_TIME);

	//设置控件
	GetDlgItem(IDC_DRAW_TIME)->EnableWindow(FALSE);
	GetDlgItem(IDC_STEP_TIME)->EnableWindow(FALSE);
	GetDlgItem(IDC_SECOND_TIME)->EnableWindow(FALSE);

	return;
}

//慢棋模式
VOID CDlgRuleOption::OnBnClickedSlowMode()
{
	//设置数值
	SetComboBoxValue(IDC_DRAW_TIME,MODE_SLOW_DRAW_TIME/60L);
	SetComboBoxValue(IDC_STEP_TIME,MODE_SLOW_STEP_TIME/60L);
	SetComboBoxValue(IDC_SECOND_TIME,MODE_SLOW_SECOND_TIME);

	//设置控件
	GetDlgItem(IDC_DRAW_TIME)->EnableWindow(FALSE);
	GetDlgItem(IDC_STEP_TIME)->EnableWindow(FALSE);
	GetDlgItem(IDC_SECOND_TIME)->EnableWindow(FALSE);

	return;
}

//自定模式
VOID CDlgRuleOption::OnBnClickedCustomMode()
{
	//设置控件
	GetDlgItem(IDC_DRAW_TIME)->EnableWindow(TRUE);
	GetDlgItem(IDC_STEP_TIME)->EnableWindow(TRUE);
	GetDlgItem(IDC_SECOND_TIME)->EnableWindow(TRUE);

	return;
}

//重画函数
VOID CDlgRuleOption::OnPaint()
{
	CPaintDC dc(this);

	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//创建缓冲
	CDC DCBuffer;
	CBitmap ImageBuffer;
	DCBuffer.CreateCompatibleDC(&dc);
	DCBuffer.SetTextAlign(TA_LEFT|TA_TOP);
	ImageBuffer.CreateCompatibleBitmap(&dc,rcClient.Width(),rcClient.Height());

	//设置 DC
	DCBuffer.SetBkMode(TRANSPARENT);
	DCBuffer.SelectObject(&ImageBuffer);
	DCBuffer.SelectObject(CSkinResourceManager::GetDefaultFont());

	//绘画背景
	CImageHandle HandleViewBack(&m_ImageViewBack);
	m_ImageViewBack.BitBlt(DCBuffer,0,0);

	//绘画信息
	if (m_bControl==false)
	{
		//设置 DC
		DCBuffer.SetTextColor(RGB(64,10,10));

		//游戏模式
		TCHAR szModeString[64]=TEXT("");
		LPCTSTR pszGameMode[]={TEXT("快棋模式"),TEXT("慢棋模式"),TEXT("自定义模式")};
		_sntprintf(szModeString,CountChar(szModeString),TEXT("游戏模式：%s"),pszGameMode[m_GameRuleInfo.cbCurrentMode-1]);

		//游戏局时
		TCHAR szDrawString[64]=TEXT("");
		_sntprintf(szDrawString,CountChar(szDrawString),TEXT("游戏局时：%d 分钟"),m_GameRuleInfo.wRuleDrawTime/60);

		//游戏步时
		TCHAR szStepString[64]=TEXT("");
		_sntprintf(szStepString,CountChar(szStepString),TEXT("游戏步时：%d 分钟"),m_GameRuleInfo.wRuleStepTime/60);

		//游戏读秒
		TCHAR szSecondString[64]=TEXT("");
		_sntprintf(szSecondString,CountChar(szSecondString),TEXT("读秒时间：%d 秒"),m_GameRuleInfo.wRuleSecondTime);

		//输出信息
		const INT nXPos=50,nYPos=87,nStringSpace=25;
		DCBuffer.TextOut(nXPos,nYPos,szModeString,lstrlen(szModeString));
		DCBuffer.TextOut(nXPos,nYPos+nStringSpace,szDrawString,lstrlen(szDrawString));
		DCBuffer.TextOut(nXPos,nYPos+nStringSpace*2,szStepString,lstrlen(szStepString));
		DCBuffer.TextOut(nXPos,nYPos+nStringSpace*3,szSecondString,lstrlen(szSecondString));
	}

	//设置 DC
	DCBuffer.SetTextColor(RGB(125,125,125));
	DCBuffer.SetTextAlign(TA_CENTER|TA_TOP);

	//剩余时间
	TCHAR szTimeLeave[32]=TEXT("");
	_sntprintf(szTimeLeave,CountChar(szTimeLeave),TEXT("此对话框在 %d 秒后自动关闭"),m_nResidualTime);
	DCBuffer.TextOut(rcClient.Width()/2,rcClient.bottom-80,szTimeLeave,lstrlen(szTimeLeave));

	//绘画界面
	dc.BitBlt(0,0,rcClient.Width(),rcClient.Height(),&DCBuffer,0,0,SRCCOPY);

	//清理资源
	DCBuffer.DeleteDC();
	ImageBuffer.DeleteObject();

	return;
}

//绘画背景
BOOL CDlgRuleOption::OnEraseBkgnd(CDC * pDC)
{
	Invalidate(FALSE);
	UpdateWindow();

	return TRUE;
}

//销毁消息
VOID CDlgRuleOption::OnDestroy()
{
	//清理资源
	m_Brush.DeleteObject();

	__super::OnDestroy();
}

//时间消息
VOID CDlgRuleOption::OnTimer(UINT nIDEvent)
{
	if (nIDEvent==IDI_GAME_RULE_TIME)
	{
		//关闭判断
		if (m_nResidualTime>0)
		{
			//设置变量
			m_nResidualTime--;

			//更新界面
			Invalidate(FALSE);
		}
		else PostMessage(WM_CLOSE,0,0);

		return;
	}

	__super::OnTimer(nIDEvent);
}

//鼠标消息
VOID CDlgRuleOption::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags,Point);

	//消息模拟
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(Point.x,Point.y));

	return;
}

//控件颜色
HBRUSH CDlgRuleOption::OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor)
{
	switch (nCtlColor)
	{
	case CTLCOLOR_DLG:
	case CTLCOLOR_STATIC:
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(RGB(64,10,10));
			pDC->SetBkColor(COLOR_RULE_BACK);
			return m_Brush;
		}
	}

	return __super::OnCtlColor(pDC,pWnd,nCtlColor);
}

//////////////////////////////////////////////////////////////////////////
