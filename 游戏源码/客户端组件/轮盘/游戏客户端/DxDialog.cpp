#include "stdafx.h"
#include "DxDialog.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDxDialog, CWnd)
	//ON_WM_PAINT()
	ON_MESSAGE(WM_DX_RENDER, OnFrameRender)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

static bool		bD3DSucess;						//DX初始化成功标志

//////////////////////////////////////////////////////////////////////////

//构造函数
CDxDialog::CDxDialog()
{
}
//析构函数
CDxDialog::~CDxDialog()
{
}

void CDxDialog::OnPaint()
{
	CPaintDC dc(this);
	if(g_pD3DDevice==NULL) return ;
	HRESULT hResult=g_pD3DDevice->TestCooperativeLevel();
	if (hResult==D3DERR_DEVICELOST) return ;
	if (hResult==D3DERR_DEVICENOTRESET)
	{
		UpdateDxWindow(d3dpp.BackBufferWidth,d3dpp.BackBufferHeight);
	}
	OnRender() ;
	Invalidate(true);
}

//渲染线程
void DxThreadRender(LPVOID pThreadData)
{
	//效验参数
	ASSERT(pThreadData!=NULL);
	if(pThreadData==NULL) return;
	
	while(bD3DSucess)
	{	
		Sleep(5);	
		SendMessage((HWND)pThreadData, WM_DX_RENDER, 0, 0) ;
	}
}

//渲染消息
LRESULT CDxDialog::OnFrameRender(WPARAM wParam, LPARAM lParam)
{
	if(g_pD3DDevice==NULL) return 0;
	HRESULT hResult=g_pD3DDevice->TestCooperativeLevel();
	if (hResult==D3DERR_DEVICELOST) return 0;
	if (hResult==D3DERR_DEVICENOTRESET)
	{
		UpdateDxWindow(d3dpp.BackBufferWidth,d3dpp.BackBufferHeight);
	}
	OnRender() ;
	return 0 ;
}

//dx窗口初始化
bool CDxDialog::InitDxWindow(int nWidth,int nHeight)
{
	//已经创建过的话返回
	if(bD3DSucess==true) return true;

	//获得实例
	g_hWnd=this->GetSafeHwnd();
	if(g_hWnd==NULL) return false;

	//初始化d3d
	try{
		//取得DX版本,显示模式
		g_pD3D=Direct3DCreate9(D3D_SDK_VERSION);
		g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);


		//设置显示参数
		ZeroMemory(&d3dpp, sizeof(d3dpp));
		d3dpp.Windowed=true;										//是否窗口模式显示
		d3dpp.BackBufferFormat=d3ddm.Format;						//后缓冲区模式
		d3dpp.SwapEffect=D3DSWAPEFFECT_DISCARD;						//交换缓冲类型(直接翻屏)
		d3dpp.BackBufferWidth=nWidth;								//后缓冲宽
		d3dpp.BackBufferHeight=nHeight;								//后缓冲高
		d3dpp.hDeviceWindow=g_hWnd;									//窗口句柄
		d3dpp.FullScreen_RefreshRateInHz=D3DPRESENT_RATE_DEFAULT;	//显示器刷新率
		d3dpp.PresentationInterval=D3DPRESENT_INTERVAL_IMMEDIATE;	//图像最大刷新速度
		d3dpp.AutoDepthStencilFormat=D3DFMT_D16;					//深度缓冲格式
		d3dpp.EnableAutoDepthStencil=TRUE;							//激活深度缓冲

		CString xx;xx.Format("%d",d3dpp.BackBufferHeight);
		//AfxMessageBox(xx);
		//创建设备
		g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWnd,D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pD3DDevice);
		if(g_pD3DDevice==NULL) return false;

		//开启渲染线程
		UINT uThreadID=0;
		bD3DSucess = TRUE;
		m_hDxThread=(HANDLE)::CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)DxThreadRender,LPVOID(this->m_hWnd),0,0);
	}catch(...){}

	//////////////////////////////////////////////////////////////////////////////////////
	//设置设备
	//打开环境光
	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING,TRUE);
	g_pD3DDevice->SetRenderState(D3DRS_AMBIENT,0xffffffff);

	//打开深度缓冲
	g_pD3DDevice->SetRenderState(D3DRS_ZENABLE,TRUE);
	g_pD3DDevice->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);
	g_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);

	//使用Alpha纹理混合
	g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);		
	g_pD3DDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	g_pD3DDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA );

	//关闭灯光
	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING,FALSE);
	//////////////////////////////////////////////////////////////////////////////////////

	//创建字体
	D3DXCreateFont(g_pD3DDevice,12,0,0,1,0,GB2312_CHARSET,0,0,0,TEXT("宋体"),&g_pFont);	g_pFont->AddRef();
 	//初始化输入
	g_UserControl.InitDXInput(AfxGetInstanceHandle(),g_hWnd);
	//设置相机
	g_Camera.CreateCamera(g_pD3DDevice);
	//初始化场景
	InitScene(nWidth,nHeight);
	return (g_hWnd!=NULL);
}
//更新窗口
bool CDxDialog::UpdateDxWindow(int nWidth,int nHeight)
{
	//窗口大小改变需要重新设置d3d设备，与初始化相似
	//释放设备
	ReleaseScene();
 	g_Camera.Release();
	g_UserControl.Release();
	SAFE_RELEASE(g_pFont);
	SAFE_RELEASE(g_pD3DDevice);
	if(g_pD3D) { 
		 int xx=(g_pD3D)->Release(); 
		 (g_pD3D)=NULL; }

	try{
		g_pD3D=Direct3DCreate9(D3D_SDK_VERSION);
		g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);

		//设置显示参数
		ZeroMemory(&d3dpp, sizeof(d3dpp));
		d3dpp.Windowed=true;										//是否窗口模式显示
		d3dpp.BackBufferFormat=d3ddm.Format;						//后缓冲区模式
		d3dpp.SwapEffect=D3DSWAPEFFECT_DISCARD;						//交换缓冲类型(直接翻屏)
		d3dpp.BackBufferWidth=nWidth;								//后缓冲宽
		d3dpp.BackBufferHeight=nHeight;								//后缓冲高
		d3dpp.hDeviceWindow=g_hWnd;									//窗口句柄
		d3dpp.FullScreen_RefreshRateInHz=D3DPRESENT_RATE_DEFAULT;	//显示器刷新率
		d3dpp.PresentationInterval=D3DPRESENT_INTERVAL_IMMEDIATE;	//图像最大刷新速度
		d3dpp.AutoDepthStencilFormat=D3DFMT_D16;					//深度缓冲格式
		d3dpp.EnableAutoDepthStencil=TRUE;							//激活深度缓冲

		//创建设备
		g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWnd,D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pD3DDevice);
		if(g_pD3DDevice==NULL) return false;
	}catch(...){}


	//////////////////////////////////////////////////////////////////////////////////////
	//若不在此设置设备，则vertexbuffer方式绘画无透明效果
	//灯光
	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING,TRUE);
	g_pD3DDevice->SetRenderState(D3DRS_AMBIENT,0xffffffff);

	//打开深度缓冲
	g_pD3DDevice->SetRenderState(D3DRS_ZENABLE,TRUE);
	g_pD3DDevice->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);
	g_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);

	//使用Alpha纹理混合
	g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);		
	g_pD3DDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	g_pD3DDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA );

	//关闭灯光
	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING,FALSE);
	//////////////////////////////////////////////////////////////////////////////////////

	//创建字体
	D3DXCreateFont(g_pD3DDevice,12,0,0,1,0,GB2312_CHARSET,0,0,0,TEXT("宋体"),&g_pFont);g_pFont->AddRef();
	//初始化输入
	g_UserControl.InitDXInput(AfxGetInstanceHandle(),g_hWnd);
	//设置相机
	g_Camera.CreateCamera(g_pD3DDevice);
	//初始化场景
	InitScene(nWidth,nHeight);
	return true;
}

