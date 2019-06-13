#include "Stdafx.h"
#include "GameOption.h"
#include "GameClientDlg.h"

//////////////////////////////////////////////////////////////////////////

//时间标识
#define IDI_OUT_CARD					200								//出牌定时器
#define IDI_CALL_CARD					201								//叫牌定时器
#define IDI_START_GAME					202								//开始定时器
#define IDI_SEND_CONCEAL				203								//底牌定时器

//时间范围
#define TIME_OUT_CARD					30								//出牌时间
#define TIME_START_GAME					30								//开始时间
#define TIME_SEND_CONCEAL				30								//留底时间
#define TIME_CALL_SCORE					30								//叫分时间

//功能定时器
#define IDI_DISPATCH_CARD				301								//发牌定时器
#define IDI_LAST_TURN_CARD				302								//上轮定时器
#define IDI_THROW_CARD_RESULT			303								//甩牌定时器
#define IDI_LAND_SCORE					304								//叫分定时器

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientDlg, CGameFrameDlg)
	ON_WM_TIMER()
	ON_MESSAGE(IDM_START,OnStart)
	ON_MESSAGE(IDM_OUT_CARD,OnOutCard)
	ON_MESSAGE(IDM_OUT_PROMPT,OnOutPrompt)
	ON_MESSAGE(IDM_REQUEST_LEAVE,OnRuquestLeave)
	ON_MESSAGE(IDM_LAST_TURN_CARD,OnLastTurnCard)
	ON_MESSAGE(IDM_TRUSTEE_CONTROL,OnStusteeControl)
	ON_MESSAGE(IDM_LEFT_HIT_CARD,OnLeftHitCard)
	ON_MESSAGE(IDM_RIGHT_HIT_CARD,OnRightHitCard)
	ON_MESSAGE(IDM_LAND_SCORE,OnLandScore)
	ON_MESSAGE(IDM_LAND_MULTIPLES,OnMultiples)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientDlg::CGameClientDlg() : CGameFrameDlg(&m_GameClientView)
{
	//逻辑变量
	m_cbPackCount=0;
	m_cbMainColor=COLOR_ERROR;
	m_GameClientView.m_CallScore.m_cbLandScore=250;
	m_GameClientView.m_CallScore.m_cbLandColor=0xFF;
	m_GameClientView.m_CallScore.m_cbMultiples=1;

	m_cbValueOrder[0]=VALUE_ERROR;
	m_cbValueOrder[1]=VALUE_ERROR;
	m_cbMainValue=VALUE_ERROR;
	
	//叫牌信息
	m_cbCallCard=0;
	m_cbCallCount=0;
	m_wCallCardUser=INVALID_CHAIR;


	//状态变量
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	
	
	//得分信息
	m_cbScoreCardCount=0;
	ZeroMemory(m_cbScoreCardData,sizeof(m_cbScoreCardData));

	//甩牌变量
	m_bThrowCard=false;
	m_cbResultCardCount=0;
	m_wThrowCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbResultCardData,sizeof(m_cbResultCardData));

	//出牌变量
	m_bLastTurn=false;
	m_wFirstOutUser=INVALID_CHAIR;
	ZeroMemory(m_cbOutCardData,sizeof(m_cbOutCardData));
	ZeroMemory(m_cbOutCardCount,sizeof(m_cbOutCardCount));

	//底牌扑克
	m_cbConcealCount=0;
	ZeroMemory(m_cbConcealCard,sizeof(m_cbConcealCard));

	//用户扑克
	m_cbHandCardCount=0;
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

	//明手扑克
	ZeroMemory(m_HandOutCardCount,sizeof(m_HandOutCardCount));	
	ZeroMemory(m_HandOutCardData,sizeof(m_HandOutCardData));		

	//配置变量
	m_bStustee=false;
	m_bDeasilOrder=false;
	m_dwCardHSpace=DEFAULT_PELS;

	//辅助变量
	m_cbRemnantCardCount=0;
	m_cbDispatchCardCount=0;
	m_cbFocusUser=0;
	m_bHandOutControl=false;
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
	SetWindowText(TEXT("视频桥牌游戏"));

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
	m_GameClientView.m_HandCardControl.SetCardSpace(m_dwCardHSpace,0,DEF_CARD_SHOOT);

	for (byte i=0;i<GAME_PLAYER;i++)
	{
		for (WORD j=0;j<GAME_PLAYER;j++)
		{
			m_GameClientView.m_HandOutCardControl[i][j].SetCardSpace(m_dwCardHSpace,DEF_CARD_V_SPACE,DEF_CARD_SHOOT);
		}
	}

	return true;
}

//重置框架
void CGameClientDlg::ResetGameFrame()
{
	//逻辑变量
	m_cbPackCount=0;
	m_cbMainColor=COLOR_ERROR;
	m_GameClientView.m_CallScore.m_cbLandScore=250;
	m_GameClientView.m_CallScore.m_cbLandColor=0xFF;
	m_GameClientView.m_CallScore.m_cbMultiples=1;
	m_cbValueOrder[0]=VALUE_ERROR;
	m_cbValueOrder[1]=VALUE_ERROR;
	m_cbMainValue=VALUE_ERROR;
	
	//叫牌信息
	m_cbCallCard=0;
	m_wCallCardUser=INVALID_CHAIR;

	//状态变量
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;

	//得分信息
	m_cbScoreCardCount=0;
	ZeroMemory(m_cbScoreCardData,sizeof(m_cbScoreCardData));

	//甩牌变量
	m_bThrowCard=false;
	m_cbResultCardCount=0;
	m_wThrowCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbResultCardData,sizeof(m_cbResultCardData));

	//出牌变量
	m_bLastTurn=false;
	m_wFirstOutUser=INVALID_CHAIR;
	ZeroMemory(m_cbOutCardData,sizeof(m_cbOutCardData));
	ZeroMemory(m_cbOutCardCount,sizeof(m_cbOutCardCount));

	//底牌扑克
	m_cbConcealCount=0;
	ZeroMemory(m_cbConcealCard,sizeof(m_cbConcealCard));

	//用户扑克
	m_cbHandCardCount=0;
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

	//明手扑克
	ZeroMemory(m_HandOutCardCount,sizeof(m_HandOutCardCount));	
	ZeroMemory(m_HandOutCardData,sizeof(m_HandOutCardData));	

	//配置变量
	m_bStustee=false;

	//辅助变量
	m_cbRemnantCardCount=0;
	m_cbDispatchCardCount=0;
	m_bHandOutControl=false;

	ZeroMemory(m_cbDispatchCardData,sizeof(m_cbDispatchCardData));

	//删除定时器
	KillTimer(IDI_DISPATCH_CARD);
	KillTimer(IDI_LAST_TURN_CARD);
	KillTimer(IDI_THROW_CARD_RESULT);
	KillTimer(IDI_LAND_SCORE);
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
			if ((nElapse==0)||((m_bStustee==true)&&(nElapse<(TIME_OUT_CARD))))
			{
				if ((IsLookonMode()==false)&&(wChairID==GetMeChairID())) AutomatismOutCard();
				return true;
			}

			//播放声音
			if ((nElapse<=3)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false)) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));

			return true;
		}
	case IDI_LAND_SCORE:		//叫分
		{
			if ((nElapse==0)||((m_bStustee==true)&&(nElapse<(TIME_CALL_SCORE))))
			{
				if ((IsLookonMode()==false)&&(wChairID==GetMeChairID())) OnLandScore(255,250);
				return false;
			}
			if ((nElapse<=10)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false)) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));

			return true;

		}
	case IDI_START_GAME:		//开始游戏
		{
			if ((nElapse==0)||((m_bStustee==true)&&(nElapse<(TIME_START_GAME))))
			{
				if ((IsLookonMode()==false)&&(wChairID==GetMeChairID())) OnStart(0,0);
				return true;
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
	//扑克控制
	m_GameClientView.m_HandCardControl.SetDisplayFlag((IsAllowLookon()==true));

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
	case SUB_S_SEND_CARD:		//发送扑克
		{
			return OnSubSendCard(pBuffer,wDataSize);
		}
	case SUB_S_CALL_SCORE:		//用户叫分
		{
			return OnSubCallScore(pBuffer,wDataSize);
		}
	case SUB_S_GAME_PLAY:		//游戏开始
		{
			return OnSubGamePlay(pBuffer,wDataSize);
		}
	case SUB_S_OUT_CARD:		//用户出牌
		{
			return OnSubOutCard(pBuffer,wDataSize);
		}
	case SUB_S_THROW_RESULT:	//甩牌结果
		{
			return OnSubThrowResult(pBuffer,wDataSize);
		}
	case SUB_S_TURN_BALANCE:	//一轮结算
		{
			return OnSubTurnBalance(pBuffer,wDataSize);
		}
	case SUB_S_GAME_END:		//游戏结束
		{
			return OnSubGameEnd(pBuffer,wDataSize);
		}
	}

	return true;
}

