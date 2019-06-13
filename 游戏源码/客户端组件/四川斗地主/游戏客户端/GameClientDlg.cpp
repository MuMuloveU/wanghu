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
#define IDI_LOOK_CATCH					205								//叫庄定时器

#ifdef _DEBUG
#define TIME_OUT_CARD					99								//出牌定时器
#define TIME_START_GAME					99								//开始定时器
#define TIME_LAND_SCORE					99								//叫分定时器
#define TIME_LOOK_CATCH					99								//叫庄定时器
#else
#define TIME_OUT_CARD					30								//出牌定时器
#define TIME_START_GAME					30								//开始定时器
#define TIME_LAND_SCORE					10								//叫分定时器
#define TIME_LOOK_CATCH					10								//叫庄定时器
#endif

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

	ON_MESSAGE(IDM_LOOK_OR_CATCH,OnLookOrCatch)
	ON_MESSAGE(IDM_CALL_BANKER,OnCallBanker)
	ON_MESSAGE(IDM_ADD_SCORE,OnAddScore)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientDlg::CGameClientDlg() : CGameFrameDlg(&m_GameClientView)
{
	//游戏变量
	m_wBombTime=1;
	m_bHandCardCount=0;
	m_wLandUser=INVALID_CHAIR;
	memset(m_bBackCard,0,sizeof(m_bBackCard));
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
	//SetWindowText(TEXT("四川斗地主游戏  --  Ver：6.5.1.0"));

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

#ifdef _DEBUG
	//HINSTANCE hInstance=AfxGetInstanceHandle();

	//m_GameClientView.m_btAddScore.SetButtonImage(IDB_BT_BANKER_ADD1,hInstance,false);	
	//m_GameClientView.m_btNoAddScore.SetButtonImage(IDB_BT_BANKER_NO_ADD1,hInstance,false);

	//m_GameClientView.m_btAddScore.SetButtonImage(IDB_BT_BANKER_ADD2,hInstance,false);	
	//m_GameClientView.m_btNoAddScore.SetButtonImage(IDB_BT_BANKER_NO_ADD2,hInstance,false);

	//m_GameClientView.m_btAddScore.SetButtonImage(IDB_BT_ADD2,hInstance,false);	
	//m_GameClientView.m_btNoAddScore.SetButtonImage(IDB_BT_NO_ADD2,hInstance,false);

	//BYTE cbCardData[20];
	//ZeroMemory(cbCardData,sizeof(cbCardData));
	//		m_GameClientView.m_HandCardControl[0].SetCardData(cbCardData,20);
	//		m_GameClientView.m_HandCardControl[1].SetCardData(cbCardData,20);
	//		m_GameClientView.m_HandCardControl[2].SetCardData(cbCardData,20);
	//m_GameClientView. m_btAddScore.ShowWindow(5);
	//m_GameClientView. m_btNoAddScore.ShowWindow(5);

	//if (m_GameClientView.m_ScoreView.m_hWnd==NULL)
	//{
	//	m_GameClientView.m_ScoreView.Create(IDD_GAME_SCORE,&m_GameClientView);
	//}
	//m_GameClientView.m_ScoreView.ShowWindow(5);
	//m_GameClientView.m_ScoreView.SetGameTax(1,1);
#endif

	return true;
}

