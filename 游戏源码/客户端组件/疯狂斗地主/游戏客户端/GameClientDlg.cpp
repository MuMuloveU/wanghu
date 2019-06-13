#include "Stdafx.h"
#include "GameOption.h"
#include "GameClient.h"
#include "GameClientDlg.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

//游戏定时器
#define IDI_OUT_CARD					200								//出牌定时器
#define IDI_MOST_CARD					201								//最大定时器
#define IDI_START_GAME					202								//开始定时器
#define IDI_LAND_SCORE					203								//叫分定时器
#define IDI_DISPATCH_CARD				204								//发牌定时器
#define IDI_DOUBLE_SCORE				205								//加倍定时器

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientDlg, CGameFrameDlg)
	ON_WM_TIMER()
	ON_MESSAGE(IDM_START,OnStart)
	ON_MESSAGE(IDM_OUT_CARD,OnOutCard)
	ON_MESSAGE(IDM_PASS_CARD,OnPassCard)
	ON_MESSAGE(IDM_LAND_SCORE,OnLandScore)
	ON_MESSAGE(IDM_AUTO_OUTCARD,OnAutoOutCard)
	ON_MESSAGE(IDM_LEFT_HIT_CARD,OnLeftHitCard)
	ON_MESSAGE(IDM_RIGHT_HIT_CARD,OnRightHitCard)
	ON_MESSAGE(IDM_LAND_AUTOPLAY,OnAutoPlay)
	ON_MESSAGE(IDM_SORT_HAND_CARD,OnMessageSortCard)

	ON_MESSAGE(IDM_BRIGHT_START,OnBrightStart)
	ON_MESSAGE(IDM_DOUBLE_SCORE, OnDoubleScore)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientDlg::CGameClientDlg() : CGameFrameDlg(&m_GameClientView)
{
	//游戏变量
	m_wBombTime=1;
	memset(m_bHandCardData,0,sizeof(m_bHandCardData));
	memset(m_bHandCardCount,0,sizeof(m_bHandCardCount));
	m_wLandUser=INVALID_CHAIR;
	memset(m_bCardCount,0,sizeof(m_bCardCount));
	memset(m_bHandCardData,0,sizeof(m_bHandCardData));
	m_bAutoPlay = false ;
	m_cbSortType=ST_COUNT;

	//配置变量
	m_dwCardHSpace=DEFAULT_PELS;

	//出牌变量
	m_bTurnCardCount=0;
	m_bTurnOutType=CT_ERROR;
	memset(m_bTurnCardData,0,sizeof(m_bTurnCardData));

	//辅助变量
	m_wTimeOutCount=0;
	m_wMostUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_wTimeOutCount=0;
    
	//辅助变量
	m_cbRemnantCardCount=0;
	m_cbDispatchCardCount=0;
	ZeroMemory(m_cbDispatchCardData,sizeof(m_cbDispatchCardData));

	//疯狂变量
	m_cbCallScorePhase = CSD_NORMAL;

	return;
}

//析构函数
CGameClientDlg::~CGameClientDlg()
{
}

//初始函数
bool CGameClientDlg::InitGameFrame()
{
	//设置标题
	SetWindowText(TEXT("斗地主游戏  --  Ver：6.0.1.0"));

	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//读取配置
	m_dwCardHSpace=AfxGetApp()->GetProfileInt(TEXT("GameOption"),TEXT("CardSpace"),DEFAULT_PELS);
	m_bDeasilOrder=AfxGetApp()->GetProfileInt(TEXT("GameOption"),TEXT("DeasilOrder"),FALSE)?true:false;
	m_bAllowLookon = AfxGetApp()->GetProfileInt(TEXT("GameOption"),TEXT("AllowLookon"),FALSE)?true:false;

	//调整参数
	if ((m_dwCardHSpace>MAX_PELS)||(m_dwCardHSpace<LESS_PELS)) m_dwCardHSpace=DEFAULT_PELS;

	//配置控件
	m_GameClientView.m_HandCardControl[1].SetCardSpace(m_dwCardHSpace,0,20);

	return true;
}

//重置框架
void CGameClientDlg::ResetGameFrame()
{
	//游戏变量
	m_wBombTime=1;
	memset(m_bHandCardData,0,sizeof(m_bHandCardData));
	memset(m_bHandCardCount,0,sizeof(m_bHandCardCount));
	m_wLandUser=INVALID_CHAIR;
	memset(m_bCardCount,0,sizeof(m_bCardCount));
	memset(m_bHandCardData,0,sizeof(m_bHandCardData));
	m_bAutoPlay = false ;
	m_cbSortType=ST_COUNT;

	//出牌变量
	m_bTurnCardCount=0;
	m_bTurnOutType=CT_ERROR;
	memset(m_bTurnCardData,0,sizeof(m_bTurnCardData));

	//辅助变量
	m_wTimeOutCount=0;
	m_wMostUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_wTimeOutCount=0;
    
	//辅助变量
	m_cbRemnantCardCount=0;
	m_cbDispatchCardCount=0;
	ZeroMemory(m_cbDispatchCardData,sizeof(m_cbDispatchCardData));

	//疯狂变量
	m_cbCallScorePhase = CSD_NORMAL;

	//删除定时
	KillGameTimer(0);
	KillTimer(IDI_MOST_CARD);
	KillTimer(IDI_DISPATCH_CARD);

	return;
}

//游戏设置
void CGameClientDlg::OnGameOptionSet()
{
	//构造数据
	CGameOption GameOption;
	GameOption.m_dwCardHSpace=m_dwCardHSpace;
	GameOption.m_bEnableSound=IsEnableSound();
	GameOption.m_bDeasilOrder=m_GameClientView.IsDeasilOrder();
	GameOption.m_bAllowLookOn=IsAllowUserLookon();

	//配置数据
	if (GameOption.DoModal()==IDOK)
	{
		//获取参数
		m_bDeasilOrder=GameOption.m_bDeasilOrder;
		m_dwCardHSpace=GameOption.m_dwCardHSpace;

		//设置控件
		EnableSound(GameOption.m_bEnableSound);
		m_GameClientView.SetUserOrder(GameOption.m_bDeasilOrder);
		m_GameClientView.m_HandCardControl[1].SetCardSpace(m_dwCardHSpace,0,20);
		AllowUserLookon(0,GameOption.m_bAllowLookOn);

		//保存配置
		AfxGetApp()->WriteProfileInt(TEXT("GameOption"),TEXT("CardSpace"),m_dwCardHSpace);
		AfxGetApp()->WriteProfileInt(TEXT("GameOption"),TEXT("DeasilOrder"),m_bDeasilOrder?TRUE:FALSE);
		AfxGetApp()->WriteProfileInt(TEXT("GameOption"),TEXT("AllowLookon"),m_bDeasilOrder?TRUE:FALSE);
		AfxGetApp()->WriteProfileInt(TEXT("GameOption"),TEXT("CardSpace"),m_dwCardHSpace);
	}

	return;
}

//时间消息
bool CGameClientDlg::OnTimerMessage(WORD wChairID, UINT nElapse, UINT nTimerID)
{
	switch (nTimerID)
	{
	case IDI_OUT_CARD:			//用户出牌
		{
			//自动处理
			if ((((nElapse==0)||((m_bAutoPlay==true)&&(nElapse<29))))&&(wChairID==GetMeChairID()))
			{
				//超时判断
				if ((IsLookonMode()==false)&&(m_bAutoPlay==false)&&((++m_wTimeOutCount)>=3)) 
				{
					//状态切换
					m_wTimeOutCount=0;
					
					//托管功能
					m_GameClientView.m_btAutoPlayOn.ShowWindow(SW_HIDE);
					m_GameClientView.m_btAutoPlayOff.ShowWindow(SW_SHOW);
					InsertSystemString( TEXT( "由于您多次超时，切换为“系统托管”模式" ) );
					m_bAutoPlay = true;

					CMD_C_UserTrustee UserTrustee;
					ZeroMemory( &UserTrustee, sizeof( UserTrustee ) );
					UserTrustee.bTrustee = true;
					UserTrustee.wUserChairID = GetMeChairID();

					SendData( SUB_C_TRUSTEE, &UserTrustee, sizeof( UserTrustee ) );
				}

				//自动开始
				if ( m_GameClientView.m_btStart.IsWindowVisible() ) {
					OnStart( 0, 0 );
				}
				else {
					//自动出牌
					OnAutoOutCard( 0, 0 ) ;
					OnOutCard( 0, 0 ) ;
				}
				return true;
			}

			//播放声音
			WORD wMeChairID=GetMeChairID();
			if (IsLookonMode() ==false && m_bHandCardCount[wMeChairID]<m_bTurnCardCount) return true;
			if ((nElapse<=10)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false)) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));

			return true;
		}
	case IDI_START_GAME:		//开始游戏
		{
			if (nElapse==0)
			{
				if ((IsLookonMode()==false)&&(wChairID==GetMeChairID())) 
				{
					PostMessage(WM_CLOSE,0,0);
				}
				return false;
			}
			if ((nElapse<=10)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false)) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));

			return true;
		}
	case IDI_LAND_SCORE:		//挖坑叫分
		{
			if ( nElapse==0 || ( nElapse <= 27 && m_bAutoPlay ) )
			{
				if ((IsLookonMode()==false)&&(wChairID==GetMeChairID())) OnLandScore(255,255);
				return false;
			}
			if ((nElapse<=10)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false)) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));

			return true;
		}
	case IDI_DOUBLE_SCORE:		//加倍定时
		{
			if (nElapse==0 || ( nElapse <= 27 && m_bAutoPlay ))
			{
				if ((IsLookonMode()==false)&&(wChairID==GetMeChairID())) 
				{
					//发送消息
					SendMessage( IDM_DOUBLE_SCORE, 1, 1 );
				}
				return false;
			}
			if ((nElapse<=10)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false)) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));

			return true;
		}
	}

	return false;
}

//旁观状态
void CGameClientDlg::OnLookonChanged(bool bLookonUser, const void * pBuffer, WORD wDataSize)
{
	//旁观设置
	m_GameClientView.m_HandCardControl[1].SetDisplayFlag( ( IsLookonMode() == true ) && ( IsAllowLookon() == true ) );

	return;
}

