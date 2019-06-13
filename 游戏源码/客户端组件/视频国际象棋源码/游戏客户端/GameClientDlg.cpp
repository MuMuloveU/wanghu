#include "Stdafx.h"
#include "GameOption.h"
#include "GameClient.h"
#include "GameClientDlg.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

#define IDI_GAME_TIMER				100									//游戏定时器
#define IDI_START_GAME				101									//开始定时器

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientDlg, CGameFrameDlg)
	ON_WM_TIMER()
	ON_MESSAGE(IDM_START,OnStart)
	ON_MESSAGE(IDM_PEACE,OnPeace)
	ON_MESSAGE(IDM_STUDY,OnStudy)
	ON_MESSAGE(IDM_REGRET,OnRegret)
	ON_MESSAGE(IDM_GIVEUP,OnGiveUp)
	ON_MESSAGE(IDM_PRESERVE,OnPreserve)
	ON_MESSAGE(WM_SELECT_CHESS, OnSelectChess)
	ON_MESSAGE(IDM_PEACE_ANSWER,OnPeaceAnswer)
	ON_MESSAGE(IDM_REGRET_ANSWER,OnRegretAnswer)
	ON_MESSAGE(WM_HIT_CHESS_BORAD, OnHitChessBorad)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientDlg::CGameClientDlg() : CGameFrameDlg(&m_GameClientView)
{
	//游戏变量
	m_cbChessColor=BLACK_CHESS;
	m_wBlackUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	memset(m_wLeftClock,0,sizeof(m_wLeftClock));

	//辅助变量
	m_ShamView=false;
	m_cbXSourcePos=255;
	m_cbYSourcePos=255;
	m_cbXTargetPos=255;
	m_cbYTargetPos=255;

	//配置变量
	m_bShowChessName=false;
	m_bAutoSaveManual=false;

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
	TCHAR szTitle[128]=TEXT("");
	_snprintf(szTitle,sizeof(szTitle),TEXT("%s"),GAME_NAME);
	SetWindowText(szTitle);

	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//读取配置
	m_bShowChessName=AfxGetApp()->GetProfileInt(TEXT("GameOption"),TEXT("ShowChessName"),FALSE)?true:false;
	m_bAutoSaveManual=AfxGetApp()->GetProfileInt(TEXT("GameOption"),TEXT("AutoSaveManual"),FALSE)?true:false;

	//设置控件
	m_GameClientView.m_ChessBorad.ShowChessName(m_bShowChessName);

	return true;
}

//重置框架
void CGameClientDlg::ResetGameFrame()
{
	//辅助变量
	m_cbXSourcePos=255;
	m_cbYSourcePos=255;
	m_cbXTargetPos=255;
	m_cbYTargetPos=255;

	//游戏变量
	m_cbChessColor=BLACK_CHESS;
	m_wBlackUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	memset(m_wLeftClock,0,sizeof(m_wLeftClock));

	return;
}

//游戏设置
void CGameClientDlg::OnGameOptionSet()
{
	//构造数据
	CGameOption GameOption;
	GameOption.m_bEnableSound=IsEnableSound();
	GameOption.m_bShowChessName=m_bShowChessName;

	//配置数据
	if (GameOption.DoModal()==IDOK)
	{
		//设置变量
		m_bShowChessName=GameOption.m_bShowChessName;

		//保存配置
		AfxGetApp()->WriteProfileInt(TEXT("GameOption"),TEXT("ShowChessName"),m_bShowChessName);

		//功能设置
		EnableSound(GameOption.m_bEnableSound);
		m_GameClientView.m_ChessBorad.ShowChessName(m_bShowChessName);
	}

	return;
}

