#include "Stdafx.h"
#include "GameOption.h"
#include "GameClient.h"
#include "GameClientDlg.h"
#include ".\gameclientdlg.h"
#include "cassert"
//////////////////////////////////////////////////////////////////////////
//宏定义

//游戏定时器
#define IDI_OUT_CARD					200								//出牌定时器
#define IDI_START_GAME					201								//开始定时器

//游戏定时器
#define IDI_MOST_CARD					300								//最大定时器
#define IDI_DISPATCH_CARD				301								//发牌定时器

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientDlg, CGameFrameDlg)
	ON_WM_TIMER()
	ON_MESSAGE(IDM_START,OnStart)
	ON_MESSAGE(IDM_OUT_CARD,OnOutCard)
	ON_MESSAGE(IDM_PASS_CARD,OnPassCard)
	ON_MESSAGE(IDM_OUT_PROMPT,OnOutPrompt)
	ON_MESSAGE(IDM_SORT_BY_COLOR,OnSortByColor)
	ON_MESSAGE(IDM_SORT_BY_VALUE,OnSortByValue)
	ON_MESSAGE(IDM_LEFT_HIT_CARD,OnLeftHitCard)
	ON_MESSAGE(IDM_RIGHT_HIT_CARD,OnRightHitCard)
	ON_MESSAGE(IDM_TRUSTEE_CONTROL,OnStusteeControl)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientDlg::CGameClientDlg() : CGameFrameDlg(&m_GameClientView)
{
	//游戏变量
	m_wBankerUser=INVALID_CHAIR;

	//游戏变量
	m_wCurrentUser=INVALID_CHAIR;

	//胜利信息
	m_wWinCount=0;
	ZeroMemory(m_wWinOrder,sizeof(m_wWinOrder));

	//配置变量
	m_bStustee=false;
	m_bDeasilOrder=false;
	m_dwCardHSpace=DEFAULT_PELS;

	//扑克变量
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));
	ZeroMemory(m_cbPartnerCardData,sizeof(m_cbPartnerCardData));

	//出牌变量
	m_cbTurnCardCount=0;
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));

	//辅助变量
	m_cbRemnantCardCount=0;
	m_cbDispatchCardCount=0;
	m_wMostUser=INVALID_CHAIR;
	m_wTimeOutCount=0;
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

	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//读取配置
	m_dwCardHSpace=AfxGetApp()->GetProfileInt(TEXT("GameOption"),TEXT("CardSpace"),DEFAULT_PELS);
	m_bDeasilOrder=AfxGetApp()->GetProfileInt(TEXT("GameOption"),TEXT("DeasilOrder"),FALSE)?true:false;

	//调整参数
	if ((m_dwCardHSpace>MAX_PELS)||(m_dwCardHSpace<LESS_PELS)) m_dwCardHSpace=DEFAULT_PELS;

	//配置控件
	m_GameClientView.SetUserOrder(m_bDeasilOrder);
	m_GameClientView.m_HandCardControl[2].SetCardSpace(m_dwCardHSpace,0,20);
	m_GameClientView.m_UserCardControl[1].SetCardSpace(0,18,20);
	m_GameClientView.m_UserCardControl[3].SetCardSpace(0,18,20);
	m_GameClientView.m_UserCardControl[0].SetCardSpace(18,18,20);
	m_GameClientView.m_UserCardControl[2].SetCardSpace(18,18,20);


	return true;
}

//重置框架
void CGameClientDlg::ResetGameFrame()
{
	//游戏变量
	m_wBankerUser=INVALID_CHAIR;

	//删除定时
	KillGameTimer(0);
	KillTimer(IDI_MOST_CARD);
	KillTimer(IDI_DISPATCH_CARD);
	
	//游戏变量
	m_wCurrentUser=INVALID_CHAIR;

	//配置变量
	m_bStustee=false;

	//胜利信息
	m_wWinCount=0;
	ZeroMemory(m_wWinOrder,sizeof(m_wWinOrder));

	//扑克变量
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));
	ZeroMemory(m_cbPartnerCardData,sizeof(m_cbPartnerCardData));


	//出牌变量
	m_cbTurnCardCount=0;
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));

	//辅助变量
	m_cbRemnantCardCount=0;
	m_cbDispatchCardCount=0;
	m_wTimeOutCount=0;
	m_wMostUser=INVALID_CHAIR;
	ZeroMemory(m_cbDispatchCardData,sizeof(m_cbDispatchCardData));

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

	//配置数据
	if (GameOption.DoModal()==IDOK)
	{
		//获取参数
		m_bDeasilOrder=GameOption.m_bDeasilOrder;
		m_dwCardHSpace=GameOption.m_dwCardHSpace;

		//设置控件
		EnableSound(GameOption.m_bEnableSound);
		m_GameClientView.SetUserOrder(GameOption.m_bDeasilOrder);
		m_GameClientView.m_HandCardControl[2].SetCardSpace(m_dwCardHSpace,0,20);

		//保存配置
		AfxGetApp()->WriteProfileInt(TEXT("GameOption"),TEXT("CardSpace"),m_dwCardHSpace);
		AfxGetApp()->WriteProfileInt(TEXT("GameOption"),TEXT("DeasilOrder"),m_bDeasilOrder?TRUE:FALSE);
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
			//超次托管
			if ((m_wTimeOutCount>=3)&&(m_bStustee==false))
				OnStusteeControl(0,0);

			//超时判断
			if ((nElapse==0) || (m_bStustee==true&&nElapse<29))
			{
				if ((IsLookonMode()==false)&&(wChairID==GetMeChairID()))
				{
					if (m_bStustee==false)
					{
						m_wTimeOutCount++;
						if(m_wTimeOutCount==3)
						{
							InsertSystemString(TEXT(" 由于您多次超时，切换为“系统托管”模式."));
						}

					}
					AutomatismOutCard();
				}
				return true;
			}

			//播放声音
			if (m_cbHandCardCount[GetMeChairID()]<m_cbTurnCardCount) return true;
			if ((nElapse<=3)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false)) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));

			return true;
		}
	case IDI_START_GAME:		//开始游戏
		{
			if (nElapse==0)
			{
				if ((IsLookonMode()==false)&&(wChairID==GetMeChairID()))
						PostMessage(WM_CLOSE);
				return false;
			}
			if ((nElapse<=3)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false)) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));

			return true;
		}
	}

	return false;
}

