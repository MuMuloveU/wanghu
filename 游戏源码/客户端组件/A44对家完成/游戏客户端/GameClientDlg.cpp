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
#define IDI_CHA_CARD					205								//叉牌定时器
#define IDI_DIAN_CARD					206								//点牌定时器

//////////////////////////////////////添加的定时器/////////////////////////////////////////////
#define IDI_LI_GUN                      207
#define IDI_JUE_GUN                     208
#define IDI_LI_GUN_AGREE                209                             //同意立棍否
//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientDlg, CGameFrameDlg)
	ON_WM_TIMER()
	ON_MESSAGE(IDM_START,OnStart)
	ON_MESSAGE(IDM_OUT_CARD,OnOutCard)
	ON_MESSAGE(IDM_PASS_CARD,OnPassCard)
	ON_MESSAGE(IDM_AUTO_OUTCARD,OnAutoOutCard)
	ON_MESSAGE(IDM_LEFT_HIT_CARD,OnLeftHitCard)
	ON_MESSAGE(IDM_RIGHT_HIT_CARD,OnRightHitCard)
	ON_MESSAGE(IDM_LAND_AUTOPLAY,OnAutoPlay)
	ON_MESSAGE(IDM_SORT_HAND_CARD,OnMessageSortCard)
	/////////////////////////////////////////////////
	ON_MESSAGE(IDM_SORT_HAND_CHA,OnCha)
	ON_MESSAGE(IDM_SORT_HAND_NOCHA,OnNoCha)
	ON_MESSAGE(IDM_SORT_HAND_DIAN,OnDian)
	ON_MESSAGE(IDM_SORT_HAND_NODIAN,OnNoDian)

	ON_MESSAGE(IDM_LI_GUN, OnHitLiGun)
	ON_MESSAGE(IDM_JUE_GUN, OnHitJueGun)
	ON_MESSAGE(IDM_AGREE, OnHitAgree)
	ON_MESSAGE(IDM_NOT_LI_GUN, OnHitNotLiGun)
	ON_MESSAGE(IDM_NOT_JUE_GUN, OnHitNotJueGun)
	/////////////////////////////////////////////////
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientDlg::CGameClientDlg() : CGameFrameDlg(&m_GameClientView)
{
	//游戏变量
	m_wBombTime=1;
	m_bHandCardCount=0;
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

////////////////////////////////////////////////////////
	m_cbLiOrJue = 0;
////////////////////////////////////////////////////////

	return;
}

//析构函数
CGameClientDlg::~CGameClientDlg()
{
}

//初始函数
bool CGameClientDlg::InitGameFrame()
{
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
	m_GameClientView.m_HandCardControl[ME_VIEW_CHAIR].SetCardSpace(m_dwCardHSpace,0,20);

	return true;
}

//重置框架
void CGameClientDlg::ResetGameFrame()
{
	//游戏变量
	m_wBombTime=1;
	m_bHandCardCount=0;
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
		m_GameClientView.m_HandCardControl[ME_VIEW_CHAIR].SetCardSpace(m_dwCardHSpace,0,20);
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
		////////////////////////////////////////////添加的定时器处理////////////////////////////
	case IDI_LI_GUN:
		{
			//超时的时候，全部当作不立棍处理
			if (nElapse == 0)
			{
				SendData(SUB_C_LI_GUN_SUPERTIME);
			}
			return true;
		}
	//撅棍，超时的时候，当作不撅棍处理
	case IDI_JUE_GUN:
		{
			//超时的时候，全部当作不立棍处理
			if (nElapse == 0)
			{
				SendData(SUB_C_LI_GUN_SUPERTIME);
			}
			return true;
		}
	//超时的时候，作为不同意处理
	case IDI_LI_GUN_AGREE:
		{
			if (nElapse == 0)
			{
				OnHitAgree(0,0);
			}
			return true;
		}
///////////////////////////////////////////////////////////////////////////////////////////////////
	case IDI_OUT_CARD:			//用户出牌
		{
			//自动处理
			if ((((nElapse==0)||((m_bAutoPlay==true)&&(nElapse<28))))&&(wChairID==GetMeChairID()))
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
			if (m_bHandCardCount<m_bTurnCardCount) return true;
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
	case IDI_CHA_CARD:			//叉牌
		{
			if (nElapse==0)
				if ( m_GameClientView.m_btTwoScore.IsWindowVisible() ) 
					OnNoCha( 0, 0 );

			return true;
		}
	case IDI_DIAN_CARD:			//点牌
		{
			if (nElapse==0)
				if ( m_GameClientView.m_btGiveUpScore.IsWindowVisible() ) 
					OnNoDian( 0, 0 );

			return true;
		}
	}

	return false;
}

//旁观状态
void CGameClientDlg::OnLookonChanged(bool bLookonUser, const void * pBuffer, WORD wDataSize)
{
	//旁观设置
	m_GameClientView.m_HandCardControl[ME_VIEW_CHAIR].SetDisplayFlag( ( IsLookonMode() == true ) && ( IsAllowLookon() == true ) );

	return;
}

