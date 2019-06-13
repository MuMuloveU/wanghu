// D3DWnd.cpp : 实现文件

#include "stdafx.h"
#include "D3DWnd.h"
#include ".\d3dwnd.h"
#include "mmsystem.h"

#define			RENDER_THREAD			101				

///////////////////////////////////////////////
//定时器命令
#define			ID_STOPIT				100

static bool keyi;

// CD3DWnd

IMPLEMENT_DYNAMIC(CD3DWnd, CWnd)
CD3DWnd::CD3DWnd()
{
	TFPS=0;
	FPS=0;
	fTimerFPS=0;
	for(int i=0; i<46; i++)
        m_link_Texture[i]	= NULL;
	for(int i=0; i<10; i++)
		m_line_Texture[i]	= NULL;
	m_ready_Texture			= NULL;

	m_p2DFont				= NULL;

	m_pSurface				= NULL;
	m_pBkSurface			= NULL;
	m_pLoseSurface			= NULL;
	m_pChess_Surface		= NULL;
	m_pChessFace_Surface	= NULL;
	m_pChessFaceSmall_Surface = NULL;


	g_bDeviceLost = false;

	memset(m_UserData,0,sizeof(m_UserData));
}

CD3DWnd::~CD3DWnd()
{
	
}


BEGIN_MESSAGE_MAP(CD3DWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_MESSAGE(RENDER_THREAD, OnRender)
END_MESSAGE_MAP()

void ThreadRender(LPTHREAD_START_ROUTINE pThreadData)
{
	//效验参数
	ASSERT(pThreadData!=NULL);
	CD3DWnd * pDirect3D=(CD3DWnd *)pThreadData;
	while(keyi)
	{
		Sleep(10);
		pDirect3D->SendMessage(RENDER_THREAD,0,0);
	}
}

LRESULT CD3DWnd::OnRender(WPARAM wParam, LPARAM lParam)
{
	Render();
	return true;
}

// 初始化D3D
void CD3DWnd::InitD3D(void)
{
	HRESULT rslt;

	m_pD3D = Direct3DCreate9( D3D_SDK_VERSION );

    //D3DCAPS9 d3dCaps;
   // m_pD3D->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &d3dCaps );

    rslt = m_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &D3DDisplayMode );
	if(FAILED(rslt)) {MessageBox(TEXT("获得显示适配器模式失败!"),"",MB_OK);return;}

    ZeroMemory( &d3dpp, sizeof(d3dpp) );

    d3dpp.Windowed         = TRUE;
    d3dpp.SwapEffect       = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DDisplayMode.Format;

    d3dpp.EnableAutoDepthStencil = FALSE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_UNKNOWN;
    //d3dpp.PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE; // Do NOT sync to vertical retrace
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
    d3dpp.PresentationInterval   = D3DPRESENT_INTERVAL_DEFAULT; // Sync to vertical retrace
    d3dpp.Flags                  = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

	//创建DirectX9驱动
    rslt = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd,
                          D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                          &d3dpp, &m_pDevice );
	if(FAILED(rslt))
	{
		rslt = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, m_hWnd,
                          D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                          &d3dpp, &m_pDevice );
						  
		if(FAILED(rslt))
		{
			if(MessageBox(TEXT("亲爱的用户：\r\n运行连连看的时候，创建DirectX9驱动失败!\r\n您的电脑可能没有安装DirectX9.0c,必须安装之后才能运行游戏！\r\n是否立即安装？"),"",MB_OKCANCEL) == IDOK)
			{
				m_Logic.OpenUrl("http://abc.zxip.com/down/directx_9c.exe");
			}
			return;
		}
	}

	/*
	//创建参数
	
	::ZeroMemory(&d3dpp,sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;
	d3dpp.EnableAutoDepthStencil = false;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8; //D3DDisplayMode.Format;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	
	//创建驱动
	rslt = m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,m_hWnd,D3DCREATE_SOFTWARE_VERTEXPROCESSING,&d3dpp,
		&m_pDevice);
	if(FAILED(rslt)) {MessageBox(TEXT("创建DirectX驱动失败!"),"",MB_OK);return;}
	*/

//----------------------------
	D3DXMATRIX matView;
    D3DXMatrixLookAtLH( &matView, &D3DXVECTOR3(0.0f, 0.0f,-25.0f), 
		                          &D3DXVECTOR3(0.0f, 0.0f, 0.0f), 
		                          &D3DXVECTOR3(0.0f, 1.0f, 0.0f ) );
    m_pDevice->SetTransform( D3DTS_VIEW, &matView );

    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DXToRadian( 45.0f ), 1.0f, 1.0f, 100.0f );
    m_pDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    m_pDevice->SetRenderState( D3DRS_LIGHTING,  FALSE );
	//----------------------------------

	
	//建模
    Geometry();

	//初始棋盘
	InitChess();
	UpdateBackSurface();

	//开启渲染线程
	UINT uThreadID=0;
	keyi = TRUE;
	m_hRenderThread=(HANDLE)::CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ThreadRender,this,0,0);
}

int CD3DWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	InitD3D();
	return 0;
}

void CD3DWnd::MakeBlast(void)
{
	//绘制连接爆炸效果
	for(int k=0; k<m_Blast.GetCount(); ++k)
	{
		for(int i=0, p=0; i < m_Blast.ElementAt(k).result.GetCount(); ++i,++p)
		{
			if(i == 0|| i+1 == m_Blast.ElementAt(k).result.GetCount())
			{
				m_Blast.ElementAt(k).m_A_Link.m_nPosX = (float)CHESSFRAME_W+(m_Blast.ElementAt(k).result[i].x)*CHESSWIDTH-12;
				m_Blast.ElementAt(k).m_A_Link.m_nPosY = (float)CHESSFRAME_H+(m_Blast.ElementAt(k).result[i].y)*CHESSHEIGHT-6;
				m_Blast.ElementAt(k).m_A_Link.SetTexture(m_link_Texture[m_Blast.ElementAt(k).m_A_Link.m_nCurrentFrame]);
				m_Blast.ElementAt(k).m_A_Link.DrawSprite(m_pDevice,0.8f);
			}
			else
			{
				m_Blast.ElementAt(k).m_A_Line.m_nPosX = (float)CHESSFRAME_W+(m_Blast.ElementAt(k).result[i].x)*CHESSWIDTH;
				m_Blast.ElementAt(k).m_A_Line.m_nPosY = (float)CHESSFRAME_H+(m_Blast.ElementAt(k).result[i].y)*CHESSHEIGHT;
				switch(m_Blast.ElementAt(k).m_LinePath[p])
				{
				case 0:
					{
						m_Blast.ElementAt(k).m_A_Line.SetTexture(m_line_Texture[RandNumber(0,5)]);
						m_Blast.ElementAt(k).m_A_Line.DrawSprite(m_pDevice,(10-m_Blast.ElementAt(k).m_A_Line.m_nCurrentFrame)*0.1f);
						m_Blast.ElementAt(k).m_A_Line.SetTexture(m_line_Texture[RandNumber(0,5)]);
						m_Blast.ElementAt(k).m_A_Line.DrawSprite(m_pDevice,(10-m_Blast.ElementAt(k).m_A_Line.m_nCurrentFrame)*0.1f);
						m_Blast.ElementAt(k).m_A_Line.SetTexture(m_line_Texture[RandNumber(0,5)]);
						m_Blast.ElementAt(k).m_A_Line.DrawSprite(m_pDevice,(10-m_Blast.ElementAt(k).m_A_Line.m_nCurrentFrame)*0.1f);
					}break;
				case 1:
					{
						m_Blast.ElementAt(k).m_A_Line.SetTexture(m_line_Texture[RandNumber(5,10)]);
						m_Blast.ElementAt(k).m_A_Line.DrawSprite(m_pDevice,(10-m_Blast.ElementAt(k).m_A_Line.m_nCurrentFrame)*0.1f);
						m_Blast.ElementAt(k).m_A_Line.SetTexture(m_line_Texture[RandNumber(5,10)]);
						m_Blast.ElementAt(k).m_A_Line.DrawSprite(m_pDevice,(10-m_Blast.ElementAt(k).m_A_Line.m_nCurrentFrame)*0.1f);
						m_Blast.ElementAt(k).m_A_Line.SetTexture(m_line_Texture[RandNumber(5,10)]);
						m_Blast.ElementAt(k).m_A_Line.DrawSprite(m_pDevice,(10-m_Blast.ElementAt(k).m_A_Line.m_nCurrentFrame)*0.1f);
					}break;
				case 2:
					{
						//1
						m_Blast.ElementAt(k).m_A_Line.m_nFrameWidth = CHESSWIDTH/2;
						m_Blast.ElementAt(k).m_A_Line.m_nFrameHeight = CHESSHEIGHT;
						m_Blast.ElementAt(k).m_A_Line.SetTexture(m_line_Texture[RandNumber(0,5)]);
						m_Blast.ElementAt(k).m_A_Line.DrawSprite(m_pDevice,(10-m_Blast.ElementAt(k).m_A_Line.m_nCurrentFrame)*0.1f);

						m_Blast.ElementAt(k).m_A_Line.m_nFrameWidth = CHESSWIDTH;
						m_Blast.ElementAt(k).m_A_Line.m_nFrameHeight = CHESSHEIGHT/2+3;
						m_Blast.ElementAt(k).m_A_Line.m_nPosY += 21;
						m_Blast.ElementAt(k).m_A_Line.SetTexture(m_line_Texture[RandNumber(5,10)]);
						m_Blast.ElementAt(k).m_A_Line.DrawSprite(m_pDevice,(10-m_Blast.ElementAt(k).m_A_Line.m_nCurrentFrame)*0.1f);
						m_Blast.ElementAt(k).m_A_Line.m_nPosY -= 21;
						//2
						m_Blast.ElementAt(k).m_A_Line.m_nFrameWidth = CHESSWIDTH/2+1;
						m_Blast.ElementAt(k).m_A_Line.m_nFrameHeight = CHESSHEIGHT;
						m_Blast.ElementAt(k).m_A_Line.SetTexture(m_line_Texture[RandNumber(0,5)]);
						m_Blast.ElementAt(k).m_A_Line.DrawSprite(m_pDevice,(10-m_Blast.ElementAt(k).m_A_Line.m_nCurrentFrame)*0.1f);

						m_Blast.ElementAt(k).m_A_Line.m_nFrameWidth = CHESSWIDTH;
						m_Blast.ElementAt(k).m_A_Line.m_nFrameHeight = CHESSHEIGHT/2+3;
						m_Blast.ElementAt(k).m_A_Line.m_nPosY += 21;
						m_Blast.ElementAt(k).m_A_Line.SetTexture(m_line_Texture[RandNumber(5,10)]);
						m_Blast.ElementAt(k).m_A_Line.DrawSprite(m_pDevice,(10-m_Blast.ElementAt(k).m_A_Line.m_nCurrentFrame)*0.1f);
						m_Blast.ElementAt(k).m_A_Line.m_nPosY -= 21;
						//3
						m_Blast.ElementAt(k).m_A_Line.m_nFrameWidth = CHESSWIDTH/2+1;
						m_Blast.ElementAt(k).m_A_Line.m_nFrameHeight = CHESSHEIGHT;
						m_Blast.ElementAt(k).m_A_Line.SetTexture(m_line_Texture[RandNumber(0,5)]);
						m_Blast.ElementAt(k).m_A_Line.DrawSprite(m_pDevice,(10-m_Blast.ElementAt(k).m_A_Line.m_nCurrentFrame)*0.1f);

						m_Blast.ElementAt(k).m_A_Line.m_nFrameWidth = CHESSWIDTH;
						m_Blast.ElementAt(k).m_A_Line.m_nFrameHeight = CHESSHEIGHT/2+3;
						m_Blast.ElementAt(k).m_A_Line.m_nPosY += 21;
						m_Blast.ElementAt(k).m_A_Line.SetTexture(m_line_Texture[RandNumber(5,10)]);
						m_Blast.ElementAt(k).m_A_Line.DrawSprite(m_pDevice,(10-m_Blast.ElementAt(k).m_A_Line.m_nCurrentFrame)*0.1f);
						m_Blast.ElementAt(k).m_A_Line.m_nPosY -= 21;
					}break;
				case 3:
					{
						//1
						m_Blast.ElementAt(k).m_A_Line.m_nFrameWidth = CHESSWIDTH/2+1;
						m_Blast.ElementAt(k).m_A_Line.m_nFrameHeight = CHESSHEIGHT;
						m_Blast.ElementAt(k).m_A_Line.m_nPosX += 21;
						m_Blast.ElementAt(k).m_A_Line.SetTexture(m_line_Texture[RandNumber(0,5)]);
						m_Blast.ElementAt(k).m_A_Line.DrawSprite(m_pDevice,(10-m_Blast.ElementAt(k).m_A_Line.m_nCurrentFrame)*0.1f);
						m_Blast.ElementAt(k).m_A_Line.m_nPosX -= 21;

						m_Blast.ElementAt(k).m_A_Line.m_nFrameWidth = CHESSWIDTH;
						m_Blast.ElementAt(k).m_A_Line.m_nFrameHeight = CHESSHEIGHT/2+3;
						m_Blast.ElementAt(k).m_A_Line.m_nPosY += 21;
						m_Blast.ElementAt(k).m_A_Line.SetTexture(m_line_Texture[RandNumber(5,10)]);
						m_Blast.ElementAt(k).m_A_Line.DrawSprite(m_pDevice,(10-m_Blast.ElementAt(k).m_A_Line.m_nCurrentFrame)*0.1f);
						m_Blast.ElementAt(k).m_A_Line.m_nPosY -= 21;
						//2
						m_Blast.ElementAt(k).m_A_Line.m_nFrameWidth = CHESSWIDTH/2+1;
						m_Blast.ElementAt(k).m_A_Line.m_nFrameHeight = CHESSHEIGHT;
						m_Blast.ElementAt(k).m_A_Line.m_nPosX += 21;
						m_Blast.ElementAt(k).m_A_Line.SetTexture(m_line_Texture[RandNumber(0,5)]);
						m_Blast.ElementAt(k).m_A_Line.DrawSprite(m_pDevice,(10-m_Blast.ElementAt(k).m_A_Line.m_nCurrentFrame)*0.1f);
						m_Blast.ElementAt(k).m_A_Line.m_nPosX -= 21;

						m_Blast.ElementAt(k).m_A_Line.m_nFrameWidth = CHESSWIDTH;
						m_Blast.ElementAt(k).m_A_Line.m_nFrameHeight = CHESSHEIGHT/2+3;
						m_Blast.ElementAt(k).m_A_Line.m_nPosY += 21;
						m_Blast.ElementAt(k).m_A_Line.SetTexture(m_line_Texture[RandNumber(5,10)]);
						m_Blast.ElementAt(k).m_A_Line.DrawSprite(m_pDevice,(10-m_Blast.ElementAt(k).m_A_Line.m_nCurrentFrame)*0.1f);
						m_Blast.ElementAt(k).m_A_Line.m_nPosY -= 21;
						//3
						m_Blast.ElementAt(k).m_A_Line.m_nFrameWidth = CHESSWIDTH/2+1;
						m_Blast.ElementAt(k).m_A_Line.m_nFrameHeight = CHESSHEIGHT;
						m_Blast.ElementAt(k).m_A_Line.m_nPosX += 21;
						m_Blast.ElementAt(k).m_A_Line.SetTexture(m_line_Texture[RandNumber(0,5)]);
						m_Blast.ElementAt(k).m_A_Line.DrawSprite(m_pDevice,(10-m_Blast.ElementAt(k).m_A_Line.m_nCurrentFrame)*0.1f);
						m_Blast.ElementAt(k).m_A_Line.m_nPosX -= 21;

						m_Blast.ElementAt(k).m_A_Line.m_nFrameWidth = CHESSWIDTH;
						m_Blast.ElementAt(k).m_A_Line.m_nFrameHeight = CHESSHEIGHT/2+3;
						m_Blast.ElementAt(k).m_A_Line.m_nPosY += 21;
						m_Blast.ElementAt(k).m_A_Line.SetTexture(m_line_Texture[RandNumber(5,10)]);
						m_Blast.ElementAt(k).m_A_Line.DrawSprite(m_pDevice,(10-m_Blast.ElementAt(k).m_A_Line.m_nCurrentFrame)*0.1f);
						m_Blast.ElementAt(k).m_A_Line.m_nPosY -= 21;
					}break;
				case 4:
					{
						//1
						m_Blast.ElementAt(k).m_A_Line.m_nFrameWidth = CHESSWIDTH/2+1;
						m_Blast.ElementAt(k).m_A_Line.m_nFrameHeight = CHESSHEIGHT;
						m_Blast.ElementAt(k).m_A_Line.SetTexture(m_line_Texture[RandNumber(0,5)]);
						m_Blast.ElementAt(k).m_A_Line.DrawSprite(m_pDevice,(10-m_Blast.ElementAt(k).m_A_Line.m_nCurrentFrame)*0.1f);
						
						m_Blast.ElementAt(k).m_A_Line.m_nFrameWidth = CHESSWIDTH;
						m_Blast.ElementAt(k).m_A_Line.m_nFrameHeight = CHESSHEIGHT/2+3;
						m_Blast.ElementAt(k).m_A_Line.SetTexture(m_line_Texture[RandNumber(5,10)]);
						m_Blast.ElementAt(k).m_A_Line.DrawSprite(m_pDevice,(10-m_Blast.ElementAt(k).m_A_Line.m_nCurrentFrame)*0.1f);
						//2
						m_Blast.ElementAt(k).m_A_Line.m_nFrameWidth = CHESSWIDTH/2+1;
						m_Blast.ElementAt(k).m_A_Line.m_nFrameHeight = CHESSHEIGHT;
						m_Blast.ElementAt(k).m_A_Line.SetTexture(m_line_Texture[RandNumber(0,5)]);
						m_Blast.ElementAt(k).m_A_Line.DrawSprite(m_pDevice,(10-m_Blast.ElementAt(k).m_A_Line.m_nCurrentFrame)*0.1f);
						
						m_Blast.ElementAt(k).m_A_Line.m_nFrameWidth = CHESSWIDTH;
						m_Blast.ElementAt(k).m_A_Line.m_nFrameHeight = CHESSHEIGHT/2+3;
						m_Blast.ElementAt(k).m_A_Line.SetTexture(m_line_Texture[RandNumber(5,10)]);
						m_Blast.ElementAt(k).m_A_Line.DrawSprite(m_pDevice,(10-m_Blast.ElementAt(k).m_A_Line.m_nCurrentFrame)*0.1f);
						//3
						m_Blast.ElementAt(k).m_A_Line.m_nFrameWidth = CHESSWIDTH/2+1;
						m_Blast.ElementAt(k).m_A_Line.m_nFrameHeight = CHESSHEIGHT;
						m_Blast.ElementAt(k).m_A_Line.SetTexture(m_line_Texture[RandNumber(0,5)]);
						m_Blast.ElementAt(k).m_A_Line.DrawSprite(m_pDevice,(10-m_Blast.ElementAt(k).m_A_Line.m_nCurrentFrame)*0.1f);
						
						m_Blast.ElementAt(k).m_A_Line.m_nFrameWidth = CHESSWIDTH;
						m_Blast.ElementAt(k).m_A_Line.m_nFrameHeight = CHESSHEIGHT/2+3;
						m_Blast.ElementAt(k).m_A_Line.SetTexture(m_line_Texture[RandNumber(5,10)]);
						m_Blast.ElementAt(k).m_A_Line.DrawSprite(m_pDevice,(10-m_Blast.ElementAt(k).m_A_Line.m_nCurrentFrame)*0.1f);
					};
					break;
				case 5:
					{
						//1
						m_Blast.ElementAt(k).m_A_Line.m_nFrameWidth = CHESSWIDTH/2+1;
						m_Blast.ElementAt(k).m_A_Line.m_nFrameHeight = CHESSHEIGHT;
						m_Blast.ElementAt(k).m_A_Line.m_nPosX += 21;
						m_Blast.ElementAt(k).m_A_Line.SetTexture(m_line_Texture[RandNumber(0,5)]);
						m_Blast.ElementAt(k).m_A_Line.DrawSprite(m_pDevice,(10-m_Blast.ElementAt(k).m_A_Line.m_nCurrentFrame)*0.1f);
						m_Blast.ElementAt(k).m_A_Line.m_nPosX -= 21;

						m_Blast.ElementAt(k).m_A_Line.m_nFrameWidth = CHESSWIDTH;
						m_Blast.ElementAt(k).m_A_Line.m_nFrameHeight = CHESSHEIGHT/2+3;
						m_Blast.ElementAt(k).m_A_Line.SetTexture(m_line_Texture[RandNumber(5,10)]);
						m_Blast.ElementAt(k).m_A_Line.DrawSprite(m_pDevice,(10-m_Blast.ElementAt(k).m_A_Line.m_nCurrentFrame)*0.1f);
						//2
						m_Blast.ElementAt(k).m_A_Line.m_nFrameWidth = CHESSWIDTH/2+1;
						m_Blast.ElementAt(k).m_A_Line.m_nFrameHeight = CHESSHEIGHT;
						m_Blast.ElementAt(k).m_A_Line.m_nPosX += 21;
						m_Blast.ElementAt(k).m_A_Line.SetTexture(m_line_Texture[RandNumber(0,5)]);
						m_Blast.ElementAt(k).m_A_Line.DrawSprite(m_pDevice,(10-m_Blast.ElementAt(k).m_A_Line.m_nCurrentFrame)*0.1f);
						m_Blast.ElementAt(k).m_A_Line.m_nPosX -= 21;

						m_Blast.ElementAt(k).m_A_Line.m_nFrameWidth = CHESSWIDTH;
						m_Blast.ElementAt(k).m_A_Line.m_nFrameHeight = CHESSHEIGHT/2+3;
						m_Blast.ElementAt(k).m_A_Line.SetTexture(m_line_Texture[RandNumber(5,10)]);
						m_Blast.ElementAt(k).m_A_Line.DrawSprite(m_pDevice,(10-m_Blast.ElementAt(k).m_A_Line.m_nCurrentFrame)*0.1f);
						//3
						m_Blast.ElementAt(k).m_A_Line.m_nFrameWidth = CHESSWIDTH/2+1;
						m_Blast.ElementAt(k).m_A_Line.m_nFrameHeight = CHESSHEIGHT;
						m_Blast.ElementAt(k).m_A_Line.m_nPosX += 21;
						m_Blast.ElementAt(k).m_A_Line.SetTexture(m_line_Texture[RandNumber(0,5)]);
						m_Blast.ElementAt(k).m_A_Line.DrawSprite(m_pDevice,(10-m_Blast.ElementAt(k).m_A_Line.m_nCurrentFrame)*0.1f);
						m_Blast.ElementAt(k).m_A_Line.m_nPosX -= 21;

						m_Blast.ElementAt(k).m_A_Line.m_nFrameWidth = CHESSWIDTH;
						m_Blast.ElementAt(k).m_A_Line.m_nFrameHeight = CHESSHEIGHT/2+3;
						m_Blast.ElementAt(k).m_A_Line.SetTexture(m_line_Texture[RandNumber(5,10)]);
						m_Blast.ElementAt(k).m_A_Line.DrawSprite(m_pDevice,(10-m_Blast.ElementAt(k).m_A_Line.m_nCurrentFrame)*0.1f);
					};
					break;
				default:
					break;
				}
				m_Blast.ElementAt(k).m_A_Line.m_nFrameWidth = CHESSWIDTH;
				m_Blast.ElementAt(k).m_A_Line.m_nFrameHeight = CHESSHEIGHT;
			}
		}
		if(m_Blast.ElementAt(k).m_A_Link.m_bActive == false)
		{
			m_Blast.RemoveAt(k);
		}
	}
}


