#include "StdAfx.h"
#include "Resource.h"
#include "ChessBorad.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

#define CHESS_WIDTH					30									//棋子宽度
#define CHESS_HEIGHT				30									//棋子高度

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CChessBorad, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CChessBorad::CChessBorad()
{
	//配置变量
	m_bShowChessID=false;
	m_bDoubleMouse=false;
	m_cbSelectMode=SM_NO_SELECT;

	//辅助变量
	m_cbXCursor=255;
	m_cbYCursor=255;

	//状态变量
	m_cbXPosFocus=255;
	m_cbYPosFocus=255;
	memset(m_ChessInfo,0,sizeof(m_ChessInfo));

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
	//设置变量
	m_cbXPosFocus=255;
	m_cbYPosFocus=255;
	memset(m_ChessInfo,0,sizeof(m_ChessInfo));

	//更新界面
	Invalidate(FALSE);

	return;
}

//设置棋子
void CChessBorad::SetChess(BYTE cbXPos, BYTE cbYPos, BYTE cbColor, BYTE cbIdentifier)
{
	//效验参数
	ASSERT(cbXPos<15);
	ASSERT(cbYPos<15);

	//设置变量
	m_ChessInfo[cbXPos][cbYPos].cbColor=cbColor;
	m_ChessInfo[cbXPos][cbYPos].cbIdentifier=cbIdentifier;

	//更新棋盘
	UpdateBoradPos(cbXPos,cbYPos);

	return;
}

//设置棋盘
void CChessBorad::SetChessBorad(const BYTE cbChessBorad[15][15], const BYTE cbIdentifier[15][15])
{
	//设置变量
	m_cbXPosFocus=255;
	m_cbYPosFocus=255;
	memset(m_ChessInfo,0,sizeof(m_ChessInfo));

	//设置棋盘
	for (BYTE cbXPos=0;cbXPos<15;cbXPos++)
	{
		for (BYTE cbYPos=0;cbYPos<15;cbYPos++)
		{
			if (cbChessBorad[cbXPos][cbYPos]!=NO_CHESS)
			{
				m_ChessInfo[cbXPos][cbYPos].cbColor=cbChessBorad[cbXPos][cbYPos];
				m_ChessInfo[cbXPos][cbYPos].cbIdentifier=cbIdentifier[cbXPos][cbYPos];
			}
		}
	}

	//更新界面
	Invalidate(FALSE);

	return;
}

//显示手数
void CChessBorad::ShowChessID(bool bShowChessID)
{
	if (m_bShowChessID!=bShowChessID)
	{
		m_bShowChessID=bShowChessID;
		Invalidate(FALSE);
	}

	return;
}

//双击设置
void CChessBorad::SetDoubleMouse(bool bDoubleMouse)
{
	m_bDoubleMouse=bDoubleMouse;
	return;
}

//设置响应
void CChessBorad::SetSelectMode(BYTE cbSelectMode)
{
	//状态效验
	if (m_cbSelectMode==cbSelectMode) return;

	//响应处理
	m_cbSelectMode=cbSelectMode;
	if (m_cbSelectMode==SM_NO_SELECT)
	{
		//设置变量
		BYTE cbXCursor=m_cbXCursor;
		BYTE cbYCursor=m_cbYCursor;
		m_cbXCursor=255,m_cbYCursor=255;

		//设置视图
		SetCursor(LoadCursor(NULL,IDC_ARROW));
		if ((cbXCursor!=255)&&(cbYCursor!=255)) UpdateBoradPos(cbXCursor,cbYCursor);
	}

	return;
}

//设置焦点
void CChessBorad::SetChessFocus(BYTE cbXPos, BYTE cbYPos)
{
	//保存变量
	BYTE cbXFocus=m_cbXPosFocus;
	BYTE cbYFocus=m_cbYPosFocus;

	//设置变量
	m_cbXPosFocus=cbXPos;
	m_cbYPosFocus=cbYPos;

	//更新视图
	if ((cbXFocus!=255)&&(cbYFocus!=255)) UpdateBoradPos(cbXFocus,cbYFocus);
	if ((m_cbXPosFocus!=255)&&(m_cbYPosFocus!=255)) UpdateBoradPos(m_cbXPosFocus,m_cbYPosFocus);

	return;
}