//网络消息
bool CGameClientDlg::OnGameMessage(WORD wSubCmdID, const void * pBuffer, WORD wDataSize)
{
	switch (wSubCmdID)
	{
/////////////////////////////////////////////////////添加的消息处理//////////////////////////////////////////
	case SUB_S_LI_GUN:
		{
			//AfxMessageBox("y");
			CMD_S_CanJueGun *pCanLiGun;
			pCanLiGun = ( CMD_S_CanJueGun * )pBuffer;
			//KillGameTimer()

			//m_GameClientView.m_btLiGun.ShowWindow(SW_HIDE);
			//m_GameClientView.m_btNotLiGun.ShowWindow(SW_HIDE);
			m_GameClientView.m_btJueGun.ShowWindow(SW_HIDE);
			m_GameClientView.m_btNotJueGun.ShowWindow(SW_HIDE);
			m_GameClientView.m_btAgree.ShowWindow(SW_HIDE);
			m_GameClientView.m_btDisagree.ShowWindow(SW_HIDE);
			//CString s;
			//s.Format("%d, %d", pCanLiGun->wCanJueChair[0], pCanLiGun->wCanJueChair[1]);
			//AfxMessageBox(s);

			

			if ( GetMeChairID() == pCanLiGun->wCanJueChair[0] || GetMeChairID() == pCanLiGun->wCanJueChair[1] )
			{
				m_GameClientView.m_btLiGun.ShowWindow(SW_SHOW);
				m_GameClientView.m_btNotLiGun.ShowWindow(SW_SHOW);
				m_GameClientView.m_btLiGun.EnableWindow(TRUE);
				m_GameClientView.m_btNotLiGun.EnableWindow(TRUE);
			}

			SetGameTimer(GetMeChairID(), IDI_LI_GUN, 10);

			return true;
		}
	case SUB_S_JUE_GUN_REQUEST:
		{
			//AfxMessageBox("a");
			CMD_S_LiGunRequest *pJueGunRequest;
			pJueGunRequest = (CMD_S_LiGunRequest *)pBuffer;


			m_GameClientView.m_btLiGun.ShowWindow(SW_HIDE);
			m_GameClientView.m_btNotLiGun.ShowWindow(SW_HIDE);
			m_GameClientView.m_btJueGun.ShowWindow(SW_HIDE);
			m_GameClientView.m_btNotJueGun.ShowWindow(SW_HIDE);

			if ( GetMeChairID() == pJueGunRequest->wRequest)
			{
				m_GameClientView.m_btAgree.ShowWindow(SW_SHOW);
				m_GameClientView.m_btDisagree.ShowWindow(SW_SHOW);
				m_GameClientView.m_btAgree.EnableWindow(TRUE);
				m_GameClientView.m_btDisagree.EnableWindow(TRUE);
			}

			m_cbLiOrJue = 2;

			SetGameTimer(GetMeChairID(), IDI_LI_GUN_AGREE, 5);

			return true;
		}
	case SUB_S_JUE_GUN:
		{
			CMD_S_CanJueGun *pCanJueGun;
			pCanJueGun = ( CMD_S_CanJueGun *)pBuffer;

			if (GetMeChairID()==pCanJueGun->wCanJueChair[0] || GetMeChairID()==pCanJueGun->wCanJueChair[1])
			{
				m_GameClientView.m_btJueGun.ShowWindow(SW_SHOW);
				m_GameClientView.m_btJueGun.EnableWindow(TRUE);
				m_GameClientView.m_btNotJueGun.ShowWindow(SW_SHOW);
				m_GameClientView.m_btNotJueGun.EnableWindow(TRUE);
			}
			m_cbLiOrJue = 2;

			SetGameTimer(GetMeChairID(), IDI_JUE_GUN, 10);
			return true;
		}
	case SUB_S_LI_GUN_REQUEST:
		{
			//AfxMessageBox("b");
			CMD_S_LiGunRequest *pLiGunRequest;
			pLiGunRequest = (CMD_S_LiGunRequest *)pBuffer;

			m_GameClientView.m_btLiGun.ShowWindow(SW_HIDE);
			m_GameClientView.m_btNotLiGun.ShowWindow(SW_HIDE);

			if ( GetMeChairID() == pLiGunRequest->wRequest)
			{
				m_GameClientView.m_btAgree.ShowWindow(SW_SHOW);
				m_GameClientView.m_btDisagree.ShowWindow(SW_SHOW);
				m_GameClientView.m_btAgree.EnableWindow(TRUE);
				m_GameClientView.m_btDisagree.EnableWindow(TRUE);
			}

			m_cbLiOrJue = 1;

			SetGameTimer(GetMeChairID(), IDI_LI_GUN_AGREE, 5);

			return true;
		}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	case SUB_S_SEND_CARD:		//发送扑克
		{
			return OnSubSendCard(pBuffer,wDataSize);
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
	case SUB_C_TRUSTEE:			//玩家托管
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
	case SUB_S_GAME_CHA:		//用户差牌
		{
			return OnSubChaCard(pBuffer,wDataSize);
		}
	case SUB_S_GAME_NOCHA:		//放弃叉牌
		{
			return OnSubPassCard(pBuffer,wDataSize);
		}
	case SUB_S_GAME_DIAN:		//用户点牌
		{
			return OnSubDianCard(pBuffer,wDataSize);
		}
	case SUB_S_GAME_NODIAN:		//放弃点牌
		{
			return OnSubPassCard(pBuffer,wDataSize);
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

			//设置控件
			if (IsLookonMode()==false)
			{
				m_GameClientView.m_btStart.ShowWindow(TRUE);
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
			m_GameClientView.m_HandCardControl[ME_VIEW_CHAIR].SetDisplayFlag((IsLookonMode()==false)||(IsAllowLookon()==true));

			//设置时间
			SetGameTimer(GetMeChairID(),IDI_START_GAME,30);

			return true;
		}
	case GS_WK_PLAYING:		//游戏状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pBuffer;

			//设置变量
			m_bTurnCardCount=pStatusPlay->bTurnCardCount;
			m_bHandCardCount=pStatusPlay->bCardCount[GetMeChairID()];
			m_bTurnOutType=m_GameLogic.GetCardType(pStatusPlay->bTurnCardData,m_bTurnCardCount);//出牌类型
			CopyMemory(m_bHandCardData,pStatusPlay->bCardData[GetMeChairID()],m_bHandCardCount);
			CopyMemory(m_bTurnCardData,pStatusPlay->bTurnCardData,pStatusPlay->bTurnCardCount);

			//设置界面
			for (BYTE i=0;i<GAME_PLAYER;i++)
			{
				WORD wViewChairID=SwitchViewChairID(i);
				m_bCardCount[i]=pStatusPlay->bCardCount[i];
				m_GameClientView.SetCardCount(wViewChairID,pStatusPlay->bCardCount[i]);
			}
			m_GameClientView.m_HandCardControl[ME_VIEW_CHAIR].SetCardData(m_bHandCardData,m_bHandCardCount);
			
			//CString strScore;
			//strScore.Format( TEXT( " 够:%d \n---点:%ld\n---看牌:%ld\n---现在出牌的人:%ld\n,自己的id:%d\n" ),pStatusPlay->gou,pStatusPlay->dian,IsLookonMode(),
			//	pStatusPlay->wCurrentUser,GetMeChairID());
			//InsertShtickString( strScore, RGB( 255, 0, 0 ), true );


			//玩家设置
			if((pStatusPlay->gou)&&(IsLookonMode()==false)&&(pStatusPlay->wCurrentUser==GetMeChairID()))
			{
				m_GameClientView.m_btOneScore.ShowWindow(SW_SHOW);
				m_GameClientView.m_btOneScore.EnableWindow(FALSE);
				m_GameClientView.m_btTwoScore.ShowWindow(SW_SHOW);
				m_GameClientView.m_btTwoScore.EnableWindow(FALSE);
				SetGameTimer(pStatusPlay->wCurrentUser,IDI_CHA_CARD,10);
				//InsertShtickString( TEXT( " 11111111111111" ), RGB( 255, 0, 0 ), true );
			}
			else if((pStatusPlay->dian)&&(IsLookonMode()==false)&&(pStatusPlay->wCurrentUser==GetMeChairID()))
			{
				m_GameClientView.m_btThreeScore.ShowWindow(SW_SHOW);
				m_GameClientView.m_btThreeScore.EnableWindow(FALSE);
				m_GameClientView.m_btGiveUpScore.ShowWindow(SW_SHOW);
				m_GameClientView.m_btGiveUpScore.EnableWindow(FALSE);
				SetGameTimer(pStatusPlay->wCurrentUser,IDI_DIAN_CARD,10);
				////InsertShtickString( TEXT( " 222222222222222" ), RGB( 255, 0, 0 ), true );
			}
			else if ((IsLookonMode()==false)&&(pStatusPlay->wCurrentUser==GetMeChairID()))
			{
				m_GameClientView.m_btOutCard.EnableWindow(FALSE);
				m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
				m_GameClientView.m_btPassCard.ShowWindow(SW_SHOW);
				m_GameClientView.m_btAutoOutCard.ShowWindow(SW_SHOW);
				m_GameClientView.m_btPassCard.EnableWindow((m_bTurnCardCount!=0)?TRUE:FALSE);
				m_GameClientView.m_btAutoOutCard.EnableWindow(TRUE);
				//InsertShtickString( TEXT( " 3333333333333333" ), RGB( 255, 0, 0 ), true );
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

			//设置扑克
			BYTE cbCardData[20];
			ZeroMemory(cbCardData,sizeof(cbCardData));
			for (BYTE i=0;i<GAME_PLAYER;i++)
			{
				WORD wViewChairID=SwitchViewChairID(i);
				if (wViewChairID!=ME_VIEW_CHAIR) m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(cbCardData,pStatusPlay->bCardCount[i]);
			}

			//设置定时器
			SetGameTimer(pStatusPlay->wCurrentUser,IDI_OUT_CARD,30);

			for ( WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID )
			{
				m_GameClientView.SetUserTrustee( SwitchViewChairID( wChairID ), pStatusPlay->bUserTrustee[ wChairID ] );
				
				//自己判断
				if ( ! IsLookonMode() && wChairID == GetMeChairID() &&pStatusPlay->bUserTrustee[ wChairID ] )
				{
					//托管功能
					m_GameClientView.m_btAutoPlayOn.ShowWindow(SW_HIDE);
					m_GameClientView.m_btAutoPlayOff.ShowWindow(SW_SHOW);
					OnAutoPlay( true, 0 );
				}
			}

			//设置扑克
			m_GameClientView.m_HandCardControl[ME_VIEW_CHAIR].SetPositively(IsLookonMode()==false);
			m_GameClientView.m_HandCardControl[ME_VIEW_CHAIR].SetDisplayFlag((IsLookonMode()==false)||(IsAllowLookon()==true));

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

	//设置数据
	m_wCurrentUser=pSendCard->wCurrentUser;//当前用户

	CopyMemory(m_bHandCardData,pSendCard->bCardData[GetMeChairID()],sizeof(pSendCard->bCardData[GetMeChairID()]));//手上扑克
	
	int numtemp =0;
	for (WORD i=0;i<15;i++)
	{
		if(pSendCard->bCardData[GetMeChairID()][i] != 0) 	numtemp++;
		else break;
	}
	m_bHandCardCount=numtemp;//NORMAL_COUNT;//手中的扑克数目	
	
	for (WORD i=0;i<GAME_PLAYER;i++) 
		m_bCardCount[i]=pSendCard->cbCardNum[i];

	//设置界面
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameClientView.SetLandScore(i,0);
		m_GameClientView.SetPassFlag(i,false);
		m_GameClientView.m_UserCardControl[i].SetCardData(NULL,0);
		m_GameClientView.m_HandCardControl[i].SetCardData(NULL,0);
	}

	m_GameClientView.m_HandCardControl[ME_VIEW_CHAIR].SetDisplayFlag(((IsLookonMode()==false)||(IsAllowLookon()==true)));

	//设置扑克
	for ( BYTE i = 0; i < GAME_PLAYER; ++i )
		if (i!=ME_VIEW_CHAIR)
			m_GameClientView.m_HandCardControl[i].SetCardSpace(8,8,0);

	//扑克
	
	DispatchUserCard(m_bHandCardData,m_bHandCardCount);
	m_GameLogic.SortCardList(m_bHandCardData,m_bHandCardCount,ST_ORDER);//排列扑克

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
	m_bTurnCardCount=0;//出牌数目
	m_bTurnOutType=CT_ERROR;//出牌类型//错误类型
	ZeroMemory(m_bTurnCardData,sizeof(m_bTurnCardData));//出牌列表

	//关闭成绩
	if (m_GameClientView.m_ScoreView.m_hWnd!=NULL)
	{
		m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE) ;
		m_GameClientView.m_ScoreView.DestroyWindow();
	}

	//设置控件
	m_GameClientView.ShowLandTitle(false);//显示提示

	//关掉按钮
	m_GameClientView.m_btLiGun.ShowWindow(SW_HIDE);
	m_GameClientView.m_btNotLiGun.ShowWindow(SW_HIDE);
	m_GameClientView.m_btJueGun.ShowWindow(SW_HIDE);
	m_GameClientView.m_btNotJueGun.ShowWindow(SW_HIDE);
	m_GameClientView.m_btAgree.ShowWindow(SW_HIDE);
	m_GameClientView.m_btDisagree.ShowWindow(SW_HIDE);

	//设置界面
	m_GameClientView.SetLandScore(INVALID_CHAIR,0);//挖坑分数

	m_GameClientView.m_HandCardControl[ME_VIEW_CHAIR].SetPositively( IsLookonMode()==false || IsAllowLookon() );

	//当前玩家
	if ((IsLookonMode()==false)&&(pGameStart->wCurrentUser==GetMeChairID()))
	{
		ActiveGameFrame();
		m_GameClientView.m_btOutCard.EnableWindow(FALSE);
		m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btPassCard.EnableWindow(FALSE);
		m_GameClientView.m_btPassCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btAutoOutCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btAutoOutCard.EnableWindow(TRUE);

	}

	//设置扑克
	for ( BYTE i = 0; i < GAME_PLAYER; ++i )
		if (i!=ME_VIEW_CHAIR)
			m_GameClientView.m_HandCardControl[i].SetCardSpace(8,8,0);//设置间距

	//播放声音
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));

	//设置时间
	SetGameTimer(pGameStart->wCurrentUser,IDI_OUT_CARD,30);

	return true;
}