void CD3DWnd::Render(void)
{
	HRESULT hr;

	//先清除后备缓冲区
	m_pDevice->Clear(0,NULL,D3DCLEAR_TARGET,D3DCOLOR_XRGB(0,0,0),1.0f,0);
	static char buffer[255];

	static float  g_fElpasedTime=0;
	static float  g_fMoveSpeed = 50.0f;
	static float fPosition = 0.0f;
	static DWORD tttt=0;

	//
	TFPS++;
	DWORD t = GetTickCount();
	if(tttt!=0)
        g_fElpasedTime =  (t-tttt)*0.0001f;
	if ( t-fTimerFPS>=1000)
	{
		fTimerFPS = t;
		FPS=TFPS;
		TFPS=0;
	}
	tttt=t;
	sprintf(buffer,"FPS: %ld",FPS);

	//SetM();
	
m_pDevice->BeginScene();
	//绘制表面
	blitToSurface(m_pSurface,CRect(0,0,788,768),CRect(0,0,788,768));

	if(!(m_bGameOver || m_bGameLose[2]))
	{
		//爆炸效果
		MakeBlast();
	}
	else
	{
		m_Blast.RemoveAll();
	}
	

	//刷新频率
	//ShowFPS(buffer);
	//界面信息
	DrawGameInfo();

	m_PointSprite_Star.DrawPointSprite();
m_pDevice->EndScene();
	//------------------------------------
	//控制动画和释放动画资源
	static float fAnimationTimer = 0.0f;
	static float fLinkTimer = 0.0f;
	fAnimationTimer += g_fElpasedTime;
	fLinkTimer  += g_fElpasedTime;

	//爆炸动画
	if( fAnimationTimer > 0.003f )
	{
		for(int k=0; k<m_Blast.GetCount(); ++k)
		{
			m_Blast.ElementAt(k).m_A_Line.LoopIt();
			m_Blast.ElementAt(k).m_A_Link.LoopIt();
		}
		fAnimationTimer = 0.0f;
	}
	//定时器响应
	if( m_bCanGame && fLinkTimer >= 0.05f && m_nTimeCounter <= 30)
	{
		++m_nTimeCounter;
		fLinkTimer = 0.0f;
		StepItTimer();
		::SendMessage(AfxGetMainWnd()->m_hWnd,IDM_D3D_TIMER,m_nTimeCounter,0);
	}
	//--------------------------------------
	m_pDevice->Present(NULL,NULL,NULL,NULL);

        if( FAILED( hr = m_pDevice->TestCooperativeLevel()))
        {
			Sleep( 100 ); 
            if( hr == D3DERR_DEVICELOST )
                return;
            if( hr == D3DERR_DEVICENOTRESET )
            {
                Cleanup_();
				HRESULT rslt;

				m_pD3D = Direct3DCreate9( D3D_SDK_VERSION );

				//D3DCAPS9 d3dCaps;
				// m_pD3D->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &d3dCaps );

				rslt = m_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &D3DDisplayMode );
				if(FAILED(rslt)) {MessageBox(TEXT("获得显示适配器模式失败!"),"",MB_OK);return;}

				ZeroMemory( &d3dpp, sizeof(d3dpp) );

				d3dpp.Windowed         = TRUE;
				d3dpp.SwapEffect       = D3DSWAPEFFECT_DISCARD;
				d3dpp.BackBufferFormat = D3DDisplayMode.Format;

				d3dpp.EnableAutoDepthStencil = FALSE;
				d3dpp.AutoDepthStencilFormat = D3DFMT_UNKNOWN;
				//d3dpp.PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE; // Do NOT sync to vertical retrace
				d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
				d3dpp.PresentationInterval   = D3DPRESENT_INTERVAL_DEFAULT; // Sync to vertical retrace
				d3dpp.Flags                  = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

				//创建DirectX9驱动
				rslt = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd,
					D3DCREATE_SOFTWARE_VERTEXPROCESSING,
					&d3dpp, &m_pDevice );
				if(FAILED(rslt))
				{
					rslt = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, m_hWnd,
						D3DCREATE_SOFTWARE_VERTEXPROCESSING,
						&d3dpp, &m_pDevice );

					if(FAILED(rslt))
					{
						if(MessageBox(TEXT("亲爱的用户：\r\n运行连连看的时候，创建DirectX9驱动失败!\r\n您的电脑可能没有安装DirectX9.0c,必须安装之后才能运行游戏！\r\n是否立即安装？"),"",MB_OKCANCEL) == IDOK)
						{
							m_Logic.OpenUrl("http://www.cngame.com/down/directx_9c_oct04sdk_redist.exe");
						}
						return;
					}
				}

				/*
				//创建参数

				::ZeroMemory(&d3dpp,sizeof(d3dpp));
				d3dpp.Windowed = TRUE;
				d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;
				d3dpp.EnableAutoDepthStencil = false;
				d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
				d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8; //D3DDisplayMode.Format;
				d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
				d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

				//创建驱动
				rslt = m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,m_hWnd,D3DCREATE_SOFTWARE_VERTEXPROCESSING,&d3dpp,
				&m_pDevice);
				if(FAILED(rslt)) {MessageBox(TEXT("创建DirectX驱动失败!"),"",MB_OK);return;}
				*/

				//----------------------------
				D3DXMATRIX matView;
				D3DXMatrixLookAtLH( &matView, &D3DXVECTOR3(0.0f, 0.0f,-25.0f), 
					&D3DXVECTOR3(0.0f, 0.0f, 0.0f), 
					&D3DXVECTOR3(0.0f, 1.0f, 0.0f ) );
				m_pDevice->SetTransform( D3DTS_VIEW, &matView );

				D3DXMATRIX matProj;
				D3DXMatrixPerspectiveFovLH( &matProj, D3DXToRadian( 45.0f ), 1.0f, 1.0f, 100.0f );
				m_pDevice->SetTransform( D3DTS_PROJECTION, &matProj );

				m_pDevice->SetRenderState( D3DRS_LIGHTING,  FALSE );

                Geometry();
				UpdateBackSurface();
            }

            return;
        }
}