//时间消息
bool CGameClientDlg::OnTimerMessage(WORD wChairID, UINT nElapse, UINT nTimerID)
{
	if ( nTimerID == IDI_START_GAME )
	{
		if (wChairID == GetMeChairID() && ! IsLookonMode())		//开始游戏
		{
			if (nElapse==0)
			{
				SendMessage(WM_CLOSE,0,0);
				return false;
			}
			if ((nElapse<=10)&&(IsLookonMode()==false)) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));

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
	case SUB_S_GAME_START:		//游戏开始
		{
			return OnSubGameStart(pBuffer,wDataSize);
		}
	case SUB_S_MOVE_CHESS:		//移动棋子
		{
			return OnSubMoveChess(pBuffer,wDataSize);
		}
	case SUB_S_REGRET_REQ:		//悔棋请求
		{
			return OnSubRegretReq(pBuffer,wDataSize);
		}
	case SUB_S_REGRET_FAILE:	//悔棋失败
		{
			return OnSubRegretFaile(pBuffer,wDataSize);
		}
	case SUB_S_REGRET_RESULT:	//悔棋结果
		{
			return OnSubRegretResult(pBuffer,wDataSize);
		}
	case SUB_S_PEACE_REQ:		//和棋请求
		{
			return OnSubPeaceReq(pBuffer,wDataSize);
		}
	case SUB_S_PEACE_ANSWER:	//和棋应答
		{
			return OnSubPeaceAnser(pBuffer,wDataSize);
		}
	case SUB_S_GAME_END:		//游戏结束
		{
			return OnSubGameEnd(pBuffer,wDataSize);
		}
	case SUB_S_CHESS_MANUAL:	//游戏棋谱
		{
			return OnSubChessManual(pBuffer,wDataSize);
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
	m_wBlackUser=pGameStart->wBlackUser;
	m_wCurrentUser=pGameStart->wBlackUser;
	m_cbChessColor=(m_wBlackUser==GetMeChairID())?BLACK_CHESS:WHITE_CHESS;

	//设置时间
	m_wLeftClock[0]=m_wLeftClock[1]=pGameStart->wGameClock;
	m_GameClientView.SetGameClock(pGameStart->wGameClock);
	m_GameClientView.SetUserClock(SwitchViewChairID(0),m_wLeftClock[0]);
	m_GameClientView.SetUserClock(SwitchViewChairID(1),m_wLeftClock[1]);

	//设置变量
	m_cbXSourcePos=255;
	m_cbYSourcePos=255;
	m_cbXTargetPos=255;
	m_cbYTargetPos=255;

	//设置棋盘 
	m_GameLogic.ResetChessBorad();
	m_GameClientView.m_ChessBorad.SetChessColor(m_cbChessColor);
	m_GameClientView.m_ChessBorad.SetChessBorad(m_GameLogic.m_ChessBorad);

	//设置界面
	m_GameClientView.m_btPreserve.EnableWindow(TRUE);
	m_GameClientView.m_GameScoreWnd.ShowWindow(SW_HIDE);
	m_GameClientView.SetBlackUser(SwitchViewChairID(m_wBlackUser));

	//全部玩家
	if (IsLookonMode()==false)
	{
		m_GameClientView.m_btStart.EnableWindow(FALSE);
		m_GameClientView.m_btPeace.EnableWindow(TRUE);
		m_GameClientView.m_btGiveUp.EnableWindow(TRUE);
	}

	//玩家设置
	if ((IsLookonMode()==false)&&(m_wCurrentUser==GetMeChairID()))
	{
		ActiveGameFrame();
		CancelChessSelect();
		m_GameClientView.m_ChessBorad.SetPositively(true);
	}

	//设置定时器
	SetTimer(IDI_GAME_TIMER,1000,NULL);

	//播放声音
	PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));

	return true;
}

//移动棋子
bool CGameClientDlg::OnSubMoveChess(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_MoveChess));
	if (wDataSize!=sizeof(CMD_S_MoveChess)) return false;

	//消息处理
	CMD_S_MoveChess * pMoveChess=(CMD_S_MoveChess *)pBuffer;
	m_wCurrentUser=pMoveChess->wCurrentUser;

	//设置时间
	CopyMemory(m_wLeftClock,pMoveChess->wLeftClock,sizeof(m_wLeftClock));

	//记录棋谱
	WORD wMoveUser=(pMoveChess->wCurrentUser+1)%GAME_PLAYER;

	//设置界面
	m_ShamView=false;
	WORD wMeChairID=GetMeChairID();
	if ((IsLookonMode()==true)||(wMoveUser!=wMeChairID))
	{
		//变量定义
		BYTE cbXSourcePos=pMoveChess->cbXSourcePos,cbYSourcePos=pMoveChess->cbYSourcePos;
		BYTE cbXTargetPos=pMoveChess->cbXTargetPos,cbYTargetPos=pMoveChess->cbYTargetPos;

		//播放声音
		const tagChessItem * pSourceChessItem=m_GameLogic.GetChessItem(cbXSourcePos,cbYSourcePos);
		const tagChessItem * pTargetChessItem=m_GameLogic.GetChessItem(cbXTargetPos,cbYTargetPos);
		if ((pTargetChessItem!=NULL)&&(pSourceChessItem->cbColor!=pTargetChessItem->cbColor))
		{
			if (pSourceChessItem->cbColor==m_cbChessColor) PlayGameSound(AfxGetInstanceHandle(),TEXT("CHESS_EAT"));
			else PlayGameSound(AfxGetInstanceHandle(),TEXT("CHESS_DEAD"));
		}
		else PlayGameSound(AfxGetInstanceHandle(),TEXT("PLACE_CHESS"));

		//移动棋子
		m_GameLogic.MoveChess(cbXSourcePos,cbYSourcePos,cbXTargetPos,cbYTargetPos,pMoveChess->cbSwitchChess);

		//设置棋盘
		m_GameClientView.m_ChessBorad.SetChessBorad(m_GameLogic.m_ChessBorad);
		m_GameClientView.m_ChessBorad.SetChessFrame(cbXSourcePos,cbYSourcePos,cbXTargetPos,cbYTargetPos);
	}

	//玩家设置
	if ((IsLookonMode()==false)&&(pMoveChess->wCurrentUser==wMeChairID))
	{
		//设置界面
		ActiveGameFrame();
		CancelChessSelect();
		m_GameClientView.m_ChessBorad.SetPositively(true);
		m_GameClientView.m_ChessBorad.SetSelectChess(m_cbXSourcePos,m_cbYSourcePos);
	}

	return true;
}