//游戏场景
bool CGameClientDlg::OnGameSceneMessage(BYTE cbGameStation, bool bLookonOther, const void * pBuffer, WORD wDataSize)
{
	switch (cbGameStation)
	{
	case GS_UG_FREE:	//空闲状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pBuffer;

			//设置控件
			if (IsLookonMode()==false)
			{
				m_GameClientView.m_btStart.ShowWindow(TRUE);
				m_GameClientView.m_btStart.SetFocus();
			}

			//玩家设置
			if (IsLookonMode()==false)
			{
				m_GameClientView.m_btStusteeControl.EnableWindow(TRUE);
				m_GameClientView.m_HandCardControl.SetDisplayFlag(true);
			}

			return true;
		}
	
	case GS_UG_SCORE:  //叫分
		{
			
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusScore)) return false;
			CMD_S_StatusScore * pStatusScore=(CMD_S_StatusScore *)pBuffer;

			//逻辑变量
			m_cbPackCount=pStatusScore->cbPackCount;
			m_wBankerUser=pStatusScore->wBankerUser;

			//叫牌信息
			m_cbCallCard=pStatusScore->cbCallCard;
			m_wCallCardUser=pStatusScore->wCallCardUser;

			//设置扑克
			m_cbHandCardCount=pStatusScore->cbCardCount;
			CopyMemory(m_cbHandCardData,pStatusScore->cbCardData,sizeof(BYTE)*m_cbHandCardCount);

			//逻辑组件
			m_GameLogic.SetPackCount(m_cbPackCount);
			m_GameLogic.SetMainColor(m_cbMainColor);

			//设置界面
			for (WORD i=0;i<GAME_PLAYER;i++)	
			{
				WORD wViewChairID=SwitchViewChairID(i);
				SetHandCardControl(m_cbHandCardData,m_cbHandCardCount);
				m_GameClientView.SetBankerUser(SwitchViewChairID(m_wBankerUser));
			}
			//m_GameClientView.ShowLandTitle(true);
			//m_GameClientView.SetBaseScore(pStatusScore->lBaseScore);

			//按钮控制
			if ((IsLookonMode()==false)&&(pStatusScore->wCurrentUser==GetMeChairID()))
			{
				/*
				m_GameClientView.m_btGiveUpScore.ShowWindow(SW_SHOW);
				m_GameClientView.m_bt60Score.ShowWindow(pStatusScore->bBossScore<=4?SW_SHOW:SW_HIDE);
				m_GameClientView.m_bt65Score.ShowWindow(pStatusScore->bBossScore<=3?SW_SHOW:SW_HIDE);
				m_GameClientView.m_bt70Score.ShowWindow(pStatusScore->bBossScore<=2?SW_SHOW:SW_HIDE);
				m_GameClientView.m_bt75Score.ShowWindow(pStatusScore->bBossScore<=1?SW_SHOW:SW_HIDE);
				m_GameClientView.m_bt80Score.ShowWindow(pStatusScore->bBossScore==0?SW_SHOW:SW_HIDE);
				*/

			}
			
			//设置扑克
			m_GameClientView.m_btStusteeControl.EnableWindow(TRUE);
			m_GameClientView.m_HandCardControl.SetDisplayFlag(true);
		
			if (IsLookonMode()==false) m_GameClientView.m_HandCardControl.SetDisplayFlag(true);
		
			//设置时间
			//SetGameTimer(pStatusScore->wCurrentUser,IDI_LAND_SCORE,TIME_CALL_SCORE);

			return true;
		}
	
	case GS_UG_PLAY:	//游戏状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pBuffer;

			//逻辑变量
			m_cbPackCount=pStatusPlay->cbPackCount;
			m_cbMainColor=pStatusPlay->cbMainColor;

			//叫牌信息
			m_cbCallCard=pStatusPlay->cbCallCard;
			m_wCallCardUser=pStatusPlay->wCallCardUser;

			//用户变量
			m_wBankerUser=pStatusPlay->wBankerUser;
			m_wCurrentUser=pStatusPlay->wCurrentUser;
			m_wFirstOutUser=pStatusPlay->wFirstOutUser;

			//逻辑组件
			m_GameLogic.SetPackCount(m_cbPackCount);
			m_GameLogic.SetMainColor(m_cbMainColor);

			//设置底牌
			m_cbConcealCount=pStatusPlay->cbConcealCount;
			CopyMemory(m_cbConcealCard,pStatusPlay->cbConcealCard,m_cbConcealCount*sizeof(BYTE));

			//设置扑克
			m_cbHandCardCount=pStatusPlay->cbCardCount;
			CopyMemory(m_cbHandCardData,pStatusPlay->cbCardData,sizeof(BYTE)*m_cbHandCardCount);

			//出牌变量
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				m_cbOutCardCount[i][0]=pStatusPlay->cbOutCardCount[i];
				CopyMemory(m_cbOutCardData[i][0],pStatusPlay->cbOutCardData[i],sizeof(BYTE)*m_cbOutCardCount[i][0]);
			}

			//得分信息
			m_cbScoreCardCount=pStatusPlay->cbScoreCardCount;
			CopyMemory(m_cbScoreCardData,pStatusPlay->cbScoreCardData,sizeof(BYTE)*m_cbScoreCardCount);

			//主牌信息
			WORD wMeChairID=GetMeChairID();
			BYTE cbValueMySelf=pStatusPlay->cbValueOrder[wMeChairID%2];
			BYTE cbValueOther=pStatusPlay->cbValueOrder[(wMeChairID+1)%2];

			//叫牌信息
			if (m_wCallCardUser!=INVALID_CHAIR)
			{
				WORD wViewChairID=SwitchViewChairID(m_wCallCardUser);
				BYTE cbCallColor=m_GameLogic.GetCardColor(m_cbCallCard);
			}

			//得分视图
			m_GameClientView.m_CardScore.SetCardData(m_cbScoreCardData,m_cbScoreCardCount);
			
			//出牌界面
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				WORD wViewChairID=SwitchViewChairID(i);
				m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(m_cbOutCardData[i][0],m_cbOutCardCount[i][0]);
			}

			//排列扑克
			m_GameLogic.SortCardList(m_cbHandCardData,m_cbHandCardCount);

			//界面设置
			SetHandCardControl(m_cbHandCardData,m_cbHandCardCount);
			m_GameClientView.SetBankerUser(SwitchViewChairID(m_wBankerUser));

			//玩家设置
			if (IsLookonMode()==false) 
			{
				m_GameClientView.m_btStusteeControl.EnableWindow(TRUE);
				m_GameClientView.m_HandCardControl.SetPositively(true);
				m_GameClientView.m_HandCardControl.SetDisplayFlag(true);
			}

			//旁观设置
			if (IsAllowLookon()==true) m_GameClientView.m_HandCardControl.SetDisplayFlag(true);

			//出牌设置
			if ((IsLookonMode()==false)&&(m_wCurrentUser==GetMeChairID()))
			{
				m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
				m_GameClientView.m_btOutPrompt.ShowWindow(SW_SHOW);
				m_GameClientView.m_btOutCard.EnableWindow((VerdictOutCard()==true)?TRUE:FALSE);
			}

			//时间设置
			SetGameTimer(m_wCurrentUser,IDI_OUT_CARD,TIME_OUT_CARD);

			return true;
		}
	}

	return false;
}