void CD3DWnd::OnPaint()
{
	CPaintDC dc(this);
	Render();

}

// 释放资源
void CD3DWnd::Cleanup(void)
{
	keyi = FALSE;
	if (m_hRenderThread!=NULL)
	{
	//::WaitForSingleObject(m_hRenderThread,0xFFFFFFFF);
	CloseHandle(m_hRenderThread);
	}

	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pD3D);
	SAFE_RELEASE(m_p2DFont);
	for(int i=0; i<46; i++)
	SAFE_RELEASE(m_link_Texture[i]);
	for(int i=0; i<10; i++)
	SAFE_RELEASE(m_line_Texture[i]);
	SAFE_RELEASE(m_ready_Texture);
	SAFE_RELEASE(m_StopIt_Texture);
	SAFE_RELEASE(m_Select_Texture);
	SAFE_RELEASE(m_Number_Texture);
	SAFE_RELEASE(m_Lose_Texture);
	SAFE_RELEASE(m_Win_Texture);
	SAFE_RELEASE(m_ScoreFrame_Texture);
	SAFE_RELEASE(m_SmallCry_Texture);
	SAFE_RELEASE(m_Cry_Texture);


	SAFE_RELEASE(m_pBkSurface);
	SAFE_RELEASE(m_pLoseSurface);
	SAFE_RELEASE(m_pSurface);	
	SAFE_RELEASE(m_pChess_Surface)
	SAFE_RELEASE(m_pChessFace_Surface)
	SAFE_RELEASE(m_pChessFaceSmall_Surface)
	SAFE_RELEASE(m_pTimerBK_Surface);
	SAFE_RELEASE(m_pTimer_Surface);
	SAFE_RELEASE(m_pChessFaceProp_Surface);

	///
	m_Blast.RemoveAll();
}

// 释放资源
void CD3DWnd::Cleanup_(void)
{
		
	SAFE_RELEASE(m_p2DFont);
	SAFE_RELEASE(m_p2DFontBig);
	for(int i=0; i<46; i++)
	SAFE_RELEASE(m_link_Texture[i]);
	for(int i=0; i<10; i++)
	SAFE_RELEASE(m_line_Texture[i]);
	SAFE_RELEASE(m_ready_Texture);
	SAFE_RELEASE(m_StopIt_Texture);
	SAFE_RELEASE(m_Select_Texture);
	SAFE_RELEASE(m_Number_Texture);
	SAFE_RELEASE(m_Lose_Texture);
	SAFE_RELEASE(m_Win_Texture);
	SAFE_RELEASE(m_ScoreFrame_Texture);
	SAFE_RELEASE(m_SmallCry_Texture);
	SAFE_RELEASE(m_Cry_Texture);


	SAFE_RELEASE(m_pBkSurface);
	SAFE_RELEASE(m_pLoseSurface);
	SAFE_RELEASE(m_pSurface);	
	SAFE_RELEASE(m_pChess_Surface)
	SAFE_RELEASE(m_pChessFace_Surface)
	SAFE_RELEASE(m_pChessFaceSmall_Surface)
	SAFE_RELEASE(m_pTimerBK_Surface);
	SAFE_RELEASE(m_pTimer_Surface);
	SAFE_RELEASE(m_pChessFaceProp_Surface);

	///
	m_Blast.RemoveAll();
}

void CD3DWnd::OnDestroy()
{
	CWnd::OnDestroy();

	Cleanup();
}

// 建模
void CD3DWnd::Geometry(void)
{
	HRESULT rslt;
	//创建2维文字字体
	D3DXCreateFont( m_pDevice, 14, 0, 0/*FW_BOLD*/, 0, FALSE, 
         DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 
         DEFAULT_PITCH , TEXT("宋体"), 
         &m_p2DFont );
	D3DXCreateFont( m_pDevice, 18, 0, FW_BOLD, 0, FALSE, 
         DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 
         DEFAULT_PITCH , TEXT("宋体"),
         &m_p2DFontBig );

	D3DXIMAGE_INFO d3dxImageInfo;

	srand((unsigned int)time(NULL));	

	//创建离屏表面
	m_pDevice->CreateOffscreenPlainSurface(788, 768, D3DDisplayMode.Format, D3DPOOL_DEFAULT, &m_pSurface, NULL);
	m_pDevice->CreateOffscreenPlainSurface(788, 768, D3DDisplayMode.Format, D3DPOOL_DEFAULT, &m_pBkSurface, NULL);

	m_pChess_Surface = getSurfaceFromResource(IDB_CHESS,100,53);
	m_pChessFace_Surface = getSurfaceFromResource(IDB_CHESS_FACE,1176,47);
	m_pChessFaceProp_Surface = getSurfaceFromResource(IDB_CHESS_PROP,1176,47);
	m_pChessFaceSmall_Surface = getSurfaceFromResource(IDT_CHESS_FACE_SMALL,250,11);
	m_pTimerBK_Surface = getSurfaceFromResource(IDB_TIMER_BK,415,21);
	m_pTimer_Surface = getSurfaceFromResource(IDB_TIMER,415,21);

	for(int i=0; i<10; i++)
	{
		rslt = D3DXCreateTextureFromResourceEx(m_pDevice, GetModuleHandle( NULL ), MAKEINTRESOURCE((IDT_LINK001+i)),
                                      42,
									  47,
									  1,
									  0,
									  D3DFMT_UNKNOWN, 
									  D3DPOOL_MANAGED,
									  D3DX_DEFAULT,
									  D3DX_DEFAULT,
                                      D3DCOLOR_COLORVALUE(0.0f,0.0f,0.0f,1.0f),
									  &d3dxImageInfo,
									  NULL, 
									  &m_line_Texture[i]);

		if(FAILED(rslt)) {MessageBox(TEXT("图片加载失败!"),"",MB_OK);return;}
	}

	for(int i=0; i<BLASTFRAMECOUNT; i++)
	{
		rslt = D3DXCreateTextureFromResourceEx(m_pDevice, GetModuleHandle( NULL ), MAKEINTRESOURCE((IDT_BLAST001+i)),
                                      60, 
									  61, 
									  1,
									  0,
                                      D3DFMT_UNKNOWN, 
									  D3DPOOL_MANAGED,
									  D3DX_DEFAULT,
									  D3DX_DEFAULT,
                                      D3DCOLOR_COLORVALUE(0.0f,0.0f,0.0f,1.0f),
									  &d3dxImageInfo,
									  NULL, 
									  &m_link_Texture[i]);
		if(FAILED(rslt)) {MessageBox(TEXT("图片加载失败!"),"",MB_OK);return;}
	}
	
	D3DXCreateTextureFromResourceEx(m_pDevice, GetModuleHandle( NULL ), MAKEINTRESOURCE(IDT_READY),
                                      176, 
									  50, 
									  1,
									  0,
                                      D3DFMT_UNKNOWN,
									  D3DPOOL_MANAGED,
									  D3DX_DEFAULT, 
									  D3DX_DEFAULT,
                                      D3DCOLOR_COLORVALUE(0.0f,0.0f,0.0f,1.0f),
									  &d3dxImageInfo,
									  NULL, 
									  &m_ready_Texture);
	D3DXCreateTextureFromResourceEx(m_pDevice, GetModuleHandle( NULL ), MAKEINTRESOURCE(IDT_STOPIT),
                                      323, 
									  420, 
									  1,
									  0,
                                      D3DFMT_UNKNOWN, 
									  D3DPOOL_MANAGED,
									  D3DX_DEFAULT, 
									  D3DX_DEFAULT,
                                      D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),
									  &d3dxImageInfo,
									  NULL, 
									  &m_StopIt_Texture);
	D3DXCreateTextureFromResourceEx(m_pDevice, GetModuleHandle( NULL ), MAKEINTRESOURCE(IDT_CRY),
                                      318, 
									  416, 
									  1,
									  0,
                                      D3DFMT_UNKNOWN, 
									  D3DPOOL_MANAGED,
									  D3DX_DEFAULT, 
									  D3DX_DEFAULT,
                                      D3DCOLOR_COLORVALUE(0.0f,0.0f,0.0f,1.0f),
									  &d3dxImageInfo,
									  NULL, 
									  &m_Cry_Texture);

	D3DXCreateTextureFromResourceEx(m_pDevice, GetModuleHandle( NULL ), MAKEINTRESOURCE(IDT_SMALLCRY),
                                      80, 
									  105, 
									  1,
									  0,
                                      D3DFMT_UNKNOWN, 
									  D3DPOOL_MANAGED,
									  D3DX_DEFAULT, 
									  D3DX_DEFAULT,
                                      D3DCOLOR_COLORVALUE(0.0f,0.0f,0.0f,1.0f),
									  &d3dxImageInfo,
									  NULL, 
									  &m_SmallCry_Texture);

	D3DXCreateTextureFromResourceEx(m_pDevice, GetModuleHandle( NULL ), MAKEINTRESOURCE(IDB_FRAME_CHOOSE),
                                      43, 
									  47, 
									  1,
									  0,
                                      D3DFMT_UNKNOWN, 
									  D3DPOOL_MANAGED,
									  D3DX_DEFAULT, 
									  D3DX_DEFAULT,
                                      D3DCOLOR_COLORVALUE(0.0f,0.0f,0.0f,1.0f),
									  &d3dxImageInfo,
									  NULL, 
									  &m_Select_Texture);
	
    D3DXCreateTextureFromResourceEx(m_pDevice, GetModuleHandle( NULL ), MAKEINTRESOURCE(IDT_NUMBER),
                                      228, 
									  65, 
									  1,
									  0,
                                      D3DFMT_UNKNOWN, 
							 		  D3DPOOL_MANAGED,
									  D3DX_DEFAULT, 
									  D3DX_DEFAULT,
                                      D3DCOLOR_COLORVALUE(0.0f,0.0f,0.0f,1.0f),
									  &d3dxImageInfo,
									  NULL, 
									  &m_Number_Texture);
	D3DXCreateTextureFromResourceEx(m_pDevice, GetModuleHandle( NULL ), MAKEINTRESOURCE(IDT_LOSE),
                                      204, 
									  58, 
									  1,
									  0,
                                      D3DFMT_UNKNOWN, 
									  D3DPOOL_MANAGED,
									  D3DX_DEFAULT, 
									  D3DX_DEFAULT,
                                      D3DCOLOR_COLORVALUE(0.0f,0.0f,0.0f,1.0f),
									  &d3dxImageInfo,
									  NULL, 
									  &m_Lose_Texture);
	D3DXCreateTextureFromResourceEx(m_pDevice, GetModuleHandle( NULL ), MAKEINTRESOURCE(IDT_WIN),
                                      165, 
									  58, 
									  1,
									  0,
                                      D3DFMT_UNKNOWN, 
									  D3DPOOL_MANAGED,
									  D3DX_DEFAULT, 
									  D3DX_DEFAULT,
                                      D3DCOLOR_COLORVALUE(0.0f,0.0f,0.0f,1.0f),
									  &d3dxImageInfo,
									  NULL, 
									  &m_Win_Texture);
	D3DXCreateTextureFromResourceEx(m_pDevice, GetModuleHandle( NULL ), MAKEINTRESOURCE(IDT_SCORE),
                                      159, 
									  56, 
									  1,
									  0,
                                      D3DFMT_UNKNOWN, 
									  D3DPOOL_MANAGED,
									  D3DX_DEFAULT, 
									  D3DX_DEFAULT,
                                      D3DCOLOR_COLORVALUE(0.0f,0.0f,0.0f,1.0f),
									  &d3dxImageInfo,
									  NULL, 
									  &m_ScoreFrame_Texture);

	//m_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	//m_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
	m_pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	m_pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

	//设置定点格式
	m_pDevice->SetFVF(D3DFVF_CUSTOMVERTEX);	

	AccountChessCount();

	//------------------------------------------------
	
	//ready

	m_Sprite_ready.zeroSpriteValues();
	m_Sprite_ready.m_bActive = true;
	strncpy(m_Sprite_ready.m_sType ,"ready",sizeof(m_Sprite_ready.m_sType));
	m_Sprite_ready.m_bAutoLoop = false;
	m_Sprite_ready.m_bSingleFrame = true;
	m_Sprite_ready.m_nFrameWidth = 176;
	m_Sprite_ready.m_nFrameHeight = 50;
	m_Sprite_ready.m_nCurrentFrame = 0;
	m_Sprite_ready.m_nSrcPosX = 0;
	m_Sprite_ready.m_nSrcPosY = 0;
	m_Sprite_ready.m_nPosX = 0;
	m_Sprite_ready.m_nPosY = 0;
	m_Sprite_ready.m_nFrameBegin = 0;
	m_Sprite_ready.m_nFrameEnd = 1;
	m_Sprite_ready.m_nWidthCount = 1;
	m_Sprite_ready.m_nFrameRate = 0;
	m_Sprite_ready.m_nFrameCount = 1;
	m_Sprite_ready.SetTexture(m_ready_Texture);

	//鸭子
	m_Sprite_StopIt.zeroSpriteValues();
	m_Sprite_StopIt.m_bActive = true;
	strncpy(m_Sprite_StopIt.m_sType ,"鸭子笑",sizeof(m_Sprite_StopIt.m_sType));
	m_Sprite_StopIt.m_bAutoLoop = false;
	m_Sprite_StopIt.m_bSingleFrame = true;
	m_Sprite_StopIt.m_nFrameWidth = 357;
	m_Sprite_StopIt.m_nFrameHeight = 420;
	m_Sprite_StopIt.m_nCurrentFrame = 0;
	m_Sprite_StopIt.m_nSrcPosX = 0;
	m_Sprite_StopIt.m_nSrcPosY = 0;
	m_Sprite_StopIt.m_nPosX = 0;
	m_Sprite_StopIt.m_nPosY = 0;
	m_Sprite_StopIt.m_nFrameBegin = 0;
	m_Sprite_StopIt.m_nFrameEnd = 1;
	m_Sprite_StopIt.m_nWidthCount = 1;
	m_Sprite_StopIt.m_nFrameRate = 0;
	m_Sprite_StopIt.m_nFrameCount = 1;
	m_Sprite_StopIt.SetTexture(m_StopIt_Texture);

	//鸭子
	m_Sprite_Cry.zeroSpriteValues();
	m_Sprite_Cry.m_bActive = true;
	strncpy(m_Sprite_Cry.m_sType ,"鸭子哭",sizeof(m_Sprite_Cry.m_sType));
	m_Sprite_Cry.m_bAutoLoop = false;
	m_Sprite_Cry.m_bSingleFrame = true;
	m_Sprite_Cry.m_nFrameWidth = 318;
	m_Sprite_Cry.m_nFrameHeight = 416;
	m_Sprite_Cry.m_nCurrentFrame = 0;
	m_Sprite_Cry.m_nSrcPosX = 0;
	m_Sprite_Cry.m_nSrcPosY = 0;
	m_Sprite_Cry.m_nPosX = 0;
	m_Sprite_Cry.m_nPosY = 0;
	m_Sprite_Cry.m_nFrameBegin = 0;
	m_Sprite_Cry.m_nFrameEnd = 1;
	m_Sprite_Cry.m_nWidthCount = 1;
	m_Sprite_Cry.m_nFrameRate = 0;
	m_Sprite_Cry.m_nFrameCount = 1;
	m_Sprite_Cry.SetTexture(m_Cry_Texture);

	//鸭子
	m_Sprite_SmallCry.zeroSpriteValues();
	m_Sprite_SmallCry.m_bActive = true;
	strncpy(m_Sprite_SmallCry.m_sType ,"小鸭子哭",sizeof(m_Sprite_SmallCry.m_sType));
	m_Sprite_SmallCry.m_bAutoLoop = false;
	m_Sprite_SmallCry.m_bSingleFrame = true;
	m_Sprite_SmallCry.m_nFrameWidth = 80;
	m_Sprite_SmallCry.m_nFrameHeight = 105;
	m_Sprite_SmallCry.m_nCurrentFrame = 0;
	m_Sprite_SmallCry.m_nSrcPosX = 0;
	m_Sprite_SmallCry.m_nSrcPosY = 0;
	m_Sprite_SmallCry.m_nPosX = 0;
	m_Sprite_SmallCry.m_nPosY = 0;
	m_Sprite_SmallCry.m_nFrameBegin = 0;
	m_Sprite_SmallCry.m_nFrameEnd = 1;
	m_Sprite_SmallCry.m_nWidthCount = 1;
	m_Sprite_SmallCry.m_nFrameRate = 0;
	m_Sprite_SmallCry.m_nFrameCount = 1;
	m_Sprite_SmallCry.SetTexture(m_SmallCry_Texture);
	

	// 选择框
	m_Sprite_Select.zeroSpriteValues();
	m_Sprite_Select.m_bActive = true;
	strncpy(m_Sprite_Select.m_sType ,"选择框",sizeof(m_Sprite_Select.m_sType));
	m_Sprite_Select.m_bAutoLoop = false;
	m_Sprite_Select.m_bSingleFrame = true;
	m_Sprite_Select.m_nFrameWidth = 43;
	m_Sprite_Select.m_nFrameHeight = 47;
	m_Sprite_Select.m_nCurrentFrame = 0;
	m_Sprite_Select.m_nSrcPosX = 0;
	m_Sprite_Select.m_nSrcPosY = 0;
	m_Sprite_Select.m_nPosX = 0;
	m_Sprite_Select.m_nPosY = 0;
	m_Sprite_Select.m_nFrameBegin = 0;
	m_Sprite_Select.m_nFrameEnd = 1;
	m_Sprite_Select.m_nWidthCount = 1;
	m_Sprite_Select.m_nFrameRate = 0;
	m_Sprite_Select.m_nFrameCount = 1;
	m_Sprite_Select.SetTexture(m_Select_Texture);

	// 名次
	m_Sprite_Number.zeroSpriteValues();
	m_Sprite_Number.m_bActive = true;
	strncpy(m_Sprite_Number.m_sType ,"名次",sizeof(m_Sprite_Number.m_sType));
	m_Sprite_Number.m_bAutoLoop = false;
	m_Sprite_Number.m_bSingleFrame = true;
	m_Sprite_Number.m_nFrameWidth = 57;
	m_Sprite_Number.m_nFrameHeight = 65;
	m_Sprite_Number.m_nCurrentFrame = 0;
	m_Sprite_Number.m_nSrcPosX = 0;
	m_Sprite_Number.m_nSrcPosY = 0;
	m_Sprite_Number.m_nPosX = 0;
	m_Sprite_Number.m_nPosY = 0;
	m_Sprite_Number.m_nFrameBegin = 0;
	m_Sprite_Number.m_nFrameEnd = 1;
	m_Sprite_Number.m_nWidthCount = 4;
	m_Sprite_Number.m_nFrameRate = 0;
	m_Sprite_Number.m_nFrameCount = 4;
	m_Sprite_Number.SetTexture(m_Number_Texture);

	m_Sprite_ScoreFrame.zeroSpriteValues();
	m_Sprite_ScoreFrame.m_bActive = true;
	strncpy(m_Sprite_ScoreFrame.m_sType ,"分数框",sizeof(m_Sprite_ScoreFrame.m_sType));
	m_Sprite_ScoreFrame.m_bAutoLoop = false;
	m_Sprite_ScoreFrame.m_bSingleFrame = true;
	m_Sprite_ScoreFrame.m_nFrameWidth = 159;
	m_Sprite_ScoreFrame.m_nFrameHeight = 56;
	m_Sprite_ScoreFrame.m_nCurrentFrame = 0;
	m_Sprite_ScoreFrame.m_nSrcPosX = 0;
	m_Sprite_ScoreFrame.m_nSrcPosY = 0;
	m_Sprite_ScoreFrame.m_nPosX = 0;
	m_Sprite_ScoreFrame.m_nPosY = 0;
	m_Sprite_ScoreFrame.m_nFrameBegin = 0;
	m_Sprite_ScoreFrame.m_nFrameEnd = 1;
	m_Sprite_ScoreFrame.m_nWidthCount = 1;
	m_Sprite_ScoreFrame.m_nFrameRate = 0;
	m_Sprite_ScoreFrame.m_nFrameCount = 1;
	m_Sprite_ScoreFrame.SetTexture(m_ScoreFrame_Texture);

	m_Sprite_Win.zeroSpriteValues();
	m_Sprite_Win.m_bActive = true;
	strncpy(m_Sprite_Win.m_sType ,"赢",sizeof(m_Sprite_Win.m_sType));
	m_Sprite_Win.m_bAutoLoop = false;
	m_Sprite_Win.m_bSingleFrame = true;
	m_Sprite_Win.m_nFrameWidth = 165;
	m_Sprite_Win.m_nFrameHeight = 58;
	m_Sprite_Win.m_nCurrentFrame = 0;
	m_Sprite_Win.m_nSrcPosX = 0;
	m_Sprite_Win.m_nSrcPosY = 0;
	m_Sprite_Win.m_nPosX = 0;
	m_Sprite_Win.m_nPosY = 0;
	m_Sprite_Win.m_nFrameBegin = 0;
	m_Sprite_Win.m_nFrameEnd = 1;
	m_Sprite_Win.m_nWidthCount = 4;
	m_Sprite_Win.m_nFrameRate = 0;
	m_Sprite_Win.m_nFrameCount = 4;
	m_Sprite_Win.SetTexture(m_Win_Texture);

	m_Sprite_Lose.zeroSpriteValues();
	m_Sprite_Lose.m_bActive = true;
	strncpy(m_Sprite_Lose.m_sType ,"输",sizeof(m_Sprite_Lose.m_sType));
	m_Sprite_Lose.m_bAutoLoop = false;
	m_Sprite_Lose.m_bSingleFrame = true;
	m_Sprite_Lose.m_nFrameWidth = 204;
	m_Sprite_Lose.m_nFrameHeight = 58;
	m_Sprite_Lose.m_nCurrentFrame = 0;
	m_Sprite_Lose.m_nSrcPosX = 0;
	m_Sprite_Lose.m_nSrcPosY = 0;
	m_Sprite_Lose.m_nPosX = 0;
	m_Sprite_Lose.m_nPosY = 0;
	m_Sprite_Lose.m_nFrameBegin = 0;
	m_Sprite_Lose.m_nFrameEnd = 1;
	m_Sprite_Lose.m_nWidthCount = 4;
	m_Sprite_Lose.m_nFrameRate = 0;
	m_Sprite_Lose.m_nFrameCount = 4;
	m_Sprite_Lose.SetTexture(m_Lose_Texture);


	///////////////////
	m_PointSprite_Star.SetPointSprite(m_pDevice,IDB_PARTICLE);
	
}