//网络消息
bool CGameClientDlg::OnGameMessage(WORD wSubCmdID, const void * pBuffer, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_SEND_CARD:		//发送扑克
		{
			return OnSubSendCard(pBuffer,wDataSize);
		}
	case SUB_S_LAND_SCORE:	//用户叫分
		{
			return OnSubLandScore(pBuffer,wDataSize);
		}
	case SUB_S_GAME_START:		//游戏开始
		{
			return OnSubGameStart(pBuffer,wDataSize);
		}
	case SUB_S_OUT_CARD:		//用户出牌
		{
			return OnSubOutCard(pBuffer,wDataSize);
		}
	case SUB_S_PASS_CARD:		//放弃出牌
		{
			return OnSubPassCard(pBuffer,wDataSize);
		}
	case SUB_S_GAME_END:		//游戏结束
		{
			return OnSubGameEnd(pBuffer,wDataSize);
		}
	case SUB_S_BRIGHT_START:	//明牌开始
		{
			return OnSubBrightStart(pBuffer,wDataSize);			
		}
	case SUB_S_BRIGHT_CARD:	//玩家明牌
		{
			return OnSubBrightCard(pBuffer,wDataSize);
		}
	case SUB_C_TRUSTEE:			//玩家托管
		{
			return OnSubPlayerTrustee(pBuffer,wDataSize);
		}
	case SUB_S_DOUBLE_SCORE:	//加倍消息
		{
			return OnSubDoubleScore(pBuffer, wDataSize);
		}
	case SUB_S_USER_DOUBLE:		//玩家加倍
		{
			return OnSubUserDouble(pBuffer, wDataSize);
		}
	}

	return false;
}

//游戏场景
bool CGameClientDlg::OnGameSceneMessage(BYTE cbGameStation, bool bLookonOther, const void * pBuffer, WORD wDataSize)
{
	switch (cbGameStation)
	{
	case GS_WK_FREE:	//空闲状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pBuffer;

			//设置界面
			m_GameClientView.SetBaseScore(pStatusFree->lBaseScore);
			for ( WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID)
			{
				WORD wViewID = SwitchViewChairID(wChairID) ;
				bool bBrightStart = pStatusFree->bBrightStart[wChairID];
				m_GameClientView.SetBrightStart(wViewID, bBrightStart );
				if ( bBrightStart )
				{
					m_GameClientView.SetBrightTime( BRIGHT_START_TIME );
					m_GameClientView.ShowLandTime( true );
				}
			}

			//设置控件
			if (IsLookonMode()==false)
			{
				m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
				m_GameClientView.m_btBrightStart.ShowWindow(SW_SHOW);
				m_GameClientView.m_btStart.SetFocus();
			}
			else
			{
				m_GameClientView.m_btAutoPlayOff.ShowWindow( SW_HIDE ) ;
				m_GameClientView.m_btAutoPlayOn.ShowWindow( SW_HIDE ) ;
				m_GameClientView.m_btAutoPlayOff.EnableWindow( FALSE ) ;
				m_GameClientView.m_btAutoPlayOn.EnableWindow( FALSE ) ;
				m_GameClientView.m_btSortCard.ShowWindow( SW_HIDE );
			}

			//设置扑克
			m_GameClientView.m_HandCardControl[1].SetDisplayFlag((IsLookonMode()==false)||(IsAllowLookon()==true));

			//设置时间
			SetGameTimer(GetMeChairID(),IDI_START_GAME,30);

			return true;
		}
	case GS_WK_SCORE:	//叫分状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusScore)) return false;
			CMD_S_StatusScore * pStatusScore=(CMD_S_StatusScore *)pBuffer;

			//设置变量
					//设置变量
			for (WORD i=0;i<GAME_PLAYER;i++) 
			{
				m_bCardCount[i]=17;
				m_bHandCardCount[i]=17;
				CopyMemory(m_bHandCardData[i],pStatusScore->bCardData[i],sizeof(BYTE)*m_bHandCardCount[i]);

				//排序扑克
				m_GameLogic.SortCardList(m_bHandCardData[i], 17, ST_ORDER);
			}
			//设置界面
			for (WORD i=0;i<GAME_PLAYER;i++)	
			{
				WORD wViewChairID=SwitchViewChairID(i);
				m_GameClientView.SetCardCount(wViewChairID,m_bCardCount[i]);
				m_GameClientView.SetLandScore(wViewChairID,pStatusScore->bScoreInfo[i],pStatusScore->bCallScorePhase,pStatusScore->bCallScorePhase);
			}
			m_GameClientView.ShowLandTitle(true);
			m_GameClientView.SetBaseScore(pStatusScore->lBaseScore);
			m_GameClientView.SetBrightTime(pStatusScore->bBrightTime);
			m_GameClientView.ShowLandTime(true);

			//按钮控制
			if ((IsLookonMode()==false)&&(pStatusScore->wCurrentUser==GetMeChairID()))
			{
				m_GameClientView.m_btGiveUpScore.ShowWindow(SW_SHOW);

				m_GameClientView.m_btGiveUpScore.ShowWindow(SW_SHOW);
				m_GameClientView.m_btGiveUpScore.SetButtonImage(IDB_GIVE_UP,AfxGetInstanceHandle(),false);

				if (pStatusScore->bCallScorePhase==CSD_NORMAL)
				{
					m_GameClientView.m_btOneScore.ShowWindow( SW_HIDE );
					m_GameClientView.m_btTwoScore.ShowWindow( SW_HIDE );
					m_GameClientView.m_btThreeScore.ShowWindow( SW_SHOW );

					m_GameClientView.m_btOneScore.EnableWindow(pStatusScore->bLandScore==0?TRUE:FALSE);
					m_GameClientView.m_btTwoScore.EnableWindow(pStatusScore->bLandScore<=1?TRUE:FALSE);
					m_GameClientView.m_btThreeScore.EnableWindow(pStatusScore->bLandScore<=2?TRUE:FALSE);
				}
				else if (pStatusScore->bCallScorePhase==CSD_SNATCHLAND)
				{
					m_GameClientView.m_btSnatchLand.ShowWindow(SW_SHOW);
					m_GameClientView.m_btGiveUpScore.SetButtonImage(IDB_NOT_SNATCH_LAND,AfxGetInstanceHandle(),false);
				}
				else if (pStatusScore->bCallScorePhase==CSD_BRIGHTCARD)
				{
					m_GameClientView.m_btBrightCard.ShowWindow(SW_SHOW);
					m_GameClientView.m_btGiveUpScore.SetButtonImage(IDB_NOT_BRIGHT_CARD,AfxGetInstanceHandle(),false);
				}
			}

			if ( IsLookonMode() )
			{
				m_GameClientView.m_btAutoPlayOff.ShowWindow( SW_HIDE ) ;
				m_GameClientView.m_btAutoPlayOn.ShowWindow( SW_HIDE ) ;
				m_GameClientView.m_btAutoPlayOff.EnableWindow( FALSE ) ;
				m_GameClientView.m_btAutoPlayOn.EnableWindow( FALSE ) ;
				m_GameClientView.m_btSortCard.ShowWindow( SW_HIDE );
			}
			else 
			{
				m_GameClientView.m_btSortCard.EnableWindow(TRUE);
			}

			//设置扑克
			BYTE bCardData[3]={0,0,0};
			WORD wMeChairID=GetMeChairID();
			m_GameClientView.m_BackCardControl.SetCardData(bCardData,3);
			m_GameClientView.m_HandCardControl[1].SetCardData(m_bHandCardData[wMeChairID],m_bHandCardCount[wMeChairID]);
			m_GameClientView.m_HandCardControl[1].SetDisplayFlag((IsLookonMode()==false)||(IsAllowLookon()==true));

			for ( WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID )
			{
				m_GameClientView.SetUserTrustee( SwitchViewChairID( wChairID ), pStatusScore->bUserTrustee[ wChairID ] );
				
				//自己判断
				if ( ! IsLookonMode() && wChairID == GetMeChairID() && pStatusScore->bUserTrustee[ wChairID ] )
				{
					//托管功能
					m_GameClientView.m_btAutoPlayOn.ShowWindow(SW_HIDE);
					m_GameClientView.m_btAutoPlayOff.ShowWindow(SW_SHOW);
					OnAutoPlay( 1, 1 );
				}
			}

			//设置扑克
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				WORD wViewChairID = SwitchViewChairID(i);
				m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(m_bHandCardData[i],m_bHandCardCount[i]);
			}

			//明牌判断
			for ( WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID )
			{
				WORD wViewChairID = SwitchViewChairID( wChairID );

				//显示扑克
				if ( pStatusScore->bUserBrightCard[wChairID] )
				{
					m_GameClientView.m_HandCardControl[wViewChairID].ShowWindow(SW_SHOW);
					m_GameClientView.m_HandCardControl[wViewChairID].SetDisplayFlag(true);
				}
			}

			//设置时间
			if ( pStatusScore->bCallScorePhase == CSD_NORMAL )
				SetGameTimer(pStatusScore->wCurrentUser,IDI_LAND_SCORE,30);
			else
				SetGameTimer(pStatusScore->wCurrentUser,IDI_LAND_SCORE,10);

			return true;
		}
	case GS_WK_DOUBLE_SCORE:	//加倍状态
			{
				//效验数据
				if (wDataSize!=sizeof(CMD_S_StatusDoubleScore)) return false;
				CMD_S_StatusDoubleScore * pStatusDoubleScore=(CMD_S_StatusDoubleScore *)pBuffer;

				//设置按钮
				if ( ! IsLookonMode() )
				{
					bool bAllowDouble = pStatusDoubleScore->bAllowDouble;
					m_GameClientView.m_btDoubleScore.ShowWindow( SW_SHOW );
					m_GameClientView.m_btDoubleScore.EnableWindow( bAllowDouble ? TRUE : FALSE );
					m_GameClientView.m_btNotDoubleScore.ShowWindow( SW_SHOW );
				}

				//地主设置
				m_wLandUser = pStatusDoubleScore->wLandUser;
				m_GameClientView.SetLandUser(SwitchViewChairID(pStatusDoubleScore->wLandUser), pStatusDoubleScore->bLandScore );
				m_GameClientView.ShowLandTime(true);

				//状态设置
				m_GameClientView.SetLandScore(INVALID_CHAIR,0,CSD_GAMESTART,CSD_GAMESTART);

				//基础设置
				m_GameClientView.SetBaseScore(pStatusDoubleScore->lBaseScore);

				//设置扑克
				for (WORD i=0;i<GAME_PLAYER;i++)
				{
					m_bHandCardCount[i]=pStatusDoubleScore->bCardCount[i];
					CopyMemory(m_bHandCardData[i],pStatusDoubleScore->bCardData[i],m_bHandCardCount[i]);
					
					//排序扑克
					m_GameLogic.SortCardList(m_bHandCardData[i], m_bHandCardCount[i], ST_ORDER);

					WORD wViewChairID = SwitchViewChairID(i);
					m_GameClientView.SetCardCount(wViewChairID, m_bHandCardCount[i]);
				}
				m_GameClientView.m_BackCardControl.SetCardData(pStatusDoubleScore->bBackCard, 3);
				m_GameClientView.m_BackCardControl.SetDisplayFlag(true);
				WORD wMeChairID = GetMeChairID();
				m_GameClientView.m_HandCardControl[1].SetCardData(m_bHandCardData[wMeChairID],m_bHandCardCount[wMeChairID]);
				if ( ! IsLookonMode() )
				{
					m_GameClientView.m_HandCardControl[1].SetDisplayFlag(true);
					m_GameClientView.m_HandCardControl[1].SetPositively(true);
				}
				
				//加倍玩家
				for ( WORD wChairID = 0; wChairID < GAME_PLAYER; wChairID++ )
				{
					WORD wViewChairID = SwitchViewChairID( wChairID );
					m_GameClientView.SetDoubleScoreUser( wViewChairID, pStatusDoubleScore->bDoubleUser[wChairID] );
				}

				//托管设置
				for ( WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID )
				{
					m_GameClientView.SetUserTrustee( SwitchViewChairID( wChairID ), pStatusDoubleScore->bUserTrustee[ wChairID ] );
					
					//自己判断
					if ( ! IsLookonMode() && wChairID == GetMeChairID() && pStatusDoubleScore->bUserTrustee[ wChairID ] )
					{
						//托管功能
						m_GameClientView.m_btAutoPlayOn.ShowWindow(SW_HIDE);
						m_GameClientView.m_btAutoPlayOff.ShowWindow(SW_SHOW);
						OnAutoPlay( 1, 1 );
					}
				}

				//设置消息
				if ( ! IsLookonMode() ) SetGameTimer(GetMeChairID(), IDI_DOUBLE_SCORE, 30);

				//设置扑克
				for (WORD i=0;i<GAME_PLAYER;i++)
				{
					WORD wViewChairID = SwitchViewChairID(i);
					m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(m_bHandCardData[i],m_bHandCardCount[i]);
				}

				//明牌判断
				for ( WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID )
				{
					WORD wViewChairID = SwitchViewChairID( wChairID );

					//显示扑克
					if ( pStatusDoubleScore->bUserBrightCard[wChairID] )
					{
						m_GameClientView.m_HandCardControl[wViewChairID].ShowWindow(SW_SHOW);
						m_GameClientView.m_HandCardControl[wViewChairID].SetDisplayFlag(true);
					}
				}

				return true;
			}
	case GS_WK_PLAYING:		//游戏状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pBuffer;

			//设置变量
			m_bTurnCardCount=pStatusPlay->bTurnCardCount;
			m_bTurnOutType=m_GameLogic.GetCardType(pStatusPlay->bTurnCardData,m_bTurnCardCount);
			CopyMemory(m_bTurnCardData,pStatusPlay->bTurnCardData,pStatusPlay->bTurnCardCount);

			//设置扑克
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				m_bHandCardCount[i]=pStatusPlay->bCardCount[i];
				CopyMemory(m_bHandCardData[i],pStatusPlay->bCardData[i],m_bHandCardCount[i]);

				//排序扑克
				m_GameLogic.SortCardList(m_bHandCardData[i], m_bHandCardCount[i], ST_ORDER);
			}

			//设置界面
			for (BYTE i=0;i<GAME_PLAYER;i++)
			{
				WORD wViewChairID=SwitchViewChairID(i);
				m_bCardCount[i]=pStatusPlay->bCardCount[i];
				m_GameClientView.SetCardCount(wViewChairID,pStatusPlay->bCardCount[i]);
				m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(m_bHandCardData[i],m_bHandCardCount[i]);
			}
			m_GameClientView.SetBombTime(pStatusPlay->wBombTime);
			m_GameClientView.SetBaseScore(pStatusPlay->lBaseScore);
			m_GameClientView.m_BackCardControl.SetCardData(pStatusPlay->bBackCard,3);
			m_GameClientView.m_BackCardControl.SetDisplayFlag( true );
			WORD wMeChairID = GetMeChairID();
			m_GameClientView.m_HandCardControl[1].SetCardData(m_bHandCardData[wMeChairID],m_bHandCardCount[wMeChairID]);
			m_GameClientView.SetLandUser(SwitchViewChairID(pStatusPlay->wLandUser),pStatusPlay->bLandScore);
			m_GameClientView.SetBrightTime(pStatusPlay->bBrightTime);
			m_GameClientView.ShowLandTime(false);

			//玩家设置
			if ((IsLookonMode()==false)&&(pStatusPlay->wCurrentUser==GetMeChairID()))
			{
				m_GameClientView.m_btOutCard.EnableWindow(FALSE);
				m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
				m_GameClientView.m_btPassCard.ShowWindow(SW_SHOW);
				m_GameClientView.m_btAutoOutCard.ShowWindow(SW_SHOW);
				m_GameClientView.m_btPassCard.EnableWindow((m_bTurnCardCount!=0)?TRUE:FALSE);
				m_GameClientView.m_btAutoOutCard.EnableWindow(TRUE);
			}

			//桌面设置
			if (m_bTurnCardCount!=0)
			{
				WORD wViewChairID=SwitchViewChairID(pStatusPlay->wLastOutUser);
				m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(m_bTurnCardData,m_bTurnCardCount);
			}

			if ( IsLookonMode() )
			{
				m_GameClientView.m_btAutoPlayOff.ShowWindow( SW_HIDE ) ;
				m_GameClientView.m_btAutoPlayOn.ShowWindow( SW_HIDE ) ;
				m_GameClientView.m_btAutoPlayOff.EnableWindow( FALSE ) ;
				m_GameClientView.m_btAutoPlayOn.EnableWindow( FALSE ) ;
				m_GameClientView.m_btSortCard.ShowWindow( SW_HIDE );
			}
			else
			{
				m_GameClientView.m_btSortCard.EnableWindow(TRUE);
			}

			for ( WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID )
			{
				m_GameClientView.SetUserTrustee( SwitchViewChairID( wChairID ), pStatusPlay->bUserTrustee[ wChairID ] );
				
				//自己判断
				if ( ! IsLookonMode() && wChairID == GetMeChairID() &&pStatusPlay->bUserTrustee[ wChairID ] )
				{
					//托管功能
					m_GameClientView.m_btAutoPlayOn.ShowWindow(SW_HIDE);
					m_GameClientView.m_btAutoPlayOff.ShowWindow(SW_SHOW);
					OnAutoPlay( 1, 1 );
				}
			}

			//设置定时器
			SetGameTimer(pStatusPlay->wCurrentUser,IDI_OUT_CARD,30);

			//设置扑克
			m_GameClientView.m_HandCardControl[1].SetPositively(IsLookonMode()==false);
			m_GameClientView.m_HandCardControl[1].SetDisplayFlag((IsLookonMode()==false)||(IsAllowLookon()==true));

			//明牌判断
			for ( WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID )
			{
				WORD wViewChairID = SwitchViewChairID( wChairID );

				//显示扑克
				if ( pStatusPlay->bUserBrightCard[wChairID] )
				{
					m_GameClientView.m_HandCardControl[wViewChairID].ShowWindow(SW_SHOW);
					m_GameClientView.m_HandCardControl[wViewChairID].SetDisplayFlag(true);
				}
				else if ( wViewChairID != 1 ) m_GameClientView.m_HandCardControl[wViewChairID].SetDisplayFlag(false);
			}

			return true;
		}
	}

	return false;
}