//游戏开始
bool CGameClientDlg::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;

	//消息处理
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;

	//设置状态
	SetGameStatus(GS_UG_SCORE);

	//逻辑变量
	m_cbMainColor=COLOR_ERROR;
	m_cbPackCount=pGameStart->cbPackCount;
	m_cbJuKuang=pGameStart->cbJuKuang;

	//叫牌变量
	m_cbCallCard=0;
	m_wCallCardUser=INVALID_CHAIR;
	m_GameClientView.m_CallScore.m_cbLandScore=250;
	m_GameClientView.m_CallScore.m_cbLandColor=0xFF;
	m_GameClientView.m_CallScore.m_cbMultiples=1;
	
	m_cbValueOrder[0]=VALUE_ERROR;
	m_cbValueOrder[1]=VALUE_ERROR;
	m_cbMainValue=VALUE_ERROR;

	//界面控制
	m_GameClientView.SetDingYue(m_cbMainValue,m_cbMainColor);
	
	//状态变量
	m_wCurrentUser=INVALID_CHAIR;
	m_wBankerUser=pGameStart->wBankerUser;

	//得分信息
	m_cbScoreCardCount=0;
	ZeroMemory(m_cbScoreCardData,sizeof(m_cbScoreCardData));

	
	
	//出牌变量
	m_wFirstOutUser=INVALID_CHAIR;
	ZeroMemory(m_cbOutCardData,sizeof(m_cbOutCardData));
	ZeroMemory(m_cbOutCardCount,sizeof(m_cbOutCardCount));

	//逻辑组件
	m_GameLogic.SetPackCount(m_cbPackCount);
	m_GameLogic.SetMainColor(m_cbMainColor);

	//叫分控制
	m_GameClientView.m_CallScore.ShowWindow(SW_HIDE);
	m_GameClientView.m_CallScoreInfo.ShowWindow(SW_HIDE);

	//主牌信息
	WORD wMeChairID=GetMeChairID();
	BYTE cbValueMySelf=pGameStart->cbValueOrder[wMeChairID%2];
	BYTE cbValueOther=pGameStart->cbValueOrder[(wMeChairID+1)%2];

	//设置庄家
	if (m_wBankerUser==INVALID_CHAIR) m_GameClientView.SetBankerUser(INVALID_CHAIR);
	else m_GameClientView.SetBankerUser(SwitchViewChairID(m_wBankerUser));

	//设置界面
	if (IsLookonMode()==true)
	{
		SetHandCardControl(NULL,0);
		m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE);
		m_GameClientView.m_CardScore.SetCardData(NULL,0);
		m_GameClientView.m_UserCardControl[0].SetCardData(NULL,0);
		m_GameClientView.m_UserCardControl[1].SetCardData(NULL,0);
		m_GameClientView.m_UserCardControl[2].SetCardData(NULL,0);
		m_GameClientView.m_UserCardControl[3].SetCardData(NULL,0);
	}

	//控制设置
	m_bHandOutControl=false;
	
	bool bPlayerMode=(IsLookonMode()==false);
	m_GameClientView.m_HandCardControl.SetDisplayFlag(bPlayerMode);
	if ((IsLookonMode()==false)&&(m_bStustee==false)) //ActiveGameFrame();

	//环境设置
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));

	return true;
}

//发送扑克
bool CGameClientDlg::OnSubSendCard(const void * pBuffer, WORD wDataSize)
{
	//变量定义
	CMD_S_SendCard * pSendCard=(CMD_S_SendCard *)pBuffer;
	WORD wFirstSize=sizeof(CMD_S_SendCard)-sizeof(pSendCard->cbCardData);

	//效验数据
	ASSERT(wDataSize>=wFirstSize);

	if (wDataSize<wFirstSize) return false;

	ASSERT(wDataSize==(wFirstSize+pSendCard->cbCardCount*sizeof(BYTE)));
	if (wDataSize!=(wFirstSize+pSendCard->cbCardCount*sizeof(BYTE))) return false;

	//时间设置
	KillGameTimer(IDI_LAND_SCORE);

	//局况设置
	m_GameClientView.SetJuKuang(m_cbJuKuang);

	//设置扑克
	m_cbHandCardCount=pSendCard->cbCardCount;
	CopyMemory(m_cbHandCardData,pSendCard->cbCardData,sizeof(BYTE)*m_cbHandCardCount);

	//派发扑克
	DispatchUserCard(m_cbHandCardData,m_cbHandCardCount);


	//设置界面
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameClientView.SetPassFlag(i,false);
		m_GameClientView.m_UserCardControl[i].SetCardData(NULL,0);
	}
	if (IsLookonMode()==true)
	{
		m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE);
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			for (WORD j=0;j<GAME_PLAYER;j++)
			{
				m_GameClientView.m_HandOutCardControl[i][j].SetCardData(NULL,0);
			}
		}
	}
	m_cbFocusUser=pSendCard->wCurrentUser;  //add
	
	
	//当前玩家
	if ((IsLookonMode()==false)&&(pSendCard->wCurrentUser==GetMeChairID())&&(m_bStustee==false))
	{
		//播放声音
		PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));

		
	}

	m_GameClientView.m_CallScoreInfo.SetCallGameInfo(INVALID_CHAIR,255,0);
	//叫分历史信息处理
	switch (m_cbJuKuang)
	{
	case 2:
		{
			m_GameClientView.m_CallScoreInfo.SetCallGameInfo(0,240,240);
			break;
		}
	case 3:
		{
			m_GameClientView.m_CallScoreInfo.SetCallGameInfo(0,240,240);
			m_GameClientView.m_CallScoreInfo.SetCallGameInfo(1,240,240);
			break;
		}
	case 4:
		{
			m_GameClientView.m_CallScoreInfo.SetCallGameInfo(0,240,240);
			m_GameClientView.m_CallScoreInfo.SetCallGameInfo(1,240,240);
			m_GameClientView.m_CallScoreInfo.SetCallGameInfo(2,240,240);
			break;
		}
	}
	m_bHandOutControl=false;

	//设置叫分记录
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		const tagUserData * pUserData=GetUserData(i);
		m_GameClientView.m_CallScoreInfo.SetCallUserName(i,pUserData->szName);
	}
	return true;
}


//游戏开始
bool CGameClientDlg::OnSubGamePlay(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GamePlay));
	if (wDataSize!=sizeof(CMD_S_GamePlay)) return false;

	//界面设置
	m_GameClientView.m_CallScoreInfo.ShowWindow(SW_HIDE);

	//消息处理
	CMD_S_GamePlay * pGamePlay=(CMD_S_GamePlay *)pBuffer;

	//设置变量
	m_wCurrentUser=pGamePlay->wCurrentUser;
	m_wFirstOutUser=(pGamePlay->wCurrentUser+1)%GAME_PLAYER;
	m_wBankerUser=m_wCurrentUser;
	m_cbMultiples=pGamePlay->bMultiples;
	m_cbMainColor=pGamePlay->m_cbLandColor;
	m_cbMainValue=pGamePlay->m_cbLandScore;
	
	m_GameLogic.SetMainColor(m_cbMainColor);
	m_GameLogic.SetPackCount(1);
	m_GameClientView.SetBankerUser(SwitchViewChairID(m_wBankerUser));

	m_GameClientView.SetDingYue(m_cbMainValue,m_cbMainColor);
	
	//设置状态
	SetGameStatus(GS_UG_PLAY);

	//控制设置
	bool bPlayerMode=(IsLookonMode()==false);
	m_GameClientView.m_HandCardControl.SetPositively(bPlayerMode);
	
	//设置扑克
	m_GameLogic.SortCardList(m_cbHandCardData,m_cbHandCardCount);
	SetHandCardControl(m_cbHandCardData,m_cbHandCardCount);
	
	//当前玩家
	if ((IsLookonMode()==false)&&(m_wFirstOutUser==GetMeChairID()))
	{
		if (m_bStustee==false) //ActiveGameFrame();
		m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btOutCard.EnableWindow(FALSE);
		m_GameClientView.m_btOutPrompt.ShowWindow(SW_SHOW);
	}

	m_GameClientView.m_CallScore.m_bDbl=false;
	m_GameClientView.m_CallScore.m_bReDbl=false;

	//设置明手扑克
	BYTE bHandOutUser=(m_wBankerUser+2)%GAME_PLAYER;
	m_HandOutCardCount[bHandOutUser]=pGamePlay->bCardCount[bHandOutUser];
	CopyMemory(&m_HandOutCardData[bHandOutUser],pGamePlay->bCardData[bHandOutUser],m_HandOutCardCount[bHandOutUser]*sizeof(BYTE));
	m_GameLogic.SortCardList(m_HandOutCardData[bHandOutUser],m_HandOutCardCount[bHandOutUser]);

	
	//环境设置
	SetGameTimer(m_wFirstOutUser,IDI_OUT_CARD,TIME_OUT_CARD);
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));

	return true;
}


