#include "StdAfx.h"
#include "GameClient.h"
#include "ChessBorad.h"

//////////////////////////////////////////////////////////////////////////

//位置定义
#define EXCURSION_X					16									//偏移坐标
#define EXCURSION_Y					15									//偏移坐标
#define SPACE_BORAD_X				57									//间隙大小
#define SPACE_BORAD_Y				57									//间隙大小

//位置定义
#define EXCURSION_X_S				10									//偏移坐标
#define EXCURSION_Y_S				8									//偏移坐标
#define SPACE_BORAD_X_S				45									//间隙大小
#define SPACE_BORAD_Y_S				45									//间隙大小

//////////////////////////////////////////////////////////////////////////

//构造函数
CChessBorad::CChessBorad()
{
	//配置变量
	m_bPositively=false;
	m_bManualView=false;
	m_bWaitGameRule=false;
	m_cbUserColor=CHESS_BLACK;

	//辅助变量
	m_cbXPosStart=0xFF;
	m_cbYPosStart=0xFF;
	m_cbXPosSelect=0xFF;
	m_cbYPosSelect=0xFF;
	m_cbXPosTerminal=0xFF;
	m_cbYPosTerminal=0xFF;

	//设置变量
	m_SizeBorad.SetSize(0,0);
	m_SizeChess.SetSize(0,0);
	m_BenchmarkPos.SetPoint(0,0);

	//棋盘变量
	ZeroMemory(m_ChessItemPtr,sizeof(m_ChessItemPtr));

	//加载资源
	m_ImageWaitGameRule.SetLoadInfo(IDB_WAIT_GAME_RULE,AfxGetInstanceHandle());

	return;
}

//析构函数
CChessBorad::~CChessBorad()
{
}

//绘画界面
VOID CChessBorad::DrawChessBorad(CDC * pDC)
{
	//加载资源
	CImageHandle HandleChessFrame(&m_ImageChessFrame);
	CImageHandle HandleChessBorad(&m_ImageChessBorad);

	//绘画棋盘
	m_ImageChessBorad.BitBlt(pDC->m_hDC,m_BenchmarkPos.x,m_BenchmarkPos.y);

	//棋谱标志
	if (m_bManualView==true)
	{
		//加载资源
		CPngImage ImageManualFlag;
		ImageManualFlag.LoadImage(AfxGetInstanceHandle(),TEXT("MANUAL_FLAG"));

		//绘画标志
		INT nImageWidth=ImageManualFlag.GetWidth();
		INT nImageHeight=ImageManualFlag.GetHeight();
		ImageManualFlag.DrawImage(pDC,m_BenchmarkPos.x+(m_SizeBorad.cx-nImageWidth)/2,m_BenchmarkPos.y+m_SizeBorad.cy/2+108);
		ImageManualFlag.DrawImage(pDC,m_BenchmarkPos.x+(m_SizeBorad.cx-nImageWidth)/2,m_BenchmarkPos.y+m_SizeBorad.cy/2-nImageHeight-120);
	}

	//绘画棋子
	for (BYTE cbXPos=0;cbXPos<COUNT_X;cbXPos++)
	{
		for (BYTE cbYPos=0;cbYPos<COUNT_Y;cbYPos++)
		{
			//计算位置
			INT nXBoradPos=GetChessXPos(cbXPos);
			INT nYBoradPos=GetChessYPos(cbYPos);
			tagChessItem * pChessItem=m_ChessItemPtr[cbXPos][cbYPos];

			//绘画棋子
			if (pChessItem!=NULL)
			{
				INT nXPosImage=(pChessItem->cbChess-1)*m_SizeChess.cx;
				INT nYPosImage=(pChessItem->cbColor-1)*m_SizeChess.cy;
				m_ImageChessItem.DrawImage(pDC,nXBoradPos+(m_XBoradSpace-m_SizeChess.cx)/2,
					nYBoradPos+(m_YBoradSpace-m_SizeChess.cy)/2,m_SizeChess.cx,m_SizeChess.cy,nXPosImage,nYPosImage);
			}

			//绘画框架
			bool bFrame=false;
			if ((cbXPos==m_cbXPosStart)&&(cbYPos==m_cbYPosStart)) bFrame=true;
			if ((cbXPos==m_cbXPosSelect)&&(m_cbYPosSelect==cbYPos)) bFrame=true;
			if ((cbXPos==m_cbXPosTerminal)&&(cbYPos==m_cbYPosTerminal)) bFrame=true;
			if (bFrame==true) m_ImageChessFrame.AlphaDrawImage(pDC,nXBoradPos,nYBoradPos,RGB(255,0,255));
		}
	}

	//等待标志
	if (m_bWaitGameRule==true)
	{
		CImageHandle HandleWaitGameRule(&m_ImageWaitGameRule);
		INT nXPos=(m_SizeBorad.cx-m_ImageWaitGameRule.GetWidth())/2;
		INT nYPos=(m_SizeBorad.cy-m_ImageWaitGameRule.GetHeight())/2;
		m_ImageWaitGameRule.AlphaDrawImage(pDC,m_BenchmarkPos.x+nXPos,m_BenchmarkPos.y+nYPos,RGB(255,0,255));
	}

	return;
}