//发送扑克
bool CGameClientDlg::OnSubSendCard(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_SendAllCard));
	if (wDataSize!=sizeof(CMD_S_SendAllCard)) return false;

	//变量定义
	CMD_S_SendAllCard * pSendCard=(CMD_S_SendAllCard *)pBuffer;

	//关闭成绩
	if (m_GameClientView.m_ScoreView.m_hWnd!=NULL)
	{
		m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE) ;
		m_GameClientView.m_ScoreView.DestroyWindow();
	}

	//隐藏扑克
//	m_GameClientView.m_HandCardControl[0].ShowWindow(SW_HIDE);
	m_GameClientView.m_HandCardControl[0].SetDisplayFlag(false);
//	m_GameClientView.m_HandCardControl[2].ShowWindow(SW_HIDE);
	m_GameClientView.m_HandCardControl[2].SetDisplayFlag(false);

	//设置数据
	m_wCurrentUser=pSendCard->wCurrentUser;

	//设置数据
	for (WORD i=0;i<GAME_PLAYER;i++) 
	{
		m_bCardCount[i]=17;
		m_bHandCardCount[i]=17;
		CopyMemory(m_bHandCardData[i],pSendCard->bCardData[i],sizeof(pSendCard->bCardData[i]));

		//排序扑克
		m_GameLogic.SortCardList(m_bHandCardData[i], 17, ST_ORDER);
	}

	//设置界面
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameClientView.SetLandScore(i,0,CSD_NORMAL,CSD_NORMAL);
		m_GameClientView.SetPassFlag(i,false);
		m_GameClientView.m_UserCardControl[i].SetCardData(NULL,0);
		m_GameClientView.m_HandCardControl[i].SetCardData(NULL,0);
	}
	WORD wMeChairID = GetMeChairID();
	if ( ! IsLookonMode() && ! pSendCard->bUserBrightCard[wMeChairID] ) m_GameClientView.m_btBrightCard.ShowWindow( SW_SHOW );
	else m_GameClientView.m_btBrightCard.ShowWindow( SW_HIDE );

	//设置界面
	m_GameClientView.ShowLandTitle(true);
	m_GameClientView.SetBombTime(m_wBombTime);
	m_GameClientView.SetLandUser(INVALID_CHAIR,0);

	//设置扑克
	BYTE bBackCard[3]={0,0,0};
	m_GameClientView.m_BackCardControl.SetCardData(bBackCard,CountArray(bBackCard));
	m_GameClientView.m_HandCardControl[1].SetDisplayFlag(((IsLookonMode()==false)||(IsAllowLookon()==true)));

	//排放扑克
	wMeChairID = GetMeChairID();
	DispatchUserCard(m_bHandCardData[wMeChairID],m_bHandCardCount[wMeChairID]);
	m_GameLogic.SortCardList(m_bHandCardData[wMeChairID],m_bHandCardCount[wMeChairID],ST_ORDER);

	//明牌判断
	for ( WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID )
	{
		WORD wViewChairID = SwitchViewChairID( wChairID );

		//显示扑克
		if ( pSendCard->bUserBrightCard[wChairID] )
		{
			m_GameClientView.m_HandCardControl[wViewChairID].ShowWindow(SW_SHOW);
			m_GameClientView.m_HandCardControl[wViewChairID].SetDisplayFlag(true);
		}
		else if ( wChairID != GetMeChairID() )
		{
	//		m_GameClientView.m_HandCardControl[wViewChairID].ShowWindow(SW_HIDE);
			m_GameClientView.m_HandCardControl[wViewChairID].SetDisplayFlag(false);
		}
	}

	return true;
}

