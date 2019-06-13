#include "StdAfx.h"
#include ".\pointsprite.h"
#include "mmsystem.h"

CPointSprite::CPointSprite(void)
{
	zeroSpriteValues();
}

CPointSprite::~CPointSprite(void)
{
	m_pVertexBuffer->Release();
	m_pTexture->Release();

}

// 初始
void CPointSprite::zeroSpriteValues(void)
{
	m_bActive = FALSE;
	m_bFirstFrame = FALSE;
}

float CPointSprite::getRandomMinMax( float fMin, float fMax )
{
    float fRandNum = (float)rand () / RAND_MAX;
    return fMin + (fMax - fMin) * fRandNum;
}
D3DXVECTOR3 CPointSprite::getRandomVector( void )
{
	D3DXVECTOR3 vVector;

    // Pick a random Z between -1.0f and 1.0f.
    vVector.z = getRandomMinMax( -1.0f, 1.0f );
    
    // Get radius of this circle
    float radius = (float)sqrt(1 - vVector.z * vVector.z);
    
    // Pick a random point on a circle.
    float t = getRandomMinMax( -D3DX_PI, D3DX_PI );

    // Compute matching X and Y for our Z.
    vVector.x = (float)cosf(t) * radius;
    vVector.y = (float)sinf(t) * radius;

	return vVector;
}

void CPointSprite::initPointSprites( LONG RCID )
{
	//
	// Load up the point sprite's texture...
	//

//	D3DXCreateTextureFromFile( m_pDevice, "particle.bmp", &g_pTexture );
	HRESULT rslt = D3DXCreateTextureFromResourceEx(m_pDevice, GetModuleHandle( NULL ), MAKEINTRESOURCE((RCID)),
                                      63, 
									  63, 
									  1,
									  0,
                                      D3DFMT_UNKNOWN, 
									  D3DPOOL_MANAGED,
									  D3DX_DEFAULT,
									  D3DX_DEFAULT,
                                      D3DCOLOR_COLORVALUE(0.0f,0.0f,0.0f,1.0f),
									  NULL,
									  NULL, 
									  &m_pTexture);
	if(FAILED(rslt)) {return;}

	m_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	//
	// Create a vertex bufer which can be used with point sprites...
	//

     m_pDevice->CreateVertexBuffer( 2048 * sizeof(Vertex), 
                                      D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY | D3DUSAGE_POINTS, 
									  Vertex::FVF_Flags, D3DPOOL_DEFAULT, 
									  &m_pVertexBuffer, NULL );

	//
    // If you want to know the max size that a point sprite can be set to, 
	// and whether or not you can change the size of point sprites in hardware 
	// by sending D3DFVF_PSIZE with the FVF, do this.
	//

	float fMaxPointSize = 0.0f;
	bool  bDeviceSupportsPSIZE = false;
	
    D3DCAPS9 d3dCaps;
    m_pDevice->GetDeviceCaps( &d3dCaps );

    fMaxPointSize = d3dCaps.MaxPointSize;

    if( d3dCaps.FVFCaps & D3DFVFCAPS_PSIZE )
        bDeviceSupportsPSIZE = true;
    else
        bDeviceSupportsPSIZE = false;

	//
	// Initialize our particles so they'll start at the origin with some 
	// random direction and color.
	//

	for( int i = 0; i < MAX_PARTICLES; ++i )
    {
		g_particles[i].m_vCurPos = D3DXVECTOR3(0.0f,0.0f,0.0f);
		g_particles[i].m_vCurVel = getRandomVector() * getRandomMinMax( 0.3f, 7.0f );

		g_particles[i].m_vColor = D3DCOLOR_COLORVALUE( getRandomMinMax( 0.0f, 1.0f ), 
                                                       getRandomMinMax( 0.0f, 1.0f ), 
                                                       getRandomMinMax( 0.0f, 1.0f ), 
                                                       1.0f );
	}
}
bool CPointSprite::updatePointSprites( void )
{
	//
	// To repeat the sample automatically, keep track of the overall app time.
	//

	static double dStartAppTime = timeGetTime();
	static double dLastFrameTime = timeGetTime();

	if(m_bFirstFrame)
	{
		dStartAppTime = timeGetTime();
		dLastFrameTime = timeGetTime();
		m_bFirstFrame = false;
	}

	float fElpasedAppTime = (float)((timeGetTime() - dStartAppTime) * 0.001);

	//
	// To move the particles via their velocity, keep track of how much time
	// has elapsed since last frame update...
	//

	
	double dCurrenFrameTime = timeGetTime();
	double dElpasedFrameTime = (float)((dCurrenFrameTime - dLastFrameTime) * 0.001);
	dLastFrameTime = dCurrenFrameTime;

	//
	// After 5 seconds, repeat the sample by returning all the particles 
	// back to the origin.
	//

    if( fElpasedAppTime >= 4.0f )
	{
		srand((unsigned int)time(0));
		Off();		
		return false;
	}
	
	//
	// Move each particle via its velocity and elapsed frame time.
	//
	
	for( int i = 0; i < MAX_PARTICLES; ++i )
		g_particles[i].m_vCurPos += g_particles[i].m_vCurVel * (float)dElpasedFrameTime;
	return true;

}

