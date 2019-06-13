#include "StdAfx.h"
#include "GameClient.h"
#include "ScoreControl.h"
#include ".\scorecontrol.h"

//////////////////////////////////////////////////////////////////////////

//按钮标识
#define IDC_CLOSE_SCORE				100									//关闭成绩

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CScoreControl, CWnd)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_CLOSE_SCORE, OnBnClickedClose)
	ON_WM_NCDESTROY()
	ON_WM_MOVE()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CScoreControl::CScoreControl()
{
	//设置变量
	m_cbWeaveCount=0;
	ZeroMemory(&m_ScoreInfo,sizeof(m_ScoreInfo));

	//加载资源
	HINSTANCE hResInstance=AfxGetInstanceHandle();
	m_ImageWinLose.SetLoadInfo(IDB_WIN_LOSE,hResInstance);
	m_ImageGameScore.LoadFromResource(hResInstance,IDB_GAME_SCORE);
	m_ImageGameScoreFlag.SetLoadInfo(IDB_GAME_SCORE_FLAG,hResInstance);

	//设置控件
	for (BYTE i=0;i<CountArray(m_WeaveCard);i++) m_WeaveCard[i].SetDirection(Direction_South);

	return;
}

//析构函数
CScoreControl::~CScoreControl()
{
}

//复位数据
void CScoreControl::RestorationData()
{
	//设置变量
	m_cbWeaveCount=0;
	ZeroMemory(&m_ScoreInfo,sizeof(m_ScoreInfo));

	//隐藏窗口
	if (m_hWnd!=NULL) ShowWindow(SW_HIDE);

	return;
}

//设置积分
void CScoreControl::SetScoreInfo(const tagScoreInfo & ScoreInfo, const tagWeaveInfo & WeaveInfo)
{
	//设置变量
	m_ScoreInfo=ScoreInfo;
	m_cbWeaveCount=WeaveInfo.cbWeaveCount;

	//组合变量
	for (BYTE i=0;i<m_cbWeaveCount;i++)
	{
		bool bPublicWeave=(WeaveInfo.cbPublicWeave[i]==TRUE);
		m_WeaveCard[i].SetCardData(WeaveInfo.cbCardData[i],WeaveInfo.cbCardCount[i]);
		m_WeaveCard[i].SetDisplayItem(true);
	}

	//显示窗口
	ShowWindow(SW_SHOW);

	return;
}

//关闭按钮
void CScoreControl::OnBnClickedClose()
{
	//隐藏窗口
	RestorationData();

	return;
}