//用户出牌
bool CGameClientDlg::OnSubOutCard(const void * pBuffer, WORD wDataSize)
{
	//变量定义
	CMD_S_OutCard * pOutCard=(CMD_S_OutCard *)pBuffer;
	WORD wHeadSize=sizeof(CMD_S_OutCard)-sizeof(pOutCard->bCardData);

	m_GameClientView.m_btOneScore.ShowWindow(SW_HIDE);
	m_GameClientView.m_btOneScore.EnableWindow(FALSE);
	m_GameClientView.m_btTwoScore.ShowWindow(SW_HIDE);
	m_GameClientView.m_btTwoScore.EnableWindow(FALSE);
	m_GameClientView.m_btThreeScore.ShowWindow(SW_HIDE);
	m_GameClientView.m_btThreeScore.EnableWindow(FALSE);
	m_GameClientView.m_btGiveUpScore.ShowWindow(SW_HIDE);
	m_GameClientView.m_btGiveUpScore.EnableWindow(FALSE);
	//效验数据
	ASSERT(wDataSize>=wHeadSize);
	if (wDataSize<wHeadSize) return false;
	ASSERT(wDataSize==(wHeadSize+pOutCard->bCardCount*sizeof(pOutCard->bCardData[0])));
	if (wDataSize!=(wHeadSize+pOutCard->bCardCount*sizeof(pOutCard->bCardData[0]))) return false;

	//删除定时器
	KillTimer(IDI_MOST_CARD);
	KillGameTimer(IDI_OUT_CARD);
	WORD wOutViewChairID=SwitchViewChairID(pOutCard->wOutCardUser);

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

	//记录出牌
	m_bTurnCardCount=pOutCard->bCardCount;
	m_bTurnOutType=m_GameLogic.GetCardType(pOutCard->bCardData,pOutCard->bCardCount);//出牌类型
	CopyMemory(m_bTurnCardData,pOutCard->bCardData,sizeof(BYTE)*pOutCard->bCardCount);

	//炸弹判断
	if (m_bTurnOutType>=4)
	{
		//播放动画
		m_GameClientView.SetBombEffect(true);
	}
	
	//自己扑克
	if ((IsLookonMode()==true)&&(pOutCard->wOutCardUser==GetMeChairID()))
	{
		//删除扑克 
		BYTE bSourceCount=m_bHandCardCount;
		m_bHandCardCount-=pOutCard->bCardCount;
		m_GameLogic.RemoveCard(pOutCard->bCardData,pOutCard->bCardCount,m_bHandCardData,bSourceCount);

		//设置界面
		m_GameClientView.m_HandCardControl[ME_VIEW_CHAIR].SetCardData(m_bHandCardData,m_bHandCardCount);
	}

	//界面设置	
	m_bCardCount[pOutCard->wOutCardUser]-=pOutCard->bCardCount;
	m_GameClientView.SetCardCount(wOutViewChairID,m_bCardCount[pOutCard->wOutCardUser]);
	for (WORD wChairID=0; wChairID<GAME_PLAYER; ++wChairID)
	{
		WORD wViewID = SwitchViewChairID(wChairID) ;
		if(ME_VIEW_CHAIR!=wViewID)
		{
			BYTE cbCardData[20] ;
			ZeroMemory(cbCardData, sizeof(cbCardData)) ;
			m_GameClientView.m_HandCardControl[wViewID].SetCardData(cbCardData,m_bCardCount[wChairID]);
		}
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
		PlayGameSound(AfxGetInstanceHandle(),TEXT("MOST_CARD"));

		//设置定时器
		SetTimer(IDI_MOST_CARD,3000,NULL);

		return true;
	}

	if ((IsLookonMode()==true)||(GetMeChairID()!=pOutCard->wOutCardUser))
	{
		//炸弹判断
		if (/*(m_bTurnOutType==CT_BOMB_CARD)||*/(m_bTurnOutType==CT_MISSILE_CARD))
			PlayGameSound(AfxGetInstanceHandle(),TEXT("BOMB_CARD"));
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

	/////////////////////////////////////////////////
	m_GameClientView.m_btOneScore.ShowWindow(SW_HIDE);
	m_GameClientView.m_btOneScore.EnableWindow(FALSE);
	m_GameClientView.m_btTwoScore.ShowWindow(SW_HIDE);
	m_GameClientView.m_btTwoScore.EnableWindow(FALSE);
	m_GameClientView.m_btThreeScore.ShowWindow(SW_HIDE);
	m_GameClientView.m_btThreeScore.EnableWindow(FALSE);
	m_GameClientView.m_btGiveUpScore.ShowWindow(SW_HIDE);
	m_GameClientView.m_btGiveUpScore.EnableWindow(FALSE);
	/////////////////////////////////////////////////
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

	////////////////////添加的禁用按钮控件/////////////////
	m_GameClientView.m_btLiGun.ShowWindow(SW_HIDE);
	m_GameClientView.m_btNotLiGun.ShowWindow(SW_HIDE);
	m_GameClientView.m_btJueGun.ShowWindow(SW_HIDE);
	m_GameClientView.m_btNotJueGun.ShowWindow(SW_HIDE);
	m_GameClientView.m_btAgree.ShowWindow(SW_HIDE);
	m_GameClientView.m_btDisagree.ShowWindow(SW_HIDE);

	//禁用控件
	m_GameClientView.m_btOutCard.EnableWindow(FALSE);
	m_GameClientView.m_btPassCard.EnableWindow(FALSE);

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
		cbCardPos += pGameEnd->bCardCount[i] ;

		if (i!=ME_VIEW_CHAIR)
			m_GameClientView.m_HandCardControl[i].SetCardSpace(16,16,0);
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
		SetGameTimer(GetMeChairID(),IDI_START_GAME,30);

		//托管功能
		 m_GameClientView.m_btAutoPlayOn.ShowWindow(SW_SHOW);
		m_GameClientView.m_btAutoPlayOff.ShowWindow(SW_HIDE);
		OnAutoPlay( false, 0 );
	}
	m_GameClientView.ShowLandTitle(false);

#ifdef VIDEO_GAME
	//显示扑克
	//m_GameClientView.m_HandCardControl[ 0 ].ShowWindow( SW_SHOW );
	//m_GameClientView.m_HandCardControl[ 1 ].ShowWindow( SW_SHOW );
	//m_GameClientView.m_HandCardControl[ 3 ].ShowWindow( SW_SHOW );
#endif

	return true;
}