//旁观状态
void CGameClientDlg::OnLookonChanged(bool bLookonUser, const void * pBuffer, WORD wDataSize)
{
	//旁观设置
	m_GameClientView.m_HandCardControl[2].SetDisplayFlag((IsLookonMode()==false)||(IsAllowLookon()==true));

	return;
}

//网络消息
bool CGameClientDlg::OnGameMessage(WORD wSubCmdID, const void * pBuffer, WORD wDataSize)
{
	switch (wSubCmdID)
	{
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
	case SUB_S_TRUSTEE:			//用户托管
		{
			return OnSubTrustee(pBuffer,wDataSize);
		}
	}

	return true;
}

//游戏场景
bool CGameClientDlg::OnGameSceneMessage(BYTE cbGameStation, bool bLookonOther, const void * pBuffer, WORD wDataSize)
{
	switch (cbGameStation)
	{
	case GS_RF_FREE:	//空闲状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusFree))
			{
				ASSERT(FALSE);
				return false;
			}
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pBuffer;

			//设置控件
			if (IsLookonMode()==false)
			{
				m_GameClientView.m_btStart.ShowWindow(TRUE);
				m_GameClientView.m_btStart.SetFocus();
				m_GameClientView.m_btStusteeControl.EnableWindow(TRUE);

			}
			//设置扑克
			if (IsLookonMode()==false) m_GameClientView.m_HandCardControl[2].SetDisplayFlag(true);
			//设置时间
			SetGameTimer(GetMeChairID(),IDI_START_GAME,30);
			return true;
		}

	case GS_RF_PLAYING:		//游戏状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pBuffer;

			//变量定义
			WORD wMeChairID=GetMeChairID();

			//设置变量
			m_wCurrentUser=pStatusPlay->wCurrentUser;

			//胜利信息
			m_wWinCount=pStatusPlay->wWinCount;
			CopyMemory(m_wWinOrder,pStatusPlay->wWinOrder,sizeof(m_wWinOrder));

			//出牌变量
			m_cbTurnCardCount=pStatusPlay->cbTurnCardCount;
			CopyMemory(m_cbTurnCardData,pStatusPlay->cbTurnCardData,m_cbTurnCardCount*sizeof(BYTE));

			//扑克数据
			CopyMemory(m_cbHandCardCount,pStatusPlay->cbHandCardCount,sizeof(m_cbHandCardCount));
			CopyMemory(m_cbHandCardData,pStatusPlay->cbHandCardData,sizeof(BYTE)*m_cbHandCardCount[wMeChairID]);
		


			//设置界面
			for (BYTE i=0;i<GAME_PLAYER;i++)
			{
				WORD wViewChairID=SwitchViewChairID(i);
				m_GameClientView.SetCardCount(wViewChairID,pStatusPlay->cbHandCardCount[i]);
			}

			//设置名次
			for (WORD i=0;i<m_wWinCount;i++)
			{
				m_GameClientView.SetUserWinOrder(SwitchViewChairID(m_wWinOrder[i]),i+1);
			}

			//托管设置
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				m_GameClientView.SetTrustee(SwitchViewChairID(i),pStatusPlay->bTrustee[i]);
				m_bStustee=pStatusPlay->bTrustee[GetMeChairID()];
				m_wTimeOutCount=0;
			}
			//设置扑克
			BYTE cbCardCount[MAX_COUNT];
			ZeroMemory(cbCardCount,sizeof(cbCardCount));

			//设置扑克
			for (BYTE i=0;i<GAME_PLAYER;i++)
			{
				WORD wViewChairID=SwitchViewChairID(i);
#ifdef VIDEO_GAME
				if (wViewChairID!=2)
				{
					if(wViewChairID==0)
					{
						m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(cbCardCount,pStatusPlay->cbHandCardCount[i]);
					}
				}
#else
				if (wViewChairID!=2)
				{
					m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(cbCardCount,pStatusPlay->cbHandCardCount[i]);
				}
#endif
				else m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(m_cbHandCardData,m_cbHandCardCount[i]);
			}

			//玩家设置
			if ((IsLookonMode()==false)&&(pStatusPlay->wCurrentUser==GetMeChairID()))
			{
				m_GameClientView.m_btOutCard.EnableWindow(FALSE);
				m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
				//m_GameClientView.m_btOutPrompt.ShowWindow(SW_SHOW);
				m_GameClientView.m_btPassCard.ShowWindow(SW_SHOW);
				m_GameClientView.m_btPassCard.EnableWindow((m_cbTurnCardCount!=0)?TRUE:FALSE);
				
			}
			if(IsLookonMode()==false)
			{
				m_GameClientView.m_btStusteeControl.SetButtonImage((m_bStustee==true)?IDB_BT_STOP_TRUSTEE:IDB_BT_START_TRUSTEE,AfxGetInstanceHandle(),false);
				m_GameClientView.m_btStusteeControl.EnableWindow(TRUE);

			}

			//桌面设置
			if (m_cbTurnCardCount!=0)
			{
				WORD wViewChairID=SwitchViewChairID(pStatusPlay->wTurnWiner);
				m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(m_cbTurnCardData,m_cbTurnCardCount);
			}

			//设置定时器
			SetGameTimer(pStatusPlay->wCurrentUser,IDI_OUT_CARD,30);

			//设置扑克
			if (IsLookonMode()==false) 
			{
				m_GameClientView.m_HandCardControl[2].SetPositively(true);
				m_GameClientView.m_HandCardControl[2].SetDisplayFlag(true);
			}

			return true;
		}
	}

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


	m_wBankerUser=pGameStart->wCurrentUser;

	//游戏变量
	m_wCurrentUser=pGameStart->wCurrentUser;

	//游戏变量
	m_cbTurnCardCount=0;


	//胜利信息
	m_wWinCount=0;
	ZeroMemory(m_wWinOrder,sizeof(m_wWinOrder));

	//出牌变量
	m_cbTurnCardCount=0;
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));

	
	//玩家扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//获取位置
		WORD wViewChairID=SwitchViewChairID(i);

		//设置扑克
		if (wViewChairID==2)
		{
			bool bShowCard=((IsLookonMode()==false)||(IsAllowLookon()==true));
			m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);
			m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(NULL,0);
			m_GameClientView.m_HandCardControl[wViewChairID].SetPositively(false);
			m_GameClientView.m_HandCardControl[wViewChairID].SetDisplayFlag(bShowCard);
		}
		else if(wViewChairID==0)
		{
			m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);
			m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(NULL,0);
			m_GameClientView.m_HandCardControl[wViewChairID].SetDisplayFlag(false);
		}
		else
		{
			m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);
			m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(NULL,0);
			m_GameClientView.m_HandCardControl[wViewChairID].SetDisplayFlag(false);
		}
	}

	//设置扑克
	WORD wMeChairID=GetMeChairID();
	for (WORD i=0;i<GAME_PLAYER;i++) 
	{
		m_cbHandCardCount[i]=CountArray(pGameStart->cbCardData);
		m_GameClientView.SetCardCount(i,m_cbHandCardCount[i]);
	}
	CopyMemory(m_cbHandCardData,pGameStart->cbCardData,sizeof(BYTE)*m_cbHandCardCount[wMeChairID]);
	CopyMemory(m_cbPartnerCardData,pGameStart->cbPartnerCardData,sizeof(BYTE)*m_cbHandCardCount[(wMeChairID+2)%GAME_PLAYER]);

	//派发扑克
	DispatchUserCard(m_cbHandCardData,m_cbHandCardCount[wMeChairID]);

	//排列扑克
	m_GameLogic.SortCardList(m_cbHandCardData,m_cbHandCardCount[wMeChairID]);
	
	//托管设置
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameClientView.SetTrustee(SwitchViewChairID(i),pGameStart->bTrustee[i]);
	}

	//更新界面
	m_GameClientView.SetUserWinOrder(INVALID_CHAIR,0);
	m_GameClientView.UpdateGameView(NULL);

	//环境设置
	if ((IsLookonMode()==false)) ActiveGameFrame(); 

 	m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE);

	return true;
}