//悔棋请求
bool CGameClientDlg::OnSubRegretReq(const void * pBuffer, WORD wDataSize)
{
	m_GameClientView.ShowRegretRequest();

	return true;
}

//悔棋失败
bool CGameClientDlg::OnSubRegretFaile(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_RegretFaile));
	if (wDataSize!=sizeof(CMD_S_RegretFaile)) return false;

	//消息处理
	CMD_S_RegretFaile * pRegretFaile=(CMD_S_RegretFaile *)pBuffer;

	//次数限制
	if (pRegretFaile->cbFaileReason==FR_COUNT_LIMIT)
	{
		InsertSystemString(TEXT("由于你的 [ 悔棋 ] 请求次数过多，悔棋失败"));
		return true;
	}

	//对家反对
	if (pRegretFaile->cbFaileReason==FR_PLAYER_OPPOSE)
	{
		InsertSystemString(TEXT("对家不同意你 [ 悔棋 ]"));
		WORD wStepCount=m_GameLogic.GetStepCount((GetMeChairID()==m_wBlackUser)?BLACK_CHESS:WHITE_CHESS);
		m_GameClientView.m_btRegret.EnableWindow(wStepCount>0);
		return true;
	}

	return true;
}

//悔棋结果
bool CGameClientDlg::OnSubRegretResult(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_RegretResult));
	if (wDataSize!=sizeof(CMD_S_RegretResult)) return false;

	//变量定义
	CMD_S_RegretResult * pRegretResult=(CMD_S_RegretResult *)pBuffer;
	CopyMemory(m_wLeftClock,pRegretResult->wLeftClock,sizeof(m_wLeftClock));

	//调整次数
	if (m_ShamView==true)
	{
		m_ShamView=false;
		pRegretResult->cbRegretCount++;
	}

	//设置棋盘
	m_GameLogic.RegretChess(pRegretResult->cbRegretCount);
	m_GameClientView.m_ChessBorad.SetChessBorad(m_GameLogic.m_ChessBorad);

	//设置框架
	const tagChessManual * pChessManual=m_GameLogic.GetLastChessManual();
	if (pChessManual!=NULL) 
	{
		BYTE cbXPosStart=pChessManual->cbXSourceChessPos;
		BYTE cbYPosStart=pChessManual->cbYSourceChessPos;
		BYTE cbXPosTerminal=pChessManual->cbXTargetChessPos;
		BYTE cbYPosTerminal=pChessManual->cbYTargetChessPos;
		m_GameClientView.m_ChessBorad.SetChessFrame(cbXPosStart,cbYPosStart,cbXPosTerminal,cbYPosTerminal);
	}
	else m_GameClientView.m_ChessBorad.SetChessFrame(255,255,255,255);

	//玩家设置
	WORD wMeChairID=GetMeChairID();
	m_wCurrentUser=pRegretResult->wCurrentUser;
	if ((IsLookonMode()==false)&&(pRegretResult->wCurrentUser==wMeChairID))
	{
		ActiveGameFrame();
		CancelChessSelect();
		m_GameClientView.m_ChessBorad.SetPositively(true);
	}

	//悔棋控制
	if (IsLookonMode()==false)
	{
		WORD wStepCount=m_GameLogic.GetStepCount((GetMeChairID()==m_wBlackUser)?BLACK_CHESS:WHITE_CHESS);
		if (wStepCount==0) m_GameClientView.m_btRegret.EnableWindow(FALSE);
		if ((wStepCount>0)&&(pRegretResult->wRegretUser==wMeChairID)) m_GameClientView.m_btRegret.EnableWindow(TRUE);
	}

	return true;
}

//和棋请求
bool CGameClientDlg::OnSubPeaceReq(const void * pBuffer, WORD wDataSize)
{
	m_GameClientView.ShowPeaceRequest();

	return true;
}

