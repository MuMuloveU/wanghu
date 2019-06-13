#include "Stdafx.h"
#include "Resource.h"
#include "ChessCountInfo.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CChessCountInfo, CDialog)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CChessCountInfo::CChessCountInfo() : CDialog(IDD_CHESS_COUNT)
{
	//设置变量
	m_fBlackCount=0;
	m_fWhiteCount=0;
	m_fCommomCount=0;

	//加载资源
	m_ImageBack.SetLoadInfo(IDB_CHESS_COUNT_INFO,AfxGetInstanceHandle());

	return;
}

//析构函数
CChessCountInfo::~CChessCountInfo()
{
}

//控件绑定
void CChessCountInfo::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btOk);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
}

//初始化函数
BOOL CChessCountInfo::OnInitDialog()
{
	__super::OnInitDialog();

	//创建画刷
	m_Brush.CreateSolidBrush(RGB(245,238,221));

	//设置标题
	SetWindowText(TEXT("点目结果："));

	//移动窗口
	CImageHandle BackImageHandle(&m_ImageBack);
	SetWindowPos(NULL,0,0,m_ImageBack.GetWidth(),m_ImageBack.GetHeight(),SWP_NOZORDER|SWP_NOMOVE);

	//设置按钮
	m_btOk.SetButtonImage(IDB_COUNT_BT_ACCEPT,AfxGetInstanceHandle(),false);
	m_btCancel.SetButtonImage(IDB_COUNT_BT_CONTINUE,AfxGetInstanceHandle(),false);

	return TRUE;
}

//确定消息
void CChessCountInfo::OnOK()
{
	//发送消息
	AfxGetMainWnd()->PostMessage(IDM_CHESS_COUNT_RESPONSION,1,1);

	//关闭窗口
	DestroyWindow();

	return;
}

//取消消息
void CChessCountInfo::OnCancel()
{
	//发送消息
	AfxGetMainWnd()->PostMessage(IDM_CHESS_COUNT_RESPONSION,0,0);

	//关闭窗口
	DestroyWindow();

	return;
}

//设置信息
void CChessCountInfo::SetChessCountInfo(float fBlackCount, float fWhiteCount, float fCommomCount)
{
	//设置变量
	m_fBlackCount=fBlackCount;
	m_fWhiteCount=fWhiteCount;
	m_fCommomCount=fCommomCount;

	//更新界面
	if (GetSafeHwnd()!=NULL) Invalidate(FALSE);

	return;
}

//重画函数
void CChessCountInfo::OnPaint()
{
	CPaintDC dc(this);

	//绘画背景
	CImageHandle BackImageHandle(&m_ImageBack);
	m_ImageBack.BitBlt(dc,0,0);

	//设置 DC
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(10,10,10));
	dc.SelectObject(CSkinResourceManager::GetDefaultFont());

	//绘画信息
	TCHAR szBuffer[64];
	_snprintf(szBuffer,sizeof(szBuffer),TEXT("%0.1f"),m_fBlackCount);
	dc.TextOut(133,57,szBuffer,lstrlen(szBuffer));
	_snprintf(szBuffer,sizeof(szBuffer),TEXT("%0.1f"),m_fWhiteCount);
	dc.TextOut(133,83,szBuffer,lstrlen(szBuffer));
	_snprintf(szBuffer,sizeof(szBuffer),TEXT("%0.1f"),m_fCommomCount);
	dc.TextOut(133,110,szBuffer,lstrlen(szBuffer));

	return;
}

//绘画背景
BOOL CChessCountInfo::OnEraseBkgnd(CDC * pDC)
{
	Invalidate(FALSE);
	UpdateWindow();

	return TRUE;
}

//销毁消息
void CChessCountInfo::OnDestroy()
{
	//清理资源
	m_Brush.DeleteObject();

	__super::OnDestroy();
}

//鼠标消息
void CChessCountInfo::OnLButtonDown(UINT nFlags, CPoint point)
{
	__super::OnLButtonDown(nFlags,point);

	//消息模拟
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));

	return;
}

//控件颜色
HBRUSH CChessCountInfo::OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor)
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