//出牌判断
bool CGameClientDlg::VerdictOutCard()
{
	//状态判断
	if (m_GameClientView.m_btOutCard.IsWindowVisible()==FALSE) return false;

	//获取扑克
	BYTE bCardData[MAX_COUNT];
	BYTE bShootCount=(BYTE)m_GameClientView.m_HandCardControl[ME_VIEW_CHAIR].GetShootCard(bCardData,CountArray(bCardData));

	//出牌判断
	if (bShootCount>0L)
	{
		//分析类型
		BYTE bCardType=m_GameLogic.GetCardType(bCardData,bShootCount);//出牌类型

		//类型判断
		if (bCardType==CT_ERROR) return false;

		//跟牌判断
		if (m_bTurnCardCount==0) return true;//出牌数目
		//出牌列表//自己扑克数据//出牌数目//自己的张数
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

		//发送数据
		CMD_C_OutCard OutCard;
		OutCard.bCardCount=1;
		OutCard.bCardData[0]=m_bHandCardData[m_bHandCardCount-1];
		SendData(SUB_C_OUT_CART,&OutCard,sizeof(OutCard)-sizeof(OutCard.bCardData)+OutCard.bCardCount*sizeof(BYTE));

		//预先处理
		PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
		m_GameClientView.m_UserCardControl[ME_VIEW_CHAIR].SetCardData(OutCard.bCardData,OutCard.bCardCount);

		//预先删除
		BYTE bSourceCount=m_bHandCardCount;
		m_bHandCardCount-=OutCard.bCardCount;
		m_GameLogic.RemoveCard(OutCard.bCardData,OutCard.bCardCount,m_bHandCardData,bSourceCount);
		m_GameClientView.m_HandCardControl[ME_VIEW_CHAIR].SetCardData(m_bHandCardData,m_bHandCardCount);
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
			if (wViewChairID!=ME_VIEW_CHAIR)
			{
				BYTE cbCardData[20] ;
				ZeroMemory(cbCardData, sizeof(cbCardData)) ;
				m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(cbCardData,NORMAL_COUNT);
			}
			else m_GameClientView.m_HandCardControl[ME_VIEW_CHAIR].SetCardData(m_bHandCardData,m_bHandCardCount); 
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
	m_cbDispatchCardCount=0;//派发数目
	m_cbRemnantCardCount=cbCardCount;//剩余数目
	CopyMemory(m_cbDispatchCardData,cbCardData,sizeof(BYTE)*cbCardCount);//派发扑克

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
			BYTE cbUserCard[NORMAL_COUNT];
			ZeroMemory(cbUserCard,sizeof(cbUserCard));

			//派发扑克
			m_cbRemnantCardCount--;
			m_cbDispatchCardCount++;
			m_GameLogic.SortCardList(m_cbDispatchCardData,m_cbDispatchCardCount,ST_ORDER);

			//设置界面
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				WORD wViewID = SwitchViewChairID(i) ;
				if (wViewID!=ME_VIEW_CHAIR)
				{
					m_GameClientView.m_HandCardControl[wViewID].SetCardData(cbUserCard,m_cbDispatchCardCount);
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

			}
			//设置界面
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				WORD wViewID = SwitchViewChairID(i) ;
				if (wViewID!=ME_VIEW_CHAIR)
				{
					BYTE cbCardData[20] ;
					ZeroMemory(cbCardData, sizeof(cbCardData)) ;
					m_GameClientView.m_HandCardControl[wViewID].SetCardData(cbCardData,NORMAL_COUNT);
					m_GameClientView.m_HandCardControl[wViewID].SetDisplayFlag(false) ;
				}
				else 
				{
					m_GameClientView.m_HandCardControl[wViewID].SetCardData(m_bHandCardData,m_bHandCardCount); 
					if(false==IsLookonMode())
					{
						m_GameClientView.m_HandCardControl[wViewID].SetDisplayFlag(true) ;
						m_GameClientView.m_HandCardControl[wViewID].SetPositively(true) ;
					}
				}
			}

			if(IsLookonMode()==false) m_GameClientView.m_btSortCard.EnableWindow(TRUE);

			//播放声音
			PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));

			////////////////////////////////////////////////添加的代码//////////////////////////////////////////
			//点亮立棍按钮
			m_GameClientView.m_btLiGun.ShowWindow(SW_SHOW);
			m_GameClientView.m_btLiGun.EnableWindow(TRUE);
			m_GameClientView.m_btNotLiGun.ShowWindow(SW_SHOW);
			m_GameClientView.m_btNotLiGun.EnableWindow(TRUE);
			SetGameTimer(GetMeChairID(), IDI_LI_GUN, 10);
			////////////////////////////////////////////////////////////////////////////////////////////////////

			//设置时间
			//if (m_wCurrentUser!=INVALID_CHAIR) SetGameTimer(m_wCurrentUser,IDI_LAND_SCORE,30);
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
	m_wTimeOutCount=0;
	m_bHandCardCount=0;
	m_bTurnCardCount=0;
	m_bTurnOutType=CT_ERROR;
	m_wMostUser=INVALID_CHAIR;
	memset(m_bHandCardData,0,sizeof(m_bHandCardData));
	memset(m_bTurnCardData,0,sizeof(m_bTurnCardData));

	//设置界面
	KillGameTimer(IDI_START_GAME);
	m_GameClientView.ShowLandTitle(false);
	m_GameClientView.SetBombTime(m_wBombTime);
	m_GameClientView.SetCardCount(INVALID_CHAIR,0);
	m_GameClientView.SetLandUser(INVALID_CHAIR,0);
	m_GameClientView.SetLandScore(INVALID_CHAIR,0);
	m_GameClientView.SetPassFlag(INVALID_CHAIR,false);

	//隐藏控件
	m_GameClientView.m_btStart.ShowWindow(FALSE);
	if (m_GameClientView.m_ScoreView.m_hWnd!=NULL) m_GameClientView.m_ScoreView.DestroyWindow();

	//设置扑克
	//m_GameClientView.m_BackCardControl.SetCardData(NULL,0);
	m_GameClientView.m_HandCardControl[ME_VIEW_CHAIR].SetPositively(false);
	for (WORD i=0;i<GAME_PLAYER;i++) 
	{
		m_GameClientView.m_UserCardControl[i].SetCardData(NULL,0);
		m_GameClientView.m_HandCardControl[i].SetCardData(NULL,0);
		WORD wViewID = SwitchViewChairID(i) ;
		if(wViewID==ME_VIEW_CHAIR)	m_GameClientView.m_HandCardControl[wViewID].SetDisplayFlag(true);
		else m_GameClientView.m_HandCardControl[wViewID].SetDisplayFlag(false);
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
	CMD_C_OutCard OutCard;
	OutCard.bCardCount=(BYTE)m_GameClientView.m_HandCardControl[ME_VIEW_CHAIR].GetShootCard(OutCard.bCardData,CountArray(OutCard.bCardData));
	m_GameLogic.SortCardList( OutCard.bCardData, OutCard.bCardCount, ST_ORDER);
	SendData(SUB_C_OUT_CART,&OutCard,sizeof(OutCard)-sizeof(OutCard.bCardData)+OutCard.bCardCount*sizeof(BYTE));

	//预先显示
	PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
	m_GameClientView.m_UserCardControl[ME_VIEW_CHAIR].SetCardData(OutCard.bCardData,OutCard.bCardCount);

	//预先删除
	BYTE bSourceCount=m_bHandCardCount;
	m_bHandCardCount-=OutCard.bCardCount;
	m_GameLogic.RemoveCard(OutCard.bCardData,OutCard.bCardCount,m_bHandCardData,bSourceCount);
	m_GameClientView.m_HandCardControl[ME_VIEW_CHAIR].SetCardData(m_bHandCardData,m_bHandCardCount);

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
	m_GameClientView.SetPassFlag(ME_VIEW_CHAIR,true);
	PlayGameSound(AfxGetInstanceHandle(),TEXT("PASS_CARD"));
	m_GameClientView.m_HandCardControl[ME_VIEW_CHAIR].SetCardData(m_bHandCardData,m_bHandCardCount);

	return 0;
}

