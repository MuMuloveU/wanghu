
//***********************************************
//File		: ScoreWnd.cpp 
//Time		: 2005-02-25
//Author	: Luwenguang
//Comment	: 显示得分情况类
//***********************************************


//===============================================
//	include 
//===============================================
#include "stdafx.h"
#include "resource.h"
#include <math.h>
#include "ScoreWnd.h"
#include "Define.h"


IMPLEMENT_DYNAMIC(CScoreWnd, CWnd)


//===============================================
//	构造,析构,初始化函数
//===============================================
CScoreWnd::CScoreWnd()
{
	this->Init();
}

CScoreWnd::~CScoreWnd()
{
}

void	CScoreWnd::Init(void)
{	
	for(int i=0; i<GAME_PLAYER; i++)
	{
		::memset(m_name[i], 0, sizeof(m_name[i]));
		m_factscore[i] = 0;
	}
}

void	CScoreWnd::GetSize(CSize & size)
{
	if(m_bkg.IsNull())
	{
		return ;
	}

	size.cx = m_bkg.GetWidth();
	size.cy = m_bkg.GetHeight();
}


BEGIN_MESSAGE_MAP(CScoreWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_CREATE()
END_MESSAGE_MAP()


//===============================================
//	重载虚函数与消息响应函数
//===============================================
void	CScoreWnd::OnPaint()
{
	CPaintDC dc(this);

	m_bkg.AlphaDrawImage(&dc, 
					m_Rct_Play.left,m_Rct_Play.top, 
					m_Rct_Play.Width(),m_Rct_Play.Height(), 
					0,0,								
					RGB(0x00,0x00,0xff));
	
	CFont font;
	VERIFY(font.CreateFont(
					14,                        // nHeight
   					0,                         // nWidth
   					0,                         // nEscapement
   					0,                         // nOrientation
   					FW_NORMAL,                 // nWeight
   					FALSE,                     // bItalic
   					FALSE,                     // bUnderline
   					0,                         // cStrikeOut
   					ANSI_CHARSET,              // nCharSet
   					OUT_DEFAULT_PRECIS,        // nOutPrecision
   					CLIP_DEFAULT_PRECIS,       // nClipPrecision
   					DEFAULT_QUALITY,           // nQuality
   					DEFAULT_PITCH|FF_SWISS,	   // nPitchAndFamily
   					"Arial"));

	HFONT hFont = (HFONT)dc.SelectObject(font.m_hObject);
	this->DrawScore(&dc, m_Rct_Play);
	dc.SelectObject(hFont);
}

void CScoreWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	
	m_Rct_Play.left		= 0;
	m_Rct_Play.top		= 0;
	m_Rct_Play.right	= cx;
	m_Rct_Play.bottom	= cy;

	this->LoadBitmap_();
}

int		CScoreWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	return 1;
}


//===============================================
//	其他成员函数
//===============================================
//函数功能：比赛结束后显示得分情况
//函数参数: 
//函数返回:
bool	CScoreWnd::LoadBitmap_(void)
{
	if(!m_bkg.IsNull())
	{
		m_bkg.Detach();
	}
	m_bkg.LoadFromResource(::AfxGetInstanceHandle(), IDB_SCOREBKG);

	return true;
}

//函数功能：比赛结束后显示得分情况
//函数参数: 
//函数返回:
void	CScoreWnd::DrawScore(CDC *pDC, 
								CRect &rect)
{
	//
	//画得分
	//
	//     玩家		得分	   
	//     玩家1	100		
	//     玩家2	100		
	//     玩家3	20		
	//     玩家4	30		

	int			i;
	int			v		= rect.top+110;
	int			h		= rect.left+75;
	const int	v_space = 25;
	const int	h_space = 47;
	const int   size	= 2;
	TCHAR *title[]		= {"玩家", "得分"};
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(200,124,76)); 
	pDC->SetTextAlign(TA_BOTTOM|TA_LEFT);
	for(i=0; i<size; i++)
	{
		pDC->TextOut(h+h_space*i*2, v-20, title[i]); 
	}
	int v_base = v + v_space + 3;
	int h_base = h;

	pDC->SetTextAlign(TA_BOTTOM|TA_LEFT);
	pDC->SetTextColor(RGB(255,255,0)); 


	//画名字
	for(i=0; i<GAME_PLAYER; i++)
	{
		CRect rect_temp;
		
		rect_temp.left		= h_base;
		rect_temp.top		= v_base+v_space*(i-1)-20;
		rect_temp.right		= rect_temp.left + 60;
		rect_temp.bottom	= rect_temp.top + 35 ;
		pDC->DrawText(m_name[i], &rect_temp, DT_LEFT|DT_SINGLELINE|DT_BOTTOM|DT_END_ELLIPSIS);
	}

	
	TCHAR temp[64];

	//画实际得分
	for(i=0; i<GAME_PLAYER; i++)
	{
		::memset(temp, 0, sizeof(temp));
		
		::itoa(m_factscore[i], temp, 10);
		pDC->TextOut(h_base+h_space*2, v_base+v_space*(i-1), temp);
	}
}

//函数功能：设置用户名
//函数参数: 
//函数返回:
void	CScoreWnd::SetName(int desk, TCHAR *pName)
{
	::strcpy(m_name[desk], pName);
}


//函数功能：设置实际得分
//函数参数: 
//函数返回:
void	CScoreWnd::SetFactScore(int desk, LONG score)
{
	m_factscore[desk] = score;
}

//===============================================
//					the end
//===============================================
