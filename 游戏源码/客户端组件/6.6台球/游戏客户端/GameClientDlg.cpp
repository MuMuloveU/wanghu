// GameClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "GameClientDlg.h"
#include "resource.h"
#include "define.h"
#include "GameOption.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
BEGIN_MESSAGE_MAP(CGameClientDlg, CGameFrameDlg)
	ON_WM_TIMER()
	ON_MESSAGE(IDM_BEGIN,OnHitBegin)
	ON_MESSAGE(IDM_HITGIVEUP,OnHitGiveUp)
	ON_MESSAGE(IDM_HITREPLACE,OnHitReplace)
	ON_MESSAGE(IDM_HITCONCESSION,OnHitConession)
	ON_MESSAGE(IDM_HIT_BALL,OnHitBall)
	ON_MESSAGE(IDM_PLACE_BALL,OnPlaceBall)
	ON_MESSAGE(UM_AGREEPEACE,OnAgreePeace)
END_MESSAGE_MAP()

//绘画信息
extern struct DrawInfoStruct			g_DrawInfo;

// CGameClientDlg 对话框

CGameClientDlg::CGameClientDlg(): CGameFrameDlg(&m_GameClientView)
{
	InitData();
	m_bCanHitBall=FALSE;
	m_hTimerThreadHandle=NULL;
	//初始化台球桌
	g_DrawInfo.pDesk=&m_Desk;
	g_DrawInfo.pBallList=m_Desk.m_Ball;
	g_DrawInfo.m_StarPos=250;
	g_DrawInfo.bStation=NO_STATE;
	bAgreeConession=FALSE;
	//启动绘画线程
	UINT dwThreadID=0;
	if(!m_hTimerThreadHandle)
	{
		m_hTimerThreadHandle=(HANDLE)::_beginthreadex(NULL,0,TimerThread,this,CREATE_SUSPENDED,&dwThreadID);
		//::SetThreadPriority(m_hTimerThreadHandle,THREAD_PRIORITY_LOWEST);
	}
	::ResumeThread(m_hTimerThreadHandle);
}