//用户出牌
bool CGameClientDlg::OnSubOutCard(const void * pBuffer, WORD wDataSize)
{
 	//变量定义
 	CMD_S_OutCard * pOutCard=(CMD_S_OutCard *)pBuffer;
 	WORD wHeadSize=sizeof(CMD_S_OutCard)-sizeof(pOutCard->cbCardData);
 
 	//效验数据
 	ASSERT(wDataSize>=wHeadSize);
 	if (wDataSize<wHeadSize) return false;
 	ASSERT(wDataSize==(wHeadSize+pOutCard->cbCardCount*sizeof(pOutCard->cbCardData[0])));
 	if (wDataSize!=(wHeadSize+pOutCard->cbCardCount*sizeof(pOutCard->cbCardData[0]))) return false;
 
	//停止发牌
	DoncludeDispatchCard();

 	//删除定时器
 	KillTimer(IDI_MOST_CARD);
 	KillGameTimer(IDI_OUT_CARD);

	//变量定义
	WORD wMeChairID=GetMeChairID();
	WORD wViewChairID=SwitchViewChairID(pOutCard->wOutCardUser); 
	bool bPlayGameSound=((IsLookonMode()==false)&&(wMeChairID==pOutCard->wOutCardUser));


	//历史清理
	if (m_cbTurnCardCount==0)
	{
		//用户扑克
		for (WORD i=0;i<GAME_PLAYER;i++)
		{

			//桌面清理
			if (i!=pOutCard->wOutCardUser)
			{
				WORD wViewChairID=SwitchViewChairID(i);
				m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);
			}
		}

		//放弃标志
		m_GameClientView.SetPassFlag(INVALID_CHAIR,false);
	}
	
	//出牌变量
	m_wCurrentUser=pOutCard->wCurrentUser;
	m_cbTurnCardCount=pOutCard->cbCardCount;
 	CopyMemory(m_cbTurnCardData,pOutCard->cbCardData,sizeof(BYTE)*pOutCard->cbCardCount);

	//引用变量
	BYTE cbStarLevel;

	//获取牌型
	BYTE cbTurnCardType=m_GameLogic.GetCardType(pOutCard->cbCardData,pOutCard->cbCardCount,cbStarLevel);
	//播放声音
	if ((bPlayGameSound==false)&&((IsLookonMode()==true)||(wMeChairID!=pOutCard->wOutCardUser)))
	{
		if (cbTurnCardType>=CT_BOMB)
		{
			//播放声音
			if (bPlayGameSound==false)
			{
				bPlayGameSound=true;
				PlayBombSound(pOutCard->wOutCardUser,cbStarLevel);
			}
		}
		else
		{
			if (bPlayGameSound==false)
			{
				bPlayGameSound=true;
				PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
			}
		}
	}

 
	//其他扑克
	//出牌设置
	if ((IsLookonMode()==true)||(wMeChairID!=pOutCard->wOutCardUser))
	{

		m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(pOutCard->cbCardData,pOutCard->cbCardCount);
		//删除扑克
		if (wViewChairID==2)
		{
			//删除扑克
			BYTE cbSourceCount=m_cbHandCardCount[wMeChairID];
			m_cbHandCardCount[pOutCard->wOutCardUser]-=pOutCard->cbCardCount;
			m_GameLogic.RemoveCard(pOutCard->cbCardData,pOutCard->cbCardCount,m_cbHandCardData,cbSourceCount);

			//设置扑克
			m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(m_cbHandCardData,m_cbHandCardCount[wMeChairID]);
		}
		else if(wViewChairID==0)
		{
			//删除扑克
			BYTE cbSourceCount=m_cbHandCardCount[pOutCard->wOutCardUser];
			m_cbHandCardCount[pOutCard->wOutCardUser]-=pOutCard->cbCardCount;
			m_GameLogic.RemoveCard(pOutCard->cbCardData,pOutCard->cbCardCount,m_cbPartnerCardData,cbSourceCount);

			//设置扑克
			m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(m_cbPartnerCardData,m_cbHandCardCount[pOutCard->wOutCardUser]);
		}
		else
		{
			m_cbHandCardCount[pOutCard->wOutCardUser]-=pOutCard->cbCardCount;
#ifdef VIDEO_GAME
#else
			//设置扑克
			BYTE cbCardCount[MAX_COUNT];
			ZeroMemory(cbCardCount,sizeof(cbCardCount));
			m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(cbCardCount,m_cbHandCardCount[pOutCard->wOutCardUser]);
#endif
		}

	}
	m_GameClientView.SetCardCount(wViewChairID,m_cbHandCardCount[pOutCard->wOutCardUser]);

	//设置名次
	if (m_cbHandCardCount[pOutCard->wOutCardUser]==0)
	{
		m_wWinOrder[m_wWinCount++]=pOutCard->wOutCardUser;
		m_GameClientView.SetUserWinOrder(SwitchViewChairID(pOutCard->wOutCardUser),m_wWinCount);

		if((IsLookonMode()==false)&&GetMeChairID()==pOutCard->wOutCardUser)
			m_GameClientView.m_HandCardControl[0].SetDisplayFlag(true);
	}

	//最大判断
	if (m_wCurrentUser==pOutCard->wOutCardUser)
	{
		//设置变量
		m_cbTurnCardCount=0;
		m_wCurrentUser=INVALID_CHAIR;
		m_wMostUser=pOutCard->wCurrentUser;
		ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));

		//放弃动作
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			//用户过虑
			if (pOutCard->wOutCardUser==i) continue;

			//放弃界面
			WORD wViewChairID=SwitchViewChairID(i);
			m_GameClientView.SetPassFlag(wViewChairID,true);
			m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);

		}

		//播放声音
		if (bPlayGameSound==false)
		{
			bPlayGameSound=true;
			PlayGameSound(AfxGetInstanceHandle(),TEXT("MOST_CARD"));
		}

		//设置定时器
		SetTimer(IDI_MOST_CARD,3000,NULL);

		return true;
	}

	//玩家设置
	if (m_wCurrentUser!=INVALID_CHAIR)
	{
		WORD wViewChairID=SwitchViewChairID(m_wCurrentUser);
		m_GameClientView.SetPassFlag(wViewChairID,false);
		m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);
	}

	//玩家设置
	if ((IsLookonMode()==false)&&(m_wCurrentUser==GetMeChairID()))
	{
		//激活框架
		if (m_bStustee==false) ActiveGameFrame();

		m_GameClientView.m_btPassCard.EnableWindow(TRUE);
		m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btPassCard.ShowWindow(SW_SHOW);
		//m_GameClientView.m_btOutPrompt.ShowWindow(SW_SHOW);
		m_GameClientView.m_btOutCard.EnableWindow((VerdictOutCard()==true)?TRUE:FALSE);
		m_GameClientView.m_btPassCard.EnableWindow((VerdictPassCard()==true)?TRUE:FALSE);
	}

	//设置定时器
	if (pOutCard->wCurrentUser!=INVALID_CHAIR)
	{
		BYTE cbCardCount=m_cbHandCardCount[pOutCard->wCurrentUser];
		SetGameTimer(m_wCurrentUser,IDI_OUT_CARD,(cbCardCount<m_cbTurnCardCount)?3:30);
	}

	return true;
}