//叫分
bool CGameClientDlg::OnSubCallScore(const void * pBuffer, WORD wDataSize)
{
	
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_CallScore));
	if (wDataSize!=sizeof(CMD_S_CallScore)) return false;

	//消息处理
	CMD_S_CallScore * pLandScore=(CMD_S_CallScore *)pBuffer;
	
	//设置界面
	WORD wViewChairID=SwitchViewChairID(pLandScore->bCallScoreUser);
	
	//设置变量
	BYTE bPreScore=pLandScore->bCurrentScore;   //地主分数
	BYTE bPreColor=pLandScore->bCurrentColor;   //地主花色
	BYTE bMultiples=pLandScore->bMultiples;     //游戏倍数
	BYTE bLastScore=pLandScore->bLastScore;     //上次叫分
	BYTE bLastColor=pLandScore->bLastColor;     //上次花色
	bool bDbl=pLandScore->bDbl;					//加倍标志
	bool bReDbl=pLandScore->bReDbl;				//再加倍

	//设置叫分记录
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		const tagUserData * pUserData=GetUserData(i);
		if (pLandScore->bCallScoreUser==i)
		{
			m_GameClientView.m_CallScoreInfo.SetCallGameInfo(i,bLastColor,bLastScore);
		}
	}

	if (bLastScore!=200&&bLastColor!=200&&bLastScore!=210&&bLastColor!=210)		 //不是加倍
	{
		if(bPreScore!=250) m_GameClientView.m_CallScore.m_cbLandScore=bPreScore;
		if(bPreColor!=0xFF) m_GameClientView.m_CallScore.m_cbLandColor=bPreColor;
	}
	
	m_GameClientView.m_CallScore.m_cbMultiples=bMultiples;

	m_GameClientView.m_CallScoreInfo.ShowWindow(SW_SHOW);
	if ((IsLookonMode()==false)&&(pLandScore->wCurrentUser==GetMeChairID()))
	{
		//ActiveGameFrame();
		if (m_bStustee==false)
		{
			m_GameClientView.m_CallScore.m_bDbl=bDbl;
			m_GameClientView.m_CallScore.m_bReDbl=bReDbl;
			m_GameClientView.m_CallScoreInfo.ShowWindow(SW_SHOW);
			m_GameClientView.m_CallScore.ShowWindow(SW_SHOW);
		}
		else
		{
			OnLandScore(255,250);
		}
		
	}
	//SetHandCardControl(m_cbHandCardData,m_cbHandCardCount);
	m_GameLogic.SortCardList(m_cbHandCardData,m_cbHandCardCount);
	
	//播放声音
	PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
	//设置时间
	SetGameTimer(pLandScore->wCurrentUser,IDI_LAND_SCORE,TIME_CALL_SCORE);
	
	return true;
}

//用户出牌
bool CGameClientDlg::OnSubOutCard(const void * pBuffer, WORD wDataSize)
{
	//变量定义
	CMD_S_OutCard * pOutCard=(CMD_S_OutCard *)pBuffer;
	WORD wFirstSize=sizeof(CMD_S_OutCard)-sizeof(pOutCard->cbCardData);

	/*//效验数据
	ASSERT(wDataSize>=wFirstSize);
	if (wDataSize<wFirstSize) return false;
	ASSERT(wDataSize==(wFirstSize+pOutCard->cbCardCount*sizeof(BYTE)));
	if (wDataSize!=(wFirstSize+pOutCard->cbCardCount*sizeof(BYTE))) return false;
	*/
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_OutCard));
	if (wDataSize!=sizeof(CMD_S_OutCard)) return false;

	//变量定义
	WORD wMeChairID=GetMeChairID();
	WORD wOutCardUser=pOutCard->wOutCardUser;
	WORD wOutViewChairID=SwitchViewChairID(wOutCardUser);
	BYTE bHandOutUser=(m_wBankerUser+2)%GAME_PLAYER;
	WORD wViewChairID=SwitchViewChairID(bHandOutUser);
	//出牌处理
	SwitchToThrowResult();
	KillGameTimer(IDI_OUT_CARD);

	m_GameClientView.m_CallScoreInfo.ShowWindow(SW_HIDE);
	//扑克界面
	if ((IsLookonMode()==true)||(wMeChairID!=wOutCardUser))
	{
		//历史扑克
		if (wOutCardUser==m_wFirstOutUser)
		{
			for (WORD i=0;i<GAME_PLAYER;i++) 
			{
				if (i==wOutCardUser) continue;
				m_GameClientView.m_UserCardControl[SwitchViewChairID(i)].SetCardData(NULL,0);
			}
		}

		//控制界面
		m_GameClientView.m_UserCardControl[wOutViewChairID].SetCardData(pOutCard->cbCardData,pOutCard->cbCardCount);
	}

	//历史扑克
	if ((wOutCardUser==m_wFirstOutUser)&&(m_cbOutCardCount[m_wFirstOutUser][0]!=0))
	{
		//设置扑克
		for (WORD i=0;i<GAME_PLAYER;i++) 
		{
			//保存扑克
			m_cbOutCardCount[i][1]=m_cbOutCardCount[i][0];
			CopyMemory(m_cbOutCardData[i][1],m_cbOutCardData[i][0],sizeof(BYTE)*m_cbOutCardCount[i][0]);

			//清理扑克
			m_cbOutCardCount[i][0]=0;
			ZeroMemory(m_cbOutCardData[i][0],sizeof(m_cbOutCardData[i][0]));
		}

		//控制按钮
		m_GameClientView.m_btLastTurnCard.EnableWindow(TRUE);
	}

	//出牌记录
	m_cbOutCardCount[wOutCardUser][0]=pOutCard->cbCardCount;
	CopyMemory(m_cbOutCardData[wOutCardUser][0],pOutCard->cbCardData,pOutCard->cbCardCount*sizeof(BYTE));

	//当前界面
	SwitchToCurrentCard();

	//玩家扑克
	if ((wMeChairID==bHandOutUser)&&(wOutCardUser==wMeChairID))
	{
		//删除扑克 
		BYTE cbSourceCount=m_HandOutCardCount[wMeChairID];
		m_HandOutCardCount[wMeChairID]-=pOutCard->cbCardCount;
		m_GameLogic.RemoveCard(pOutCard->cbCardData,pOutCard->cbCardCount,m_HandOutCardData[wMeChairID],cbSourceCount);
			
		//设置扑克
		SetHandCardControl(m_HandOutCardData[wMeChairID],m_HandOutCardCount[wMeChairID]);
	}
	else
	{
		//++++++++++++++++++++++++++++++++++++++++++++++
		if(wMeChairID==m_wBankerUser&&wOutCardUser==bHandOutUser)
		{
			BYTE cbSourceCount=m_HandOutCardCount[bHandOutUser];
			m_HandOutCardCount[bHandOutUser]-=pOutCard->cbCardCount;
			m_GameLogic.RemoveCard(pOutCard->cbCardData,pOutCard->cbCardCount,m_HandOutCardData[bHandOutUser],cbSourceCount);
				
			SetHandOutCardControl(m_HandOutCardData[bHandOutUser],m_HandOutCardCount[bHandOutUser],m_wBankerUser,0);
		}
		//++++++++++++++++++++++++++++++++++++++++++++++
		if ((IsLookonMode()==true)&&(wOutCardUser==wMeChairID))
		{
			//删除扑克 
			BYTE cbSourceCount=m_cbHandCardCount;
			m_cbHandCardCount-=pOutCard->cbCardCount;
			m_GameLogic.RemoveCard(pOutCard->cbCardData,pOutCard->cbCardCount,m_cbHandCardData,cbSourceCount);
			
			//设置扑克
			SetHandCardControl(m_cbHandCardData,m_cbHandCardCount);
			
		}

	}

	//播放声音
	if ((IsLookonMode()==true)||(wMeChairID!=wOutCardUser)) PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));

	//设置变量
	m_wCurrentUser=pOutCard->wCurrentUser;
	/*
	if (wOutCardUser!=wMeChairID) 
	{
		m_GameClientView.m_HandCardControl.SetPositively(false);
	}
	else
	{
		m_GameClientView.m_HandCardControl.SetPositively(true);
	}
	*/
	BYTE bHandOutCardData[60];
	BYTE bHandOutCardCount=0;
	ZeroMemory(bHandOutCardData,sizeof(bHandOutCardData));
	CopyMemory(bHandOutCardData,pOutCard->bHandOutCardData,sizeof(pOutCard->bHandOutCardData));
	bHandOutCardCount=pOutCard->bHandOutCardCount;	
	
	//设置明手
	if(wViewChairID==1||wViewChairID==3) m_GameClientView.m_HandOutCardControl[wMeChairID][wViewChairID].SetDirection(false);

	if(bHandOutUser==wMeChairID)
	{
		if (bHandOutCardCount!=0)
		{
			m_GameClientView.m_HandOutCardControl[wMeChairID][wViewChairID].SetCardData(NULL,0);
			m_GameClientView.m_HandCardControl.SetCardData(m_HandOutCardData[bHandOutUser],m_HandOutCardCount[bHandOutUser]);
			SetHandCardControl(m_HandOutCardData[bHandOutUser],m_HandOutCardCount[bHandOutUser]);
		}
	}
	else
	{
		if (bHandOutCardCount!=0)
		{
			//排列扑克
			m_GameLogic.SortCardList(bHandOutCardData,bHandOutCardCount);
			m_GameClientView.m_HandOutCardControl[wMeChairID][wViewChairID].SetCardData(bHandOutCardData,bHandOutCardCount);
		}
		else
		{
			m_GameClientView.m_HandOutCardControl[wMeChairID][wViewChairID].SetCardData(NULL,0);
		}
	}
	
	//庄家操作明手牌
	if(wMeChairID==(bHandOutUser+2)%GAME_PLAYER)
	{
		//轮到明手出牌,庄家才能控制明手牌
		//if(m_wCurrentUser==bHandOutUser) 
		if(pOutCard->bHandOutFlag[wMeChairID]&&(m_wCurrentUser==bHandOutUser))
		{
			m_bHandOutControl=true;
			m_GameClientView.m_HandOutCardControl[wMeChairID][wViewChairID].SetHandOutControl(true);
		}
		SetHandOutCardControl(m_HandOutCardData[bHandOutUser],m_HandOutCardCount[bHandOutUser],wMeChairID,wViewChairID);
		m_GameClientView.m_HandOutCardControl[wMeChairID][wViewChairID].SetPositively(true);
		m_GameClientView.m_HandOutCardControl[wMeChairID][wViewChairID].SetSinkWindow(AfxGetMainWnd());
	}

	//玩家设置
	if ((IsLookonMode()==false)&&(m_wCurrentUser==wMeChairID))
	{
		if (m_bStustee==false&&bHandOutUser!=wMeChairID) //ActiveGameFrame();
		{
			m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
			m_GameClientView.m_btOutPrompt.ShowWindow(SW_SHOW);
			m_GameClientView.m_btOutCard.EnableWindow((VerdictOutCard()==true)?TRUE:FALSE);
		}
		else
		{
			if(bHandOutUser==wMeChairID)
			{
				m_GameClientView.m_btStusteeControl.ShowWindow(false);
				m_GameClientView.m_btLastTurnCard.ShowWindow(false);
			}
			else
			{
				m_GameClientView.m_btStusteeControl.ShowWindow(true);
				m_GameClientView.m_btLastTurnCard.ShowWindow(true);
			}
		}
	}

	//设置时间
	if (m_wCurrentUser!=INVALID_CHAIR) SetGameTimer(m_wCurrentUser,IDI_OUT_CARD,TIME_OUT_CARD);

	return true;
}

