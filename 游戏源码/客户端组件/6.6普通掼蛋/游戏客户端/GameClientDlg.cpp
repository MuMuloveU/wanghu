#include "Stdafx.h"
#include "GameClient.h"
#include "GameClientDlg.h"
#include "GameOption.h"
#include ".\gameclientdlg.h"
#include "ReDialog.h"
#include "ADialog.h"
#include "AnswerDlg.h"
#include "GamePrompt.h"
//////////////////////////////////////////////////////////////////////////

//游戏定时器
#define IDI_OUT_CARD					200								//出牌定时器
#define IDI_START_GAME					202								//开始定时器
#define IDI_PAY_TRIBUTE					203								//进贡定时器
#define IDI_AI							204								//托管定时器

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientDlg, CGameFrameDlg)
	ON_MESSAGE(IDM_START,OnStart)
	ON_MESSAGE(IDM_OUT_CARD,OnOutCard)
	ON_MESSAGE(IDM_PASS_CARD,OnPassCard)
	ON_MESSAGE(IDM_PAY_TRIBUTE,OnPayTribute)
	ON_MESSAGE(IDM_LEFT_HIT_CARD,OnLeftHitCard)
	ON_MESSAGE(IDM_RIGHT_HIT_CARD,OnRightHitCard)
	ON_MESSAGE(IDM_SORT_CARD,OnSortCard)
	ON_MESSAGE(IDM_CHANGE_POSITION,OnChangePosition)
	ON_MESSAGE(IDM_AI,OnAI)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientDlg::CGameClientDlg() : CGameFrameDlg(&m_GameClientView)
{
	//游戏变量
	m_bHandCardCount	=		0;
	m_bPayTribute		=		0;
	ZeroMemory(m_bCardCount,sizeof(m_bCardCount));
	ZeroMemory(m_bHandCardData,sizeof(m_bHandCardData));
	m_bAI				=		false;
	m_bLookon			=       false;
	
	//配置变量
	m_bDeasilOrder		=		false;
	m_dwCardHSpace		=		DEFAULT_PELS;
	m_bGameEnd			=		true;
	m_wToUser			=		INVALID_CHAIR;

	//出牌变量
	m_bTurnCardType		=		short(CT_ERROR);
	m_bTurnCardCount	=		0;
	ZeroMemory(m_bTurnCardData,sizeof(m_bTurnCardData));

}

//析构函数
CGameClientDlg::~CGameClientDlg()
{
}

//初始函数
bool CGameClientDlg::InitGameFrame()
{
	//设置标题
	SetWindowText(TEXT("掼蛋游戏"));
	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//读取变量
	m_dwCardHSpace	=	AfxGetApp()->GetProfileInt(TEXT("GameOption"),TEXT("CardSpace"),DEFAULT_PELS);
    m_bDeasilOrder	=	AfxGetApp()->GetProfileInt(TEXT("GameOption"),TEXT("DeasilOrder"),FALSE)?true:false;

	//调整参数
	if ((m_dwCardHSpace>MAX_PELS)||(m_dwCardHSpace<LESS_PELS)) m_dwCardHSpace=DEFAULT_PELS;

	//配置控件
	m_GameClientView.SetUserOrder(m_bDeasilOrder);
	m_GameClientView.m_HandCardControl.SetCardSpace(m_dwCardHSpace,0,20);

	return true;
}

//重置框架
void CGameClientDlg::ResetGameFrame()
{

	//游戏变量
	m_bHandCardCount=0;
	ZeroMemory(m_bCardCount,sizeof(m_bCardCount));
	ZeroMemory(m_bHandCardData,sizeof(m_bHandCardData));
	m_wToUser			=		INVALID_CHAIR;
	m_bAI				=       false;
	m_bLookon			=       false;
	

	//出牌变量
	m_bTurnCardType   =		short(CT_ERROR);
	m_bTurnCardCount  =		0;
	ZeroMemory(m_bTurnCardData,sizeof(m_bTurnCardData));
}

//游戏设置
void CGameClientDlg::OnGameOptionSet()
{
	//创建变量
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
	//定时处理
	switch(nTimerID)
	{
	
	//出牌超时
	case IDI_OUT_CARD:
		{
			
		//超时判断
		 if(nElapse==0||m_bAI&&(m_wTime-nElapse)==3)
		 {
			 //当前用户
			 if ((IsLookonMode()==false)&&(wChairID==GetMeChairID()))
				 //自动出牌
				 AutoOutCard();
			 return false;
		 }
         
		 //提示声音
		 if(nElapse<5&&(IsLookonMode()==false)&&(wChairID==GetMeChairID()))
			 PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
		 return true;
		}
	
	//开始游戏
	case IDI_START_GAME:		
		{
			if (nElapse==0)
			{
				if ((IsLookonMode()==false)&&(wChairID==GetMeChairID())) OnStart(0,0);
				return false;
			}
			if ((nElapse<=3)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false)) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));

			return true;
		}
	//进贡超时
	case IDI_PAY_TRIBUTE:
		{
		   if (nElapse==0)
			{
				if(m_bPayTribute==2)
				{
					BYTE bCard;
					bCard=m_GameClientView.m_HandCardControl.GetMarkCard();
					OnPayTribute(1,bCard);
				}
				else
				{
				    OnPayTribute(1,m_bHandCardData[27]);
				}
				
				return false;
			}
			if ((nElapse<=3)&&(wChairID==GetMeChairID())&&(IsLookonMode()==false)) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
			return true;
		}
	default:
		break;
	}
   return true;
}

//旁观状态
void CGameClientDlg::OnLookonChanged(bool bLookonUser, const void * pBuffer, WORD wDataSize)
{

}

