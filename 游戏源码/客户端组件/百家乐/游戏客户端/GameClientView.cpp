#include "StdAfx.h"
#include "Math.h"
#include "Resource.h"
#include "GameClientView.h"
#include ".\gameclientview.h"

//////////////////////////////////////////////////////////////////////////

//时间标识
#define IDI_FLASH_WINNER			100									//闪动标识
#define IDI_SHOW_CHANGE_BANKER		101									//轮换庄家

//按钮标识
#define IDC_JETTON_BUTTON_500		200									//按钮标识
#define IDC_JETTON_BUTTON_50000		201									//按钮标识
#define IDC_JETTON_BUTTON_100		202									//按钮标识
#define IDC_JETTON_BUTTON_1000		203									//按钮标识
#define IDC_JETTON_BUTTON_10000		204									//按钮标识
#define IDC_JETTON_BUTTON_100000	205									//按钮标识
#define IDC_APPY_BANKER				206									//按钮标识
#define IDC_CANCEL_BANKER			207									//按钮标识
#define IDC_JETTON_BUTTON_500000	208									//按钮标识
#define IDC_SCORE_MOVE_L			209									//按钮标识
#define IDC_SCORE_MOVE_R			210									//按钮标识

//庄家信息
#define BANKER_INFO_LEN				150									//庄家信息


//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameView)
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_BN_CLICKED(IDC_JETTON_BUTTON_500, OnJettonButton500)
	ON_BN_CLICKED(IDC_JETTON_BUTTON_50000, OnJettonButton50000)
	ON_BN_CLICKED(IDC_JETTON_BUTTON_500000, OnJettonButton500000)
	ON_BN_CLICKED(IDC_JETTON_BUTTON_100, OnJettonButton100)
	ON_BN_CLICKED(IDC_JETTON_BUTTON_1000, OnJettonButton1000)
	ON_BN_CLICKED(IDC_JETTON_BUTTON_10000, OnJettonButton10000)
	ON_BN_CLICKED(IDC_JETTON_BUTTON_100000, OnJettonButton100000)
	ON_BN_CLICKED(IDC_APPY_BANKER, OnApplyBanker)
	ON_BN_CLICKED(IDC_CANCEL_BANKER, OnCancelBanker)
	ON_BN_CLICKED(IDC_SCORE_MOVE_L, OnScoreMoveL)
	ON_BN_CLICKED(IDC_SCORE_MOVE_R, OnScoreMoveR)

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientView::CGameClientView() : CGameFrameView(true,24)
{
	//状态信息
	m_lCurrentJetton=0L;
	m_cbWinnerSide=0xFF;
	m_cbWinnerFlash=0xFF;
	m_cbKingWinner=0xFF;
	m_wMeChairID=INVALID_CHAIR;
	m_cbPreJettonArea = 255;

	m_bShowGameResult = false;
	m_lMeCurGameScore=0L;			
	m_lMeCurGameReturnScore=0L;	
	m_lBankerCurGameScore=0L;		

	m_lAreaLimitScore=0L;	

	m_bDispatchCard = false;

	//下注信息
	m_lAllTieScore=0L;			
	m_lAllBankerScore=0L;		
	m_lAllPlayerScore=0L;		
	m_lAllTieSamePointScore=0L;
	m_lAllBankerKingScore=0L;	
	m_lAllPlayerKingScore=0L;

	//位置信息
	m_nScoreHead = 0;
	m_nRecordFirst= 0;	
	m_nRecordLast= 0;	

	//历史成绩
	m_wDrawCount=0;
	m_lMeResultCount=0;

	//下注信息
	m_lMeMaxScore=0L;
	m_lMeTieScore=0L;
	m_lMeBankerScore=0L;
	m_lMePlayerScore=0L;
	m_lMeTieSamePointScore=0L;
	m_lMePlayerKingScore=0L;
	m_lMeBankerKingScore=0L;

	//庄家信息
	m_bShowChangeBanker = false;
	m_wCurrentBankerChairID = INVALID_CHAIR;
	m_cbBankerTime = 0;			
	m_lBankerScore = 0;		
	m_lBankerTreasure=0;


	//加载位图
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_ImageViewFill.SetLoadInfo(IDB_VIEW_FILL,hInstance);
	m_ImageViewBack.SetLoadInfo(IDB_VIEW_BACK,hInstance);
	m_ImageWinFlags.SetLoadInfo(IDB_WIN_FLAGS,hInstance);
	m_ImageJettonView.SetLoadInfo(IDB_JETTOM_VIEW,hInstance);
	m_ImageScoreNumber.SetLoadInfo(IDB_SCORE_NUMBER,hInstance);
	m_ImageMeScoreNumber.SetLoadInfo(IDB_ME_SCORE_NUMBER,hInstance);

	m_ImageFrameXianJia.SetLoadInfo( IDB_FRAME_XIAN_JIA, hInstance );
	m_ImageFrameZhuangJia.SetLoadInfo( IDB_FRAME_ZHUANG_JIA, hInstance );
	m_ImageFrameXianTianWang.SetLoadInfo( IDB_FRAME_XIAN_TIAN_WANG, hInstance );
	m_ImageFrameZhuangTianWang.SetLoadInfo( IDB_FRAME_ZHUANG_TIAN_WANG, hInstance );
	m_ImageFramePingJia.SetLoadInfo( IDB_FRAME_PING_JIA, hInstance );
	m_ImageFrameTongDianPing.SetLoadInfo( IDB_FRAME_TONG_DIAN_PING, hInstance );

	m_ImageGameEnd.SetLoadInfo( IDB_GAME_END, hInstance );
	m_ImageGameEndFrame.SetLoadInfo( IDB_GAME_END_FRAME, hInstance );
	m_ImageGamePoint.SetLoadInfo( IDB_GAME_POINT, hInstance );
	m_ImageCard.SetLoadInfo(IDB_GAME_CARD,hInstance);

	m_ImageMeBanker.SetLoadInfo( IDB_ME_BANKER, hInstance );
	m_ImageChangeBanker.SetLoadInfo( IDB_CHANGE_BANKER, hInstance );
	m_ImageNoBanker.SetLoadInfo( IDB_NO_BANKER, hInstance );	

	m_ImageTimeFlag.SetLoadInfo(IDB_TIME_FLAG, hInstance);

	GetMaxTieScore();


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
	CRect rcCreate(0,0,0,0);
	m_GameRecord.Create(IDD_DLG_GAME_RECORD,this);
	m_ApplyUser.Create( IDD_DLG_GAME_RECORD	, this );
	m_GameRecord.ShowWindow(SW_HIDE);
	
	m_CardControl[0].Create(NULL,NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,100);
	m_CardControl[1].Create(NULL,NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,101);

	//创建按钮
	m_btJetton100.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_100);
	m_btJetton1000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_1000);
	m_btJetton10000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_10000);
	m_btJetton100000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_100000);
	m_btJetton500.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_500);
	m_btJetton50000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_50000);
	m_btJetton500000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_500000);	
	
	m_btApplyBanker.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_APPY_BANKER);
	m_btCancelBanker.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_CANCEL_BANKER);
	m_btApplyBanker.ShowWindow(SW_SHOW);
	m_btApplyBanker.EnableWindow(FALSE);
	m_btCancelBanker.ShowWindow(SW_HIDE);

	m_btScoreMoveL.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_SCORE_MOVE_L);
	m_btScoreMoveR.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_SCORE_MOVE_R);

	//设置按钮
	HINSTANCE hResInstance=AfxGetInstanceHandle();
	m_btJetton100.SetButtonImage(IDB_BT_JETTON_100,hResInstance,false);
	m_btJetton1000.SetButtonImage(IDB_BT_JETTON_1000,hResInstance,false);
	m_btJetton10000.SetButtonImage(IDB_BT_JETTON_10000,hResInstance,false);
	m_btJetton100000.SetButtonImage(IDB_BT_JETTON_100000,hResInstance,false);
	m_btJetton500.SetButtonImage(IDB_BT_JETTON_500,hResInstance,false);	
	m_btJetton50000.SetButtonImage(IDB_BT_JETTON_50000,hResInstance,false);
	m_btJetton500000.SetButtonImage(IDB_BT_JETTON_500000,hResInstance,false);	
	
	m_btApplyBanker.SetButtonImage(IDB_BT_APPLY_BANKER,hResInstance,false);
	m_btCancelBanker.SetButtonImage(IDB_BT_CANCEL_APPLY,hResInstance,false);

	m_btScoreMoveL.SetButtonImage(IDB_BT_SCORE_MOVE_L,hResInstance,false);
	m_btScoreMoveR.SetButtonImage(IDB_BT_SCORE_MOVE_R,hResInstance,false);

	return 0;
}

