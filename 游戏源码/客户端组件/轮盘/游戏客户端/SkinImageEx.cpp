#include "StdAfx.h"
#include "SkinImageEx.h"

/////////////////////////////////////////////////////////////////////////////////////////

//构造函数
CSkinImageEx::CSkinImageEx(void)
{
	m_pTexture=NULL;
	m_pD3DDevice=NULL;
	m_pVertexBuffer = NULL;

	m_nImageWidth = 0;
	m_nImageHeight = 0;
	m_nScreenWidth = 0;
	m_nScreenHeight = 0;
	m_dwColour = -1;
}

//析构函数
CSkinImageEx::~CSkinImageEx(void)
{
	
}

//是否设置加载信息
bool CSkinImageEx::IsSetLoadInfo()
{
	return true;
}

//清除加载参数
bool CSkinImageEx::RemoveLoadInfo()
{
	return true;
}

//获取加载参数
bool CSkinImageEx::GetLoadInfo()
{
	return true;
}

//创建顶点缓冲
bool CSkinImageEx::CreateVertexBuffer()
{
    if(FAILED(m_pD3DDevice->CreateVertexBuffer(4 * sizeof(PANEL_CUSTOMVERTEX),0, D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1, D3DPOOL_DEFAULT, &m_pVertexBuffer,NULL)))	return false;
    return true;
}
//更新顶点
bool CSkinImageEx::UpdateVertices()
{
	PANEL_CUSTOMVERTEX* pVertices = NULL;
	m_pVertexBuffer->Lock(0, 4 * sizeof(PANEL_CUSTOMVERTEX), (void**)&pVertices, 0);
	
	//设置颜色
	if(m_dwColour == -1)	m_dwColour = D3DCOLOR_XRGB(255, 255, 255);

	//把所有的顶点初始化颜色
	pVertices[0].colour = m_dwColour;	pVertices[1].colour = m_dwColour;
	pVertices[2].colour = m_dwColour;	pVertices[3].colour = m_dwColour;
	
	//设置位置
	pVertices[0].x = -(m_nImageWidth) / 2.0f;	pVertices[0].y = -(m_nImageHeight) / 2.0f;
	pVertices[1].x = -(m_nImageWidth) / 2.0f;	pVertices[1].y = m_nImageHeight / 2.0f;
	pVertices[2].x = (m_nImageWidth) / 2.0f;	pVertices[2].y = -(m_nImageHeight) / 2.0f;
	pVertices[3].x = (m_nImageWidth) / 2.0f;	pVertices[3].y = m_nImageHeight / 2.0f;
	pVertices[0].z = 1.0f;					pVertices[1].z = 1.0f;
	pVertices[2].z = 1.0f; 					pVertices[3].z = 1.0f;

	//设置纹理
	pVertices[0].u = 0.0f;		pVertices[0].v = 1.0f;
	pVertices[1].u = 0.0f;		pVertices[1].v = 0.0f;
	pVertices[2].u = 1.0f;		pVertices[2].v = 1.0f;
	pVertices[3].u = 1.0f;		pVertices[3].v = 0.0f;
	m_pVertexBuffer->Unlock();

	return true;
}

//设置加载参数
bool CSkinImageEx::SetLoadInfo(LPDIRECT3DDEVICE9 pD3DDevice, LPCTSTR pszFileName, int ScreenWidth,int ScreenHeight)
{
	//取得设备
	m_pD3DDevice=pD3DDevice;


	D3DXGetImageInfoFromFile(pszFileName, &m_ImageInfo);

	//取得图片与显示区域大小
	m_nImageWidth = m_ImageInfo.Width;
	m_nImageHeight = m_ImageInfo.Height;
	m_nScreenWidth = ScreenWidth;
	m_nScreenHeight = ScreenHeight;

	//更新顶点
	CreateVertexBuffer();
	UpdateVertices();

	//载入图片纹理
	D3DXCreateTextureFromFileEx(pD3DDevice, pszFileName, 0, 0, 0, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT,D3DX_DEFAULT, D3DCOLOR_XRGB(255, 0, 255), NULL, NULL, &m_pTexture);
	
	if(m_pTexture==NULL) throw 0 ;

	return true;
}

//设置加载参数 测试中。。
bool CSkinImageEx::SetLoadInfo(LPDIRECT3DDEVICE9 pD3DDevice, UINT uResourceID, int ScreenWidth,int ScreenHeight,HINSTANCE hResourceDLL)
{
	//取得设备
	m_pD3DDevice=pD3DDevice;

	D3DXGetImageInfoFromResource(GetModuleHandle( NULL ),MAKEINTRESOURCE(uResourceID),&m_ImageInfo);

	//取得图片与显示区域大小
	m_nImageWidth = m_ImageInfo.Width;
	m_nImageHeight = m_ImageInfo.Height;
	m_nScreenWidth = ScreenWidth;
	m_nScreenHeight = ScreenHeight;

	//更新顶点
	CreateVertexBuffer();
	UpdateVertices();

	//载入纹理
	HRESULT	rslt = D3DXCreateTextureFromResourceEx(m_pD3DDevice, GetModuleHandle( NULL ), MAKEINTRESOURCE(uResourceID),
									m_nImageWidth,					
									m_nImageHeight,
									  1,
									  0,
									  D3DFMT_UNKNOWN, 
									  D3DPOOL_DEFAULT,
									  D3DX_DEFAULT,
									  D3DX_DEFAULT,
                                      D3DCOLOR_XRGB(255, 0, 255),
									  NULL,
									  NULL, 
									  &m_pTexture);

	if(FAILED(rslt)) return false;
	return true;
}