//放弃出牌
bool CGameClientDlg::OnSubPassCard(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_PassCard));
	if (wDataSize!=sizeof(CMD_S_PassCard)) return false;

	//变量定义
	CMD_S_PassCard * pPassCard=(CMD_S_PassCard *)pBuffer;

	//删除定时器
	KillGameTimer(IDI_OUT_CARD);

	//设置变量
	m_wCurrentUser=pPassCard->wCurrentUser;

	//放弃设置
	if ((IsLookonMode()==true)||(pPassCard->wPassCardUser!=GetMeChairID()))
	{
		WORD wPassCardUser=pPassCard->wPassCardUser;
		m_GameClientView.SetPassFlag(SwitchViewChairID(wPassCardUser),true);
	}
	if(m_wCurrentUser != INVALID_CHAIR)
	{
		//视图位置
		WORD wViewChairID=SwitchViewChairID(m_wCurrentUser);

		//设置界面
		m_GameClientView.SetPassFlag(wViewChairID,false);
		m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);
	}

	//一轮判断
	if (pPassCard->cbTurnOver==TRUE)
	{
		//出牌变量
		m_cbTurnCardCount=0;
		ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));
        
		//清除界面
		m_GameClientView.SetPassFlag(INVALID_CHAIR,false);

	}

	//玩家设置
	if ((IsLookonMode()==false)&&(m_wCurrentUser==GetMeChairID()))
	{
		//激活框架
		ActiveGameFrame();

		//设置按钮
		m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btPassCard.ShowWindow(SW_SHOW);
		//m_GameClientView.m_btOutPrompt.ShowWindow(SW_SHOW);
		m_GameClientView.m_btOutCard.EnableWindow((VerdictOutCard()==true)?TRUE:FALSE);
		m_GameClientView.m_btPassCard.EnableWindow((VerdictPassCard()==true)?TRUE:FALSE);
	}

	//播放声音
	if ((IsLookonMode()==true)||(pPassCard->wPassCardUser!=GetMeChairID()))
	{
		PlayGameSound(AfxGetInstanceHandle(),TEXT("PASS_CARD"));
	}

	//设置时间
	if (m_cbTurnCardCount!=0)
	{
		BYTE cbCardCount=m_cbHandCardCount[m_wCurrentUser];
		SetGameTimer(m_wCurrentUser,IDI_OUT_CARD,(cbCardCount<m_cbTurnCardCount)?3:30);
	}
	else 
		SetGameTimer(m_wCurrentUser,IDI_OUT_CARD,30);

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

	//设置状态
	SetGameStatus(GS_RF_FREE);

	//停止发牌
	DoncludeDispatchCard();

	//删除定时器
	KillTimer(IDI_MOST_CARD);
	KillGameTimer(IDI_OUT_CARD);
	KillTimer(IDI_DISPATCH_CARD);

	//隐藏控件
	m_GameClientView.m_btOutCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btPassCard.ShowWindow(SW_HIDE);
	//m_GameClientView.m_btOutPrompt.ShowWindow(SW_HIDE);

	//禁用控件
	m_GameClientView.m_btOutCard.EnableWindow(FALSE);
	m_GameClientView.m_btPassCard.EnableWindow(FALSE);

	//设置积分
	CString strTemp ,strEnd = "\n";
	//设置积分
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		const tagUserData * pUserData=GetUserData(i);
		m_GameClientView.m_ScoreView.SetGameScore(i,pUserData->szName,pGameEnd->lGameScore[i]);
		if(pGameEnd->lGameScore[i]>0)
			strTemp.Format("%s: %+ld\n",pUserData->szName,pGameEnd->lGameScore[i]);
		else
			strTemp.Format("%s: %ld\n",pUserData->szName,pGameEnd->lGameScore[i]);
		strEnd += strTemp;
	}

	//消息积分
	InsertSystemString((LPCTSTR)strEnd);
	m_GameClientView.m_ScoreView.ShowWindow(SW_SHOW);

	//设置扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		WORD wViewChairID=SwitchViewChairID(i);

