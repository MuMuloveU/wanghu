#include "Stdafx.h"
#include "GameOption.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameOption, CDialog)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameOption::CGameOption() : CDialog(IDD_OPTION)
{
	//设置变量
	m_bEnableSound=true;
	m_bAutoSaveManual=false;

	//加载资源
	m_ImageBack.SetLoadInfo(IDB_GAME_OPTION,AfxGetInstanceHandle());

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
}

//初始化函数
BOOL CGameOption::OnInitDialog()
{
	__super::OnInitDialog();

	//设置标题
	SetWindowText(TEXT("游戏配置"));

	//创建画刷
	m_Brush.CreateSolidBrush(RGB(245,238,221));

	//移动窗口
	CImageHandle BackImageHandle(&m_ImageBack);
	SetWindowPos(NULL,0,0,m_ImageBack.GetWidth(),m_ImageBack.GetHeight(),SWP_NOZORDER|SWP_NOMOVE);

	//设置按钮
	m_btOK.SetButtonImage(IDB_OPTION_BT_OK,AfxGetInstanceHandle(),false);
	m_btCancel.SetButtonImage(IDB_OPTION_BT_CANCEL,AfxGetInstanceHandle(),false);

	//设置控件
	if (m_bEnableSound==true) ((CButton *)GetDlgItem(IDC_ENABLE_SOUND))->SetCheck(BST_CHECKED);
	((CButton *)GetDlgItem((m_bAutoSaveManual==true)?IDC_AUTO_SAVE_MANUAL:IDC_NO_AUTO_SAVE_MANUAL))->SetCheck(BST_CHECKED);

	return TRUE;
}

//确定消息
void CGameOption::OnOK()
{
	//获取变量
	m_bEnableSound=(((CButton *)GetDlgItem(IDC_ENABLE_SOUND))->GetCheck()==BST_CHECKED);
	m_bAutoSaveManual=(((CButton *)GetDlgItem(IDC_AUTO_SAVE_MANUAL))->GetCheck()==BST_CHECKED);

	__super::OnOK();
}

//重画函数
void CGameOption::OnPaint()
{
	CPaintDC dc(this);

	//绘画背景
	CImageHandle BackImageHandle(&m_ImageBack);
	m_ImageBack.BitBlt(dc,0,0);

	return;
}

//绘画背景
BOOL CGameOption::OnEraseBkgnd(CDC * pDC)
{
	Invalidate(FALSE);
	UpdateWindow();

	return TRUE;
}

//销毁消息
void CGameOption::OnDestroy()
{
	//清理资源
	m_Brush.DeleteObject();

	__super::OnDestroy();
}

//鼠标消息
void CGameOption::OnLButtonDown(UINT nFlags, CPoint point)
{
	__super::OnLButtonDown(nFlags,point);

	//消息模拟
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));

	return;
}

//控件颜色
HBRUSH CGameOption::OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor)
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

//////////////////////////////////////////////////////////////////////////
