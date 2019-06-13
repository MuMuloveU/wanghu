#pragma once
#include "d3d9.h"
#include <d3dx9.h>
#include "d3dx9math.h"


//位图的FVF格式：RHW坐标，顶点颜色、纹理坐标
struct CUSTOMVERTEX
{
	float x,y,z,rhw;
	DWORD color;
	float tu,tv;			//纹理坐标
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)


class CD3DImagePro
{
public:
	CD3DImagePro(void);
	~CD3DImagePro(void);
private:
	LPDIRECT3DVERTEXBUFFER9 m_pVB; //图片顶点缓存区指针
	LPDIRECT3DTEXTURE9 m_pTexture;	//位图矩形的纹理对象接口指针
	
	CUSTOMVERTEX vertices[4];     //顶点缓存
	//数据
public:
	int x, y;//记录当前图片相对位置
public:
	//从文件载入图片
	void CreateVertexBuffer(LPDIRECT3DDEVICE9 m_ppDevice,float nx,float ny,float Width,float Height);
	//设置图片位置
	void MoveImage(float x, float y, float width ,float height);
	// 更新缓存
	void Update(void);
	// 绘制
	void BitBlt(LPDIRECT3DDEVICE9 m_ppDevice);
	//绑定纹理缓存
	void SetTexture(LPDIRECT3DTEXTURE9 pTexture,float u, float v, float nu, float nv);
	//设置纹理 u 左上角x, v 左上角y, nu 右下角x ,nv 右下角y
	void SetTexture(float u, float v, float nu, float nv);
	//拷贝纹理
	void CopyTexture(CD3DImagePro &img);
	// 获取纹理缓存区指针
	LPDIRECT3DTEXTURE9 GetTexture(void);
	// 清除纹理绑定
	void ClearTexture(void);
	// 图像是否为空
	bool IsNull(void);
};