void CPointSprite::renderPointSprites( void )
{
	//
	// Setting D3DRS_ZWRITEENABLE to FALSE makes the Z-Buffer read-only, which 
	// helps remove graphical artifacts generated from  rendering a list of 
	// particles that haven't been sorted by distance to the eye.
	//
    // Setting D3DRS_ALPHABLENDENABLE to TRUE allows particles, which overlap, 
	// to alpha blend with each other correctly.
	//

    m_pDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

    m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

	//
	// Set up the render states for using point sprites...
	//

    m_pDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );    // Turn on point sprites
    m_pDevice->SetRenderState( D3DRS_POINTSCALEENABLE,  TRUE );    // Allow sprites to be scaled with distance
    m_pDevice->SetRenderState( D3DRS_POINTSIZE,     FtoDW(1.0) );  // Float value that specifies the size to use for point size computation in cases where point size is not specified for each vertex.
    m_pDevice->SetRenderState( D3DRS_POINTSIZE_MIN, FtoDW(1.0f) ); // Float value that specifies the minimum size of point primitives. Point primitives are clamped to this size during rendering. 
    m_pDevice->SetRenderState( D3DRS_POINTSCALE_A,  FtoDW(0.0f) ); // Default 1.0
    m_pDevice->SetRenderState( D3DRS_POINTSCALE_B,  FtoDW(0.0f) ); // Default 0.0
    m_pDevice->SetRenderState( D3DRS_POINTSCALE_C,  FtoDW(1.0f) ); // Default 0.0

	//
	// Lock the vertex buffer, and set up our point sprites in accordance with 
	// our particles that we're keeping track of in our application.
	//

	Vertex *pPointVertices;

	m_pVertexBuffer->Lock( 0, MAX_PARTICLES * sizeof(Vertex),
		                   (void**)&pPointVertices, D3DLOCK_DISCARD );

	for( int i = 0; i < MAX_PARTICLES; ++i )
    {
        pPointVertices->posit = g_particles[i].m_vCurPos;
        pPointVertices->color = g_particles[i].m_vColor;
        pPointVertices++;
    }

    m_pVertexBuffer->Unlock();
	
	//
	// Render point sprites...
	//

    m_pDevice->SetStreamSource( 0, m_pVertexBuffer, 0, sizeof(Vertex) );
    m_pDevice->SetFVF( Vertex::FVF_Flags );
	m_pDevice->DrawPrimitive( D3DPT_POINTLIST, 0, MAX_PARTICLES );

	//
    // Reset render states...
	//
	
    m_pDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, FALSE );
    m_pDevice->SetRenderState( D3DRS_POINTSCALEENABLE,  FALSE );

    m_pDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
    m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
}
void CPointSprite::DrawPointSprite(void)
{
	if(!m_bActive)
		return;
	D3DXMATRIX matTrans;
	D3DXMATRIX matRot;
	D3DXMATRIX matWorld;

    D3DXMatrixTranslation( &matTrans, 0.0f, 0.0f, 10.0f );

	/*
	//旋转
	D3DXMatrixRotationYawPitchRoll( &matRot, 
		                            D3DXToRadian(0.0f), 
		                            D3DXToRadian(0.0f), 
		                            0.0f );
									*/
    //matWorld = matRot * matTrans;

	matWorld = matTrans;
    m_pDevice->SetTransform( D3DTS_WORLD, &matWorld );

	if(!updatePointSprites())
		return;

	m_pDevice->SetTexture( 0, m_pTexture );

	renderPointSprites();
}


// 设置顶点精灵参数
void CPointSprite::SetPointSprite(LPDIRECT3DDEVICE9		pDevice, LONG RCID)
{
	m_pDevice = pDevice;
	initPointSprites( RCID );
}

// 开按钮
void CPointSprite::On(void)
{
	m_bActive = TRUE;
	m_bFirstFrame = TRUE;

}

// 关按钮
void CPointSprite::Off(void)
{
	m_bActive = FALSE;
	m_bFirstFrame = FALSE;

	for( int i = 0; i < MAX_PARTICLES; ++i )
	{
		g_particles[i].m_vCurPos = D3DXVECTOR3(0.0f,0.0f,0.0f);
		g_particles[i].m_vCurVel = getRandomVector() * getRandomMinMax( 0.3f, 7.0f );

		g_particles[i].m_vColor = D3DCOLOR_COLORVALUE( getRandomMinMax( 0.0f, 1.0f ), 
													getRandomMinMax( 0.0f, 1.0f ), 
													getRandomMinMax( 0.0f, 1.0f ), 
													1.0f );
	}
}
