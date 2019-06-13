#include "StdAfx.h"
#include "CardControl.h"
#include "GameScoreWnd.h"
#include "DrawFunc.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameScoreWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameScoreWnd::CGameScoreWnd(IDataCtrl* pDataCtrl)
:	m_pDataCtrl(pDataCtrl)
{
	//设置变量
	ZeroMemory(&m_GameScoreInfo,sizeof(m_GameScoreInfo));
	m_ImageBack.SetLoadInfo(IDB_GAME_SCORE,AfxGetInstanceHandle());
}

//析构函数
CGameScoreWnd::~CGameScoreWnd()
{
}

//建立消息
int CGameScoreWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//移动窗口
	CImageHandle BackImageHandle(&m_ImageBack);
	SetWindowPos(NULL,0,0,m_ImageBack.GetWidth(),m_ImageBack.GetHeight(),SWP_NOZORDER|SWP_NOREPOSITION);

	return TRUE;
}

//重画函数
void CGameScoreWnd::OnPaint()
{
	CPaintDC dc(this);

	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//创建缓冲
	CDC BufferDC;
	CBitmap ImageBuffer;
	BufferDC.CreateCompatibleDC(&dc);
	ImageBuffer.CreateCompatibleBitmap(&dc,rcClient.Width(),rcClient.Height());
	BufferDC.SelectObject(&ImageBuffer);

	//绘画背景
	CImageHandle BackImageHandle(&m_ImageBack);
	m_ImageBack.BitBlt(BufferDC,0,0);

	//设置 DC
	BufferDC.SetBkMode(TRANSPARENT);
	BufferDC.SetTextColor(RGB(230,230,230));
	BufferDC.SelectObject(CSkinResourceManager::GetDefaultFont());

	//m_GameScoreInfo.wBankerUser = 1;
	//for(int i=0; i<4; i++)
	//{
	//	m_GameScoreInfo.lScore[i] = 80;
	//	strcpy(m_GameScoreInfo.szUserName[i], "慈溪玩家");
	//}
	//m_GameScoreInfo.stHuCourt.cbHuCardData = 1;
	//m_GameScoreInfo.stHuCourt.cbHuCardQueueType = EN_HU_CARD_QUEUE_13A;
	//m_GameScoreInfo.stHuCourt.wHuAttachedType = EN_HU_ATTACHED_ZIMO|EN_HU_ATTACHED_TIANDI|EN_HU_ATTACHED_ALL_ASK;
	//m_GameScoreInfo.stHuCourt.wFuseChair = 3;
	//m_GameScoreInfo.stHuCourt.wWinChair = 2;
	//m_GameScoreInfo.stHuCourt.AffordItem[0].wChair = 4;

	
	//绘画扑克
	BYTE cbHuCardData = m_GameScoreInfo.stHuCourt.cbHuCardData;
	int iX = 57, iY = 23;
	if(cbHuCardData == CARD_KING_DATA_MASK)
	{
		cbHuCardData = m_pDataCtrl->GetKingCardData();
		g_CardResource.m_ImageUserBottom.DrawCardItem(&BufferDC,cbHuCardData,iX,iY);
		RECT rcKing;
		rcKing.left		= iX+2;
		rcKing.top		= iY+5;
		rcKing.right	= iX+g_CardResource.m_ImageUserBottom.GetViewWidth();
		rcKing.bottom	= iY+g_CardResource.m_ImageUserBottom.GetViewHeight()-3;				
		CDrawFunc::DrawAlphaRect(&BufferDC, &rcKing, RGB(255,255,0), 0.6f);
	}
	else
	{
		g_CardResource.m_ImageUserBottom.DrawCardItem(&BufferDC,cbHuCardData,iX,iY);
	}
	
	//变量定义
	CRect rcDraw;
	TCHAR szBuffer[64];
	szBuffer[0] = 0;

	//胡牌详情
	iX = 119; 
	iY = 18;
	rcDraw.left = iX;
	rcDraw.right = rcDraw.left+170;
	rcDraw.top = iY;
	rcDraw.bottom = rcDraw.top+16;
	_snprintf(szBuffer,sizeof(szBuffer),TEXT("底 注： %ld"),m_pDataCtrl->GetCellScore());
	BufferDC.DrawText(szBuffer,lstrlen(szBuffer),&rcDraw,DT_LEFT|DT_VCENTER|DT_SINGLELINE);

	if(m_GameScoreInfo.stHuCourt.IsHu())
	{
		//基本胡
		int iPosCal = 0;
		CRect rcOrg = rcDraw + CPoint(0, 16);
		rcOrg.right = rcOrg.left + 85;

		if(m_GameScoreInfo.stHuCourt.wHuCardQueueType == EN_HU_CARD_QUEUE_WW)
		{
			rcDraw = rcOrg + CPoint((iPosCal%2)*85,(iPosCal/2)*16);
			iPosCal ++;
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("全风向  赢完"));
			BufferDC.DrawText(szBuffer,lstrlen(szBuffer),&rcDraw,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
		}
		else
		{
			rcDraw = rcOrg + CPoint((iPosCal%2)*85,(iPosCal/2)*16);
			iPosCal ++;
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("平  胡 %d龙"),SCORE_ADD_COMMON);
			BufferDC.DrawText(szBuffer,lstrlen(szBuffer),&rcDraw,DT_LEFT|DT_VCENTER|DT_SINGLELINE);

			if(m_GameScoreInfo.stHuCourt.wHuCardQueueType != EN_HU_CARD_QUEUE_3A2)
			{
				rcDraw = rcOrg + CPoint((iPosCal%2)*85,(iPosCal/2)*16);
				iPosCal ++;
				switch(m_GameScoreInfo.stHuCourt.wHuCardQueueType)
				{
				case EN_HU_CARD_QUEUE_3A2:
					break;
				case EN_HU_CARD_QUEUE_7TS:
					_snprintf(szBuffer,sizeof(szBuffer),TEXT("七对胡 %d龙"),SCORE_ADD_7TS);
					break;
				case EN_HU_CARD_QUEUE_13A:
					_snprintf(szBuffer,sizeof(szBuffer),TEXT("13不搭 %d龙"),SCORE_ADD_13A);
					break;
				case EN_HU_CARD_QUEUE_W7T:
					_snprintf(szBuffer,sizeof(szBuffer),TEXT("清七对 %d龙"),SCORE_ADD_W7T);
					break;
				}
				BufferDC.DrawText(szBuffer,lstrlen(szBuffer),&rcDraw,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
			}

			if(m_GameScoreInfo.stHuCourt.wHuAttachedType & EN_HU_ATTACHED_ZIMO)
			{
				_snprintf(szBuffer,sizeof(szBuffer),TEXT("自  摸 %d番"),SCORE_TIME_ZIMO);
				rcDraw = rcOrg + CPoint((iPosCal%2)*85,(iPosCal/2)*16);
				iPosCal ++;
				BufferDC.DrawText(szBuffer,lstrlen(szBuffer),&rcDraw,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
			}
			if(m_GameScoreInfo.stHuCourt.wHuAttachedType & EN_HU_ATTACHED_TIANDI)
			{
				_snprintf(szBuffer,sizeof(szBuffer),TEXT("天地胡 %d龙"),SCORE_ADD_TIANDI);
				rcDraw = rcOrg + CPoint((iPosCal%2)*85,(iPosCal/2)*16);
				iPosCal ++;
				BufferDC.DrawText(szBuffer,lstrlen(szBuffer),&rcDraw,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
			}
			if(m_GameScoreInfo.stHuCourt.wHuAttachedType & EN_HU_ATTACHED_KING_WAIT)
			{
				_snprintf(szBuffer,sizeof(szBuffer),TEXT("抛花麻 %d龙"),SCORE_ADD_KING_WAIT);
				rcDraw = rcOrg + CPoint((iPosCal%2)*85,(iPosCal/2)*16);
				iPosCal ++;
				BufferDC.DrawText(szBuffer,lstrlen(szBuffer),&rcDraw,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
			}
			if(m_GameScoreInfo.stHuCourt.wHuAttachedType & EN_HU_ATTACHED_WHOLE_COLOR)
			{
				_snprintf(szBuffer,sizeof(szBuffer),TEXT("清一色 %d龙"),SCORE_ADD_WHOLE_COLOR);
				rcDraw = rcOrg + CPoint((iPosCal%2)*85,(iPosCal/2)*16);
				iPosCal ++;
				BufferDC.DrawText(szBuffer,lstrlen(szBuffer),&rcDraw,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
			}
			if(m_GameScoreInfo.stHuCourt.wHuAttachedType & EN_HU_ATTACHED_ALL_ASK)
			{
				_snprintf(szBuffer,sizeof(szBuffer),TEXT("抓麻雀 %d龙"),SCORE_ADD_ALL_ASK);
				rcDraw = rcOrg + CPoint((iPosCal%2)*85,(iPosCal/2)*16);
				iPosCal ++;
				BufferDC.DrawText(szBuffer,lstrlen(szBuffer),&rcDraw,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
			}
			if(m_GameScoreInfo.stHuCourt.wHuAttachedType & EN_HU_ATTACHED_GANG_FLOWER)
			{
				_snprintf(szBuffer,sizeof(szBuffer),TEXT("杠上花 %d龙"),SCORE_ADD_GANG_FLOWER);
				rcDraw = rcOrg + CPoint((iPosCal%2)*85,(iPosCal/2)*16);
				iPosCal ++;
				BufferDC.DrawText(szBuffer,lstrlen(szBuffer),&rcDraw,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
			}
			if(m_GameScoreInfo.stHuCourt.wHuAttachedType & EN_HU_ATTACHED_7WINDS)
			{
				_snprintf(szBuffer,sizeof(szBuffer),TEXT("七风齐 %d龙"),SCORE_ADD_7WINDS);
				rcDraw = rcOrg + CPoint((iPosCal%2)*85,(iPosCal/2)*16);
				iPosCal ++;
				BufferDC.DrawText(szBuffer,lstrlen(szBuffer),&rcDraw,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
			}
			
			switch(m_GameScoreInfo.stHuCourt.cbKingCount)
			{
			case 0:
				_snprintf(szBuffer,sizeof(szBuffer),TEXT("无财神 2龙"));
				break;
			case 1:
				_snprintf(szBuffer,sizeof(szBuffer),TEXT("一财神 1龙"));
				break;
			case 2:;
				_snprintf(szBuffer,sizeof(szBuffer),TEXT("二财神 2龙"));
				break;
			case 3:
				_snprintf(szBuffer,sizeof(szBuffer),TEXT("三财神 3龙"));
				break;
			case 4:
				_snprintf(szBuffer,sizeof(szBuffer),TEXT("四财神 4龙"));
				break;
			}
			rcDraw = rcOrg + CPoint((iPosCal%2)*85,(iPosCal/2)*16);
			iPosCal ++;
			BufferDC.DrawText(szBuffer,lstrlen(szBuffer),&rcDraw,DT_LEFT|DT_VCENTER|DT_SINGLELINE);

			BYTE cbCount = m_GameScoreInfo.stHuCourt.cbBankerCount;
			if(cbCount > 1)
			{
				_snprintf(szBuffer,sizeof(szBuffer),TEXT("%d连庄 %d龙"), cbCount-1, (cbCount-1)*2);
				rcDraw = rcOrg + CPoint((iPosCal%2)*85,(iPosCal/2)*16);
				iPosCal ++;
				BufferDC.DrawText(szBuffer,lstrlen(szBuffer),&rcDraw,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
			}
		}		
	}
	

	//用户成绩
	iX = 20;
	iY = 138;
	for (int i=0;i<GAME_PLAYER;i++)
	{
		//用户名字
		rcDraw.left=iX;
		rcDraw.right=rcDraw.left+97;
		rcDraw.top=iY+i*25;
		rcDraw.bottom=rcDraw.top+12;
		BufferDC.DrawText(m_GameScoreInfo.szUserName[i],lstrlen(m_GameScoreInfo.szUserName[i]),&rcDraw,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

		//用户积分
		rcDraw.left=rcDraw.right;
		rcDraw.right=rcDraw.left+53;
		rcDraw.top=iY+i*25;
		rcDraw.bottom=rcDraw.top+12;
		_snprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),m_GameScoreInfo.lScore[i]);
		BufferDC.DrawText(szBuffer,lstrlen(szBuffer),&rcDraw,DT_CENTER|DT_VCENTER|DT_SINGLELINE);

		//用户状态
		szBuffer[0] = 0;
		rcDraw.left = rcDraw.right;
		rcDraw.right=rcDraw.left+44;
		rcDraw.top=iY+i*25;
		rcDraw.bottom=rcDraw.top+12;
		if (m_GameScoreInfo.stHuCourt.wWinChair == i) 
		{		
			if(m_GameScoreInfo.stHuCourt.wHuAttachedType & EN_HU_ATTACHED_ZIMO)
			{
				strcpy(szBuffer, _T("自摸"));
			}
			else
			{
				strcpy(szBuffer, _T("胡牌"));
			}
		}
		else if (m_GameScoreInfo.stHuCourt.AffordItem[0].wChair==i) 
		{
			strcpy(szBuffer, _T("包牌"));
		}
		else if(m_GameScoreInfo.stHuCourt.wFuseChair==i) 
		{
			strcpy(szBuffer, _T("放炮"));
		}
		BufferDC.DrawText(szBuffer,lstrlen(szBuffer),&rcDraw,DT_CENTER|DT_VCENTER|DT_SINGLELINE);

		//庄家
		szBuffer[0] = 0;
		rcDraw.left = rcDraw.right;
		rcDraw.right=rcDraw.left+75;
		rcDraw.top=iY+i*25;
		rcDraw.bottom=rcDraw.top+12;
		if (m_GameScoreInfo.wBankerUser==i) 
		{
			strcpy(szBuffer, _T("是"));
			BufferDC.DrawText(szBuffer,lstrlen(szBuffer),&rcDraw,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		}
		
	}

	//绘画界面
	dc.BitBlt(0,0,rcClient.Width(),rcClient.Height(),&BufferDC,0,0,SRCCOPY);

	//清理资源
	BufferDC.DeleteDC();
	ImageBuffer.DeleteObject();

	return;
}

//绘画背景
BOOL CGameScoreWnd::OnEraseBkgnd(CDC * pDC)
{
	Invalidate(FALSE);
	UpdateWindow();
	return TRUE;
}

//鼠标消息
void CGameScoreWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	__super::OnLButtonDown(nFlags,point);

	//消息模拟
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));

	return;
}

//设置积分
void CGameScoreWnd::SetScoreInfo(tagGameScoreInfo & GameScoreInfo)
{
	//设置变量
	m_GameScoreInfo=GameScoreInfo;

	//显示窗口
	Invalidate(NULL);
	ShowWindow(SW_SHOW);

	return;
}

//////////////////////////////////////////////////////////////////////////