#ifdef  VIDEO_GAME
		if(wViewChairID%2==0)
		{
			m_GameClientView.m_HandCardControl[wViewChairID].SetDisplayFlag(true);
			m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(pGameEnd->cbCardData[i],pGameEnd->cbCardCount[i]);

		}
#else
		m_GameClientView.m_HandCardControl[wViewChairID].SetDisplayFlag(true);
		m_GameClientView.m_HandCardControl[wViewChairID].SetCardData(pGameEnd->cbCardData[i],pGameEnd->cbCardCount[i]);

#endif	
	}

	//显示扑克
	if (IsLookonMode()==true) m_GameClientView.m_HandCardControl[2].SetDisplayFlag(true);

	//播放声音
	WORD wMeChairID=GetMeChairID();
	LONG lMeScore=pGameEnd->lGameScore[GetMeChairID()];
	if (lMeScore>0L) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WIN"));
	else if (lMeScore<0L) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_LOST"));
	else PlayGameSound(GetModuleHandle(NULL),TEXT("GAME_END"));

	//设置界面
	if (IsLookonMode()==false)
	{
		m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
		SetGameTimer(GetMeChairID(),IDI_START_GAME,30);
	}
	m_GameClientView.m_HandCardControl[2].SetPositively(false);

	//取消托管
	if(m_bStustee)
		OnStusteeControl(0,0);

	return true;
}

