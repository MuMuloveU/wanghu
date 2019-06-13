#include "Stdafx.h"
#include "GameClient.h"
#include "GameOption.h"
#include "GameClientDlg.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

#define IDI_GAME_TIMER				100									//游戏定时器

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientDlg, CGameFrameDlg)
	ON_WM_TIMER()
	ON_MESSAGE(IDM_START,OnStart)
	ON_MESSAGE(IDM_PEACE,OnPeace)
	ON_MESSAGE(IDM_STUDY,OnStudy)
	ON_MESSAGE(IDM_REGRET,OnRegret)
	ON_MESSAGE(IDM_GIVEUP,OnGiveUp)
	ON_MESSAGE(IDM_PRESERVE,OnPreserve)
	ON_MESSAGE(IDM_PEACE_ANSWER,OnPeaceAnswer)
	ON_MESSAGE(IDM_REGRET_ANSWER,OnRegretAnswer)
	ON_MESSAGE(WM_HIT_CHESS_BORAD, OnHitChessBorad)
	ON_MESSAGE(IDM_AGREE_GAME_RULE, OnAgreeGameRule)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientDlg::CGameClientDlg() : CGameFrameDlg(&m_GameClientView)
{
	//配置变量
	m_bAutoSaveManual=false;

	//游戏变量
	m_cbChessColor=BLACK_CHESS;
	m_wBlackUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	memset(m_StatusInfo,0,sizeof(m_StatusInfo));
	memset(&m_GameRuleInfo,0,sizeof(m_GameRuleInfo));

	//辅助变量
	m_ShamView=false;
	m_bAgreeRule=false;
	m_bCustomize=false;
	m_bLimitTime=false;
	m_cbXSourcePos=255;
	m_cbYSourcePos=255;
	m_cbXTargetPos=255;
	m_cbYTargetPos=255;

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
	SetWindowText(TEXT("中国象棋"));

	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//读取配置
	m_bAutoSaveManual=AfxGetApp()->GetProfileInt(TEXT("GameOption"),TEXT("AutoSaveManual"),FALSE)?true:false;

	return true;
}

//重置框架
void CGameClientDlg::ResetGameFrame()
{
	//辅助变量
	m_ShamView=false;
	m_bAgreeRule=false;
	m_bCustomize=false;
	m_bLimitTime=false;
	m_cbXSourcePos=255;
	m_cbYSourcePos=255;
	m_cbXTargetPos=255;
	m_cbYTargetPos=255;

	//游戏变量
	m_cbChessColor=BLACK_CHESS;
	m_wBlackUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	memset(m_StatusInfo,0,sizeof(m_StatusInfo));
	memset(&m_GameRuleInfo,0,sizeof(m_GameRuleInfo));

	return;
}

//游戏设置
void CGameClientDlg::OnGameOptionSet()
{
	//构造数据
	CGameOption GameOption;
	GameOption.m_bEnableSound=IsEnableSound();
	GameOption.m_bAutoSaveManual=m_bAutoSaveManual;

	//配置数据
	if (GameOption.DoModal()==IDOK)
	{
		//设置变量
		m_bAutoSaveManual=GameOption.m_bAutoSaveManual;

		//保存配置
		AfxGetApp()->WriteProfileInt(TEXT("GameOption"),TEXT("AutoSaveManual"),m_bAutoSaveManual);

		//功能设置
		EnableSound(GameOption.m_bEnableSound);
	}

	return;
}