void CD3DWnd::OnTimer(UINT nIDEvent)
{
	switch(nIDEvent)
	{
	case ID_STOPIT:
		{
			m_bStopIt = false;
			m_bCanGame = true;
		}return;
	}
	CWnd::OnTimer(nIDEvent);
}

void CD3DWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(!m_bCanGame)
		return ;
	if(point.x<CHESSFRAME_W||
				point.x>CHESSFRAME_W+CHESSCOUNT_W*CHESSWIDTH||
				point.y<CHESSFRAME_H||
				point.y>CHESSFRAME_H+CHESSCOUNT_H*CHESSHEIGHT)
				;//none
			else
			{
				int w = (point.x - CHESSFRAME_W) / CHESSWIDTH;
				int h = (point.y - CHESSFRAME_H) / CHESSHEIGHT;

				// 按键处理
				SeystrokeProcess(w,h);
			}
	switch(m_nMouseState)
	{
	case MOUSE_NORMAL:
		{
			return;
		}
	case MOUSE_STOPIT:
		{
			if(point.x > 150 && point.y>10 && point.y<180)
			{
				LONG UserID = (point.x - 150)/(206+6);
				if(UserID==2)UserID=3;
				if(m_UserData[UserID] == NULL)
				{
					return;
				}
				::PostMessage(AfxGetMainWnd()->m_hWnd,IDM_D3D_STOPIT,m_UserData[UserID]->wChairID,0);
				m_nMouseState = MOUSE_NORMAL;
			}
		}
		return;
	case MOUSE_CLOG:
		{
			if(point.x > 150 && point.y>10 && point.y<180)
			{
				
				LONG UserID = ((point.x - 150)/(206+6));
				if(UserID==2)UserID=3;
				if(m_UserData[UserID] == NULL)
				{
					return;
				}
				::PostMessage(AfxGetMainWnd()->m_hWnd, IDM_D3D_CLOG, m_UserData[UserID]->wChairID,0);
				m_nMouseState = MOUSE_NORMAL;
			}
		}
		return;
default:
		return;
	}
}

void CD3DWnd::ShowFPS(char *buffer)
{
   CRect rect(50,30,0,0);
	m_p2DFont->DrawTextA(NULL,buffer,-1,&rect,DT_CALCRECT,0);
	m_p2DFont->DrawText(NULL,
		buffer, //文本内容
		-1, //文本长度
		&rect, //文本的显示区域，使用窗口坐标
		DT_LEFT, //显示格式：左对齐
		D3DCOLOR_XRGB(255,0,0) ); //
}

void CD3DWnd::SetM(void)
{
	D3DXMATRIX matTrans;
	D3DXMATRIX matRot;
	D3DXMATRIX matWorld;

    D3DXMatrixTranslation( &matTrans, 0.0f, 0.0f, 4.0f );

	D3DXMatrixRotationYawPitchRoll( &matRot,
		                            D3DXToRadian(0),
		                            D3DXToRadian(0), 
		                            0.0f );
    matWorld = matRot * matTrans;
    m_pDevice->SetTransform( D3DTS_WORLD, &matWorld );	
}

//
int CD3DWnd::RandNumber(int min, int max)
{
	return rand()%(max-min) + min;
}

// 按键处理
bool CD3DWnd::SeystrokeProcess(int w, int h)
{
	//刚使用过指南针
	if(m_nSelectFirst.x != ERROR_ALL && m_nSelectFirst.y != ERROR_ALL&&
		m_nSelectSecond.x != ERROR_ALL && m_nSelectSecond.y != ERROR_ALL)
	{
		m_nSelectFirst.x = ERROR_ALL;
		m_nSelectFirst.y = ERROR_ALL;
		m_nSelectSecond.x = ERROR_ALL;
		m_nSelectSecond.y = ERROR_ALL;
	}

	if(m_Chess[2][h][w] != ERROR_ALL)
	{
		if(m_nSelectFirst.x == ERROR_ALL && m_nSelectFirst.y == ERROR_ALL )
		{
			//point.x 横向的个数		.y纵向的个数
			m_nSelectFirst.x = w;
			m_nSelectFirst.y = h;
			::SendMessage(AfxGetMainWnd()->m_hWnd,IDM_D3D_ALLMUSIC,IDM_MUSIC_SEL,0);
			return false;
		}
		else
		{
			m_nSelectSecond.x = w;
			m_nSelectSecond.y = h;

			//未选择
			if(m_nSelectFirst.x == m_nSelectSecond.x && m_nSelectFirst.y == m_nSelectSecond.y)
			{
				m_nSelectSecond.x = ERROR_ALL;
				m_nSelectSecond.y = ERROR_ALL;
				return false;
			}
			//已选择一个
			if(m_Chess[2][m_nSelectFirst.y][m_nSelectFirst.x] == m_Chess[2][m_nSelectSecond.y][m_nSelectSecond.x])
			{
				//是否可以连接
				if( LinkProcess(m_nSelectFirst,m_nSelectSecond,2))
				{
					if(m_bHitState)
					{
						++m_nHitCount;
						AddHitState(m_nHitCount);
						::SendMessage(this->GetParent()->m_hWnd,IDM_D3D_LINK,2,0);
					}
					else
					{
						m_nHitCount = 0;
						::SendMessage(this->GetParent()->m_hWnd,IDM_D3D_LINK,1,0);
					}
					m_bHitState = true;
				}
				else
				{
					m_bHitState = false;
					m_nHitCount = 0;
				}
			}
			else
			{
				m_bHitState = false;
				m_nHitCount = 0;
				m_nSelectFirst.x = w;
				m_nSelectFirst.y = h;
				m_nSelectSecond.x = ERROR_ALL;
				m_nSelectSecond.y = ERROR_ALL;

				::SendMessage(AfxGetMainWnd()->m_hWnd,IDM_D3D_ALLMUSIC,IDM_MUSIC_SEL,0);
				return false;
			}
		}
	}
	return false;
}