//重置界面
void CGameClientView::ResetGameView()
{
	//庄家信息
	m_bShowChangeBanker = false;
	m_wCurrentBankerChairID = INVALID_CHAIR;
	m_cbBankerTime = 0;
	m_lBankerScore = 0;	
	m_lMeMaxScore=0L;
	m_lMeTieScore=0L;
	m_lMeBankerScore=0L;
	m_lMePlayerScore=0L;
	m_lMeTieSamePointScore=0L;
	m_lMePlayerKingScore=0L;
	m_lMeBankerKingScore=0L;


	m_bShowGameResult = false;
	m_lMeCurGameScore=0L;			
	m_lMeCurGameReturnScore=0L;	
	m_lBankerCurGameScore=0L;
	m_nScoreHead = 0;
	m_nRecordFirst= 0;	
	m_nRecordLast= 0;	
	m_lBankerTreasure=0;

	m_bDispatchCard = false;
	m_strDispatchCardTips=TEXT("");
    
	m_lAreaLimitScore=0L;	

	//下注信息
	m_lAllTieScore=0L;			
	m_lAllBankerScore=0L;		
	m_lAllPlayerScore=0L;		
	m_lAllTieSamePointScore=0L;
	m_lAllBankerKingScore=0L;	
	m_lAllPlayerKingScore=0L;	

	//清空列表
	m_ApplyUser.ClearAll();

	//设置按钮
	m_btApplyBanker.ShowWindow(SW_SHOW);
	m_btApplyBanker.EnableWindow(FALSE);
	m_btCancelBanker.ShowWindow(SW_HIDE);
	m_btCancelBanker.SetButtonImage(IDB_BT_CANCEL_APPLY,AfxGetInstanceHandle(),false);

	return;
}

//调整控件
void CGameClientView::RectifyGameView(int nWidth, int nHeight)
{
	//位置信息
	m_nWinFlagsExcursionX = nWidth/2-148;	
	m_nWinFlagsExcursionY = nHeight/2 + 251;
	m_nWinPointsExcursionX = nWidth/2-353;
	m_nWinPointsExcursionY = nHeight/2 - 267;	

	//筹码位置
	m_PointJetton[0].SetPoint(nWidth / 2 - 314 + 94,nHeight / 2 - 159 +104);
	m_PointJetton[1].SetPoint(nWidth / 2 - 130 + 133,nHeight / 2 - 60+54);
	m_PointJetton[2].SetPoint(nWidth / 2 + 132+93,nHeight / 2 - 159+104);
	m_PointJetton[3].SetPoint(nWidth / 2 - 314+94,nHeight / 2 + 45+43);
	m_PointJetton[4].SetPoint(nWidth / 2 + 132+93,nHeight / 2 + 45+43);
	m_PointJetton[5].SetPoint(nWidth / 2 - 130+133,nHeight / 2 + 45+43);

	//扑克控件
	m_CardControl[0].SetBenchmarkPos(CPoint(nWidth/2-25+15,nHeight/2-235+66),enXRight,enYTop);
	m_CardControl[1].SetBenchmarkPos(CPoint(nWidth/2+25-15,nHeight/2-235+66),enXLeft,enYTop);

	//移动控件
	HDWP hDwp=BeginDeferWindowPos(32);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS;

	//列表控件
	DeferWindowPos(hDwp,m_ApplyUser,NULL,nWidth/2 + 118,nHeight/2-314,256,80,uFlags);

	//筹码按钮
	CRect rcJetton;
	m_btJetton100.GetWindowRect(&rcJetton);
	int nYPos = nHeight/2+145;
	int nXPos = nWidth/2-160+8;
	int nSpace = 2;
	DeferWindowPos(hDwp,m_btJetton100,NULL,nXPos,nYPos,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btJetton500,NULL,nXPos + nSpace + rcJetton.Width(),nYPos,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btJetton1000,NULL,nXPos + nSpace * 2 + rcJetton.Width() * 2,nYPos,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btJetton10000,NULL,nXPos + nSpace * 3 + rcJetton.Width() * 3,nYPos,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btJetton50000,NULL,nXPos + nSpace * 4 + rcJetton.Width() * 4,nYPos,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btJetton100000,NULL,nXPos + nSpace * 5 + rcJetton.Width() * 5,nYPos,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btJetton500000,NULL,nXPos + nSpace * 6 + rcJetton.Width() * 6,nYPos,0,0,uFlags|SWP_NOSIZE);

	//上庄按钮
	DeferWindowPos(hDwp,m_btApplyBanker,NULL,nWidth/2+280,nHeight/2-352,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btCancelBanker,NULL,nWidth/2+280,nHeight/2-352,0,0,uFlags|SWP_NOSIZE);

	DeferWindowPos(hDwp,m_btScoreMoveL,NULL,nWidth/2-203,nHeight/2+272,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btScoreMoveR,NULL,nWidth/2+302,nHeight/2+270,0,0,uFlags|SWP_NOSIZE);

	//结束移动
	EndDeferWindowPos(hDwp);

	return;
}