//时间消息
bool CGameClientDlg::OnTimerMessage(WORD wChairID, UINT nElapse, UINT nTimerID)
{
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
	case SUB_S_REQ_SET_RULE:	//请求设置
		{
			return OnSubSetRule(pBuffer,wDataSize);
		}
	case SUB_S_REQ_AGREE_RULE:	//请求同意
		{
			return OnSubAgreeRule(pBuffer,wDataSize);
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

	//游戏变量
	m_wBlackUser=pGameStart->wBlackUser;
	m_wCurrentUser=pGameStart->wBlackUser;
	m_GameRuleInfo=pGameStart->GameRuleInfo;
	m_cbChessColor=(m_wBlackUser==GetMeChairID())?BLACK_CHESS:WHITE_CHESS;

	//辅助变量
	m_ShamView=false;
	m_bCustomize=false;
	m_bAgreeRule=false;
	m_cbXSourcePos=255;
	m_cbYSourcePos=255;
	m_cbXTargetPos=255;
	m_cbYTargetPos=255;
	memset(m_StatusInfo,0,sizeof(m_StatusInfo));
	m_bLimitTime=((m_GameRuleInfo.wRuleStepTime>0)||(m_GameRuleInfo.wRuleLimitTime>0));

	//设置棋盘 
	m_GameLogic.ResetChessBorad();
	m_GameClientView.m_ChessBorad.SetChessColor(m_cbChessColor);
	m_GameClientView.m_ChessBorad.SetChessBorad(m_GameLogic.m_ChessBorad);

	//设置界面
	m_GameClientView.m_btPreserve.EnableWindow(TRUE);
	m_GameClientView.m_GameScoreWnd.ShowWindow(SW_HIDE);
	m_GameClientView.SetBlackUser(SwitchViewChairID(m_wBlackUser));

	//设置信息
	WORD wLimitTimeCount=m_GameRuleInfo.wRuleLimitTime;
	if (wLimitTimeCount==0) wLimitTimeCount=m_GameRuleInfo.wRuleStepTime;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		m_StatusInfo[i].wUseTimeCount=0;
		m_StatusInfo[i].wLimitTimeCount=wLimitTimeCount;
		m_GameClientView.SetUserStatusInfo(SwitchViewChairID(i),m_StatusInfo[i]);
	}

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
		//设置变量
		m_cbXSourcePos=255;
		m_cbYSourcePos=255;
		m_cbXTargetPos=255;
		m_cbYTargetPos=255;

		//设置界面
		ActiveGameFrame();
		m_GameClientView.m_ChessBorad.SetPositively(true);
		m_GameClientView.m_ChessBorad.SetSelectChess(m_cbXSourcePos,m_cbYSourcePos);
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

	//设置变量
	m_wCurrentUser=pMoveChess->wCurrentUser;
	WORD wMoveUser=(pMoveChess->wCurrentUser+1)%GAME_PLAYER;

	//设置界面
	m_ShamView=false;
	WORD wMeChairID=GetMeChairID();
	if ((IsLookonMode()==true)||(wMoveUser!=wMeChairID))
	{
		//变量定义
		BYTE cbXSourcePos=pMoveChess->cbXSourcePos,cbYSourcePos=pMoveChess->cbYSourcePos;
		BYTE cbXTargetPos=pMoveChess->cbXTargetPos,cbYTargetPos=pMoveChess->cbYTargetPos;

		//获取棋子
		const tagChessItem * pSourceChessItem=m_GameLogic.GetChessItem(cbXSourcePos,cbYSourcePos);
		const tagChessItem * pTargetChessItem=m_GameLogic.GetChessItem(cbXTargetPos,cbYTargetPos);

		//移动棋子
		m_GameLogic.MoveChess(cbXSourcePos,cbYSourcePos,cbXTargetPos,cbYTargetPos);

		//设置棋盘
		m_GameClientView.m_ChessBorad.SetChessBorad(m_GameLogic.m_ChessBorad);
		m_GameClientView.m_ChessBorad.SetChessFrame(cbXSourcePos,cbYSourcePos,cbXTargetPos,cbYTargetPos);

		//将军判断
		bool bKingDanger=m_GameLogic.IsKingDanger(BLACK_CHESS);
		if (bKingDanger==false) bKingDanger=m_GameLogic.IsKingDanger(WHITE_CHESS);

		//播放声音
		if (bKingDanger==false)
		{
			if ((pTargetChessItem!=NULL)&&(pSourceChessItem->cbColor!=pTargetChessItem->cbColor))
			{
				if (pSourceChessItem->cbColor==m_cbChessColor) PlayGameSound(AfxGetInstanceHandle(),TEXT("CHESS_EAT"));
				else PlayGameSound(AfxGetInstanceHandle(),TEXT("CHESS_DEAD"));
			}
			else PlayGameSound(AfxGetInstanceHandle(),TEXT("CHESS_MOVE"));
		}
		else PlayGameSound(AfxGetInstanceHandle(),TEXT("GENERAL"));
	}

	//用户信息
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		BYTE cbColor=(i==m_wBlackUser)?BLACK_CHESS:WHITE_CHESS;
		m_StatusInfo[i].wUseTimeCount=pMoveChess->wUseClock[i];
		m_StatusInfo[i].wLimitTimeCount=pMoveChess->wLeftClock[i];
		m_GameClientView.SetUserStatusInfo(SwitchViewChairID(i),m_StatusInfo[i]);
	}

	//玩家设置
	if ((IsLookonMode()==false)&&(pMoveChess->wCurrentUser==wMeChairID))
	{
		//设置变量
		m_cbXSourcePos=255;
		m_cbYSourcePos=255;
		m_cbXTargetPos=255;
		m_cbYTargetPos=255;

		//设置界面
		ActiveGameFrame();
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
		//设置变量
		m_cbXSourcePos=255;
		m_cbYSourcePos=255;
		m_cbXTargetPos=255;
		m_cbYTargetPos=255;

		//设置界面
		ActiveGameFrame();
		m_GameClientView.m_ChessBorad.SetPositively(true);
		m_GameClientView.m_ChessBorad.SetSelectChess(m_cbXSourcePos,m_cbYSourcePos);
	}

	//用户信息
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		BYTE cbColor=(i==m_wBlackUser)?BLACK_CHESS:WHITE_CHESS;
		m_StatusInfo[i].wUseTimeCount=pRegretResult->wUseClock[i];
		m_StatusInfo[i].wLimitTimeCount=pRegretResult->wLeftClock[i];
		m_GameClientView.SetUserStatusInfo(SwitchViewChairID(i),m_StatusInfo[i]);
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
		//设置变量
		m_cbXSourcePos=255;
		m_cbYSourcePos=255;
		m_cbXTargetPos=255;
		m_cbYTargetPos=255;

		//设置界面
		ActiveGameFrame();
		m_GameClientView.m_btStart.EnableWindow(TRUE);
		m_GameClientView.m_btStart.SetFocus();
		m_GameClientView.m_ChessBorad.SetSelectChess(m_cbXSourcePos,m_cbYSourcePos);

		//关闭窗口
		if (m_GameClientView.m_PeaceRequest.GetSafeHwnd()!=NULL) m_GameClientView.m_PeaceRequest.DestroyWindow();
		if (m_GameClientView.m_RegretRequest.GetSafeHwnd()!=NULL) m_GameClientView.m_RegretRequest.DestroyWindow();
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
		_snprintf(szFileName,sizeof(szFileName),TEXT("%s\\中国象棋棋谱_%04ld_%02ld_%02ld_%02ld_%02ld_%02ld.CCM"),
			szPath,SystemTime.wYear,SystemTime.wMonth,SystemTime.wDay,SystemTime.wHour,SystemTime.wMinute,SystemTime.wSecond);

		//保存棋谱
		if (SaveChessManual(szFileName)==false)	InsertSystemString(TEXT("中国象棋游戏“棋谱记录”保存失败"));
		else InsertSystemString(TEXT("中国象棋游戏“棋谱记录”保存成功"));
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

