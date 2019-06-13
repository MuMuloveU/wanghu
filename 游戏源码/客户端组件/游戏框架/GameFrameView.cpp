#include "StdAfx.h"
#include "Resource.h"
#include "GlobalOption.h"
#include "GameFrameView.h"
#include ".\gameframeview.h"

//////////////////////////////////////////////////////////////////////////

//静态变量
const int			CGameFrameView::m_nXFace=32;						//头像高度
const int			CGameFrameView::m_nYFace=32;						//头像宽度
const int			CGameFrameView::m_nXTimer=48;						//定时器宽
const int			CGameFrameView::m_nYTimer=48;						//定时器高
const int			CGameFrameView::m_nXBorder=0;						//定时器高
const int			CGameFrameView::m_nYBorder=0;						//定时器高

//时间定义
#define IDI_FLOWER_START			3000								//鲜花定时器

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameFrameView, CWnd)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameFrameView::CGameFrameView(bool bDoubleBuf ,UINT uColorCount, CThreadDraw * pThreadDraw) 
	: m_bDoubleBuf(bDoubleBuf), m_uColorCount(uColorCount), m_pThreadDraw(pThreadDraw)
{
	//设置变量
	m_bReDraw=true;
	m_pReserve=NULL;
	m_nAnimeStep=30;
	m_pIUserFaceRes=NULL;
	memset(&m_wTimer,0,sizeof(m_wTimer));
	memset(&m_ptName,0,sizeof(m_ptName));
	memset(&m_ptFace,0,sizeof(m_ptFace));
	memset(&m_ptTimer,0,sizeof(m_ptTimer));
	memset(&m_ptReady,0,sizeof(m_ptReady));
	memset(&m_pUserItem,0,sizeof(m_pUserItem));
	ZeroMemory(m_FlowerInfo, sizeof(m_FlowerInfo));
	m_nFlowerCount=0;

	return;
}

//析构函数
CGameFrameView::~CGameFrameView()
{
	//释放控件
	for(WORD i=0;i<CountArray(m_FlowerEffectControl);i++)
		if ( m_FlowerEffectControl[i].m_hWnd != NULL )
			m_FlowerEffectControl[i].DestroyWindow();
}

//接口查询
void * __cdecl CGameFrameView::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IGameFrameView,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IGameFrameView,Guid,dwQueryVer);
	return NULL;
}

//更新界面
void CGameFrameView::UpdateGameView(const CRect * pRect)
{
	if (m_bDoubleBuf==true) m_bReDraw=true;
	if (pRect!=NULL) InvalidateRect(pRect,FALSE);
	else Invalidate(FALSE);
	return;
}

//获取界面
bool CGameFrameView::GetGameViewImage(CImage & ImageBuffer, int nXPos, int nYPos, int nWidth, int nHeight)
{
	ASSERT(ImageBuffer.IsNull()==false);
	ASSERT(m_ImageBuffer.IsNull()==false);
	if (m_ImageBuffer.IsNull()==false)
	{
		m_ImageBuffer.BitBlt(ImageBuffer.GetDC(),nXPos,nYPos,nWidth,nHeight,nXPos,nYPos);
		ImageBuffer.ReleaseDC();
		return true;
	}
	return false;
}

//设置位置
void CGameFrameView::SetFlowerControlInfo(WORD wChairID,int nXPos, int nYPos, int nWidth, int nHeight, UINT nAnimeStep)
{
	//合法判断
	ASSERT(wChairID<CountArray(m_FlowerEffectControl));
	if (wChairID>=CountArray(m_FlowerEffectControl)) return;

	//设置变量
	m_nAnimeStep = nAnimeStep;

	//移动窗体
	if ( m_FlowerEffectControl[wChairID].m_hWnd != NULL )
	{
		//坐标转换
		CRect rcScreen(nXPos, nYPos, nXPos+nWidth, nYPos+nHeight);
		ClientToScreen(&rcScreen);

		//移动窗体
		m_FlowerEffectControl[wChairID].MoveWindow(rcScreen);
	}

	return ;
}

