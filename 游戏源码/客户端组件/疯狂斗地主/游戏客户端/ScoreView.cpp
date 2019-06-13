#include "StdAfx.h"
#include "ScoreView.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CScoreView, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CScoreView::CScoreView(void)
{
	//设置数据
	m_lGameTax=0L;
	memset(m_lGameScore,0,sizeof(m_lGameScore));
	memset(m_szUserName,0,sizeof(m_szUserName));

	//加载资源
	m_ImageBack.SetLoadInfo(IDB_GAME_END,AfxGetInstanceHandle());
	m_ImageWinLose.SetLoadInfo(IDB_WIN_LOSE,AfxGetInstanceHandle());
}

//析构函数
CScoreView::~CScoreView(void)
{
}

//初始化函数
BOOL CScoreView::OnInitDialog()
{
	__super::OnInitDialog();

	//设置数据
	m_lGameTax=0L;
	memset(m_lGameScore,0,sizeof(m_lGameScore));
	memset(m_szUserName,0,sizeof(m_szUserName));

	//设置界面
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);

	//居中窗口
	CenterWindow(GetParent());

	CImageHandle ImageHandle(&m_ImageBack);
	if (ImageHandle.IsResourceValid())
	{
		//创建临时图
		CSkinImage BufferImage;
		int nImageWidth=m_ImageBack.GetWidth();
		int nImageHeight=m_ImageBack.GetHeight();
		BufferImage.Create(nImageWidth,nImageHeight,32);
		ImageHandle->BitBlt(BufferImage.GetDC(),0,0,nImageWidth,nImageHeight,0,0);
		BufferImage.ReleaseDC();

		//创建区域
		CRgn ImageRgn;
		BufferImage.CreateImageRegion( ImageRgn, RGB( 255, 0, 255 ) );
		if ( (( HRGN )ImageRgn) !=NULL)
		{
			SetWindowPos(NULL,0,0,nImageWidth,nImageHeight,SWP_NOMOVE);
			SetWindowRgn(( HRGN )ImageRgn,TRUE);

			ImageRgn.DeleteObject();

			return true;
		}
	}


	return TRUE;
}

//重画函数
void CScoreView::OnPaint() {

	CPaintDC dc(this); 

	//设置 DC
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(250,250,250));
	dc.SelectObject(CSkinResourceManager::GetDefaultFont());

	//绘画背景
	CImageHandle ImageHandle(&m_ImageBack);
	m_ImageBack.BitBlt(dc,0,0);

	//绘画标志
	CImageHandle ImageHandleWinLose( &m_ImageWinLose );

	//显示分数
	CRect rcDraw;
	TCHAR szBuffer[64]=TEXT("");
	for (WORD i=0;i<CountArray(m_szUserName);i++)
	{
		//用户名字
		rcDraw.left=26;
		rcDraw.right=117;
		rcDraw.top=i*28+87;
		rcDraw.bottom=rcDraw.top+28;
		dc.DrawText(m_szUserName[i],lstrlen(m_szUserName[i]),&rcDraw,DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS);

		//用户积分
		rcDraw.left=117;
		rcDraw.right=206;
		_snprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),m_lGameScore[i]);
		dc.DrawText(szBuffer,lstrlen(szBuffer),&rcDraw,DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS);

		//绘画标志
		int nWinLoseWidth = m_ImageWinLose.GetWidth() / 3;
		int nWinLoseHeight = m_ImageWinLose.GetHeight();
		int nDesPosX = ( 296 - 206 - nWinLoseWidth ) / 2 + 200;
		int nDesPoxY = ( 28 - nWinLoseHeight ) / 2 + 79 + i * 29;
		if ( m_lGameScore[ i ] == 0 ) {
			m_ImageWinLose.AlphaDrawImage( &dc, nDesPosX, nDesPoxY, nWinLoseWidth, nWinLoseHeight, 2 * nWinLoseWidth, 0, RGB( 255, 0, 255 ) );
		}
		else if ( 0 < m_lGameScore[ i ] ) {
			m_ImageWinLose.AlphaDrawImage( &dc, nDesPosX, nDesPoxY, nWinLoseWidth, nWinLoseHeight, 0, 0, RGB( 255, 0, 255 ) );
		}
		else {
			m_ImageWinLose.AlphaDrawImage( &dc, nDesPosX, nDesPoxY, nWinLoseWidth, nWinLoseHeight, nWinLoseWidth, 0, RGB( 255, 0, 255 ) );
		}
	}

	//显示税收
	rcDraw.left=26;
	rcDraw.right=117;
	rcDraw.top=i*28+88;
	rcDraw.bottom=rcDraw.top+28;
	LPCTSTR pszTax=TEXT("交易税");
//	dc.DrawText(pszTax,lstrlen(pszTax),&rcDraw,DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS);

	//显示税收
	rcDraw.left=117;
	rcDraw.right=206;
	rcDraw.top=i*28+88;
	rcDraw.bottom=rcDraw.top+28;
	_snprintf(szBuffer,sizeof(szBuffer),"%ld",m_lGameTax);
//	dc.DrawText(szBuffer,lstrlen(szBuffer),&rcDraw,DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS);	

	return;
}

//绘画背景
BOOL CScoreView::OnEraseBkgnd(CDC * pDC)
{
	Invalidate(FALSE);
	UpdateWindow();
	return TRUE;
}

//鼠标消息
void CScoreView::OnLButtonDown(UINT nFlags, CPoint point)
{
	__super::OnLButtonDown(nFlags,point);

	//消息模拟
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));

	return;
}

//重置积分
void CScoreView::ResetScore()
{
	//设置数据
	m_lGameTax=0L;
	memset(m_lGameScore,0,sizeof(m_lGameScore));
	memset(m_szUserName,0,sizeof(m_szUserName));

	//绘画视图
	if (m_hWnd!=NULL) Invalidate(NULL);

	return;
}

//设置税收
void CScoreView::SetGameTax(LONG lGameTax)
{
	//设置变量
	if (m_lGameTax!=lGameTax)
	{
		m_lGameTax=lGameTax;
		if (m_hWnd!=NULL) Invalidate(FALSE);
	}

	return;
}

//设置积分
void CScoreView::SetGameScore(WORD wChairID, LPCTSTR pszUserName, LONG lScore)
{
	//设置变量
	if (wChairID<CountArray(m_lGameScore))
	{
		m_lGameScore[wChairID]=lScore;
		lstrcpyn(m_szUserName[wChairID],pszUserName,CountArray(m_szUserName[wChairID]));
		if (m_hWnd!=NULL) Invalidate(FALSE);
	}

	return;
}

//////////////////////////////////////////////////////////////////////////
