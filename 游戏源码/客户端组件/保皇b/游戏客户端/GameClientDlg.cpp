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

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientDlg, CGameFrameDlg)
	ON_WM_TIMER()
	ON_MESSAGE(IDM_START,OnStart)
	ON_MESSAGE(IDM_HOUSECARL,OnHousecarl)
	ON_MESSAGE(IDM_EMPEROR,OnEmperor)
	ON_MESSAGE(IDM_OUT_CARD,OnOutCard)
	ON_MESSAGE(IDM_PASS_CARD,OnPassCard)
	ON_MESSAGE(IDM_AUTO_OUTCARD,OnAutoOutCard)
	ON_MESSAGE(IDM_LEFT_HIT_CARD,OnLeftHitCard)
	ON_MESSAGE(IDM_RIGHT_HIT_CARD,OnRightHitCard)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientDlg::CGameClientDlg() : CGameFrameDlg(&m_GameClientView)
{
	//游戏变量
	
	m_bHandCardCount=0;
	m_bHintCardCount=0;
	memset(m_bCardCount,0,sizeof(m_bCardCount));
	memset(m_bHandCardData,0,sizeof(m_bHandCardData));

	//配置变量
	m_bDeasilOrder=false;
	m_dwCardHSpace=DEFAULT_PELS;

	//出牌变量
	m_bTurnCardCount=0;
	m_bTurnOutType=CT_INVALID;
	memset(m_bTurnCardData,0,sizeof(m_bTurnCardData));
	memset(m_bHintCardData,0,sizeof(m_bHintCardData));

	//辅助变量
	m_wTimeOutCount=0;
	m_wMostUser=INVALID_CHAIR;

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
	SetWindowText(TEXT("保皇 快速场(无3,4,5)"));

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
	m_GameClientView.m_HandCardControl.SetCardSpace(m_dwCardHSpace,0,20);

#ifdef DEBUG_GAME

	BYTE bData[20]={67,1,2,3,4,20,7,27,39,8,24,40,9,25,10,26,11,27,43,29};

	m_bHandCardCount=20;

	m_GameLogic.SortCardList(bData,m_bHandCardCount);
	CopyMemory(m_bHandCardData,bData,m_bHandCardCount);
	m_GameClientView.m_HandCardControl.SetCardData(m_bHandCardData,m_bHandCardCount);
	BYTE bOutData[2]={6,22};
	m_bTurnCardCount=sizeof(bOutData);
	CopyMemory(m_bTurnCardData,bOutData,m_bTurnCardCount);
	m_bTurnOutType=m_GameLogic.GetCardType(m_bTurnCardData,m_bTurnCardCount);
	//char *str=new char[100];
	//sprintf(str,"%d",m_bTurnOutType);
	//MessageBox(str);
	//delete []str;
	//m_bTurn
	ActiveGameFrame();
	m_GameClientView.m_btOutCard.EnableWindow(FALSE);
	m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
	m_GameClientView.m_btPassCard.EnableWindow(FALSE);
	m_GameClientView.m_btPassCard.ShowWindow(SW_SHOW);
	m_GameClientView.m_btAutoOutCard.ShowWindow(SW_SHOW);
	m_GameClientView.m_btAutoOutCard.EnableWindow(TRUE);	
	m_GameClientView.m_HandCardControl.SetPositively(true);
	m_GameClientView.m_HandCardControl.SetDisplayFlag(true);
#endif
	return true;
}