//和棋应答
bool CGameClientDlg::OnSubPeaceAnser(const void * pBuffer, WORD wDataSize)
{
	//设置界面
	m_GameClientView.m_btPeace.EnableWindow(TRUE);
	InsertSystemString(TEXT("对家不同意你 [ 和棋 ]"));

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

	//设置变量
	m_wCurrentUser=INVALID_CHAIR;

	//设置界面
	m_GameClientView.m_btPeace.EnableWindow(FALSE);
	m_GameClientView.m_btRegret.EnableWindow(FALSE);
	m_GameClientView.m_btGiveUp.EnableWindow(FALSE);
	m_GameClientView.m_ChessBorad.SetPositively(false);

	//播放声音
	if (IsLookonMode()==false)
	{
		if (pGameEnd->wWinUser==GetMeChairID()) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WIN"));
		else PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_LOST"));
	}
	else PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_END"));

	//成绩设置
	tagGameScoreInfo GameScoreInfo;
	memset(&GameScoreInfo,0,sizeof(GameScoreInfo));
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		WORD wUserChairID=(m_wBlackUser+i)%GAME_PLAYER;
		const tagUserData * pUserData=GetUserData(wUserChairID);
		GameScoreInfo.lScore[i]=pGameEnd->lUserScore[wUserChairID];
		lstrcpyn(GameScoreInfo.szUserName[i],pUserData->szName,CountArray(GameScoreInfo.szUserName[i]));
	}
	m_GameClientView.m_GameScoreWnd.SetScoreInfo(GameScoreInfo);

	//设置界面
	if (IsLookonMode()==false)
	{
		ActiveGameFrame();
		CancelChessSelect();
		m_GameClientView.m_btStart.EnableWindow(TRUE);
		m_GameClientView.m_btStart.SetFocus();
		if (m_GameClientView.m_PeaceRequest.GetSafeHwnd()!=NULL) m_GameClientView.m_PeaceRequest.DestroyWindow();
		if (m_GameClientView.m_RegretRequest.GetSafeHwnd()!=NULL) m_GameClientView.m_RegretRequest.DestroyWindow();

		//设置时间
		SetGameTimer(GetMeChairID(), IDI_START_GAME, 30) ;
	}

	//删除定时器
	KillTimer(IDI_GAME_TIMER);

	//保存棋谱
	if (m_bAutoSaveManual==true)
	{
		//获取时间
		SYSTEMTIME SystemTime;
		GetLocalTime(&SystemTime);

		//获取目录
		TCHAR szPath[MAX_PATH]=TEXT("");
		GetCurrentDirectory(sizeof(szPath),szPath);

		//创建文件
		TCHAR szFileName[MAX_PATH];
		_snprintf(szFileName,sizeof(szFileName),TEXT("%s\\围棋棋谱_%04ld_%02ld_%02ld_%02ld_%02ld_%02ld.WQM"),
			szPath,SystemTime.wYear,SystemTime.wMonth,SystemTime.wDay,SystemTime.wHour,SystemTime.wMinute,SystemTime.wSecond);

		//保存棋谱
		if (SaveChessManual(szFileName)==false)	InsertSystemString(TEXT("围棋游戏“棋谱记录”保存失败"));
		else InsertSystemString(TEXT("围棋游戏“棋谱记录”保存成功"));
	}
	else
	{
		static bool bNotify=false;
		if (bNotify==false)
		{
			bNotify=true;
			InsertSystemString(TEXT("游戏结束了，若需要保存棋谱，请点击 [ 保存 ] 按钮保存棋谱记录"));
		}
	}

	return true;
}

//游戏棋谱
bool CGameClientDlg::OnSubChessManual(const void * pBuffer, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize%sizeof(tagChessManual)==0);
	if (wDataSize%sizeof(tagChessManual)!=0) return false;

	//设置棋谱
	WORD wManualCount=wDataSize/sizeof(tagChessManual);
	tagChessManual * pChessManual=(tagChessManual *)pBuffer;
	m_GameLogic.InsertChessManual(pChessManual,wManualCount);

	return true;
}