//甩牌结果
bool CGameClientDlg::OnSubThrowResult(const void * pBuffer, WORD wDataSize)
{
	//变量定义
	CMD_S_ThrowResult * pThrowResult=(CMD_S_ThrowResult *)pBuffer;
	WORD wCardCount=pThrowResult->cbThrowCardCount+pThrowResult->cbResultCardCount;
	WORD wFirstSize=sizeof(CMD_S_ThrowResult)-sizeof(pThrowResult->cbCardDataArray);

	//效验数据
	ASSERT(wDataSize>=wFirstSize);
	if (wDataSize<wFirstSize) return false;
	ASSERT(wDataSize==(wFirstSize+wCardCount*sizeof(BYTE)));
	if (wDataSize!=(wFirstSize+wCardCount*sizeof(BYTE))) return false;

	//变量定义
	WORD wMeChairID=GetMeChairID();
	WORD wOutCardUser=pThrowResult->wOutCardUser;
	WORD wOutViewChairID=SwitchViewChairID(wOutCardUser);

	//扑克变量
	BYTE cbThrowCardCount=pThrowResult->cbThrowCardCount;
	BYTE cbResultCardCount=pThrowResult->cbResultCardCount;
	BYTE * pcbThrowCardData=&pThrowResult->cbCardDataArray[0];
	BYTE * pcbResultCardData=&pThrowResult->cbCardDataArray[pThrowResult->cbThrowCardCount];

	//还原界面
	SwitchToCurrentCard();

	//出牌处理
	KillGameTimer(IDI_OUT_CARD);
	PlayGameSound(AfxGetInstanceHandle(),TEXT("THROW_RESULT"));

	//清理扑克
	for (WORD i=0;i<GAME_PLAYER;i++) 
	{
		//设置扑克
		if (i!=wOutCardUser) m_GameClientView.m_UserCardControl[SwitchViewChairID(i)].SetCardData(NULL,0);
	}

	//历史扑克
	if (m_cbOutCardCount[m_wFirstOutUser][0]!=0)
	{
		//设置扑克
		for (WORD i=0;i<GAME_PLAYER;i++) 
		{
			//保存扑克
			m_cbOutCardCount[i][1]=m_cbOutCardCount[i][0];
			CopyMemory(m_cbOutCardData[i][1],m_cbOutCardData[i][0],sizeof(BYTE)*m_cbOutCardCount[i][0]);

			//清理扑克
			m_cbOutCardCount[i][0]=0;
			ZeroMemory(m_cbOutCardData[i][0],sizeof(m_cbOutCardData[i][0]));
		}
	}

	//出牌界面
	m_GameClientView.m_UserCardControl[wOutViewChairID].SetCardData(pcbThrowCardData,cbThrowCardCount);

	//出牌记录
	m_cbOutCardCount[wOutCardUser][0]=cbResultCardCount;
	CopyMemory(m_cbOutCardData[wOutCardUser][0],pcbResultCardData,cbResultCardCount*sizeof(BYTE));

	//玩家扑克
	if (wOutCardUser==wMeChairID)
	{
		//填补扑克
		if (IsLookonMode()==false)
		{
			//设置扑克
			BYTE cbSourceCount=m_cbHandCardCount;
			m_cbHandCardCount+=cbThrowCardCount;
			CopyMemory(&m_cbHandCardData[cbSourceCount],pcbThrowCardData,sizeof(BYTE)*cbThrowCardCount);

			//排列扑克
			m_GameLogic.SortCardList(m_cbHandCardData,m_cbHandCardCount);
		}

		//删除扑克
		BYTE cbSourceCount=m_cbHandCardCount;
		m_cbHandCardCount-=cbResultCardCount;
		m_GameLogic.RemoveCard(pcbResultCardData,cbResultCardCount,m_cbHandCardData,cbSourceCount);

		//设置界面
		SetHandCardControl(m_cbHandCardData,m_cbHandCardCount);
	
	}

	//设置变量
	m_bThrowCard=true;
	m_cbResultCardCount=cbResultCardCount;
	m_wCurrentUser=pThrowResult->wCurrentUser;
	m_wThrowCardUser=pThrowResult->wOutCardUser;
	CopyMemory(m_cbResultCardData,pcbResultCardData,sizeof(BYTE)*cbResultCardCount);

	//设置时间
	SetTimer(IDI_THROW_CARD_RESULT,2000,NULL);

	//上轮设置
	m_GameClientView.m_btLastTurnCard.EnableWindow(FALSE);

	return true;
}