//出牌判断
bool CGameClientDlg::VerdictOutCard()
{
	//状态判断
	if (m_GameClientView.m_btOutCard.IsWindowVisible()==FALSE) return false;
	if (m_wCurrentUser!=GetMeChairID()) return false;


	//获取扑克
	BYTE cbCardData[MAX_COUNT];
	BYTE bShootCount=(BYTE)m_GameClientView.m_HandCardControl[2].GetShootCard(cbCardData,CountArray(cbCardData));

	//出牌判断
	if (bShootCount>0L)
	{
		//排列扑克
		m_GameLogic.SortCardList(cbCardData,bShootCount);

		BYTE cbStarLevel;
		//分析类型
		BYTE bCardType=m_GameLogic.GetCardType(cbCardData,bShootCount,cbStarLevel);

		//类型判断
		if (bCardType==CT_ERROR) return false;
	
		//跟牌判断
		if (m_cbTurnCardCount==0) return true;
		return m_GameLogic.CompareCard(m_cbTurnCardData,cbCardData,m_cbTurnCardCount,bShootCount);
	}

	return false;
}

//放弃判断
bool CGameClientDlg::VerdictPassCard()
{
	//首出判断
	if (m_cbTurnCardCount==0) return false;


	return true;
}

//自动出牌
bool CGameClientDlg::AutomatismOutCard()
{
	if ((IsLookonMode()==true)||(m_wCurrentUser!=GetMeChairID())) return false;

	//当前弹起
	CSkinButton & btOutCard=m_GameClientView.m_btOutCard;
	if ((btOutCard.IsWindowEnabled())&&(btOutCard.IsWindowVisible()))
	{
		OnOutCard(0,0);
		return true;
	}

	//出牌处理
	if ((m_cbTurnCardCount==0))
	{
		//分析结果
		tagOutCardResult OutCardResult;
		m_GameLogic.SearchOutCard(m_cbHandCardData,m_cbHandCardCount[GetMeChairID()],m_cbTurnCardData,m_cbTurnCardCount,OutCardResult);

		//设置界面
		if (OutCardResult.cbCardCount>0)
		{
			//设置界面
			m_GameClientView.m_btOutCard.EnableWindow(TRUE);
			m_GameClientView.m_HandCardControl[2].SetShootCard(OutCardResult.cbResultCard,OutCardResult.cbCardCount);

			//出牌动作
			OnOutCard(0,0);

			return true;
		}
	}

	//放弃出牌
	if (m_cbTurnCardCount>0) 
		OnPassCard(0,0);

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

		//扑克数据
		BYTE cbUserCard[MAX_COUNT];
		ZeroMemory(cbUserCard,sizeof(cbUserCard));

		//设置界面
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
#ifdef VIDEO_GAME
			if (i ==2)
			{
				m_GameClientView.m_HandCardControl[i].SetCardData(m_cbHandCardData,m_cbHandCardCount[i]);
			}
			else
			{
				if(i==0)
				{
					m_GameClientView.m_HandCardControl[i].SetCardData(cbUserCard,MAX_COUNT);
				}

			}
#else
			if (i ==2)
			{
				m_GameClientView.m_HandCardControl[i].SetCardData(m_cbHandCardData,m_cbHandCardCount[i]);
			}
			else
			{
				m_GameClientView.m_HandCardControl[i].SetCardData(cbUserCard,MAX_COUNT);

			}
#endif

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
	SetTimer(IDI_DISPATCH_CARD,100,NULL);

	return true;
}