//游戏场景
bool CGameClientDlg::OnGameSceneMessage(BYTE cbGameStation, bool bLookonOther, const void * pBuffer, WORD wDataSize)
{
	switch (cbGameStation)
	{
	case GS_FREE:		//空闲状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pBuffer;

			//设置界面
			m_wBlackUser=pStatusFree->wBlackUser;
			m_wCurrentUser=pStatusFree->wBlackUser;
			m_GameClientView.SetBlackUser(SwitchViewChairID(m_wBlackUser));

			//设置控件
			if (IsLookonMode()==false)
			{
				m_GameClientView.m_btStart.EnableWindow(TRUE);
				m_GameClientView.m_btStart.SetFocus();

				//设置时间
				SetGameTimer(GetMeChairID(), IDI_START_GAME, 30) ;
			}

			return true;
		}
	case GS_PLAYING:	//游戏状态
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_StatusPlay));
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;

			//消息处理
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pBuffer;

			//设置变量
			m_wBlackUser=pStatusPlay->wBlackUser;
			m_wCurrentUser=pStatusPlay->wCurrentUser;
			m_cbChessColor=(m_wBlackUser==GetMeChairID())?BLACK_CHESS:WHITE_CHESS;

			//设置时间
			CopyMemory(m_wLeftClock,pStatusPlay->wLeftClock,sizeof(m_wLeftClock));
			m_GameClientView.SetGameClock(pStatusPlay->wGameClock);
			m_GameClientView.SetUserClock(SwitchViewChairID(0),m_wLeftClock[0]);
			m_GameClientView.SetUserClock(SwitchViewChairID(1),m_wLeftClock[1]);

			//设置界面
			m_GameClientView.m_btPreserve.EnableWindow(TRUE);
			m_GameClientView.SetBlackUser(SwitchViewChairID(m_wBlackUser));

			//逻辑数据
			CopyMemory(m_GameLogic.m_ChessItem,pStatusPlay->ChessItemArray,sizeof(m_GameLogic.m_ChessItem));
			CopyMemory(m_GameLogic.m_wStepCount,pStatusPlay->wChessStepCount,sizeof(m_GameLogic.m_wStepCount));

			//棋盘界面
			m_GameLogic.UpdateChessBorad();
			m_GameClientView.m_ChessBorad.SetChessColor(m_cbChessColor);
			m_GameClientView.m_ChessBorad.SetChessBorad(m_GameLogic.m_ChessBorad);

			//设置框架
			const tagChessManual * pChessManual=m_GameLogic.GetLastChessManual();
			if (pChessManual!=NULL) 
			{
				BYTE cbXPosStart=pChessManual->cbXSourceChessPos;
				BYTE cbYPosStart=pChessManual->cbYSourceChessPos;
				BYTE cbXPosTerminal=pChessManual->cbXTargetChessPos;
				BYTE cbYPosTerminal=pChessManual->cbYTargetChessPos;
				m_GameClientView.m_ChessBorad.SetChessFrame(cbXPosStart,cbYPosStart,cbXPosTerminal,cbYPosTerminal);
			}

			//玩家设置
			if (IsLookonMode()==false)
			{
				//变量定义
				WORD wMeChairID=GetMeChairID();
				BOOL bEnablePeace=((pStatusPlay->cbBegStatus[wMeChairID]&GU_WAIT_PEACE)==0);
				WORD wStepCount=m_GameLogic.GetStepCount((GetMeChairID()==m_wBlackUser)?BLACK_CHESS:WHITE_CHESS);

				//按钮设置
				m_GameClientView.m_btGiveUp.EnableWindow(TRUE);
				m_GameClientView.m_btPeace.EnableWindow(bEnablePeace);
				if (wStepCount>0)
				{
					BOOL bEnableRegret=((pStatusPlay->cbBegStatus[wMeChairID]&GU_WAIT_REGRET)==0);
					m_GameClientView.m_btRegret.EnableWindow(bEnableRegret);
				}

				//变量定义
				WORD wEnemyUser=(wMeChairID+1)%GAME_PLAYER;
				BOOL bPeaceReq=((pStatusPlay->cbBegStatus[wEnemyUser]&GU_WAIT_PEACE)!=0);
				BOOL bRegretReq=((pStatusPlay->cbBegStatus[wEnemyUser]&GU_WAIT_REGRET)!=0);

				//请求设置
				if (bPeaceReq==TRUE) m_GameClientView.ShowPeaceRequest();
				if (bRegretReq==TRUE) m_GameClientView.ShowRegretRequest();

				//操作设置
				if (wMeChairID==m_wCurrentUser) m_GameClientView.m_ChessBorad.SetPositively(true);
			}

			//设置定时器
			SetTimer(IDI_GAME_TIMER,1000,NULL);

			return true;
		}
	}

	return false;
}

//定时器消息
void CGameClientDlg::OnTimer(UINT nIDEvent)
{
	//倒数定时器
	if ((nIDEvent==IDI_GAME_TIMER)&&(m_wCurrentUser!=INVALID_CHAIR))
	{
		//设置视图
		if (m_wLeftClock[m_wCurrentUser]>0)
		{
			m_wLeftClock[m_wCurrentUser]--;
			m_GameClientView.SetUserClock(SwitchViewChairID(m_wCurrentUser),m_wLeftClock[m_wCurrentUser]);
		}
		
		//超时判断
		if ((IsLookonMode()==false)&&(GetMeChairID()==m_wCurrentUser))
		{
			if (m_wLeftClock[m_wCurrentUser]==0)
			{
				//发送消息
				SendData(SUB_C_GIVEUP_REQ);

				//设置界面
				m_GameClientView.m_btPeace.EnableWindow(FALSE);
				m_GameClientView.m_btRegret.EnableWindow(FALSE);
				m_GameClientView.m_btGiveUp.EnableWindow(FALSE);
				m_GameClientView.m_ChessBorad.SetPositively(false);

				return;
			}
			else if (m_wLeftClock[m_wCurrentUser]<=30) PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
		}

		return;
	}

	__super::OnTimer(nIDEvent);
}