//用户叫分
bool CGameClientDlg::OnSubLandScore(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_LandScore));
	if (wDataSize!=sizeof(CMD_S_LandScore)) return false;

	//消息处理
	CMD_S_LandScore * pLandScore=(CMD_S_LandScore *)pBuffer;

	//设置界面
	WORD wViewChairID=SwitchViewChairID(pLandScore->bLandUser);
	m_GameClientView.SetLandScore(wViewChairID,pLandScore->bLandScore,pLandScore->bCallScorePhase,pLandScore->bPreCallScorePhase);
	m_GameClientView.SetLandUser(wViewChairID, pLandScore->bCurrentScore);
	m_GameClientView.SetBrightStart(INVALID_CHAIR, false);

	//设置变量
	m_cbCallScorePhase = pLandScore->bCallScorePhase;

	//玩家设置
	if ((IsLookonMode()==false)&&( ( pLandScore->wCurrentUser == GetMeChairID() && pLandScore->bCallScorePhase!=CSD_BRIGHTCARD) || ( pLandScore->bCallScorePhase==CSD_BRIGHTCARD && 
		! pLandScore->bBrightCardUser[GetMeChairID()] )))
	{
		ActiveGameFrame();
		m_GameClientView.m_btGiveUpScore.ShowWindow(SW_SHOW);
		m_GameClientView.m_btGiveUpScore.SetButtonImage(IDB_GIVE_UP,AfxGetInstanceHandle(),false);

		if (pLandScore->bCallScorePhase==CSD_NORMAL)
		{
			m_GameClientView.m_btOneScore.ShowWindow(pLandScore->bCurrentScore==0?SW_HIDE:SW_HIDE);
			m_GameClientView.m_btTwoScore.ShowWindow(pLandScore->bCurrentScore<=1?SW_HIDE:SW_HIDE);
			m_GameClientView.m_btThreeScore.ShowWindow(pLandScore->bCurrentScore<=2?SW_SHOW:SW_HIDE);
		}
		else if (pLandScore->bCallScorePhase==CSD_SNATCHLAND)
		{
			m_GameClientView.m_btSnatchLand.ShowWindow(SW_SHOW);
			m_GameClientView.m_btGiveUpScore.SetButtonImage(IDB_NOT_SNATCH_LAND,AfxGetInstanceHandle(),false);
		}
		else if (pLandScore->bCallScorePhase==CSD_BRIGHTCARD)
		{
			m_GameClientView.m_btBrightCard.ShowWindow(SW_SHOW);
			m_GameClientView.m_btGiveUpScore.SetButtonImage(IDB_NOT_BRIGHT_CARD,AfxGetInstanceHandle(),false);
		}
	}

	//播放声音
	if ( pLandScore->bPreCallScorePhase == CSD_NORMAL && pLandScore->bLandScore == 1 )
		PlayGameSound(AfxGetInstanceHandle(),TEXT("ONE_SCORE"));
	else if ( pLandScore->bPreCallScorePhase == CSD_NORMAL && pLandScore->bLandScore == 2 )
		PlayGameSound(AfxGetInstanceHandle(),TEXT("TWO_SCORE"));
	else if ( pLandScore->bPreCallScorePhase == CSD_NORMAL && pLandScore->bLandScore == 3 )
		PlayGameSound(AfxGetInstanceHandle(),TEXT("CALL_CARD"));
	else if ( pLandScore->bPreCallScorePhase == CSD_NORMAL && pLandScore->bLandScore == 255 )
		PlayGameSound(AfxGetInstanceHandle(),TEXT("GIVE_UP_SCORE"));
	else if ( pLandScore->bPreCallScorePhase == CSD_SNATCHLAND && pLandScore->bLandScore == 4 )
		PlayGameSound(AfxGetInstanceHandle(),TEXT("SNATCH_LAND"));
	else if ( pLandScore->bPreCallScorePhase == CSD_BRIGHTCARD && pLandScore->bLandScore == 5 )
		PlayGameSound(AfxGetInstanceHandle(),TEXT("BRIGHT_CARD"));
	else if ( pLandScore->bPreCallScorePhase == CSD_SNATCHLAND && pLandScore->bLandScore == 255 )
		PlayGameSound(AfxGetInstanceHandle(),TEXT("NOT_SNATCH_LAND"));

	//设置时间
	if ( pLandScore->bCallScorePhase==CSD_BRIGHTCARD )
		SetGameTimer(GetMeChairID(),IDI_LAND_SCORE,10);
	else if ( pLandScore->bCallScorePhase != CSD_NORMAL )
		SetGameTimer(pLandScore->wCurrentUser,IDI_LAND_SCORE,10);
	else 
		SetGameTimer(pLandScore->wCurrentUser,IDI_LAND_SCORE,30);

	return true;
}

//游戏开始
bool CGameClientDlg::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;

	//消息处理
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;

	//设置变量
	m_wBombTime=1;
	m_bTurnCardCount=0;
	m_bTurnOutType=CT_ERROR;
	m_wLandUser=pGameStart->wLandUser;
	m_bCardCount[pGameStart->wLandUser]=20;
	ZeroMemory(m_bTurnCardData,sizeof(m_bTurnCardData));
	m_GameClientView.ShowLandTime(false);
	m_GameClientView.SetBrightStart(INVALID_CHAIR, false);
	m_GameClientView.ShowLandTitle(false);

	//设置界面
	m_GameClientView.SetLandScore(INVALID_CHAIR,0,CSD_GAMESTART,CSD_GAMESTART);
	m_GameClientView.SetDoubleScoreUser(INVALID_CHAIR, false);
	m_GameClientView.m_BackCardControl.SetCardData(pGameStart->bBackCardData,CountArray(pGameStart->bBackCardData));
	m_GameClientView.m_BackCardControl.SetDisplayFlag( true );
	m_GameClientView.SetCardCount(SwitchViewChairID(pGameStart->wLandUser),20);

	//地主设置
	WORD wLandUser=pGameStart->wLandUser;
	if ( IsLookonMode() )
	{
		m_bHandCardCount[wLandUser]=20;
		CopyMemory(&m_bHandCardData[wLandUser][17],pGameStart->bBackCardData,sizeof(pGameStart->bBackCardData));
		m_GameLogic.SortCardList(m_bHandCardData[wLandUser],m_bHandCardCount[wLandUser],ST_ORDER);
		m_GameClientView.m_HandCardControl[SwitchViewChairID(wLandUser)].SetCardData(m_bHandCardData[wLandUser],m_bHandCardCount[wLandUser]);
	}
	m_GameClientView.SetLandUser(SwitchViewChairID(pGameStart->wLandUser),pGameStart->bLandScore);

	//明牌设置
	if (pGameStart->bBrightCard) 
	{
		WORD wViewChariID=SwitchViewChairID(pGameStart->wLandUser);
		m_GameClientView.m_HandCardControl[wViewChariID].SetDisplayFlag(true);
		m_GameClientView.m_HandCardControl[wViewChariID].ShowWindow(SW_SHOW);
	}

	//玩家设置
	if (IsLookonMode()==false) m_GameClientView.m_HandCardControl[1].SetPositively(true);

	//当前玩家
	if ((IsLookonMode()==false)&&(pGameStart->wCurrentUser==GetMeChairID()))
	{
		ActiveGameFrame();
		m_GameClientView.m_btOutCard.EnableWindow(FALSE);
		m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btPassCard.EnableWindow(FALSE);
		m_GameClientView.m_btPassCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btAutoOutCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btAutoOutCard.EnableWindow(FALSE);
	}

	//播放声音
	if (pGameStart->bBrightCard) 
		PlayGameSound(AfxGetInstanceHandle(),TEXT("BRIGHT_CARD"));
	else
        PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));

	//设置时间
	SetGameTimer(pGameStart->wCurrentUser,IDI_OUT_CARD,30);

	//明牌判断
	for ( WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID )
	{
		WORD wViewChairID = SwitchViewChairID( wChairID );

		//显示扑克
		if ( pGameStart->bUserBrightCard[wChairID] )
		{
			m_GameClientView.m_HandCardControl[wViewChairID].ShowWindow(SW_SHOW);
			m_GameClientView.m_HandCardControl[wViewChairID].SetDisplayFlag(true);
		}
	}

	return true;
}