// 初始棋盘
void CD3DWnd::InitChess(void)
{
	m_bCanGame = false;
	m_bGameOver = false;
	m_ExerciseMode = false;
	memset(m_bGameLose,false,sizeof(m_bGameLose));

	m_bHitState = false;
	m_nHitCount = 0;
	m_nMouseState = MOUSE_NORMAL;

	//道具
	m_bStopIt  = false;

	m_nTimeCounter = 0;//

	m_ChessCountString[0] = 0;

	m_nSelectFirst.x = ERROR_ALL;
	m_nSelectFirst.y = ERROR_ALL;
	m_nSelectSecond.x = ERROR_ALL;
	m_nSelectSecond.y = ERROR_ALL;
	for(int p = 0; p<GAME_PLAYER; p++)
	for(int i=0; i<CHESSCOUNT_H; ++i)
	for(int j=0; j<CHESSCOUNT_W; ++j)
	{
		m_Chess[p][i][j] = ERROR_ALL;
	}

	memset(m_UserTaxis,0,sizeof(m_UserTaxis));
	memset(m_UserGrade,0,sizeof(m_UserGrade));
	m_Blast.RemoveAll();	

	//清除上次Lose屏幕缓存
	SAFE_RELEASE(m_pLoseSurface);
}

//连接器
bool CD3DWnd::LinkProcess(CPoint first, CPoint second,WORD userID)
{
	Blast blast;
	
	m_KeepIPoint1.x = ERROR_ALL;
	m_KeepIPoint1.y = ERROR_ALL;
	m_KeepIPoint2.x = ERROR_ALL;
	m_KeepIPoint2.y = ERROR_ALL;

	if(m_Logic.IsLink(m_Chess[userID],first,second,&(blast.result),&m_KeepIPoint1,&m_KeepIPoint2))
	{
		m_KeepFirst = m_nSelectFirst;
		m_KeepEnd	= m_nSelectSecond;

		//计算连接线
		LineProcess(&(blast.result),&(blast.m_LinePath));

		//连接动画
		blast.m_A_Line.zeroSpriteValues();
		blast.m_A_Line.m_bActive = false;
		blast.m_A_Line.m_bMultiPic = true;
		strncpy(blast.m_A_Line.m_sType ,"line",sizeof(blast.m_A_Line.m_sType));
		blast.m_A_Line.m_bAutoLoop = false;
		blast.m_A_Line.m_bSingleFrame = false;
		blast.m_A_Line.m_nFrameWidth = 42;
		blast.m_A_Line.m_nFrameHeight = 47;
		blast.m_A_Line.m_nCurrentFrame = 0;
		blast.m_A_Line.m_nSrcPosX = 0;
		blast.m_A_Line.m_nSrcPosY = 0;
		blast.m_A_Line.m_nPosX = 0;
		blast.m_A_Line.m_nPosY = 0;
		blast.m_A_Line.m_nFrameBegin = 0;
		blast.m_A_Line.m_nFrameEnd = 10;
		blast.m_A_Line.m_nWidthCount = 1;
		blast.m_A_Line.m_nFrameRate = 40;
		blast.m_A_Line.m_nFrameCount = 5;

		blast.m_A_Link.zeroSpriteValues();
		blast.m_A_Link.m_bActive = false;
		blast.m_A_Link.m_bMultiPic = true;
		strncpy(blast.m_A_Link.m_sType ,"link",sizeof(blast.m_A_Link.m_sType));
		blast.m_A_Link.m_bAutoLoop = false;
		blast.m_A_Link.m_bSingleFrame = false;
		blast.m_A_Link.m_nFrameWidth = 60;
		blast.m_A_Link.m_nFrameHeight = 61;
		blast.m_A_Link.m_nCurrentFrame = 0;
		blast.m_A_Link.m_nSrcPosX = 0;
		blast.m_A_Link.m_nSrcPosY = 0;
		blast.m_A_Link.m_nPosX = 0;
		blast.m_A_Link.m_nPosY = 0;
		blast.m_A_Link.m_nFrameBegin = 0;
		blast.m_A_Link.m_nFrameEnd = BLASTFRAMECOUNT;
		blast.m_A_Link.m_nWidthCount = 1;
		blast.m_A_Link.m_nFrameRate = 8;
		blast.m_A_Link.m_nFrameCount = 1;

		// 打开显示状态
		blast.m_A_Link.m_bActive = true;
		blast.m_A_Line.m_bActive = true;

        // 放入容器
		m_Blast.InsertAt(m_Blast.GetCount(), blast);
		
		m_Chess[userID][m_nSelectFirst.y][m_nSelectFirst.x] =ERROR_ALL;
		m_Chess[userID][m_nSelectSecond.y][m_nSelectSecond.x] = ERROR_ALL;

		m_ptLinkFirst = m_nSelectFirst;
		m_ptLinkSecond = m_nSelectSecond;

		//更新棋盘背景
		ExpunctionChess(2,m_nSelectFirst);
		ExpunctionChess(2,m_nSelectSecond);

		//更新棋子个数
		AccountChessCount();
		
		m_nSelectFirst.x = ERROR_ALL;
		m_nSelectFirst.y = ERROR_ALL;
		m_nSelectSecond.x = ERROR_ALL;
		m_nSelectSecond.y = ERROR_ALL;
		return true;
	}
	else
	{
		m_nSelectFirst.x = m_nSelectSecond.x;
		m_nSelectFirst.y = m_nSelectSecond.y;
		m_nSelectSecond.x = ERROR_ALL;
		m_nSelectSecond.y = ERROR_ALL;
		return false;
	}
}

void CD3DWnd::LineProcess(CArrayTemplate<CPoint>	*result, CArrayTemplate<int>		*m_LinePath)
{
	int count = 0;
	m_LinePath->RemoveAll();
	for(int i=0; i<result->GetCount(); i++)
	{
		if(i != 0)
		{
			if(i+1 == result->GetCount())
			{
				m_LinePath->InsertAt(m_LinePath->GetCount(),0);
				return;
			}

			// 横向
			if(result->ElementAt(i).y == result->ElementAt(i-1).y &&result->ElementAt(i).y == result->ElementAt(i+1).y)
			{
				m_LinePath->InsertAt(m_LinePath->GetCount(),0);
			}else
			// 竖
			if(result->ElementAt(i).x == result->ElementAt(i-1).x && result->ElementAt(i).x == result->ElementAt(i+1).x)
			{
				m_LinePath->InsertAt(m_LinePath->GetCount(),1);
			}else

			// 横转竖
			if(result->ElementAt(i-1).y == result->ElementAt(i).y && result->ElementAt(i).y != result->ElementAt(i+1).y)
			{
				if(count==0)
				{
					// 原方向 右
					if( m_KeepIPoint1.x > m_KeepFirst.x)
					{
						// 向上走
						if(result->ElementAt(i+1).y > result->ElementAt(i).y)
							m_LinePath->InsertAt(m_LinePath->GetCount(),2);
						else
						// 下
							m_LinePath->InsertAt(m_LinePath->GetCount(),4);
					}
					// 原方向 左
					else
					{
						// 上
						if(result->ElementAt(i+1).y>result->ElementAt(i).y)
							m_LinePath->InsertAt(m_LinePath->GetCount(),3);
						else
						// 下
							m_LinePath->InsertAt(m_LinePath->GetCount(),5);
					}
					count ++;
				}
				else
				if(count==1)
				{
					
					// 原方向 右
					if( m_KeepIPoint2.x > m_KeepIPoint1.x)
					{
						// 向上走
						if(result->ElementAt(i+1).y>result->ElementAt(i).y)
							m_LinePath->InsertAt(m_LinePath->GetCount(),2);
						else
						// 下
							m_LinePath->InsertAt(m_LinePath->GetCount(),4);
					}
					// 原方向 左
					else
					{
						// 上
						if(result->ElementAt(i+1).y>result->ElementAt(i).y)
							m_LinePath->InsertAt(m_LinePath->GetCount(),3);
						else
						// 下
							m_LinePath->InsertAt(m_LinePath->GetCount(),5);
					}
				}
			}else
			// 竖转横
			if(result->ElementAt(i-1).x == result->ElementAt(i).x && result->ElementAt(i).x != result->ElementAt(i+1).x)
			{
				if(count==0)
				{
					// 原方向 上
					if( m_KeepIPoint1.y > m_KeepFirst.y)
					{
						//右拐
						if(result->ElementAt(i+1).x>result->ElementAt(i).x)
							m_LinePath->InsertAt(m_LinePath->GetCount(),5);
						else
						//左拐
							m_LinePath->InsertAt(m_LinePath->GetCount(),4);
						
					}
					// 方向 下
					else
					{
						// 右
						if(result->ElementAt(i+1).x>result->ElementAt(i).x)
							m_LinePath->InsertAt(m_LinePath->GetCount(),3);
						else
						// 左
							m_LinePath->InsertAt(m_LinePath->GetCount(),2);
						
					}
					count ++;
				}
				else
				if(count==1)
				{
					// 原方向 上
					if( m_KeepIPoint2.y > m_KeepIPoint1.y)
					{
						// 右
						if(result->ElementAt(i+1).x>result->ElementAt(i).x)
							m_LinePath->InsertAt(m_LinePath->GetCount(),5);
						else
						// 左
							m_LinePath->InsertAt(m_LinePath->GetCount(),4);
					}
					// 方向 下
					else
					{
						//右拐
						if(result->ElementAt(i+1).x>result->ElementAt(i).x)
							m_LinePath->InsertAt(m_LinePath->GetCount(),3);
						else
						//左拐
							m_LinePath->InsertAt(m_LinePath->GetCount(),2);
					}
				}
			}
		}
		else
		{
			m_LinePath->InsertAt(m_LinePath->GetCount(),0);
		}
	}
}

// 炸弹
void CD3DWnd::AutoLink()
{
	if(m_Logic.LinkPrompt(m_Chess[2],&m_nSelectFirst,&m_nSelectSecond))
	{
		//是否可以连接
		if(LinkProcess(m_nSelectFirst,m_nSelectSecond,2))
		{
			//::SendMessage(AfxGetMainWnd()->m_hWnd,IDM_D3D_BOMB,0,0);
			if(m_bHitState)
			{
				++m_nHitCount ;
				AddHitState(m_nHitCount);
				::SendMessage(this->GetParent()->m_hWnd,IDM_D3D_LINK,2,0);
			}
			else
			{
				m_nHitCount = 0;
				::SendMessage(this->GetParent()->m_hWnd,IDM_D3D_LINK,1,0);
			}
			m_bHitState = true;

		}
	}
}

// 绘制游戏信息
void CD3DWnd::DrawGameInfo(void)
{
	CRect rect(0,0,0,0);

	//用户信息
	for(int i=0; i<GAME_PLAYER; i++)
	{
		if(m_UserData[i] != NULL)
		{
			if(i==2)
			{
				rect.left = 320;
				rect.top = 510;

				if(m_bGameLose[i])
				{
					m_Sprite_Cry.m_nPosX = 230;
					m_Sprite_Cry.m_nPosY = 195;
					//m_Sprite_Cry.DrawSprite(m_pDevice, 0.6f);
				}

				if(m_bGameOver)
				{
					if(m_UserTaxis[i] == 1)
					{
						m_Sprite_StopIt.m_nPosX = 230;
						m_Sprite_StopIt.m_nPosY = 195;
						//m_Sprite_StopIt.DrawSprite(m_pDevice, 0.7f);
						m_Sprite_Win.m_nPosX = (float)(rect.left - 13);
						m_Sprite_Win.m_nPosY = (float)(rect.top - 90);
						m_Sprite_Win.DrawSprite(m_pDevice,0.6f);
					}
					else
					{
						m_Sprite_Lose.m_nPosX = (float)(rect.left - 30);
						m_Sprite_Lose.m_nPosY = (float)(rect.top - 90);
						m_Sprite_Lose.DrawSprite(m_pDevice,0.6f);
					}
					m_Sprite_ScoreFrame.m_nPosX = (float)(rect.left - 10);
					m_Sprite_ScoreFrame.m_nPosY = (float)(rect.top - 20);
					m_Sprite_ScoreFrame.DrawSprite(m_pDevice,0.6f);

					m_Sprite_Number.m_nPosX = (float)(rect.left+110);
					m_Sprite_Number.m_nPosY = (float)(rect.top - 25);
					m_Sprite_Number.m_nCurrentFrame = m_UserTaxis[i]-1;
					m_Sprite_Number.DrawSprite(m_pDevice,0.7f);

					CString fenshu;
					if(m_UserGrade[i]<=0)
						fenshu.Format("本局积分：%d",m_UserGrade[i]);
					else
						fenshu.Format("本局积分：+%d",m_UserGrade[i]);
					//游戏信息
					m_p2DFont->DrawTextA(NULL,fenshu,-1,&rect,DT_CALCRECT,0);
					m_p2DFont->DrawText(NULL,
					fenshu, //文本内容
					-1, //文本长度
					&rect, //文本的显示区域，使用窗口坐标
					DT_LEFT, //显示格式：左对齐
					D3DCOLOR_XRGB(255,255,0) ); //
				}
				
			}
			else
			{
				int t = (i == 3?t=2:t=i);
				rect.left = CHESSFRAME_W_S+t*(206+6)+30;
				rect.top = CHESSFRAME_H_S + 140;
				
				//用户名
				m_p2DFont->DrawTextA(NULL,m_UserData[i]->szName,-1,&rect,DT_CALCRECT,0);
				m_p2DFont->DrawText(NULL,
				m_UserData[i]->szName, //文本内容
				-1, //文本长度
				&rect, //文本的显示区域，使用窗口坐标
				DT_LEFT, //显示格式：左对齐
				D3DCOLOR_XRGB(255,255,0) ); //

				if(m_bGameLose[i])
				{
					m_Sprite_SmallCry.m_nPosX = (float)(rect.left+20);
					m_Sprite_SmallCry.m_nPosY = (float)(rect.top - 140);
					//m_Sprite_SmallCry.DrawSprite(m_pDevice, 0.7f);
				}

				if(m_bGameOver)
				{
					m_Sprite_Number.m_nPosX = (float)(rect.left+30);
					m_Sprite_Number.m_nPosY = (float)(rect.top - 110);
					m_Sprite_Number.m_nCurrentFrame = m_UserTaxis[i]-1;
					m_Sprite_Number.DrawSprite(m_pDevice,0.7f);
				}
			}

			if(m_UserData[i]->cbUserStatus == US_READY)
			{
				m_Sprite_ready.m_nPosX = (float)(rect.left - 30);
				m_Sprite_ready.m_nPosY = (float)(rect.top - 100);
				m_Sprite_ready.DrawSprite(m_pDevice,0.5);
			}
		}
	}	

	rect.left = 650;
	rect.top = 680;
	m_p2DFont->DrawTextA(NULL,m_ChessCountString,-1,&rect,DT_CALCRECT,0);
	m_p2DFont->DrawText(NULL,
				m_ChessCountString, //文本内容
				-1, //文本长度
				&rect, //文本的显示区域，使用窗口坐标
				DT_LEFT, //显示格式：左对齐
				D3DCOLOR_XRGB(255,255,0) ); //

	if(m_bStopIt)
	{
		m_Sprite_StopIt.m_nPosX = 220.0f;
		m_Sprite_StopIt.m_nPosY = 180.0f;
		//m_Sprite_StopIt.DrawSprite(m_pDevice,0.8f);
	}

	if(m_nSelectFirst.x != ERROR_ALL && m_nSelectFirst.y != ERROR_ALL)
	{
		m_Sprite_Select.m_nPosX = (float)(CHESSFRAME_W+m_nSelectFirst.x*CHESSWIDTH);
		m_Sprite_Select.m_nPosY = (float)(CHESSFRAME_H+m_nSelectFirst.y*CHESSHEIGHT);
		m_Sprite_Select.DrawSprite(m_pDevice, 0.5);
	}
	if(m_nSelectSecond.x != ERROR_ALL && m_nSelectSecond.y != ERROR_ALL)
	{
		m_Sprite_Select.m_nPosX = (float)(CHESSFRAME_W+m_nSelectSecond.x*CHESSWIDTH);
		m_Sprite_Select.m_nPosY = (float)(CHESSFRAME_H+m_nSelectSecond.y*CHESSHEIGHT);
		m_Sprite_Select.DrawSprite(m_pDevice, 0.5);
	}
	for(int i = 0; i<m_RollText.GetCount(); i++)
	{
		
		if(m_RollText.ElementAt(i).m_nFrameCount<=0)
		{
			m_RollText.RemoveAt(i);
		}else
		{
			m_RollText.ElementAt(i).DrawText();
		}
	}
}