//开始按钮
LRESULT CGameClientDlg::OnStart(WPARAM wParam, LPARAM lParam)
{
	//游戏变量
	m_wBlackUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	memset(m_wLeftClock,0,sizeof(m_wLeftClock));

	//设置界面
	m_GameClientView.m_ChessBorad.CleanChess();
	m_GameClientView.m_btStart.EnableWindow(FALSE);
	m_GameClientView.m_btPreserve.EnableWindow(FALSE);
	m_GameClientView.m_GameScoreWnd.ShowWindow(SW_HIDE);

	//发送消息
	SendUserReady(NULL,0);

	//关闭时间
	KillGameTimer(IDI_START_GAME) ;

	return 0;
}

//悔棋按钮
LRESULT CGameClientDlg::OnRegret(WPARAM wParam, LPARAM lParam)
{
	//发送消息
	SendData(SUB_C_REGRET_REQ);

	//设置界面
	m_GameClientView.m_btRegret.EnableWindow(FALSE);
	InsertSystemString(TEXT("正在等待对家回应 [ 悔棋 ] 请求..."));

	return 0;
}

//求和按钮
LRESULT CGameClientDlg::OnPeace(WPARAM wParam, LPARAM lParam)
{
	//和棋确认
	int nResult=AfxMessageBox(TEXT("你确实要 [ 和棋 ] 吗？"),MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2);
	if (nResult!=IDYES) return 0;

	//状态判断
	if (m_wCurrentUser==INVALID_CHAIR) return 0;

	//发送消息
	SendData(SUB_C_PEACE_REQ);

	//设置界面
	m_GameClientView.m_btPeace.EnableWindow(FALSE);
	InsertSystemString(TEXT("正在等待对家回应 [ 和棋 ] 请求..."));

	return 0;
}

//认输按钮
LRESULT CGameClientDlg::OnGiveUp(WPARAM wParam, LPARAM lParam)
{
	//认输确认
	int nResult=AfxMessageBox(TEXT("你确实要 [ 认输 ] 吗？"),MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2);
	if (nResult!=IDYES) return 0;

	//状态判断
	if (m_wCurrentUser==INVALID_CHAIR) return 0;

	//发送消息
	SendData(SUB_C_GIVEUP_REQ);

	//设置界面
	m_GameClientView.m_btPeace.EnableWindow(FALSE);
	m_GameClientView.m_btRegret.EnableWindow(FALSE);
	m_GameClientView.m_btGiveUp.EnableWindow(FALSE);
	m_GameClientView.m_ChessBorad.SetPositively(false);

	return 0;
}

//保存按钮
LRESULT CGameClientDlg::OnPreserve(WPARAM wParam, LPARAM lParam)
{
	//获取目录
	TCHAR szPath[MAX_PATH]=TEXT("");
	GetCurrentDirectory(sizeof(szPath),szPath);

	//选择文件
	LPCTSTR pszFilter=TEXT("国际象棋棋谱文件 （*.CHM）|*.CHM||");
	CFileDialog DlgSaveManual(FALSE,TEXT("CHM"),NULL,OFN_NOCHANGEDIR|OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,pszFilter);
	DlgSaveManual.m_pOFN->lpstrInitialDir=szPath;
	if (DlgSaveManual.DoModal()!=IDOK) return 0;

	//保存棋谱
	if (SaveChessManual(DlgSaveManual.GetPathName())==false)	
	{
		InsertSystemString(TEXT("国际象棋游戏“棋谱记录”保存失败"));
	}
	else InsertSystemString(TEXT("国际象棋游戏“棋谱记录”保存成功"));

	return 0;
}

//研究按钮
LRESULT CGameClientDlg::OnStudy(WPARAM wParam, LPARAM lParam)
{
	//设置用户
	if (m_wBlackUser!=INVALID_CHAIR)
	{
		BYTE cbChessColor=(GetMeChairID()==m_wBlackUser)?BLACK_CHESS:WHITE_CHESS;
		m_GameClientView.m_ChessManual.m_ChessBorad.SetChessColor(cbChessColor);
	}

	//创建棋谱
	if (m_GameClientView.m_ChessManual.GetSafeHwnd()==NULL)
	{
		m_GameClientView.m_ChessManual.m_pDlgGameLogic=&m_GameLogic;
		m_GameClientView.m_ChessManual.Create(IDD_MANUAL,&m_GameClientView);
	}

	//显示棋谱
	m_GameClientView.m_ChessManual.CenterWindow(NULL);
	m_GameClientView.m_ChessManual.ShowWindow(SW_SHOW);

	return 0;
}

