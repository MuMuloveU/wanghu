#include "StdAfx.h"
#include "GameThreadDraw.h"
#include "GameClientView.h"
#include "math.h"
#include ".\gamethreaddraw.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
DrawInfoStruct			g_DrawInfo;
//构造函数
CGameThreadDraw::CGameThreadDraw(CWnd * pGameView) : CThreadDraw(pGameView)
{
}
CGameThreadDraw::~CGameThreadDraw()
{
	g_DrawInfo.memDC.DeleteDC();
	BigFont.DeleteObject();
	MidFont.DeleteObject();
	LittleFont.DeleteObject();
 }

bool CGameThreadDraw::InitDraw(CDC * pDrawDC)
 {
	 GDIModeInitThread(pDrawDC,&g_DrawInfo);
	 return CThreadDraw::InitDraw(pDrawDC);
 }
	//销毁绘画
 bool CGameThreadDraw::UnInitDraw()
 {
	return CThreadDraw::UnInitDraw();
 }
	//绘画函数
void CGameThreadDraw::ThreadDrawView(CDC * pDC, int nClinetWith, int nClinetHeight)
 {
	GDIModeDraw(pDC,&g_DrawInfo);
 }

 void CGameThreadDraw::DrawViewFrameAndBg(CDC *pDC)
 {
	 CBrush brush;	 
	 brush.CreateSolidBrush(RGB(48,136,217));
	 CBitmap bit;
	 bit.CreateCompatibleBitmap(pDC,g_DrawInfo.m_ViewWidth,g_DrawInfo.m_ViewHeight);
	 g_DrawInfo.memDC.SelectObject(bit);
	 g_DrawInfo.memDC.SelectObject(&brush);
	 g_DrawInfo.memDC.Rectangle(CRect(0,0,g_DrawInfo.m_ViewWidth,g_DrawInfo.m_ViewHeight));


	/////////////////////////////////
	//计算主棋盘坐标
	 g_DrawInfo.kx = (g_DrawInfo.m_ViewWidth - g_DrawInfo.m_XBoard*2 - m_ImageChessboard.GetWidth()-m_ImageFrame.GetWidth())/3;
	g_DrawInfo.m_ChessPos_X = g_DrawInfo.m_ViewWidth - g_DrawInfo.m_XBoard - m_ImageChessboard.GetWidth() -g_DrawInfo.kx+20;
	g_DrawInfo.m_ChessPos_Y = 10;

	m_ImageBkg.BitBlt(g_DrawInfo.memDC.m_hDC,0,0,m_ImageBkg.GetWidth(),m_ImageBkg.GetHeight(),0,23,SRCCOPY);


	//绘制棋盘背景
	m_ImageChessboard.BitBlt(g_DrawInfo.memDC.m_hDC,g_DrawInfo.m_ChessPos_X, 
		g_DrawInfo.m_ChessPos_Y,SRCCOPY);
	
	//球框
	g_DrawInfo.m_nextballpos.x = g_DrawInfo.m_ChessPos_X +5;
	g_DrawInfo.m_nextballpos.y = g_DrawInfo.m_ChessPos_Y-m_ImageMARK.GetHeight() -10;

	//边框
	m_ImageFrame.AlphaDrawImage(&(g_DrawInfo.memDC),10,10,RGB(255,0,255));

	//绘制缩略棋盘背景
	for(int i=0; i<3; i++)
	{
		m_sImageChessboard.AlphaDrawImage(&(g_DrawInfo.memDC),40+ g_DrawInfo.kx,
			10+i*(m_sImageChessboard.GetHeight())+13+i*12,RGB(255,0,255));
	}

	//道具
	m_ImageProp.AlphaDrawImage(&(g_DrawInfo.memDC),g_DrawInfo.m_PropPos.x,g_DrawInfo.m_PropPos.y,RGB(255,0,255));

	//七龙珠
	m_ImageMARK.AlphaDrawImage(&(g_DrawInfo.memDC),g_DrawInfo.m_PropPos.x-10, 
		440,RGB(255,0,255));

	//定时器
	m_ImageTimer1.AlphaDrawImage(&(g_DrawInfo.memDC),
		g_DrawInfo.m_ChessPos_X+207,g_DrawInfo.m_ChessPos_Y+627,RGB(255,0,255));//627

	//进度条
	m_ImageRate_B.AlphaDrawImage(
					&(g_DrawInfo.memDC),
					g_DrawInfo.pPlayView->m_ptName[2].x+33+90, 
					g_DrawInfo.pPlayView->m_ptName[2].y+10,
					RGB(255,0,255)
					);

	//聊天
	m_ImageTalk.AlphaDrawImage(&(g_DrawInfo.memDC),
		25,
		657,
		RGB(255,0,255)
		);

	bit.DeleteObject();
 }