//请求设置
bool CGameClientDlg::OnSubSetRule(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_Req_SetRult));
	if (wDataSize!=sizeof(CMD_S_Req_SetRult)) return false;

	//消息处理
	CMD_S_Req_SetRult * pSetRult=(CMD_S_Req_SetRult *)pBuffer;
	m_GameRuleInfo=pSetRult->GameRuleInfo;

	//设置界面
	if (IsLookonMode()==false)
	{
		//设置变量
		m_bCustomize=true;
		m_bAgreeRule=false;
		m_wBlackUser=GetMeChairID();
		m_GameClientView.SetBlackUser(SwitchViewChairID(m_wBlackUser));

		//设置界面
		if (m_GameClientView.m_btStart.IsWindowEnabled()==FALSE)
		{
			//规则设置
			m_GameClientView.m_GameRule.m_GameRuleInfo=m_GameRuleInfo;

			//显示窗口
			if (m_GameClientView.m_GameRule.GetSafeHwnd()==NULL)
			{
				m_GameClientView.m_GameRule.Create(IDD_GAME_RULE,&m_GameClientView);
			}
			else m_GameClientView.m_GameRule.UpdateControlStatus();

			//设置控件
			m_GameClientView.m_GameRule.ControlEnable(true);
			m_GameClientView.m_GameRule.ShowWindow(SW_SHOW);

			//激活界面
			ActiveGameFrame();
		}
	}

	return true;
}

