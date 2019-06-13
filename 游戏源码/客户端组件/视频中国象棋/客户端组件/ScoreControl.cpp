#include "StdAfx.h"
#include "GameClient.h"
#include "ScoreControl.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CScoreControl, CWnd)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CScoreControl::CScoreControl()
{
	//状态变量
	ZeroMemory(&m_GameScoreInfo,sizeof(m_GameScoreInfo));

	//加载资源
	HINSTANCE hResInstance=AfxGetInstanceHandle();
	m_ImageWinLose.SetLoadInfo(IDB_WIN_LOSE,hResInstance);
	m_ImageGameScore.SetLoadInfo(IDB_GAME_SCORE,hResInstance);

	return;
}

//析构函数
CScoreControl::~CScoreControl()
{
}

//复位数据
VOID CScoreControl::RestorationData()
{
	//设置变量
	ZeroMemory(&m_GameScoreInfo,sizeof(m_GameScoreInfo));

	//隐藏窗口
	if (m_hWnd!=NULL) ShowWindow(SW_HIDE);

	return;
}

//设置积分
VOID CScoreControl::SetScoreInfo(const tagScoreInfo & ScoreInfo)
{
	//设置变量
	m_GameScoreInfo=ScoreInfo;

	//更新界面
	if (m_hWnd!=NULL) Invalidate(FALSE);

	//显示窗口
	if (m_hWnd!=NULL) ShowWindow(SW_SHOW);

	return;
}