//重置框架
void CGameClientDlg::ResetGameFrame()
{
	//游戏变量
	
	m_bHandCardCount=0;
	m_bHintCardCount=0;
	memset(m_bCardCount,0,sizeof(m_bCardCount));
	memset(m_bHandCardData,0,sizeof(m_bHandCardData));

	//出牌变量
	m_bTurnCardCount=0;
	m_bTurnOutType=CT_INVALID;
	memset(m_bTurnCardData,0,sizeof(m_bTurnCardData));
	memset(m_bHandCardData,0,sizeof(m_bHintCardData));

	//辅助变量
	m_wTimeOutCount=0;
	m_wMostUser=INVALID_CHAIR;

	//删除定时
	KillGameTimer(0);
	KillTimer(IDI_MOST_CARD);

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
		m_GameClientView.m_HandCardControl.SetCardSpace(m_dwCardHSpace,0,20);

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
			//超时判断
			if (nElapse==0)
			{
				if ((IsLookonMode()==false)&&(wChairID==GetMeChairID())) AutomatismOutCard();
				return false;
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
				if ((IsLookonMode()==false)&&(wChairID==GetMeChairID())) OnStart(0,0);
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
			m_GameClientView.m_btStart.ShowWindow( SW_SHOW);

			//设置控件
			if (IsLookonMode()==false)
			{
				m_GameClientView.m_btStart.ShowWindow(TRUE);
				m_GameClientView.m_btStart.SetFocus();
			}

			//设置扑克
			if (IsLookonMode()==false) m_GameClientView.m_HandCardControl.SetDisplayFlag(true);

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
			m_GameClientView.m_HandCardControl.SetCardData(m_bHandCardData,m_bHandCardCount);
			
			//玩家设置
			if ((IsLookonMode()==false)&&(pStatusPlay->wCurrentUser==GetMeChairID()))
			{
				m_GameClientView.m_btOutCard.EnableWindow(FALSE);
				m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
				m_GameClientView.m_btPassCard.ShowWindow(SW_SHOW);
				m_GameClientView.m_btAutoOutCard.ShowWindow(SW_SHOW);
				m_GameClientView.m_btPassCard.EnableWindow((m_bTurnCardCount!=0)?TRUE:FALSE);
				m_GameClientView.m_btAutoOutCard.EnableWindow((m_bTurnCardCount!=0)?TRUE:FALSE);
			}

			//桌面设置
			if (m_bTurnCardCount!=0)
			{
				WORD wViewChairID=SwitchViewChairID(pStatusPlay->wLastOutUser);
				m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(m_bTurnCardData,m_bTurnCardCount);
			}

			//设置定时器
			SetGameTimer(pStatusPlay->wCurrentUser,IDI_OUT_CARD,30);

			//设置扑克
			if (IsLookonMode()==false) 
			{
				m_GameClientView.m_HandCardControl.SetPositively(true);
				m_GameClientView.m_HandCardControl.SetDisplayFlag(true);
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
	ASSERT(wDataSize==sizeof(CMD_S_SendCard));
	if (wDataSize!=sizeof(CMD_S_SendCard)) return false;

	//变量定义
	CMD_S_SendCard * pSendCard=(CMD_S_SendCard *)pBuffer;

	//设置数据
	m_bHandCardCount=CountArray(pSendCard->bCardData);
	CopyMemory(m_bHandCardData,pSendCard->bCardData,sizeof(pSendCard->bCardData));
	for (WORD i=0;i<GAME_PLAYER;i++) m_bCardCount[i]=CountArray(pSendCard->bCardData);

	//设置界面
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameClientView.SetPassFlag(i,false);
		m_GameClientView.SetCardCount(i,m_bCardCount[i]);
		m_GameClientView.m_UserCardControl[i].SetCardData(NULL,0);
	}
	if (IsLookonMode()==true)
	{
		m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE);
	
	}
	if (IsLookonMode()==true) m_GameClientView.m_HandCardControl.SetDisplayFlag(false);

	//当前玩家
	if ((IsLookonMode()==false))//&&(pSendCard->wCurrentUser==GetMeChairID()))
	{
		ActiveGameFrame();
		m_GameClientView.m_HandCardControl.SetCardData(m_bHandCardData,m_bHandCardCount);
		if (pSendCard->wCurrentEnthronement == GetMeChairID())//皇帝
		{
			if (pSendCard->wCurrentHousecarl  == GetMeChairID())//侍卫
			{
				CMD_C_ChairID ChairID;
				ChairID.IsChair = pSendCard->wCurrentUser;
				ChairID.IsGet = 0;
				SendData(SUB_C_CHAIRID,&ChairID,sizeof(ChairID));
				m_GameClientView.m_wCurrentHousecarl = pSendCard->wCurrentHousecarl;				
			}
			else
			{
				CMD_C_ChairID ChairID;
				ChairID.IsChair = pSendCard->wCurrentUser;
				ChairID.IsGet = 1;
				SendData(SUB_C_CHAIRID,&ChairID,sizeof(ChairID));
				m_GameClientView.m_wCurrentEnthronement = pSendCard->wCurrentEnthronement;		
			}
						
		}
		else if (pSendCard->wCurrentHousecarl == GetMeChairID())//侍卫
		{
			CMD_C_ChairID ChairID;
			ChairID.IsChair = pSendCard->wCurrentUser;
			ChairID.IsGet = 0;
			SendData(SUB_C_CHAIRID,&ChairID,sizeof(ChairID));
			m_GameClientView.m_wCurrentHousecarl = pSendCard->wCurrentHousecarl;		
			
		}
		
	}
	m_GameClientView.m_wCurrentEnthronement = SwitchViewChairID(pSendCard->wCurrentEnthronement);	
	m_GameClientView.m_wCurrentHousecarl = SwitchViewChairID(pSendCard->wCurrentHousecarl);		
	//播放声音
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));

	return OnSubGameStart(pBuffer,wDataSize);

	return true;
}
//游戏开始
bool CGameClientDlg::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_SendCard));
	if (wDataSize!=sizeof(CMD_S_SendCard)) return false;

	//消息处理
	CMD_S_SendCard * pGameStart=(CMD_S_SendCard *)pBuffer;

	//设置变量

	m_bTurnCardCount=0;
	m_bTurnOutType=CT_INVALID;
	ZeroMemory(m_bTurnCardData,sizeof(m_bTurnCardData));

	//设置界面
	m_GameClientView.SetCardCount(SwitchViewChairID(pGameStart->wCurrentEnthronement),m_bCardCount[pGameStart->wCurrentEnthronement]);

	////地主设置
	//if (pGameStart->wCurrentEnthronement==GetMeChairID())
	//{
	//	m_GameClientView.m_HandCardControl.SetCardData(m_bHandCardData,m_bHandCardCount);
	//}

	//玩家设置
	if (IsLookonMode()==false) m_GameClientView.m_HandCardControl.SetPositively(true);

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
//	m_GameClientView.m_LeaveCardControl[0].SetBackCard(m_bCardCount[0]);//.SetCardData(m_GameClientView.m_byZerodata,m_bCardCount[0]);
//	m_GameClientView.m_LeaveCardControl[1].SetBackCard(m_bCardCount[1]);//.SetCardData(m_GameClientView.m_byZerodata,m_bCardCount[1]);
	//播放声音
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));

	//设置时间
	SetGameTimer(pGameStart->wCurrentUser,IDI_OUT_CARD,30);
	m_GameClientView.m_bGamePlaying = true;
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
	for (BYTE i = 0; i < pOutCard->bCardCount; i++)
	{
		if (pOutCard->bCardData[i] == 0x43)
			m_GameClientView.m_bOutHousecarl = true;
	}
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

	//自己扑克
	//if ((IsLookonMode()==true)&&(pOutCard->wOutCardUser==GetMeChairID()))
	//{
	//	//删除扑克 
	//	BYTE bSourceCount=m_bHandCardCount;
	//	m_bHandCardCount-=pOutCard->bCardCount;
	//	m_GameLogic.RemoveCard(pOutCard->bCardData,pOutCard->bCardCount,m_bHandCardData,bSourceCount);

	//	//设置界面
	//	m_GameClientView.m_HandCardControl.SetCardData(m_bHandCardData,m_bHandCardCount);
	//}

	//最大判断
	if (pOutCard->wCurrentUser==pOutCard->wOutCardUser)
	{
		//设置变量
		m_bTurnCardCount=0;
		m_bTurnOutType=CT_INVALID;
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
			else
			{
				if (m_bCardCount[i] <= 0)
				{
					BYTE a = (i+1)%GAME_PLAYER;
					while(1)
					{
						if (a < GAME_PLAYER && m_bCardCount[a]>0)
						{	
							m_wMostUser=pOutCard->wCurrentUser=a;
							break;
						}
						else
							a = (a++)%GAME_PLAYER;
					}
				}
					
			}
		}
		

		//播放声音
		//PlayGameSound(AfxGetInstanceHandle(),TEXT("MOST_CARD"));

		//设置定时器
		SetTimer(IDI_MOST_CARD,3000,NULL);

		return true;
	}

	//播放声音
	if ((IsLookonMode()==true)||(GetMeChairID()!=pOutCard->wOutCardUser)) PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));

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
		m_bTurnOutType=CT_INVALID;
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
		m_GameClientView.m_btAutoOutCard.EnableWindow((m_bTurnCardCount>0)?TRUE:FALSE);
	}

	//播放声音
	if ((IsLookonMode()==true)||(pPassCard->wPassUser!=GetMeChairID()))	PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));

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
	
	//隐藏控件
	m_GameClientView.m_btOutCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btPassCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btAutoOutCard.ShowWindow(SW_HIDE);

	//禁用控件
	m_GameClientView.m_btOutCard.EnableWindow(FALSE);
	m_GameClientView.m_btPassCard.EnableWindow(FALSE);

	//设置积分
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		const tagUserData * pUserData=GetUserData(i);
		m_GameClientView.m_ScoreView.SetGameScore(i,pUserData->szName,pGameEnd->lGameScore[i]);
	}
	//m_GameClientView.m_LeaveCardControl[0].SetDisplayFlag(true);
	//m_GameClientView.m_LeaveCardControl[1].SetDisplayFlag(true);

	m_GameClientView.m_ScoreView.SetGameTax(pGameEnd->lGameTax);
	m_GameClientView.m_ScoreView.ShowWindow(SW_SHOW);

	//设置扑克
	BYTE bCardPos=0;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		WORD wViewChairID=SwitchViewChairID(i);
		
		bCardPos+=pGameEnd->bCardCount[i];
		if (pGameEnd->bCardCount[i]!=0)
		{
			m_GameClientView.SetPassFlag(wViewChairID,false);
			m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);
		}
	}

	//显示扑克
	if (IsLookonMode()==true) m_GameClientView.m_HandCardControl.SetDisplayFlag(true);

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
	m_GameClientView.m_wCurrentEnthronement = -1;				
	m_GameClientView.m_wCurrentHousecarl = -1;		
	m_GameClientView.m_bGamePlaying = false;
	m_GameClientView.m_bOutHousecarl = false;
	return true;
}