//重置框架
void CGameClientDlg::ResetGameFrame()
{
	//游戏变量
	m_wBombTime=1;
	m_bHandCardCount=0;
	m_wLandUser=INVALID_CHAIR;
	memset(m_bBackCard,0,sizeof(m_bBackCard));
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
			if ((((nElapse==0)||((m_bAutoPlay==true)&&(nElapse<TIME_OUT_CARD))))&&(wChairID==GetMeChairID()))
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
	case IDI_LAND_SCORE:		//挖坑叫分
		{
			if ( nElapse==0 || ( nElapse <= TIME_OUT_CARD && m_bAutoPlay ) )
			{
				if ((IsLookonMode()==false)&&(wChairID==GetMeChairID())) 
				{
					if(m_GameClientView.m_btCatch.IsWindowVisible()==TRUE)OnCallBanker(0,0);
					else OnAddScore(0,0);
				}
				return false;
			}
			if ((nElapse<=10)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false)) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));

			return true;
		}
	case IDI_LOOK_CATCH:		//用户看抓
		{
			if ( nElapse==0 || ( nElapse <= TIME_OUT_CARD && m_bAutoPlay ) )
			{
				if ((IsLookonMode()==false)&&(wChairID==GetMeChairID())) OnLookOrCatch(0,0);
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
	case SUB_S_CHOICE_LOOK:		//用户叫庄
		{
			return OnSubCallBanker(pBuffer,wDataSize);
		}
	case SUB_S_SEND_CARD:		//发送扑克
		{
			return OnSubSendCard(pBuffer,wDataSize);
		}
	case SUB_S_LAND_SCORE:		//用户叫分
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
			m_GameClientView.m_HandCardControl[1].SetDisplayFlag((IsLookonMode()==false)||(IsAllowLookon()==true));

			//设置时间
			SetGameTimer(GetMeChairID(),IDI_START_GAME,TIME_START_GAME);

			return true;
		}
	case GS_WK_SCORE:	//叫分状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusScore)) return false;
			CMD_S_StatusScore * pStatusScore=(CMD_S_StatusScore *)pBuffer;

			//设置界面
			m_GameClientView.SetBaseScore(pStatusScore->lBaseScore);
			m_GameClientView.SetSingleRandCard(SwitchViewChairID(pStatusScore->wFirstUser),pStatusScore->bMingCard);

			//设置地主
			if(pStatusScore->wBankerUser<GAME_PLAYER)
			{
				m_wLandUser=pStatusScore->wBankerUser;
				m_GameClientView.SetLandUser(SwitchViewChairID(m_wLandUser));

				//叫分信息
				for(WORD i=0;i<GAME_PLAYER;i++)
				{
					if(pStatusScore->bCallScoreTimes[i]>0)
						m_GameClientView.SetUserScroeTag(SwitchViewChairID(i),pStatusScore->bCallScoreTimes[i]);
				}
			}

			//底牌控件
			CopyMemory(m_bBackCard,pStatusScore->bBackCard,sizeof(m_bBackCard));
			if(m_bBackCard[0]!=0 && m_wLandUser<GAME_PLAYER)
			{
				m_GameClientView.m_BackCardControl.SetDisplayFlag(true);
				m_GameClientView.m_BackCardControl.SetCardData(m_bBackCard,CountArray(m_bBackCard));
			}
			else
			{
				BYTE bBackCard[3]={0,0,0};
				m_GameClientView.m_BackCardControl.SetCardData(bBackCard,CountArray(bBackCard));
			}

			//扑克控件
			BYTE bCurrentState = pStatusScore->bCurrentState;
			WORD wMeChairID = GetMeChairID();
			m_bHandCardCount=17;
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//扑克数目
				if(i!=m_wLandUser || pStatusScore->bBackCatchTag==TRUE)m_bCardCount[i] = 17;
				else m_bCardCount[i] = 20;
				WORD wViewChairID = SwitchViewChairID(i);
				m_GameClientView.SetCardCount(wViewChairID,m_bCardCount[i]);

				//设置控件
				BYTE cbCardData[20] ;
				ZeroMemory(cbCardData, sizeof(cbCardData)) ;
				m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(cbCardData,m_bCardCount[i]);
			}

			//设置扑克
			if(pStatusScore->bCardData[0]!=0 && pStatusScore->bBackCatchTag==FALSE)
			{
				BYTE bTempCount = ((m_bBackCard[0]!=0 && wMeChairID==m_wLandUser)?20:17);
				m_bHandCardCount = bTempCount;
				CopyMemory(m_bHandCardData,pStatusScore->bCardData,sizeof(m_bHandCardData));
				m_GameClientView.m_HandCardControl[1].SetCardData(m_bHandCardData,bTempCount);
			}
			m_GameClientView.m_HandCardControl[1].SetDisplayFlag((IsLookonMode()==false)||(IsAllowLookon()==true));

			//等待信息
			BYTE bTemp = 0;
			switch((bCurrentState&0x0F))
			{
			case 0:				
				{
					m_GameClientView.SetUserWaitType(1);
					break;
				}
			case CALL_BANKER:
				{
					m_GameClientView.SetUserWaitType(2);
					break;
				}
			case CALL_SCORE:
				{
					if(m_wLandUser==pStatusScore->wCurrentUser)m_GameClientView.SetUserWaitType(4);
					else m_GameClientView.SetUserWaitType(3);
					break;
				}
			case CALL_TWO_SCORE:
				{
					if(m_wLandUser==pStatusScore->wCurrentUser)m_GameClientView.SetUserWaitType(6);
					else m_GameClientView.SetUserWaitType(5);
					break;
				}
			}

			//设置控件
			if ((IsLookonMode()==false)&&(pStatusScore->wCurrentUser==wMeChairID))
			{
				ActiveGameFrame();

				//显示控件
				switch((bCurrentState&0x0F))
				{
				case 0:
					{
						m_GameClientView.m_btBackCatch.ShowWindow(SW_SHOW);
						m_GameClientView.m_btLookCard.ShowWindow(SW_SHOW);

						break;
					}
				case CALL_BANKER:
					{
						m_GameClientView.m_btCatch.ShowWindow(SW_SHOW);
						m_GameClientView.m_btNoCatch.ShowWindow(SW_SHOW);

						break;
					}
				case CALL_SCORE:
					{
						ASSERT(m_wLandUser<GAME_PLAYER);
						HINSTANCE hInstance=AfxGetInstanceHandle();
						if(m_wLandUser==wMeChairID)
						{
							m_GameClientView.m_btAddScore.SetButtonImage(IDB_BT_BANKER_ADD1,hInstance,false);	
							m_GameClientView.m_btNoAddScore.SetButtonImage(IDB_BT_BANKER_NO_ADD1,hInstance,false);
						}
						else
						{
							m_GameClientView.m_btAddScore.SetButtonImage(IDB_BT_ADD1,hInstance,false);	
							m_GameClientView.m_btNoAddScore.SetButtonImage(IDB_BT_NO_ADD1,hInstance,false);
						}
						m_GameClientView.m_btAddScore.ShowWindow(SW_SHOW);
						m_GameClientView.m_btNoAddScore.ShowWindow(SW_SHOW);

						break;
					}
				case CALL_TWO_SCORE:
					{
						ASSERT(m_wLandUser<GAME_PLAYER);
						HINSTANCE hInstance=AfxGetInstanceHandle();
						if(m_wLandUser==wMeChairID)
						{
							m_GameClientView.m_btAddScore.SetButtonImage(IDB_BT_BANKER_ADD2,hInstance,false);	
							m_GameClientView.m_btNoAddScore.SetButtonImage(IDB_BT_BANKER_NO_ADD2,hInstance,false);
						}
						else
						{
							m_GameClientView.m_btAddScore.SetButtonImage(IDB_BT_ADD2,hInstance,false);	
							m_GameClientView.m_btNoAddScore.SetButtonImage(IDB_BT_NO_ADD2,hInstance,false);
						}
						m_GameClientView.m_btAddScore.ShowWindow(SW_SHOW);
						m_GameClientView.m_btNoAddScore.ShowWindow(SW_SHOW);

						break;
					}
				}
			}

			//辅助按钮
			if (IsLookonMode())
			{
				m_GameClientView.m_btAutoPlayOff.ShowWindow( SW_HIDE ) ;
				m_GameClientView.m_btAutoPlayOn.ShowWindow( SW_HIDE ) ;
				m_GameClientView.m_btAutoPlayOff.EnableWindow( FALSE ) ;
				m_GameClientView.m_btAutoPlayOn.EnableWindow( FALSE ) ;
				m_GameClientView.m_btSortCard.ShowWindow( SW_HIDE );
			}
			else if(m_bHandCardData[0]!=0)
			{
				m_GameClientView.m_btSortCard.EnableWindow(TRUE);
			}

			//设置托管
			for ( WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID )
			{
				m_GameClientView.SetUserTrustee( SwitchViewChairID( wChairID ), pStatusScore->bUserTrustee[ wChairID ] );

				//自己判断
				if ( ! IsLookonMode() && wChairID == GetMeChairID() && pStatusScore->bUserTrustee[ wChairID ] )
				{
					//托管功能
					m_GameClientView.m_btAutoPlayOn.ShowWindow(SW_HIDE);
					m_GameClientView.m_btAutoPlayOff.ShowWindow(SW_SHOW);
					OnAutoPlay( true, 0 );
				}
			}

			//设置时间
			if((bCurrentState&0x0F)==0)
				SetGameTimer(pStatusScore->wCurrentUser,IDI_LOOK_CATCH,TIME_LOOK_CATCH);
			else 
				SetGameTimer(pStatusScore->wCurrentUser,IDI_LAND_SCORE,TIME_LAND_SCORE);

			return true;
		}
	case GS_WK_PLAYING:		//游戏状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pBuffer;

			//设置界面
			m_GameClientView.SetBaseScore(pStatusPlay->lBaseScore);
			m_GameClientView.SetSingleRandCard(SwitchViewChairID(pStatusPlay->wFirstUser),pStatusPlay->bMingCard);

			//设置地主
			m_wLandUser=pStatusPlay->wBankerUser;
			m_GameClientView.SetLandUser(SwitchViewChairID(m_wLandUser));

			//叫分信息
			for(WORD i=0;i<GAME_PLAYER;i++)
			{
				if(pStatusPlay->bCallScoreTimes[i]>0)
					m_GameClientView.SetUserScroeTag(SwitchViewChairID(i),pStatusPlay->bCallScoreTimes[i]);
			}

			//底牌控件
			CopyMemory(m_bBackCard,pStatusPlay->bBackCard,sizeof(m_bBackCard));
			if(m_bBackCard[0]!=0)
			{
				m_GameClientView.m_BackCardControl.SetDisplayFlag(true);
				m_GameClientView.m_BackCardControl.SetCardData(m_bBackCard,CountArray(m_bBackCard));
			}
			else
			{
				BYTE bBackCard[3]={0,0,0};
				m_GameClientView.m_BackCardControl.SetCardData(bBackCard,CountArray(bBackCard));
			}

			//设置变量
			m_bTurnCardCount=pStatusPlay->bTurnCardCount;
			m_bHandCardCount=pStatusPlay->bCardCount[GetMeChairID()];
			m_bTurnOutType=m_GameLogic.GetCardType(pStatusPlay->bTurnCardData,m_bTurnCardCount);
			CopyMemory(m_bHandCardData,pStatusPlay->bCardData,m_bHandCardCount);
			CopyMemory(m_bTurnCardData,pStatusPlay->bTurnCardData,pStatusPlay->bTurnCardCount);

			//设置界面
			for (BYTE i=0;i<GAME_PLAYER;i++)
			{
				WORD wViewChairID=SwitchViewChairID(i);
				m_bCardCount[i]=pStatusPlay->bCardCount[i];
				m_GameClientView.SetCardCount(wViewChairID,pStatusPlay->bCardCount[i]);
			}
			m_GameClientView.m_HandCardControl[1].SetCardData(m_bHandCardData,m_bHandCardCount);

			//设置倍数
			m_GameClientView.SetBombTime((pStatusPlay->wOutBombCount+1));
			WORD wUserMultiple[GAME_PLAYER];
			WORD wTempCount = (INT)pow(2,pStatusPlay->wOutBombCount);
			for(WORD i=0;i<GAME_PLAYER;i++)wUserMultiple[i]=(WORD)pStatusPlay->bScoreTimes[i]/wTempCount;
			m_GameClientView.SetUserScroeTimes(wUserMultiple);

			//设置扑克
			BYTE cbCardData[20];
			ZeroMemory(cbCardData,sizeof(cbCardData));
			for (BYTE i=0;i<GAME_PLAYER;i++)
			{
				WORD wViewChairID=SwitchViewChairID(i);
				if (wViewChairID!=1) m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(cbCardData,pStatusPlay->bCardCount[i]);
			}

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
					OnAutoPlay( true, 0 );
				}
			}

			//设置定时器
			SetGameTimer(pStatusPlay->wCurrentUser,IDI_OUT_CARD,TIME_OUT_CARD);

			//设置扑克
			m_GameClientView.m_HandCardControl[1].SetPositively(IsLookonMode()==false);
			m_GameClientView.m_HandCardControl[1].SetDisplayFlag((IsLookonMode()==false)||(IsAllowLookon()==true));

			return true;
		}
	}

	return false;
}