CGameClientDlg::~CGameClientDlg()
{
	if(m_hTimerThreadHandle)
	{
		TerminateThread(m_hTimerThreadHandle,0);
		m_hTimerThreadHandle=NULL;
	}
}
void	CGameClientDlg::InitData()
{
	m_bCanHitBall=FALSE;
	g_DrawInfo.bStation=NO_STATE;
	g_DrawInfo.m_iPowerPos=0.9;
	if (m_GameClientView.m_GiveUp) m_GameClientView.m_GiveUp.EnableWindow(FALSE);
	if (m_GameClientView.m_Replace) m_GameClientView.m_Replace.EnableWindow(FALSE);
	m_Desk.InitDesk();
}
bool	CGameClientDlg::InitGameFrame()
{
	InitData();
	m_VorSplitter.SetSplitterType(HorSpliter);
	//	m_hTimerThreadHandle=(HANDLE)::_beginthreadex(NULL,0,TimerThread,this,0,&dwThreadID);
	//建立游戏视图
	m_Desk.InitDesk();
	return true;
}
void	CGameClientDlg::ResetGameFrame()
{
	InitData();
}
void	CGameClientDlg::OnGameOptionSet()
{
	CGameOption GameOption;
	GameOption.m_bEnableSound=IsEnableSound();
	//配置数据
	if (GameOption.DoModal()==IDOK)
	{
		//设置控件
		EnableSound(GameOption.m_bEnableSound);
	}
	return;
}
void	CGameClientDlg::OnLookonChanged(bool bLookonUser, 
										const void * pBuffer, 
										WORD wDataSize)
{
	return ;
}
bool	CGameClientDlg::OnGameMessage(WORD wSubCmdID, 
										const void *pBuffer, 
										WORD wDataSize)
{
	
	switch(wSubCmdID)
	{
	case SUB_GAME_BEGIN	:
		{
			if(wDataSize != sizeof(tagGS_BeginStruct)) 
			{
				return false;
			}
			tagGS_BeginStruct *p = (tagGS_BeginStruct *)(pBuffer);
			for (int i=0;i<GAME_PLAYER;i++) 
			{
				m_GameClientView.m_Station.SetUserPoint(i,0,false);
			}
			bool m_bWatchMode=this->IsLookonMode();
			m_Desk.InitDesk();
			g_DrawInfo.iPower=0;
			bAgreeConession=FALSE;
			g_DrawInfo.m_StarPos=250;
			g_DrawInfo.bStation=NO_STATE;
			m_GameClientView.m_Station.SetPower(0);
			m_GameClientView.m_Station.EndPower();
			m_GameClientView.m_Station.SetBallPos(0.0,0.0);
			m_GameClientView.m_Station.SetTargertBall(0,255);
			m_GameClientView.m_Station.SetTargertBall(1,255);
			m_GameClientView.m_Station.SetTargertBall(this->SwitchViewChairID(p->bFirstHit),1);
			m_GameClientView.m_Station.SetUserPoint(0,0,false);
			m_GameClientView.m_Station.SetUserPoint(1,0,false);
			m_Desk.m_iInBagCount=0;
			m_GameClientView.m_Station.m_IsTrain=p->bIsTrain;
			m_GameClientView.m_ScoreWnd.ShowWindow(SW_HIDE);
			m_bCanHitBall=TRUE;
			if(!m_GameClientView.m_Station.m_IsTrain)
			{
				m_GameClientView.m_GiveUp.EnableWindow(TRUE);
				m_GameClientView.m_Replace.EnableWindow(TRUE);
			}
			m_GameClientView.m_ScoreWnd.ShowWindow(SW_HIDE);
				//处理数据
			//SetTimer(BALL_TIMER,10,NULL);
			if(!p->bIsTrain)
				this->SetGameStatus(GS_GAME_PLAY);
			WORD	MeSeat	= this->GetMeChairID();
			if ((!m_bWatchMode)&&(MeSeat==p->bFirstHit))
			{
				g_DrawInfo.bStation=ME_HIT_BALL;
				m_Desk.SetMessageText(TEXT("游戏开始了，请你先击球！"),-1,g_DrawInfo.pDesk->m_dwHeight-50);
				this->ActiveGameFrame();
			}
			else 
			{
				g_DrawInfo.bStation=OTHER_HIT_BALL;
				m_Desk.SetMessageText(TEXT("游戏开始了， 先由其他玩家击球！"),-1,g_DrawInfo.pDesk->m_dwHeight-50);
			}
				
			//播放声音
			this->SetGameTimer(p->bFirstHit,ID_HIT_BALL,30);
			m_GameClientView.m_Station.Invalidate(FALSE);
			PlayGameSound(AfxGetInstanceHandle(),"GAME_BEGIN");
			return true;
		}
	case ASS_TQ_GAME_FINISH:
		{
			if(wDataSize != sizeof(tagGS_GameFinishStruct)) 
			{
				return false;
			}
			tagGS_GameFinishStruct *pFinish = (tagGS_GameFinishStruct *)(pBuffer);
			this->SetGameStatus(GS_TQ_WAIT_BEGIN);
			KillTimer(BALL_TIMER);
			this->KillGameTimer(0);
			WORD	MeSeat	= this->GetMeChairID();
			if(pFinish->FinishMode==2)
			{
				m_GameClientView.m_Station.SetUserPoint(this->SwitchViewChairID(pFinish->m_NowHitUser),m_Desk.bIsGetStar?12:7,true);
			}
			if(!m_GameClientView.m_Station.m_IsTrain)
			{
				CString strFirstName,strSecondName;
				const tagUserData *pUserData = m_GameClientView.GetUserInfo(this->SwitchViewChairID(MeSeat));
				strFirstName=pUserData->szName;
				m_GameClientView.m_ScoreWnd.SetName(0, const_cast<TCHAR *>(pUserData->szName));
				pUserData = m_GameClientView.GetUserInfo(this->SwitchViewChairID((MeSeat+1)%2));
				strSecondName=pUserData->szName;
				m_GameClientView.m_ScoreWnd.SetName(1, const_cast<TCHAR *>(pUserData->szName));
				m_GameClientView.m_ScoreWnd.SetFactScore(0, pFinish->Point[MeSeat]);
				m_GameClientView.m_ScoreWnd.SetFactScore(1, pFinish->Point[(MeSeat+1)%2]);
				if(pFinish->Point[MeSeat]>pFinish->Point[(MeSeat+1)%2])
				{
					if(pFinish->FinishMode==1)
					{
						this->InsertGeneralString("游戏结束，对方认输",RGB(250,0,0), true);
						m_Desk.SetMessageText(TEXT("对方认输！"),-1,g_DrawInfo.pDesk->m_dwHeight-50);
					}
					PlayGameSound(AfxGetInstanceHandle(),"GAME_WIN");
				}
				else
				{
					PlayGameSound(AfxGetInstanceHandle(),"GAME_LOST");
				}
			}
			m_GameClientView.m_ScoreWnd.ShowWindow(SW_SHOW);
			if(pFinish->m_ReSetRule==MeSeat&&!this->IsLookonMode())
			{
				//m_GameClientView.m_RuleInfo.ShowWindow(SW_SHOW);
				//m_GameClientView.m_RuleInfo.SetFocus();
				m_GameClientView.m_Begin.ShowWindow(SW_SHOW);
			}
			//初始化数据 
			m_Desk.InitDesk();
			m_GameClientView.m_Station.SetBallPos(0,0);
			m_GameClientView.m_Station.SetPower(0);
			m_GameClientView.m_Station.EndPower();
			m_GameClientView.m_Station.SetTargertBall(0,255);
			m_GameClientView.m_Station.SetTargertBall(1,255);
			m_Desk.m_iInBagCount=0;
			g_DrawInfo.bStation=NO_STATE;
			m_GameClientView.m_Station.m_IsTrain=TRUE;
			if(!this->IsLookonMode())
			{
				this->ActiveGameFrame();
				//m_GameClientView.m_GiveUp.EnableWindow(FALSE);
				m_GameClientView.m_Replace.EnableWindow(FALSE);
			}
			return TRUE;
		}
	case ASS_TQ_HIT_INFO:	//击球信息
		{
			if(wDataSize != sizeof(tagGS_HitInfoStruct)) 
			{
				return false;
			}
			tagGS_HitInfoStruct *pHitInfo = (tagGS_HitInfoStruct *)(pBuffer);
			//处理数据
			m_GameClientView.m_Station.SetPower((int)pHitInfo->iPower);
			g_DrawInfo.OtherPolePos.x=pHitInfo->CursorPos.x+m_GameClientView.GetXBase();
			g_DrawInfo.OtherPolePos.y=pHitInfo->CursorPos.y+m_GameClientView.GetYBase();
			return TRUE;
		}
	case ASS_TQ_HIT_BALL:	//击球动作
		{
			if(wDataSize != sizeof(tagGS_HitBallStruct)) 
			{
				return false;
			}
			tagGS_HitBallStruct *pHitInfo = (tagGS_HitBallStruct *)(pBuffer);
			//处理数据
			this->KillGameTimer(0);
				//显示动作 
			if (m_bCanHitBall)
			{
				m_bCanHitBall=FALSE;
				g_DrawInfo.bStation=BALL_RUN_ING;
				g_DrawInfo.m_iPowerPos=pHitInfo->iPowePos;
				g_DrawInfo.Degree=(int)pHitInfo->iPoweAngle;
				m_Desk.HitBall(pHitInfo->iXPole,pHitInfo->iYPole,0.3+pHitInfo->iPowePos*8.0*(double)pHitInfo->iPower/100.0,
					pHitInfo->iYBallPoint,pHitInfo->iXBallPoint,g_DrawInfo.Degree);
			}
			return TRUE;
		}
	case ASS_TQ_NEXT_HIT:	//下一动作信息
		{	
			if(wDataSize != sizeof(tagGS_NextHitInfoStruct)) 
			{
				return false;
			}
			
			tagGS_NextHitInfoStruct *pNextHit = (tagGS_NextHitInfoStruct *)(pBuffer);
			g_DrawInfo.m_StarPos=250;
			WORD	MeSeat	= this->GetMeChairID();
			if(!m_Desk.m_Ball[0].m_bAccess)   //取出白球，系统发给服务器的数据可能会出现错误，不能确定问题是否解决，这里做了强制
			{
				m_Desk.m_BallBag.TakeBall(0);
				if(pNextHit->bHitPeople==MeSeat&&!IsLookonMode())
				{
					g_DrawInfo.bStation=WATI_SET_BALL;
					m_Desk.SetMessageText(TEXT("对手白球进袋，请你重新放置白球！"),-1,g_DrawInfo.pDesk->m_dwHeight-50,true);
					this->SetGameTimer(MeSeat,PLACE_WHITE_BALL,15);
				}
				else
				{
					g_DrawInfo.bStation=NO_STATE;
					m_Desk.SetMessageText(TEXT("白球进袋！"),-1,g_DrawInfo.pDesk->m_dwHeight-50,true);
					this->SetGameTimer(MeSeat,PLACE_WHITE_BALL,15);
				}
			}
			else
			{
				//恢复数据
				g_DrawInfo.iPower=0;
				m_GameClientView.m_Station.SetPower(0);
				m_GameClientView.m_Station.SetBallPos(0.0,0.0);
				if(!m_GameClientView.m_Station.m_IsTrain)
					m_GameClientView.m_Station.SetUserPoint(this->SwitchViewChairID(pNextHit->bHitPeople),abs(pNextHit->iPoint),true);
				else
					m_GameClientView.m_Station.SetUserPoint(this->SwitchViewChairID(pNextHit->bHitPeople),pNextHit->iPoint,true);
				m_GameClientView.m_Station.SetTargertBall(this->SwitchViewChairID(pNextHit->bHitPeople),pNextHit->bTargerBall);
				for(int i=0;i<BALL_NUM;i++)
				{
					if(m_Desk.m_Ball[i].m_bAccess)
					{
						m_Desk.m_Ball[i].m_xPos=pNextHit->bPlaceInfo[i].xPos;
						m_Desk.m_Ball[i].m_yPos=pNextHit->bPlaceInfo[i].yPos;
						m_Desk.m_Ball[i].SetStation(pNextHit->bPlaceInfo[i].xPos,pNextHit->bPlaceInfo[i].yPos);
					}
					else    //取出彩球，系统发给服务器的数据可能会出现错误，不能确定问题是否解决，这里做了强制
					{
						if(m_Desk.m_Ball[i].m_bColor>1&&(pNextHit->bTargerBall<m_Desk.m_Ball[i].m_bColor||(pNextHit->bTargerBall>m_Desk.m_Ball[i].m_bColor&&pNextHit->bTargerBall==8)))
						{
							tagCS_PlaceBallStruct PlaceBall;
							int x,y;
							m_Desk.m_BallBag.TakeBall(m_Desk.m_Ball[i].m_bColor);	
							switch (m_Desk.m_Ball[i].m_bIndex)
							{
							case 18:
								x=140;
								y=176;
								break;
							case 16:
								x=140;
								y=252;
								break;
							case 17:
								x=140;
								y=110;
								break;
							case 19:
								x=358;
								y=176;
								break;
							case 20:
								x=506;
								y=176;
								break;
							case 21:
								x=654;
								y=176;
								break;
							}

							while(!m_Desk.CanPlaceBall(x,y,false))//当不能放到合适的位置时
							{
								x--;
								if(x<10)
									break;
							}
							m_Desk.m_Ball[m_Desk.m_Ball[i].m_bIndex].SetStation(x,y);
							PlaceBall.bBallCount=1;
							PlaceBall.bPlaceInfo[0].xPos=x;
							PlaceBall.bPlaceInfo[0].yPos=y;
							PlaceBall.bPlaceInfo[0].bBallIndex=m_Desk.m_Ball[i].m_bIndex;
							this->SendData(ASS_TQ_PLACE_COLOR,&PlaceBall,sizeof(PlaceBall));
							return true;
						}
					}
				}
				m_bCanHitBall=TRUE;
				//处理数据
				CString str;
				bool m_bWatchMode=this->IsLookonMode();
				if ((!m_bWatchMode&&MeSeat==pNextHit->bHitPeople))
				{
					this->ActiveGameFrame();
					SetCursor(m_GameClientView.m_hMouse);
					g_DrawInfo.bStation=ME_HIT_BALL;
					if(pNextHit->iPoint>0)
					{
						g_DrawInfo.m_StarPos=pNextHit->mStarPos;
						g_DrawInfo.m_StarPoint=pNextHit->mStarPoint;
						if(pNextHit->bIsGetStarPoint)
							str.Format("击中幸运星，获得额外奖励，继续击球，得 %d 分！",abs(pNextHit->iPoint));
						else
							str.Format("继续击球，得 %d 分！",abs(pNextHit->iPoint));
						m_Desk.SetMessageText(str.GetBuffer(MAX_PATH),-1,g_DrawInfo.pDesk->m_dwHeight-50);
					}
					else
					{
						if(pNextHit->iPoint==0)
						{
							m_Desk.SetMessageText(TEXT("轮到你击球！"),-1,g_DrawInfo.pDesk->m_dwHeight-50,true);
						}
						else
						{
							if(!m_GameClientView.m_Station.m_IsTrain)
							{
								//if(m_NextHitBall!=1&&m_NextHitBall!=8)
								m_GameClientView.m_Concession.EnableWindow(TRUE);
								str.Format("对方击球错误，轮到你击球，得 %d 分！",abs(pNextHit->iPoint));
							}
							else
								str.Format("击球错误，扣 %d 分！",abs(pNextHit->iPoint));
							m_Desk.SetMessageText(str.GetBuffer(MAX_PATH),-1,g_DrawInfo.pDesk->m_dwHeight-50);
						}
					}
				}
				else
				{
					g_DrawInfo.bStation=OTHER_HIT_BALL;
					if(pNextHit->iPoint!=0)
					{
						if(pNextHit->iPoint>0)
						{
							g_DrawInfo.m_StarPos=pNextHit->mStarPos;
							g_DrawInfo.m_StarPoint=pNextHit->mStarPoint;
							if(pNextHit->bIsGetStarPoint)
								str.Format("其他玩家击中幸运星，获得额外奖励，继续击球，得 %d 分！",abs(pNextHit->iPoint));
							else
								str.Format("其他玩家继续击球，对方得 %d 分！",abs(pNextHit->iPoint));
							m_Desk.SetMessageText(str.GetBuffer(MAX_PATH),-1,g_DrawInfo.pDesk->m_dwHeight-50);
						}
						else
						{
							str.Format("击球错误，轮到其他玩家击球，对方得 %d 分！",abs(pNextHit->iPoint));
							m_Desk.SetMessageText(str.GetBuffer(MAX_PATH),-1,g_DrawInfo.pDesk->m_dwHeight-50);
						}
					}
					else
					{
						m_Desk.SetMessageText(TEXT("轮到其他玩家击球！"),-1,g_DrawInfo.pDesk->m_dwHeight-50);
					}
				}
				m_NextHitBall=pNextHit->bTargerBall;
				//设置时间
				this->SetGameTimer(pNextHit->bHitPeople,ID_HIT_BALL,30);
			}
			return TRUE;
		}
	case ASS_TQ_TAKE_WHITE:	//取白球
		{
			if(wDataSize != sizeof(tagGS_CurrentPlayerStruct)) 
			{
				return false;
			}
			tagGS_CurrentPlayerStruct *m_TakeWhiteUser = (tagGS_CurrentPlayerStruct *)(pBuffer);
			//恢复数据
			g_DrawInfo.iPower=0;
			m_GameClientView.m_Station.SetPower(0);
			m_GameClientView.m_Station.SetBallPos(0.0,0.0);
			m_Desk.m_BallBag.TakeBall(0);
			bool m_bWatchMode=this->IsLookonMode();
			WORD	MeSeat	= this->GetMeChairID();
			g_DrawInfo.m_StarPos=250;
			CString str;
			if (!IsLookonMode()&&((m_TakeWhiteUser->Seat==MeSeat)||m_GameClientView.m_Station.m_IsTrain))
			{
				if(!m_bWatchMode)
					g_DrawInfo.bStation=WATI_SET_BALL;
				if(m_GameClientView.m_Station.m_IsTrain)
				{
					if(m_TakeWhiteUser->iPoint==0)
						str.Format("白球进袋，请你重新放置白球！");
					else
						str.Format("白球进袋，扣 %d 分，请你重新放置白球！",abs(m_TakeWhiteUser->iPoint));
				}
				else
					str.Format("对手白球进袋，得 %d 分，请你重新放置白球！",abs(m_TakeWhiteUser->iPoint));
				m_Desk.SetMessageText(str.GetBuffer(MAX_PATH),-1,g_DrawInfo.pDesk->m_dwHeight-50);
				this->SetGameTimer(MeSeat,PLACE_WHITE_BALL,15);
				if(m_GameClientView.m_Station.m_IsTrain)
					m_GameClientView.m_Station.SetUserPoint(this->SwitchViewChairID(MeSeat),-abs(m_TakeWhiteUser->iPoint),true);
				else
					m_GameClientView.m_Station.SetUserPoint(this->SwitchViewChairID(MeSeat),abs(m_TakeWhiteUser->iPoint),true);
			}
			else
			{
				g_DrawInfo.bStation=NO_STATE;
				if(m_TakeWhiteUser->iPoint!=0)
					str.Format("误进白球，对方得 %d 分！",abs(m_TakeWhiteUser->iPoint));
				else
					str.Format("误进白球！");
				m_Desk.SetMessageText(str.GetBuffer(MAX_PATH),-1,g_DrawInfo.pDesk->m_dwHeight-50);
				//m_Desk.SetMessageText(TEXT("白球进袋，请等待对手放置白球！"),-1,g_DrawInfo.pDesk->m_dwHeight-50,true);
				this->SetGameTimer((MeSeat+1)%2,PLACE_WHITE_BALL,15);
				m_GameClientView.m_Station.SetUserPoint(this->SwitchViewChairID((MeSeat+1)%2),abs(m_TakeWhiteUser->iPoint),true);
			}
			return TRUE;
		}
	case ASS_TQ_PLACE_WHITE:	//放置白球
		{
			//设置数据
			tagGS_PlaceInfoStruct * pPlaceWhite=(tagGS_PlaceInfoStruct *)pBuffer;
			m_Desk.m_pBallList[0].SetStation(pPlaceWhite->bPlaceInfo[0].xPos,pPlaceWhite->bPlaceInfo[0].yPos);
			g_DrawInfo.m_StarPos=250;
			g_DrawInfo.iPower=0;
			m_GameClientView.m_Station.SetPower(0);
			m_GameClientView.m_Station.SetBallPos(0.0,0.0);
			m_GameClientView.m_Station.SetTargertBall(this->SwitchViewChairID(pPlaceWhite->bHitPeople),pPlaceWhite->bTargerBall);	
			for(int i=0;i<BALL_NUM;i++)//这里修正两方数据相等		
			{
				if(m_Desk.m_Ball[i].m_bAccess)
				{
					m_Desk.m_Ball[i].SetStation(pPlaceWhite->bPlaceInfo[i].xPos,pPlaceWhite->bPlaceInfo[i].yPos);
				}
			}
			bool m_bWatchMode=this->IsLookonMode();
			WORD	MeSeat	= this->GetMeChairID();
			m_bCanHitBall=TRUE;
			if ((!m_bWatchMode)&&(MeSeat==pPlaceWhite->bHitPeople))
			{
				g_DrawInfo.bStation=ME_HIT_BALL;
				SetCursor(m_GameClientView.m_hMouse);
				if(!m_GameClientView.m_Station.m_IsTrain)
					m_GameClientView.m_Concession.EnableWindow(TRUE);
				m_Desk.SetMessageText(TEXT("轮到你击球！"),-1,g_DrawInfo.pDesk->m_dwHeight-50);
			}
			else
			{
				g_DrawInfo.bStation=OTHER_HIT_BALL;
				m_Desk.SetMessageText(TEXT("轮到对手击球！"),-1,g_DrawInfo.pDesk->m_dwHeight-50);
			}
			//设置时间
			this->SetGameTimer(pPlaceWhite->bHitPeople,ID_HIT_BALL,30);
			return TRUE;
		}
		case ASS_TQ_PLACE_COLOR:		//GSQ:如果玩家误进彩球，发送给每一个玩家，重新放彩球的位置
			{
				if(wDataSize != sizeof(tagGS_ColorBallPlaceInfoStruct)) 
				{
					return false;
				}
				tagGS_ColorBallPlaceInfoStruct *pColorBall = (tagGS_ColorBallPlaceInfoStruct *)(pBuffer);

				bool m_bWatchMode=this->IsLookonMode();
				WORD	MeSeat	= this->GetMeChairID();
				CString str;
				if ((!m_bWatchMode && pColorBall->BallError))
				{
					if ( MeSeat!=pColorBall->Seat)
					{
						if(m_GameClientView.m_Station.m_IsTrain)
							str.Format("误进球，扣 %d 分！",abs(pColorBall->BallValue));
						else
							str.Format("自己误进球，对方得 %d 分！",abs(pColorBall->BallValue));
						m_Desk.SetMessageText(str.GetBuffer(MAX_PATH),-1,g_DrawInfo.pDesk->m_dwHeight-50);
					}
					else 
					{
						if(m_GameClientView.m_Station.m_IsTrain)
							str.Format("误进球，扣 %d 分！",abs(pColorBall->BallValue));
						else
						{
							str.Format("其他玩家误进球，得 %d 分！",abs(pColorBall->BallValue));
							m_GameClientView.m_Concession.EnableWindow(TRUE);
						}
						m_Desk.SetMessageText(str.GetBuffer(MAX_PATH),-1,g_DrawInfo.pDesk->m_dwHeight-50);
					}
				}
				g_DrawInfo.m_StarPos=250;
				tagCS_PlaceBallStruct PlaceBall;
				PlaceBall.bBallCount=0;
				int mmm=pColorBall->bBallCount;
				for(int i=0;i<pColorBall->bBallCount;i++)
				{
					m_Desk.m_BallBag.TakeBall(m_Desk.m_Ball[pColorBall->bInBallIndex[i]].m_bColor);
					//下面处理放彩球，如果欲放的位置有其它球，则先向上逐渐偏移20，还不行，向下，向左，向右，都不行，显示“你是霉星”，结束游戏
					int x,y;
					if(m_Desk.m_Ball[pColorBall->bInBallIndex[i]].m_bIndex==0)
						continue;
					switch (m_Desk.m_Ball[pColorBall->bInBallIndex[i]].m_bIndex)
					{
					case 18:
						x=140;
						y=176;
						break;
					case 16:
						x=140;
						y=252;
						break;
					case 17:
						x=140;
						y=110;
						break;
					case 19:
						x=358;
						y=176;
						break;
					case 20:
						x=506;
						y=176;
						break;
					case 21:
						x=654;
						y=176;
						break;
					default:
						AfxMessageBox("出现错误!");
						CString str;
						str.Format("误进球的索引是：%d",pColorBall->bInBallIndex[i]);
						MessageBox(str);
						break;
					}
	
					while(!m_Desk.CanPlaceBall(x,y,false))//当不能放到合适的位置时
					{
						x--;
						if(x<10)
							break;
					}
					m_Desk.m_Ball[pColorBall->bInBallIndex[i]].SetStation(x,y);
					PlaceBall.bBallCount++;
					PlaceBall.bPlaceInfo[i].xPos=x;
					PlaceBall.bPlaceInfo[i].yPos=y;
					PlaceBall.bPlaceInfo[i].bBallIndex=pColorBall->bInBallIndex[i];
				}
				this->SendData(ASS_TQ_PLACE_COLOR,&PlaceBall,sizeof(PlaceBall));
				return TRUE;
			}
		case ASS_TQ_1:
			{
				return true;
			}
		case ASS_TQ_2:
			{
				return true;
			}
		case ASS_TQ_HIT_REPLACE:
		 {
			 if(!this->IsLookonMode())
			 {
				 bAgreeConession=TRUE;
				 m_GameClientView.m_PeaceDlg.ShowWindow(SW_SHOW);
			 }
			 return true;
		 }
		case ASS_TQ_NOAGREE:
			{
				if(!this->IsLookonMode())
				{
					this->InsertGeneralString("继续游戏，对方不同意求和",RGB(250,0,0), true);
					return true;
				}
			}
		}
	return true;
}
bool	CGameClientDlg::OnGameSceneMessage(BYTE cbGameStatus, 
											bool bLookonOther, 
											const void * pBuffer, 
											WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GS_TQ_WAIT_BEGIN:		//游戏等待中状态
		{
			//显示设置
			m_Desk.InitDesk();
			if(!IsLookonMode())
			{
				m_GameClientView.m_Begin.ShowWindow(SW_SHOW);
				//m_GameClientView.m_RuleInfo.ShowWindow(SW_SHOW);
				//m_GameClientView.m_RuleInfo.SetFocus();
			}
			/*for(int i=0;i<2;i++)
			{
				const tagUserData *UserDara=GetUserData(i);
				CString str;
				str.Format("%d",UserDara->wTableID);
				::MessageBox(NULL,str,NULL,MB_OK);
			}*/
			return TRUE;
		}
	case GS_GAME_PLAY:
		{
			if(wDataSize != sizeof(tagGS_GameStation_2)) 
			{
				return false;
			}
			m_GameClientView.m_Station.m_IsTrain=FALSE;
			m_bCanHitBall=TRUE;
			bool m_bWatchMode=this->IsLookonMode();
			m_Desk.InitDesk();
			//SetTimer(BALL_TIMER,10,NULL);
			g_DrawInfo.bStation=OTHER_HIT_BALL;
			if(!m_bWatchMode)
			{
				m_GameClientView.m_GiveUp.EnableWindow(TRUE);
				m_GameClientView.m_Replace.EnableWindow(TRUE);
				g_DrawInfo.bStation=NO_STATE;
			}
			g_DrawInfo.iPower=0;
			m_GameClientView.m_Station.SetPower(0);
			m_GameClientView.m_Station.SetBallPos(0.0,0.0);
			WORD	MeSeat	= this->GetMeChairID();
			tagGS_GameStation_2 * pGameStation=(tagGS_GameStation_2 *)pBuffer;
			for(int i=0;i<BALL_NUM;i++)
			{
				m_Desk.m_Ball[i].m_bAccess=pGameStation->bBallAccess[i];
				if(!pGameStation->bBallAccess[i])
				{
					m_Desk.m_BallBag.PushBall(m_Desk.m_pBallList[i]);
				}
				else
				{
					m_Desk.m_Ball[i].SetStation(pGameStation->bPlaceInfo[i].xPos,pGameStation->bPlaceInfo[i].yPos);
				}
			}
			m_NextHitBall=pGameStation->bTargetBall;					//目标球
			m_GameClientView.m_Station.SetTargertBall(this->SwitchViewChairID(pGameStation->bNowHitUser),pGameStation->bTargetBall);
			m_GameClientView.m_Station.SetUserPoint(this->SwitchViewChairID(0),pGameStation->iUserPoint[0],false);				//当前分数
			m_GameClientView.m_Station.SetUserPoint(this->SwitchViewChairID(1),pGameStation->iUserPoint[1],false);				//当前分数
			g_DrawInfo.m_StarPos=pGameStation->mStarPos;
			if(pGameStation->bNowHitUser==MeSeat&&!m_bWatchMode)
			{
				if(pGameStation->bIsTakeWhite)
				{
					m_Desk.SetMessageText(TEXT("对手白球进袋，请你重新放置白球！"),-1,g_DrawInfo.pDesk->m_dwHeight-50);
					g_DrawInfo.bStation=WATI_SET_BALL;
					this->SetGameTimer(MeSeat,PLACE_WHITE_BALL,15);
				}
				else
				{
					g_DrawInfo.bStation=ME_HIT_BALL;
					g_DrawInfo.iPower=0;
					m_Desk.SetMessageText(TEXT("请击球！"),-1,g_DrawInfo.pDesk->m_dwHeight-50);
					this->SetGameTimer(MeSeat,ID_HIT_BALL,30);
				}
			}
			else
			{
				if(pGameStation->bIsTakeWhite)
				{
					m_Desk.SetMessageText(TEXT("白球进袋，其他玩家放置白球！"),-1,g_DrawInfo.pDesk->m_dwHeight-50);
					g_DrawInfo.bStation=NO_STATE;
				}
			}
			this->SetGameStatus(GS_GAME_PLAY);
			return true;
		}
	}
	return TRUE;
}
//时间消息
bool	CGameClientDlg::OnTimerMessage(WORD wChairID, 
										UINT nElapse, 
										UINT nTimerID)
{
	bool m_bWatchMode=this->IsLookonMode();
	WORD	MeSeat	= this->GetMeChairID();
	switch (nTimerID)
	{
	case ID_HIT_BALL:	//击球定时器
		{

			//超时自动出牌
			if((!m_bWatchMode) 
				&& (wChairID==MeSeat))
			{
				if(nElapse < 10)
				{
					if (nElapse<1)
					{
						OnHitBall(0,0);
						return false;
					}
					PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
				}
				if(!m_GameClientView.m_Station.m_IsTrain)
				{
					tagGS_HitInfoStruct HitInfo;
					POINT Point;
					::GetCursorPos(&Point);
					m_GameClientView.ScreenToClient(&Point);
					Point.x-=m_GameClientView.GetXBase();
					Point.y-=m_GameClientView.GetYBase();
					HitInfo.CursorPos=Point;
					HitInfo.iPower=g_DrawInfo.iPower;
					this->SendData(ASS_TQ_HIT_INFO,&HitInfo,sizeof(HitInfo));
				}
			}
			return true;
		}
	case PLACE_WHITE_BALL:
		{
			if((!m_bWatchMode) 
				&& (wChairID==MeSeat))
			{
				if (nElapse<=10)
					PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));

				if (nElapse<1)
				{
					double x,y;
					x=170;
					y=254;
					int temp=0;
					while(!m_Desk.CanPlaceBall(x,y,true))
					{
						x--;
						if(x<110)
						{
							x=170;
							y=174;
						}
						if(temp==120)
						{
							x=170;
							y=216;
						}
						if(temp++>180)
						{
							AfxMessageBox("老兄，你的运气太好了，程序中百年难得一见的错误被你遇上了，程序即将退出，有疑问请与开发商联系！");
							GameQuit();
							return true;
						}
					}

					//发送放球消息
					tagCS_PlaceBallStruct Place;
					Place.bBallCount=1;
					Place.bPlaceInfo[0].bBallIndex=0;
					Place.bPlaceInfo[0].xPos=x;
					Place.bPlaceInfo[0].yPos=y;
					this->SendData(ASS_TQ_PLACE_WHITE,&Place,sizeof(Place));
					return false;
				}
			}
			return true;
		}
		default:
		{
			return false;
		}
	}
	return false;
}
void CGameClientDlg::GameQuit()
{
	::SendMessage(((CWnd*)this)->GetSafeHwnd(),WM_CLOSE,0,0);
}
void CGameClientDlg::GameStart()
{
	m_GameClientView.m_Station.m_IsTrain=TRUE;
	KillGameTimer(0);
	m_GameClientView.m_ScoreWnd.ShowWindow(SW_HIDE);
	this->SendUserReady(NULL,0);
}