//定时器消息
void CGameClientDlg::OnTimer(UINT nIDEvent)
{
	if ((nIDEvent==IDI_MOST_CARD))
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
			//m_GameClientView.m_btOutPrompt.ShowWindow(SW_SHOW);
			m_GameClientView.m_btPassCard.EnableWindow(FALSE);
			m_GameClientView.m_btOutCard.EnableWindow((VerdictOutCard()==true)?TRUE:FALSE);
			m_GameClientView.m_btPassCard.EnableWindow((VerdictPassCard()==true)?TRUE:FALSE);
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
			BYTE cbUserCard[MAX_COUNT];
			ZeroMemory(cbUserCard,sizeof(cbUserCard));

			//派发扑克
			m_cbRemnantCardCount--;
			m_cbDispatchCardCount++;
			m_GameLogic.SortCardList(m_cbDispatchCardData,m_cbDispatchCardCount);

			//设置界面
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
#ifdef  VIDEO_GAME
				if (i!=2)
				{
					if(i==0)
					{
						m_GameClientView.m_HandCardControl[i].SetCardData(cbUserCard,m_cbDispatchCardCount);
					}
				}
				else
					m_GameClientView.m_HandCardControl[i].SetCardData(m_cbDispatchCardData,m_cbDispatchCardCount); 

#else
				if (i!=2)
				{
					if(i==0)
					{
						m_GameClientView.m_HandCardControl[i].SetCardData(cbUserCard,m_cbDispatchCardCount);
					}
					else
					{
						m_GameClientView.m_HandCardControl[i].SetCardData(cbUserCard,m_cbDispatchCardCount);
					}
				}
				else
					m_GameClientView.m_HandCardControl[i].SetCardData(m_cbDispatchCardData,m_cbDispatchCardCount); 

#endif 
		}

			//播放声音
			PlayGameSound(AfxGetInstanceHandle(),TEXT("SEND_CARD"));
		}

		//中止判断
		if (m_cbRemnantCardCount==0)
		{
			//删除定时器
			KillTimer(IDI_DISPATCH_CARD);

			//扑克数目
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				m_GameClientView.SetCardCount(SwitchViewChairID(i),m_cbHandCardCount[i]);
			}

			//当前玩家
			if ((IsLookonMode()==false)&&(m_wCurrentUser==GetMeChairID()))
			{
				ActiveGameFrame();
				m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
				m_GameClientView.m_btOutCard.EnableWindow(FALSE);
				m_GameClientView.m_btPassCard.EnableWindow(FALSE);
				m_GameClientView.m_btPassCard.ShowWindow(SW_SHOW);
				//m_GameClientView.m_btOutPrompt.ShowWindow(SW_SHOW);
			}

			if(IsLookonMode()==false)
			{
				m_GameClientView.m_HandCardControl[2].SetPositively(IsLookonMode()==false);
				m_GameLogic.SortCardList(m_cbPartnerCardData,m_cbHandCardCount[(GetMeChairID()+2)%GAME_PLAYER]);
				m_GameClientView.m_HandCardControl[0].SetCardData(m_cbPartnerCardData,m_cbHandCardCount[(GetMeChairID()+2)%GAME_PLAYER]);
			}

			//播放剩余
			PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));

			//设置时间
			if (m_wCurrentUser!=INVALID_CHAIR)
				SetGameTimer(m_wCurrentUser,IDI_OUT_CARD,30);
		}

		return;
	}
	__super::OnTimer(nIDEvent);
}

//开始按钮
LRESULT CGameClientDlg::OnStart(WPARAM wParam, LPARAM lParam)
{   

	//胜利信息
	m_wWinCount=0;
	ZeroMemory(m_wWinOrder,sizeof(m_wWinOrder));

	//配置变量
	m_bDeasilOrder=false;
	m_dwCardHSpace=DEFAULT_PELS;

	//扑克变量
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));

	//出牌变量
	m_cbTurnCardCount=0;
	ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));

	//设置界面
	KillGameTimer(IDI_START_GAME);
	m_GameClientView.SetCardCount(INVALID_CHAIR,0);
	m_GameClientView.SetPassFlag(INVALID_CHAIR,false);
	m_GameClientView.SetUserWinOrder(INVALID_CHAIR,0);

	m_GameClientView.m_btStart.ShowWindow(SW_HIDE);

	//隐藏控件
	m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE);

	//设置扑克
	for (WORD i=0;i<GAME_PLAYER;i++) 
	{
		m_GameClientView.m_UserCardControl[i].SetCardData(NULL,0);
		m_GameClientView.m_HandCardControl[i].SetCardData(NULL,0);
	}
	m_GameClientView.m_HandCardControl[i].SetPositively(false);


	//发送消息
	SendUserReady(NULL,0);
	return 0;
}

//出牌消息
LRESULT CGameClientDlg::OnOutCard(WPARAM wParam, LPARAM lParam)
{
	//状态判断
	if (m_GameClientView.m_btOutCard.IsWindowVisible()==FALSE) return 0;
	if (m_GameClientView.m_btOutCard.IsWindowEnabled()==FALSE) return 0;

	//设置界面
	KillGameTimer(IDI_OUT_CARD);
	m_GameClientView.m_btOutCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btOutCard.EnableWindow(FALSE);
	m_GameClientView.m_btPassCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btPassCard.EnableWindow(FALSE);
	//m_GameClientView.m_btOutPrompt.ShowWindow(SW_HIDE);
	
	//获取扑克
	BYTE cbCardData[MAX_COUNT];
	BYTE cbCardCount=(BYTE)m_GameClientView.m_HandCardControl[2].GetShootCard(cbCardData,CountArray(cbCardData));

	//排列扑克
	m_GameLogic.SortCardList(cbCardData,cbCardCount,ST_ORDER);

	//变量定义
	WORD wMeChairID=GetMeChairID();
	BYTE cbSourceCount=m_cbHandCardCount[wMeChairID];
	
	BYTE cbStarLevel;
	//炸弹判断
	BYTE cbTurnCardType=m_GameLogic.GetCardType(cbCardData,cbCardCount,cbStarLevel);
	if (cbTurnCardType>=CT_BOMB)
	{
		PlayBombSound(GetMeChairID(),cbStarLevel);

	}
	else  
		PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
	m_GameClientView.m_UserCardControl[2].SetCardData(cbCardData,cbCardCount);


	//删除扑克
	m_cbHandCardCount[wMeChairID]-=cbCardCount;
	m_GameLogic.RemoveCard(cbCardData,cbCardCount,m_cbHandCardData,cbSourceCount);
	m_GameClientView.m_HandCardControl[2].SetCardData(m_cbHandCardData,m_cbHandCardCount[wMeChairID]);

	//发送数据
	CMD_C_OutCard OutCard;
	OutCard.cbCardCount=cbCardCount;
	CopyMemory(OutCard.cbCardData,cbCardData,cbCardCount);
	SendData(SUB_C_OUT_CARD,&OutCard,sizeof(OutCard)-sizeof(OutCard.cbCardData)+OutCard.cbCardCount*sizeof(BYTE));

	//更新界面
	m_GameClientView.UpdateGameView(NULL);
	return 0;
}