//和棋应答
LRESULT CGameClientDlg::OnPeaceAnswer(WPARAM wParam, LPARAM lParam)
{
	//发送消息
	CMD_C_PeaceAnswer PeaceAnswer;
	PeaceAnswer.cbApprove=BYTE(wParam);
	SendData(SUB_C_PEACE_ANSWER,&PeaceAnswer,sizeof(PeaceAnswer));

	return 0;
}

//悔棋应答
LRESULT CGameClientDlg::OnRegretAnswer(WPARAM wParam, LPARAM lParam)
{
	//设置界面
	if (wParam==TRUE)
	{
		m_wCurrentUser=INVALID_CHAIR;
		m_GameClientView.m_ChessBorad.SetPositively(false);
	}

	//发送消息
	CMD_C_RegretAnswer ReqretAnswer;
	ReqretAnswer.cbApprove=BYTE(wParam);
	SendData(SUB_C_REGRET_ANSWER,&ReqretAnswer,sizeof(ReqretAnswer));

	return 0;
}

//选择棋子
LRESULT CGameClientDlg::OnSelectChess(WPARAM wParam, LPARAM lParam)
{
	//执行走棋
	if (m_wCurrentUser==GetMeChairID())	PerformMoveChess((BYTE)wParam);

	return 0;
}

//点击棋盘
LRESULT CGameClientDlg::OnHitChessBorad(WPARAM wParam, LPARAM lParam)
{
	if (m_wCurrentUser==GetMeChairID())
	{
		//变量定义
		bool bMoveChess=false;
		m_cbXTargetPos=(BYTE)wParam;
		m_cbYTargetPos=(BYTE)lParam;
		const tagChessItem * pChessItem=m_GameLogic.GetChessItem(m_cbXTargetPos,m_cbYTargetPos);

		//取消判断
		if ((m_cbXSourcePos==m_cbXTargetPos)&&(m_cbYSourcePos==m_cbYTargetPos))
		{
			CancelChessSelect();
			return 0;
		}

		//选择判断
		if ((pChessItem!=NULL)&&(pChessItem->cbColor==m_cbChessColor))
		{
			//王车易位
			if ((m_cbXSourcePos!=255)&&(m_cbYSourcePos!=255))
			{
				bMoveChess=m_GameLogic.IsWalkLegality(m_cbXSourcePos,m_cbYSourcePos,m_cbXTargetPos,m_cbYTargetPos);
			}

			//选择棋子
			if (bMoveChess==false)
			{
				m_cbXSourcePos=m_cbXTargetPos;
				m_cbYSourcePos=m_cbYTargetPos;
				PlayGameSound(AfxGetInstanceHandle(),TEXT("CHESS_SELECT"));
				m_GameClientView.m_ChessBorad.SetSelectChess(m_cbXSourcePos,m_cbYSourcePos);

				return 0;
			}
		}

		//走棋判断
		if ((m_cbXSourcePos!=255)&&(m_cbYSourcePos!=255))
		{
			bMoveChess=m_GameLogic.IsWalkLegality(m_cbXSourcePos,m_cbYSourcePos,m_cbXTargetPos,m_cbYTargetPos);
		}

		//走棋判断
		if (bMoveChess==true)
		{
			//兵变判断
			const tagChessItem * pSourceChessItem=m_GameLogic.GetChessItem(m_cbXSourcePos,m_cbYSourcePos);
			if ((pSourceChessItem->cbChess==CHESS_SOLDIER)&&((m_cbYTargetPos==0)||(m_cbYTargetPos==7)))
			{
				m_GameClientView.m_ChessBorad.SetPositively(false);
				m_GameClientView.m_ChessSelect.ShowWindow(SW_SHOW);
				InsertSystemString(TEXT("请点击选择“兵”升变的棋子"));
				return 0;
			}

			//执行走棋
			PerformMoveChess(NO_CHESS);

			return 0;
		}
		else
		{
			CancelChessSelect();
			return 0;
		}
	}

	return 0;
}

//取消选择
void CGameClientDlg::CancelChessSelect()
{
	//设置变量
	m_cbXSourcePos=255;
	m_cbYSourcePos=255;
	m_cbXTargetPos=255;
	m_cbYTargetPos=255;

	//设置界面
	m_GameClientView.m_ChessBorad.SetSelectChess(m_cbXSourcePos,m_cbYSourcePos);

	return;
}

