#include "StdAfx.h"
#include "resource.h"
#include "math.h"
#include "FlowerEffectControl.h"
#include ".\flowereffectcontrol.h"
///////////////////////////////////////////////////////////////////////////////////

//时间定义
#define IDI_FLOWER_START				3000							//鲜花定时器

//常量定义
#define LWA_COLORKEY					0x00000001
#define WS_EX_LAYERED					0x00080000

///////////////////////////////////////////////////////////////////////////////////

//构造函数
BEGIN_MESSAGE_MAP(CFlowerEffectControl, CDialog)
	ON_WM_PAINT()
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////////

//构造函数
CFlowerEffectControl::CFlowerEffectControl(): CDialog(IDD_FLOWER_EFFECT)
{
	m_pFlowerEffectThread = new CFlowerEffectThread;
}

//析构函数
CFlowerEffectControl::~CFlowerEffectControl()
{
	if(m_pFlowerEffectThread!=NULL)
	{
		delete m_pFlowerEffectThread;
		m_pFlowerEffectThread=NULL;
	}
}

//控件绑定
void CFlowerEffectControl::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

//启动动画
bool CFlowerEffectControl::StartFlowerEffect(UINT uFlowerEffectID)
{
	//启动线程
	m_pFlowerEffectThread->InitThread(GetSafeHwnd());
	m_pFlowerEffectThread->StartFlowerEffect(uFlowerEffectID);

	return true;
}

//创建窗体
BOOL CFlowerEffectControl::CreateControl(CWnd *pParent)
{
	return Create(IDD_FLOWER_EFFECT, pParent);
}

//初始函数
BOOL CFlowerEffectControl::OnInitDialog()
{
	__super::OnInitDialog();

	return TRUE; 
}

//绘画函数
void CFlowerEffectControl::OnPaint()
{
	CPaintDC dc(this); 
}
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

//构造函数
CFlowerEffectThread::CFlowerEffectThread(void)
{
	//初始变量
	m_hFlowerEffectControl=NULL;
	m_wFlowerID=0;
}

//析构函数
CFlowerEffectThread::~CFlowerEffectThread(void)
{
	ConcludeThread(0);
}

//开始事件
bool CFlowerEffectThread::OnEventThreadStrat()
{
	return true;
}

//停止事件
bool CFlowerEffectThread::OnEventThreadConclude()
{
	//初始变量
	m_hFlowerEffectControl=NULL;
	m_wFlowerID=0;

	return true;
}

//初始函数
void CFlowerEffectThread::InitThread(HWND hWnd)
{
	if ( m_hFlowerEffectControl != NULL ) return ;

	//句柄赋值
	m_hFlowerEffectControl = hWnd;

	//设置大小
	CRect rcFlowerEffectControl;
	::GetWindowRect(m_hFlowerEffectControl, &rcFlowerEffectControl);
	if ( m_ImageBuffer.IsNull() == false )
		m_ImageBuffer.Destroy();
	m_ImageBuffer.Create(rcFlowerEffectControl.Width(),rcFlowerEffectControl.Height(),24);

	//获取区域
	CRect ClientRect;
	GetClientRect(m_hFlowerEffectControl, ClientRect);
	CPoint ptLeftTop(ClientRect.left,ClientRect.top),ptRightBottom(ClientRect.right,ClientRect.bottom);
	ClientToScreen(m_hFlowerEffectControl, &ptLeftTop);
	ClientToScreen(m_hFlowerEffectControl, &ptRightBottom);

	//获取DC
	if ( m_ImageBufferScreen.IsNull() )
        m_ImageBufferScreen.Create(ClientRect.Width(),ClientRect.Height(),24);
	CDC *pBufferDC=CDC::FromHandle(m_ImageBufferScreen.GetDC());

	//拷贝屏幕
	HWND hScreenWnd= GetDesktopWindow();
	if ( hScreenWnd!=NULL )
	{
		HDC hScreenDC = GetWindowDC(hScreenWnd);

		if ( hScreenDC!=NULL )
		{
			for ( int nXPos=ptLeftTop.x; nXPos < ptRightBottom.x; ++nXPos)
				for ( int nYPos=ptLeftTop.y; nYPos < ptRightBottom.y; ++nYPos )
				{
					pBufferDC->SetPixel(nXPos-ptLeftTop.x, nYPos-ptLeftTop.y, GetPixel(hScreenDC,nXPos, nYPos));
				}

			ReleaseDC(hScreenWnd,hScreenDC);
		}
	}

	//释放DC
	m_ImageBufferScreen.ReleaseDC();

	return;
}