//设置棋盘
void CChessBorad::SetChessBorad(const WORD wBlackChess[], BYTE cbBlackCount, const WORD wWhiteChess[], BYTE cbWhiteCount)
{
	//清理棋盘
	m_cbXPosFocus=255;
	m_cbYPosFocus=255;
	memset(m_ChessInfo,0,sizeof(m_ChessInfo));

	//设置棋子
	BYTE cbXPos=255,cbYPos=255;
	for (BYTE i=0;i<cbBlackCount;i++)
	{
		cbXPos=LOBYTE(wBlackChess[i]);
		cbYPos=HIBYTE(wBlackChess[i]);
		m_ChessInfo[cbXPos][cbYPos].cbFlag=FALSE;
		m_ChessInfo[cbXPos][cbYPos].cbIdentifier=i*2+1;
		m_ChessInfo[cbXPos][cbYPos].cbColor=BLACK_CHESS;
	}
	for (BYTE i=0;i<cbWhiteCount;i++)
	{
		cbXPos=LOBYTE(wWhiteChess[i]);
		cbYPos=HIBYTE(wWhiteChess[i]);
		m_ChessInfo[cbXPos][cbYPos].cbFlag=FALSE;
		m_ChessInfo[cbXPos][cbYPos].cbIdentifier=i*2+2;
		m_ChessInfo[cbXPos][cbYPos].cbColor=WHITE_CHESS;
	}

	//更新界面
	Invalidate(FALSE);

	return;
}

//显示胜利
void CChessBorad::ShowWinChessLine(BYTE cbXFirstPos, BYTE cbYFirstPos, BYTE cbXEndPos, BYTE cbYEndPos)
{
	//效验参数
	ASSERT((cbXEndPos<15)&&(cbYEndPos<15));
	ASSERT((cbXFirstPos<15)&&(cbYFirstPos<15));

	//设置标志
	BYTE cbXPos=cbXFirstPos;
	BYTE cbYPos=cbYFirstPos;
	do
	{
		//设置变量
		ASSERT(m_ChessInfo[cbXPos][cbYPos].cbFlag!=NO_CHESS);
		m_ChessInfo[cbXPos][cbYPos].cbFlag=TRUE;

		//退出判断
		if ((cbXPos==cbXEndPos)&&(cbYPos==cbYEndPos)) break;

		//调整坐标
		if (cbXEndPos>cbXPos) cbXPos++;
		else if (cbXEndPos<cbXPos) cbXPos--;

		//调整坐标
		if (cbYEndPos>cbYPos) cbYPos++;
		else if (cbYEndPos<cbYPos) cbYPos--;

	} while (true);

	//更新界面
	Invalidate(NULL);
		
	return;
}

//更新棋子
void CChessBorad::UpdateBoradPos(BYTE cbXPos, BYTE cbYPos)
{
	//效验参数
	ASSERT(cbXPos!=255);
	ASSERT(cbYPos!=255);

	//构造位置
	CRect rcChess;
	rcChess.left=GetChessXPos(cbXPos)-1;
	rcChess.top=GetChessYPos(cbYPos)-1;
	rcChess.right=rcChess.left+SPACE_BORAD_X+2;
	rcChess.bottom=rcChess.top+SPACE_BORAD_Y+2;

	//刷新界面
	InvalidateRect(&rcChess);

	return;
}

//选择判断
bool CChessBorad::EfficacySelect(BYTE cbXPos, BYTE cbYPos)
{
	//效验参数
	ASSERT(cbXPos!=255);
	ASSERT(cbYPos!=255);

	//选择判断
	BYTE cbColor=m_ChessInfo[cbXPos][cbYPos].cbColor;
	if (m_cbSelectMode==SM_NO_SELECT) return false;
	if ((m_cbSelectMode&SM_NULL_CHESS)&&(cbColor==NO_CHESS)) return true;
	if ((m_cbSelectMode&SM_BLACK_CHESS)&&(cbColor==BLACK_CHESS)) return true;
	if ((m_cbSelectMode&SM_WHITE_CHESS)&&(cbColor==WHITE_CHESS)) return true;

	return false;
}