//绘画界面
void CGameClientView::DrawGameView(CDC * pDC, int nWidth, int nHeight)
{
	//绘画背景
	DrawViewImage(pDC,m_ImageViewFill,enMode_Spread);
	DrawViewImage(pDC,m_ImageViewBack,enMode_Centent);

	//胜利标志
	DrawWinFlags(pDC);

	//显示结果
	ShowGameResult(pDC, nWidth, nHeight);	

	//发牌提示
	if ( ! m_strDispatchCardTips.IsEmpty() )
	{
		pDC->SetTextColor(RGB(255,234,0));
		CRect rcDispatchCardTips(m_nWinPointsExcursionX, m_nWinPointsExcursionY+50, m_nWinPointsExcursionX + 220, m_nWinPointsExcursionY+115);
		pDC->DrawText( m_strDispatchCardTips, rcDispatchCardTips, DT_END_ELLIPSIS | DT_LEFT | DT_TOP| DT_WORDBREAK );
	}

	//时间标识
	bool bDispatchCard = (0 < m_CardControl[0].GetCardCount() || 0 < m_CardControl[1].GetCardCount() ) ? true : false;
	CImageHandle ImageHandleTimeFlag(&m_ImageTimeFlag);
	int nTimeFlagWidth = m_ImageTimeFlag.GetWidth()/3;
	if ( bDispatchCard ) 
		m_ImageTimeFlag.BitBlt(pDC->GetSafeHdc(), nWidth/2-230, nHeight/2+195, nTimeFlagWidth, m_ImageTimeFlag.GetHeight(),
		2 * nTimeFlagWidth, 0);
	else if ( m_wCurrentBankerChairID != INVALID_CHAIR )
		m_ImageTimeFlag.BitBlt(pDC->GetSafeHdc(), nWidth/2-230, nHeight/2+195, nTimeFlagWidth, m_ImageTimeFlag.GetHeight(),
		nTimeFlagWidth, 0);
	else 
		m_ImageTimeFlag.BitBlt(pDC->GetSafeHdc(), nWidth/2-230, nHeight/2+195, nTimeFlagWidth, m_ImageTimeFlag.GetHeight(), 
		0, 0);
        
	//发牌边框
	if ( m_bDispatchCard )
	{
		CImageHandle ImageHandleGameEndFrame(&m_ImageGameEndFrame);

		int	nXPos = nWidth / 2 - 129;
		int	nYPos = nHeight / 2 - 208;

		CRect rcAlpha(nXPos, nYPos, nXPos+ m_ImageGameEndFrame.GetWidth(), nYPos+m_ImageGameEndFrame.GetHeight());
		DrawAlphaRect(pDC, &rcAlpha, RGB(74,70,73), 0.8f);

		//绘画边框
		m_ImageGameEndFrame.AlphaDrawImage(pDC, nXPos, nYPos, RGB(255,0,255));
	}

	//最大下注
	pDC->SetTextColor(RGB(255,234,0));
	CString strLimitScore;
	strLimitScore.Format(TEXT("%ld"), GetMaxBankerScore());
	pDC->TextOut(nWidth/2-300, nHeight/2 - 347,strLimitScore);
	strLimitScore.Format(TEXT("%ld"), GetMaxPlayerScore());
	pDC->TextOut(nWidth/2-300, nHeight/2 - 322,strLimitScore);
	strLimitScore.Format(TEXT("%ld"), GetMaxTieScore());
	pDC->TextOut(nWidth/2-300, nHeight/2 - 299,strLimitScore);

	//绘画边框
	int nXPos=0, nYPos=0;
	if ( m_lCurrentJetton == 0 ) m_cbPreJettonArea = 255;
	switch ( m_cbPreJettonArea )
	{
	case ID_XIAN_JIA:
		{
			CImageHandle ImageHandleXianJia(&m_ImageFrameXianJia);
			nXPos = nWidth / 2 - 314;
			nYPos = nHeight / 2 - 159;

			m_ImageFrameXianJia.AlphaDrawImage(pDC,nXPos, nYPos , RGB(255,0,255));

			break;
		}
	case ID_PING_JIA:
		{
			CImageHandle ImageHandlePingJia(&m_ImageFramePingJia);

			nXPos = nWidth / 2 - 130;
			nYPos = nHeight / 2 - 60;
			m_ImageFramePingJia.AlphaDrawImage(pDC,nXPos, nYPos , RGB(255,0,255));

			break;
		}
	case ID_ZHUANG_JIA:
		{
			CImageHandle ImageHandleZhuangJia(&m_ImageFrameZhuangJia);
			nXPos = nWidth / 2 + 132;
			nYPos = nHeight / 2 - 159;

			m_ImageFrameZhuangJia.AlphaDrawImage(pDC,nXPos, nYPos , RGB(255,0,255));

			break;
		}
	case ID_XIAN_TIAN_WANG:
		{
			CImageHandle ImageHandleXianTianWang(&m_ImageFrameXianTianWang);
			nXPos = nWidth / 2 - 314;
			nYPos = nHeight / 2 - 158 + 204;

			m_ImageFrameXianTianWang.AlphaDrawImage(pDC,nXPos, nYPos , RGB(255,0,255));
			

			break;
		}
	case ID_ZHUANG_TIAN_WANG:
		{
			CImageHandle ImageHandleZhuangTianWang(&m_ImageFrameZhuangTianWang);
			nXPos = nWidth / 2 - 312 + 184 + 260;
			nYPos = nHeight / 2 - 158 + 203 ;

			m_ImageFrameZhuangTianWang.AlphaDrawImage(pDC,nXPos, nYPos , RGB(255,0,255));
			

			break;
		}
	case ID_TONG_DIAN_PING:
		{
			CImageHandle ImageHandleTongDianPing(&m_ImageFrameTongDianPing);
			nXPos = nWidth / 2 - 312 + 182;
			nYPos = nHeight / 2 - 158 + 203;
			
			m_ImageFrameTongDianPing.AlphaDrawImage(pDC,nXPos, nYPos , RGB(255,0,255));

			break;
		}
	}

	//胜利标志
	if (m_cbWinnerFlash!=0xFF)
	{
		//加载资源
		CImageHandle HandleWinFlags(&m_ImageWinFlags);
		CSize SizeWinFlags(m_ImageWinFlags.GetWidth()/3,m_ImageWinFlags.GetHeight());

		//闲家标志
		switch (m_cbWinnerFlash)
		{
		case ID_XIAN_JIA:
			{
				CImageHandle ImageHandleXianJia(&m_ImageFrameXianJia);
				CImageHandle ImageHandleXianTianWang(&m_ImageFrameXianTianWang);

				nXPos = nWidth / 2 - 314;
				nYPos = nHeight / 2 - 159;

				m_ImageFrameXianJia.AlphaDrawImage(pDC,nXPos, nYPos , RGB(255,0,255));
				
				//闲天王
				if ( m_cbKingWinner == ID_XIAN_TIAN_WANG )
				{
					nYPos = nHeight / 2 - 158 + 204;
					m_ImageFrameXianTianWang.AlphaDrawImage(pDC,nXPos, nYPos , RGB(255,0,255));
				}

				break;
			}
		case ID_PING_JIA:
			{
				CImageHandle ImageHandlePingJia(&m_ImageFramePingJia);
				CImageHandle ImageHandleTongDianPing(&m_ImageFrameTongDianPing);

				nXPos = nWidth / 2 - 130;
				nYPos = nHeight / 2 - 60;
				m_ImageFramePingJia.AlphaDrawImage(pDC,nXPos, nYPos , RGB(255,0,255));

				if ( m_cbKingWinner == ID_TONG_DIAN_PING )
				{
					nXPos = nWidth / 2 - 312 + 182;
					nYPos = nHeight / 2 - 158 + 203;
					m_ImageFrameTongDianPing.AlphaDrawImage(pDC,nXPos, nYPos , RGB(255,0,255));
				}

				break;
			}
		case ID_ZHUANG_JIA:
			{
				CImageHandle ImageHandleZhuangJia(&m_ImageFrameZhuangJia);
				CImageHandle ImageHandleZhuangTianWang(&m_ImageFrameZhuangTianWang);
				
				nXPos = nWidth / 2 + 132;
				nYPos = nHeight / 2 - 159;

				m_ImageFrameZhuangJia.AlphaDrawImage(pDC,nXPos, nYPos , RGB(255,0,255));
								
				if ( m_cbKingWinner == ID_ZHUANG_TIAN_WANG )
				{
					nXPos = nWidth / 2 - 312 + 184 + 260;
					nYPos = nHeight / 2 - 158 + 203 ;

					m_ImageFrameZhuangTianWang.AlphaDrawImage(pDC,nXPos, nYPos , RGB(255,0,255));
				}

				break;
			}
		}
	}

	//切换庄家
	if ( m_bShowChangeBanker )
	{
		int	nXPos = nWidth / 2 - 130;
		int	nYPos = nHeight / 2 - 160;

		//由我做庄
		if ( m_wMeChairID == m_wCurrentBankerChairID )
		{
			CImageHandle ImageHandleBanker(&m_ImageMeBanker);
			m_ImageMeBanker.BitBlt(pDC->GetSafeHdc(), nXPos, nYPos);
		}
		else if ( m_wCurrentBankerChairID != INVALID_CHAIR )
		{
			CImageHandle ImageHandleBanker(&m_ImageChangeBanker);
			m_ImageChangeBanker.BitBlt(pDC->GetSafeHdc(), nXPos, nYPos);
		}
		else
		{
			CImageHandle ImageHandleBanker(&m_ImageNoBanker);
			m_ImageNoBanker.BitBlt(pDC->GetSafeHdc(), nXPos, nYPos);
		}
	}
	//庄家信息																											
	pDC->SetTextColor(RGB(255,234,0));
	if ( m_wCurrentBankerChairID != INVALID_CHAIR )
	{
		//获取玩家
		tagUserData const *pUserData = GetUserInfo( m_wCurrentBankerChairID );
		if ( pUserData )
		{
			//位置信息
			CRect StrRect;
			StrRect.left = nWidth/2-85;
			StrRect.top = nHeight/2 - 340;
			StrRect.right = StrRect.left + 190;
			StrRect.bottom = StrRect.top + 15;

			//庄家名字
			pDC->DrawText( pUserData->szName, StrRect, DT_END_ELLIPSIS | DT_LEFT | DT_TOP| DT_SINGLELINE );

			//庄家总分
			CString strBankerTotalScore;
			strBankerTotalScore.Format( "%d", pUserData->lScore );
			StrRect.left = nWidth/2-85;
			StrRect.top = nHeight/2 - 316;
			StrRect.right = StrRect.left + 190;
			StrRect.bottom = StrRect.top + 15;
			pDC->DrawText( strBankerTotalScore, StrRect, DT_END_ELLIPSIS | DT_LEFT | DT_TOP| DT_SINGLELINE );

			//庄家局数
			CString strBankerTime;
			strBankerTime.Format( "%d", m_cbBankerTime );
			StrRect.left = nWidth/2-85;
			StrRect.top = nHeight/2 - 266;
			StrRect.right = StrRect.left + 190;
			StrRect.bottom = StrRect.top + 15;
			pDC->DrawText( strBankerTime, StrRect, DT_END_ELLIPSIS | DT_LEFT | DT_TOP| DT_SINGLELINE );

			//庄家成绩
			CString strBankerScore;
			strBankerScore.Format( "%ld", m_lBankerScore );
			StrRect.left = nWidth/2-85;
			StrRect.top = nHeight/2 - 291;
			StrRect.right = StrRect.left + 190;
			StrRect.bottom = StrRect.top + 15;
			pDC->DrawText( strBankerScore, StrRect, DT_END_ELLIPSIS | DT_LEFT | DT_TOP| DT_SINGLELINE );
		}
	}
	else
	{
		//位置信息
		CRect StrRect;
		StrRect.left = nWidth/2-85;
		StrRect.top = nHeight/2 - 337;
		StrRect.right = StrRect.left + 190;
		StrRect.bottom = StrRect.top + 15;

		//庄家名字
		pDC->DrawText( TEXT("无人做庄"), StrRect, DT_END_ELLIPSIS | DT_LEFT | DT_TOP| DT_SINGLELINE );

		//庄家局数
		StrRect.left = nWidth/2-85;
		StrRect.top = nHeight/2 - 316;
		StrRect.right = StrRect.left + 190;
		StrRect.bottom = StrRect.top + 15;
		pDC->DrawText( "......", StrRect, DT_END_ELLIPSIS | DT_LEFT | DT_TOP| DT_SINGLELINE );

		//庄家成绩
		StrRect.left = nWidth/2-85;
		StrRect.top = nHeight/2 - 291;
		StrRect.right = StrRect.left + 190;
		StrRect.bottom = StrRect.top + 15;
		pDC->DrawText( "......", StrRect, DT_END_ELLIPSIS | DT_LEFT | DT_TOP| DT_SINGLELINE );

		//庄家成绩
		StrRect.left = nWidth/2-85;
		StrRect.top = nHeight/2 - 266;
		StrRect.right = StrRect.left + 190;
		StrRect.bottom = StrRect.top + 15;
		pDC->DrawText( "......", StrRect, DT_END_ELLIPSIS | DT_LEFT | DT_TOP| DT_SINGLELINE );
	}

	//筹码资源
	CImageHandle HandleJettonView(&m_ImageJettonView);
	CSize SizeJettonItem(m_ImageJettonView.GetWidth()/JETTON_COUNT,m_ImageJettonView.GetHeight());

	//绘画筹码
	for (INT i=0;i<6;i++)
	{
		//变量定义
		LONG lScoreCount=0L;
		LONG lScoreJetton[JETTON_COUNT]={100L,500L,1000L,10000L,50000L,100000L,500000L};

		//绘画筹码
		for (INT_PTR j=0;j<m_JettonInfoArray[i].GetCount();j++)
		{
			//获取信息
			tagJettonInfo * pJettonInfo=&m_JettonInfoArray[i][j];

			//累计数字
			ASSERT(pJettonInfo->cbJettonIndex<JETTON_COUNT);
			lScoreCount+=lScoreJetton[pJettonInfo->cbJettonIndex];

			//绘画界面
			m_ImageJettonView.AlphaDrawImage(pDC,pJettonInfo->nXPos+m_PointJetton[i].x-SizeJettonItem.cx/2,
				pJettonInfo->nYPos+m_PointJetton[i].y-SizeJettonItem.cy/2,SizeJettonItem.cx,SizeJettonItem.cy,
				pJettonInfo->cbJettonIndex*SizeJettonItem.cx,0,RGB(255,0,255));
		}

		//绘画数字
		if (lScoreCount>0L)	DrawNumberString(pDC,lScoreCount,m_PointJetton[i].x,m_PointJetton[i].y);
	}

	//我的下注
	DrawMeJettonNumber(pDC);

	//绘画时间
	if (m_wMeChairID!=INVALID_CHAIR)
	{
		WORD wUserTimer=GetUserTimer(m_wMeChairID);
		if (wUserTimer!=0) DrawUserTimer(pDC,nWidth/2-260,nHeight/2-260+435,wUserTimer);
	}

	//绘画用户
	if (m_wMeChairID!=INVALID_CHAIR)
	{
		//变量定义
		TCHAR *pBuffer[6]={TEXT("和：%ld"), TEXT("庄：%ld"), TEXT("闲：%ld"), TEXT("同点平：%ld"), TEXT("庄天王：%ld"), 
			TEXT("闲天王：%ld")};

		LONG lJettonScore[6] = {m_lMeTieScore, m_lMeBankerScore, m_lMePlayerScore, m_lMeTieSamePointScore, m_lMeBankerKingScore, 
			m_lMePlayerKingScore};

		int nSpace = 50;

		//绘画信息
		pDC->SetTextColor(RGB(255,0,0));
		TCHAR szBuffer[256];
		for ( WORD wInfoIndex = 0; wInfoIndex < 6; wInfoIndex++ )
		{
			_sntprintf(szBuffer,CountArray(szBuffer),pBuffer[wInfoIndex], lJettonScore[wInfoIndex]);
			pDC->TextOut( nWidth/2-350+wInfoIndex*120, nHeight/2+222, szBuffer);
		}

		const tagUserData *pMeUserData = GetUserInfo(m_wMeChairID);
		if ( pMeUserData != NULL )
		{
			//游戏信息
			TCHAR szResultScore[16]=TEXT("");
			TCHAR szGameScore[16]=TEXT("");
			pDC->SetTextColor(RGB(0,0,0));
			LONG lMeJetton = m_lMeTieScore+m_lMeBankerScore+m_lMePlayerScore+m_lMeTieSamePointScore+m_lMePlayerKingScore+m_lMeBankerKingScore;
			_sntprintf(szGameScore,CountArray(szGameScore),TEXT("%ld"),pMeUserData->lScore-lMeJetton);
			_sntprintf(szResultScore,CountArray(szResultScore),TEXT("%ld"),m_lMeResultCount);
			CRect rcAccount(CPoint(nWidth/2-297,nHeight/2+272),CPoint(nWidth/2-297+73,nHeight/2+272+10));
			CRect rcGameScore(CPoint(nWidth/2-297,nHeight/2+294),CPoint(nWidth/2-297+73,nHeight/2+294+10));
			CRect rcResultScore(CPoint(nWidth/2-297,nHeight/2+294+23),CPoint(nWidth/2-297+73,nHeight/2+294+10+23));
			pDC->DrawText(pMeUserData->szName,lstrlen(pMeUserData->szName),rcAccount,DT_VCENTER|DT_SINGLELINE|DT_CENTER|DT_END_ELLIPSIS);
			pDC->DrawText(szGameScore,lstrlen(szGameScore),rcGameScore,DT_VCENTER|DT_SINGLELINE|DT_CENTER|DT_END_ELLIPSIS);
			pDC->DrawText(szResultScore,lstrlen(szResultScore),rcResultScore,DT_VCENTER|DT_SINGLELINE|DT_CENTER|DT_END_ELLIPSIS);
		}
	}

	return;
}

