#pragma once

#pragma comment(lib, "d3d9.lib") 
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxerr9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dsound.lib")

#include "d3d9.h"
#include <d3dx9.h>
#include "d3dx9math.h"
#include "sprite.h"
#include "D3DImagePro.h"
#include "pointsprite.h"
#include "rollfont.h"

#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

/////////////////////////////////////////////////////////////////////
#define			IDM_D3D_LINK				WM_USER+150			//连接消息
#define			IDM_D3D_GUIDE				WM_USER+151			//连接消息
#define			IDM_D3D_BOMB				WM_USER+152			//连接消息
#define			IDM_D3D_REALIGN				WM_USER+153			//重列消息
#define			IDM_D3D_STOPIT				WM_USER+154			//禁手消息
#define			IDM_D3D_CLOG				WM_USER+155			//障碍消息

#define			IDM_D3D_TIMER				WM_USER+160			//定时器
#define			IDM_D3D_ALLMUSIC			WM_USER+161			//声音消息

//声音
#define			IDM_MUSIC_START					WM_USER+171			//开始声音	
#define			IDM_MUSIC_LINK					WM_USER+172			//开始声音	
#define			IDM_MUSIC_BOMB					WM_USER+173			//开始声音	
#define			IDM_MUSIC_FLYSTAR					WM_USER+174		//开始声音	
#define			IDM_MUSIC_END						WM_USER+175		//开始声音	
#define			IDM_MUSIC_PROP					WM_USER+176			//开始声音
#define			IDM_MUSIC_SEL						WM_USER+177		//开始声音
#define			IDM_MUSIC_BG					WM_USER+178			//背景声音
#define			IDM_MUSIC_STP					WM_USER+179

/////////////////////////////////////////////////////////////////////
// 连接爆炸效果
class Blast{
public:
	CArrayTemplate<CPoint>	result;				//连接线结果
	CArrayTemplate<int>		m_LinePath;			//连接线类型
	CSprite					m_A_Link;			//爆炸动画
	CSprite					m_A_Line;

	Blast & operator = (Blast & b)
	{	
		if(this == &b) return *this;
		for(int i=0; i<b.result.GetCount();++i)
			result.InsertAt(result.GetCount(),b.result.ElementAt(i));
		for(int i=0; i<b.m_LinePath.GetCount(); ++i)
			m_LinePath.InsertAt(m_LinePath.GetCount(), b.m_LinePath.ElementAt(i));
		m_A_Link = b.m_A_Link;
		m_A_Line = b.m_A_Line;
		return *this;
	}
	
};

// CD3DWnd

class CD3DWnd : public CWnd
{
	DECLARE_DYNAMIC(CD3DWnd)

	bool					g_bDeviceLost;
	HANDLE 					m_hRenderThread;	//线程句柄
public:
	CPoint					m_nSelectFirst,m_ptLinkFirst;
	CPoint					m_nSelectSecond,m_ptLinkSecond;
	BYTE					m_Chess[GAME_PLAYER][CHESSCOUNT_H][CHESSCOUNT_W]; //棋盘数据
	BYTE					m_bPlayState[GAME_PLAYER];			//用户状态
	const tagUserData *		m_UserData[GAME_PLAYER];			//用户数据接口
	bool					m_bCanGame;							//是否可以游戏
	bool					m_bGameOver;						//游戏是否结束
	bool					m_bGameLose[GAME_PLAYER];			//游戏是否失败
	bool					m_ExerciseMode;						//是否练习模式
	BYTE					m_UserTaxis[GAME_PLAYER];			//名次
	int						m_UserGrade[GAME_PLAYER];			//用户积分	

protected:
	CLogic					m_Logic;							//游戏逻辑类
	D3DPRESENT_PARAMETERS	d3dpp;								//驱动方法
	LPDIRECT3D9				m_pD3D;								//指向D3D对象接口指针
	LPDIRECT3DDEVICE9		m_pDevice;							//指向D3D驱动设备的接口指针
	D3DDISPLAYMODE			D3DDisplayMode;						//显示模式
	
	CArrayTemplate<Blast,Blast &>	m_Blast;					//存储连接效果容器

	CArrayTemplate<CRollFont,CRollFont &>   m_RollText;					//滚动字体容器

	//有必要都做成精灵吗？又不是人物图片

	CSprite					m_Sprite_ready;
	CSprite					m_Sprite_StopIt;
	CSprite					m_Sprite_Cry;
	CSprite					m_Sprite_SmallCry;
	CSprite					m_Sprite_Select;
	CSprite					m_Sprite_Number;
	CSprite					m_Sprite_ScoreFrame;
	CSprite					m_Sprite_Win;
	CSprite					m_Sprite_Lose;

	LPD3DXFONT				m_p2DFont;
	LPD3DXFONT				m_p2DFontBig;

