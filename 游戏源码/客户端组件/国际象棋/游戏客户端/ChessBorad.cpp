#include "StdAfx.h"
#include "Resource.h"
#include "ChessBorad.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

//位置定义
#define EXCURSION_X					18									//偏移坐标
#define EXCURSION_Y					15									//偏移坐标
#define CHESS_WIDTH					37									//棋子宽度
#define CHESS_HEIGHT				46									//棋子高度
#define SPACE_BORAD_X				56									//间隙大小
#define SPACE_BORAD_Y				56									//间隙大小

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CChessBorad, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CChessBorad::CChessBorad()
{
	//配置变量
	m_bPositively=false;
	m_bShowChessName=false;
	m_cbUserColor=BLACK_CHESS;

	//辅助变量
	m_cbXPosStart=255;
	m_cbYPosStart=255;
	m_cbXPosSelect=255;
	m_cbYPosSelect=255;
	m_cbXPosTerminal=255;
	m_cbYPosTerminal=255;

	//状态变量
	memset(m_ChessBorad,0,sizeof(m_ChessBorad));

	//加载资源
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_ImageChess.SetLoadInfo(IDB_CHESS,hInstance);
	m_ImageFrame.SetLoadInfo(IDB_CHESS_FRAME,hInstance);
	m_ImageBorad.SetLoadInfo(IDB_CHESS_BOARD,hInstance);

	//获取大小
	CImageHandle BoradHandle(&m_ImageBorad);
	m_BoradSize.cx=m_ImageBorad.GetWidth();
	m_BoradSize.cy=m_ImageBorad.GetHeight();

	return;
}

//析构函数
CChessBorad::~CChessBorad()
{
}

//清除棋子
void CChessBorad::CleanChess()
{
	//辅助变量
	m_cbXPosStart=255;
	m_cbYPosStart=255;
	m_cbXPosSelect=255;
	m_cbYPosSelect=255;
	m_cbXPosTerminal=255;
	m_cbYPosTerminal=255;

	//设置变量
	m_cbUserColor=BLACK_CHESS;
	memset(m_ChessBorad,0,sizeof(m_ChessBorad));

	//更新界面
	Invalidate(FALSE);

	return;
}

//设置棋子
void CChessBorad::SetSelectChess(BYTE cbXPos, BYTE cbYPos)
{
	//设置变量
	m_cbXPosSelect=cbXPos;
	m_cbYPosSelect=cbYPos;

	//更新界面
	Invalidate(FALSE);

	return;
}

//设置棋盘
void CChessBorad::SetChessBorad(tagChessItem * ChessBorad[8][8])
{
	//辅助变量
	m_cbXPosSelect=255;
	m_cbYPosSelect=255;

	//设置棋盘
	CopyMemory(m_ChessBorad,ChessBorad,sizeof(m_ChessBorad));

	//更新界面
	Invalidate(FALSE);

	return;
}

//设置颜色
void CChessBorad::SetChessColor(BYTE cbUserColor)
{
	if (m_cbUserColor!=cbUserColor)
	{
		m_cbUserColor=cbUserColor;
		if (GetSafeHwnd()!=NULL) Invalidate(FALSE);
	}

	return;
}

//设置响应
void CChessBorad::SetPositively(bool bPositively)
{
	m_bPositively=bPositively;
	return;
}

//显示名字
void CChessBorad::ShowChessName(bool bShowChessName)
{
	if (m_bShowChessName!=bShowChessName)
	{
		m_bShowChessName=bShowChessName;
		Invalidate(FALSE);
	}

	return;
}

//设置棋框
void CChessBorad::SetChessFrame(BYTE cbXPosStart, BYTE cbYPosStart, BYTE cbXPosTerminal, BYTE cbYPosTerminal)
{
	//设置变量
	m_cbXPosStart=cbXPosStart;
	m_cbYPosStart=cbYPosStart;
	m_cbXPosTerminal=cbXPosTerminal;
	m_cbYPosTerminal=cbYPosTerminal;

	//更新界面
	Invalidate(FALSE);

	return;
}

//获取坐标
int CChessBorad::GetChessXPos(BYTE cbXPos) 
{ 
	if (m_cbUserColor==WHITE_CHESS) cbXPos=7-cbXPos;
	return EXCURSION_X+cbXPos*SPACE_BORAD_X; 
}

//获取坐标
int CChessBorad::GetChessYPos(BYTE cbYPos)
{ 
	if (m_cbUserColor==WHITE_CHESS) cbYPos=7-cbYPos;
	return EXCURSION_Y+cbYPos*SPACE_BORAD_Y; 
}

//更新棋子
void CChessBorad::UpdateBoradPos(BYTE cbXPos, BYTE cbYPos)
{
	//效验参数
	ASSERT(cbXPos!=255);
	ASSERT(cbYPos!=255);

	//转换坐标
	if (m_cbUserColor==WHITE_CHESS)
	{
		cbXPos=7-cbXPos;
		cbYPos=7-cbYPos;
	}

	//构造位置
	CRect rcChess;
	rcChess.top=GetChessYPos(cbYPos)-1;
	rcChess.left=GetChessXPos(cbXPos)-1;
	rcChess.right=rcChess.left+SPACE_BORAD_X+2;
	rcChess.bottom=rcChess.top+SPACE_BORAD_Y+2;

	//刷新界面
	InvalidateRect(&rcChess);

	return;
}