//启动动画
bool CFlowerEffectThread::StartFlowerEffect(WORD wFlowerID)
{
	//效验参数
	ASSERT( wFlowerID < FLOWER_COUNT*2 );
	if ( wFlowerID >= FLOWER_COUNT*2 ) return false;

	m_wFlowerID = wFlowerID;

	//启动线程
	if ( IsRuning() == false ) StartThread();

	return true;
}

//运行函数
bool CFlowerEffectThread::OnEventThreadRun()
{
	//加载图片
	CPngImage ImageFlower;
	CString strInfo=TEXT("GIFT_"),strImage;
	strImage.Format(TEXT("%s%ld"),strInfo,m_wFlowerID/2+1);
	ImageFlower.LoadImage(GetModuleHandle(PROPERTY_MODULE_DLL_NAME),strImage);
	if(ImageFlower.IsNull())return false;

	//窗体大小
	CRect ClientRect;
	::GetWindowRect(m_hFlowerEffectControl, &ClientRect);

	//绘画大小
	int nCartoonWidth = 150;
	int nCartoonCount = ImageFlower.GetWidth() / nCartoonWidth;

	//过虑散频
	MoveWindow(m_hFlowerEffectControl,0,0,0,0,false);
	::ShowWindow(m_hFlowerEffectControl, SW_SHOW);

	//绘画图片
	for ( int nCartoonIndex=0; nCartoonIndex < nCartoonCount; ++nCartoonIndex )
	{		
		//设备句柄
		HDC hDC=GetDC(m_hFlowerEffectControl);
		if ( hDC==NULL ) return false;

		//系上D C
		CDC	TempDC;
		TempDC.Attach(hDC);

		//内存设备
		CDC *pMemoryDC = CDC::FromHandle(m_ImageBuffer.GetDC());
		CDC *pScreenDC = CDC::FromHandle(m_ImageBufferScreen.GetDC());

		//绘画背景
		pMemoryDC->BitBlt(0,0,ClientRect.Width(),ClientRect.Height(),pScreenDC,0,0,SRCCOPY);

		//绘画礼物
		ImageFlower.DrawImage(pMemoryDC, 0, 0, ClientRect.Width(), ClientRect.Height(),
			nCartoonWidth * nCartoonIndex,0,nCartoonWidth, ImageFlower.GetHeight());

		TempDC.BitBlt(0, 0, ClientRect.Width(), ClientRect.Height(), pMemoryDC, 0, 0, SRCCOPY);

		//显示窗体
		if (nCartoonIndex==0) MoveWindow(m_hFlowerEffectControl,ClientRect.left,ClientRect.top,ClientRect.Width(),ClientRect.Height(),false);

		//释放设备
		TempDC.Detach();
		ReleaseDC(m_hFlowerEffectControl, hDC);
		hDC=NULL;
		m_ImageBuffer.ReleaseDC();
		m_ImageBufferScreen.ReleaseDC();
		pMemoryDC=NULL;

		//线程睡眠
		Sleep(120);
	}

	//隐藏窗体
	::ShowWindow(m_hFlowerEffectControl, SW_HIDE);

	return false;
}

//运行函数
bool CFlowerEffectThread::RepetitionRunBMP()
{
	return false;
}

//运行函数
bool CFlowerEffectThread::RepetitionRunPNG()
{
	return false;
}

///////////////////////////////////////////////////////////////////////////////////