LRESULT	CGameClientDlg::OnHitGiveUp(WPARAM wparam, LPARAM lparam)
{
	this->SendData(ASS_TQ_HIT_GIVEUP);
	return 0;
}
//开始按钮函数
LRESULT	CGameClientDlg::OnHitBegin(WPARAM wparam, LPARAM lparam)
{
	m_GameClientView.m_Station.m_IsTrain=TRUE;
	KillGameTimer(0);
	this->SendUserReady(NULL,0);
	return 0;
}
LRESULT	CGameClientDlg::OnHitReplace(WPARAM wparam, LPARAM lparam)
{
	this->SendData(ASS_TQ_HIT_REPLACE);
	return 0;
}
LRESULT	CGameClientDlg::OnHitConession(WPARAM wparam, LPARAM lparam)
{
	this->SendData(ASS_TQ_HIT_CONESSION);
	return 0;
}

//放球消息
LRESULT	CGameClientDlg::OnPlaceBall(WPARAM wparam, LPARAM lparam)
{
	CPoint * pPoint=(CPoint *)wparam;

	double x=(double)(pPoint->x-m_GameClientView.GetXBase())/g_DrawInfo.dwTimes;
	double y=(double)(pPoint->y-m_GameClientView.GetYBase())/g_DrawInfo.dwTimes;
	if (m_Desk.CanPlaceBall(x,y,true)&&!m_Desk.m_bNeedMove&&!IsLookonMode())
	{
		//发送放球消息
		tagCS_PlaceBallStruct Place;
		Place.bBallCount=1;
		Place.bPlaceInfo[0].bBallIndex=0;
		Place.bPlaceInfo[0].xPos=x;
		Place.bPlaceInfo[0].yPos=y;
		this->SendData(ASS_TQ_PLACE_WHITE,&Place,sizeof(Place));
		this->KillGameTimer(PLACE_WHITE_BALL);
	}
	return 0;
}

