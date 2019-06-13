#include "StdAfx.h"
#include ".\sprite.h"

CSprite::CSprite(void)
{
	m_pSprite = NULL;
}

CSprite::~CSprite(void)
{
    if(m_pSprite != NULL)
	{
		m_pSprite->Release();
		m_pSprite = NULL;
	}

}

CSprite & CSprite::operator = (const CSprite & s)
{
	if(this == &s)return *this;
	//privete:
	m_RateModi		= 0;

	//punlic:
	m_bActive = s.m_bActive;
	m_bSingleFrame = s.m_bSingleFrame;
	m_bAutoLoop = s.m_bAutoLoop;
	m_bMultiPic = s.m_bMultiPic;

	
	m_nFrameRate	= s.m_nFrameRate;
	m_nFrameWidth	= s.m_nFrameWidth;
	m_nFrameHeight	= s.m_nFrameHeight;
	m_nFrameBegin	= s.m_nFrameBegin;
	m_nFrameEnd		= s.m_nFrameEnd	;	
	m_nLoopCount	= s.m_nLoopCount;	
	m_nWidthCount	= s.m_nWidthCount;	
	m_nCurrentFrame	= s.m_nCurrentFrame;	
	m_nSrcPosX		= s.m_nSrcPosX;
	m_nSrcPosY		= s.m_nSrcPosY;		
	m_nPosX			= s.m_nPosX	;		
	m_nPosY			= s.m_nPosY	;		

	memcpy(m_sType,s.m_sType,sizeof(s.m_sType));
	memcpy(m_sActiveLoadPath,s.m_sActiveLoadPath,sizeof(s.m_sActiveLoadPath));

	// 如果是动态载入则不需要传递纹理指针
	if(m_bMultiPic)
		m_pTexture = NULL;
	else
		m_pTexture = s.m_pTexture;
	return *this;

}

// 重置所有变量值
void CSprite::zeroSpriteValues(void)
{
	//privete:
	m_RateModi		= 0;

	//punlic:
	m_bActive = true;
	m_bSingleFrame = false;
	m_bAutoLoop = true;
	m_bMultiPic = false;

	
	m_nFrameRate	= 0;
	m_nFrameWidth	= 0;
	m_nFrameHeight	= 0;
	m_nFrameBegin	= 0;
	m_nFrameEnd		= 0;
	m_nLoopCount	= 1; //调用一次LoopIt(),循环的次数
	m_nWidthCount	= 1;
	m_nCurrentFrame	= 0;
	m_nSrcPosX		= 0;
	m_nSrcPosY		= 0;
	m_nPosX			= 0;
	m_nPosY			= 0;

	u				= 0.0f;
	v				= 0.0f;
	nu				= 1.0f;
	nv				= 1.0f;

	memset(m_sType,0,sizeof(m_sType));
	memset(m_sActiveLoadPath,0,sizeof(m_sActiveLoadPath));

	m_pTexture = NULL;
}

// 绑定纹理
bool CSprite::SetTexture(LPDIRECT3DTEXTURE9 pTexture)
{
	m_pTexture = pTexture;
	return false;
}

// 绘制精灵
void CSprite::DrawSprite(LPDIRECT3DDEVICE9 pDevice, float a)
{
	if(m_pSprite == NULL)
	{
		D3DXCreateSprite( pDevice, &m_pSprite );
	}
	if(IsNull())
		return;
	if(m_bActive == true)
	{
		if(m_bSingleFrame != true)
		{
			if(m_bAutoLoop == true && m_bMultiPic == false)
			{
				m_RateModi++;
				if(m_RateModi > m_nFrameRate)
				{
					m_nCurrentFrame ++;
					if(m_nCurrentFrame >= m_nFrameEnd)
					{
						m_nCurrentFrame = m_nFrameBegin;
						m_bActive = false;
					}
					m_RateModi = 0;
				}
			}
		}

		if(m_bMultiPic)
		{
			srcRect.top    = (LONG)(m_nSrcPosY + v*m_nFrameHeight);
			srcRect.left   = (LONG)(m_nSrcPosX + u*m_nFrameWidth);
			srcRect.bottom = (LONG)(srcRect.top  + m_nFrameHeight -(1-nv)*m_nFrameHeight);
			srcRect.right  = (LONG)((srcRect.left + m_nFrameWidth) - (1-nu)*m_nFrameWidth);
		}
		else
		{
			srcRect.top    = (LONG)(m_nSrcPosY+(( m_nCurrentFrame / m_nWidthCount ) * m_nFrameHeight) + v*m_nFrameHeight);
			srcRect.left   = (LONG)(m_nSrcPosX+(( m_nCurrentFrame % m_nWidthCount ) * m_nFrameWidth) +u*m_nFrameWidth);
			srcRect.bottom = (LONG)(srcRect.top  + m_nFrameHeight -(1-nv)*m_nFrameHeight);
			srcRect.right  = (LONG)(srcRect.left + m_nFrameWidth - (1-nu)*m_nFrameWidth);
		}

		vCenter = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		vPosition = D3DXVECTOR3(m_nPosX, m_nPosY, 0.0f );

		m_pSprite->Begin( D3DXSPRITE_ALPHABLEND );

		m_pSprite->Draw( m_pTexture,
			&srcRect,
			&vCenter,
			&vPosition,
			D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,a) );
		m_pSprite->End();

		u=0.0f;v=0.0f;nu=1.0f;nv=1.0f;
	}
}

