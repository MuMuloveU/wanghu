#include "StdAfx.h"
#include "GameClient.h"
#include "ScoreControl.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CScoreControl::CScoreControl()
{
	//设置变量
	m_bValid=false;
	ZeroMemory(&m_ScoreInfo,sizeof(m_ScoreInfo));

	//加载资源
	HINSTANCE hResInstance=AfxGetInstanceHandle();
	m_ImageWinLose.SetLoadInfo(IDB_WIN_LOSE,hResInstance);
	m_ImageGameScore.SetLoadInfo(IDB_GAME_SCORE,hResInstance);

	return;
}

//析构函数
CScoreControl::~CScoreControl()
{
}

//复位数据
VOID CScoreControl::RestorationData()
{
	//设置变量
	m_bValid=false;
	ZeroMemory(&m_ScoreInfo,sizeof(m_ScoreInfo));

	return;
}

//设置积分
VOID CScoreControl::SetScoreInfo(const tagScoreInfoC & ScoreInfo)
{
	//设置变量
	m_bValid=true;
	m_ScoreInfo=ScoreInfo;

	return;
}

//绘画界面
VOID CScoreControl::DrawScoreView(CDC * pDC, INT nXPos, INT nYPos)
{
	//有效判断
	if (m_bValid==false) return;

	//加载资源
	CImageHandle HandleWinLose(&m_ImageWinLose);
	CImageHandle HandleGameScore(&m_ImageGameScore);

	//变量定义
	INT nXBenchmark=nXPos-m_ImageGameScore.GetWidth()/2;
	INT nYBenchmark=nYPos-m_ImageGameScore.GetHeight()/2;
	UINT nFormat=DT_SINGLELINE|DT_END_ELLIPSIS|DT_VCENTER;

	//绘画背景
	m_ImageGameScore.BlendDrawImage(pDC,nXBenchmark,nYBenchmark,RGB(255,0,255),250);

	//绘画属性
	pDC->SetTextAlign(TA_LEFT|TA_TOP);
	pDC->SetTextColor(RGB(255,215,0));

	//绘画信息
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//变量定义
		TCHAR szUserScore[16]=TEXT("");
		_sntprintf(szUserScore,CountArray(szUserScore),TEXT("%I64d"),m_ScoreInfo.lGameScore[i]);

		//位置计算
		CRect rcName(nXBenchmark+30,nYBenchmark+70+i*30,nXBenchmark+128,nYBenchmark+124+i*24);
		CRect rcScore(nXBenchmark+128,nYBenchmark+70+i*30,nXBenchmark+203,nYBenchmark+124+i*24);

		//绘画信息
		pDC->DrawText(szUserScore,lstrlen(szUserScore),&rcScore,nFormat|DT_CENTER);
		pDC->DrawText(m_ScoreInfo.szUserName[i],lstrlen(m_ScoreInfo.szUserName[i]),&rcName,nFormat|DT_CENTER);

		//输赢标志
		INT nImageWidht=m_ImageWinLose.GetWidth()/3;
		INT nImageHeight=m_ImageWinLose.GetHeight();

		//输赢标志
		INT nImageExcursion=nImageWidht;
		if (m_ScoreInfo.lGameScore[i]>0L) nImageExcursion=0;
		if (m_ScoreInfo.lGameScore[i]==0L) nImageExcursion=2*nImageWidht;
		if (m_ScoreInfo.lGameScore[i]<0L) nImageExcursion=nImageWidht;
		m_ImageWinLose.AlphaDrawImage(pDC,nXBenchmark+225,nYBenchmark+80+i*30,nImageWidht,nImageHeight,nImageExcursion,0,RGB(255,0,255));
	}

	return;
}

//////////////////////////////////////////////////////////////////////////