//网络消息
bool CGameClientDlg::OnGameMessage(WORD wSubCmdID, const void * pBuffer, WORD wDataSize)
{
	

	switch(wSubCmdID)
	{
	case SUB_S_GAME_START:
		return OnSubGameStart(pBuffer,wDataSize);
	case SUB_S_OUT_CARD:
		return OnSubOutCard(pBuffer,wDataSize);
	case SUB_S_PASS_CARD:
		return OnSubPassCard(pBuffer,wDataSize);
	case SUB_S_GAME_END:
		return OnSubGameEnd(pBuffer,wDataSize);
	case SUB_S_PAY_TRIBUTE:
		return OnSubPayTribute(pBuffer,wDataSize);
	case SUB_S_END:
		{
			m_bGameEnd=true;
			m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
			SetGameTimer(GetMeChairID(),IDI_START_GAME,60);
		}
		return true;
	case SUB_S_REQUEST_QUIT:
		
		return OnRequest(pBuffer);
	case  SUB_S_ANSWER_QUIT:
		return OnAnswer(pBuffer);
	case SUB_S_CARD:
		{
			//设置控件
			m_GameClientView.m_LeaveCardControl[0].SetDisplayFlag(true);
			//变量定义
			CMD_S_CARD *pSCard=(CMD_S_CARD *)pBuffer;
			ASSERT(wDataSize==sizeof(CMD_S_CARD));
			//设置扑克
			m_GameClientView.m_LeaveCardControl[0].SetCardData(pSCard->bCardData,pSCard->bCardCount);

			return true;
		}
	case SUB_S_SYSTEM_MESSAGE:
		{
			//获取消息
			CMD_S_MESSAGE * pMessage=(CMD_S_MESSAGE *)pBuffer;
			InsertSystemString(pMessage->szBuffer);
			return true;
		}
	case SUB_S_LOOKON:
		if(IsLookonMode()==true)
		{
				bool * pb=(bool *)pBuffer;
				m_bLookon=*pb;
				if(m_bLookon)
					m_GameClientView.m_HandCardControl.SetDisplayFlag(m_bLookon);
				return true;
		}
		if(MessageBox(TEXT("是否允许旁观者看牌"),TEXT("旁观请求"),MB_YESNO)==IDYES)
		{
			WORD   wChairID=GetMeChairID();
			SendData(SUB_C_LOOKON,&wChairID,sizeof(wChairID));
		}
		return true;
	default:
		break;
    }
	return false;
}