// 碰撞测试
bool CSprite::CollisionTest(CSprite & pOtherSprite)
{
	 // Get the radi of each rect
	int width1  = (m_nFrameWidth>>1) - (m_nFrameWidth>>3);
    int height1 = (m_nFrameHeight>>1) - (m_nFrameHeight>>3);

    int width2  = (pOtherSprite.m_nFrameWidth>>1) - (pOtherSprite.m_nFrameWidth>>3);
	int height2 = (pOtherSprite.m_nFrameHeight>>1) - (pOtherSprite.m_nFrameHeight>>3);

    // Compute center of each rect
    int cx1 = (int)(m_nPosX + width1);
    int cy1 = (int)(m_nPosX + height1);

    int cx2 = (int)(pOtherSprite.m_nPosX + width2);
    int cy2 = (int)(pOtherSprite.m_nPosX + height2);

    // Compute deltas
    int dx = (int)(abs(cx2 - cx1));
    int dy = (int)(abs(cy2 - cy1));

    if(dx < (width1+width2) && dy < (height1+height2))
        return true;
    else 
        return false;

	return false;
}

// 循环一次
void CSprite::LoopIt(void)
{
	if(m_bActive)
	{
		m_nCurrentFrame ++;
		if(m_nCurrentFrame >= m_nFrameEnd)
		{
			m_nCurrentFrame = m_nFrameBegin;
			m_bActive = false;
		}
	}
}

// 动态载入产生循环
void CSprite::ActiveLoopIt( LPDIRECT3DDEVICE9 pDevice)
{
	char buffer[30];
	char temp[10];

	memset(buffer,false,sizeof(buffer));
	if(m_sActiveLoadPath == NULL)
		return ;
	if(m_bActive)
	{
		m_nCurrentFrame ++;
		if(m_pTexture != NULL)
			m_pTexture->Release();
		strcat(buffer,m_sActiveLoadPath);
		_snprintf(temp,10,TEXT("%02d.tga"),m_nCurrentFrame);
        strcat(buffer,temp);
		D3DXCreateTextureFromFileEx( pDevice,
								buffer,
                                 50, // I had to set width manually. D3DPOOL_DEFAULT works for textures but causes problems for D3DXSPRITE.
                                 50, // I had to set height manually. D3DPOOL_DEFAULT works for textures but causes problems for D3DXSPRITE.
                                 1,   // Don't create mip-maps when you plan on using D3DXSPRITE. It throws off the pixel math for sprite animation.
                                 0,
                                 D3DFMT_UNKNOWN,
                                 D3DPOOL_DEFAULT,	
                                 D3DX_DEFAULT,
                                 D3DX_DEFAULT,
                                 D3DCOLOR_COLORVALUE(0.0f,0.0f,0.0f,1.0f),
                                 NULL,
                                 NULL,
                                 &m_pTexture );
		if(m_nCurrentFrame >= m_nFrameEnd)
		{
			m_nCurrentFrame = m_nFrameBegin;
			m_bActive = false;
			m_pTexture->Release();
			m_pTexture = NULL;
		}
	}

}

// 是否存在
bool CSprite::IsNull(void)
{
	if(m_pTexture == NULL || m_pSprite == NULL)
		return true;
	else
		return false;
}