//按键测试
void CChessBorad::SwitchMousePos(const CPoint & MousePoint, BYTE & cbXPos, BYTE & cbYPos)
{
	if ((MousePoint.x>EXCURSION_X)&&(MousePoint.x<(EXCURSION_X+15*SPACE_BORAD_X))
		&&(MousePoint.y>EXCURSION_Y)&&(MousePoint.y<(EXCURSION_Y+15*SPACE_BORAD_Y)))
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
	BufferDC.SelectObject(CSkinResourceManager::GetDefaultFont());

	//绘画棋盘
	CImageHandle BoradHandle(&m_ImageBorad);
	m_ImageBorad.BitBlt(BufferDC,0,0);

	//绘画棋子
	CRect rcChessID;
	TCHAR szBuffer[16];
	CImageHandle ChessHandle(&m_ImageChess);
	int nPosImage=0,nXBoradPos=0,nYBoradPos=0;
	for (BYTE cbXPos=0;cbXPos<15;cbXPos++)
	{
		for (BYTE cbYPos=0;cbYPos<15;cbYPos++)
		{
			if (m_ChessInfo[cbXPos][cbYPos].cbColor!=NO_CHESS)
			{
				//绘画棋子
				nXBoradPos=GetChessXPos(cbXPos);
				nYBoradPos=GetChessYPos(cbYPos);
				nPosImage=(m_ChessInfo[cbXPos][cbYPos].cbColor-1)*CHESS_WIDTH;
				if (m_ChessInfo[cbXPos][cbYPos].cbFlag==TRUE) nPosImage+=CHESS_WIDTH*2;
				else if ((cbXPos==m_cbXPosFocus)&&(cbYPos==m_cbYPosFocus)) nPosImage+=CHESS_WIDTH*2;
				m_ImageChess.AlphaDrawImage(&BufferDC,nXBoradPos,nYBoradPos,CHESS_WIDTH,CHESS_HEIGHT,nPosImage,0,RGB(255,0,255));

				//绘画手数
				if (m_bShowChessID==true)
				{
					//设置位置
					rcChessID.left=nXBoradPos;
					rcChessID.top=nYBoradPos;
					rcChessID.right=rcChessID.left+CHESS_WIDTH;
					rcChessID.bottom=rcChessID.top+CHESS_HEIGHT;

					//输出信息
					_snprintf(szBuffer,sizeof(szBuffer),TEXT("%d"),m_ChessInfo[cbXPos][cbYPos].cbIdentifier);
					BufferDC.SetTextColor((m_ChessInfo[cbXPos][cbYPos].cbColor==WHITE_CHESS)?RGB(0,0,0):RGB(255,255,255));
					BufferDC.DrawText(szBuffer,lstrlen(szBuffer),&rcChessID,DT_SINGLELINE|DT_VCENTER|DT_CENTER);
				}
			}
			
			//绘画光标
			if ((cbXPos==m_cbXCursor)&&(cbYPos==m_cbYCursor))
			{
				nXBoradPos=GetChessXPos(m_cbXCursor)-1;
				nYBoradPos=GetChessYPos(m_cbYCursor)-1;
				CImageHandle FrameHandle(&m_ImageFrame);
				m_ImageFrame.AlphaDrawImage(&BufferDC,nXBoradPos,nYBoradPos,RGB(255,0,255));
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
	if (m_bDoubleMouse==true) return;
	if (m_cbSelectMode==SM_NO_SELECT) return;
	if ((m_cbXCursor==255)||(m_cbYCursor==255)) return;
	if (EfficacySelect(m_cbXCursor,m_cbYCursor)==false) return;

	//发送消息
	GetParent()->PostMessage(WM_HIT_CHESS_BORAD,m_cbXCursor,m_cbYCursor);

	return;
}

//鼠标双击
void CChessBorad::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	__super::OnLButtonDblClk(nFlags, point);

	//状态判断
	if (m_bDoubleMouse==false) return;
	if (m_cbSelectMode==SM_NO_SELECT) return;
	if ((m_cbXCursor==255)||(m_cbYCursor==255)) return;
	if (EfficacySelect(m_cbXCursor,m_cbYCursor)==false) return;

	//发送消息
	GetParent()->PostMessage(WM_HIT_CHESS_BORAD,m_cbXCursor,m_cbYCursor);

	return;
}

//光标消息
BOOL CChessBorad::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
{
	if (m_cbSelectMode!=SM_NO_SELECT)
	{
		//获取光标
		CPoint MousePoint;
		GetCursorPos(&MousePoint);
		ScreenToClient(&MousePoint);

		//转换坐标
		BYTE cbXPos=255,cbYPos=255;
		SwitchMousePos(MousePoint,cbXPos,cbYPos);

		//绘画光标
		if ((m_cbXCursor!=cbXPos)||(m_cbYCursor!=cbYPos))
		{
			//设置光标
			if ((m_cbXCursor!=255)&&(m_cbYCursor!=255)) UpdateBoradPos(m_cbXCursor,m_cbYCursor);
			if ((cbXPos!=255)&&(cbYPos!=255)&&(EfficacySelect(cbXPos,cbYPos)==true)) 
			{
				m_cbXCursor=cbXPos;
				m_cbYCursor=cbYPos;
				UpdateBoradPos(m_cbXCursor,m_cbYCursor);
			}
			else
			{
				m_cbXCursor=255;
				m_cbYCursor=255;
			}
		}

		//位置判断
		if ((m_cbXCursor!=255)&&(m_cbYCursor!=255))
		{
			SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_ENABLE)));
			return TRUE;
		}
		else 
		{
			SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_DISABLE)));
			return TRUE;
		}
	}
	
	return __super::OnSetCursor(pWnd,nHitTest,uMessage);
}

//////////////////////////////////////////////////////////////////////////