//一轮结算
bool CGameClientDlg::OnSubTurnBalance(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_TurnBalance));
	if (wDataSize!=sizeof(CMD_TurnBalance)) return false;

	//消息处理
	CMD_TurnBalance * pTurnBalance=(CMD_TurnBalance *)pBuffer;

	WORD wWinnerUser=pTurnBalance->wTurnWinner;
	
	//计算得分
	m_cbValueOrder[0]=pTurnBalance->bBankerScore;
	m_cbValueOrder[1]=pTurnBalance->bOtherScore;
	m_GameClientView.SetValueOrder(m_cbValueOrder[0],m_cbValueOrder[1]);
	
	//设置用户
	m_wCurrentUser=pTurnBalance->wCurrentUser;
	m_wFirstOutUser=pTurnBalance->wCurrentUser;

	//设置明手
	BYTE bHandOutUser=(m_wBankerUser+2)%GAME_PLAYER;
	
	//玩家设置
	if ((IsLookonMode()==false)&&(m_wCurrentUser==GetMeChairID()))
	{
		if (m_bStustee==false&&bHandOutUser!=m_wCurrentUser) //ActiveGameFrame();
		{
			m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
			m_GameClientView.m_btOutPrompt.ShowWindow(SW_SHOW);
			m_GameClientView.m_btOutCard.EnableWindow((VerdictOutCard()==true)?TRUE:FALSE);
		}
	}

	//设置变量
	WORD wMeChairID=GetMeChairID();
	WORD wViewChairID=SwitchViewChairID(bHandOutUser);

	//庄家操作明手牌
	if(wMeChairID==(bHandOutUser+2)%GAME_PLAYER)
	{
		//轮到明手庄家出牌才能控制明手牌
		//if((m_wCurrentUser==bHandOutUser)) 
		if (pTurnBalance->bHandOutFlag[wMeChairID])
		{
			m_bHandOutControl=true;
			m_GameClientView.m_HandOutCardControl[wMeChairID][wViewChairID].SetHandOutControl(true);
		}
		SetHandOutCardControl(m_HandOutCardData[bHandOutUser],m_HandOutCardCount[bHandOutUser],wMeChairID,wViewChairID);
		m_GameClientView.m_HandOutCardControl[wMeChairID][wViewChairID].SetPositively(true);
		m_GameClientView.m_HandOutCardControl[wMeChairID][wViewChairID].SetSinkWindow(AfxGetMainWnd());
	}

	//设置时间
	if (m_wCurrentUser!=INVALID_CHAIR) SetGameTimer(m_wCurrentUser,IDI_OUT_CARD,TIME_OUT_CARD);

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
	
	if (m_wBankerUser==INVALID_CHAIR) m_wBankerUser=0;
	m_cbValueOrder[0]=pGameEnd->bBankerScore;
	m_cbValueOrder[1]=pGameEnd->bOtherScore;
	m_GameClientView.SetValueOrder(m_cbValueOrder[0],m_cbValueOrder[1]);

	//游戏定时器
	KillGameTimer(IDI_OUT_CARD);
	
	//功能定时器
	KillTimer(IDI_DISPATCH_CARD);
	KillTimer(IDI_LAST_TURN_CARD);
	KillTimer(IDI_THROW_CARD_RESULT);
	KillTimer(IDI_LAND_SCORE);
	
	//隐藏控件
	m_GameClientView.m_btOutCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btOutPrompt.ShowWindow(SW_HIDE);
	

	//控制界面
	m_GameClientView.m_btLastTurnCard.EnableWindow(FALSE);
	m_GameClientView.m_btOutCard.EnableWindow(FALSE);

	//设置控件
	SwitchToCurrentCard();
	SwitchToThrowResult();
	
	

	//设置结束
	tagScoreViewInfo ScoreViewInfo;
	ScoreViewInfo.wConcealTime=pGameEnd->wConcealTime;
	
	//设置积分
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		const tagUserData * pUserData=GetUserData(i);
		m_GameClientView.m_ScoreView.SetGameScore(i,pUserData->szName,pGameEnd->lScore[i]);
	}

	//显示成绩
	//m_GameClientView.m_CallScoreInfo.ShowWindow(SW_HIDE);
	m_GameClientView.m_ScoreView.ShowWindow(SW_SHOW);

	//播放声音
	WORD wMeChairID=GetMeChairID();
	LONG lMeScore=pGameEnd->lScore[GetMeChairID()];
	
	if (lMeScore>0L) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WIN"));
	else if (lMeScore<0L) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_LOST"));
	else PlayGameSound(GetModuleHandle(NULL),TEXT("GAME_END"));

	//设置界面
	if (IsLookonMode()==false)
	{
		m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
		//SetGameTimer(GetMeChairID(),IDI_START_GAME,TIME_START_GAME);
	}

	return true;
}

//出牌判断
bool CGameClientDlg::VerdictOutCard()
{
	//状态判断
	if (m_GameClientView.m_btOutCard.IsWindowVisible()==FALSE) return false;
	
	//获取扑克
	BYTE cbCardData[MAX_COUNT];
	BYTE cbShootCount;
	cbShootCount=(BYTE)m_GameClientView.m_HandCardControl.GetShootCard(cbCardData,CountArray(cbCardData));
	
	//出牌判断
	if (cbShootCount>0L&&cbShootCount==1)
	{
		//排列扑克
		m_GameLogic.SortCardList(cbCardData,cbShootCount);

		//首出牌者
		if (m_wFirstOutUser==GetMeChairID())
		{
			//类型判断
			BYTE cbCardType=m_GameLogic.GetCardType(cbCardData,cbShootCount);
			if (cbCardType!=CT_SINGLE) return false;

			return true;
		}

		//跟随出牌
		BYTE cbTurnCardCount=m_cbOutCardCount[m_wFirstOutUser][0];
		bool bLegality=m_GameLogic.EfficacyOutCard(cbCardData,cbShootCount,m_cbOutCardData[m_wFirstOutUser][0],
			cbTurnCardCount,m_cbHandCardData,m_cbHandCardCount);
		

		return bLegality;
	}

	return false;
}

//明手出牌判断
bool CGameClientDlg::VerdictHandOutCard()
{
	//获取扑克
	BYTE cbCardData[MAX_COUNT];
	ZeroMemory(cbCardData,sizeof(cbCardData));
	BYTE cbShootCount=0;
	WORD wMeChairID=GetMeChairID();
	BYTE bHandOutUser=(m_wBankerUser+2)%GAME_PLAYER;
	WORD wViewChairID=SwitchViewChairID(bHandOutUser);
	
	if(m_bHandOutControl)
	{
		cbShootCount=(BYTE)m_GameClientView.m_HandOutCardControl[wMeChairID][wViewChairID].GetShootCard(cbCardData,CountArray(cbCardData));
	}
	else
	{
		return false;
	}

	//出牌判断
	if (cbShootCount>0L&&cbShootCount==1)
	{
		//排列扑克
		m_GameLogic.SortCardList(cbCardData,cbShootCount);

		//首出牌者
		if (m_wFirstOutUser==bHandOutUser)
		{
			//类型判断
			BYTE cbCardType=m_GameLogic.GetCardType(cbCardData,cbShootCount);
			if (cbCardType!=CT_SINGLE) return false;

			return true;
		}
		
		//跟随出牌
		BYTE cbTurnCardCount=m_cbOutCardCount[m_wFirstOutUser][0];
		bool bLegality=m_GameLogic.EfficacyOutCard(cbCardData,cbShootCount,m_cbOutCardData[m_wFirstOutUser][0],
			cbTurnCardCount,m_HandOutCardData[bHandOutUser],m_HandOutCardCount[bHandOutUser]);
		
		return bLegality;
	}
	

	return false;
}


//自动出牌
bool CGameClientDlg::AutomatismOutCard()
{
	//变量定义
	tagOutCardResult OutCardResult;
	ZeroMemory(&OutCardResult,sizeof(OutCardResult));

	//搜索出牌
	BYTE cbTurnCardCount=(m_wCurrentUser==m_wFirstOutUser)?0:m_cbOutCardCount[m_wFirstOutUser][0];
	
	m_GameLogic.SearchOutCard(m_cbHandCardData,m_cbHandCardCount,m_cbOutCardData[m_wFirstOutUser][0],cbTurnCardCount,OutCardResult);

	//设置变量
	KillGameTimer(IDI_OUT_CARD);
	m_wCurrentUser=INVALID_CHAIR;

	//设置控件
	m_GameClientView.m_btOutCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btOutCard.EnableWindow(FALSE);
	m_GameClientView.m_btOutPrompt.ShowWindow(SW_HIDE);

	//设置扑克
	if (m_wFirstOutUser==GetMeChairID())
	{
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			if (i!=2) m_GameClientView.m_UserCardControl[i].SetCardData(NULL,0);
		}
	}

	WORD bHandOutUser=(m_wBankerUser+2)%GAME_PLAYER;
	WORD wViewChairID=SwitchViewChairID(bHandOutUser);
	
	//发送数据
	CMD_C_OutCard OutCard;
	OutCard.bIsHandOut[GetMeChairID()]=false;
	OutCard.bIsAutoOut=true;
	OutCard.cbCardCount=OutCardResult.cbCardCount;
	CopyMemory(OutCard.cbCardData,OutCardResult.cbResultCard,sizeof(BYTE)*OutCard.cbCardCount);
	SendData(SUB_C_OUT_CARD,&OutCard,sizeof(OutCard));

	//预先显示
	PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
	m_GameClientView.m_UserCardControl[2].SetCardData(OutCard.cbCardData,OutCard.cbCardCount);

	//明手控制失效
	m_bHandOutControl=false;
	m_GameClientView.m_HandOutCardControl[m_wBankerUser][0].SetHandOutControl(m_bHandOutControl);
	/*if(m_bHandOutControl)
	{
		ASSERT(1>2);
		BYTE cbSourceCount=m_HandOutCardCount[bHandOutUser];
		m_HandOutCardCount[bHandOutUser]-=OutCard.cbCardCount;
		m_GameLogic.RemoveCard(OutCard.cbCardData,OutCard.cbCardCount,m_HandOutCardData[bHandOutUser],cbSourceCount);
			
		SetHandOutCardControl(m_HandOutCardData[bHandOutUser],m_HandOutCardCount[bHandOutUser],m_wBankerUser,0);
		//明手控制失效
		m_bHandOutControl=false;
	}
	else
	{*/
		
		//预先删除
		BYTE cbSourceCount=m_cbHandCardCount;
		m_cbHandCardCount-=OutCard.cbCardCount;
		m_GameLogic.RemoveCard(OutCard.cbCardData,OutCard.cbCardCount,m_cbHandCardData,cbSourceCount);

		//设置扑克
		SetHandCardControl(m_cbHandCardData,m_cbHandCardCount);
	//}

	return true;
}

