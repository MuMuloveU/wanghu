#include "StdAfx.h"
#include "SkinSprite.h"


//////////////////////////////////////////////////////////////////////////////////////

//构造函数
CSkinSprite::CSkinSprite()
{
	m_Sprite=NULL;
	m_pTexture=NULL;
	offsetX=0;offsetY=0;
	m_StartX=0;m_StartY=0;m_StopX=0;m_StopY=0;//移动路径
	ZeroMemory(&m_ImageInfo,sizeof(m_ImageInfo));
}

//析构函数
CSkinSprite::~CSkinSprite()
{
	SAFE_RELEASE(m_Sprite);
	SAFE_RELEASE(m_pTexture);
}

//////////////////////////////////////////////////////////////////////////////////////
//释放函数
void CSkinSprite::Release()
{
	SAFE_RELEASE(m_Sprite);
	SAFE_RELEASE(m_pTexture);
}

//加载函数
void CSkinSprite::SetLoadInfo(LPDIRECT3DDEVICE9	pD3DDevice,LPCTSTR szFileName,D3DCOLOR dwKeyColor)
{
	//创建精灵
	D3DXCreateSprite( pD3DDevice, &m_Sprite);
	//加载纹理
	D3DXCreateTextureFromFileEx(pD3DDevice,	szFileName, 0, 0, 0, 0,
		D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_FILTER_NONE,D3DX_DEFAULT,	dwKeyColor, NULL, NULL,
		&m_pTexture);

	if(m_Sprite==NULL||m_pTexture==NULL) throw 0 ;
	//获取信息
	D3DXGetImageInfoFromFile(szFileName, &m_ImageInfo);
}
//加载函数
void CSkinSprite::SetLoadInfo(LPDIRECT3DDEVICE9	pD3DDevice,UINT uResourceID,D3DCOLOR dwKeyColor)
{
	//创建精灵
	D3DXCreateSprite( pD3DDevice, &m_Sprite);
	//加载纹理
	D3DXCreateTextureFromResourceEx(pD3DDevice,	NULL,MAKEINTRESOURCE(uResourceID), 0, 0, 0, 0,
		D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_FILTER_NONE,D3DX_DEFAULT,	dwKeyColor, NULL, NULL,
		&m_pTexture);
	if(m_Sprite==NULL||m_pTexture==NULL) throw 0 ;
	//获取信息
	D3DXGetImageInfoFromResource(GetModuleHandle(NULL),MAKEINTRESOURCE(uResourceID),&m_ImageInfo);
}

//设置图片移移路径
void CSkinSprite::SetMovePath(int StartX,int StartY,int StopX,int StopY)
{
	//设置变量
	m_StartX=StartX;	m_StartY=StartY;
	m_StopX=StopX;		m_StopY=StopY;
	Setoffset();
	return ;
}
//设置移动步进
void CSkinSprite::Setoffset()
{
	//设置默认值
	offsetX=5;
	offsetY=5;	

	//计算偏移量
	int xp=abs(m_StopX-m_StartX);//x偏移数值
	int yp=abs(m_StopY-m_StartY);//y偏移数值

	//当x与y方向上的移动量不同时重新设定步进
	if(xp!=yp&&xp!=0&&yp!=0)
	{
		//ASSERT(FALSE);
		int longRange=xp>yp?xp:yp;//长距离
		int shortRange=xp>yp?yp:xp;//短距离

		int longTime=longRange/5;//移动长距离所需时间

		int xspeed=xp/longTime;//x速度
		int yspeed=yp/longTime;//y速度

		offsetX=xspeed;
		offsetY=yspeed;
	}
}
//绘画移动动画
bool CSkinSprite::DrawMoveSprite()
{
	//图片未移动到指定位置 返回false
	if(m_StartX!=m_StopX||m_StartY!=m_StopY)
	{
		if(m_StartX<m_StopX) m_StartX+=offsetX;
		if(m_StartX>m_StopX) m_StartX-=offsetX;
		if(m_StartY<m_StopY) m_StartY+=offsetY;
		if(m_StartY>m_StopY) m_StartY-=offsetY;
		DrawSprite(m_StartX,m_StartY);
		return false;
	}
	//渲染最终位置
	DrawSprite(m_StopX,m_StopY);
	//渲染完成返回true
	return true;
}
//绘画函数
void CSkinSprite::DrawSprite(int x,int y,D3DCOLOR channelColor)
{
	//绘画位置
	D3DXVECTOR3 drawAtPoint((float)x,(float)y,0);
	m_Sprite->Begin(D3DXSPRITE_ALPHABLEND);
	m_Sprite->Draw(m_pTexture, CRect(0,0,m_ImageInfo.Width,m_ImageInfo.Height), NULL, &drawAtPoint, channelColor);
	m_Sprite->Flush();
	m_Sprite->End();
}

//绘画函数
void CSkinSprite::DrawSprite(int x,int y,D3DXVECTOR3 vCenterPoint,CRect drawRect,D3DXMATRIX *matRot)
{
	//绘画位置
	D3DXVECTOR3 drawAtPoint((float)x,(float)y,0);
	m_Sprite->Begin(D3DXSPRITE_ALPHABLEND);
	m_Sprite->SetTransform(matRot);
	m_Sprite->Draw(m_pTexture, drawRect, &vCenterPoint, &drawAtPoint, 0xffffffff);
	m_Sprite->Flush();
	m_Sprite->End();
}

//HRESULT Draw(
//　　LPDIRECT3DTEXTURE9 pTexture,
//　　CONST RECT *pSrcRect,
//　　CONST D3DXVECTOR3 *pCenter,
//　　CONST D3DXVECTOR3 *pPosition,
//　　D3DCOLOR Color );
//
//pTexture 是需要绘制的贴图
//pSrcRect 是需要绘制的贴图上的一个矩形区域, 绘制整过贴图可以指定为NULL.
//pCenter 是绘制的中心座标(旋转时会以此点为中心), 指定NULL表示中心座标为(0,0,0)
//pPosition 是绘制的位置座标, 也可以指定NULL表示(0,0,0)
//Color 是绘制的颜色, 一般情况下指定为 0xffffffff. 最高位到底位的各8字节为Alpha, 红, 绿, 蓝, 如果指定 0x80ffffff就是半透明贴图. 
//如果0xffff0000就只保留贴图里的红色分量, 具体功能自己体会. 当然贴图本身可以包含Alpha信息.
//需要旋转等功能可以使用 pSprite->SetTransform(), 函数原型为:
//HRESULT SetTransform( CONST D3DXMATRIX *pTransform );
//////////////////////////////////////////////////////////////////////////////////////