//设置信息
void CGameClientView::SetMeMaxScore(LONG lMeMaxScore)
{
	if (m_lMeMaxScore!=lMeMaxScore)
	{
		//设置变量
		m_lMeMaxScore=lMeMaxScore;

		//更新界面
		UpdateGameView(NULL);
	}

	return;
}


//设置信息
void CGameClientView::SetMeTieScore(LONG lMeTieScore)
{
	if (m_lMeTieScore!=lMeTieScore)
	{
		//设置变量
		m_lMeTieScore=lMeTieScore;

		//更新界面
		UpdateGameView(NULL);
	}

	return;
}

//最大下注
void CGameClientView::SetAreaLimitScore(LONG lAreaLimitScore)
{
	if ( m_lAreaLimitScore!= lAreaLimitScore )
	{
		//设置变量
		m_lAreaLimitScore=lAreaLimitScore;
	}
}

//设置信息
void CGameClientView::SetMeBankerScore(LONG lMeBankerScore)
{
	if (m_lMeBankerScore!=lMeBankerScore)
	{
		//设置变量
		m_lMeBankerScore=lMeBankerScore;

		//更新界面
		UpdateGameView(NULL);
	}

	return;
}

//设置信息
void CGameClientView::SetMePlayerScore(LONG lMePlayerScore)
{
	if (m_lMePlayerScore!=lMePlayerScore)
	{
		//设置变量
		m_lMePlayerScore=lMePlayerScore;

		//更新界面
		UpdateGameView(NULL);
	}

	return;
}

//设置信息
void CGameClientView::SetMePlayerKingScore(LONG lMePlayerKingScore)
{
	if (m_lMePlayerKingScore!=lMePlayerKingScore)
	{
		//设置变量
		m_lMePlayerKingScore=lMePlayerKingScore;

		//更新界面
		UpdateGameView(NULL);
	}
}
//设置信息
void CGameClientView::SetMeBankerKingScore(LONG lMeBankerKingScore)
{
	if (m_lMeBankerKingScore!=lMeBankerKingScore)
	{
		//设置变量
		m_lMeBankerKingScore=lMeBankerKingScore;

		//更新界面
		UpdateGameView(NULL);
	}
}
//设置信息
void CGameClientView::SetMeTieSamePointScore(LONG lMeTieSamePointScore)
{
	if (m_lMeTieSamePointScore!=lMeTieSamePointScore)
	{
		//设置变量
		m_lMeTieSamePointScore=lMeTieSamePointScore;

		//更新界面
		UpdateGameView(NULL);
	}
}

//设置筹码
void CGameClientView::SetCurrentJetton(LONG lCurrentJetton)
{
	//设置变量
	ASSERT(lCurrentJetton>=0L);
	m_lCurrentJetton=lCurrentJetton;

	return;
}


//历史记录
void CGameClientView::SetGameHistory(enOperateResult OperateResult, BYTE cbPlayerCount, BYTE cbBankerCount)
{
	//设置数据
	tagClientGameRecord &GameRecord = m_GameRecordArrary[m_nRecordLast];
	GameRecord.enOperateFlags = OperateResult;
	GameRecord.cbPlayerCount = cbPlayerCount;
	GameRecord.cbBankerCount = cbBankerCount;

	//移动下标
	m_nRecordLast = (m_nRecordLast+1) % MAX_SCORE_HISTORY;
	if ( m_nRecordLast == m_nRecordFirst )
	{
		m_nRecordFirst = (m_nRecordFirst+1) % MAX_SCORE_HISTORY;
		if ( m_nScoreHead < m_nRecordFirst ) m_nScoreHead = m_nRecordFirst;
	}

	int nHistoryCount = (m_nRecordLast - m_nRecordFirst + MAX_SCORE_HISTORY) % MAX_SCORE_HISTORY;
	if ( 12 < nHistoryCount ) m_btScoreMoveR.EnableWindow(TRUE);

	//移到最新记录
	if ( 12 < nHistoryCount )
	{
		m_nScoreHead = (m_nRecordLast - 12 + MAX_SCORE_HISTORY) % MAX_SCORE_HISTORY;
		m_btScoreMoveL.EnableWindow(TRUE);
		m_btScoreMoveR.EnableWindow(FALSE);
	}

	//更新界面
	UpdateGameView(NULL);

	return;
}

//历史成绩
void CGameClientView::SetHistoryScore(WORD wDrawCount,LONG lMeResultCount)
{
	//设置变量
	m_wDrawCount=wDrawCount;
	m_lMeResultCount=lMeResultCount;

	//更新界面
	UpdateGameView(NULL);

	return;
}