//用户出牌
bool CGameClientDlg::OnSubOutCard(const void * pBuffer, WORD wDataSize)
{
	//变量定义
	CMD_S_OutCard * pOutCard=(CMD_S_OutCard *)pBuffer;
	WORD wHeadSize=sizeof(CMD_S_OutCard)-sizeof(pOutCard->bCardData);

	//效验数据
	ASSERT(wDataSize>=wHeadSize);
	if (wDataSize<wHeadSize) return false;
	ASSERT(wDataSize==(wHeadSize+pOutCard->bCardCount*sizeof(pOutCard->bCardData[0])));
	if (wDataSize!=(wHeadSize+pOutCard->bCardCount*sizeof(pOutCard->bCardData[0]))) return false;

	//删除定时器
	KillTimer(IDI_MOST_CARD);
	KillGameTimer(IDI_OUT_CARD);

	//界面设置
	WORD wOutViewChairID=SwitchViewChairID(pOutCard->wOutCardUser);
	m_bCardCount[pOutCard->wOutCardUser]-=pOutCard->bCardCount;
	m_GameClientView.SetCardCount(wOutViewChairID,m_bCardCount[pOutCard->wOutCardUser]);

	//出牌设置
	if ((IsLookonMode()==true)||(GetMeChairID()!=pOutCard->wOutCardUser))
	{
		m_GameClientView.m_UserCardControl[wOutViewChairID].SetCardData(pOutCard->bCardData,pOutCard->bCardCount);
	}

	//清理桌面
	if (m_bTurnCardCount==0)
	{
		for (WORD i=0;i<GAME_PLAYER;i++) 
		{
			if (i==pOutCard->wOutCardUser) continue;
			m_GameClientView.m_UserCardControl[SwitchViewChairID(i)].SetCardData(NULL,0);
		}
		m_GameClientView.SetPassFlag(INVALID_CHAIR,false);
	}

	//报牌信息
	bool bWarmingCard = false;
	if(m_bCardCount[pOutCard->wOutCardUser]<=2) 
	{
		WORD wViewChairID = SwitchViewChairID(pOutCard->wOutCardUser) ;

		if ( pOutCard->wOutCardUser!=GetMeChairID() )
		{
			PlayGameSound(AfxGetInstanceHandle(),TEXT("WARMING_CARD"));
			bWarmingCard = true;
		}

		m_GameClientView.SetShowWarming(wViewChairID, true) ;
	}

	//记录出牌
	m_bTurnCardCount=pOutCard->bCardCount;
	m_bTurnOutType=m_GameLogic.GetCardType(pOutCard->bCardData,pOutCard->bCardCount);
	CopyMemory(m_bTurnCardData,pOutCard->bCardData,sizeof(BYTE)*pOutCard->bCardCount);

	//炸弹判断
	if ((m_bTurnOutType==CT_BOMB_CARD)||(m_bTurnOutType==CT_MISSILE_CARD))
	{
		m_wBombTime*=2;
		m_GameClientView.SetBombTime(m_wBombTime);

		//播放动画
		m_GameClientView.SetBombEffect(true);
	}

	//自己扑克
	if ((IsLookonMode()==true)||(pOutCard->wOutCardUser!=GetMeChairID()))
	{
		//删除扑克
		WORD wOutCardUser=pOutCard->wOutCardUser;
		BYTE bSourceCount=m_bHandCardCount[wOutCardUser];
		m_bHandCardCount[wOutCardUser]-=pOutCard->bCardCount;
		m_GameLogic.RemoveCard(pOutCard->bCardData,pOutCard->bCardCount,m_bHandCardData[wOutCardUser],bSourceCount);

		//设置界面
		m_GameClientView.m_HandCardControl[SwitchViewChairID(wOutCardUser)].SetCardData(m_bHandCardData[wOutCardUser],m_bHandCardCount[wOutCardUser]);
	}

	//最大判断
	if (pOutCard->wCurrentUser==pOutCard->wOutCardUser)
	{
		//设置变量
		m_bTurnCardCount=0;
		m_bTurnOutType=CT_ERROR;
		m_wMostUser=pOutCard->wCurrentUser;
		memset(m_bTurnCardData,0,sizeof(m_bTurnCardData));

		//设置界面
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			if (i!=pOutCard->wOutCardUser)
			{
				WORD wViewChairID=SwitchViewChairID(i);
				m_GameClientView.SetPassFlag(wViewChairID,true);
				m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);
			}
		}

		//播放声音
		if ( !bWarmingCard ) PlayGameSound(AfxGetInstanceHandle(),TEXT("MOST_CARD"));

		//设置定时器
		SetTimer(IDI_MOST_CARD,3000,NULL);

		return true;
	}

	//播放声音
	if (((IsLookonMode()==true)||(GetMeChairID()!=pOutCard->wOutCardUser)) && !bWarmingCard )
	{
		BYTE cbOutCardType = m_GameLogic.GetCardType(pOutCard->bCardData, pOutCard->bCardCount );
		if ( cbOutCardType == CT_BOMB_CARD )
			PlayGameSound(AfxGetInstanceHandle(),TEXT("BOMB_CARD"));
		else if ( cbOutCardType == CT_MISSILE_CARD )
			PlayGameSound(AfxGetInstanceHandle(),TEXT("MOST_CARD"));
		else
			PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
	}

	//玩家设置
	if (pOutCard->wCurrentUser!=INVALID_CHAIR)
	{
		WORD wViewChairID=SwitchViewChairID(pOutCard->wCurrentUser);
		m_GameClientView.SetPassFlag(wViewChairID,false);
		m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);
	}

	//玩家设置
	if ((IsLookonMode()==false)&&(pOutCard->wCurrentUser==GetMeChairID()))
	{
		ActiveGameFrame();
		m_GameClientView.m_btPassCard.EnableWindow(TRUE);
		m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btPassCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btAutoOutCard.EnableWindow(TRUE);
		m_GameClientView.m_btAutoOutCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btOutCard.EnableWindow((VerdictOutCard()==true)?TRUE:FALSE);
	}

	//设置时间
	if (pOutCard->wCurrentUser!=INVALID_CHAIR)
	{
		BYTE bCardCount=m_bCardCount[pOutCard->wCurrentUser];
		SetGameTimer(pOutCard->wCurrentUser,IDI_OUT_CARD,(bCardCount<m_bTurnCardCount)?3:30);
	}

	return true;
}

//放弃出牌
bool CGameClientDlg::OnSubPassCard(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_PassCard)) return false;
	CMD_S_PassCard * pPassCard=(CMD_S_PassCard *)pBuffer;

	//删除定时器
	KillGameTimer(IDI_OUT_CARD);

	//玩家设置
	if ((IsLookonMode()==true)||(pPassCard->wPassUser!=GetMeChairID()))
	{
		WORD wViewChairID=SwitchViewChairID(pPassCard->wPassUser);
		m_GameClientView.SetPassFlag(wViewChairID,true);
		m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);
	}

	//一轮判断
	if (pPassCard->bNewTurn==TRUE)
	{
		m_bTurnCardCount=0;
		m_bTurnOutType=CT_ERROR;
		memset(m_bTurnCardData,0,sizeof(m_bTurnCardData));
	}

	//设置界面
	WORD wViewChairID=SwitchViewChairID(pPassCard->wCurrentUser);
	m_GameClientView.SetPassFlag(wViewChairID,false);
	m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);

	//玩家设置
	if ((IsLookonMode()==false)&&(pPassCard->wCurrentUser==GetMeChairID()))
	{
		ActiveGameFrame();
		m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btPassCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btAutoOutCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btPassCard.EnableWindow((m_bTurnCardCount>0)?TRUE:FALSE);
		m_GameClientView.m_btOutCard.EnableWindow((VerdictOutCard()==true)?TRUE:FALSE);
		m_GameClientView.m_btAutoOutCard.EnableWindow( TRUE );
	}

	//播放声音
	if ((IsLookonMode()==true)||(pPassCard->wPassUser!=GetMeChairID()))	PlayGameSound(AfxGetInstanceHandle(),TEXT("PASS_CARD"));

	//设置时间
	if (m_bTurnCardCount!=0)
	{
		BYTE bCardCount=m_bCardCount[pPassCard->wCurrentUser];
		SetGameTimer(pPassCard->wCurrentUser,IDI_OUT_CARD,(bCardCount<m_bTurnCardCount)?3:30);
	}
	else SetGameTimer(pPassCard->wCurrentUser,IDI_OUT_CARD,30);

	return true;
}

