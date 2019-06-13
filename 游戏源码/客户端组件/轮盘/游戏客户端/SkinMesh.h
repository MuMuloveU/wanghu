#ifndef SKIN_MESH_HEAD_FILE
#define SKIN_MESH_HEAD_FILE
#pragma once


//网格模型类
class CSkinMesh  
{
	//将浮点类型转换为DWORD
	inline DWORD F2DW(FLOAT f) {return *((DWORD*)&f);}

	//内部变量
private:
	D3DXVECTOR3			m_vPos;					//模型位置
	D3DXVECTOR3			m_vMin,m_vMax;			//AABB方盒的最小点和最大点，本地坐标系
	DWORD				m_dwNumOfMaterials;		//材质数量
	TCHAR				m_szResourcePath[260];	//资源路径
	D3DMATERIAL9		m_SetMaterials;			//设置材质

	LPDIRECT3DDEVICE9	m_pDevice;				//3d设备
	LPD3DXMESH			m_pMesh;				//网格模型
	D3DMATERIAL9*		m_pMeshMaterials;		//模型材质
	LPDIRECT3DTEXTURE9*	m_pMeshTextures;		//网格纹理
	LPDIRECT3DTEXTURE9 m_pBumpTextures;			//凹凸纹理
	LPDIRECT3DTEXTURE9 m_pReflectTextures;		//反射纹理

	//ID3DXEffect*		m_pEffect;
	//渲染状态
private:
	float				m_fModelScale;			//缩放大小
	float				m_fRotateSpeed;			//旋转速度
	bool				m_bTranslucenceRender;	//半透明渲染
	BYTE				m_bFillMode;			//渲染模式
	BYTE				m_bShadeMode;			//着色模式
	bool				m_bUserSetMaterial;

	D3DXMATRIX			matScale,matRot;
	//函数定义
public:
	CSkinMesh();
	virtual ~CSkinMesh();	

	D3DXMATRIX GetRotate(){return matRot;}
public:
	//加载函数
	void SetLoadInfo(LPDIRECT3DDEVICE9 pDevice,LPTSTR strMesh,BOOL bOptimize=TRUE);
	//设置材质
	void SetModelMaterial(COLORREF diffuse=0xFFFFFF,COLORREF ambient=0,COLORREF specular=0,COLORREF Emisive=0,float power=0);
	//设置凹凸纹理
	void SetBumpTexture(LPDIRECT3DTEXTURE9 bumpTexture);
	//设置反射纹理
	void SetReflectTexture(LPDIRECT3DTEXTURE9 reflectTexture);
	//暂空
	BOOL InterSect( D3DVECTOR *pRayOrig,D3DVECTOR *pRayDir,D3DVECTOR* pVRet,DWORD dwFlag=0);
	//设定位置
	void SetPosition(float x,float y,float z);
	void SetPosition(D3DXVECTOR3 vPos);
	//获取位置
	D3DXVECTOR3 GetPosition(){return m_vPos;}
	//设置渲染方式
	void SetRenderState(float ModelScal=1,float RotateSpeed=0,bool TranslucenceRender=false,BYTE FillMode=3,BYTE ShadeMode=2);
	//渲染函数
	void Render();
	//释放函数
	void Release();
	//模型优化
	void Optimize();
	//取得包围盒
	void GetBoundingBox(D3DXVECTOR3 *pVmin,D3DXVECTOR3 *pVmax);
	//实现方盒碰撞检测
	bool CollisionTest(CSkinMesh *pMesh);

private:
	//开启透明渲染
	void OpenTranslucenceSet();
	//关闭透明渲染
	void CloseTranslucenceSet();
	//开启反射纹理
	void OpenReflect();
	//关闭反射纹理
	void CloseReflect();
	//开启凹凸纹理
	void OpenBump();
};

#endif