	//实际上只需要一些离屏表面就可以了
	//游戏中并不需要对模型进行渲染
	//背景表面
	IDirect3DSurface9*		m_pSurface;
	//保存背景
	IDirect3DSurface9*		m_pBkSurface;
	IDirect3DSurface9*		m_pLoseSurface;
	IDirect3DSurface9*		m_pChess_Surface;
	IDirect3DSurface9*		m_pChessFace_Surface;
	IDirect3DSurface9*		m_pChessFaceSmall_Surface;
	IDirect3DSurface9*		m_pTimerBK_Surface;
	IDirect3DSurface9*		m_pTimer_Surface;
	IDirect3DSurface9*		m_pChessFaceProp_Surface;

	//纹理
	LPDIRECT3DTEXTURE9		m_link_Texture[46];
	LPDIRECT3DTEXTURE9		m_line_Texture[10];
	LPDIRECT3DTEXTURE9		m_ready_Texture;
	LPDIRECT3DTEXTURE9		m_StopIt_Texture;
	LPDIRECT3DTEXTURE9		m_Cry_Texture;
	LPDIRECT3DTEXTURE9		m_SmallCry_Texture;
	LPDIRECT3DTEXTURE9		m_Select_Texture;
	LPDIRECT3DTEXTURE9		m_Number_Texture;
	LPDIRECT3DTEXTURE9		m_Lose_Texture;
	LPDIRECT3DTEXTURE9		m_Win_Texture;
	LPDIRECT3DTEXTURE9		m_ScoreFrame_Texture;

	CPoint					m_KeepFirst,	m_KeepEnd, m_KeepIPoint1, m_KeepIPoint2;
	DWORD					TFPS, FPS, fTimerFPS;	

	

	TCHAR					m_ChessCountString[64];					//棋子数量
	bool					m_bHitState;							//连击状态
	int						m_nHitCount;							//连接计数
	//出现道具
	bool					m_bStopIt;
public:
	int						m_nTimeCounter;							//定时器时间
	BYTE					m_nMouseState;							//鼠标状态

	CPointSprite			m_PointSprite_Star;						//重列粒子
public:
	CD3DWnd();
	virtual ~CD3DWnd();	
	// 从资源创建表面
	IDirect3DSurface9* getSurfaceFromResource(LONG RCID, int width, int height) ;
	// 透明拷贝
	HRESULT LoadAlphaIntoSurface(LONG RCID,LPDIRECT3DSURFACE9 psurf);
	// 获取后备设备的指针
	IDirect3DSurface9* getBackBuffer(void) ;
	// 将离屏表面拷贝到设备
	void blitToSurface(IDirect3DSurface9* srcSurface, const RECT *srcRect, const RECT *destRect) ;
	// 从一个表面过去表面
	void getSurfaceFromSurface(IDirect3DSurface9* des,const RECT* srcRect, IDirect3DSurface9* src, const RECT* desRect,COLORREF colorkey);
	// 消去棋子
	void ExpunctionChess(int UserID, CPoint first);
	// 所略棋盘消去棋子
	void CD3DWnd::sExpunctionChess(int UserID, CPoint first);
	// 更新
	void UpdateBackSurface(void);
	// 计算棋子数量
	void AccountChessCount(void);
	// 走动定时器
	void StepItTimer(void);
	// 爆炸效果
	void MakeBlast(void);
	// 更新小棋盘
	void UpdateChessSmall(int UserID);
	// 游戏失败表面处理
	void GameLoseSurface(IDirect3DSurface9* srcSurface, const RECT* srcRect);
	// 练习模式
	void Execise(void);

protected:
	DECLARE_MESSAGE_MAP()
public:
	// 初始化D3D
	void InitD3D(void);
	// 创建
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	// 渲染场景
	void Render(void);
	afx_msg void OnPaint();
	// 释放资源
	void Cleanup(void);
	// 释放资源
	void Cleanup_(void);
	// 销毁
	afx_msg void OnDestroy();
	// 建模
	void Geometry(void);
	// 定时器
	afx_msg void OnTimer(UINT nIDEvent);
	// 鼠标消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	// 显示刷新频率
	void ShowFPS(char *buffer);
	// 设置矩阵
	void SetM(void);
	// 随机整数
	int RandNumber(int min, int max);
	// 按键处理
	bool SeystrokeProcess(int w, int h);
	// 初始棋盘
	void InitChess(void);
	// 连接处理
	bool LinkProcess(CPoint first, CPoint second,WORD userID);
	// 连接线处理
	void LineProcess(CArrayTemplate<CPoint>	*result, CArrayTemplate<int>		*m_LinePath);
	// 绘制游戏信息
	void DrawGameInfo(void);
	// 显示连接状态
	void AddHitState(int nHitCount);

	////////////////////////////////////////////////////////////
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	LRESULT OnRender(WPARAM wParam, LPARAM lParam);
	// 道具
	// 连接提示
	void AutoLink();
	// 重列
	void Realign(void);
	// 禁手
	void StopIt(void);
	// 障碍
	void Clog(BYTE posX[12], BYTE posY[12], BYTE color[12], int UserID);
	// 指南针
	void Guide(void);
	// 逃跑
	void UserFlee(int userid);
};