//绘画鲜花
void CGameFrameView::ShowFlower(WORD wSourceChairID, WORD wTargetChairID, UINT uFlowerID, UINT uFlowerEffectID, bool bBombEffect)
{
	//合法判断
	ASSERT(wTargetChairID<CountArray(m_FlowerEffectControl) && wSourceChairID<CountArray(m_FlowerEffectControl));
	if (wTargetChairID>=CountArray(m_FlowerEffectControl) || wSourceChairID>=CountArray(m_FlowerEffectControl)) return;
	if(m_FlowerEffectControl[wTargetChairID].m_hWnd == NULL || m_FlowerEffectControl[wSourceChairID].m_hWnd == NULL) return ;
	if (m_nFlowerCount>0) return;

	for ( int n = 0; n < MAX_FLOWER_COUNT; ++n )
	{
		tagFlowerEffectInfo & FlowerInfo = m_FlowerInfo[n];

		if ( FlowerInfo.wMoveCount == 0 ) 
		{
			//获取位置
			CRect rcVideo,rcTargetVideo;
			m_FlowerEffectControl[wSourceChairID].GetClientRect(&rcVideo);
			m_FlowerEffectControl[wTargetChairID].GetClientRect(&rcTargetVideo);
			m_FlowerEffectControl[wSourceChairID].MapWindowPoints(this, &rcVideo);
			m_FlowerEffectControl[wTargetChairID].MapWindowPoints(this, &rcTargetVideo);

			//位置信息
			FlowerInfo.ptFrom.x = rcVideo.left + rcVideo.Width()/2;
			FlowerInfo.ptFrom.y = rcVideo.top  + rcVideo.Height()/2;
			FlowerInfo.ptDest.x = rcTargetVideo.left + rcTargetVideo.Width()/2;
			FlowerInfo.ptDest.y = rcTargetVideo.top  + rcTargetVideo.Height()/2;

			//判断位置
			if(FlowerInfo.ptFrom.x==FlowerInfo.ptDest.x && FlowerInfo.ptFrom.y==FlowerInfo.ptDest.y)
			{
				//显示动画
				m_FlowerEffectControl[wTargetChairID].StartFlowerEffect(uFlowerEffectID);
				m_FlowerEffectControl[wTargetChairID].BringWindowToTop();

				//清除变量
				ZeroMemory(&FlowerInfo, sizeof(FlowerInfo));
			}
			else
			{
				//起始位置
				FlowerInfo.ptCurPos = FlowerInfo.ptFrom ;

				//步数计算
				INT nXCount=abs(FlowerInfo.ptDest.x-FlowerInfo.ptFrom.x)/m_nAnimeStep;
				INT nYCount=abs(FlowerInfo.ptDest.y-FlowerInfo.ptFrom.y)/m_nAnimeStep;
				FlowerInfo.wMoveCount=__max(1,__max(nXCount,nYCount));
				FlowerInfo.wMoveIndex = 0;

				//用户信息
				FlowerInfo.wSourceChairID = wSourceChairID;
				FlowerInfo.wTargetChairID = wTargetChairID;
				FlowerInfo.uFlowerID = uFlowerID;
				FlowerInfo.uFlowerEffectID = uFlowerEffectID;
				FlowerInfo.bBombEffect = bBombEffect;

				//递增次数
				m_nFlowerCount++;

				//首次判断
				if ( m_nFlowerCount == 1 ) SetTimer(IDI_FLOWER_START, 100, NULL);
			}
			break;
		}
	}

	return ;
}

//获取时间
WORD CGameFrameView::GetUserTimer(WORD wChairID)
{
	ASSERT(wChairID<MAX_CHAIR);
	if (wChairID>=MAX_CHAIR) return 0;
	return m_wTimer[wChairID];
}

//获取玩家
const tagUserData * CGameFrameView::GetUserInfo(WORD wChairID)
{
	ASSERT(wChairID<MAX_CHAIR);
	if (wChairID>=MAX_CHAIR) return NULL;
	return m_pUserItem[wChairID];
}

//绘画背景
void CGameFrameView::DrawViewImage(CDC * pDC, CSkinImage & SkinImage, enImageMode ImageMode)
{
	//获取位置
	RECT rcClient;
	GetClientRect(&rcClient);

	//绘画位图
	switch (ImageMode)
	{
	case enMode_ElongGate:	//拉伸模式
		{
			CImageHandle ImageHandle(&SkinImage);
			int nImageWidth=ImageHandle->GetWidth();
			int nImageHeight=ImageHandle->GetHeight();
			ImageHandle->StretchBlt(pDC->m_hDC,0,0,rcClient.right,rcClient.bottom,0,0,nImageWidth,nImageHeight);
			return;
		}
	case enMode_Centent:	//居中模式
		{
			CImageHandle ImageHandle(&SkinImage);
			int nXPos=(rcClient.right-ImageHandle->GetWidth())/2;
			int nYPos=(rcClient.bottom-ImageHandle->GetHeight())/2;
			ImageHandle->BitBlt(pDC->m_hDC,nXPos,nYPos);
			return;
		}
	case enMode_Spread:		//平铺模式
		{
			CImageHandle ImageHandle(&SkinImage);
			int nImageWidth=ImageHandle->GetWidth();
			int nImageHeight=ImageHandle->GetHeight();
			for (int nXPos=0;nXPos<rcClient.right;nXPos+=nImageWidth)
			{
				for (int nYPos=0;nYPos<rcClient.bottom;nYPos+=nImageHeight)
				{
					ImageHandle->BitBlt(pDC->m_hDC,nXPos,nYPos);
				}
			}
			return;
		}
	}
	return;
}

