//////////////////////////////////////////////////////////////////////
//摄像机类，主要用于控制，更新2D，3D视图变换矩阵，投影变换矩阵
//实现计算鼠标指针射线(从摄像机开始穿过鼠标指针的一条3D空间的射线)
//摄像机从m_vEyePos观察，方向为:从m_vEyePos水平偏角为yaw,仰角为pitch
//////////////////////////////////////////////////////////////////////

#ifndef CAMERA_HEAD_FILE
#define CAMERA_HEAD_FILE
#pragma once

#include "d3dx9.h"

//////////////////////////////////////////////////////////////////////

//摄像机类
class CCamera  
{
	//内部变量
protected:
	float				m_Yaw,m_Pitch;	//偏移角度
	float				m_Distance;		//观察距离
	long				m_scrWidth;		//屏幕宽
	long				m_scrHeight;	//屏幕高
	D3DXVECTOR3			m_vEyePos;			//相机位置
	LPDIRECT3DDEVICE9	m_pDevice;		//3D设备

public:
	CCamera();
	virtual ~CCamera();

	//功能函数
public:
	//创建相机
	void CreateCamera(LPDIRECT3DDEVICE9 pDevice);
	//释放函数
	void Release();
	//设置位置
	void SetPosition(D3DXVECTOR3 camposition);
	//瞄准焦点
	void AimTo(D3DVECTOR *pLookAt);
	//更新2d矩阵
	void Update2DMatrix();
	//更新3d矩阵
	void Update3DMatrix();
	//改变水平偏角
	void YawPitchAt(float yaw,float pitch);
	//改变垂直仰角
	void YawPitchPos(float yaw,float pitch);

	//相机控制
public:	
	//移动焦点坐标(实质上是移动摄像机)
	void SetFocus(D3DXVECTOR3 *vFocus);
	//移动摄像机
	void MovePosTo(D3DXVECTOR3 *pV);
	//移动摄像机，在摄像机空间,也就是沿着摄像机朝向前后移动，和侧向左右移动
	void Move(float dSide,float dForward);

	//数据获取
public:
	//屏幕宽度
	long GetScreenWidth()  {return m_scrWidth;	};
	//屏幕高度
	long GetScreenHeight() {return m_scrHeight;	};
	//获得摄像机坐标
	D3DVECTOR*	GetPos();	
	//获得摄像机坐标
	void GetPos(D3DVECTOR *vPos);	
	//获得观察点坐标
	void GetCameraFocus(D3DVECTOR *pLookAt);	
	//获得方向向量(单位向量)
	void GetDirection(D3DVECTOR *vDir) ;
	//计算鼠标指针射线(从摄像机开始穿过鼠标指针的一条3D空间的射线)
	void GetCursorRayDir(POINT ptCursor,D3DVECTOR *vPickRayDir);
};

#endif