//清理筹码
void CGameClientView::CleanUserJetton()
{
	//清理数组
	for (BYTE i=0;i<CountArray(m_JettonInfoArray);i++)
	{
		m_JettonInfoArray[i].RemoveAll();
	}

	//下注信息
	m_lAllTieScore=0L;			
	m_lAllBankerScore=0L;		
	m_lAllPlayerScore=0L;		
	m_lAllTieSamePointScore=0L;
	m_lAllBankerKingScore=0L;	
	m_lAllPlayerKingScore=0L;

	//更新界面
	UpdateGameView(NULL);

	return;
}

//设置筹码
void CGameClientView::PlaceUserJetton(BYTE cbViewIndex, LONG lScoreCount)
{
	//效验参数
	ASSERT(cbViewIndex<=ID_TONG_DIAN_PING);
	if (cbViewIndex>ID_TONG_DIAN_PING) return;

	//变量定义
	bool bPlaceJetton=false;
	LONG lScoreIndex[JETTON_COUNT]={100L,500L,1000L,10000L,50000L,100000L,500000L};

	//边框宽度
	int nFrameWidth = 0, nFrameHeight = 0;

	switch ( cbViewIndex )
	{
	case ID_XIAN_JIA:
		{ 
			m_lAllPlayerScore += lScoreCount;

			nFrameWidth = 94;
			nFrameHeight = 104;
			break;
		}
	case ID_PING_JIA:
		{
			m_lAllTieScore += lScoreCount;
			nFrameWidth = 133;
			nFrameHeight = 54;
			break;
		}
	case ID_ZHUANG_JIA:
		{ 
			m_lAllBankerScore += lScoreCount;

			nFrameWidth = 93;
			nFrameHeight = 104;
			break;
		}
	case ID_XIAN_TIAN_WANG:
		{ 
			m_lAllPlayerKingScore += lScoreCount;

			nFrameWidth = 94;
			nFrameHeight = 43;
			break;
		}
	case ID_ZHUANG_TIAN_WANG:
		{ 
			m_lAllBankerKingScore += lScoreCount;

			nFrameWidth = 93;
			nFrameHeight = 43;
			break;
		}
	case ID_TONG_DIAN_PING:
		{ 
			m_lAllTieSamePointScore += lScoreCount;

			nFrameWidth = 133;
			nFrameHeight = 43;
			break;
		}
	}

	//增加筹码
	for (BYTE i=0;i<CountArray(lScoreIndex);i++)
	{
		//计算数目
		BYTE cbScoreIndex=JETTON_COUNT-i-1;
		LONG lCellCount=lScoreCount/lScoreIndex[cbScoreIndex];

		//插入过虑
		if (lCellCount==0L) continue;

		//加入筹码
		for (LONG j=0;j<lCellCount;j++)
		{
			//计算位置
			INT nXPos=rand()%JETTON_RADII;
			INT nYPos=(INT)sqrt((FLOAT)(rand()%(JETTON_RADII*JETTON_RADII-nXPos*nXPos)));

			//构造变量
			tagJettonInfo JettonInfo;
			JettonInfo.cbJettonIndex=cbScoreIndex;
			JettonInfo.nXPos=rand()%(nFrameWidth-30);
			JettonInfo.nYPos=rand()%(nFrameHeight-30);

			//插入数组
			bPlaceJetton=true;
			m_JettonInfoArray[cbViewIndex-1].Add(JettonInfo);
		}

		//减少数目
		lScoreCount-=lCellCount*lScoreIndex[cbScoreIndex];
	}

	//更新界面
	if (bPlaceJetton==true) UpdateGameView(NULL);

	return;
}

//设置胜方
void CGameClientView::SetWinnerSide(LPCTSTR pszGameResult, BYTE cbWinnerSide, BYTE cbKingWinner)
{
	//设置变量
	m_cbWinnerSide=cbWinnerSide;
	m_cbKingWinner=cbKingWinner;
	m_cbWinnerFlash=cbWinnerSide;
	m_strGameCardResult=pszGameResult;

	//设置时间
	if (cbWinnerSide!=0xFF)
	{
		SetTimer(IDI_FLASH_WINNER,500,NULL);
	}
	else KillTimer(IDI_FLASH_WINNER);

	//更新界面
	UpdateGameView(NULL);

	return;
}

//获取区域
BYTE CGameClientView::GetJettonArea(CPoint MousePoint)
{
	//获取大小
	CRect rcClient ;
	GetClientRect(rcClient);

	int nWidth = rcClient.Width();
	int nHeight = rcClient.Height();

	CRect rcXianJia;
	rcXianJia.left = nWidth / 2 - 312;
	rcXianJia.top = nHeight / 2 - 158;
	rcXianJia.right = rcXianJia.left + 182;
	rcXianJia.bottom = rcXianJia.top + 205;
	if ( rcXianJia.PtInRect(MousePoint) ) return ID_XIAN_JIA;

	CRect rcPingJia;
	rcPingJia.left = nWidth / 2 - 130;
	rcPingJia.top = nHeight / 2 - 58;
	rcPingJia.right = rcPingJia.left + 263;
	rcPingJia.bottom = rcPingJia.top + 105;
	if ( rcPingJia.PtInRect(MousePoint) ) return ID_PING_JIA;

	CRect rcZhuangJia;
	rcZhuangJia.left = nWidth / 2 + 135;
	rcZhuangJia.top = nHeight / 2 - 158;
	rcZhuangJia.right = rcZhuangJia.left + 182;
	rcZhuangJia.bottom = rcZhuangJia.top + 205;
	if ( rcZhuangJia.PtInRect(MousePoint) ) return ID_ZHUANG_JIA;

	CRect rcXianTianWang;
	rcXianTianWang.left = nWidth / 2 - 312;
	rcXianTianWang.top = nHeight / 2 - 158 + 205 + 2;
	rcXianTianWang.right = rcXianTianWang.left + 182;
	rcXianTianWang.bottom = rcXianTianWang.top + 81;
	if ( rcXianTianWang.PtInRect(MousePoint) ) return ID_XIAN_TIAN_WANG;	

	CRect rcTongDianPing;
	rcTongDianPing.left = nWidth / 2 - 312 + 184;
	rcTongDianPing.top = nHeight / 2 - 158 + 205 + 2;
	rcTongDianPing.right = rcTongDianPing.left + 263;
	rcTongDianPing.bottom = rcTongDianPing.top + 81;
	if ( rcTongDianPing.PtInRect(MousePoint) ) return ID_TONG_DIAN_PING;	

	CRect rcZhuangTianWang;
	rcZhuangTianWang.left = nWidth / 2 - 312 + 184 + 265;
	rcZhuangTianWang.top = nHeight / 2 - 158 + 205 + 2;
	rcZhuangTianWang.right = rcZhuangTianWang.left + 180;
	rcZhuangTianWang.bottom = rcZhuangTianWang.top + 81;
	if ( rcZhuangTianWang.PtInRect(MousePoint) ) return ID_ZHUANG_TIAN_WANG;

	return 0xFF;
}

//绘画数字
void CGameClientView::DrawNumberString(CDC * pDC, LONG lNumber, INT nXPos, INT nYPos, bool bMeScore)
{
	//加载资源
	CImageHandle HandleScoreNumber(&m_ImageScoreNumber);
	CImageHandle HandleMeScoreNumber(&m_ImageMeScoreNumber);
	CSize SizeScoreNumber(m_ImageScoreNumber.GetWidth()/10,m_ImageScoreNumber.GetHeight());

	if ( bMeScore ) SizeScoreNumber.SetSize(m_ImageMeScoreNumber.GetWidth()/10, m_ImageMeScoreNumber.GetHeight());

	//计算数目
	LONG lNumberCount=0;
	LONGLONG lNumberTemp=lNumber;
	do
	{
		lNumberCount++;
		lNumberTemp/=10;
	} while (lNumberTemp>0);

	//位置定义
	INT nYDrawPos=nYPos-SizeScoreNumber.cy/2;
	INT nXDrawPos=nXPos+lNumberCount*SizeScoreNumber.cx/2-SizeScoreNumber.cx;

	//绘画桌号
	for (LONG i=0;i<lNumberCount;i++)
	{
		//绘画号码
		LONG lCellNumber=(LONG)(lNumber%10);
		if ( bMeScore )
		{
			m_ImageMeScoreNumber.AlphaDrawImage(pDC,nXDrawPos,nYDrawPos,SizeScoreNumber.cx,SizeScoreNumber.cy,
				lCellNumber*SizeScoreNumber.cx,0,RGB(255,0,255));
		}
		else
		{
			m_ImageScoreNumber.AlphaDrawImage(pDC,nXDrawPos,nYDrawPos,SizeScoreNumber.cx,SizeScoreNumber.cy,
				lCellNumber*SizeScoreNumber.cx,0,RGB(255,0,255));
		}

		//设置变量
		lNumber/=10;
		nXDrawPos-=SizeScoreNumber.cx;
	};

	return;
}

//筹码按钮
void CGameClientView::OnJettonButton500()
{
	//设置变量
	m_lCurrentJetton=500L;

	return;
}

//筹码按钮
void CGameClientView::OnJettonButton50000()
{
	//设置变量
	m_lCurrentJetton=50000L;

	return;
}