//设置模式
VOID CChessBorad::SetChessBoradMode(bool bSmallMode)
{
	//设置资源
	if (bSmallMode==false)
	{
		//设置变量
		m_XExcursion=EXCURSION_X;
		m_YExcursion=EXCURSION_Y;
		m_XBoradSpace=SPACE_BORAD_X;
		m_YBoradSpace=SPACE_BORAD_Y;

		//加载资源
		m_ImageChessItem.DestroyImage();
		m_ImageChessItem.LoadImage(AfxGetInstanceHandle(),TEXT("CHESS_ITEM"));
		m_ImageChessFrame.SetLoadInfo(IDB_CHESS_FRAME,AfxGetInstanceHandle());
		m_ImageChessBorad.SetLoadInfo(IDB_CHESS_BORAD,AfxGetInstanceHandle());

		//获取大小
		CImageHandle HandleChessBorad(&m_ImageChessBorad);
		m_SizeBorad.SetSize(m_ImageChessBorad.GetWidth(),m_ImageChessBorad.GetHeight());
		m_SizeChess.SetSize(m_ImageChessItem.GetWidth()/7,m_ImageChessItem.GetHeight()/2);
	}
	else
	{
		//设置变量
		m_XExcursion=EXCURSION_X_S;
		m_YExcursion=EXCURSION_Y_S;
		m_XBoradSpace=SPACE_BORAD_X_S;
		m_YBoradSpace=SPACE_BORAD_Y_S;

		//加载资源
		m_ImageChessItem.DestroyImage();
		m_ImageChessItem.LoadImage(AfxGetInstanceHandle(),TEXT("CHESS_ITEM_S"));
		m_ImageChessFrame.SetLoadInfo(IDB_CHESS_FRAME_S,AfxGetInstanceHandle());
		m_ImageChessBorad.SetLoadInfo(IDB_CHESS_BORAD_S,AfxGetInstanceHandle());

		//获取大小
		CImageHandle HandleChessBorad(&m_ImageChessBorad);
		m_SizeBorad.SetSize(m_ImageChessBorad.GetWidth(),m_ImageChessBorad.GetHeight());
		m_SizeChess.SetSize(m_ImageChessItem.GetWidth()/7,m_ImageChessItem.GetHeight()/2);
	}

	return;
}

//清除棋子
VOID CChessBorad::CleanChessItem()
{
	//辅助变量
	m_cbXPosStart=0xFF;
	m_cbYPosStart=0xFF;
	m_cbXPosSelect=0xFF;
	m_cbYPosSelect=0xFF;
	m_cbXPosTerminal=0xFF;
	m_cbYPosTerminal=0xFF;

	//设置变量
	m_cbUserColor=CHESS_BLACK;
	ZeroMemory(m_ChessItemPtr,sizeof(m_ChessItemPtr));

	return;
}