//用户看抓
bool CGameClientDlg::OnSubCallBanker(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_CallBanker));
	if (wDataSize!=sizeof(CMD_S_CallBanker)) return false;

	//关闭成绩
	if (m_GameClientView.m_ScoreView.m_hWnd!=NULL)
	{
		m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE) ;
		m_GameClientView.m_ScoreView.DestroyWindow();
	}

	//清理界面
	m_wBombTime = 1;
	m_GameClientView.SetBombTime(m_wBombTime);
	m_GameClientView.SetLandUser(INVALID_CHAIR);
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameClientView.SetLandAction(i,0);
		m_GameClientView.SetPassFlag(i,false);
		m_GameClientView.m_UserCardControl[i].SetCardData(NULL,0);
	}

	//变量定义
	CMD_S_CallBanker * pCallBanker=(CMD_S_CallBanker *)pBuffer;

	//控件判断
	WORD wMeChairID = GetMeChairID();
	if(wMeChairID==pCallBanker->wCurrentUser && !IsLookonMode())
	{
		//显示控件
		ActiveGameFrame();
		m_GameClientView.m_btBackCatch.ShowWindow(SW_SHOW);
		m_GameClientView.m_btLookCard.ShowWindow(SW_SHOW);
	}

	//设置界面
	m_bHandCardCount=17;
	m_GameClientView.SetUserWaitType(1);
	m_GameClientView.SetSingleRandCard(SwitchViewChairID(pCallBanker->wCurrentUser),pCallBanker->bMingCard);

	//扑克控件
	BYTE bBackCard[3]={0,0,0};
	m_GameClientView.m_BackCardControl.SetCardData(bBackCard,CountArray(bBackCard));
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_bCardCount[i] = 17;
		m_GameClientView.SetCardCount(i,m_bCardCount[i]);
		BYTE cbCardData[20] ;
		ZeroMemory(cbCardData, sizeof(cbCardData)) ;
		m_GameClientView.m_HandCardControl[i].SetCardData(cbCardData,17);
	}

	//设置时间
	SetGameTimer(pCallBanker->wCurrentUser,IDI_LOOK_CATCH,TIME_LOOK_CATCH);

	//播放声音
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));

	return true;
}