//游戏结束
bool CGameClientDlg::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameEnd));
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return false;

	//消息处理
	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pBuffer;

	//删除定时器
	KillTimer(IDI_MOST_CARD);
	KillGameTimer(IDI_OUT_CARD);
	KillGameTimer(IDI_LAND_SCORE);
	KillTimer(IDI_DISPATCH_CARD);

	//疯狂变量
	m_cbCallScorePhase = CSD_NORMAL;

	//停止发牌
	DoncludeDispatchCard();

	//隐藏控件
	m_GameClientView.m_btOutCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btPassCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btOneScore.ShowWindow(SW_HIDE);
	m_GameClientView.m_btTwoScore.ShowWindow(SW_HIDE);
	m_GameClientView.m_btGiveUpScore.ShowWindow(SW_HIDE);
	m_GameClientView.m_btAutoOutCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btThreeScore.ShowWindow(SW_HIDE);
	m_GameClientView.m_btSortCard.EnableWindow(FALSE);
	m_GameClientView.m_btSnatchLand.ShowWindow(SW_HIDE);
	m_GameClientView.m_btBrightCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btDoubleScore.ShowWindow(SW_HIDE);
	m_GameClientView.m_btNotDoubleScore.ShowWindow(SW_HIDE);
    
	//禁用控件
	m_GameClientView.m_btOutCard.EnableWindow(FALSE);
	m_GameClientView.m_btPassCard.EnableWindow(FALSE);

	//界面设置
	m_GameClientView.SetShowWarming(INVALID_CHAIR, false) ;

	//创建成绩
	if (m_GameClientView.m_ScoreView.m_hWnd==NULL)
	{
		m_GameClientView.m_ScoreView.Create(IDD_GAME_SCORE,&m_GameClientView);
	}

	InsertShtickString( TEXT( " 本局结束，成绩统计：" ), RGB( 255, 0, 0 ), true );

	//设置积分
	CString strScore;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		const tagUserData * pUserData=GetUserData(i);
		m_GameClientView.m_ScoreView.SetGameScore(i,pUserData->szName,pGameEnd->lGameScore[i]);
		strScore.Format( TEXT( " %s ：%ld" ), pUserData->szName,pGameEnd->lGameScore[i]);
		InsertShtickString( strScore, RGB( 255, 0, 0 ), true );
	}

	//游戏类型
	IClientKernel *pIClientKernel = ( IClientKernel * )GetClientKernel( IID_IClientKernel, VER_IClientKernel );
	const tagServerAttribute *pServerAttribute  = pIClientKernel->GetServerAttribute();

	if ( pServerAttribute->wGameGenre == GAME_GENRE_GOLD ) {
		strScore.Format( TEXT( "扣税：%ld" ), pGameEnd->lGameTax);
		InsertShtickString( strScore, RGB( 255, 0, 0 ), true );
	
	}

	m_GameClientView.m_ScoreView.SetGameTax(pGameEnd->lGameTax);

	//创建成绩
	CRect scoreRect, gameViewRect ;
	m_GameClientView.GetWindowRect( gameViewRect ) ;
	m_GameClientView.m_ScoreView.GetWindowRect( scoreRect ) ;
	m_GameClientView.m_ScoreView.MoveWindow( gameViewRect.left + ( gameViewRect.Width() - scoreRect.Width() ) / 2, 
		gameViewRect.top + ( gameViewRect.Height() - scoreRect.Height() ) / 2, scoreRect.Width(), scoreRect.Height()) ;
	m_GameClientView.m_ScoreView.ShowWindow(SW_SHOW);

	//显示扑克
	BYTE cbCardPos=0 ;
	for(BYTE i=0; i<GAME_PLAYER; ++i)
	{
		WORD wViewID = SwitchViewChairID(i) ;
		m_GameClientView.m_HandCardControl[wViewID].SetCardData(pGameEnd->bCardData+cbCardPos, pGameEnd->bCardCount[i]) ;
		m_GameClientView.m_HandCardControl[wViewID].SetDisplayFlag(true) ;
		m_GameClientView.m_HandCardControl[wViewID].ShowWindow(SW_SHOW);
		cbCardPos += pGameEnd->bCardCount[i] ;
	}

	//播放声音
	WORD wMeChairID=GetMeChairID();
	DOUBLE lMeScore=pGameEnd->lGameScore[GetMeChairID()];
	if (lMeScore>0L) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WIN"));
	else if (lMeScore<0L) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_LOST"));
	else PlayGameSound(GetModuleHandle(NULL),TEXT("GAME_END"));

	//设置界面
	if (IsLookonMode()==false)
	{
		m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
		m_GameClientView.m_btBrightStart.ShowWindow(SW_SHOW);
		SetGameTimer(GetMeChairID(),IDI_START_GAME,30);

		//托管功能
		 m_GameClientView.m_btAutoPlayOn.ShowWindow(SW_SHOW);
		m_GameClientView.m_btAutoPlayOff.ShowWindow(SW_HIDE);
		OnAutoPlay( 0, 0 );
	}
	m_GameClientView.ShowLandTitle(false);

#ifdef VIDEO_GAME
	//显示扑克
	m_GameClientView.m_HandCardControl[ 0 ].ShowWindow( SW_SHOW );
	m_GameClientView.m_HandCardControl[ 2 ].ShowWindow( SW_SHOW );
#endif

	return true;
}

//出牌判断
bool CGameClientDlg::VerdictOutCard()
{
	//状态判断
	if (m_GameClientView.m_btOutCard.IsWindowVisible()==FALSE) return false;

	//获取扑克
	BYTE bCardData[20];
	BYTE bShootCount=(BYTE)m_GameClientView.m_HandCardControl[1].GetShootCard(bCardData,CountArray(bCardData));

	//出牌判断
	if (bShootCount>0L)
	{
		//排序扑克
		m_GameLogic.SortCardList(bCardData,bShootCount,ST_ORDER);

		//分析类型
		BYTE bCardType=m_GameLogic.GetCardType(bCardData,bShootCount);

		//类型判断
		if (bCardType==CT_ERROR) return false;

		//跟牌判断
		if (m_bTurnCardCount==0) return true;
		return m_GameLogic.CompareCard(m_bTurnCardData,bCardData,m_bTurnCardCount,bShootCount);
	}

	return false;
}

//自动出牌
bool CGameClientDlg::AutomatismOutCard()
{
	//先出牌者
	if (m_bTurnCardCount==0)
	{
		//控制界面
		KillGameTimer(IDI_OUT_CARD);
		m_GameClientView.m_btOutCard.ShowWindow(SW_HIDE);
		m_GameClientView.m_btOutCard.EnableWindow(FALSE);
		m_GameClientView.m_btPassCard.ShowWindow(SW_HIDE);
		m_GameClientView.m_btPassCard.EnableWindow(FALSE);
		m_GameClientView.m_btAutoOutCard.ShowWindow(SW_HIDE);
		m_GameClientView.m_btAutoOutCard.EnableWindow(FALSE);

		WORD wMeChairID=GetMeChairID();

		//发送数据
		CMD_C_OutCard OutCard;
		OutCard.bCardCount=1;
		OutCard.bCardData[0]=m_bHandCardData[wMeChairID][m_bHandCardCount[wMeChairID]-1];
		SendData(SUB_C_OUT_CART,&OutCard,sizeof(OutCard)-sizeof(OutCard.bCardData)+OutCard.bCardCount*sizeof(BYTE));

		//预先处理
		PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
		m_GameClientView.m_UserCardControl[1].SetCardData(OutCard.bCardData,OutCard.bCardCount);

		//预先删除
		BYTE bSourceCount=m_bHandCardCount[wMeChairID];
		m_bHandCardCount[wMeChairID]-=OutCard.bCardCount;
		m_GameLogic.RemoveCard(OutCard.bCardData,OutCard.bCardCount,m_bHandCardData[wMeChairID],bSourceCount);
		m_GameClientView.m_HandCardControl[1].SetCardData(m_bHandCardData[wMeChairID],m_bHandCardCount[wMeChairID]);
	}
	else OnPassCard(0,0);

	return true;
}

//停止发牌
bool CGameClientDlg::DoncludeDispatchCard()
{
	//停止发牌
	if (m_cbRemnantCardCount>0)
	{
		//删除时间
		KillTimer(IDI_DISPATCH_CARD);

		//设置界面
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			WORD wViewChairID = SwitchViewChairID(i) ;
			if (wViewChairID!=1)
			{
				BYTE cbCardData[20] ;
				ZeroMemory(cbCardData, sizeof(cbCardData)) ;
				m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(cbCardData,17);
			}
			else{
				WORD wMeChairID = GetMeChairID();
				m_GameClientView.m_HandCardControl[1].SetCardData(m_bHandCardData[wMeChairID],m_bHandCardCount[wMeChairID]); 
			}
		}

		//设置变量
		m_cbRemnantCardCount=0;
		m_cbDispatchCardCount=0;
		ZeroMemory(m_cbDispatchCardData,sizeof(m_cbDispatchCardData));
	}

	return true;
}

//派发扑克
bool CGameClientDlg::DispatchUserCard(BYTE cbCardData[], BYTE cbCardCount)
{
	//设置变量
	m_cbDispatchCardCount=0;
	m_cbRemnantCardCount=cbCardCount;
	CopyMemory(m_cbDispatchCardData,cbCardData,sizeof(BYTE)*cbCardCount);

	//设置定时器
	SetTimer(IDI_DISPATCH_CARD,120,NULL);

	return true;
}

//定时器消息
void CGameClientDlg::OnTimer(UINT nIDEvent)
{
	if ((nIDEvent==IDI_MOST_CARD)&&(m_wMostUser!=INVALID_CHAIR))
	{
		//变量定义
		WORD wCurrentUser=m_wMostUser;
		m_wMostUser=INVALID_CHAIR;

		//删除定时器
		KillTimer(IDI_MOST_CARD);

		//设置界面
		m_GameClientView.SetPassFlag(INVALID_CHAIR,false);
		for (WORD i=0;i<GAME_PLAYER;i++) m_GameClientView.m_UserCardControl[i].SetCardData(NULL,0);

		//玩家设置
		if ((IsLookonMode()==false)&&(wCurrentUser==GetMeChairID()))
		{
			ActiveGameFrame();
			m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
			m_GameClientView.m_btPassCard.ShowWindow(SW_SHOW);
			m_GameClientView.m_btPassCard.EnableWindow(FALSE);
			m_GameClientView.m_btAutoOutCard.ShowWindow(SW_SHOW);
			m_GameClientView.m_btAutoOutCard.EnableWindow(FALSE);
			m_GameClientView.m_btOutCard.EnableWindow((VerdictOutCard()==true)?TRUE:FALSE);
		}

		//设置时间
		SetGameTimer(wCurrentUser,IDI_OUT_CARD,30);

		return;
	}

	//派发扑克
	if (nIDEvent==IDI_DISPATCH_CARD)
	{
		//派发控制
		if (m_cbRemnantCardCount>0)
		{
			//扑克数据
			BYTE cbUserCard[20];
			ZeroMemory(cbUserCard,sizeof(cbUserCard));

			//派发扑克
			m_cbRemnantCardCount--;
			m_cbDispatchCardCount++;
			m_GameLogic.SortCardList(m_cbDispatchCardData,m_cbDispatchCardCount,ST_ORDER);

			//设置界面
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				WORD wViewID = SwitchViewChairID(i) ;
				if (wViewID!=1)
				{
					m_GameClientView.m_HandCardControl[wViewID].SetCardData(m_bHandCardData[i],m_cbDispatchCardCount);
				}
				else m_GameClientView.m_HandCardControl[wViewID].SetCardData(m_cbDispatchCardData,m_cbDispatchCardCount); 
			}

			//播放声音
			PlayGameSound(AfxGetInstanceHandle(),TEXT("DISPATCH_CARD"));
		}

		//中止判断
		if (m_cbRemnantCardCount==0)
		{
			//删除定时器
			KillTimer(IDI_DISPATCH_CARD);

			//扑克数目
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				m_GameClientView.SetCardCount(SwitchViewChairID(i),m_bCardCount[i]);
			}

			//当前玩家
			if ((IsLookonMode()==false)&&(m_wCurrentUser==GetMeChairID()))
			{
				ActiveGameFrame();
				m_GameClientView.m_btOneScore.ShowWindow(SW_HIDE);
				m_GameClientView.m_btTwoScore.ShowWindow(SW_HIDE);
				m_GameClientView.m_btThreeScore.ShowWindow(SW_SHOW);
				m_GameClientView.m_btGiveUpScore.ShowWindow(SW_SHOW);
				m_GameClientView.m_btGiveUpScore.SetButtonImage(IDB_GIVE_UP,AfxGetInstanceHandle(),false);
			}
			//设置界面
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				WORD wViewID = SwitchViewChairID(i) ;
				if (wViewID!=1)
				{
					BYTE cbCardData[20] ;
					ZeroMemory(cbCardData, sizeof(cbCardData)) ;
					m_GameClientView.m_HandCardControl[wViewID].SetCardData(m_bHandCardData[i],17);
				}
				else 
				{
					WORD wMeChairID = GetMeChairID();
					m_GameClientView.m_HandCardControl[wViewID].SetCardData(m_bHandCardData[wMeChairID],m_bHandCardCount[wMeChairID]); 
					if(false==IsLookonMode())
					{
						m_GameClientView.m_HandCardControl[wViewID].SetDisplayFlag(true) ;
						m_GameClientView.m_HandCardControl[wViewID].SetPositively(true) ;
					}
				}
			}

			if(IsLookonMode()==false) 
			{
				m_GameClientView.m_btSortCard.EnableWindow(TRUE);
				m_GameClientView.m_btBrightCard.ShowWindow( SW_HIDE );
			}


			//播放声音
			PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));

			//设置时间
			if (m_wCurrentUser!=INVALID_CHAIR) SetGameTimer(m_wCurrentUser,IDI_LAND_SCORE,30);
		}

		return;
	}

	__super::OnTimer(nIDEvent);
}