//执行走棋
void CGameClientDlg::PerformMoveChess(BYTE cbSwitchChess)
{
	//效验状态
	ASSERT((m_cbXSourcePos<8)&&(m_cbYSourcePos<8));
	ASSERT((m_cbXTargetPos<8)&&(m_cbYTargetPos<8));

	//获取棋子
	const tagChessItem * pSourceChessItem=m_GameLogic.GetChessItem(m_cbXSourcePos,m_cbYSourcePos);
	const tagChessItem * pTargetChessItem=m_GameLogic.GetChessItem(m_cbXTargetPos,m_cbYTargetPos);

	//移动棋子
	m_ShamView=true;
	m_GameLogic.MoveChess(m_cbXSourcePos,m_cbYSourcePos,m_cbXTargetPos,m_cbYTargetPos,cbSwitchChess);

	//设置棋盘
	m_GameClientView.m_ChessBorad.SetPositively(false);
	m_GameClientView.m_ChessBorad.SetChessBorad(m_GameLogic.m_ChessBorad);

	//危险判断
	if (m_GameLogic.IsKingDanger(m_cbChessColor)==true)
	{
		int nResult=AfxMessageBox(TEXT("[ 王 ] 将会被杀，确定要这样走棋吗？"),MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2);
		if (m_wCurrentUser!=GetMeChairID()) return;
		if (nResult!=IDYES)
		{
			//取消选择
			CancelChessSelect();

			//悔棋控制
			m_ShamView=false;
			m_GameLogic.RegretChess(1);
			m_GameClientView.m_ChessBorad.SetPositively(true);
			m_GameClientView.m_ChessBorad.SetChessBorad(m_GameLogic.m_ChessBorad);

			return;
		}
	}

	//设置框架
	m_GameClientView.m_ChessBorad.SetChessFrame(m_cbXSourcePos,m_cbYSourcePos,m_cbXTargetPos,m_cbYTargetPos);

	//悔棋判断
	WORD wStepCount=m_GameLogic.GetStepCount((GetMeChairID()==m_wBlackUser)?BLACK_CHESS:WHITE_CHESS);
	if (wStepCount==1) m_GameClientView.m_btRegret.EnableWindow(TRUE);

	//播放声音
	if ((pTargetChessItem!=NULL)&&(pSourceChessItem->cbColor!=pTargetChessItem->cbColor))
	{
		if (pSourceChessItem->cbColor==m_cbChessColor) PlayGameSound(AfxGetInstanceHandle(),TEXT("CHESS_EAT"));
		else PlayGameSound(AfxGetInstanceHandle(),TEXT("CHESS_DEAD"));
	}
	else PlayGameSound(AfxGetInstanceHandle(),TEXT("PLACE_CHESS"));

	//发送消息
	CMD_C_MoveChess MoveChess;
	MoveChess.cbXSourcePos=m_cbXSourcePos;
	MoveChess.cbYSourcePos=m_cbYSourcePos;
	MoveChess.cbXTargetPos=m_cbXTargetPos;
	MoveChess.cbYTargetPos=m_cbYTargetPos;
	MoveChess.cbSwitchChess=cbSwitchChess;
	SendData(SUB_C_MOVE_CHESS,&MoveChess,sizeof(MoveChess));

	//设置变量
	m_cbXSourcePos=255;
	m_cbYSourcePos=255;
	m_cbXTargetPos=255;
	m_cbYTargetPos=255;
	m_GameClientView.m_ChessBorad.SetSelectChess(255,255);

	return;
}

//保存棋谱
bool CGameClientDlg::SaveChessManual(LPCTSTR pszFileName)
{
	//打开文件
	CFile FileManual;
	if (FileManual.Open(pszFileName,CFile::modeWrite|CFile::modeCreate)==FALSE) return false;

	//文件头
	tagChessManualHead ChessManualHead;
	ChessManualHead.wVersion=VER_MANUAL;
	ChessManualHead.dwManualDate=(DWORD)time(NULL);
	ChessManualHead.dwManualCount=(DWORD)m_GameLogic.m_ChessManualInfo.GetCount();

	//写入文件头
	FileManual.SeekToBegin();
	FileManual.Write(&ChessManualHead,sizeof(ChessManualHead));

	//棋谱数据
	tagChessManual * pChessManual=NULL;
	for (DWORD i=0;i<ChessManualHead.dwManualCount;i++)
	{
		pChessManual=&m_GameLogic.m_ChessManualInfo[i];
		FileManual.Write(pChessManual,sizeof(tagChessManual));
	}

	//关闭文件
	FileManual.SetLength(sizeof(ChessManualHead)+ChessManualHead.dwManualCount*sizeof(tagChessManual));
	FileManual.Close();

	return true;
}

//////////////////////////////////////////////////////////////////////////