//出牌判断
bool CGameClientDlg::VerdictOutCard()
{
	//状态判断
	if (m_GameClientView.m_btOutCard.IsWindowVisible()==FALSE) return false;

	//获取扑克
	BYTE bCardData[PLAYER_CARDS];
	BYTE bShootCount=(BYTE)m_GameClientView.m_HandCardControl.GetShootCard(bCardData,CountArray(bCardData));

	//出牌判断
	if (bShootCount>0L)
	{
		//分析类型
		BYTE bCardType=m_GameLogic.GetCardType(bCardData,bShootCount);

		//类型判断
		if (bCardType==CT_INVALID) return false;

		//跟牌判断
		if (m_bTurnCardCount==0) return true;
		return m_GameLogic.CompareCard(bCardData,m_bTurnCardData,bShootCount,m_bTurnCardCount);
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
		m_GameClientView.m_UserCardControl[2].SetCardData(OutCard.bCardData,OutCard.bCardCount);

		//预先删除
		BYTE bSourceCount=m_bHandCardCount;
		m_bHandCardCount-=OutCard.bCardCount;
		m_GameLogic.RemoveCard(OutCard.bCardData,OutCard.bCardCount,m_bHandCardData,bSourceCount);
		m_GameClientView.m_HandCardControl.SetCardData(m_bHandCardData,m_bHandCardCount);
	}
	else //OnPassCard(0,0);
		AutoOutCard111(0);

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

	__super::OnTimer(nIDEvent);
}