//开始按钮
LRESULT CGameClientDlg::OnStart(WPARAM wParam, LPARAM lParam)
{
	//设置变量
	m_wBombTime=1;
	m_wTimeOutCount=0;
	m_bTurnCardCount=0;
	m_bTurnOutType=CT_ERROR;
	m_wMostUser=INVALID_CHAIR;
	memset(m_bHandCardData,0,sizeof(m_bHandCardData));
	memset(m_bTurnCardData,0,sizeof(m_bTurnCardData));
	memset(m_bHandCardCount,0,sizeof(m_bHandCardCount));

	//设置界面
	KillGameTimer(IDI_START_GAME);
	m_GameClientView.ShowLandTitle(false);
	m_GameClientView.SetBombTime(m_wBombTime);
	m_GameClientView.SetCardCount(INVALID_CHAIR,0);
	m_GameClientView.SetLandUser(INVALID_CHAIR,0);
	m_GameClientView.SetLandScore(INVALID_CHAIR,0,CSD_NORMAL,CSD_NORMAL);
	m_GameClientView.SetPassFlag(INVALID_CHAIR,false);
	m_GameClientView.ShowLandTime(true);

	//隐藏控件
	m_GameClientView.m_btStart.ShowWindow(SW_HIDE);
	m_GameClientView.m_btBrightStart.ShowWindow(SW_HIDE);
	if ( m_GameClientView.m_ScoreView.m_hWnd != NULL ) m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE);

	//隐藏扑克
//	m_GameClientView.m_HandCardControl[ 0 ].ShowWindow( SW_HIDE );
//	m_GameClientView.m_HandCardControl[ 2 ].ShowWindow( SW_HIDE );
	m_GameClientView.m_BackCardControl.SetDisplayFlag( false );

	//设置扑克
	m_GameClientView.m_BackCardControl.SetCardData(NULL,0);
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameClientView.m_HandCardControl[i].SetCardData(NULL,0);
		m_GameClientView.m_UserCardControl[i].SetCardData(NULL,0);
	}

	//发送消息
	SendUserReady(NULL,0);

	return 0;
}

//出牌消息
LRESULT CGameClientDlg::OnOutCard(WPARAM wParam, LPARAM lParam)
{
	//状态判断
	if ((m_GameClientView.m_btOutCard.IsWindowEnabled()==FALSE)||
		(m_GameClientView.m_btOutCard.IsWindowVisible()==FALSE)) return 0;

	//设置界面
	KillGameTimer(IDI_OUT_CARD);
	m_GameClientView.m_btOutCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btOutCard.EnableWindow(FALSE);
	m_GameClientView.m_btPassCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btPassCard.EnableWindow(FALSE);
	m_GameClientView.m_btAutoOutCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btAutoOutCard.EnableWindow(FALSE);

	//发送数据
	CMD_C_OutCard OutCard;
	OutCard.bCardCount=(BYTE)m_GameClientView.m_HandCardControl[1].GetShootCard(OutCard.bCardData,CountArray(OutCard.bCardData));
	SendData(SUB_C_OUT_CART,&OutCard,sizeof(OutCard)-sizeof(OutCard.bCardData)+OutCard.bCardCount*sizeof(BYTE));

	//预先显示
	BYTE cbOutCardType = m_GameLogic.GetCardType(OutCard.bCardData, OutCard.bCardCount );
	if ( cbOutCardType == CT_BOMB_CARD )
		PlayGameSound(AfxGetInstanceHandle(),TEXT("BOMB_CARD"));
	else if ( cbOutCardType == CT_MISSILE_CARD )
		PlayGameSound(AfxGetInstanceHandle(),TEXT("MOST_CARD"));
	else
		PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
	m_GameClientView.m_UserCardControl[1].SetCardData(OutCard.bCardData,OutCard.bCardCount);

	//预先删除
	WORD wMeChairID=GetMeChairID();
	BYTE bSourceCount=m_bHandCardCount[wMeChairID];
	m_bHandCardCount[wMeChairID]-=OutCard.bCardCount;
	m_GameLogic.RemoveCard(OutCard.bCardData,OutCard.bCardCount,m_bHandCardData[wMeChairID],bSourceCount);
	m_GameClientView.m_HandCardControl[1].SetCardData(m_bHandCardData[wMeChairID],m_bHandCardCount[wMeChairID]);

	return 0;
}

//放弃出牌
LRESULT CGameClientDlg::OnPassCard(WPARAM wParam, LPARAM lParam)
{
	//状态判断
	if (m_GameClientView.m_btPassCard.IsWindowEnabled()==FALSE) return 0;

	//重置变量
	m_wTimeOutCount=0;

	//设置界面
	KillGameTimer(IDI_OUT_CARD);
	m_GameClientView.m_btOutCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btOutCard.EnableWindow(FALSE);
	m_GameClientView.m_btPassCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btPassCard.EnableWindow(FALSE);
	m_GameClientView.m_btAutoOutCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btAutoOutCard.EnableWindow(FALSE);

	//发送数据
	SendData(SUB_C_PASS_CARD);

	//预先显示
	m_GameClientView.SetPassFlag(1,true);
	PlayGameSound(AfxGetInstanceHandle(),TEXT("PASS_CARD"));
	WORD wMeChairID = GetMeChairID();
	m_GameClientView.m_HandCardControl[1].SetCardData(m_bHandCardData[wMeChairID],m_bHandCardCount[wMeChairID]);

	return 0;
}

//叫分消息
LRESULT CGameClientDlg::OnLandScore(WPARAM wParam, LPARAM lParam)
{
	//设置定时器
	KillGameTimer(IDI_LAND_SCORE);

	//发牌明牌判断
	WORD wHandCardCount = (WORD)m_GameClientView.m_HandCardControl[1].GetCardCount();
	if ( wParam == 5 && wHandCardCount < NORMAL_COUNT )
	{
		//设置界面
		m_GameClientView.m_btBrightCard.ShowWindow( SW_HIDE );

		//设置消息
		CMD_C_BrightCard BrightCard;
		ZeroMemory( &BrightCard, sizeof( BrightCard ) );
		/*if ( wHandCardCount <= 8 ) BrightCard.cbBrightCardTime = 4;
		else if ( wHandCardCount < NORMAL_COUNT ) BrightCard.cbBrightCardTime = 3;*/
		BrightCard.cbBrightCardTime = BRIGHT_CARD_TIME;

		//发送消息
		SendData( SUB_C_BRIGHT, &BrightCard, sizeof( BrightCard ) );

		return 0;
	}

	//明牌判断
	if ( m_cbCallScorePhase == CSD_BRIGHTCARD )
	{
		//设置界面
		m_GameClientView.m_btBrightCard.ShowWindow( SW_HIDE );
		m_GameClientView.m_btGiveUpScore.ShowWindow( SW_HIDE );

		//设置消息
		CMD_C_BrightCard BrightCard;
		ZeroMemory( &BrightCard, sizeof( BrightCard ) );
		BrightCard.cbBrightCardTime = (BYTE)wParam;

		//发送消息
		SendData( SUB_C_BRIGHT, &BrightCard, sizeof( BrightCard ) );

		return 0;
	}

	//设置界面	
	m_GameClientView.m_btOneScore.ShowWindow(SW_HIDE);
	m_GameClientView.m_btTwoScore.ShowWindow(SW_HIDE);
	m_GameClientView.m_btGiveUpScore.ShowWindow(SW_HIDE);
	m_GameClientView.m_btThreeScore.ShowWindow(SW_HIDE);
	m_GameClientView.m_btSnatchLand.ShowWindow(SW_HIDE);
	m_GameClientView.m_btBrightCard.ShowWindow(SW_HIDE);

	//发送数据
	CMD_C_LandScore LandScore;
	LandScore.bLandScore=(BYTE)wParam;
	SendData(SUB_C_LAND_SCORE,&LandScore,sizeof(LandScore));

	return 0;
}

//出牌提示
LRESULT CGameClientDlg::OnAutoOutCard(WPARAM wParam, LPARAM lParam)
{
	//分析结果
	tagOutCardResult OutCardResult;
	WORD wMeChairID = GetMeChairID();
	m_GameLogic.SearchOutCard(m_bHandCardData[wMeChairID],m_bHandCardCount[wMeChairID],m_bTurnCardData,m_bTurnCardCount,OutCardResult);

	//设置界面
	if (OutCardResult.cbCardCount>0)
	{
		//设置界面
		m_GameClientView.m_HandCardControl[1].SetShootCard(OutCardResult.cbResultCard,OutCardResult.cbCardCount);

		//设置控件
		bool bOutCard=VerdictOutCard();
		m_GameClientView.m_btOutCard.EnableWindow((bOutCard==true)?TRUE:FALSE);
	}
	else 
	{
		//放弃出牌
		OnPassCard(0,0);
	}

	return 0;
}

//右键扑克
LRESULT CGameClientDlg::OnLeftHitCard(WPARAM wParam, LPARAM lParam)
{
	//设置控件
	bool bOutCard=VerdictOutCard();
	m_GameClientView.m_btOutCard.EnableWindow(bOutCard?TRUE:FALSE);

	return 0;
}