//绘画函数
void CGameThreadDraw::GDIModeDraw(CDC * pDC, DrawInfoStruct * pDrawInfo)
{
	//绘制view边框和背景棋盘
	if(pDC!=NULL && g_DrawInfo.ismemup==true)
	{
		DrawViewFrameAndBg(pDC);
		g_DrawInfo.ismemup = false;
	}
	pDC->BitBlt(0,0,g_DrawInfo.m_ViewWidth,g_DrawInfo.m_ViewHeight,&(g_DrawInfo.memDC),0,0,SRCCOPY);


	//绘制小球
	int x,y;
	for(BYTE i=1; i< 82; i++)
	{
		if(g_DrawInfo.m_Chess[i].color != 0)
		{
			x = g_DrawInfo.m_ChessPos_X + g_DrawInfo.m_ChessBoardX+ (CHESS_C_W)*((i-1)%9);
			y = g_DrawInfo.m_ChessPos_Y +
				g_DrawInfo.m_ChessBoardY + (CHESS_C_H)*((i-1)/9);
			if(g_DrawInfo.m_Chess[i].prop == 0)
			{
				m_ImageChessman.AlphaDrawImage(
					pDC, 
					x+2, y+2,
					BALLSIZE,BALLSIZE,
					BALLSIZE *(g_DrawInfo.m_Chess[i].color -1),  
					0,
					RGB(255,0,255));
			}
			else
			{
				m_ImageBallImplement.AlphaDrawImage(
					pDC, 
					x+2, y+2,
					BALLSIZE,BALLSIZE,
					BALLSIZE *(g_DrawInfo.m_Chess[i].color -1),  
					0,
					RGB(255,0,255));
			}
		}
	}


	//缩略小球
	for(BYTE q = 0; q<3;q++)
	{
		for(BYTE i=1; i< 82; i++)
		{
			if(g_DrawInfo.m_BreviaryView[q]->m_Chess[i].color != 0)
			{
				x =  40+ g_DrawInfo.kx + g_DrawInfo.m_sChessBoardX + (15+1)*((i-1)%9);
				y = 10+q*(m_sImageChessboard.GetHeight())+13+q*12 + g_DrawInfo.m_sChessBoardY + (15+1)*((i-1)/9);
				m_sImageChessman.AlphaDrawImage(pDC, x+1, y+1,14,14,
					14*(g_DrawInfo.m_BreviaryView[q]->m_Chess[i].color-1),0,RGB(255,0,255)); 
			}
		}
	}

	//路径
	if(g_DrawInfo.m_Result!=ERROR_POINT)
	{
		x = g_DrawInfo.m_ChessPos_X + g_DrawInfo.m_ChessBoardX + (CHESS_C_W)*((g_DrawInfo.m_Result-1)%9);
		
		y = g_DrawInfo.m_ChessPos_Y + g_DrawInfo.m_ChessBoardY + (CHESS_C_H)*((g_DrawInfo.m_Result-1)/9);
		if(g_DrawInfo.m_ResultColor.prop==0)
		{
		m_ImageChessman.AlphaDrawImage(
			pDC, 
			x+2, y+2,
			BALLSIZE,
			BALLSIZE,
			BALLSIZE*(g_DrawInfo.m_ResultColor.color-1), 
			0,
			RGB(255,0,255)
			);
		}
		else
			{
				m_ImageBallImplement.AlphaDrawImage(
					pDC, 
					x+2, y+2,
					BALLSIZE,BALLSIZE,
					BALLSIZE *(g_DrawInfo.m_ResultColor.color-1),  
					0,
					RGB(255,0,255));
			}
	}

	//开始位置
	if(g_DrawInfo.m_start != ERROR_POINT)
	{
		x = g_DrawInfo.m_ChessPos_X + g_DrawInfo.m_ChessBoardX + (CHESS_C_W)*((g_DrawInfo.m_start-1)%9);
		y = g_DrawInfo.m_ChessPos_Y +
			g_DrawInfo.m_ChessBoardY + (CHESS_C_H)*((g_DrawInfo.m_start-1)/9);

		if(g_DrawInfo.start_do)
		{
			if(g_DrawInfo.m_Chess[g_DrawInfo.m_start].prop==0)
			{
			m_ImageChessman2.AlphaDrawImage(
				pDC, 
				x+2, y+2,
				BALLSIZE,BALLSIZE,
				BALLSIZE *(g_DrawInfo.m_Chess[g_DrawInfo.m_start].color -1), 
				0,
				RGB(255,0,255));
			}
			else
			{
				m_ImageBallImplement.AlphaDrawImage(
				pDC, 
				x+2, y+2,
				BALLSIZE,BALLSIZE,
				BALLSIZE *(g_DrawInfo.m_Chess[g_DrawInfo.m_start].color -1),
				0,
				RGB(255,0,255));
			}
		}
		else
		{
			if(g_DrawInfo.m_Chess[g_DrawInfo.m_start].prop==0)
			{
			m_ImageChessman.AlphaDrawImage(
				pDC, 
				x+2, y+2,
				BALLSIZE,BALLSIZE,
				BALLSIZE *(g_DrawInfo.m_Chess[(g_DrawInfo.m_start)].color-1),
				0,
				RGB(255,0,255));
			}
			else
			{
				m_ImageBallImplementPick.AlphaDrawImage(
				pDC, 
				x+2, y+2,
				BALLSIZE,BALLSIZE,
				BALLSIZE *(g_DrawInfo.m_Chess[g_DrawInfo.m_start].color-1), 
				0,
				RGB(255,0,255));
			}
		}
	}
	
	//绘制下一轮发球的颜色
	for(BYTE i  = 0; i<3; i++)
	{
		if(g_DrawInfo.m_nextballcolor[i].color > 0&& g_DrawInfo.m_nextballcolor[i].color <8) 
		{
			m_ImageChessman.AlphaDrawImage(pDC, g_DrawInfo.m_ChessPos_X + 312 + i*(15 + 58) + 5,g_DrawInfo.m_ChessPos_Y + 12 + 2,BALLSIZE,BALLSIZE,BALLSIZE*(g_DrawInfo.m_nextballcolor[i].color -1),0,RGB(255,0,255));	
		}
	}

	//走动定时器
	m_ImageTimer2.AlphaDrawImage(pDC,
		g_DrawInfo.m_ChessPos_X+207+g_DrawInfo.m_TimerCount,
		g_DrawInfo.m_ChessPos_Y+627,
		m_ImageTimer2.GetWidth()-g_DrawInfo.m_TimerCount,
		m_ImageTimer2.GetHeight(),
		g_DrawInfo.m_TimerCount,
		0,
		RGB(255,0,255)
		);

	//炸弹效果
	if(g_DrawInfo.isbomb)
	{
		m_ImageBomb.AlphaDrawImage(pDC,g_DrawInfo.m_BombPos.x,g_DrawInfo.m_BombPos.y,RGB(255,0,255));
	}
	//超时
	if(g_DrawInfo.m_GameTimeOut)
	{
		pDC->SelectObject(&BigFont);
		pDC->TextOut(g_DrawInfo.m_ViewWidth/2-200,
			g_DrawInfo.m_ChessPos_Y+m_ImageChessboard.GetHeight()/2-10,
			"游戏失败，等待其他玩家");
	}

	//绘制游戏开始
	if(g_DrawInfo.isstart)
	{
		pDC->SelectObject(&BigFont);
		pDC->TextOut(g_DrawInfo.m_ViewWidth/2-85,
			g_DrawInfo.m_ChessPos_Y+m_ImageChessboard.GetHeight()/2-10,
			"游戏已经开始");
	}

	if(g_DrawInfo.m_succ != ERROR_POINT)
	{
		
		for(int i=0; g_DrawInfo.m_ChessView->m_DeadBall[i].standby!=0; i++)
		{
			x = g_DrawInfo.m_ChessPos_X + g_DrawInfo.m_ChessBoardX + (CHESS_C_W)*((g_DrawInfo.m_ChessView->m_DeadBall[i].standby-1)%9);
			y = g_DrawInfo.m_ChessPos_Y +
				g_DrawInfo.m_ChessBoardY + (CHESS_C_H)*((g_DrawInfo.m_ChessView->m_DeadBall[i].standby-1)/9);

			if(g_DrawInfo.m_succ_up ==1) //
			{
			m_ImageDeadBall1.AlphaDrawImage(
					pDC, 
					x+2, y+2,
					BALLSIZE,BALLSIZE,
					BALLSIZE *(g_DrawInfo.m_ChessView->m_DeadBall[1].color-1),  
					0,
					RGB(255,0,255));
			}
			else
			if(g_DrawInfo.m_succ_up ==2) 
			m_ImageDeadBall2.AlphaDrawImage(
					pDC, 
					x+2, y+2,
					BALLSIZE,BALLSIZE,
					BALLSIZE *(g_DrawInfo.m_ChessView->m_DeadBall[1].color-1),  
					0,
					RGB(255,0,255));
		}

		pDC->SelectObject(&CSkinWndAttribute::m_DefaultFont);
		x = g_DrawInfo.m_ChessPos_X + g_DrawInfo.m_ChessBoardX + (CHESS_C_W)*((g_DrawInfo.m_succ-1)%9);
		y = g_DrawInfo.m_ChessPos_Y +
			g_DrawInfo.m_ChessBoardY + (CHESS_C_H)*((g_DrawInfo.m_succ-1)/9) - g_DrawInfo.m_TextScorePosKx;

		if(x>g_DrawInfo.m_ViewWidth - 150)
			x = g_DrawInfo.m_ViewWidth - 150;
		pDC->TextOut(x,y-40,g_DrawInfo.m_TextScore);
		i=0;

		if(g_DrawInfo.isHitToo)
		{
			i+=20;
			pDC->TextOut(x,y-40+i,g_DrawInfo.m_TextHitToo);
		}
		if(g_DrawInfo.m_isGetSelect)
		{
			i+=20;
			pDC->TextOut(x,y+i,g_DrawInfo.m_TextGetSelect);
		}
		if(g_DrawInfo.m_isGetBomb)
		{
			i+=20;
			pDC->TextOut(x,y+i,g_DrawInfo.m_TextGetBomb);
		}
		if(g_DrawInfo.m_isGetMagic)
		{
			i+=20;
			pDC->TextOut(x,y+i,g_DrawInfo.m_TextGetMagic);
		}
		if(g_DrawInfo.m_isGetStay)
		{
			i+=20;
			pDC->TextOut(x,y+i,g_DrawInfo.m_TextGetStay);
		}
		
	}

	//暂停
	if(g_DrawInfo.m_TimeStay)
	{
		pDC->SelectObject(BigFont);
		pDC->TextOut(g_DrawInfo.m_ViewWidth/2-175,
			g_DrawInfo.m_ChessPos_Y+m_ImageChessboard.GetHeight()/2-10,
			g_DrawInfo.m_TimeStayString);
	}


	//释放道具
	if(g_DrawInfo.m_isSetStay)
	{
		pDC->SelectObject(&CSkinWndAttribute::m_DefaultFont);
		pDC->TextOut(g_DrawInfo.m_SetStayPos.x,g_DrawInfo.m_SetStayPos.y,"成功释放道具");
	}

	
	g_DrawInfo.pPlayView->SetUserPer();
	//绘画用户
	pDC->SelectObject(&LittleFont);
	pDC->SetTextColor(RGB(255,255,0));
	for (BYTE i=0;i<GAME_PLAYER;i++)
	{
		//变量定义
		const tagUserData * pUserData=g_DrawInfo.pPlayView->m_UserID[i];
		if (pUserData!=NULL)
		{
			//用户名字
			pDC->SetTextAlign(TA_LEFT);
			pDC->TextOut(g_DrawInfo.pPlayView->m_ptName[i].x,
				g_DrawInfo.pPlayView->m_ptName[i].y,
				pUserData->szName,lstrlen(pUserData->szName));

			//其他信息
			if (pUserData->cbUserStatus==US_READY) g_DrawInfo.pPlayView->DrawUserReady(pDC,g_DrawInfo.pPlayView->m_ptReady[i].x,g_DrawInfo.pPlayView->m_ptReady[i].y);

			//用户得分
			
			if(i == 3)
			{
				pDC->TextOut(g_DrawInfo.pPlayView->m_ptName[i].x+90, 
					g_DrawInfo.pPlayView->m_ptName[i].y-8, 
					g_DrawInfo.m_BreviaryView[2]->m_class,lstrlen(g_DrawInfo.m_BreviaryView[2]->m_class));
				pDC->TextOut(g_DrawInfo.pPlayView->m_ptName[i].x+90, 
					g_DrawInfo.pPlayView->m_ptName[i].y+8, 
					g_DrawInfo.m_BreviaryView[2]->m_scorestring,lstrlen(g_DrawInfo.m_BreviaryView[2]->m_scorestring));

				//团队信息
				if(g_DrawInfo.m_BreviaryView[2]->m_GroupID == 1)
				{
				m_ImageMemA.BitBlt(pDC->m_hDC,48,200*(i-1)+38);
				}
				else if(g_DrawInfo.m_BreviaryView[2]->m_GroupID == 2)
				{
				m_ImageMemB.BitBlt(pDC->m_hDC,48,200*(i-1)+38);
				}

				if(g_DrawInfo.pPlayView->m_BreviaryView[2].m_isLose)
				{
					m_ImageLose.AlphaDrawImage(pDC,g_DrawInfo.pPlayView->m_ptName[i].x,g_DrawInfo.pPlayView->m_ptName[i].y-100,RGB(255,0,255));
				}
				else
				{
					if(g_DrawInfo.m_isGameOver)
					{
						m_ImageWin.AlphaDrawImage(pDC,g_DrawInfo.pPlayView->m_ptFace[i].x-32,g_DrawInfo.pPlayView->m_ptFace[i].y-100,RGB(255,0,255));
					}
				}
			}
			else
			if(i==2)
			{
				pDC->TextOut(g_DrawInfo.pPlayView->m_ptName[i].x+90, 
				g_DrawInfo.pPlayView->m_ptName[i].y -5,
				g_DrawInfo.m_ChessView->m_class);

				pDC->TextOut(g_DrawInfo.pPlayView->m_ptName[i].x+90, 
				g_DrawInfo.pPlayView->m_ptName[i].y+10,"升级:");

				m_ImageRate.AlphaDrawImage(
					pDC,
					g_DrawInfo.pPlayView->m_ptName[i].x+33+g_DrawInfo.pPlayView->m_ChessView.m_Iper/2+1+90, 
					g_DrawInfo.pPlayView->m_ptName[i].y+10,
					52-g_DrawInfo.pPlayView->m_ChessView.m_Iper/2,
					13,
					g_DrawInfo.pPlayView->m_ChessView.m_Iper/2+1,
					0,
					RGB(255,0,255)
					);

				pDC->TextOut(g_DrawInfo.pPlayView->m_ptName[i].x+90, 
				g_DrawInfo.pPlayView->m_ptName[i].y + 25,
				g_DrawInfo.m_ChessView->m_scorestring);

				if(g_DrawInfo.m_ChessView->m_GroupID == 1)
				{
				m_ImageMemA.BitBlt(pDC->m_hDC,g_DrawInfo.pPlayView->m_ptName[i].x,g_DrawInfo.pPlayView->m_ptName[i].y+15);
				}
				else if(g_DrawInfo.m_ChessView->m_GroupID == 2)
				{
				m_ImageMemB.BitBlt(pDC->m_hDC,g_DrawInfo.pPlayView->m_ptName[i].x,g_DrawInfo.pPlayView->m_ptName[i].y+15);
				}

				if(g_DrawInfo.pPlayView->m_ChessView.m_isLose)
				{
					m_ImageLose.AlphaDrawImage(pDC,960-150,590-40,RGB(255,0,255));
				}
				else
				if(g_DrawInfo.m_isGameOver)
					{
						m_ImageWin.AlphaDrawImage(pDC,960-150,590-40,RGB(255,0,255));
					}
			}
			else
			{
				pDC->TextOut(g_DrawInfo.pPlayView->m_ptName[i].x+90,
				g_DrawInfo.pPlayView->m_ptName[i].y-8, 
				g_DrawInfo.m_BreviaryView[i]->m_class);

				pDC->TextOut(g_DrawInfo.pPlayView->m_ptName[i].x+90,
				g_DrawInfo.pPlayView->m_ptName[i].y+8, 
				g_DrawInfo.m_BreviaryView[i]->m_scorestring);

				if(g_DrawInfo.m_BreviaryView[i]->m_GroupID == 1)
				{
				m_ImageMemA.BitBlt(pDC->m_hDC,48,(i)*200+35);
				}
				else if(g_DrawInfo.m_BreviaryView[i]->m_GroupID == 2)
				{
				m_ImageMemB.BitBlt(pDC->m_hDC,48,(i)*200+35);
				}
				if(g_DrawInfo.pPlayView->m_BreviaryView[i].m_isLose)
				{
					m_ImageLose.AlphaDrawImage(pDC,g_DrawInfo.pPlayView->m_ptName[i].x,g_DrawInfo.pPlayView->m_ptName[i].y-100,RGB(255,0,255));
				}
				else
				if(g_DrawInfo.m_isGameOver)
				{
					m_ImageWin.AlphaDrawImage(pDC,g_DrawInfo.pPlayView->m_ptName[i].x,g_DrawInfo.pPlayView->m_ptName[i].y-100,RGB(255,0,255));
				}
			}
		}
	}
	
}