//开始击球消息
LRESULT	CGameClientDlg::OnHitBall(WPARAM wparam, LPARAM lparam)
{
	this->KillGameTimer(0);
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_HITBALL"));
	m_GameClientView.m_Concession.EnableWindow(FALSE);
	if (g_DrawInfo.bStation==ME_HIT_BALL)
	{
		if(bAgreeConession)
		{
			bAgreeConession=FALSE;
			this->SendData(ASS_TQ_NOAGREE);
			m_GameClientView.m_PeaceDlg.ShowWindow(SW_HIDE);
		}
		POINT Point;
		tagGS_HitBallStruct HitBall;
		::GetCursorPos(&Point);
		m_GameClientView.ScreenToClient(&Point);
		g_DrawInfo.bStation=BALL_RUN_ING;
		::SetCursor(AfxGetApp()->LoadCursor(IDC_ARROW));
		m_bCanHitBall=FALSE;
		//发送网络消息
		HitBall.iXPole=(int)((double)(Point.x-m_GameClientView.GetXBase())/g_DrawInfo.dwTimes);
		HitBall.iYPole=(int)((double)(Point.y-m_GameClientView.GetYBase())/g_DrawInfo.dwTimes);
		HitBall.iXBallPoint=m_GameClientView.m_Station.GetBallYPos();
		HitBall.iYBallPoint=m_GameClientView.m_Station.GetBallXPos();
		HitBall.iPower=m_GameClientView.m_Station.GetPower();
		HitBall.iPowePos=g_DrawInfo.m_iPowerPos;
		HitBall.iPoweAngle=g_DrawInfo.Degree;
		m_GameClientView.m_Station.EndPower();
		g_DrawInfo.pDesk->HitBall(HitBall.iXPole,HitBall.iYPole,0.3+HitBall.iPowePos*8.0*(double)HitBall.iPower/100.0,
			HitBall.iYBallPoint,HitBall.iXBallPoint,HitBall.iPoweAngle);
		if(!m_GameClientView.m_Station.m_IsTrain)
			this->SendData(ASS_TQ_HIT_BALL,&HitBall,sizeof(HitBall));
	}
	else
	{
		::SetCursor(AfxGetApp()->LoadCursor(IDC_ARROW));
	}
	return 0;
}
//定时器线程
unsigned __stdcall CGameClientDlg::TimerThread(LPVOID pThreadData)
{
	CGameClientDlg * pGameDlg=(CGameClientDlg *)pThreadData;
	while (1)
	{
		Sleep(10);
		pGameDlg->PostMessage(WM_TIMER,BALL_TIMER,0);
	}
	return 0;
}
LRESULT CGameClientDlg::OnAgreePeace(WPARAM wParam, LPARAM lParam)
{
	 m_GameClientView.m_PeaceDlg.ShowWindow(SW_HIDE);
	 bAgreeConession=FALSE;
	 if(wParam==1)
		this->SendData(ASS_TQ_AGREE);
	 else
		this->SendData(ASS_TQ_NOAGREE);
	return 0;
}
void CGameClientDlg::OnTimer(UINT nIDEvent)
{
	if(nIDEvent==BALL_TIMER)		//桌子定时器
	{
		bool m_bWatchMode=this->IsLookonMode();
		m_Desk.OnTimer();
		if (m_Desk.bSoundInBag)
			PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_INPOLE"));
		else if (m_Desk.bSoundHitBall) 
			PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_BUMPBALL"));
		else if (m_Desk.bSoundHitDesk)
			PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_BUMPSIDE"));
		if ((!m_Desk.m_bNeedMove)&&(!m_Desk.m_BallBag.m_bNeedMove)&&(g_DrawInfo.bStation==BALL_RUN_ING))
		{
			g_DrawInfo.bStation=NO_STATE;
			g_DrawInfo.m_StarPos=250;
			if (!m_bWatchMode)
			{
				tagGS_HitFinishStruct HitFinish;
				::memset(&HitFinish,0,sizeof(HitFinish));
				HitFinish.bFirstHitBall=(m_Desk.m_iFirstHitBag==-1)?255:(BYTE)m_Desk.m_iFirstHitBag;
				HitFinish.bInBallCount=m_Desk.m_iInBagCount;
				//判断进球
				for (int i=0;i<BALL_NUM;i++)
				{
					HitFinish.iXBallStation[i]=m_Desk.m_Ball[i].m_xPos;
					HitFinish.iYBallStation[i]=m_Desk.m_Ball[i].m_yPos;
					HitFinish.bBallAccess[i]=TRUE;
					if (m_Desk.m_Ball[i].m_bAccess)
					{
						HitFinish.bBallAccess[i]=TRUE;
					}
					else
						HitFinish.bBallAccess[i]=FALSE;
				}
				for (int i=0;i<m_Desk.m_iInBagCount;i++)
				{
					HitFinish.bInBallIndex[i]=m_Desk.m_BallBag.m_pBall[m_Desk.m_BallBag.m_iBallCount-i-1]->m_bIndex;
					HitFinish.bBallAccess[HitFinish.bInBallIndex[i]]=FALSE;
					m_Desk.m_Ball[HitFinish.bInBallIndex[i]].m_bAccess=FALSE;
				}
				if(m_Desk.bIsGetStar)
				{
					HitFinish.bIsGetStar=TRUE;
					m_Desk.bIsGetStar=FALSE;
				}
				else
					HitFinish.bIsGetStar=FALSE;
				this->SendData(ASS_TQ_HIT_FINISH,&HitFinish,sizeof(HitFinish));
			}
		}
	}
}