//筹码按钮
void CGameClientView::OnJettonButton500000()
{
	//设置变量
	m_lCurrentJetton=500000L;

	return;
}

//筹码按钮
void CGameClientView::OnJettonButton100()
{
	//设置变量
	m_lCurrentJetton=100L;

	return;
}

//筹码按钮
void CGameClientView::OnJettonButton1000()
{
	//设置变量
	m_lCurrentJetton=1000L;

	return;
}

//筹码按钮
void CGameClientView::OnJettonButton10000()
{
	//设置变量
	m_lCurrentJetton=10000L;

	return;
}

//筹码按钮
void CGameClientView::OnJettonButton100000()
{
	//设置变量
	m_lCurrentJetton=100000L;

	return;
}

//定时器消息
void CGameClientView::OnTimer(UINT nIDEvent)
{
	//闪动胜方
	if (nIDEvent==IDI_FLASH_WINNER)
	{
		//设置变量
		if (m_cbWinnerFlash!=m_cbWinnerSide)
		{
			m_cbWinnerFlash=m_cbWinnerSide;
		}
		else 
		{
			m_cbWinnerFlash=0xFF;
		}

		//更新界面
		UpdateGameView(NULL);

		return;
	}

	//轮换庄家
	if ( nIDEvent == IDI_SHOW_CHANGE_BANKER )
	{
		ShowChangeBanker( false );

		return;
	}

	__super::OnTimer(nIDEvent);
}

//鼠标消息
void CGameClientView::OnLButtonDown(UINT nFlags, CPoint Point)
{
	if (m_lCurrentJetton!=0L)
	{
		BYTE cbJettonArea=GetJettonArea(Point);

		//大小判断
		switch( cbJettonArea )
		{
		case ID_XIAN_JIA:
			{
				LONG lMaxPlayerScore = GetMaxPlayerScore();
				if ( lMaxPlayerScore < m_lCurrentJetton )
					return ;
				break;
			}
		case ID_PING_JIA:
			{
				LONG lMaxTieScore  = GetMaxTieScore();
				if ( lMaxTieScore < m_lCurrentJetton )
					return ;
				break;
			}
		case ID_ZHUANG_JIA:
			{
				LONG lMaxBankerScore = GetMaxBankerScore();
				if ( lMaxBankerScore < m_lCurrentJetton )
					return ;
				break;
			}
		case ID_TONG_DIAN_PING:
			{
				LONG lMaxTieKingScore= GetMaxTieKingScore();
				if ( lMaxTieKingScore < m_lCurrentJetton )
					return ;
				break;
			}
		case ID_XIAN_TIAN_WANG:
			{
				LONG lMaxPlayerKingScore = GetMaxPlayerKingScore();
				if ( lMaxPlayerKingScore < m_lCurrentJetton )
					return ;
				break;
			}
		case ID_ZHUANG_TIAN_WANG:
			{
				LONG lMaxBankerKingScore = GetMaxBankerKingScore();
				if ( lMaxBankerKingScore < m_lCurrentJetton )
					return ;
				break;
			}
		}

		if (cbJettonArea!=0xFF) AfxGetMainWnd()->SendMessage(IDM_PLACE_JETTON,cbJettonArea,m_lCurrentJetton);
	}

	__super::OnLButtonDown(nFlags,Point);
}

//鼠标消息
void CGameClientView::OnRButtonDown(UINT nFlags, CPoint Point)
{
	//设置变量
	m_lCurrentJetton=0L;

	__super::OnLButtonDown(nFlags,Point);
}

//光标消息
BOOL CGameClientView::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
{
	if (m_lCurrentJetton!=0L)
	{
		//获取区域
		CPoint MousePoint;
		GetCursorPos(&MousePoint);
		ScreenToClient(&MousePoint);
		BYTE cbJettonArea=GetJettonArea(MousePoint);

		//设置变量
		if ( m_cbPreJettonArea != cbJettonArea )
		{
			m_cbPreJettonArea = cbJettonArea;
			UpdateGameView(NULL);
		}

		//区域判断
		if (cbJettonArea==0xFF)
		{
			SetCursor(LoadCursor(NULL,IDC_NO));
			return TRUE;
		}

		//大小判断
		switch( cbJettonArea )
		{
		case ID_XIAN_JIA:
			{
				LONG lMaxPlayerScore = GetMaxPlayerScore();
				if ( lMaxPlayerScore < m_lCurrentJetton )
				{
					SetCursor(LoadCursor(NULL,IDC_NO));
					return TRUE;
				}
				break;
			}
		case ID_PING_JIA:
			{
				LONG lMaxTieScore  = GetMaxTieScore();
				if ( lMaxTieScore < m_lCurrentJetton )
				{
					SetCursor(LoadCursor(NULL,IDC_NO));
					return TRUE;
				}
				break;
			}
		case ID_ZHUANG_JIA:
			{
				LONG lMaxBankerScore = GetMaxBankerScore();
				if ( lMaxBankerScore < m_lCurrentJetton  )
				{
					SetCursor(LoadCursor(NULL,IDC_NO));
					return TRUE;
				};
				break;
			}
		case ID_TONG_DIAN_PING:
			{
				LONG lMaxTieKingScore= GetMaxTieKingScore();
				if ( lMaxTieKingScore < m_lCurrentJetton )
				{
					SetCursor(LoadCursor(NULL,IDC_NO));
					return TRUE;
				}
				break;
			}
		case ID_XIAN_TIAN_WANG:
			{
				LONG lMaxPlayerKingScore = GetMaxPlayerKingScore();
				if ( lMaxPlayerKingScore < m_lCurrentJetton )
				{
					SetCursor(LoadCursor(NULL,IDC_NO));
					return TRUE;
				}
				break;
			}
		case ID_ZHUANG_TIAN_WANG:
			{
				LONG lMaxBankerKingScore = GetMaxBankerKingScore();
				if ( lMaxBankerKingScore < m_lCurrentJetton )
				{
					SetCursor(LoadCursor(NULL,IDC_NO));
					return TRUE;
				}
				break;
			}
		}


		//设置光标
		switch (m_lCurrentJetton)
		{
		case 500:
			{
				SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_500)));
				return TRUE;
			}
		case 50000:
			{
				SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_50000)));
				return TRUE;
			}
		case 500000:
			{
				SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_500000)));
				return TRUE;
			}
		case 100:
			{
				SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_100)));
				return TRUE;
			}
		case 1000:
			{
				SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_1000)));
				return TRUE;
			}
		case 10000:
			{
				SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_10000)));
				return TRUE;
			}
		case 100000:
			{
				SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_100000)));
				return TRUE;
			}
		}
	}

	return __super::OnSetCursor(pWnd, nHitTest, uMessage);
}

//轮换庄家
void CGameClientView::ShowChangeBanker( bool bChangeBanker )
{
	//轮换庄家
	if ( bChangeBanker )
	{
		SetTimer( IDI_SHOW_CHANGE_BANKER, 3000, NULL );
		m_bShowChangeBanker = true;
	}
	else
	{
		KillTimer( IDI_SHOW_CHANGE_BANKER );
		m_bShowChangeBanker = false ;
	}

	//更新界面
	UpdateGameView(NULL);
}

//上庄按钮
void CGameClientView::OnApplyBanker()
{
	AfxGetMainWnd()->SendMessage(IDM_APPLY_BANKER,1,0);

}

//下庄按钮
void CGameClientView::OnCancelBanker()
{
	AfxGetMainWnd()->SendMessage(IDM_APPLY_BANKER,0,0);
}

//艺术字体
void CGameClientView::DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos)
{
	//变量定义
	int nStringLength=lstrlen(pszString);
	int nXExcursion[8]={1,1,1,0,-1,-1,-1,0};
	int nYExcursion[8]={-1,0,1,1,1,0,-1,-1};

	//绘画边框
	pDC->SetTextColor(crFrame);
	for (int i=0;i<CountArray(nXExcursion);i++)
	{
		pDC->TextOut(nXPos+nXExcursion[i],nYPos+nYExcursion[i],pszString,nStringLength);
	}

	//绘画字体
	pDC->SetTextColor(crText);
	pDC->TextOut(nXPos,nYPos,pszString,nStringLength);

	return;
}

//庄家信息
void CGameClientView::SetBankerInfo( WORD wChairID, BYTE cbBankerTime, LONG lScore )
{
	//设置变量
	m_wCurrentBankerChairID = wChairID;
	m_cbBankerTime = cbBankerTime;
	m_lBankerScore = lScore;

	//更新界面
	UpdateGameView( NULL );
}

//庄家金币
void CGameClientView::SetBankerTreasure(LONG lBankerTreasure)
{
	m_lBankerTreasure = lBankerTreasure;
	UpdateGameView(NULL);
}