//出牌提示
LRESULT CGameClientDlg::OnAutoOutCard(WPARAM wParam, LPARAM lParam)
{
	//分析结果
	tagOutCardResult OutCardResult;
	m_GameLogic.SearchOutCard(m_bHandCardData,m_bHandCardCount,m_bTurnCardData,m_bTurnCardCount,OutCardResult);

	//设置界面
	if (OutCardResult.cbCardCount>0)
	{
		//设置界面
		m_GameClientView.m_HandCardControl[ME_VIEW_CHAIR].SetShootCard(OutCardResult.cbResultCard,OutCardResult.cbCardCount);

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

	bool bChaCard=VerdictChaCard();
	m_GameClientView.m_btOneScore.EnableWindow(bChaCard?TRUE:FALSE);

	bool bDianCard=VerdictDianCard();
	m_GameClientView.m_btThreeScore.EnableWindow(bDianCard?TRUE:FALSE);

	//CString ss;
	//ss.Format( TEXT( " %d" ), bChaCard);
	//InsertShtickString( ss, RGB( 255, 0, 0 ), true );
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
	m_GameLogic.SortCardList(m_bHandCardData,m_bHandCardCount,m_cbSortType);

	//获取扑克
	BYTE cbShootCount=(BYTE)m_GameClientView.m_HandCardControl[ME_VIEW_CHAIR].SetCardData(m_bHandCardData,m_bHandCardCount);

	//设置变量
	m_cbSortType=(m_cbSortType==ST_ORDER)?ST_COUNT:ST_ORDER;

	//设置按钮
	UINT uImageID=(m_cbSortType==ST_ORDER)?IDB_ORDER_SORT:IDB_COUNT_SORT;
	m_GameClientView.m_btSortCard.SetButtonImage(uImageID,AfxGetInstanceHandle(),false);

	//更新界面
	m_GameClientView.UpdateGameView(NULL);

	return 0;
}
//用户叉牌
bool CGameClientDlg::OnSubChaCard(const void * pBuffer, WORD wDataSize)
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
	WORD wOutViewChairID=SwitchViewChairID(pOutCard->wOutCardUser);

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

	//记录出牌
	m_bTurnCardCount=pOutCard->bCardCount;
	m_bTurnOutType=m_GameLogic.GetCardType(pOutCard->bCardData,pOutCard->bCardCount);//出牌类型
	CopyMemory(m_bTurnCardData,pOutCard->bCardData,sizeof(BYTE)*pOutCard->bCardCount);

	//自己扑克
	if ((IsLookonMode()==true)&&(pOutCard->wOutCardUser==GetMeChairID()))
	{
		//删除扑克 
		BYTE bSourceCount=m_bHandCardCount;
		m_bHandCardCount-=pOutCard->bCardCount;
		m_GameLogic.RemoveCard(pOutCard->bCardData,pOutCard->bCardCount,m_bHandCardData,bSourceCount);

		//设置界面
		m_GameClientView.m_HandCardControl[ME_VIEW_CHAIR].SetCardData(m_bHandCardData,m_bHandCardCount);
	}

	//界面设置	
	m_bCardCount[pOutCard->wOutCardUser]-=pOutCard->bCardCount;//扑克数目
	m_GameClientView.SetCardCount(wOutViewChairID,m_bCardCount[pOutCard->wOutCardUser]);
	for (WORD wChairID=0; wChairID<GAME_PLAYER; ++wChairID)
	{
		WORD wViewID = SwitchViewChairID(wChairID) ;
		if(ME_VIEW_CHAIR!=wViewID)
		{
			BYTE cbCardData[20] ;
			ZeroMemory(cbCardData, sizeof(cbCardData)) ;
			m_GameClientView.m_HandCardControl[wViewID].SetCardData(cbCardData,m_bCardCount[wChairID]);
		}
	}

	if (pOutCard->wCurrentUser!=INVALID_CHAIR)//当前玩家
	{
		WORD wViewChairID=SwitchViewChairID(pOutCard->wCurrentUser);
		m_GameClientView.SetPassFlag(wViewChairID,false);
		m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);
	}

	//玩家设置
	if ((IsLookonMode()==false)&&(pOutCard->wCurrentUser==GetMeChairID()))
	{
		ActiveGameFrame();
		//够牌显示和不够
		m_GameClientView.m_btOneScore.ShowWindow(SW_SHOW);
		m_GameClientView.m_btOneScore.EnableWindow((VerdictChaCard()==true)?TRUE:FALSE);
		m_GameClientView.m_btTwoScore.ShowWindow(SW_SHOW);
		m_GameClientView.m_btTwoScore.EnableWindow(TRUE);
	}

	//设置时间
	if (pOutCard->wCurrentUser!=INVALID_CHAIR)
	{
		BYTE bCardCount=m_bCardCount[pOutCard->wCurrentUser];
		SetGameTimer(pOutCard->wCurrentUser,IDI_CHA_CARD,10);
	}

	
	return true;
}
//叉牌消息
LRESULT CGameClientDlg::OnCha(WPARAM wParam, LPARAM lParam)
{
	//状态判断
	if ((m_GameClientView.m_btOneScore.IsWindowEnabled()==FALSE)||
		(m_GameClientView.m_btOneScore.IsWindowVisible()==FALSE)) return 0;

	//重置变量
	m_wTimeOutCount=0;

	//设置界面
	KillGameTimer(IDI_CHA_CARD);
	m_GameClientView.m_btOneScore.ShowWindow(SW_HIDE);
	m_GameClientView.m_btOneScore.EnableWindow(FALSE);
	m_GameClientView.m_btTwoScore.ShowWindow(SW_HIDE);
	m_GameClientView.m_btTwoScore.EnableWindow(FALSE);

	//发送数据
	CMD_C_OutCard OutCard;
	OutCard.bCardCount=(BYTE)m_GameClientView.m_HandCardControl[ME_VIEW_CHAIR].GetShootCard(OutCard.bCardData,CountArray(OutCard.bCardData));
	m_GameLogic.SortCardList( OutCard.bCardData, OutCard.bCardCount, ST_ORDER);
	SendData(SUB_C_GAME_CHA,&OutCard,sizeof(OutCard)-sizeof(OutCard.bCardData)+OutCard.bCardCount*sizeof(BYTE));

	//预先显示
	PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
	m_GameClientView.m_UserCardControl[ME_VIEW_CHAIR].SetCardData(OutCard.bCardData,OutCard.bCardCount);
	
	//预先删除
	BYTE bSourceCount=m_bHandCardCount;
	m_bHandCardCount-=OutCard.bCardCount;
	m_GameLogic.RemoveCard(OutCard.bCardData,OutCard.bCardCount,m_bHandCardData,bSourceCount);
	m_GameClientView.m_HandCardControl[ME_VIEW_CHAIR].SetCardData(m_bHandCardData,m_bHandCardCount);

	return 0;
}