//释放函数
void CSkinImageEx::Release()
{
	SAFE_RELEASE(m_pTexture);
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pD3DDevice);
}

//设置2维摄象机
void CSkinImageEx::SetupImageCamera()
{
	D3DXMATRIX matOrtho;	
	D3DXMATRIX matIdentity;
	
	//正交投影
	D3DXMatrixOrthoLH(&matOrtho, (float)m_nScreenWidth, (float)m_nScreenHeight, 0.0f, 1.0f);
	D3DXMatrixIdentity(&matIdentity);

	//坐标变换
	m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &matOrtho);
	m_pD3DDevice->SetTransform(D3DTS_WORLD, &matIdentity);
	m_pD3DDevice->SetTransform(D3DTS_VIEW, &matIdentity);

	//关闭灯光与深度缓冲
	m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
}

//移动位置
void CSkinImageEx::MoveTo(int x, int y)
{
	//移动二维窗口的位置
	D3DXMATRIX matMove;
	x -= (m_nScreenWidth / 2) - (m_nImageWidth / 2);
	y -= (m_nScreenHeight / 2) - (m_nImageHeight / 2);
	D3DXMatrixTranslation(&matMove, (float)x, -(float)y, 0.0f);
	m_pD3DDevice->SetTransform(D3DTS_WORLD, &matMove);
	return;
}

//渲染图片
void CSkinImageEx::AlphaDrawImage(int x,int y)
{
	SetupImageCamera();
	MoveTo(x,y);

	//在后台缓冲区绘制图形
	m_pD3DDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(PANEL_CUSTOMVERTEX));
	m_pD3DDevice->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1);
	if(m_pTexture != NULL)
	{
		m_pD3DDevice->SetTexture(0, m_pTexture);
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	}
	else
	{
		m_pD3DDevice->SetTexture(0, NULL);
	}
	m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	//打开灯光与深度缓冲
	m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	return;
}


///////////////////////////////////////////////////////////////////////////////////////////
//构造函数
CSkinSurface::CSkinSurface()
{
	m_Surface=NULL;
	m_pD3DDevice=NULL;
	m_backbuffer=NULL;
	ZeroMemory(&m_ImageInfo,sizeof(m_ImageInfo));
}

//析构函数
CSkinSurface::~CSkinSurface()
{
	SAFE_RELEASE(m_Surface);
	SAFE_RELEASE(m_backbuffer);
	SAFE_RELEASE(m_pD3DDevice);
}

//加载函数
bool CSkinSurface::SetLoadInfo(LPDIRECT3DDEVICE9 pD3DDevice, LPCTSTR pszFileName,D3DFORMAT Format)
{
	//保存设备
	if(pD3DDevice==NULL) return false;
	m_pD3DDevice=pD3DDevice;

	//加载图片
	D3DXGetImageInfoFromFile(pszFileName, &m_ImageInfo);
	pD3DDevice->CreateOffscreenPlainSurface(m_ImageInfo.Width, m_ImageInfo.Height,Format,D3DPOOL_DEFAULT, &m_Surface, NULL);
	D3DXLoadSurfaceFromFile(m_Surface, NULL, NULL, pszFileName, NULL, D3DX_FILTER_NONE, D3DCOLOR_ARGB(0x80,0xff,0,0xff), NULL);

	if(m_Surface==NULL) throw 0 ;
	return true;
}

//加载函数
bool CSkinSurface::SetLoadInfo(LPDIRECT3DDEVICE9 pD3DDevice, UINT uResourceID,D3DFORMAT Format)
{
	//保存设备
	m_pD3DDevice=pD3DDevice;

	return true;
}
//释放函数
void CSkinSurface::Release()
{

	SAFE_RELEASE(m_Surface);
	SAFE_RELEASE(m_backbuffer);
	SAFE_RELEASE(m_pD3DDevice);
}

//绘画函数
void CSkinSurface::DrawSurface(int x,int y,bool drawAsBackGround)
{
    m_pD3DDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &m_backbuffer ); 
	if(drawAsBackGround)
		m_pD3DDevice->StretchRect(m_Surface,NULL,m_backbuffer,NULL,D3DTEXF_NONE);
	else
		m_pD3DDevice->StretchRect(m_Surface,NULL,m_backbuffer,CRect(x,y,x+m_ImageInfo.Width,y+m_ImageInfo.Height),D3DTEXF_NONE);
}