//设置棋盘
VOID CChessBorad::SetChessBorad(tagChessItem * ChessItemPtr[COUNT_X][COUNT_Y])
{
	//设置棋盘
	CopyMemory(m_ChessItemPtr,ChessItemPtr,sizeof(m_ChessItemPtr));

	return;
}

//设置棋子
VOID CChessBorad::SelectChessItem(BYTE cbXPos, BYTE cbYPos)
{
	//设置变量
	m_cbXPosSelect=cbXPos;
	m_cbYPosSelect=cbYPos;

	return;
}

//走棋路线
VOID CChessBorad::SetMoveChessTrace(BYTE cbXPosStart, BYTE cbYPosStart, BYTE cbXPosTerminal, BYTE cbYPosTerminal)
{
	//设置变量
	m_cbXPosStart=cbXPosStart;
	m_cbYPosStart=cbYPosStart;
	m_cbXPosTerminal=cbXPosTerminal;
	m_cbYPosTerminal=cbYPosTerminal;

	return;
}

//鼠标点击
VOID CChessBorad::OnEventLMouseDown(CPoint Point)
{
	//状态判断
	if ((m_bPositively==false)||(m_bManualView==true)) return;

	//获取位置
	INT nXCursor=Point.x-m_BenchmarkPos.x;
	INT nYCursor=Point.y-m_BenchmarkPos.y;

	//转换坐标
	BYTE cbXPos=INVALID_POS,cbYPos=INVALID_POS;
	SwitchMousePos(nXCursor,nYCursor,cbXPos,cbYPos);

	//发送消息
	if ((cbXPos!=INVALID_POS)&&(cbYPos!=INVALID_POS)) 
	{
		//坐标转换
		if (m_cbUserColor==CHESS_WHITE)
		{
			cbXPos=COUNT_X-cbXPos-1;
			cbYPos=COUNT_Y-cbYPos-1;
		}

		//查询接口
		//CGlobalUnits * pGlobalUnits=CGlobalUnits::GetGlobalUnits();
		//IGameFrameEngine * pIGameFrameEngine=(IGameFrameEngine *)pGlobalUnits->QueryGlobalModule(MODULE_GAME_FRAME_ENGINE,IID_IGameFrameEngine,VER_IGameFrameEngine);

		//控制界面
		//ASSERT(pIGameFrameEngine!=NULL);
		/*if (pIGameFrameEngine!=NULL)*/ AfxGetMainWnd()->SendMessage(WM_HIT_CHESS_BORAD,cbXPos,cbYPos);
	}

	return;
}

//获取坐标
INT CChessBorad::GetChessXPos(BYTE cbXPos) 
{ 
	if (m_cbUserColor==CHESS_WHITE) cbXPos=COUNT_X-cbXPos-1;
	return m_BenchmarkPos.x+m_XExcursion+cbXPos*m_XBoradSpace; 
}

//获取坐标
INT CChessBorad::GetChessYPos(BYTE cbYPos)
{ 
	if (m_cbUserColor==CHESS_WHITE) cbYPos=COUNT_Y-cbYPos-1;
	return m_BenchmarkPos.y+m_XExcursion+cbYPos*m_YBoradSpace; 
}

//转换坐标
VOID CChessBorad::SwitchMousePos(INT nXCursor, INT nYCursor, BYTE & cbXIndex, BYTE & cbYIndex)
{
	//位置检测
	if ((nXCursor>m_XExcursion)&&(nXCursor<(m_XExcursion+COUNT_X*m_XBoradSpace))
		&&(nYCursor>m_XExcursion)&&(nYCursor<(m_XExcursion+COUNT_Y*m_YBoradSpace)))
	{
		//计算位置
		cbXIndex=(BYTE)((nXCursor-m_XExcursion)/m_XBoradSpace);
		cbYIndex=(BYTE)((nYCursor-m_XExcursion)/m_YBoradSpace);
	}
	else
	{
		//设置结果
		cbXIndex=0xFF;
		cbYIndex=0xFF;
	}

	return;
}

//////////////////////////////////////////////////////////////////////////