//输出字体
void CDxDialog::DxDrawText(LPCTSTR pText,int x,int y,D3DCOLOR rgbFontColor)
{
	if(g_pFont==NULL) return;

	RECT Rect;
	Rect.left = x;	Rect.top = y;
	Rect.right = 0;	Rect.bottom = 0;
	g_pFont->DrawText(NULL,pText, -1, &Rect, DT_CALCRECT, 0);					//通过此语句计算文字占用的矩形大小
	g_pFont->DrawText(NULL,pText, -1, &Rect, DT_LEFT, rgbFontColor|0xFF000000);	//然后输出文本
}

//用户控制
void CDxDialog::UpDateControl()
{
	if(&g_UserControl==NULL) return;

	//更新输入
	g_UserControl.UpdateInput();

	float mouseX,mouseY,mouseZ;	//鼠标状态
	float dSide=0,dForward=0;	//偏移变量

	//按住鼠标右键旋转镜头
	if(g_UserControl.m_MouseState.rgbButtons[1]&0x80)
	{
		mouseX=g_UserControl.GetLX()*D3DX_PI/720.0f;	//每移动一个点，方向改变1/4度
		mouseY=g_UserControl.GetLY()*D3DX_PI/720.0f;
		g_Camera.YawPitchAt(-mouseX,mouseY);
		dSide+=mouseX;
	}

	//鼠标滚轮控制摄象机距离
	mouseZ=g_UserControl.GetLZ()*D3DX_PI/10;
	g_Camera.Move(0,mouseZ);

	//设置焦点位置
	g_Camera.SetFocus(&D3DXVECTOR3(0,0,0));
}

//渲染函数 客户端可以重载onrender();必须重写DrawGameViewEx()
void CDxDialog::OnRender()
{	
	if(bD3DSucess==false) return;
	//贞数限制
	if((GetTickCount()-g_Time)<10) return;
	g_Time=GetTickCount();

#ifdef _DEBUG
	//开始时间
	starttime=GetTickCount();
	float fps=1000/(float)(starttime-endtime);
#endif

	//更新控制
	UpDateControl();

	if(g_pD3DDevice==NULL) return;

	//清除后备缓冲区
	g_pD3DDevice->Clear(0,NULL,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,D3DCOLOR_XRGB(0,0,0),1.0f,0);
	//开始绘制场景
	g_pD3DDevice->BeginScene();

	g_Camera.Update2DMatrix();
	DrawGameView2D();
	g_Camera.Update3DMatrix();
	DrawGameView3D();

#ifdef _DEBUG
	TCHAR szBuffer[256];
	_snprintf(szBuffer,sizeof(szBuffer),TEXT("fps： %3.1f"),fps);
	DxDrawText(szBuffer,d3dpp.BackBufferWidth/2+35,d3dpp.BackBufferHeight/2-329,RGB(0,0,0));
#endif

	//结束绘制
	g_pD3DDevice->EndScene();
	//翻页
	g_pD3DDevice->Present(NULL,NULL,NULL,NULL);

#ifdef _DEBUG
	//结束时间
	endtime=GetTickCount();
#endif
}


void CDxDialog::StopRender()
{
	bD3DSucess=false;
}