//开始按钮
LRESULT CGameClientDlg::OnStart(WPARAM wParam, LPARAM lParam)
{
	//设置变量
	m_wTimeOutCount=0;
	m_bHandCardCount=0;
	m_bTurnCardCount=0;
	m_bTurnOutType=CT_INVALID;
	m_wMostUser=INVALID_CHAIR;
	memset(m_bHandCardData,0,sizeof(m_bHandCardData));
	memset(m_bTurnCardData,0,sizeof(m_bTurnCardData));

	//设置界面
	KillGameTimer(IDI_START_GAME);
	
	m_GameClientView.SetCardCount(INVALID_CHAIR,0);
	m_GameClientView.SetPassFlag(INVALID_CHAIR,false);

	//隐藏控件
	m_GameClientView.m_btStart.ShowWindow(FALSE);
	m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE);

	//设置扑克
	m_GameClientView.m_HandCardControl.SetCardData(NULL,0);
	m_GameClientView.m_HandCardControl.SetPositively(false);
//	m_GameClientView.m_LeaveCardControl[0].SetCardData(NULL,0);
//	m_GameClientView.m_LeaveCardControl[1].SetCardData(NULL,0);
	for (WORD i=0;i<GAME_PLAYER;i++) m_GameClientView.m_UserCardControl[i].SetCardData(NULL,0);

	//发送消息
	SendUserReady(NULL,0);
	PlayGameSound(AfxGetInstanceHandle(),TEXT("Ready"));
	return 0;
}