//游戏场景
bool CGameClientDlg::OnGameSceneMessage(BYTE cbGameStation, bool bLookonOther, const void * pBuffer, WORD wDataSize)
{
	
	switch(cbGameStation)
	{
	     //空闲状态  
	case GS_RF_FREE:
		  {
		   //校验数据
		  if(wDataSize!=sizeof(CMD_S_StatusFree)) return false;
          
		  //显示控件
		  if(IsLookonMode()== false)
		  {
			m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
			m_GameClientView.m_btStart.SetFocus();
			SetGameTimer(GetMeChairID(),IDI_START_GAME,60);
		  }
		  //设置扑克
		  if (IsLookonMode()==false) m_GameClientView.m_HandCardControl.SetDisplayFlag(true);
          
		  return true;
		  }
		  //游戏状态
	case GS_RF_PLAYING:		
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pBuffer;
			
			//状态设置
			SetGameStatus(GS_RF_PLAYING);
			//设置变量
			m_wBankerUser=pStatusPlay->wBankerUser;
			m_bTurnCardCount=pStatusPlay->bTurnCardCount;
			m_bHandCardCount=pStatusPlay->bCardCount[GetMeChairID()];
			m_bTurnCardType=m_GameLogic.GetCardType(pStatusPlay->bTurnCardData,m_bTurnCardCount);
			CopyMemory(m_bHandCardData,pStatusPlay->bCardData,m_bHandCardCount);
			CopyMemory(m_bTurnCardData,pStatusPlay->bTurnCardData,pStatusPlay->bTurnCardCount);
            m_RoomType			=		pStatusPlay->RoomType;
			m_wServerType			=		pStatusPlay->wServerType;

			//参数设置
			if(GetMeChairID()%2==0)
				m_GameClientView.m_Series					=	pStatusPlay->series;
			else
				{
				m_GameClientView.m_Series.bCurrentSeries	=	pStatusPlay->series.bCurrentSeries;
				m_GameClientView.m_Series.bOtherSeries		=	pStatusPlay->series.bOurSeries;
				m_GameClientView.m_Series.bOurSeries		=	pStatusPlay->series.bOtherSeries;
				}
			
			if(pStatusPlay->series.bCurrentSeries<14)
				m_GameLogic.m_bSeries				=   pStatusPlay->series.bCurrentSeries|0x20;
			else
				m_GameLogic.m_bSeries				=	0x21;

			m_GameClientView.m_HongTao			=	m_GameLogic.m_bSeries;
			
			for(BYTE i=0;i<GAME_PLAYER;i++)
			{
				WORD    wChairID=SwitchViewChairID(i);
				m_GameClientView.m_bGameResult[wChairID]=pStatusPlay->m_iGameResult[i];
			}
             
			m_GameClientView.SetGamePlay(true);
			//设置界面
			for (BYTE i=0;i<GAME_PLAYER;i++)
			{
				WORD wViewChairID=SwitchViewChairID(i);
				m_bCardCount[i]=pStatusPlay->bCardCount[i];
				m_GameClientView.SetCardCount(wViewChairID,pStatusPlay->bCardCount[i]);
				m_GameClientView.m_lAllTurnScore[wViewChairID]=pStatusPlay->lAllTurnScore[i];
				m_GameClientView.m_lLastTurnScore[wViewChairID]=pStatusPlay->lLastTurnScore[i];
			}
			m_GameClientView.m_HandCardControl.SetCardData(m_bHandCardData,m_bHandCardCount);

			//玩家设置
			if ((IsLookonMode()==false)&&(pStatusPlay->wCurrentUser==GetMeChairID()))
			{
				m_GameClientView.m_btOutCard.EnableWindow(FALSE);
				m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
				m_GameClientView.m_btPassCard.ShowWindow(SW_SHOW);
				m_GameClientView.m_btPassCard.EnableWindow((m_bTurnCardCount!=0)?TRUE:FALSE);
				
			}
			//辅助按钮设置
			if(IsLookonMode()==false)
			{
				m_GameClientView.m_btResetCard.ShowWindow(SW_SHOW);
				m_GameClientView.m_btResetCard.EnableWindow(true);
				m_GameClientView.m_btColorSort.ShowWindow(SW_SHOW);
				m_GameClientView.m_btColorSort.EnableWindow(true);
				m_GameClientView.m_btAI.ShowWindow(SW_SHOW);
				m_GameClientView.m_btAI.EnableWindow(true);
			}
			//桌面设置
			if (m_bTurnCardCount!=0)
			{
				WORD wViewChairID=SwitchViewChairID(pStatusPlay->wLastOutUser);
				m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(m_bTurnCardData,m_bTurnCardCount);
			}

			//设置定时器
			WORD wTimeCount=30;
			SetGameTimer(pStatusPlay->wCurrentUser,IDI_OUT_CARD,wTimeCount);

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

//开始游戏
LRESULT CGameClientDlg::OnStart(WPARAM wParam,LPARAM lParam)
{
   //设置变量
	m_bHandCardCount	=	0;
	m_bTurnCardCount	=	0;
	m_bTurnCardType		=	CT_ERROR;

	
	ZeroMemory(m_bHandCardData,sizeof(m_bHandCardData));
	ZeroMemory(m_bTurnCardData,sizeof(m_bTurnCardData));
    
	//设置界面
	KillGameTimer(IDI_START_GAME);
	m_GameClientView.SetCardCount(INVALID_CHAIR,0);
	m_GameClientView.SetPassFlag(INVALID_CHAIR,false);
	
	//隐藏控件
	m_GameClientView.m_btStart.ShowWindow(FALSE);
	m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE);

	
	//设置扑克控件
	m_GameClientView.m_HandCardControl.SetCardData(NULL,0);
	m_GameClientView.m_HandCardControl.SetPositively(false);
	for(BYTE i=0;i<3;i++)
		m_GameClientView.m_LeaveCardControl[i].SetCardData(NULL,0);
	for (WORD i=0;i<GAME_PLAYER;i++) m_GameClientView.m_UserCardControl[i].SetCardData(NULL,0);
	
	//发送准备消息
	SendUserReady(NULL,0);
	
	return 0;
}

//出牌处理
LRESULT CGameClientDlg::OnOutCard(WPARAM wParam,LPARAM lParam)
{
	//状态判断
	if ((m_GameClientView.m_btOutCard.IsWindowEnabled()==FALSE)
		||(m_GameClientView.m_btOutCard.IsWindowVisible()==FALSE)) 
	return 0;

	//发送数据
	CMD_C_OutCard OutCard;
	OutCard.bCardCount=(BYTE)m_GameClientView.m_HandCardControl.GetShootCard(OutCard.bCardData,CountArray(OutCard.bCardData));
	if(OutCard.bCardCount==0)
	{	
		m_GameClientView.m_btOutCard.EnableWindow(FALSE);
		return 0;
	}
	m_GameLogic.SortCardList(OutCard.bCardData,OutCard.bCardCount);
	SendData(SUB_C_OUT_CARD,&OutCard,sizeof(OutCard)-sizeof(OutCard.bCardData)+OutCard.bCardCount*sizeof(BYTE));

	
	//设置界面
	KillGameTimer(IDI_OUT_CARD);
	m_wTime=0;
	m_GameClientView.m_btOutCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btOutCard.EnableWindow(FALSE);
	m_GameClientView.m_btPassCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btPassCard.EnableWindow(FALSE);
	
	m_GameClientView.m_UserCardControl[2].SetCardData(OutCard.bCardData,OutCard.bCardCount);

	//预先删除
	BYTE bSourceCount=m_bHandCardCount;
	m_bHandCardCount-=OutCard.bCardCount;
	m_GameLogic.RemoveCard(OutCard.bCardData,OutCard.bCardCount,m_bHandCardData,bSourceCount);
	m_GameClientView.m_HandCardControl.SetCardData(m_bHandCardData,m_bHandCardCount);
	m_bTurnCardType=m_GameLogic.GetCardType(OutCard.bCardData,OutCard.bCardCount);

	//预先显示
	if(((BYTE)m_bTurnCardType)>=CT_SI_ZHANG_BOMB)
		PlayGameSound(AfxGetInstanceHandle(),TEXT("BOMB"));
	else
		PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
	
	//隐藏排序按钮
	if(m_bHandCardCount==0)
	{
		m_GameClientView.m_btResetCard.ShowWindow(SW_HIDE);
		m_GameClientView.m_btColorSort.ShowWindow(SW_HIDE);
	}
	return 0;
}

//放弃处理
LRESULT CGameClientDlg::OnPassCard(WPARAM wParam,LPARAM lParam)
{
	//状态判断
	if(m_GameClientView.m_btPassCard.IsWindowVisible()==false)
		return 0;

	//预先显示
	PlayGameSound(AfxGetInstanceHandle(),TEXT("BRUSH_CARD"));
	
	//设置界面
	KillGameTimer(IDI_OUT_CARD);
	m_GameClientView.m_btOutCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btPassCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btOutCard.EnableWindow(false);
	m_GameClientView.m_btPassCard.EnableWindow(false);

	//发送消息
	SendData(SUB_C_PASS_CARD);

	//预先显示
	m_GameClientView.m_UserCardControl[2].SetCardData(NULL,0);
	m_GameClientView.SetPassFlag(2,true);
	PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
	m_GameClientView.m_HandCardControl.SetCardData(m_bHandCardData,m_bHandCardCount);

	return 0;
}

//进贡处理
LRESULT CGameClientDlg::OnPayTribute(WPARAM wParam,LPARAM lParam)
{
	ASSERT(m_bPayTribute!=0);
	if(wParam==0)
	{
		//有效判断
		if(m_bPayTribute==2&&m_GameClientView.m_btPayTribute.IsWindowEnabled()==false)
			return 0;
		if(m_bPayTribute==1&&m_GameClientView.m_btChangeCard.IsWindowEnabled()==false)
			return 0;
	}
	//创建消息
	CMD_C_PayTribute PayTribute;
	if(wParam==0)
	{
		DWORD dwCardCount;
		dwCardCount=m_GameClientView.m_HandCardControl.GetShootCard(&PayTribute.bCard,1);
		if(dwCardCount!=1)
		{
			if(m_bPayTribute==2)
				m_GameClientView.m_btPayTribute.EnableWindow(false);
			if(m_bPayTribute==1)
				m_GameClientView.m_btChangeCard.EnableWindow(false);
			return 0;
		}
	}
	else
		 PayTribute.bCard	=	(BYTE)lParam;

	//删除定时
	 KillGameTimer(IDI_PAY_TRIBUTE);
	//调整界面
	if(m_bPayTribute==2){
		m_GameClientView.m_btPayTribute.ShowWindow(SW_HIDE);
		m_GameClientView.m_btPayTribute.EnableWindow(false);
	}
	else if(m_bPayTribute==1)
	{
		m_GameClientView.m_btChangeCard.ShowWindow(SW_HIDE);
		m_GameClientView.m_btChangeCard.EnableWindow(false);
	}
	
	
	PayTribute.wUser		=	GetMeChairID();
	PayTribute.wToUser		=	m_wToUser;
	
	if(m_bPayTribute==2)
		PayTribute.bPayType	=	enJin_Gong;
	else
		PayTribute.bPayType	=	enHui_Gong;
	
	SendData(SUB_C_PAY_TRIBUTE,&PayTribute,sizeof(PayTribute));

	//预先显示
	m_GameClientView.m_UserCardControl[2].SetCardData(&PayTribute.bCard,1);

	//预先删除
	m_GameLogic.RemoveCard(&PayTribute.bCard,1,m_bHandCardData,m_bHandCardCount);
	m_bHandCardCount--;
	m_GameClientView.m_HandCardControl.SetCardData(m_bHandCardData,m_bHandCardCount);
	m_bPayTribute = 0;
    

	
	return 0;
}

//游戏开始
bool	CGameClientDlg::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;
	//消息处理
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;
    
	//状态设置
	SetGameStatus(GS_RF_PLAYING);
	m_GameClientView.SetGamePlay(true);
	m_RoomType			=		pGameStart->RoomType;
	if(m_RoomType==enRoomType_All)
		m_bGameEnd=false;
	m_wServerType=pGameStart->wServerType;

	//设置变量
	m_bTurnCardCount=0;
	m_bTurnCardType=CT_ERROR;
	m_wBankerUser=pGameStart->wBankerUser;
	ZeroMemory(m_bTurnCardData,sizeof(m_bTurnCardData));

	//设置界面
	for(BYTE i=0;i<3;i++)
		m_GameClientView.m_LeaveCardControl[i].SetCardData(NULL,0);
	//调整界面
	ZeroMemory(m_GameClientView.m_bKanGong,sizeof(bool)*4);
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_GameClientView.SetPassFlag(i,false);
		m_GameClientView.m_UserCardControl[i].SetCardData(NULL,0);
	}
	
	if (IsLookonMode()==true)
	{
		m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE);
		m_GameClientView.m_LeaveCardControl[0].SetCardData(NULL,0);
		m_GameClientView.m_LeaveCardControl[1].SetCardData(NULL,0);
		m_GameClientView.m_LeaveCardControl[2].SetCardData(NULL,0);
	}

	//设置扑克
	m_bHandCardCount=CountArray(pGameStart->cbCardData);
	CopyMemory(m_bHandCardData,pGameStart->cbCardData,sizeof(pGameStart->cbCardData));
	m_GameClientView.m_HandCardControl.SetCardData(pGameStart->cbCardData,m_bHandCardCount);
    
    //参数设置
	if(GetMeChairID()%2==0)
		m_GameClientView.m_Series					=	pGameStart->series;
	else
	{
		m_GameClientView.m_Series.bCurrentSeries	=	pGameStart->series.bCurrentSeries;
		m_GameClientView.m_Series.bOtherSeries		=	pGameStart->series.bOurSeries;
		m_GameClientView.m_Series.bOurSeries		=	pGameStart->series.bOtherSeries;
	}
	
	
	if(pGameStart->series.bCurrentSeries<14)
	  m_GameLogic.m_bSeries				=   pGameStart->series.bCurrentSeries|0x20;
	else
	  m_GameLogic.m_bSeries				=	0x21;
	
	m_GameClientView.m_HongTao			=	m_GameLogic.m_bSeries;
	m_GameClientView.m_HandCardControl.m_bSeries=m_GameLogic.m_bSeries;
	for(BYTE i=0;i<GAME_PLAYER;i++)
	{
		WORD    wChairID=SwitchViewChairID(i);
		m_GameClientView.m_bGameResult[wChairID]=pGameStart->m_iGameResult[i];
	}

	
	//扑克数目
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_bCardCount[i]=CountArray(pGameStart->cbCardData);
		m_GameClientView.SetCardCount(SwitchViewChairID(i),m_bCardCount[i]);
	}
     
	//控制设置
	bool bPlayerMode=(IsLookonMode()==false);
	m_GameClientView.m_HandCardControl.SetPositively(bPlayerMode);
	m_GameClientView.m_HandCardControl.SetDisplayFlag(bPlayerMode);
	if(IsLookonMode()==true)
		m_GameClientView.m_HandCardControl.SetDisplayFlag(m_bLookon);
    
	if(IsLookonMode()==false)
	{
		m_GameClientView.m_btResetCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btResetCard.EnableWindow(true);
		m_GameClientView.m_btColorSort.ShowWindow(SW_SHOW);
		m_GameClientView.m_btColorSort.EnableWindow(true);
		m_GameClientView.m_btAI.ShowWindow(SW_SHOW);
		m_GameClientView.m_btAI.EnableWindow(true);
	}
	
	//当前玩家
	if ((IsLookonMode()==false)&&(pGameStart->wCurrentUser==GetMeChairID()))
	{
		ActiveGameFrame();
		m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btOutCard.EnableWindow(FALSE);
		m_GameClientView.m_btPassCard.EnableWindow(FALSE);
		m_GameClientView.m_btPassCard.ShowWindow(SW_SHOW);
	}

	//播放声音
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));
	//设置定时
	WORD  wTimeCount=50;
	
	if(pGameStart->wCurrentUser!=INVALID_CHAIR)
	{  
		//显示开始用户
		WORD   wChairID=SwitchViewChairID(pGameStart->wCurrentUser);
		m_GameClientView.SetFirstUser(wChairID,true,pGameStart->bLiangCard);
		SetGameTimer(pGameStart->wCurrentUser,IDI_OUT_CARD,wTimeCount);
		if(m_bAI)
				m_wTime=m_GameClientView.GetUserTimer(2);
	}
		
	return true;
}