void CGameThreadDraw::GDIModeInitThread(CDC *pDrawDC,DrawInfoStruct * pDrawInfo)
{
	
	//绘画背景
	dwSpeed=0L;
	dwBeginTime=(long int)time(NULL);
	dwEndTime=(long int)time(NULL);
	BigFont.CreateFont(-28,0,0,0,400,NULL,NULL,NULL,134,3,2,1,2,TEXT("黑体"));
	MidFont.CreateFont(-18,0,0,0,400,NULL,NULL,NULL,134,3,2,1,2,TEXT("宋体"));
	LittleFont.CreateFont(-12,0,0,0,400,NULL,NULL,NULL,134,3,2,1,2,TEXT("宋体"));
	int SCREEN=::GetSystemMetrics(SM_CYSCREEN);
	HINSTANCE hInstance = ::AfxGetInstanceHandle();
	//棋盘
	if(m_ImageChessboard.IsNull())
	{
		m_ImageChessboard.LoadFromResource(hInstance,IDB_CAIQIU);
	}
	if(m_sImageChessboard.IsNull())
	{
		m_sImageChessboard.LoadFromResource(hInstance,IDB_XIAOCHESS);
	}

	//棋子
		m_ImageChessman.LoadFromResource(hInstance,IDB_BALL_MAIN);
		m_ImageChessman2.LoadFromResource(hInstance,IDB_BALL_MAIN2);
	

	//缩略棋盘棋子资源
	if(m_sImageChessman.IsNull())
	{
			m_sImageChessman.LoadFromResource(hInstance,IDB_BALL_SMALL);
	}

	m_ImageFrame.LoadFromResource(hInstance,IDB_FRAME);

	//背景
	m_ImageBkg.LoadFromResource(hInstance,IDB_BKG);
	//标志
	m_ImageMARK.LoadFromResource(hInstance,IDB_MARK);
	//道具
	m_ImageProp.LoadFromResource(hInstance,IDB_PROP);
	//炸弹
	m_ImageBomb.LoadFromResource(hInstance,IDB_BOMB_BOMB);


	//聊天
	m_ImageTalk.LoadFromResource(hInstance,IDB_TALK);

	//团队信息
	m_ImageMemA.LoadFromResource(hInstance,IDB_MEMINFO_A);
	m_ImageMemB.LoadFromResource(hInstance,IDB_MEMINFO_B);

	//Timer
	m_ImageTimer1.LoadFromResource(hInstance,IDB_TIMERONE);
	m_ImageTimer2.LoadFromResource(hInstance,IDB_TIMERTWO);

	//预藏道具标志
	m_ImageBallImplement.LoadFromResource(hInstance,IDB_BALL_IMPLEMENT);
	m_ImageBallImplementPick.LoadFromResource(hInstance,IDB_BALL_IMPLEMENT_PICK);

	m_ImageLose.LoadFromResource(hInstance,IDB_LOSE);
	m_ImageWin.LoadFromResource(hInstance,IDB_WIN);

	//
	m_ImageRate.LoadFromResource(hInstance,IDB_SPEED);
	m_ImageRate_B.LoadFromResource(hInstance,IDB_SPEED_B);

	//
	m_ImageDeadBall1.LoadFromResource(hInstance,IDB_DEADBALL1);
	m_ImageDeadBall2.LoadFromResource(hInstance,IDB_DEADBALL2);

	g_DrawInfo.isbomb = false;
	g_DrawInfo.isstart = false;
	g_DrawInfo.start_do = false;
	g_DrawInfo.isHitToo = false;
	g_DrawInfo.m_isGetSelect = false;
	g_DrawInfo.m_isGetBomb = false;
	g_DrawInfo.m_isGetMagic = false;
	g_DrawInfo.m_isGetStay = false;
	g_DrawInfo.m_TimeStay = false;
	g_DrawInfo.m_HundredDisplayed = false;
	g_DrawInfo.m_Exered = false;
	g_DrawInfo.m_GameTimeOut = false;
	g_DrawInfo.m_isSetStay = false;
	g_DrawInfo.m_isGameOver = false;

	g_DrawInfo.m_succ_up = 0;
	g_DrawInfo.m_XBoard = 10;
	g_DrawInfo.m_YBoard = 12;
	g_DrawInfo.m_ChessBoardX = 28;
	g_DrawInfo.m_ChessBoardY = 92;
	g_DrawInfo.m_sChessBoardX = 31;
	g_DrawInfo.m_sChessBoardY = 6;
	g_DrawInfo.m_Result=ERROR_POINT;
	g_DrawInfo.m_start = ERROR_POINT;

	g_DrawInfo.m_succ = ERROR_POINT;
	g_DrawInfo.m_TextScorePosKx = 0;
	_snprintf(g_DrawInfo.m_TextScore,sizeof(g_DrawInfo.m_TextScore),"");
	_snprintf(g_DrawInfo.m_TextHitToo,sizeof(g_DrawInfo.m_TextHitToo),"");
	_snprintf(g_DrawInfo.m_TimeStayString,sizeof(g_DrawInfo.m_TimeStayString),"");
	_snprintf(g_DrawInfo.m_TextGetSelect,sizeof(g_DrawInfo.m_TextGetSelect),"");
	_snprintf(g_DrawInfo.m_TextGetBomb,sizeof(g_DrawInfo.m_TextGetBomb),"");
	_snprintf(g_DrawInfo.m_TextGetMagic,sizeof(g_DrawInfo.m_TextGetMagic),"");
	_snprintf(g_DrawInfo.m_TextGetStay,sizeof(g_DrawInfo.m_TextGetStay),"");

	g_DrawInfo.m_nextballcolor = g_DrawInfo.m_ChessView->m_nextballcolor;

	g_DrawInfo.m_Chess = g_DrawInfo.m_ChessView->m_Chess;

	pDrawInfo->memDC.CreateCompatibleDC(pDrawDC);
}

void CGameThreadDraw::DrawBlog(CDC * pDC, int type, int x, int y)
{
}