//左键扑克
LRESULT CGameClientDlg::OnRightHitCard(WPARAM wParam, LPARAM lParam)
{
	//用户出牌
	OnOutCard(0,0);

	return 0;
}

//////////////////////////////////////////////////////////////////////////

LRESULT CGameClientDlg::OnAutoPlay(WPARAM wParam, LPARAM lParam)
{
    bool bAutoPlay = (wParam == 1);
    if (bAutoPlay == m_bAutoPlay)
        return 0;

    m_bAutoPlay = bAutoPlay;

    if (m_bAutoPlay)
		InsertSystemString("使用托管功能, 程序进入自动代打状态!");
    else
		InsertSystemString("取消托管功能. ");

	//发送消息
	CMD_C_UserTrustee UserTrustee;
	ZeroMemory( &UserTrustee, sizeof( UserTrustee ) );
	UserTrustee.bTrustee = m_bAutoPlay;
	UserTrustee.wUserChairID = GetMeChairID();

	SendData( SUB_C_TRUSTEE, &UserTrustee, sizeof( UserTrustee ) );

    return 0;
}


LRESULT CGameClientDlg::OnMessageSortCard(WPARAM wParam, LPARAM lParam)
{

	//排列扑克
	WORD wMeChairID=GetMeChairID();
	m_GameLogic.SortCardList(m_bHandCardData[wMeChairID],m_bHandCardCount[wMeChairID],m_cbSortType);

	//获取扑克
	BYTE cbShootCount=(BYTE)m_GameClientView.m_HandCardControl[1].SetCardData(m_bHandCardData[wMeChairID],m_bHandCardCount[wMeChairID]);

	//设置变量
	m_cbSortType=(m_cbSortType==ST_ORDER)?ST_COUNT:ST_ORDER;

	//设置按钮
	UINT uImageID=(m_cbSortType==ST_ORDER)?IDB_ORDER_SORT:IDB_COUNT_SORT;
	m_GameClientView.m_btSortCard.SetButtonImage(uImageID,AfxGetInstanceHandle(),false);

	//更新界面
	m_GameClientView.UpdateGameView(NULL);

	return 0;
}

//明牌开始
LRESULT CGameClientDlg::OnBrightStart(WPARAM wParam, LPARAM lParam)
{
	//界面设置
	m_GameClientView.ShowLandTime(true);

	//发送消息
	SendData( SUB_C_BRIGHT_START);

	//开始消息
	SendMessage(IDM_START, 0, 0 );

	//播放声音
	PlayGameSound(AfxGetInstanceHandle(),TEXT("BRIGHT_CARD"));

	return 0;
}

bool CGameClientDlg::OnSubBrightStart(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_BrightStart));
	if (wDataSize != sizeof(CMD_S_BrightStart)) return false;

	//变量定义
	CMD_S_BrightStart *pBrightStart = ( CMD_S_BrightStart * )pBuffer;

	//设置界面
	WORD wViewID = SwitchViewChairID(pBrightStart->wBrightUser) ;
	m_GameClientView.SetBrightStart(wViewID, pBrightStart->bBright);
	if ( pBrightStart->bBright ) m_GameClientView.SetBrightTime(BRIGHT_START_TIME);

	return true;
}

//玩家明牌
bool CGameClientDlg::OnSubBrightCard(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_BrightCard));
	if (wDataSize != sizeof(CMD_S_BrightCard)) return false;

	//变量定义
	CMD_S_BrightCard *pBrightCard = ( CMD_S_BrightCard * )pBuffer;

	if ( pBrightCard->cbCallScorePhase == CSD_BRIGHTCARD )
	{
		//椅子转换
		WORD wViewChairID = SwitchViewChairID(pBrightCard->wBrightUser);

		//清除界面
		m_GameClientView.m_bScore[wViewChairID] = 0 ;
		m_GameClientView.m_bPass[wViewChairID] = false;
		m_GameClientView.m_bDoubleScore[wViewChairID] = false;

		if ( pBrightCard->cbBrightTime != 255 )
		{
			//设置界面
			m_GameClientView.SetBrightTime(pBrightCard->cbCurrenBrightTime);
			m_GameClientView.SetBrightStart(wViewChairID, true);

			//设置扑克
			m_GameClientView.m_HandCardControl[wViewChairID].ShowWindow(SW_SHOW);
			m_GameClientView.m_HandCardControl[wViewChairID].SetDisplayFlag(true);
		}
		else
		{
			m_GameClientView.m_cbUserPhase[wViewChairID] = CSD_BRIGHTCARD;	
			m_GameClientView.m_bScore[wViewChairID] = 255 ;
		}

		m_GameClientView.UpdateGameView(NULL);

		return true;
	}
	else
	{
		//设置界面
		m_GameClientView.SetBrightTime(pBrightCard->cbCurrenBrightTime);
		WORD wViewChairID = SwitchViewChairID(pBrightCard->wBrightUser);
		m_GameClientView.SetBrightStart(wViewChairID, true);

		//设置扑克
		m_GameClientView.m_HandCardControl[wViewChairID].ShowWindow(SW_SHOW);
		m_GameClientView.m_HandCardControl[wViewChairID].SetDisplayFlag(true);

		return true;
	}
	return true;
}

//玩家托管
bool CGameClientDlg::OnSubPlayerTrustee(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_C_UserTrustee));
	if (wDataSize != sizeof(CMD_C_UserTrustee)) return false;

	CMD_C_UserTrustee *pUserTrustee = ( CMD_C_UserTrustee * )pBuffer;

	m_GameClientView.SetUserTrustee( SwitchViewChairID( pUserTrustee->wUserChairID ), pUserTrustee->bTrustee );

	//显示托管
	if ( pUserTrustee->wUserChairID != GetMeChairID() || IsLookonMode() ) {

		CString strMsg;
		if ( pUserTrustee->bTrustee )
			strMsg.Format( TEXT( "[ %s ] 玩家选择了托管" ), GetUserData( pUserTrustee->wUserChairID )->szName );
		else
			strMsg.Format( TEXT( "[ %s ] 玩家取消了托管" ), GetUserData( pUserTrustee->wUserChairID )->szName );
		InsertSystemString( strMsg );
	}

	return true;
}

//加倍消息
bool CGameClientDlg::OnSubDoubleScore(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_DoubleScore));
	if (wDataSize != sizeof(CMD_S_DoubleScore)) return false;

	//变量定义
	CMD_S_DoubleScore *pDoubleScore = ( CMD_S_DoubleScore * )pBuffer;

	//设置按钮
	bool bAllowDouble = pDoubleScore->bAllowDouble;
	m_GameClientView.m_btDoubleScore.ShowWindow( SW_SHOW );
	m_GameClientView.m_btDoubleScore.EnableWindow( bAllowDouble ? TRUE : FALSE );
	m_GameClientView.m_btNotDoubleScore.ShowWindow( SW_SHOW );

	//设置控件
	m_GameClientView.ShowLandTitle(false);
	m_GameClientView.m_BackCardControl.SetCardData(pDoubleScore->bBackCard,CountArray(pDoubleScore->bBackCard));
	m_GameClientView.m_BackCardControl.SetDisplayFlag( true );
	m_GameClientView.SetBrightStart(INVALID_CHAIR, false);

	//设置界面
	m_bCardCount[pDoubleScore->wLandUser]=20;
	m_GameClientView.SetLandScore(INVALID_CHAIR,0,CSD_GAMESTART,CSD_GAMESTART);
	m_GameClientView.SetCardCount(SwitchViewChairID(pDoubleScore->wLandUser),m_bCardCount[pDoubleScore->wLandUser]);
	WORD wViewChairID=SwitchViewChairID(pDoubleScore->wLandUser);
	m_GameClientView.SetLandUser(wViewChairID, pDoubleScore->bCurrentScore);

	//地主设置
	WORD wLandUser=pDoubleScore->wLandUser;
	BYTE bCardCound=m_bHandCardCount[wLandUser];
	m_bHandCardCount[wLandUser]+=CountArray(pDoubleScore->bBackCard);
	CopyMemory(&m_bHandCardData[wLandUser][bCardCound],pDoubleScore->bBackCard,sizeof(pDoubleScore->bBackCard));
	m_GameLogic.SortCardList(m_bHandCardData[wLandUser],m_bHandCardCount[wLandUser],ST_ORDER);
	m_GameClientView.m_HandCardControl[SwitchViewChairID(wLandUser)].SetCardData(m_bHandCardData[wLandUser],m_bHandCardCount[wLandUser]);

	//设置消息
	SetGameTimer(GetMeChairID(), IDI_DOUBLE_SCORE, 10);

	return true;
}

//加倍消息
LRESULT CGameClientDlg::OnDoubleScore(WPARAM wParam, LPARAM lParam)
{
	//加倍判断
	bool bDoubleScore = wParam == 0 ? true : false;

	//设置界面
	m_GameClientView.m_btDoubleScore.ShowWindow( SW_HIDE );
	m_GameClientView.m_btNotDoubleScore.ShowWindow( SW_HIDE );
    
	//定义消息
	CMD_C_DoubleScore DoubleScore;
	ZeroMemory( &DoubleScore, sizeof( DoubleScore ) );
	DoubleScore.bDoubleScore = bDoubleScore;

	//发送消息
	SendData( SUB_C_DOUBLE_SCORE, &DoubleScore, sizeof( DoubleScore ) );

	//关闭定时器
	KillGameTimer(IDI_DOUBLE_SCORE);

	return 0;
}

//玩家加倍
bool CGameClientDlg::OnSubUserDouble(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_UserDouble));
	if (wDataSize != sizeof(CMD_S_UserDouble)) return false;

	//变量定义
	CMD_S_UserDouble *pUserDouble = ( CMD_S_UserDouble * )pBuffer;

	//设置界面
	WORD wViewChairID=SwitchViewChairID(m_wLandUser);
	m_GameClientView.SetLandUser(wViewChairID, pUserDouble->bCurrentScore);
	m_GameClientView.SetDoubleScoreUser( SwitchViewChairID(pUserDouble->wDoubleUser), pUserDouble->bDoubleScore );
	m_GameClientView.SetBrightStart(wViewChairID, false);

	return true;
}