///左键扑克
LRESULT CGameClientDlg::OnLeftHitCard(WPARAM wParam, LPARAM lParam)
{
	bool bOutCard=false;
	PlayGameSound(AfxGetInstanceHandle(),TEXT("HIT_CARD"));
	//设置控件
		//正常出牌
	if(m_bPayTribute==0)
	{
		bOutCard=VerdictOutCard();
		m_GameClientView.m_btOutCard.EnableWindow(bOutCard);
	}
	else
	{
		
		//获取扑克
		BYTE bCardData[MAX_COUNT];
		BYTE bShootCount=(BYTE)m_GameClientView.m_HandCardControl.GetShootCard(bCardData,CountArray(bCardData));
		if(bShootCount!=1)
		{
			 bOutCard=false;
		}
		else
		{
			//进贡大牌
			if(m_bPayTribute==2){
				BYTE bCard;
				bCard=m_GameClientView.m_HandCardControl.GetMarkCard();
				if(m_GameLogic.GetCardLogicValue(bCard)==m_GameLogic.GetCardLogicValue(bCardData[0])
					&&bCardData[0]!=m_GameLogic.m_bSeries)
					bOutCard=true;
				else
					bOutCard=false;
			}else if(m_bPayTribute==1)
				bOutCard=true;
		}
        if(m_bPayTribute==2)
			m_GameClientView.m_btPayTribute.EnableWindow(bOutCard);
		else if(m_bPayTribute==1)
			m_GameClientView.m_btChangeCard.EnableWindow(bOutCard);
		
	}
	return 0;
}