//绘画标识
void CGameClientView::DrawWinFlags(CDC * pDC)
{
	CImageHandle ImageHandleWinFlags(&m_ImageWinFlags);
	int nIndex = m_nScoreHead;
	COLORREF clrOld ;
	clrOld = pDC->SetTextColor(RGB(255,234,0));
	CString strPoint;
	int nDrawCount = 0;
	while ( nIndex != m_nRecordLast && ( m_nRecordLast!=m_nRecordFirst ) && nDrawCount < 12 )
	{
		tagClientGameRecord &ClientGameRecord = m_GameRecordArrary[nIndex];
		int nYPos = 0;
		if ( ClientGameRecord.cbBankerCount > ClientGameRecord.cbPlayerCount ) nYPos = m_nWinFlagsExcursionY ;
		else if (ClientGameRecord.cbBankerCount < ClientGameRecord.cbPlayerCount ) nYPos = m_nWinFlagsExcursionY + 31;
		else nYPos = m_nWinFlagsExcursionY + 2 * 31;

		int nXPos = m_nWinFlagsExcursionX + ((nIndex - m_nScoreHead + MAX_SCORE_HISTORY) % MAX_SCORE_HISTORY) * 37;

		int nFlagsIndex = 0;
		if ( ClientGameRecord.enOperateFlags == enOperateResult_NULL ) nFlagsIndex = 0;
		else if ( ClientGameRecord.enOperateFlags == enOperateResult_Win) nFlagsIndex = 1;
		else if ( ClientGameRecord.enOperateFlags == enOperateResult_Lost) nFlagsIndex = 2;

		m_ImageWinFlags.AlphaDrawImage( pDC, nXPos, nYPos, m_ImageWinFlags.GetWidth()/3 , 
			m_ImageWinFlags.GetHeight(),m_ImageWinFlags.GetWidth()/3 * nFlagsIndex, 0, RGB(255, 0, 255) );

		strPoint.Format(TEXT("%d"), ClientGameRecord.cbBankerCount);
		pDC->TextOut(m_nWinPointsExcursionX + nDrawCount * 17, m_nWinPointsExcursionY, strPoint);

		strPoint.Format(TEXT("%d"), ClientGameRecord.cbPlayerCount);
		pDC->TextOut(m_nWinPointsExcursionX + nDrawCount * 17, m_nWinPointsExcursionY + 21, strPoint);

		nIndex = (nIndex+1) % MAX_SCORE_HISTORY;
		nDrawCount++;
	}
	pDC->SetTextColor(clrOld);

	//统计个数
	nIndex = m_nRecordFirst;
	int nPlayerCount = 0, nBankerCount = 0, nTieCount = 0;
	while ( nIndex != m_nRecordLast && ( m_nRecordLast!=m_nRecordFirst ))
	{
		tagClientGameRecord &ClientGameRecord = m_GameRecordArrary[nIndex];
		if ( ClientGameRecord.cbBankerCount < ClientGameRecord.cbPlayerCount ) nPlayerCount++;
		else if ( ClientGameRecord.cbBankerCount == ClientGameRecord.cbPlayerCount ) nTieCount++;
		else nBankerCount++;

		nIndex = (nIndex+1) % MAX_SCORE_HISTORY;
	}

	int nWidth, nHeight;
	CRect rcClient;
	GetClientRect(&rcClient);
	nWidth = rcClient.Width();
	nHeight = rcClient.Height();
	CFont font;
	VERIFY(font.CreateFont(
		25,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		"Arial"));                 // lpszFacename

	pDC->SetTextColor(RGB(255,255,255));
	CFont* def_font = pDC->SelectObject(&font);
	strPoint.Format(TEXT("%d"), nBankerCount);
	pDC->TextOut(nWidth/2-350+5*120+78, nHeight/2+257, strPoint);
	strPoint.Format(TEXT("%d"), nPlayerCount);
	pDC->TextOut(nWidth/2-350+5*120+78, nHeight/2+288, strPoint);
	strPoint.Format(TEXT("%d"), nTieCount);
	pDC->TextOut(nWidth/2-350+5*120+78, nHeight/2+320, strPoint);
	pDC->SelectObject(def_font);
	font.DeleteObject();
}

//移动按钮
void CGameClientView::OnScoreMoveL()
{
	if ( m_nRecordFirst == m_nScoreHead ) return;

	m_nScoreHead = (m_nScoreHead - 1 + MAX_SCORE_HISTORY) % MAX_SCORE_HISTORY;
	if ( m_nScoreHead == m_nRecordFirst ) m_btScoreMoveL.EnableWindow(FALSE);

	m_btScoreMoveR.EnableWindow(TRUE);

	//更新界面
	UpdateGameView(NULL);
}

//移动按钮
void CGameClientView::OnScoreMoveR()
{
	int nHistoryCount = ( m_nRecordLast - m_nScoreHead + MAX_SCORE_HISTORY ) % MAX_SCORE_HISTORY;
	if ( nHistoryCount == 12 ) return;

	m_nScoreHead = ( m_nScoreHead + 1 ) % MAX_SCORE_HISTORY;
	if ( nHistoryCount-1 == 12 ) m_btScoreMoveR.EnableWindow(FALSE);

	m_btScoreMoveL.EnableWindow(TRUE);

	//更新界面
	UpdateGameView(NULL);
}

//显示结果
void CGameClientView::ShowGameResult(CDC *pDC, int nWidth, int nHeight)
{
	//显示判断
	if ( m_bShowGameResult == false ) return;

	int	nXPos = nWidth / 2 - 129;
	int	nYPos = nHeight / 2 - 208;

	CImageHandle ImageHandleGameEnd(&m_ImageGameEnd);
	CImageHandle ImageHandleGameEndFrame(&m_ImageGameEndFrame);
	CImageHandle ImageHandleGamePoint(&m_ImageGamePoint);

	//绘画边框
	m_ImageGameEndFrame.AlphaDrawImage(pDC, nXPos, nYPos, RGB(255,0,255));

	//加载位图
	CImageHandle HandleCard(&m_ImageCard);
	CSize CardSize;
	CardSize.cx=m_ImageCard.GetWidth()/13;
	CardSize.cy=m_ImageCard.GetHeight()/5;

	//扑克点数
	BYTE cbPlayerPoint, cbBankerPoint;
	CGameLogic GameLogic;

	//绘画扑克
	DWORD dwXImagePos,dwYImagePos;
	DWORD dwXScreenPos,dwYScreenPos;
	BYTE cbCardData[3];
	int nPlayerCardCount = m_CardControl[INDEX_PLAYER].GetCardCount();
	for (int i=nPlayerCardCount-1; 0 <= i; i--)
	{
		//获取扑克
		BYTE cbCardItem=0x00;

		m_CardControl[INDEX_PLAYER].GetCardData(i, cbCardItem);
		cbCardData[i] = cbCardItem;

		//图片位置
		if (cbCardItem!=0)
		{
			dwXImagePos=((cbCardItem&CARD_MASK_VALUE)-1)*CardSize.cx;
			dwYImagePos=((cbCardItem&CARD_MASK_COLOR)>>4)*CardSize.cy;
		}
		else
		{
			dwXImagePos=CardSize.cx*2;
			dwYImagePos=CardSize.cy*4;
		}

		//屏幕位置
		dwYScreenPos=0;
		dwXScreenPos=nXPos + 10 + 20*(nPlayerCardCount-i-1);

		//绘画扑克
		m_ImageCard.BitBlt(pDC->GetSafeHdc(),dwXScreenPos, nYPos+38,CardSize.cx,CardSize.cy,dwXImagePos,dwYImagePos);
	}

	//计算点数
	cbPlayerPoint = GameLogic.GetCardListPip(cbCardData, BYTE(m_CardControl[INDEX_PLAYER].GetCardCount()));
	
	for (DWORD i=0;i<m_CardControl[INDEX_BANKER].GetCardCount();i++)
	{
		//获取扑克
		BYTE cbCardItem=0x00;

		m_CardControl[INDEX_BANKER].GetCardData(i, cbCardItem);
		cbCardData[i] = cbCardItem;

		//图片位置
		if (cbCardItem!=0)
		{
			dwXImagePos=((cbCardItem&CARD_MASK_VALUE)-1)*CardSize.cx;
			dwYImagePos=((cbCardItem&CARD_MASK_COLOR)>>4)*CardSize.cy;
		}
		else
		{
			dwXImagePos=CardSize.cx*2;
			dwYImagePos=CardSize.cy*4;
		}

		//屏幕位置
		dwYScreenPos=0;
		dwXScreenPos=nXPos + 10 + 20*i;

		//绘画扑克
		m_ImageCard.BitBlt(pDC->GetSafeHdc(),nXPos + 10 + 20*i + 130, nYPos+38,CardSize.cx,CardSize.cy,dwXImagePos,dwYImagePos);
	}

	//计算点数
	cbBankerPoint = GameLogic.GetCardListPip(cbCardData, BYTE(m_CardControl[INDEX_BANKER].GetCardCount()));

	//绘画点数
	int nPointWidth = m_ImageGamePoint.GetWidth() / 10;
	m_ImageGamePoint.AlphaDrawImage(pDC, nXPos + 28, nYPos + 10, nPointWidth, m_ImageGamePoint.GetHeight(),
		cbPlayerPoint * nPointWidth, 0, RGB(255, 0, 255));
	m_ImageGamePoint.AlphaDrawImage(pDC, nXPos + 28 + 136, nYPos + 10, nPointWidth, m_ImageGamePoint.GetHeight(),
		cbBankerPoint * nPointWidth, 0, RGB(255, 0, 255));

	CRect rcAlpha(nXPos+2, nYPos+70, nXPos+2 + m_ImageGameEnd.GetWidth(), nYPos+70+m_ImageGameEnd.GetHeight());
	DrawAlphaRect(pDC, &rcAlpha, RGB(74,70,73), 0.8f);
	m_ImageGameEnd.AlphaDrawImage(pDC, nXPos+2, nYPos+70, RGB(255,0,255));

	pDC->SetTextColor(RGB(255,234,0));
	CRect rcMeWinScore, rcMeReturnScore;
	rcMeWinScore.left = nXPos+2 + 40;
	rcMeWinScore.top = nYPos+70 + 32;
	rcMeWinScore.right = rcMeWinScore.left + 111;
	rcMeWinScore.bottom = rcMeWinScore.top + 34;

	rcMeReturnScore.left = nXPos+2 + 150;
	rcMeReturnScore.top = nYPos+70 + 32;
	rcMeReturnScore.right = rcMeReturnScore.left + 111;
	rcMeReturnScore.bottom = rcMeReturnScore.top + 34;

	CString strMeGameScore, strMeReturnScore;
	strMeGameScore.Format(TEXT("%ld"), m_lMeCurGameScore);
	strMeReturnScore.Format(TEXT("%ld"), m_lMeCurGameReturnScore);
	pDC->DrawText(strMeGameScore, rcMeWinScore, DT_END_ELLIPSIS | DT_VCENTER | DT_CENTER | DT_SINGLELINE );
	pDC->DrawText(strMeReturnScore, rcMeReturnScore, DT_END_ELLIPSIS | DT_VCENTER | DT_CENTER | DT_SINGLELINE );

	CRect rcBankerWinScore;
	rcBankerWinScore.left = nXPos+2 + 40;
	rcBankerWinScore.top = nYPos+70 + 69;
	rcBankerWinScore.right = rcBankerWinScore.left + 111;
	rcBankerWinScore.bottom = rcBankerWinScore.top + 34;

	CString strBankerCurGameScore;
	strBankerCurGameScore.Format(TEXT("%ld"), m_lBankerCurGameScore);
	pDC->DrawText(strBankerCurGameScore, rcBankerWinScore, DT_END_ELLIPSIS | DT_VCENTER | DT_CENTER | DT_SINGLELINE );
}

