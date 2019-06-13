#include "Stdafx.h"
#include "ChessSelect.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CChessSelect, CDialog)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_ROOK, OnRook)
	ON_BN_CLICKED(IDC_HORSE, OnHorse)
	ON_BN_CLICKED(IDC_QUEEN, OnQueen)
	ON_BN_CLICKED(IDC_ELEPHANT, OnElephant)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CChessSelect::CChessSelect() : CDialog(IDD_CHESS_SELECT)
{
	//加载资源
	m_ImageBack.SetLoadInfo(IDB_CHESS_SELECT,AfxGetInstanceHandle());

	return;
}

//析构函数
CChessSelect::~CChessSelect()
{
}

//控件绑定
void CChessSelect::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ROOK, m_btRook);
	DDX_Control(pDX, IDC_HORSE, m_btHorse);
	DDX_Control(pDX, IDC_QUEEN, m_btQueen);
	DDX_Control(pDX, IDC_ELEPHANT, m_btElephant);
}

//初始化函数
BOOL CChessSelect::OnInitDialog()
{
	__super::OnInitDialog();

	//设置按钮
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_btRook.SetButtonImage(IDB_CHESS_ROOK,hInstance,false);
	m_btQueen.SetButtonImage(IDB_CHESS_QUEEN,hInstance,false);
	m_btHorse.SetButtonImage(IDB_CHESS_HORSE,hInstance,false);
	m_btElephant.SetButtonImage(IDB_CHESS_ELEPHANT,hInstance,false);

	//调整位置
	CImageHandle ImageBack(&m_ImageBack);
	SetWindowPos(NULL,0,0,m_ImageBack.GetWidth(),m_ImageBack.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);

	//获取位置
	CRect rcButton;
	m_btRook.GetWindowRect(&rcButton);
	int nXPos=(m_ImageBack.GetWidth()-rcButton.Width()*4-15)/2;

	//移动按钮
	HDWP hDwp=BeginDeferWindowPos(4);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;
	DeferWindowPos(hDwp,m_btQueen,NULL,nXPos,33,0,0,uFlags);
	DeferWindowPos(hDwp,m_btRook,NULL,nXPos+rcButton.Width()+5,33,0,0,uFlags);
	DeferWindowPos(hDwp,m_btHorse,NULL,nXPos+rcButton.Width()*2+10,33,0,0,uFlags);
	DeferWindowPos(hDwp,m_btElephant,NULL,nXPos+rcButton.Width()*3+15,33,0,0,uFlags);
	EndDeferWindowPos(hDwp);

	//居中窗口
	CenterWindow(GetParent());

	return TRUE;
}

//重画函数
void CChessSelect::OnPaint()
{
	CPaintDC dc(this);

	//绘画背景
	CImageHandle ImageBack(&m_ImageBack);
	m_ImageBack.BitBlt(dc,0,0);

	return;
}

//绘画背景
BOOL CChessSelect::OnEraseBkgnd(CDC * pDC)
{
	return TRUE;
}

//鼠标消息
void CChessSelect::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags,Point);

	//模拟按标题
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(Point.x,Point.y));

	return;
}

//棋子按钮
void CChessSelect::OnRook()
{
	//发送消息
	AfxGetMainWnd()->PostMessage(WM_SELECT_CHESS,CHESS_ROOK,CHESS_ROOK);

	//隐藏窗口
	ShowWindow(SW_HIDE);

	return;
}

//棋子按钮
void CChessSelect::OnHorse()
{
	//发送消息
	AfxGetMainWnd()->PostMessage(WM_SELECT_CHESS,CHESS_HORSE,CHESS_HORSE);

	//隐藏窗口
	ShowWindow(SW_HIDE);

	return;
}

//棋子按钮
void CChessSelect::OnQueen()
{
	//发送消息
	AfxGetMainWnd()->PostMessage(WM_SELECT_CHESS,CHESS_QUEEN,CHESS_QUEEN);

	//隐藏窗口
	ShowWindow(SW_HIDE);

	return;
}

//棋子按钮
void CChessSelect::OnElephant()
{
	//发送消息
	AfxGetMainWnd()->PostMessage(WM_SELECT_CHESS,CHESS_ELEPHANT,CHESS_ELEPHANT);

	//隐藏窗口
	ShowWindow(SW_HIDE);

	return;
}

//////////////////////////////////////////////////////////////////////////