//出牌消息
LRESULT CGameClientDlg::OnOutCard(WPARAM wParam, LPARAM lParam)
{
	//状态判断
	if ((m_GameClientView.m_btOutCard.IsWindowEnabled()==FALSE)||
		(m_GameClientView.m_btOutCard.IsWindowVisible()==FALSE)) return 0;

	memset(m_bHintCardData,0,sizeof(m_bHintCardData));
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
	OutCard.bCardCount=(BYTE)m_GameClientView.m_HandCardControl.GetShootCard(OutCard.bCardData,CountArray(OutCard.bCardData));
	SendData(SUB_C_OUT_CART,&OutCard,sizeof(OutCard)-sizeof(OutCard.bCardData)+OutCard.bCardCount*sizeof(BYTE));

	//预先显示
	PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
	m_GameClientView.m_UserCardControl[2].SetCardData(OutCard.bCardData,OutCard.bCardCount);

	//预先删除
	BYTE bSourceCount=m_bHandCardCount;
	m_bHandCardCount-=OutCard.bCardCount;
	m_GameLogic.RemoveCard(OutCard.bCardData,OutCard.bCardCount,m_bHandCardData,bSourceCount);
	m_GameClientView.m_HandCardControl.SetCardData(m_bHandCardData,m_bHandCardCount);

	return 0;
}

//放弃出牌
LRESULT CGameClientDlg::OnPassCard(WPARAM wParam, LPARAM lParam)
{
	//状态判断
	if (m_GameClientView.m_btPassCard.IsWindowEnabled()==FALSE) return 0;

	memset(m_bHintCardData,0,sizeof(m_bHintCardData));
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
	PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
	m_GameClientView.m_HandCardControl.SetCardData(m_bHandCardData,m_bHandCardCount);

	return 0;
}

