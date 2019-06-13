#pragma once

struct SingleStar
{
	D3DXVECTOR3 posit;
    D3DCOLOR    color;

	enum FVF
	{
		FVF_Flags = D3DFVF_XYZ|D3DFVF_DIFFUSE
	};
};


class CStarSprite
{
private:
	CPoint		m_SrcPoint;
	CPoint		m_DesPoint;

public:
	CStarSprite(void);
	~CStarSprite(void);

	LPDIRECT3DDEVICE9		m_pDevice;		 //设备接口指针
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer ;//顶点缓存区
	LPDIRECT3DTEXTURE9      m_pTexture      ;//纹理接口

	// 运动动作
	void SetRunPos(CPoint src, CPoint src);

	// 绘制接口
	void DrawPointSprite(void);
	// 初始
	void zeroSpriteValues(void);
	// 设置顶点精灵参数
	void SetPointSprite(LPDIRECT3DDEVICE9		pDevice, LONG RCID);
	void initPointSprites( LONG RCID )
};