//右键扑克
LRESULT CGameClientDlg::OnRightHitCard(WPARAM wParam, LPARAM lParam)
{
	//用户出牌
	OnOutCard(0,0);
	if(m_bPayTribute!=0)
		OnPayTribute(0,0);

	return 0;
}

// 出牌判断
bool CGameClientDlg::VerdictOutCard(void)
{
	//状态判断
	if (m_GameClientView.m_btOutCard.IsWindowVisible()==FALSE) return false;

	//获取扑克
	BYTE bCardData[MAX_COUNT];
	BYTE bShootCount=(BYTE)m_GameClientView.m_HandCardControl.GetShootCard(bCardData,CountArray(bCardData));
    

	//错误判断
	if(bShootCount>10||bShootCount==0) return false;
	
	
	//出牌判断
	if (bShootCount>0L)
	{
		//排列扑克
		m_GameLogic.SortCardList(bCardData,bShootCount);

		//分析类型
	    short bCardType=m_GameLogic.GetCardType(bCardData,bShootCount);

		//类型判断
		if (bCardType==CT_ERROR) return false;
		//跟牌判断
		if (m_bTurnCardCount==0) return true;

		return m_GameLogic.CompareCard(bCardData,m_bTurnCardData,bShootCount,m_bTurnCardCount);
	}

	return false;
}

//用户出牌
bool CGameClientDlg::OnSubOutCard(const void * pBuffer, WORD wDataSize)
{
	//变量定义
	CMD_S_OutCard * pOutCard=(CMD_S_OutCard *)pBuffer;
	WORD wHeadSize=sizeof(CMD_S_OutCard)-sizeof(pOutCard->bCardData);
    
	//效验数据
	if (wDataSize<wHeadSize) return false;
	if (wDataSize!=(wHeadSize+pOutCard->bCardCount*sizeof(pOutCard->bCardData[0]))) return false;

	//删除定时器
	KillGameTimer(IDI_OUT_CARD);
	m_GameClientView.SetFirstUser(INVALID_CHAIR,false);

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

	//记录出牌
	m_bTurnCardCount=pOutCard->bCardCount;
	m_bTurnCardType=m_GameLogic.GetCardType(pOutCard->bCardData,pOutCard->bCardCount);
	CopyMemory(m_bTurnCardData,pOutCard->bCardData,sizeof(BYTE)*pOutCard->bCardCount);

	//自己扑克
	if ((IsLookonMode()==true)&&(pOutCard->wOutCardUser==GetMeChairID()))
	{
		//删除扑克 
		BYTE bSourceCount=m_bHandCardCount;
		m_bHandCardCount-=pOutCard->bCardCount;
		m_GameLogic.RemoveCard(pOutCard->bCardData,pOutCard->bCardCount,m_bHandCardData,bSourceCount);

		//设置界面
		m_GameClientView.m_HandCardControl.SetCardData(m_bHandCardData,m_bHandCardCount);
	}

	//最大判断
	if (pOutCard->wCurrentUser==pOutCard->wOutCardUser)
	{
		//设置变量
		m_bTurnCardCount=0;
		m_bTurnCardType=CT_ERROR;
		ZeroMemory(m_bTurnCardData,sizeof(m_bTurnCardData));

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

		//环境设置
		PlayGameSound(AfxGetInstanceHandle(),TEXT("MOST_CARD"));
	}

	//播放声音
	if ((IsLookonMode()==true)||(GetMeChairID()!=pOutCard->wOutCardUser))
	{	
		if(((BYTE)m_bTurnCardType)>=CT_SI_ZHANG_BOMB)
			PlayGameSound(AfxGetInstanceHandle(),TEXT("BOMB"));
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
		m_GameClientView.m_btOutCard.EnableWindow((VerdictOutCard()==true)?TRUE:FALSE);
		m_GameClientView.m_btPassCard.EnableWindow((VerdictPassCard()==true)?TRUE:FALSE);
	}
    
	if(IsLookonMode()==false&&m_bTurnCardCount>m_bHandCardCount&&m_bHandCardCount<=3&&(pOutCard->wCurrentUser==GetMeChairID()))
	{
		WORD wTimeCount=2;
		SetGameTimer(pOutCard->wCurrentUser,IDI_OUT_CARD,wTimeCount);
		return true;
	}
	
	//设置定时器
	if (pOutCard->wCurrentUser!=INVALID_CHAIR)
	{
		WORD wTimeCount=30;
		SetGameTimer(pOutCard->wCurrentUser,IDI_OUT_CARD,wTimeCount);
		if(m_bAI)
					m_wTime=m_GameClientView.GetUserTimer(2);
	}

	return true;
}

//不出判断
bool CGameClientDlg::VerdictPassCard()
{   
	//首出判断
	if (m_bTurnCardCount==0) return false;
    
	return true;
}