//按键测试
void CChessBorad::SwitchMousePos(const CPoint & MousePoint, BYTE & cbXPos, BYTE & cbYPos)
{
	if ((MousePoint.x>EXCURSION_X)&&(MousePoint.x<(EXCURSION_X+8*SPACE_BORAD_X))
		&&(MousePoint.y>EXCURSION_Y)&&(MousePoint.y<(EXCURSION_Y+8*SPACE_BORAD_Y)))
	{
		cbXPos=(BYTE)((MousePoint.x-EXCURSION_X)/SPACE_BORAD_X);
		cbYPos=(BYTE)((MousePoint.y-EXCURSION_Y)/SPACE_BORAD_Y);
	}
	else
	{
		cbXPos=255;
		cbYPos=255;
	}

	return;
}

//重画函数
void CChessBorad::OnPaint()
{
	CPaintDC dc(this);

	//获取大小
	CRect rcClient;
	GetClientRect(&rcClient);

	//创建缓冲图
	CDC BufferDC;
	CBitmap BufferBmp;
	BufferDC.CreateCompatibleDC(&dc);
	BufferBmp.CreateCompatibleBitmap(&dc,rcClient.Width(),rcClient.Height());
	BufferDC.SelectObject(&BufferBmp);

	//设置 DC
	BufferDC.SetBkMode(TRANSPARENT);
	BufferDC.SetTextColor(RGB(200,0,0));
	BufferDC.SelectObject(CSkinResourceManager::GetDefaultFont());

	//绘画棋盘
	CImageHandle BoradHandle(&m_ImageBorad);
	m_ImageBorad.BitBlt(BufferDC,0,0);

	//棋子名字
	LPCTSTR pszChessName[]={TEXT("王"),TEXT("后"),TEXT("车"),TEXT("象"),TEXT("马"),TEXT("士")};

	//绘画棋子
	CImageHandle FrameHandle(&m_ImageFrame);
	CImageHandle ChessHandle(&m_ImageChess);
	int nXPosImage=0,nYPosImage=0,nXBoradPos=0,nYBoradPos=0;
	for (BYTE cbXPos=0;cbXPos<8;cbXPos++)
	{
		for (BYTE cbYPos=0;cbYPos<8;cbYPos++)
		{
			//计算位置
			nXBoradPos=GetChessXPos(cbXPos);
			nYBoradPos=GetChessYPos(cbYPos);
			tagChessItem * pChessItem=m_ChessBorad[cbXPos][cbYPos];

			//绘画棋子
			if (pChessItem!=NULL)
			{
				nXPosImage=(pChessItem->cbChess-1)*CHESS_WIDTH;
				nYPosImage=(pChessItem->cbColor-1)*CHESS_HEIGHT;
				m_ImageChess.AlphaDrawImage(&BufferDC,nXBoradPos+(SPACE_BORAD_X-CHESS_WIDTH)/2,
					nYBoradPos+(SPACE_BORAD_Y-CHESS_HEIGHT)/2,CHESS_WIDTH,CHESS_HEIGHT,
					nXPosImage,nYPosImage,RGB(255,0,255));
			}

			//绘画框架
			bool bFrame=((cbXPos==m_cbXPosStart)&&(cbYPos==m_cbYPosStart));
			if (bFrame==false) bFrame=((cbXPos==m_cbXPosSelect)&&(m_cbYPosSelect==cbYPos));
			if (bFrame==false) bFrame=((cbXPos==m_cbXPosTerminal)&&(cbYPos==m_cbYPosTerminal));
			if (bFrame==true) m_ImageFrame.AlphaDrawImage(&BufferDC,nXBoradPos,nYBoradPos,RGB(255,0,255));

			//绘画棋字
			if ((m_bShowChessName==true)&&(pChessItem!=NULL))
			{
				LPCTSTR pszTitle=pszChessName[pChessItem->cbChess-1];
				BufferDC.SetTextColor(((cbXPos+cbYPos)%2==1)?RGB(255,255,255):RGB(200,0,0));
				BufferDC.TextOut(nXBoradPos+5,nYBoradPos+5,pszTitle,lstrlen(pszTitle));
			}
		}
	}

	//绘画界面
	dc.BitBlt(0,0,rcClient.Width(),rcClient.Height(),&BufferDC,0,0,SRCCOPY);

	//清理资源
	BufferBmp.DeleteObject();
	BufferDC.DeleteDC();

	return;
}

//建立消息
int CChessBorad::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//设置背景
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);

	return 0;
}

//鼠标消息
void CChessBorad::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags, Point);

	//状态判断
	if (m_bPositively==false) return;

	//获取光标
	CPoint MousePoint;
	GetCursorPos(&MousePoint);
	ScreenToClient(&MousePoint);

	//转换坐标
	BYTE cbXPos=255,cbYPos=255;
	SwitchMousePos(MousePoint,cbXPos,cbYPos);

	//发送消息
	if ((cbXPos!=255)&&(cbYPos!=255)) 
	{
		if (m_cbUserColor==WHITE_CHESS) 
		{
			cbXPos=7-cbXPos;
			cbYPos=7-cbYPos;
		}
		GetParent()->PostMessage(WM_HIT_CHESS_BORAD,cbXPos,cbYPos);
	}

	return;
}

//////////////////////////////////////////////////////////////////////////