void CD3DWnd::OnRButtonDown(UINT nFlags, CPoint point)
{	
	CWnd::OnRButtonDown(nFlags, point);
}

// 重列
void CD3DWnd::Realign(void)
{
	m_PointSprite_Star.On();
	m_Logic.Realign(m_Chess[2]);
	UpdateBackSurface();
	::SendMessage(AfxGetMainWnd()->m_hWnd,IDM_D3D_REALIGN,0,0);
}

IDirect3DSurface9* CD3DWnd::getSurfaceFromResource(LONG RCID, int width, int height) 
{ 
	HRESULT hResult; 
	IDirect3DSurface9* surface = NULL; 

	hResult = m_pDevice->CreateOffscreenPlainSurface(width, height, D3DDisplayMode.Format, D3DPOOL_DEFAULT, &surface, NULL); 
	if (FAILED(hResult)) 
		return NULL;


	hResult = D3DXLoadSurfaceFromResource(surface, 
				       NULL, 
					   NULL,GetModuleHandle( NULL ),
				       MAKEINTRESOURCE(RCID),
					   NULL, D3DX_FILTER_NONE,
					   D3DCOLOR_COLORVALUE(0.0f,0.0f,0.0f,0.0f),
					   NULL);

	if(FAILED(hResult)) {MessageBox(TEXT("图片加载失败!"),"",MB_OK);return NULL;}

	return surface;
} 

HRESULT CD3DWnd::LoadAlphaIntoSurface(LONG RCID, LPDIRECT3DSURFACE9 psurf)
{
    HRESULT hr;
    D3DSURFACE_DESC sd;
    LPDIRECT3DTEXTURE9 ptexAlpha;
    LPDIRECT3DSURFACE9 psurfAlpha;
    LPDIRECT3DSURFACE9 psurfTarget;

    psurf->GetDesc(&sd);

   hr = D3DXCreateTextureFromResourceEx(m_pDevice, GetModuleHandle( NULL ),
						MAKEINTRESOURCE(RCID), sd.Width, sd.Height, 1, 0, 
						 D3DDisplayMode.Format, D3DPOOL_MANAGED, D3DX_DEFAULT, 
						 D3DX_DEFAULT, 0, NULL, NULL, &ptexAlpha);
    hr = ptexAlpha->GetSurfaceLevel(0, &psurfAlpha);

    hr = m_pDevice->CreateOffscreenPlainSurface(sd.Width, sd.Height, D3DDisplayMode.Format, D3DPOOL_SCRATCH, &psurfTarget, NULL);
    hr = D3DXLoadSurfaceFromSurface(psurfTarget, NULL, NULL, psurf, NULL, NULL, 
        D3DX_DEFAULT, 0);

    D3DLOCKED_RECT lrSrc;
    D3DLOCKED_RECT lrDest;

    hr = psurfAlpha->LockRect(&lrSrc, NULL, D3DLOCK_READONLY);
    hr = psurfTarget->LockRect(&lrDest, NULL, 0);

    DWORD xp;
    DWORD yp;
    DWORD* pdwRowSrc = (DWORD*)lrSrc.pBits;
    DWORD* pdwRowDest = (DWORD*)lrDest.pBits;
    DWORD* pdwSrc;
    DWORD* pdwDest;
    DWORD dwAlpha;
    LONG dataBytesPerRow = 4 * sd.Width;

    for (yp = 0; yp < sd.Height; yp++)
    {
        pdwSrc = pdwRowSrc;
        pdwDest = pdwRowDest;
        for (xp = 0; xp < sd.Width; xp++)
        {
            dwAlpha = *pdwSrc << 24;
            *pdwDest &= 0x00ffffff;
            *pdwDest |= dwAlpha;

            pdwSrc++;
            pdwDest++;
        }
        pdwRowSrc += lrSrc.Pitch / 4;
        pdwRowDest += lrDest.Pitch / 4;
    }

    psurfAlpha->UnlockRect();
    psurfTarget->UnlockRect();

    // Copy psurfTarget back into real surface
    hr = D3DXLoadSurfaceFromSurface(psurf, NULL, NULL, psurfTarget, NULL, NULL, 
        D3DX_DEFAULT, 0);
    
    // Release allocated interfaces
	psurfTarget->Release();
	psurfAlpha->Release();
	ptexAlpha->Release();

    return S_OK;
}

IDirect3DSurface9* CD3DWnd::getBackBuffer(void) 
{ 
	IDirect3DSurface9* backbuffer = NULL; 

	if (!m_pDevice) 
	{
		return NULL; 
	}

	HRESULT hResult = m_pDevice->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO, &backbuffer); 
	if (FAILED(hResult)) 
	{
		//AfxMessageBox("获取背景缓存区指针失败");
		return NULL; 
	}
	else 
		return backbuffer; 
} 

void CD3DWnd::blitToSurface(IDirect3DSurface9* srcSurface, const RECT *srcRect, const RECT *destRect) 
{ 
	//首先在缓冲表面进行
	IDirect3DSurface9* backbuffer = getBackBuffer(); 
	
	if(m_bGameOver || m_bGameLose[2])
	{
		if(m_pLoseSurface == NULL)
		{
		GameLoseSurface(srcSurface, CRect(CHESSFRAME_W+5,CHESSFRAME_H+10,782,703));
		m_pDevice->CreateOffscreenPlainSurface(782-CHESSFRAME_W, 703-CHESSFRAME_H, D3DDisplayMode.Format, D3DPOOL_DEFAULT, &m_pLoseSurface, NULL); 
		m_pDevice->StretchRect(srcSurface,CRect(CHESSFRAME_W+5,CHESSFRAME_H+10,782,703),m_pLoseSurface, CRect(0,0,782 - CHESSFRAME_W-5,703-CHESSFRAME_H-10), D3DTEXF_NONE); 
		}
		else
		{
			m_pDevice->StretchRect(m_pLoseSurface, CRect(0,0,782 - CHESSFRAME_W-5,703-CHESSFRAME_H-10),srcSurface,CRect(CHESSFRAME_W+5,CHESSFRAME_H+10,782,703), D3DTEXF_NONE); 
		}
	}

	HRESULT rslt = m_pDevice->StretchRect(srcSurface, srcRect,backbuffer, destRect, D3DTEXF_NONE); 
	if(FAILED(rslt)) {MessageBox(TEXT("离屏表面拷贝失败！"),"",MB_OK);return;}

	backbuffer->Release();
} 
// 更新
void CD3DWnd::UpdateBackSurface(void)
{
	HRESULT hResult;
	if(m_pDevice == NULL)
		return;
	IDirect3DSurface9* surface = NULL; 

	int temp=0;
	int	XPos=0;

	//整个背景
	surface = getSurfaceFromResource(IDB_BK_LEFT,788,768);
	hResult = m_pDevice->StretchRect(surface,CRect(0,28,788,768),m_pBkSurface,CRect(0,0,788,740),D3DTEXF_NONE);
	if(FAILED(hResult)) {MessageBox(TEXT("场景图片加载失败!"),"",MB_OK);return ;}
	SAFE_RELEASE(surface)

	//所略棋盘背景
	surface = getSurfaceFromResource(IDB_SBKT,206,180);
	for(int i=0; i<3; i++)
	m_pDevice->StretchRect(surface,CRect(0,0,206,180),m_pBkSurface,CRect(150+(206+6)*i,10,150+(206+6)*i+206,10+180),D3DTEXF_NONE);
	SAFE_RELEASE(surface)

	StepItTimer();
	m_pDevice->StretchRect(m_pBkSurface,CRect(0,0,788,740),m_pSurface,CRect(0,0,788,740),D3DTEXF_NONE);

	for(int p=0; p<GAME_PLAYER; ++p)
	for(int j=CHESSCOUNT_W-1; j>=0; --j)
	for(int i=0; i<CHESSCOUNT_H; ++i)
	{
		if(p == 2)
		{
			if(m_Chess[p][i][j] != ERROR_ALL)
			{
				temp = m_Chess[p][i][j];
				if(temp<PROP_MIN_VALUE)
					XPos = temp>=12?1:0;
				else
					XPos = (temp-PROP_MIN_VALUE)>=2?1:0;
				
				getSurfaceFromSurface(m_pSurface,
					CRect(CHESSFRAME_W+j*CHESSWIDTH - 7,CHESSFRAME_H+i*CHESSHEIGHT,CHESSFRAME_W+j*CHESSWIDTH+CHESSWIDTH,CHESSFRAME_H+i*CHESSHEIGHT+CHESSHEIGHT+6),
					m_pChess_Surface,CRect(50*XPos,0,50*XPos+49,53),RGB(255,0,255));

				if(temp<PROP_MIN_VALUE)
				{
					m_pDevice->StretchRect(
						m_pChessFace_Surface,CRect(temp*CHESSWIDTH,0,(temp+1)*CHESSWIDTH,47),m_pSurface,
						CRect(
						CHESSFRAME_W+j*CHESSWIDTH,
						CHESSFRAME_H+i*CHESSHEIGHT,
						CHESSFRAME_W+j*CHESSWIDTH+CHESSWIDTH,
						CHESSFRAME_H+i*CHESSHEIGHT+CHESSHEIGHT
							),
						D3DTEXF_NONE);
				}
				else
				{
					m_pDevice->StretchRect(
						m_pChessFaceProp_Surface,CRect((temp-PROP_MIN_VALUE)*CHESSWIDTH,0,(temp-PROP_MIN_VALUE+1)*CHESSWIDTH,47),m_pSurface,
						CRect(
						CHESSFRAME_W+j*CHESSWIDTH,
						CHESSFRAME_H+i*CHESSHEIGHT,
						CHESSFRAME_W+j*CHESSWIDTH+CHESSWIDTH,
						CHESSFRAME_H+i*CHESSHEIGHT+CHESSHEIGHT
							),
						D3DTEXF_NONE);
				}
			}
		}
		else
		{
			if(m_Chess[p][i][j] != ERROR_ALL)
			{
				int t = (p == 3)?2:p;
				temp = m_Chess[p][i][j];
				/*
				m_pDevice->StretchRect(surface2,CRect(0,0,11,12),m_pSurface,
					CRect(CHESSFRAME_W_S+t*(206+6)+j*CHESSWIDTH_S -1,CHESSFRAME_H_S+i*CHESSHEIGHT_S,
					CHESSFRAME_W_S+t*(206+6)+j*CHESSWIDTH_S -1 + CHESSWIDTH_S+1, CHESSFRAME_H_S+i*CHESSHEIGHT_S + CHESSHEIGHT_S+1),
					D3DTEXF_NONE);
					*/
				m_pDevice->StretchRect(m_pChessFaceSmall_Surface,CRect(temp*CHESSWIDTH_S,0,(temp+1)*CHESSWIDTH_S,CHESSHEIGHT_S),m_pSurface,
					CRect(CHESSFRAME_W_S+t*(206+6)+j*CHESSWIDTH_S ,CHESSFRAME_H_S+i*CHESSHEIGHT_S,
					CHESSFRAME_W_S+t*(206+6)+j*CHESSWIDTH_S + CHESSWIDTH_S, CHESSFRAME_H_S+i*CHESSHEIGHT_S + CHESSHEIGHT_S),
					D3DTEXF_NONE);
			}
		}
	}
}

