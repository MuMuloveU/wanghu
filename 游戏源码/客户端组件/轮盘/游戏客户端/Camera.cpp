#include "StdAfx.h"
#include "Camera.h"

//////////////////////////////////////////////////////////////////////

//构造函数
CCamera::CCamera()
{
	m_Yaw=0;
	m_Pitch=0;
	m_Distance=150;
	m_pDevice=NULL;
}

//析构函数
CCamera::~CCamera()
{
	SAFE_RELEASE(m_pDevice);
}

//创建相机
void CCamera::CreateCamera(LPDIRECT3DDEVICE9 pDevice)
{
	//设置参数
	m_Yaw=0.417f;
	m_Pitch=-0.791f;
	m_Distance=200;
	m_pDevice=pDevice;

	//初始位置
	D3DXVECTOR3 CreatePos;
	CreatePos.x=0;
	CreatePos.y=0;
	CreatePos.z=0;
	m_vEyePos=CreatePos;

	//计算屏幕宽高
	D3DSURFACE_DESC m_d3dsdBackBuffer;
	LPDIRECT3DSURFACE9 pBackBuffer = NULL;
	m_pDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
	pBackBuffer->GetDesc( &m_d3dsdBackBuffer );
	m_scrWidth=m_d3dsdBackBuffer.Width;
	m_scrHeight=m_d3dsdBackBuffer.Height;
}
void CCamera::Release()
{
	SAFE_RELEASE(m_pDevice);
}

//设定位置
void CCamera::SetPosition(D3DXVECTOR3 cameraPosition)
{
	m_vEyePos=cameraPosition;
	return ;
}

//更新3D矩阵
void CCamera::Update3DMatrix()
{
	if(m_pDevice==NULL) return;

	D3DXMATRIX matView;
	D3DXVECTOR3 vFocus;
	GetCameraFocus(&vFocus);
	D3DXMatrixLookAtLH(&matView,&m_vEyePos,&vFocus,&D3DXVECTOR3(0,1,0));

	//从(100,100,100)朝向(0,0,0)
	//第二个参数是观察点位置(100,100,100),第三个参数是焦点位置(0,0,0)
	//D3DXMatrixLookAtLH(&matView,&D3DXVECTOR3(100,100,100),&D3DXVECTOR3(0,0,0),&D3DXVECTOR3(0,1,0));

	m_pDevice->SetTransform(D3DTS_VIEW,&matView);

	D3DXMATRIX matProjection;
	D3DXMatrixPerspectiveFovLH(&matProjection,D3DX_PI/4,(float)m_scrWidth/m_scrHeight,1.0f,5000.0f);
	m_pDevice->SetTransform(D3DTS_PROJECTION,&matProjection);
};


//更新2D矩阵
void CCamera::Update2DMatrix()
{
	if(m_pDevice==NULL) return;

	RECT rect;
	m_pDevice->GetScissorRect(&rect);
	D3DXMATRIX matOrtho;
	D3DXMATRIX matIdentity;
	D3DVIEWPORT9 viewPort;
	m_pDevice->GetViewport(&viewPort);

	//设置正交投影矩阵
	D3DXMatrixOrthoLH(&matOrtho,(float)m_scrWidth ,(float)m_scrHeight, 0.0f, 1000.0f);
	D3DXMatrixIdentity(&matIdentity);

	m_pDevice->SetTransform(D3DTS_PROJECTION, &matOrtho);
	m_pDevice->SetTransform(D3DTS_WORLD, &matIdentity);
	matIdentity._22=-1;
	matIdentity._41=float(-m_scrWidth/2);
	matIdentity._42=float( m_scrHeight/2);
	m_pDevice->SetTransform(D3DTS_VIEW, &matIdentity);

};

//摄像机绕着观察点旋转，上下偏转
void CCamera::YawPitchAt(float yaw,float pitch)
{
	//沿着观察点变化
	D3DXVECTOR3 vFocus;
	GetCameraFocus(&vFocus); //先保存变换前的焦点坐标

	m_Yaw+=yaw; //改变偏位角度
	if(m_Pitch-pitch<D3DX_PI/3.0f && m_Pitch-pitch>-D3DX_PI/3.0f) //改变仰角
		m_Pitch-=pitch;

	m_vEyePos.x=vFocus.x-m_Distance*cosf(m_Pitch)*cosf(m_Yaw);	//更新摄像机坐标
	m_vEyePos.y=vFocus.y-m_Distance*sinf(m_Pitch);				//更新摄像机坐标
	m_vEyePos.z=vFocus.z-m_Distance*cosf(m_Pitch)*sinf(m_Yaw);	//更新摄像机坐标

}; 

//沿着观察点变换方位
void CCamera::YawPitchPos(float yaw,float pitch)
{
	m_Yaw+=yaw;
	if(m_Pitch+pitch<D3DX_PI/3.0f && m_Pitch+pitch>-D3DX_PI/3.0f)
		m_Pitch+=pitch;

};