//不出处理
bool CGameClientDlg::OnSubPassCard(const void *pBuffer, WORD wDataSize)
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
		m_bTurnCardType=CT_ERROR;
		ZeroMemory(m_bTurnCardData,sizeof(m_bTurnCardData));
		
		for(BYTE i=0;i<GAME_PLAYER;i++)
		{
			WORD wViewChairID=SwitchViewChairID(i);
			if(m_GameClientView.m_bPass[wViewChairID]==false)
				m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);
		}
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
		m_GameClientView.m_btOutCard.EnableWindow((VerdictOutCard()==true)?TRUE:FALSE);
		m_GameClientView.m_btPassCard.EnableWindow((VerdictPassCard()==true)?TRUE:FALSE);
		
	}

	//播放声音
	if ((IsLookonMode()==true)||(pPassCard->wPassUser!=GetMeChairID()))	
			PlayGameSound(AfxGetInstanceHandle(),TEXT("BRUSH_CARD"));
	
	if(IsLookonMode()==false&&m_bTurnCardCount>m_bHandCardCount&&m_bHandCardCount<=3&&(pPassCard->wCurrentUser==GetMeChairID()))
	{
		WORD wTimeCount=2;
		SetGameTimer(pPassCard->wCurrentUser,IDI_OUT_CARD,wTimeCount);
		return true;
	}
	//设置时间
	WORD wTimeCount=30;
	SetGameTimer(pPassCard->wCurrentUser,IDI_OUT_CARD,wTimeCount);
	if(m_bAI)
			m_wTime=m_GameClientView.GetUserTimer(2);
	


	return true;

}

//用户进贡
bool CGameClientDlg::OnSubPayTribute(const void *pBuffer,WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_PayTribute));
	if (wDataSize!=sizeof(CMD_S_PayTribute)) return false;

	//消息处理
	CMD_S_PayTribute *pPayTribute=(CMD_S_PayTribute *)pBuffer;
	if(pPayTribute->bPayStatus==enKang_Gong)
	{
		for(WORD i=0;i<GAME_PLAYER;i++)
		{
			WORD wChairID=SwitchViewChairID(i);
			if(pPayTribute->bPayType[i]==2)
			{
				m_GameClientView.m_bKanGong[wChairID]=true;
			}
		}
		return true;
	
	}
	if(pPayTribute->bPayStatus==enJin_Gong)
	{

		if(pPayTribute->bPayType[GetMeChairID()]==2)
		{
			m_bPayTribute=pPayTribute->bPayType[GetMeChairID()];
			
			m_GameClientView.m_btPayTribute.ShowWindow(SW_SHOW);
			m_GameClientView.m_btPayTribute.EnableWindow(false);
			SetGameTimer(GetMeChairID(),IDI_PAY_TRIBUTE,60);
			BYTE i=0;
			while(m_GameLogic.m_bSeries==m_bHandCardData[i])i++;
			m_GameClientView.m_HandCardControl.MarkCard(m_bHandCardData[i],m_GameLogic.m_bSeries);
		}
	}
	else if(pPayTribute->bPayStatus==enHui_Gong)
	{
		
		
		m_bPayTribute=pPayTribute->bPayType[GetMeChairID()];
		

		//获取进贡
		m_GameLogic.Add(pPayTribute->bCard,m_bHandCardData,m_bHandCardCount);
		m_bHandCardCount++;
		m_wToUser=pPayTribute->wFromUser;

		//更新界面
		m_GameClientView.m_HandCardControl.SetCardData(m_bHandCardData,m_bHandCardCount);
		for(BYTE i=0;i<m_bHandCardCount;i++)
		{
			if(m_bHandCardData[i]==pPayTribute->bCard)
			{
				m_GameClientView.m_HandCardControl.SetShootCard(&i,1);
				break;
			}
		}
		m_GameClientView.m_btChangeCard.ShowWindow(SW_SHOW);
		m_GameClientView.m_btChangeCard.EnableWindow(VerdictChangeCard());
		SetGameTimer(GetMeChairID(),IDI_PAY_TRIBUTE,60);
		
	}
	else if(pPayTribute->bPayStatus==enGet_Hui_Gong)
	{
		
		//获取进贡
		m_GameLogic.Add(pPayTribute->bCard,m_bHandCardData,m_bHandCardCount);
		m_bHandCardCount++;
  
		//更新界面
		m_GameClientView.m_HandCardControl.SetCardData(m_bHandCardData,m_bHandCardCount);
		for(BYTE i=0;i<m_bHandCardCount;i++)
		{
			if(m_bHandCardData[i]==pPayTribute->bCard)
			{
				m_GameClientView.m_HandCardControl.SetShootCard(&i,1);
				break;
			}
		}
	}
	else{
			if(pPayTribute->wCurrentUser==INVALID_CHAIR)
			{
				
				if(IsLookonMode()==true||pPayTribute->wFromUser!=GetMeChairID())
					m_GameClientView.m_UserCardControl[SwitchViewChairID(pPayTribute->wFromUser)].SetCardData(
					&(pPayTribute->bCard),1);
				if(pPayTribute->bPayType[pPayTribute->wFromUser]==2)
				{
					WORD   wChairID=SwitchViewChairID(pPayTribute->wFromUser);
					m_GameClientView.ShowJinGong(wChairID,true);
				}

			}
			else if(pPayTribute->wCurrentUser==GetMeChairID()&&IsLookonMode()==false)
			{
				m_GameClientView.ShowJinGong(INVALID_CHAIR,false);
				//清理界面
				for(BYTE i=0;i<GAME_PLAYER;i++)
					m_GameClientView.m_UserCardControl[i].SetCardData(NULL,0);

				//显示控件
				ActiveGameFrame();
				m_GameClientView.m_btOutCard.ShowWindow(SW_SHOW);
				m_GameClientView.m_btOutCard.EnableWindow(VerdictOutCard());
				m_GameClientView.m_btPassCard.ShowWindow(SW_SHOW);
				m_GameClientView.m_btPassCard.EnableWindow(VerdictPassCard());
        
				WORD   wChairID=SwitchViewChairID(pPayTribute->wCurrentUser);
				m_GameClientView.SetFirstUser(wChairID,true);
				SetGameTimer(pPayTribute->wCurrentUser,IDI_OUT_CARD,50);
				if(m_bAI)
					m_wTime=m_GameClientView.GetUserTimer(2);
			}
			else
			{
				m_GameClientView.ShowJinGong(INVALID_CHAIR,false);
				WORD  wTimeCount=50;
				//清理界面
				for(BYTE i=0;i<GAME_PLAYER;i++)
					m_GameClientView.m_UserCardControl[i].SetCardData(NULL,0);
	
				if(pPayTribute->wCurrentUser!=INVALID_CHAIR)
				{  
					//显示开始用户
					WORD   wChairID=SwitchViewChairID(pPayTribute->wCurrentUser);
					m_GameClientView.SetFirstUser(wChairID,true);
					SetGameTimer(pPayTribute->wCurrentUser,IDI_OUT_CARD,wTimeCount);
				}
			}
	}
   return true;
}

