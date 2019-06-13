#include "StdAfx.h"
#include "Resource.h"
#include "GameClientView.h"


//////////////////////////////////////////////////////////////////////////
//按钮标识 

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameView)
	ON_WM_TIMER()
	ON_WM_CREATE()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientView::CGameClientView(void) : CGameFrameView(true,24)
{
	//游戏变量


	//配置变量
	m_bDeasilOrder=true;

	//状态变量


	//位置信息


	//加载资源

	//计算位置

	return;
}

//析构函数
CGameClientView::~CGameClientView(void)
{
}

//建立消息
int CGameClientView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{	
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;


	//创建控件
	CRect CreateRect(0,0,0,0);
	m_ScoreView.Create(IDD_GAME_SCORE,this);


	//读取配置
	m_bDeasilOrder=AfxGetApp()->GetProfileInt(TEXT("GameOption"),TEXT("DeasilOrder"),FALSE)?true:false;

	return 0;
}


//重置界面
void CGameClientView::ResetGameView()
{
	//游戏变量


	//状态变量

	//隐藏控件
	m_ScoreView.ShowWindow(SW_HIDE);

	//隐藏按钮

	//禁用控件

	//扑克控件

	return;
}

//调整控件
void CGameClientView::RectifyGameView(int nWidth, int nHeight)
{

	//调整大小
	if(m_DxDialog.GetSafeHwnd()!=NULL)
	{
		//m_DxDialog.MoveWindow(0,0,nWidth,nHeight);
		m_DxDialog.CenterWindow(this);
		//m_DxDialog.UpdateDxWindow(815,706);
	}

	//创建dx窗口
	if(m_DxDialog.GetSafeHwnd()==NULL)
	{
		/*if(nWidth<813||nHeight<704) 
		{
			AfxMessageBox("您的分辨率过低，不能支持此游戏窗口，程序即将退出。");
			AfxGetMainWnd()->SendMessage(WM_CLOSE);
			return;
		}*/
		//m_DxDialog.Create(AfxRegisterWndClass(0,NULL,NULL,NULL),NULL,WS_CHILD|WS_VISIBLE,CRect(0,0,nWidth,nHeight ),this,0);
		m_DxDialog.Create(AfxRegisterWndClass(0,NULL,NULL,NULL),NULL,WS_CHILD|WS_VISIBLE,CRect(0,0,813,709 ),this,0);
		m_DxDialog.CenterWindow(this);
		SetWindowLong(m_DxDialog.GetSafeHwnd(),GWL_STYLE,GetStyle()|WS_CLIPCHILDREN);
		bool bSuccess=m_DxDialog.InitDxWindow(813,709);
		if(bSuccess==false) 
		{
			AfxMessageBox("D3D设备初始化失败，程序即将退出。");
			AfxGetMainWnd()->SendMessage(WM_CLOSE);
			return;
		}
	}

	return;
}

//绘画界面
void CGameClientView::DrawGameView(CDC * pDC, int nWidth, int nHeight)
{
	//绘画背景

	return;
}

//用户顺序
void CGameClientView::SetUserOrder(bool bDeasilOrder)
{
	//状态判断
	if (m_bDeasilOrder==bDeasilOrder) return;

	//设置变量
	m_bDeasilOrder=bDeasilOrder;
	AfxGetApp()->WriteProfileInt(TEXT("GameOption"),TEXT("DeasilOrder"),m_bDeasilOrder?TRUE:FALSE);

	//设置界面
	CRect rcClient;
	GetClientRect(&rcClient);
	RectifyGameView(rcClient.Width(),rcClient.Height());

	//刷新界面
	UpdateGameView(NULL);

	return;
}



//开始按钮
void CGameClientView::OnStart()
{
	AfxGetMainWnd()->SendMessage(IDM_START,0,0);
	return;
}

//设置界面
void CGameClientView::SetUserCome(BYTE bHistoryInfo[],long lMinBetScore,long lMaxBetWinScore,long lMaxUserScore,bool bLookOnUser)
{
	m_DxDialog.m_btChipIn.ShowWindow(SW_SHOW);
	m_DxDialog.m_bAllowBet=true;
	m_DxDialog.m_lMinBetScore=lMinBetScore;
	m_DxDialog.m_lMaxBetWin=lMaxBetWinScore;
	m_DxDialog.m_lMaxUserScore=lMaxUserScore;
	m_DxDialog.ClearAllMoney();
	m_DxDialog.SetHistoryNumber(bHistoryInfo);

	//设置旁观用户
	if(bLookOnUser) 
	{
		m_DxDialog.m_bAllowBet=false;
		m_DxDialog.m_btChipIn.ShowWindow(SW_HIDE);
	}

	//刷新界面
	UpdateGameView(NULL);
}

//开始旋转
void CGameClientView::SetGameStart(BYTE bTargetNumber)
{
	m_DxDialog.OnGameStart(bTargetNumber);
}
//所有下注
void CGameClientView::SetUserChip(long lAllChip[256])
{
	m_DxDialog.OnUserChip(lAllChip);
}
//结束旋转
void CGameClientView::SetRollStop(BYTE bWinNumber)
{
	m_DxDialog.OnRollStop(bWinNumber);
}
//显示结算
void CGameClientView::SetGameEnd(long lScore)
{
	m_DxDialog.m_lGameEndScore=lScore;
	if(lScore>=0)
	{
		m_DxDialog.m_bShowEnd=1;
	}
	if(lScore<0)
	{
		m_DxDialog.m_bShowEnd=2;
	}
	m_DxDialog.OnGameEnd();
	SetTimer(100,3000,NULL);
}

//设置倍率
void CGameClientView::SetCellScore(long lCellScore)
{
	m_DxDialog.OnSetCellScore(lCellScore);
}

//定时消息
void CGameClientView::OnTimer(UINT_PTR nIDEvent)
{
	switch(nIDEvent)
	{
	case 100:
		{
			m_DxDialog.m_bShowEnd=0;
			m_DxDialog.m_bDrawMoveChips=false;
			m_DxDialog.m_iMoveChipsCount=0;
			KillTimer(100);
			break;
		}
	default:
		break;
	}
	return;
}