//绘画头像
void CGameFrameView::DrawUserFace(CDC * pDC,const tagUserData * pUserData, int nXPos, int nYPos, bool bOffLine)
{
	ASSERT(m_pIUserFaceRes!=NULL && pUserData!=NULL);
	m_pIUserFaceRes->DrawNormalFace(pDC,nXPos,nYPos,pUserData->wFaceID,pUserData->dwUserID,pUserData->dwCustomFaceVer);
	return;
}

//绘画头像
void CGameFrameView::DrawCustomFace(CDC * pDC,const tagUserData * pUserData, int nXPos, int nYPos)
{
	ASSERT(m_pIUserFaceRes!=NULL && pUserData!=NULL);
	m_pIUserFaceRes->DrawCustomBigFace(pDC,nXPos,nYPos,pUserData->dwUserID,pUserData->dwCustomFaceVer);

	return;
}

//绘画准备
void CGameFrameView::DrawUserReady(CDC * pDC, int nXPos, int nYPos)
{
	//加载资源
	CPngImage ImageUserReady;
	ImageUserReady.LoadImage(GetModuleHandle(GAME_FRAME_DLL_NAME),TEXT("USER_READY"));

	//绘画准备
	CSize SizeImage(ImageUserReady.GetWidth(),ImageUserReady.GetHeight());
	ImageUserReady.DrawImage(pDC,nXPos-SizeImage.cx/2,nYPos-SizeImage.cy/2);

	return;
}

//绘画时间
void CGameFrameView::DrawUserTimer(CDC * pDC, int nXPos, int nYPos, WORD wTime, WORD wTimerArea)
{
	//加载资源
	CPngImage ImageTimeBack;
	CPngImage ImageTimeNumber;
	ImageTimeBack.LoadImage(GetModuleHandle(GAME_FRAME_DLL_NAME),TEXT("TIME_BACK"));
	ImageTimeNumber.LoadImage(GetModuleHandle(GAME_FRAME_DLL_NAME),TEXT("TIME_NUMBER"));

	//获取属性
	const INT nNumberHeight=ImageTimeNumber.GetHeight();
	const INT nNumberWidth=ImageTimeNumber.GetWidth()/10;

	//计算数目
	LONG lNumberCount=0;
	WORD wNumberTemp=wTime;
	do
	{
		lNumberCount++;
		wNumberTemp/=10;
	} while (wNumberTemp>0L);

	//位置定义
	INT nYDrawPos=nYPos-nNumberHeight/2+1;
	INT nXDrawPos=nXPos+(lNumberCount*nNumberWidth)/2-nNumberWidth;

	//绘画背景
	CSize SizeTimeBack(ImageTimeBack.GetWidth(),ImageTimeBack.GetHeight());
	ImageTimeBack.DrawImage(pDC,nXPos-SizeTimeBack.cx/2,nYPos-SizeTimeBack.cy/2);

	//绘画号码
	for (LONG i=0;i<lNumberCount;i++)
	{
		//绘画号码
		WORD wCellNumber=wTime%10;
		ImageTimeNumber.DrawImage(pDC,nXDrawPos,nYDrawPos,nNumberWidth,nNumberHeight,wCellNumber*nNumberWidth,0);

		//设置变量
		wTime/=10;
		nXDrawPos-=nNumberWidth;
	};

	return;
}

//透明绘画
void CGameFrameView::AlphaDrawImage(CDC * pDesDC, int nXDes, int nYDes, int nDesWidth, int nDesHeight, CDC * pScrDC, int nXScr, int nYScr, COLORREF crTransColor)
{
	::AlphaDrawImage(pDesDC,nXDes,nYDes,nDesWidth,nDesHeight,pScrDC,nXScr,nYScr,nDesWidth,nDesHeight,crTransColor);
	return;
}

