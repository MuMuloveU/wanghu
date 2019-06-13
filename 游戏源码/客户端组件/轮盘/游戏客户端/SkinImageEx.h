#ifndef SKIN_IMAGE_EX_HEAD_FILE
#define SKIN_IMAGE_EX_HEAD_FILE

#pragma once

#include <d3d9.h>
#include <d3dx9.h>

//dx图片显示
class CSkinImageEx
{
	//内部变量
private:	
	int 				m_nImageWidth;			//图片宽
	int 				m_nImageHeight;			//图片高
	int 				m_nScreenWidth;			//屏幕宽
	int 				m_nScreenHeight;		//屏幕高
	DWORD				m_dwColour;				//透明颜色
	D3DXIMAGE_INFO		m_ImageInfo;//图片信息
	struct				PANEL_CUSTOMVERTEX		//FVF 灵活顶点格式
	{
		FLOAT x, y, z;		//三维坐标
		DWORD colour;		//颜色
		FLOAT u, v;			//纹理坐标
	};

	//dx设备
private:	
	LPDIRECT3DTEXTURE9			m_pTexture;			//图片纹理
	LPDIRECT3DDEVICE9			m_pD3DDevice;		//设备对象
	LPDIRECT3DVERTEXBUFFER9		m_pVertexBuffer;	//顶点缓冲

	//内部函数
private:
	//更新顶点
	bool UpdateVertices();	
	//创建顶点缓冲
	bool CreateVertexBuffer();	
	//设置平面摄象机
	void SetupImageCamera();
	//图片位置移动函数
	void MoveTo(int x, int y);

	//函数定义
public:
	//构造函数
	CSkinImageEx(void);
	//析构函数
	virtual ~CSkinImageEx(void);

	//功能函数
public:
	//是否设置加载信息
	bool IsSetLoadInfo();
	//清除加载参数
	bool RemoveLoadInfo();
	//获取加载参数
	bool GetLoadInfo();
	//设置加载参数
	bool SetLoadInfo(LPDIRECT3DDEVICE9 pD3DDevice, LPCTSTR pszFileName, int ScreenWidth,int ScreenHeight);
	//设置加载参数
	bool SetLoadInfo(LPDIRECT3DDEVICE9 pD3DDevice, UINT uResourceID, int ScreenWidth,int ScreenHeight,HINSTANCE hResourceDLL=NULL);
	//释放函数
	void Release();
	//渲染函数
	void AlphaDrawImage(int x,int y);

public:
	//返回图片宽度
	int GetWidth()	{return m_nImageWidth;}
	//返回图片高度
	int GetHeight(){return m_nImageHeight;}
};

class CSkinSurface
{
	//内部变量
private:
	LPDIRECT3DSURFACE9			m_Surface;			//纹理表面
	LPDIRECT3DDEVICE9			m_pD3DDevice;		//设备对象
	D3DXIMAGE_INFO				m_ImageInfo;		//图象信息
	IDirect3DSurface9*			m_backbuffer;		//后台缓冲
	//函数定义
public:
	//构造函数
	CSkinSurface(void);
	//析构函数
	virtual ~CSkinSurface(void);

	//功能函数
public:
	//设置加载参数
	bool SetLoadInfo(LPDIRECT3DDEVICE9 pD3DDevice, LPCTSTR pszFileName,D3DFORMAT Format=D3DFMT_X8R8G8B8);
	//设置加载参数
	bool SetLoadInfo(LPDIRECT3DDEVICE9 pD3DDevice, UINT uResourceID,D3DFORMAT Format=D3DFMT_X8R8G8B8);
	//释放函数
	void Release();
	//绘画函数
	void DrawSurface(int x=0,int y=0,bool drawAsBackGround=false);
	//取得图片宽度
	int GetWidth(){return m_ImageInfo.Width;};
	//取得图片高度
	int GetHeight(){return m_ImageInfo.Height;};
};
//////////////////////////////////////////////////////////////////////////

#endif