//不叉
LRESULT CGameClientDlg::OnNoCha(WPARAM wParam, LPARAM lParam)
{
	//状态判断
	if (m_GameClientView.m_btTwoScore.IsWindowEnabled()==FALSE) return 0;

	//重置变量
	m_wTimeOutCount=0;

	//设置界面
	KillGameTimer(IDI_CHA_CARD);
	KillGameTimer(IDI_OUT_CARD);
	m_GameClientView.m_btOneScore.ShowWindow(SW_HIDE);
	m_GameClientView.m_btOneScore.EnableWindow(FALSE);
	m_GameClientView.m_btTwoScore.ShowWindow(SW_HIDE);
	m_GameClientView.m_btTwoScore.EnableWindow(FALSE);

	//发送数据
	SendData(SUB_C_GAME_NOCHA);

	//预先显示
	//m_GameClientView.SetPassFlag(ME_VIEW_CHAIR,true);
	PlayGameSound(AfxGetInstanceHandle(),TEXT("PASS_CARD"));
	m_GameClientView.m_HandCardControl[ME_VIEW_CHAIR].SetCardData(m_bHandCardData,m_bHandCardCount);

	return 0;
}
//点牌消息
LRESULT CGameClientDlg::OnDian(WPARAM wParam, LPARAM lParam)
{
	//状态判断
	if ((m_GameClientView.m_btThreeScore.IsWindowEnabled()==FALSE)||
		(m_GameClientView.m_btThreeScore.IsWindowVisible()==FALSE)) return 0;

	//重置变量
	m_wTimeOutCount=0;

	//设置界面
	KillGameTimer(IDI_DIAN_CARD);
	m_GameClientView.m_btThreeScore.ShowWindow(SW_HIDE);
	m_GameClientView.m_btThreeScore.EnableWindow(FALSE);
	m_GameClientView.m_btGiveUpScore.ShowWindow(SW_HIDE);
	m_GameClientView.m_btGiveUpScore.EnableWindow(FALSE);

	//发送数据
	CMD_C_OutCard OutCard;
	OutCard.bCardCount=(BYTE)m_GameClientView.m_HandCardControl[ME_VIEW_CHAIR].GetShootCard(OutCard.bCardData,CountArray(OutCard.bCardData));
	m_GameLogic.SortCardList( OutCard.bCardData, OutCard.bCardCount, ST_ORDER);
	SendData(SUB_C_GAME_DIAN,&OutCard,sizeof(OutCard)-sizeof(OutCard.bCardData)+OutCard.bCardCount*sizeof(BYTE));

	//预先显示
	PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
	m_GameClientView.m_UserCardControl[ME_VIEW_CHAIR].SetCardData(OutCard.bCardData,OutCard.bCardCount);

	//预先删除
	BYTE bSourceCount=m_bHandCardCount;
	m_bHandCardCount-=OutCard.bCardCount;
	m_GameLogic.RemoveCard(OutCard.bCardData,OutCard.bCardCount,m_bHandCardData,bSourceCount);
	m_GameClientView.m_HandCardControl[ME_VIEW_CHAIR].SetCardData(m_bHandCardData,m_bHandCardCount);

	return 0;
}