//自动出牌
void CGameClientDlg::AutoOutCard(void)
{
     if(m_bTurnCardCount==0)
	 {
		 //界面调整
		 m_GameClientView.m_btOutCard.EnableWindow(false);
		 m_GameClientView.m_btOutCard.ShowWindow  (SW_HIDE);
		 m_GameClientView.m_btPassCard.EnableWindow(false);
		 m_GameClientView.m_btPassCard.ShowWindow (SW_HIDE);

		 //随意出牌
		 BYTE bOutCard=m_bHandCardData[m_bHandCardCount-1];

		 //发送消息
		 CMD_C_OutCard OutCard;
		 OutCard.bCardCount		=	1;
		 OutCard.bCardData[0]	=	bOutCard;

		 SendData(SUB_C_OUT_CARD,&OutCard,sizeof(OutCard)-
			 sizeof(OutCard.bCardData)+OutCard.bCardCount*sizeof(BYTE));

		//预先处理
		PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
		m_GameClientView.m_UserCardControl[2].SetCardData(OutCard.bCardData,OutCard.bCardCount);

		//预先删除
		BYTE bSourceCount=m_bHandCardCount;
		m_bHandCardCount-=OutCard.bCardCount;
		m_GameLogic.RemoveCard(OutCard.bCardData,OutCard.bCardCount,m_bHandCardData,bSourceCount);
		m_GameClientView.m_HandCardControl.SetCardData(m_bHandCardData,m_bHandCardCount);
	}
	 else if(m_bAI)
	 {
		 //游戏提示
		 CGamePrompt                     m_Prompt;
		 m_Prompt.m_bSeries		=	m_GameLogic.m_bSeries;
		 m_Prompt.m_bTurnCardType	=	m_bTurnCardType;
		 m_Prompt.m_bCardCount=m_bHandCardCount;
		 CopyMemory(m_Prompt.m_bCardData,m_bHandCardData,m_bHandCardCount);
		 m_Prompt.Prompt();
		 DWORD position =(DWORD)m_Prompt.OutCardList.GetCount();
		 for(DWORD i=0;i<position;i++)
		 {
			 m_GameClientView.m_HandCardControl.ShootAllCard(false);
			 m_GameClientView.m_HandCardControl.SetShootCard(m_Prompt.OutCardList[i].bCardLocation,m_Prompt.OutCardList[i].bCardCount);
			 if(VerdictOutCard())
			 {
				 m_GameClientView.m_btOutCard.EnableWindow(true);
				 OnOutCard(0,0);
				 return ;
			 }

		 }
		OnPassCard(0,0);
	 }
	 else
		 OnPassCard(0,0);
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
	m_GameClientView.SetGamePlay(false);

	//删除定时器
	KillGameTimer(IDI_OUT_CARD);

	//隐藏控件
	m_GameClientView.m_btOutCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btPassCard.ShowWindow(SW_HIDE);
    //排序按钮
	m_GameClientView.m_btResetCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btColorSort.ShowWindow(SW_HIDE);
	m_GameClientView.m_btChangeCard.ShowWindow(SW_HIDE);
	m_GameClientView.m_btPayTribute.ShowWindow(SW_HIDE);

	//禁用控件
	m_GameClientView.m_btOutCard.EnableWindow(FALSE);
	m_GameClientView.m_btPassCard.EnableWindow(FALSE);

	//设置积分
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		const tagUserData * pUserData=GetUserData(i);
		m_GameClientView.m_ScoreView.SetGameScore(i,pUserData->szName,pGameEnd->lGameScore[i]);
		m_GameClientView.m_lAllTurnScore[SwitchViewChairID(i)]+=pGameEnd->lGameScore[i];
		m_GameClientView.m_lLastTurnScore[SwitchViewChairID(i)]=pGameEnd->lGameScore[i];
	}

	ShowScore();
	
	m_GameClientView.m_ScoreView.SetTax(pGameEnd->lGameTax);
	m_GameClientView.m_ScoreView.ShowWindow(SW_SHOW);

	//设置扑克
	for(int i=0;i<GAME_PLAYER-1;i++)
		m_GameClientView.m_LeaveCardControl[i].SetDisplayFlag(true);

	BYTE bCardPos=0;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//设置扑克
		WORD wViewChairID=SwitchViewChairID(i);
		if (wViewChairID!=2)
		{
			if (wViewChairID>2) m_GameClientView.m_LeaveCardControl[wViewChairID-1].SetCardData(&pGameEnd->bCardData[bCardPos],pGameEnd->bCardCount[i]);
			else m_GameClientView.m_LeaveCardControl[wViewChairID].SetCardData(&pGameEnd->bCardData[bCardPos],pGameEnd->bCardCount[i]);
		}

		//界面设置
		bCardPos+=pGameEnd->bCardCount[i];
		if (pGameEnd->bCardCount[i]!=0)
		{
			m_GameClientView.SetPassFlag(wViewChairID,false);
			m_GameClientView.m_UserCardControl[wViewChairID].SetCardData(NULL,0);
		}

	}
	//调整界面
	ZeroMemory(m_GameClientView.m_bKanGong,sizeof(bool)*4);
	
	//显示扑克
	if (IsLookonMode()==true) m_GameClientView.m_HandCardControl.SetDisplayFlag(true);

	//播放声音
	WORD wMeChairID=GetMeChairID();
	LONG lMeScore=pGameEnd->lGameScore[GetMeChairID()];
	if (lMeScore>0L) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WIN"));
	else if (lMeScore<0L) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_LOST"));
	else PlayGameSound(GetModuleHandle(NULL),TEXT("GAME_END"));

	//设置界面
	if (IsLookonMode()==false&&m_bGameEnd)
	{
		m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
		SetGameTimer(GetMeChairID(),IDI_START_GAME,60);
	}

	return true;
}