//透明绘画
bool CGameClientView::DrawAlphaRect(CDC* pDC, LPRECT lpRect, COLORREF clr, FLOAT fAlpha)
{
	ASSERT(pDC != 0 && lpRect != 0 && clr != CLR_NONE);
	if(pDC == 0 || lpRect == 0 || clr == CLR_NONE)
	{
		return false;
	}
	//全透明
	if( abs(fAlpha) <= 0.000001 )
	{
		return true;
	}

	for(LONG l=lpRect->top; l<lpRect->bottom; l++)
	{
		for(LONG k=lpRect->left; k<lpRect->right; k++)
		{
			COLORREF clrBk = pDC->GetPixel(k, l);
			COLORREF clrBlend = RGB(GetRValue(clrBk)*(1-fAlpha)+GetRValue(clr)*fAlpha, 
								GetGValue(clrBk)*(1-fAlpha)+GetGValue(clr)*fAlpha,
								GetBValue(clrBk)*(1-fAlpha)+GetBValue(clr)*fAlpha);
			pDC->SetPixel(k, l, clrBlend);
		}
	}

	return true;
}

//最大下注
LONG CGameClientView::GetMaxPlayerScore()
{
	if ( m_wCurrentBankerChairID == INVALID_CHAIR ) return 0;

	//其他区域
	LONG lOtherAreaScore = m_lAllTieScore + m_lAllTieSamePointScore + m_lAllBankerScore + m_lAllBankerKingScore;

	//最大下注
	LONG lMaxPlayerScore = lOtherAreaScore + m_lBankerTreasure;
	lMaxPlayerScore -= (m_lAllPlayerScore + m_lAllPlayerKingScore*2);
	lMaxPlayerScore = min(lMaxPlayerScore, (m_lAreaLimitScore-m_lAllPlayerScore));

	return lMaxPlayerScore;
}

//最大下注
LONG CGameClientView::GetMaxPlayerKingScore()
{
	if ( m_wCurrentBankerChairID == INVALID_CHAIR ) return 0;

	//其他区域
	LONG lOtherAreaScore = m_lAllTieScore + m_lAllTieSamePointScore + m_lAllBankerScore + m_lAllBankerKingScore;

	//最大下注
	LONG lMaxPlayerScore = lOtherAreaScore + m_lBankerTreasure;
	lMaxPlayerScore -= (m_lAllPlayerScore + m_lAllPlayerKingScore*2);
	lMaxPlayerScore = min(lMaxPlayerScore/2, (m_lAreaLimitScore-m_lAllPlayerKingScore));

	return lMaxPlayerScore;
}

//最大下注
LONG CGameClientView::GetMaxBankerScore()
{
	if ( m_wCurrentBankerChairID == INVALID_CHAIR ) return 0;

	//其他区域
	LONG lOtherAreaScore = m_lAllTieScore + m_lAllPlayerScore + m_lAllTieSamePointScore + m_lAllPlayerKingScore;

	//最大下注
	LONG lMaxPlayerScore = lOtherAreaScore + m_lBankerTreasure;
	lMaxPlayerScore -= (m_lAllBankerScore + m_lAllBankerKingScore*2);
	lMaxPlayerScore = min(lMaxPlayerScore, (m_lAreaLimitScore-m_lAllBankerScore));

	return lMaxPlayerScore;
}

//最大下注
LONG CGameClientView::GetMaxBankerKingScore()
{
	if ( m_wCurrentBankerChairID == INVALID_CHAIR ) return 0;

	//其他区域
	LONG lOtherAreaScore = m_lAllTieScore + m_lAllPlayerScore + m_lAllTieSamePointScore + m_lAllPlayerKingScore;

	//最大下注
	LONG lMaxPlayerScore = lOtherAreaScore + m_lBankerTreasure;
	lMaxPlayerScore -= (m_lAllBankerScore + m_lAllBankerKingScore*2);
	lMaxPlayerScore = min(lMaxPlayerScore/2, (m_lAreaLimitScore-m_lAllBankerKingScore));

	return lMaxPlayerScore;
}

//最大下注
LONG CGameClientView::GetMaxTieScore()
{
	if ( m_wCurrentBankerChairID == INVALID_CHAIR ) return 0;

	//返回积分
	LONG lReturnScore = m_lAllTieSamePointScore * 33 + m_lAllTieScore * 8;

	//可下积分
	LONG lMaxTieScore = min((m_lBankerTreasure - lReturnScore)/8, (m_lAreaLimitScore-m_lAllTieScore));

	return lMaxTieScore;
}

//最大下注
LONG CGameClientView::GetMaxTieKingScore()
{
	if ( m_wCurrentBankerChairID == INVALID_CHAIR ) return 0;

	//返回积分
	LONG lReturnScore = m_lAllTieSamePointScore * 33 + m_lAllTieScore * 8;

	//可下积分
	LONG lMaxTieScore = min(( m_lBankerTreasure - lReturnScore )/33, (m_lAreaLimitScore-m_lAllTieSamePointScore)) ;

	return lMaxTieScore;
}

//成绩设置
void CGameClientView::SetGameScore(LONG lMeCurGameScore, LONG lMeCurGameReturnScore, LONG lBankerCurGameScore)
{
	m_lMeCurGameScore=lMeCurGameScore;
	m_lMeCurGameReturnScore=lMeCurGameReturnScore;
	m_lBankerCurGameScore=lBankerCurGameScore;
}

//设置标识
void CGameClientView::SetShowGameFlag(bool bShowGameResult)
{
	m_bShowGameResult = bShowGameResult;
	UpdateGameView(NULL);
}

//绘画数字
void CGameClientView::DrawMeJettonNumber(CDC *pDC)
{
	//绘画数字
	if ( 0 < m_lMePlayerScore ) DrawNumberString(pDC,m_lMePlayerScore,m_PointJetton[0].x,m_PointJetton[0].y+25, true);
	if ( 0 < m_lMeTieScore) DrawNumberString(pDC,m_lMeTieScore,m_PointJetton[1].x,m_PointJetton[1].y+25, true);
	if ( 0 < m_lMeBankerScore ) DrawNumberString(pDC,m_lMeBankerScore,m_PointJetton[2].x,m_PointJetton[2].y+25, true);
	if ( 0 < m_lMePlayerKingScore ) DrawNumberString(pDC,m_lMePlayerKingScore,m_PointJetton[3].x,m_PointJetton[3].y+25, true);
	if ( 0 < m_lMeBankerKingScore ) DrawNumberString(pDC,m_lMeBankerKingScore,m_PointJetton[4].x,m_PointJetton[4].y+25, true);
	if ( 0 < m_lMeTieSamePointScore ) DrawNumberString(pDC,m_lMeTieSamePointScore,m_PointJetton[5].x,m_PointJetton[5].y+25, true);
}

//////////////////////////////////////////////////////////////////////////