//不点
LRESULT CGameClientDlg::OnNoDian(WPARAM wParam, LPARAM lParam)
{
	//状态判断
	if (m_GameClientView.m_btGiveUpScore.IsWindowEnabled()==FALSE) return 0;

	//重置变量
	m_wTimeOutCount=0;

	//设置界面
	KillGameTimer(IDI_DIAN_CARD);
	KillGameTimer(IDI_OUT_CARD);
	m_GameClientView.m_btThreeScore.ShowWindow(SW_HIDE);
	m_GameClientView.m_btThreeScore.EnableWindow(FALSE);
	m_GameClientView.m_btGiveUpScore.ShowWindow(SW_HIDE);
	m_GameClientView.m_btGiveUpScore.EnableWindow(FALSE);


	//发送数据
	SendData(SUB_C_GAME_NODIAN);

	//预先显示
	//m_GameClientView.SetPassFlag(ME_VIEW_CHAIR,true);
	PlayGameSound(AfxGetInstanceHandle(),TEXT("PASS_CARD"));
	m_GameClientView.m_HandCardControl[ME_VIEW_CHAIR].SetCardData(m_bHandCardData,m_bHandCardCount);

	return 0;
}
//用户点牌
bool CGameClientDlg::OnSubDianCard(const void * pBuffer, WORD wDataSize)
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
	WORD wOutViewChairID=SwitchViewChairID(pOutCard->wOutCardUser);

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

	//记录出牌
	m_bTurnCardCount=pOutCard->bCardCount;
	m_bTurnOutType=m_GameLogic.GetCardType(pOutCard->bCardData,pOutCard->bCardCount);//出牌类型
	CopyMemory(m_bTurnCardData,pOutCard->bCardData,sizeof(BYTE)*pOutCard->bCardCount);

	//自己扑克
	if ((IsLookonMode()==true)&&(pOutCard->wOutCardUser==GetMeChairID()))
	{
		//删除扑克 
		BYTE bSourceCount=m_bHandCardCount;
		m_bHandCardCount-=pOutCard->bCardCount;
		m_GameLogic.RemoveCard(pOutCard->bCardData,pOutCard->bCardCount,m_bHandCardData,bSourceCount);

		//设置界面
		m_GameClientView.m_HandCardControl[ME_VIEW_CHAIR].SetCardData(m_bHandCardData,m_bHandCardCount);
	}

	//界面设置	
	m_bCardCount[pOutCard->wOutCardUser]-=pOutCard->bCardCount;//扑克数目
	m_GameClientView.SetCardCount(wOutViewChairID,m_bCardCount[pOutCard->wOutCardUser]);
	for (WORD wChairID=0; wChairID<GAME_PLAYER; ++wChairID)
	{
		WORD wViewID = SwitchViewChairID(wChairID) ;
		if(ME_VIEW_CHAIR!=wViewID)
		{
			BYTE cbCardData[20] ;
			ZeroMemory(cbCardData, sizeof(cbCardData)) ;
			m_GameClientView.m_HandCardControl[wViewID].SetCardData(cbCardData,m_bCardCount[wChairID]);
		}
	}

	if (pOutCard->wCurrentUser!=INVALID_CHAIR)//当前玩家
	{
		WORD wViewChairID=SwitchViewChairID(pOutCard->wCurrentUser);
		m_GameClientView.SetPassFlag(wViewChairID,false);
		m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);
	}

	//玩家设置
	if ((IsLookonMode()==false)&&(pOutCard->wCurrentUser==GetMeChairID()))
	{
		ActiveGameFrame();
		//够牌显示和不够
		m_GameClientView.m_btThreeScore.ShowWindow(SW_SHOW);
		m_GameClientView.m_btThreeScore.EnableWindow((VerdictChaCard()==true)?TRUE:FALSE);
		m_GameClientView.m_btGiveUpScore.ShowWindow(SW_SHOW);
		m_GameClientView.m_btGiveUpScore.EnableWindow(TRUE);
	}

	//设置时间
	if (pOutCard->wCurrentUser!=INVALID_CHAIR)
	{
		BYTE bCardCount=m_bCardCount[pOutCard->wCurrentUser];
		SetGameTimer(pOutCard->wCurrentUser,IDI_DIAN_CARD,10);
	}

	return true;
}
//叉牌判断
bool CGameClientDlg::VerdictChaCard()
{
	//状态判断
	if (m_GameClientView.m_btOneScore.IsWindowVisible()==FALSE) return false;

	//获取扑克
	BYTE bCardData[MAX_COUNT];
	BYTE bShootCount=(BYTE)m_GameClientView.m_HandCardControl[ME_VIEW_CHAIR].GetShootCard(bCardData,CountArray(bCardData));
	//出牌判断
	if (bShootCount>0L)
	{
		//分析类型
		BYTE bCardType=m_GameLogic.GetCardType(bCardData,bShootCount);//出牌类型

		//类型判断
		if (bCardType==CT_ERROR) return false;

		//跟牌判断
		if (m_bTurnCardCount==0) return true;//出牌数目

		//出牌列表//自己扑克数据//出牌数目//自己的张数
		return m_GameLogic.CompareChaCard(m_bTurnCardData,bCardData,m_bTurnCardCount,bShootCount);
	}
	return false;
}
//点牌判断
bool CGameClientDlg::VerdictDianCard()
{
	//状态判断
	if (m_GameClientView.m_btThreeScore.IsWindowVisible()==FALSE) return false;

	//获取扑克
	BYTE bCardData[MAX_COUNT];
	BYTE bShootCount=(BYTE)m_GameClientView.m_HandCardControl[ME_VIEW_CHAIR].GetShootCard(bCardData,CountArray(bCardData));
	//出牌判断
	if (bShootCount>0L)
	{
		//分析类型
		BYTE bCardType=m_GameLogic.GetCardType(bCardData,bShootCount);//出牌类型

		//类型判断
		if (bCardType==CT_ERROR) return false;

		//跟牌判断
		if (m_bTurnCardCount==0) return true;//出牌数目

		//出牌列表//自己扑克数据//出牌数目//自己的张数
		return m_GameLogic.CompareDianCard(m_bTurnCardData,bCardData,m_bTurnCardCount,bShootCount);
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////
LRESULT CGameClientDlg::OnHitLiGun(WPARAM wParam, LPARAM lParam)
{
	m_GameClientView.m_btLiGun.ShowWindow(SW_HIDE);

	CMD_C_LiGun LiGun;
	ZeroMemory(&LiGun, sizeof(LiGun));
	LiGun.wLiGunUserChairID = GetMeChairID();
	SendData(SUB_C_LI_GUN, &LiGun, sizeof(LiGun));

	return 0;
}

LRESULT CGameClientDlg::OnHitJueGun(WPARAM wParam, LPARAM lParam)
{
	m_GameClientView.m_btJueGun.ShowWindow(SW_HIDE);
	m_GameClientView.m_btNotJueGun.ShowWindow(SW_HIDE);

	CMD_C_LiGun JueGun;
	ZeroMemory(&JueGun, sizeof(JueGun));
	JueGun.wLiGunUserChairID = GetMeChairID();

	SendData(SUB_C_JUE_GUN, &JueGun, sizeof(JueGun));

	return 0;
}


LRESULT CGameClientDlg::OnHitAgree(WPARAM wParam, LPARAM lParam)
{
	m_GameClientView.m_btAgree.ShowWindow(SW_HIDE);
	m_GameClientView.m_btDisagree.ShowWindow(SW_HIDE);
	BYTE cbAgree = (BYTE)wParam;
	CMD_C_LiGunAnswer LiGunAnswer;
	ZeroMemory(&LiGunAnswer, sizeof(LiGunAnswer));
	LiGunAnswer.cbAnswer = (BYTE)wParam;
	LiGunAnswer.wAgreeChairID = GetMeChairID();
	LiGunAnswer.cbLiOrJue = m_cbLiOrJue;

	SendData(SUB_C_LI_GUN_ANSWER, &LiGunAnswer, sizeof(LiGunAnswer));

	//同意立棍或者撅棍，那么当前玩家不参与游戏，所有的牌预显示为背面
	if (cbAgree == 1)
	{
		memset(m_cbLiOrJueCardData, 0, sizeof(m_cbLiOrJueCardData));
		m_GameClientView.m_HandCardControl[2].SetCardData(m_cbLiOrJueCardData, m_bHandCardCount);
	}

	return 0;
}

LRESULT CGameClientDlg::OnHitNotLiGun(WPARAM wParam, LPARAM lParam)
{
	m_GameClientView.m_btLiGun.ShowWindow(SW_HIDE);
	m_GameClientView.m_btJueGun.ShowWindow(SW_HIDE);
	m_GameClientView.m_btNotLiGun.ShowWindow(SW_HIDE);
	m_GameClientView.m_btNotJueGun.ShowWindow(SW_HIDE);
	m_GameClientView.m_btAgree.ShowWindow(SW_HIDE);
	m_GameClientView.m_btDisagree.ShowWindow(SW_HIDE);

	CMD_C_NotLiOrJue NotLiOrJue;
	ZeroMemory(&NotLiOrJue, sizeof(NotLiOrJue));
	NotLiOrJue.wNotLiOrJueChair = GetMeChairID();

	SendData(SUB_C_NOT_LI_GUN, &NotLiOrJue, sizeof(NotLiOrJue));

	return 0;
}

LRESULT CGameClientDlg::OnHitNotJueGun(WPARAM wParam, LPARAM lParam)
{
	m_GameClientView.m_btLiGun.ShowWindow(SW_HIDE);
	m_GameClientView.m_btJueGun.ShowWindow(SW_HIDE);
	m_GameClientView.m_btNotLiGun.ShowWindow(SW_HIDE);
	m_GameClientView.m_btNotJueGun.ShowWindow(SW_HIDE);
	m_GameClientView.m_btAgree.ShowWindow(SW_HIDE);
	m_GameClientView.m_btDisagree.ShowWindow(SW_HIDE);

	CMD_C_NotLiOrJue NotLiOrJue;
	ZeroMemory(&NotLiOrJue, sizeof(NotLiOrJue));
	NotLiOrJue.wNotLiOrJueChair = GetMeChairID();

	SendData(SUB_C_NOT_JUE_GUN, &NotLiOrJue, sizeof(NotLiOrJue));

	return 0;
}