//绘画透明图
void CGameFrameView::AlphaDrawImage(CDC * pDesDC, int nXDes, int nYDes, int nDesWidth, int nDesHeight, HBITMAP hBitBmp, int nXScr, int nYScr, COLORREF crTransColor)
{
	//创建 DC
	CDC DCBuffer;
	DCBuffer.CreateCompatibleDC(NULL);
	HGDIOBJ hOldGdiObj=DCBuffer.SelectObject(hBitBmp);

	//绘画位图
	::AlphaDrawImage(pDesDC,nXDes,nYDes,nDesWidth,nDesHeight,&DCBuffer,nXScr,nYScr,nDesWidth,nDesHeight,crTransColor);

	//清理资源
	DCBuffer.SelectObject(hOldGdiObj);
	DCBuffer.DeleteDC();

	return;
}

//重置界面
void CGameFrameView::ResetData()
{
	//设置变量
	memset(m_wTimer,0,sizeof(m_wTimer));
	memset(m_pUserItem,0,sizeof(m_pUserItem));

	//重置界面
	ResetGameView();
	UpdateGameView(NULL);

	return;
}

//设置接口
bool CGameFrameView::SetUserFaceRes(IUnknownEx * pIUnknownEx)
{
	ASSERT(pIUnknownEx!=NULL);
	m_pIUserFaceRes=(IUserFaceRes *)pIUnknownEx->QueryInterface(IID_IUserFaceRes,VER_IUserFaceRes);
	return (m_pIUserFaceRes!=NULL);
}

//设置时间
bool CGameFrameView::SetUserTimer(WORD wChairID, WORD wTimer)
{
	ASSERT(wChairID<MAX_CHAIR);
	if (wChairID>=MAX_CHAIR) return false;
	m_wTimer[wChairID]=wTimer;
	UpdateGameView(NULL);
	return true;
}

//设置用户
bool CGameFrameView::SetUserInfo(WORD wChairID, tagUserData * pUserItem)
{
	ASSERT(wChairID<MAX_CHAIR);
	if (wChairID>=MAX_CHAIR) return false;
	m_pUserItem[wChairID]=pUserItem;
	UpdateGameView(NULL);
	return true;
}

//创建消息
int CGameFrameView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//初始化
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);
	if (m_pThreadDraw!=NULL) m_pThreadDraw->BeginThread();
	
	//创建控件
	for(WORD i=0;i<CountArray(m_FlowerEffectControl);i++)
	{
		SetFlowerControlInfo(i,0,0);
		m_FlowerEffectControl[i].CreateControl(this);
	}

	return 0;
}

//绘画函数
void CGameFrameView::OnPaint()
{
	CPaintDC dc(this);

	//判断模式
	if (m_pThreadDraw!=NULL) return;

	//获取位置
	CRect ClientRect;
	GetClientRect(&ClientRect);

	//重画缓冲区
	if ((m_bDoubleBuf==true)&&(m_ImageBuffer.IsNull()==false))
	{
		if (m_bReDraw==true)
		{
			//创建字体
			CFont DrawFont;
			DrawFont.CreateFont(-12,0,0,0,400,0,0,0,134,3,2,1,2,TEXT("宋体"));
			CDC * pDC=CDC::FromHandle(m_ImageBuffer.GetDC());
			CFont * pOldFont=pDC->SelectObject(&DrawFont);

			//更新缓冲
			m_bReDraw=false;
			pDC->SetBkMode(TRANSPARENT);
			pDC->FillSolidRect(0,0,ClientRect.Width(),ClientRect.Height(),RGB(0,0,0));

			//绘画界面
			DrawGameView(pDC,ClientRect.Width(),ClientRect.Height());

			//绘画鲜花
			for ( int n = 0; n < MAX_FLOWER_COUNT; ++n )
			{
				tagFlowerEffectInfo & FlowerInfo = m_FlowerInfo[n];

				if ( FlowerInfo.ptCurPos.x == 0 && FlowerInfo.ptCurPos.y == 0 ) continue;
				//m_FlowerResHelper->DrawGift(pDC, FlowerInfo.ptCurPos.x, FlowerInfo.ptCurPos.y, FlowerInfo.uFlowerID);

				//加载图片
				CPngImage PngGift;
				PngGift.LoadImage(g_GlobalOption.m_PlatformResourceModule->GetResInstance(),g_GlobalOption.m_GiftFlashImage.pszGiftMove[FlowerInfo.uFlowerID]);

				//绘画图片
				PngGift.DrawImage(pDC, FlowerInfo.ptCurPos.x, FlowerInfo.ptCurPos.y);
			}

			//清理资源
			pDC->SelectObject(pOldFont);
			m_ImageBuffer.ReleaseDC();
			DrawFont.DeleteObject();
		}

		//绘画界面
		CRect rcClip;
		dc.GetClipBox(&rcClip);
		m_ImageBuffer.BitBlt(dc,rcClip.left,rcClip.top,rcClip.Width(),rcClip.Height(),rcClip.left,rcClip.top);
	}
	else 
	{
		//创建字体
		CFont DrawFont;
		DrawFont.CreateFont(-12,0,0,0,400,0,0,0,134,3,2,1,2,TEXT("宋体"));
		CFont * pOldFont=dc.SelectObject(&DrawFont);

		//绘画界面
		dc.SetBkMode(TRANSPARENT);
		DrawGameView(&dc,ClientRect.Width(),ClientRect.Height());

		//清理资源
		dc.SelectObject(pOldFont);
		DrawFont.DeleteObject();
	}

	return;
}