//当前扑克
bool CGameClientDlg::SwitchToCurrentCard()
{
	//处理判断
	if (m_bLastTurn==false) return false;

	//环境设置
	m_bLastTurn=false;
	KillTimer(IDI_LAST_TURN_CARD);
	m_GameClientView.SetLastTurnCard(false);

	//设置扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		WORD wViewChairID=SwitchViewChairID(i);
		m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(m_cbOutCardData[i][0],m_cbOutCardCount[i][0]);
	}

	return true;
}

//甩牌结果
bool CGameClientDlg::SwitchToThrowResult()
{
	//处理判断
	if (m_bThrowCard==false) return false;
	if (m_wThrowCardUser==INVALID_CHAIR) return false;

	//时间设置
	KillTimer(IDI_THROW_CARD_RESULT);

	//设置界面
	WORD wViewChairID=SwitchViewChairID(m_wThrowCardUser);
	m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(m_cbResultCardData,m_cbResultCardCount);

	//界面设置
	if (m_cbOutCardCount[0][1]!=0) m_GameClientView.m_btLastTurnCard.EnableWindow(TRUE);

	//设置变量
	m_bThrowCard=false;
	m_cbResultCardCount=0;
	m_wThrowCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbResultCardData,sizeof(m_cbResultCardData));

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



//设置扑克
bool CGameClientDlg::SetHandCardControl(BYTE cbCardData[], BYTE cbCardCount)
{
	//设置过滤
	if (cbCardCount>0)
	{
		//变量定义
		BYTE cbRectifyCount=1;
		BYTE cbRectifyCard[MAX_COUNT+4]={cbCardData[0]};

		//获取属性
		BYTE cbLogicColor=m_GameLogic.GetCardLogicColor(cbCardData[0]);

		//变换扑克
		for (BYTE i=1;i<cbCardCount;i++)
		{
			//获取属性
			BYTE cbCurrentColor=m_GameLogic.GetCardLogicColor(cbCardData[i]);

			//插入判断
			if (cbCurrentColor!=cbLogicColor)
			{
				cbLogicColor=cbCurrentColor;
				cbRectifyCard[cbRectifyCount++]=255;
				cbRectifyCard[cbRectifyCount++]=cbCardData[i];
			}
			else cbRectifyCard[cbRectifyCount++]=cbCardData[i];
		}

		//设置扑克
		m_GameClientView.m_HandCardControl.SetCardData(cbRectifyCard,cbRectifyCount);

		return true;
	}
	else m_GameClientView.m_HandCardControl.SetCardData(NULL,0);
	
	return true;
}

//设置明手扑克
bool CGameClientDlg::SetHandOutCardControl(BYTE cbCardData[], BYTE cbCardCount, WORD wMeChairID, WORD wViewChairID)
{
	//设置过滤
	if (cbCardCount>0)
	{
		//变量定义
		BYTE cbRectifyCount=1;
		BYTE cbRectifyCard[MAX_COUNT+4]={cbCardData[0]};

		//获取属性
		BYTE cbLogicColor=m_GameLogic.GetCardLogicColor(cbCardData[0]);

		//变换扑克
		for (BYTE i=1;i<cbCardCount;i++)
		{
			//获取属性
			BYTE cbCurrentColor=m_GameLogic.GetCardLogicColor(cbCardData[i]);

			//插入判断
			if (cbCurrentColor!=cbLogicColor)
			{
				cbLogicColor=cbCurrentColor;
				cbRectifyCard[cbRectifyCount++]=255;
				cbRectifyCard[cbRectifyCount++]=cbCardData[i];
			}
			else cbRectifyCard[cbRectifyCount++]=cbCardData[i];
		}

		//设置扑克
		m_GameClientView.m_HandOutCardControl[wMeChairID][wViewChairID].SetCardData(cbRectifyCard,cbRectifyCount);

		return true;
	}
	else m_GameClientView.m_HandOutCardControl[wMeChairID][wViewChairID].SetCardData(NULL,0);
	
	return true;
}

//定时器消息
void CGameClientDlg::OnTimer(UINT nIDEvent)
{
	switch (nIDEvent)
	{
	case IDI_DISPATCH_CARD:		//派发扑克
		{
			//派发控制
			if (m_cbRemnantCardCount>0)
			{
				//获取属性
				BYTE cbCardColor=m_GameLogic.GetCardColor(m_cbDispatchCardData[m_cbDispatchCardCount]);
				BYTE cbCardValue=m_GameLogic.GetCardValue(m_cbDispatchCardData[m_cbDispatchCardCount]);

				//派发扑克
				m_cbRemnantCardCount--;
				m_cbDispatchCardCount++;

				m_GameLogic.SortCardList(m_cbDispatchCardData,m_cbDispatchCardCount);
				SetHandCardControl(m_cbDispatchCardData,m_cbDispatchCardCount);
				
				//播放声音
				PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
			}
			
			//中止判断
			if (m_cbRemnantCardCount==0) 
			{
				//删除定时器
				KillTimer(IDI_DISPATCH_CARD);
				
				//环境设置
				
				if ((IsLookonMode()==false)&&(m_cbFocusUser==GetMeChairID())) 
				{
					//ActiveGameFrame();
					//设置时间
					SetGameTimer(m_cbFocusUser,IDI_LAND_SCORE,TIME_CALL_SCORE);
					m_GameClientView.m_CallScoreInfo.ShowWindow(SW_HIDE);
					m_GameClientView.m_CallScore.ShowWindow(SW_SHOW);
				
				}
			}

			return;
		}
	case IDI_LAST_TURN_CARD:	//上轮扑克
		{
			//还原界面
			SwitchToCurrentCard();

			return;
		}

	
	case IDI_THROW_CARD_RESULT:	//甩牌结果
		{
			//还原界面
			SwitchToThrowResult();

			//当前玩家
			if ((IsLookonMode()==false)&&(m_wCurrentUser==GetMeChairID()))
			{
				if (m_bStustee==false) //ActiveGameFrame();
				m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
				m_GameClientView.m_btOutPrompt.ShowWindow(SW_SHOW);
				m_GameClientView.m_btOutCard.EnableWindow((VerdictOutCard()==true)?TRUE:FALSE);
			}

			//设置定时器
			SetGameTimer(m_wCurrentUser,IDI_OUT_CARD,TIME_OUT_CARD);

			return;
		}
	}

	__super::OnTimer(nIDEvent);
}

//开始按钮
LRESULT CGameClientDlg::OnStart(WPARAM wParam, LPARAM lParam)
{
	//设置环境
	KillGameTimer(IDI_START_GAME);
	
	//设置界面
	m_GameClientView.SetBankerUser(INVALID_CHAIR);
	m_cbValueOrder[0]=VALUE_ERROR;
	m_cbValueOrder[1]=VALUE_ERROR;
	m_cbMainValue=VALUE_ERROR;
	m_GameClientView.SetValueOrder(m_cbValueOrder[0],m_cbValueOrder[1]);
	m_GameClientView.SetDingYue(m_cbMainValue,VALUE_ERROR);
	m_GameClientView.SetJuKuang(VALUE_ERROR);

	//隐藏控件
	m_GameClientView.m_btStart.ShowWindow(FALSE);
	m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE);

	//叫分控制
	m_GameClientView.m_CallScore.ShowWindow(SW_HIDE);
	m_GameClientView.m_CallScoreInfo.ShowWindow(SW_HIDE);

	//设置扑克
	SetHandCardControl(NULL,0);
	m_GameClientView.m_CardScore.SetCardData(NULL,0);
	m_GameClientView.m_HandCardControl.SetPositively(false);
	m_GameClientView.SetMeChair(GetMeChairID());

	//明手扑克
	ZeroMemory(m_HandOutCardCount,sizeof(m_HandOutCardCount));	
	ZeroMemory(m_HandOutCardData,sizeof(m_HandOutCardData));
	m_bHandOutControl=false;

	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		for (WORD j=0;j<GAME_PLAYER;j++)
		{
			m_GameClientView.m_HandOutCardControl[i][j].SetCardData(NULL,0);
		}
	}
	for (WORD i=0;i<GAME_PLAYER;i++) m_GameClientView.m_UserCardControl[i].SetCardData(NULL,0);

	//发送消息
	SendUserReady(NULL,0);

	return 0;
}