void CD3DWnd::getSurfaceFromSurface(IDirect3DSurface9* des,const RECT* desRect, IDirect3DSurface9* src, const RECT* srcRect,COLORREF colorkey)
{
	D3DSURFACE_DESC ddsd;
	D3DLOCKED_RECT lockedRectBackBufferSrc;
	D3DLOCKED_RECT lockedRectBackBufferDes;
	IDirect3DSurface9* targetsuffer = NULL; 
	int nColorDepth ;
	int linearPitchSrc =0;
	int linearPitchDes =0;

	m_pDevice->CreateOffscreenPlainSurface(desRect->right - desRect->left, desRect->bottom - desRect->top, D3DDisplayMode.Format, D3DPOOL_DEFAULT, &targetsuffer, NULL); 

	m_pDevice->StretchRect(des, desRect, targetsuffer, CRect(0,0,desRect->right - desRect->left, desRect->bottom - desRect->top),D3DTEXF_NONE);

	src->LockRect( &lockedRectBackBufferSrc, NULL, D3DLOCK_READONLY) ;
	targetsuffer->LockRect( &lockedRectBackBufferDes, NULL, 0 ) ;

	des->GetDesc( &ddsd );

	switch( ddsd.Format)
	{
		case D3DFMT_R8G8B8 :

			nColorDepth = 24;
			break;

		case D3DFMT_A2R10G10B10:
			// 32-bit pixel format using 10 bits each for red, green, and blue, 
			// and 2 bits for alpha. Used only inn full-screen mode.
			nColorDepth = 32;
			break;

		case D3DFMT_A8R8G8B8:
			// 32-bit ARGB pixel format with alpha, using 8 bits per channel.
			nColorDepth = 32;
			break;

		case D3DFMT_X8R8G8B8:
			// 32-bit RGB pixel format, where 8 bits are reserved for each color.
			nColorDepth = 32;
			break;

		case D3DFMT_A4R4G4B4 :
			nColorDepth = 16;
			break;

		case D3DFMT_A1R5G5B5:
			// 16-bit pixel format where 5 bits are reserved for each color 
			// and 1 bit is reserved for alpha.
			nColorDepth = 16;
			break;

		case D3DFMT_X1R5G5B5:
			// 16-bit pixel format where 5 bits are reserved for each color.
			nColorDepth = 16;
			break;

		case D3DFMT_R5G6B5:
			// 16-bit RGB pixel format with 5 bits for red, 6 bits for green, 
			// and 5 bits for blue.
			nColorDepth = 16;
			break;
		case D3DFMT_A8R3G3B2 :
			nColorDepth = 16;
			break;
		case D3DFMT_X4R4G4B4 :
			nColorDepth = 16;
			break;

		default:
			CString msg;
			msg.Format("%d",ddsd.Format );
			AfxMessageBox(msg);
			break;
	}

	linearPitchSrc = lockedRectBackBufferSrc.Pitch;
	linearPitchDes= lockedRectBackBufferDes.Pitch;

	if( nColorDepth == 16 )
	{
		colorkey = 0x0000;
		USHORT *surfaceBufferSrc = static_cast<USHORT*>(lockedRectBackBufferSrc.pBits);
		USHORT *surfaceBufferDes = static_cast<USHORT*>(lockedRectBackBufferDes.pBits);
		linearPitchSrc = (linearPitchSrc>>1);
		linearPitchDes=(linearPitchDes>>1);

		for( int j = srcRect->top,m = 0; m < desRect->bottom-desRect->top; j++ ,m++)
			for( int i = srcRect->left,n=0; n < desRect->right-desRect->left; i++ ,n++)
			{
				if(surfaceBufferSrc[i+j*linearPitchSrc] != (USHORT)colorkey)
					surfaceBufferDes[n+m*linearPitchDes] = (USHORT)surfaceBufferSrc[i+j*linearPitchSrc];
			}
	}	
	else
	if( nColorDepth == 24)
	{
		colorkey = 0x000000;
		UINT *surfaceBufferSrc = static_cast<UINT*>(lockedRectBackBufferSrc.pBits);
		UINT *surfaceBufferDes = static_cast<UINT*>(lockedRectBackBufferDes.pBits);

		for( int j = srcRect->top,m = 0; m < desRect->bottom-desRect->top; j++ ,m++)
			for( int i = srcRect->left,n=0; n < desRect->right-desRect->left; i++ ,n++)
			{
				if(surfaceBufferSrc[i*3+j*lockedRectBackBufferSrc.Pitch] != (USHORT)colorkey)
					surfaceBufferDes[n*3+m*lockedRectBackBufferDes.Pitch] = (USHORT)surfaceBufferSrc[i*3+j*lockedRectBackBufferSrc.Pitch];
			}
	}
	else
	if( nColorDepth == 32 )
	{
		colorkey = 0x00000000;
		UINT *surfaceBufferSrc = static_cast<UINT*>(lockedRectBackBufferSrc.pBits);
		UINT *surfaceBufferDes = static_cast<UINT*>(lockedRectBackBufferDes.pBits);
		linearPitchSrc = (linearPitchSrc>>2);
		linearPitchDes=(linearPitchDes>>2);
		
		for( int j = srcRect->top,m = 0; m < desRect->bottom-desRect->top; j++ ,m++)
			for( int i = srcRect->left,n=0; n < desRect->right-desRect->left; i++ ,n++)
			{
				if(surfaceBufferSrc[i+j*linearPitchSrc] != colorkey)
					surfaceBufferDes[n+m*linearPitchDes] = surfaceBufferSrc[i+j*linearPitchSrc];
			}
	}

	targetsuffer->UnlockRect();
	src->UnlockRect();

	m_pDevice->StretchRect(targetsuffer, CRect(0,0,desRect->right - desRect->left, desRect->bottom - desRect->top),des,  desRect, D3DTEXF_NONE);

	targetsuffer->Release();
}

// // 消去棋子
void CD3DWnd::ExpunctionChess(int UserID, CPoint first)
{
	if(UserID != 2)return;
	IDirect3DSurface9* surface = NULL;
	int temp=0;
	m_pDevice->CreateOffscreenPlainSurface(CHESSWIDTH*3+2, CHESSHEIGHT*3+2, D3DDisplayMode.Format, D3DPOOL_DEFAULT, &surface, NULL);
	// 烤一个背景
	if(first.x ==17)
	m_pDevice->StretchRect(m_pBkSurface,
		CRect(
		CHESSFRAME_W+(first.x-1)*CHESSWIDTH,
		CHESSFRAME_H+(first.y-1)*CHESSHEIGHT,
		CHESSFRAME_W+(first.x-1)*CHESSWIDTH+CHESSWIDTH*2,
		CHESSFRAME_H+(first.y-1)*CHESSHEIGHT+CHESSHEIGHT*3),
		surface,
        CRect(0,0,CHESSWIDTH*2, CHESSHEIGHT*3),
		D3DTEXF_NONE);
	else
	m_pDevice->StretchRect(m_pBkSurface,
		CRect(
		CHESSFRAME_W+(first.x-1)*CHESSWIDTH,
		CHESSFRAME_H+(first.y-1)*CHESSHEIGHT,
		CHESSFRAME_W+(first.x-1)*CHESSWIDTH+CHESSWIDTH*3,
		CHESSFRAME_H+(first.y-1)*CHESSHEIGHT+CHESSHEIGHT*3),
		surface,
        CRect(0,0,CHESSWIDTH*3, CHESSHEIGHT*3),
		D3DTEXF_NONE);
	
	int m,n;
	int XPos=0;
	for(int j=first.y-1; j<=first.y+1 ; j++)
	{
		for(int i=first.x+1; i>=first.x-1; i--)
		{
	
			if(i>=0 && i<CHESSCOUNT_W && j >=0 && j <CHESSCOUNT_H)
			{
				m = j-first.y+1;
				n = i-first.x+1;
				
				if(j-1 >0 && j-1 <CHESSCOUNT_H && i+1>0 && i+1 <CHESSCOUNT_W)
				if(m_Chess[UserID][j-1][i+1]!= ERROR_ALL)
				{
					temp = m_Chess[UserID][j-1][i+1];
					if(temp<PROP_MIN_VALUE)
						XPos = temp>=12?1:0;
					else	
						XPos = (temp-PROP_MIN_VALUE)>=2?1:0;

					getSurfaceFromSurface(surface,
						CRect((n+1)*(CHESSWIDTH)-7,(m)*(CHESSHEIGHT),(n+1)*(CHESSWIDTH),(m)*(CHESSHEIGHT)+6),
						m_pChess_Surface,CRect(XPos*50,47,XPos*50+7,53),RGB(255,0,255));
				}

				
				if(i>=0 && i+1<CHESSCOUNT_W)
				if(m_Chess[UserID][j][i+1]!= ERROR_ALL)
				{
					temp = m_Chess[UserID][j][i+1];
					if(temp<PROP_MIN_VALUE)
						XPos = temp>=12?1:0;
					else	
						XPos = (temp-PROP_MIN_VALUE)>=2?1:0;

					getSurfaceFromSurface(surface,
						CRect((n+1)*(CHESSWIDTH)-7,(m)*(CHESSHEIGHT),(n+1)*(CHESSWIDTH),(m+1)*(CHESSHEIGHT)),
						m_pChess_Surface,CRect(XPos*50,0,XPos*50+7,47),RGB(255,0,255));
				}

				if(j >0 && j <CHESSCOUNT_H )
				if(m_Chess[UserID][j-1][i]!= ERROR_ALL)
				{
					temp = m_Chess[UserID][j-1][i];
					if(temp<PROP_MIN_VALUE)
						XPos = temp>=12?1:0;
					else
						XPos = (temp-PROP_MIN_VALUE)>=2?1:0;

					getSurfaceFromSurface(surface,
						CRect((n)*(CHESSWIDTH),(m)*(CHESSHEIGHT),(n+1)*(CHESSWIDTH),(m)*(CHESSHEIGHT)+6),
						m_pChess_Surface,CRect(XPos*50+7,47,XPos*50+49,53),RGB(255,0,255));
				}				

				if(m_Chess[UserID][j][i] != ERROR_ALL)
				{					
					temp = m_Chess[UserID][j][i];

					if(temp<PROP_MIN_VALUE)
					{
						m_pDevice->StretchRect(
							m_pChessFace_Surface,CRect(temp*CHESSWIDTH,0,(temp+1)*CHESSWIDTH,CHESSHEIGHT),surface,
							CRect(
							CRect((n)*(CHESSWIDTH),
							(m)*(CHESSHEIGHT),
							(n+1)*(CHESSWIDTH),
							(m+1)*(CHESSHEIGHT))
							),
							D3DTEXF_NONE);
					}
					else
					{
						m_pDevice->StretchRect(
							m_pChessFaceProp_Surface,CRect((temp-PROP_MIN_VALUE)*CHESSWIDTH,0,(temp-PROP_MIN_VALUE+1)*CHESSWIDTH,CHESSHEIGHT),surface,
							CRect(
							CRect((n)*(CHESSWIDTH),
							(m)*(CHESSHEIGHT),
							(n+1)*(CHESSWIDTH),
							(m+1)*(CHESSHEIGHT))
							),
							D3DTEXF_NONE);
					}
				}
			}
		}
	}
	if(first.x ==17)
	m_pDevice->StretchRect(surface,CRect(0,0,CHESSWIDTH*2, CHESSHEIGHT*3),m_pSurface,
		CRect(
		CHESSFRAME_W+(first.x-1)*CHESSWIDTH,
		CHESSFRAME_H+(first.y-1)*CHESSHEIGHT,
		CHESSFRAME_W+(first.x-1)*CHESSWIDTH+CHESSWIDTH*2,
		CHESSFRAME_H+(first.y-1)*CHESSHEIGHT+CHESSHEIGHT*3),
		D3DTEXF_NONE);
	else
		m_pDevice->StretchRect(surface,CRect(0,0,CHESSWIDTH*3, CHESSHEIGHT*3),m_pSurface,
		CRect(
		CHESSFRAME_W+(first.x-1)*CHESSWIDTH,
		CHESSFRAME_H+(first.y-1)*CHESSHEIGHT,
		CHESSFRAME_W+(first.x-1)*CHESSWIDTH+CHESSWIDTH*3,
		CHESSFRAME_H+(first.y-1)*CHESSHEIGHT+CHESSHEIGHT*3),
		D3DTEXF_NONE);
	SAFE_RELEASE(surface)
}

void CD3DWnd::sExpunctionChess(int UserID, CPoint first)
{
	if(UserID == 2)return;
	IDirect3DSurface9* surface = NULL;
	int temp=0,t = UserID==3?2:UserID;
	m_pDevice->CreateOffscreenPlainSurface(CHESSWIDTH_S*3, CHESSHEIGHT_S*3, D3DDisplayMode.Format, D3DPOOL_DEFAULT, &surface, NULL);
	// 烤一个背景
	m_pDevice->StretchRect(m_pBkSurface,
		CRect(
		CHESSFRAME_W_S+(first.x-1)*CHESSWIDTH_S+t*(206+6),
		CHESSFRAME_H_S+(first.y-1)*CHESSHEIGHT_S,
		CHESSFRAME_W_S+(first.x-1)*CHESSWIDTH_S+CHESSWIDTH_S*3+t*(206+6),
		CHESSFRAME_H_S+(first.y-1)*CHESSHEIGHT_S+CHESSHEIGHT_S*3),
		surface,
        CRect(0,0,CHESSWIDTH_S*3, CHESSHEIGHT_S*3),
		D3DTEXF_NONE);

	int m,n;
	
	for(int j=first.y-1; j<=first.y+1 ; j++)
	{
		for(int i=first.x+1; i>=first.x-1; i--)
		{
			if(i>=0 && i<CHESSCOUNT_W && j >=0 && j <CHESSCOUNT_H)
			{
				m = j-first.y+1;
				n = i-first.x+1;

				if(m_Chess[UserID][j][i] != ERROR_ALL)
				{
					temp = m_Chess[UserID][j][i];
					m_pDevice->StretchRect(
					m_pChessFaceSmall_Surface,CRect(temp*CHESSWIDTH_S,0,(temp+1)*CHESSWIDTH_S,CHESSHEIGHT_S),surface,
					CRect(
					CRect((n)*(CHESSWIDTH_S),
					(m)*(CHESSHEIGHT_S),
					(n+1)*(CHESSWIDTH_S),
					(m+1)*(CHESSHEIGHT_S))
					),
					D3DTEXF_NONE);
				}
			}
		}
	}
	
	m_pDevice->StretchRect(surface,CRect(0,0,CHESSWIDTH_S*3, CHESSHEIGHT_S*3),m_pSurface,
		CRect(
		CHESSFRAME_W_S+(first.x-1)*CHESSWIDTH_S+t*(206+6),
		CHESSFRAME_H_S+(first.y-1)*CHESSHEIGHT_S,
		CHESSFRAME_W_S+(first.x-1)*CHESSWIDTH_S+CHESSWIDTH_S*3+t*(206+6),
		CHESSFRAME_H_S+(first.y-1)*CHESSHEIGHT_S+CHESSHEIGHT_S*3),
		D3DTEXF_NONE);

	SAFE_RELEASE(surface)
}
// 计算棋子数量
void CD3DWnd::AccountChessCount(void)
{
	int count=0;
	for(int j=0; j<CHESSCOUNT_W-1; ++j)
	for(int i=0; i<CHESSCOUNT_H; ++i)
	{
		if(m_Chess[2][i][j] != ERROR_ALL)
		{
			++count;
		}
	}
	_snprintf(m_ChessCountString,64,TEXT("剩余棋子数量：%d个"),count);

}