//位置变化
void CGameFrameView::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);
	
	//效验状态
	if ((cx==0)||(cy==0)) return;
	if (GetSystemMetrics(SM_CXSCREEN)<cx) return;

	//更改缓冲图
	if ((m_bDoubleBuf==true)&&(nType!=SIZE_MINIMIZED))
	{
		if ((cx>m_ImageSize.cx)||(cy>m_ImageSize.cy))
		{
			m_bReDraw=true;
			m_ImageBuffer.Destroy();
			m_ImageSize.cx=__max(cx,m_ImageSize.cx);
			m_ImageSize.cy=__max(cy,m_ImageSize.cy);
			m_ImageBuffer.Create(m_ImageSize.cx,m_ImageSize.cy,m_uColorCount);
		}
	}

	//更新视图
	RectifyGameView(cx,cy);

	//设置绘画数据
	if (m_pThreadDraw!=NULL)
	{
		m_pThreadDraw->SetClientRange(cx,cy);
		if (nType==SIZE_MINIMIZED) m_pThreadDraw->SuspendDrawThread();
		else m_pThreadDraw->ResumeDrawThread();
	}
	else UpdateGameView(NULL);

	return;
}

//鼠标消息
void CGameFrameView::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();
	__super::OnLButtonDown(nFlags, point);
}

//销毁消息
void CGameFrameView::OnDestroy()
{
	__super::OnDestroy();

	//清理线程
	if (m_pThreadDraw!=NULL) m_pThreadDraw->EndThread();

	return;
}

//时间消息
void CGameFrameView::OnTimer(UINT nIDEvent)
{
	if ( nIDEvent == IDI_FLOWER_START )
	{
		//移动判断
		for ( int nIndex = 0; nIndex < MAX_FLOWER_COUNT; ++nIndex )
		{
			//合法判断
			if(m_FlowerInfo[nIndex].wMoveCount==0)continue;

			//移动步数
			if(m_FlowerInfo[nIndex].wMoveIndex<m_FlowerInfo[nIndex].wMoveCount)
			{
				m_FlowerInfo[nIndex].wMoveIndex++;
				WORD wMoveIndex = m_FlowerInfo[nIndex].wMoveIndex;
				WORD wMoveCount = m_FlowerInfo[nIndex].wMoveCount;
				m_FlowerInfo[nIndex].ptCurPos.x =m_FlowerInfo[nIndex].ptFrom.x + (m_FlowerInfo[nIndex].ptDest.x-m_FlowerInfo[nIndex].ptFrom.x)*wMoveIndex/wMoveCount;
				m_FlowerInfo[nIndex].ptCurPos.y =m_FlowerInfo[nIndex].ptFrom.y + (m_FlowerInfo[nIndex].ptDest.y-m_FlowerInfo[nIndex].ptFrom.y)*wMoveIndex/wMoveCount;
			}
			//结束判断
			else
			{
				//显示动画
				m_FlowerEffectControl[m_FlowerInfo[nIndex].wTargetChairID].StartFlowerEffect(m_FlowerInfo[nIndex].uFlowerEffectID);
				m_FlowerEffectControl[m_FlowerInfo[nIndex].wTargetChairID].BringWindowToTop();

				//减少次数
				m_nFlowerCount--;

				//清除变量
				ZeroMemory(&m_FlowerInfo[nIndex], sizeof(m_FlowerInfo[nIndex]));
			}
		}

		//结束判断
		if ( m_nFlowerCount == 0 ) KillTimer(nIDEvent);

		//更新界面
		UpdateGameView(NULL);

		return;
	}
	__super::OnTimer(nIDEvent);
}

//////////////////////////////////////////////////////////////////////////
