#ifndef SKIN_SPRITE_HEAD_FILE
#define SKIN_SPRITE_HEAD_FILE

#pragma once

#include <d3d9.h>
#include <d3dx9.h>

////////////////////////////////////////////////////////////////////////////////

//dx图片显示
class CSkinSprite
{
	//运行变量
private:
	ID3DXSprite*		m_Sprite;	//精灵
	IDirect3DTexture9*	m_pTexture;	//纹理
	D3DXIMAGE_INFO		m_ImageInfo;//图片信息

	//移动动画
protected:
	int m_StartX,m_StartY,m_StopX,m_StopY;	//图片移动路径
	int offsetX,offsetY;					//图片移动步进

	//函数定义
public:
	CSkinSprite();
	~CSkinSprite();

	//功能函数
public:
	//加载函数
	void SetLoadInfo(LPDIRECT3DDEVICE9	pD3DDevice,LPCTSTR szFileName,D3DCOLOR dwKeyColor=D3DCOLOR_XRGB(255, 0, 255));
	void SetLoadInfo(LPDIRECT3DDEVICE9	pD3DDevice,UINT uResourceID,D3DCOLOR dwKeyColor=D3DCOLOR_XRGB(255, 0, 255));
	//释放函数
	void Release();
	//绘画函数
	void DrawSprite(int x=0,int y=0,D3DCOLOR channelColor=0xffffffff);
	void DrawSprite(int x,int y,D3DXVECTOR3 vCenterPoint, CRect drawRect,D3DXMATRIX *matRot=NULL);
	//绘画移动动画
	bool DrawMoveSprite();
public:
	//返回图片宽度
	int GetWidth()	{return m_ImageInfo.Width;}
	//返回图片高度
	int GetHeight(){return m_ImageInfo.Height;}
	//计算步进
	void Setoffset();
	//设置移动路径
	void SetMovePath(int StartX,int StartY,int StopX,int StopY);

	bool IsLoaded(){return m_Sprite!=NULL;}
};


////////////////////////////////////////////////////////////////////////////////
#endif