// 走动定时器
void CD3DWnd::StepItTimer(void)
{	
	m_pDevice->StretchRect(m_pTimerBK_Surface,CRect(0,0,412,21),m_pBkSurface,
					CRect(14,680,426,701),
					D3DTEXF_NONE);
	
	int currentwidth = (int)(55 + 1.0f*(LINKALLTIME - m_nTimeCounter)/LINKALLTIME*355);

	m_pDevice->StretchRect(m_pTimer_Surface,CRect(0,0,currentwidth,21),m_pBkSurface,
					CRect(14,680,14+currentwidth,701),
					D3DTEXF_NONE);
	
	m_pDevice->StretchRect(m_pBkSurface,CRect(14,680,429,701),m_pSurface,
					CRect(14,680,429,701),
					D3DTEXF_NONE);

	if(m_ExerciseMode == true &&m_nTimeCounter>=30)
	{
		m_nTimeCounter = 0;
		Execise();
	}
}

// 更新小棋盘
void CD3DWnd::UpdateChessSmall(int UserID)
{
	IDirect3DSurface9* surface = NULL;
	IDirect3DSurface9* surface2 = NULL;
	
	m_pDevice->CreateOffscreenPlainSurface(CHESSWIDTH_S*CHESSCOUNT_W, CHESSHEIGHT_S*CHESSCOUNT_H, D3DDisplayMode.Format, D3DPOOL_DEFAULT, &surface, NULL);

	int t = (UserID == 3)?2:UserID;
	m_pDevice->StretchRect(m_pBkSurface,CRect(CHESSFRAME_W_S+t*(206+6) ,CHESSFRAME_H_S,
					CHESSFRAME_W_S+t*(206+6)+CHESSWIDTH_S*CHESSCOUNT_W, CHESSFRAME_H_S+CHESSHEIGHT_S*CHESSCOUNT_H),
					surface,CRect(0,0,CHESSWIDTH_S*CHESSCOUNT_W,CHESSHEIGHT_S*CHESSCOUNT_H)	,
					D3DTEXF_NONE);

	int temp=0;
	for(int j=CHESSCOUNT_W-1; j>=0; --j)
	for(int i=0; i<CHESSCOUNT_H; ++i)
	{
		if(UserID != 2)
		{
			if(m_Chess[UserID][i][j] != ERROR_ALL)
			{
				temp = m_Chess[UserID][i][j];
				m_pDevice->StretchRect(m_pChessFaceSmall_Surface,CRect(temp*CHESSWIDTH_S,0,(temp+1)*CHESSWIDTH_S,CHESSHEIGHT_S),surface,
					CRect(j*CHESSWIDTH_S ,i*CHESSHEIGHT_S,
					j*CHESSWIDTH_S + CHESSWIDTH_S, i*CHESSHEIGHT_S + CHESSHEIGHT_S),
					D3DTEXF_NONE);
			}
		}
	}
	m_pDevice->StretchRect(surface,CRect(0,0,CHESSWIDTH_S*CHESSCOUNT_W,CHESSHEIGHT_S*CHESSCOUNT_H),m_pSurface,
					CRect(CHESSFRAME_W_S+t*(206+6) ,CHESSFRAME_H_S,
					CHESSFRAME_W_S+t*(206+6)+CHESSWIDTH_S*CHESSCOUNT_W, CHESSFRAME_H_S+CHESSHEIGHT_S*CHESSCOUNT_H),
					D3DTEXF_NONE);
}

BOOL CD3DWnd::PreTranslateMessage(MSG* pMsg)
{
	return CWnd::PreTranslateMessage(pMsg);
}

// 禁手
void CD3DWnd::StopIt(void)
{
	m_bStopIt = true;
	m_bCanGame = false;
	SetTimer(ID_STOPIT,5000,NULL);
}

// 增加障碍
void CD3DWnd::Clog(BYTE posX[12], BYTE posY[12], BYTE color[12], int UserID)
{
	for(int i=0; i<12; i++)
	{
		if(posX[i] != ERROR_ALL && posY[i] != ERROR_ALL && color[i] != ERROR_ALL)
		{
			if(m_Chess[UserID][posY[i]][posX[i]] ==  ERROR_ALL)
			m_Chess[UserID][posY[i]][posX[i]] = color[i];
		}
	}

	if(UserID == 2)
		UpdateBackSurface();
	else
		UpdateChessSmall(UserID);
}

// 指南针
void CD3DWnd::Guide(void)
{
	m_Logic.LinkPrompt(m_Chess[2],&m_nSelectFirst,&m_nSelectSecond);
	::SendMessage(AfxGetMainWnd()->m_hWnd,IDM_D3D_GUIDE,0,0);
}

// 显示连接状态
void CD3DWnd::AddHitState(int nHitCount)
{
	CRollFont RollFont;
	CString msg;
	msg.Format("连击：%d",nHitCount);
	RollFont.InitFont(50,650,msg,80,1,m_p2DFontBig);
	m_RollText.InsertAt(m_RollText.GetCount(),RollFont);

	/*if(m_nHitCount>5)
	{
		m_nHitCount = 0;
		m_bHitState = false;
	}*/
}

// 游戏失败表面处理
void CD3DWnd::GameLoseSurface(IDirect3DSurface9* srcSurface, const RECT* srcRect)
{
	D3DSURFACE_DESC ddsd;
	D3DLOCKED_RECT lockedRectBackBufferSrc;
	//双缓冲画图
	IDirect3DSurface9* targetsuffer = NULL; 
	int nColorDepth ;
	int linearPitchSrc =0;
	//=======位转换============
	int cMask; //取24位
	int aMask; //alphi掩码
	int rMask; //红色掩码
	int gMask; //绿色掩码
	int bMask; //兰色掩码

	int rMove;
	int gMove;

	m_pDevice->CreateOffscreenPlainSurface(srcRect->right - srcRect->left, srcRect->bottom - srcRect->top, D3DDisplayMode.Format, D3DPOOL_DEFAULT, &targetsuffer, NULL); 
	m_pDevice->StretchRect(srcSurface, srcRect, targetsuffer, CRect(0,0,srcRect->right - srcRect->left, srcRect->bottom - srcRect->top),D3DTEXF_NONE);
	
	int sour,r,g,b,t; //临时变量

	targetsuffer->LockRect( &lockedRectBackBufferSrc, NULL, 0 ) ;

	targetsuffer->GetDesc( &ddsd );

	switch( ddsd.Format )
	{
		case D3DFMT_A2R10G10B10:
			// 32-bit pixel format using 10 bits each for red, green, and blue, 
			// and 2 bits for alpha. Used only inn full-screen mode.
			nColorDepth = 32;

			//=======32位转换============
			cMask=0x03FFFFFF; //取30位
			aMask=0xC0000000; //alphi掩码
			rMask=0x3ff00000; //红色掩码
			gMask=0x000FFC00; //绿色掩码
			bMask=0x000003FF; //兰色掩码

			rMove = 20;
			gMove = 10;
			break;

		case D3DFMT_A8R8G8B8:
			// 32-bit ARGB pixel format with alpha, using 8 bits per channel.
			nColorDepth = 32;
			//=======32位转换============
			cMask=0x00FFFFFF; //取24位
			aMask=0xff000000; //alphi掩码
			rMask=0x00ff0000; //红色掩码
			gMask=0x0000ff00; //绿色掩码
			bMask=0x000000ff; //兰色掩码

			rMove = 16;
			gMove = 8;
			break;

		case D3DFMT_X8R8G8B8:
			// 32-bit RGB pixel format, where 8 bits are reserved for each color.
			nColorDepth = 32;

			//=======32位转换============
			cMask=0x00FFFFFF; //取24位
			aMask=0xff000000; //alphi掩码
			rMask=0x00ff0000; //红色掩码
			gMask=0x0000ff00; //绿色掩码
			bMask=0x000000ff; //兰色掩码

			rMove = 16;
			gMove = 8;
			break;

		case D3DFMT_A1R5G5B5:
			// 16-bit pixel format where 5 bits are reserved for each color 
			// and 1 bit is reserved for alpha.
			nColorDepth = 16;

			//=======16位转换============
			rMask=0x03ff; //红色掩码
			gMask=0x03E0; //绿色掩码
			bMask=0x001F; //兰色掩码

			rMove = 10;
			gMove = 5;
			break;

		case D3DFMT_X1R5G5B5:
			// 16-bit pixel format where 5 bits are reserved for each color.
			nColorDepth = 16;

			//=======16位转换============
			rMask=0x03ff; //红色掩码
			gMask=0x03E0; //绿色掩码
			bMask=0x001F; //兰色掩码

			rMove = 10;
			gMove = 5;
			break;

		case D3DFMT_R5G6B5:
			// 16-bit RGB pixel format with 5 bits for red, 6 bits for green, 
			// and 5 bits for blue.
			nColorDepth = 16;

			//=======16位转换============
			rMask=0x07ff; //红色掩码
			gMask=0x07E0; //绿色掩码
			bMask=0x001F; //兰色掩码

			rMove = 11;
			gMove = 6;
			break;

		default:
			targetsuffer->UnlockRect();
			targetsuffer->Release();
			return;
	}

	linearPitchSrc = lockedRectBackBufferSrc.Pitch;

	

	if( nColorDepth == 16 )
	{
		USHORT *surfaceBufferSrc = static_cast<USHORT*>(lockedRectBackBufferSrc.pBits);
		linearPitchSrc = (linearPitchSrc>>1);

		for( int j = 0; j < srcRect->bottom-srcRect->top; j++)
			for( int i = 0; i < srcRect->right-srcRect->left; i++ )
			{
				sour =  (USHORT)surfaceBufferSrc[i+j*linearPitchSrc];
				r=(sour>>rMove);
				g=(sour & gMask) >>gMove;
				b=sour & bMask;
				t=(r*3+g*6+b)/10;
				surfaceBufferSrc[i+j*linearPitchSrc] = (USHORT)((t<<rMove)|(t<<gMove)|t);
			}
	}
	else
		if( nColorDepth == 32 )
		{
			UINT *surfaceBufferSrc = static_cast<UINT*>(lockedRectBackBufferSrc.pBits);
			linearPitchSrc = (linearPitchSrc>>2);

			for( int j = 0; j < srcRect->bottom-srcRect->top; j++)
			for( int i = 0; i < srcRect->right-srcRect->left; i++ )
				{
					sour = surfaceBufferSrc[i+j*linearPitchSrc] & cMask;

					r=(sour>>rMove);
					g=(sour & gMask) >>gMove;
					b=sour & bMask;
					t=(r*3+g*6+b)/10;
					surfaceBufferSrc[i+j*linearPitchSrc] = surfaceBufferSrc[i+j*linearPitchSrc]&aMask | (t<<rMove)|(t<<gMove)|t;
				}
		}
	
	targetsuffer->UnlockRect();
	

	m_pDevice->StretchRect(targetsuffer, CRect(0,0,srcRect->right - srcRect->left, srcRect->bottom - srcRect->top),srcSurface, srcRect, D3DTEXF_NONE);
	targetsuffer->Release();
}

// 练习模式
void CD3DWnd::Execise(void)
{
	//---------------------------------------
	//初始基本状态

	m_bCanGame = true;
	m_bGameOver = false;
	m_ExerciseMode = true;

	m_bHitState = false;
	m_nHitCount = 0;
	m_nMouseState = MOUSE_NORMAL;


	//初始选择项
	m_nSelectFirst.x = ERROR_ALL;
	m_nSelectFirst.y = ERROR_ALL;
	m_nSelectSecond.x = ERROR_ALL;
	m_nSelectSecond.y = ERROR_ALL;

	//道具
	m_bStopIt  = false;

	m_nTimeCounter = 0;//

	m_ChessCountString[0] = 0;

	m_bGameLose[2] = false;
	//清除上次Lose屏幕缓存
	SAFE_RELEASE(m_pLoseSurface);
	//---------------------------------------

	//特效
	m_PointSprite_Star.On();
	::SendMessage(AfxGetMainWnd()->m_hWnd,IDM_D3D_ALLMUSIC,IDM_MUSIC_FLYSTAR,0);
	//请空棋盘
	for(int p = 0; p<GAME_PLAYER; p++)
	for(int i=0; i<CHESSCOUNT_H; ++i)
	for(int j=0; j<CHESSCOUNT_W; ++j)
	{
		m_Chess[p][i][j] = ERROR_ALL;
	}

	//------------------------------------------------
	//初始化棋盘
	int tempvalue,randnum;
	CArrayTemplate<CPoint> Map;

	int mapid = RandNumber(0,18);
		Map.RemoveAll();
		for(int i=0;i<CHESSCOUNT_H;i++)
		{
			for(int j=0;j<CHESSCOUNT_W;j++)
			{
				if(LLK_MAP[mapid][i][j])
				{
					Map.Add(CPoint(i,j));
				}
			}
		}
		
	//随地图
	for(; Map.GetCount()!=0;)
	{
		//随机一个棋子类型
		tempvalue = RandNumber(0,MAX_CHESS_VALUE);
		//找出第一个位置
		randnum = RandNumber(0,(int)Map.GetCount());
		m_Chess[2][Map.ElementAt(randnum).x][Map.ElementAt(randnum).y] = tempvalue;
		Map.RemoveAt(randnum);
		//找出第二个位置
		randnum = RandNumber(0,(int)Map.GetCount());
		m_Chess[2][Map.ElementAt(randnum).x][Map.ElementAt(randnum).y] = tempvalue;
		Map.RemoveAt(randnum);
	}
	AccountChessCount();
	UpdateBackSurface();
}

// 逃跑
void CD3DWnd::UserFlee(int userid)
{
	if(userid <0 && userid >= 4 || userid == 2)
		return;
	if(userid == 3)
		userid = 2;
	//所略棋盘背景
	m_pDevice->StretchRect(m_pBkSurface,CRect(150+(206+6)*userid,10,150+(206+6)*userid+206,10+180),m_pSurface,CRect(150+(206+6)*userid,10,150+(206+6)*userid+206,10+180),D3DTEXF_NONE);
}

