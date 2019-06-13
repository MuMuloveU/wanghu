/////////////////////////////////////////
//这个是dx窗口
////////////////////////////////////////
#ifndef DX_DIALOG_HEAD_FILE
#define DX_DIALOG_HEAD_FILE
#pragma once
////////////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include <d3d9.h>
#include <d3dx9.h>
#include "Camera.h"
#include "DxControl.h"

#define WM_DX_RENDER					WM_APP+100

////////////////////////////////////////////////////////////////////////////////

//dx窗口管理类
class CDxDialog : public CWnd
{
	//变量定义
private:
	HANDLE 					m_hDxThread;	//线程句柄
	//应用变量
private:
	D3DDISPLAYMODE			d3ddm;			//显示模式
	D3DPRESENT_PARAMETERS	d3dpp;			//显示参数
	//设备变量
public:
	HWND					g_hWnd;			//d3d窗口句柄
	LPD3DXFONT				g_pFont;		//dx字体
	LPDIRECT3D9             g_pD3D;			//d3d对象
	LPDIRECT3DDEVICE9       g_pD3DDevice;	//d3d设备对象
	//控件变量
public:
	CCamera					g_Camera;		//相机
	CDxControl				g_UserControl;	//用户控制
	DWORD					g_Time;
	DWORD					starttime,endtime;
	//函数定义
public:
	CDxDialog();
	~CDxDialog();

public:
	//创建窗口
	bool InitDxWindow(int nWidth,int nHeight);
	//更新窗口
	bool UpdateDxWindow(int nWidth,int nHeight);
	//输出字体
	void DxDrawText(LPCTSTR pText,int x,int y,D3DCOLOR rgbFontColor=D3DCOLOR_XRGB(255,255,255));

	//继承函数
protected:
	//渲染函数
	virtual void OnRender();
	//绘画2d
	virtual void DrawGameView2D()=NULL;
	//绘画3d
	virtual void DrawGameView3D()=NULL;
	//初始化场景
	virtual void InitScene(int nWidth,int nHeight)=NULL;
	//设备重置
    virtual void ReleaseScene()=NULL;
	//输入控制
	virtual void UpDateControl();

	virtual void StopRender();

	//内部函数
private:
	//渲染消息
	LRESULT OnFrameRender(WPARAM wParam, LPARAM lParam) ;

	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
};

#endif