//叫分消息
LRESULT CGameClientDlg::OnLandScore(WPARAM wParam, LPARAM lParam)
{
	//设置界面
	m_GameClientView.m_CallScore.ShowWindow(SW_HIDE);
	m_GameClientView.m_CallScoreInfo.ShowWindow(SW_SHOW);

	//发送数据
	CMD_C_CallScore LandScore;
	
	LandScore.bBossScore=(BYTE)lParam;
	LandScore.bBossColor=(BYTE)wParam;
	
	SendData(SUB_C_CALL_SCORE,&LandScore,sizeof(LandScore));
	return 0;
}

//加倍消息
LRESULT CGameClientDlg::OnMultiples(WPARAM wParam, LPARAM lParam)
{
	//设置界面
	m_GameClientView.m_CallScore.ShowWindow(SW_HIDE);


	//发送数据
	CMD_C_OnMultiples Multiples;
	Multiples.bMultiples=(BYTE)lParam;

	SendData(SUB_C_MULTIPLES,&Multiples,sizeof(Multiples));
	return 0;
}


//出牌消息
LRESULT CGameClientDlg::OnOutCard(WPARAM wParam, LPARAM lParam)
{
	//状态判断
	if (m_GameClientView.m_btOutCard.IsWindowEnabled()==FALSE) return 0;
	if (m_GameClientView.m_btOutCard.IsWindowVisible()==FALSE) return 0;

	//设置变量
	KillGameTimer(IDI_OUT_CARD);
	m_wCurrentUser=INVALID_CHAIR;

	//设置控件
	m_GameClientView.m_btOutCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btOutCard.EnableWindow(FALSE);
	m_GameClientView.m_btOutPrompt.ShowWindow(SW_HIDE);

	//设置明手变量
	WORD wMeChairID=GetMeChairID();
	BYTE bHandOutUser=(m_wBankerUser+2)%GAME_PLAYER;
	WORD wViewChairID=SwitchViewChairID(bHandOutUser);

	//设置扑克
	if(m_bHandOutControl==true)
	{
		if (m_wFirstOutUser==bHandOutUser)
		{
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				if (i!=wViewChairID) m_GameClientView.m_UserCardControl[i].SetCardData(NULL,0);
			}
		}
		//发送数据
		CMD_C_OutCard OutCard;
		OutCard.bIsHandOut[GetMeChairID()]=m_bHandOutControl;
		OutCard.bIsAutoOut=false;
		OutCard.cbCardCount=(BYTE)m_GameClientView.m_HandOutCardControl[wMeChairID][wViewChairID].GetShootCard(OutCard.cbCardData,CountArray(OutCard.cbCardData));
		SendData(SUB_C_OUT_CARD,&OutCard,sizeof(OutCard));
		
		PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));

		//预先删除
		//BYTE cbSourceCount=m_HandOutCardCount[bHandOutUser];
		//m_HandOutCardCount[bHandOutUser]-=OutCard.cbCardCount;
		//m_GameLogic.RemoveCard(OutCard.cbCardData,OutCard.cbCardCount,m_HandOutCardData[bHandOutUser],cbSourceCount);
		//
		//SetHandOutCardControl(m_HandOutCardData[bHandOutUser],m_HandOutCardCount[bHandOutUser],wMeChairID,wViewChairID);
		
	}
	else
	{
		if (m_wFirstOutUser==GetMeChairID())
		{
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				if (i!=2) m_GameClientView.m_UserCardControl[i].SetCardData(NULL,0);
			}
		}
		//发送数据
		CMD_C_OutCard OutCard;
		OutCard.bIsAutoOut=false;
		OutCard.bIsHandOut[GetMeChairID()]=m_bHandOutControl;
		OutCard.cbCardCount=(BYTE)m_GameClientView.m_HandCardControl.GetShootCard(OutCard.cbCardData,CountArray(OutCard.cbCardData));
		//m_GameLogic.SortCardList(OutCard.cbCardData,OutCard.cbCardCount);
		SendData(SUB_C_OUT_CARD,&OutCard,sizeof(OutCard));

		//预先显示
		PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
		m_GameClientView.m_UserCardControl[2].SetCardData(OutCard.cbCardData,OutCard.cbCardCount);

		//预先删除
		BYTE cbSourceCount=m_cbHandCardCount;
		m_cbHandCardCount-=OutCard.cbCardCount;
		m_GameLogic.RemoveCard(OutCard.cbCardData,OutCard.cbCardCount,m_cbHandCardData,cbSourceCount);

		//设置扑克
		SetHandCardControl(m_cbHandCardData,m_cbHandCardCount);
	}
	
	//明手控制失效
	m_bHandOutControl=false;
	m_GameClientView.m_HandOutCardControl[wMeChairID][wViewChairID].SetHandOutControl(m_bHandOutControl);
	return 0;
}

//出牌提示
LRESULT CGameClientDlg::OnOutPrompt(WPARAM wParam, LPARAM lParam)
{
	//变量定义
	tagOutCardResult OutCardResult;
	ZeroMemory(&OutCardResult,sizeof(OutCardResult));

	//搜索出牌
	BYTE cbTurnCardCount=(m_wCurrentUser==m_wFirstOutUser)?0:m_cbOutCardCount[m_wFirstOutUser][0];
	bool bSuccess=m_GameLogic.SearchOutCard(m_cbHandCardData,m_cbHandCardCount,m_cbOutCardData[m_wFirstOutUser][0],cbTurnCardCount,OutCardResult);

	//失败判断
	if (bSuccess==false)
	{
		InsertSystemString(TEXT("自动提示出牌错误，请你把此状态信息截图并给我们提供，谢谢！"));
		return false;
	}

	//设置界面
	m_GameClientView.m_btOutCard.EnableWindow(TRUE);
	m_GameClientView.m_HandCardControl.SetShootCard(OutCardResult.cbResultCard,OutCardResult.cbCardCount);

	return 0;
}


//请求离开
LRESULT CGameClientDlg::OnRuquestLeave(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

//上轮扑克
LRESULT CGameClientDlg::OnLastTurnCard(WPARAM wParam, LPARAM lParam)
{
	if (m_bLastTurn==false)
	{
		//设置扑克
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			WORD wViewChairID=SwitchViewChairID(i);
			m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(m_cbOutCardData[i][1],m_cbOutCardCount[i][1]);
		}

		//环境设置
		m_bLastTurn=true;
		m_GameClientView.SetLastTurnCard(true);
		SetTimer(IDI_LAST_TURN_CARD,3000,NULL);
	}
	else SwitchToCurrentCard();

	return 0;
}

//拖管控制
LRESULT CGameClientDlg::OnStusteeControl(WPARAM wParam, LPARAM lParam)
{
	//设置状态
	if (m_bStustee==true)
	{
		m_bStustee=false;
		m_GameClientView.m_btStusteeControl.SetButtonImage(IDB_START_TRUSTEE,AfxGetInstanceHandle(),false);
	}
	else
	{
		m_bStustee=true;
		m_GameClientView.m_btStusteeControl.SetButtonImage(IDB_STOP_TRUSTEE,AfxGetInstanceHandle(),false);
	}

	return 0;
}

//左键扑克
LRESULT CGameClientDlg::OnLeftHitCard(WPARAM wParam, LPARAM lParam)
{
	if (m_bHandOutControl==false)
	{
		//获取状态
		BYTE cbGameStatue=GetGameStatus();
		
		//留底状态
		switch (cbGameStatue)
		{
		
		case GS_UG_PLAY:	//游戏状态
			{
				//设置控件
				bool bOutCard=VerdictOutCard();
				m_GameClientView.m_btOutCard.EnableWindow(bOutCard?TRUE:FALSE);

				return 0;
			}
		}
	}
	return 0;
}

//右键扑克
LRESULT CGameClientDlg::OnRightHitCard(WPARAM wParam, LPARAM lParam)
{
	if (m_bHandOutControl==true)
	{
		//获取状态
		BYTE cbGameStatue=GetGameStatus();
		
		//留底状态
		switch (cbGameStatue)
		{
		
		case GS_UG_PLAY:	//游戏状态
			{
				//设置控件
				bool bOutCard=VerdictHandOutCard();
				
				m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
				m_GameClientView.m_btOutCard.EnableWindow(bOutCard==true?TRUE:FALSE);

				return 0;
			}
		}
	//OnOutCard(0,0);
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