//放弃出牌
LRESULT CGameClientDlg::OnPassCard(WPARAM wParam, LPARAM lParam)
{
	//状态判断
	if (m_GameClientView.m_btPassCard.IsWindowVisible()==FALSE) return 0;
	if (m_GameClientView.m_btPassCard.IsWindowEnabled()==FALSE) return 0;

	//设置界面
	KillGameTimer(IDI_OUT_CARD);
	m_GameClientView.m_btOutCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btOutCard.EnableWindow(FALSE);
	m_GameClientView.m_btPassCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btPassCard.EnableWindow(FALSE);
	//m_GameClientView.m_btOutPrompt.ShowWindow(SW_HIDE);

	//发送数据
	SendData(SUB_C_PASS_CARD);

	//预先显示
	m_GameClientView.m_UserCardControl[2].SetCardData(NULL,0);
	m_GameClientView.SetPassFlag(2,true);
	PlayGameSound(AfxGetInstanceHandle(),TEXT("PASS_CARD"));

	return 0;
}

//出牌提示
LRESULT CGameClientDlg::OnOutPrompt(WPARAM wParam, LPARAM lParam)
{
	//分析结果
	tagOutCardResult OutCardResult;
	m_GameLogic.SearchOutCard(m_cbHandCardData,m_cbHandCardCount[GetMeChairID()],m_cbTurnCardData,m_cbTurnCardCount,OutCardResult);

	//设置界面
	if (OutCardResult.cbCardCount>0)
	{
		//设置界面
		m_GameClientView.m_HandCardControl[2].SetShootCard(OutCardResult.cbResultCard,OutCardResult.cbCardCount);

		//设置控件
		bool bOutCard=VerdictOutCard();
		m_GameClientView.m_btOutCard.EnableWindow((bOutCard==true)?TRUE:FALSE);

		//更新界面
		m_GameClientView.UpdateGameView(NULL);
	}
	else 
	{
		//放弃出牌
		OnPassCard(0,0);
	}
	return 0;
}

//花色排序
LRESULT CGameClientDlg::OnSortByColor(WPARAM wParam, LPARAM lParam)
{
	return 0;
}
//大小排序
LRESULT CGameClientDlg::OnSortByValue(WPARAM wParam, LPARAM lParam)
{
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
//拖管控制
LRESULT CGameClientDlg::OnStusteeControl(WPARAM wParam, LPARAM lParam)
{
	//设置变量
	m_wTimeOutCount=0;

	//设置状态
	if (m_bStustee==true)
	{
		m_bStustee=false;
		m_GameClientView.m_btStusteeControl.SetButtonImage(IDB_BT_START_TRUSTEE,AfxGetInstanceHandle(),false);
		InsertSystemString(_T("您取消了托管功能."));
		CMD_C_Trustee Trustee;
		Trustee.bTrustee = false;
		SendData(SUB_C_TRUSTEE,&Trustee,sizeof(Trustee));

	}
	else
	{
		m_bStustee=true;
		m_GameClientView.m_btStusteeControl.SetButtonImage(IDB_BT_STOP_TRUSTEE,AfxGetInstanceHandle(),false);
		InsertSystemString(_T("您选择了托管功能."));
		CMD_C_Trustee Trustee;
		Trustee.bTrustee = true;
		SendData(SUB_C_TRUSTEE,&Trustee,sizeof(Trustee));
	}

	return 0;
}


//播放出牌声音
void CGameClientDlg::PlayBombSound(WORD wChairID, BYTE cbStarLevel)
{
	if(wChairID < 0 || wChairID > 3)
	{
		return;
	}
	if(cbStarLevel<4||cbStarLevel>12)
	{
		return;
	}

	//判断性别
	const tagUserData* pUserData = GetUserData(wChairID);
	if(pUserData == 0)
	{
		assert(0 && "得不到玩家信息");
		return;
	}
	bool bBoy = (pUserData->cbGender == 1 ? false : true);
	CString strSoundName;
	strSoundName.Format(_T("%d"),cbStarLevel);
	if(bBoy)
	{
		strSoundName = _T("BOY_") +strSoundName;
	}
	else
	{
		strSoundName = _T("GIRL_") + strSoundName;
	}
	PlayGameSound(AfxGetInstanceHandle(), strSoundName);
}

//用户托管
bool CGameClientDlg::OnSubTrustee(const void * pBuffer,WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_Trustee));
	if (wDataSize!=sizeof(CMD_S_Trustee)) return false;

	//消息处理
	CMD_S_Trustee * pTrustee=(CMD_S_Trustee *)pBuffer;
	m_GameClientView.SetTrustee(SwitchViewChairID(pTrustee->wChairID),pTrustee->bTrustee);
	if ((IsLookonMode()==true)||(pTrustee->wChairID!=GetMeChairID()))
	{
		const tagUserData * pUserData=GetUserData(pTrustee->wChairID);
		TCHAR szBuffer[256];
		if(pTrustee->bTrustee==true)
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("玩家[%s]选择了托管功能."),pUserData->szName);
		else
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("玩家[%s]取消了托管功能."),pUserData->szName);
		InsertSystemString(szBuffer);
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////