//出牌提示
LRESULT CGameClientDlg::OnAutoOutCard(WPARAM wParam, LPARAM lParam)
{
	AutoOutCard(0);
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

//即位
LRESULT CGameClientDlg::OnEmperor(WPARAM wParam, LPARAM lParam)
{
	CMD_C_ChairID ChairID;
	ChairID.IsChair = (BYTE)wParam;
	ChairID.IsGet = (BYTE)lParam;
	SendData(SUB_C_CHAIRID,&ChairID,sizeof(ChairID));
	return 0;
}
//侍卫
LRESULT CGameClientDlg::OnHousecarl(WPARAM wParam, LPARAM lParam)
{
	CMD_C_ChairID ChairID;
	ChairID.IsChair = (BYTE)wParam;
	ChairID.IsGet = (BYTE)lParam;
	SendData(SUB_C_CHAIRID,&ChairID,sizeof(ChairID));
	return 0;
}
//左键扑克
LRESULT CGameClientDlg::OnRightHitCard(WPARAM wParam, LPARAM lParam)
{
	//用户出牌
	OnOutCard(0,0);

	return 0;
}

void CGameClientDlg::AutoOutCard(BYTE WhichOnsKindCard)
{
	if(m_bHintCardData[0]==0)
	{
		m_bHintCardCount=m_bTurnCardCount;
		CopyMemory(m_bHintCardData,m_bTurnCardData,m_bTurnCardCount);
	}
	//如果没有人出牌，不提示
	if(m_bTurnCardCount==0)
		return;

	BYTE							bTempSCardCount = 0;						//扑克数目
	BYTE							bTempSCardData[PLAYER_CARDS];				//手上扑克
	BYTE							bTempCCardData[PLAYER_CARDS];				//手上扑克
	BYTE							TurnCard = 0;
	BYTE							TempCard = 0;
	BYTE							DXCard = 0;
	memset(bTempSCardData,0,sizeof(bTempSCardData));
	memset(bTempCCardData,0,sizeof(bTempCCardData));
	TurnCard = m_GameLogic.GetCardLogicValue(m_bTurnCardData[0]);
	TempCard = m_GameLogic.GetCardLogicValue(m_bHandCardData[m_bHandCardCount-1]);
	m_GameClientView.m_HandCardControl.ShootAllCard(false);

	for(BYTE i=m_bHandCardCount-1;i>0;i--)
	{	
		BYTE GetCard=m_GameLogic.GetCardLogicValue(m_bHandCardData[i]);
		BYTE GetCardCount = 0;
		
		if(TurnCard < GetCard)
		{
			if (TempCard == GetCard)
			{
				bTempSCardData[bTempSCardCount]=m_bHandCardData[i];
				bTempCCardData[bTempSCardCount]=i;
				bTempSCardCount++;
				if (bTempSCardCount>0 && bTempSCardCount == m_bTurnCardCount)
				{
					m_bHintCardCount = bTempSCardCount;
					memset(m_bHintCardData,0,sizeof(m_bHintCardData));
					CopyMemory(m_bHintCardData,bTempSCardData,m_bHintCardCount);
					m_GameClientView.m_HandCardControl.SetShootCard(bTempCCardData,m_bHintCardCount);
					m_GameClientView.m_btOutCard.EnableWindow(TRUE);
					return;
				}
			}
			else
			{
				if (i>0) i++;
				TempCard=GetCard;
				memset(bTempSCardData,0,sizeof(bTempSCardData));
				memset(bTempCCardData,0,sizeof(bTempCCardData));
				bTempSCardCount = 0;
			}
		}
	}
	if (m_bTurnCardCount == 1)
	{
		TurnCard = m_GameLogic.GetCardLogicValue(m_bTurnCardData[0]);
		TempCard = m_GameLogic.GetCardLogicValue(m_bHandCardData[0]);
		if(TurnCard < TempCard)
		{
			bTempSCardData[0]=m_bHandCardData[0];
			bTempCCardData[0]=0;
			m_bHintCardCount = 1;
			memset(m_bHintCardData,0,sizeof(m_bHintCardData));
			CopyMemory(m_bHintCardData,bTempSCardData,m_bHintCardCount);
			m_GameClientView.m_HandCardControl.SetShootCard(bTempCCardData,m_bHintCardCount);
			m_GameClientView.m_btOutCard.EnableWindow(TRUE);
			return;
		}
	}
	//return;
	memset(bTempSCardData,0,sizeof(bTempSCardData));
	memset(bTempCCardData,0,sizeof(bTempCCardData));
	TurnCard = m_GameLogic.GetCardLogicValue(m_bTurnCardData[0]);
	TempCard = m_GameLogic.GetCardLogicValue(m_bHandCardData[m_bHandCardCount-1]);
	m_GameClientView.m_HandCardControl.ShootAllCard(false);

	for (BYTE i = 0; i < 8; i++)
	{
		if (m_GameLogic.GetCardLogicValue(m_bHandCardData[i]) > 15)
			DXCard++;
		else
			break;
	}
	if (DXCard <= 0) return;
	for(BYTE i=m_bHandCardCount-1;i>0;i--)
	{	
		BYTE GetCard=m_GameLogic.GetCardLogicValue(m_bHandCardData[i]);
		BYTE GetCardCount = 0;
		
		if(TurnCard < GetCard)
		{
			if (TempCard == GetCard)
			{
				bTempSCardData[bTempSCardCount]=m_bHandCardData[i];
				bTempCCardData[bTempSCardCount]=i;
				bTempSCardCount++;
				
			}
			else
			{
				if (bTempSCardCount > 0)
				{
					BYTE b = m_bTurnCardCount - bTempSCardCount;
					if (b > DXCard) break;
					if (DXCard > 0)
					{
						for (BYTE c = DXCard-1; c > 0; c--)
						{
							BYTE CardValue=m_GameLogic.GetCardLogicValue(m_bHandCardData[c]);
							if (CardValue > 15)
							{
								bTempSCardData[bTempSCardCount]=m_bHandCardData[c];
								bTempCCardData[bTempSCardCount]=c;
								bTempSCardCount++;
								if (bTempSCardCount>0 && bTempSCardCount == m_bTurnCardCount)
								{
									m_bHintCardCount = bTempSCardCount;
									m_GameLogic.SortCardList(bTempSCardData,bTempSCardCount);
									CopyMemory(m_bHintCardData,bTempSCardData,m_bHintCardCount);
									m_GameClientView.m_HandCardControl.SetShootCard(bTempCCardData,m_bHintCardCount);
									m_GameClientView.m_btOutCard.EnableWindow(TRUE);
									return;
								}
							}
						}
					}
				}
				//i++;
				TempCard=GetCard;
				memset(bTempSCardData,0,sizeof(bTempSCardData));
				memset(bTempCCardData,0,sizeof(bTempCCardData));
				bTempSCardCount = 0;
			}
		}
	}

}

void CGameClientDlg::AutoOutCard111(BYTE WhichOnsKindCard)
{
	if(m_bHintCardData[0]==0)
	{
		m_bHintCardCount=m_bTurnCardCount;
		CopyMemory(m_bHintCardData,m_bTurnCardData,m_bTurnCardCount);
	}
	//如果没有人出牌，不提示
	if(m_bTurnCardCount==0)
		return;

	BYTE							bTempSCardCount = 0;							//扑克数目
	BYTE							bTempSCardData[PLAYER_CARDS];				//手上扑克
	BYTE							bTempCCardData[PLAYER_CARDS];				//手上扑克
	BYTE							TurnCard = 0;
	BYTE							TempCard = 0;
	BYTE							DXCard = 0;
	memset(bTempSCardData,0,sizeof(bTempSCardData));
	memset(bTempCCardData,0,sizeof(bTempCCardData));
	TurnCard = m_GameLogic.GetCardLogicValue(m_bTurnCardData[0]);
	TempCard = m_GameLogic.GetCardLogicValue(m_bHandCardData[m_bHandCardCount-1]);
	m_GameClientView.m_HandCardControl.ShootAllCard(false);

	for(BYTE i=m_bHandCardCount-1;i>0;i--)
	{	
		BYTE GetCard=m_GameLogic.GetCardLogicValue(m_bHandCardData[i]);
		BYTE GetCardCount = 0;
		
		if(TurnCard < GetCard)
		{
			if (TempCard == GetCard)
			{
				bTempSCardData[bTempSCardCount]=m_bHandCardData[i];
				bTempCCardData[bTempSCardCount]=i;
				bTempSCardCount++;
				if (bTempSCardCount  > 0 && bTempSCardCount == m_bTurnCardCount)
				{
					m_bHintCardCount = bTempSCardCount;
					memset(m_bHintCardData,0,sizeof(m_bHintCardData));
					CopyMemory(m_bHintCardData,bTempSCardData,m_bHintCardCount);
					//发送数据
					CMD_C_OutCard OutCard;
					OutCard.bCardCount=bTempSCardCount;
					CopyMemory(OutCard.bCardData,bTempSCardData,bTempSCardCount);
					SendData(SUB_C_OUT_CART,&OutCard,sizeof(OutCard)-sizeof(OutCard.bCardData)+OutCard.bCardCount*sizeof(BYTE));

					//预先处理
					PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
					m_GameClientView.m_UserCardControl[2].SetCardData(OutCard.bCardData,OutCard.bCardCount);

					//预先删除
					BYTE bSourceCount=m_bHandCardCount;
					m_bHandCardCount-=OutCard.bCardCount;
					m_GameLogic.RemoveCard(OutCard.bCardData,OutCard.bCardCount,m_bHandCardData,bSourceCount);
					m_GameClientView.m_HandCardControl.SetCardData(m_bHandCardData,m_bHandCardCount);

					m_GameClientView.m_btOutCard.ShowWindow(SW_HIDE);
					m_GameClientView.m_btPassCard.ShowWindow(SW_HIDE);
					m_GameClientView.m_btAutoOutCard.ShowWindow(SW_HIDE);
					////////
					//m_GameClientView.m_HandCardControl.SetShootCard(bTempCCardData,m_bHintCardCount);
					//m_GameClientView.m_btOutCard.EnableWindow(TRUE);
					return;
				}
			}
			else
			{
				if (i>0) i++;
				TempCard=GetCard;
				memset(bTempSCardData,0,sizeof(bTempSCardData));
				memset(bTempCCardData,0,sizeof(bTempCCardData));
				bTempSCardCount = 0;
			}
		}
	}
	if (m_bTurnCardCount == 1)
	{
		TurnCard = m_GameLogic.GetCardLogicValue(m_bTurnCardData[0]);
		TempCard = m_GameLogic.GetCardLogicValue(m_bHandCardData[0]);
		if(TurnCard < TempCard)
		{
			bTempSCardData[0]=m_bHandCardData[0];
			bTempCCardData[0]=0;
			m_bHintCardCount = 1;
			memset(m_bHintCardData,0,sizeof(m_bHintCardData));
			CopyMemory(m_bHintCardData,bTempSCardData,m_bHintCardCount);
			m_GameClientView.m_HandCardControl.SetShootCard(bTempCCardData,m_bHintCardCount);
			m_GameClientView.m_btOutCard.EnableWindow(TRUE);
			return;
		}
	}
	// OnPassCard(0,0);
	// return;
	memset(bTempSCardData,0,sizeof(bTempSCardData));
	memset(bTempCCardData,0,sizeof(bTempCCardData));
	TurnCard = m_GameLogic.GetCardLogicValue(m_bTurnCardData[0]);
	TempCard = m_GameLogic.GetCardLogicValue(m_bHandCardData[m_bHandCardCount-1]);
	m_GameClientView.m_HandCardControl.ShootAllCard(false);

	for (BYTE i = 0; i < 8; i++)
	{
		if (m_GameLogic.GetCardLogicValue(m_bHandCardData[i]) > 15)
			DXCard++;
		else
			break;
	}
	if (DXCard <= 0) OnPassCard(0,0);
	for(BYTE i=m_bHandCardCount-1;i>0;i--)
	{	
		BYTE GetCard=m_GameLogic.GetCardLogicValue(m_bHandCardData[i]);
		BYTE GetCardCount = 0;
		
		if(TurnCard < GetCard)
		{
			if (TempCard == GetCard)
			{
				bTempSCardData[bTempSCardCount]=m_bHandCardData[i];
				bTempCCardData[bTempSCardCount]=i;
				bTempSCardCount++;
				
			}
			else
			{
				if (bTempSCardCount > 0)
				{
					BYTE b = m_bTurnCardCount - bTempSCardCount;
					if (b > DXCard) break;
					if (DXCard > 0)
					{
						for (BYTE c = DXCard-1; c > 0; c--)
						{
							BYTE CardValue=m_GameLogic.GetCardLogicValue(m_bHandCardData[c]);
							if (CardValue > 15)
							{
								bTempSCardData[bTempSCardCount]=m_bHandCardData[c];
								bTempCCardData[bTempSCardCount]=c;
								bTempSCardCount++;
								if (bTempSCardCount  > 0 && bTempSCardCount == m_bTurnCardCount)
								{
									m_bHintCardCount = bTempSCardCount;
									CopyMemory(m_bHintCardData,bTempSCardData,m_bHintCardCount);
									//发送数据
									CMD_C_OutCard OutCard;
									OutCard.bCardCount=bTempSCardCount;
									CopyMemory(OutCard.bCardData,bTempSCardData,bTempSCardCount);
									SendData(SUB_C_OUT_CART,&OutCard,sizeof(OutCard)-sizeof(OutCard.bCardData)+OutCard.bCardCount*sizeof(BYTE));

									//预先处理
									PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
									m_GameClientView.m_UserCardControl[2].SetCardData(OutCard.bCardData,OutCard.bCardCount);

									//预先删除
									BYTE bSourceCount=m_bHandCardCount;
									m_bHandCardCount-=OutCard.bCardCount;
									m_GameLogic.RemoveCard(OutCard.bCardData,OutCard.bCardCount,m_bHandCardData,bSourceCount);
									m_GameClientView.m_HandCardControl.SetCardData(m_bHandCardData,m_bHandCardCount);
									
									m_GameClientView.m_btOutCard.ShowWindow(SW_HIDE);
									m_GameClientView.m_btPassCard.ShowWindow(SW_HIDE);
									m_GameClientView.m_btAutoOutCard.ShowWindow(SW_HIDE);
									return;
								}
							}
						}
					}
				}
				//i++;
				TempCard=GetCard;
				memset(bTempSCardData,0,sizeof(bTempSCardData));
				memset(bTempCCardData,0,sizeof(bTempCCardData));
				bTempSCardCount = 0;
			}
		}
	}
	OnPassCard(0,0);
}

//////////////////////////////////////////////////////////////////////////

