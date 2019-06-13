#include "StdAfx.h"
#include "d3dimagepro.h"
#include ".\d3dimagepro.h"

CD3DImagePro::CD3DImagePro(void)
{
	x = 0;
	y = 0;
	m_pVB = NULL;
	m_pTexture = NULL;
}

CD3DImagePro::~CD3DImagePro(void)
{
	if(m_pVB != NULL)
	{
        m_pVB->Release();
		m_pVB = NULL;
	}
}

//设置纹理
void CD3DImagePro::SetTexture(float u, float v, float nu, float nv)
{
	vertices[0].tu = u;
	vertices[0].tv = v;
	vertices[1].tu = nu;
	vertices[1].tv = v;
	vertices[2].tu = u;
	vertices[2].tv = nv;
	vertices[3].tu = nu;
	vertices[3].tv = nv;

	Update();
}

//
void CD3DImagePro::CreateVertexBuffer(LPDIRECT3DDEVICE9 m_ppDevice,
		float nx,float ny,float Width,float Height)
{
	HRESULT rslt;
	//建立位图矩形数学模型
	vertices[0].x = nx;
	vertices[0].y = ny;
	vertices[0].z = 0.0f;
	vertices[0].rhw = 1.0f;
	vertices[0].color = D3DCOLOR_XRGB(255,255,255);
	vertices[0].tu = 0.0f;
	vertices[0].tv = 0.0f;

	vertices[1].x = nx+Width;
	vertices[1].y = ny;
	vertices[1].z = 0.0f;
	vertices[1].rhw = 1.0f;
	vertices[1].color = D3DCOLOR_XRGB(255,255,255);
	vertices[1].tu = 0.0f;
	vertices[1].tv = 0.0f;

	vertices[2].x = nx;
	vertices[2].y = ny+Height;
	vertices[2].z = 0.0f;
	vertices[2].rhw = 1.0f;
	vertices[2].color = D3DCOLOR_XRGB(255,255,255);
	vertices[2].tu = 0.0f;
	vertices[2].tv = 0.0f;

	vertices[3].x = nx+Width;
	vertices[3].y = ny+Height;
	vertices[3].z = 0.0f;
	vertices[3].rhw = 1.0f;
	vertices[3].color = D3DCOLOR_XRGB(255,255,255);
	vertices[3].tu = 0.0f;
	vertices[3].tv = 0.0f;

	rslt = m_ppDevice->CreateVertexBuffer(4*sizeof(CUSTOMVERTEX),
		0,D3DFVF_CUSTOMVERTEX,D3DPOOL_DEFAULT,&m_pVB,NULL);

	if(FAILED(rslt)) {MessageBox(NULL,TEXT("创建顶点缓存区失败！"),"",MB_OK);}
}

//移动图片
void CD3DImagePro::MoveImage(float x, float y, float width ,float height)
{
	vertices[0].x = x;
	vertices[0].y = y;
	vertices[1].x = x+width;
	vertices[1].y = y;
	vertices[2].x = x;
	vertices[2].y = y+height;
	vertices[3].x = x+width;
	vertices[3].y = y+height;

	//Update();
}

// 更新缓存
void CD3DImagePro::Update(void)
{
	void *pvertices;
	m_pVB->Lock(0,sizeof(vertices),(void**)&pvertices,D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK);
	memcpy(pvertices,vertices,sizeof(vertices));
	m_pVB->Unlock();
}

// 绘制
void CD3DImagePro::BitBlt(LPDIRECT3DDEVICE9 m_pDevice)
{
	if(IsNull())
		return ;
	//加载纹理
	m_pDevice->SetTexture(0,m_pTexture);
	m_pDevice->SetStreamSource(0,m_pVB,0,sizeof(CUSTOMVERTEX));
	m_pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,2);
}

//绑定纹理
void CD3DImagePro::SetTexture(LPDIRECT3DTEXTURE9 pTexture,float u, float v, float nu, float nv)
{
	m_pTexture = pTexture;
	vertices[0].tu = u;
	vertices[0].tv = v;
	vertices[1].tu = nu;
	vertices[1].tv = v;
	vertices[2].tu = u;
	vertices[2].tv = nv;
	vertices[3].tu = nu;
	vertices[3].tv = nv;

	Update();
}

//拷贝纹理及坐标
void CD3DImagePro::CopyTexture(CD3DImagePro &img)
{
	m_pTexture = img.GetTexture();
	vertices[0].tu = img.vertices[0].tu ;
	vertices[0].tv = img.vertices[0].tv ;
	vertices[1].tu = img.vertices[1].tu ;
	vertices[1].tv = img.vertices[1].tv ;
	vertices[2].tu = img.vertices[2].tu ;
	vertices[2].tv = img.vertices[2].tv ;
	vertices[3].tu = img.vertices[3].tu ;
	vertices[3].tv = img.vertices[3].tv ;
	Update();
}

// 清除纹理
void CD3DImagePro::ClearTexture(void)
{
	m_pTexture = NULL;
	vertices[0].tu = 0;
	vertices[0].tv = 0;
	vertices[1].tu = 0;
	vertices[1].tv = 0;
	vertices[2].tu = 0;
	vertices[2].tv = 0;
	vertices[3].tu = 0;
	vertices[3].tv = 0;
	Update();
}

// 图像是否为空
bool CD3DImagePro::IsNull(void)
{
	if(m_pTexture == NULL)
		return true;
	else
		return false;
}

// 获取纹理缓存区指针
LPDIRECT3DTEXTURE9 CD3DImagePro::GetTexture(void)
{
	return m_pTexture;
}