//重画函数
void CScoreControl::OnPaint()
{
	CPaintDC dc(this);

	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//创建字体
	CFont InfoFont;
	InfoFont.CreatePointFont(110,TEXT("宋体"),&dc);

	//创建缓冲
	CDC DCBuffer;
	CBitmap ImageBuffer;
	DCBuffer.CreateCompatibleDC(&dc);
	ImageBuffer.CreateCompatibleBitmap(&dc,rcClient.Width(),rcClient.Height());

	//设置 DC
	DCBuffer.SetBkMode(TRANSPARENT);
	DCBuffer.SelectObject(&ImageBuffer);
	DCBuffer.SetTextColor(RGB(255,255,255));
	DCBuffer.SelectObject(CSkinResourceManager::GetDefaultFont());

	//加载资源
	CImageHandle HandleWinLose(&m_ImageWinLose);
	CImageHandle HandleGameScoreFlag(&m_ImageGameScoreFlag);

	//绘画背景
	m_ImageGameScore.Draw(DCBuffer.GetSafeHdc(),0,0);

	//位置变量
	int nCardSpace=5;
	int nItemWidth=g_CardResource.m_ImageTableBottom.GetViewWidth();
	int nTotalWidth=m_cbWeaveCount*(nItemWidth*3+nCardSpace)+nItemWidth*m_ScoreInfo.cbCardCount+nCardSpace;

	//计算位置
	int nYCardPos=110;
	int nXCardPos=(m_ImageGameScore.GetWidth()-nTotalWidth)/2;

	//绘画组合
	for (BYTE i=0;i<m_cbWeaveCount;i++)
	{
		//绘画扑克
		m_WeaveCard[i].DrawCardControl(&DCBuffer,nXCardPos,nYCardPos);

		//设置位置
		nXCardPos+=(nCardSpace+nItemWidth*3);
	}

	//绘画扑克
	for (BYTE i=0;i<m_ScoreInfo.cbCardCount;i++)
	{
		//绘画扑克
		int nXCurrentPos=nXCardPos;
		int nYCurrentPos=nYCardPos-g_CardResource.m_ImageTableBottom.GetViewHeight()-5;
		g_CardResource.m_ImageTableBottom.DrawCardItem(&DCBuffer,m_ScoreInfo.cbCardData[i],nXCurrentPos,nYCurrentPos);

		//设置位置
		nXCardPos+=nItemWidth;
		if ((i+2)==m_ScoreInfo.cbCardCount) nXCardPos+=nCardSpace;
	}

	//绘画牌型
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//用户过虑
		if (m_ScoreInfo.dwChiHuKind[i]==CHK_NULL) continue;

		//牌型信息
		DWORD dwCardKind[]={CHK_PENG_PENG,CHK_QI_XIAO_DUI,CHK_SHI_SAN_YAO};
		DWORD dwCardRight[]={CHR_DI,CHR_TIAN,CHR_QING_YI_SE,CHR_QIANG_GANG,CHK_QUAN_QIU_REN};

		//牌型信息
		CString strCardInfo;
		LPCTSTR pszCardKind[]={TEXT("碰碰胡"),TEXT("七小对"),TEXT("十三幺")};
		LPCTSTR pszCardRight[]={TEXT("地胡"),TEXT("天胡"),TEXT("清一色"),TEXT("杠胡"),TEXT("全求人")};

		//牌型信息
		for (BYTE j=0;j<CountArray(dwCardKind);j++)
		{
			if (m_ScoreInfo.dwChiHuKind[i]&dwCardKind[j])
			{
				strCardInfo+=pszCardKind[j];
				strCardInfo+=TEXT("  ");
			}
		}

		//牌权信息
		for (BYTE j=0;j<CountArray(dwCardRight);j++)
		{
			if (m_ScoreInfo.dwChiHuRight[i]&dwCardRight[j])
			{
				strCardInfo+=pszCardRight[j];
				strCardInfo+=TEXT("  ");
			}
		}

		//绘画信息
		CRect rcCardInfo(20,310,406,322);
		DCBuffer.DrawText(strCardInfo,rcCardInfo,DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

		break;
	}

	//设置 DC
	DCBuffer.SelectObject(InfoFont);
	DCBuffer.SetTextColor(RGB(255,255,255));

	//积分信息
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//变量定义
		TCHAR szUserScore[16]=TEXT("");
		_sntprintf(szUserScore,CountArray(szUserScore),TEXT("%d"),m_ScoreInfo.lGameScore[i]);

		//位置计算
		CRect rcName(29,165+i*30,132,165+(i+1)*30);
		CRect rcScore(130,165+i*30,190,165+(i+1)*30);
		CRect rcStatus(220,165+i*30,260,165+(i+1)*30);

		//绘画信息
		UINT nFormat=DT_SINGLELINE|DT_END_ELLIPSIS|DT_VCENTER;
		DCBuffer.DrawText(szUserScore,lstrlen(szUserScore),&rcScore,nFormat|DT_CENTER);
		DCBuffer.DrawText(m_ScoreInfo.szUserName[i],lstrlen(m_ScoreInfo.szUserName[i]),&rcName,nFormat|DT_CENTER);

		//庄家标志
		if (m_ScoreInfo.wBankerUser==i)
		{
			int nImageWidht=m_ImageGameScoreFlag.GetWidth();
			int nImageHeight=m_ImageGameScoreFlag.GetHeight();
			m_ImageGameScoreFlag.BlendDrawImage(&DCBuffer,310,168+i*30,RGB(255,0,255),240);
		}
		//用户状态
		if ((m_ScoreInfo.dwChiHuKind[i]!=0)&&((m_ScoreInfo.wProvideUser!=i)))
		DCBuffer.DrawText(TEXT("胡牌"),lstrlen(TEXT("胡牌")),&rcStatus,nFormat|DT_CENTER);

		//用户状态
		if ((m_ScoreInfo.wProvideUser==i)&&(m_ScoreInfo.dwChiHuKind[i]==0))
			DCBuffer.DrawText(TEXT("放炮"),lstrlen(TEXT("放炮")),&rcStatus,nFormat|DT_CENTER);

		//用户状态
		if ((m_ScoreInfo.dwChiHuKind[i]!=0)&&((m_ScoreInfo.wProvideUser==i)))
		DCBuffer.DrawText(TEXT("自摸"),lstrlen(TEXT("自摸")),&rcStatus,nFormat|DT_CENTER);



		//输赢标志
		int nImageWidht=m_ImageWinLose.GetWidth()/3;
		int nImageHeight=m_ImageWinLose.GetHeight();

		//绘画标志
		int nImageExcursion=2*nImageWidht;
		if (m_ScoreInfo.lGameScore[i]>0L) nImageExcursion=0;
		if (m_ScoreInfo.lGameScore[i]<0L) nImageExcursion=nImageWidht;
		m_ImageWinLose.BlendDrawImage(&DCBuffer,390,169+i*29,nImageWidht,nImageHeight,nImageExcursion,0,RGB(255,0,255),240);
	}

	//绘画界面
	dc.BitBlt(0,0,rcClient.Width(),rcClient.Height(),&DCBuffer,0,0,SRCCOPY);

	//清理资源
	DCBuffer.DeleteDC();
	InfoFont.DeleteObject();
	ImageBuffer.DeleteObject();

	return;
}

//绘画背景
BOOL CScoreControl::OnEraseBkgnd(CDC * pDC)
{
	//更新界面
	Invalidate(FALSE);
	UpdateWindow();

	return TRUE;
}

//创建函数
int CScoreControl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;


	//设置窗口
	SetWindowPos(NULL,0,0,m_ImageGameScore.GetWidth(),m_ImageGameScore.GetHeight(),SWP_NOZORDER|SWP_NOMOVE);

	//创建按钮
	CRect rcCreate(0,0,0,0);
	m_btCloseScore.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_CLOSE_SCORE);
	m_btCloseScore.SetButtonImage(IDB_BT_SCORE_CLOSE,AfxGetInstanceHandle(),false);

	//调整按钮
	CRect rcClient;
	GetClientRect(&rcClient);
	m_btCloseScore.SetWindowPos(NULL,rcClient.Width()-31,3,0,0,SWP_NOSIZE|SWP_NOZORDER);

	return 0;
}

//鼠标消息
void CScoreControl::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags,Point);

	//消息模拟
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(Point.x,Point.y));

	return;
}

//////////////////////////////////////////////////////////////////////////


void CScoreControl::OnMove(int x, int y)
{
	__super::OnMove(x, y);

	//更新界面
	Invalidate(FALSE);
	UpdateWindow();

}