//请求同意
bool CGameClientDlg::OnSubAgreeRule(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_Req_AgreeRult));
	if (wDataSize!=sizeof(CMD_S_Req_AgreeRult)) return false;

	//消息处理
	CMD_S_Req_AgreeRult * pAgreeRult=(CMD_S_Req_AgreeRult *)pBuffer;
	m_GameRuleInfo=pAgreeRult->GameRuleInfo;

	//设置界面
	if (IsLookonMode()==false)
	{
		//设置变量
		m_bAgreeRule=true;
		m_bCustomize=false;

		//设置界面
		if (m_GameClientView.m_btStart.IsWindowEnabled()==FALSE)
		{
			//规则设置
			m_GameClientView.m_GameRule.m_GameRuleInfo=m_GameRuleInfo;

			//显示窗口
			if (m_GameClientView.m_GameRule.GetSafeHwnd()==NULL)
			{
				m_GameClientView.m_GameRule.Create(IDD_GAME_RULE,&m_GameClientView);
			}
			else m_GameClientView.m_GameRule.UpdateControlStatus();

			//设置控件
			m_GameClientView.m_GameRule.ControlEnable(false);
			m_GameClientView.m_GameRule.ShowWindow(SW_SHOW);

			//激活界面
			ActiveGameFrame();
		}
	}

	return true;
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
			m_GameRuleInfo=pStatusFree->GameRuleInfo;
			m_bCustomize=((IsLookonMode()==false)&&(m_wBlackUser==GetMeChairID()));

			//设置界面
			m_GameClientView.SetBlackUser(SwitchViewChairID(m_wBlackUser));

			//设置规则
			bool bShowGameRule=((IsLookonMode()==false)&&((m_wBlackUser==GetMeChairID())||(pStatusFree->cbGameRule==TRUE)));
			if (bShowGameRule==true)
			{
				//规则设置
				m_GameClientView.m_GameRule.m_GameRuleInfo=m_GameRuleInfo;

				//显示窗口
				if (m_GameClientView.m_GameRule.GetSafeHwnd()==NULL)
				{
					m_GameClientView.m_GameRule.Create(IDD_GAME_RULE,&m_GameClientView);
				}
				else m_GameClientView.m_GameRule.UpdateControlStatus();

				//设置控件
				bool bEnableControl=(m_wBlackUser==GetMeChairID());
				m_GameClientView.m_GameRule.ControlEnable(bEnableControl);
				m_GameClientView.m_GameRule.ShowWindow(SW_SHOW);
			}
			else InsertSystemString(TEXT("正等待“红方”玩家设置游戏规则"));

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
			m_GameRuleInfo=pStatusPlay->GameRuleInfo;
			m_cbChessColor=(m_wBlackUser==GetMeChairID())?BLACK_CHESS:WHITE_CHESS;
			m_bLimitTime=((m_GameRuleInfo.wRuleStepTime>0)||(m_GameRuleInfo.wRuleLimitTime>0));

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

			//用户信息
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				BYTE cbColor=(i==m_wBlackUser)?BLACK_CHESS:WHITE_CHESS;
				m_StatusInfo[i].wUseTimeCount=pStatusPlay->wUseClock[i];
				m_StatusInfo[i].wLimitTimeCount=pStatusPlay->wLeftClock[i];
				m_GameClientView.SetUserStatusInfo(SwitchViewChairID(i),m_StatusInfo[i]);
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
				if ((wStepCount>0)&&(m_GameRuleInfo.cbRegretFlag==TRUE))
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
		//设置时间
		if ((m_bLimitTime==true)&&(m_StatusInfo[m_wCurrentUser].wLimitTimeCount!=0xFFFF))
		{
			//递减时间
			m_StatusInfo[m_wCurrentUser].wLimitTimeCount--;

			//时间切换
			if (m_StatusInfo[m_wCurrentUser].wLimitTimeCount==0xFFFF)
			{
				if (m_GameRuleInfo.wRuleTimeOutCount>0)
				{
					//设置变量
					m_GameRuleInfo.wRuleTimeOutCount--;
					m_StatusInfo[m_wCurrentUser].wLimitTimeCount=m_GameRuleInfo.wRuleStepTime;

					//玩家设置
					if ((GetMeChairID()==m_wCurrentUser)&&(IsLookonMode()==false))
					{
						//发送消息
						SendData(SUB_C_TIME_OUT);

						//提示消息
						TCHAR szMessage[128];
						_snprintf(szMessage,sizeof(szMessage),TEXT("下棋超时了，还有 %ld 次 %ld 秒的超时机会"),
							m_GameRuleInfo.wRuleTimeOutCount+1,m_GameRuleInfo.wRuleStepTime);
						InsertSystemString(szMessage);
					}
				}
				else 
				{
					//删除定时器
					KillTimer(IDI_GAME_TIMER);
					m_StatusInfo[m_wCurrentUser].wLimitTimeCount=0;

					//玩家设置
					if ((GetMeChairID()==m_wCurrentUser)&&(IsLookonMode()==false))
					{
						//发送消息
						SendData(SUB_C_TIME_OUT);

						//设置界面
						m_GameClientView.m_btPeace.EnableWindow(FALSE);
						m_GameClientView.m_btRegret.EnableWindow(FALSE);
						m_GameClientView.m_btGiveUp.EnableWindow(FALSE);
						m_GameClientView.m_ChessBorad.SetPositively(false);

						//提示消息
						InsertSystemString(TEXT("下棋超时，你被判输"));
						PlayGameSound(AfxGetInstanceHandle(),"TIME_OUT");
					}
				}
			}

			//播放声音
			if ((m_StatusInfo[m_wCurrentUser].wLimitTimeCount<=5)&&(GetMeChairID()==m_wCurrentUser)&&(IsLookonMode()==false))
			{
				PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
			}
		}
		
		//设置视图
		m_StatusInfo[m_wCurrentUser].wUseTimeCount++;
		m_GameClientView.SetUserStatusInfo(SwitchViewChairID(m_wCurrentUser),m_StatusInfo[m_wCurrentUser]);

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
	memset(m_StatusInfo,0,sizeof(m_StatusInfo));

	//设置界面
	m_GameClientView.m_ChessBorad.CleanChess();
	m_GameClientView.m_btStart.EnableWindow(FALSE);
	m_GameClientView.m_btPreserve.EnableWindow(FALSE);
	m_GameClientView.m_GameScoreWnd.ShowWindow(SW_HIDE);

	//定制规则
	if ((m_bCustomize==true)||(m_bAgreeRule==true))
	{
		//规则设置
		m_GameClientView.m_GameRule.m_GameRuleInfo=m_GameRuleInfo;

		//显示窗口
		if (m_GameClientView.m_GameRule.GetSafeHwnd()==NULL)
		{
			m_GameClientView.m_GameRule.Create(IDD_GAME_RULE,&m_GameClientView);
		}
		else m_GameClientView.m_GameRule.UpdateControlStatus();

		//设置控件
		m_GameClientView.m_GameRule.ShowWindow(SW_SHOW);
		m_GameClientView.m_GameRule.ControlEnable(m_bCustomize);
	}
	else InsertSystemString(TEXT("正等待“红方”玩家设置游戏规则"));

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
	LPCTSTR pszFilter=TEXT("中国象棋棋谱文件 （*.CCM）|*.CCM||");
	CFileDialog DlgSaveManual(FALSE,TEXT("CCM"),NULL,OFN_NOCHANGEDIR|OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,pszFilter);
	DlgSaveManual.m_pOFN->lpstrInitialDir=szPath;
	if (DlgSaveManual.DoModal()!=IDOK) return 0;

	//保存棋谱
	if (SaveChessManual(DlgSaveManual.GetPathName())==false)	
	{
		InsertSystemString(TEXT("中国象棋游戏“棋谱记录”保存失败"));
	}
	else InsertSystemString(TEXT("中国象棋游戏“棋谱记录”保存成功"));

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