//发送扑克
bool CGameClientDlg::OnSubSendCard(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_SendAllCard));
	if (wDataSize!=sizeof(CMD_S_SendAllCard)) return false;

	//变量定义
	CMD_S_SendAllCard * pSendCard=(CMD_S_SendAllCard *)pBuffer;

	//设置数据
	CopyMemory(m_bBackCard,pSendCard->bBackCardData,sizeof(m_bBackCard));
	m_wCurrentUser=pSendCard->wCurrentUser;
	CopyMemory(m_bHandCardData,pSendCard->bCardData[GetMeChairID()],sizeof(pSendCard->bCardData[GetMeChairID()]));

	//设置扑克
	m_GameClientView.m_HandCardControl[1].SetDisplayFlag(((IsLookonMode()==false)||(IsAllowLookon()==true)));

#ifdef _DEBUG
	for ( WORD wChairID = 0; wChairID < GAME_PLAYER; ++wChairID )
	{
		m_GameClientView.m_HandCardControl[SwitchViewChairID(wChairID)].SetCardData(pSendCard->bCardData[wChairID], 17);
		m_GameClientView.m_HandCardControl[SwitchViewChairID(wChairID)].SetDisplayFlag(true);
	}
#endif

	//排放扑克
	DispatchUserCard(m_bHandCardData,m_bHandCardCount);
	m_GameLogic.SortCardList(m_bHandCardData,m_bHandCardCount,ST_ORDER);
	DoncludeDispatchCard();

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

	//设置地主
	if(pLandScore->wBankerUser!=INVALID_CHAIR)
	{
		m_wLandUser=pLandScore->wBankerUser;
		m_GameClientView.SetLandUser(SwitchViewChairID(m_wLandUser));
	}

	//设置界面
	WORD wViewChairID=SwitchViewChairID(pLandScore->bLandUser);
	m_GameClientView.SetLandAction(INVALID_CHAIR,0);
	m_GameClientView.SetLandAction(wViewChairID,pLandScore->bLandScore);

	//播放声音
	WORD wMeChairID = GetMeChairID();
	if (IsLookonMode()||(pLandScore->bLandUser!=wMeChairID))
	{
		switch(pLandScore->bLandScore)
		{
		case PASS_ACTION:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("PASS_CARD"));
			break;
		case BACK_CATCH:
		case MING_CATCH:
		case LOOK_CARD:
			PlayGameSound(AfxGetInstanceHandle(),TEXT("CALL_CARD"));
			break;
		case CALL_SCORE:
		case CALL_TWO_SCORE:
			if(pLandScore->bLandUser==m_wLandUser)
				PlayGameSound(AfxGetInstanceHandle(),TEXT("LAND_SCORE"));
			else PlayGameSound(AfxGetInstanceHandle(),TEXT("IDLE_SCORE"));
			break;
		}
	}

	//玩家设置
	if ((IsLookonMode()==false)&&(pLandScore->wCurrentUser==wMeChairID))
	{
		ActiveGameFrame();

		//显示控件
		switch((pLandScore->bCurrentState&0x0F))
		{
		case CALL_BANKER:
			{
				m_GameClientView.m_btCatch.ShowWindow(SW_SHOW);
				m_GameClientView.m_btNoCatch.ShowWindow(SW_SHOW);

				break;
			}
		case CALL_SCORE:
			{
				ASSERT(m_wLandUser<GAME_PLAYER);
				HINSTANCE hInstance=AfxGetInstanceHandle();
				if(m_wLandUser==wMeChairID)
				{
					m_GameClientView.m_btAddScore.SetButtonImage(IDB_BT_BANKER_ADD1,hInstance,false);	
					m_GameClientView.m_btNoAddScore.SetButtonImage(IDB_BT_BANKER_NO_ADD1,hInstance,false);
				}
				else
				{
					m_GameClientView.m_btAddScore.SetButtonImage(IDB_BT_ADD1,hInstance,false);	
					m_GameClientView.m_btNoAddScore.SetButtonImage(IDB_BT_NO_ADD1,hInstance,false);
				}
				m_GameClientView.m_btAddScore.ShowWindow(SW_SHOW);
				m_GameClientView.m_btNoAddScore.ShowWindow(SW_SHOW);

				break;
			}
		case CALL_TWO_SCORE:
			{
				ASSERT(m_wLandUser<GAME_PLAYER);
				HINSTANCE hInstance=AfxGetInstanceHandle();
				if(m_wLandUser==wMeChairID)
				{
					m_GameClientView.m_btAddScore.SetButtonImage(IDB_BT_BANKER_ADD2,hInstance,false);	
					m_GameClientView.m_btNoAddScore.SetButtonImage(IDB_BT_BANKER_NO_ADD2,hInstance,false);
				}
				else
				{
					m_GameClientView.m_btAddScore.SetButtonImage(IDB_BT_ADD2,hInstance,false);	
					m_GameClientView.m_btNoAddScore.SetButtonImage(IDB_BT_NO_ADD2,hInstance,false);
				}
				m_GameClientView.m_btAddScore.ShowWindow(SW_SHOW);
				m_GameClientView.m_btNoAddScore.ShowWindow(SW_SHOW);

				break;
			}
		}
	}

	//等待信息
	WORD wCurrentUser = pLandScore->wCurrentUser;
	switch((pLandScore->bCurrentState&0x0F))
	{
	case CALL_BANKER:
		{
			m_GameClientView.SetUserWaitType(2);
			break;
		}
	case CALL_SCORE:
		{
			if(m_wLandUser==wCurrentUser)m_GameClientView.SetUserWaitType(4);
			else m_GameClientView.SetUserWaitType(3);
			break;
		}
	case CALL_TWO_SCORE:
		{
			if(m_wLandUser==wCurrentUser)m_GameClientView.SetUserWaitType(6);
			else m_GameClientView.SetUserWaitType(5);
			break;
		}
	}

	//叫分信息
	m_GameClientView.SetUserScroeTag(SwitchViewChairID(pLandScore->bLandUser),pLandScore->bUserCallTimes);

	//插入底牌
	if(pLandScore->bCurrentState&0xF0)
	{
		//当前用户
		WORD wChairID = m_wLandUser;
		m_bCardCount[wChairID] = 20;

		//插入底牌
		ASSERT(m_bBackCard[0]!=0);
		m_GameClientView.m_BackCardControl.SetCardData(m_bBackCard,CountArray(m_bBackCard));
		m_GameClientView.m_BackCardControl.SetDisplayFlag(true);

		//设置界面
		m_GameClientView.SetLandAction(INVALID_CHAIR,0);
		m_GameClientView.SetCardCount(SwitchViewChairID(wChairID),m_bCardCount[wChairID]);

		//地主设置
		if (wChairID==GetMeChairID())
		{
			BYTE bCardCound=m_bHandCardCount;
			m_bHandCardCount+=CountArray(m_bBackCard);
			CopyMemory(&m_bHandCardData[bCardCound],m_bBackCard,sizeof(m_bBackCard));
			m_GameLogic.SortCardList(m_bHandCardData,m_bHandCardCount,ST_ORDER);
			m_GameClientView.m_HandCardControl[1].SetCardData(m_bHandCardData,m_bHandCardCount);
		}
		else
		{	
			BYTE cbCardData[20] ;
			ZeroMemory(cbCardData, sizeof(cbCardData)) ;
			m_GameClientView.m_HandCardControl[SwitchViewChairID(wChairID)].SetCardData(cbCardData,20);
		}
	}

	//设置时间
	if(pLandScore->wCurrentUser<GAME_PLAYER)
		SetGameTimer(pLandScore->wCurrentUser,IDI_LAND_SCORE,TIME_LAND_SCORE);

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
	m_bTurnCardCount=0;
	m_bTurnOutType=CT_ERROR;
	WORD wMeChiarID = GetMeChairID();
	ASSERT(m_wLandUser<GAME_PLAYER);
	ZeroMemory(m_bTurnCardData,sizeof(m_bTurnCardData));
	if(pGameStart->bBackCard[0]!=0)CopyMemory(m_bBackCard,pGameStart->bBackCard,sizeof(m_bBackCard));
	if(!IsLookonMode())m_GameClientView.m_btSortCard.EnableWindow(TRUE);

	//设置控件
	if(m_bCardCount[m_wLandUser]<20)
	{
		m_bCardCount[m_wLandUser]=20;
		m_GameClientView.m_BackCardControl.SetCardData(m_bBackCard,CountArray(m_bBackCard));
		m_GameClientView.m_BackCardControl.SetDisplayFlag( true );
		m_GameClientView.SetCardCount(SwitchViewChairID(m_wLandUser),m_bCardCount[m_wLandUser]);

		//地主设置
		if (m_wLandUser==GetMeChairID())
		{
			ASSERT(m_bBackCard[0]!=0);
			BYTE bCardCound=m_bHandCardCount;
			m_bHandCardCount=20;
			CopyMemory(&m_bHandCardData[bCardCound],m_bBackCard,sizeof(m_bBackCard));
			m_GameLogic.SortCardList(m_bHandCardData,m_bHandCardCount,ST_ORDER);
			m_GameClientView.m_HandCardControl[1].SetCardData(m_bHandCardData,m_bHandCardCount);
		}
		else 
		{
			BYTE cbCardData[20] ;
			ZeroMemory(cbCardData, sizeof(cbCardData)) ;
			m_GameClientView.m_HandCardControl[SwitchViewChairID(m_wLandUser)].SetCardData(cbCardData,20);
		}
	}

	//设置界面
	m_GameClientView.SetUserWaitType(0);
	WORD wUserMultiple[GAME_PLAYER];
	ZeroMemory(wUserMultiple,sizeof(wUserMultiple));
	for(WORD i=0;i<GAME_PLAYER;i++)
	{
		wUserMultiple[SwitchViewChairID(i)]=pGameStart->wUserMultiple[i];
	}
	m_GameClientView.SetUserScroeTimes(wUserMultiple);
	m_GameClientView.SetLandAction(INVALID_CHAIR,0);

	//玩家设置
	m_GameClientView.m_HandCardControl[1].SetPositively( IsLookonMode()==false || IsAllowLookon() );

	//当前玩家
	if ((IsLookonMode()==false)&&(pGameStart->wCurrentUser==wMeChiarID))
	{
		ActiveGameFrame();
		m_GameClientView.m_btOutCard.EnableWindow(FALSE);
		m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btPassCard.EnableWindow(FALSE);
		m_GameClientView.m_btPassCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btAutoOutCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btAutoOutCard.EnableWindow(TRUE);
	}

	//设置时间
	SetGameTimer(pGameStart->wCurrentUser,IDI_OUT_CARD,TIME_OUT_CARD);

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
	m_bTurnOutType=m_GameLogic.GetCardType(pOutCard->bCardData,pOutCard->bCardCount);
	CopyMemory(m_bTurnCardData,pOutCard->bCardData,sizeof(BYTE)*pOutCard->bCardCount);

	//炸弹判断
	if ((m_bTurnOutType==CT_BOMB_CARD)||(m_bTurnOutType==CT_MISSILE_CARD))
	{
		m_wBombTime++;
		m_GameClientView.SetBombTime(m_wBombTime);

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
		m_GameClientView.m_HandCardControl[1].SetCardData(m_bHandCardData,m_bHandCardCount);
	}

	//界面设置	
	m_bCardCount[pOutCard->wOutCardUser]-=pOutCard->bCardCount;
	m_GameClientView.SetCardCount(wOutViewChairID,m_bCardCount[pOutCard->wOutCardUser]);
	for (WORD wChairID=0; wChairID<GAME_PLAYER; ++wChairID)
	{
		WORD wViewID = SwitchViewChairID(wChairID) ;
		if(1!=wViewID)
		{
			BYTE cbCardData[20] ;
			ZeroMemory(cbCardData, sizeof(cbCardData)) ;
			m_GameClientView.m_HandCardControl[wViewID].SetCardData(cbCardData,m_bCardCount[wChairID]);
		}
	}

	//报双声音
	if(m_bCardCount[pOutCard->wOutCardUser]==1)PlayGameSound(AfxGetInstanceHandle(),TEXT("ONE_SHEET"));
	//报单声音
	else if(m_bCardCount[pOutCard->wOutCardUser]==2)PlayGameSound(AfxGetInstanceHandle(),TEXT("TWO_SHEET"));
	//炸弹判断
	else if ((m_bTurnOutType==CT_BOMB_CARD)||(m_bTurnOutType==CT_MISSILE_CARD))
		PlayGameSound(AfxGetInstanceHandle(),TEXT("BOMB_CARD"));
	//三带一型
	else if(m_bTurnOutType==CT_THREE_LINE_TAKE_ONE && pOutCard->bCardCount==4)
		PlayGameSound(AfxGetInstanceHandle(),TEXT("THREE_LINE_TAKE_ONE"));
	//六张以上
	else if(pOutCard->bCardCount>=6)
		PlayGameSound(AfxGetInstanceHandle(),TEXT("SIX_SHEET"));
	//单张声音
	else if(pOutCard->bCardCount==1)
	{
		BYTE bCardValue = m_GameLogic.GetCardValue( pOutCard->bCardData[0]);
		BYTE bCardColor = m_GameLogic.GetCardColor( pOutCard->bCardData[0]);
		if(bCardValue==2)
		{
			//黑二声音
			if((bCardColor>>4)%2==1)PlayGameSound(AfxGetInstanceHandle(),TEXT("BLACK_TWO"));
			//红二声音
			else PlayGameSound(AfxGetInstanceHandle(),TEXT("RED_TWO"));
		}
		//小王声音
		else if(0x0E==bCardValue)PlayGameSound(AfxGetInstanceHandle(),TEXT("KING1"));
		//大王声音
		else if(0x0F==bCardValue)PlayGameSound(AfxGetInstanceHandle(),TEXT("KING2"));
		else PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
	}
	//出牌声音
	else if ((IsLookonMode()==true)||(GetMeChairID()!=pOutCard->wOutCardUser))
		PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));

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

		//PlayGameSound(AfxGetInstanceHandle(),TEXT("MOST_CARD"));

		//设置定时器
		SetTimer(IDI_MOST_CARD,3000,NULL);

		return true;
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
		SetGameTimer(pOutCard->wCurrentUser,IDI_OUT_CARD,(bCardCount<m_bTurnCardCount)?3:TIME_OUT_CARD);
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
		SetGameTimer(pPassCard->wCurrentUser,IDI_OUT_CARD,(bCardCount<m_bTurnCardCount)?3:TIME_OUT_CARD);
	}
	else SetGameTimer(pPassCard->wCurrentUser,IDI_OUT_CARD,TIME_OUT_CARD);

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

	//清理界面
	m_GameClientView.SetLandAction(INVALID_CHAIR,0);

	//隐藏控件
	m_GameClientView.m_btOutCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btPassCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btGiveUpScore.ShowWindow(SW_HIDE);
	m_GameClientView.m_btAutoOutCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btCatch.ShowWindow(SW_HIDE);
	m_GameClientView.m_btNoCatch.ShowWindow(SW_HIDE);
	m_GameClientView.m_btAddScore.ShowWindow(SW_HIDE);
	m_GameClientView.m_btNoAddScore.ShowWindow(SW_HIDE);
	m_GameClientView.m_btBackCatch.ShowWindow(SW_HIDE);
	m_GameClientView.m_btLookCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btSortCard.EnableWindow(FALSE);

	//禁用控件
	m_GameClientView.m_btOutCard.EnableWindow(FALSE);
	m_GameClientView.m_btPassCard.EnableWindow(FALSE);

	//底牌数据
	m_GameClientView.m_BackCardControl.SetCardData(pGameEnd->bBackCard,3);
	m_GameClientView.m_BackCardControl.SetDisplayFlag(true);

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

	m_GameClientView.m_ScoreView.SetGameTax(pGameEnd->lGameTax,m_GameClientView.GetBaseScore());

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
		SetGameTimer(GetMeChairID(),IDI_START_GAME,TIME_START_GAME);

		//托管功能
		 m_GameClientView.m_btAutoPlayOn.ShowWindow(SW_SHOW);
		m_GameClientView.m_btAutoPlayOff.ShowWindow(SW_HIDE);
		OnAutoPlay( false, 0 );
	}
	m_GameClientView.SetUserWaitType(0);

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

		//发送数据
		CMD_C_OutCard OutCard;
		OutCard.bCardCount=1;
		OutCard.bCardData[0]=m_bHandCardData[m_bHandCardCount-1];
		SendData(SUB_C_OUT_CART,&OutCard,sizeof(OutCard)-sizeof(OutCard.bCardData)+OutCard.bCardCount*sizeof(BYTE));

		//预先处理
		PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
		m_GameClientView.m_UserCardControl[1].SetCardData(OutCard.bCardData,OutCard.bCardCount);

		//预先删除
		BYTE bSourceCount=m_bHandCardCount;
		m_bHandCardCount-=OutCard.bCardCount;
		m_GameLogic.RemoveCard(OutCard.bCardData,OutCard.bCardCount,m_bHandCardData,bSourceCount);
		m_GameClientView.m_HandCardControl[1].SetCardData(m_bHandCardData,m_bHandCardCount);
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
		m_GameClientView.m_HandCardControl[1].SetCardData(m_bHandCardData,m_bHandCardCount); 

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
		SetGameTimer(wCurrentUser,IDI_OUT_CARD,TIME_OUT_CARD);

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
				m_GameClientView.m_btGiveUpScore.ShowWindow(SW_SHOW);
			}
			//设置界面
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				WORD wViewID = SwitchViewChairID(i) ;
				if (wViewID!=1)
				{
					BYTE cbCardData[20] ;
					ZeroMemory(cbCardData, sizeof(cbCardData)) ;
					m_GameClientView.m_HandCardControl[wViewID].SetCardData(cbCardData,17);
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

			//设置时间
			if (m_wCurrentUser!=INVALID_CHAIR) SetGameTimer(m_wCurrentUser,IDI_LAND_SCORE,TIME_LAND_SCORE);
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
	m_GameClientView.SetUserScroeTimes(NULL);
	m_GameClientView.SetUserWaitType(0);
	m_GameClientView.SetBombTime(m_wBombTime);
	m_GameClientView.SetCardCount(INVALID_CHAIR,0);
	m_GameClientView.SetLandUser(INVALID_CHAIR);
	m_GameClientView.SetLandAction(INVALID_CHAIR,0);
	m_GameClientView.SetPassFlag(INVALID_CHAIR,false);

	//隐藏控件
	m_GameClientView.m_btStart.ShowWindow(FALSE);
	if (m_GameClientView.m_ScoreView.m_hWnd!=NULL) m_GameClientView.m_ScoreView.DestroyWindow();

	//设置扑克
	m_GameClientView.m_BackCardControl.SetCardData(NULL,0);
	m_GameClientView.m_HandCardControl[1].SetPositively(false);
	for (WORD i=0;i<GAME_PLAYER;i++) 
	{
		m_GameClientView.SetUserScroeTag(i,0);
		m_GameClientView.SetSingleRandCard(i,0);
		m_GameClientView.m_UserCardControl[i].SetCardData(NULL,0);
		m_GameClientView.m_HandCardControl[i].SetCardData(NULL,0);
		WORD wViewID = SwitchViewChairID(i) ;
		if(wViewID==1)	m_GameClientView.m_HandCardControl[wViewID].SetDisplayFlag(true);
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
	OutCard.bCardCount=(BYTE)m_GameClientView.m_HandCardControl[1].GetShootCard(OutCard.bCardData,CountArray(OutCard.bCardData));
	SendData(SUB_C_OUT_CART,&OutCard,sizeof(OutCard)-sizeof(OutCard.bCardData)+OutCard.bCardCount*sizeof(BYTE));

	//预先显示
	PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
	m_GameClientView.m_UserCardControl[1].SetCardData(OutCard.bCardData,OutCard.bCardCount);

	//预先删除
	BYTE bSourceCount=m_bHandCardCount;
	m_bHandCardCount-=OutCard.bCardCount;
	m_GameLogic.RemoveCard(OutCard.bCardData,OutCard.bCardCount,m_bHandCardData,bSourceCount);
	m_GameClientView.m_HandCardControl[1].SetCardData(m_bHandCardData,m_bHandCardCount);

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
	m_GameClientView.m_HandCardControl[1].SetCardData(m_bHandCardData,m_bHandCardCount);

	return 0;
}

//叫分消息
LRESULT CGameClientDlg::OnLandScore(WPARAM wParam, LPARAM lParam)
{
	//设置界面
	KillGameTimer(IDI_LAND_SCORE);
	m_GameClientView.m_btGiveUpScore.ShowWindow(SW_HIDE);

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
	m_GameLogic.SearchOutCard(m_bHandCardData,m_bHandCardCount,m_bTurnCardData,m_bTurnCardCount,OutCardResult);

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

//排序扑克
LRESULT CGameClientDlg::OnMessageSortCard(WPARAM wParam, LPARAM lParam)
{
	//排列扑克
	WORD wMeChairID=GetMeChairID();
	m_GameLogic.SortCardList(m_bHandCardData,m_bHandCardCount,m_cbSortType);

	//获取扑克
	BYTE cbShootCount=(BYTE)m_GameClientView.m_HandCardControl[1].SetCardData(m_bHandCardData,m_bHandCardCount);

	//设置变量
	m_cbSortType=(m_cbSortType==ST_ORDER)?ST_COUNT:ST_ORDER;

	//设置按钮
	UINT uImageID=(m_cbSortType==ST_ORDER)?IDB_ORDER_SORT:IDB_COUNT_SORT;
	m_GameClientView.m_btSortCard.SetButtonImage(uImageID,AfxGetInstanceHandle(),false);

	//更新界面
	m_GameClientView.UpdateGameView(NULL);

	return 0;
}

//看抓消息
LRESULT CGameClientDlg::OnLookOrCatch(WPARAM wParam, LPARAM lParam)
{
	//设置界面
	KillGameTimer(IDI_LOOK_CATCH);
	m_GameClientView.m_btLookCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btBackCatch.ShowWindow(SW_HIDE);

	//发送数据
	CMD_C_LandScore LandScore;
	LandScore.bLandScore=((BYTE(wParam)==TRUE)?BACK_CATCH:LOOK_CARD);
	SendData(SUB_C_LAND_SCORE,&LandScore,sizeof(LandScore));

	//播放声音
	PlayGameSound(AfxGetInstanceHandle(),TEXT("CALL_CARD"));

	return 0;
}

//叫庄消息
LRESULT CGameClientDlg::OnCallBanker(WPARAM wParam, LPARAM lParam)
{
	//设置界面
	KillGameTimer(IDI_LAND_SCORE);
	m_GameClientView.m_btCatch.ShowWindow(SW_HIDE);
	m_GameClientView.m_btNoCatch.ShowWindow(SW_HIDE);

	//发送数据
	CMD_C_LandScore LandScore;
	LandScore.bLandScore=((BYTE(wParam)==TRUE)?MING_CATCH:0);
	SendData(SUB_C_LAND_SCORE,&LandScore,sizeof(LandScore));

	//播放声音
	if(BYTE(wParam)==TRUE)PlayGameSound(AfxGetInstanceHandle(),TEXT("CALL_CARD"));
	else PlayGameSound(AfxGetInstanceHandle(),TEXT("PASS_CARD"));

	return 0;
}

//叫分消息
LRESULT CGameClientDlg::OnAddScore(WPARAM wParam, LPARAM lParam)
{
	//设置界面
	KillGameTimer(IDI_LAND_SCORE);
	m_GameClientView.m_btAddScore.ShowWindow(SW_HIDE);
	m_GameClientView.m_btNoAddScore.ShowWindow(SW_HIDE);

	//发送数据
	CMD_C_LandScore LandScore;
	LandScore.bLandScore=((BYTE(wParam)==TRUE)?DOUBLE_SCORE:0);
	SendData(SUB_C_LAND_SCORE,&LandScore,sizeof(LandScore));

	//播放声音
	if(BYTE(wParam)==TRUE)
	{
		if(GetMeChairID()==m_wLandUser)
			PlayGameSound(AfxGetInstanceHandle(),TEXT("LAND_SCORE"));
		else PlayGameSound(AfxGetInstanceHandle(),TEXT("IDLE_SCORE"));
	}
	else PlayGameSound(AfxGetInstanceHandle(),TEXT("PASS_CARD"));

	return 0;
}