//重画函数
VOID CScoreControl::OnPaint()
{
	CPaintDC dc(this);

	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//创建缓冲
	CDC DCBuffer;
	CBitmap ImageBuffer;
	DCBuffer.CreateCompatibleDC(&dc);
	ImageBuffer.CreateCompatibleBitmap(&dc,rcClient.Width(),rcClient.Height());

	//设置 DC
	DCBuffer.SetBkMode(TRANSPARENT);
	DCBuffer.SelectObject(&ImageBuffer);
	DCBuffer.SetTextColor(RGB(125,125,125));
	DCBuffer.SelectObject(CSkinResourceManager::GetDefaultFont());

	//加载资源
	CImageHandle HandleWinLose(&m_ImageWinLose);
	CImageHandle HandleGameScore(&m_ImageGameScore);

	//变量定义
	TCHAR szBuffer[32]=TEXT("");
	UINT nFormat=DT_SINGLELINE|DT_END_ELLIPSIS|DT_VCENTER|DT_CENTER;

	//绘画背景
	m_ImageGameScore.AlphaDrawImage(&DCBuffer,0,0,RGB(255,0,255));

	//绘画属性
	DCBuffer.SetTextColor(RGB(10,10,10));
	DCBuffer.SetTextAlign(TA_LEFT|TA_TOP);

	//判断十步
	//if(m_GameScoreInfo.cbReason!=REASON_TEN_STEP)
	{
		//绘画信息
		for (BYTE i=0;i<GAME_PLAYER;i++)
		{
			//变量定义
			TCHAR szWinRate[16]=TEXT("");
			TCHAR szUserScore[16]=TEXT("");
			_sntprintf(szWinRate,CountChar(szWinRate),TEXT("%5.2f%%"),m_GameScoreInfo.fWinRate[i]);
			_sntprintf(szUserScore,CountChar(szUserScore),TEXT("%I64d"),m_GameScoreInfo.lGameScore[i]);

			//位置计算
			CRect rcName(76,133+i*52,169,184+i*52);
			CRect rcScore(170,133+i*52,240,184+i*52);
			CRect rcWinRate(241,133+i*52,306,184+i*52);
			CRect rcLevelName(307,133+i*52,382,184+i*52);

			//绘画信息
			DCBuffer.DrawText(szWinRate,lstrlen(szWinRate),&rcWinRate,nFormat);
			DCBuffer.DrawText(szUserScore,lstrlen(szUserScore),&rcScore,nFormat);
			DCBuffer.DrawText(m_GameScoreInfo.szUserName[i],lstrlen(m_GameScoreInfo.szUserName[i]),&rcName,nFormat);
			DCBuffer.DrawText(m_GameScoreInfo.szLevelName[i],lstrlen(m_GameScoreInfo.szLevelName[i]),&rcLevelName,nFormat);

			//输赢标志
			INT nImageWidht=m_ImageWinLose.GetWidth()/3;
			INT nImageHeight=m_ImageWinLose.GetHeight();

			//绘画标志
			INT nImageExcursion=0L;
			if (m_GameScoreInfo.cbReason<REASON_AGREE_DRAW)
			{
				if (m_GameScoreInfo.lGameScore[i]<0L) nImageExcursion=nImageWidht;
			}
			else nImageExcursion=nImageWidht*2;
			m_ImageWinLose.AlphaDrawImage(&DCBuffer,22,138+i*52,nImageWidht,nImageHeight,nImageExcursion,0,RGB(255,0,255));
		}
	}

	//创建字体
	CFont InfoFont;
	InfoFont.CreateFont(-14,0,0,0,700,0,0,0,134,3,2,1,2,TEXT("宋体"));

	//加载资源
	CFont * pOldFont = DCBuffer.SelectObject(&InfoFont);
	DCBuffer.SetTextColor(RGB(60,60,60));
	DCBuffer.SetTextAlign(TA_CENTER|TA_TOP);

	//信息内容
	TCHAR tCh[256] = TEXT("");

	//结束原因
	WORD wMeChiarID = m_GameScoreInfo.wMeChiarID;
	//m_GameScoreInfo.cbReason = REASON_CHESS_FEAZE;
	//m_GameScoreInfo.lGameScore[wMeChiarID]=-10;
	switch(m_GameScoreInfo.cbReason)
	{
	case REASON_CATCH_CHESS:		//长捉判输
		{
			if (m_GameScoreInfo.lGameScore[wMeChiarID]<0L)
				_sntprintf(tCh,sizeof(tCh),TEXT("游戏结束:长捉犯规 T_T"));
			else _sntprintf(tCh,sizeof(tCh),TEXT("游戏结束:对方长捉犯规"));
			break;
		}
	case REASON_CATCH_GENERA:		//长将判输
		{
			if (m_GameScoreInfo.lGameScore[wMeChiarID]<0L)
				_sntprintf(tCh,sizeof(tCh),TEXT("游戏结束:本方长将犯规 T_T"));
			else _sntprintf(tCh,sizeof(tCh),TEXT("游戏结束:对方长将犯规"));
			break;
		}
	case REASON_CHESS_FEAZE:		//困毙判输
		{
			if (m_GameScoreInfo.lGameScore[wMeChiarID]<0L)
				_sntprintf(tCh,sizeof(tCh),TEXT("本方被困毙了 T_T"));
			else _sntprintf(tCh,sizeof(tCh),TEXT("困毙对方,获胜! ＾.＾"));
			break;
		}
	case REASON_KILL_GENERA:		//将死判输
		{
			if (m_GameScoreInfo.lGameScore[wMeChiarID]<0L)
				_sntprintf(tCh,sizeof(tCh),TEXT("本方被将死了 T_T"));
			else _sntprintf(tCh,sizeof(tCh),TEXT("将死对方,获胜! ＾.＾"));
			break;
		}
	//case REASON_GIVE_UP:			//认输结束
	//	{
	//		if (m_GameScoreInfo.lGameScore[wMeChiarID]>=0L)
	//			_sntprintf(tCh,sizeof(tCh),TEXT("对方认输"));
	//		break;
	//	}
	case REASON_OVER_TIME:			//超时判输
		{
			if (m_GameScoreInfo.lGameScore[wMeChiarID]<0L)
				_sntprintf(tCh,sizeof(tCh),TEXT("超时输了 T_T"));
			//else _sntprintf(tCh,sizeof(tCh),TEXT("获胜:对方超时 ＾.＾"));
			break;
		}
	case REASON_USER_EXIT:			//强退判输
		{
			_sntprintf(tCh,sizeof(tCh),TEXT("对方强退"));
			break;
		}
	//case REASON_AGREE_DRAW:			//同意和局
	//	{
	//		_sntprintf(tCh,sizeof(tCh),TEXT("同意和棋,双方判和"));
	//		break;
	//	}
	case REASON_OVER_STEP:			//满步和局
		{
			_sntprintf(tCh,sizeof(tCh),TEXT("游戏结束:总步数达到400步,双方判和"));
			break;
		}
	case REASON_LEISURE_STEP:		//空步和局
		{
			_sntprintf(tCh,sizeof(tCh),TEXT("游戏结束:无吃子步数达到120步,双方判和."));
			break;
		}
	case REASON_TEN_STEP:			//十步以内
		{
			_sntprintf(tCh,sizeof(tCh),TEXT("总步数在10步以内,输赢不计分."));
			break;
		}
	case REASON_TWENTY_STEP:		//二十以内
		{
			_sntprintf(tCh,sizeof(tCh),TEXT("总步数在10步-20步之间,赢家不得分."));
			break;
		}
	}

	//绘画信息
	//DCBuffer.TextOut(200,65,tCh);

	//清理资源
	DCBuffer.SelectObject(pOldFont);
	InfoFont.DeleteObject();

	//绘画界面
	dc.BitBlt(0,0,rcClient.Width(),rcClient.Height(),&DCBuffer,0,0,SRCCOPY);

	//清理资源
	DCBuffer.DeleteDC();
	ImageBuffer.DeleteObject();

	return;
}

//绘画背景
BOOL CScoreControl::OnEraseBkgnd(CDC * pDC)
{
	Invalidate(FALSE);
	UpdateWindow();

	return TRUE;
}

//创建函数
INT CScoreControl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//设置背景
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);

	//移动窗口
	CImageHandle ImageHandle(&m_ImageGameScore);
	SetWindowPos(NULL,0,0,m_ImageGameScore.GetWidth(),m_ImageGameScore.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);

	////创建区域
	//CRgn RgnImage;
	//CImageHandle HandleGameScore(&m_ImageGameScore);
	//m_ImageGameScore.CreateImageRegion(RgnImage,RGB(255,0,255));

	////设置窗口
	//SetWindowRgn(RgnImage,TRUE);
	//SetWindowPos(NULL,0,0,m_ImageGameScore.GetWidth(),m_ImageGameScore.GetHeight(),SWP_NOZORDER|SWP_NOMOVE);

	return 0;
}

//鼠标消息
VOID CScoreControl::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags,Point);

	//消息模拟
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(Point.x,Point.y));

	return;
}

//////////////////////////////////////////////////////////////////////////