//排序消息
LRESULT CGameClientDlg::OnSortCard(WPARAM wParam, LPARAM lParam)
{
	
	//修改标志
	BYTE m_bSortType=(BYTE)lParam;
    BYTE bCardData[PIECES];
	DWORD dwCardCount=m_GameClientView.m_HandCardControl.GetShootCard(bCardData,PIECES);
	BYTE bCard=m_GameClientView.m_HandCardControl.GetMarkCard();
	switch(m_bSortType)
	{
	case 0:
		m_GameLogic.SortCardList(m_bHandCardData, m_bHandCardCount);
		break;
	case 1:
		m_GameLogic.SortByColor(m_bHandCardData,m_bHandCardCount);
		break;
	case 2:
		m_GameLogic.SortByBomb(m_bHandCardData,m_bHandCardCount);
	default:
		break;
	}
    
	m_GameClientView.m_HandCardControl.SetCardData(m_bHandCardData,m_bHandCardCount);
	//恢复状态
	m_GameClientView.m_HandCardControl.SetShootCardData(bCardData,dwCardCount);
	if(bCard!=0)
		m_GameClientView.m_HandCardControl.MarkCard(bCard,m_GameLogic.m_bSeries);
	return 0;

}
//改便位置
LRESULT CGameClientDlg::OnChangePosition(WPARAM wParam,LPARAM lParam)
{
	//更新扑克
	m_GameClientView.m_HandCardControl.GetCardData(m_bHandCardData,PIECES);
	if(m_GameClientView.m_btPayTribute.IsWindowVisible())
		m_GameClientView.m_btPayTribute.EnableWindow(false);
	if(m_GameClientView.m_btChangeCard.IsWindowVisible())
		m_GameClientView.m_btChangeCard.EnableWindow(false);


	return 0;

}

//托管按钮
LRESULT CGameClientDlg::OnAI(WPARAM wParam,LPARAM lParam)
{
	
	CMD_C_MESSAGE message;
	if(m_bAI)
	{
		_sntprintf(message.szBuffer,sizeof(message.szBuffer),"%-s 取消托管",
			GetUserData(GetMeChairID())->szName);
	
	}
	else
	{	_sntprintf(message.szBuffer,sizeof(message.szBuffer),"%-s 开始托管",
		GetUserData(GetMeChairID())->szName);
		m_wTime=m_GameClientView.GetUserTimer(2);
	}
	SendData(SUB_C_SYSTEM_MESSAGE,&message,sizeof(message));
	m_bAI=!m_bAI;
	return 0;
}


void CGameClientDlg::OnClose()
{
	
	//请求退出
	if(IsLookonMode()==false&&(GetGameStatus()==GS_RF_PLAYING||(m_bGameEnd==false&&m_RoomType==enRoomType_All)))
	{
		CReDialog dlg;
		int iRet;
		iRet=(int)dlg.DoModal();
		if(iRet==IDOK)
		{
			//发送请求消息
			TCHAR   szBuffer[100];
			ASSERT(dlg.m_bCause.GetLength()<=50);
			_sntprintf(szBuffer,sizeof(szBuffer),"%s",dlg.m_bCause);
			CMD_C_REQUEST  request;
			request.wChairID=GetMeChairID();
			CopyMemory(request.szBuffer,szBuffer,sizeof(szBuffer));
			SendData(SUB_C_REQUEST_QUIT,&request,sizeof(request));

		}
		else if(iRet==IDQUIT)
		{
			CDialog::DestroyWindow();
		}
	}
	//正常退出
	else
	{
		CGameFrameDlg::OnClose();
	}

}

//处理请求消息
bool CGameClientDlg::OnRequest(const void * pBuffer)
{
		//获取信息
		CMD_S_REQUEST *request=(CMD_S_REQUEST *)pBuffer;
		CMD_C_ANSWER  answer;
		answer.wChairID=request->wChairID;
        const tagUserData * pUserData=m_GameClientView.GetUserInfo(SwitchViewChairID(request->wChairID));
		//定义变量
		TCHAR  szTitle[50];
		_sntprintf(szTitle,sizeof(szTitle),"用户 %s 请求退出",pUserData->szName);
		CADialog dlg;
		dlg.m_title=szTitle;
		dlg.m_bCause=request->szBuffer;
		if(dlg.DoModal()==IDOK)
		{
			answer.bAnswer=true;
			
		}else
		{
			answer.bAnswer=false;
		}
        SendData(SUB_C_ANSWER_QUIT,&answer,sizeof(answer));
	return true;
}


//显示回复
bool CGameClientDlg::OnAnswer(const void * pBuffer)
{
		
		//获取消息
		CMD_S_ANSWER *sanswer=(CMD_S_ANSWER *)pBuffer;
		
		//定义变量
		TCHAR  szbuffer[50];
		if(sanswer->bAnswer)
		{
          _sntprintf(szbuffer,sizeof(szbuffer),"对方同意你的请求,按确定退出");
		}
		else
		{
		   _sntprintf(szbuffer,sizeof(szbuffer),"对方不同意你的请求");
		}
		CAnswerDlg adlg;
		adlg.m_answer=szbuffer;
		if(adlg.DoModal()==IDOK&&sanswer->bAnswer)
		{	
			CDialog::DestroyWindow();
		}
	return true;
}

//显示得分
void CGameClientDlg::ShowScore(void)
{
	TCHAR   szBuffer[256];
	_sntprintf(szBuffer,sizeof(szBuffer),"\n得分如下\n%-s 得到 %2d分\n%-s 得到 %2d分\n%-s 得到 %2d分\n%-s 得到 %2d分",
		GetUserData(0)->szName,m_GameClientView.m_lLastTurnScore[SwitchViewChairID(0)],
		GetUserData(1)->szName,m_GameClientView.m_lLastTurnScore[SwitchViewChairID(1)],
		GetUserData(2)->szName,m_GameClientView.m_lLastTurnScore[SwitchViewChairID(2)],
		GetUserData(3)->szName,m_GameClientView.m_lLastTurnScore[SwitchViewChairID(3)]
		);
	InsertSystemString(szBuffer);
}

//牌是否可交换
bool CGameClientDlg::VerdictChangeCard(void)
{
	if(m_GameClientView.m_btChangeCard.IsWindowVisible()==false)return false;
	BYTE bCardData[MAX_COUNT];
	DWORD dwShootCount=m_GameClientView.m_HandCardControl.GetShootCard(bCardData,CountArray(bCardData));
	if(dwShootCount==1)return true;
	
	return false;
}