//移动焦点坐标(实质上是移动摄像机)
void CCamera::SetFocus(D3DXVECTOR3 *vFocus)
{
	m_vEyePos.x=vFocus->x-m_Distance*cosf(m_Pitch)*cosf(m_Yaw);
	m_vEyePos.y=vFocus->y-m_Distance*sinf(m_Pitch);
	m_vEyePos.z=vFocus->z-m_Distance*cosf(m_Pitch)*sinf(m_Yaw);
};

//移动摄像机
void CCamera::MovePosTo(D3DXVECTOR3 *pV)
{
	m_vEyePos=*pV; 
}

//移动,向前/侧向移动，主要用于自由摄像机
void CCamera::Move(float dSide,float dForward)
{
	D3DXVECTOR3 vDir;
	D3DXVECTOR3 vSide;
	vDir.x=cosf(m_Pitch)*cosf(m_Yaw);
	vDir.y=sinf(m_Pitch);
	vDir.z=cosf(m_Pitch)*sinf(m_Yaw);

	D3DXVec3Cross(&vSide,&D3DXVECTOR3(0,1,0),&vDir);
	D3DXVec3Normalize(&vSide,&vSide);
	if(m_Distance-dForward>0)
	{
		m_Distance-=dForward;
		m_vEyePos+=vDir*dForward+ vSide*dSide;
	}
}

//相机焦点
void CCamera::AimTo(D3DVECTOR *pLookAt)
{
	D3DVECTOR vDir;
	vDir.x=pLookAt->x-m_vEyePos.x;
	vDir.y=pLookAt->y-m_vEyePos.y;
	vDir.z=pLookAt->z-m_vEyePos.z;
	D3DXVec3Normalize((D3DXVECTOR3*)&vDir,(D3DXVECTOR3*)&vDir);
	m_Pitch=acosf(vDir.y);
	if(vDir.z!=0 ) m_Yaw=atan2f(vDir.z,vDir.x);
	else m_Yaw=D3DX_PI/2;
}

//取得观察点位置
void CCamera::GetCameraFocus(D3DVECTOR *pLookAt)
{
	pLookAt->x= m_vEyePos.x+m_Distance*cosf(m_Pitch)*cosf(m_Yaw);
	pLookAt->y= m_vEyePos.y+m_Distance*sinf(m_Pitch);
	pLookAt->z= m_vEyePos.z+m_Distance*cosf(m_Pitch)*sinf(m_Yaw);
};

//获得方向向量(单位向量)
void CCamera::GetDirection(D3DVECTOR *vDir)
{
	vDir->x=cosf(m_Pitch)*cosf(m_Yaw);
	vDir->y=sinf(m_Pitch);
	vDir->z=cosf(m_Pitch)*sinf(m_Yaw);
};

//获得摄像机坐标
D3DVECTOR* CCamera::GetPos()
{
	return &m_vEyePos;
}

//获得摄像机坐标
void CCamera::GetPos(D3DVECTOR *vPos)
{ 
	vPos->x=m_vEyePos.x;
	vPos->y=m_vEyePos.y;
	vPos->z=m_vEyePos.z;
};

//-----------------------------------------------------------------
//根据鼠标坐标，计算射线
//实现计算鼠标指针射线(从摄像机开始穿过鼠标指针的一条3D空间的射线)
//-----------------------------------------------------------------
void CCamera::GetCursorRayDir(POINT ptCursor,D3DVECTOR *vPickRayDir)
{
	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH(&matProj,D3DX_PI/4,(float)m_scrWidth/m_scrHeight,1.0f,6000.0f);
	//计算屏幕空间的坐标(-1,-1)～(1,1)
	D3DXVECTOR3 v;
	v.x = ( (-1+2* ptCursor.x /float( m_scrWidth) ) /*- 1*/ ) / matProj._11;
	v.y = ( ( 1-2* ptCursor.y /float( m_scrHeight)) /*- 1*/ ) / matProj._22;
	v.z = 1.0f;

	//获得视图变换矩阵的逆矩阵
	D3DVECTOR vFocus;
	GetCameraFocus(&vFocus);
	D3DXMATRIXA16 matView, m;
	D3DXMatrixLookAtLH(&matView,(D3DXVECTOR3*)&m_vEyePos,(D3DXVECTOR3*)&vFocus,&D3DXVECTOR3(0,1,0));

	D3DXMatrixInverse( &m, NULL, &matView );

	//把上述坐标转换成世界坐标
	vPickRayDir->x = v.x*m._11 + v.y*m._21 + v.z*m._31;
	vPickRayDir->y = v.x*m._12 + v.y*m._22 + v.z*m._32;
	vPickRayDir->z = v.x*m._13 + v.y*m._23 + v.z*m._33;
};
