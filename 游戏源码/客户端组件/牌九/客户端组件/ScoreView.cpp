#include "StdAfx.h"
#include "ScoreView.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CScoreView, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CScoreView::CScoreView(void)
{
	//设置数据
	m_lTax=0L;
	memset(m_lScore,0,sizeof(m_lScore));
	memset(m_szUserName,0,sizeof(m_szUserName));
	////////////////////////////////////
	bImage.LoadBitmap(IDB_GAME_END);
	////////////////////////////////////
	//加载资源
	m_ImageBack.SetLoadInfo(IDB_GAME_END,AfxGetInstanceHandle());
}

//析构函数
CScoreView::~CScoreView(void)
{
}

//建立消息
int CScoreView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//设置数据
	m_lTax=0L;
	memset(m_lScore,0,sizeof(m_lScore));
	memset(m_szUserName,0,sizeof(m_szUserName));

	//设置界面
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);

	//移动窗口
	CImageHandle ImageHandle(&m_ImageBack);
	SetWindowPos(NULL,0,0,m_ImageBack.GetWidth(),m_ImageBack.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);

	return 0;
}

//重画函数
void CScoreView::OnPaint() 
{
	CPaintDC dc(this); 
	///////////////////////////////////////
	SetupRgn(&dc,bImage,RGB(255,0,255));
	//SetupRgn(&dc,m_ImageBack,RGB(255,0,255));
	///////////////////////////////////////
	//设置 DC
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(255,255,255));
	dc.SelectObject(CSkinResourceManager::GetDefaultFont());

	//绘画背景
	CImageHandle ImageHandle(&m_ImageBack);
	//m_ImageBack.BitBlt(dc,0,0);
	m_ImageBack.AlphaDrawImage(&dc,0,0,RGB(255,0,255));	

	//绘画标题
	LPCTSTR pszAccount=TEXT("用户名"),pszScore=TEXT("成绩");
	dc.TextOut(70,70,pszAccount,lstrlen(pszAccount));
	dc.TextOut(180,70,pszScore,lstrlen(pszScore));

	//显示分数
	WORD wStation=0;
	TCHAR szBuffer[64]=TEXT("");
	for (WORD i=0;i<CountArray(m_szUserName);i++)
	{
		if (m_szUserName[i][0]!=0)
		{
			dc.TextOut(70,(int)(wStation*20+90),m_szUserName[i]);
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),m_lScore[i]);
			dc.TextOut(180,(int)(wStation*20+90),szBuffer,lstrlen(szBuffer));
			wStation++;
		}
	}

	//显示税收
	if (wStation>0)
	{
		LPCTSTR pszTax=TEXT("交易税");
		dc.TextOut(70,(int)(wStation*20+90),pszTax,lstrlen(pszTax));
		_snprintf(szBuffer,sizeof(szBuffer),"%ld",m_lTax);
		dc.TextOut(180,(int)(wStation*20+90),szBuffer,lstrlen(szBuffer));
	}

	return;
}

//重置积分
void CScoreView::ResetScore()
{
	//设置数据
	m_lTax=0L;
	memset(m_lScore,0,sizeof(m_lScore));
	memset(m_szUserName,0,sizeof(m_szUserName));

	//绘画视图
	Invalidate(NULL);
	
	return;
}

//设置税收
void CScoreView::SetTax(LONG lTax)
{
	if (m_lTax!=lTax)
	{
		m_lTax=lTax;
		Invalidate(NULL);
	}
	return;
}

//设置积分
void CScoreView::SetGameScore(WORD wChairID, LPCTSTR pszUserName, LONG lScore)
{
	if (wChairID<CountArray(m_lScore))
	{
		m_lScore[wChairID]=lScore;
		lstrcpyn(m_szUserName[wChairID],pszUserName,CountArray(m_szUserName[wChairID]));
		Invalidate(FALSE);
	}
	return;
}

//////////////////////////////////////////////////////////////////////////

void CScoreView::SetupRgn(CDC *pDC,				//窗体的DC指针
						  CBitmap &cBitmap,		//含有窗体形状的位图对象
						 // CSkinImage skinImage,
						  COLORREF TransColor)	//透明色
{

	CDC memDC;
	//创建与传入DC兼容的临时DC
	memDC.CreateCompatibleDC(pDC);

	CBitmap *pOldMemBmp=NULL;
	//将位图选入临时DC
	pOldMemBmp=memDC.SelectObject(&cBitmap);

	CRgn wndRgn;
	//创建总的窗体区域，初始region为0
	wndRgn.CreateRectRgn(0,0,0,0);


	BITMAP bit;   
	cBitmap.GetBitmap (&bit);//取得位图参数，这里要用到位图的长和宽     

	int y;
	for(y=0;y<=bit.bmHeight  ;y++)
	{
		CRgn rgnTemp; //保存临时region

		int iX = 0;
		do
		{
			//跳过透明色找到下一个非透明色的点.
			while (iX <= bit.bmWidth  && memDC.GetPixel(iX, y) == TransColor)
				iX++;

			//记住这个起始点
			int iLeftX = iX;

			//寻找下个透明色的点
			while (iX <= bit.bmWidth  && memDC.GetPixel(iX, y) != TransColor)
				++iX;

			//创建一个包含起点与重点间高为1像素的临时“region”
			rgnTemp.CreateRectRgn(iLeftX, y, iX, y+1);

			//合并到主"region".
			wndRgn.CombineRgn(&wndRgn, &rgnTemp, RGN_OR);

			//删除临时"region",否则下次创建时和出错
			rgnTemp.DeleteObject();
		}while(iX <bit.bmWidth );
		iX = 0;
	}
	if(pOldMemBmp)
		memDC.SelectObject(pOldMemBmp);
	//HRGN wndRgn=skinImage.CreateImageRegion(RGB(255,0,255));
	CWnd * pWnd = pDC->GetWindow();
	pWnd->SetWindowRgn(wndRgn,TRUE);    
	pWnd->SetForegroundWindow();    
}
