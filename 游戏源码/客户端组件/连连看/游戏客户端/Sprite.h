#pragma once
//-------------------------
//
//类名：精灵类
//
//作者：梁攀
//
//时间：2005年11月17日
//
//功能：绘制动画或图片，碰撞检测
//
//-------------------------


#include "d3d9.h"
#include <d3dx9.h>
#include "d3dx9math.h"

//精灵类
class CSprite
{
	//接口
public:
	LPDIRECT3DTEXTURE9		m_pTexture;								//位图矩形的纹理对象接口指针
	LPD3DXSPRITE            m_pSprite;								//精灵接口指针

	//变量
private:
	int						m_RateModi;								//帧调整

	RECT srcRect;
	D3DXVECTOR3 vCenter;
	D3DXVECTOR3 vPosition;
public:
	bool					m_bActive;								//是否可用
	bool					m_bSingleFrame;							//是否是单帧
	bool					m_bAutoLoop;							//循环播放
	bool					m_bMultiPic;							//动态载入

	char					m_sType[25];
	char					m_sActiveLoadPath[30];

	int						m_nFrameRate;							//帧速率
	int						m_nFrameWidth;							//帧的宽度
	int						m_nFrameHeight;							//帧的高度
	int						m_nFrameBegin;							//起始帧
	int						m_nFrameEnd;							//结束帧
	int						m_nLoopCount;							//循环次数，0为无限循环
	int						m_nWidthCount;							//横向个数
	int						m_nFrameCount;							//动画帧总数
	int						m_nCurrentFrame;						//当前帧
	int						m_nSrcPosX;								//图片起始X坐标
	int						m_nSrcPosY;								//图片起始Y坐标
	float					m_nPosX;								//屏幕x坐标
	float					m_nPosY;								//屏幕y坐标

	//u\v
	float					u;
	float					v;
	float					nu;
	float					nv;
	//方法
public:
	CSprite(void);
	~CSprite(void);

	CSprite & operator = (const CSprite & s);
	
	void zeroSpriteValues(void);									// 重置所有变量值
	bool SetTexture(LPDIRECT3DTEXTURE9 pTexture);		// 绑定纹理
	
	void DrawSprite(LPDIRECT3DDEVICE9 pDevice,float a = 1.0f);		// 绘制精灵, a为绘制的半透明度
	// 碰撞测试
	bool CollisionTest(CSprite & pOtherSprite);
	// 循环一次
	void LoopIt(void);
	// 动态载入产生循环
	void ActiveLoopIt(LPDIRECT3DDEVICE9 pDevice);
	// 是否存在
	bool IsNull(void);
};