//点击棋盘
LRESULT CGameClientDlg::OnHitChessBorad(WPARAM wParam, LPARAM lParam)
{
	if (m_wCurrentUser==GetMeChairID())
	{
		//变量定义
		m_cbXTargetPos=(BYTE)wParam;
		m_cbYTargetPos=(BYTE)lParam;
		const tagChessItem * pChessItem=m_GameLogic.GetChessItem(m_cbXTargetPos,m_cbYTargetPos);

		//取消判断
		if ((m_cbXSourcePos==m_cbXTargetPos)&&(m_cbYSourcePos==m_cbYTargetPos))
		{
			m_cbXSourcePos=255;
			m_cbYSourcePos=255;
			m_cbXTargetPos=255;
			m_cbYTargetPos=255;
			m_GameClientView.m_ChessBorad.SetSelectChess(m_cbXSourcePos,m_cbYSourcePos);

			return 0;
		}

		//选择判断
		if ((pChessItem!=NULL)&&(pChessItem->cbColor==m_cbChessColor))
		{
			m_cbXSourcePos=m_cbXTargetPos;
			m_cbYSourcePos=m_cbYTargetPos;
			PlayGameSound(AfxGetInstanceHandle(),TEXT("CHESS_SELECT"));
			m_GameClientView.m_ChessBorad.SetSelectChess(m_cbXSourcePos,m_cbYSourcePos);

			return 0;
		}

		//走棋判断
		bool bMoveChess=false;
		if ((m_cbXSourcePos!=255)&&(m_cbYSourcePos!=255))
		{
			const tagChessItem * pSourceChessItem=m_GameLogic.GetChessItem(m_cbXSourcePos,m_cbYSourcePos);
			bMoveChess=m_GameLogic.IsWalkLegality(pSourceChessItem,m_cbXTargetPos,m_cbYTargetPos);
		}

		//走棋判断
		if (bMoveChess==true)
		{
			//获取棋子
			const tagChessItem * pSourceChessItem=m_GameLogic.GetChessItem(m_cbXSourcePos,m_cbYSourcePos);
			const tagChessItem * pTargetChessItem=m_GameLogic.GetChessItem(m_cbXTargetPos,m_cbYTargetPos);

			//移动棋子
			m_GameLogic.MoveChess(m_cbXSourcePos,m_cbYSourcePos,m_cbXTargetPos,m_cbYTargetPos);

			//设置棋盘
			m_GameClientView.m_ChessBorad.SetPositively(false);
			m_GameClientView.m_ChessBorad.SetChessBorad(m_GameLogic.m_ChessBorad);

			//危险判断
			if (m_GameLogic.IsKingDanger(m_cbChessColor)==true)
			{
				int nResult=AfxMessageBox(TEXT("[ 王 ] 将会被杀，确定要这样走棋吗？"),MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2);
				if (m_wCurrentUser!=GetMeChairID()) return 0;
				if (nResult!=IDYES)
				{
					//设置变量
					m_cbXSourcePos=255;
					m_cbYSourcePos=255;
					m_cbXTargetPos=255;
					m_cbYTargetPos=255;

					//悔棋控制
					m_GameLogic.RegretChess(1);
					m_GameClientView.m_ChessBorad.SetPositively(true);
					m_GameClientView.m_ChessBorad.SetChessBorad(m_GameLogic.m_ChessBorad);
					m_GameClientView.m_ChessBorad.SetSelectChess(m_cbXSourcePos,m_cbYSourcePos);

					return 0;
				}
			}

			//设置变量
			m_ShamView=true;
			m_wCurrentUser=INVALID_CHAIR;

			//设置棋盘
			m_GameClientView.m_ChessBorad.SetChessFrame(m_cbXSourcePos,m_cbYSourcePos,m_cbXTargetPos,m_cbYTargetPos);

			//悔棋判断
			WORD wStepCount=m_GameLogic.GetStepCount((GetMeChairID()==m_wBlackUser)?BLACK_CHESS:WHITE_CHESS);
			if (wStepCount==1) m_GameClientView.m_btRegret.EnableWindow(TRUE);

			//将军判断
			bool bKingDanger=m_GameLogic.IsKingDanger(BLACK_CHESS);
			if (bKingDanger==false) bKingDanger=m_GameLogic.IsKingDanger(WHITE_CHESS);

			//播放声音
			if (bKingDanger==false)
			{
				if (pTargetChessItem!=NULL) PlayGameSound(AfxGetInstanceHandle(),TEXT("CHESS_EAT"));
				else PlayGameSound(AfxGetInstanceHandle(),TEXT("CHESS_MOVE"));
			}
			else PlayGameSound(AfxGetInstanceHandle(),TEXT("GENERAL"));

			//发送消息
			CMD_C_MoveChess MoveChess;
			MoveChess.cbXSourcePos=m_cbXSourcePos;
			MoveChess.cbYSourcePos=m_cbYSourcePos;
			MoveChess.cbXTargetPos=m_cbXTargetPos;
			MoveChess.cbYTargetPos=m_cbYTargetPos;
			SendData(SUB_C_MOVE_CHESS,&MoveChess,sizeof(MoveChess));

			//设置变量
			m_cbXSourcePos=255;
			m_cbYSourcePos=255;
			m_cbXTargetPos=255;
			m_cbYTargetPos=255;
			m_GameClientView.m_ChessBorad.SetSelectChess(255,255);

			return 0;
		}
		else
		{
			//取消选择
			m_cbXSourcePos=255;
			m_cbYSourcePos=255;
			m_cbXTargetPos=255;
			m_cbYTargetPos=255;
			m_GameClientView.m_ChessBorad.SetSelectChess(m_cbXSourcePos,m_cbYSourcePos);

			return 0;
		}
	}

	return 0;
}

//同意规则
LRESULT CGameClientDlg::OnAgreeGameRule(WPARAM wParam, LPARAM lParam)
{
	//规则变量
	m_GameRuleInfo=m_GameClientView.m_GameRule.m_GameRuleInfo;

	//发送同意
	if (m_bCustomize==true)
	{
		//构造数据
		CMD_C_CustomizeRult CustomizeRult;
		CustomizeRult.GameRuleInfo=m_GameRuleInfo;

		//发送消息
		SendUserReady(&CustomizeRult,sizeof(CustomizeRult));
	}
	else SendUserReady(NULL,0);

	return 0;
}

//////////////////////////////////////////////////////////////////////////
