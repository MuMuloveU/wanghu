#pragma once
////////------------------------------------------------------------
#include "d3d9.h"
#include <d3dx9.h>
#include "d3dx9math.h"

struct Vertex
{
    D3DXVECTOR3 posit;
    D3DCOLOR    color;

	enum FVF
	{
		FVF_Flags = D3DFVF_XYZ|D3DFVF_DIFFUSE
	};
};

struct Particle
{
    D3DXVECTOR3 m_vCurPos;
    D3DXVECTOR3 m_vCurVel;
	D3DCOLOR    m_vColor;
};

const int MAX_PARTICLES = 100;

inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }
class CPointSprite
{
public:
	bool	m_bActive;
	bool	m_bFirstFrame;
	Particle g_particles[MAX_PARTICLES];	 //顶点精灵容器

	//--------------------
	LPDIRECT3DDEVICE9		m_pDevice;		 //设备接口指针
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer ;//顶点缓存区
	LPDIRECT3DTEXTURE9      m_pTexture      ;//纹理接口

public:
	CPointSprite(void);
	~CPointSprite(void);

	//----------------
	// 随机函数
	float getRandomMinMax( float fMin, float fMax );
	// 随机函数
	D3DXVECTOR3 getRandomVector( void );
	// 初始化顶点精灵
	void initPointSprites(LONG RCID);
	// 更新顶点位置
	bool updatePointSprites( void );
	// 绘制
	void renderPointSprites( void );
	// 绘制接口
	void DrawPointSprite(void);
	// 初始
	void zeroSpriteValues(void);
	// 设置顶点精灵参数
	void SetPointSprite(LPDIRECT3DDEVICE9		pDevice, LONG RCID);
	// 开按钮
	void On(void);
	// 关按钮
	void Off(void);
};
